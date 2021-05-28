//# CalibratingVi2FactoryI.cc: Implementation of the CalibratingVi2FactoryI class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#include <msvis/MSVis/CalibratingVi2FactoryI.h>
#include <casa/Exceptions.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
namespace vi { //# NAMESPACE VI - BEGIN

CalibratingVi2FactoryI::CalViFacGenerator CalibratingVi2FactoryI::generator_p=0;
CalibratingVi2FactoryI::CalViFac_byRec_Generator CalibratingVi2FactoryI::byRec_generator_p=0;


// -----------------------------------------------------------------------
CalibratingVi2FactoryI* 
CalibratingVi2FactoryI::generate() {

  ThrowIf (generator_p==0, "No CalibratingVi2FactoryI::generator (generic) available!");

  //cout << "CVFI::generate()" << endl;

  return generator_p();

}
// -----------------------------------------------------------------------
CalibratingVi2FactoryI* 
CalibratingVi2FactoryI::generate(MeasurementSet* ms, 
				 const Record& calrec,
				 const IteratingParameters& iterpar) {

  ThrowIf (byRec_generator_p==0, "No CalibratingVi2FactoryI::generator (by Record) available!");

  //cout << "CVFI::generate(ms,calrec,iterpar)" << endl;

  return byRec_generator_p(ms,calrec,iterpar);

}

// -----------------------------------------------------------------------
Bool
CalibratingVi2FactoryI::setGenerator(CalViFacGenerator cvfg) {

  generator_p=cvfg;

  return true;

}
// -----------------------------------------------------------------------
Bool
CalibratingVi2FactoryI::set_byRec_Generator(CalViFac_byRec_Generator cvfg) {

  byRec_generator_p=cvfg;

  return true;

}


} //# NAMESPACE VI - END
} //# NAMESPACE CASA - END


