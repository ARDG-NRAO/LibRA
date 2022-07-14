//# MultiTermFTNew.cc: Implementation of MultiTermFTNew class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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

#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Timer.h>
#include <sstream>

#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/TransformMachines2/VisModelData.h>
#include <casacore/lattices/Lattices/LatticeLocker.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/SubImage.h>

#include <casacore/scimath/Mathematics/MatrixMathLA.h>

#include <synthesis/TransformMachines2/MultiTermFTNew.h>
#include <synthesis/TransformMachines2/MosaicFTNew.h>
#include <synthesis/TransformMachines2/Utils.h>

// This is the list of FTMachine types supported by MultiTermFTNew
//#include <synthesis/TransformMachines/GridFT.h>
//#include <synthesis/TransformMachines/AWProjectFT.h>
//#include <synthesis/TransformMachines/AWProjectWBFT.h>

#include<synthesis/ImagerObjects/SIImageStoreMultiTerm.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
namespace refim { // namespace for refactor

using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::refim;
using namespace casacore;
using namespace casa::vi;

#define PSOURCE false
#define psource (IPosition(4,1536,1536,0,0))

  //---------------------------------------------------------------------- 
  //-------------------- constructors and descructors ---------------------- 
  //---------------------------------------------------------------------- 
  MultiTermFTNew::MultiTermFTNew(CountedPtr<FTMachine>&subftm,  Int nterms, Bool forward)
    :FTMachine(), nterms_p(nterms), 
     reffreq_p(0.0), imweights_p(Matrix<Float>(0,0)), machineName_p("MultiTermFTNew")
     //     donePSF_p(false)
  {
    
    this->setBasePrivates(*subftm);
    canComputeResiduals_p = subftm->canComputeResiduals();

    if( forward ) psfnterms_p = nterms_p;
    else psfnterms_p = 2*nterms_p-1;

    subftms_p.resize(psfnterms_p);
    for(uInt termindex=0;termindex<psfnterms_p;termindex++)
      {
	//        cout << "Creating new FTM of type : " << subftm->name() << endl;
	if( termindex==0 ){ 
	  subftms_p[termindex] = subftm; 
	}
	else { 
	  subftms_p[termindex] = getNewFTM(subftm); 
	  if((subftms_p[termindex]->name())=="MosaicFTNew"){ 
	    (static_cast<MosaicFTNew *>(subftms_p[termindex].get()))->setConvFunc(  (static_cast<MosaicFTNew * >(subftm.get()))->getConvFunc());

	    }
	}

	subftms_p[termindex]->setMiscInfo(termindex); 
      }

    //	printFTTypes();
  }
  
  //---------------------------------------------------------------------- 
  // Construct from the input state record
  MultiTermFTNew::MultiTermFTNew(const RecordInterface& stateRec)
  : FTMachine()
  {
    String error;
    if (!fromRecord(error, stateRec)) {
      throw (AipsError("Failed to create gridder: " + error));
    };
  }
  
  //----------------------------------------------------------------------
  // Copy constructor
  MultiTermFTNew::MultiTermFTNew(const MultiTermFTNew& other) : FTMachine(), machineName_p("MultiTermFTNew")
  { 
    operator=(other);
  }
  
  MultiTermFTNew& MultiTermFTNew::operator=(const MultiTermFTNew& other)
  {
    
    if(this!=&other)
      {
	FTMachine::operator=(other);
	
	// Copy local privates
	machineName_p = other.machineName_p;
	nterms_p = other.nterms_p;
	psfnterms_p = other.psfnterms_p;
	reffreq_p = other.reffreq_p;
	//	donePSF_p = other.donePSF_p;

	// Make the list of subftms
	subftms_p.resize(other.subftms_p.nelements());
	for (uInt termindex=0;termindex<other.subftms_p.nelements();termindex++)
	  {
	    subftms_p[termindex] = getNewFTM(  (other.subftms_p[termindex]) );
	    subftms_p[termindex]->setMiscInfo(termindex);
	  }
	//	   subftms_p[termindex] = getNewFTM(  &(*(other.subftms_p[termindex])) );

	// Just checking....
	AlwaysAssert( subftms_p.nelements()>0 , AipsError );
	
	// Check if the sub ftm type can calculate its own residuals....
	canComputeResiduals_p = subftms_p[0]->canComputeResiduals();
	
      }
    
    //    cout << "Checking FTtypes at the end of MTFT operator= for " << ( (other.subftms_p.nelements() > nterms_p)?String("grid"):String("degrid") ) << endl;
    //    printFTTypes();
    
    return *this;
    
  }
  
