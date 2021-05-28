//# SynthesisImagerVi2.h: Imager functionality sits here; 
//# Copyright (C) 2016
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
#ifndef SYNTHESIS_SYNTHESISIMAGERVI2_H
#define SYNTHESIS_SYNTHESISIMAGERVI2_H

#include <synthesis/ImagerObjects/SynthesisImager.h>
#include <synthesis/TransformMachines2/FTMachine.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>

namespace casacore{

class MeasurementSet;
template<class T> class ImageInterface;
}

namespace casa { //# NAMESPACE CASA - BEGIN
class VisImagingWeight;
 class SynthesisImagerVi2  : public SynthesisImager
{

public:
  // Default constructor

  SynthesisImagerVi2();
  virtual ~SynthesisImagerVi2();
  using SynthesisImager::selectData;
  virtual casacore::Bool selectData(const SynthesisParamsSelect& selpars);
  virtual casacore::Bool defineImage(SynthesisParamsImage& impars, const SynthesisParamsGrid& gridpars);
  virtual casacore::Vector<SynthesisParamsSelect> tuneSelectData();
  virtual casacore::Bool defineImage(casacore::CountedPtr<SIImageStore> imstor, const casacore::String& ftmachine);
  //Define image via a predefined SIImageStore object and ftmachines
  virtual casacore::Bool defineImage(casacore::CountedPtr<SIImageStore> imstor, 
                                     const casacore::Record& ftmachine, const casacore::Record& invftmachine);
  //This version is for premade imstor but passing extra parameters that may be needed to 
  //make facets 
  virtual casacore::Bool defineImage(casacore::CountedPtr<SIImageStore> imstor, 
                                     SynthesisParamsImage& impars, const SynthesisParamsGrid& gridpars);
  virtual casacore::Bool weight(const casacore::String& type="natural", 
	      const casacore::String& rmode="norm",
	      const casacore::Quantity& noise=casacore::Quantity(0.0, "Jy"), 
	      const casacore::Double robust=0.0,
	      const casacore::Quantity& fieldofview=casacore::Quantity(0.0, "arcsec"),
	      const casacore::Int npixels=0, 
	      const casacore::Bool multiField=false,
	      const casacore::Bool useCubeBriggs=false,
	      const casacore::String& filtertype=casacore::String("Gaussian"),
	      const casacore::Quantity& filterbmaj=casacore::Quantity(0.0,"deg"),
	      const casacore::Quantity& filterbmin=casacore::Quantity(0.0,"deg"),
	      const casacore::Quantity& filterbpa=casacore::Quantity(0.0,"deg"),
          casacore::Double fracBW=0.0);
  //set the weight from a Record generated from SynthesisUtils::fillWeightRecord
  virtual casacore::Bool weight(const Record& inrec);
  //set the weight density to the visibility iterator
  //the default is to set it from the imagestore griwt() image
  //Otherwise it will use this image passed here; useful for parallelization to
  //share one grid to all children process
  casacore::Bool setWeightDensity(const casacore::String& imagename=casacore::String(""));
  
  void predictModel();
  //make primary beam for standard gridder
  bool makePB();
  virtual void makeSdImage(casacore::Bool dopsf=false);
  ///This should replace makeSDImage and makePSF etc in the long run
  ///But for now you can do the following images i.e string recognized by type
  ///"observed", "model", "corrected", "psf", "residual", "singledish-observed", 
  ///"singledish", "coverage", "holography", "holography-observed"
  ///For holography the FTmachine should be SDGrid and the baselines
  //selected should be those that are pointed up with the antenna which is rastering.
  virtual void makeImage(casacore::String type, const casacore::String& imagename, const casacore::String& complexImage=casacore::String(""), const Int whichModel=0);

  void dryGridding(const casacore::Vector<casacore::String>& cfList);
  void fillCFCache(const casacore::Vector<casacore::String>& cfList,
		   const casacore::String& ftmName,
		   const casacore::String& cfcPath,
		   const casacore::Bool& psTermOn,
		   const casacore::Bool& aTermOn,
		   const casacore::Bool& conjBeams);
  void reloadCFCache();
  ///load the weightimage in the MosaicFT machine if it has already been done
  //this can be called only after defineimage
  void loadMosaicSensitivity();
  //Some access methods
  casacore::CountedPtr<vi::VisibilityIterator2> getVi();
  casacore::CountedPtr<refim::FTMachine> getFTM(const casacore::Int whichfield=0,
												casacore::Bool ift=true);

