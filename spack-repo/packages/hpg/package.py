# Copyright 2013-2020 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack_repo.builtin.build_systems.cmake import CMakePackage
from spack.package import *


class Hpg(CMakePackage):
    """High performance (de-)gridding kernel implementation library"""

    homepage = "https://gitlab.com/dsa-2000/rcp/hpg"
    git = "https://gitlab.com/dsa-2000/rcp/hpg.git"
    url = "https://gitlab.com/dsa-2000/rcp/hpg/-/archive/v2.0.1/hpg-v2.0.1.tar.gz"
    maintainers("mpokorny")

    license("Apache-2.0", checked_by="mpokorny")

    version("main", branch="main")
    version("3.2.1",
            sha256="eff88fb1d30ccb959db04eee7effbd7e613a0ec6cd40deabbe6fba2aa2180444")
    version("3.2.0",
            sha256="e11f6a1a68f81b6a14782bee17b5744357242255dc9e6d74700852cac63d6532")
    version("3.1.1",
            sha256="dbbf890a946037b473a70203808a156fbd588a59953f4311195f8e339650d1c0")
    version("3.1.0",
            sha256="d936484d4198dadfeb38ed8aa5f62450f5af2d2d85eb2baf1fc6ce4b56024e22")
    version("3.0.4",
            sha256="489bbfd4bad1cc9bd412e9c2b595e0cea4e8b433af46a0c7e14008bf0e816c5a")
    version("3.0.3",
            sha256="d8a59982ce5129bffea4c92068c65e125456aafb6605d60ee8212888eef2042e")
    version("3.0.2",
            sha256="7daf632b23f94cee6c4f701f5b8ac0d8639865de08925b4538729e9fd55a6cca")
    version("3.0.1",
            sha256="a611b45f0021592016726efdba5121b1991aefc7df900a7aad41f23d78aa57b6")
    version("2.0.2",
            sha256="2f7eb6f172f895f699a95120e16f3e8e01cdbfc242192c106545fab661366086")
    version("2.0.1",
            sha256="f201cb31ad961d097846b7dc77f5850e40f7939b42029c12d8a322dd3cbecd46")
    version("1.2.3",
            commit="2afac2a1a3d02c9c90d3bd64afa6ecf5b7182102")
    version("1.2.2",
            commit="3291b9a3218fe5876cafbefd730d34e886b2426a")

    variant("api", default="17", description="C++ standard for API",
            values=("11", "17"), multi=False)
    variant("cuda", default=False, description="Enable CUDA device")
    variant("exptl", default=False,
            description="Enable experimental kernel implementations")
    variant("max_cf_grps", default="1000",
            description="Maximum number of CF groups")
    variant("mixed", default=False, description="Use mixed precision in gridding kernels")
    variant("openmp", default=False, description="Enable OpenMP device")
    variant("serial", default=True, description="Enable serial device")
    variant("shared", default=True, description="Build shared libraries")

    variant("array_index", default="uint", description="Array index value type",
            values=("uint", "ulong", "ullong"), multi=False)

    variant("cf_fp", default="float", description="CF value base fp type",
            values=("float", "double"), multi=False)
    variant("vis_fp", default="float",
            description="Visibility value base fp type",
            values=("float", "double"), multi=False)
    variant("grid_fp", default="double",
            description="Grid pixel value base fp type",
            values=("float", "double"), multi=False)
    variant("uvw_fp", default="double", description="UVW value fp type",
            values=("float", "double"), multi=False)
    variant("freq_fp", default="double",
            description="Channel frequency value fp type",
            values=("float", "double"), multi=False)
    variant("phase_fp", default="double",
            description="Visibility phase value fp type",
            values=("float", "double"), multi=False)

    variant("flgbits", default="8", description="Flag value number of bits",
            values=("8", "16"), multi=False)

    variant("uvw_rank", default="2", description="UVW values array rank",
            values=("2", "3"), multi=False)
    variant("cfidx_rank", default="2", description="CF arrays index value rank",
            values=("1", "2"), multi=False)

    conflicts("~openmp", when="~serial",
              msg="At least one host device must be enabled")
    conflicts("~exptl", when="+mixed",
              msg="Mixed precision is supported only in experimental kernels")

    depends_on("cxx", type="build")
    depends_on("c", type="build")

    depends_on("cmake@3.18:", type="build", when="@:2")
    depends_on("cmake@3.23:", type="build", when="@3:")
    depends_on("pkgconfig", type="build")
    depends_on("hdf5@1.13: +cxx", when="@:2")
    depends_on("hdf5@1.14: +cxx", when="@3:")

    depends_on("cuda@11.0.2:", when="+cuda")
    depends_on("fftw@3.3.8: precision=float,double", when="+serial")
    depends_on("fftw@3.3.8: +openmp precision=float,double", when="+openmp")
    depends_on("kokkos@3.2.00:3.99.99", when="@:1.2.3")
    depends_on("kokkos@4.0.0:4.99.99", when="@2:")
    depends_on("kokkos+cuda+cuda_lambda+wrapper", when="+cuda")
    depends_on("kokkos+openmp", when="+openmp")
    depends_on("kokkos+serial", when="+serial")

    conflicts("kokkos cxxstd=11")
    conflicts("kokkos cxxstd=14")
    

    index_type_map = {
        "uint": "unsigned int",
        "ulong": "unsigned long",
        "ullong": "unsigned long long"
    }

    def common_cmake_args(self):
        return [
            self.define("INSTALL_GTEST", False),
            self.define("BUILD_GMOCK", False),
            self.define("Hpg_BUILD_DOCS", False),
            self.define("Hpg_BUILD_TESTS", self.run_tests),
            self.define_from_variant("BUILD_SHARED_LIBS", "shared"),
            self.define_from_variant("Hpg_ENABLE_SERIAL", "serial"),
            self.define_from_variant("Hpg_ENABLE_OPENMP", "openmp"),
            self.define_from_variant("Hpg_ENABLE_CUDA", "cuda"),
            self.define_from_variant("Hpg_API", "api"),
            self.define_from_variant("Hpg_MAX_NUM_CF_GROUPS", "max_cf_grps"),
            self.define_from_variant("Hpg_ENABLE_MIXED_PRECISION", "mixed"),
            self.define_from_variant(
                "Hpg_ENABLE_EXPERIMENTAL_IMPLEMENTATIONS",
                "exptl"),
            self.define(
                "Hpg_ARRAY_INDEX_TYPE",
                self.index_type_map[self.spec.variants["array_index"].value]),
            self.define_from_variant("Hpg_CF_FP_TYPE", "cf_fp"),
            self.define_from_variant("Hpg_VIS_FP_TYPE", "vis_fp"),
            self.define_from_variant("Hpg_GRID_FP_TYPE", "grid_fp"),
            self.define_from_variant("Hpg_UVW_FP_TYPE", "uvw_fp"),
            self.define_from_variant("Hpg_FREQ_FP_TYPE", "freq_fp"),
            self.define_from_variant("Hpg_PHASE_FP_TYPE", "phase_fp"),
            self.define_from_variant("Hpg_NUM_FLAG_BITS", "flgbits"),
            self.define_from_variant("Hpg_UVW_INDEX_RANK", "uvw_rank"),
            self.define_from_variant("Hpg_CF_INDEX_RANK", "cfidx_rank")]

    @when("^kokkos@:3.2")
    def cmake_args(self):
        args = self.common_cmake_args()
        if self.spec.satisfies("^kokkos-nvcc-wrapper~mpi"):
            args.extend([
                self.define(
                    "CMAKE_CXX_COMPILER",
                    self.spec["kokkos"].kokkos_cxx)])
        elif self.spec.satisfies("^kokkos-nvcc-wrapper+mpi"):
            args.extend([
                self.define(
                    "CMAKE_CXX_COMPILER",
                    self.spec["mpi"].mpicxx)])
        return args

    @when("^kokkos@3.3:")
    def cmake_args(self):
        return self.common_cmake_args()