  CountedPtr<FTMachine> MultiTermFTNew::getNewFTM(const CountedPtr<FTMachine>& ftm)
  {

    return ftm->cloneFTM();

    /*
    if(ftm->name()=="GridFT")
      {
       	return new GridFT(static_cast<const GridFT&>(*ftm)); 
      }
      else  if(ftm->name()=="AWProjectWBFT") 
      { return new AWProjectWBFT(static_cast<const AWProjectWBFT&>(*ftm)); }
    else
      {throw(AipsError("FTMachine "+ftm->name()+" is not supported with MS-MFS")); }
    
    return NULL;
    */

  }

  FTMachine* MultiTermFTNew::cloneFTM()
  {
    return new MultiTermFTNew(*this);
  }

  
  //----------------------------------------------------------------------
  MultiTermFTNew::~MultiTermFTNew()
  {
  }
  void MultiTermFTNew::setMovingSource(const String& sourcename, const String& ephemtable){
    for (uInt k=0;  k < subftms_p.nelements(); ++k)
      (subftms_p[k])->setMovingSource(sourcename, ephemtable);
  }
  void MultiTermFTNew::setMovingSource(const MDirection& mdir){
    for (uInt k=0;  k < subftms_p.nelements(); ++k)
      (subftms_p[k])->setMovingSource(mdir);
  }
  void MultiTermFTNew::setLocation(const MPosition& mloc){
    for (uInt k=0;  k < subftms_p.nelements(); ++k)
      (subftms_p[k])->setLocation(mloc);
  }
  //---------------------------------------------------------------------------------------------------
  //------------ Multi-Term Specific Functions --------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  
  // Multiply the imaging weights by Taylor functions - in place
  // This function MUST be called in ascending Taylor-term order
  // NOTE : Add checks to ensure this.
  Bool MultiTermFTNew::modifyVisWeights(VisBuffer2& vb,uInt thisterm)
  {
    {
    ///There is no setImagingWeight in vb2 !
    Matrix<Float>& imwgt=const_cast<Matrix<Float>& >(vb.imagingWeight());
    ////remove the above line when using setImagingWeight

      if(imweights_p.shape() != vb.imagingWeight().shape())
	imweights_p.resize(vb.imagingWeight().shape());
      imweights_p = vb.imagingWeight();
      
      Float freq=0.0,mulfactor=1.0;
      Vector<Double> selfreqlist(vb.getFrequencies(0));
      
      for (rownr_t row=0; row<vb.nRows(); row++)
	for (Int chn=0; chn<vb.nChannels(); chn++)
	  {
	    freq = selfreqlist(IPosition(1,chn));
	    mulfactor = ((freq-reffreq_p)/reffreq_p);
	    (imwgt)(chn,row) *= pow( mulfactor,(Int)thisterm);
	    //	      sumwt_p += (vb.imagingWeight())(chn,row);
	  }
      vb.setImagingWeight(imwgt);
    }
    /* // For debugging.
       else
       {
       for (Int row=0; row<vb.nRow(); row++)
       for (Int chn=0; chn<vb.nChannel(); chn++)
       {
       sumwt_p += (vb.imagingWeight())(chn,row);
       }
       }
    */
    return true;
  }
  
