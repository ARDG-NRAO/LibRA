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

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisSet.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/OS/Timer.h>
#include <casa/sstream.h>

#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/TransformMachines/VisModelData.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/SubImage.h>

#include <scimath/Mathematics/MatrixMathLA.h>

#include <synthesis/TransformMachines/MultiTermFTNew.h>
#include <synthesis/TransformMachines/Utils.h>

// This is the list of FTMachine types supported by MultiTermFTNew
//#include <synthesis/TransformMachines/GridFT.h>
#include <synthesis/TransformMachines/AWProjectFT.h>
#include <synthesis/TransformMachines/AWProjectWBFT.h>

#include<synthesis/ImagerObjects/SIImageStoreMultiTerm.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  
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
	if( termindex==0 ){ subftms_p[termindex] = subftm; }
	else { subftms_p[termindex] = getNewFTM(subftm); }

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
  
  
  //---------------------------------------------------------------------------------------------------
  //------------ Multi-Term Specific Functions --------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  
  // Multiply the imaging weights by Taylor functions - in place
  // This function MUST be called in ascending Taylor-term order
  // NOTE : Add checks to ensure this.
  Bool MultiTermFTNew::modifyVisWeights(VisBuffer& vb,uInt thisterm)
  {
    {
      
      if(imweights_p.shape() != vb.imagingWeight().shape())
	imweights_p.resize(vb.imagingWeight().shape());
      imweights_p = vb.imagingWeight();
      
      Float freq=0.0,mulfactor=1.0;
      Vector<Double> selfreqlist(vb.frequency());
      
      for (Int row=0; row<vb.nRow(); row++)
	for (Int chn=0; chn<vb.nChannel(); chn++)
	  {
	    freq = selfreqlist(IPosition(1,chn));
	    mulfactor = ((freq-reffreq_p)/reffreq_p);
	    (vb.imagingWeight())(chn,row) *= pow( mulfactor,(Int)thisterm);
	    //	      sumwt_p += (vb.imagingWeight())(chn,row);
	  }
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
  
  void MultiTermFTNew::initMaps(const VisBuffer& vb){
    for (uInt k=0;  k < subftms_p.nelements(); ++k)
      (subftms_p[k])->initMaps(vb);
  }
  // Reset the imaging weights back to their original values
  // to be called just after "put"
  void MultiTermFTNew::restoreImagingWeights(VisBuffer &vb)
  {
    AlwaysAssert( imweights_p.shape() == vb.imagingWeight().shape() ,AipsError);
    vb.imagingWeight() = imweights_p;
  }
  
  
  // Multiply the model visibilities by the Taylor functions - in place.
  Bool MultiTermFTNew::modifyModelVis(VisBuffer& vb, uInt thisterm)
  {
    Float freq=0.0,mulfactor=1.0;
    Vector<Double> selfreqlist(vb.frequency());
    
    // DComplex modcount=0.0;

    for (uInt pol=0; pol< uInt((vb.modelVisCube()).shape()[0]); pol++)
      for (uInt chn=0; chn< uInt(vb.nChannel()); chn++)
	for (uInt row=0; row< uInt(vb.nRow()); row++)
	  {
	    // modcount += ( vb.modelVisCube())(pol,chn,row);
	    freq = selfreqlist(IPosition(1,chn));
	    mulfactor = ((freq-reffreq_p)/reffreq_p);
	    (vb.modelVisCube())(pol,chn,row) *= pow(mulfactor, (Int) thisterm);
	  }
    
    // cout << "field : " << vb.fieldId() << " spw : " 
    // 	 << vb.spectralWindow() << "  --- predicted model before taylor wt mult :" 
    // 	 << thisterm << "  sumvis : " << modcount << endl;

    return true;
  }
  
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Prediction and De-gridding -----------------------------------
  //---------------------------------------------------------------------------------------------------

//  void MultiTermFTNew::initializeToVis(Block<CountedPtr<ImageInterface<Complex> > > & compImageVec,PtrBlock<SubImage<Float> *> & modelImageVec, PtrBlock<SubImage<Float> *>& weightImageVec, PtrBlock<SubImage<Float> *>& fluxScaleVec,Block<Matrix<Float> >& weightsVec, const VisBuffer& vb)
  
void MultiTermFTNew::initializeToVisNew(const VisBuffer& vb,
				     CountedPtr<SIImageStore> imstore)
{
  
  // Convert Stokes planes to correlation planes..
  for(uInt taylor=0;taylor<nterms_p;taylor++)
    {
      stokesToCorrelation( *(imstore->model(taylor)) , *(imstore->forwardGrid(taylor) ) );
      
      if(vb.polFrame()==MSIter::Linear) {
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

  
  
  void MultiTermFTNew::get(VisBuffer& vb, Int row)
  {
    
    // De-grid the model for the zeroth order Taylor term
    subftms_p[0]->get(vb,row);
    // Save the model visibilities in a local cube
    modviscube_p.assign( vb.modelVisCube() );
    
    for(uInt tix=1;tix<nterms_p;tix++) // Only nterms.... not 2nterms-1
      {
	// Reset the model visibilities to zero
	vb.setModelVisCube(Complex(0.0,0.0));
	// De-grid the model onto the modelviscube (other Taylor terms)
	subftms_p[tix]->get(vb,row);
	// Multiply visibilities by taylor-weights
	modifyModelVis(vb,tix); 
	// Accumulate model visibilities across Taylor terms
	modviscube_p += vb.modelVisCube();
      }
    // Set the vb.modelviscube to what has been accumulated
    vb.setModelVisCube(modviscube_p);
  }
  
  void MultiTermFTNew::finalizeToVis()
  {
    AlwaysAssert(subftms_p.nelements() >= nterms_p , AipsError);
    for(uInt taylor=0;taylor<nterms_p;taylor++) subftms_p[taylor]->finalizeToVis();
  }

  //---------------------------------------------------------------------------------------------------
  //----------------------  Calculate Residual Visibilities -------------------------------
  //---------------------------------------------------------------------------------------------------
  void MultiTermFTNew::ComputeResiduals(VisBuffer &vb, Bool useCorrected)
  {
    
    if(subftms_p[0]->canComputeResiduals()) subftms_p[0]->ComputeResiduals(vb,useCorrected);
    else throw(AipsError("MultiTerm::ComputeResiduals : subftm of MultiTermFTNew cannot compute its own residuals !"));
    
  }
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Gridding --------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------

///  void MultiTermFTNew::initializeToSky(Block<CountedPtr<ImageInterface<Complex> > > & compImageV
//ec, Block<Matrix<Float> >& weightsVec, const VisBuffer& vb, const Bool dopsf)

void MultiTermFTNew::initializeToSkyNew(const Bool dopsf, 
					const VisBuffer& vb, 
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
      subftms_p[taylor]->initializeToSky(*(imstore->backwardGrid(taylor) ), sumWeight,vb);
    }
  
}// end of initializeToSky
  


void MultiTermFTNew::put(VisBuffer& vb, Int row, Bool dopsf, FTMachine::Type type) 
  {
    
    subftms_p[0]->put(vb,row,dopsf,type);
    
    if (!dryRun())
      {
	Int gridnterms=nterms_p;
	if(dopsf==true) // && donePSF_p==false) 
	  {
	    gridnterms=2*nterms_p-1;
	  }
	
	//cout << "  Calling put for " << gridnterms << " terms, nelements :  " << subftms_p.nelements() << "  and dopsf " << dopsf << endl;
	
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
				   const VisBuffer& /*vb*/,
				   CountedPtr<SIImageStore> imstore  )			
  {
    
    // Collect images and weights from all FTMs
    for(uInt taylor=0;taylor< (dopsf ? psfnterms_p : nterms_p) ;taylor++) 
      {
	Matrix<Float> sumWeights;
	subftms_p[taylor]->finalizeToSky();
	correlationToStokes( subftms_p[taylor]->getImage(sumWeights, false) , ( dopsf ? *(imstore->psf(taylor)) : *(imstore->residual(taylor)) ), dopsf);

	if( subftms_p[taylor]->useWeightImage() && dopsf ) {
	  subftms_p[taylor]->getWeightImage(*(imstore->weight(taylor)), sumWeights);
	}

	// Take sumWeights from corrToStokes here....
	Matrix<Float> sumWeightStokes( (imstore->sumwt())->shape()[2], (imstore->sumwt())->shape()[3]   );
	StokesImageUtil::ToStokesSumWt( sumWeightStokes, sumWeights );

	AlwaysAssert( ( (imstore->sumwt(taylor))->shape()[2] == sumWeightStokes.shape()[0] ) && 
		      ((imstore->sumwt(taylor))->shape()[3] == sumWeightStokes.shape()[1] ) , AipsError );
	
	(imstore->sumwt(taylor))->put( sumWeightStokes.reform((imstore->sumwt(taylor))->shape()) );

	//	cout << "taylor : " << taylor << "   sumwt : " << sumWeights << endl;

      }// end for taylor

    //    if( dopsf ) donePSF_p = true;
    
  }//end of finalizeToSkyNew


  //---------------------------------------------------------------------------------------------------
  //----------------------------- Obtain Images -----------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  //----------------------------------------------------------------------
  void MultiTermFTNew::makeImage(FTMachine::Type type, VisSet& vs,
				 ImageInterface<Complex>& theImage,  Matrix<Float>& weight) 
  {
    //    cout << "MTFT :: makeImage for taylor 0 only "<< endl;
    subftms_p[0]->makeImage(type, vs, theImage, weight);
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
    //    cout << "MTFTNew :: fromRecord "<< endl;
    Bool retval = true;
    
    inRec.get("nterms",nterms_p);
    inRec.get("reffreq",reffreq_p);
    inRec.get("machinename",machineName_p);
    inRec.get("psfnterms",psfnterms_p);
    //    inRec.get("donePSF_p",donePSF_p);

    Int nftms=1;
    inRec.get("numfts",nftms);
    
    subftms_p.resize(nftms);
    for(Int tix=0;tix<nftms;tix++)
      {
	Record subFTMRec=inRec.asRecord("subftm_"+String::toString(tix));
	subftms_p[tix]=VisModelData::NEW_FT(subFTMRec);
	retval = (retval || subftms_p[tix]->fromRecord(error, subFTMRec));    
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
  
  

  //---------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  
} //# NAMESPACE CASA - END

