//# NewMultiTermFT.cc: Implementation of NewMultiTermFT class
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
#include <images/Images/ImageInterface.h>
#include <images/Images/SubImage.h>

#include <scimath/Mathematics/MatrixMathLA.h>

#include <synthesis/TransformMachines/NewMultiTermFT.h>
#include <synthesis/TransformMachines/Utils.h>

// This is the list of FTMachine types supported by NewMultiTermFT
//#include <synthesis/TransformMachines/GridFT.h>
#include <synthesis/TransformMachines/AWProjectFT.h>
#include <synthesis/TransformMachines/AWProjectWBFT.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  
#define PSOURCE false
#define psource (IPosition(4,1536,1536,0,0))
  
  //---------------------------------------------------------------------- 
  //-------------------- constructors and descructors ---------------------- 
  //---------------------------------------------------------------------- 
  NewMultiTermFT::NewMultiTermFT(FTMachine *subftm,  Int nterms, Double reffreq)
    :FTMachine(), nterms_p(nterms), donePSF_p(false),doingPSF_p(false),
     reffreq_p(reffreq), imweights_p(Matrix<Float>(0,0)), machineName_p("NewMultiTermFT"),
     pblimit_p((Float)1e-04), doWideBandPBCorrection_p(false), cacheDir_p("."), 
     donePBTaylor_p(false), useConjBeams_p(true)
  {
    dbg_p=false;
    
    this->setBasePrivates(*subftm);
    canComputeResiduals_p = subftm->canComputeResiduals();
    if(dbg_p) cout << "MTFT :: constructor with subftm : "<< subftm->name() << ". It can compute residuals : " << canComputeResiduals_p << endl;
    
    subftms_p.resize(2*nterms_p-1);
    for(uInt termindex=0;termindex<2*nterms_p-1;termindex++)
      {
        if(dbg_p) cout << "Creating new FTM of type : " << subftm->name() << endl;
	subftms_p[termindex] = getNewFTM(subftm);
	subftms_p[termindex]->setMiscInfo(termindex); 
      }
    if(dbg_p)
      {
	cout << "Checking FTtypes at the end of MTFT constructor" << endl;
	printFTTypes();
      }
    
    sumwt_p=0.0; 
    
    /// Make empty lists
    sensitivitymaps_p.resize(0);
    sumweights_p.resize(0);
    pbcoeffs_p.resize(0);
    
    if(subftm->name()=="AWProjectWBFT") 
      {
	// doWideBandPBCorrection_p = ((AWProjectWBFT*)subftm)->getDOPBCorrection();
	// useConjBeams_p = ((AWProjectWBFT*)subftm)->getConjBeams();
	setDOPBCorrection(((AWProjectWBFT*)subftm)->getDOPBCorrection());
	setConjBeams(((AWProjectWBFT*)subftm)->getConjBeams());

	pblimit_p = ((AWProjectWBFT*)subftm)->getPBLimit();
	cout << "dowidebandpbcor : " << doWideBandPBCorrection_p << " conjbeams : " << useConjBeams_p << endl;
      }
    
    if(dbg_p) cout << "Running MTFT with doWideBandPBCorrection : " << doWideBandPBCorrection_p 
		   << " and pblimit : " << pblimit_p << endl;
    
    pblimit_p = 1e-07;
    
    cacheDir_p = subftm->getCacheDir();
    
    time_get=0.0;
    time_put=0.0;
    time_res=0.0;
  }
  
  //---------------------------------------------------------------------- 
  // Construct from the input state record
  NewMultiTermFT::NewMultiTermFT(const RecordInterface& stateRec)
  : FTMachine()
  {
    String error;
    if (!fromRecord(error, stateRec)) {
      throw (AipsError("Failed to create gridder: " + error));
    };
  }
  
  //----------------------------------------------------------------------
  // Copy constructor
  NewMultiTermFT::NewMultiTermFT(const NewMultiTermFT& other) : FTMachine(), machineName_p("NewMultiTermFT")
  { 
    operator=(other);
  }
  
  NewMultiTermFT& NewMultiTermFT::operator=(const NewMultiTermFT& other)
  {
    
    dbg_p = other.dbg_p;
    if(dbg_p) cout << "In MTFT operator=  " << endl;
    
    if(this!=&other)
      {
	FTMachine::operator=(other);
	
	// Copy local privates
	machineName_p = other.machineName_p;
	nterms_p = other.nterms_p;
	reffreq_p = other.reffreq_p;
	sumwt_p = other.sumwt_p;
	donePSF_p=other.donePSF_p;
	doingPSF_p=other.doingPSF_p;
	doWideBandPBCorrection_p = other.doWideBandPBCorrection_p;
	pblimit_p = other.pblimit_p;
	cacheDir_p = other.cacheDir_p;
	donePBTaylor_p = other.donePBTaylor_p;
	useConjBeams_p = other.useConjBeams_p;
	
	// Make the list of subftms
	subftms_p.resize(other.subftms_p.nelements());
	for (uInt termindex=0;termindex<other.subftms_p.nelements();termindex++)
	  {
	    subftms_p[termindex] = getNewFTM(  &(*(other.subftms_p[termindex])) );
	    subftms_p[termindex]->setMiscInfo(termindex);
	  }
	//	   subftms_p[termindex] = getNewFTM(  &(*(other.subftms_p[termindex])) );
	
	// Just checking....
	AlwaysAssert( subftms_p.nelements()>0 , AipsError );
	
	// Check if the sub ftm type can calculate its own residuals....
	canComputeResiduals_p = subftms_p[0]->canComputeResiduals();
	
      }
    
    if(dbg_p)
      {
	cout << "Checking FTtypes at the end of MTFT operator=" << endl;
	printFTTypes();
      }
    
    return *this;
    
  }
  
  FTMachine* NewMultiTermFT::getNewFTM(const FTMachine *ftm)
  {
    //    if(dbg_p) cout << "MTFT::getNewFTM " << endl;
    /*
    if(ftm->name()=="GridFT")
      {
       	return new GridFT(static_cast<const GridFT&>(*ftm)); 
      }
      else */ if(ftm->name()=="AWProjectWBFT") 
      { return new AWProjectWBFT(static_cast<const AWProjectWBFT&>(*ftm)); }
    else
      {throw(AipsError("FTMachine "+ftm->name()+" is not supported with MS-MFS")); }
    
    return NULL;
  }
  
  //----------------------------------------------------------------------
  NewMultiTermFT::~NewMultiTermFT()
  {
    if(dbg_p) cout << "MTFT :: destructor - assumes automatic deletion of subftm " << endl;
  }
  
  
  //---------------------------------------------------------------------------------------------------
  //------------ Multi-Term Specific Functions --------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  
  // Multiply the imaging weights by Taylor functions - in place
  // This function MUST be called in ascending Taylor-term order
  // NOTE : Add checks to ensure this.
  Bool NewMultiTermFT::modifyVisWeights(VisBuffer& vb,uInt thisterm)
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
  
  // Reset the imaging weights back to their original values
  // to be called just after "put"
  void NewMultiTermFT::restoreImagingWeights(VisBuffer &vb)
  {
    AlwaysAssert( imweights_p.shape() == vb.imagingWeight().shape() ,AipsError);
    vb.imagingWeight() = imweights_p;
  }
  
  
  // Multiply the model visibilities by the Taylor functions - in place.
  Bool NewMultiTermFT::modifyModelVis(VisBuffer& vb, uInt thisterm)
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
  
  void NewMultiTermFT::initializeToVis(Block<CountedPtr<ImageInterface<Complex> > > & compImageVec,PtrBlock<SubImage<Float> *> & modelImageVec, PtrBlock<SubImage<Float> *>& weightImageVec, PtrBlock<SubImage<Float> *>& fluxScaleVec,Block<Matrix<Float> >& weightsVec, const VisBuffer& vb)
  {
    if(dbg_p) cout << "MTFT::initializeToVis " << endl;
    AlwaysAssert(compImageVec.nelements()==nterms_p, AipsError);
    AlwaysAssert(modelImageVec.nelements()==nterms_p, AipsError);
    AlwaysAssert(weightImageVec.nelements()==nterms_p, AipsError);
    AlwaysAssert(fluxScaleVec.nelements()==nterms_p, AipsError);
    AlwaysAssert(weightsVec.nelements()==nterms_p, AipsError);
    Matrix<Float> tempWts;

    // Use doWideBandPBCorrection_p to trigger whether or not to do a wideband PB correction before prediction, for the first go ( i.e. simulation )


    // This is to make sure weight images and avgPBs are ready.
    //AlwaysAssert( donePSF_p == true && donePBTaylor_p == true, AipsError )

    if(PSOURCE) cout << "------ model, before de-gridding norm : " << modelImageVec[0]->getAt(psource) << "," << modelImageVec[1]->getAt(psource) << endl;

        
    for(uInt taylor=0;taylor<nterms_p;taylor++)
      {
	// Make the sensitivity Image if applicable
	subftms_p[taylor]->findConvFunction(*(compImageVec[taylor]), vb);
	// Get the sensitivity Image
	tempWts.resize();
	subftms_p[taylor]->getWeightImage(*(weightImageVec[taylor]), tempWts);
      }
    
    /////////////////////////////   
    //UU     normAvgPBs(weightImageVec);

    // Pre-prediction correction of the model, to rid it of the Primary Beam.
    if(PSOURCE) cout << "Divide the models by the PB before prediction" << endl;
    if( useConjBeams_p == true )
      {
	// Model contains only avgPB scaling, but no PB frequency dependence
	// Divide all terms of the model image by the sensitivity image only (from Taylor0)
	if (doWideBandPBCorrection_p)
	  for(uInt taylor=0;taylor<nterms_p;taylor++)
	    {
	      // Divide by PB  ////// PBWeight
	      //normalizeImage( *(modelImageVec[taylor]) , weightsVec[0], *(weightImageVec[0]) , false, (Float)pblimit_p, (Int)1);// normtype 1 divides by weightImageVec and ignores wegithsVec
	      // Divide by sqrt(PB)  ////// PBSQWeight
	      normalizeImage( *(modelImageVec[taylor]) , weightsVec[0], *(weightImageVec[0]) , false, (Float)pblimit_p, (Int)4);
	    }
      }
    else 
      {
	cout << "NewMultiTermFT::inittoVis --> Prediction normalization of model will be wrong....... " << endl;
	// The model contains avgPB scaling and twice the frequency-dependence of the PB in it. 
	// Multiply all terms of the model by pbcoeffs_0. Divide TWICE by the wideband PB.
	for(uInt taylor=0;taylor<nterms_p;taylor++)
	  {
	    normalizeImage( *(modelImageVec[taylor]) , weightsVec[0], *(weightImageVec[0]) , false, (Float)pblimit_p, (Int)3); // normtype 3 multiplies the model image with the pb
	  }
	// Connect pbcoeffs_p to fluxScaleVec. This is where PB Taylor coefficients were put in by the 'iftm'.
	if(pbcoeffs_p.nelements() != nterms_p) pbcoeffs_p.resize(nterms_p);
	for(uInt taylor=0;taylor<nterms_p;taylor++)
	  { 
	    pbcoeffs_p[taylor] = &(*(fluxScaleVec[taylor])); 
	  }
	
	applyWideBandPB( String("divide") , modelImageVec );
	applyWideBandPB( String("divide") , modelImageVec );
      }
    
    //	for(uInt taylor=0;taylor<nterms_p;taylor++)
    //  storeAsImg("flattenedModel_"+String::toString(taylor) , *(modelImageVec[taylor]) );
    
    if(PSOURCE) cout << "------ model, after de-gridding norm : " << modelImageVec[0]->getAt(psource) << "," << modelImageVec[1]->getAt(psource) << endl;
    
    
    
    // Convert Stokes planes to correlation planes..
    for(uInt taylor=0;taylor<nterms_p;taylor++)
      {
	stokesToCorrelation( *(modelImageVec[taylor]) , *(compImageVec[taylor]) );
      }
    
    // Call initializeToVis for all sub ftms..
    for(uInt taylor=0;taylor<nterms_p;taylor++)
      {
	subftms_p[taylor]->initializeToVis(*(compImageVec[taylor]),vb);
      }
    time_get=0.0;
    
    /// Multiply the model with the avgPB again, so that it's ready for the minor cycle incremental accumulation
    if(PSOURCE) cout << "Multiplying the models by the weightimage to reset it to flat-noise for the minor cycle" << endl;

    if( useConjBeams_p == true )
      {
	
	for(uInt taylor=0;taylor<nterms_p;taylor++)
	  {
	    //Mulitply by PB  ///// PBWeight
	    //normalizeImage( *(modelImageVec[taylor]) , weightsVec[0], *(weightImageVec[0]) , false, (Float)pblimit_p, (Int)3); // normtype 3 multiplies the model image with the pb
	    //Mulitply by sqrt(PB) //// PBSQWeight
	    normalizeImage( *(modelImageVec[taylor]) , weightsVec[0], *(weightImageVec[0]) , false, (Float)pblimit_p, (Int)5); 
	    
	  }
      }
    else
      {

	applyWideBandPB( String("multiply") , modelImageVec );
	applyWideBandPB( String("multiply") , modelImageVec );

	// Divide by the avg PB 0
	for(uInt taylor=0;taylor<nterms_p;taylor++)
	  {
	    normalizeImage( *(modelImageVec[taylor]) , weightsVec[0], *(weightImageVec[0]) , false, (Float)pblimit_p, (Int)1);
	  }


      }
    
    if(PSOURCE) cout << "------ model, gone back : " << modelImageVec[0]->getAt(psource) << "," << modelImageVec[1]->getAt(psource) << endl;
    
    
  }// end of initializeToVis
  
  
  
  void NewMultiTermFT::get(VisBuffer& vb, Int row)
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
  
  void NewMultiTermFT::finalizeToVis()
  {
    if(dbg_p) cout << "MTFT::finalizeToVis for " << nterms_p << " terms "<<endl;
    AlwaysAssert(subftms_p.nelements() >= nterms_p , AipsError);
    for(uInt taylor=0;taylor<nterms_p;taylor++) subftms_p[taylor]->finalizeToVis();
  }
  
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Calculate Residual Visibilities -------------------------------
  //---------------------------------------------------------------------------------------------------
  void NewMultiTermFT::ComputeResiduals(VisBuffer &vb, Bool useCorrected)
  {
    
    if(subftms_p[0]->canComputeResiduals()) subftms_p[0]->ComputeResiduals(vb,useCorrected);
    else throw(AipsError("MultiTerm::ComputeResiduals : subftm of NewMultiTermFT cannot compute its own residuals !"));
    
  }
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Gridding --------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  
  void NewMultiTermFT::initializeToSky(Block<CountedPtr<ImageInterface<Complex> > > & compImageVec, Block<Matrix<Float> >& weightsVec, const VisBuffer& vb, const Bool dopsf)
  {
    if(dbg_p) cout << "MTFT::initializeToSky with dopsf :  " << dopsf << endl;
    
    // If PSF is already done, don't ask again !
    AlwaysAssert( !(donePSF_p && dopsf) , AipsError ); 
    
    // The PSF needs to be the first thing made (because of weight images)
    AlwaysAssert( !(dopsf==false && donePSF_p==false) , AipsError); 
    
    doingPSF_p=false;
    if(donePSF_p==true) // TODO : Check if we can clean up the extra ftmachines, or if the weightImageVecs from extra ones are still used later
      {
	if( subftms_p.nelements() != nterms_p )  
	  { 
	    subftms_p.resize( nterms_p ,true);
	    if(dbg_p) cout << "MTFT::initializeToSky : resizing to " << nterms_p << " terms" << endl;
	  }
      }
    uInt gridnterms=nterms_p;
    if(dopsf){ gridnterms=2*nterms_p-1; }
    
    AlwaysAssert(gridnterms <= subftms_p.nelements() , AipsError);
    AlwaysAssert(compImageVec.nelements()==gridnterms, AipsError);
    AlwaysAssert(weightsVec.nelements()==gridnterms, AipsError);
    
    
    if(dbg_p) cout << "MTFT : Calling subft initializeToSky for " << gridnterms << " terms  " << endl;
    for(uInt taylor=0;taylor<gridnterms;taylor++) 
      {
	subftms_p[taylor]->initializeToSky(*(compImageVec[taylor]),weightsVec[taylor],vb);
      }
    
  }// end of initializeToSky
  
  
  
  void NewMultiTermFT::put(VisBuffer& vb, Int row, Bool dopsf, FTMachine::Type type) 
  {
    
    subftms_p[0]->put(vb,row,dopsf,type);
    
    Int gridnterms=nterms_p;
    if(dopsf==true && donePSF_p==false) 
      {
	gridnterms=2*nterms_p-1;
	doingPSF_p=true;
      }
    
    //cout << "  Calling put for " << gridnterms << " terms, nelements :  " << subftms_p.nelements() << "  and dopsf " << dopsf << endl;
    
    for(Int tix=1;tix<gridnterms;tix++)
      {
	modifyVisWeights(vb,tix);
	subftms_p[tix]->put(vb,row,dopsf,type); 
	restoreImagingWeights(vb);
      }
    
    
  }// end of put
  
  //----------------------------------------------------------------------
  void NewMultiTermFT::finalizeToSky(Block<CountedPtr<ImageInterface<Complex> > > & compImageVec, PtrBlock<SubImage<Float> *> & resImageVec, PtrBlock<SubImage<Float> *>& weightImageVec, PtrBlock<SubImage<Float> *>& fluxScaleVec, Bool dopsf, Block<Matrix<Float> >& weightsVec, const VisBuffer& /*vb*/)
  {
    uInt gridnterms=nterms_p;
    if(dopsf==true) { gridnterms=2*nterms_p-1; }
    
    if(dbg_p) cout << "MTFT : finalizeToSky for " << gridnterms << " terms  and dopsf : " << dopsf << endl;
    
    AlwaysAssert(gridnterms <= subftms_p.nelements() , AipsError);
    AlwaysAssert(compImageVec.nelements()==gridnterms, AipsError);
    AlwaysAssert(resImageVec.nelements()==gridnterms, AipsError);
    AlwaysAssert(weightImageVec.nelements()==gridnterms, AipsError);
    AlwaysAssert(fluxScaleVec.nelements()==gridnterms, AipsError);
    AlwaysAssert(weightsVec.nelements()==gridnterms, AipsError);
    
    // Collect images and weights from all FTMs
    for(uInt taylor=0;taylor<gridnterms;taylor++) 
      {
	// Call finalizeToSky for subftm
	subftms_p[taylor]->finalizeToSky();
	// Get the gridded image
	(*(compImageVec[taylor])).copyData(subftms_p[taylor]->getImage(weightsVec[taylor],false));
	// Convert to Stokes planes
	correlationToStokes((*(compImageVec[taylor])) , (*(resImageVec[taylor])) , dopsf );
	// Get the weight image.
	subftms_p[taylor]->getWeightImage(*(weightImageVec[taylor]), weightsVec[taylor]);
      }// end for taylor

    //Normalize the weight images by the peak of the zero'th order weightImage.
    //UU    normAvgPBs(weightImageVec);

    // Norm by sumwt for PSFs and Residuals.
    for(uInt taylor=0;taylor<gridnterms;taylor++)
      {
	normalizeImage( *(resImageVec[taylor]) , weightsVec[0] , *(weightImageVec[0]) , 
			dopsf , (Float)pblimit_p, (Int)0);
	
      }// end for taylor  
    
    
    if(PSOURCE) cout << "------ residual, before normalization : " << resImageVec[0]->getAt(psource) << "," << resImageVec[1]->getAt(psource) << endl;
    
    // Use sumwts to make Hessian, invert, apply to weight images to fill in pbcoeffs
    // TODO : Clean up this list of state variables !
    if(dopsf==true && doingPSF_p==true &&  donePSF_p==false)
      {
	
	if( donePBTaylor_p == false )
	  {

	    cout << "MTFT::finalizeToSky for PSF and Weights : Calculating PB coefficients" << endl;
	    
	    // Gather normalized sumweights for the Hessian matrix.
	    sumweights_p.resize(gridnterms);
	    for(uInt taylor=0;taylor<gridnterms;taylor++)
	      { sumweights_p[taylor] = weightsVec[taylor]/weightsVec[0]; }
	    
	    // Connect pbcoeffs_p to fluxScaleVec. This is where PB Taylor coefficients will end up.
	    if(pbcoeffs_p.nelements() != nterms_p) pbcoeffs_p.resize(nterms_p);
	    for(uInt taylor=0;taylor<nterms_p;taylor++)
	      { 
		pbcoeffs_p[taylor] = &(*(fluxScaleVec[taylor])); 
	      }
	    
	    // Do the calculation
	    // UUU Taken out ( Sep2013 ) because sqrt(PB) is being accumulated and this is not OK
	    //         for calculating Coeff PBs.  Need to eventually.....
	    //         ......store PBSQ, Calc coeffs for PB squared, Do a polynomial square root.
	    //         Until then, 'coeffPB' cannot be used for post-deconv corrections.
	    //         Use sensitivityPB, and remember when it's a square and when not.
	    //
	    //calculateTaylorPBs(weightImageVec);
	  
	donePBTaylor_p = true;
	
	
	  }

	// Show the value....
	if(PSOURCE) cout << " PB 0 : " << pbcoeffs_p[0]->getAt(psource) << " PB 1 : " <<  pbcoeffs_p[1]->getAt(psource)  << endl;
	
	
      }// if dopsf
    
    
    /*
    // Normalize all by the Taylor0 weights
    AlwaysAssert( sensitivitymaps_p.nelements() > 0 , AipsError );
    for(uInt taylor=0;taylor<gridnterms;taylor++)
    {
    normalizeImage( *(resImageVec[taylor]) , weightsVec[0] , *(weightImageVec[0]) , dopsf , (Float)pblimit_p, (Int)1);   //// use locally-normalized avgPB0.
    }// end for taylor  
    */
    
    if(PSOURCE) cout << "------ residual, after normalization : " << resImageVec[0]->getAt(psource) << "," << resImageVec[1]->getAt(psource) << endl;
    
    
    
    if(doingPSF_p==true)
      {doingPSF_p=false; donePSF_p=true; if(dbg_p) cout << "Setting donePSF to true" << endl;}
    
    
  }//end of finalizeToSky
  
  //---------------------------------------------------------------------------------------------------
  //----------------------------- Obtain Images -----------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  //----------------------------------------------------------------------
  void NewMultiTermFT::makeImage(FTMachine::Type type, VisSet& vs,
				 ImageInterface<Complex>& theImage,  Matrix<Float>& weight) 
  {
    if(dbg_p) cout << "MTFT :: makeImage for taylor 0 only "<< endl;
    subftms_p[0]->makeImage(type, vs, theImage, weight);
  }
  
  // Connect sensitivitymaps_p to weightImages, and normalize all by peak of 0th one. 
  void NewMultiTermFT::normAvgPBs(PtrBlock<SubImage<Float> *>& weightImageVec)
  {  
    AlwaysAssert( weightImageVec.nelements() >= nterms_p , AipsError );
    Matrix<Float> tempMat;
    Array<Float> tempArr;
    ( *(weightImageVec[0]) ).get(tempArr,true);
    tempMat.reference(tempArr);
    Float maxval = max(tempMat);
    
    /*
      if(sensitivitymaps_p.nelements()==0 && weightImageVec.nelements() == 2*nterms_p-1) 
      {
      sensitivitymaps_p.resize(2*nterms_p-1);
      for(uInt taylor=0;taylor<2*nterms_p-1;taylor++)
      {
      Float rmaxval = maxval;
      cout << "Normalizing pb : " << taylor << " by peak of zeroth : " << rmaxval << endl;
      
      sensitivitymaps_p[taylor] = new PagedImage<Float>( (weightImageVec[taylor])->shape() , (weightImageVec[taylor])->coordinates() , cacheDir_p+"/sensitivityPB_"+String::toString(taylor)  );
      sensitivitymaps_p[taylor]->copyData( (LatticeExpr<Float>) ( (*(weightImageVec[taylor]))/rmaxval ) );
      }
      }
    */
    
    // Normalize weightImageVecs in-place
    for(uInt taylor=0;taylor<weightImageVec.nelements();taylor++)
      {
	//cout << "MTFT :: Normalizing wtimg : " << taylor << " by peak of zeroth : " << maxval << endl;
	weightImageVec[taylor]->copyData( (LatticeExpr<Float>) ( (*(weightImageVec[taylor]))/maxval ) );
      }
    
  }
  
  
  //---------------------------------------------------------------------------------------------------
  //------------------------ To / From Records ---------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  Bool NewMultiTermFT::toRecord(String& error, RecordInterface& outRec, Bool withImage, const String diskimage) 
  {
    if(dbg_p) cout << "MTFT :: toRecord for " << nterms_p << endl;
    Bool retval = true;
    
    for(uInt tix=0;tix<nterms_p;tix++)
      {
	Record subFTContainer;
	String elimage="";
	if(diskimage != ""){
	  elimage=diskimage+String("_term_")+String::toString(tix);
	}
	subftms_p[tix]->toRecord(error, subFTContainer,withImage, elimage);
	outRec.defineRecord("subftm_"+String::toString(tix),subFTContainer);
      }
    
    //    Record subFTContainer;
    //  subftm_p->toRecord(error, subFTContainer,withImage);
    //  outRec.defineRecord("subftm",subFTContainer);
    
    outRec.define("nterms",nterms_p);
    outRec.define("reffreq",reffreq_p);
    
    return retval;
  }
  
  //---------------------------------------------------------------------------------------------------
  Bool NewMultiTermFT::fromRecord(String& error, const RecordInterface& inRec)
  {
    if(dbg_p) cout << "MTFT :: fromRecord "<< endl;
    Bool retval = true;
    
    //    Record subFTMRec=inRec.asRecord("subftm");
    //  retval = (retval || subftm_p->fromRecord(error, subFTMRec));    
    
    inRec.get("nterms",nterms_p);
    inRec.get("reffreq",reffreq_p);
    
    subftms_p.resize(nterms_p);
    for(uInt tix=0;tix<nterms_p;tix++)
      {
	Record subFTMRec=inRec.asRecord("subftm_"+String::toString(tix));
	retval = (retval || subftms_p[tix]->fromRecord(error, subFTMRec));    
      }
    
    
    return retval;
  }
  //---------------------------------------------------------------------------------------------------
  
  Bool NewMultiTermFT::storeAsImg(String fileName, ImageInterface<Float>& theImg)
  {
    PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), fileName);
    LatticeExpr<Float> le(theImg);
    tmp.copyData(le);
    return true;
  }
  
  
  //---------------------------------------------------------------------------------------------------
  // Make pixel-by-pixel matrices from pbcoeffs, (invert), and multiply with the given vector (in place)
  void NewMultiTermFT::applyWideBandPB(String action, PtrBlock<SubImage<Float> *> &imageVec)
  {
    //readAvgPBs(); // Should read only once.
    AlwaysAssert( pbcoeffs_p.nelements()==nterms_p , AipsError );
    AlwaysAssert( imageVec.nelements()==nterms_p , AipsError );
    
    Int nX=imageVec[0]->shape()(0);
    Int nY=imageVec[0]->shape()(1);
    Int npol=imageVec[0]->shape()(2);
    Int nchan=imageVec[0]->shape()(3);
    
    AlwaysAssert(nchan==1,AipsError);
    AlwaysAssert(npol==1,AipsError);
    
    Double deter=0.0;
    Matrix<Double> mat( IPosition(2,nterms_p,nterms_p) );
    Matrix<Double> invmat( IPosition(2,nterms_p,nterms_p) );

    mat.set(0.0);
    invmat.set(0.0);

    // Go over all pixels for which coeffPB_0 is above pblimit_p
    Vector<Float> pbvec(nterms_p), invec(nterms_p), outvec(nterms_p);
    IPosition tip(4,0,0,0,0);
    for(tip[0]=0;tip[0]<nX;tip[0]++)
      {
	for(tip[1]=0;tip[1]<nY;tip[1]++)
	  {
	    // Normalize only if pb > limit
	    if(fabs(pbcoeffs_p[0]->getAt(tip)) > pblimit_p )
	      {
		// Fill in the single-pixel Hessian, and RHS and LHS vectors
		for(uInt taylor1=0;taylor1<nterms_p;taylor1++)
		  {
		    for(uInt taylor2=0;taylor2<=taylor1;taylor2++)
		      {
			mat(taylor1,taylor2) = (pbcoeffs_p[ taylor1 - taylor2 ])->getAt(tip);
		      }
		    invec[taylor1] = imageVec[taylor1]->getAt(tip);
		    outvec[taylor1]=0.0;
		  }
		
		// Invert hess_p into invhess_p;
		try
		  {
		    //invertSymPosDef((invmat),deter,(mat));
		    invert((invmat),deter,(mat));
		  }
		catch(AipsError &x)
		  {
		    cout << "The non-invertible Matrix is : " << (mat) << endl;
		    throw( AipsError("Cannot Invert matrix for PB application: " + x.getMesg() ) );
		  }

		if(PSOURCE)
		  {
		    if( tip[0] == psource[0] && tip[1] == psource[1] )
		      {
			cout << "PB mat : " << mat << endl;
			cout << "InvPB mat : " << invmat << endl;
		      }
		  }
	
		// Multiply invhess_p by RHS and fill in LHS vector
		for(uInt taylor1=0;taylor1<nterms_p;taylor1++)
		  {
		    outvec[taylor1]=0.0;
		    for(uInt taylor2=0;taylor2<nterms_p;taylor2++)
		      {
			if( action == String("divide") )
			  {
			    outvec[taylor1] += invmat(taylor1,taylor2) * (invec[taylor2]) ;
			  }
			else
			  {
			    outvec[taylor1] += mat(taylor1,taylor2) * (invec[taylor2]) ;
			  }
		      } // for taylor2
		  }// for taylor1
		
		/*
		  if(tip==psource)
		  {
		  cout << "------ normalizeWideBandPB2 : before : " << imageVec[0]->getAt(tip) << "," << imageVec[1]->getAt(tip) << " after : " << outvec[0] << "," << outvec[1] << "  hess : " << hess_p << endl;
		  }
		*/
		
		// Put the solution into the residual images.
		for(uInt taylor=0;taylor<nterms_p;taylor++)
		  imageVec[taylor]->putAt(outvec[taylor], tip);
		
	      }// if larger than pblimit
	    else // if smaller than pblimit
	      {
		// Put 0.0 into the residual images for un-normalizable parts
		for(uInt taylor=0;taylor<nterms_p;taylor++)
		  imageVec[taylor]->putAt((Float)0.0, tip);
	      }
	    
	  }//end of for y
      }// end of for x
    
  }// end of applyWideBandPB


  /*
  
  //---------------------------------------------------------------------------------------------------
  // Make pixel-by-pixel matrices from pbcoeffs, (invert), and multiply with the given vector (in place)
  void NewMultiTermFT::applyWideBandPB(String action, PtrBlock<SubImage<Float> *> &imageVec)
  {
    //readAvgPBs(); // Should read only once.
    AlwaysAssert( sensitivitymaps_p.nelements()==2*nterms_p-1 , AipsError );
    AlwaysAssert( imageVec.nelements()==nterms_p , AipsError );
    
    Int nX=imageVec[0]->shape()(0);
    Int nY=sensitivitymaps_p[0]->shape()(1);
    Int npol=sensitivitymaps_p[0]->shape()(2);
    Int nchan=sensitivitymaps_p[0]->shape()(3);
    
    AlwaysAssert(nchan==1,AipsError);
    AlwaysAssert(npol==1,AipsError);
    
    Double deter=0.0;
    hess_p.resize(IPosition(2,nterms_p,nterms_p) );
    invhess_p.resize(IPosition(2,nterms_p,nterms_p) );
    
    //  IPosition psource(4,nX/2+22,nY/2,0,0);
    
    // cout << "Source Position : " << psource << endl;
    
    // Go over all pixels for which avgPB_tt0 is above pblimit_p
    Vector<Float> pbvec(nterms_p), resvec(nterms_p), normedvec(nterms_p);
    IPosition tip(4,0,0,0,0);
    for(tip[0]=0;tip[0]<nX;tip[0]++)
      {
	for(tip[1]=0;tip[1]<nY;tip[1]++)
	  {
	    // Normalize only if pb > limit
	    if(1) // fabs(sensitivitymaps_p[0]->getAt(tip)) > pblimit_p )
	      {
		
		// Fill in the single-pixel Hessian, and RHS and LHS vectors
		for(uInt taylor1=0;taylor1<nterms_p;taylor1++)
		  {
		    for(uInt taylor2=0;taylor2<nterms_p;taylor2++)
		      {
			hess_p(taylor1,taylor2) = (sensitivitymaps_p[taylor1+taylor2])->getAt(tip);
		      }
		    resvec[taylor1] = imageVec[taylor1]->getAt(tip);
		    normedvec[taylor1]=0.0;
		  }
		
		// Invert hess_p into invhess_p;
		try
		  {
		    invertSymPosDef((invhess_p),deter,(hess_p));
		  }
		catch(AipsError &x)
		  {
		    cout << "The non-invertible Hessian is : " << (hess_p) << endl;
		    throw( AipsError("Cannot Invert matrix : " + x.getMesg() ) );
		  }
		
		// Multiply invhess_p by RHS and fill in LHS vector
		for(uInt taylor1=0;taylor1<nterms_p;taylor1++)
		  {
		    normedvec[taylor1]=0.0;
		    for(uInt taylor2=0;taylor2<nterms_p;taylor2++)
		      {
			normedvec[taylor1] += invhess_p(taylor1,taylor2) * (resvec[taylor2]) ;
		      } // for taylor2
		  }// for taylor1
		
		// Put the solution into the residual images.
		for(uInt taylor=0;taylor<nterms_p;taylor++)
		  imageVec[taylor]->putAt(normedvec[taylor], tip);
		
	      }// if larger than pblimit
	    else // if smaller than pblimit
	      {
		// Put 0.0 into the residual images for un-normalizable parts
		for(uInt taylor=0;taylor<nterms_p;taylor++)
		  imageVec[taylor]->putAt((Float)0.0, tip);
	      }
	    
	  }//end of for y
      }// end of for x
    
  }// end of normalizeWideBandPB2
  
 */
 
  //---------------------------------------------------------------------------------------------------
  // Use sumwts to make a Hessian, invert it, apply to weight images, fill in pbcoeffs_p
  // This should get called only once, while making PSFs, when there are 2n-1 terms.
  void NewMultiTermFT::calculateTaylorPBs(PtrBlock<SubImage<Float> *> & weightImageVec)
  {

    AlwaysAssert( weightImageVec.nelements() == 2*nterms_p-1 , AipsError );

	for(uInt taylor=0;taylor<2*nterms_p-1;taylor++)
	  {
	    storeImg( cacheDir_p+"/sensitivityPB_"+String::toString(taylor) , *weightImageVec[taylor] );
	  }
	
	/// Just read from weightimage, instead of sensitivity images...
	
	AlwaysAssert( pbcoeffs_p.nelements()==nterms_p, AipsError );
	AlwaysAssert( sumweights_p.nelements()==2*nterms_p-1, AipsError );
	
	Int npol=weightImageVec[0]->shape()(2);
	Int nchan=weightImageVec[0]->shape()(3);
	
	AlwaysAssert(nchan==1,AipsError);
	AlwaysAssert(npol==1,AipsError);
	
	Double deter=0.0;
	hess_p.resize(IPosition(2,nterms_p,nterms_p) );
	invhess_p.resize(IPosition(2,nterms_p,nterms_p) );
	
	// Fill  hess_p from sumweights_p;
	for(uInt taylor1=0;taylor1<nterms_p;taylor1++)
	  {
	    for(uInt taylor2=0;taylor2<nterms_p;taylor2++)
	      {
		hess_p(taylor1,taylor2) = (Double)( (sumweights_p[taylor1+taylor2])(0,0) );
	      }// for taylor2
	  }// for taylor1
	cout << "Hessian : " << hess_p << endl;
	
	// Invert hess_p into invhess_p;
	try
	  {
	    invertSymPosDef((invhess_p),deter,(hess_p));
	  }
	catch(AipsError &x)
	  {
	    cout << "The non-invertible Hessian is : " << (hess_p) << endl;
	    throw( AipsError("Cannot Invert matrix : " + x.getMesg() ) );
	  }
	cout << "Inverse Hessian : " << invhess_p << endl;

	multiplyHMatrix( invhess_p, weightImageVec, pbcoeffs_p, String("/coeffPB_") );

/*	
	// Multiply invhess_p by sumweights_p and fill in pbcoeffs_p (Fig 7.3)
	for(uInt taylor1=0;taylor1<nterms_p;taylor1++)
	  {
	    LatticeExprNode len(0.0);
	    for(uInt taylor2=0;taylor2<nterms_p;taylor2++)
	      {
		len = len + LatticeExprNode(invhess_p(taylor1,taylor2) * (*(weightImageVec[taylor2])) );
	      } // for taylor2
	    
	    pbcoeffs_p[taylor1]->copyData(LatticeExpr<Float> (iif( abs(len)<pblimit_p*pblimit_p , 0.0 , len   )) );
	    ///pbcoeffs_p[taylor1]->copyData(LatticeExpr<Float> (len) );
	    storeImg(cacheDir_p+"/coeffPB_"+String::toString(taylor1) , *(pbcoeffs_p[taylor1]) );
	  }// for taylor1
*/
	
    return;
  }

  void NewMultiTermFT::multiplyHMatrix( Matrix<Double> &hmat, 
					PtrBlock<SubImage<Float>* > &invec, 
					PtrBlock<SubImage<Float>* > &outvec,
					String saveImagePrefix )
  {
    AlwaysAssert( hmat.shape().nelements()==2 && 
		  hmat.shape()[0] == nterms_p && 
		  hmat.shape()[1] == nterms_p , AipsError );
    AlwaysAssert( invec.nelements() >= nterms_p , AipsError );
    AlwaysAssert( outvec.nelements() >= nterms_p, AipsError );

    for(uInt taylor1=0;taylor1<nterms_p;taylor1++)
      {
	LatticeExprNode len(0.0);
	for(uInt taylor2=0;taylor2<nterms_p;taylor2++)
	  {
	    len = len + LatticeExprNode(hmat(taylor1,taylor2) * (*(invec[taylor2])) );
	  } // for taylor2
	
	outvec[taylor1]->copyData(LatticeExpr<Float> (iif( abs(len)<pblimit_p*pblimit_p , 0.0 , len   )) );

	if( saveImagePrefix.length() > 0 )
	  {
	    storeImg(cacheDir_p+saveImagePrefix+String::toString(taylor1) , *(outvec[taylor1]) );
	  }
      }// for taylor1
    
  }// end of multiplyHMatrix
  

  /*

  //---------------------------------------------------------------------------------------------------
  // Apply inverse of single Hessian to residuals
  void NewMultiTermFT::normalizeWideBandPB(PtrBlock<SubImage<Float> *> &resImageVec, PtrBlock<SubImage<Float> *>& scratchImageVec)
  {
    AlwaysAssert( scratchImageVec.nelements()==nterms_p , AipsError );
    AlwaysAssert( pbcoeffs_p.nelements()==nterms_p , AipsError );
    AlwaysAssert( resImageVec.nelements()>=nterms_p , AipsError );
    
    
    cout << "MTFT::normWideBandPB  : normalizing with " << nterms_p << " terms and pblim :  " << pblimit_p << endl;
    // Do a polynomial division of the PB from the residual images (using pbcoeffs_p) (Fig 7.4)
    
    
    switch(nterms_p)
      {
      case 1:
	{
	  LatticeExprNode deter1( (LatticeExpr<Float>) ( (*(pbcoeffs_p[0])) ) );
	  ///(resImageVec[0])->copyData( (LatticeExpr<Float>)( (*(resImageVec[0]))/(*(pbcoeffs_p[0]))  )  );
	  (resImageVec[0])->copyData( (LatticeExpr<Float>) 
				      ( iif( abs(deter1)<pblimit_p , 0.0 , (*(resImageVec[0]))/deter1 ) ) );
	  break;
	}// end case 1
	
      case 2:
	{
	  LatticeExprNode deter2( (LatticeExpr<Float>) ( (*(pbcoeffs_p[0])) * (*(pbcoeffs_p[0])) ) );
	  
	  (scratchImageVec[0])->copyData( (LatticeExpr<Float>)( 
							       (*(pbcoeffs_p[0]))*(*(resImageVec[0])) )   );
	  (scratchImageVec[1])->copyData( (LatticeExpr<Float>) ( 
								(  ( -1.0* (*(pbcoeffs_p[1]))*(*(resImageVec[0])) ) + 
								   ( (*(pbcoeffs_p[0]))*(*(resImageVec[1])) )
								   ) )   );
	  
	  for (uInt tay=0;tay<nterms_p;tay++)
	    {
	      (resImageVec[tay])->copyData( (LatticeExpr<Float>) 
					    ( iif( abs(deter2)<pblimit_p*pblimit_p , 0.0 , (*(scratchImageVec[tay]))/deter2 ) ) );
	    }
	  break;
	}// end case 2
	
      case 3:
	{
	  LatticeExprNode deter3( (LatticeExpr<Float>) ( (*(pbcoeffs_p[0])) * (*(pbcoeffs_p[0])) * (*(pbcoeffs_p[0])  ) ) );
	  (scratchImageVec[0])->copyData( (LatticeExpr<Float>) ( 
								(*(pbcoeffs_p[0])) * (*(pbcoeffs_p[0])) * (*(resImageVec[0]))  )  );
	  (scratchImageVec[1])->copyData( (LatticeExpr<Float>) ( ( 
								  ( -1.0 * (*(pbcoeffs_p[0]) )*(*(pbcoeffs_p[1]))*(*(resImageVec[0])) ) + 
								  ( (*(pbcoeffs_p[0]))*(*(pbcoeffs_p[0]))*(*(resImageVec[1])) )   
								   )  )  );
	  (scratchImageVec[2])->copyData((LatticeExpr<Float>) ( ( 
								 ( 
								  ( (*(pbcoeffs_p[1]))*(*(pbcoeffs_p[1])) - (*(pbcoeffs_p[0]))*(*(pbcoeffs_p[2])) 
								    ) * (*(resImageVec[0]))  
								   ) + 
								 ( -1.0 * (*(pbcoeffs_p[0])) * (*(pbcoeffs_p[1])) * (*(resImageVec[1]))  ) + 
								 ( (*(pbcoeffs_p[0])) * (*(pbcoeffs_p[0])) * (*(resImageVec[2]))  )
								  )  )  );
	  for (uInt tay=0;tay<nterms_p;tay++)
	    {
	      (resImageVec[tay])->copyData( (LatticeExpr<Float>) 
					    ( iif( abs(deter3)<pblimit_p*pblimit_p*pblimit_p , 0.0 , (*(scratchImageVec[tay]))/deter3 ) ) );
	    }
	  break;
	}// end case 3
	
      default:
	throw( AipsError("Cannot compute PB Coefficients for more than nterms=3 right now...") );
	
      }// end of switch nterms
    
    /////// Note : To test this, apply it to the pbcoeffs_p and write to disk. They should be ones. 
    
    return;
  }

  */

  /*  
  // invert, apply to residuals
  void NewMultiTermFT::applyWideBandPB(PtrBlock<SubImage<Float> *> &modelImageVec)
  {
    AlwaysAssert( pbcoeffs_p.nelements()==nterms_p , AipsError );
    AlwaysAssert( modelImageVec.nelements()>=nterms_p , AipsError );
    
    multiplyHMatrix( hess_p, modelImageVec, pbcoeffs_p, String("/coeffPB_") );
  }
  */
  
  
  //---------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  
} //# NAMESPACE CASA - END

