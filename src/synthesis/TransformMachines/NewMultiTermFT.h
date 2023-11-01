//# NewMultiTermFT.h: Definition for NewMultiTermFT
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

#ifndef SYNTHESIS_NEWMULTITERMFT_H
#define SYNTHESIS_NEWMULTITERMFT_H

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


class NewMultiTermFT : public FTMachine {
public:

  // Construct using an existing FT-Machine 
  NewMultiTermFT(FTMachine *subftm, casacore::Int nterms=1, casacore::Double reffreq=0.0);

  // Construct from a casacore::Record containing the NewMultiTermFT state
  NewMultiTermFT(const casacore::RecordInterface& stateRec);

  // Copy constructor. 
  // This first calls the default "=" operator, and then instantiates objects for member pointers.
  NewMultiTermFT(const NewMultiTermFT &other);

  // Assignment operator --- leave it as the default
  NewMultiTermFT &operator=(const NewMultiTermFT &other);

  // Destructor
  ~NewMultiTermFT();

  // Called at the start of de-gridding : subftm->initializeToVis()
  // Note : Pre-de-gridding model-image divisions by PBs will go here.
  void initializeToVis(casacore::ImageInterface<casacore::Complex>& /*image*/, 
                       const VisBuffer& /*vb*/)
  {throw(casacore::AipsError("NewMultiTermFT::initializeToVis called without vectors !"));};

   // Vectorized InitializeToVis
  void initializeToVis(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec,casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & modelImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& weightImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& fluxScaleVec, casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec, const VisBuffer& vb);

  // Called at the end of de-gridding : subftm->finalizeToVis()
  void finalizeToVis();

  // Called at the start of gridding : subftm->initializeToSky()
  void initializeToSky(casacore::ImageInterface<casacore::Complex>& /*image*/,  
                       casacore::Matrix<casacore::Float>& /*weight*/, const VisBuffer& /*vb*/) 
   {throw(casacore::AipsError("NewMultiTermFT::initializeToSky() called without vectors!"));};

  void initializeToSky(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec, casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec, const VisBuffer& vb, const casacore::Bool dopsf);


  // Called at the end of gridding : subftm->finalizeToSky()
  void finalizeToSky(){throw(casacore::AipsError("NewMultiTermFT::finalizeToSky() called without arguments!"));};

  void finalizeToSky(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & resImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& weightImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& fluxScaleVec, casacore::Bool dopsf, casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec, const VisBuffer& vb);

  //  void normalizeToSky(casacore::ImageInterface<casacore::Complex>& compImage, casacore::ImageInterface<casacore::Float>& resImage, casacore::ImageInterface<casacore::Float>& weightImage, casacore::Bool dopsf, casacore::Matrix<casacore::Float>& weights)
  // {throw(casacore::AipsError("NewMultiTermFT::normalizeToSky should not get called !"));};


  // Do the degridding via subftm->get() and modify model-visibilities by Taylor-weights
  void get(VisBuffer& vb, casacore::Int row=-1);

  // Modify imaging weights with Taylor-weights and do gridding via subftm->put()
  void put(VisBuffer& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
	   FTMachine::Type type=FTMachine::OBSERVED);

