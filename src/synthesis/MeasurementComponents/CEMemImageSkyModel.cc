//# CEMemImageSkyModel.cc: Implementation of CEMemImageSkyModel class
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <casacore/casa/Arrays/ArrayMath.h>
#include <synthesis/MeasurementComponents/CEMemImageSkyModel.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/OS/File.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/LEL/LatticeExprNode.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/lattices/Lattices/SubLattice.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/IncCEMemModel.h>
#include <synthesis/MeasurementEquations/CEMemProgress.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

CEMemImageSkyModel::
CEMemImageSkyModel(Float sigma, 
		   Float targetFlux,
		   Bool constrainFlux,
		   const Vector<String>& priors,
		   const String& entropy)
 : 
  itsSigma(sigma),
  itsTargetFlux(targetFlux),
  itsConstrainFlux(constrainFlux),
  itsPrior(priors),
  itsEntropy(entropy),
  itsInitializeModel(true),
  itsProgress(0)
{
};

CEMemImageSkyModel::~CEMemImageSkyModel()
{
}

// Mem solver
Bool CEMemImageSkyModel::solve(SkyEquation& se) {

  LogIO os(LogOrigin("CEMemImageSkyModel","solve",WHERE));
  

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
  
  PagedImage<Float>* priorImagePtr;
  priorImagePtr=0;
  if(itsPrior.nelements()>0) {
    if(Table::isReadable(itsPrior(0)))
      priorImagePtr=new PagedImage<Float>(itsPrior(0));
  }

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
    if((xend - xbeg)>nx/2) {
      xbeg=nx/4-1; //if larger than quarter take inner of mask
      os << LogIO::WARN << "Mask span over more than half the x-axis: Considering inner half of the x-axis" << LogIO::POST;
    } 
    if((yend - ybeg)>ny/2) { 
      ybeg=ny/4-1;
      os << LogIO::WARN << "Mask span over more than half the y-axis: Considering inner half of the y-axis" << LogIO::POST;
    }  
    xend=min(xend,xbeg+nx/2-1);
    yend=min(yend,ybeg+ny/2-1); 
  }

  // Mask logic used here: one mask for all Stokes, for all Channels
  SubLattice<Float>* mask_sl_p = 0;  
  if (hasMask(0) & (xend > xbeg) && (yend > ybeg) ) {
    LCBox maskbox (IPosition(4, xbeg, ybeg, 0, 0), 
		   IPosition(4, xend, yend, 0, 0), mask(0).shape());
    mask_sl_p = new SubLattice<Float> (mask(0), maskbox, false);
  }


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

    TempLattice<Float> dirty_sl( residual_sl.shape());
    dirty_sl.copyData(residual_sl);

    ResidualEquation<Lattice<Float> > * eqn_p;
    Float psfmax;
    {
      LatticeExprNode node = max(psf_sl);
      psfmax = node.getFloat();
    }
    if(nchan>1) {
      os<<"Processing channel "<<ichan+1<<" of "<<nchan<<LogIO::POST;
    }
    if(psfmax==0.0) {
      os << "No data for this channel: skipping" << LogIO::POST;
    } else {
      eqn_p = new LatConvEquation (psf_sl, dirty_sl);

      IncEntropy * myEnt_p;
      String entString = entropy();
      if(entString=="entropy") {
	os << "Deconvolving image using maximum entropy algorithm"
	   << LogIO::POST;
	myEnt_p = new IncEntropyI;
      }
      else if (entString=="emptiness") {
	myEnt_p = new IncEntropyEmptiness;
      }
      else {
	os << " Known MEM entropies: entropy | emptiness " << LogIO::POST;
	os << LogIO::SEVERE << "Unknown MEM entropy: " << entString
	   << LogIO::POST;
	return false;
      }

      TempLattice<Float> zero (model_sl.shape());
      zero.set(0.0);

      IncCEMemModel memer(*myEnt_p,  zero, model_sl, numberIterations(),
			  sigma(),
			  targetFlux(), constrainFlux(),
			  initializeModel(), false );

      if(priorImagePtr!=0) {
	memer.setPrior(*priorImagePtr);
      }
      if (mask_sl_p != 0 ) {
	memer.setMask( *mask_sl_p );
      }

      if (displayProgress_p) {
	itsProgress = new CEMemProgress( pgplotter_p );
	memer.setProgress(*itsProgress);
      }

      memer.solve(*eqn_p);
      // memer.setChoose(false);  // not yet implemented!
      //      os << "Mem used " << cleaner.numberIterations() << " iterations" 
      //	 << " to get to a max residual of " << cleaner.threshold() 
      //	 << LogIO::POST;

      eqn_p->residual(residual_sl, memer);
      if (itsProgress) {
	delete itsProgress;
      }
    }
  }
  if (priorImagePtr) delete priorImagePtr; priorImagePtr=0;
  if (mask_sl_p)  delete mask_sl_p; mask_sl_p=0;
  return(true);
};


} //# NAMESPACE CASA - END

