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

#ifndef SYNTHESIS_TRANSFORM2_FTMACHINE_H
#define SYNTHESIS_TRANSFORM2_FTMACHINE_H

#include <casacore/measures/Measures/Measure.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Containers/RecordInterface.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/scimath/Mathematics/InterpolateArray1D.h>
#include <synthesis/TransformMachines2/CFCache.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/ConvolutionFunction.h>
#include <synthesis/TransformMachines2/PolOuterProduct.h>
#include <msvis/MSVis/VisBufferUtil.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/SubImage.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/Utilities/FFT2D.h>
#include <synthesis/ImagerObjects/SIImageStore.h>
#include <synthesis/ImagerObjects/SIImageStoreMultiTerm.h>

#include <synthesis/TransformMachines2/SkyJones.h>
#include <iomanip>
namespace casacore{

  class UVWMachine;
}

namespace casa{ //# namespace casa

  class VisModelData;
  namespace vi{ class VisBuffer2;
                  class VisibilityIterator2;
  }
 namespace refim{ //#	 namespace for refactored imaging code with vi2/vb2
  class BriggsCubeWeightor;
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
  //should make it pure virtual forcing every ftm to have a cloner
  virtual FTMachine* cloneFTM();
  // Initialize transform to Visibility plane
  virtual void initializeToVis(casacore::ImageInterface<casacore::Complex>& image, const vi::VisBuffer2& vb) = 0;

  virtual void initializeToVisNew(const vi::VisBuffer2& vb,
					     casacore::CountedPtr<SIImageStore> imstore);

  //-------------------------------------------------------------------------------------
  // Finalize transform to Visibility plane
  // This is mostly a no-op, and is not-even called from CubeSkyEquation.
  virtual void finalizeToVis() = 0;

  // Note : No vectorized form of finalizeToVis yet.....

  //-------------------------------------------------------------------------------------
  // Initialize transform to Sky plane
  
  virtual void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,
  			       casacore::Matrix<casacore::Float>& weight, const vi::VisBuffer2& vb) = 0;
  

  virtual void initializeToSkyNew(const casacore::Bool dopsf, 
				  const vi::VisBuffer2& vb, 
				  casacore::CountedPtr<SIImageStore> imstore);

  //-------------------------------------------------------------------------------------
  //This function has to be called after initMaps to initialize Briggs
  //Cube weighting scheme
  virtual void initBriggsWeightor(vi::VisibilityIterator2& vi);

  
  // Finalize transform to Sky plane
  virtual void finalizeToSky() = 0;

  virtual void finalizeToSky(casacore::ImageInterface<casacore::Complex>& iimage){(void)iimage;};

 
  virtual void finalizeToSkyNew(casacore::Bool dopsf, 
				const vi::VisBuffer2& vb,
					   casacore::CountedPtr<SIImageStore> imstore  );

  //-------------------------------------------------------------------------------------

  // Get actual coherence from grid
  virtual void get(vi::VisBuffer2& vb, casacore::Int row=-1) = 0;

  // Put coherence to grid
  virtual void put(const vi::VisBuffer2& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
  		   refim::FTMachine::Type type= refim::FTMachine::OBSERVED)=0;
  
  // Non const vb version - so that weights can be modified in-place
  // Currently, used only by MultiTermFT
  virtual void put(vi::VisBuffer2& vb, casacore::Int row=-1, casacore::Bool dopsf=false, 
  	           refim::FTMachine::Type type= refim::FTMachine::OBSERVED)
  {put((const vi::VisBuffer2&)vb,row,dopsf,type);};

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
  virtual casacore::Bool isSkyJonesSet(){return (sj_p.nelements()>0) && !( sj_p[0]).null()  ;}
  virtual casacore::Bool isSkyJonesChanged(vi::VisBuffer2& vb, casacore::Int row){if(sj_p.nelements()>0){return sj_p[0]->changed(vb,row);} else {return false;} };

  // Set SkyJones if image domain corrections /applycation are needed
  // To reset the the FTMachine for stopping image based correction/applycation
  // set in a casacore::Vector of size 0.
  // The pointers have to be handled by the caller ..no delete happening here
  virtual void setSkyJones(casacore::Vector<casacore::CountedPtr<SkyJones> >& sj);
  
  casacore::Bool changedSkyJonesLogic(const vi::VisBuffer2& vb, casacore::Bool& firstRow, casacore::Bool& internalRow);


  //-------------------------------------------------------------------------------------
 
