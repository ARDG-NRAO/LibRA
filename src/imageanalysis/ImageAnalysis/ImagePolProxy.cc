//# ImagePolProxy.cc:  C++ casa namespace proxy for ImagePol tool
//# Copyright (C) 2007
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
//# $Id: ImagePolProxy.cc 20615 2009-06-09 02:16:01Z Malte.Marquarding $
#include <imageanalysis/ImageAnalysis/ImagePolProxy.h>
#include <casacore/casa/aips.h>
#include <iostream>
#include <sstream>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayUtil.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/casa/BasicMath/Random.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogFilter.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/images/Images/ImageExpr.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Regions/RegionHandler.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/lattices/Lattices/LatticeUtilities.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/LogTables/NewFile.h>
#include <casacore/casa/namespace.h>

#include <memory>

using namespace casacore;
namespace casa { //# name space casa begins


  ImagePol::ImagePol() : itsImPol(0)
  {
    itsLog= new LogIO();
    
  }

  ImagePol::ImagePol(ImageInterface<Float>& im) :  itsLog(0),itsImPol(0)
  {
    open(im);
    
  }

  ImagePol::~ImagePol(){
    if(itsLog)
      delete itsLog;
    if(itsImPol)
      delete itsImPol;
  }

  Bool ImagePol::open(ImageInterface<Float>& im){
    if(!itsLog)
      itsLog= new LogIO();
    if(itsImPol)
      delete itsImPol;
    itsImPol= new ImagePolarimetry(im);
    
    return true;

  }

  Bool ImagePol::open(const String& infile) {
    Bool rstat(false);

    if(!itsLog) itsLog = new LogIO();
    *itsLog << LogOrigin("ImagePol", "open(const String& infile)");
    if (infile.size() == 0) {
      *itsLog << LogIO::WARN << "You must give an infile" << LogIO::POST;
      return rstat;
    }

    if(itsImPol) delete itsImPol;
    itsImPol=0;
    //
    ImageInterface<Float>* imagePointer = 0;
    ImageUtilities::openImage (imagePointer, infile);

    //
    try {
      itsImPol = new ImagePolarimetry(*imagePointer);
      rstat = true;
    } catch (AipsError &x) {
      delete imagePointer;
      *itsLog << x.getMesg() << LogIO::EXCEPTION;
    }
    //
    delete imagePointer;
    imagePointer = 0;
    return rstat;
  }

  Bool
  ImagePol::imagepoltestimage(const String& outFile, const Vector<Double>& rm,
			      Bool rmDefault, Double pa0, Double sigma,
			      Int nx, Int ny, Int nf, Double f0, Double df)
  {

    Bool rstat(false);

    if (itsLog==0) itsLog= new LogIO();
    *itsLog << LogOrigin("ImagePol", "imagepoltestimage");

    if (outFile.size() == 0) {
      *itsLog << LogIO::WARN << "You must give an outfile" << LogIO::POST;
      return rstat;
    }

    if(itsImPol) delete itsImPol;
    itsImPol = 0;

    // If not given make RM with no ambiguity
    Vector<Float> rm2(rm.size());
    if (rmDefault) {
      Double l1 = QC::c( ).getValue(Unit("m/s")) / f0;
      Double l2 = QC::c( ).getValue(Unit("m/s")) / (f0+df);
      rm2.resize(1);
      rm2(0) = M_PI / 2 / (l1*l1 - l2*l2);
    } else {
      for (uInt i = 0; i < rm.size(); i++) rm2[i] = rm[i];
    }

    const uInt nRM = rm2.nelements();
    //
    if (nRM == 1) {
      *itsLog << LogIO::NORMAL << "Using Rotation Measure = " << rm2(0)
      	      << " radians/m/m" << endl;
    } else {
      *itsLog << LogIO::NORMAL  << "Using Rotation Measures : " << endl;
      for (uInt i=0; i<nRM; i++) {
	*itsLog << "                          " << rm2(i)
		<< " radians/m/m" << endl;
      }
    }

    *itsLog << "Using pa0              = " << pa0 << " degrees" << endl;
    *itsLog << "Using frequency        = " << f0 << " Hz" << endl;
    *itsLog << "Using bandwidth        = " << df << " Hz " << endl;
    *itsLog << "Using number channels  = " << nf << LogIO::POST;

    // Make image
    IPosition shape(4,nx,ny,4,nf);
    ImageInterface<Float>* pImOut = 0;
    makeIQUVImage(pImOut, outFile, sigma, pa0*M_PI/180.0, rm2, shape, f0, df);
    try {
      itsImPol = new ImagePolarimetry(*pImOut);
      rstat = true;
    } catch (AipsError &x) {
      delete pImOut;
      pImOut = 0;
      *itsLog << x.getMesg() << LogIO::EXCEPTION;
    }
    //
    delete pImOut;
    pImOut = 0;

    return rstat;
  }


