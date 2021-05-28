//# SynthesisDeconvolver.h: Imager functionality sits here; 
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
//#
//# $Id$

#ifndef SYNTHESIS_SYNTHESISDECONVOLVER_H
#define SYNTHESIS_SYNTHESISDECONVOLVER_H

#include <casa/aips.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>

#include<synthesis/ImagerObjects/SDAlgorithmBase.h>
#include<synthesis/ImagerObjects/SDAlgorithmHogbomClean.h>
#include<synthesis/ImagerObjects/SDAlgorithmClarkClean.h>
#include<synthesis/ImagerObjects/SDAlgorithmClarkClean2.h>
#include<synthesis/ImagerObjects/SDAlgorithmMSMFS.h>
#include<synthesis/ImagerObjects/SDAlgorithmMSClean.h>
#include<synthesis/ImagerObjects/SDAlgorithmMEM.h>
#include<synthesis/ImagerObjects/SDAlgorithmAAspClean.h>

#include<synthesis/ImagerObjects/SDMaskHandler.h>
#include <synthesis/ImagerObjects/SIMinorCycleController.h>

#include<synthesis/ImagerObjects/SynthesisUtilMethods.h>

namespace casacore{

class MeasurementSet;
template<class T> class ImageInterface;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations

// <summary> Class that contains functions needed for imager </summary>

class SynthesisDeconvolver 
{
 public:
  // Default constructor

  SynthesisDeconvolver();
  ~SynthesisDeconvolver();

  // Copy constructor and assignment operator

  // make all pure-inputs const

  void setupDeconvolution(const SynthesisParamsDeconv& decpars);

  //  void setupDeconvolution(casacore::Record recpars);

  casacore::Record initMinorCycle();
  casacore::Record initMinorCycle(std::shared_ptr<SIImageStore> imstor); 
  casacore::Record executeMinorCycle(casacore::Record& subIterBot);
  casacore::Record executeCoreMinorCycle(casacore::Record& subIterBot);
  //minor cycle for cubes
  //doDeconvAndAutoMask=1 //do automask without deconv
  //doDeconvAndAutoMask=0 //do deconv no automask
  //doDeconvAndAutoMask=-1 //do automask then deconv
  casacore::Record executeCubeMinorCycle(casacore::Record& minorCycleControlRec, const casacore::Int doDeconvAndAutomask=-1);

  casacore::Record interactiveGUI(casacore::Record& iterRec);

  // Helpers
  /*
  casacore::Float getPeakResidual();
  casacore::Float getModelFlux();
  casacore::Float getPSFSidelobeLevel();
  */
  // Restoration (and post-restoration PB-correction)
  void restore();
  void pbcor();// maybe add a way to take in arbitrary PBs here.

  // For interaction
  void getCopyOfResidualAndMask( casacore::TempImage<casacore::Float> &/*residual*/, casacore::TempImage<casacore::Float>& /*mask*/ );
  void setMask( casacore::TempImage<casacore::Float> &/*mask*/ );

  void setStartingModel();
  casacore::Bool setupMask();
  void setAutoMask();
  void checkRestoringBeam();
  ///in case one wants this deconvolver object to start from where another deconvolver left.
  void setIterDone(const casacore::Int iterdone);
  ////set the posmask image that is used in automasking...needed for restarting deconvolver
  void setPosMask(std::shared_ptr<casacore::ImageInterface<casacore::Float> > posmaskim);
  ////return estimate of memory usage in kB
  casacore::Long estimateRAM(const std::vector<int>& imsize);
  ///automask parameters that can be needed for cubes
  void setChanFlag(const casacore::Vector<casacore::Bool>& chanflag);
  casacore::Vector<casacore::Bool> getChanFlag();
  void setRobustStats(const casacore::Record& rec);
  casacore::Record getRobustStats();
  void setMinorCycleControl(const casacore::Record& minorCycleControlRec);
protected:

