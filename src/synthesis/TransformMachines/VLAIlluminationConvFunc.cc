// -*- C++ -*-
//# VLAIlluminationConvFunc.cc: Implementation for VLAIlluminationConvFunc
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

#define USETABLES 1
#include <synthesis/TransformMachines/VLAIlluminationConvFunc.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/LinearCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <synthesis/TransformMachines/Utils.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/images/Images/ImageRegrid.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/OS/File.h>
#include <fstream>

using namespace casacore;
namespace casa{
  
  //
  //------------------------------------------------------------------------
  //
  void VLAIlluminationConvFunc::getIdealConvFunc(Array<Complex>& buf)
  {
    convFunc_p.get(buf);
  }
  //
  //------------------------------------------------------------------------
  //
  VLAIlluminationConvFunc::VLAIlluminationConvFunc(String fileName):
    IlluminationConvFunc(),convFunc_p(),resolution()
  {
    pbRead_p=false;
    String parts="Re"+fileName;
    PagedImage<Float> reApp(parts);
    parts="Im"+fileName;
    PagedImage<Float> imApp(parts);
  }
  //
  //------------------------------------------------------------------------
  //
  void VLAIlluminationConvFunc::load(String& fileName,
				     Vector<Int>& whichStokes,
				     Float overSampling,
				     Bool putCoords)
  {
    Int Nx,Ny,NPol, NCol;
    Vector<Int> origin(2);
    ifstream inp(fileName.c_str());
    Vector<Float> line;
    Float Freq, Dia;
    
    Int nStokes=whichStokes.nelements();
    Vector<Int> polnMap(nStokes);
    for(Int i=0;i<nStokes;i++)
      {
	switch(whichStokes(i))
	  {
	  case Stokes::RR:
	    {
	      polnMap(i)=3;
	      break;
	    };
	  case Stokes::RL:
	    {
	      polnMap(i)=5;
	      break;
	    }
	  case Stokes::LR:
	    {
	      polnMap(i)=7;
	      break;
	    }
	  case Stokes::LL:
	    {
	      polnMap(i)=9;
	      break;
	    }
	  }
      }
    inp >> Nx >> Ny >> NPol >> NCol >> freq_p >> Dia;
    Freq = freq_p*1E9;
    line.resize(NCol);
    IPosition shape(4);
    shape(0)=(Int)(Nx*overSampling);
    shape(1)=(Int)(Ny*overSampling);
    shape(0)=shape(1)=512;
    shape(2)=nStokes;
    shape(3)=1;
    origin(0) = shape(0)/2+1;
    origin(1) = shape(1)/2+1;
    //     origin(0) = Int((shape(0)+1)/2);
    //     origin(1) = Int((shape(1)+1)/2);
    
    convFunc_p.resize(shape);
    //     reAperture_p.resize(shape);
    //     imAperture_p.resize(shape);
    
    convFunc_p.set(Complex(0,0));
    //     reAperture_p.set(0.0);
    //     imAperture_p.set(0.0);
    
    Double rx,ry,blockage;
    Double re,im;
    Double dx0,dy0,dx1,dy1,dx,dy;
    IPosition ndx(4);
    dx0 = dy0 = dx1 = dy1 = dx = dy = 0;
    ndx(3)=0;
    
    for (ndx(0)=0;ndx(0)<Nx;ndx(0)++)
      for(ndx(1)=0;ndx(1)<Ny;ndx(1)++)
	{
	  IPosition ndx1(ndx);
	  Int i;
	  for(i=0;i<NCol;i++) inp >> line(i);
	  rx = line(0);ry=line(1); blockage=line(2);
	  
	  if (ndx(0)==0) dx0=rx; else {dx0=dx1;dx1=rx;dx += (dx1-dx0);}
	  if (ndx(1)==0) dy0=ry; else {dy0=dy1;dy1=ry;dy += (dy1-dy0);}
	  
	  i=3;
	  
	  if ((ndx(0) < Nx-1) && (ndx(1) < Ny-1))
	    for(ndx(2)=0;ndx(2)<shape(2);ndx(2)++)
	      {
		Float reVal, imVal;
		i = polnMap(ndx(2));
		re = line(i);
		im = line(i+1);
		
		ndx1=ndx;
		ndx1(0)=origin(0)-(Nx/2)+ndx1(0);
		ndx1(1)=origin(1)-(Ny/2)+ndx1(1);
		//		  convFunc_p.putAt(Complex(re,im)*blockage,ndx1);
		//  		  Float amp=sqrt(re*re+im*im);
		//  		  Float phs=-atan2(im,re);
		if (blockage != 0.0)
		  {
		    // 		      reVal = amp*cos(phs);
		    // 		      imVal = amp*sin(phs);
		    reVal = re; imVal = -im;
		    convFunc_p.putAt(Complex(reVal,imVal),ndx1);
		    // 		      reAperture_p.putAt(reVal,ndx1);
		    // 		      imAperture_p.putAt(imVal,ndx1);
		  }
		else
		  {
		    convFunc_p.putAt(Complex(0,0),ndx1);
		  }
		// 		  if (amp > 1e-4) amp=1.0; else amp=0.0;
		// 		  reVal = amp*cos(phs);
		// 		  imVal = amp*sin(phs);
		// 		  convFunc_p.putAt(Complex(reVal,imVal),ndx1);
		// 		  reAperture_p.putAt(reVal,ndx1);
		// 		  imAperture_p.putAt(imVal,ndx1);
	      }
	}
    
    dx /= Nx/2; dy /= Ny/2;
    dy = dx;
    //
    // Make coordinate systems: 2 Linear, 1 Full Stokes, and 1 spectral 
    // axis.
    //
    Int nAxis = 2;
    Vector<String> axisNames(nAxis), axisUnits(nAxis);
    Vector<Double> refPixel(nAxis), increment(nAxis);
    Vector<Double> refValue(nAxis);
    axisNames(0)="UU"; axisNames(1)="VV";
    axisUnits(0)=axisUnits(1)="lambda";
    Double Lambda = C::c/(Freq), R=Dia/2;
    increment(0)=dx*R;
    increment(1)=dy*R;
    resolution.resize(2);
    resolution(0)=-increment(0)/Lambda;resolution(1)=increment(1)/Lambda;
    
    refPixel(0)=origin(0);refPixel(1)=origin(1);
    refValue(0)=refValue(1)=0.0;
    
    
    //
    // Probably a silly way of making a co-ordinate system for UV-plane.
    //
    Matrix<Double> xform(2,2);                   
    xform = 0.0; xform.diagonal() = 1.0;     
    DirectionCoordinate dirCoords(MDirection::J2000,
				  Projection(Projection::SIN),  
				  135*C::pi/180.0, 60*C::pi/180.0,
				  -1*C::pi/180.0, 1*C::pi/180,    
				  xform,                          
				  128, 128); 
    Vector<Bool> diraxes(2); diraxes=true;
    Vector<Int> dirShape(2); 
    dirShape(0)=convFunc_p.shape()(0);
    dirShape(1)=convFunc_p.shape()(1);
    Coordinate* FTdc=dirCoords.makeFourierCoordinate(diraxes,dirShape);
    FTdc->setReferencePixel(refPixel);
    FTdc->setIncrement(resolution);
    /*
      LinearCoordinate linearCoords(nAxis);
      linearCoords.setWorldAxisNames(axisNames);
      linearCoords.setWorldAxisUnits(axisUnits);
      linearCoords.setReferencePixel(refPixel);
      linearCoords.setReferenceValue(refValue);
      linearCoords.setIncrement(resolution);
    */
    
    
    Vector<Int> stokes(4);
    stokes(0)=Stokes::RR;
    stokes(1)=Stokes::RL;
    stokes(2)=Stokes::LR;
    stokes(3)=Stokes::LL;
    StokesCoordinate stokesCoords(whichStokes);
    
    SpectralCoordinate spectralCoords(MFrequency::TOPO,Freq,1.0,0.0);
    //
    // Make the full coordinate system for the image
    //
    CoordinateSystem cs;
    //	cs.addCoordinate(linearCoords);
    cs.addCoordinate(*FTdc);
    cs.addCoordinate(stokesCoords);
    cs.addCoordinate(spectralCoords);
    if (putCoords)
      convFunc_p.setCoordinateInfo(cs);
    delete FTdc;
    // 	String fn="vlaAperture.im";
    // 	storeImg(fn,convFunc_p);
    // 	exit(0);
  };
  
  
  CoordinateSystem VLAIlluminationConvFunc::makeUVCoords(CoordinateSystem& imageCoordSys,
							 IPosition& shape)
  {
    CoordinateSystem FTCoords = imageCoordSys;
    
    Int dirIndex=FTCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=imageCoordSys.directionCoordinate(dirIndex);
    Vector<Bool> axes(2); axes=true;
    Vector<Int> dirShape(2); dirShape(0)=shape(0);dirShape(1)=shape(1);
    Coordinate* FTdc=dc.makeFourierCoordinate(axes,dirShape);
    FTCoords.replaceCoordinate(*FTdc,dirIndex);
    delete FTdc;
    
    return FTCoords;
  }
  