  void MultiTermFTNew::initMaps(const VisBuffer2& vb){

    for (uInt k=0;  k < subftms_p.nelements(); ++k){
      (subftms_p[k])->setFrameValidity( freqFrameValid_p);
      (subftms_p[k])->freqInterpMethod_p=this->freqInterpMethod_p;
      (subftms_p[k])->initMaps(vb);
    }
  }
  // Reset the imaging weights back to their original values
  // to be called just after "put"
  void MultiTermFTNew::restoreImagingWeights(VisBuffer2 &vb)
  {
    AlwaysAssert( imweights_p.shape() == vb.imagingWeight().shape() ,AipsError);
    ///There is no setImagingWeight in vb2 !
    // Matrix<Float>& imwgt=const_cast<Matrix<Float>& >(vb.imagingWeight());
    // ////remove the above line when using setImagingWeight
    // imwgt = imweights_p;
    vb.setImagingWeight(imweights_p);
 }
  
  
  // Multiply the model visibilities by the Taylor functions - in place.
  Bool MultiTermFTNew::modifyModelVis(VisBuffer2& vb, uInt thisterm)
  {
    Float freq=0.0,mulfactor=1.0;
    Vector<Double> selfreqlist(vb.getFrequencies(0));
    
    // DComplex modcount=0.0;

    Cube<Complex> modelVisCube(vb.visCubeModel().shape());
    modelVisCube=vb.visCubeModel();
    for (uInt pol=0; pol< uInt((vb.visCubeModel()).shape()[0]); pol++)
      for (uInt chn=0; chn< uInt(vb.nChannels()); chn++)
	for (uInt row=0; row< uInt(vb.nRows()); row++)
	  {
	    // modcount += ( vb.modelVisCube())(pol,chn,row);
	    freq = selfreqlist(IPosition(1,chn));
	    mulfactor = ((freq-reffreq_p)/reffreq_p);
	    modelVisCube(pol,chn,row) *= pow(mulfactor, (Int) thisterm);
	  }
    vb.setVisCubeModel(modelVisCube);
    // cout << "field : " << vb.fieldId() << " spw : " 
    // 	 << vb.spectralWindow() << "  --- predicted model before taylor wt mult :" 
    // 	 << thisterm << "  sumvis : " << modcount << endl;

    return true;
  }
  
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Prediction and De-gridding -----------------------------------
  //---------------------------------------------------------------------------------------------------

//  void MultiTermFTNew::initializeToVis(Block<CountedPtr<ImageInterface<Complex> > > & compImageVec,PtrBlock<SubImage<Float> *> & modelImageVec, PtrBlock<SubImage<Float> *>& weightImageVec, PtrBlock<SubImage<Float> *>& fluxScaleVec,Block<Matrix<Float> >& weightsVec, const VisBuffer& vb)
  
void MultiTermFTNew::initializeToVisNew(const VisBuffer2& vb,
				     CountedPtr<SIImageStore> imstore)
{
  
  // Convert Stokes planes to correlation planes..
  for(uInt taylor=0;taylor<nterms_p;taylor++)
    {
      
      if(!(imstore->forwardGrid(taylor)).get())
        throw(AipsError("MultiTermFTNew::InitializeToVisNew error imagestore has no valid grid initialized for taylor term "+String::toString(taylor)));
      stokesToCorrelation( *(imstore->model(taylor)) , *(imstore->forwardGrid(taylor) ) );
      
      if(vb.polarizationFrame()==MSIter::Linear) {
	StokesImageUtil::changeCStokesRep(  *(imstore->forwardGrid(taylor) ), StokesImageUtil::LINEAR);
      } else {
	StokesImageUtil::changeCStokesRep( *(imstore->forwardGrid(taylor) ) , StokesImageUtil::CIRCULAR);
      }
    }
      
  reffreq_p = imstore->getReferenceFrequency();
  
  for(uInt taylor=0;taylor<nterms_p;taylor++)
    {
      subftms_p[taylor]->initializeToVis(*(imstore->forwardGrid(taylor)),vb);
    }
  
}// end of initializeToVis

  
  
  void MultiTermFTNew::get(VisBuffer2& vb, Int row)
  {
    
    // De-grid the model for the zeroth order Taylor term
    subftms_p[0]->get(vb,row);
    // Save the model visibilities in a local cube
    modviscube_p.assign( vb.visCubeModel() );
    
    for(uInt tix=1;tix<nterms_p;tix++) // Only nterms.... not 2nterms-1
      {
	// Reset the model visibilities to zero
	vb.setVisCubeModel(Complex(0.0,0.0));
	// De-grid the model onto the modelviscube (other Taylor terms)
	subftms_p[tix]->get(vb,row);
	// Multiply visibilities by taylor-weights
	modifyModelVis(vb,tix); 
	// Accumulate model visibilities across Taylor terms
	modviscube_p += vb.visCubeModel();
      }
    // Set the vb.modelviscube to what has been accumulated
    vb.setVisCubeModel(modviscube_p);
  }
  
