// -*- C++ -*-
//# AWConvFuncEPJones.cc: Implementation of the AWConvFuncEPJones class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
//
#include <synthesis/TransformMachines/AWConvFuncEPJones.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <casacore/images/Images/ImageInterface.h>
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <synthesis/TransformMachines/CFStore.h>
#include <synthesis/TransformMachines/CFStore2.h>
#include <synthesis/TransformMachines/PSTerm.h>
#include <synthesis/TransformMachines/WTerm.h>
#include <synthesis/TransformMachines/ATerm.h>
#include <synthesis/TransformMachines/VLACalcIlluminationConvFunc.h>
#include <synthesis/TransformMachines/ConvolutionFunction.h>
#include <synthesis/TransformMachines/PolOuterProduct.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/casa/Utilities/CompositeNumber.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <ostream>

#define MAX_FREQ 1e30

using namespace casacore;
namespace casa{
  //
  //----------------------------------------------------------------------
  //
  AWConvFuncEPJones& AWConvFuncEPJones::operator=(const AWConvFuncEPJones& other)
  {
    if(this!=&other) 
      {
	AWConvFunc::operator=(other);
	imageDC_p = other.imageDC_p;
	imageObsInfo_p = other.imageObsInfo_p;
      }
    return *this;
  }
  //
  //----------------------------------------------------------------------
  // Find the offset between the VB and the image phase center
  //
  Vector<Double> AWConvFuncEPJones::findPointingOffset(const ImageInterface<Complex>& image,
						       const VisBuffer& vb)
  {
    storeImageParams(image,vb);
    //where in the image in pixels is this pointing
    pixFieldGrad_p.resize(2);
    toPix(vb);
    pixFieldGrad_p=thePix_p;

    MDirection fieldDir=direction1_p;
    //shift from center
    pixFieldGrad_p(0) = pixFieldGrad_p(0) - Double(nx_p / 2);
    pixFieldGrad_p(1) = pixFieldGrad_p(1) - Double(ny_p / 2);

    //    Int convSampling=aTerm_p->getOversampling();
    Int convSampling=getOversampling(*psTerm_p,*wTerm_p,*aTerm_p);

    //phase gradient per pixel to apply
    pixFieldGrad_p(0) = -pixFieldGrad_p(0)*2.0*C::pi/Double(nx_p)/Double(convSampling);
    pixFieldGrad_p(1) = -pixFieldGrad_p(1)*2.0*C::pi/Double(ny_p)/Double(convSampling);

    return pixFieldGrad_p;
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFuncEPJones::makeConvFunction(const ImageInterface<Complex>& image,
					   const VisBuffer& vb,
					   const Int wConvSize,
					   const CountedPtr<PolOuterProduct>& pop,
					   const Float pa,
					   const Float dpa,
					   const Vector<Double>& uvScale, const Vector<Double>& uvOffset,
					   const Matrix<Double>& spwFreqSel,
					   CFStore2& cfs,
					   CFStore2& cfwts,
					   Bool fillCF)
  {
    findPointingOffset(image,vb);
    AWConvFunc::makeConvFunction(image,vb,wConvSize,pop,pa,dpa,uvScale,uvOffset,spwFreqSel,cfs,cfwts,fillCF);
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFuncEPJones::toPix(const VisBuffer& vb)
  {
    thePix_p.resize(2);

    if(dc_p.directionType() !=  MDirection::castType(vb.direction1()(0).getRef().getType())){
      //pointToPix_p.setModel(theDir);
      
      MEpoch timenow(Quantity(vb.time()(0), timeUnit_p), timeMType_p);
      //cout << "Ref " << vb.direction1()(0).getRefString() << " ep "
      //<< timenow.getRefString() << " time " <<
      //MVTime(timenow.getValue().getTime()).string(MVTime::YMD) <<
      //endl;
      pointFrame_p.resetEpoch(timenow);
      //////////////////////////
      //pointToPix holds pointFrame_p by reference...
      //thus good to go for conversion
      direction1_p=pointToPix_p(vb.direction1()(0));
      direction2_p=pointToPix_p(vb.direction2()(0));
      dc_p.toPixel(thePix_p, direction1_p);

    }
    else{
      direction1_p=vb.direction1()(0);
      direction2_p=vb.direction2()(0);
      dc_p.toPixel(thePix_p, vb.direction1()(0));
    }
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFuncEPJones::storeImageParams(const ImageInterface<Complex>& iimage,
					   const VisBuffer& vb){
    //image signature changed...rather simplistic for now
    if((iimage.shape().product() != nx_p*ny_p*nchan_p*npol_p) || nchan_p < 1){
      csys_p=iimage.coordinates();
      Int coordIndex=csys_p.findCoordinate(Coordinate::DIRECTION);
      AlwaysAssert(coordIndex>=0, AipsError);
      directionIndex_p=coordIndex;
      dc_p=csys_p.directionCoordinate(directionIndex_p);
      ObsInfo imInfo=csys_p.obsInfo();
      String tel= imInfo.telescope();
      MPosition pos;
      if (vb.msColumns().observation().nrow() > 0) {
	tel = vb.msColumns().observation().telescopeName()(vb.msColumns().observationId()(0));
      }
      if (tel.length() == 0 || !tel.contains("VLA") ||
	  !MeasTable::Observatory(pos,tel)) {
	// unknown observatory, use first antenna
   	  Int ant1=vb.antenna1()(0);
	  pos=vb.msColumns().antenna().positionMeas()(ant1);
      }
      //cout << "TELESCOPE " << tel << endl;
      //Store this to build epochs via the time access of visbuffer later
      timeMType_p=MEpoch::castType(vb.msColumns().timeMeas()(0).getRef().getType());
      timeUnit_p=Unit(vb.msColumns().timeMeas().measDesc().getUnits()(0).getName());
      // timeUnit_p=Unit("s");
      //cout << "UNIT " << timeUnit_p.getValue() << " name " << timeUnit_p.getName()  << endl;
      pointFrame_p=MeasFrame(imInfo.obsDate(), pos);
      MDirection::Ref elRef(dc_p.directionType(), pointFrame_p);
      //For now we set the conversion from this direction 
      pointToPix_p=MDirection::Convert( MDirection(), elRef);
      nx_p=iimage.shape()(coordIndex);
      ny_p=iimage.shape()(coordIndex+1);
      // pointingPix_p.resize(nx_p, ny_p);
      // pointingPix_p.set(false);
      coordIndex=csys_p.findCoordinate(Coordinate::SPECTRAL);
      Int pixAxis=csys_p.pixelAxes(coordIndex)[0];
      nchan_p=iimage.shape()(pixAxis);
      coordIndex=csys_p.findCoordinate(Coordinate::STOKES);
      pixAxis=csys_p.pixelAxes(coordIndex)[0];
      npol_p=iimage.shape()(pixAxis);
    }

  }
}
