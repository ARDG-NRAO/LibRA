//# CubeSkyEquation.h: CubeSkyEquation definition
//# Copyright (C) 2007
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
//# $Id$
#ifndef SYNTHESIS_CUBESKYEQUATION_H
#define SYNTHESIS_CUBESKYEQUATION_H

#include <synthesis/MeasurementEquations/SkyEquation.h>
//#include <synthesis/Utilities/ThreadTimers.h>


namespace casacore{

template <class T> class ImageInterface;
template <class T> class TempImage;
template <class T> class SubImage;
template <class T> class Block;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//Forward

class VisibilityIterator;
class ROVisibilityIterator;

class CubeSkyEquation : public SkyEquation {

 public:
  CubeSkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft, ComponentFTMachine& cft, casacore::Bool noModelCol=false);
 
  //Read only iterator...hence no scratch col
  CubeSkyEquation(SkyModel& sm, ROVisibilityIterator& vi, FTMachine& ft, ComponentFTMachine& cft, casacore::Bool noModelCol=false);

  virtual ~CubeSkyEquation();
  virtual void predict(casacore::Bool incremental=false, casacore::MS::PredefinedColumns Type=casacore::MS::MODEL_DATA);
  virtual void gradientsChiSquared(casacore::Bool incremental, casacore::Bool commitModel=false);
  
  virtual void initializePutSlice(const VisBuffer& vb, casacore::Bool dopsf, casacore::Int cubeSlice=0, casacore::Int nCubeSlice=1);
  virtual void newInitializePutSlice(const VisBuffer& vb, casacore::Bool dopsf, casacore::Int cubeSlice=0, casacore::Int nCubeSlice=1);
  virtual void oldInitializePutSlice(const VisBuffer& vb, casacore::Bool dopsf, casacore::Int cubeSlice=0, casacore::Int nCubeSlice=1);
  virtual void putSlice(VisBuffer& vb, casacore::Bool dopsf, 
			FTMachine::Type col,casacore::Int cubeSlice=0, 
			casacore::Int nCubeSlice=1);
  virtual void finalizePutSlice(const VisBuffer& vb,  casacore::Bool dopsf,
				casacore::Int cubeSlice=0, casacore::Int nCubeSlice=1);
  virtual void newFinalizePutSlice(const VisBuffer& vb,  casacore::Bool dopsf,
				   casacore::Int cubeSlice=0, casacore::Int nCubeSlice=1);
  virtual void oldFinalizePutSlice(const VisBuffer& vb,  casacore::Bool dopsf,
				   casacore::Int cubeSlice=0, casacore::Int nCubeSlice=1);
  void initializeGetSlice(const VisBuffer& vb, casacore::Int row,
			  casacore::Bool incremental, casacore::Int cubeSlice=0, 
			  casacore::Int nCubeSlice=1);   
  void newInitializeGetSlice(const VisBuffer& vb, casacore::Int row,
			     casacore::Bool incremental, casacore::Int cubeSlice=0, 
			     casacore::Int nCubeSlice=1);   
  void oldInitializeGetSlice(const VisBuffer& vb, casacore::Int row,
			     casacore::Bool incremental, casacore::Int cubeSlice=0, 
			     casacore::Int nCubeSlice=1);   
  virtual VisBuffer& getSlice(VisBuffer& vb, 
			      casacore::Bool incremental, casacore::Int cubeSlice=0,
			      casacore::Int nCubeSlice=1); 
  void finalizeGetSlice();
  void isLargeCube(casacore::ImageInterface<casacore::Complex>& theIm, casacore::Int& nCubeSlice);
  //void makeApproxPSF(casacore::Int model, casacore::ImageInterface<casacore::Float>& psf);
  //virtual void makeApproxPSF(casacore::Int model, casacore::ImageInterface<casacore::Float>& psf); 
  void makeApproxPSF(casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * >& psfs);

  //Get the flux scale that the ftmachines have if they have
  virtual void getCoverageImage(casacore::Int model, casacore::ImageInterface<casacore::Float>& im);

  //get the weight image from the ftmachines
  virtual void getWeightImage(const casacore::Int model, casacore::ImageInterface<casacore::Float>& weightim);
  void tmpWBNormalizeImage(casacore::Bool& dopsf, const casacore::Float& pbLimit);

  casacore::Bool isNewFTM();
  void setPhaseCenterTime(const casacore::Double time);

 protected:

  void configureAsyncIo (ROVisibilityIterator * & oldRvi, VisibilityIterator * & oldWvi);

  //Different versions of psf making
  void makeSimplePSF(casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * >& psfs);
  void makeMosaicPSF(casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * >& psfs);
  virtual void fixImageScale();
  casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > >imGetSlice_p;
  casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > >imPutSlice_p;
  casacore::Block<casacore::Matrix<casacore::Float> >weightSlice_p;
  casacore::Slicer sl_p;
  casacore::Int nchanPerSlice_p;
  // Type of copy 
  // 0 => a independent image just with coordinates gotten from cImage
  // 1 => a subImage referencing cImage ...no image copy
  void sliceCube(casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> >& slice,casacore::Int model, casacore::Int cubeSlice, casacore::Int nCubeSlice, casacore::Int typeOfCopy=0); 
  void sliceCube(casacore::SubImage<casacore::Float>*& slice,casacore::ImageInterface<casacore::Float>& image, casacore::Int cubeSlice, casacore::Int nCubeSlice);
  //frequency range from image
  casacore::Bool getFreqRange(ROVisibilityIterator& vi, const casacore::CoordinateSystem& coords,
		  casacore::Int slice, casacore::Int nslice);

  casacore::Bool isNewFTM(FTMachine *);
 private:
  // if skyjones changed in get or put we need to tell put or get respectively
  // about it
  void init(FTMachine& ft);

  casacore::Bool destroyVisibilityIterator_p;

  casacore::Bool internalChangesPut_p;
  casacore::Bool internalChangesGet_p;
  casacore::Bool firstOneChangesPut_p;
  casacore::Bool firstOneChangesGet_p;

  casacore::Block< casacore::Vector<casacore::Int> >blockNumChanGroup_p, blockChanStart_p;
  casacore::Block< casacore::Vector<casacore::Int> > blockChanWidth_p, blockChanInc_p;
  casacore::Block<casacore::Vector<casacore::Int> > blockSpw_p;
  casacore::Block<casacore::CountedPtr<FTMachine> > ftm_p;
  casacore::Block<casacore::CountedPtr<FTMachine> > iftm_p;

  // DT aInitGrad, aGetChanSel, aCheckVisRows, aGetFreq, aOrigChunks, aVBInValid, aInitGetSlice, aInitPutSlice, aPutSlice, aFinalizeGetSlice, aFinalizePutSlice, aChangeStokes, aInitModel, aGetSlice, aSetModel, aGetRes, aExtra;


};

} //# NAMESPACE CASA - ENDf

#endif
