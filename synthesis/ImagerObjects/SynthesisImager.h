//# SynthesisImager.h: Imager functionality sits here; 
//# Copyright (C) 2012-2013
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

#ifndef SYNTHESIS_SYNTHESISIMAGER_H
#define SYNTHESIS_SYNTHESISIMAGER_H

#include <casa/aips.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>
#include<synthesis/ImagerObjects/SynthesisUtilMethods.h>

#include<synthesis/ImagerObjects/SIMapperCollection.h>
#include <msvis/MSVis/ViFrequencySelection.h>


#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines/ATerm.h>

namespace casacore{

class MeasurementSet;
template<class T> class ImageInterface;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
 class SIIterBot;
 class VisImagingWeight;

 /**
 * Holds technical processing info related to parallelization and memory use, as introduced
 * in CAS-12204. This is meant to go into the image meta-info, such as the 'miscinfo' record.
 * The TcleanProcessingInfo holds: number of subcubes (chanchunks), number of MPI
 * processors used for these calculations, memory available, estimated required memory.
 */
struct TcleanProcessingInfo
{
  unsigned int mpiprocs = 0;
  unsigned int chnchnks = 0;
  float memavail = -.1;
  float memreq = -.1;
};

// <summary> Class that contains functions needed for imager </summary>
class SynthesisImager 
{
 public:
  // Default constructor

  SynthesisImager();
  virtual ~SynthesisImager();

  // Copy constructor and assignment operator
  //Imager(const Imager&);
  //Imager& operator=(const Imager&);

  virtual casacore::Bool selectData(const SynthesisParamsSelect& selpars);
 

  // make all pure-inputs const
  virtual casacore::Bool selectData(const casacore::String& msname, 
			  const casacore::String& spw="*", 
			  const casacore::String& freqBeg="", 
			  const casacore::String& freqEnd="",
			  const casacore::MFrequency::Types freqFrame=casacore::MFrequency::LSRK, 
			  const casacore::String& field="*", 
			  const casacore::String& antenna="",  
			  const casacore::String& timestr="", 
			  const casacore::String& scan="", 
			  const casacore::String& obs="",
			  const casacore::String& state="",
			  const casacore::String& uvdist="", 
			  const casacore::String& taql="",
			  const casacore::Bool usescratch=false, 
			  const casacore::Bool readonly=false, 
			  const casacore::Bool incrementModel=false);

  virtual casacore::Bool defineImage(SynthesisParamsImage& impars, const SynthesisParamsGrid& gridpars);

  //When having a facetted image ...call with (facets > 1)  first and  once only ..
  //Easier to keep track of the imstores that way
  ////CAREFUL: make sure you donot overwrite if you want to predict the model or subtract it to make residual
  virtual casacore::Bool defineImage(const casacore::String& imagename, const casacore::Int nx, const casacore::Int ny,
			   const casacore::Quantity& cellx, const casacore::Quantity& celly,
			   const casacore::String& stokes,
			   const casacore::MDirection& phaseCenter, 
			   const casacore::Int nchan,
			   const casacore::Quantity& freqStart,
			   const casacore::Quantity& freqStep, 
			   const casacore::Vector<casacore::Quantity>& restFreq,
			   const casacore::Int facets=1,
			   //			   const casacore::Int chanchunks=1,
			   const casacore::String ftmachine="gridft",
			   const casacore::Int nTaylorTerms=1,
			   const casacore::Quantity& refFreq = casacore::Quantity(0,"Hz"),
			   const casacore::Projection& projection=casacore::Projection::SIN,
			   const casacore::Quantity& distance=casacore::Quantity(0,"m"),
			   const casacore::MFrequency::Types& freqFrame=casacore::MFrequency::LSRK,
			   const casacore::Bool trackSource=false, 
			   const casacore::MDirection& trackDir=casacore::MDirection(casacore::Quantity(0.0, "deg"), casacore::Quantity(90.0, "deg")), 
			   const casacore::Bool overwrite=false,
			   const casacore::Float padding=1.0, 
			   const casacore::Bool useAutocorr=false, 
			   const bool useDoublePrec=true, 
			   const casacore::Int wprojplanes=1, 
			   const casacore::String convFunc="SF", 
			   const casacore::String startmodel="",
			   // The extra params for WB-AWP
			   const casacore::Bool aTermOn    = true,
			   const casacore::Bool psTermOn   = true,
			   const casacore::Bool mTermOn    = false,
			   const casacore::Bool wbAWP      = true,
			   const casacore::String cfCache  = "",
			   const casacore::Bool usePointing = false,
			   const casacore::Bool doPBCorr   = true,
			   const casacore::Bool conjBeams  = true,
			   const casacore::Float computePAStep=360.0,
			   const casacore::Float rotatePAStep=5.0
			   );
  //Define image via a predefine SIImageStore object
  virtual casacore::Bool defineImage(casacore::CountedPtr<SIImageStore> imstor, 
			   const casacore::String& ftmachine);

