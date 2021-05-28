//# FTMachine.h: Definition for FTMachine
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_FTMACHINE_H
#define SYNTHESIS_FTMACHINE_H

#include <measures/Measures/Measure.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Containers/RecordInterface.h>
#include <casa/Containers/Block.h>
#include <images/Images/TempImage.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <scimath/Mathematics/InterpolateArray1D.h>
#include <synthesis/TransformMachines/CFCache.h>
#include <synthesis/TransformMachines/CFStore2.h>
#include <synthesis/Utilities/FFT2D.h>
#include <synthesis/TransformMachines/ConvolutionFunction.h>
#include <synthesis/TransformMachines/PolOuterProduct.h>

#include <images/Images/ImageInterface.h>
#include <images/Images/SubImage.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>

#include <synthesis/ImagerObjects/SIImageStore.h>
#include <synthesis/ImagerObjects/SIImageStoreMultiTerm.h>
#include <iomanip>

namespace casacore{

  class UVWMachine;
}

namespace casa { //# NAMESPACE CASA - BEGIN

  class VisSet;
  class VisBuffer;
  class ROVisibilityIterator;
  class VisModelData;
  class SkyJones;

// <summary> defines interface for the Fourier Transform Machine </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=SkyModel>SkyModel</linkto> module
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> module
//   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
// </prerequisite>
//
// <etymology>
// FTMachine is a Machine for Fourier Transforms
// </etymology>
//
// <synopsis> 
// The <linkto class=SkyEquation>SkyEquation</linkto> needs to be able
// to perform Fourier transforms on visibility data. FTMachine
// allows efficient Fourier Transform processing using a 
// <linkto class=VisBuffer>VisBuffer</linkto> which encapsulates
// a chunk of visibility (typically all baselines for one time)
// together with all the information needed for processing
// (e.g. UVW coordinates).
// </synopsis> 
//
// <example>
// A simple example of a FTMachine is found in 
// <linkto class=GridFT>GridFT</linkto>.
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// Define an interface to allow efficient processing of chunks of 
// visibility data
//
// Note that the image must be Complex. It must contain the
// casacore::Complex casacore::Stokes values (e.g. RR,RL,LR,LL). FTMachine
// uses the image coordinate system to determine mappings
// between the polarization and frequency values in the
// casacore::PagedImage and in the VisBuffer.
//
// </motivation>
//
// <todo asof="97/10/01">
// </todo>

class FTMachine {
public:

  //# Enumerations
  // Types of known Images that may be made using the makeImage method 
  enum Type {
    OBSERVED=0,		// From OBSERVED visibility data (default)
    MODEL,		// From MODEL visibility data
    CORRECTED,		// From CORRECTED visibility data
    RESIDUAL,		// From RESIDUAL (OBSERVED-MODEL) visibility data
    PSF,		// POINT SPREAD FUNCTION
    COVERAGE,		// COVERAGE (SD only)
    N_types,		// Number of types
    DEFAULT=OBSERVED
  };

  FTMachine();


  FTMachine(casacore::CountedPtr<CFCache>& cfcache,casacore::CountedPtr<ConvolutionFunction>& cfctor);

  FTMachine(const FTMachine& other);

  FTMachine& operator=(const FTMachine& other);

  void setBasePrivates(const FTMachine& other){FTMachine::operator=(other);}

  virtual ~FTMachine();
  

  //clone copy
  //the default cloner clones via a Record copy
  virtual FTMachine* cloneFTM();
  // Initialize transform to Visibility plane
  virtual void initializeToVis(casacore::ImageInterface<casacore::Complex>& image, const VisBuffer& vb) = 0;

  // Vectorized InitializeToVis
  virtual void initializeToVis(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec,
			       casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & modelImageVec, 
			       casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& weightImageVec, 
			       casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& fluxScaleVec, 
			       casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec,
			       const VisBuffer& vb);

  virtual void initializeToVisNew(const VisBuffer& vb,
					     casacore::CountedPtr<SIImageStore> imstore);

  //-------------------------------------------------------------------------------------
  // Finalize transform to Visibility plane
  // This is mostly a no-op, and is not-even called from CubeSkyEquation.
  virtual void finalizeToVis() = 0;

  // Note : No vectorized form of finalizeToVis yet.....

