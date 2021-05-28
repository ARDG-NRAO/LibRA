
//# RFFlagCube.cc: this defines RFFlagCube
//# Copyright (C) 2000,2001,2002
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
#include <flagging/Flagging/RFFlagCube.h>
#include <casa/Exceptions/Error.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/LogiVector.h>
#include <casa/Utilities/Regex.h>
#include <casa/OS/Time.h>
#include <casa/Quanta/MVTime.h>
#include <stdio.h>

#include <casa/System/PGPlotterInterface.h>
        
namespace casa { //# NAMESPACE CASA - BEGIN

template<class T> casacore::LogicalArray  maskBits  ( const casacore::Array<T> &arr,const T &val)
{
  return (arr&val) != (T)0;
}

} //# NAMESPACE CASA - END

