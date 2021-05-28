//# ALMAIlluminationConvFunc.h: Definition for ALMAIlluminationConvFunc
//# Copyright (C) 1996,1997,1998,1999,2000,2002
//# Associated Universities, Inc. Washington DC, USA.
//# Copyright by ESO (in the framework of the ALMA collaboration)
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

#ifndef SYNTHESIS_ALMACALCILLUMINATIONCONVFUNC_H
#define SYNTHESIS_ALMACALCILLUMINATIONCONVFUNC_H

#include <synthesis/TransformMachines/IlluminationConvFunc.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <synthesis/TransformMachines/ALMAAperture.h>
#include <synthesis/MeasurementComponents/CExp.new3.h>
#include <synthesis/MeasurementComponents/ExpCache.h>
#include <images/Images/TempImage.h>
#include <casa/Exceptions.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/sstream.h>
#include <synthesis/TransformMachines/Utils.h>
namespace casa{

  class ALMACalcIlluminationConvFunc: public IlluminationConvFunc
  {
  public:

    ALMACalcIlluminationConvFunc();
    ALMACalcIlluminationConvFunc(casacore::Int n):IlluminationConvFunc(n)
      {pbRead_p=false;};
    ~ALMACalcIlluminationConvFunc() 
      {delete ap.aperture;};

    void setBandID(casacore::Int /*bandID*/) {/*ap.band=(ALMABeamCalcBandCode)bandID;*/}

    void setAntRayPath(const casacore::String& antRayPath) {otherAntRayPath_p = antRayPath;}

    void loadFromImage(casacore::String &fileName);
    void getIdealConvFunc(casacore::Array<casacore::Complex>& buf);
    void ftAperture(casacore::TempImage<casacore::Complex>& uvgrid);
    void ftAperture() {ftAperture(convFunc_p); pbRead_p=true;};
    void storePB(casacore::String& fileName);

    casacore::Bool pbReady() {return pbRead_p;}

    casacore::CoordinateSystem makeUVCoords(casacore::CoordinateSystem& imageCoordSys,
				  casacore::IPosition& shape, casacore::Double refFreq=-1.0);
    void regridAperture(casacore::CoordinateSystem& skyCS, 
			casacore::IPosition& skyShape, 
			casacore::TempImage<casacore::Complex>& uvGrid, 
			const VisBuffer& vb,
			casacore::Bool doSquint=true,casacore::Int bandID=-1);
    void regridAperture(casacore::CoordinateSystem& skyCS,
			casacore::IPosition& skyShape,
			casacore::TempImage<casacore::Complex>& uvGrid,
			const VisBuffer &vb,
			const casacore::Vector<casacore::Float>& paList,
			casacore::Bool doSquint, casacore::Int bandID);

    void regridAperture(casacore::CoordinateSystem& skyCS, 
			casacore::IPosition& skyShape, 
			casacore::TempImage<casacore::Complex>& uvGrid, 
			const casacore::String& telescope,
			const casacore::MVFrequency& freqQ,
			casacore::Float pa = 0.,
			casacore::Bool doSquint=true,
			casacore::Int bandID=-1);

    void applyPB(casacore::ImageInterface<casacore::Float>& pbImage, 
		 const VisBuffer& vb, casacore::Bool doSquint=false, casacore::Int cfKey=0);

    void applyPB(casacore::ImageInterface<casacore::Complex>& pbImage, 
		 const VisBuffer& vb, casacore::Bool doSquint=true, casacore::Int cfKey=0);

    void applyPB(casacore::ImageInterface<casacore::Float>& pbImage, 
		 const casacore::String& telescope, const casacore::MEpoch& obsTime, 
		 const casacore::String& antType0, const casacore::String& antType1,
		 const casacore::MVFrequency& freqQ, casacore::Double pa=0.,
		 casacore::Bool doSquint=false);

    void applyPB(casacore::ImageInterface<casacore::Complex>& pbImage, 
		 const casacore::String& telescope, const casacore::MEpoch& obsTime,
		 const casacore::String& antType0, const casacore::String& antType1,
		 const casacore::MVFrequency& freqQ, casacore::Double pa=0.,
		 casacore::Bool doSquint=true);

    void applyVP(casacore::ImageInterface<casacore::Complex>& pbImage, 
		 const casacore::String& telescope, const casacore::MEpoch& obsTime,
		 const casacore::String& antType0, const casacore::String& antType1,
		 const casacore::MVFrequency& freqQ, casacore::Double pa=0.,
		 casacore::Bool doSquint=true);

    void skyMuller(casacore::ImageInterface<casacore::Complex>& skyJones);

    //    casacore::Int getALMABandId(const casacore::Double& freq);
    virtual void prepareConvFunction(const VisBuffer& /*vb*/, CFStore& /*cfs*/){};

    

  private:
    
    void fillPB(casacore::ImageInterface<casacore::Complex>& inImg, casacore::ImageInterface<casacore::Float>& outImg, casacore::Bool Square=false);
    void fillPB(casacore::ImageInterface<casacore::Complex>& inImg, casacore::ImageInterface<casacore::Complex>& outImg, casacore::Bool Square=false);
    void fillVP(casacore::ImageInterface<casacore::Complex>& inImg, casacore::ImageInterface<casacore::Complex>& outImg, casacore::Bool Square=false);

    casacore::TempImage<casacore::Complex> convFunc_p;
    casacore::Vector<casacore::Double> resolution;
    casacore::Bool pbRead_p;
    casacore::Float freq_p,lastPA;
    ApertureCalcParams ap;

    casacore::Bool haveCannedResponses;

    casacore::String otherAntRayPath_p;
  };

};
#endif