  Bool ImagePol::depolratio(ImageInterface<Float>*& returnim,
			    const String& infile, Bool debias, 
			    Double clip, Double sigma, 
			    const String& outfile){
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog <<"No attached image, please use open "
	      << LogIO::EXCEPTION;
      return rstat;
    }
    auto im1=itsImPol->imageInterface();
    std::unique_ptr<ImageInterface<Float> > im2;
    ImageUtilities::openImage(im2, infile);
    ImageExpr<Float> tmpim=itsImPol->depolarizationRatio(*im1, *im2, 
							 debias, 
							 Float(clip),
							 Float(sigma));

    String err;
    if(!copyImage(returnim, tmpim, outfile, true)){
      *itsLog <<"Could not convert ratio image "
	      << LogIO::EXCEPTION;
    }
    rstat = true;
    return rstat;
    
  }

  
Bool ImagePol::complexlinpol(const String& outfile) {
    *itsLog << LogOrigin("ImagePolProxy", __FUNCTION__);
    if (! itsImPol) {
        *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	        << LogIO::POST;
        return false;
    }

    if (outfile.size() == 0) {
        *itsLog << LogIO::WARN << "You must give an outfile" << LogIO::POST;
        return false;
    }
    CoordinateSystem cSysPol;
    const auto shapePol = itsImPol->singleStokesShape(
            cSysPol, Stokes::Plinear
        );
    const auto isMasked = itsImPol->isMasked();
    auto pOutComplex = _makeImage(
        outfile, cSysPol, shapePol, isMasked, false
    );
    auto expr = itsImPol->complexLinearPolarization();
    fiddleStokesCoordinate(*pOutComplex, Stokes::Plinear);

    // Copy to output

    pOutComplex->setCoordinateInfo(expr.coordinates());
    LatticeUtilities::copyDataAndMask(*itsLog, *pOutComplex, expr);
    auto p = itsImPol->imageInterface();
    copyMiscellaneous (*pOutComplex, *p);
    return true;
}
  
  // Summary
  void ImagePol::summary() const {
    *itsLog << LogOrigin("imagepol", "summary()");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return;
    }
    itsImPol->summary(*itsLog);
  }

  // sigma
  Float ImagePol::sigma(Float clip) const {
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return -1.0;
    }
    return itsImPol->sigma(clip);
  }

  // Stokes I
  Bool ImagePol::stokesI(ImageInterface<Float>*& rtnim, const String& outfile){
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", "stokesI(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }
    ImageExpr<Float> expr = itsImPol->stokesI();
    // Create output image
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }

  Float ImagePol::sigmaStokesI(Float clip) const {
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return -1.0;
    }
    return itsImPol->sigmaStokesI(clip);
  }

  // Stokes Q
  Bool ImagePol::stokesQ(ImageInterface<Float>*& rtnim, const String& outfile){
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", "stokesQ(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }
    ImageExpr<Float> expr = itsImPol->stokesQ();

    // Create output image if needed
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }

  Float ImagePol::sigmaStokesQ(Float clip) const {
    *itsLog << LogOrigin("imagepol", "sigmaStokesQ(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return -1.0;
    }
    return itsImPol->sigmaStokesQ(clip);
  }

  // Stokes U
  Bool ImagePol::stokesU(ImageInterface<Float>*& rtnim, const String& outfile){
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", "stokesU(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }
    ImageExpr<Float> expr = itsImPol->stokesU();

    // Create output image if needed
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }

  Float ImagePol::sigmaStokesU(Float clip) const {
    *itsLog << LogOrigin("imagepol", "sigmaStokesU(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return -1.0;
    }
    return itsImPol->sigmaStokesU(clip);
  }

  // Stokes V
  Bool ImagePol::stokesV(ImageInterface<Float>*& rtnim, const String& outfile){
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", "stokesV(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }
    ImageExpr<Float> expr = itsImPol->stokesV();

    // Create output image if needed
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }

  Float ImagePol::sigmaStokesV(Float clip) const {
   *itsLog << LogOrigin("imagepol", "sigmaStokesV(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return -1.0;
    }
    return itsImPol->sigmaStokesV(clip);
  }

  Float ImagePol::sigmaLinPolInt(Float clip, Float sigma) const {
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return -1.0;
    }
    return itsImPol->sigmaLinPolInt(clip, sigma);
  }

  Float ImagePol::sigmaTotPolInt(Float clip, Float sigma) const {
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return -1.0;
    }
    return itsImPol->sigmaTotPolInt(clip, sigma);
  }

  // Complex linear polarization
  void ImagePol::complexLinearPolarization (const String& outfile) {
    *itsLog << LogOrigin("imagepol", "complexLinearPolarization(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return;
    }

    if (outfile.size() == 0)
      *itsLog << LogIO::WARN << "You must give an outfile" << LogIO::POST;

    // Make output complex image
    ImageInterface<Complex>* pOutComplex = 0;
    CoordinateSystem cSysPol;
    IPosition shapePol = itsImPol->singleStokesShape(cSysPol, Stokes::Plinear);
    _makeImage (pOutComplex, outfile, cSysPol, shapePol,
	       itsImPol->isMasked(), false);

    // Make Expr
    ImageExpr<Complex> expr = itsImPol->complexLinearPolarization();
    fiddleStokesCoordinate(*pOutComplex, Stokes::Plinear);

    // Copy to output
    pOutComplex->setCoordinateInfo(expr.coordinates());
    LatticeUtilities::copyDataAndMask(*itsLog, *pOutComplex, expr);
    //
    auto p = itsImPol->imageInterface();
    copyMiscellaneous (*pOutComplex, *p);
    delete pOutComplex;
  }

  // Complex linear polarization
  void ImagePol::complexFractionalLinearPolarization (const String& outfile) {
    *itsLog << LogOrigin("imagepol", "complexFractionalLinearPolarization(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return;
    }

    // Make output complex image
    ImageInterface<Complex>* pOutComplex = 0;
    CoordinateSystem cSysPol;
    IPosition shapePol = itsImPol->singleStokesShape(cSysPol, Stokes::PFlinear);
    _makeImage (pOutComplex, outfile, cSysPol, shapePol,
	       itsImPol->isMasked(), false);

    std::unique_ptr<ImageInterface<Complex> > x(pOutComplex);

    // Make Expr
    ImageExpr<Complex> expr = itsImPol->complexFractionalLinearPolarization();
    fiddleStokesCoordinate(*pOutComplex, Stokes::PFlinear);

    // Copy to output
    pOutComplex->setCoordinateInfo(expr.coordinates());
    LatticeUtilities::copyDataAndMask(*itsLog, *pOutComplex, expr);
    copyMiscellaneous (*pOutComplex, *(itsImPol->imageInterface()));
  }

  Bool ImagePol::sigmaLinPolPosAng(ImageInterface<Float>*& rtnim, Float clip,
				   Float sigma, const String& outfile) {
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }

    Bool radians = false;
    ImageExpr<Float> expr = itsImPol->sigmaLinPolPosAng(radians, clip, sigma);

    // Create output image if needed
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }


  // Fractional linearly polarized intensity
  Bool ImagePol::fracLinPol(ImageInterface<Float>*& rtnim, Bool debias,
			    Float clip, Float sigma, const String& outfile) {
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", "fracLinPol(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }

    ImageExpr<Float> expr = itsImPol->fracLinPol(debias, clip, sigma);

    // Create output image
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }


  Bool ImagePol::sigmaFracLinPol(ImageInterface<Float>*& rtnim, Float clip,
				 Float sigma, const String& outfile){
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }

    ImageExpr<Float> expr = itsImPol->sigmaFracLinPol(clip, sigma);

    // Create output image if needed
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }


  // Fractional total polarized intensity
  Bool ImagePol::fracTotPol(ImageInterface<Float>*& rtnim, Bool debias,
			    Float clip, Float sigma, const String& outfile) {
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", "fracTotPol(...)");
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }

    ImageExpr<Float> expr = itsImPol->fracTotPol(debias, clip, sigma);

    // Create output image if needed
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }


  Bool ImagePol::sigmaFracTotPol(ImageInterface<Float>*& rtnim, Float clip,
		       Float sigma, const String& outfile){
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }

    ImageExpr<Float> expr = itsImPol->sigmaFracTotPol(clip, sigma);

    // Create output image if needed
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }


  // Depolarization ratio
  Bool ImagePol::depolarizationRatio (ImageInterface<Float>*& rtnim, 
			    const String& infile,
			    Bool debias, Float clip,
			    Float sigma, const String& outfile) {
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << "No attached image, please use open "
	      << LogIO::EXCEPTION;
    }
    auto imagePointer1 = itsImPol->imageInterface();
    ImageInterface<Float>* imagePointer2 = 0;
    ImageUtilities::openImage (imagePointer2, infile);
    //
    ImageExpr<Float> expr =
      ImagePolarimetry::depolarizationRatio(*imagePointer1, *imagePointer2,
					    debias, clip, sigma);
    //
    if (imagePointer2) delete imagePointer2;
    imagePointer2 = 0;
    
    // Create output image
    rstat = copyImage (rtnim, expr, outfile, true);

    return rstat;
  }


  Bool ImagePol::sigmaDepolarizationRatio (ImageInterface<Float>*& rtnim,
				 const String& infile,
				 Bool debias, Float clip,
				 Float sigma, const String& outfile) {
    Bool rstat(false);
    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return rstat;
    }
    //
    auto imagePointer1 = itsImPol->imageInterface();
    //
    ImageInterface<Float>* imagePointer2 = 0;
    ImageUtilities::openImage(imagePointer2, infile);
    //
    ImageExpr<Float> expr =
      ImagePolarimetry::sigmaDepolarizationRatio(*imagePointer1,
						 *imagePointer2,
						 debias, clip, sigma);
    //
    if (imagePointer2) delete imagePointer2;
    imagePointer2 = 0;

    // Create output image
    rstat = copyImage (rtnim, expr, outfile, true);
    return rstat;
  }


  // Find Rotation Measure from Fourier method
  void ImagePol::fourierRotationMeasure(const String& outfile,
			      const String& outfileAmp,
			      const String& outfilePA,
			      const String& outfileReal,
			      const String& outfileImag,
			      Bool zeroZeroLag) {

    *itsLog << LogOrigin("imagepol", __FUNCTION__, WHERE);
    if(itsImPol==0){
      *itsLog <<"No attached image, please use open "
	      << LogIO::EXCEPTION;
      return;
    }
    if (itsImPol->imageInterface()->imageInfo().hasMultipleBeams()) {
    	*itsLog << "Cannot run " << __FUNCTION__
    		<< " on an image with multiple beams"
    		<< LogIO::EXCEPTION;
    }


    // Make output complex image
    ImageInterface<Complex>* pOutComplex = 0;
    CoordinateSystem cSysPol;
    IPosition shapePol = itsImPol->singleStokesShape(cSysPol, Stokes::Plinear);
    _makeImage (pOutComplex, outfile, cSysPol, shapePol,
	       itsImPol->isMasked(), true);

    // Make output amplitude and position angle images
    ImageInterface<Float>* pOutAmp = 0;
    ImageInterface<Float>* pOutPA = 0;
    makeImage (pOutAmp, outfileAmp, cSysPol, shapePol,
	       itsImPol->isMasked(), false);
    makeImage (pOutPA, outfilePA, cSysPol, shapePol,
	       itsImPol->isMasked(), false);

    // Make output real and imaginary images
    ImageInterface<Float>* pOutReal = 0;
    ImageInterface<Float>* pOutImag = 0;
    makeImage (pOutReal, outfileReal, cSysPol, shapePol,
	       itsImPol->isMasked(), false);
    makeImage (pOutImag, outfileImag, cSysPol, shapePol,
	       itsImPol->isMasked(), false);

    // The output complex image will have correct Coordinates, mask, and
    // miscellaneous things copied to it
    itsImPol->fourierRotationMeasure(*pOutComplex, zeroZeroLag);

    // Copy to output
    auto p = itsImPol->imageInterface();
    if (pOutAmp!=0) {
      LatticeExprNode node(abs(*pOutComplex));
      LatticeExpr<Float> le(node);
      LatticeUtilities::copyDataAndMask(*itsLog, *pOutAmp, le);
      //
      pOutAmp->setCoordinateInfo(pOutComplex->coordinates());
      copyMiscellaneous (*pOutAmp, *p);
      pOutAmp->setUnits(p->units());
      fiddleStokesCoordinate(*pOutAmp, Stokes::Plinear);
      delete pOutAmp;
    }
    if (pOutPA!=0) {
      LatticeExprNode node(pa(imag(*pOutComplex),real(*pOutComplex)));  // degrees
      LatticeExpr<Float> le(node);
      LatticeUtilities::copyDataAndMask(*itsLog, *pOutPA, le);
      //
      pOutPA->setCoordinateInfo(pOutComplex->coordinates());
      copyMiscellaneous (*pOutPA, *p);
      pOutPA->setUnits("deg");
      fiddleStokesCoordinate(*pOutPA, Stokes::Pangle);
      delete pOutPA;
    }
    if (pOutReal!=0) {
      LatticeExprNode node(real(*pOutComplex));
      LatticeExpr<Float> le(node);
      LatticeUtilities::copyDataAndMask(*itsLog, *pOutReal, le);
      pOutReal->setCoordinateInfo(pOutComplex->coordinates());
      copyMiscellaneous (*pOutReal, *p);
      pOutReal->setUnits(p->units());
      fiddleStokesCoordinate(*pOutReal, Stokes::Plinear);  // Not strictly correct
      delete pOutReal;
    }
    if (pOutImag!=0) {
      LatticeExprNode node(imag(*pOutComplex));
      LatticeExpr<Float> le(node);
      LatticeUtilities::copyDataAndMask(*itsLog, *pOutImag, le);
      pOutImag->setCoordinateInfo(pOutComplex->coordinates());
      copyMiscellaneous (*pOutImag, *p);
      pOutImag->setUnits(p->units());
      fiddleStokesCoordinate(*pOutImag, Stokes::Plinear);  // Not strictly correct
      delete pOutImag;
    }
  }