  // Get the gridded visibilities or weight 
  template <typename T> void getGrid(casacore::Array<T>& thegrid);
  // Get the final image
  virtual casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true) = 0;
  virtual const casacore::CountedPtr<refim::ConvolutionFunction>& getAWConvFunc() {return convFuncCtor_p;};

  virtual void findConvFunction(const casacore::ImageInterface<casacore::Complex>&,// image,
				const vi::VisBuffer2& /*vb*/) {};
  // Get the final weights image
  virtual void getWeightImage(casacore::ImageInterface<casacore::Float>& weightImage, casacore::Matrix<casacore::Float>& weights) = 0;

  // Get a flux (divide by this to get a flux density correct image) 
  // image if there is one
  virtual void getFluxImage(casacore::ImageInterface<casacore::Float>& image){(void)image;};

  // Make the entire image
  // Make the entire image using a ROVisIter
  virtual void makeImage(FTMachine::Type type,
			 vi::VisibilityIterator2& vi,
			 casacore::ImageInterface<casacore::Complex>& image,
			 casacore::Matrix<casacore::Float>& weight);

  //-------------------------------------------------------------------------------------

  // Rotate the uvw from the observed phase center to the
  // desired phase center.
  void rotateUVW(casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Double>& dphase,
  		 const vi::VisBuffer2& vb);

  //rotate with facetting style rephasing..for multifield mosaic
  void girarUVW(casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Double>& dphase,
  		const vi::VisBuffer2& vb);

  // Refocus on a finite distance
  void refocus(casacore::Matrix<casacore::Double>& uvw, const casacore::Vector<casacore::Int>& ant1,
  	       const casacore::Vector<casacore::Int>& ant2,
  	       casacore::Vector<casacore::Double>& dphase, const vi::VisBuffer2& vb);
  //helper function for openmp to call ...no private dependency
  static void locateuvw(const casacore::Double*& uvw, const casacore::Double*&dphase, const casacore::Double*& freq, const casacore::Int& nchan, const casacore::Double*& scale, const casacore::Double*& offset,  const casacore::Int& sampling, casacore::Int*& loc,casacore::Int*& off, casacore::Complex*& phasor, const casacore::Int& row, const casacore::Bool& doW=false); 
		 

  // Save and restore the FTMachine to and from a record
  virtual casacore::Bool toRecord(casacore::String& error, casacore::RecordInterface& outRecord, 
			casacore::Bool withImage=false, const casacore::String diskimagename="");
  virtual casacore::Bool fromRecord(casacore::String& error, const casacore::RecordInterface& inRecord);

  // Has this operator changed since the last application?
  virtual casacore::Bool changed(const vi::VisBuffer2& vb);
  // Can this FTMachine be represented by Fourier convolutions?
  virtual casacore::Bool isFourier() {return false;}

  //set  otf spectral frame transform is on or off;
  casacore::Bool setFrameValidity(casacore::Bool validFrame);

  //return whether the ftmachine is using a double precision grid
  virtual casacore::Bool doublePrecGrid();

  // To make sure no padding is used in certain gridders
  virtual void setNoPadding(casacore::Bool nopad){(void)nopad;};
  
  // Return the name of the machine

  virtual casacore::String name() const =0;// { return "None";};
 
  // set and get the location used for frame 
  virtual void setLocation(const casacore::MPosition& loc);
  virtual casacore::MPosition& getLocation();

  // set a moving source aka planets or comets =>  adjust phase center
  // on the fly for gridding 
  virtual void setMovingSource(const casacore::String& sourcename, const casacore::String& ephemtable="");
  virtual void setMovingSource(const casacore::MDirection& mdir);

  //reset stuff in an FTMachine
  virtual void reset();

  //set frequency interpolation type
  virtual void setFreqInterpolation(const casacore::String& method);
  virtual void setFreqInterpolation(const casacore::InterpolateArray1D<casacore::Double,casacore::Complex>::InterpolationMethod type);
  //tell ftmachine which Pointing table column to use for Direction
  //Mosaic or Single dish ft use this for example
  virtual void setPointingDirColumn(const casacore::String& column="DIRECTION");

  virtual casacore::String getPointingDirColumnInUse();

  virtual void setSpwChanSelection(const casacore::Cube<casacore::Int>& spwchansels);
  virtual void setSpwFreqSelection(const casacore::Matrix<casacore::Double>& spwfreqs);

  // set the order of the Taylor term for MFS this is to tell
  // A-casacore::Projection to qualify the accumulated avgPB for each Taylor
  // term in the CFCache.
  virtual void setMiscInfo(const casacore::Int qualifier)=0;

  virtual void setCanComputeResiduals(casacore::Bool& b) {canComputeResiduals_p=b;};
  virtual casacore::Bool canComputeResiduals() {return canComputeResiduals_p;};
  //
  // Make the VB and VBStore interefaces for the interim re-factoring
  // work.  Finally removed the VB interface.
  virtual void ComputeResiduals(vi::VisBuffer2&vb, casacore::Bool useCorrected) = 0;
  virtual casacore::Float getPBLimit() {return pbLimit_p;};
  //virtual void ComputeResiduals(VBStore& vb)=0;
  //get and set numthreads
  void setnumthreads(casacore::Int n);
  casacore::Int getnumthreads();

  virtual void setCFCache(casacore::CountedPtr<CFCache>& cfc, const casacore::Bool resetCFC=true);
  casacore::CountedPtr<CFCache> getCFCache() {return cfCache_p;};
  casacore::String getCacheDir() { return cfCache_p->getCacheDir(); };

  virtual void setDryRun(casacore::Bool val) {isDryRun=val;};
  virtual casacore::Bool dryRun() {return isDryRun;}
  virtual casacore::Bool isUsingCFCache() {return (cfCache_p.nrefs()!=0);}

  virtual const casacore::CountedPtr<refim::FTMachine>& getFTM2(const casacore::Bool ) 
  {throw(casacore::AipsError("FTMachine::getFTM2() called directly!"));}

  casacore::Bool isDryRun;
  void setPseudoIStokes(casacore::Bool pseudoI){isPseudoI_p=pseudoI;};

   //set and get Time to calculate phasecenter  -1.0 means using the time available at 
  //each iteration..this is used when the phasecenter in the field table is either 
  //a polynomial or has a ephemerides tables associated with it
  //Using double in the units and epoch-frame of the ms(s) ..caller is responsible for conversion
  void setPhaseCenterTime(const casacore::Double time){phaseCenterTime_p=time;};
  casacore::Double getPhaseCenterTime(){return phaseCenterTime_p;};
  casacore::Vector<casacore::Int> channelMap(const vi::VisBuffer2& vb);
  casacore::Matrix<casacore::Double> getSumWeights(){return  sumWeight;};

  ///Functions associated with Briggs weighting for cubes
  void setBriggsCubeWeight(casacore::CountedPtr<refim::BriggsCubeWeightor> bwght){briggsWeightor_p=bwght;};
  void getImagingWeight(casacore::Matrix<casacore::Float>& imwght, const vi::VisBuffer2& vb);
  ///utility function that returns a rough estimate of memory needed.
  virtual casacore::Long estimateRAM(const casacore::CountedPtr<SIImageStore>& imstore);
  ///call this to clear temporary file
  // for e.g imaging weight column associated with this ftmachine
  virtual casacore::Vector<casacore::String> cleanupTempFiles(const casacore::String& message);