  //Define image via a predefine SIImageStore object and ftmachines
  virtual casacore::Bool defineImage(casacore::CountedPtr<SIImageStore> , 
                                     const casacore::Record& , const casacore::Record& ){return false;}; /*not implemented here*/

  casacore::Record getcsys() {return itsCsysRec;};
  casacore::Int updateNchan() {return itsNchan;};

  //casacore::Function to tune the data selection to intersect with image definition chosen
  //This is to optimize the data selection so that unnecessary data is not parsed despite the user
  // deciding to select so.
  // defineimage has to be run first (and thus selectData prior to that) otherwise an exception is thrown
  virtual casacore::Vector<SynthesisParamsSelect> tuneSelectData();

  //Defining componentlist to use while degriding
  //This should be called once...if multiple lists are used..they can be merged in one
  //if sdgrid=true then image plane degridding is done

  virtual void setComponentList(const ComponentList& cl, 
				casacore::Bool sdgrid=false);
  virtual casacore::Bool weight(const casacore::String& type="natural", 
	      const casacore::String& rmode="norm",
	      const casacore::Quantity& noise=casacore::Quantity(0.0, "Jy"), 
	      const casacore::Double robust=0.0,
	      const casacore::Quantity& fieldofview=casacore::Quantity(0.0, "arcsec"),
	      const casacore::Int npixels=0, 
	      const casacore::Bool multiField=false,
	      const casacore::Bool usecubebriggs=false,
	      const casacore::String& filtertype=casacore::String("Gaussian"),
	      const casacore::Quantity& filterbmaj=casacore::Quantity(0.0,"deg"),
	      const casacore::Quantity& filterbmin=casacore::Quantity(0.0,"deg"),
	      const casacore::Quantity& filterbpa=casacore::Quantity(0.0,"deg"), casacore::Double fracBW=0.0);

  virtual casacore::Bool weight(const Record&){ return false;}; /*not implemented here */
  //Stores the weight density in an image. Returns the image name 
  casacore::String getWeightDensity();
  //set the weight density to the visibility iterator
  //the default is to set it from the imagestore griwt() image
  //Otherwise it will use this image passed here; useful for parallelization to
  //share one grid to all children process
  virtual casacore::Bool setWeightDensity(const casacore::String& imagename=casacore::String(""));

  //the following get rid of the mappers in this object
  void resetMappers();

  casacore::CountedPtr<SIImageStore> imageStore(const casacore::Int id=0);

  //casacore::Record getMajorCycleControls();
  Record executeMajorCycle(const casacore::Record& controls);

  // make the psf images  i.e grid weight rather than data
  // Returns a record which may contains names of temporary files to be deleted
  casacore::Record makePSF();

  // Calculate apparent sensitivity (for _Visibility_ spectrum)
  //  _Image_ spectral grid TBD
  // Throws an exception because not supported in old VI (see SynthesisImagerVi2)
  virtual casacore::Record apparentSensitivity();

  virtual bool makePB();
  
  virtual void predictModel();
  virtual void makeSdImage(casacore::Bool dopsf=false);
  ///This should replace makeSDImage and makePSF etc in the long run
  ///But for now you can do the following images i.e string recognized by type
  ///"observed", "model", "corrected", "psf", "residual", "singledish-observed", 
  ///"singledish", "coverage", "holography", "holography-observed"
  ///For holography the FTmachine should be SDGrid and the baselines
  //selected should be those that are pointed up with the antenna which is rastering.
  virtual void makeImage(casacore::String type, const casacore::String& imagename, const casacore::String& complexImage=casacore::String(""), const Int whichModel=0);

  /* Access method to the Loop Controller held in this class */
  //SIIterBot& getLoopControls();

  virtual void dryGridding(const casacore::Vector<casacore::String>& cfList);
  virtual void fillCFCache(const casacore::Vector<casacore::String>& cfList, const casacore::String& ftmName, const casacore::String& cfcPath,
			   const casacore::Bool& psTermOn, const casacore::Bool& aTermOn, const casacore::Bool& conjBeams); 
  virtual void reloadCFCache();

  static casacore::String doubleToString(const casacore::Double& df); 
 
