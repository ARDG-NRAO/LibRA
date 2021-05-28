//# CalibratingVi2Factory.h: Interface definition of the CalibratingVi2Factory class.
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

#ifndef CalibratingVi2Factory_H_
#define CalibratingVi2Factory_H_

// Where ViFactory interface is defined
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/IteratingParameters.h>
#include <msvis/MSVis/CalibratingVi2FactoryI.h>


// Implementation returned by the factory method
#include <synthesis/MeasurementComponents/CalibratingVi2.h>


namespace casa { //# NAMESPACE CASA - BEGIN
namespace vi { //# NAMESPACE VI - BEGIN

// <summary>
// A top level class defining the data handling interface for the CalibratingTvi2 module
// </summary>
//
// <use visibility=export>
//
// <prerequisite>
//   <li> <linkto class="VisibilityIterator2:description">VisibilityIterator2</linkto>
//   <li> <linkto class="CalibratingVi2:description">CalibratingVi2</linkto>
// </prerequisite>
//
// <etymology>
// CalibratingVi2Factory is a class that generates a Visibility Iterator implementation 
// that can calibrate data
// </etymology>
//
// <synopsis>
// CalibratingVi2Factory generates a Visibility Iterator implementation (ViImplementation2)
// object that can be plugged into a Visibility Iterator (VisibilityIterator2) object,
// so that the user can access _corrected_ data (and related) using the VisibilityIterator2 
// interface.
// </synopsis>
//
// <motivation>
// This class makes calibration application portable, and available for all VisibilityIterator2
//  users
// </motivation>
//
// <example>
// External usage is quite simple, and compliant with the 'normal' VI/VB framework.
//
// The user first makes objects describing the data iteration and calibration parameters:
//
// <srcblock>
// 	IteratingParameters iterpar;
//      iterpar.setChunkInterval(60.0);
//
//      CalibratingParameters calpar(10.0);   // a simple factor of 10.0 to multiply the data
//                                            //   (in leiu of full VisEquation functionality (TBD)
//
//
// </srcblock>
//
// Then these parameter objects, along with a casacore::MeasurementSet pointer, are used to make 
//  a factory suitable for the generic VisibilityIterator2 ctor, which is then invoked
//
// <srcblock>
// 	CalibratingVi2Factory factory(ms,calpar,iterpar);
// 	vi::VisibilityIterator2 *visIter = new vi::VisibilityIterator2 (factory);
// 	vi::VisBuffer2 *visBuffer = visIter->getVisBuffer();
// </srcblock>
//
// Once this is done one can normally iterate and access OTF calibrated data:
//
// <srcblock>
// 	while (visIter->moreChunks())
// 	{
// 		visIter->origin();
//
//		while (visIter->more())
//		{
//
//			casacore::Vector<casacore::Int> ddi = visBuffer->dataDescriptionIds();
//			casacore::Vector<casacore::Int> antenna1 = visBuffer->antenna1();
//			casacore::Vector<casacore::Int> antenna2 = visBuffer->antenna2();
//                      casacore::Cube<casacore::Complex> cvis = visBuffer->visCubeCorrected();
//
//			visIter->next();
//		}
//
//		visIter->nextChunk();
//	}
// </srcblock>
//
//
// casacore::Notice that it is the responsibility of the application layer to delete the VisibilityIterator2
// pointer returned by the factory method. However the life cycle of the VisBuffer2 object is
// responsibility of the VisibilityIterator2 object.
//
// <srcblock>
// 	delete visIter;
// </srcblock>
//
// </example>

class CalibratingVi2Factory : public vi::CalibratingVi2FactoryI
{

public:

  CalibratingVi2Factory(casacore::MeasurementSet* ms,
			const CalibratingParameters& calpar, 
			const IteratingParameters& iterpar=IteratingParameters());

  CalibratingVi2Factory();
			
  CalibratingVi2Factory(casacore::MeasurementSet* ms,
			const casacore::Record& calrec,
			const IteratingParameters& iterpar=IteratingParameters());
			
  CalibratingVi2Factory(casacore::MeasurementSet* ms,
			const casacore::String& callib,
			const IteratingParameters& iterpar=IteratingParameters());
			
  virtual ~CalibratingVi2Factory();

  // Public functions to initialize a generic CVi2F
  virtual void initialize(casacore::MeasurementSet* ms,
			  const casacore::Record& calrec,
			  const IteratingParameters& iterpar);
  virtual void initialize(casacore::MeasurementSet* ms,
			  const casacore::String& callib,
			  const IteratingParameters& iterpar);



protected:

  // Create CalibratingVi2 implementation
  //  If vii=NULL, create underlying vii internally
  virtual vi::ViImplementation2 * createVi () const;
  virtual vi::ViImplementation2 * createVi (vi::ViImplementation2 * vii) const;

private:

  casacore::Bool valid_p;
  casacore::MeasurementSet* ms_p;
  CalibratingParameters calpar_p;
  IteratingParameters iterpar_p;

};


} //# NAMESPACE VI - END
} //# NAMESPACE CASA - END


#endif /* CalibratingVi2Factory_H_ */

