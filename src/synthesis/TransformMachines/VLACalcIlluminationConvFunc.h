//# VLAIlluminationConvFunc.h: Definition for VLAIlluminationConvFunc
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

#ifndef SYNTHESIS_VLACALCILLUMINATIONCONVFUNC_H
#define SYNTHESIS_VLACALCILLUMINATIONCONVFUNC_H

#include <synthesis/TransformMachines/IlluminationConvFunc.h>
//#include <synthesis/MeasurementComponents/BeamCalcConstants.h>
#include <synthesis/TransformMachines/BeamCalc.h>
//#include <synthesis/MeasurementComponents/BeamCalcAntenna.h>
#include <synthesis/MeasurementComponents/CExp.new3.h>
#include <synthesis/MeasurementComponents/ExpCache.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Exceptions.h>
#include <msvis/MSVis/VisBuffer.h>
#include <sstream>
#include <synthesis/TransformMachines/Utils.h>
namespace casa{

  //  casacore::Int getVLABandID(casacore::Double& freq,casacore::String&telescopeName);
  class VLACalcIlluminationConvFunc: public IlluminationConvFunc
  {
  public:

    VLACalcIlluminationConvFunc();
    VLACalcIlluminationConvFunc(casacore::Int n):
      IlluminationConvFunc(n),
      convFunc_p(),resolution(),pbRead_p(false),freq_p(0),lastPA(0),ap()
    {pbRead_p=false;};
    ~VLACalcIlluminationConvFunc() {delete ap.aperture;};

    void setBandID(casacore::Int bandID) {ap.band=bandID;}
    // void storeImg(casacore::String &fileName,casacore::ImageInterface<casacore::Complex>& img);
    // void storeImg(casacore::String &fileName,casacore::ImageInterface<casacore::Float>& img);
    // void store(casacore::String &fileName);
    void loadFromImage(casacore::String &fileName);
    void getIdealConvFunc(casacore::Array<casacore::Complex>& buf);
    //void ftAperture(casacore::TempImage<casacore::Complex>& uvgrid);
    void ftAperture(casacore::TempImage<casacore::Complex>& uvgrid, casacore::Bool makeMueller);
    void ftAperture(casacore::TempImage<casacore::Complex>& uvgrid, casacore::Int makeMuerller=0);
    void ftAperture() {ftAperture(convFunc_p); pbRead_p=true;};
    void storePB(casacore::String& fileName);

    casacore::Bool pbReady() {return pbRead_p;}

    casacore::CoordinateSystem makeUVCoords(casacore::CoordinateSystem& imageCoordSys,
				  casacore::IPosition& shape, casacore::Double refFreq=-1.0);
    void regridAperture(casacore::CoordinateSystem& skyCS, 
			casacore::IPosition& skyShape, 
			casacore::TempImage<casacore::Complex>& uvGrid, 
			//const VisBuffer& vb,
			casacore::Double& pa,
			casacore::Bool doSquint=true,casacore::Int bandID=-1,casacore::Int muellerTerm=0 ,casacore::Double freqVal=-1.0);
    void regridAperture(casacore::CoordinateSystem& skyCS,
			casacore::IPosition& skyShape,
			casacore::TempImage<casacore::Complex>& uvGrid,
			const VisBuffer &vb,
			const casacore::Vector<casacore::Float>& paList,
			casacore::Bool doSquint, casacore::Int bandID);
    void regridApertureEngine(ApertureCalcParams& ap,
			      const casacore::Int& inStokes);
    void setApertureParams(ApertureCalcParams& ap,
			   const casacore::Float& Freq, const casacore::Float& pa, 
			   const casacore::Int& bandID,
			   const casacore::Int& inStokes,
			   const casacore::IPosition& skyShape,
			   const casacore::Vector<casacore::Double>& uvIncr);


    void applyPB(casacore::ImageInterface<casacore::Float>& pbImage, casacore::Double& pa,//const VisBuffer& vb, 
		 const casacore::Vector<casacore::Float>& paList, casacore::Int bandID, casacore::Bool doSquint=false);
    void applyPB(casacore::ImageInterface<casacore::Float>& pbImage, casacore::Double& pa,//const VisBuffer& vb, 
		 casacore::Int bandID=-1, casacore::Bool doSquint=false, casacore::Double freqVal=-1.0);
    void applyPB(casacore::ImageInterface<casacore::Complex>& pbImage, casacore::Double& pa,//const VisBuffer& vb, 
		 casacore::Bool doSquint=true,casacore::Int bandID=-1, casacore::Int muellerTerm=0, casacore::Double freqVal=-1.0);
    void applyPBSq(casacore::ImageInterface<casacore::Float>& pbImage, casacore::Double& pa,//const VisBuffer& vb, 
		   const casacore::Vector<casacore::Float>& paList, casacore::Int bandID, casacore::Bool doSquint=false);
    void applyPBSq(casacore::ImageInterface<casacore::Float>& pbImage, casacore::Double& pa,//const VisBuffer& vb, 
		   casacore::Int bandID=-1, casacore::Bool doSquint=false);
    void applyPBSq(casacore::ImageInterface<casacore::Complex>& pbImage, casacore::Double& pa,//const VisBuffer& vb, 
		   casacore::Int bandID=-1,  casacore::Bool doSquint=true);
    void makeFullJones(casacore::ImageInterface<casacore::Complex>& pbImage, const VisBuffer& vb, casacore::Bool doSquint, 
	       casacore::Int bandID, casacore::Double freqVal=-1.0);
    void skyMuller(casacore::ImageInterface<casacore::Complex>& skyJones);
    void skyMuller(casacore::ImageInterface<casacore::Complex>& skyJones, casacore::Int muellerTerm=0);
    void skyMuller(casacore::Array<casacore::Complex>& skyJones, const casacore::IPosition& shape, const casacore::Int& inStokes);

    casacore::Bool findSupport(casacore::Array<casacore::Complex>& /*func*/, casacore::Float& /*threshold*/,casacore::Int& /*origin*/, casacore::Int& /*R*/)
    {throw(casacore::AipsError("VLACalcIlluminationConvFunc::findSupport() not implemented"));};
    virtual casacore::Bool makeAverageResponse(const VisBuffer& /*vb*/,
				     const casacore::ImageInterface<casacore::Complex>& /*image*/,
				     //				     casacore::TempImage<casacore::Float>& theavgPB,
				     casacore::ImageInterface<casacore::Float>& /*theavgPB*/,
				     casacore::Bool /*reset*/=true)
    {throw(casacore::AipsError("VLACalc::makeAverageRes() called"));};


  private:
    
    void fillPB(casacore::ImageInterface<casacore::Complex>& inImg, casacore::ImageInterface<casacore::Float>& outImg, casacore::Bool Square=false);
    void fillPB(casacore::ImageInterface<casacore::Complex>& inImg, casacore::ImageInterface<casacore::Complex>& outImg, casacore::Bool Square=false);

    casacore::TempImage<casacore::Complex> convFunc_p;
    //    casacore::TempImage<casacore::Float> reAperture_p, imAperture_p;
    casacore::Vector<casacore::Double> resolution;
    casacore::Bool pbRead_p;
    casacore::Float freq_p,lastPA;
    ApertureCalcParams ap;
  };

};
#endif
