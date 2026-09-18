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
# License for more details.
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
# FindContainerRuntime.cmake
# Detects Docker and Singularity/Apptainer availability and validates they work

# Initialize found flags
set(DOCKER_FOUND FALSE)
set(SINGULARITY_FOUND FALSE)

# Detect Docker
find_program(DOCKER_EXECUTABLE docker)
if(DOCKER_EXECUTABLE)
    # Test if docker actually works
    execute_process(
        COMMAND ${DOCKER_EXECUTABLE} --version
        RESULT_VARIABLE DOCKER_VERSION_RESULT
        OUTPUT_VARIABLE DOCKER_VERSION_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(DOCKER_VERSION_RESULT EQUAL 0)
        # Docker exists and version command works
        # Now test if we can actually use it (permissions check)
        execute_process(
            COMMAND ${DOCKER_EXECUTABLE} ps
            RESULT_VARIABLE DOCKER_PS_RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )

        if(DOCKER_PS_RESULT EQUAL 0)
            set(DOCKER_FOUND TRUE)
            message("${LIBRA_ID} Docker found: ${DOCKER_EXECUTABLE}")
            message("${LIBRA_ID} Docker version: ${DOCKER_VERSION_OUTPUT}")
        else()
            message("${LIBRA_ID} Docker found but 'docker ps' failed - check permissions")
        endif()
    endif()
endif()

if(NOT DOCKER_FOUND AND DOCKER_EXECUTABLE)
    message("${LIBRA_ID} Docker executable found but not working: ${DOCKER_EXECUTABLE}")
endif()

# Detect Singularity (try singularity first, then apptainer)
find_program(SINGULARITY_EXECUTABLE singularity)
if(NOT SINGULARITY_EXECUTABLE)
    find_program(SINGULARITY_EXECUTABLE apptainer)
    if(SINGULARITY_EXECUTABLE)
        set(SINGULARITY_IS_APPTAINER TRUE)
    endif()
endif()

if(SINGULARITY_EXECUTABLE)
    # Test if singularity/apptainer actually works
    execute_process(
        COMMAND ${SINGULARITY_EXECUTABLE} --version
        RESULT_VARIABLE SINGULARITY_VERSION_RESULT
        OUTPUT_VARIABLE SINGULARITY_VERSION_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(SINGULARITY_VERSION_RESULT EQUAL 0)
        set(SINGULARITY_FOUND TRUE)
        if(SINGULARITY_IS_APPTAINER)
            message("${LIBRA_ID} Apptainer found: ${SINGULARITY_EXECUTABLE}")
        else()
            message("${LIBRA_ID} Singularity found: ${SINGULARITY_EXECUTABLE}")
        endif()
        message("${LIBRA_ID} Version: ${SINGULARITY_VERSION_OUTPUT}")
    endif()
endif()

if(NOT SINGULARITY_FOUND AND SINGULARITY_EXECUTABLE)
    message("${LIBRA_ID} Singularity/Apptainer executable found but not working: ${SINGULARITY_EXECUTABLE}")
endif()

# Summary
if(NOT DOCKER_FOUND AND NOT SINGULARITY_FOUND)
    message("${LIBRA_ID} No container runtime detected (Docker or Singularity/Apptainer)")
endif()
