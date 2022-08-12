//# Imager.h: Imager functionality sits here; 
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

#ifndef SYNTHESIS_IMAGER_H
#define SYNTHESIS_IMAGER_H

#include <casacore/casa/aips.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <components/ComponentModels/ConstantSpectrum.h>

#include <casacore/measures/Measures/MDirection.h>
#include <components/ComponentModels/FluxStandard.h>
//#include <measures/Measures/MDirection.h
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/measures/Measures/MRadialVelocity.h>

#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>

#include <synthesis/MeasurementComponents/CleanImageSkyModel.h>
#include <synthesis/TransformMachines/EVLAAperture.h>
#include <synthesis/TransformMachines/BeamSquint.h>
#include <synthesis/MeasurementComponents/WFCleanImageSkyModel.h>
#include <synthesis/MeasurementComponents/ClarkCleanImageSkyModel.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <synthesis/TransformMachines/ATerm.h>
#include <graphics/GenericPlotter/SimplePlotter.h>


namespace casacore{

class MSHistoryHandler;
class MeasurementSet;
class MDirection;
class MFrequency;
class File;
template<class T> class ImageInterface;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class VisSet;
class VisImagingWeight_p;
class PBMath;
class VPSkyJones;
class EPJones;
// <summary> Class that contains functions needed for imager </summary>


class Imager 
{
 public:
  // Default constructor

  Imager();

  Imager(casacore::MeasurementSet& ms, casacore::Bool compress=false, casacore::Bool useModel=false);
  Imager(casacore::MeasurementSet& ms, casacore::Bool compress=false);

  // Copy constructor and assignment operator
  Imager(const Imager&);
  Imager& operator=(const Imager&);

  // Destructor
  virtual ~Imager();
  
  // open all the subtables as userNoReadLock
  virtual casacore::Bool openSubTables();


  // Lock the ms and its subtables
  virtual casacore::Bool lock();

  // Unlock the ms and its subtables
  virtual casacore::Bool unlock();


  // Utility function to do channel selection

  casacore::Bool selectDataChannel(casacore::Vector<casacore::Int>& spectralwindowids, 
				 casacore::String& dataMode, 
				 casacore::Vector<casacore::Int>& dataNchan, 
				 casacore::Vector<casacore::Int>& dataStart, casacore::Vector<casacore::Int>& dataStep,
				 casacore::MRadialVelocity& mDataStart, 
				 casacore::MRadialVelocity& mDataStep);
  //Utility function to check coordinate match with existing image

  virtual casacore::Bool checkCoord(const casacore::CoordinateSystem& coordsys, 
			  const casacore::String& imageName); 

  virtual void setImageParam(casacore::Int& nx, casacore::Int& ny, casacore::Int& npol, casacore::Int& nchan); 

  //VisSet and resort 
  virtual void makeVisSet(casacore::MeasurementSet& ms, 
			  casacore::Bool compress=false, casacore::Bool mosaicOrder=false);
  //Just to create the SORTED_TABLE if one can
  //virtual void makeVisSet(casacore::MeasurementSet& ms, 
  //			  casacore::Bool compress=false, casacore::Bool mosaicOrder=false);

  virtual void writeHistory(casacore::LogIO& os);

  virtual void writeCommand(casacore::LogIO& os);

  //make an empty image
  casacore::Bool makeEmptyImage(casacore::CoordinateSystem& imageCoord, casacore::String& name, casacore::Int fieldID=0);

  //Functions to make Primary beams 
  casacore::Bool makePBImage(casacore::ImageInterface<casacore::Float>& pbImage, 
		   casacore::Bool useSymmetricBeam=true);
  casacore::Bool makePBImage(const casacore::CoordinateSystem& imageCoord, 
		   const casacore::String& telescopeName, const casacore::String& diskPBName, 
		   casacore::Bool useSymmetricBeam=true, casacore::Double dishdiam=-1.0);
  
  casacore::Bool makePBImage(const casacore::CoordinateSystem& imageCoord, 
		   const casacore::Table& vpTable, const casacore::String& diskPBName);
  
  casacore::Bool makePBImage(const casacore::Table& vpTable, casacore::ImageInterface<casacore::Float>& pbImage);
  
  casacore::Bool makePBImage(const casacore::CoordinateSystem& imageCoord, PBMath& pbMath, const casacore::String& diskPBName);
  
  casacore::Bool makePBImage(PBMath& pbMath, casacore::ImageInterface<casacore::Float>& pbImage);
  
  void setObsInfo(casacore::ObsInfo& obsinfo);
  casacore::ObsInfo& latestObsInfo();
// Close the current ms, and replace it with the supplied ms.
  // Optionally compress the attached calibration data
  // columns if they are created here.
  casacore::Bool open(casacore::MeasurementSet &thems, casacore::Bool compress=false, casacore::Bool useModel=false);
  
  // Flush the ms to disk and detach from the ms file. All function
  // calls after this will be a no-op.
  casacore::Bool close();
  
  // Return the name of the MeasurementSet
  casacore::String name() const;
  
  // The following setup methods define the state of the imager.
  // <group>
  // Set image construction parameters
  virtual casacore::Bool setimage(const casacore::Int nx, const casacore::Int ny,
		const casacore::Quantity& cellx, const casacore::Quantity& celly,
		const casacore::String& stokes,
                casacore::Bool doShift,
		const casacore::MDirection& phaseCenter, 
                const casacore::Quantity& shiftx, const casacore::Quantity& shifty,
		const casacore::String& mode, const casacore::Int nchan,
                const casacore::Int start, const casacore::Int step,
		const casacore::MRadialVelocity& mStart, const casacore::MRadialVelocity& mStep,
		const casacore::Vector<casacore::Int>& spectralwindowids, const casacore::Int fieldid,
		const casacore::Int facets, const casacore::Quantity& distance);

