//# Deconvolver: defines classes for deconvolver.
//# Copyright (C) 1996-2007
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
//#
//# $Id: DOdeconvolver.h,v 19.7 2005/03/18 01:43:31 kgolap Exp $

#ifndef SYNTHESIS_DECONVOLVER_H
#define SYNTHESIS_DECONVOLVER_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/measures/Measures/MRadialVelocity.h>
#include <casacore/lattices/LatticeMath/LatticeCleaner.h>
#include <casacore/scimath/Mathematics/GaussianBeam.h>

#include <synthesis/MeasurementEquations/MultiTermMatrixCleaner.h>

#include <casacore/casa/namespace.h>
namespace casacore{

template<class T> class Lattice;
template<class T> class PagedImage;
template<class T> class TempImage;
template<class T> class ImageInterface;
template<class T> class LatticeConvolver;
template<class T> class SubImage;
class File;
}

namespace casa { //# NAMESPACE CASA - BEGIN
template<class T> class ResidualEquation;


class CEMemModel;
class ClarkCleanLatModel;
class LatConvEquation;
class ImageMSCleaner;
class ImageNACleaner;
// <summary> A simple deconvolver operating on images (no SkyEquation) </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="SkyEquation">SkyEquation</linkto>
//   <li> <linkto class="SkyModel">SkyModel</linkto>
// </prerequisite>
//
// <etymology>
// Undo convolution or at least try to do it !
// </etymology>
//
// <synopsis>
// This class is a container that allows many SkyComponents to be grouped
// together and manipulated as a group. In this respect this class is identical
// to the <linkto class="ComponentList">ComponentList</linkto> class. The user
// is encouraged to read the synopsis of that class for a general description
// of the capabilities of this class.
//
// This class is differs from the ComponentList class in the following ways:
// <ul>
// <li> All components are indexed starting at one. This means the first
//      component in this class is obtained by <src>component(1)</src> rather
//      than <src>component(0)</src> in the ComponentList class.
// <li> Copies of the components, rather than references, are returned to the
//      user. This means that this class needs a replace function whereas
//      ComponentList does not.
// <li> Components that have been removed from the list are stored in a
//      temporary place. In the ComponentList class once they are deleted they
//      are gone.
// <li> This class is derived from ApplicationObject and follows the AIPS++
//      conventions for "distributed objects". Hence the fuunctions in this
//      class can be made accessible from glish. 
// <li> This class can generate simulated components and add them to the list.
// </ul>
//
// There is a one-to-one correspondence between the functions in the glish
// componentlist object (see the AIPS++ User Reference manual) and functions in
// this class. This is make simplify the porting from glish to C++ of a glish
// script using the componentlist distributed object.
// </synopsis>
//
// <example>
// These examples are coded in the tDOcomponentlist.h file.
// <h4>Example 1:</h4>
// In this example a ComponentList object is created and used to calculate the
// ...
// <srcblock>
// </srcblock>
// </example>
//
// <motivation> 
// This class was written to make the componentlist classes usable from glish
// </motivation>
//
// <thrown>
// <li> casacore::AipsError - If an internal inconsistancy is detected, when compiled in 
// debug mode only.
// </thrown>
//
// <todo asof="1998/05/22">
//   <li> Nothing I hope. But I expect users will disagree.
// </todo>

class Deconvolver 
{
public:
  // "deconvolver" ctor
  Deconvolver();
  
  Deconvolver(const casacore::String& dirty, const casacore::String& psf);
  
  Deconvolver(const Deconvolver &other);
  Deconvolver &operator=(const Deconvolver &other);
  ~Deconvolver();

  // Open the given dirty image and psf
  // If warn is true,  print warnings about there being
  // no psf if one is not supplied.
  casacore::Bool open(const casacore::String& dirty, const casacore::String& psf, casacore::Bool warn=true);

  // After some cleaning, the dirty image is replaced with the
  // residual image in the deconvolver tool.  reopen reinstates
  // that dirty image; cannot be invoked before open has been
  // invoked
  casacore::Bool reopen();
  
  // Flush the ms to disk and detach from the ms file. All function
  // calls after this will be a no-op.
  casacore::Bool close();
  
  casacore::String dirtyname() const;
  casacore::String psfname() const;

  // Output a summary of the state of the object
  casacore::Bool summary() const;
  
  // Return the state of the object as a string
  casacore::String state() const;
  
  // Return the image shape
  casacore::IPosition imageshape() const;

  // Restore
  casacore::Bool restore(const casacore::String& model,
	       const casacore::String& image, casacore::GaussianBeam& mbeam);

  // Residual
  casacore::Bool residual(const casacore::String& model, 
	       const casacore::String& image);

  // Smooth
  casacore::Bool smooth(const casacore::String& model, 
	      const casacore::String& image,
	      casacore::GaussianBeam& mbeam,
	      casacore::Bool normalizeVolume);