  void MultiTermFTNew::finalizeToVis()
  {
    AlwaysAssert(subftms_p.nelements() >= nterms_p , AipsError);
    for(uInt taylor=0;taylor<nterms_p;taylor++) subftms_p[taylor]->finalizeToVis();
  }

  //---------------------------------------------------------------------------------------------------
  //----------------------  Calculate Residual Visibilities -------------------------------
  //---------------------------------------------------------------------------------------------------
  void MultiTermFTNew::ComputeResiduals(VisBuffer2 &vb, Bool useCorrected)
  {
    
    if(subftms_p[0]->canComputeResiduals()) subftms_p[0]->ComputeResiduals(vb,useCorrected);
    else throw(AipsError("MultiTerm::ComputeResiduals : subftm of MultiTermFTNew cannot compute its own residuals !"));
    
  }
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Gridding --------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------

///  void MultiTermFTNew::initializeToSky(Block<CountedPtr<ImageInterface<Complex> > > & compImageV
//ec, Block<Matrix<Float> >& weightsVec, const VisBuffer& vb, const Bool dopsf)
 Long MultiTermFTNew::estimateRAM(const CountedPtr<SIImageStore>& imstor){
   Long mem=0;
   for(uInt k=0; k < subftms_p.nelements(); ++k){

     mem+=subftms_p[k]->estimateRAM(imstor);
   }

   return mem;
  }

  void MultiTermFTNew::initializeToSkyNew(const Bool dopsf, 
					const VisBuffer2& vb,
					CountedPtr<SIImageStore> imstore)
{
  
  // If PSF is already done, don't ask again !
  //  AlwaysAssert( !(donePSF_p && dopsf) , AipsError ); 
  
  // The PSF needs to be the first thing made (because of weight images)
  //  AlwaysAssert( !(dopsf==false && donePSF_p==false) , AipsError); 
  
  //  if(donePSF_p==true)
  if(dopsf==false)
    {
      if( subftms_p.nelements() != nterms_p )  
	{ 
	  subftms_p.resize( nterms_p ,true);
	  //	  cout << "MTFT::initializeToSky : resizing to " << nterms_p << " terms" << endl;
	}
    }

  // Make the relevant float grid. 
  // This is needed mainly for facetting (to set facet shapes), but is harmless for non-facetting.
  if( dopsf ) { imstore->psf(0); } else { imstore->residual(0); } 
  
  reffreq_p = imstore->getReferenceFrequency();
 
  Matrix<Float> sumWeight;
  for(uInt taylor=0;taylor< (dopsf ? psfnterms_p : nterms_p);taylor++) 
    {

        if(! (imstore->backwardGrid(taylor)).get())
        throw(AipsError("MultiTermFTNew::InitializeToSkyNew error imagestore has no valid grid initialized for taylor term "+String::toString(taylor)));
      subftms_p[taylor]->initializeToSky(*(imstore->backwardGrid(taylor) ), sumWeight,vb);
    }
  
}// end of initializeToSky
  
 void MultiTermFTNew::initBriggsWeightor(vi::VisibilityIterator2& vi){


   for (uInt k=0; k < subftms_p.nelements(); ++k){
     subftms_p[k]->initBriggsWeightor(vi);
   }

  }

  void MultiTermFTNew::put(VisBuffer2& vb, Int row, Bool dopsf, refim::FTMachine::Type type)
  {
    
    subftms_p[0]->put(vb,row,dopsf,type);
    if (!dryRun())
      {
	Int gridnterms=nterms_p;
	if(dopsf==true) // && donePSF_p==false) 
	  {
	    gridnterms=2*nterms_p-1;
	  }
    
	//cerr << "  Calling put for " << gridnterms << " terms, nelements :  " << subftms_p.nelements() << "  and dopsf " << dopsf << " reffreq " << reffreq_p << endl;
    
	for(Int tix=1;tix<gridnterms;tix++)
	  {
	    modifyVisWeights(vb,tix);
	    subftms_p[tix]->put(vb,row,dopsf,type); 
	    restoreImagingWeights(vb);
	  }
      }    
    
  }// end of put
  