protected:

  friend class VisModelData;
  friend class MultiTermFT;
  friend class MultiTermFTNew;
  friend class BriggsCubeWeightor;
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
  casacore::String ephemTableName_p;
  casacore::MDirection firstMovingDir_p;
  // This will hold the angular difference between movingDir and firstMovingDir with 
  // the frame conversion done properly etc..
  casacore::MVDirection movingDirShift_p;

  casacore::Double distance_p;

  casacore::uInt nAntenna_p;

  casacore::Int lastFieldId_p;
  casacore::Int lastMSId_p;
  casacore::CountedPtr<casacore::MSColumns> romscol_p;
  //Use douple precision grid in gridding process
  casacore::Bool useDoubleGrid_p;

  virtual void initMaps(const vi::VisBuffer2& vb);
  
  virtual void initPolInfo(const vi::VisBuffer2& vb);

  // Sum of weights per polarization and per chan
  casacore::Matrix<casacore::Double> sumWeight, sumCFWeight;

  // Sizes
  casacore::Int nx, ny, npol, nchan, nvischan, nvispol;

  // Maps of channels and polarization
  casacore::Vector<casacore::Int> chanMap, polMap;

  // Stokes pseudo I only? single parallel flagged will be allowed.
  casacore::Bool isPseudoI_p;

  // Default Position used for phase rotations
  casacore::MPosition mLocation_p;

  // Set if uvwrotation is necessary

  casacore::Bool doUVWRotation_p;
  virtual void ok();

  // check if image is big enough for gridding
  
  virtual void gridOk (casacore::Int gridsupport);

  
  // setup multiple spectral window for cubes
  //casacore::Block <casacore::Vector <casacore::Int> > multiChanMap_p;
  //casacore::Vector<casacore::Int> selectedSpw_p;
  casacore::Vector<casacore::Int> nVisChan_p;
  casacore::Bool matchChannel(const casacore::Int& spw, 
		    const VisBuffer& vb);
  casacore::Bool matchChannel(const vi::VisBuffer2& vb);
  casacore::Bool matchPol(const vi::VisBuffer2& vb);
  //redo all spw chan match especially if ms has changed underneath 
  casacore::Bool matchAllSpwChans(const VisBuffer& vb);
  //casacore::Bool matchAllSpwChans(const vi::VisBuffer2& vb);
  //interpolate visibility data of vb to grid frequency definition
  //flag will be set the one as described in interpolateArray1D
  //return false if no interpolation is done...for e.g for nearest case

  virtual casacore::Bool interpolateFrequencyTogrid(const vi::VisBuffer2& vb,
  					  const casacore::Matrix<casacore::Float>& wt,
  					  casacore::Cube<casacore::Complex>& data,
  					  casacore::Cube<casacore::Int>& flag,
  					  casacore::Matrix<casacore::Float>& weight,
  					  FTMachine::Type type=FTMachine::OBSERVED );
  //degridded data interpolated back onto visibilities

  virtual casacore::Bool interpolateFrequencyFromgrid(vi::VisBuffer2& vb,
  					    casacore::Cube<casacore::Complex>& data,
  					    FTMachine::Type type=FTMachine::MODEL );

  //Interpolate visibilities to be degridded upon

  virtual void getInterpolateArrays(const vi::VisBuffer2& vb,
  				    casacore::Cube<casacore::Complex>& data, casacore::Cube<casacore::Int>& flag);


  void setSpectralFlag(const vi::VisBuffer2& vb, casacore::Cube<casacore::Bool>& modflagcube);
  //Save/Recover some elements of state of ftmachine in/from record
  casacore::Bool storeMovingSourceState(casacore::String& error, casacore::RecordInterface& outRecord);
  //helper to save Measures in a record
  casacore::Bool saveMeasure(casacore::RecordInterface& rec, const casacore::String& name, casacore::String& error, const casacore::Measure& ms);
  casacore::Bool recoverMovingSourceState(casacore::String& error, const casacore::RecordInterface& inRecord);
  casacore::Matrix<casacore::Double> negateUV(const vi::VisBuffer2& vb);

  // Private variables needed for spectral frame conversion 
  casacore::SpectralCoordinate spectralCoord_p;
  casacore::Vector<casacore::Stokes::StokesTypes> visPolMap_p;
  //casacore::Vector<casacore::Bool> doConversion_p;
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
  casacore::CountedPtr<VisBufferUtil> vbutil_p;
  casacore::Double phaseCenterTime_p;
  ///Some parameters and helpers for multithreaded gridders
  casacore::Int doneThreadPartition_p;
  casacore::Vector<casacore::Int> xsect_p, ysect_p, nxsect_p, nysect_p;
  casacore::CountedPtr<refim::BriggsCubeWeightor> briggsWeightor_p;
  virtual void   findGridSector(const casacore::Int& nxp, const casacore::Int& nyp, const casacore::Int& ixsub, const casacore::Int& iysub, const casacore::Int& minx, const casacore::Int& miny, const casacore::Int& icounter, casacore::Int& x0, casacore::Int& y0, casacore::Int& nxsub, casacore::Int& nysub, const casacore::Bool linear); 
  
  virtual void tweakGridSector(const casacore::Int& nx, const casacore::Int& ny, 
			       const casacore::Int& ixsub, const casacore::Int& iysub);
  void initSourceFreqConv();
  void shiftFreqToSource(casacore::Vector<casacore::Double>& freqs);
  ///moving source spectral frame stuff
  casacore::MRadialVelocity::Convert obsvelconv_p;
  casacore::MeasTable::Types mtype_p;
  FFT2D ft_p;
  casacore::Vector<casacore::String> tempFileNames_p;

 private:
  virtual casacore::Bool isSD() const {return false;}

  //Some temporary wasteful function for swapping axes because we don't 
  //Interpolation along the second axis...will need to implement 
  //interpolation on y axis of a cube. 
  
  void swapyz(casacore::Cube<casacore::Complex>& out, const casacore::Cube<casacore::Complex>& in);
  void swapyz(casacore::Cube<casacore::Complex>& out, const casacore::Cube<casacore::Bool>& outFlag, const casacore::Cube<casacore::Complex>& in);
  void swapyz(casacore::Cube<casacore::Bool>& out, const casacore::Cube<casacore::Bool>& in);
  void convUVW(casacore::Double& dphase, casacore::Vector<casacore::Double>& thisrow);
  
};

#include <synthesis/TransformMachines/FTMachine.tcc>

  }//# end namespace refim
} // end namespace casa
#endif