  //-------------------------------------------------------------------------------------
  // Initialize transform to Sky plane
  virtual void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,
			       casacore::Matrix<casacore::Float>& weight, const VisBuffer& vb) = 0;

  // Vectorized InitializeToSky
  virtual void initializeToSky(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec,
			       casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec, 
			       const VisBuffer& vb, 
			       const casacore::Bool dopsf=false);

  virtual void initializeToSkyNew(const casacore::Bool dopsf, 
				  const VisBuffer& vb, 
				  casacore::CountedPtr<SIImageStore> imstore);

  //-------------------------------------------------------------------------------------
  // Finalize transform to Sky plane
  virtual void finalizeToSky() = 0;

  virtual void finalizeToSky(casacore::ImageInterface<casacore::Complex>& iimage){(void)iimage;};

  // Vectorized finalizeToSky
  virtual void finalizeToSky(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec, 
			     casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & resImageVec, 
			     casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& weightImageVec, 
			     casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& fluxScaleVec, 
			     casacore::Bool dopsf, 
			     casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec, const VisBuffer& vb);

  virtual void finalizeToSkyNew(casacore::Bool dopsf, 
					   const VisBuffer& vb,
					   casacore::CountedPtr<SIImageStore> imstore  );

  //-------------------------------------------------------------------------------------

  // Get actual coherence from grid
  virtual void get(VisBuffer& vb, casacore::Int row=-1) = 0;


  // Put coherence to grid
  virtual void put(const VisBuffer& vb, casacore::Int row=-1, casacore::Bool dopsf=false, 
		   FTMachine::Type type= FTMachine::OBSERVED)=0;

  // Non const vb version - so that weights can be modified in-place
  // Currently, used only by MultiTermFT
  virtual void put(VisBuffer& vb, casacore::Int row=-1, casacore::Bool dopsf=false, 
  	           FTMachine::Type type= FTMachine::OBSERVED)
                    {put((const VisBuffer&)vb,row,dopsf,type);};

  //-------------------------------------------------------------------------------------
  virtual void correlationToStokes(casacore::ImageInterface<casacore::Complex>& compImage, 
				   casacore::ImageInterface<casacore::Float>& resImage, 
				   const casacore::Bool dopsf);
 
  virtual void stokesToCorrelation(casacore::ImageInterface<casacore::Float>& modelImage,
				   casacore::ImageInterface<casacore::Complex>& compImage);

  /*
  virtual void normalizeSumWeight(casacore::ImageInterface<casacore::Float>& inOutImage, 
			       casacore::ImageInterface<casacore::Float>& weightImage, 
			       const casacore::Bool dopsf);
  */

  virtual void normalizeImage(casacore::Lattice<casacore::Complex>&,//skyImage,
			      const casacore::Matrix<casacore::Double>&,// sumOfWts,
			      casacore::Lattice<casacore::Float>&,// sensitivityImage,
			      casacore::Bool /*fftNorm*/){return;};

  virtual void normalizeImage(casacore::ImageInterface<casacore::Float>& skyImage,
			      casacore::Matrix<casacore::Float>& sumOfWts,
			      casacore::ImageInterface<casacore::Float>& sensitivityImage,
			      casacore::Bool dopsf, casacore::Float pblimit, casacore::Int normtype);


  // All FTMachines that fill weightimage, need to set this.
  // TODO : Make this pure virtual.
  virtual casacore::Bool useWeightImage(){return false;}; 
  virtual casacore::Bool isSkyJonesSet(){return (sj_p.nelements()>0) && !(sj_p[0]).null()  ;}
  virtual casacore::Bool isSkyJonesChanged(VisBuffer& vb, casacore::Int row){if(sj_p.nelements()>0){return sj_p[0]->changed(vb,row);} else {return false;} };

  // Set SkyJones if image domain corrections /applycation are needed
  // To reset the the FTMachine for stopping image based correction/applycation
  // set in a casacore::Vector of size 0.
  // The pointers have to be handled by the caller ..no delete happening here
  virtual void setSkyJones(casacore::Vector<casacore::CountedPtr<SkyJones> >& sj);
  
  casacore::Bool changedSkyJonesLogic(const VisBuffer& vb, casacore::Bool& firstRow, casacore::Bool& internalRow);

 
  //-------------------------------------------------------------------------------------

