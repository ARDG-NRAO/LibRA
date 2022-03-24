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

#ifndef SYNTHESIS_TRANSFORM2_MULTITERMFTNEW_H
#define SYNTHESIS_TRANSFORM2_MULTITERMFTNEW_H

#include <synthesis/TransformMachines2/FTMachine.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <msvis/MSVis/VisBuffer2.h>
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

namespace vi{
 	 	 	  class VisibilityIterator2;
			}

namespace refim { //# namespace refactor imaginging
class MultiTermFTNew : public FTMachine {
public:

  // Construct using an existing FT-Machine 
  MultiTermFTNew(casacore::CountedPtr<FTMachine>& subftm, casacore::Int nterms=1, casacore::Bool forward=false);

  // Construct from a casacore::Record containing the MultiTermFTNew state
  MultiTermFTNew(const casacore::RecordInterface& stateRec);

  // Copy constructor. 
  // This first calls the default "=" operator, and then instantiates objects for member pointers.
  MultiTermFTNew(const MultiTermFTNew &other);

  // Assignment operator --- leave it as the default
  MultiTermFTNew &operator=(const MultiTermFTNew &other);

  // Destructor
  ~MultiTermFTNew();

  // Called at the start of de-gridding : subftm->initializeToVis()
  // Note : Pre-de-gridding model-image divisions by PBs will go here.

  void initializeToVis(casacore::ImageInterface<casacore::Complex>& /*image*/,
                         const vi::VisBuffer2& /*vb*/){throw(casacore::AipsError("not implemented"));};
   // Vectorized InitializeToVis
  //  void initializeToVis(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec,casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & modelImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& weightImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& fluxScaleVec, casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec, const VisBuffer& vb);

  virtual void initializeToVisNew(const vi::VisBuffer2& vb,
					     casacore::CountedPtr<SIImageStore> imstore);

  // Called at the end of de-gridding : subftm->finalizeToVis()
  void finalizeToVis();

  // Called at the start of gridding : subftm->initializeToSky()
  void initializeToSky(casacore::ImageInterface<casacore::Complex>& /*image*/,
                         casacore::Matrix<casacore::Float>& /*weight*/, const vi::VisBuffer2& /*vb*/){throw(casacore::AipsError("not implemented"));};
  //  void initializeToSky(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec, casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec, const VisBuffer& vb, const casacore::Bool dopsf);

  virtual void initializeToSkyNew(const casacore::Bool dopsf,
				  const vi::VisBuffer2& vb,
				  casacore::CountedPtr<SIImageStore> imstore);

  virtual void initBriggsWeightor(vi::VisibilityIterator2& vi);
  // Called at the end of gridding : subftm->finalizeToSky()
  void finalizeToSky(){throw(casacore::AipsError("MultiTermFTNew::finalizeToSky() called without arguments!"));};

  //void finalizeToSky(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & resImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& weightImageVec, casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& fluxScaleVec, casacore::Bool dopsf, casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec, const VisBuffer& vb);

  virtual void finalizeToSkyNew(casacore::Bool dopsf, 
					   const vi::VisBuffer2& vb,
					   casacore::CountedPtr<SIImageStore> imstore  );

  //  void normalizeToSky(casacore::ImageInterface<casacore::Complex>& compImage, casacore::ImageInterface<casacore::Float>& resImage, casacore::ImageInterface<casacore::Float>& weightImage, casacore::Bool dopsf, casacore::Matrix<casacore::Float>& weights)
  // {throw(casacore::AipsError("MultiTermFTNew::normalizeToSky should not get called !"));};


  // Do the degridding via subftm->get() and modify model-visibilities by Taylor-weights
  void get(vi::VisBuffer2& vb, casacore::Int row=-1);
  // Modify imaging weights with Taylor-weights and do gridding via subftm->put()
  void put(vi::VisBuffer2& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
  	   refim::FTMachine::Type type=refim::FTMachine::OBSERVED);
  // Have a const version for compatibility with other FTMs.. Throw an exception if called.
  void put(const vi::VisBuffer2& /*vb*/, casacore::Int /*row=-1*/, casacore::Bool /*dopsf=false*/,
    	   refim::FTMachine::Type /*type=FTMachine::OBSERVED*/)
  {throw(casacore::AipsError("Internal error: called MultiTermFTNew::put(const VB2)"));};
  // Calculate residual visibilities if possible.
  // The purpose is to allow rGridFT to make this multi-threaded
  virtual void ComputeResiduals(vi::VisBuffer2& vb, casacore::Bool useCorrected);

