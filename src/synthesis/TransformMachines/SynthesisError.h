//# SynthesisError.h: Synthesis module exception classes
//# Copyright (C) 1994,1995,1996,1997,1999,2000
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

#ifndef SYNTHESIS_SYNTHESISERROR_H
#define SYNTHESIS_SYNTHESISERROR_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Exceptions/Error.h>


namespace casa { //# NAMESPACE CASA - BEGIN

  //# This header file defines the error classes thrown by the
  //# Synthesis module and related classes.


  // <summary>
  // </summary>
  // <use visibility=export>
  // <reviewed reviewer="UNKNOWN" date="" tests="">
  // </reviewed>

  // <synopsis> 
  //
  // The top-level generic SynthesisError exception class.  All
  // exceptions either generated in the synthesis module, or
  // exceptions caught in the synthesis module and transformed into
  // more specialized exceptions are derived from this.  Catching this
  // class will catch only Synthesis module exceptions (but all
  // exceptions from the Synthesis module).  To catch more specific
  // exceptions, catch the derived classes.  Note that you have to
  // catch casacore::AipsError to catch all possible exceptions thrown by all of
  // AIPS++ modules!
  //
  //</synopsis>
  
  class SynthesisError : public casacore::AipsError {
  public:
    // The default constructor generates the message "casacore::Table error".
    SynthesisError (Category c=GENERAL);
    // Construct with given message.
    void changeMessage(casacore::String& message);
    void addMessage(casacore::String& message);
    SynthesisError (const casacore::String& message,Category c=GENERAL);
    ~SynthesisError () throw();
  };
  
  
  // <summary>
  // Error thrown by the classes in the Synthesis module
  // </summary>
  // <use visibility=export>
  // <reviewed reviewer="UNKNOWN" date="" tests="">
  // </reviewed>
  
  // <synopsis> 
  // </synopsis> 
  
  class SynthesisFTMachineError : public SynthesisError {
  public:
    SynthesisFTMachineError (const casacore::String& message,Category c=GENERAL);
    ~SynthesisFTMachineError () throw();
  };

  class PSFZero: public SynthesisFTMachineError {
  public:
    PSFZero (const casacore::String& message,Category c=GENERAL);
    ~PSFZero () throw();
  };

  class NoSenPattern: public SynthesisFTMachineError {
  public:
    NoSenPattern(const casacore::String& message, Category c=GENERAL);
    ~NoSenPattern() throw();
  };

  class CFNotCached: public SynthesisError{
  public:
    CFNotCached(const casacore::String& message, Category c=GENERAL);
    ~CFNotCached() throw();
  };
} //# NAMESPACE CASA - END

#endif