  // Have a const version for compatibility with other FTMs.. Throw an exception if called.
  void put(const VisBuffer& /*vb*/, casacore::Int /*row*/=-1, casacore::Bool /*dopsf*/=false,
	   FTMachine::Type /*type*/=FTMachine::OBSERVED)
  {throw(casacore::AipsError("NewMultiTermFT::put called with a const vb. This FTM needs to modify the vb."));};

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
  //  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>& weights, casacore::Bool normalize=true)
  //{return getImage(weights,normalize,0);};
  //casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>& weights, casacore::Bool normalize=true, 
  //                                                         const casacore::Int taylorindex=0);
  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>& /*weights*/, casacore::Bool /*normalize*/=true)
  {throw(casacore::AipsError("NewMultiTermFT::getImage() should not be called"));}
 

  // Place-holder for possible use with AWProject and AWProjectWB FTMs
  /*
  virtual void normalizeImage(casacore::Lattice<casacore::Complex>& skyImage,
			      const casacore::Matrix<casacore::Double>& sumOfWts,
			      casacore::Lattice<casacore::Float>& sensitivityImage,
			      casacore::Bool fftNorm)
    {throw(casacore::AipsError("NewMultiTermFT::normalizeImage() is not implemented"));}
  */
  // Get the final weights image - this will hold PB2
  //void getWeightImage(casacore::ImageInterface<casacore::Float>& weightImage, casacore::Matrix<casacore::Float>& weights)
  //{getWeightImage(weightImage, weights, 0);};
  //void getWeightImage(casacore::ImageInterface<casacore::Float>& weightImage, casacore::Matrix<casacore::Float>& weights, 
  //                                const casacore::Int taylorindex);
  void getWeightImage(casacore::ImageInterface<casacore::Float>& weightImage, casacore::Matrix<casacore::Float>& weights)
  {subftms_p[0]->getWeightImage(weightImage, weights);}
  //  {throw(casacore::AipsError("NewMultiTermFT::getWeightImage() should not be called"));}

  // Save and restore the NewMultiTermFT to and from a record
  virtual casacore::Bool toRecord(casacore::String& error, casacore::RecordInterface& outRec, casacore::Bool withImage=false,
			const casacore::String diskimage="");
  virtual casacore::Bool fromRecord(casacore::String& error, const casacore::RecordInterface& inRec);

  // Various small inline functions
  virtual casacore::Bool isFourier() {return true;}
  virtual void setNoPadding(casacore::Bool nopad){subftms_p[0]->setNoPadding(nopad);};
  virtual casacore::String name()const {return machineName_p;};
  virtual void setMiscInfo(const casacore::Int qualifier){(void)qualifier;};

  void printFTTypes()
  {
    cout << "** Number of FTs : " << subftms_p.nelements() << " -- " ;
    for(casacore::uInt tix=0; tix<(subftms_p).nelements(); tix++)
      cout << tix << " : " << (subftms_p[tix])->name() << "   " ;
    cout << endl;
  };

  virtual void setDOPBCorrection(casacore::Bool doit=true) {doWideBandPBCorrection_p=doit;};
  virtual casacore::Bool getDOPBCorrection() {return doWideBandPBCorrection_p;};
  virtual void setConjBeams(casacore::Bool useit=true) {useConjBeams_p=useit;};
  virtual casacore::Bool getConjBeams() {return useConjBeams_p;};


protected:

  // Instantiate a new sub FTM
  FTMachine* getNewFTM(const FTMachine *ftm);

  // Multiply Imaging weights by Taylor-function weights - during "put"
  casacore::Bool modifyVisWeights(VisBuffer& vb, casacore::uInt thisterm);
  // Multiply model visibilities by Taylor-function weights - during "get"
  casacore::Bool modifyModelVis(VisBuffer &vb, casacore::uInt thisterm);
  // Restore vb.imagingweights to the original
  void restoreImagingWeights(VisBuffer &vb);

  // Use sumwts to make a Hessian, invert it, apply to weight images, fill in pbcoeffs_p
  void normAvgPBs(casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & weightImageVec);
  void calculateTaylorPBs(casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & weightImageVec);
  // Make pixel-by-pixel matrices from pbcoeffs, invert, apply to residuals
  //  void normalizeWideBandPB2(casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & resImageVec);
  //  void normalizeWideBandPB(casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & resImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& scratchImageVec);
  void applyWideBandPB(casacore::String action, casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & imageVec);

  void multiplyHMatrix( casacore::Matrix<casacore::Double> &hmat, casacore::PtrBlock<casacore::SubImage<casacore::Float>* > &invec,
			casacore::PtrBlock<casacore::SubImage<casacore::Float>* > &outvec, casacore::String saveImagePrefix );

  // Helper function to write ImageInterfaces to disk
  casacore::Bool storeAsImg(casacore::String fileName, casacore::ImageInterface<casacore::Float> & theImg);


  casacore::Cube<casacore::Complex> modviscube_p;

  //// New MTFT specific internal parameters and functions
  casacore::uInt nterms_p;
  casacore::Bool donePSF_p, doingPSF_p;
  casacore::Double reffreq_p;
  casacore::Matrix<casacore::Float> imweights_p;
  casacore::String machineName_p;
  casacore::Float pblimit_p;
  casacore::Bool doWideBandPBCorrection_p;
  casacore::String cacheDir_p;
  casacore::Bool donePBTaylor_p;
  casacore::Bool useConjBeams_p;

  casacore::Block< casacore::CountedPtr<FTMachine> > subftms_p;
  casacore::Block<casacore::Matrix<casacore::Float> > sumweights_p;

  casacore::Double sumwt_p;
  casacore::Matrix<casacore::Double> hess_p, invhess_p; // casacore::Block is for the pol axis

  casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > > sensitivitymaps_p;
  casacore::PtrBlock<casacore::SubImage<casacore::Float>* > pbcoeffs_p;

  casacore::Bool dbg_p,dotime_p;
  casacore::Timer tmr_p;
  casacore::Double time_get, time_put, time_res;
};

} //# NAMESPACE CASA - END

#endif
