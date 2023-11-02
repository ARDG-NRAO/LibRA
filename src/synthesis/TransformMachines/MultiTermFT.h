//# MultiTermFT.h: Definition for MultiTermFT
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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

#ifndef SYNTHESIS_MULTITERMFT_H
#define SYNTHESIS_MULTITERMFT_H

#include <synthesis/TransformMachines/FTMachine.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/scimath/Mathematics/ConvolveGridder.h>
#include <casacore/lattices/Lattices/LatticeCache.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
//#include <synthesis/MeasurementComponents/SynthesisPeek.h>
#include <casacore/casa/OS/Timer.h>

namespace casacore{

class UVWMachine;
}

namespace casa { //# NAMESPACE CASA - BEGIN


class MultiTermFT : public FTMachine {
public:

  // Construct using an existing FT-Machine 
  MultiTermFT(FTMachine *subftm, casacore::String subFTMname, casacore::Int nterms=1, casacore::Double reffreq=0.0);

  // Construct from a casacore::Record containing the MultiTermFT state
  MultiTermFT(const casacore::RecordInterface& stateRec);

  // Copy constructor. 
  // This first calls the default "=" operator, and then instantiates objects for member pointers.
  MultiTermFT(const MultiTermFT &other);

  // Assignment operator --- leave it as the default
  MultiTermFT &operator=(const MultiTermFT &other);

  // Destructor
  ~MultiTermFT();

  // Called at the start of de-gridding : subftm->initializeToVis()
  // Note : Pre-de-gridding model-image divisions by PBs will go here.
  void initializeToVis(casacore::ImageInterface<casacore::Complex>& image, const VisBuffer& vb);

  // Called at the end of de-gridding : subftm->finalizeToVis()
  void finalizeToVis();

  // Called at the start of gridding : subftm->initializeToSky()
  void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,  casacore::Matrix<casacore::Float>& weight,  const VisBuffer& vb);

  // Called at the end of gridding : subftm->finalizeToSky()
  void finalizeToSky();

  // Do the degridding via subftm->get() and modify model-visibilities by Taylor-weights
  void get(VisBuffer& vb, casacore::Int row=-1);

  // Modify imaging weights with Taylor-weights and do gridding via subftm->put()
  void put(VisBuffer& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
	   FTMachine::Type type=FTMachine::OBSERVED);

  // Have a const version for compatibility with other FTMs.. Throw an exception if called.
  void put(const VisBuffer& /*vb*/, casacore::Int /*row=-1*/, casacore::Bool /*dopsf=false*/,
	   FTMachine::Type /*type=FTMachine::OBSERVED*/)
  {throw(casacore::AipsError("MultiTermFT::put called with a const vb. This FTM needs to modify the vb."));};

  // Calculate residual visibilities if possible.
  // The purpose is to allow rGridFT to make this multi-threaded
  virtual void ComputeResiduals(VisBuffer&vb, casacore::Bool useCorrected); 

  // Make an image : subftm->makeImage()
  void makeImage(FTMachine::Type type,
		 VisSet& vs,
		 casacore::ImageInterface<casacore::Complex>& image,
		 casacore::Matrix<casacore::Float>& weight);
  
  // Get the final image: do the Fourier transform grid-correct, then 
  // optionally normalize by the summed weights
  // Note : Post-gridding residual-image divisions by PBs will go here.
  //           For now, it just calls subftm->getImage()
  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true);

  // Place-holder for possible use with AWProject and AWProjectWB FTMs
  virtual void normalizeImage(casacore::Lattice<casacore::Complex>& /*skyImage*/,
			      const casacore::Matrix<casacore::Double>& /*sumOfWts*/,
			      casacore::Lattice<casacore::Float>& /*sensitivityImage*/,
			      casacore::Bool /*fftNorm*/)
    {throw(casacore::AipsError("MultiTermFT::normalizeImage() is not implemented"));}

  // Get the final weights image - this will hold PB2
  void getWeightImage(casacore::ImageInterface<casacore::Float>&, casacore::Matrix<casacore::Float>&);

  // Save and restore the MultiTermFT to and from a record
  virtual casacore::Bool toRecord(casacore::String& error, casacore::RecordInterface& outRec, casacore::Bool withImage=false, const casacore::String diskimage="");
  virtual casacore::Bool fromRecord(casacore::String& error, const casacore::RecordInterface& inRec);

  // Various small inline functions
  virtual casacore::Bool isFourier() {return true;}
  virtual void setNoPadding(casacore::Bool nopad){subftm_p->setNoPadding(nopad);};
  virtual casacore::String name() const {return machineName_p;};
  virtual void setMiscInfo(const casacore::Int qualifier){thisterm_p=qualifier;};

protected:

  // Multiply Imaging weights by Taylor-function weights - during "put"
  casacore::Bool modifyVisWeights(VisBuffer& vb);
  // Multiply model visibilities by Taylor-function weights - during "get"
  casacore::Bool modifyModelVis(VisBuffer &vb);
  // Restore vb.imagingweights to the original
  casacore::Bool restoreImagingWeights(VisBuffer &vb);
  // have to call the initmaps of subftm
  virtual void initMaps(const VisBuffer& vb);
  //// New MTFT specific internal parameters and functions
  casacore::CountedPtr<FTMachine> subftm_p; 
  casacore::String  subFTMname_p;
  casacore::Int nterms_p;
  casacore::Int thisterm_p;
  casacore::Double reffreq_p;

  casacore::Matrix<casacore::Float> imweights_p;
  casacore::Double sumwt_p;
  casacore::String machineName_p;

  casacore::Bool dbg_p,dotime_p;
  casacore::Timer tmr_p;
  casacore::Double time_get, time_put, time_res;
};

} //# NAMESPACE CASA - END

#endif