  // Make an image : subftm->makeImage()
  void makeImage(refim::FTMachine::Type type,
		 vi::VisibilityIterator2& vs,
		 casacore::ImageInterface<casacore::Complex>& image,
		 casacore::Matrix<casacore::Float>& weight);
  ////Make the multi term images
  ////caller make sure vector is the size of nterms or npsfterms required 
  void makeMTImages(refim::FTMachine::Type type,
		 vi::VisibilityIterator2& vi,
		    casacore::Vector<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > >& image,
		    casacore::Vector<casacore::CountedPtr<casacore::Matrix<casacore::Float> > >& weight);
  // Get the final image: do the Fourier transform grid-correct, then 
  // optionally normalize by the summed weights
  // Note : Post-gridding residual-image divisions by PBs will go here.
  //           For now, it just calls subftm->getImage()
  //  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>& weights, casacore::Bool normalize=true)
  //{return getImage(weights,normalize,0);};
  //casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>& weights, casacore::Bool normalize=true, 
  //                                                         const casacore::Int taylorindex=0);
  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>& /*weights*/, casacore::Bool /*normalize*/=true)
  {throw(casacore::AipsError("MultiTermFTNew::getImage() should not be called"));}
 
  virtual casacore::Bool useWeightImage()
  {AlwaysAssert(subftms_p.nelements()>0,casacore::AipsError); return subftms_p[0]->useWeightImage(); };

  void getWeightImage(casacore::ImageInterface<casacore::Float>& weightImage, casacore::Matrix<casacore::Float>& weights)
  {AlwaysAssert(subftms_p.nelements()>0,casacore::AipsError); 
    subftms_p[0]->getWeightImage(weightImage, weights);}
  //  {throw(casacore::AipsError("MultiTermFTNew::getWeightImage() should not be called"));}

  // Save and restore the MultiTermFTNew to and from a record
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

  FTMachine* cloneFTM();
  virtual void setDryRun(casacore::Bool val) 
  {
    isDryRun=val;
    //cerr << "MTFTMN: " << isDryRun << endl;
    for (casacore::uInt i=0;i<subftms_p.nelements();i++)
      subftms_p[i]->setDryRun(val);
  };
  virtual casacore::Bool isUsingCFCache() {casacore::Bool v=false; if (subftms_p.nelements() > 0) v=subftms_p[0]->isUsingCFCache(); return v;};
  virtual const casacore::CountedPtr<refim::FTMachine>& getFTM2(const casacore::Bool ) {return subftms_p[0];}
  virtual void setCFCache(casacore::CountedPtr<CFCache>& cfc, const casacore::Bool resetCFC=true);


  ///return number of terms

  virtual casacore::Int nTerms(){ return nterms_p;};
  virtual casacore::Int psfNTerms(){ return psfnterms_p;};

  // set a moving source aka planets or comets =>  adjust phase center
  // on the fly for gridding 
  virtual void setMovingSource(const casacore::String& sourcename, const casacore::String& ephemtable="");
  virtual void setMovingSource(const casacore::MDirection& mdir);
  // set and get the location used for frame 
  virtual void setLocation(const casacore::MPosition& loc);
   ///estimate of memory necessary in kB
   virtual casacore::Long estimateRAM(const casacore::CountedPtr<SIImageStore>& imstore);
protected:
  // have to call the initmaps of subftm
  virtual void initMaps(const vi::VisBuffer2& vb);
  // Instantiate a new sub FTM
  casacore::CountedPtr<FTMachine> getNewFTM(const casacore::CountedPtr<FTMachine>& ftm);

  // Multiply Imaging weights by Taylor-function weights - during "put"
  casacore::Bool modifyVisWeights(vi::VisBuffer2& vb, casacore::uInt thisterm);
  // Multiply model visibilities by Taylor-function weights - during "get"
  casacore::Bool modifyModelVis(vi::VisBuffer2 &vb, casacore::uInt thisterm);
  // Restore vb.imagingweights to the original
  void restoreImagingWeights(vi::VisBuffer2 &vb);

  // Helper function to write ImageInterfaces to disk
  casacore::Bool storeAsImg(casacore::String fileName, casacore::ImageInterface<casacore::Float> & theImg);


  casacore::Cube<casacore::Complex> modviscube_p;

  //// New MTFT specific internal parameters and functions
  casacore::uInt nterms_p, psfnterms_p;
  casacore::Double reffreq_p;
  casacore::Matrix<casacore::Float> imweights_p;
  casacore::String machineName_p;

  //  casacore::Bool donePSF_p;

  casacore::Block< casacore::CountedPtr<FTMachine> > subftms_p;

};

} //end namespace refim
} //# NAMESPACE CASA - END

#endif