  virtual casacore::Bool defineImage(const casacore::Int nx, const casacore::Int ny,
			   const casacore::Quantity& cellx, const casacore::Quantity& celly,
			   const casacore::String& stokes,
			   const casacore::MDirection& phaseCenter, 
			   const casacore::Int fieldid,
			   const casacore::String& mode, const casacore::Int nchan,
			   const casacore::Int start, const casacore::Int step,
			   const casacore::MFrequency& mFreqStart,
			   const casacore::MRadialVelocity& mStart, 
			   const casacore::Quantity& qStep,
			   const casacore::Vector<casacore::Int>& spectralwindowids, 
			   const casacore::Int facets=1, 
			   const casacore::Quantity& restFreq=casacore::Quantity(0,"Hz"),
                           const casacore::MFrequency::Types& mFreqFrame=casacore::MFrequency::LSRK,
			   const casacore::Quantity& distance=casacore::Quantity(0,"m"),
			   const casacore::Bool trackSource=false, const casacore::MDirection& 
			   trackDir=casacore::MDirection(casacore::Quantity(0.0, "deg"), 
					       casacore::Quantity(90.0, "deg")),
				 const casacore::String& projection=casacore::String("SIN"));
  // Set the data selection parameters
 
  // The parameters useModelcol and readonly is dummy here
  //as they are useful for the ImagerMultiMS version only
  virtual  casacore::Bool setDataPerMS(const casacore::String& msname, const casacore::String& mode, 
			     const casacore::Vector<casacore::Int>& nchan, 
			     const casacore::Vector<casacore::Int>& start,
			     const casacore::Vector<casacore::Int>& step,
			     const casacore::Vector<casacore::Int>& spectralwindowids,
			     const casacore::Vector<casacore::Int>& fieldid,
			     const casacore::String& msSelect="",
                             const casacore::String& timerng="",
			     const casacore::String& fieldnames="",
			     const casacore::Vector<casacore::Int>& antIndex=casacore::Vector<casacore::Int>(),
			     const casacore::String& antnames="",
			     const casacore::String& spwstring="",
			     const casacore::String& uvdist="",
                             const casacore::String& scan="",
                             const casacore::String& intent="",
                             const casacore::String& obs="",
                             const casacore::Bool useModelCol=false,
			     const casacore::Bool readonly=false);

  // Select some data.
  // Sets nullSelect_p and returns !nullSelect_p.
  // be_calm: lowers the logging level of some messages if true.
  casacore::Bool setdata(const casacore::String& mode, const casacore::Vector<casacore::Int>& nchan, 
	       const casacore::Vector<casacore::Int>& start,
	       const casacore::Vector<casacore::Int>& step, const casacore::MRadialVelocity& mStart,
	       const casacore::MRadialVelocity& mStep,
	       const casacore::Vector<casacore::Int>& spectralwindowids,
	       const casacore::Vector<casacore::Int>& fieldid,
	       const casacore::String& msSelect="",
	       const casacore::String& timerng="",
	       const casacore::String& fieldnames="",
	       const casacore::Vector<casacore::Int>& antIndex=casacore::Vector<casacore::Int>(),
	       const casacore::String& antnames="",
	       const casacore::String& spwstring="",
	       const casacore::String& uvdist="",
               const casacore::String& scan="",
               const casacore::String& intent="",
               const casacore::String& obs="",
               const casacore::Bool usemodelCol=false,
               const casacore::Bool be_calm=false);
  
  // Set the processing options
  casacore::Bool setoptions(const casacore::String& ftmachine, const casacore::Long cache, const casacore::Int tile,
		  const casacore::String& gridfunction, const casacore::MPosition& mLocation,
                  const casacore::Float padding,
		  const casacore::Int wprojplanes=-1,
		  const casacore::String& epJTableName="",
		  const casacore::Bool applyPointingOffsets=true,
		  const casacore::Bool doPointingCorrection=true,
		  const casacore::String& cfCacheDirName="", 
		  const casacore::Float& rotpastep=5.0,
		  const casacore::Float& computepastep=360.0,
		  const casacore::Float& pbLimit=5.0e-2,
		  const casacore::String& freqinterpmethod="linear",
		  const casacore::Int imageTileSizeInPix=0,
		  const casacore::Bool singleprecisiononly=false,
		  const casacore::Int numthreads=-1,
		  const casacore::Bool psTermOn=true,
		  const casacore::Bool aTermOn=true,
		  const casacore::Bool mTermOn=false,
		  const casacore::Bool wbAWP=false,
		  const casacore::Bool conjBeams=true);

  // Set the single dish processing options
  casacore::Bool setsdoptions(const casacore::Float scale, const casacore::Float weight, 
		    const casacore::Int convsupport=-1, casacore::String pointingColToUse="DIRECTION",
		    const casacore::Quantity truncate=casacore::Quantity(),
		    const casacore::Quantity gwidth=casacore::Quantity(),
		    const casacore::Quantity jwidth=casacore::Quantity(),
		    const casacore::Float minweight=0.,
		    const casacore::Bool clipminmax=false);

  // Set the voltage pattern
  casacore::Bool setvp(const casacore::Bool dovp,
	     const casacore::Bool defaultVP,
	     const casacore::String& vpTable,
	     const casacore::Bool doSquint,
	     const casacore::Quantity &parAngleInc,
	     const casacore::Quantity &skyPosThreshold,
	     casacore::String defaultTel="",
             const casacore::Bool verbose=true);

  // Set the scales to be searched in Multi Scale clean
  casacore::Bool setscales(const casacore::String& scaleMethod,          // "nscales"  or  "uservector"
		 const casacore::Int inscales,
		 const casacore::Vector<casacore::Float>& userScaleSizes);
  // set bias
  casacore::Bool setSmallScaleBias(const casacore::Float inbias);

  // Set the number of taylor series terms in the expansion of the
  // image as a function of frequency.
  casacore::Bool settaylorterms(const casacore::Int intaylor, 
		      const casacore::Double inreffreq);

  // </group>
  
  // Advise on suitable values
  casacore::Bool advise(const casacore::Bool takeAdvice, const casacore::Float amplitudeloss,
              const casacore::Quantity& fieldOfView,
	      casacore::Quantity& cell, casacore::Int& npixels, casacore::Int& facets,
	      casacore::MDirection& phaseCenter);

  // Output a summary of the state of the object
  casacore::Bool summary();
  
  // Return the state of the object as a string
  casacore::String state();
  
  // Return the # of visibilities accessible to *rvi, optionally excluding
  // flagged ones (if unflagged_only is true) and/or ones without imaging
  // weights (if must_have_imwt is true).
  casacore::uInt count_visibilities(ROVisibilityIterator *rvi,
                          const casacore::Bool unflagged_only, const casacore::Bool must_have_imwt);