  // Get the gridded visibilities or weight 
  template <typename T> void getGrid(casacore::Array<T>& thegrid);
   // Get the final image
  virtual casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true) = 0;

  virtual const casacore::CountedPtr<ConvolutionFunction>& getAWConvFunc() {return convFuncCtor_p;};

  virtual void findConvFunction(const casacore::ImageInterface<casacore::Complex>&,// image,
				const VisBuffer& /*vb*/) {};
  // Get the final weights image
  virtual void getWeightImage(casacore::ImageInterface<casacore::Float>& weightImage, casacore::Matrix<casacore::Float>& weights) = 0;

  // Get a flux (divide by this to get a flux density correct image) 
  // image if there is one
  virtual void getFluxImage(casacore::ImageInterface<casacore::Float>& image){(void)image;};

  // Make the entire image
  virtual void makeImage(FTMachine::Type type,
			 VisSet& vs,
			 casacore::ImageInterface<casacore::Complex>& image,
			 casacore::Matrix<casacore::Float>& weight);
  // Make the entire image using a ROVisIter
  virtual void makeImage(FTMachine::Type type,
			 ROVisibilityIterator& vi,
			 casacore::ImageInterface<casacore::Complex>& image,
			 casacore::Matrix<casacore::Float>& weight);

  //-------------------------------------------------------------------------------------

  // Rotate the uvw from the observed phase center to the
  // desired phase center.
  void girarUVW(casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Double>& dphase,
   		const VisBuffer& vb);
  void rotateUVW(casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Double>& dphase,
		 const VisBuffer& vb);

  // Refocus on a finite distance
  void refocus(casacore::Matrix<casacore::Double>& uvw, const casacore::Vector<casacore::Int>& ant1,
	       const casacore::Vector<casacore::Int>& ant2,
	       casacore::Vector<casacore::Double>& dphase, const VisBuffer& vb);

  //helper function for openmp to call ...no private dependency
  static void locateuvw(const casacore::Double*& uvw, const casacore::Double*&dphase, const casacore::Double*& freq, const casacore::Int& nchan, const casacore::Double*& scale, const casacore::Double*& offset,  const casacore::Int& sampling, casacore::Int*& loc,casacore::Int*& off, casacore::Complex*& phasor, const casacore::Int& row, const casacore::Bool& doW=false); 
		 

  // Save and restore the FTMachine to and from a record
  virtual casacore::Bool toRecord(casacore::String& error, casacore::RecordInterface& outRecord, 
			casacore::Bool withImage=false, const casacore::String diskimagename="");
  virtual casacore::Bool fromRecord(casacore::String& error, const casacore::RecordInterface& inRecord);

  // Has this operator changed since the last application?
  virtual casacore::Bool changed(const VisBuffer& vb);

  // Can this FTMachine be represented by Fourier convolutions?
  virtual casacore::Bool isFourier() {return false;}

  //set  spw for cube that will be used;
  casacore::Bool setSpw(casacore::Vector<casacore::Int>& spw, casacore::Bool validFrame);

  //return whether the ftmachine is using a double precision grid
  virtual casacore::Bool doublePrecGrid();

  // To make sure no padding is used in certain gridders
  virtual void setNoPadding(casacore::Bool nopad){(void)nopad;};
  
  // Return the name of the machine

  virtual casacore::String name() const =0;// { return "None";};
 
  // set and get the location used for frame 
  void setLocation(const casacore::MPosition& loc);
  casacore::MPosition& getLocation();

  // set a moving source aka planets or comets =>  adjust phase center
  // on the fly for gridding 
  virtual void setMovingSource(const casacore::String& sourcename);
  virtual void setMovingSource(const casacore::MDirection& mdir);

  //set and get Time to calculate phasecenter  -1.0 means using the time available at 
  //each iteration..this is used when the phasecenter in the field table is either 
  //a polynomial or has a ephemerides tables associated with it
  //Using double in the units and epoch-frame of the ms(s) ..caller is responsible for conversion
  void setPhaseCenterTime(const casacore::Double time){phaseCenterTime_p=time;};
  casacore::Double getPhaseCenterTime(){return phaseCenterTime_p;};
  //reset stuff in an FTMachine
  virtual void reset(){};

  //set frequency interpolation type
  virtual void setFreqInterpolation(const casacore::String& method);

  //tell ftmachine which Pointing table column to use for Direction
  //Mosaic or Single dish ft use this for example
  virtual void setPointingDirColumn(const casacore::String& column="DIRECTION");

  virtual casacore::String getPointingDirColumnInUse();

  virtual void setSpwChanSelection(const casacore::Cube<casacore::Int>& spwchansels);
  virtual casacore::Cube<casacore::Int> getSpwChanSelection(){return spwChanSelFlag_p;};
  virtual void setSpwFreqSelection(const casacore::Matrix<casacore::Double>& spwfreqs);
  virtual casacore::Matrix<casacore::Double> getSpwFreqSelection() {return spwFreqSel_p;};

  // set the order of the Taylor term for MFS this is to tell
  // A-casacore::Projection to qualify the accumulated avgPB for each Taylor
  // term in the CFCache.
  virtual void setMiscInfo(const casacore::Int qualifier)=0;

  virtual void setCanComputeResiduals(casacore::Bool& b) {canComputeResiduals_p=b;};
  virtual casacore::Bool canComputeResiduals() {return canComputeResiduals_p;};
  //
  // Make the VB and VBStore interefaces for the interim re-factoring
  // work.  Finally removed the VB interface.
  virtual void ComputeResiduals(VisBuffer&vb, casacore::Bool useCorrected) = 0;
  virtual casacore::Float getPBLimit() {return pbLimit_p;};
  //virtual void ComputeResiduals(VBStore& vb)=0;
  //get and set numthreads
  void setnumthreads(casacore::Int n);
  casacore::Int getnumthreads();

  virtual void setCFCache(casacore::CountedPtr<CFCache>& cfc, const casacore::Bool resetCFC=true);
  casacore::CountedPtr<CFCache> getCFCache() {return cfCache_p;};
  casacore::String getCacheDir() { return cfCache_p->getCacheDir(); };

  virtual void setDryRun(casacore::Bool val) 
  {
    isDryRun=val;
    //cerr << "FTM: " << isDryRun << endl;
  };
  virtual casacore::Bool dryRun() {return isDryRun;}
  virtual casacore::Bool isUsingCFCache() 
  {
    // cerr << "@#%$@% = " << cfCache_p.nrefs() << endl;
    return (cfCache_p.nrefs()!=0);
  }
  casacore::Bool isDryRun;

