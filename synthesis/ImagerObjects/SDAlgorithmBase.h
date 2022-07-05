//# SDAlgorithmBase.h: Definition for SDAlgorithmBase
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

#ifndef SYNTHESIS_SDALGORITHMBASE_H
#define SYNTHESIS_SDALGORITHMBASE_H

#include <ms/MeasurementSets/MeasurementSet.h>
//#include <synthesis/MeasurementComponents/SkyModel.h>
#include <casa/Arrays/Matrix.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/System/PGPlotter.h>

#include <casa/aips.h>
#include <images/Images/ImageInterface.h>
#include <components/ComponentModels/ComponentList.h>
#include <casa/BasicSL/String.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>

#include<synthesis/ImagerObjects/SDMaskHandler.h>
#include<synthesis/ImagerObjects/SIImageStore.h>
#include<synthesis/ImagerObjects/SIImageStoreMultiTerm.h>

// supporting code for ContextBoundBool
#include <mutex>

namespace casa { //# NAMESPACE CASA - BEGIN

  /* Forware Declaration */
  class SIMinorCycleController;

/**
 * Specialty bool that is guaranteed to be toggled (stop method)
 * when the context closes via RAII (Resource Acquisition Is Initialization).
 * 
 * Useful for telling a thread to stop executing, even if the calling context
 * stops unexpectedly or exceptions out of existance.
 */
class ContextBoundBool
{
public:
  ContextBoundBool(bool startValue);
  bool getVal();
  void stop();
  ~ContextBoundBool();

  bool itsVal;
  bool itsInitialValue;
  std::mutex itsMutex;
};

class SDAlgorithmBase {
public:

  // Empty constructor
  SDAlgorithmBase();
 virtual  ~SDAlgorithmBase();

  // Non virtual. Wrapper function implemented only in the base class. 
  void deconvolve( SIMinorCycleController& loopController,  
		   std::shared_ptr<SIImageStore> &imagestore,
		   casacore::Int deconvolverid, 
                   casacore::Bool isautomasking=false, 
                   //casacore::Bool fastnoise=true);
                   casacore::Bool fastnoise=true,
                   casacore::Record robuststats=casacore::Record());

  void setRestoringBeam( casacore::GaussianBeam restbeam, casacore::String usebeam );
  //  void setMaskOptions( casacore::String maskstring );

  // Base Class contains standard restoration. Overload for more complex behaviour.
  virtual void restore( std::shared_ptr<SIImageStore> imagestore );
  virtual void pbcor( std::shared_ptr<SIImageStore> imagestore );

  virtual casacore::String getAlgorithmName(){return itsAlgorithmName;};

  virtual casacore::uInt getNTaylorTerms(){return 1;};
  ///returns the estimate of memory used in kilobytes (kB);
  virtual casacore::Long estimateRAM(const std::vector<int>& imsize);
protected:

  // Pure virtual functions to be implemented by various algorithm deconvolvers.
  virtual void takeOneStep( casacore::Float loopgain, casacore::Int cycleNiter, casacore::Float cycleThreshold, 
			    casacore::Float &peakresidual, casacore::Float &modelflux, casacore::Int& iterdone )=0;
  //  virtual void initializeDeconvolver( casacore::Float &peakresidual, casacore::Float &modelflux )=0;
  virtual void initializeDeconvolver()=0;
  virtual void finalizeDeconvolver()=0;

  // Base Class implements the option of single-plane images for the minor cycle.
  virtual void queryDesiredShape(casacore::Int &nchanchunks, casacore::Int& npolchunks, casacore::IPosition imshape);


  // Non virtual. Implemented only in the base class.
  casacore::Int checkStop( SIMinorCycleController &loopcontrols, casacore::Float currentresidual );
  casacore::Bool findMaxAbs(const casacore::Array<casacore::Float>& lattice,casacore::Float& maxAbs,casacore::IPosition& posMaxAbs);
  casacore::Bool findMaxAbsMask(const casacore::Array<casacore::Float>& lattice,const casacore::Array<casacore::Float>& mask,
		      casacore::Float& maxAbs,casacore::IPosition& posMaxAbs);

  // sample memory usage for profiling
  void profileMinorCycle(ContextBoundBool &stop, casacore::uLong &peakMem);

  // Algorithm name
  casacore::String itsAlgorithmName;

  std::shared_ptr<SIImageStore> itsImages; //sOriginalImages;

  //    casacore::Vector<casacore::Slicer> itsDecSlices;
  //   casacore::SubImage<casacore::Float> itsResidual, itsPsf, itsModel, itsImage;
  
  casacore::IPosition itsMaxPos;
  casacore::Float itsPeakResidual;
  casacore::Float itsModelFlux;

  SDMaskHandler itsMaskHandler;
  //casacore::Array<casacore::Float> itsMatMask;

  casacore::GaussianBeam itsRestoringBeam;
  casacore::String itsUseBeam;
  //  casacore::String itsMaskString;
  //  casacore::Bool itsIsMaskLoaded; // Annoying state variable. Remove if possible. 

};

} //# NAMESPACE CASA - END

#endif