  // Clean algorithm
  //maxResidual and iterationsDone are return values
  casacore::Bool clean(const casacore::String& algorithm,
	     const casacore::Int niter, const casacore::Float gain, const casacore::Quantity& threshold, 
	     const casacore::Bool displayProgress,
             const casacore::String& model, const casacore::String& mask, casacore::Float& maxResidual, 
	     casacore::Int& iterationsDone );
  casacore::Bool naclean(const casacore::Int niter,
			const casacore::Float gain, const casacore::Quantity& threshold, 
	       const casacore::String& model, const casacore::String& mask, const casacore::Int masksupp, const casacore::Int memoryType, const casacore::Float numSigma, casacore::Float& maxResidual, casacore::Int& iterationsDone) ;

  //Clark Clean but image, psf, mask has to be 4-axes in the canonical casa order.
  //Useful for cleaning dirty images made in CASA
  //if mask is larger than a quarter of the image it will do a full image clean ...unlike the one below
  casacore::Bool clarkclean(const casacore::Int niter, 
		  const casacore::Float gain, const casacore::Quantity& threshold, 
		  const casacore::String& model, const casacore::String& maskName,
		  casacore::Float& maxresid, casacore::Int& iterused,
		  casacore::Float cycleFactor=1.5);

  // Clark Clean algorithm
  casacore::Bool clarkclean(const casacore::Int niter, 
		  const casacore::Float gain, const casacore::Quantity& threshold, 
		  const casacore::Bool displayProgress, 
		  const casacore::String& model, const casacore::String& mask,
		  const casacore::Int histBins, 
		  const casacore::Vector<casacore::Int>& psfPatchSize, const casacore::Float maxExtPsf,
		  const casacore::Float speedUp, casacore::Int maxNumPix,
		  const casacore::Int maxNumMajorCycles,
		  const casacore::Int maxNumMinorIterations);
  
  
  // MEM algorithm    add other inputs as required
  casacore::Bool mem(const casacore::String& algorithm,
	   const casacore::Int niter, const casacore::Quantity& sigma, 
	   const casacore::Quantity& targetFlux, 
	   casacore::Bool constrainTargetFlux, 
	   casacore::Bool displayprogress, 
	   const casacore::String& model, 
	   const casacore::String& prior = "",
	   const casacore::String& mask = "",
	   const casacore::Bool imagePlane = false);
  
  // make a prior image
  casacore::Bool makeprior(const casacore::String& prior,
		 const casacore::String& templateImage,
		 const casacore::Quantity& lowClipfrom, 
		 const casacore::Quantity& lowClipto, 
		 const casacore::Quantity& highClipfrom, 
		 const casacore::Quantity& highClipto, 
		 const casacore::Vector<casacore::Int>& blc,
		 const casacore::Vector<casacore::Int>& trc);
  
  // Set up scales: based on scaleMethod = "nscales" or "uservector",
  // we will create the scale sizes in pixels via a power law or
  // use the user specified scale sizes.
  casacore::Bool setscales(const casacore::String& scaleMethod, const casacore::Int nscales, 
		 const casacore::Vector<casacore::Float>& userScaleSizes);
  
  // NNLS algorithm
  casacore::Bool nnls(const casacore::String& algorithm, const casacore::Int niter, const casacore::Float tolerance,
	    const casacore::String& model, 
	    const casacore::String& fluxMask, const casacore::String& dataMask);

  // Fourier transform the model and componentlist
  casacore::Bool ft(const casacore::String& model, const casacore::String& transform);

  // Make an empty image
  casacore::Bool make(const casacore::String& model);

  // Make an empty image with just one casacore::Stokes pixel (ie, for a mask)
  casacore::Bool make1(const casacore::String& imagename);

  // Make an empty image modeled after templateImage
  casacore::Bool make(const casacore::String& model, casacore::ImageInterface<casacore::Float>& templateImage);

  // Make a Box Mask
  casacore::Bool boxmask(const casacore::String& boxmask,
	       const casacore::Vector<casacore::Int> blc,
	       const casacore::Vector<casacore::Int> trc,
	       const casacore::Quantity& fillValue=1.0,
	       const casacore::Quantity& externalValue=0.0);

  //make a mask image from regions
  casacore::Bool regionmask(const casacore::String& maskimage, casacore::Record* imageRegRec, 
		  casacore::Matrix<casacore::Quantity>& blctrcs, const casacore::Float& value=1.0);

  // Clip an image below some casacore::Stokes I threshold
  casacore::Bool clipimage(const casacore::String& clippedImage, const casacore::String& inputImage,
		 const casacore::Quantity& threshold);

  // Fit the psf
  casacore::Bool fitpsf(const casacore::String& psf, casacore::GaussianBeam& beam);

