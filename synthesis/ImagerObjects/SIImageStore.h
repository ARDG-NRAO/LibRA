//# SIImageStore.h: Imager functionality sits here; 
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
//#        casacore::Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#
//# $Id$

#ifndef SYNTHESIS_SIIMAGESTORE_H
#define SYNTHESIS_SIIMAGESTORE_H

#include <casa/aips.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <lattices/Lattices/LatticeLocker.h>
#include <casa/BasicSL/Constants.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  
  template <class T>
  void openImage(const casacore::String& imagenamefull,std::shared_ptr<casacore::ImageInterface<T> >* img);
    

class SIImageStore 
{
 public:
  enum IMAGE_IDS {MASK=0,PSF,MODEL,RESIDUAL,WEIGHT,IMAGE,SUMWT,GRIDWT,PB,FORWARDGRID,BACKWARDGRID, IMAGEPBCOR, MAX_IMAGE_IDS};
  // Default constructor

  SIImageStore();

  SIImageStore(const casacore::String &imagename,
               const casacore::Bool ignorefacets=casacore::False,
	       const casacore::Bool noRequireSumwt=casacore::False);

  SIImageStore(const casacore::String &imagename,
	       const casacore::CoordinateSystem &imcoordsys,
	       const casacore::IPosition &imshape,
	       const casacore::String &objectname,
               const casacore::Record &miscinfo,
	       // const casacore::Int nfacets=1,
	       const casacore::Bool overwrite=casacore::False,
	       const casacore::Bool useweightimage=casacore::False);

  SIImageStore(const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &modelim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &residim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &psfim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &weightim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &restoredim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &maskim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &sumwtim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &gridwtim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &pbim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &restoredpbcorim,
               const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &tempworkim,
	       const casacore::CoordinateSystem &csys,
	       const casacore::IPosition &imshape,
	       const casacore::String &imagename,
	       const casacore::String &objectname,
	       const casacore::Record &miscinfo,
	       const casacore::Int facet=0, const casacore::Int nfacets=1,
	       const casacore::Int chan=0, const casacore::Int nchanchunks=1,
	       const casacore::Int pol=0, const casacore::Int npolchunks=1,
	       const casacore::Bool useweightimage=casacore::False);

  
    
  virtual void init();

  virtual ~SIImageStore();


  casacore::IPosition getShape();
  casacore::String getName();

