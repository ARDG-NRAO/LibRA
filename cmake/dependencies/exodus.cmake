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
# Exodus external project - creates relocatable bundle of LibRA applications
include(ExternalProject)

if (LIBRA_USE_EXODUS)
  message("${LIBRA_ID} Exodus bundle creation: ENABLED")
  
  # Find Python 3.10 or higher
  find_package(Python3 3.10 COMPONENTS Interpreter)
  if (NOT Python3_FOUND)
    message(FATAL_ERROR "${LIBRA_ID} Python 3.10+ not found. Please install Python 3.10 or higher to use Exodus")
  endif()
  
  message("${LIBRA_ID} Python3 executable: ${Python3_EXECUTABLE}")
  message("${LIBRA_ID} Python3 version: ${Python3_VERSION}")

  # Set bundle path if not specified
  if (LIBRA_EXODUS_BUNDLE_PATH STREQUAL "")
    set(LIBRA_EXODUS_BUNDLE_PATH ${INSTALL_DIR}/bin)
  endif()

  # Define LibRA applications to bundle
  set(EXODUS_APPS "roadrunner,hummbee,coyote,chip,dale,dataiter,mssplit,subms,tableinfo,acme,taylor")
  
  message("${LIBRA_ID} Bundle output: ${LIBRA_EXODUS_BUNDLE_PATH}/${LIBRA_EXODUS_BUNDLE_NAME}")
  message("${LIBRA_ID} Applications to bundle: ${EXODUS_APPS}")

  # Clone Exodus source
  ExternalProject_Add(
    Exodus_source
    GIT_REPOSITORY ${LIBRA_EXODUS_GIT_REPOSITORY}
    GIT_TAG        ${LIBRA_EXODUS_GIT_TAG}
    SOURCE_DIR     ${LIBRA_EXODUS_SOURCE_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND    ""
    INSTALL_COMMAND  ""
  )

  # Setup Python virtual environment and install Exodus
  ExternalProject_Add(
    Exodus_setup
    DOWNLOAD_COMMAND ""
    SOURCE_DIR      ${LIBRA_EXODUS_SOURCE_DIR}
    BINARY_DIR      ${BUILD_DIR}/exodus_setup
    CONFIGURE_COMMAND ""
    BUILD_COMMAND   ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Setting up Exodus virtual environment and creating bundle..."
                    COMMAND ${Python3_EXECUTABLE} -m venv ${LIBRA_PYTHON_VENV_DIR}
                    COMMAND ${LIBRA_PYTHON_VENV_DIR}/bin/pip3 install <SOURCE_DIR>/
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRA_EXODUS_BUNDLE_PATH}
                    COMMAND ${LIBRA_PYTHON_VENV_DIR}/bin/exodus ${INSTALL_DIR}/bin/{${EXODUS_APPS}} -o ${LIBRA_EXODUS_BUNDLE_PATH}/${LIBRA_EXODUS_BUNDLE_NAME}
    DEPENDS         Exodus_source
  )

else()
  message("${LIBRA_ID} Exodus bundle creation: DISABLED")
endif()