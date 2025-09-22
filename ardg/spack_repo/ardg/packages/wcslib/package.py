# Copyright Spack Project Developers. See COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack_repo.builtin.build_systems.autotools import AutotoolsPackage

from spack.package import *


class Wcslib(AutotoolsPackage):
    """WCSLIB a C implementation of the coordinate transformations
    defined in the FITS WCS papers."""

    homepage = "https://www.atnf.csiro.au/people/mcalabre/WCS/wcslib/"
    url = "ftp://ftp.atnf.csiro.au/pub/software/wcslib/wcslib-7.3.tar.bz2"

    license("LGPL-3.0-or-later", checked_by="lgarrison")

    version(
        "8.4", sha256="960b844426d14a8b53cdeed78258aa9288cded99a7732c0667c64fa6a50126dc"
    )
    version(
        "7.3", sha256="4b01cf425382a26ca4f955ed6841a5f50c55952a2994367f8e067e4183992961"
    )
    version(
        "6.4", sha256="13c11ff70a7725563ec5fa52707a9965fce186a1766db193d08c9766ea107000"
    )

    variant("cfitsio", default=False, description="Include CFITSIO support")
    variant("x", default=False, description="Use the X Window System")

    depends_on("c", type="build")
    depends_on("fortran", type="build")

    depends_on("curl")
    depends_on("gmake", type="build")
    depends_on("flex@2.5.9:", type="build")
    depends_on("cfitsio", when="+cfitsio")
    depends_on("libx11", when="+x")

    def configure_args(self):
        spec = self.spec

        # TODO: Add PGPLOT package
        args = ["--without-pgplot"]

        if "+cfitsio" in spec:
            args.extend(
                [
                    "--with-cfitsio",
                    "--with-cfitsiolib={0}".format(spec["cfitsio"].libs.directories[0]),
                    "--with-cfitsioinc={0}".format(
                        spec["cfitsio"].headers.directories[0]
                    ),
                ]
            )
        else:
            args.append("--without-cfitsio")

        if "+x" in spec:
            args.append("--with-x")
        else:
            args.append("--without-x")

        return args

    @run_after("install")
    def darwin_fix(self):
        # The shared library is not installed correctly on Darwin; fix this
        if self.spec.satisfies("platform=darwin"):
            fix_darwin_install_name(self.prefix.lib)
