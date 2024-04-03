//# ALMAAperture.h: Definition of the ALMAAperture class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//# Copyright (C) 2011 by ESO (in the framework of the ALMA collaboration)
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
//
#ifndef SYNTHESIS_ALMAAPERTURE_H
#define SYNTHESIS_ALMAAPERTURE_H

#include <casacore/images/Images/PagedImage.h>
#include <synthesis/TransformMachines/AzElAperture.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <synthesis/MeasurementEquations/AntennaResponses.h>
#include <imageanalysis/ImageAnalysis/ImageConvolver.h>
#include <casacore/images/Images/ImageRegrid.h>
#include <casacore/images/Images/ImageRegrid.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

//
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// TEMPS The following #defines should REALLY GO!
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//
// #define CONVSIZE (1024*2)
// #define CONVWTSIZEFACTOR sqrt(2.0)
// #define OVERSAMPLING 10
// #define THRESHOLD 1E-3

namespace casacore{

  template<class T> class ImageInterface;

}

namespace casa { //# NAMESPACE CASA - BEGIN
  class VisBuffer;

  enum ALMAAntennaType {
    ALMA_INVALID = 0,
    ALMA_DA = 1,
    ALMA_DV,
    ALMA_CM,
    ALMA_PM,
    ALMA_numAntTypes
  };

  //  class ALMAAperture : public ATerm
  class ALMAAperture : public AzElAperture
  {
  public:
    ALMAAperture();

    ~ALMAAperture();

    ALMAAperture& operator=(const ALMAAperture& other);
    //
    // Overload these functions.  They are pure virtual in the base class (ATerm).
    //
    virtual void makeFullJones(casacore::ImageInterface<casacore::Complex>& pbImage,
			       const VisBuffer& vb,
			       casacore::Bool doSquint, casacore::Int& bandID, casacore::Double freqVal);

    virtual casacore::String name() {return casacore::String("ALMA Aperture");};

    virtual void applySky(casacore::ImageInterface<casacore::Float>& outputImage,
			  const VisBuffer& vb, 
			  const casacore::Bool doSquint=true,
			  const casacore::Int& cfKey=0,
			  const casacore::Double freqVal=-1.0)
    {
      (void)freqVal;applySky(outputImage, vb, doSquint, cfKey, false);
    };
    virtual void applySky(casacore::ImageInterface<casacore::Complex>& outputImage,
			  const VisBuffer& vb, 
			  const casacore::Bool doSquint=true,
			  const casacore::Int& cfKey=0,
			  const casacore::Double freqVal=-1.0)
    {(void)freqVal;applySky(outputImage, vb, doSquint, cfKey, false);};

    void applySky(casacore::ImageInterface<casacore::Float>& outputImage,
		  const VisBuffer& vb, 
		  const casacore::Bool doSquint,
		  const casacore::Int& cfKey,
		  const casacore::Bool raytrace); // if true, use raytracing even if canned responses exist
    void applySky(casacore::ImageInterface<casacore::Complex>& outputImage,
		  const VisBuffer& vb, 
		  const casacore::Bool doSquint,
		  const casacore::Int& cfKey,
		  const casacore::Bool raytrace); // if true, use raytracing even if canned responses exist

    virtual casacore::Vector<casacore::Int> vbRow2CFKeyMap(const VisBuffer& vb, casacore::Int& nUnique);

    void cacheVBInfo(const casacore::String& telescopeName, const casacore::Float& diameter);
    void cacheVBInfo(const VisBuffer& vb);
    casacore::Int getBandID(const casacore::Double& freq, const casacore::String& telescopeName);

    virtual void setPolMap(const casacore::Vector<casacore::Int>& polMap) {polMap_p.resize(0);polMap_p=polMap;};
    virtual void getPolMap(casacore::Vector<casacore::Int>& polMap) {polMap.resize(0);polMap=polMap_p;};
    // virtual casacore::Int getConvSize() {return CONVSIZE;};
    // virtual casacore::Int getOversampling() {return OVERSAMPLING;}
    // virtual casacore::Float getConvWeightSizeFactor() {return CONVWTSIZEFACTOR;};
    // virtual casacore::Float getSupportThreshold() {return THRESHOLD;};

    // tell the antenna type number for each antenna in the antenna table
    casacore::Vector<ALMAAntennaType> antTypeMap(const VisBuffer& vb);

    // call this before reusing the same ALMAAperture object on a different MS
    void resetAntTypeMap(){antTypeMap_p.resize(0);};

    // derive type number from first two characters in antenna name, 
    // return -1 if not recognised 
    static ALMAAntennaType antTypeFromName(const casacore::String& name);
    static casacore::String antTypeStrFromType(const ALMAAntennaType& aType);

    static casacore::Int cFKeyFromAntennaTypes(const ALMAAntennaType aT1, const ALMAAntennaType aT2);
    static casacore::Vector<ALMAAntennaType> antennaTypesFromCFKey(const casacore::Int& cFKey);

    // generate the lists of antenna types for the whole vb
    casacore::Vector<ALMAAntennaType> antTypeList(const VisBuffer& vb);

    casacore::Int getVisParams(const VisBuffer& vb, const casacore::CoordinateSystem& skyCoord=casacore::CoordinateSystem());
    casacore::Int makePBPolnCoords(const VisBuffer&vb,
			 const casacore::Int& convSize,
			 const casacore::Int& convSampling,
			 const casacore::CoordinateSystem& skyCoord,
			 const casacore::Int& skyNx, const casacore::Int& skyNy,
			 casacore::CoordinateSystem& feedCoord);

    void destroyAntResp(){ delete aR_p; aR_p=0;};


  private:
    static AntennaResponses* aR_p; // shared between all instances of this class
    static casacore::Bool orderMattersInCFKey;
    casacore::Vector<casacore::Int> polMap_p;
    casacore::Bool haveCannedResponses_p; // true if there are precalculated response images available
    casacore::Vector<ALMAAntennaType> antTypeMap_p; // maps antenna id to antenna type
    casacore::Vector<casacore::PagedImage<casacore::Complex>* > respImage_p;
  };
};
#endif