void ImagePol::rotationMeasure(
    const String& outRM, const String& outRMErr,
    const String& outPA0, const String& outPA0Err,
    const String& outNTurns, const String& outChiSq,
    Int axis2, Float sigmaQU, Float rmFg,
    Float rmMax, Float maxPaErr
) {
    // Find Rotation Measure from traditional method
    *itsLog << LogOrigin("ImagePol", __func__);
    if(itsImPol==0) {
        *itsLog << LogIO::SEVERE <<"No attached image, please use open "
            << LogIO::POST;
        return;
    }
    // Make output images.  Give them all a mask as we don't know if output
    // will be masked or not.
    CoordinateSystem cSysRM;
    Int fAxis, sAxis;
    Int axis = axis2;
    IPosition shapeRM = itsImPol->rotationMeasureShape(
        cSysRM, fAxis, sAxis, *itsLog, axis
    );
    ImageInterface<Float>* pRMOut = 0;
    ImageInterface<Float>* pRMOutErr = 0;
    makeImage (pRMOut, outRM, cSysRM, shapeRM, true, false);
    // manage naked pointers so exception throwing doesn't leave open images
    std::unique_ptr<ImageInterface<Float> > managed5(pRMOut);
    makeImage (pRMOutErr, outRMErr, cSysRM, shapeRM, true, false);
    std::unique_ptr<ImageInterface<Float> > managed6(pRMOutErr);
    CoordinateSystem cSysPA;
    IPosition shapePA = itsImPol->positionAngleShape(
        cSysPA, fAxis, sAxis, *itsLog, axis
    );
    ImageInterface<Float>* pPA0Out = 0;
    ImageInterface<Float>* pPA0OutErr = 0;
    makeImage (pPA0Out, outPA0, cSysPA, shapePA, true, false);
    std::unique_ptr<ImageInterface<Float> > managed1(pPA0Out);
    makeImage (pPA0OutErr, outPA0Err, cSysPA, shapePA, true, false);
    std::unique_ptr<ImageInterface<Float> > managed2(pPA0OutErr);
    ImageInterface<Float>* pNTurnsOut = 0;
    makeImage (pNTurnsOut, outNTurns, cSysRM, shapeRM, true, false);
    std::unique_ptr<ImageInterface<Float> > managed3(pNTurnsOut);
    ImageInterface<Float>* pChiSqOut = 0;
    makeImage (pChiSqOut, outChiSq, cSysRM, shapeRM, true, false);
    std::unique_ptr<ImageInterface<Float> > managed4(pChiSqOut);
    itsImPol->rotationMeasure(
    	pRMOut, pRMOutErr, pPA0Out, pPA0OutErr,
    	pNTurnsOut, pChiSqOut, 
    	axis, rmMax, maxPaErr,
    	sigmaQU, rmFg, true
    );
    auto p = itsImPol->imageInterface();
    if (pRMOut) {
        copyMiscellaneous (*pRMOut, *p);
    }
    if (pRMOutErr) {
        copyMiscellaneous (*pRMOutErr, *p);
    }
    if (pPA0Out) {
        copyMiscellaneous (*pPA0Out, *p);
    }
    if (pPA0OutErr) {
      copyMiscellaneous (*pPA0OutErr, *p);
    }
    if (pNTurnsOut) {
        copyMiscellaneous (*pNTurnsOut, *p);
    }
    if (pChiSqOut) {
        copyMiscellaneous (*pChiSqOut, *p);
    }
}


  // Make a complex image
  void ImagePol::makeComplex (const String& outfile, const String& real,
		    const String& imag, const String& amp,
		    const String& phase) {

    *itsLog << LogOrigin("imagepol", __FUNCTION__);
    if(itsImPol==0){
      *itsLog << LogIO::SEVERE <<"No attached image, please use open " 
	      << LogIO::POST;
      return;
    }

    // Checks
    if (outfile.empty()) {
      *itsLog << "You must give the output complex image file name"
	      << LogIO::EXCEPTION;
    }

    Bool doRI = !real.empty() && !imag.empty();
    Bool doAP = !amp.empty() && !phase.empty();
    if (doRI && doAP) {
      *itsLog << "You must give either real and imaginary, or amplitude and phase; Not all four of them"
	      << LogIO::EXCEPTION;
    }

    // Make output complex image
    ImageInterface<Complex>* pOutComplex = 0;
    CoordinateSystem cSysPol;
    IPosition shapePol = itsImPol->singleStokesShape(cSysPol, Stokes::I);
    _makeImage (pOutComplex, outfile, cSysPol, shapePol,
	       itsImPol->isMasked(), false);

    // Make Expression. Only limited Stokes types that make sense are allowed.
    ImageExpr<Complex>* pExpr = 0;
    if (doRI) {
      PagedImage<Float> rr(real);
      Stokes::StokesTypes tR = stokesType(rr.coordinates());
      //
      PagedImage<Float> ii(imag);
      Stokes::StokesTypes tI = stokesType(ii.coordinates());
      //
      if (tR!=Stokes::Q || tI!=Stokes::U) {
	*itsLog << "The real and imaginary components must be Q and U, respectively"
		<< LogIO::EXCEPTION;
      }
      Stokes::StokesTypes typeOut = Stokes::Plinear;
      //
      LatticeExprNode node(formComplex(rr,ii));
      LatticeExpr<Complex> le(node);
      pExpr = new ImageExpr<Complex>(le, String("ComplexLinearPolarization"));
      fiddleStokesCoordinate(*pExpr, typeOut);
    } else {
      PagedImage<Float> aa(amp);
      Stokes::StokesTypes tA = stokesType(aa.coordinates());
      //
      PagedImage<Float> pp(phase);
      Stokes::StokesTypes tP = stokesType(pp.coordinates());
      //
      if (tP!=Stokes::Pangle) {
	*itsLog << "The phase must be of Stokes type position angle (Pangle)"
		<< LogIO::EXCEPTION;
      }
      Float fac = 1.0;
      String units = pp.units().getName();
      if (units.contains(String("deg"))) {
	fac = M_PI / 180.0;
      } else if (units.contains(String("rad"))) {
      } else {
	*itsLog << LogIO::WARN
		<< "Units for phase are neither radians nor degrees. radians assumed"
		<< LogIO::POST;
      }
      //
      Stokes::StokesTypes typeOut = Stokes::Undefined;
      String exprName("");
      if (tA==Stokes::Ptotal) {
	typeOut = Stokes::Ptotal;
	exprName = String("ComplexTotalPolarization");
      } else if (tA==Stokes::Plinear) {
	typeOut = Stokes::Plinear;
	exprName = String("ComplexLinearPolarization");
      } else if (tA==Stokes::PFtotal) {
	typeOut = Stokes::PFtotal;
	exprName = String("ComplexFractionalTotalPolarization");
      } else if (tA==Stokes::PFlinear) {
	typeOut = Stokes::PFlinear;
	exprName = String("ComplexFractionalLinearPolarization");
      } else {
	*itsLog << "Cannot form Complex image for this amplitude image"
		<< endl;
	*itsLog << "Expect linear, total, or fractional polarization"
		<< LogIO::EXCEPTION;
      }
      //
      LatticeExprNode node0(2.0*fac*pp);
      LatticeExprNode node(formComplex(aa*cos(node0),aa*sin(node0)));
      LatticeExpr<Complex> le(node);
      pExpr = new ImageExpr<Complex>(le, exprName);
      fiddleStokesCoordinate(*pExpr, typeOut);
    }

    // Copy to output
    pOutComplex->setCoordinateInfo(pExpr->coordinates());
    LatticeUtilities::copyDataAndMask(*itsLog, *pOutComplex, *pExpr);
    //
    auto p = itsImPol->imageInterface();
    copyMiscellaneous (*pOutComplex, *p);
    //
    delete pExpr; pExpr = 0;
    delete pOutComplex; pOutComplex = 0;
  }


  // Private functions

  Bool ImagePol::copyImage(ImageInterface<Float>*& out, 
			   const ImageInterface<Float>& in, 
			   const String& outfile, Bool overwrite){

    // if no outfile, just make out image from the input image
    if (outfile.empty()){
      out = in.cloneII();
      return true;
    }

    // The user wants to write the image out; verify file
    if (!overwrite) {
       NewFile validfile;
       String errmsg;
       if (!validfile.valueOK(outfile, errmsg)) {
           *itsLog << errmsg << LogIO::EXCEPTION;
       }
    }

    // Create output image
    out= new PagedImage<Float>(in.shape(), in.coordinates(), outfile);
    if (out == 0) {
      *itsLog << "Failed to create PagedImage" << LogIO::EXCEPTION;
    } else {
      *itsLog << LogIO::NORMAL << "Creating image '" << outfile
	      << "' of shape " << out->shape() << LogIO::POST;
    }

    // Make mask
    if (in.isMasked()) makeMask(*out, false);

    // Copy stuff
    LatticeUtilities::copyDataAndMask (*itsLog, *out, in);
    ImageUtilities::copyMiscellaneous (*out, in);
    
    return true;

  }

  Bool ImagePol::makeMask(ImageInterface<Float>& out, Bool init){

    if (out.canDefineRegion()) {

      // Generate mask name if not given
      String maskName = out.makeUniqueRegionName(String("mask"), 0);
      
      // Make the mask if it does not exist
      if (!out.hasRegion (maskName, RegionHandler::Masks)) {
         out.makeMask (maskName, true, true, init, true);
	 /* if (init) {
            *itsLog << LogIO::NORMAL << "Created and initialized mask `" << maskName << "'" << LogIO::POST;
         } else {
            *itsLog << LogIO::NORMAL << "Created mask `" << maskName << "'" << LogIO::POST;
	    }*/
      }
      return true;
    } else {
      *itsLog << LogIO::WARN
	      << "Cannot make requested mask for this type of image" << endl;
      return false;
    }
    
  }
  Bool ImagePol::makeMask(ImageInterface<Complex>& out, Bool init){

    if (out.canDefineRegion()) {

      // Generate mask name if not given
      String maskName = out.makeUniqueRegionName(String("mask"), 0);
      
      // Make the mask if it does not exist
      if (!out.hasRegion (maskName, RegionHandler::Masks)) {
	out.makeMask (maskName, true, true, init, true);
	/* if (init) {
	 *itsLog << LogIO::NORMAL << "Created and initialized mask `" << maskName << "'" << LogIO::POST;
         } else {
	 *itsLog << LogIO::NORMAL << "Created mask `" << maskName << "'" << LogIO::POST;
	 }*/
      }
      return true;
    } else {
      *itsLog << LogIO::WARN
	      << "Cannot make requested mask for this type of image" << endl;
      return false;
    }
    
  }


  Bool ImagePol::makeImage(ImageInterface<Float>*& out, 
			   const String& outfile, const CoordinateSystem& cSys,
			   const IPosition& shape, Bool isMasked,
			   Bool tempAllowed) {
    // Verify outfile
    if (outfile.empty()) {
      if (!tempAllowed) return false;
    }
    // else {
    //  NewFile validfile;
    //  String errmsg;
    //  if (!validfile.valueOK(outfile, errmsg)) {
    //	*itsLog << errmsg << LogIO::EXCEPTION;
    //  }
    //}


    uInt ndim = shape.nelements();
    if (ndim != cSys.nPixelAxes()) {
      *itsLog << "Supplied CoordinateSystem and image shape are inconsistent"
	      << LogIO::EXCEPTION;
    }
    if (outfile.empty()) {
       out = new TempImage<Float>(shape, cSys);
       if (out == 0) {
          *itsLog << "Failed to create TempImage" << LogIO::EXCEPTION;
       }
       *itsLog << LogIO::NORMAL << "Creating (temp)image of shape "
	       << out->shape() << LogIO::POST;
    } else {
       out = new PagedImage<Float>(shape, cSys, outfile);
       if (out == 0) {
	 *itsLog << "Failed to create PagedImage" << LogIO::EXCEPTION;
       }
       *itsLog << LogIO::NORMAL << "Creating image '" << outfile
	       << "' of shape " << out->shape() << LogIO::POST;
    }
    //
    if (isMasked) {
       makeMask(*out, true);
    }

    return true;
  }

