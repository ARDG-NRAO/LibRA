//# MultiTermFT.cc: Implementation of MultiTermFT class
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
//#include <lattices/Lattices/ArrayLattice.h>
//#include <measures/Measures/UVWMachine.h>
//#include <lattices/Lattices/SubLattice.h>
//#include <lattices/LRegions/LCBox.h>
//#include <lattices/Lattices/LatticeCache.h>
//#include <lattices/LatticeMath/LatticeFFT.h>
//#include <lattices/Lattices/LatticeIterator.h>
//#include <lattices/Lattices/LatticeStepper.h>
//#include <scimath/Mathematics/ConvolveGridder.h>
//#include <casa/Utilities/CompositeNumber.h>
#include <casacore/casa/OS/Timer.h>
#include <sstream>

#include <synthesis/TransformMachines/MultiTermFT.h>
#include <synthesis/TransformMachines/VisModelData.h>

// This is the list of FTMachine types supported by MultiTermFT
#include <synthesis/TransformMachines/GridFT.h>
#include <synthesis/TransformMachines/WProjectFT.h>
//#include <synthesis/MeasurementComponents/AWProjectWBFT.h>
//#include <synthesis/MeasurementComponents/rGridFT.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
//---------------------------------------------------------------------- 
//-------------------- constructors and descructors ---------------------- 
//---------------------------------------------------------------------- 
  MultiTermFT::MultiTermFT(FTMachine *subftm, String subFTMname, Int nterms, Double reffreq)
    :FTMachine(), subftm_p(subftm), subFTMname_p(subFTMname), nterms_p(nterms), 
     thisterm_p(0), reffreq_p(reffreq), imweights_p(Matrix<Float>(0,0)), machineName_p("MultiTermFT")
  {
    dbg_p=false;
    dotime_p=false;
    this->setBasePrivates(*subftm_p);
    canComputeResiduals_p = subftm_p->canComputeResiduals();
    if(dbg_p) cout << "MTFT :: constructor with subftm : "<< subFTMname_p << endl;
    if(dbg_p) cout << "can compute residuals : " << canComputeResiduals_p << endl;

    sumwt_p=0.0; 

    time_get=0.0;
    time_put=0.0;
    time_res=0.0;
  }

//---------------------------------------------------------------------- 
// Construct from the input state record
MultiTermFT::MultiTermFT(const RecordInterface& stateRec)
: FTMachine()
{
  String error;
  if (!fromRecord(error, stateRec)) {
    throw (AipsError("Failed to create gridder: " + error));
  };
}

//----------------------------------------------------------------------
// Copy constructor
  MultiTermFT::MultiTermFT(const MultiTermFT& other) : FTMachine(), machineName_p("MultiTermFT")
{ 
    operator=(other);
}

