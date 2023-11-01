//# NNLSImageSkyModel.cc: Implementation of NNLSImageSkyModel class
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayPosIter.h>
#include <synthesis/MeasurementComponents/NNLSImageSkyModel.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/HostInfo.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <casacore/scimath/Mathematics/NNLSMatrixSolver.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// NNLS solver: This could be a whole lot smarter about memory use!
Bool NNLSImageSkyModel::solve(SkyEquation& se) {
  
  LogMessage message(LogOrigin("NNLSImageSkyModel","solve"));
  
  if(numberOfModels()>1) {
    message.message("Cannot process more than one field");
    logSink().post(message);
    return false;
  }
  
  // Zero Stokes I for the masked region
  maskedZeroI();
  
  // Make the dirty image
  makeNewtonRaphsonStep(se);
  
  //Make the PSF
  makeApproxPSFs(se);
  
  if(isSolveable(0)) {
    
    Int nx=image(0).shape()(0);
    Int ny=image(0).shape()(1);
    Int npol=image(0).shape()(2);
    Int nchan=image(0).shape()(3);
    
    NNLSMatrixSolver matrixSolver;	// Matrix solver to be used
    matrixSolver.setGain(gain());
    matrixSolver.setMaxIters(numberIterations());
    matrixSolver.setTolerance(tolerance());
    
    // Loop over all channels
    LatticeStepper psfls(PSF(0).shape(), IPosition(4,nx,ny,1,1),
			 IPosition(4,0,1,2,3));
    RO_LatticeIterator<Float> psfli(PSF(0),psfls);
    LatticeStepper ls(image(0).shape(), IPosition(4, nx, ny, npol, 1),
		      IPosition(4, 0, 1, 2, 3));
    LatticeIterator<Float> imageli(image(0), ls);
    
    // Get I plane of image
    Matrix<Float> limage;
    if(npol>1) {
      limage=imageli.cubeCursor().xyPlane(0);
    }
    else {
      limage=imageli.matrixCursor();
    }
    LatticeIterator<Float> imageStepli(residual(0), ls);
    Matrix<Float> limageStep;
    if(npol>1) {
      limageStep=imageStepli.cubeCursor().xyPlane(0);
    }
    else {
      limageStep=imageStepli.matrixCursor();
    }
    // Get the flux mask?
    ArrayPositionIterator fai(limageStep.shape(),0);
    Int lFluxMask=nx*ny;
    Matrix<Float> lfluxmask;
    if(hasFluxMask(0)) {
      AlwaysAssert(fluxMask(0).shape()(0)==nx, AipsError);
      AlwaysAssert(fluxMask(0).shape()(1)==ny, AipsError);
      LatticeStepper mls(fluxMask(0).shape(),
			 IPosition(4, nx, ny, npol, 1),
			 IPosition(4, 0, 1, 2, 3));
      RO_LatticeIterator<Float> fluxmaskli(fluxMask(0), mls);
      fluxmaskli.reset();
      if(npol>1) {
	lfluxmask=fluxmaskli.cubeCursor().xyPlane(0); 
      }
      else {
	lfluxmask=fluxmaskli.matrixCursor();
      }
      lFluxMask=0;
      ArrayPositionIterator fai(limage.shape(),0);
      for(fai.origin();!fai.pastEnd();fai.next()) {
	if(lfluxmask(fai.pos())>0.0) lFluxMask++;
      }
      AlwaysAssert(lFluxMask>0, AipsError);
    }
    else {
      lfluxmask.resize(nx,ny);
      lfluxmask=1.0;
    }
    // Get the data mask?
    Int lMask=nx*ny;
    Matrix<Float> lmask;
    ArrayPositionIterator dai(limageStep.shape(),0);
    if(hasMask(0)) {
      AlwaysAssert(mask(0).shape()(0)==nx, AipsError);
      AlwaysAssert(mask(0).shape()(1)==ny, AipsError);
      LatticeStepper mls(mask(0).shape(),
			 IPosition(4, nx, ny, npol, 1),
			 IPosition(4, 0, 1, 2, 3));
      RO_LatticeIterator<Float> maskli(mask(0), mls); maskli.reset();
      if(npol>1) {
	lmask=maskli.cubeCursor().xyPlane(0);
      }
      else {
	lmask=maskli.matrixCursor();
      }
      lMask=0;
      ArrayPositionIterator dai(limageStep.shape(),0);
      for(dai.origin();!dai.pastEnd();dai.next()) {
	if(lmask(dai.pos())>0.0) lMask++;
      }
      AlwaysAssert(lMask>0, AipsError);
    }
    else {
      lmask.resize(nx,ny);
      lmask=1.0;
    }
    
    {
      ostringstream o; o <<""<<lMask<<" constraints on "<<lFluxMask
		      <<" free pixels";message.message(o);
      logSink().post(message);
    }

    if(4.0*Double(lMask)*Double(lFluxMask)>Double(HostInfo::memoryTotal(true))*1024.0) {
      ostringstream o;
      o << "Insufficient memory for PSF matrix: reduce the size of the masks";
      message.message(o);
      logSink().post(message);
      return false;
    }
    Matrix<Float> AMatrix(lMask, lFluxMask);
    Vector<Float> XVector(lFluxMask); // Unknown X vector
    Vector<Float> BVector(lMask); // Data Constraint Vector AX=B
    
    Int chan=0;
    for (imageStepli.reset(),imageli.reset(),psfli.reset();
	 !imageStepli.atEnd();
	 imageStepli++,imageli++,psfli++,chan++) {
      {
	ostringstream o; o<<"Processing channel "<<chan+1<<" of "
		       <<nchan<<endl;message.message(o);
	logSink().post(message);
      }
      
      const Matrix<Float>& lpsf=psfli.matrixCursor();
      // Make IPositions and find position of peak of PSF
      IPosition psfposmax(lpsf.ndim());
      IPosition psfposmin(lpsf.ndim());
      Float psfmax;
      Float psfmin;
      minMax(psfmin, psfmax, psfposmin, psfposmax, lpsf); 
      
      // Loop through masks setting AMatrix as required 
      if(psfmax==0.0) {
	ostringstream o; o<<"No data for this channel: skipping";
	message.message(o);
	logSink().post(message);
      }
      else {
	int xvi=0;
	int bvi=0;
	for(fai.origin();!fai.pastEnd();fai.next()) {
	  if(lfluxmask(fai.pos())>0.000001) {
	    bvi=0;
	    for(dai.origin();!dai.pastEnd();dai.next()) {
	      if(lmask(dai.pos())>0.0) {
		AMatrix(bvi,xvi)=lpsf(psfposmax+fai.pos()-dai.pos());
		bvi++;
	      }
	    }
	    xvi++;
	  }
	}
	
	// Construct the X vector.
	xvi=0;
	for(fai.origin();!fai.pastEnd();fai.next()) {
	  if(lfluxmask(fai.pos())>0.000001) {
	    XVector(xvi)=limage(fai.pos());
	    xvi++;
	  }
	}
	
	// Construct the B vector
	bvi=0;
	for(dai.origin();!dai.pastEnd();dai.next()) {
	  if(lmask(dai.pos())>0.0000001) {
	    BVector(bvi)=limageStep(dai.pos());
	    bvi++;
	  }
	}
	
	// Set A matrix and B vector in the matrixSolver
	matrixSolver.setAB(AMatrix, BVector);
	
	// Call MatrixSolver
	{
	  ostringstream o; o<<"Performing solution";message.message(o);
	  logSink().post(message);
	}
	if(!matrixSolver.solve()) {
	  {
	    ostringstream o; o<<"matrixSolver nominally failed";
	    message.message(o);
	    logSink().post(message);
	  }
	}
	XVector=matrixSolver.getSolution();
	
	// Fill in final image and residual image
	xvi=0;
	for(fai.origin();!fai.pastEnd();fai.next()) {
	  if(lfluxmask(fai.pos())>0.0000001) {
	    limage(fai.pos())=XVector(xvi);
	    xvi++;
	  }
	}
	bvi=0;
	for(dai.origin();!dai.pastEnd();dai.next()) {
	  if(lmask(dai.pos())>0.0000001) {
	    limageStep(dai.pos())=BVector(bvi);
	    bvi++;
	  }
	}
	
	if(npol>1) {
	  imageli.rwCubeCursor().xyPlane(0)=limage;
	}
	else {
	  imageli.woMatrixCursor()=limage;
	}
	if(npol>1) {
	  imageStepli.rwCubeCursor().xyPlane(0)=limageStep;
	}
	else {
	  imageStepli.woMatrixCursor()=limageStep;
	}
      }
    }
  }
  return(true);
};