  void VLAIlluminationConvFunc::applyPB(ImageInterface<Float>& pbImage,
					const VisBuffer& vb)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());
    TempImage<Complex> uvGrid(skyShape, skyCS);
    regridApeture(skyCS, skyShape, uvGrid, vb,false);
    fillPB(uvGrid,pbImage);
  }
  
  void VLAIlluminationConvFunc::applyPB(ImageInterface<Complex>& pbImage, 
					const VisBuffer& vb)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());
    TempImage<Complex> uvGrid(skyShape, skyCS);
    regridApeture(skyCS, skyShape, uvGrid, vb);
    fillPB(uvGrid,pbImage);
  }
  
  void VLAIlluminationConvFunc::regridApeture(CoordinateSystem& skyCS,
					      IPosition& skyShape,
					      TempImage<Complex>& uvGrid,
					      const VisBuffer& vb,
					      Bool doSquint)
  {
    CoordinateSystem skyCoords(skyCS);
    
    Int index;
    Double timeValue = getCurrentTimeStamp(vb);
    Float pa = vb.feed_pa(timeValue)(0);
    
    IPosition imsize(skyShape);
    CoordinateSystem uvCoords = SynthesisUtils::makeUVCoords(skyCoords,imsize);
    CoordinateSystem appCoords(convFunc_p.coordinates());
    index=uvCoords.findCoordinate(Coordinate::LINEAR);
    LinearCoordinate uvLC=uvCoords.linearCoordinate(index);
    index=appCoords.findCoordinate(Coordinate::LINEAR);
    LinearCoordinate appLC=appCoords.linearCoordinate(index);
    
    Vector<Double> incrUVGrid(2), incrApp(2), ratio(2);
    incrApp = appLC.increment();
    incrUVGrid = uvLC.increment();
    ratio = incrUVGrid/incrApp;
    
    IPosition ndx(imsize),uvndx(2,0,0);
    
    IPosition appShape(convFunc_p.shape());
    IPosition uvSize(2,imsize(0),imsize(1));
    IPosition appSize(2,appShape(0),appShape(1));
    //
    // Extract the linear axes from the UV-coordinate system.  Make 
    // co-ordinate system with only two Linear axes with +PA rotation.
    //
    Matrix<Double> paRot(2,2);                   
    paRot(0,0) = cos(pa);  paRot(1,0) = +sin(pa);
    paRot(0,1) = -sin(pa); paRot(1,1) = cos(pa);
    
    Vector<Double> refVal(2);refVal = 0.0;
    
    uvLC.setReferenceValue(refVal);
    CoordinateSystem onlyUVLinCoords;
    onlyUVLinCoords.addCoordinate(uvLC);
    
    //
    // Make a co-ordinate system with only 2 Linear axes with the
    // resolution of the finer sampled aperture function.  
    index=appCoords.findCoordinate(Coordinate::LINEAR);
    LinearCoordinate dc=appCoords.linearCoordinate(index);
    dc.setLinearTransform(paRot);
    CoordinateSystem onlyAppLinCoords;
    onlyAppLinCoords.addCoordinate(dc);
    //
    // Make images with PA rotated co-ordinate system.  This is the
    // UV-grid consistent with the SkyImage, but rotated by PA and has
    // only 2 Linear axes (holds only one poln.).
    //
    // Put this in a scope so that when the code gets to the FFT, the
    // big temp. mem. (regriddedUVGrid) is released.
    //
    {
      TempImage<Float> regriddedUVGrid(uvSize, onlyUVLinCoords);
      //
      // Make a TempImage to hold the real or imag parts of the
      // aperture function for this polarization product.
      //
      TempImage<Float> theApp(appSize, onlyAppLinCoords);
      //
      // Re-grid convFunc_p on uvGrid one polarization axis at a time.
      //
      regriddedUVGrid.set(0.0);
      uvGrid.set(Complex(0,0));
      ndx = convFunc_p.shape();
      ndx(3)=0;
      
      index = uvCoords.findCoordinate(Coordinate::STOKES);
      StokesCoordinate skyStokesCo=uvCoords.stokesCoordinate(index);
      Vector<Int> skyStokes = skyStokesCo.stokes();
      //      cout << "Sky stokes = " << skyStokes << endl;
      
      index = appCoords.findCoordinate(Coordinate::STOKES);
      StokesCoordinate appStokesCo=appCoords.stokesCoordinate(index);
      Vector<Int> appStokes = appStokesCo.stokes();
      //      cout << "Aperture stokes = " << appStokes << endl;
      
      
      std::complex<double> aperture;
      ImageRegrid<Float> ir;
      IPosition ndx2d(2,0,0);
      //char Roter[6] = {'-','|','/','-','\\','|'};
      //int RotNdx=0;
      for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++)    // The Poln. axes
	{
	  //
	  // Extract the real and imag parts of the aperture function
	  // for this Poln.
	  //
	  for(Int F=0;F<2;F++) 
	    {
	      //	      cerr << Roter[RotNdx%6] << "\b"; RotNdx++;
	      theApp.set(0);
	      for(ndx2d(0)=0,ndx(0)=0;ndx(0)<appSize(0);ndx(0)++,ndx2d(0)++) // The spatial axes
		if(F==0) {
		  for(ndx2d(1)=0,ndx(1)=0;ndx(1)<appSize(1);ndx(1)++,ndx2d(1)++)
		    {
		      aperture = convFunc_p.getAt(ndx);
		      if (!doSquint) aperture=Complex(abs(aperture),0.0);
		      theApp.putAt((std::real)(aperture),ndx2d);
		    }
		}
		else {
		  for(ndx2d(1)=0,ndx(1)=0;ndx(1)<appSize(1);ndx(1)++,ndx2d(1)++)
		    {
		      aperture = convFunc_p.getAt(ndx);
		      if (!doSquint) aperture=Complex(abs(aperture),0.0);
		      theApp.putAt((std::imag)(aperture),ndx2d);
		    }
		}
	      //
	      // Re-grid the real and imag parts of the aperture function
	      // onto the real imag parts of the uvGrid.
	      //
	      //	      cerr << Roter[RotNdx%6] << "\b"; RotNdx++;
	      IPosition whichAxes(2, 0, 1);
	      ir.regrid(regriddedUVGrid, Interpolate2D::LINEAR, whichAxes, theApp);
	      //	    ir.regrid(imUVGrid, Interpolate2D::LINEAR, whichAxes, imApp);
	      
	      //
	      // Copy the re-gridded real and imag parts to a complex uvGrid.
	      //
	      //	      cerr << Roter[RotNdx%6] << "\b"; RotNdx++;
	      for(uvndx(0)=0,ndx(0)=0;ndx(0)<imsize(0);ndx(0)++,uvndx(0)++)  // The spatial axes
		for(uvndx(1)=0,ndx(1)=0;ndx(1)<imsize(1);ndx(1)++,uvndx(1)++)
		  {
		    // 		  Float re,im;
		    // 		  re = reUVGrid.getAt(uvndx);
		    // 		  im = imUVGrid.getAt(uvndx);
		    Complex tmp;
		    tmp = uvGrid.getAt(ndx);
		    if (F==0) tmp = Complex(regriddedUVGrid.getAt(uvndx),imag(tmp));
		    else      tmp = Complex(real(tmp),regriddedUVGrid.getAt(uvndx));
		    
		    uvGrid.putAt(tmp,ndx);
		  }
	      //	      cerr << Roter[RotNdx%6] << "\b"; RotNdx++;
	    }
	}
    }
    //
    // Now FT the re-gridded Fourier plane to get the primary beam.
    //
    ftAperture(uvGrid);
  }
  
  
  void VLAIlluminationConvFunc::fillPB(ImageInterface<Complex>& inImg,
				       ImageInterface<Complex>& outImg)
  {
    IPosition imsize(outImg.shape());
    IPosition ndx(outImg.shape());
    ndx(3)=0;
    for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++) // The poln axes
      for(ndx(0)=0;ndx(0)<imsize(0);ndx(0)++)
	for(ndx(1)=0;ndx(1)<imsize(1);ndx(1)++)
	  {
	    Complex cval;
	    cval = inImg.getAt(ndx);
	    outImg.putAt(cval*outImg.getAt(ndx),ndx);
	  }
  }
  
  void VLAIlluminationConvFunc::fillPB(ImageInterface<Complex>& inImg,
				       ImageInterface<Float>& outImg)
  {
    IPosition imsize(outImg.shape());
    IPosition ndx(outImg.shape());
    ndx(3)=0;
    for(ndx(0)=0;ndx(0)<imsize(0);ndx(0)++)
      for(ndx(1)=0;ndx(1)<imsize(1);ndx(1)++)
	{
	  //
	  // Average along the polarization axes and fillin the
	  // amp. of the average in the output image.
	  // 
	  Complex cval=0.0;
	  // 	  for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++)
	  // 	    cval += inImg.getAt(ndx);
	  // 	  cval /= imsize(2);
	  ndx(2)=0;cval = inImg.getAt(ndx);
	  for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++) // The poln axes
	    outImg.putAt(abs(cval*outImg.getAt(ndx)),ndx);
	}
  }
  
  void VLAIlluminationConvFunc::ftAperture(String& fileName, 
					   Vector<Int>& whichStokes,
					   Float& overSampling,
					   const CoordinateSystem& coordSys)
  {
    load(fileName,whichStokes,overSampling,false);
    CoordinateSystem pbCoords(coordSys);
    Int dirIndex=pbCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=coordSys.directionCoordinate(dirIndex);
    Double Lambda=C::c/(freq_p*1E9);
    IPosition shape(convFunc_p.shape());
    
    resolution(0) = (Lambda/(resolution(0)*shape(0)));
    resolution(1) = (Lambda/(resolution(1)*shape(1)));
    
    dc.setIncrement(resolution);
    
    Vector<Double> refPix(2),refValue(2);
    refPix(0)=shape(0)/2+1;
    refPix(1)=shape(1)/2+1;
    refValue(0)=refValue(1)=0;
    dc.setReferencePixel(refPix);
    
    pbCoords.replaceCoordinate(dc,dirIndex);
    
    convFunc_p.setCoordinateInfo(pbCoords);
    ftAperture();
  }
  
  void VLAIlluminationConvFunc::ftAperture(TempImage<Complex>& uvgrid)
  {
    //     String fn("reUVGrid.im");
    //     storeImg(fn,uvgrid);
    
    LatticeFFT::cfft2d(uvgrid);
    
    Array<Complex> buf=uvgrid.get();
    buf *= conj(buf);
    
    //     Float peak = abs(max(buf));
    //     buf /= Complex(peak,0.0);
    //     cout << "Peak = " << peak << endl;
    
    uvgrid.put(buf);
    
    //      String fName = "vlapb.im";
    //      storeImg(fName,uvgrid);
    
  }
  
  void VLAIlluminationConvFunc::store(String& fileName){storeImg(fileName,convFunc_p);}
  
  void VLAIlluminationConvFunc::storeImg(String& fileName,ImageInterface<Complex>& theImg)
  {
    ostringstream reName,imName;
    reName << "re" << fileName;
    imName << "im" << fileName;
    {
      PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), reName);
      LatticeExpr<Float> le(abs(theImg));
      tmp.copyData(le);
    }
    {
      PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), imName);
      LatticeExpr<Float> le(arg(theImg));
      tmp.copyData(le);
    }
  }
  
  void VLAIlluminationConvFunc::storeImg(String& fileName,ImageInterface<Float>& theImg)
  {
    PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), fileName);
    LatticeExpr<Float> le(theImg);
    tmp.copyData(le);
  }
  
  void VLAIlluminationConvFunc::storePB(String& fileName)
  {
    {
      ostringstream Name;
      Name << "re" << fileName;
      IPosition newShape(convFunc_p.shape());
      newShape(0)=newShape(1)=200;
      PagedImage<Float> tmp(newShape, convFunc_p.coordinates(), Name);
      //    PagedImage<Float> tmp(convFunc_p.shape(), FTCoords, Name);
      LatticeExpr<Float> le(real(convFunc_p));
      tmp.copyData(le);
    }
    {
      ostringstream Name;
      Name << "im" << fileName;
      
      IPosition newShape(convFunc_p.shape());
      newShape(0)=newShape(1)=200;
      PagedImage<Float> tmp(newShape, convFunc_p.coordinates(), Name);
      //    PagedImage<Float> tmp(convFunc_p.shape(), FTCoords, Name);
      LatticeExpr<Float> le(imag(convFunc_p));
      tmp.copyData(le);
    }
  }
  void VLAIlluminationConvFunc::loadFromImage(String& /*fileName*/) {};
};