  // Return the image coordinates
  casacore::Bool imagecoordinates(casacore::CoordinateSystem& coordInfo, const casacore::Bool verbose=true);
  // new version
  casacore::Bool imagecoordinates2(casacore::CoordinateSystem& coordInfo, const casacore::Bool verbose=true);

  // Return the image shape
  casacore::IPosition imageshape() const;

  // Weight the MeasurementSet
  //For some time of weighting briggs/uniform ...one can do it on a per field basis to calculate 
  //weight density distribution. If that is what is wanted multiField should be set to true
  //multifield is inoperative for natural, radial weighting
  casacore::Bool weight(const casacore::String& algorithm, const casacore::String& rmode,
	      const casacore::Quantity& noise, const casacore::Double robust,
              const casacore::Quantity& fieldofview, const casacore::Int npixels, const casacore::Bool multiField=false);
  
  // Filter the MeasurementSet
  casacore::Bool filter(const casacore::String& type, const casacore::Quantity& bmaj, const casacore::Quantity& bmin,
	      const casacore::Quantity& bpa);
  
  // Apply a uvrange
  casacore::Bool uvrange(const casacore::Double& uvmin, const casacore::Double& uvmax);
  
  // Sensitivity
  casacore::Bool sensitivity(casacore::Quantity& pointsourcesens, casacore::Double& relativesens, casacore::Double& sumwt,
		   casacore::Double& effectiveBandwidth, casacore::Double& effectiveIntegration, casacore::Int& mBaselines,
		   casacore::Matrix<casacore::Int>& mssChanSel,
		   casacore::Vector<casacore::Vector<casacore::Int> >& nData,
		   casacore::Vector<casacore::Vector<casacore::Double> >& sumwtChan,
		   casacore::Vector<casacore::Vector<casacore::Double> >& sumwtsqChan,
		   casacore::Vector<casacore::Vector<casacore::Double> >& sumInverseVarianceChan);
  
  // Apparent sensitivity calculator 
  //   Accounts for gridding weights and assumes casacore::MS weights have correct units
  casacore::Bool apparentSensitivity(casacore::Double& effSensitivity,
			   casacore::Double& relToNat);

  // Make plain image + keep the complex image as well if complexImageName != "".
  casacore::Bool makeimage(const casacore::String& type, const casacore::String& imageName,
                 const casacore::String& complexImageName="", const casacore::Bool verbose=true);
  
  // Fill in a region of a mask
  casacore::Bool boxmask(const casacore::String& mask, const casacore::Vector<casacore::Int>& blc,
	       const casacore::Vector<casacore::Int>& trc,const casacore::Float value);

  //Make a region either from record or array of blc trc 
  //(casacore::Matrix(nboxes,4)) into a mask image
  //value is the value of the mask pixels
  //circular masks has form casacore::Matrix(ncircles,3)
  //where the 3 values on a row are radius, x, y pixel values 
  casacore::Bool regionmask(const casacore::String& maskimage, casacore::Record* imageRegRec, 
		  casacore::Matrix<casacore::Quantity>& blctrcs, casacore::Matrix<casacore::Float>& circles, 
		  const casacore::Float& value=1.0);

  static casacore::Bool regionToImageMask(const casacore::String& maskimage, casacore::Record* imageRegRec, 
				casacore::Matrix<casacore::Quantity>& blctrcs, 
				casacore::Matrix<casacore::Float>& circles, 
				const casacore::Float& value=1.0);
  // Clip on casacore::Stokes I
  casacore::Bool clipimage(const casacore::String& image, const casacore::Quantity& threshold);

  // Make a mask image
  static casacore::Bool mask(const casacore::String& mask, const casacore::String& imageName,
                   const casacore::Quantity& threshold);
  
  // Restore
  casacore::Bool restore(const casacore::Vector<casacore::String>& model, const casacore::String& complist,
	       const casacore::Vector<casacore::String>& image, const casacore::Vector<casacore::String>& residual);

  // similar to restore except this is to be called if you fiddle with the model and complist
  // outside of this object (say you clip stuff etc) ...keep the sm_p and se_p state but just calculate new residuals and 
  // restored images. Will throw an exception is se_p or sm_p is not valid (i.e you should have used clean, mem etc before hand).
  casacore::Bool updateresidual(const casacore::Vector<casacore::String>& model, const casacore::String& complist,
	       const casacore::Vector<casacore::String>& image, const casacore::Vector<casacore::String>& residual);

  // Setbeam
  casacore::Bool setbeam(const casacore::ImageBeamSet& beam);

  // Residual
  casacore::Bool residual(const casacore::Vector<casacore::String>& model, const casacore::String& complist,
	       const casacore::Vector<casacore::String>& image);

  // Approximate PSF
  casacore::Bool approximatepsf(const casacore::String& psf);

  // Smooth
  casacore::Bool smooth(const casacore::Vector<casacore::String>& model, 
	      const casacore::Vector<casacore::String>& image, casacore::Bool usefit,
	      casacore::ImageBeamSet& mbeam,
	      casacore::Bool normalizeVolume);

  // Clean algorithm
  casacore::Record clean(const casacore::String& algorithm,
	     const casacore::Int niter, 
	     const casacore::Float gain, 
	     const casacore::Quantity& threshold, 
	     const casacore::Bool displayProgress, 
	     const casacore::Vector<casacore::String>& model, const casacore::Vector<casacore::Bool>& fixed,
	     const casacore::String& complist,
	     const casacore::Vector<casacore::String>& mask,
	     const casacore::Vector<casacore::String>& restored,
	     const casacore::Vector<casacore::String>& residual,
	     const casacore::Vector<casacore::String>& psf=casacore::Vector<casacore::String>(0),
             const casacore::Bool firstrun=true);

  casacore::Record iClean(const casacore::String& algorithm, 
	      const casacore::Int niter, 
	      const casacore::Double gain,
	      //const casacore::String& threshold, 
	      const casacore::Quantity& threshold,
	      const casacore::Bool displayprogress,
	      const casacore::Vector<casacore::String>& model,
	      const casacore::Vector<casacore::Bool>& keepfixed, const casacore::String& complist,
	      const casacore::Vector<casacore::String>& mask,
	      const casacore::Vector<casacore::String>& image,
	      const casacore::Vector<casacore::String>& residual,
	      const casacore::Vector<casacore::String>& psfnames,
	      const casacore::Bool interactive, const casacore::Int npercycle,
	      const casacore::String& masktemplate);
  