// Zero Stokes I for masked region
Bool NNLSImageSkyModel::maskedZeroI() {
  
  LogMessage message(LogOrigin("NNLSImageSkyModel","maskedZeroI"));
  
  if(isSolveable(0)) {
    
    Int nx=image(0).shape()(0);
    Int ny=image(0).shape()(1);
    Int npol=image(0).shape()(2);
    
    LatticeStepper ls(image(0).shape(), IPosition(4, nx, ny, npol, 1),
		      IPosition(4, 0, 1, 2, 3));
    LatticeIterator<Float> imageli(image(0), ls);
    
    // Get the flux mask?
    ArrayPositionIterator fai(IPosition(4, nx, ny, npol, 1), 0);
    Matrix<Float> lfluxmask;
    if(hasFluxMask(0)) {
      LatticeStepper mls(fluxMask(0).shape(), IPosition(4, nx, ny, npol, 1),
			 IPosition(4, 0, 1, 2, 3));
      RO_LatticeIterator<Float> fluxmaskli(fluxMask(0), mls);
      fluxmaskli.reset();
      if(npol>1) {
	lfluxmask=fluxmaskli.cubeCursor().xyPlane(0); 
      }
      else {
	lfluxmask=fluxmaskli.matrixCursor();
      }
    }
    else {
      lfluxmask.resize(nx,ny);
      lfluxmask=1.0;
    }
    
    Int chan=0;
    for (imageli.reset();!imageli.atEnd();imageli++,chan++) {
      
      for(fai.origin();!fai.pastEnd();fai.next()) {
	if(lfluxmask(fai.pos())>0.000001) {
	  if(npol>1) {
	    imageli.rwCubeCursor().xyPlane(0)(fai.pos())=0.0;
	  }
	  else {
	    imageli.rwMatrixCursor()(fai.pos())=0.0;
	  }
	}
      }
    }
  }
  
  return(true);
};
  

} //# NAMESPACE CASA - END