  //----------------------------------------------------------------------

//  void MultiTermFTNew::finalizeToSky(Block<CountedPtr<ImageInterface<Complex> > > & compImageVec, PtrBlock<SubImage<Float> *> & resImageVec, PtrBlock<SubImage<Float> *>& weightImageVec, PtrBlock<SubImage<Float> *>& fluxScaleVec, Bool dopsf, Block<Matrix<Float> >& weightsVec, const VisBuffer& /*vb*/)

void MultiTermFTNew::finalizeToSkyNew(Bool dopsf, 
				   const VisBuffer2& /*vb*/,
				   CountedPtr<SIImageStore> imstore  )			
  {
    
    // Collect images and weights from all FTMs
    for(uInt taylor=0;taylor< (dopsf ? psfnterms_p : nterms_p) ;taylor++) 
      {
	Matrix<Float> sumWeights;
	subftms_p[taylor]->finalizeToSky();
        shared_ptr<ImageInterface<Float> > theim=dopsf ? imstore->psf(taylor) : imstore->residual(taylor);
        { LatticeLocker lock1 (*theim, FileLocker::Write);
          correlationToStokes( subftms_p[taylor]->getImage(sumWeights, false) , *theim, dopsf);
        }
	if( subftms_p[taylor]->useWeightImage() && dopsf ) {
          LatticeLocker lock1 (*(imstore->weight(taylor)), FileLocker::Write);
	  subftms_p[taylor]->getWeightImage(*(imstore->weight(taylor)), sumWeights);
	}

	// Take sumWeights from corrToStokes here....
	Matrix<Float> sumWeightStokes( (imstore->sumwt())->shape()[2], (imstore->sumwt())->shape()[3]   );
	StokesImageUtil::ToStokesSumWt( sumWeightStokes, sumWeights );

	AlwaysAssert( ( (imstore->sumwt(taylor))->shape()[2] == sumWeightStokes.shape()[0] ) && 
		      ((imstore->sumwt(taylor))->shape()[3] == sumWeightStokes.shape()[1] ) , AipsError );
	LatticeLocker lock1 (*(imstore->sumwt(taylor)), FileLocker::Write);
	(imstore->sumwt(taylor))->put( sumWeightStokes.reform((imstore->sumwt(taylor))->shape()) );

	//	cout << "taylor : " << taylor << "   sumwt : " << sumWeights << endl;

      }// end for taylor

    //    if( dopsf ) donePSF_p = true;
    
  }//end of finalizeToSkyNew