  // Convolve one image with another
  casacore::Bool convolve(const casacore::String& convolvedmodel, 
		const casacore::String& model);

  // Make a Gaussian -- you might want to use it for convolution, etc
  casacore::Bool makegaussian(const casacore::String& gaussianimage, casacore::GaussianBeam& mbeam, casacore::Bool normalizeVolume);

  // ------------------  Multi-Term Deconvolver functions - START ----------------------
  // Initialize the Multi-Term casacore::Matrix Cleaners and compute Hessian elements.
  casacore::Bool mtopen(const casacore::Int nTaylor,
	      const casacore::Vector<casacore::Float>& userScaleSizes,
	      const casacore::Vector<casacore::String>& psfs);

  // Do component-finding iterations
  casacore::Bool mtclean(const casacore::Vector<casacore::String>& residuals,
	       const casacore::Vector<casacore::String>& models,
	       const casacore::Int niter,
	       const casacore::Float gain, 
	       const casacore::Quantity& threshold, 
	       const casacore::Bool displayProgress,
	       const casacore::String& mask, 
	       casacore::Float& maxResidual, casacore::Int& iterationsDone);

  // Restore the output images
  casacore::Bool mtrestore(const casacore::Vector<casacore::String>& models,
		 const casacore::Vector<casacore::String>& residuals,
		 const casacore::Vector<casacore::String>& images,
		 casacore::GaussianBeam& mbeam);

  // Calculate alpha and beta from restored images.
  casacore::Bool mtcalcpowerlaw(const casacore::Vector<casacore::String>& images,
                      const casacore::Vector<casacore::String>& residuals,
                      const casacore::String& alphaname,
                      const casacore::String& betaname,
                      const casacore::Quantity& threshold,
                      const casacore::Bool calcerror);


  // ------------------  Multi-Term Deconvolver functions - END ----------------------


private:
  
  // Cut the inner quarter out of an image
  casacore::SubImage<casacore::Float>* innerQuarter(casacore::PagedImage<casacore::Float>& in);

  // Return full image as a SubImage
  casacore::SubImage<casacore::Float>* allQuarters(casacore::PagedImage<casacore::Float>& in);

  // Clone an image
  casacore::Bool clone(const casacore::String& imageName, const casacore::String& newImageName);
  

  //find which axes are the spectral and pol one
  void findAxes();

  //check mask

  void checkMask(casacore::ImageInterface<casacore::Float>& maskimage, casacore::Int& xbeg, casacore::Int& xend, 
		 casacore::Int& ybeg, casacore::Int& yend);


  // setup lattice cleaner
  casacore::Bool setupLatCleaner(const casacore::String& algorithm,
		       const casacore::Int niter, const casacore::Float gain, const casacore::Quantity& threshold, 
		       const casacore::Bool displayProgress);

  // Embed a mask into an image. A convenience function.
  casacore::Bool createMask(casacore::LatticeExpr<casacore::Bool> &lemask, casacore::ImageInterface<casacore::Float> &outimage);
  //Put a unit peak gaussian at centre
  casacore::Bool putGaussian(casacore::ImageInterface<casacore::Float>& im, const casacore::GaussianBeam& beam);
  casacore::PagedImage<casacore::Float>* dirty_p;
  casacore::PagedImage<casacore::Float>* psf_p;

  casacore::LatticeConvolver<casacore::Float>* convolver_p;
  ResidualEquation<casacore::Lattice<casacore::Float> >* residEqn_p;
  LatConvEquation* latConvEqn_p;
  casacore::CountedPtr <ImageMSCleaner> cleaner_p;
  std::shared_ptr <ImageNACleaner> naCleaner_p;

  casacore::Bool scalesValid_p;

  casacore::Int nx_p, ny_p, npol_p, nchan_p;
  casacore::Int chanAxis_p, polAxis_p;
  casacore::String mode_p;
  casacore::GaussianBeam beam_p;
//  casacore::Quantity bmaj_p, bmin_p, bpa_p;
  casacore::Bool beamValid_p;
  casacore::String dirtyName_p;
  casacore::String psfName_p;
  casacore::Bool fullPlane_p;

  casacore::Vector<casacore::Float> itsTotalFluxScale;
  casacore::Float itsTotalFlux;
  casacore::Vector<casacore::Float> scaleSizes_p;

  // Multi-Term private variables
  casacore::Int mt_nterms_p;
  MultiTermMatrixCleaner mt_cleaner_p;
  casacore::Bool mt_valid_p;

  // Set the defaults
  void defaults();

  casacore::Bool removeTable(const casacore::String& tablename);
  
  // Prints an error message if the deconvolver DO is detached and returns true.
  casacore::Bool detached() const;

  casacore::String imageName() const;

  casacore::Bool valid() const;

};

} //# NAMESPACE CASA - END

#endif
