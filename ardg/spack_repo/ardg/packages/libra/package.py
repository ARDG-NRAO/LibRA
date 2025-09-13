# Copyright Spack Project Developers. See COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)


import os
import shutil
from spack_repo.builtin.build_systems.cmake import CMakePackage
from spack.package import *


class Libra(CMakePackage):
    """LibRA (Library of Radio Astronomy) - A radio astronomy data processing library"""

    homepage = "https://github.com/ARDG-NRAO/LibRA"
    git = "https://github.com/ARDG-NRAO/LibRA.git"
    maintainers = ["ardg-nrao", "sanbw", "sanbee", "pjaganna"]

    license("GPL-3.0-or-later", checked_by="pjaganna")

    version("main", branch="main")

    # Variants for optional features
    variant("exodus", default=False, description="Enable exodus support")
    variant("openmp", default=True, description="Enable OpenMP support")
    variant("cuda", default=False, description="Enable CUDA support")
    variant(
        "all",
        default=False,
        description="Enable all backends: CUDA, OpenMP, and Serial",
    )
    variant("tests", default=False, description="Build tests")

    # Core dependencies
    depends_on(
        "kokkos@4.0.0:4.99.99 +openmp+serial+cuda+cuda_lambda+wrapper", when="+all"
    )
    depends_on("kokkos@4.0.0:4.99.99 +cuda+cuda_lambda+wrapper", when="+cuda ~all")
    depends_on("kokkos@4.0.0:4.99.99 +openmp+serial", when="+openmp ~cuda ~all")
    depends_on("kokkos@4.0.0:4.99.99 +serial", when="~openmp ~cuda ~all")

    depends_on("fftw@3.3.10: +openmp", when="+openmp")
    depends_on("fftw@3.3.10: +openmp", when="+all")
    depends_on("fftw@3.3.10:", when="~openmp ~all")
    depends_on("hpg@3.0.3: +cuda", when="+cuda")
    depends_on("hpg@3.0.3: +cuda", when="+all")
    depends_on("hpg@3.0.3: +openmp", when="+openmp ~cuda ~all")
    depends_on("hpg@3.0.3:", when="~openmp ~cuda ~all")
    depends_on("parafeed@1.1.4:")
    depends_on("py-pybind11@2.10.2:")
    depends_on("casacore@3.7.1:")

    # CUDA dependency with GCC version constraint
    depends_on("cuda@:12.2", when="+cuda %gcc@:8")
    depends_on("cuda@:12.2", when="+all %gcc@:8")
    depends_on("cuda", when="+cuda")
    depends_on("cuda", when="+all")