Bool ImagePol::_makeImage(
    ImageInterface<Complex>*& out, const String& outfile,
    const CoordinateSystem& cSys, const IPosition& shape, Bool isMasked,
    Bool tempAllowed
) {
    const auto ndim = shape.nelements();
    if (ndim != cSys.nPixelAxes()) {
        *itsLog << "Supplied CoordinateSystem and image shape are inconsistent"
	        << LogIO::EXCEPTION;
    }
    if (outfile.empty()) {
        if (tempAllowed) {
            out = new TempImage<Complex>(shape, cSys);
            if (! out) {
                *itsLog << "Failed to create TempImage" << LogIO::EXCEPTION;
            }
            *itsLog << LogIO::NORMAL << "Creating (temp)image of shape "
	            << out->shape() << LogIO::POST;
        }
        else {
            return false;
        }
    }
    else {
        out = new PagedImage<Complex>(shape, cSys, outfile);
        if (! out) {
	        *itsLog << "Failed to create PagedImage" << LogIO::EXCEPTION;
        }
        *itsLog << LogIO::NORMAL << "Creating image '" << outfile
	        << "' of shape " << out->shape() << LogIO::POST;
    }
    if (isMasked) {
        makeMask(*out, true);
    }
    return true;
}

SPIIC ImagePol::_makeImage( 
    const String& outfile, const CoordinateSystem& cSys,
    const IPosition& shape, bool isMasked,
    bool tempAllowed
) {
    ImageInterface<Complex>* out = NULL;
    const auto res = _makeImage(
        out, outfile, cSys, shape, isMasked, tempAllowed
    );
    if (res) {
        return shared_ptr<ImageInterface<Complex>>(out);
    }
    else {
        if (out) {
            delete out;
        }
        return shared_ptr<ImageInterface<Complex>>();
    }
}

  void ImagePol::copyMiscellaneous (ImageInterface<Complex>& out,
				    const ImageInterface<Float>& in)
  {
    out.setMiscInfo(in.miscInfo());
    out.appendLog(in.logger());
  }

  void ImagePol::copyMiscellaneous (ImageInterface<Float>& out,
				    const ImageInterface<Float>& in)
  {
    out.setMiscInfo(in.miscInfo());
    out.appendLog(in.logger());
  }


  void ImagePol::fiddleStokesCoordinate(ImageInterface<Float>& ie,
					Stokes::StokesTypes type)
  {
    CoordinateSystem cSys = ie.coordinates();
    //
    Int afterCoord = -1;
    Int iStokes = cSys.findCoordinate(Coordinate::STOKES, afterCoord);
    //
    Vector<Int> which(1);
    which(0) = Int(type);
    StokesCoordinate stokes(which);
    cSys.replaceCoordinate(stokes, iStokes);
    ie.setCoordinateInfo(cSys);
  }

  void ImagePol::fiddleStokesCoordinate(ImageInterface<Complex>& ie, 
					Stokes::StokesTypes type)
  {
    CoordinateSystem cSys = ie.coordinates();
    //
    Int afterCoord = -1;
    Int iStokes = cSys.findCoordinate(Coordinate::STOKES, afterCoord);
    //
    Vector<Int> which(1);
    which(0) = Int(type);
    StokesCoordinate stokes(which);
    cSys.replaceCoordinate(stokes, iStokes);
    ie.setCoordinateInfo(cSys);
  }


  Bool ImagePol::makeIQUVImage (ImageInterface<Float>*& pImOut, 
				const String& outfile, 
				Double sigma, Double pa0, 
				const Vector<Float>& rm, 
				const IPosition& shape,
				Double f0, Double dF)
    //
    // Must be 4D
    //
  {
    AlwaysAssert(shape.nelements()==4,AipsError);
    AlwaysAssert(shape(2)==4,AipsError);
    //
    CoordinateSystem cSys;
    CoordinateUtil::addDirAxes(cSys);
    //
    Vector<Int> whichStokes(4);
    whichStokes(0) = Stokes::I;
    whichStokes(1) = Stokes::Q;
    whichStokes(2) = Stokes::U;
    whichStokes(3) = Stokes::V;
    StokesCoordinate stokesCoord(whichStokes);
    cSys.addCoordinate(stokesCoord);
    //
    const Int nchan = shape(3);
    Double df = dF / nchan;
    Double refpix = 0.0;
    SpectralCoordinate spectCoord(MFrequency::TOPO, f0, df, refpix, f0);
    cSys.addCoordinate(spectCoord);
   
    // Centre reference pixel
    centreRefPix (cSys, shape);
   
    // Make image 
    makeImage (pImOut, outfile, cSys, shape, false, true);
    //
    uInt stokesAxis = 2;
    uInt spectralAxis = 3;
   
    // Fill image with I, Q, U and V. 
    fillIQUV (*pImOut, stokesAxis, spectralAxis, rm, pa0);

    // Add noise 
    Array<Float> slice = pImOut->get();
    Float maxVal = max(slice);
    Float t = sigma * maxVal;
    *itsLog << LogIO::NORMAL << "Using sigma            = "
	    << t << LogIO::POST;
    MLCG gen;
    Normal noiseGen(&gen, 0.0, t*t);
    addNoise(slice, noiseGen);
    pImOut->put(slice);
    return true;
  }

