//# SimpleSubMS.h: this defines SubMS which creates a subset of an casacore::MS with some
//# transformation
//# Copyright (C) 1997,1998,1999,2000,2001,2003
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
//# $Id$

#include <msvis/MSVis/SubMS.h>


#ifndef MSVIS_SIMPLESUBMS_H

namespace casa { //# NAMESPACE CASA - BEGIN

#define MSVIS_SUBMS_H
//
// <summary>
// SimpleSubMS provide a simpler subset of SubMS. 
// The purpose of this class to avoid unnecessary IO  for some simplistic selection and subMSsing
// No averaging no fanciness...simple time and channel subsets.
// </summary>

// <visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MeasurementSet
// </prerequisite>
//
// <etymology>
// SimpleSubMS ...from the SUBset of an casacore::MS and Simple
// </etymology>
//
// <synopsis>
// The order of operations (as in ms::split()) is:
//      ctor
//      setmsselect
//      selectTime
//      makeScratchSubMS
// </synopsis>

  class SimpleSubMS : public SubMS
{

 public:

  // Constructor with an ms
  SimpleSubMS(casacore::MeasurementSet& ms);
  //Die...die
  ~SimpleSubMS();
  // In this form whichDataCol gets passed to parseColumnNames().
  virtual casacore::MeasurementSet* makeMemSubMS(const casacore::MS::PredefinedColumns& datacol, const casacore::String& diskname=casacore::String(""));
  



};


} //# NAMESPACE CASA - END

#endif
