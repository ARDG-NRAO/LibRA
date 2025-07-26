# Copyright Spack Project Developers. See COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)


import os
from spack_repo.builtin.build_systems.cmake import CMakePackage
from spack.package import *


class Libra(CMakePackage):
    """LibRA (Library of Radio Astronomy) - A radio astronomy data processing library"""

    homepage = "https://github.com/ARDG-NRAO/LibRA"
    git = "https://github.com/ARDG-NRAO/LibRA.git"
    maintainers = ["ardg-nrao", "sanbw", "sanbee", "pjaganna"]

    license("GPL-3.0-or-later", checked_by="pjaganna")

    version("main", branch="spack")

    # Variants for optional features
    variant("exodus", default=False, description="Enable exodus support")
    variant("openmp", default=True, description="Enable OpenMP support")
    variant("cuda", default=False, description="Enable CUDA support")
    variant("all", default=False, description="Enable all backends: CUDA, OpenMP, and Serial")
    variant("tests", default=False, description="Build tests")

    # Core dependencies 
    depends_on("kokkos@4.0.0:4.99.99 +openmp+serial+cuda+cuda_lambda+wrapper", when="+all")
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
    depends_on("casacore@3.6.1:")

    # CUDA dependency with GCC version constraint
    depends_on("cuda@:12.2", when="+cuda %gcc@:8")
    depends_on("cuda@:12.2", when="+all %gcc@:8")
    depends_on("cuda", when="+cuda")
    depends_on("cuda", when="+all")
    
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
    depends_on("boost")
    depends_on("wcslib")
    depends_on("readline")
    depends_on("ncurses")
    
    

    def cmake_args(self):
        # Copy CMakeLists.txt.spack from package directory to staged source
        import shutil
        package_dir = os.path.dirname(__file__)
        source_spack_cmake = join_path(package_dir, "CMakeLists.txt.spack")
        target_main_cmake = join_path(self.stage.source_path, "CMakeLists.txt")
        
        print("*** COPYING CMAKE FROM PACKAGE DIR ***")
        print("Package dir:", package_dir)
        print("Source spack cmake:", source_spack_cmake)
        print("Target main cmake:", target_main_cmake)
        
        if os.path.exists(source_spack_cmake):
            print("*** FOUND SPACK CMAKE IN PACKAGE DIR - COPYING ***")
            shutil.copy2(source_spack_cmake, target_main_cmake)
            print("*** COPY COMPLETE ***")
        else:
            print("*** SPACK CMAKE NOT FOUND IN PACKAGE DIR ***")
        
        args = [
            # Explicitly override compiler paths to bypass spack wrapper issues
            self.define("CMAKE_C_COMPILER", "/usr/bin/gcc"),
            self.define("CMAKE_CXX_COMPILER", "/usr/bin/g++"),
            self.define("CMAKE_Fortran_COMPILER", "/usr/bin/gfortran"),
            
            # Set C++ standard and build type
            self.define("CMAKE_CXX_STANDARD", 17),
            self.define("CMAKE_BUILD_TYPE", "RelWithDebInfo"),
            self.define("CASA_BUILD_TYPE", "RelWithDebInfo"),
            
            # Install directories
            self.define("CMAKE_INSTALL_LIBDIR", "lib"),
            self.define("CMAKE_INSTALL_INCLUDEDIR", "include"),
            self.define("CMAKE_INSTALL_BINDIR", "bin"),
            
            # Build options
            self.define("CMAKE_INTERPROCEDURAL_OPTIMIZATION", False),
            self.define("LIBRA_USE_LIBSAKURA", False),
            self.define_from_variant("LIBRA_USE_EXODUS", "exodus"),
            self.define_from_variant("Apps_BUILD_TESTS", "tests"),
        ]

        
        return args

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
        # Use standard CMake build process
        super().build(spec, prefix)
