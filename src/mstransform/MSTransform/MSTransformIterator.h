//# MSTransformIterator.h: This file contains the interface definition of the MSTransformManager class.
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

#ifndef MSTransformIterator_H_
#define MSTransformIterator_H_

// Where TransformingVi2 interface is defined
#include <msvis/MSVis/TransformingVi2.h>

// Class containing the actual transformation logic
#include <mstransform/MSTransform/MSTransformManager.h>

// VisBuffer class only accessible from this class
#include <mstransform/MSTransform/MSTransformBufferImpl.h>
#include <casacore/casa/Utilities/CountedPtr.h>

namespace casa {

class MSTransformIterator : public vi::TransformingVi2
{

public:

	MSTransformIterator(	vi::ViImplementation2 * inputVii,
							std::shared_ptr<MSTransformManager>);
	~MSTransformIterator();

    const casacore::MeasurementSet & ms () const {return *transformedMS_p;};

	virtual vi::VisBuffer2 * getVisBuffer () const {return buffer_p;}

	virtual casacore::String ViiType() const ;

	ViImplementation2 * getInputViIterator () const { return inputVii_p; }

    void originChunks ();
    casacore::Bool moreChunks () const;
    void nextChunk ();

    void origin ();
    casacore::Bool more () const;
    void next ();

    void writeFlag (const casacore::Cube<casacore::Bool> & flag);
    void writeFlagRow (const casacore::Vector<casacore::Bool> & rowflags);

protected:

    void propagateChanAvgFlags (const casacore::Cube<casacore::Bool> &avgFlagCube, casacore::Cube<casacore::Bool> &expandedFlagCube);

private:
    casacore::String tmpMSFileName_p;
    std::shared_ptr<MSTransformManager> manager_p;
	MSTransformBufferImpl *buffer_p;
	casacore::MeasurementSet *transformedMS_p;
};

} //# NAMESPACE CASA - END


#endif /* MSTransformIterator_H_ */

