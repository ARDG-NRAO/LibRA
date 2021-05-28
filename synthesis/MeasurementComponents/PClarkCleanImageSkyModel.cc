//# PClarkCleanImageSkyModel.cc: Implementation of PClarkCleanImageSkyModel.h
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

#include <casa/Arrays/ArrayMath.h>
#include <synthesis/MeasurementComponents/PClarkCleanImageSkyModel.h>
#include <images/Images/PagedImage.h>
#include <casa/OS/File.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/Lattices/SubLattice.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>

#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/ClarkCleanLatModel.h>
#include <synthesis/MeasurementEquations/ClarkCleanProgress.h>

#include <synthesis/MeasurementComponents/ClarkCleanAlgorithm.h>
#include <synthesis/Parallel/Applicator.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

extern Applicator applicator;

// Clean solver
Bool PClarkCleanImageSkyModel::solve(SkyEquation& se) {

  LogIO os(LogOrigin("PClarkCleanImageSkyModel","solve",WHERE));
  

  if(numberOfModels()>1) {
    os << "Cannot process more than one field" << LogIO::EXCEPTION;
  }

  // Make the residual image
  makeNewtonRaphsonStep(se);
  
  //Make the PSF
  makeApproxPSFs(se);
  
  Int nx=image(0).shape()(0);
  Int ny=image(0).shape()(1);
  Int npol=image(0).shape()(2);
  Int nchan=image(0).shape()(3);
  
  AlwaysAssert((npol==1)||(npol==2)||(npol==4), AipsError);
  
  // Now read the mask and determine the bounding box
  Int xbeg=nx/4;
  Int ybeg=ny/4;
  
  Int xend=xbeg+nx/2-1;
  Int yend=ybeg+ny/2-1;
  
  if(hasMask(0)) {
    AlwaysAssert(mask(0).shape()(0)==nx, AipsError);
    AlwaysAssert(mask(0).shape()(1)==ny, AipsError);

    LatticeStepper mls(mask(0).shape(),
		       IPosition(4, nx, ny, 1, 1),
		       IPosition(4, 0, 1, 3, 2));
    RO_LatticeIterator<Float> maskli(mask(0), mls);
    maskli.reset();
    xbeg=nx-1;
    ybeg=ny-1;
    xend=0;
    yend=0;
    for (Int iy=0;iy<ny;iy++) {
      for (Int ix=0;ix<nx;ix++) {
	if(maskli.matrixCursor()(ix,iy)>0.000001) {
	  xbeg=min(xbeg,ix);
	  ybeg=min(ybeg,iy);
	  xend=max(xend,ix);
	  yend=max(yend,iy);
	}
      }
    }
    // Now have possible BLC. Make sure that we don't go over the
    // edge later
    if(xbeg>nx/2) xbeg=nx/2;
    if(ybeg>ny/2) ybeg=ny/2;
  }
  xend=min(xend,xbeg+nx/2-1);
  yend=min(yend,ybeg+ny/2-1);

  // Mask logic used here: one mask for all Stokes, for all Channels
  SubLattice<Float>* mask_sl_p = 0;  
  if (hasMask(0) & (xend > xbeg) && (yend > ybeg) ) {
    LCBox maskbox (IPosition(4, xbeg, ybeg, 0, 0), 
		   IPosition(4, xend, yend, 0, 0), mask(0).shape());
    mask_sl_p = new SubLattice<Float> (mask(0), maskbox, false);
  }

  // Start of the parallelization (over channel no.)
  Bool rStat=true;
  Array<Float> maskTmp;
  if (mask_sl_p) {
    rStat = mask_sl_p->get(maskTmp);
  }

  os << "Begin parallel clean" << LogIO::NORMAL << LogIO::POST;
  ClarkCleanAlgorithm clarkClean;

  // Track the assignment of channel to process rank
  std::map<Int, Int> chanNo;
  Bool allDone, assigned;
  Int rank;

  for (Int ichan=0; ichan < nchan; ichan++) {
    LCBox imagebox(IPosition(4, xbeg, ybeg, 0, ichan), 
		   IPosition(4, xend, yend, npol-1, ichan),
		   image(0).shape());
    LCBox psfbox(IPosition(4, 0, 0, 0, ichan), 
		 IPosition(4, nx-1, ny-1, 0, ichan),
		 PSF(0).shape());
    
    SubLattice<Float>  psf_sl (PSF(0), psfbox, false);
    SubLattice<Float>  residual_sl (residual(0), imagebox, true);
    SubLattice<Float>  model_sl (image(0), imagebox, true);

    // Assign the next available process to this algorithm
    assigned = applicator.nextAvailProcess(clarkClean, rank);
    while (!assigned) {
      // No free processes; wait for any Clark Clean worker 
      // process to return.
      rank = applicator.nextProcessDone(clarkClean, allDone);
      // Get the resulting plane and insert in the correct place
      Array<Float> af;
      applicator.get(af);
      image(0).putSlice(af, IPosition(4, xbeg, ybeg, 0, chanNo.at(rank)));
      // Assign the next available process
      assigned = applicator.nextAvailProcess(clarkClean, rank);
    };
   
    // Send the current channel to the assigned worker process
    Array<Float> imageTmp;
    rStat = ((Lattice<Float> &)residual_sl).get(imageTmp);
    applicator.put(imageTmp);

    Array<Float> psfTmp;
    rStat = ((Lattice<Float> &)psf_sl).get(psfTmp);
    applicator.put(psfTmp);
    applicator.put(maskTmp);

    applicator.put(gain());
    applicator.put(threshold());
    applicator.put(numberIterations());
    applicator.put(ichan);
    applicator.put(nchan);

    // Record the assignment of channel to process rank
    chanNo.insert( std::pair<Int, Int>(rank, ichan) );

    // Execute the algorithm
    applicator.apply(clarkClean);
  };

  // Wait for all outstanding processes to return
  rank = applicator.nextProcessDone(clarkClean, allDone);
  while (!allDone) {
    // Get the resulting plane and insert in the correct place
    Array<Float> af;
    applicator.get(af);
    image(0).putSlice(af, IPosition(4, xbeg, ybeg, 0, chanNo.at(rank)));
    // Wait for the next process to complete
    rank = applicator.nextProcessDone(clarkClean, allDone);
  };

  (void)rStat; // avoid compiler warning
  if (mask_sl_p != 0 )  delete mask_sl_p;
  os << "End parallel clean" << LogIO::NORMAL << LogIO::POST;
  return(rStat);
};




} //# NAMESPACE CASA - END