  //---------------------------------------------------------------------------------------------------
  //----------------------------- Obtain Images -----------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  //----------------------------------------------------------------------
  void MultiTermFTNew::makeImage(refim::FTMachine::Type type, VisibilityIterator2& vi,
				 ImageInterface<Complex>& theImage,  Matrix<Float>& weight) 
  {
    //    cout << "MTFT :: makeImage for taylor 0 only "<< endl;
    subftms_p[0]->makeImage(type, vi, theImage, weight);
  }
  void MultiTermFTNew::makeMTImages(refim::FTMachine::Type type,
				    vi::VisibilityIterator2& vi,
				    casacore::Vector<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > >& theImage,
				    casacore::Vector<casacore::CountedPtr<casacore::Matrix<casacore::Float> > >& weight){
    Int ntaylor= (type== refim::FTMachine::PSF) ? psfnterms_p : nterms_p; 
    

    
    vi::VisBuffer2* vb=vi.getVisBuffer();
    
    // Initialize put (i.e. transform to Sky) for this model
    vi.origin();
    for(Int taylor=0;taylor< ntaylor  ; ++taylor) {
       if(vb->polarizationFrame()==MSIter::Linear) {
	 StokesImageUtil::changeCStokesRep(*(theImage[taylor]), StokesImageUtil::LINEAR);
       }
       else {
	 StokesImageUtil::changeCStokesRep(*(theImage[taylor]), StokesImageUtil::CIRCULAR);
       }
       subftms_p[taylor]->initializeToSky(*(theImage[taylor]), *(weight[taylor]),*vb);
      
    }
    {
      Vector<Double> refpix = (theImage[0]->coordinates().spectralCoordinate()).referencePixel();
      (theImage[0]->coordinates().spectralCoordinate()).toWorld( reffreq_p, refpix[0] );
    }
    Bool useCorrected= !(MSColumns(vi.ms()).correctedData().isNull());
    if((type==FTMachine::CORRECTED) && (!useCorrected))
      type=FTMachine::OBSERVED;
   

    // Loop over the visibilities, putting VisBuffers
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin(); vi.more(); vi.next()) {
	
	switch(type) {
	case FTMachine::RESIDUAL:
	  vb->setVisCube(vb->visCubeCorrected());
	  vb->setVisCube(vb->visCube()-vb->visCubeModel());
	  put(*vb, -1, false);
	  break;
	case FTMachine::CORRECTED:
	  put(*vb, -1, false, FTMachine::CORRECTED);
	  break;
	case FTMachine::PSF:
	  vb->setVisCube(Complex(1.0,0.0));
	  put(*vb, -1, true, FTMachine::PSF);
	  break;
	case FTMachine::OBSERVED:
	  put(*vb, -1, false, FTMachine::OBSERVED);
	  break;
	default:
	  throw(AipsError("Cannot make multiterm image of type requested"));
	  break;
	}
      }
    }
    ///
    for(Int taylor=0;taylor< ntaylor  ; ++taylor) {
      subftms_p[taylor]->finalizeToSky();
      subftms_p[taylor]->getImage(*(weight[taylor]), false);
     
    }
  }
  //---------------------------------------------------------------------------------------------------
  //------------------------ To / From Records ---------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  Bool MultiTermFTNew::toRecord(String& error, RecordInterface& outRec, Bool withImage, const String diskimage) 
  {
    //    cout << "MTFTNew :: toRecord for " << subftms_p.nelements() << " subftms" << endl;
    Bool retval = true;
    outRec.define("name", this->name());
    outRec.define("nterms",nterms_p);
    outRec.define("reffreq",reffreq_p);
    outRec.define("machinename",machineName_p);
    outRec.define("psfnterms",psfnterms_p);
    //    outRec.define("donePSF_p",donePSF_p);

    outRec.define("numfts", (Int)subftms_p.nelements() ); // Since the forward and reverse ones are different.

    for(uInt tix=0;tix<subftms_p.nelements();tix++)
      {
	Record subFTContainer;
	String elimage="";
	if(diskimage != ""){
	  elimage=diskimage+String("_term_")+String::toString(tix);
	}
	subftms_p[tix]->toRecord(error, subFTContainer,withImage, elimage);
	outRec.defineRecord("subftm_"+String::toString(tix),subFTContainer);
      }
    
    return retval;
  }
  
  //---------------------------------------------------------------------------------------------------
  Bool MultiTermFTNew::fromRecord(String& error, const RecordInterface& inRec)
  {
    //cout << "MTFTNew :: fromRecord "<< endl;
    Bool retval = true;
    
    inRec.get("nterms",nterms_p);
    inRec.get("reffreq",reffreq_p);
    inRec.get("machinename",machineName_p);
    inRec.get("psfnterms",psfnterms_p);
    //    inRec.get("donePSF_p",donePSF_p);

    Int nftms=1;
    inRec.get("numfts",nftms);
    
    subftms_p.resize(nftms);
    //cerr << "number of ft " << nftms << endl;
    for(Int tix=0;tix<nftms;tix++)
      {
	Record subFTMRec=inRec.asRecord("subftm_"+String::toString(tix));
	subftms_p[tix]=VisModelData::NEW_FT(subFTMRec);
	if(subftms_p[tix].null())
	  throw(AipsError("Could not recover from record term "+String::toString(tix)+" ftmachine"));
	retval = (retval || subftms_p[tix]->fromRecord(error, subFTMRec));
	if(!retval) throw(AipsError("Could not recover term "+String::toString(tix)+" ftmachine; \n Error being "+error));
      }
    
    
    return retval;
  }
  //---------------------------------------------------------------------------------------------------
  
  Bool MultiTermFTNew::storeAsImg(String fileName, ImageInterface<Float>& theImg)
  {
    PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), fileName);
    LatticeExpr<Float> le(theImg);
    tmp.copyData(le);
    return true;
  }
  //
  // Set the supplied CFCache for all internal FTMs if they do use CFCache mechanism
  //
  void MultiTermFTNew::setCFCache(casacore::CountedPtr<CFCache>& cfc, const casacore::Bool resetCFC)
  {
    for (unsigned int i=0;i<subftms_p.nelements();i++)
      {
	if (subftms_p[i]->isUsingCFCache())
	  subftms_p[i]->setCFCache(cfc,resetCFC);
      }
  }
  

  //---------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
} // end namespace refim
} //# NAMESPACE CASA - END

