# Copyright Spack Project Developers. See COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack.package import *
from spack.build_systems.cmake import CMakePackage
import os


class Libra(CMakePackage):
    """LibRA (Library of Radio Astronomy) - A radio astronomy data processing library"""

    homepage = "https://github.com/ARDG-NRAO/LibRA"
    git = "https://github.com/ARDG-NRAO/LibRA.git"

    maintainers("ardg-nrao", "pjaganna")

    license("GPL-3.0-or-later", checked_by="pjaganna")

    version("main", branch="main")

    # Variants for optional features
    variant("sakura", default=False, description="Enable libsakura support")
    variant("exodus", default=False, description="Enable exodus support")
    variant("openmp", default=True, description="Enable OpenMP support")
    variant("cuda", default=False, description="Enable CUDA support")
    variant("all", default=False, description="Enable all backends: CUDA, OpenMP, and Serial")
    variant("tests", default=False, description="Build tests")

    # Core dependencies 
    depends_on("kokkos@4.6.01: +openmp+serial+cuda+cuda_lambda", when="+all")
    depends_on("kokkos@4.6.01: +cuda+cuda_lambda", when="+cuda ~all")
    depends_on("kokkos@4.6.01: +openmp+serial", when="+openmp ~cuda ~all")
    depends_on("kokkos@4.6.01: +serial", when="~openmp ~cuda ~all")
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
    depends_on("cuda", when="+cuda")
    depends_on("cuda", when="+all")
    
    # Optional dependencies
    depends_on("libsakura@5.1.3:", when="+sakura")
    depends_on("googletest@1.14.0:", when="+tests")
    
    # Build dependencies
    depends_on("cmake@3.18:", type="build")
    depends_on("pkgconfig", type="build")
    depends_on("curl", type="build")  # For downloading measures data
    
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
    depends_on("fortran", type="build")

    def cmake_args(self):
        args = [
            # Set C++ standard and build type
            self.define("CMAKE_CXX_STANDARD", 17),
            self.define("BUILD_TYPE", "RelWithDebInfo"),
            self.define("CASA_BUILD_TYPE", "RelWithDebInfo"),
            
            # Install directories
            self.define("CMAKE_INSTALL_LIBDIR", "lib"),
            self.define("CMAKE_INSTALL_INCLUDEDIR", "include"),
            self.define("CMAKE_INSTALL_BINDIR", "bin"),
            
            # Build options
            self.define("CMAKE_INTERPROCEDURAL_OPTIMIZATION", False),
            self.define_from_variant("LIBRA_USE_LIBSAKURA", "sakura"),
            self.define_from_variant("LIBRA_USE_EXODUS", "exodus"),
            self.define_from_variant("Apps_BUILD_TESTS", "tests"),
        ]
        
        # Set number of cores for parallel builds
        import multiprocessing
        ncores = max(1, multiprocessing.cpu_count() - 2)
        # Override with Spack's build_jobs if available
        if hasattr(self, 'make_jobs') and self.make_jobs:
            ncores = self.make_jobs
        args.append(self.define("NCORES", ncores))
        
        # CUDA-specific configurations
        if "+cuda" in self.spec or "+all" in self.spec:
            # Let CMake handle CUDA architecture detection
            pass
        
        return args

    def setup_build_environment(self, env):
        # Set PKG_CONFIG_PATH for dependencies
        env.prepend_path("PKG_CONFIG_PATH", self.spec.prefix.lib.pkgconfig)
        
        # Add CUDA to PATH if available
        if ("+cuda" in self.spec or "+all" in self.spec) and "cuda" in self.spec:
            env.prepend_path("PATH", self.spec["cuda"].prefix.bin)

    @run_after("patch")
    def setup_spack_cmake(self):
        # Copy Spack-specific CMakeLists.txt to override the main one
        import shutil
        spack_cmake = join_path(self.stage.source_path, "CMakeLists.txt.spack")
        main_cmake = join_path(self.stage.source_path, "CMakeLists.txt")
        tty.msg("LibRA: Looking for Spack CMakeLists.txt at: {0}".format(spack_cmake))
        if os.path.exists(spack_cmake):
            tty.msg("LibRA: Copying Spack CMakeLists.txt to main CMakeLists.txt")
            shutil.copy2(spack_cmake, main_cmake)
        else:
            tty.msg("LibRA: Spack CMakeLists.txt not found, using original")

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