#    depends_on("cuda@:12.9", when="+cuda cuda_arch=pascal60,pascal61")
    # Optional dependencies
    depends_on("googletest@1.14.0:", when="+tests")

    # Build dependencies
    depends_on("cmake@3.18:", type="build")
    depends_on("pkgconfig", type="build")
    depends_on("fortran", type="build")
    depends_on("c", type="build")
    depends_on("cxx", type="build")

    # Runtime dependencies
    depends_on("python@3.6:", type=("build", "run"))

    # System dependencies that are typically available
    depends_on("cfitsio")
    depends_on("blas")
    depends_on("lapack")
    depends_on("wcslib")
    depends_on("readline")
    depends_on("ncurses")



    def setup_build_environment(self, env):
        # Set PKG_CONFIG_PATH for dependencies
        env.prepend_path("PKG_CONFIG_PATH", self.spec.prefix.lib.pkgconfig)

        # Add CUDA to PATH if available
        if ("+cuda" in self.spec or "+all" in self.spec) and "cuda" in self.spec:
            env.prepend_path("PATH", self.spec["cuda"].prefix.bin)

        # Work around spack 1.0.0 compiler wrapper issues
        # Force use of system compilers
        env.set("CC", "gcc")
        env.set("CXX", "g++")
        env.set("FC", "gfortran")

    # @run_after("patch")
    # def setup_spack_cmake(self):
    #     # COMMENTED OUT FOR DEBUGGING
    #     pass

    @run_before("cmake")
    def setup_directories(self):
        # Create necessary directories
        mkdirp(self.spec.prefix.lib)
        mkdirp(self.spec.prefix.include)
        mkdirp(self.spec.prefix.bin)
        mkdirp(self.spec.prefix.share)

        # Set up data directory for casacore
        data_dir = join_path(self.spec.prefix, "data")
        mkdirp(data_dir)

    def build(self, spec, prefix):
        # Build Casacpp subproject first
        import subprocess
        build_dir = os.path.join(self.stage.source_path, "build_casacpp")
        os.makedirs(build_dir, exist_ok=True)
        casacpp_source = os.path.join(self.stage.source_path, "src")
        casacpp_install = prefix
        casacpp_cmake_args = [
            "-DCMAKE_INSTALL_PREFIX={}".format(casacpp_install),
            "-DCMAKE_INSTALL_LIBDIR=lib",
            "-DCMAKE_INSTALL_INCLUDEDIR=include",
            "-DCMAKE_INSTALL_BINDIR=bin/casacpp",
            "-DCMAKE_CXX_FLAGS=-I{}/include".format(casacpp_install),
            "-DCMAKE_BUILD_TYPE=RelWithDebInfo",
            "-DCASA_BUILD_TYPE=RelWithDebInfo",
            "-DCMAKE_MODULE_PATH={}/src/cmake".format(self.stage.source_path),
            "-DCMAKE_PREFIX_PATH={}:{}".format(casacpp_install, os.path.join(casacpp_install, "lib/cmake/Hpg")),
        ]
        subprocess.check_call([
            "cmake", casacpp_source
        ] + casacpp_cmake_args, cwd=build_dir)
        subprocess.check_call(["make", "-j", "16"], cwd=build_dir)
        subprocess.check_call(["make", "install"], cwd=build_dir)

        # Build Apps subproject next
        build_dir_apps = os.path.join(self.stage.source_path, "build_apps")
        os.makedirs(build_dir_apps, exist_ok=True)
        apps_source = os.path.join(self.stage.source_path, "apps")
        apps_install = prefix
        
        # Copy fixed CMakeLists.txt for apps
        package_dir = os.path.dirname(__file__)
        source_apps_cmake = join_path(package_dir, "CMakeLists.txt.apps")
        target_apps_cmake = join_path(self.stage.source_path, "apps", "src", "CMakeLists.txt")
        print("*** COPYING APPS CMAKE ***")
        print("Source apps cmake:", source_apps_cmake)
        print("Target apps cmake:", target_apps_cmake)
        if os.path.exists(source_apps_cmake):
            print("*** FOUND APPS CMAKE IN PACKAGE DIR - COPYING ***")
            shutil.copy2(source_apps_cmake, target_apps_cmake)
            print("*** APPS CMAKE COPY COMPLETE ***")
        else:
            print("*** APPS CMAKE NOT FOUND IN PACKAGE DIR ***")
        
        apps_cmake_args = [
            "-DCMAKE_INSTALL_PREFIX={}".format(apps_install),
            "-DCMAKE_INSTALL_LIBDIR=lib",
            "-DCMAKE_INSTALL_INCLUDEDIR=include",
            "-DCMAKE_CXX_FLAGS=-I{}/include".format(apps_install),
            "-DCMAKE_BUILD_TYPE=RelWithDebInfo",
            "-DCASA_BUILD_TYPE=RelWithDebInfo",
            "-DCMAKE_PREFIX_PATH={}:{}".format(apps_install, os.path.join(apps_install, "lib/cmake/Hpg")),
        ]
        subprocess.check_call([
            "cmake", apps_source
        ] + apps_cmake_args, cwd=build_dir_apps)
        subprocess.check_call(["make", "-j", "16"], cwd=build_dir_apps)
        subprocess.check_call(["make", "install"], cwd=build_dir_apps)