Bool  ImagePol::fillIQUV (ImageInterface<Float>& im, uInt stokesAxis, 
                         uInt spectralAxis, const Vector<Float>& rm, 
                         Float pa0)
  //
  // Image must be 4D
  //
{

  // Find spectral coordinate
  const CoordinateSystem& cSys = im.coordinates();   
  Int spectralCoord, iDum;
  cSys.findPixelAxis(spectralCoord, iDum, spectralAxis);
  const SpectralCoordinate& sC = cSys.spectralCoordinate(spectralCoord);
  //
  IPosition shape = im.shape();
  Double c = QC::c( ).getValue(Unit("m/s"));
  Double lambdasq;
  MFrequency freq;
  IPosition blc(4,0);
  IPosition trc(shape-1);
  //
  Double ii = 2.0;                      // arbitrary
  Double vv = 0.05 * ii;                // arbitrary
  const Int n = shape(3);
  const uInt nRM = rm.nelements();
  for (Int i=0; i<n; i++) {
    if (!sC.toWorld(freq, Double(i))) {
      *itsLog << sC.errorMessage() << LogIO::EXCEPTION;
    }
    Double fac = c / freq.get(Unit("Hz")).getValue();
    lambdasq = fac*fac;
    //
    Double chi = rm(0)*lambdasq + pa0;
    Double q = cos(2*chi);
    Double u = sin(2*chi);
    //
    if (nRM > 1) {
      for (uInt j=1; j<nRM; j++) {
	chi = rm(j)*lambdasq + pa0;
	q += cos(2*chi);
	u += sin(2*chi);
      }
    }
    q = q / Double(nRM);
    u = u / Double(nRM);
    //
    blc(spectralAxis) = i;                // channel
    trc(spectralAxis) = i;
    {
      blc(stokesAxis) = 1;                // Q       
      trc(stokesAxis) = 1;                
      Slicer sl(blc, trc, Slicer::endIsLast);
      SubImage<Float> subImage(im, sl, true);
      subImage.set(q);
    }
    {
      blc(stokesAxis) = 2;                // U
      trc(stokesAxis) = 2;                
      Slicer sl(blc, trc, Slicer::endIsLast);
      SubImage<Float> subImage(im, sl, true);
      subImage.set(u);
    }
  }
  // 
  blc(spectralAxis) = 0;  
  trc(spectralAxis) = n-1;
  {
    blc(stokesAxis) = 0;                // I
    trc(stokesAxis) = 0;                
    Slicer sl(blc, trc, Slicer::endIsLast);
    SubImage<Float> subImage(im, sl, true);
    subImage.set(ii);
  }
  {
    blc(stokesAxis) = 3;                // V
    trc(stokesAxis) = 3;                
    Slicer sl(blc, trc, Slicer::endIsLast);
    SubImage<Float> subImage(im, sl, true);
    subImage.set(vv);
  }
  return true;
  
}