 protected:
  virtual void makeComplexCubeImage(const casacore::String& cimage, const refim::FTMachine::Type imtype, const Int whichModel=0);
  void appendToMapperList(casacore::String imagename, 
			  casacore::CoordinateSystem& csys, 
			  casacore::IPosition imshape,
			  casacore::CountedPtr<refim::FTMachine>& ftm,
			  casacore::CountedPtr<refim::FTMachine>& iftm,
		  	  casacore::Quantity distance=casacore::Quantity(0.0, "m"), 
			  casacore::Int facets=1, 
			  casacore::Int chanchunks=1,
			  const casacore::Bool overwrite=false,
			  casacore::String mappertype=casacore::String("default"),
			  casacore::Float padding=1.0,
			  casacore::uInt ntaylorterms=1,
			  const casacore::Vector<casacore::String> &startmodel=casacore::Vector<casacore::String>(0));
  virtual void unlockMSs();
  virtual void lockMSs();
  virtual void lockMS(MeasurementSet& ms);
  virtual void createVisSet(const casacore::Bool writeaccess=false);
  void createFTMachine(casacore::CountedPtr<casa::refim::FTMachine>& theFT, 
		       casacore::CountedPtr<casa::refim::FTMachine>& theIFT,  
		       const casacore::String& ftname,
		       const casacore::uInt nTaylorTerms=1, 
		       const casacore::String mType="default",
		       const casacore::Int facets=1,
		       //------------------------------
		       const casacore::Int wprojplane=1,
		       const casacore::Float padding=1.0,
		       const casacore::Bool useAutocorr=false,
		       const casacore::Bool useDoublePrec=true,
		       const casacore::String gridFunction=casacore::String("SF"),
		       //------------------------------
		       const casacore::Bool aTermOn    = true,
		       const casacore::Bool psTermOn   = true,
		       const casacore::Bool mTermOn    = false,
		       const casacore::Bool wbAWP      = true,
		       const casacore::String cfCache  = "",
		       const casacore::Bool usePointing = false,
		       /* const casacore::Vector<casacore::Float> pointingOffsetSigDev=std::vector<casacore::Float>({10,10}), */
		       const std::vector<float> pointingOffsetSigDev = {10,10},
		       const casacore::Bool doPBCorr   = true,
		       const casacore::Bool conjBeams  = true,
		       const casacore::Float computePAStep   = 360.0,
		       const casacore::Float rotatePAStep    = 5.0,
		       const casacore::String interpolation = casacore::String("linear"),
		       const casacore::Bool freqFrameValid = true,
		       const casacore::Int cache=1000000000,
		       const casacore::Int tile=16,
		       const casacore::String stokes="I",
		       const casacore::String imageNamePrefix="",
		       const casacore::String &pointingDirCol=casacore::String("direction"),
		       const casacore::Float skyPosThreshold=0.0,
		       const casacore::Int convSupport=-1,
		       const casacore::Quantity &truncateSize=casacore::Quantity(-1),
		       const casacore::Quantity &gwidth=casacore::Quantity(-1),
		       const casacore::Quantity &jwidth=casacore::Quantity(-1),
		       const casacore::Float minWeight=0.1,
		       const casacore::Bool clipMinMax=false,
		       const casacore::Bool pseudoI=false);

  void createAWPFTMachine(casacore::CountedPtr<refim::FTMachine>& theFT, casacore::CountedPtr<refim::FTMachine>& theIFT, 
			  const casacore::String& ftmName,
			  const casacore::Int facets,          
			  //----------------------------
			  const casacore::Int wprojPlane,     
			  const casacore::Float padding,      
			  const casacore::Bool useAutocorr,   
			  const casacore::Bool useDoublePrec, 
			  const casacore::String gridFunction,
			  //---------------------------
			  const casacore::Bool aTermOn,      
			  const casacore::Bool psTermOn,     
			  const casacore::Bool mTermOn,      
			  const casacore::Bool wbAWP,        
			  const casacore::String cfCache,    
			  const casacore::Bool usePointing,
			  /* const casacore::Vector<casacore::Float> pointingOffsetSigDev, */
			  const vector<float> pointingOffsetSigDev,
			  const casacore::Bool doPBCorr,     
			  const casacore::Bool conjBeams,    
			  const casacore::Float computePAStep,
			  const casacore::Float rotatePAStep, 
			  const casacore::Int cache,          
			  const casacore::Int tile,
			  const casacore::String imageNamePrefix="");

