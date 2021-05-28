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

#ifndef SYNTHESIS_VLAILLUMINATIONCONVFUNC_H
#define SYNTHESIS_VLAILLUMINATIONCONVFUNC_H

#include <synthesis/TransformMachines/IlluminationConvFunc.h>
#include <synthesis/MeasurementComponents/CExp.new3.h>
#include <synthesis/MeasurementComponents/ExpCache.h>
#include <images/Images/TempImage.h>
#include <casa/Exceptions.h>
#include <msvis/MSVis/VisBuffer.h>
namespace casa
{
  class VLAIlluminationConvFunc: public IlluminationConvFunc
  {
  public:
    VLAIlluminationConvFunc(casacore::String fileName);
    VLAIlluminationConvFunc()
      :IlluminationConvFunc(),convFunc_p(),resolution()
    {pbRead_p=false;};
    VLAIlluminationConvFunc(casacore::Int n):IlluminationConvFunc(n)    {pbRead_p=false;};
    ~VLAIlluminationConvFunc() {};

    void load(casacore::String &fileName,casacore::Vector<casacore::Int>& whichStokes, 
	      casacore::Float overSampling=20,casacore::Bool putCoords=true);
    void storeImg(casacore::String &fileName,casacore::ImageInterface<casacore::Complex>& img);
    void storeImg(casacore::String &fileName,casacore::ImageInterface<casacore::Float>& img);
    void store(casacore::String &fileName);
    void loadFromImage(casacore::String &fileName);
    void getIdealConvFunc(casacore::Array<casacore::Complex>& buf);
    void ftAperture(casacore::TempImage<casacore::Complex>& uvgrid);
    void ftAperture() {ftAperture(convFunc_p); pbRead_p=true;};
    void storePB(casacore::String& fileName);

    casacore::Bool pbReady() {return pbRead_p;}

    casacore::CoordinateSystem makeUVCoords(casacore::CoordinateSystem& imageCoordSys,
				   casacore::IPosition& shape);
    void regridApeture(casacore::CoordinateSystem& skyCS, casacore::IPosition& skyShape, casacore::TempImage<casacore::Complex>& uvGrid, 
		       const VisBuffer& vb,casacore::Bool doSquint=true);
    void applyPB(casacore::ImageInterface<casacore::Float>& pbImage, const VisBuffer& vb);
    void applyPB(casacore::ImageInterface<casacore::Complex>& pbImage, const VisBuffer& vb);
    void ftAperture(casacore::String& fileName, casacore::Vector<casacore::Int>& whichStokes,
		   casacore::Float& overSampling, const casacore::CoordinateSystem& coordSys);

    /*
    virtual CF_TYPE getValue(casacore::Double *coord,
			     casacore::Double *raoff1,  casacore::Double *raoff2, 
			     casacore::Double *decoff1, casacore::Double *decoff2,
			     casacore::Double *area,
			     casacore::Int *doGrad,
			     casacore::Complex& weight,
			     casacore::Complex& dweight1,
			     casacore::Complex& dweight2
			     //		  ,casacore::Double lsigma
			     );
    */	  
  private:
    
    void fillPB(casacore::ImageInterface<casacore::Complex>& inImg, casacore::ImageInterface<casacore::Float>& outImg);
    void fillPB(casacore::ImageInterface<casacore::Complex>& inImg, casacore::ImageInterface<casacore::Complex>& outImg);

    casacore::TempImage<casacore::Complex> convFunc_p;
    //    casacore::TempImage<casacore::Float> reAperture_p, imAperture_p;
    casacore::Vector<casacore::Double> resolution;
    casacore::Bool pbRead_p;
    casacore::Float freq_p;
  };

};
#endif
