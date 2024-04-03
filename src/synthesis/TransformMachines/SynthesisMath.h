//# SynthesisMath.h: Definition for math functions
//# Copyright (C) 2008
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#ifndef SYNTHESIS_SYNTHESISMATH_H
#define SYNTHESIS_SYNTHESISMATH_H

namespace casa{
  //
  // All compiler/OS specific macros used in the systhesis module go
  // here (project managers, architects and systems team could not
  // make up their mind for a system-wide solution that also does not
  // pollute the code).
  //
#if defined(__APPLE__)
#define SINCOS(a,s,c) {s = sin(a); c = cos(a);}
#else
#define SINCOS(a,s,c) sincos(a,&s,&c)
#endif
}; // end of namespace casa

#endif