protected:

  friend class VisModelData;
  friend class MultiTermFT;
  friend class MultiTermFTNew;
  casacore::LogIO logIO_p;

  casacore::LogIO& logIO();

  casacore::ImageInterface<casacore::Complex>* image;

  casacore::UVWMachine* uvwMachine_p;
  casacore::CountedPtr<casacore::UVWMachine> phaseShifter_p;

  casacore::MeasFrame mFrame_p;

  // Direction of desired tangent plane
  casacore::Bool tangentSpecified_p;
  casacore::MDirection mTangent_p;

  casacore::MDirection mImage_p;

  // moving source stuff
  casacore::MDirection movingDir_p;
  casacore::Bool fixMovingSource_p;
  casacore::MDirection firstMovingDir_p;
    

  casacore::Double distance_p;

  casacore::uInt nAntenna_p;

  casacore::Int lastFieldId_p;
  casacore::Int lastMSId_p;
  //Use douple precision grid in gridding process
  casacore::Bool useDoubleGrid_p;

  virtual void initMaps(const VisBuffer& vb);
  virtual void initPolInfo(const VisBuffer& vb);


  // Sum of weights per polarization and per chan
  casacore::Matrix<casacore::Double> sumWeight, sumCFWeight;

  // Sizes
  casacore::Int nx, ny, npol, nchan, nvischan, nvispol;

  // Maps of channels and polarization
  casacore::Vector<casacore::Int> chanMap, polMap;
  

  // Is casacore::Stokes I only? iso XX,XY,YX,YY or LL,LR,RL,RR.
  casacore::Bool isIOnly;

  // Default Position used for phase rotations
  casacore::MPosition mLocation_p;

  // Set if uvwrotation is necessary

  casacore::Bool doUVWRotation_p;
  virtual void ok();

  // check if image is big enough for gridding
  
  virtual void gridOk (casacore::Int gridsupport);

  
  // setup multiple spectral window for cubes
  casacore::Block <casacore::Vector <casacore::Int> > multiChanMap_p;
  casacore::Vector<casacore::Int> selectedSpw_p;
  casacore::Vector<casacore::Int> nVisChan_p;
  casacore::Bool matchChannel(const casacore::Int& spw, 
		    const VisBuffer& vb);

  //redo all spw chan match especially if ms has changed underneath 
  casacore::Bool matchAllSpwChans(const VisBuffer& vb);

  //interpolate visibility data of vb to grid frequency definition
  //flag will be set the one as described in interpolateArray1D
  //return false if no interpolation is done...for e.g for nearest case
  virtual casacore::Bool interpolateFrequencyTogrid(const VisBuffer& vb, 
					  const casacore::Matrix<casacore::Float>& wt,
					  casacore::Cube<casacore::Complex>& data, 
					  casacore::Cube<casacore::Int>& flag,
					  casacore::Matrix<casacore::Float>& weight,
					  FTMachine::Type type=FTMachine::OBSERVED ); 
  //degridded data interpolated back onto visibilities
  virtual casacore::Bool interpolateFrequencyFromgrid(VisBuffer& vb, 
					    casacore::Cube<casacore::Complex>& data,
					    FTMachine::Type type=FTMachine::MODEL );
  

  //Interpolate visibilities to be degridded upon
  virtual void getInterpolateArrays(const VisBuffer& vb,
				    casacore::Cube<casacore::Complex>& data, casacore::Cube<casacore::Int>& flag);


  void setSpectralFlag(const VisBuffer& vb, casacore::Cube<casacore::Bool>& modflagcube); 

  //helper to save Measures in a record
  casacore::Bool saveMeasure(casacore::RecordInterface& rec, const casacore::String& name, casacore::String& error, const casacore::Measure& ms);

  // Private variables needed for spectral frame conversion 
  casacore::SpectralCoordinate spectralCoord_p;
  casacore::Vector<casacore::Bool> doConversion_p;
  casacore::Bool freqFrameValid_p;
  casacore::Vector<casacore::Double> imageFreq_p;
  //casacore::Vector of float lsrfreq needed for regridding
  casacore::Vector<casacore::Double> lsrFreq_p;
  casacore::Vector<casacore::Double> interpVisFreq_p;
  casacore::InterpolateArray1D<casacore::Double,casacore::Complex>::InterpolationMethod freqInterpMethod_p;
  casacore::String pointingDirCol_p;
  casacore::Cube<casacore::Int> spwChanSelFlag_p;
  casacore::Matrix<casacore::Double> spwFreqSel_p, expandedSpwFreqSel_p,expandedSpwConjFreqSel_p;
  casacore::Vector<casacore::Int> cfStokes_p;
  casacore::Int polInUse_p;

  casacore::CountedPtr<CFCache> cfCache_p;
  CFStore cfs_p, cfwts_p;
  casacore::CountedPtr<CFStore2> cfs2_p, cfwts2_p;

  casacore::CountedPtr<ConvolutionFunction> convFuncCtor_p;
  casacore::CountedPtr<PolOuterProduct> pop_p;

  casacore::Bool canComputeResiduals_p;
  casacore::Bool toVis_p;
  casacore::Int numthreads_p;
  
  // casacore::Array for non-tiled gridding
  // These are common to most FTmachines
  casacore::Array<casacore::Complex> griddedData;
  casacore::Array<casacore::DComplex> griddedData2;


  casacore::Float pbLimit_p;
  //  casacore::Vector<SkyJones *> sj_p;
  casacore::Vector<casacore::CountedPtr<SkyJones> > sj_p;
  //A holder for the complex image if nobody else is keeping it
  casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > cmplxImage_p;

  FFT2D ft_p;
  casacore::Double phaseCenterTime_p;
 private:
  //Some temporary wasteful function for swapping axes because we don't 
  //Interpolation along the second axis...will need to implement 
  //interpolation on y axis of a cube. 
  
  void swapyz(casacore::Cube<casacore::Complex>& out, const casacore::Cube<casacore::Complex>& in);
  void swapyz(casacore::Cube<casacore::Complex>& out, const casacore::Cube<casacore::Bool>& outFlag, const casacore::Cube<casacore::Complex>& in);
  void swapyz(casacore::Cube<casacore::Bool>& out, const casacore::Cube<casacore::Bool>& in);
  void convUVW(casacore::Double& dphase, casacore::Vector<casacore::Double>& thisrow);
  

};

#include <synthesis/TransformMachines/FTMachine.tcc>


} //# NAMESPACE CASA - END

#endif