  const SynthesisParamsGrid& getSynthesisParamsGrid() {return gridpars_p;};
  const SynthesisParamsImage& getSynthesisParamsImage() {return impars_p;};
  ///This will set the movingSource_p
  void setMovingSource(const casacore::String& movsource);
  ///return an estimate of memory it is going to use in kB
  virtual casacore::Long estimateRAM();
  ///set and get if using cube gridding
  virtual void setCubeGridding(const casacore::Bool val){doingCubeGridding_p=val;};
  virtual casacore::Bool getCubeGridding(){return doingCubeGridding_p;};
  //this set the normalizer info record so as we can construct
  //a SynthesisNormalizer in C++ rather than run it from python
  void normalizerinfo(const casacore::Record& normpars);

  virtual bool unlockImages();
  virtual void cleanupTempFiles();
protected:
 
  /////////////// Internal Functions

  // Choose between different types of FTMs
  void createFTMachine(casacore::CountedPtr<FTMachine>& theFT, 
		       casacore::CountedPtr<FTMachine>& theIFT,  
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
		       const casacore::Bool doPBCorr   = true,
		       const casacore::Bool conjBeams  = true,
		       const casacore::Float computePAStep   = 360.0,
		       const casacore::Float rotatePAStep    = 5.0,
		       const casacore::String interpolation = casacore::String("linear"),
		       const casacore::Bool freqFrameValid = true,
		       const casacore::Int cache=1000000000,
		       const casacore::Int tile=16,
		       const casacore::String stokes="I",
		       const casacore::String imageNamePrefix="");

  void createMosFTMachine(casacore::CountedPtr<FTMachine>& theFT,
                          casacore::CountedPtr<FTMachine>&  theIFT,
                          const casacore::Float  padding,
                          const casacore::Bool useAutoCorr,
                          const casacore::Bool useDoublePrec,
                          const casacore::Float rotatePAStep,
                          const casacore::String Stokes="I",
						  const casacore::Bool doConjConvFunc=false
 						);

  // Choose between different types of ImageStore types (single term, multiterm, faceted)
  casacore::CountedPtr<SIImageStore> createIMStore(casacore::String imageName, 
					 casacore::CoordinateSystem& cSys,
					 casacore::IPosition imShape, 
					 const casacore::Bool overwrite,
					 casacore::MSColumns& msc, 
					 casacore::String mappertype="default", 
					 casacore::uInt ntaylorterms=1,
					 casacore::Quantity distance=casacore::Quantity(0.0, "m"),
					 const TcleanProcessingInfo &procInfo = TcleanProcessingInfo(),
					 casacore::uInt facets=1,
					 casacore::Bool useweightimage=false,
					 const casacore::Vector<casacore::String> &startmodel=casacore::Vector<casacore::String>(0));
  
  // Choose between different types of Mappers (single term, multiterm, imagemosaic, faceted)
  casacore::CountedPtr<SIMapper> createSIMapper(casacore::String mappertype,  
					  casacore::CountedPtr<SIImageStore> imagestore, //// make this inside !!!!!
				      casacore::CountedPtr<FTMachine> ftmachine,
				      casacore::CountedPtr<FTMachine> iftmachine,
				      casacore::uInt ntaylorterms=1);

  casacore::Block<casacore::CountedPtr<SIImageStore> > createFacetImageStoreList(
							     casacore::CountedPtr<SIImageStore> imagestore,
							     casacore::Int facets);
  // void setPsfFromOneFacet();
  casacore::Block<casacore::CountedPtr<SIImageStore> > createChanChunkImageStoreList(
							     casacore::CountedPtr<SIImageStore> imagestore,
							     casacore::Int chanchunks);

  casacore::Bool toUseWeightImage(casacore::CountedPtr<FTMachine>& ftm, casacore::String mappertype);

  virtual void createVisSet(const casacore::Bool writeaccess=false);
  
  void createAWPFTMachine(casacore::CountedPtr<FTMachine>& theFT, casacore::CountedPtr<FTMachine>& theIFT, 
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
			  const casacore::Bool doPBCorr,     
			  const casacore::Bool conjBeams,    
			  const casacore::Float computePAStep,
			  const casacore::Float rotatePAStep, 
			  const casacore::Int cache,          
			  const casacore::Int tile,
			  const casacore::String imageNamePrefix="");
  ATerm* createTelescopeATerm(const casacore::MeasurementSet& ms, const casacore::Bool& isATermOn);

  // Get VP record
  void getVPRecord(casacore::Record &rec, PBMath::CommonPB &kpb, casacore::String telescop);

  // Do the major cycle
  virtual void runMajorCycle(const casacore::Bool dopsf=false, const casacore::Bool savemodel=false);
  // Do the major cycle for cubes
  virtual void runMajorCycleCube(const casacore::Bool dopsf=false, const casacore::Record lala=casacore::Record()){(void)dopsf; (void)lala;throw(AipsError("Not implemented"));};
  // Version of major cycle code with mappers in a loop outside vi/vb.
  virtual void runMajorCycle2(const casacore::Bool dopsf=false, const casacore::Bool savemodel=false);
  virtual bool runCubePSFGridding(){throw(AipsError("Not implemented"));};
  