void ImagePol::addNoise (Array<Float>& slice, Normal& noiseGen) 
{
   Bool deleteIt;
   Float* p = slice.getStorage(deleteIt);
   for (uInt i=0; i<slice.nelements(); i++) {
      p[i] += noiseGen();
   }
   slice.putStorage(p, deleteIt);
}

void ImagePol::centreRefPix (CoordinateSystem& cSys, const IPosition& shape)
{
   Int after = -1;
   Int iS = cSys.findCoordinate(Coordinate::STOKES, after);
   Int sP = -1;
   if (iS>=0) {
      Vector<Int> pixelAxes = cSys.pixelAxes(iS);
      sP = pixelAxes(0);
   }
   Vector<Double> refPix = cSys.referencePixel();
   for (Int i=0; i<Int(refPix.nelements()); i++) {
     if (i!=sP) refPix(i) = Double(shape(i) / 2);
   }     
   cSys.setReferencePixel(refPix);
}


Stokes::StokesTypes ImagePol::stokesType(const CoordinateSystem& cSys) 
{
  Stokes::StokesTypes type = Stokes::Undefined;
  Int afterCoord = -1;
  Int iStokes = cSys.findCoordinate(Coordinate::STOKES, afterCoord);
  if (iStokes >=0) {
    Vector<Int> which = cSys.stokesCoordinate(iStokes).stokes();
    if (which.nelements()>1) {
        *itsLog << "Stokes axis must be of length unity" << LogIO::EXCEPTION;
      } else {
         type = Stokes::type(which(0));
      }
  } else {
    *itsLog << "No StokesCoordinate" << LogIO::EXCEPTION;
   }
   return type;
}


} // end of  casa namespace