  virtual casacore::String getType(){return "default";}

  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > psf(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > residual(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > scaledresidual(casacore::uInt term=0, casacore::uInt scaleidx=0){throw(casacore::AipsError("No Scaled Residual for 1 term"));};
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > weight(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > model(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > image(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > mask(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Complex> > forwardGrid(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Complex> > backwardGrid(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > sumwt(casacore::uInt term=0);


  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > alpha(){throw(casacore::AipsError("No Alpha for 1 term"));};
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > beta(){throw(casacore::AipsError("No Beta for 1 term"));};
  ///Gridwgt is for weight density sharing (Brigg's) thus only term 0 is done
  //default will be just 1 plane of densities for all fields
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > gridwt(casacore::IPosition newshape=casacore::IPosition(0));
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > pb(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > imagepbcor(casacore::uInt term=0);
  ///This returns a temporary image of same size/coordinates ...it stays alive as long as this object stays up
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > tempworkimage(casacore::uInt term=0);

  virtual void setModelImageOne( const casacore::String &modelname, casacore::Int nterm=-1 );
  virtual void setModelImage( const casacore::Vector<casacore::String> &modelnames );
  virtual casacore::Vector<casacore::String> getModelImageName();
  virtual void setWeightDensity( std::shared_ptr<SIImageStore> imagetoset );
  virtual casacore::Bool doesImageExist(casacore::String imagename);

  void setObjectName(const casacore::String);
  void setMiscInfo(const casacore::Record miscinfo);

  virtual void resetImages( casacore::Bool resetpsf, casacore::Bool resetresidual, casacore::Bool resetweight );
  virtual void addImages( std::shared_ptr<SIImageStore> imagestoadd, 
			  casacore::Bool addpsf, casacore::Bool addresidual, casacore::Bool addweight, casacore::Bool adddensity );

  ///// Normalizers
  virtual void dividePSFByWeight(const casacore::Float pblimit=casacore::C::minfloat);
  virtual void normalizePrimaryBeam(const casacore::Float pblimit=casacore::C::minfloat);
  virtual void divideResidualByWeight(const casacore::Float pblimit=casacore::C::minfloat, const casacore::String normtype="flatnoise");
  virtual void divideModelByWeight(const casacore::Float pblimit=casacore::C::minfloat, const casacore::String normtype="flatnoise");
  virtual void multiplyModelByWeight(const casacore::Float pblimit=casacore::C::minfloat, const casacore::String normtype="flatnoise");

  /// Other
  virtual casacore::Bool releaseLocks();
  virtual casacore::Bool releaseComplexGrids();
  void releaseImage( std::shared_ptr<casacore::ImageInterface<casacore::Float> > &im );
  void releaseImage( std::shared_ptr<casacore::ImageInterface<casacore::Complex> > &im );
  virtual casacore::Double getReferenceFrequency(){return 0.0;}
  virtual casacore::uInt getNTaylorTerms(casacore::Bool dopsf=casacore::False); //{return 1;};
  casacore::GaussianBeam getPSFGaussian(casacore::Float psfcutoff=0.35);
  //  virtual casacore::GaussianBeam restorePlane();
  virtual void restore(casacore::GaussianBeam& rbeam, casacore::String& usebeam,casacore::uInt term=0, casacore::Float psfcutoff=0.35 );
  virtual void pbcor(casacore::uInt term);
  virtual void pbcor(){pbcor(0);}


  ////////// Restoring Beams
  virtual void makeImageBeamSet(casacore::Float psfcutoff=0.35, const casacore::Bool forcefit=false);
  casacore::ImageBeamSet getBeamSet(casacore::Float psfcutoff=0.35);
  virtual void setBeamSet(const casacore::ImageBeamSet& bs);
  //get the beamSet of a given channel only
  virtual casacore::ImageBeamSet getChannelBeamSet(const casacore::Int chan);
  //get the beamSet for a range of channel begining and end inclusive
  virtual casacore::ImageBeamSet getChannelSliceBeamSet(const casacore::Int begChan, const casacore::Int endChan);
    
  virtual void printBeamSet(casacore::Bool verbose=casacore::False);
  casacore::GaussianBeam findGoodBeam();
  void lineFit(casacore::Vector<casacore::Float> &data, casacore::Vector<casacore::Bool> &flag, casacore::Vector<casacore::Float> &fit, casacore::uInt lim1, casacore::uInt lim2);
  casacore::Float calcMean(casacore::Vector<casacore::Float> &vect, casacore::Vector<casacore::Bool> &flag);
  casacore::Float calcStd(casacore::Vector<casacore::Float> &vect, casacore::Vector<casacore::Bool> &flag, casacore::Vector<casacore::Float> &fit);
  casacore::Float calcStd(casacore::Vector<casacore::Float> &vect, casacore::Vector<casacore::Bool> &flag, casacore::Float mean);

  // The images internall will reference back to a given section of the main of this.
  //nfacets = nx_facets*ny_facets...assumption has been made  nx_facets==ny_facets
  virtual std::shared_ptr<SIImageStore> getSubImageStore(const casacore::Int facet=0, const casacore::Int nfacets=1, 
						    const casacore::Int chan=0, const casacore::Int nchanchunks=1, 
						    const casacore::Int pol=0, const casacore::Int npolchunks=1);

  casacore::Bool getUseWeightImage(casacore::ImageInterface<casacore::Float>& target);

  //  virtual casacore::Bool hasSensitivity(){return doesImageExist(itsImageName+imageExts(WEIGHT));}
  virtual casacore::Bool hasPB(){return doesImageExist(itsImageName+imageExts(PB));}

  virtual casacore::Bool hasSensitivity(){return (bool) itsWeight;}
  //virtual casacore::Bool hasPB(){return (bool) itsPB;}

  virtual casacore::Bool hasMask(){return doesImageExist(itsImageName+imageExts(MASK)); }
  //  virtual casacore::Bool hasModel() {return (bool) itsModel;}
  virtual casacore::Bool hasModel() {return doesImageExist(itsImageName+imageExts(MODEL));}
  virtual casacore::Bool hasPsf() {return (bool) itsPsf;}
  //  virtual casacore::Bool hasPsfImage()  {return doesImageExist(itsImageName+imageExts(PSF));}
  virtual casacore::Bool hasResidual() {return (bool) itsResidual;}
  virtual casacore::Bool hasResidualImage() {return doesImageExist(itsImageName+imageExts(RESIDUAL));}
  virtual casacore::Bool hasSumWt() {return (bool) itsSumWt;}
  //  {return doesImageExist(itsImageName+imageExts(SUMWT));}
  virtual casacore::Bool hasRestored() {return doesImageExist(itsImageName+imageExts(IMAGE));}

  // Image Statistics....
  casacore::Float getPeakResidual();
  casacore::Float getPeakResidualWithinMask();
  casacore::Float getModelFlux(casacore::uInt term=0);
  virtual casacore::Bool isModelEmpty();
  virtual casacore::Float getPSFSidelobeLevel();
  virtual void setPSFSidelobeLevel(const casacore::Float lev);
  void findMinMax(const casacore::Array<casacore::Float>& lattice,
		  const casacore::Array<casacore::Float>& mask,
		  casacore::Float& minVal, casacore::Float& maxVal,
		  casacore::Float& minValMask, casacore::Float& maxValMask);
  virtual void printImageStats();
  virtual casacore::Array<casacore::Double> calcRobustRMS(casacore::Array<casacore::Double>& mdns, 
                                                        const casacore::Float pbmasklevel=0.0, const casacore::Bool fastcalc=true); 
  casacore::Float getMaskSum();

  //
  //---------------------------------------------------------------
  //
  void makePersistent(casacore::String& fileName);
  void recreate(casacore::String& fileName);

  void validate();


  void setDataPolFrame(StokesImageUtil::PolRep datapolrep) {itsDataPolRep = datapolrep;};
  StokesImageUtil::PolRep getDataPolFrame(){ return itsDataPolRep;};
  virtual void calcSensitivity();
  virtual casacore::Double calcFractionalBandwidth();// Not implemented for SIImageStore. Only for multi-term. 

  casacore::CoordinateSystem getCSys(){return itsCoordSys;}

  ///estimate of Memory to be used by the images held in KiloBytes
  
  virtual casacore::Long estimateRAM();
  ///Make an existing PagedImage complex the same shape as this imagestore
  ///coordsys and shape...effectively copies intersecting region data 
  casacore::Bool intersectComplexImage(const casacore::String& inputImage);
  static casacore::Bool copyMask(casacore::CountedPtr<casacore::ImageInterface<casacore::Float> >inimage, casacore::CountedPtr<casacore::ImageInterface<casacore::Float> >outimage);
  static void removeMask(casacore::CountedPtr<casacore::ImageInterface<casacore::Float> >im);
protected:
  std::shared_ptr<casacore::ImageInterface<casacore::Float> > makeSubImage(const casacore::Int facet, const casacore::Int nfacets,
						  const casacore::Int chan, const casacore::Int nchanchunks,
						  const casacore::Int pol, const casacore::Int npolchunks,
						  casacore::ImageInterface<casacore::Float>& image);

  casacore::Double memoryBeforeLattice();
  casacore::IPosition tileShape();

  void regridToModelImage(casacore::ImageInterface<casacore::Float> &inputimage, casacore::Int term=0 );

  casacore::Matrix<casacore::Float> getSumWt(casacore::ImageInterface<casacore::Float>& target);
  void setSumWt(casacore::ImageInterface<casacore::Float>& target, casacore::Matrix<casacore::Float>& sumwt);
  void setUseWeightImage(casacore::ImageInterface<casacore::Float>& target, casacore::Bool useweightimage);

  void fillSumWt(casacore::Int term=0);
  casacore::Bool divideImageByWeightVal( casacore::ImageInterface<casacore::Float>& target );
  void normPSF(casacore::Int term=0);

  void makePBFromWeight(const casacore::Float pblimit);
  void makePBImage(const casacore::Float pblimit);

  void accessImage( std::shared_ptr<casacore::ImageInterface<casacore::Float> > &ptr, 
		    std::shared_ptr<casacore::ImageInterface<casacore::Float> > &parentptr, 
		    const casacore::String label );

  std::shared_ptr<casacore::ImageInterface<casacore::Float> > openImage(const casacore::String imagenamefull, 
					       const casacore::Bool overwrite, 
					       const casacore::Bool dosumwt=casacore::False,
								   const casacore::Int nfacetsperside=1, const  casacore::Bool checkCoord=casacore::True);

  void buildImage(std::shared_ptr<casacore::ImageInterface<casacore::Float> > &imptr, casacore::IPosition shape, casacore::CoordinateSystem csys, const casacore::String name);
  void buildImage(std::shared_ptr<casacore::ImageInterface<casacore::Float> > &imptr,const casacore::String name);


  casacore::Double getPbMax();
  casacore::Double getPbMax(casacore::Int pol, casacore::Int chan);

  casacore::Bool createMask(casacore::LatticeExpr<casacore::Bool> &lemask, casacore::CountedPtr<casacore::ImageInterface<casacore::Float> >outimage);
  //casacore::Bool copyMask(casacore::CountedPtr<casacore::ImageInterface<casacore::Float> >inimage, casacore::CountedPtr<casacore::ImageInterface<casacore::Float> >outimage);

  
  void rescaleResolution(casacore::Int chan, casacore::ImageInterface<casacore::Float>& subResidual, const casacore::GaussianBeam& newbeam, const casacore::GaussianBeam& oldbeam);

  casacore::Bool findMinMaxLattice(const casacore::Lattice<casacore::Float>& lattice, const casacore::Lattice<casacore::Float>& mask, const casacore::Lattice<casacore::Bool>& pixmask,
			 casacore::Float& maxAbs, casacore::Float& maxAbsMask, casacore::Float& minAbs, casacore::Float& minAbsMask );


  ///////////////////// Member Objects

  casacore::IPosition itsImageShape, itsParentImageShape;
  casacore::String itsImageName;
  casacore::CoordinateSystem itsCoordSys, itsParentCoordSys;

  casacore::Bool itsOverWrite;
  casacore::Bool itsUseWeight;
  casacore::String itsObjectName;
  casacore::Record itsMiscInfo;
  std::shared_ptr<casacore::ImageInterface<casacore::Float> > itsMask, itsParentMask, itsGridWt; // mutliterm shares this...
  casacore::Double itsPBScaleFactor;

  casacore::Int itsNFacets, itsFacetId;
  casacore::Int itsNChanChunks, itsChanId;
  casacore::Int itsNPolChunks, itsPolId;

  StokesImageUtil::PolRep itsDataPolRep;

  casacore::ImageBeamSet itsPSFBeams;
  casacore::ImageBeamSet itsRestoredBeams;

  casacore::Float itsPSFSideLobeLevel;

  //
  //------------------------------------------
  // Non-persistent internal variables
  casacore::Vector<casacore::String> imageExts;

  casacore::Int itsOpened;

private:

  virtual void initMetaInfo(std::shared_ptr<casacore::ImageInterface<casacore::Float> > &imptr,
                            const casacore::String name);

  std::shared_ptr<casacore::ImageInterface<casacore::Float> > itsPsf, itsModel, itsResidual, itsWeight, itsImage, itsSumWt, itsImagePBcor, itsPB, itsTempWorkIm;
  std::shared_ptr<casacore::ImageInterface<casacore::Complex> > itsForwardGrid, itsBackwardGrid;

  std::shared_ptr<casacore::ImageInterface<casacore::Float> > itsParentPsf, itsParentModel, itsParentResidual, itsParentWeight, itsParentImage, itsParentSumWt, itsParentGridWt, itsParentPB, itsParentImagePBcor;
    


  std::shared_ptr<casacore::LatticeLocker> itsReadLock;

};

} //# NAMESPACE CASA - END

#endif
