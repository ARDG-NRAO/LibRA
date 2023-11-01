//# nreal.h: this defines nreal time structures used by the VLA table filler
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
//#
//# $Id: nreal.h,v 19.3.18.3 2006/02/17 23:46:40 wyoung Exp $
//#! ========================================================================

#ifndef NRAO_NREAL_H
#define NRAO_NREAL_H
//
//# Forward Declarations
//
// <summary>
// Defines nreal time structures used by the VLA table filler
// </summary>
//
// <use visibility=local>   or   <use visibility=export>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
//
// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <templating arg=T>
//    <li>
//    <li>
// </templating>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="yyyy/mm/dd">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>
//
//class ClassName
//{
//public:
//
//protected:
//
//private:
//
//};
//
//
// Global Functions
//
// <linkfrom anchor=unique-string-within-this-file
//classes="class-1,...,class-n">
//     <here> Global functions </here> for foo and bar.
// </linkfrom>
//
// comments for this group of related global functions
// go here...
//
// <group name=unique-string-within-this-file>  
// </group>
//
// comments about the following group
// <group name=another-unique-string-within-this-file>
// </group>


/* Simple union to help make an internet addr printable in the
123.456.789.123 format.  Likely unnecessary but I'm learning */

#include <stdio.h>

#define BUF_SIZE   2048
#define ONLINE_RECORD_SIZE_MAX 26624
#define PHYS_RECORD_SIZE 26624
#define MAX_LOGICAL_RECORD_SIZE 850000
/* #define MAX_TIME 60 */

struct TapeHeader{
   short current;
   short total;
};

void attachFile(const char *);
void detachFile();
int olopen_(int *, char *, int);
int oladvf_(int *, int *);
int readVLALogRec(char *);

#endif
