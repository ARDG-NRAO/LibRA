//# ClarkCleanAlgorithm.cc: implementation of ClarkCleanAlgorithm.h
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

//# Includes
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/lattices/Lattices/PagedArray.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/lattices/Lattices/PagedArray.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/LEL/LatticeExprNode.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/OS/HostInfo.h>

#include <sstream>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/ClarkCleanLatModel.h>

#include <synthesis/MeasurementComponents/ClarkCleanAlgorithm.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

ClarkCleanAlgorithm::ClarkCleanAlgorithm() : model_sl_p(0), 
  myName("Clark Clean")
{
// Default constructor
//
  cache_p = HostInfo::memoryTotal(true)*1024/(16*16);
};

ClarkCleanAlgorithm::~ClarkCleanAlgorithm() 
{
// Default desctructor
//
  if (model_sl_p) delete model_sl_p;
};

void ClarkCleanAlgorithm::get() 
{
// Get the input data and parameters from the controller
//
  // Get the input parameters and data
  applicator.get(residual_sl);
  applicator.get(psf_sf);
  applicator.get(mask);
  applicator.get(gain);
  applicator.get(threshold);
  applicator.get(numberIterations);
  applicator.get(chan);
  applicator.get(nchan);
  if (model_sl_p) delete model_sl_p;
  model_sl_p = new PagedArray<Float>(TiledShape(residual_sl.shape()));
  model_sl_p->set(0.0f);
  model_sl_p->setMaximumCacheSize(cache_p);
}

void ClarkCleanAlgorithm::put() 
{
// Return the results to the controller
//
  applicator.put(model_sl_p->get());
}

String& ClarkCleanAlgorithm::name()
{
// Return the algorithm name
//
return myName;
};

void ClarkCleanAlgorithm::task()
{
// Do the parallelized part of the Clark CLEAN, acting on
// the local data obtained from the controller.
//
  LogIO os(LogOrigin("task","solve in parallel",WHERE));
  PagedArray<Float> dirty_sl(TiledShape(residual_sl.shape()));
  dirty_sl.setMaximumCacheSize(cache_p);
  dirty_sl.put(residual_sl);
  PagedArray<Float> resid_sl(TiledShape(residual_sl.shape()));
  resid_sl.setMaximumCacheSize(cache_p);
  resid_sl.put(residual_sl);

  ArrayLattice<Float> al_psf_sl(psf_sf);
  Float psfmax;
  {
    LatticeExprNode node = max(al_psf_sl);
    psfmax = node.getFloat();
  }
  if(nchan>1) {
    os<<"Processing channel "<<chan+1<<" of "<<nchan<<LogIO::POST;
  }
  if(psfmax==0.0) {
    os << "No data for this channel: skipping" << LogIO::POST;
  } else {
    LatConvEquation eqn(al_psf_sl, dirty_sl);
    ClarkCleanLatModel cleaner(*model_sl_p);
    ArrayLattice<Float> latMask(mask);
    if (mask.nelements() > 1) {
      cleaner.setMask(latMask);
    };
    
    cleaner.setGain(gain);
    cleaner.setNumberIterations(numberIterations);
    cleaner.setThreshold(threshold);
    cleaner.setPsfPatchSize(IPosition(2,51)); 
    cleaner.setHistLength(1024);
    cleaner.setMaxNumPix(32*1024);
    cleaner.solve(eqn);
    cleaner.setChoose(false);
    os << LogIO::NORMAL    // Loglevel INFO
       << "Clean used " << cleaner.numberIterations() << " iterations" 
       << " to get to a max residual of " << cleaner.threshold() 
       << LogIO::POST;
    // cleaner.getModel(image);
    eqn.residual(resid_sl, cleaner);
  }
};




} //# NAMESPACE CASA - END

