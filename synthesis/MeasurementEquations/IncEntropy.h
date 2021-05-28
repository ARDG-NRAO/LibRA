//# IncEntropy.h: this defines the virtual base class for Incremental Entropy
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#ifndef SYNTHESIS_INCENTROPY_H
#define SYNTHESIS_INCENTROPY_H

#include <casa/aips.h>
#include <lattices/Lattices/Lattice.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Array.h>
#include <casa/BasicSL/String.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//forward declaration
class IncCEMemModel;

// <summary>Base class for incremental entropies used by incremental MEM algorithm</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite> 
// <li> IncCEMemModel
// </prerequisite>
//
// <etymology>
// This class is called Entropy because it encapsulates the required
// functionality of the entropy in the CE MEM algorithm.  Inc is from
// incremental, as the dirty image specifies an increment on a previous
// major cycle's image, and the entropy is applied to the full
// image + deltaImage.
// </etymology>
//
// <synopsis>
// Provide the generic interface to entropy functions.
//
// We calculate entropy, gradients, and Hessians (diagonal) of the entropy.
// For efficiency reasons, we cannot restrict the methods to these calculations,
// but must also subsume the loops over image pixels in which they are
// used.  In this way, the Entropy classes form a tight partnership with
// the MemModel classes, taking over more responcibility than strict
// functional encapsulation requires.
// 
// This class heirarchy is used by CEMemModel, which implements
// the Cornwell-Evans Maximum Entropy algorithm.
//
// In the Entropy constructor, we create a pointer to the CEMemModel for
// reference to its Mem image, prior image, and default levels.
// Since each sort of Entropy is a friend of the CEMemModel, it
// has access to its private data.  However, we vow here NOT to
// touch it, just to look at it.  Could have done read-only access,
// but too lazy.
//
// </synopsis>
//
// <example>
// <srcblock>
// EntropyI myEntropyEngine(myCEMemModel&);
//
// casacore::Float  theEntropy myEntropyEngine.getEntropy();
// </srcblock> 
// </example>
//
// <motivation>
// This class is needed to encapsulate the methods of different
// functional forms of the entropy, used by Maximum Entropy (MEM)
// deconvolution algorithms.
// </motivation>
//
//
// <todo asof="1998/08/02">
//   <li> Nothing done yet!
// </todo>


// virtual base class
class IncEntropy
{
 public:
  
  // The default constructor is good enough, does nothing.
  // the MemImage and Prior image are stored in the MemModel.
  IncEntropy();

  
  // A virtual destructor may be necessary for use in derived classes.
  virtual ~IncEntropy();

  
  // calculate the entropy for the whole image
  virtual casacore::Float formEntropy() = 0;
  
  // calculate the Gradient dot Gradient matrix
  virtual void formGDG(casacore::Matrix<double> & ) = 0;
  
  // calculate the Gradient dot Gradient matrix, calculate Step
  virtual void formGDGStep(casacore::Matrix<double> & ) = 0;
  
  // calculate Gradient dot Step
  virtual casacore::Double formGDS() = 0;
  
  // report the entropy type for a logging message
  virtual void  entropyType(casacore::String &) = 0;
  // report the entropy name
  virtual void  entropyName(casacore::String &) = 0;
  
  // set the MemModel
  void setMemModel(IncCEMemModel& mmm) { cemem_ptr = &mmm; }

  // infoBanner
  virtual void infoBanner() = 0;

  // infoPerIteration
  virtual void infoPerIteration(casacore::uInt iteration) = 0;

  // are there any constraints on how the Image minimum
  // gets relaxed?
  virtual casacore::Float relaxMin() = 0;
  
  // each entropy type can have its distinct convergence
  // criteria
  virtual casacore::Bool testConvergence() = 0;

 protected:

  
  enum GRADTYPE {H=0, C, F, J };
  
  
  IncCEMemModel *cemem_ptr;
  
  IncEntropy(const IncEntropy &);

  

};


// <summary>Thermodynamic or Information entropy for incremental MEM</summary>

class IncEntropyI : public IncEntropy
{
public:
  
  // This default constructor is good enough for me.
  IncEntropyI();
  
  // destructor
  ~IncEntropyI();

  // calculate the entropy for the whole image
  casacore::Float formEntropy();
  
  // calculate the Gradient dot Gradient matrix
  void formGDG(casacore::Matrix<casacore::Double>& );
  
  // calculate the Gradient dot Gradient matrix, calculate Step
  void formGDGStep(casacore::Matrix<double> & );
  
  // calculate Gradient dot Step
  casacore::Double formGDS();
  
  // report the entropy type for a logging message
  void entropyType(casacore::String & str) 
    { str = "entropy type I (information/thermodynamic)"; }
  // report the entropy name
  void entropyName(casacore::String & str) 
    { str = "ENTROPY"; }
  
  // infoBanner
  void infoBanner();

  // infoIteration
  void infoPerIteration(casacore::uInt iteration);

  // relax image Min
  casacore::Float relaxMin();

  // each entropy type can have its distinct convergence
  // criteria
  casacore::Bool testConvergence();

protected:
  
  IncEntropyI(const IncEntropyI& );
  IncEntropyI& operator=(const IncEntropyI& );

};



// <summary>Emptiness measure for incremental MEM</summary>
class IncEntropyEmptiness : public IncEntropy
{
public:
  
  // This default constructor is good enough for me.
  IncEntropyEmptiness();
  
  // destructor
  ~IncEntropyEmptiness();

  // calculate the entropy for the whole image
  casacore::Float formEntropy();
  
  // calculate the Gradient dot Gradient matrix
  void formGDG(casacore::Matrix<casacore::Double>& );
  
  // calculate the Gradient dot Gradient matrix, calculate Step
  void formGDGStep(casacore::Matrix<double> & );
  
  // calculate Gradient dot Step
  casacore::Double formGDS();
  
  // report the entropy type for a logging message
  void entropyType(casacore::String & str) 
    { str = "entropy type U (emptiness)"; }
  // report the entropy Name
  void entropyName(casacore::String & str) 
    { str = "EMPTINESS"; }
  
  // infoBanner
  void infoBanner();

  // infoIteration
  void infoPerIteration(casacore::uInt iteration);

  // relax image Min
  casacore::Float relaxMin();

  // each entropy type can have its distinct convergence
  // criteria
  casacore::Bool testConvergence();

protected:
  
  IncEntropyEmptiness(const IncEntropyEmptiness& );
  IncEntropyEmptiness& operator=(const IncEntropyEmptiness& );

};






} //# NAMESPACE CASA - END

#endif