  // MEM algorithm
  casacore::Bool mem(const casacore::String& algorithm,
	   const casacore::Int niter, const casacore::Quantity& sigma, 
	   const casacore::Quantity& targetflux,
	   const casacore::Bool constrainflux,
	   const casacore::Bool displayProgress, 
	   const casacore::Vector<casacore::String>& model, const casacore::Vector<casacore::Bool>& fixed,
	   const casacore::String& complist,
	   const casacore::Vector<casacore::String>& prior,
	   const casacore::Vector<casacore::String>& mask,
	   const casacore::Vector<casacore::String>& restored,
	   const casacore::Vector<casacore::String>& residual);
  
  // NNLS algorithm
  casacore::Bool nnls(const casacore::String& algorithm, const casacore::Int niter, const casacore::Float tolerance,
	    const casacore::Vector<casacore::String>& model, const casacore::Vector<casacore::Bool>& fixed,
	    const casacore::String& complist,
	    const casacore::Vector<casacore::String>& fluxMask, const casacore::Vector<casacore::String>& dataMask,
	    const casacore::Vector<casacore::String>& restored,
	    const casacore::Vector<casacore::String>& residual);

  // Multi-field control parameters
  //flat noise is the parameter that control the search of clean components
  //in a flat noise image or an optimum beam^2 image
  casacore::Bool setmfcontrol(const casacore::Float cyclefactor,
		    const casacore::Float cyclespeedup,
		    const casacore::Float cyclemaxpsffraction,
		    const casacore::Int stoplargenegatives, 
		    const casacore::Int stoppointmode,
		    const casacore::String& scaleType,
		    const casacore::Float  minPB,
		    const casacore::Float constPB,
		    const casacore::Vector<casacore::String>& fluxscale,
		    const casacore::Bool flatnoise=true);
  
  // Feathering algorithm
  casacore::Bool feather(const casacore::String& image,
	       const casacore::String& highres,
	       const casacore::String& lowres,
	       const casacore::String& lowpsf, const casacore::Float dishDiam=-1.0, const casacore::Bool lowPassFilterSD=false);
  
  // Apply or correct for Primary Beam or Voltage Pattern
  casacore::Bool pb(const casacore::String& inimage,
	  const casacore::String& outimage,
	  const casacore::String& incomps,
	  const casacore::String& outcomps,
	  const casacore::String& operation,
	  const casacore::MDirection& pointngCenter,
	  const casacore::Quantity& pa,
	  const casacore::String& pborvp);

  // Make a linear mosaic of several images
  casacore::Bool linearmosaic(const casacore::String& mosaic,
		    const casacore::String& fluxscale,
		    const casacore::String& sensitivity,
		    const casacore::Vector<casacore::String>& images,
		    const casacore::Vector<casacore::Int>& fieldids);
  
  // Fourier transform the model and componentlist.
  // Returns its nominal success value.
  ///For moving time variable phasecenters in field table
  //phasecentertime is usually the time in the data unless 
  //you want to use a specific time which you can set here 
  casacore::Bool ft(const casacore::Vector<casacore::String>& model, const casacore::String& complist,
		    casacore::Bool incremental=false, const casacore::Double phasecenterTime=-1.0);

  // Compute the model visibility using specified source flux densities
  casacore::Bool setjy(const casacore::Int fieldid, const casacore::Int spectralwindowid,
	     const casacore::Vector<casacore::Double>& fluxDensity, const casacore::String& standard);
  casacore::Bool setjy(const casacore::Vector<casacore::Int>& fieldid, const casacore::Vector<casacore::Int>& spectralwindowid, 
	     const casacore::String& fieldnames, const casacore::String& spwstring, 
	     const casacore::Vector<casacore::Double>& fluxDensity, const casacore::String& standard);
  
  //Setjy with model image. If chanDep=true then the scaling is calculated on a 
  //per channel basis for the model image...otherwise the whole spw gets the
  //same flux density.
  //casacore::Bool setjy(const casacore::Vector<casacore::Int>& fieldid, 
  casacore::Record setjy(const casacore::Vector<casacore::Int>& fieldid, 
	     const casacore::Vector<casacore::Int>& spectralwindowid, 
	     const casacore::String& fieldnames, const casacore::String& spwstring, 
	     const casacore::String& model,
	     const casacore::Vector<casacore::Double>& fluxDensity, const casacore::String& standard, 
	     const casacore::Bool chanDep=false, //const casacore::Double spix=0.0,
             const casacore::Vector<casacore::Double>& spix=casacore::Vector<casacore::Double>(),
             const casacore::MFrequency& reffreq=casacore::MFrequency(casacore::Quantity(1.0, "GHz"),
                                                  casacore::MFrequency::LSRK),
             const casacore::Vector<casacore::Double>& pipars=casacore::Vector<casacore::Double>(),
             const casacore::Vector<casacore::Double>& papars=casacore::Vector<casacore::Double>(),
             const casacore::Double& rotMeas=0.0, 
             const casacore::String& timerange="", const casacore::String& scanstr="",
             const casacore::String& intentstr="", const casacore::String& obsidstr="",
             const casacore::String& interpolation="nearest");

  // Make an empty image
  casacore::Bool make(const casacore::String& model);

  // make a model from a SD image. 
  // This model then can be used as initial clean model to include the 
  // shorter spacing.
  casacore::Bool makemodelfromsd(const casacore::String& sdImage, const casacore::String& modelimage,
		       const casacore::String& lowPSF,
		       casacore::String& maskImage);

  // Write a component list to disk, starting with prefix, using a setjy
  // standard, and return the name of the list.
  casacore::String make_comp(const casacore::String& objName, const casacore::String& standard,
		   const casacore::MEpoch& mtime, const casacore::Vector<casacore::MFrequency>& freqv,
		   const casacore::String& prefix);

  // Clone an image
  static casacore::Bool clone(const casacore::String& imageName, const casacore::String& newImageName);
  
  // Fit the psf
  casacore::Bool fitpsf(const casacore::String& psf, casacore::ImageBeamSet& mbeam);

  // Correct the visibility data (OBSERVED->CORRECTED)
  casacore::Bool correct(const casacore::Bool doparallactic, const casacore::Quantity& t);