  void createSDFTMachine(casacore::CountedPtr<refim::FTMachine>& theFT,
      casacore::CountedPtr<refim::FTMachine>& theIFT,
      const casacore::String &pointingDirCol,
      const casacore::Float skyPosThreshold,
      const casacore::Bool doPBCorr,
      const casacore::Float rotatePAStep,
      const casacore::String& gridFunction,
      const casacore::Int convSupport,
      const casacore::Quantity& truncateSize,
      const casacore::Quantity& gwidth,
      const casacore::Quantity& jwidth,
      const casacore::Float minWeight,
      const casacore::Bool clipMinMax,
      const casacore::Int cache,
      const casacore::Int tile,
      const casacore::String &stokes,
      const casacore::Bool pseudoI=false);
 
// Do the major cycle
  virtual void runMajorCycle(const casacore::Bool dopsf=false,  const casacore::Bool savemodel=false);
  //Version for cubes
  virtual void runMajorCycleCube(const casacore::Bool dopsf=false, const casacore::Record controlrec=casacore::Record());
  // Version of major cycle code with mappers in a loop outside vi/vb.
  virtual void runMajorCycle2(const casacore::Bool dopsf=false, const casacore::Bool savemodel=false);
  
  virtual bool runCubeGridding(casacore::Bool dopsf=false, const casacore::Record controlrec=casacore::Record());
  
 
 void createMosFTMachine(casacore::CountedPtr<casa::refim::FTMachine>& theFT,
                         casacore::CountedPtr<casa::refim::FTMachine>&  theIFT,
                         const casacore::Float  padding,
                         const casacore::Bool useAutoCorr,
                         const casacore::Bool useDoublePrec,
                         const casacore::Float rotatePAStep,
                         const casacore::String Stokes="I", const casacore::Bool doConjBeam=false);
  casacore::CountedPtr<SIMapper> createSIMapper(casacore::String mappertype,  
				      casacore::CountedPtr<SIImageStore> imagestore, //// make this inside !!!!!
				      casacore::CountedPtr<refim::FTMachine> ftmachine,
				      casacore::CountedPtr<refim::FTMachine> iftmachine,
				      casacore::uInt ntaylorterms=1);

  // Calculate apparent sensitivity (for _Visibility_ spectrum)
  //  _Image_ spectral grid TBD
  virtual casacore::Record apparentSensitivity();

  
  bool makePrimaryBeam(PBMath& pbMath);
  void  andFreqSelection(const casacore::Int msId, const casacore::Int spwId,  const casacore::Double freqBeg, const casacore::Double freqEnd, const casacore::MFrequency::Types frame);
  void andChanSelection(const casacore::Int msId, const casacore::Int spwId, const casacore::Int startchan, const casacore::Int endchan);
  void tuneChunk(const casacore::Int gmap);
  //Set up tracking direction ; return False if no tracking is set.
  //return Direction of moving source is in the frame of vb.phaseCenter() at the time of the first row of the vb ..or if useImageEpoch is set at the obsTime in the image header
  casacore::Bool getMovingDirection(const vi::VisBuffer2& vb,  casacore::MDirection& movingDir, const casacore::Bool useImageEpoch=false);

  std::tuple<TcleanProcessingInfo, casacore::Vector<casacore::Int>, casacore::Vector<casacore::Int> > nSubCubeFitInMemory(const casacore::Int fudge_factor, const casacore::IPosition& imshape, const casacore::Float padding=1.0);

  void updateImageBeamSet(casacore::Record& returnRec);

   // Other Options
  //casacore::Block<const casacore::MeasurementSet *> mss_p;
  casacore::CountedPtr<vi::VisibilityIterator2>  vi_p;
  casacore::CountedPtr<vi::FrequencySelections> fselections_p;
  std::vector<std::pair<casacore::Int, casacore::Double> >freqBegs_p;
  std::vector<std::pair<casacore::Int, casacore::Double> > freqEnds_p;
  std::vector<std::pair<casacore::Int, casacore::Double> > freqSpws_p;
  //map <msid, map<spwid, vector(nchan, start)> >
  std::map<casacore::Int, std::map<casacore::Int, casacore::Vector<casacore::Int> > >  channelSelections_p;
  //	///temporary variable as we carry that for tunechunk
  casacore::MFrequency::Types selFreqFrame_p;
  casacore::Vector<SynthesisParamsImage> imparsVec_p;
  casacore::Vector<SynthesisParamsGrid> gridparsVec_p;
  casacore::Record weightParams_p;
  /// need this as writing imageinfo often seems to truncate psf size back to
  /// 0 and back to normal size...so do it only once
  casacore::ImageInfo cubePsfImageInfo_p;
};
} //# NAMESPACE CASA - END

#endif