  virtual bool runCubeResidualGridding(casacore::Bool savemodel=false){(void)savemodel; throw(AipsError("Not implemented"));};

  /////This function should be called at every define image
  /////It associated the ftmachine with a given field
  ////For facetted image distinct  ft machines will associated with each facets and 
  //// Only one facetted image allowed
  //  void appendToMapperList(casacore::String imagename, casacore::CoordinateSystem& csys, casacore::String ftmachine,
  //		  	  casacore::Quantity distance=casacore::Quantity(0.0, "m"), casacore::Int facets=1, const casacore::Bool overwrite=false);
  void appendToMapperList(casacore::String imagename, 
			  casacore::CoordinateSystem& csys, 
			  casacore::IPosition imshape,
			  casacore::CountedPtr<FTMachine>& ftm,
			  casacore::CountedPtr<FTMachine>& iftm,
		  	  casacore::Quantity distance=casacore::Quantity(0.0, "m"), 
			  casacore::Int facets=1, 
			  casacore::Int chanchunks=1,
			  const casacore::Bool overwrite=false,
			  casacore::String mappertype=casacore::String("default"),
			  float padding=1.0,
			  casacore::uInt ntaylorterms=1,
			  const casacore::Vector<casacore::String> &startmodel=casacore::Vector<casacore::String>(0));

  virtual void unlockMSs();

  bool makePBImage(const casacore::String& telescopeName,
		   bool useSymmetricBeam, double diam);
  bool makePBImage(const casacore::String telescop);
  virtual bool makePrimaryBeam(PBMath& pbMath);

  ///is any of the images defined spectral cube
  virtual bool isSpectralCube();

  /////////////// Member Objects

  SIMapperCollection itsMappers;
  ///if facetting this storage will keep the unsliced version 
  casacore::CountedPtr<SIImageStore> unFacettedImStore_p;
  casacore::CountedPtr<SIImageStore> unChanChunkedImStore_p;

  casacore::Bool itsDataLoopPerMapper;

  casacore::CoordinateSystem itsMaxCoordSys;
  casacore::IPosition itsMaxShape;
  casacore::Record itsCsysRec;
  casacore::Int itsNchan;

  casacore::String itsVpTable;
  bool itsMakeVP;

  /////////////// All input parameters

  // casacore::Data Selection
  casacore::Vector<SynthesisParamsSelect> dataSel_p;
  // Image Definition
  // Imaging/Gridding

  ///Vi2 stuff
  casacore::Block<const casacore::MeasurementSet *> mss_p;
  //vi::FrequencySelections fselections_p;
  //casacore::CountedPtr<vi::VisibilityIterator2>  vi_p;

  // Other Options
  ////////////////////////////////////Till VisibilityIterator2 works as advertised
  casacore::Bool useViVb2_p;
  casacore::Block<casacore::MeasurementSet> mss4vi_p;
  VisibilityIterator* wvi_p;
  ROVisibilityIterator* rvi_p;
  casacore::Block<casacore::Vector<casacore::Int> > blockNChan_p;
  casacore::Block<casacore::Vector<casacore::Int> > blockStart_p;
  casacore::Block<casacore::Vector<casacore::Int> > blockStep_p;
  casacore::Block<casacore::Vector<casacore::Int> > blockSpw_p;
  casacore::Matrix<casacore::Double> mssFreqSel_p;
  //the 'channel flags' to handle various channel selections in the spw parameter
  casacore::Cube<casacore::Int > chanSel_p;

  /////////////////////////////////////////////////////////////////////////////////
  casacore::Bool writeAccess_p;
  casacore::MPosition mLocation_p;
  casacore::MDirection phaseCenter_p;
  casacore::Int facetsStore_p,chanChunksStore_p;
  VisImagingWeight imwgt_p;
  casacore::Bool imageDefined_p;
  casacore::Bool useScratch_p,readOnly_p;
  //
  //  casacore::Bool freqFrameValid_p;

  FTMachine::Type datacol_p;

  casacore::Int nMajorCycles;

  SynthesisParamsGrid gridpars_p;
  SynthesisParamsImage impars_p;
  String movingSource_p;
  casacore::Bool doingCubeGridding_p;
  
  casacore::Record normpars_p;
  casacore::CoordinateSystem csys_p;
  std::vector<casacore::String> tempFileNames_p;

};


} //# NAMESPACE CASA - END

#endif