  // Plot the uv plane
  casacore::Bool plotuv(const casacore::Bool rotate);

  // Plot the visibilities
  casacore::Bool plotvis(const casacore::String& type, const casacore::Int increment);

  // Plot the weights
  casacore::Bool plotweights(const casacore::Bool gridded, const casacore::Int increment);

  // Plot a summary
  casacore::Bool plotsummary();

  // Clip visibilities
  casacore::Bool clipvis(const casacore::Quantity& threshold);


  //Check if can proceed with this object
  casacore::Bool valid() const;


  //Interactive mask drawing
  //forceReload..forces the viewer to dump previous image that is being displayed
  casacore::Int interactivemask(const casacore::String& imagename, const casacore::String& maskname, 
		      casacore::Int& niter, casacore::Int& ncycles, casacore::String& threshold, const casacore::Bool forceReload=false);


  //helper function to copy a mask from one image to another

  static casacore::Bool copyMask(casacore::ImageInterface<casacore::Float>& out, const casacore::ImageInterface<casacore::Float>& in, casacore::String maskname="mask0", casacore::Bool setdefault=true); 


  // Supports the "[] or -1 => everything" convention using the rule:
  // If v is empty or only has 1 element, and it is < 0, 
  //     replace v with 0, 1, ..., nelem - 1.
  // Returns whether or not it modified v.
  //   If so, v is modified in place.
  static casacore::Bool expand_blank_sel(casacore::Vector<casacore::Int>& v, const casacore::uInt nelem);  

  //spectral gridding calculation for output images (use SubMS::calcChanFreqs)
  casacore::Bool calcImFreqs(casacore::Vector<casacore::Double>& imfreqs, casacore::Vector<casacore::Double>& imfreqres,
                   const casacore::MFrequency::Types& oldRefFrame,
                   const casacore::MEpoch& obsEpoch, const casacore::MPosition& obsPosition,
                   const casacore::Double& restFreq);

  // Advise the chanselection needed for the frequency range or
  // give the frequency range for a give spwselection  if getFreqRange==true
  // if the parameter msname is used then the MSs associated associated with
  // this object (that have been either 'open'ed or 'selectvis'ed) are ignored
  // In this mode it is a helper function to the general world ...no need to
  // open or selectvis. You need to specify the field_id for which this calculation is 
  // being done for in the helper mode. 
  // If you have already set casacore::MS's and selected data and msname="" then 
  // the calulation is done for the field(s) selected in selectvis.
  // getFreqRange=true then the freqrange in the frame and spwselection  you choose is 
  // returned in freqStart and freqEnd (in the case of msname="" then it is for the fields 
  //and spw you have chosen in selectvis). 
  casacore::Bool adviseChanSelex(casacore::Double& freqStart, casacore::Double& freqEnd, 
		       const casacore::Double& freqStep,  const casacore::MFrequency::Types& freqframe,
		       casacore::Vector< casacore::Vector<casacore::Int> >& spw, casacore::Vector< casacore::Vector<casacore::Int> >& start,
		       casacore::Vector< casacore::Vector<casacore::Int> >& nchan, const casacore::String& msname="", 
		       const casacore::Int fieldid=0, const casacore::Bool getFreqRange=false, 
		       const casacore::String spwselection="");


  //These are utility functions when weights from different imager instances 
  //need to reconciled in parallel gridding by different instances of imagers 
  //for example.
  // when type is "imaging"
  // getweightGrid will get the weight density for uniform style imaging weight
  // the casacore::Block elements are for different fields if independent field weighting 
  // was done.
  // when type is "ftweight"..then a casacore::Vector of string is expected in weightimage 
  // which is of the same length as the number of models put in clean etc 

  casacore::Bool getWeightGrid(casacore::Block<casacore::Matrix<casacore::Float> >&weightgrid, const casacore::String& type, const casacore::Vector<casacore::String>& weightImagenames=casacore::Vector<casacore::String>());
  casacore::Bool setWeightGrid(const casacore::Block<casacore::Matrix<casacore::Float> >& weightgrid, const casacore::String& type);
  casacore::String dQuantitytoString(const casacore::Quantity& dq);

  // Automatic evaluation of map extent for given visibility.
  //
  //
  // @param[in] referenceFrame reference direction frame
  // @param[in] movingSource name of moving source
  // @param[in] pointingColumn pointing column to use
  // @param[out] center center of the map
  // @param[out] blc bottom left corner of the map
  // @param[out] trc top right corner of the map
  // @param[out] extent map extent
  //
  // @return
  virtual casacore::Bool mapExtent(const casacore::String &referenceFrame, const casacore::String &movingSource,
          const casacore::String &pointingColumn, casacore::Vector<casacore::Double> &center, casacore::Vector<casacore::Double> &blc,
          casacore::Vector<casacore::Double> &trc, casacore::Vector<casacore::Double> &extent);

  // Caluculate sampling interval of Raster map
  //
  //
  // @param[in] referenceFrame reference direction frame
  // @param[in] movingSource name of moving source
  // @param[in] pointingColumn pointing column to use
  // @param[in] antenna antenna selection string, e.g., '0&&&'
  // @param[out] sampling the sampling interval in scan direction and orthogonal direction
  // @param[out] positionAngle position angle of scan
  //
  // @return
  virtual casacore::Bool pointingSampling(const casacore::String &referenceFrame, const casacore::String &movingSource,
          const casacore::String &pointingColumn, const casacore::String &antenna,
	  casacore::Quantum<casacore::Vector<casacore::Double>> &sampling, casacore::Quantity &positionAngle);

  //Helper function to transfer history table to a logger holder
  //which can be stored in images
  static void transferHistory(casacore::LoggerHolder& imageLog, casacore::MSHistoryColumns& msHis);

protected:

