# /*
#  * Copyright (c) 2000-2012, 2013 S. Bhatnagar (bhatnagar dot sanjay at gmail dot com)
#  *
#  * This program is free software; you can redistribute it and/or modify
#  * it under the terms of the GNU General Public License as published by
#  * the Free Software Foundation; either version 2 of the License, or
#  * (at your option) any later version.
#  *
#  * This program is distributed in the hope that it will be useful,
#  * but WITHOUT ANY WARRANTY; without even the implied warranty of
#  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  * GNU General Public License for more details.
#  *
#  * You should have received a copy of the GNU General Public License
#  * along with this program; if not, write to the Free Software
#  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  *
#  */
cmake_minimum_required(VERSION 3.18)
find_package(PkgConfig REQUIRED)
if (GNUREADLINE)
  pkg_search_module(readline REQUIRED readline)
endif()

# parafeed libraries configurations
include(${CMAKE_CURRENT_LIST_DIR}/parafeed_libraries.cmake)

# Ensure parafeed target has readline linked for downstream projects
if (TARGET parafeed::parafeed AND GNUREADLINE)
  target_link_libraries(parafeed::parafeed INTERFACE readline ncurses)
endif()
