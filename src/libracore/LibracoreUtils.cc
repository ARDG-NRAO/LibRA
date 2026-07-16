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
#include <libracore/LibracoreError.h>
#include <libracore/LibracoreUtils.h>

using namespace libracore::exception;
namespace libracore
{
  namespace utils
  {
    
    std::string glob_to_regex(const std::string& glob)
    {
      std::string regex_str = "^";
      for (char c : glob)
	{
	  switch (c)
	    {
	    case '*': 
	      regex_str += ".*"; 
	      break;
	    case '?': 
	      regex_str += "."; 
	      break;
	      // Escape special regex characters
	    case '.': case '+': case '^': case '$': case '|':
	    case '(': case ')': case '[': case ']': case '{': case '}':
	      regex_str += '\\';
	      [[fallthrough]]; // C++17 attribute
	    default: 
	      regex_str += c; 
	      break;
	    }
	}
      regex_str += "$";
      return regex_str;
    }
    //
    //-------------------------------------------------------------------------------------
    //
    std::vector<std::string> getDirListing(const std::string& path,
					   const std::string& glob)
    {
      if (!directoryExists(path))
	throw(LibracoreError(path+" directory not found"));
      
      // Compile the regular expression
      const std::regex pattern(glob_to_regex(glob));
      
      const std::filesystem::path dirPath(path);
      std::vector<std::string> dirList,filteredList;
      
      // directory_iterator can be iterated using a range-for loop
      for (auto const& dirEntry : std::filesystem::directory_iterator{dirPath}) 
	dirList.push_back(dirEntry.path().filename().string());
      
      // Filter the vector using C++17 copy_if
      std::copy_if(dirList.begin(), dirList.end(), 
		   std::back_inserter(filteredList),
		   [&pattern](const std::string& str) {
		     return std::regex_match(str, pattern);
		   });
      
      return filteredList;
    }
  };
};