  std::shared_ptr<SIImageStore> makeImageStore( casacore::String imagename );
  //Merge the outputRecord from channels into one that looks like the cube one
  void mergeReturnRecord(const casacore::Record& chanRec, casacore::Record& outRec, const casacore::Int chan);
  casacore::Record getSubsetRobustStats(const casacore::Int chanBeg, const casacore::Int chanEnd);
  void setSubsetRobustStats(const casacore::Record& inrec, const casacore::Int chanBeg, const casacore::Int chanEnd, const casacore::Int numchan);
  //for parallel cube partition in block of channels to reduce lock load on model image
  casacore::Int numblockchans(casacore::Vector<casacore::Int>& startch, casacore::Vector<casacore::Int>& endch); 
  /*
  void findMinMax(const casacore::Array<casacore::Float>& lattice,
					const casacore::Array<casacore::Float>& mask,
					casacore::Float& minVal, casacore::Float& maxVal,
					casacore::Float& minValMask, casacore::Float& maxValMask);

  void printImageStats();
  */

  // Gather all part images to the 'full' one
  //void gatherImages();
  //void scatterModel();


  // For the deconvolver, decide how many sliced deconvolution calls to make
  //  casacore::Vector<casacore::Slicer> partitionImages();

  // Check if images exist on disk and are all the same shape
  //casacore::Bool setupImagesOnDisk();
  // casacore::Bool doImagesExist( casacore::String imagename );

  /////////////// Member Objects

  std::shared_ptr<SDAlgorithmBase> itsDeconvolver;
  std::shared_ptr<SDMaskHandler> itsMaskHandler;

  std::shared_ptr<SIImageStore> itsImages;

  casacore::IPosition itsImageShape;
  
  casacore::String itsImageName;
  casacore::Vector<casacore::String> itsStartingModelNames;
  casacore::Bool itsAddedModel;


  casacore::Float itsBeam;

  SIMinorCycleController itsLoopController;

  /////////////// All input parameters

  casacore::uInt itsDeconvolverId;
  casacore::Vector<casacore::Float> itsScales;

  casacore::String itsUseBeam;

  ///// for mask
  casacore::String itsMaskType;
  casacore::Vector<casacore::String> itsMaskList;
  casacore::String itsMaskString;
  casacore::Float itsPBMask;
  casacore::String itsAutoMaskAlgorithm;
  casacore::String itsMaskThreshold;
  casacore::Float itsFracOfPeak;
  casacore::String itsMaskResolution;
  casacore::Float itsMaskResByBeam;
  casacore::Int itsNMask;
  casacore::Bool itsAutoAdjust;
  
  //// for new automasking algorithm
  casacore::Float itsSidelobeThreshold;
  casacore::Float itsNoiseThreshold;
  casacore::Float itsLowNoiseThreshold;
  casacore::Float itsNegativeThreshold;
  casacore::Float itsSmoothFactor;
  casacore::Float itsMinBeamFrac;
  casacore::Float itsCutThreshold;
  casacore::Int itsIterDone;
  casacore::Int itsGrowIterations;
  casacore::Bool itsDoGrowPrune;
  casacore::Float  itsMinPercentChange;
  casacore::Bool itsVerbose;  
  casacore::Bool itsFastNoise;  
  casacore::Vector<casacore::Bool> itsChanFlag;
  casacore::Record itsRobustStats;
  casacore::Bool initializeChanMaskFlag; 
  std::shared_ptr<casacore::ImageInterface<casacore::Float> > itsPosMask;
  
  casacore::Bool itsIsMaskLoaded; // Try to get rid of this state variable ! 
  casacore::Bool itsIsInteractive;

  casacore::Float itsMaskSum;

  casacore::Float itsNsigma;
  SynthesisParamsDeconv itsDecPars;
  casacore::Float itsPreviousFutureRes;
  casacore::Record itsPreviousIterBotRec_p;
};


} //# NAMESPACE CASA - END

#endif