MultiTermFT& MultiTermFT::operator=(const MultiTermFT& other)
{
	 
   dbg_p = other.dbg_p;
   dotime_p = other.dotime_p;
     if(dbg_p) cout << "In MTFT operator= for " << other.subftm_p->name() << endl;

     if(this!=&other)
       {
	 FTMachine::operator=(other);
	 
	 //if(other.subftm_p==0) throw(AipsError("Internal Error : Empty subFTMachine"));
	 
	 // The operator= would have copied only the pointer, and not made a new instance of subftm_p
	 // Make the new instance of subftm_p here. 
	 // Ideally, this should call "clone" of that ftm, so that the if/else stuff can all go away.
	 if(other.subFTMname_p=="GridFT") 
	   { subftm_p = new GridFT(static_cast<const GridFT&>(*other.subftm_p)); }
	 else if(other.subFTMname_p=="WProjectFT") 
	   { subftm_p = new WProjectFT(static_cast<const WProjectFT&>(*other.subftm_p)); }
	 /*	 else if(other.subFTMname_p=="wbawp") 
	   { subftm_p = new AWProjectWBFT(static_cast<const AWProjectWBFT&>(*other.subftm_p)); }
	 else if(other.subFTMname_p=="nift") 
	   { 
	     subftm_p = ((rGridFT*)&(*other.subftm_p))->clone();
	     //cout << "MTFT : copy constructor : newft->visresampler_p : " << &(*(subftm_p->visResampler_p)) << endl;
	   }
	 */
	 else 
	   {throw(AipsError("FTMachine "+other.subFTMname_p+" is not supported with MS-MFS")); }
	 
	 //    subftm_p->setBasePrivates(*this);
	 
	 // Copy local privates
	 subFTMname_p = other.subFTMname_p;
	 machineName_p = other.machineName_p;
	 nterms_p = other.nterms_p;
	 thisterm_p = other.thisterm_p;
	 reffreq_p = other.reffreq_p;
	 sumwt_p = other.sumwt_p;
	 
	 // Check if the sub ftm can calculate its own residuals....
	 canComputeResiduals_p = subftm_p->canComputeResiduals();
       }

     return * this;
}
  
  //----------------------------------------------------------------------
  MultiTermFT::~MultiTermFT()
  {
    //  if(dbg_p) cerr << "MTFT :: destructor for term " << thisterm_p << " - deletes subftm explicitly " << endl;
    if(dbg_p) cout << "MTFT :: destructor for term " << thisterm_p << " - assumes automatic deletion of subftm " << endl;
    //if(subftm_p) { delete subftm_p; subftm_p=0; }
  }
  
  
  //---------------------------------------------------------------------------------------------------
  //------------ Multi-Term Specific Functions --------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  
  // Multiply the imaging weights by Taylor functions - in place
  // This function MUST be called in ascending Taylor-term order
  // NOTE : Add checks to ensure this.
  Bool MultiTermFT::modifyVisWeights(VisBuffer& vb)
  {
    if( thisterm_p > 0 )
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
	      (vb.imagingWeight())(chn,row) *= pow( mulfactor, thisterm_p );
	      //	      sumwt_p += (vb.imagingWeight())(chn,row);
            }
      }
    /*
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
Bool MultiTermFT::restoreImagingWeights(VisBuffer &vb)
{
  if(thisterm_p>0)
    {
      AlwaysAssert( imweights_p.shape() == vb.imagingWeight().shape() ,AipsError);
      vb.imagingWeight() = imweights_p;
    }
  return true;
}

  
  // Multiply the model visibilities by the Taylor functions - in place.
  Bool MultiTermFT::modifyModelVis(VisBuffer& vb)
  {
    if( thisterm_p > 0 )
      {
        Float freq=0.0,mulfactor=1.0;
        Vector<Double> selfreqlist(vb.frequency());
	
        for (uInt pol=0; pol< uInt((vb.modelVisCube()).shape()[0]); pol++)
	  for (uInt chn=0; chn< uInt(vb.nChannel()); chn++)
	    for (uInt row=0; row< uInt(vb.nRow()); row++)
	      {
		freq = selfreqlist(IPosition(1,chn));
		mulfactor = ((freq-reffreq_p)/reffreq_p);
		(vb.modelVisCube())(pol,chn,row) *= pow(mulfactor,thisterm_p);
	      }
      }
    
    return true;
  }
  
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Prediction and De-gridding -----------------------------------
  //---------------------------------------------------------------------------------------------------
  void MultiTermFT::initializeToVis(ImageInterface<Complex>& iimage,
				    const VisBuffer& vb)
  {
    if(dbg_p) cout << "MTFT::initializeToVis for term " << thisterm_p << endl;
    subftm_p->initializeToVis(iimage,vb);
    time_get=0.0;
  }
  
void MultiTermFT::initMaps(const VisBuffer& vb){
  subftm_p->initMaps(vb);
}

  void MultiTermFT::get(VisBuffer& vb, Int row)
  {
    if(dotime_p) tmr_p.mark();
    
    subftm_p->get(vb,row);
    modifyModelVis(vb);
    
    if(dotime_p) time_get += tmr_p.real();
  }
  
  void MultiTermFT::finalizeToVis()
  {
    if(dbg_p) cout << "MTFT::finalizeToVis for term " << thisterm_p <<endl;
    subftm_p->finalizeToVis();
    if(dotime_p) cout << " taylor " << thisterm_p << "***************   get time : " << time_get << endl;
  }
  
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Calculate Residual Visibilities -------------------------------
  //---------------------------------------------------------------------------------------------------
  void MultiTermFT::ComputeResiduals(VisBuffer &vb, Bool useCorrected)
  {
    if(dotime_p) tmr_p.mark();
    
    if(subftm_p->canComputeResiduals()) subftm_p->ComputeResiduals(vb,useCorrected);
    else throw(AipsError("MultiTerm::ComputeResiduals : subftm of MultiTermFT cannot compute its own residuals !"));
    
    if(dotime_p) time_res += tmr_p.real();
  }
  
  //---------------------------------------------------------------------------------------------------
  //----------------------  Gridding --------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  void MultiTermFT::initializeToSky(ImageInterface<Complex>& iimage,
				    Matrix<Float>& weight, const VisBuffer& vb)
  {
    if(dbg_p) cout << "MTFT::initializeToSky for term " << thisterm_p << endl;
    
    subftm_p->initializeToSky(iimage,weight,vb);
    
    //    sumwt_p=0.0; 
    
    if(dotime_p) {time_put=0.0; time_res=0.0;}
  }
  
  void MultiTermFT::put(VisBuffer& vb, Int row, Bool dopsf, 
			FTMachine::Type type) //, const Matrix<Float>& imwght) // Last parameter is ignored.
  {
    if(dotime_p) tmr_p.mark();
    
    modifyVisWeights(vb);
    subftm_p->put(vb,row,dopsf,type); 
    restoreImagingWeights(vb);

    if(dotime_p) time_put += tmr_p.real();
  }
  
  void MultiTermFT::finalizeToSky()
  {  
    if(dbg_p) cout << "MTFT::finalizeToSky for term " << thisterm_p << endl;
    
    subftm_p->finalizeToSky();
    
    //    cout << "***** sumwt : " << sumwt_p << endl;
    
    if(dotime_p) cout << " taylor " << thisterm_p << "*************** can compute residual " << canComputeResiduals_p << " res time : " << time_res << "   put time  :" << time_put << endl;
  }
  
  //---------------------------------------------------------------------------------------------------
  //----------------------------- Obtain Images -----------------------------------------------------
  //---------------------------------------------------------------------------------------------------
  ImageInterface<Complex>& MultiTermFT::getImage(Matrix<Float>& weights, Bool normalize) 
  {
    if(dbg_p) cout << "MTFT :: getImage for term " << thisterm_p << endl;
    return subftm_p->getImage(weights,normalize);
  }
  
  //----------------------------------------------------------------------
  void MultiTermFT::getWeightImage(ImageInterface<Float>& weightImage, Matrix<Float>& weights) 
  {
    if(dbg_p) cout << "MTFT :: getWeightImage for term " << thisterm_p << endl;
    subftm_p->getWeightImage(weightImage, weights);
  }
  
  //----------------------------------------------------------------------
  void MultiTermFT::makeImage(FTMachine::Type type, VisSet& vs,
			      ImageInterface<Complex>& theImage,  Matrix<Float>& weight) 
  {
    if(dbg_p) cout << "MTFT :: makeImage for term " << thisterm_p << endl;
    subftm_p->makeImage(type, vs, theImage, weight);
  }
  
  //---------------------------------------------------------------------------------------------------
  //------------------------ To / From Records ---------------------------------------------------------
  //---------------------------------------------------------------------------------------------------
Bool MultiTermFT::toRecord(String& error, RecordInterface& outRec, Bool withImage, const String diskimage) 
  {
    if(dbg_p) cout << "MTFT :: toRecord for term " << thisterm_p << endl;
    Bool retval = true;
    //no image is held in this machine so no image need to be saved
    // the subftm holds the image
    if(!FTMachine::toRecord(error, outRec, false))
      return false;

    Record subFTContainer;
    String elimage="";
    if(diskimage != ""){
      elimage=diskimage+String("_")+ String::toString(rand());
      while(Table::isReadable(elimage))
	    elimage=diskimage+String("_")+ String::toString(rand());
    }
    subftm_p->toRecord(error, subFTContainer,withImage, elimage);
    
    outRec.defineRecord("subftm",subFTContainer);
    outRec.define("subftname", subFTMname_p);
    outRec.define("nterms",nterms_p);
    outRec.define("thisterm",thisterm_p);
    outRec.define("reffreq",reffreq_p);
    outRec.define("dbg", dbg_p);
    outRec.define("dotime", dotime_p);
    outRec.define("time_get", time_get);
    outRec.define("time_put", time_put);
    outRec.define("time_res", time_res);

    return retval;
  }
  
  //---------------------------------------------------------------------------------------------------
  Bool MultiTermFT::fromRecord(String& error, const RecordInterface& inRec)
  {
    
    Bool retval = true;
    
    if(!FTMachine::fromRecord(error, inRec))
      return false;

    Record subFTMRec=inRec.asRecord("subftm");
    subftm_p=VisModelData::NEW_FT(subFTMRec);
    if (subftm_p.null())
      return false;
    
    
    inRec.get("subftname",subFTMname_p);
    inRec.get("nterms",nterms_p);
    inRec.get("thisterm",thisterm_p);
    //if(dbg_p) cout << "MTFT :: fromRecord for term " << thisterm_p << endl;
    inRec.get("reffreq",reffreq_p);
    inRec.get("dbg", dbg_p);
    inRec.get("dotime", dotime_p);
    inRec.get("time_get", time_get);
    inRec.get("time_put", time_put);
    inRec.get("time_res", time_res);
    machineName_p="MultiTermFT";
    return retval;
  }
  //---------------------------------------------------------------------------------------------------
  
} //# NAMESPACE CASA - END

