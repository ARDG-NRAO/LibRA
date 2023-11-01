//# nPBWProjectFT.h: Definition for nPBWProjectFT
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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

#ifndef SYNTHESIS_PBMOSAICFT_H
#define SYNTHESIS_PBMOSAICFT_H

// #include <synthesis/TransformMachines/FTMachine.h>
// #include <casa/Arrays/Matrix.h>
// #include <scimath/Mathematics/FFTServer.h>
// #include <msvis/MSVis/VisBuffer.h>
// #include <images/Images/ImageInterface.h>
// #include <images/Images/ImageInterface.h>
// #include <casa/Containers/Block.h>
// #include <casa/Arrays/Array.h>
// #include <casa/Arrays/Vector.h>
// #include <casa/Arrays/Matrix.h>
// #include <scimath/Mathematics/ConvolveGridder.h>
#include <synthesis/MeasurementComponents/nPBWProjectFT.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  
  class EPJones;
  //  class nPBWProjectFT;
  class PBMosaicFT : public nPBWProjectFT {

  public:
    // Constructor: cachesize is the size of the cache in words
    // (e.g. a few million is a good number), tilesize is the
    // size of the tile used in gridding (cannot be less than
    // 12, 16 works in most cases). 
    // <group>
    PBMosaicFT(casacore::MeasurementSet& ms, 
	       casacore::Int nFacets, casacore::Long cachesize, casacore::String& cfCacheDirName,
	       casacore::Bool applyPointingOffset=true,
	       casacore::Bool doPBCorr=true,
	       casacore::Int tilesize=16, 
	       casacore::Float paSteps=5.0, casacore::Float pbLimit=5e-2,
	       casacore::Bool usezero=false);
    // </group>
    
    // Construct from a casacore::Record containing the PBMosaicFT state
    PBMosaicFT(const casacore::RecordInterface& stateRec);
    
    // Copy constructor
    //    PBMosaicFT(const PBMosaicFT &other);
    
    // Assignment operator
    PBMosaicFT &operator=(const PBMosaicFT &othher);
    
    ~PBMosaicFT() {};

    casacore::Bool makeAveragePB0(const VisBuffer& vb, 
			const casacore::ImageInterface<casacore::Complex>& image,
			casacore::Int& polInUse,
			casacore::TempImage<casacore::Float>& avgPB);
    casacore::Int findPointingOffsets(const VisBuffer& vb, 
			    casacore::Array<casacore::Float> &l_off,
			    casacore::Array<casacore::Float> &m_off,
			    casacore::Bool Evaluate);
    void normalizeAvgPB();
    void normalizePB(casacore::ImageInterface<casacore::Float> &pb,const casacore::Float& nSamp=1);
    virtual casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true);
    inline virtual casacore::Float pbFunc(casacore::Float& a) {return (a);};
    void setObservatoryLocation(const casacore::MPosition& mLocation) {mLocation_p=mLocation;};
    virtual void finalizeToSky();
    virtual void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,  casacore::Matrix<casacore::Float>& weight,
				 const VisBuffer& vb);
    virtual casacore::String name() {return "PBMosaicFT";};
    virtual casacore::Bool verifyShapes(casacore::IPosition , casacore::IPosition )
    {return true;};

  private:
    casacore::Complex nApertures;
    casacore::Vector<casacore::Int> fieldIds_p;
    casacore::TempImage<casacore::Complex> griddedWeights;
    //casacore::Float pbNorm;
    virtual void runFortranGet(casacore::Matrix<casacore::Double>& uvw,casacore::Vector<casacore::Double>& dphase,
		       casacore::Cube<casacore::Complex>& visdata,
		       casacore::IPosition& s,
		       //casacore::Cube<casacore::Complex>& gradVisAzData,
		       //casacore::Cube<casacore::Complex>& gradVisElData,
		       //casacore::IPosition& gradS,
		       casacore::Int& Conj,
		       casacore::Cube<casacore::Int>& flags,casacore::Vector<casacore::Int>& rowFlags,
		       casacore::Int& rownr,casacore::Vector<casacore::Double>& actualOffset,
		       casacore::Array<casacore::Complex>* dataPtr,
		       casacore::Int& aNx, casacore::Int& aNy, casacore::Int& npol, casacore::Int& nchan,
		       VisBuffer& vb,casacore::Int& Nant_p, casacore::Int& scanNo,
		       casacore::Double& sigma,
		       casacore::Array<casacore::Float>& raoffsets,
		       casacore::Array<casacore::Float>& decoffsets,
		       casacore::Double area,
		       casacore::Int& doGrad,casacore::Int paIndex);
    virtual void runFortranPut(casacore::Matrix<casacore::Double>& uvw,casacore::Vector<casacore::Double>& dphase,
		       const casacore::Complex& visdata_p,
		       casacore::IPosition& s,
		       //casacore::Cube<casacore::Complex>& gradVisAzData,
		       //casacore::Cube<casacore::Complex>& gradVisElData,
		       //casacore::IPosition& gradS,
		       casacore::Int& Conj,
		       casacore::Cube<casacore::Int>& flags,casacore::Vector<casacore::Int>& rowFlags,
		       const casacore::Matrix<casacore::Float>& weight,
		       casacore::Int& rownr,casacore::Vector<casacore::Double>& actualOffset,
		       casacore::Array<casacore::Complex>& dataPtr,
		       casacore::Int& aNx, casacore::Int& aNy, casacore::Int& npol, casacore::Int& nchan,
		       const VisBuffer& vb,casacore::Int& Nant_p, casacore::Int& scanNo,
		       casacore::Double& sigma,
		       casacore::Array<casacore::Float>& raoffsets,
		       casacore::Array<casacore::Float>& decoffsets,
		       casacore::Matrix<casacore::Double>& sumWeight,
		       casacore::Double& area,
		       casacore::Int& doGrad,
		       casacore::Int& doPSF,casacore::Int paIndex);
    virtual void runFortranGetGrad(casacore::Matrix<casacore::Double>& uvw,casacore::Vector<casacore::Double>& dphase,
			 casacore::Cube<casacore::Complex>& visdata,
			 casacore::IPosition& s,
			 casacore::Cube<casacore::Complex>& gradVisAzData,
			 casacore::Cube<casacore::Complex>& gradVisElData,
			 //			 casacore::IPosition& gradS,
			 casacore::Int& Conj,
			 casacore::Cube<casacore::Int>& flags,casacore::Vector<casacore::Int>& rowFlags,
			 casacore::Int& rownr,casacore::Vector<casacore::Double>& actualOffset,
			 casacore::Array<casacore::Complex>* dataPtr,
			 casacore::Int& aNx, casacore::Int& aNy, casacore::Int& npol, casacore::Int& nchan,
			 VisBuffer& vb,casacore::Int& Nant_p, casacore::Int& scanNo,
			 casacore::Double& sigma,
			 casacore::Array<casacore::Float>& l_off,
			 casacore::Array<casacore::Float>& m_off,
			 casacore::Double area,
			 casacore::Int& doGrad,
			 casacore::Int paIndex);

  };

} //# NAMESPACE CASA - END

#endif