  casacore::CountedPtr<casacore::MeasurementSet> ms_p;
  casacore::CountedPtr<casacore::MSHistoryHandler> hist_p;
  casacore::Table antab_p;
  casacore::Table datadesctab_p;
  casacore::Table feedtab_p;
  casacore::Table fieldtab_p;
  casacore::Table obstab_p;
  casacore::Table pointingtab_p;
  casacore::Table poltab_p;
  casacore::Table proctab_p;
  casacore::Table spwtab_p;
  casacore::Table statetab_p;
  casacore::Table dopplertab_p;
  casacore::Table flagcmdtab_p;
  casacore::Table freqoffsettab_p;
  casacore::Table historytab_p;
  casacore::Table sourcetab_p;
  casacore::Table syscaltab_p;
  casacore::Table weathertab_p;
  casacore::Int lockCounter_p;
  casacore::Int nx_p, ny_p, npol_p, nchan_p;
  casacore::ObsInfo latestObsInfo_p;
  //What should be the tile volume on disk
  casacore::Int imageTileVol_p;



  casacore::String msname_p;
  casacore::CountedPtr<casacore::MeasurementSet> mssel_p;
  VisSet *vs_p;
  ROVisibilityIterator* rvi_p;
  VisibilityIterator* wvi_p;
  FTMachine *ft_p;
  ComponentFTMachine *cft_p;
  SkyEquation* se_p;
  CleanImageSkyModel* sm_p;
  VPSkyJones* vp_p;
  VPSkyJones* gvp_p;

  casacore::Bool setimaged_p, nullSelect_p;
  casacore::Bool redoSkyModel_p;   // if clean is run multiply ..use this to check
                         // if setimage was changed hence redo the skyModel.
  casacore::Float rotPAStep_p, computePAStep_p, pbLimit_p;
  casacore::Int facets_p;
  casacore::Int wprojPlanes_p;
  casacore::Quantity mcellx_p, mcelly_p;
  casacore::String stokes_p;
  casacore::String dataMode_p;
  casacore::String imageMode_p;           // channel, (optical)velocity, mfs, or frequency
  casacore::Vector<casacore::Int> dataNchan_p;
  casacore::Int imageNchan_p;
  casacore::Vector<casacore::Int> dataStart_p, dataStep_p;
  casacore::Int imageStart_p, imageStep_p;
  casacore::MRadialVelocity mDataStart_p, mImageStart_p;
  casacore::MRadialVelocity mDataStep_p,  mImageStep_p;
  casacore::MFrequency mfImageStart_p, mfImageStep_p;
  casacore::MFrequency::Types freqFrame_p;
  casacore::MDirection phaseCenter_p;
  casacore::Quantity restFreq_p;
  casacore::Quantity distance_p;
  casacore::Bool doShift_p;
  casacore::Quantity shiftx_p;
  casacore::Quantity shifty_p;
  casacore::String ftmachine_p, gridfunction_p;
  casacore::Bool wfGridding_p;
  casacore::Long cache_p;
  casacore::Int  tile_p;
  casacore::MPosition mLocation_p;
  casacore::Bool doVP_p;
  casacore::ImageBeamSet beam_p;
  casacore::Bool beamValid_p;
  casacore::Float padding_p;
  casacore::Float sdScale_p;
  casacore::Float sdWeight_p;
  casacore::Int sdConvSupport_p;

  casacore::Quantity qtruncate_p;
  casacore::Quantity qgwidth_p;
  casacore::Quantity qjwidth_p;

  casacore::Float minWeight_p;

  casacore::Bool clipminmax_p;

  // special mf control parms, etc
  casacore::Float cyclefactor_p;
  casacore::Float cyclespeedup_p;
  casacore::Float cyclemaxpsffraction_p;
  casacore::Int stoplargenegatives_p;
  casacore::Int stoppointmode_p;
  casacore::Vector<casacore::String> fluxscale_p;
  casacore::String scaleType_p;		// type of image-plane scaling: NONE, SAULT
  casacore::Float minPB_p;		// minimum value of generalized-PB pattern
  casacore::Float constPB_p;		// above this level, constant flux-scale

  casacore::Vector<casacore::Int> spectralwindowids_p;
  casacore::Int fieldid_p;

  casacore::Vector<casacore::Int> dataspectralwindowids_p;
  casacore::Vector<casacore::Int> datadescids_p;
  casacore::Vector<casacore::Int> datafieldids_p;
  //TT
  casacore::Cube<casacore::Int> spwchansels_p;
  casacore::Matrix<casacore::Double> freqrange_p;
  casacore::Matrix<casacore::Double> mssFreqSel_p;
  casacore::Matrix<casacore::Int> mssChanSel_p;

  casacore::Int numMS_p;

  casacore::String telescope_p;
  casacore::String vpTableStr_p;         // description of voltage patterns for various telescopes
                               //  in the MS
  casacore::Quantity parAngleInc_p;
  casacore::Quantity skyPosThreshold_p;
  BeamSquint::SquintType  squintType_p;
  casacore::Bool doDefaultVP_p;          // make default VPs, rather than reading in a vpTable


  casacore::Bool  doMultiFields_p;      // Do multiple fields?
  casacore::Bool  multiFields_p; 	      // multiple fields have been specified in setdata

  casacore::Bool doWideBand_p;          // Do Multi Frequency Synthesis Imaging
  casacore::String freqInterpMethod_p; //frequency interpolation mode

  casacore::Bool flatnoise_p;

  // Set the defaults
  void defaults();

  // check if it is  dettahced from ms.
  casacore::Bool detached() const;

  // Create the FTMachines when necessary or when the control parameters
  // have changed. 
  virtual casacore::Bool createFTMachine();

  void openSubTable (const casacore::Table & otherTable, casacore::Table & table, const casacore::TableLock & tableLock);

  casacore::Bool removeTable(const casacore::String& tablename);
  casacore::Bool updateSkyModel(const casacore::Vector<casacore::String>& model,
		      const casacore::String complist);
  casacore::Bool createSkyEquation(const casacore::String complist="");
  casacore::Bool createSkyEquation(const casacore::Vector<casacore::String>& image, 
			 const casacore::Vector<casacore::Bool>& fixed,
			 const casacore::String complist="");
  casacore::Bool createSkyEquation(const casacore::Vector<casacore::String>& image, 
			 const casacore::String complist="");
  casacore::Bool createSkyEquation(const casacore::Vector<casacore::String>& image, 
			 const casacore::Vector<casacore::Bool>& fixed,
			 const casacore::Vector<casacore::String>& mask,
			 const casacore::String complist="");
  casacore::Bool createSkyEquation(const casacore::Vector<casacore::String>& image, 
			 const casacore::Vector<casacore::Bool>& fixed,
			 const casacore::Vector<casacore::String>& mask,
			 const casacore::Vector<casacore::String>& fluxMask,
			 const casacore::String complist="");
  ATerm* createTelescopeATerm(casacore::MeasurementSet& ms, const casacore::Bool& isATermOn=true);
  void destroySkyEquation();

