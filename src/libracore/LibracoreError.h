//# SynthesisError.h: Synthesis module exception classes
//# Copyright (C) 2026
//# Associated Universities, Inc. Washington DC, USA.
//#
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

#ifndef LIBRACORE_ERROR_H
#define LIBRACORE_ERROR_H

//# Includes
#include <libracore/LibracoreError.h>
#include <exception>
#include <string>

namespace libracore
{
  namespace exception
  {

    enum Category {GENERAL, INFORMATIONAL, SEVERE, FATAL};
    
    class LibracoreError : public std::exception
    {
    public:

      LibracoreError ();
      LibracoreError (Category c=GENERAL);
      // Construct with given message.
      LibracoreError (const std::string& message,Category c=GENERAL);
      ~LibracoreError () throw() {};

      void changeMessage(std::string& message);
      void addMessage(std::string& message);
      std::string what() {return message;};
    private:
      std::string message;
      Category category;
    };
  };
};

#endif
