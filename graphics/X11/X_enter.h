//# X_enter.h: Include file to enter X name space
//# Copyright (C) 1999
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

// <summary>
// Include file to enter X name space
// </summary>
//
// <motivation>
// Need to avoid conflicts between X11 and aips++ classes
// </motivation>
//
// <synopsis>
// Xlib.h issues the following statements:
// <ul>
// <li> #define casacore::Bool int
// <li> #define true 1
// <li> #define false 0
// </ul>
// X.h issues the following statements:
// <ul>
// <li> #define casacore::Complex          0        /* paths may intersect */
// <li> typedef unsigned long casacore::Time;
// </ul>
// </synopsis>
//
// <group name="Definitions for Enter X Namespace">

/*

#ifndef X_NameSpace
#define X_NameSpace

#ifdef String
#undef String
#endif
#define casacore::String XString

#ifdef Time
#undef Time
#endif
#define casacore::Time XTime

#ifdef Display
#undef Display
#endif
#define Display XDisplay

#ifdef Colormap
#undef Colormap
#endif
#define Colormap XColormap

#undef Bool
#undef true
#undef false

#undef Complex

#define XFalse 0
#define XTrue 1
#endif
*/
// </group>