  //add residual to the private vars or create residual images
  casacore::Bool addResiduals(const casacore::Vector<casacore::String>& residual);
  // Add the residuals to the SkyEquation
  casacore::Bool addResidualsToSkyEquation(const casacore::Vector<casacore::String>& residual);

  // Add or replace the masks
  casacore::Bool addMasksToSkyEquation(const casacore::Vector<casacore::String>& mask, const casacore::Vector<casacore::Bool>& fixed=casacore::Vector<casacore::Bool>(0));

  // Get the rest frequency ..returns 1 element in restfreq 
  // if user specified or try to get the info from the SOURCE table 
  casacore::Bool getRestFreq(casacore::Vector<casacore::Double>& restFreq, const casacore::Int& spw);

  casacore::Bool restoreImages(const casacore::Vector<casacore::String>& restored, casacore::Bool modresiduals=true);

  // names of flux scale images
  casacore::Bool writeFluxScales(const casacore::Vector<casacore::String>& fluxScaleNames);

  // Helper functions to hide some setjy code.
  casacore::Unit sjy_setup_arrs(casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& returnFluxes,
                      casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& returnFluxErrs,
                      casacore::Vector<casacore::Vector<casacore::Double> >& fluxUsed, // mainly for logging purpose
                      casacore::Vector<casacore::String>& tempCLs,
                      casacore::Vector<casacore::Vector<casacore::MFrequency> >& mfreqs,
                      const casacore::MSSpWindowColumns& spwcols, const casacore::uInt nspws,
                      const casacore::Vector<casacore::Int>& selToRawSpwIds, const casacore::Bool chanDep);
  // Returns whether it might have made any visibilities.
  casacore::Bool sjy_make_visibilities(casacore::TempImage<casacore::Float> *tmodimage, casacore::LogIO& os,
  //casacore::Bool sjy_make_visibilities(casacore::Block<casacore::CountedPtr<casacore::TempImage<casacore::Float> > >& tmodimages, casacore::LogIO& os,
                             //const casacore::Int rawspwid, const casacore::Int fldid,
  // for new one                           
  //                           const casacore::Vector<casacore::Int>& rawspwids, const casacore::Int fldid,
                             const casacore::Int rawspwid, const casacore::Int fldid,
                             const casacore::String& clname, const casacore::String& timerange="",
                             const casacore::String& scanstr="", 
			     const casacore::String& obsidstr="",
                             const casacore::String& intentstr="", 
			    const casacore::Vector<casacore::Double>& freqofscale=casacore::Vector<casacore::Double>(0),
			     const casacore::Vector<casacore::Double>& scale=casacore::Vector<casacore::Double>(0) );
  // Concatenate multiple CLs 
  casacore::Bool sjy_concatComponentLists(casacore::LogIO& os, const casacore::Vector<casacore::String>& tempCLs, const casacore::String& outTempCL);
  // Returns whether it found a source.
  casacore::Bool sjy_computeFlux(casacore::LogIO& os, FluxStandard& fluxStd,
                       casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& returnFluxes,
                       casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& returnFluxErrs,
                       casacore::Vector<casacore::String>& tempCLs, 
                       //casacore::Vector<casacore::Double>& fluxUsed,
                       casacore::Vector<casacore::Vector<casacore::Double> >& fluxUsed,
                       casacore::String& fluxScaleName, casacore::MEpoch& aveEpoch,
                       const casacore::Vector<casacore::Vector<casacore::MFrequency> >& mfreqs,
                       const casacore::String& model, const casacore::String& fieldName, 
                       const casacore::MSColumns& msc, const casacore::Int fldid,
                       const casacore::MDirection& fieldDir, const casacore::Vector<casacore::Int>& selToRawSpwIds,
                       const casacore::String& standard);

  void sjy_makeComponentList(casacore::LogIO& os, casacore::Vector<casacore::String>& tempCLs,
                             casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& returnFluxes,
                             const casacore::Vector<casacore::Double>& fluxUsed,
                             const casacore::Vector<casacore::Int>& selToRawSpwIds,
                             const casacore::Vector<casacore::Vector<casacore::MFrequency> >& mfreqs,
                             const casacore::String& fieldName,
                             const casacore::MDirection& fieldDir,
                             const casacore::Vector<casacore::Double>& spix,
                             const casacore::Vector<casacore::Double>& pipars,
                             const casacore::Vector<casacore::Double>& papars,
                             const casacore::Double& rotMeas,
                             //const casacore::Vector<casacore::Double>& cppars,
                             const casacore::MFrequency& reffreq,
                             const casacore::MEpoch& aveEpoch,
                             const casacore::Int fldId);
  //
  // Returns NULL if no image is prepared.
  casacore::TempImage<casacore::Float>* sjy_prepImage(casacore::LogIO& os, FluxStandard& fluxStd,
                                  casacore::Vector<casacore::Double>& fluxUsed, 
				  casacore::Vector<casacore::Double>& freq, 
				  casacore::Vector<casacore::Double>& scale, const casacore::String& model,
                                  const casacore::MSSpWindowColumns& spwcols,
                                  //const casacore::Int rawspwid, const casacore::Bool chanDep,
                                  const casacore::Vector<casacore::Int> rawspwids, const casacore::Bool chanDep,
                                  const casacore::Vector<casacore::Vector<casacore::MFrequency> >& mfreqs,
                                  //const casacore::uInt selspw, const casacore::String& fieldName,
                                  const casacore::String& fieldName,
                                  const casacore::MDirection& fieldDir, const casacore::Unit& freqUnit,
                                  const casacore::Vector<casacore::Double>& fluxdens,
                                  const casacore::Bool precompute, 
                                  //const casacore::Double spix,
                                  const casacore::Vector<casacore::Double>& spix,
                                  const casacore::MFrequency& reffreq, 
				  const casacore::MEpoch& aveEpoch, const casacore::Int fieldId);
  // Returns true or throws up.
  casacore::Bool sjy_regridCubeChans(casacore::TempImage<casacore::Float>* tmodimage,
                           casacore::PagedImage<casacore::Float>& modimage, casacore::Int freqAxis);

