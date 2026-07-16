// -*- C++ -*-
//# LibracoreUtils.cc: Implementation of global functions from LibracoreUtils.h
//# Copyright (C) 2026
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
//
//-------------------------------------------------------------------------------------
//
// Converts a basic glob pattern (* and ?) into a std::regex string

#ifndef LIBRACOREUTILS_H
#define LIBRACOREUTILS_H

#include <algorithm>
#include <regex>
#include <filesystem>
//
//-------------------------------------------------------------------------------------
//
namespace libracore
{
  namespace utils
  {
    inline  bool directoryExists(const std::string& path)
    {
      std::filesystem::path dir(path);
      return std::filesystem::is_directory(dir);
    }
    inline  bool create_directory(const std::filesystem::path& path)
    {
      return std::filesystem::create_directory(path);
    }
    inline  bool remove_directory(const std::filesystem::path& path)
    {
      return std::filesystem::remove_all(path);
    }
    std::string glob_to_regex(const std::string& glob);
    std::vector<std::string> getDirListing(const std::string& path,
					   const std::string& glob);
  };
};
#endif
