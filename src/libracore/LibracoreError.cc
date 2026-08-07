//# LibracoreError.cc: Error classes for the Libracore module
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

#include <libracore/LibracoreError.h>

using namespace std;
namespace libracore
{
  namespace exception
  {
    LibracoreError::LibracoreError (Category c)
      : message(), category(c), std::exception()
      {}
    LibracoreError::LibracoreError (const std::string& str,Category c)
      :  message(str), category(c), std::exception()
      {}
  
    void LibracoreError::addMessage(std::string& mesg)
    {
      message = message+mesg;
    }
  
    void LibracoreError::changeMessage(std::string& mesg)
    {
      message = mesg;
    }
  };
};
