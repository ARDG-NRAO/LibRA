# Copyright Spack Project Developers. See COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack_repo.builtin.build_systems.cmake import CMakePackage

from spack.package import *
import os
import shutil


class Parafeed(CMakePackage):
    """Parafeed is a C++ library to parse and interact with the commandline."""

    homepage = "https://github.com/sanbee/parafeed"
    url = "https://github.com/sanbee/parafeed/archive/refs/tags/v1.1.43.tar.gz"

    maintainers("sanbee", "sanbw", "preshanth")

    license("GPL-3.0", checked_by="sanbee")

    version("1.1.43", sha256="ded82f11b32535a02d805b26acfd06d5a406ddc9e4cc4a64606b513a65315338")
    version("1.1.41", sha256="a5cb034914cab8b661469f2f1aae6af7d88045d54f3535da8ff004a04cd8abd3")
    version("1.1.4", sha256="9ca767aa1139516926456fff14fec466890caba5646e46f0553c7b648bb5510e")
    version("1.1.2", sha256="74eb62034b0c4cbfd9a4a05872c10d568b184e8cf7543405d460fc0c52001139")
    version("1.1.1", sha256="757290ec76b3762febbaadca4c597f9d668be55767dbcfdba305ddc48cb07689")

    # Build-time and run-time dependencies
    depends_on("readline")
    depends_on("bison")
    depends_on("flex")
    depends_on("pkgconfig", type="build")
    depends_on("cmake@3.1:", type="build")

    # C++ standard variant
    variant(
        "cxxstd",
        default="11",
        values=("11", "14", "17", "20", "23"),
        description="C++ standard to use",
    )
    variant("shared", default=True, description="Build shared libraries")
    
    def patch(self):
        # Copy CMakeLists.txt.code
        src_cmake = os.path.join(os.path.dirname(__file__), "CMakeLists.txt.code")
        dst_cmake = os.path.join(self.stage.source_path, "code", "CMakeLists.txt")
        if os.path.exists(src_cmake):
            shutil.copy(src_cmake, dst_cmake)
    
        # Copy FindReadline.cmake  
        src_find = os.path.join(os.path.dirname(__file__), "FindReadline.cmake")
        dst_find = os.path.join(self.stage.source_path, "cmake", "FindReadline.cmake")
        if os.path.exists(src_find):
            shutil.copy(src_find, dst_find)

        # Copy parafeed.pc.in
        src_pc = os.path.join(os.path.dirname(__file__), "parafeed.pc.in")
        dst_pc = os.path.join(self.stage.source_path, "parafeed.pc.in")
        if os.path.exists(src_pc):
            shutil.copy(src_pc, dst_pc)
            
        # Copy parafeedConfig.cmake 
        src_config = os.path.join(os.path.dirname(__file__), "parafeedConfig.cmake")
        dst_config = os.path.join(self.stage.source_path, "code", "parafeedConfig.cmake")
        if os.path.exists(src_config):
            shutil.copy(src_config, dst_config)
            
    def cmake_args(self):
        args = [
            self.define("CMAKE_CXX_STANDARD", self.spec.variants["cxxstd"].value),
            self.define("CMAKE_CXX_STANDARD_REQUIRED", "ON"),
            self.define("CMAKE_CXX_EXTENSIONS", "OFF"),
            self.define_from_variant("BUILD_SHARED_LIBS", "shared"),
        ]
        return args
