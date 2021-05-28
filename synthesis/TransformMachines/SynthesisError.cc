//# SynthesisError.cc: Error classes for the Synthesis module
//# Copyright (C) 1994,1995,1996,1997,2000
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

#include <synthesis/TransformMachines/SynthesisError.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

SynthesisError::SynthesisError (Category c)
: AipsError("Synthesis Error",c)
{}
SynthesisError::SynthesisError (const String& str,Category c)
: AipsError(str,c)
{}
SynthesisError::~SynthesisError () throw()
{}

void SynthesisError::addMessage(String& mesg)
{
  message = message+mesg;
}

void SynthesisError::changeMessage(String& mesg)
{
  message = mesg;
}

SynthesisFTMachineError::SynthesisFTMachineError (const String& str,Category c)
: SynthesisError(str,c)
{}
SynthesisFTMachineError::~SynthesisFTMachineError () throw()
{}

PSFZero::PSFZero(const String& str, Category c)
  :SynthesisFTMachineError(str,c) 
{}
PSFZero::~PSFZero() throw()
{};

CFNotCached::CFNotCached(const String& str, Category c)
  :SynthesisError(str,c) 
{}
CFNotCached::~CFNotCached() throw()
{};

} //# NAMESPACE CASA - END

