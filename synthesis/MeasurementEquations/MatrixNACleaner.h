//# MatrixNACleaner.h: this defines Cleaner a class for doing deconvolution
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
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


#ifndef SYNTHESIS_MATRIXNACLEANER_H
#define SYNTHESIS_MATRIXNACLEANER_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations

// <summary>A simple deconvolver that masks by memory of previous peaks</summary>
// <synopsis> Evolution of a simple cleaner 
// 
// </synopsis>

// <summary>A class for doing multi-dimensional cleaning</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="Mulouktanee">
// </reviewed>

// <prerequisite>
//  <li> The mathematical concept of deconvolution
// </prerequisite>
//
// <etymology>

// The MatrixNACleaner Uses the matrix class and cleaner is for deconvolver by CLEAN
// NA stands for Non Amnesiac...i.e remember where past peaks were to search in these region preferentially ...i.e no need to mask 

// </etymology>
//
// <synopsis>
// This class will perform various types of Clean deconvolution
// on Lattices.
//
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock> 
// </example>
//
// <motivation>
// </motivation>
//
// <thrown>
// <li> casacore::AipsError: if psf has more dimensions than the model. 
// </thrown>
//
// <todo asof="yyyy/mm/dd">
// </todo>

class MatrixNACleaner
{
public:

  // Create a cleaner : default constructor
  MatrixNACleaner();

  // Create a cleaner for a specific dirty image and PSF
  MatrixNACleaner(const casacore::Matrix<casacore::Float> & psf, const casacore::Matrix<casacore::Float> & dirty, const casacore::Int memtype=2, const casacore::Float numSigma=5.0);

  // The copy constructor uses reference semantics
  MatrixNACleaner(const MatrixNACleaner& other);

  // The assignment operator also uses reference semantics
  MatrixNACleaner & operator=(const MatrixNACleaner & other); 

  // The destructor does nothing special.
  ~MatrixNACleaner();
 
  
  
  //Set the dirty image without calculating convolutions..
  //can be done by calling  makeDirtyScales or setscales if one want to redo the 
  //psfscales too.
  void setDirty(const casacore::Matrix<casacore::Float>& dirty);
  
  

  //change the psf
  //don't forget to redo the setscales or run makePsfScales, 
  //followed by makeDirtyScales 
  void setPsf(const casacore::Matrix<casacore::Float>& psf);
 
 
 
  // niter - number of iterations
  // gain - loop gain used in cleaning (a fraction of the maximum 
  //        subtracted at every iteration)
  // aThreshold - absolute threshold to stop iterations
 
  void setcontrol(const casacore::Int niter,
		  const casacore::Float gain, const casacore::Quantity& aThresho, const casacore::Int masksupport=3 , const casacore::Int memType=2, const casacore::Float numsigma=5.0);

 

  // return how many iterations we did do
  casacore::Int iteration() const { return itsIteration; }
 

 
 //Total flux accumulated so far
  casacore::Float totalFlux() const {return itsTotalFlux;}


  // Clean an image.
 
  casacore::Int clean(casacore::Matrix<casacore::Float> & model);

  // Set the mask
  // mask - input mask lattice
  
  void setMask(casacore::Matrix<casacore::Float> & mask);
 

  void setPixFlag(const casacore::Matrix<casacore::Bool>& bitpix);

  // remove the mask;
  // useful when keeping object and sending a new dirty image to clean 
  // one can set another mask then 
  void unsetMask();

  void getMask(casacore::Matrix<casacore::Float>& mask);

  

  // Look at what WE think the residuals look like
  casacore::Matrix<casacore::Float>  getResidual() { return *itsResidual; }

  // Method to return threshold
  casacore::Float threshold() const;

  
  casacore::Float maxResidual() {return itsMaximumResidual;};
  // Helper function to optimize adding
  //static void addTo(casacore::Matrix<casacore::Float>& to, const casacore::Matrix<casacore::Float> & add);

protected:
  // Make sure that the peak of the Psf is within the image
  casacore::Bool validatePsf(const casacore::Matrix<casacore::Float> & psf);

 
  // Find the Peak of the lattice, applying a mask
  casacore::Bool findMaxAbsMask(const casacore::Matrix<casacore::Float>& lattice,  casacore::Matrix<casacore::Float>& mask,
		      casacore::Float& maxAbs, casacore::IPosition& posMax, const casacore::Int support=5);

  // Helper function to reduce the box sizes until the have the same   
  // size keeping the centers intact  
  static void makeBoxesSameSize(casacore::IPosition& blc1, casacore::IPosition& trc1,                               
     casacore::IPosition &blc2, casacore::IPosition& trc2);


  casacore::Float itsGain;
  casacore::Int itsMaxNiter;      // maximum possible number of iterations
  casacore::Quantum<casacore::Double> itsThreshold;
  

private:

  //# The following functions are used in various places in the code and are
  //# documented in the .cc file. Static functions are used when the functions
  //# do not modify the object state. They ensure that implicit assumptions
  //# about the current state and implicit side-effects are not possible
  //# because all information must be supplied in the input arguments

  std::shared_ptr<casacore::Matrix<casacore::Float> > itsMask;
  std::shared_ptr<casacore::Matrix<casacore::Float> > itsDirty;
  std::shared_ptr<casacore::Matrix<casacore::Float> > itsPsf;
  std::shared_ptr<casacore::Matrix<casacore::Float> >itsResidual;
  std::shared_ptr<casacore::Matrix<casacore::Bool> > itsBitPix;

  casacore::Float amnesiac(const casacore::Float& val);
  casacore::Float weak(const casacore::Float& v);
  casacore::Float medium(const casacore::Float& v);
  casacore::Float strong(const casacore::Float& v);

  casacore::Float itsMaximumResidual;
  casacore::Int itsIteration;	// what iteration did we get to?
  casacore::Int itsStartingIter;	// what iteration did we get to?
  casacore::Float itsTotalFlux;
  casacore::Int itsSupport;
  casacore::IPosition psfShape_p;
  casacore::IPosition itsPositionPeakPsf;
  casacore::Float itsRms;
  casacore::Int typeOfMemory_p;  //0 nomemory, 1 weak, 2 medium, 3 strong
  casacore::Float numSigma_p;
  std::function<casacore::Float(const casacore::Float&)> f_p;
};

} //# NAMESPACE CASA - END

#endif