  // set a radius limit if the model image is one of the known source
  // otherwise simply copy modimage to tmodimage
  casacore::Bool sjy_setRadiusLimit(casacore::TempImage<casacore::Float>* tmodimage,
                          casacore::PagedImage<casacore::Float>& modimage, const casacore::String& model,
                          casacore::DirectionCoordinate& dircsys);

  casacore::Bool sjy_calciflux(const casacore::Vector<casacore::MFrequency>& freqs, const casacore::MFrequency& reffreq, 
                     const casacore::Double refflux, const casacore::Vector<casacore::Double>& vspix, casacore::Vector<casacore::Double>& iflux);

  casacore::Bool sjy_calcquflux(const casacore::Vector<casacore::Double>& pipars, const casacore::Vector<casacore::Double>& papars,
                      const casacore::Vector<casacore::Double>& iflux, const casacore::Double rotMeas,
                      const casacore::Vector<casacore::MFrequency>& freqs, 
                      const casacore::MFrequency& reffreq, casacore::Vector<casacore::Double>& qflux,
                      casacore::Vector<casacore::Double>& uflux);

  casacore::String imageName();

  casacore::Bool pbguts(casacore::ImageInterface<casacore::Float>& in,  
	      casacore::ImageInterface<casacore::Float>& out, 
	      const casacore::MDirection&,
	      const casacore::Quantity&);

  // Helper func for printing clean's restoring beam to the logger.  May find
  // the restoring beam as a side effect, so sm_p can't be const.
  void printbeam(CleanImageSkyModel *sm_p, casacore::LogIO &os, const casacore::Bool firstrun=true);

  // Helper func for createFTMachine().  Returns phaseCenter_p as a casacore::String,
  // *assuming* it is set.  It does not check!
  casacore::String tangentPoint();
  

  casacore::Bool assertDefinedImageParameters() const;
 // Virtual methods to set the ImageSkyModel and SkyEquation.
  // This allows derived class pimager to set parallelized
  // specializations.
  //
  virtual void setWFCleanImageSkyModel() 
    {sm_p = new WFCleanImageSkyModel(facets_p, wfGridding_p); return;}; 
    
  virtual void setClarkCleanImageSkyModel()
    {sm_p = new ClarkCleanImageSkyModel(); return;};
  virtual void setSkyEquation();
    
  virtual void savePSF(const casacore::Vector<casacore::String>& psf);

  casacore::String frmtTime(const casacore::Double time);

  //copy imageregion to pixels on image as value given
  static casacore::Bool regionToMask(casacore::ImageInterface<casacore::Float>& maskImage, casacore::ImageRegion& imagreg, const casacore::Float& value=1.0);

  //set the mosaic ft machine and right convolution function
  virtual void setMosaicFTMachine(casacore::Bool useDoublePrec=false); 

  // Makes a component list on disk containing cmp (with fluxval and cspectrum)
  // named msname_p.fieldName.spw<spwid>.tempcl and returns the name.
  casacore::String makeComponentList(const casacore::String& fieldName, const casacore::Int spwid,
                           const Flux<casacore::Double>& fluxval,
                           const ComponentShape& cmp,
                           const ConstantSpectrum& cspectrum) const;

  casacore::Vector<casacore::Int> decideNPolPlanes(casacore::Bool checkwithMS);

  //returns if mLocation_p is set (= non-default casacore::MPosition)
  casacore::Bool nonDefaultLocation();
 
  // implementation of mapExtent
  casacore::Bool getMapExtent(const casacore::MeasurementSet &ms,
          const casacore::String &referenceFrame, const casacore::String &movingSource,
          const casacore::String &pointingColumn, casacore::Vector<casacore::Double> &center, casacore::Vector<casacore::Double> &blc,
          casacore::Vector<casacore::Double> &trc, casacore::Vector<casacore::Double> &extent);

  ComponentList* componentList_p;

  casacore::String scaleMethod_p;   // "nscales"   or  "uservector"
  casacore::Int nscales_p;
  casacore::Int ntaylor_p;
  casacore::Double reffreq_p;
  casacore::Bool useNewMTFT_p;
  casacore::Vector<casacore::Float> userScaleSizes_p;
  casacore::Bool scaleInfoValid_p;  // This means that we have set the information, not the scale beams
  casacore::Float smallScaleBias_p; //ms-clean
  casacore::Int nmodels_p;
  // Everything here must be a real class since we make, handle and
  // destroy these.
  casacore::Block<casacore::CountedPtr<casacore::PagedImage<casacore::Float> > > images_p;
  casacore::Block<casacore::CountedPtr<casacore::PagedImage<casacore::Float> > > masks_p;
  casacore::Block<casacore::CountedPtr<casacore::PagedImage<casacore::Float> > > fluxMasks_p;
  casacore::Block<casacore::CountedPtr<casacore::PagedImage<casacore::Float> > > residuals_p;
  
  // Freq frame is good and valid conversions can be done (or not)
  casacore::Bool freqFrameValid_p;

  // Preferred complex polarization representation
  StokesImageUtil::PolRep polRep_p;

  //Whether to use model column or use it in memory on the fly
  casacore::Bool useModelCol_p;

  //Force single precision always
  casacore::Bool singlePrec_p;
  //sink used to store history mainly
  casacore::LogSink logSink_p;


  //
  // Objects required for pointing correction (ftmachine=PBWProject)
  //
  EPJones *epJ;
  casacore::String epJTableName_p, cfCacheDirName_p;
  casacore::Bool doPointing, doPBCorr, psTermOn_p, aTermOn_p, mTermOn_p, wbAWP_p, conjBeams_p;
  //SimplePlotterPtr plotter_p;
  casacore::Record interactiveState_p;

  //Track moving source stuff
  casacore::Bool doTrackSource_p;
  casacore::MDirection trackDir_p;
  casacore::String pointingDirCol_p;
  VisImagingWeight imwgt_p;

  // viewer connection
  int clean_panel_p;
  int image_id_p;
  int mask_id_p;
  int prev_image_id_p;
  int prev_mask_id_p;
  //numthreads
  casacore::Int numthreads_p;
  casacore::Bool avoidTempLatt_p;
  casacore::String projection_p;
};


} //# NAMESPACE CASA - END

#endif
