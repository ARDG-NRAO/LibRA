#-*- cmake -*-
# Copyright (C) 2025
# Associated Universities, Inc. Washington DC, USA.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.is
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning this should be addressed as follows:
#        Postal address: National Radio Astronomy Observatory
#                        1003 Lopezville Road,
#                        Socorro, NM - 87801, USA
#

if (DEFINED ENV{SPACK_ROOT})
  if (DEFINED ENV{SPACK_ENV})
    set(SPACKENV_FOUND TRUE)
    set(SPACK_VIEW_ROOT "$ENV{SPACK_ENV}/.spack-env/view/")
  else()
    message(WARNING "SPACK_ROOT is defined, but not SPACK_ENV. "
      "If you intended to build in an spack env, please activate env for us to find the path to the root of the spack env.")
  endif()
endif()
