//# WBCleanImageSkyModel.h: Definition for WBCleanImageSkyModel
//# Copyright (C) 1996,1997,1998,1999,2000
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

#ifndef SYNTHESIS_WBCLEANIMAGESKYMODEL_H
#define SYNTHESIS_WBCLEANIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/CleanImageSkyModel.h>
#include <casacore/lattices/LatticeMath/LatticeCleanProgress.h>
#include <casacore/lattices/LEL/LatticeExprNode.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <synthesis/MeasurementEquations/MultiTermMatrixCleaner.h>
#include <casacore/casa/OS/Timer.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//forward
class SkyEquation;

// <summary> 
// WB Clean Image Sky Model: Image Sky Model implementing a Wide-Band
// multi frequency synthesis algorithm
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=casacore::LatticeCleaner>LatticeCleaner</linkto> module
//   <li> <linkto class=ImageSkyModel>ImageSkyModel</linkto> module
//   <li> <linkto class=LinearModel>LinearModel</linkto> module
// </prerequisite>
//
// <etymology>
// WBCleanImageSkyModel implements the Wide Band Clean algorithm.
// It is derived from <linkto class=SkyModel>SkyModel</linkto>.
// </etymology>
//
// <synopsis> 
// The WB Clean is the multi-frequency synthesis deconvolution
// algorithm. It decomposes an image into a linear compbination
// of models convolved with spectral dirty beams of various
// order. A multiscale variant can be invoked by supplying a
// user vector of scale sizes. Default is 1, corresponding to a
// scale insensitive mfs deconvolution.
//
// Masking is optionally performed using a mask image: only points
// where the mask is non-zero are searched for Gaussian components. 
// This can cause some difficulty, as the different Gaussian scale
// sizes will extend beyond the mask by different amounts.
// If no mask is specified
// all points in the inner quarter of the image are cleaned.
// </synopsis> 
//
// <example>
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="99/04/07">
// <ul> Improve the masking.
// </todo>

class WBCleanImageSkyModel : public CleanImageSkyModel {
public:

  // Create a WBCleanImageSkyModel - default scale size = 1 pixel
  WBCleanImageSkyModel();
  WBCleanImageSkyModel(const casacore::Int ntaylor,const casacore::Int nscales,const casacore::Double reffreq);
  WBCleanImageSkyModel(const casacore::Int ntaylor,const casacore::Vector<casacore::Float>& userScaleSizes, const casacore::Double reffreq);

  // destructor
  ~WBCleanImageSkyModel();

  // Solve for this SkyModel
  casacore::Bool solve (SkyEquation& se);
  //  casacore::Bool copyLatToImInt(casacore::TempLattice<casacore::Float>& lat, casacore::ImageInterface<casacore::Float>& im);
  // casacore::Bool copyImIntToLat(casacore::TempLattice<casacore::Float>& lat, casacore::ImageInterface<casacore::Float>& im);
  
//  casacore::Int nmodels_p; // Number of image models = nfields * ntaylor
  casacore::Int ntaylor_p; // Number of terms in the Taylor expansion to use.
//  casacore::Int nfields_p; // Number of image fields/pointings.
  casacore::Int nscales_p; // Number of scales to use for the multiscale part.
  
  casacore::Double refFrequency_p;

//   casacore::Int add(casacore::ImageInterface<casacore::Float>& iimage, const casacore::Int maxNumXfr=100);
//   casacore::Bool addResidual(casacore::Int thismodel, casacore::ImageInterface<casacore::Float>& iresidual);
//   void initializeGradients();
   casacore::Bool solveResiduals(SkyEquation& se, casacore::Bool modelToMS=false);
   casacore::Bool makeNewtonRaphsonStep(SkyEquation& se, casacore::Bool incremental=false, casacore::Bool modelToMS=false);

   casacore::Int numberOfTaylorTerms(){return ntaylor_p;};
   casacore::Double getReferenceFrequency(){return refFrequency_p;};

   // Major axis for ordering : Taylor
   casacore::Int getModelIndex(casacore::uInt model, casacore::uInt taylor){return taylor * (nfields_p) + (model);};
   casacore::Int getTaylorIndex(casacore::uInt index){return casacore::Int(index/nfields_p);};
   casacore::Int getFieldIndex(casacore::uInt index){return index%nfields_p;};

  casacore::Bool calculateCoeffResiduals();
  casacore::Bool calculateAlphaBeta(const casacore::Vector<casacore::String> &restoredNames, 
			               const casacore::Vector<casacore::String> &residualNames);

   // Major axis for ordering : Models
   //inline casacore::Int getModelIndex(casacore::uInt model, casacore::uInt taylor){return model * (ntaylor_p) + (taylor);};
   //inline casacore::Int getPSFModelIndex(casacore::uInt model, casacore::uInt taylor){return model * (2*ntaylor_p-1) + (taylor);};
   //inline casacore::Int getTaylorIndex(casacore::uInt index){return index%ntaylor_p;};
   //inline casacore::Int getFieldIndex(casacore::uInt index){return index/ntaylor_p;};
 
   casacore::Vector<casacore::String> imageNames;
   
private:

  //  casacore::PtrBlock<casacore::MultiTermLatticeCleaner<casacore::Float>* > lc_p;
  casacore::Block<MultiTermMatrixCleaner> lc_p;
   
  casacore::Vector<casacore::Float> scaleSizes_p; // casacore::Vector of scale sizes in pixels.
  casacore::Vector<casacore::Float> scaleBias_p; // casacore::Vector of scale biases !!
  //casacore::Float maxPsf_p;

  casacore::IPosition gip,imshape;
  casacore::Bool donePSF_p;
  casacore::Bool doneMTMCinit_p;
  casacore::Int nx,ny;

  casacore::Int numbermajorcycles_p;
  casacore::Float previous_maxresidual_p;
  
  // casacore::Memory to be allocated per TempLattice
  casacore::Double memoryMB_p;
  
  casacore::LogIO os;
  
  void initVars();
  casacore::Bool checkParameters();

  casacore::Int storeAsImg(casacore::String fileName, casacore::ImageInterface<casacore::Float>& theImg);
  //casacore::Int storeTLAsImg(casacore::String fileName, casacore::TempLattice<casacore::Float> &TL, casacore::ImageInterface<casacore::Float>& theImg);
  //casacore::Int storeTLAsImg(casacore::String fileName, casacore::TempLattice<casacore::Complex> &TL, casacore::ImageInterface<casacore::Float>& theImg);

  casacore::Bool mergeDataError(casacore::ImageInterface<casacore::Float> &data, casacore::ImageInterface<casacore::Float> &error, const casacore::String &outImg);

  casacore::Bool createMask(casacore::LatticeExpr<casacore::Bool> &lemask, casacore::ImageInterface<casacore::Float> &outimage);

  casacore::Bool resizeWorkArrays(casacore::Int length);
  
  casacore::Int makeSpectralPSFs(SkyEquation& se, casacore::Bool writeToDisk);
   //casacore::Int addTo(casacore::Lattice<casacore::Float>& to, const casacore::Lattice<casacore::Float>& add, casacore::Float multiplier);
  casacore::Int writeResultsToDisk();
  casacore::Float computeFluxLimit(casacore::Float &fractionOfPsf);

  void blankOverlappingModels();
  void restoreOverlappingModels();

  void saveCurrentModels();
  
  casacore::Timer tmr1,tmr2;
  casacore::Int adbg;
  casacore::Int tdbg;
  casacore::Int ddbg;
  // Put in some progress metre here...
  
  
};


} //# NAMESPACE CASA - END

#endif


