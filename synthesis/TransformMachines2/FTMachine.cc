//# FTMachine.cc: Implementation of FTMachine class
//# Copyright (C) 1997-2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU General Public License
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
#include <cmath>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/UnitMap.h>
#include <casacore/casa/Quanta/UnitVal.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/Quanta/Euler.h>
#include <casacore/casa/Quanta/RotMatrix.h>
#include <casacore/measures/Measures/MFrequency.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/lattices/Lattices/LatticeLocker.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/SkyJones.h>
#include <synthesis/TransformMachines2/VisModelData.h>
#include <synthesis/TransformMachines2/BriggsCubeWeightor.h>
#include <casacore/scimath/Mathematics/RigidVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/MSUtil.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MatrixMath.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/MatrixIter.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/BinarySearch.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/scimath/Mathematics/NNGridder.h>
#include <casacore/scimath/Mathematics/ConvolveGridder.h>
#include <casacore/measures/Measures/UVWMachine.h>

#include <casacore/casa/System/ProgressMeter.h>

#include <casacore/casa/OS/Timer.h>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace casacore;
namespace casa{//# CASA namespace
namespace refim {//# namespace refactor imaging
  
using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::refim;
using namespace casacore;
using namespace casa::vi;
  FTMachine::FTMachine() : isDryRun(false), image(0), uvwMachine_p(0), 
			   tangentSpecified_p(false), fixMovingSource_p(false), 
                           ephemTableName_p(""), 
			   movingDirShift_p(0.0), 
			   distance_p(0.0), lastFieldId_p(-1),lastMSId_p(-1), romscol_p(nullptr), 
			   useDoubleGrid_p(false), 
			   freqFrameValid_p(false), 
			   freqInterpMethod_p(InterpolateArray1D<Double,Complex>::nearestNeighbour), 
			   pointingDirCol_p("DIRECTION"),
			   cfStokes_p(), cfCache_p(), cfs_p(), cfwts_p(), cfs2_p(), cfwts2_p(), 
			   canComputeResiduals_p(false), toVis_p(true), 
                           numthreads_p(-1), pbLimit_p(0.05),sj_p(0), cmplxImage_p( ), vbutil_p(), phaseCenterTime_p(-1.0), doneThreadPartition_p(-1), briggsWeightor_p(nullptr), tempFileNames_p(0)
  {
    spectralCoord_p=SpectralCoordinate();
    isPseudoI_p=false;
    spwChanSelFlag_p=0;
    polInUse_p=0;
    pop_p = new PolOuterProduct;
    ft_p=FFT2D(true);
  }
  
  FTMachine::FTMachine(CountedPtr<CFCache>& cfcache,CountedPtr<ConvolutionFunction>& cf):
    isDryRun(false), image(0), uvwMachine_p(0), 
    tangentSpecified_p(false), fixMovingSource_p(false), 
    ephemTableName_p(""), 
    movingDirShift_p(0.0),
    distance_p(0.0), lastFieldId_p(-1),lastMSId_p(-1), romscol_p(nullptr), 
    useDoubleGrid_p(false), 
    freqFrameValid_p(false), 
    freqInterpMethod_p(InterpolateArray1D<Double,Complex>::nearestNeighbour), 
    pointingDirCol_p("DIRECTION"),
    cfStokes_p(), cfCache_p(cfcache), cfs_p(), cfwts_p(), cfs2_p(), cfwts2_p(),
    convFuncCtor_p(cf),canComputeResiduals_p(false), toVis_p(true), numthreads_p(-1), 
    pbLimit_p(0.05),sj_p(0), cmplxImage_p( ), vbutil_p(), phaseCenterTime_p(-1.0), doneThreadPartition_p(-1), briggsWeightor_p(nullptr), tempFileNames_p(0)
  {
    spectralCoord_p=SpectralCoordinate();
    isPseudoI_p=false;
    spwChanSelFlag_p=0;
    polInUse_p=0;
    pop_p = new PolOuterProduct;
    ft_p=FFT2D(true);
  }
  
  LogIO& FTMachine::logIO() {return logIO_p;};
  
  //---------------------------------------------------------------------- 
  FTMachine& FTMachine::operator=(const FTMachine& other)
  {
    if(this!=&other) {
      image=other.image;
      //generic selection stuff and state
      nAntenna_p=other.nAntenna_p;
      distance_p=other.distance_p;
      lastFieldId_p=other.lastFieldId_p;
      lastMSId_p=other.lastMSId_p;
      romscol_p=other.romscol_p;
      tangentSpecified_p=other.tangentSpecified_p;
      mTangent_p=other.mTangent_p;
      mImage_p=other.mImage_p;
      mFrame_p=other.mFrame_p;
      
      nx=other.nx;
      ny=other.ny;
      npol=other.npol;
      nchan=other.nchan;
      nvischan=other.nvischan;
      nvispol=other.nvispol;
      mLocation_p=other.mLocation_p;
      if(uvwMachine_p)
    	  delete uvwMachine_p;
      if(other.uvwMachine_p)
	uvwMachine_p=new casacore::UVWMachine(*other.uvwMachine_p);
      else
	uvwMachine_p=0;
      doUVWRotation_p=other.doUVWRotation_p;
      //Spectral and pol stuff 
      freqInterpMethod_p=other.freqInterpMethod_p;
      spwChanSelFlag_p.resize();
      spwChanSelFlag_p=other.spwChanSelFlag_p;
      freqFrameValid_p=other.freqFrameValid_p;
      //selectedSpw_p.resize();
      //selectedSpw_p=other.selectedSpw_p;
      imageFreq_p.resize();
      imageFreq_p=other.imageFreq_p;
      lsrFreq_p.resize();
      lsrFreq_p=other.lsrFreq_p;
      interpVisFreq_p.resize();
      interpVisFreq_p=other.interpVisFreq_p;
      //multiChanMap_p=other.multiChanMap_p;
      chanMap.resize();
      chanMap=other.chanMap;
      polMap.resize();
      polMap=other.polMap;
      nVisChan_p.resize();
      nVisChan_p=other.nVisChan_p;
      spectralCoord_p=other.spectralCoord_p;
      visPolMap_p.resize();
      visPolMap_p=other.visPolMap_p;
      //doConversion_p.resize();
      //doConversion_p=other.doConversion_p;
      pointingDirCol_p=other.pointingDirCol_p;
      //moving source stuff
      movingDir_p=other.movingDir_p;
      fixMovingSource_p=other.fixMovingSource_p;
      ephemTableName_p = other.ephemTableName_p;
      firstMovingDir_p=other.firstMovingDir_p;
      movingDirShift_p=other.movingDirShift_p;
      //Double precision gridding for those FTMachines that can do
      useDoubleGrid_p=other.useDoubleGrid_p;
      cfStokes_p = other.cfStokes_p;
      polInUse_p = other.polInUse_p;
      cfs_p = other.cfs_p;
      cfwts_p = other.cfwts_p;
      cfs2_p = other.cfs2_p;
      cfwts2_p = other.cfwts2_p;
      canComputeResiduals_p = other.canComputeResiduals_p;

      pop_p = other.pop_p;
      toVis_p = other.toVis_p;
      spwFreqSel_p.resize();
      spwFreqSel_p = other.spwFreqSel_p;
      expandedSpwFreqSel_p = other.expandedSpwFreqSel_p;
      expandedSpwConjFreqSel_p = other.expandedSpwConjFreqSel_p;
      cmplxImage_p=other.cmplxImage_p;
      vbutil_p=other.vbutil_p;
      numthreads_p=other.numthreads_p;
      pbLimit_p=other.pbLimit_p;
      convFuncCtor_p = other.convFuncCtor_p;      
      sj_p.resize();
      sj_p=other.sj_p;
      isDryRun=other.isDryRun;
      phaseCenterTime_p=other.phaseCenterTime_p;
      doneThreadPartition_p=other.doneThreadPartition_p;
      xsect_p=other.xsect_p;
      ysect_p=other.ysect_p;
      nxsect_p=other.nxsect_p;
      nysect_p=other.nysect_p;
      obsvelconv_p=other.obsvelconv_p;
      mtype_p=other.mtype_p;
      briggsWeightor_p=other.briggsWeightor_p;
      ft_p=other.ft_p;
    };
    return *this;
  };
  
  FTMachine* FTMachine::cloneFTM(){
    Record rec;
    String err;
    if(!(this->toRecord(err, rec)))
      throw(AipsError("Error in cloning FTMachine"));
    FTMachine* retval=VisModelData::NEW_FT(rec);
    if(retval)
      retval->briggsWeightor_p=briggsWeightor_p;
    return retval;
  }

  //----------------------------------------------------------------------
  Bool FTMachine::changed(const vi::VisBuffer2&) {
      return false;
    }
  //----------------------------------------------------------------------
  FTMachine::FTMachine(const FTMachine& other)
  {
    operator=(other);
  }
  
  Bool FTMachine::doublePrecGrid(){
    return useDoubleGrid_p;
  }

  void FTMachine::reset(){
    //ft_p=FFT2D(true);
  }
  
  //----------------------------------------------------------------------
   void FTMachine::initPolInfo(const vi::VisBuffer2& vb)
   {
     //
     // Need to figure out where to compute the following arrays/ints
     // in the re-factored code.
     // ----------------------------------------------------------------
     {
       polInUse_p = 0;
       uInt N=0;
       for(uInt i=0;i<polMap.nelements();i++) if (polMap(i) > -1) polInUse_p++;
       cfStokes_p.resize(polInUse_p);
       for(uInt i=0;i<polMap.nelements();i++)
 	if (polMap(i) > -1) {cfStokes_p(N) = vb.correlationTypes()(i);N++;}
     }
   }
  //----------------------------------------------------------------------
    void FTMachine::initMaps(const vi::VisBuffer2& vb) {
      logIO() << LogOrigin("FTMachine", "initMaps") << LogIO::NORMAL;

      AlwaysAssert(image, AipsError);
      
      // Set the frame for the UVWMachine
      if(vb.isAttached()){
	//mFrame_p=MeasFrame(MEpoch(Quantity(vb.time()(0), "s"), MSColumns(vb.ms()).timeMeas()(0).getRef()), mLocation_p);
	if(vbutil_p.null())
	  vbutil_p=new VisBufferUtil(vb);	
	romscol_p=new MSColumns(vb.ms());
	Unit epochUnit=(romscol_p->time()).keywordSet().asArrayString("QuantumUnits")(IPosition(1,0));
	if(!mFrame_p.epoch()) 
	  mFrame_p.set(MEpoch(Quantity(vb.time()(0), epochUnit),  (romscol_p->timeMeas())(0).getRef()));
	else
	  mFrame_p.resetEpoch(MEpoch(Quantity(vb.time()(0), epochUnit), (romscol_p->timeMeas())(0).getRef()));
	if(!mFrame_p.position())
	  mFrame_p.set(mLocation_p);
	else
	  mFrame_p.resetPosition(mLocation_p);
	if(!mFrame_p.direction())
	  mFrame_p.set(vbutil_p->getEphemDir(vb, phaseCenterTime_p));
	else
	  mFrame_p.resetDirection(vbutil_p->getEphemDir(vb, phaseCenterTime_p));
      }
      else{
	throw(AipsError("Cannot define some frame as no Visiter/MS is attached"));
      }
      //////TESTOOOO
      ///setMovingSource("COMET", "des_deedee_ephem2.tab");
      ///////////////////////////////////////////
      // First get the CoordinateSystem for the image and then find
      // the DirectionCoordinate
      casacore::CoordinateSystem coords=image->coordinates();
      Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
      AlwaysAssert(directionIndex>=0, AipsError);
      DirectionCoordinate
        directionCoord=coords.directionCoordinate(directionIndex);
      Int spectralIndex=coords.findCoordinate(Coordinate::SPECTRAL);
      AlwaysAssert(spectralIndex>-1, AipsError);
      spectralCoord_p=coords.spectralCoordinate(spectralIndex);
      
      // get the first position of moving source
      if(fixMovingSource_p){
	//cerr << "obsinfo time " << coords.obsInfo().obsDate() << "    epoch used in frame " <<  MEpoch((mFrame_p.epoch())) << endl;
        ///Darn vb.time()(0) may not be the earliest time due to sort issues...
        //so lets try to use the same
        ///time as SynthesisIUtilMethods::buildCoordinateSystemCore is using
        //mFrame_p.resetEpoch(romscol_p->timeMeas()(0));
	mFrame_p.resetEpoch(coords.obsInfo().obsDate());
	//Double firstTime=romscol_p->time()(0);
									  
	Double firstTime=coords.obsInfo().obsDate().get("s").getValue();
        //First convert to HA-DEC or AZEL for parallax correction
        MDirection::Ref outref1(MDirection::AZEL, mFrame_p);
        MDirection tmphadec;
	if (upcase(movingDir_p.getRefString()).contains("APP")) {
          //cerr << "phaseCenterTime_p " << phaseCenterTime_p << endl;
	  tmphadec = MDirection::Convert((vbutil_p->getEphemDir(vb, phaseCenterTime_p > 0.0 ? phaseCenterTime_p : firstTime)), outref1)();
	  MeasComet mcomet(Path((romscol_p->field()).ephemPath(vb.fieldId()(0))).absoluteName());
	  if (mFrame_p.comet())
	    mFrame_p.resetComet(mcomet);
	  else
	    mFrame_p.set(mcomet);
	} else if (upcase(movingDir_p.getRefString()).contains("COMET")) {
	  MeasComet mcomet(Path(ephemTableName_p).absoluteName());
	  if (mFrame_p.comet())
	    mFrame_p.resetComet(mcomet);
	  else
	    mFrame_p.set(mcomet);
	  tmphadec = MDirection::Convert(MDirection(MDirection::COMET), outref1)();
	} else {
	  tmphadec = MDirection::Convert(movingDir_p, outref1)();
	}
        MDirection::Ref outref(directionCoord.directionType(), mFrame_p);
        firstMovingDir_p=MDirection::Convert(tmphadec, outref)();
        ////////////////////
        /*ostringstream ss;
        Unit epochUnit=(romscol_p->time()).keywordSet().asArrayString("QuantumUnits")(IPosition(1,0));
        MEpoch(Quantity(vb.time()(0), epochUnit), (romscol_p->timeMeas())(0).getRef()).print(ss);
        cerr << std::setprecision(15) << "First time " << ss.str() << "field id " << vb.fieldId()(0) << endl;
        ss.clear();
        firstMovingDir_p.print(ss);
        cerr << "firstdir " << ss.str() << "   " << firstMovingDir_p.toString() << endl;
        */
        //////////////
        
	if(spectralCoord_p.frequencySystem(False)==MFrequency::REST){
	  ///We want the data frequency to be shifted to the SOURCE frame
	  ///which is labelled REST as we have never defined the SOURCE frame didn't we
	  initSourceFreqConv();
	}
	///TESTOO 
	///waiting for CAS-11060
	//firstMovingDir_p=MDirection::Convert(vbutil_p->getPhaseCenter(vb, phaseCenterTime_p), outref)();
	////////////////////
      }


      // Now we need MDirection of the image phase center. This is
      // what we define it to be. So we define it to be the
      // center pixel. So we have to do the conversion here.
      // This is independent of padding since we just want to know
      // what the world coordinates are for the phase center
      // pixel
      {
        Vector<Double> pixelPhaseCenter(2);
        pixelPhaseCenter(0) = Double( image->shape()(0) / 2 );
        pixelPhaseCenter(1) = Double( image->shape()(1) / 2 );
        directionCoord.toWorld(mImage_p, pixelPhaseCenter);
      }

      // Decide if uvwrotation is not necessary, if phasecenter and
      // image center are with in one pixel distance; Save some
      //  computation time especially for spectral cubes.
      {
        Vector<Double> equal= (mImage_p.getAngle()-
			       vbutil_p->getPhaseCenter(vb, phaseCenterTime_p).getAngle()).getValue();
        if((abs(equal(0)) < abs(directionCoord.increment()(0)))
  	 && (abs(equal(1)) < abs(directionCoord.increment()(1)))){
  	doUVWRotation_p=false;
        }
        else{
  	doUVWRotation_p=true;
        }
      }
      // Get the object distance in meters
      Record info(image->miscInfo());
      if(info.isDefined("distance")) {
        info.get("distance", distance_p);
        if(abs(distance_p)>0.0) {
  	logIO() << "Distance to object is set to " << distance_p/1000.0
  		<< "km: applying focus correction" << LogIO::POST;
        }
      }

      // Set up the UVWMachine.
      if(uvwMachine_p) delete uvwMachine_p; uvwMachine_p=0;
      String observatory;
      if(vb.isAttached())
	observatory=(vb.subtableColumns().observation()).telescopeName()(0);
      else
	throw(AipsError("Cannot define frame because of no access to OBSERVATION table")); 
      if(observatory.contains("ATCA") || observatory.contains("DRAO")
         || observatory.contains("WSRT")){
        uvwMachine_p=new casacore::UVWMachine(mImage_p, vbutil_p->getPhaseCenter(vb, phaseCenterTime_p), mFrame_p,
  				  true, false);
      }
      else{
        uvwMachine_p=new casacore::UVWMachine(mImage_p, vbutil_p->getPhaseCenter(vb, phaseCenterTime_p), mFrame_p,
  				  false, tangentSpecified_p);
      }
      AlwaysAssert(uvwMachine_p, AipsError);
      
      lastFieldId_p=-1;
      
      lastMSId_p=vb.msId();
      phaseShifter_p=new UVWMachine(*uvwMachine_p);
      // Set up maps
      

     
      //Store the image/grid channels freq values
      {
        Int chanNumbre=image->shape()(3);
        Vector<Double> pixindex(chanNumbre);
        imageFreq_p.resize(chanNumbre);
        Vector<Double> tempStorFreq(chanNumbre);
        indgen(pixindex);
        //    pixindex=pixindex+1.0;
        for (Int ll=0; ll< chanNumbre; ++ll){
  	if( !spectralCoord_p.toWorld(tempStorFreq(ll), pixindex(ll))){
  	  logIO() << "Cannot get imageFreq " << LogIO::EXCEPTION;

  	}
        }
        convertArray(imageFreq_p,tempStorFreq);
      }
      //Destroy any conversion layer Freq coord if freqframe is not valid
      if(!freqFrameValid_p){
        MFrequency::Types imageFreqType=spectralCoord_p.frequencySystem();
        spectralCoord_p.setFrequencySystem(imageFreqType);
        spectralCoord_p.setReferenceConversion(imageFreqType,
  					     MEpoch(Quantity(vb.time()(0), "s")),
  					     mLocation_p,
  					     mImage_p);
      }

      // Channel map: do this properly by looking up the frequencies
      // If a visibility channel does not map onto an image
      // pixel then we set the corresponding chanMap to -1.
      // This means that put and get must always check for this
      // value (see e.g. GridFT)

      nvischan  = vb.getFrequencies(0).nelements();
      interpVisFreq_p.resize();
      interpVisFreq_p=vb.getFrequencies(0);
      
      // Polarization map
      visPolMap_p.resize();
      polMap.resize();
      
      //As matchChannel calls matchPol ...it has to be called after making sure
      //polMap and visPolMap are zero size to force a polMap matching
      chanMap.resize();
      matchChannel(vb);
      //chanMap=multiChanMap_p[vb.spectralWindows()(0)];
      if(chanMap.nelements() == 0)
        chanMap=Vector<Int>(vb.getFrequencies(0).nelements(), -1);

      {
        //logIO() << LogIO::DEBUGGING << "Channel Map: " << chanMap << LogIO::POST;
      }
      // Should never get here
      if(max(chanMap)>=nchan||min(chanMap)<-2) {
        logIO() << "Illegal Channel Map: " << chanMap << LogIO::EXCEPTION;
      }


      
      initPolInfo(vb);
      Vector<Int> intpolmap(visPolMap_p.nelements());
      for (uInt kk=0; kk < intpolmap.nelements(); ++kk){
	intpolmap[kk]=Int(visPolMap_p[kk]);
      }
      pop_p->initCFMaps(intpolmap, polMap);

      //cerr << "initmaps polmap "<< polMap << endl;


      

    }
  void FTMachine::initBriggsWeightor(vi::VisibilityIterator2& vi){
    ///Lastly initialized Briggs cube weighting scheme
    if(!briggsWeightor_p.null()){
      String error;
      Record rec;
      AlwaysAssert(image, AipsError);
      if(!toRecord(error, rec))
        throw (AipsError("Could not initialize BriggsWeightor")); 
      String wgtcolname=briggsWeightor_p->initImgWeightCol(vi, *image, rec);
      tempFileNames_p.resize(tempFileNames_p.nelements()+1, True);
      tempFileNames_p[tempFileNames_p.nelements()-1]=wgtcolname;
      
    }
  }

  FTMachine::~FTMachine() 
  {
    if(uvwMachine_p) delete uvwMachine_p; uvwMachine_p=0;
  }
  

  void FTMachine::initSourceFreqConv(){
    MRadialVelocity::Types refvel=MRadialVelocity::GEO;
    if(mFrame_p.comet()){
      //Has a ephem table 
      if(((mFrame_p.comet())->getTopo().getLength("km").getValue()) > 1.0e-3){
	refvel=MRadialVelocity::TOPO;
      }
     
      
    }
    else{
      //using a canned DE-200 or 405 source
      MDirection::Types planetType=MDirection::castType(movingDir_p.getRef().getType());
    mtype_p=MeasTable::BARYEARTH;
    if(planetType >=MDirection::MERCURY && planetType <MDirection::COMET){
      //Damn these enums are not in the same order
      switch(planetType){
      case MDirection::MERCURY :
	mtype_p=MeasTable::MERCURY;
	break;
      case MDirection::VENUS :
	mtype_p=MeasTable::VENUS;
	break;	
      case MDirection::MARS :
	mtype_p=MeasTable::MARS;
	break;
      case MDirection::JUPITER :
	mtype_p=MeasTable::JUPITER;
	break;
      case MDirection::SATURN :
	mtype_p=MeasTable::SATURN;
	break;
      case MDirection::URANUS :
	mtype_p=MeasTable::URANUS;
	break;
      case MDirection::NEPTUNE :
	mtype_p=MeasTable::NEPTUNE;
	break;
      case MDirection::PLUTO :
	mtype_p=MeasTable::PLUTO;
	break;
      case MDirection::MOON :
	mtype_p=MeasTable::MOON;
	break;
      case MDirection::SUN :
	mtype_p=MeasTable::SUN;
	break;
      default:
	throw(AipsError("Cannot translate to known major solar system object"));
      }

    }
      
    }
     obsvelconv_p=MRadialVelocity::Convert (MRadialVelocity(MVRadialVelocity(0.0),
							 MRadialVelocity::Ref(MRadialVelocity::TOPO, mFrame_p)),
							 MRadialVelocity::Ref(refvel));

  }

  
  Long FTMachine::estimateRAM(const CountedPtr<SIImageStore>& imstor){
    //not set up yet 
    if(!image && !imstor)
      return -1;
    Long npixels=0;
    if(image)
      npixels=((image->shape()).product())/1024;
    else{
      if((imstor->getShape()).product() !=0)
        npixels=(imstor->getShape()).product()/1024;
    }
    if(npixels==0) npixels=1; //1 kPixels is minimum then
    Long factor=sizeof(Complex);
    if(!toVis_p && useDoubleGrid_p)
      factor=sizeof(DComplex);
    return (npixels*factor);
  }
  
  void FTMachine::shiftFreqToSource(Vector<Double>& freqs){
    MDoppler dopshift;
    MEpoch ep(mFrame_p.epoch());
    if(mFrame_p.comet()){
      ////Will use UT for now for ephem tables as it is not clear that they are being
      ///filled with TDB as intended in MeasComet.h
      MEpoch::Convert toUT(ep, MEpoch::UT);
      MVRadialVelocity cometvel;
      (*mFrame_p.comet()).getRadVel(cometvel, toUT(ep).get("d").getValue());
      //cerr << std::setprecision(10) << "UT " << toUT(ep).get("d").getValue() << " cometvel " << cometvel.get("km/s").getValue("km/s") << endl;
      
      //cerr  << "pos " << MPosition(mFrame_p.position()) << " obsevatory vel " << obsvelconv_p().get("km/s").getValue("km/s") << endl;
      dopshift=MDoppler(Quantity(-cometvel.get("km/s").getValue("km/s")+obsvelconv_p().get("km/s").getValue("km/s") , "km/s"), MDoppler::RELATIVISTIC);
      
    }
    else{
       Vector<Double> planetparam;
       Vector<Double> earthparam;
       MEpoch::Convert toTDB(ep, MEpoch::TDB);
       earthparam=MeasTable::Planetary(MeasTable::EARTH, toTDB(ep).get("d").getValue());
       planetparam=MeasTable::Planetary(mtype_p, toTDB(ep).get("d").getValue());
       //GEOcentric param
       planetparam=planetparam-earthparam;
       Vector<Double> unitdirvec(3);
       Double dist=sqrt(planetparam(0)*planetparam(0)+planetparam(1)*planetparam(1)+planetparam(2)*planetparam(2));
       unitdirvec(0)=planetparam(0)/dist;
       unitdirvec(1)=planetparam(1)/dist;
       unitdirvec(2)=planetparam(2)/dist;
       Quantity planetradvel(planetparam(3)*unitdirvec(0)+planetparam(4)*unitdirvec(1)+planetparam(5)*unitdirvec(2), "AU/d");
	dopshift=MDoppler(Quantity(-planetradvel.getValue("km/s")+obsvelconv_p().get("km/s").getValue("km/s") , "km/s"), MDoppler::RELATIVISTIC);
       
    }

    Vector<Double> newfreqs=dopshift.shiftFrequency(freqs);
    freqs=newfreqs;
  }
  
  Bool FTMachine::interpolateFrequencyTogrid(const vi::VisBuffer2& vb,
  					     const Matrix<Float>& wt,
  					     Cube<Complex>& data,
  					     Cube<Int>& flags,
  					     Matrix<Float>& weight,
  					     FTMachine::Type type){
      Cube<Complex> origdata;
      Cube<Bool> modflagCube;
      // Read flags from the vb.
      setSpectralFlag(vb,modflagCube);
      Vector<Double> visFreq(vb.getFrequencies(0).nelements());
      //if(doConversion_p[vb.spectralWindows()[0]]){
      if(freqFrameValid_p){
        visFreq.resize(lsrFreq_p.shape());
        convertArray(visFreq, lsrFreq_p);
      }
      else{
        convertArray(visFreq, vb.getFrequencies(0));
        lsrFreq_p.resize();
        lsrFreq_p=vb.getFrequencies(0);
      }
      if(type==FTMachine::MODEL){
        origdata.reference(vb.visCubeModel());
      }
      else if(type==FTMachine::CORRECTED){
        origdata.reference(vb.visCubeCorrected());
      }
      else if(type==FTMachine::OBSERVED){
        origdata.reference(vb.visCube());
      }
      else if(type==FTMachine::PSF){
        // make sure its a size 0 data ...psf
        //so avoid reading any data from disk
        origdata.resize();

      }
      else{
        throw(AipsError("Don't know which column is being regridded"));
      }
      if((imageFreq_p.nelements()==1) || (freqInterpMethod_p== InterpolateArray1D<Double, Complex>::nearestNeighbour) || (vb.nChannels()==1)){
        data.reference(origdata);
        // do something here for apply flag based on spw chan sels
        // e.g.
        
        
        flags.resize(modflagCube.shape());
        flags=0;
        //flags(vb.flagCube())=true;
	
	flags(modflagCube)=true;
	
        weight.reference(wt);
        interpVisFreq_p.resize();
        interpVisFreq_p=lsrFreq_p;

        return false;
      }

      Cube<Bool>flag;

      //okay at this stage we have at least 2 channels
      Double width=fabs(imageFreq_p[1]-imageFreq_p[0])/fabs(visFreq[1]-visFreq[0]);
      //if width is smaller than number of points needed for interpolation ...do it directly
      //
      // If image chan width is more than twice the data chan width, make a new list of
      // data frequencies on which to interpolate. This new list is sync'd with the starting image chan
      // and have the same width as the data chans.
      /*if(((width >2.0) && (freqInterpMethod_p==InterpolateArray1D<Double, Complex>::linear)) ||
         ((width >4.0) && (freqInterpMethod_p !=InterpolateArray1D<Double, Complex>::linear))){
      */
      if(width > 1.0){
        Double minVF=min(visFreq);
        Double maxVF=max(visFreq);
        Double minIF=min(imageFreq_p);
        Double maxIF=max(imageFreq_p);
        if( ((minIF-fabs(imageFreq_p[1]-imageFreq_p[0])/2.0) > maxVF) ||
  	  ((maxIF+fabs(imageFreq_p[1]-imageFreq_p[0])/2.0) < minVF)){
  	//This function should not have been called with image
  	//being out of bound of data...but still
  	interpVisFreq_p.resize(imageFreq_p.nelements());
  	interpVisFreq_p=imageFreq_p;
  	chanMap.resize(interpVisFreq_p.nelements());
  	chanMap.set(-1);
        }
        else{ // Make a new list of frequencies.
  	Bool found;
  	uInt where=0;
  	//Double interpwidth=visFreq[1]-visFreq[0];
        Double interpwidth=copysign(fabs(imageFreq_p[1]-imageFreq_p[0])/floor(width), visFreq[1]-visFreq[0]);
        //if(name() != "GridFT")
        //  cerr << "width " << width << " interpwidth " << interpwidth << endl;
  	if(minIF < minVF){ // Need to find the first image-channel with data in it
  	  where=binarySearchBrackets(found, imageFreq_p, minVF, imageFreq_p.nelements());
  	  if(where != imageFreq_p.nelements()){
  	    minIF=imageFreq_p[where];
  	  }
  	}

  	if(maxIF > maxVF){
  	   where=binarySearchBrackets(found, imageFreq_p, maxVF, imageFreq_p.nelements());
  	   if(where!= imageFreq_p.nelements()){
  	    maxIF=imageFreq_p[where];
  	   }

  	}

          // This new list of frequencies starts at the first image channel minus half image channel.
  	// It ends at the last image channel plus half image channel.
        Int ninterpchan=(Int)ceil((maxIF-minIF+fabs(imageFreq_p[1]-imageFreq_p[0]))/fabs(interpwidth))+2;
  	chanMap.resize(ninterpchan);
  	chanMap.set(-1);
  	interpVisFreq_p.resize(ninterpchan);
  	interpVisFreq_p[0]=(interpwidth > 0) ? minIF : maxIF;
        if(freqInterpMethod_p==InterpolateArray1D<Double, Complex>::linear)
          interpVisFreq_p[0]-=interpwidth;
        if(freqInterpMethod_p==InterpolateArray1D<Double, Complex>::cubic)
          interpVisFreq_p[0]-=2.0*interpwidth;
        Double startedge=abs(imageFreq_p[1]-imageFreq_p[0])/2.0 -abs(interpwidth)/2.0;
  	interpVisFreq_p[0] =(interpwidth >0) ? (interpVisFreq_p[0]-startedge):(interpVisFreq_p[0]+startedge);

  	for (Int k=1; k < ninterpchan; ++k){
  	  interpVisFreq_p[k] = interpVisFreq_p[k-1]+ interpwidth;
  	}
        Double halfdiff=fabs((imageFreq_p[1]-imageFreq_p[0])/2.0);
  	for (Int k=0; k < ninterpchan; ++k){
  	  ///chanmap with width
          //  	  Double nearestchanval = interpVisFreq_p[k]- (imageFreq_p[1]-imageFreq_p[0])/2.0;
 	  //where=binarySearchBrackets(found, imageFreq_p, nearestchanval, imageFreq_p.nelements());
          Int which=-1;
          for (Int j=0; j< Int(imageFreq_p.nelements()); ++j){
            //cerr <<  (imageFreq_p[j]-halfdiff)  << "   "   << (imageFreq_p[j]+halfdiff) << " val " << interpVisFreq_p[k] << endl;
            if( (interpVisFreq_p[k] >= (imageFreq_p[j]-halfdiff)) && (interpVisFreq_p[k] <  (imageFreq_p[j]+halfdiff)))
              which=j;
          }
  	  if((which > -1) && (which < Int(imageFreq_p.nelements()))){
  	    chanMap[k]=which;
          }
          else{
            //if(name() != "GridFT")
            //  cerr << "MISSED it " << interpVisFreq_p[k] << endl;
          }
  	
       
  	}
        //        if(name() != "GridFT")
        //  cerr << std::setprecision(10) << "chanMap " << chanMap <<  endl; //" interpvisfreq " <<  interpVisFreq_p << " orig " << visFreq << endl;

        }// By now, we have a new list of frequencies, synchronized with image channels, but with data chan widths.
      }// end of ' if (we have to make new frequencies) '
      else{
        // Interpolate directly onto output image frequencies.
        interpVisFreq_p.resize(imageFreq_p.nelements());
        convertArray(interpVisFreq_p, imageFreq_p);
        chanMap.resize(interpVisFreq_p.nelements());
        indgen(chanMap);
      }
      if(type != FTMachine::PSF){ // Interpolating the data
   	//Need to get  new interpolate functions that interpolate explicitly on the 2nd axis
  	//2 swap of axes needed
  	Cube<Complex> flipdata;
  	Cube<Bool> flipflag;

          // Interpolate the data.
          //      Input flags are from the previous step ( setSpectralFlag ).
          //      Output flags contain info about channels that could not be interpolated
          //                                   (for example, linear interp with only one data point)
  	swapyz(flipflag,modflagCube);
  	swapyz(flipdata,origdata);
  	InterpolateArray1D<Double,Complex>::
  	  interpolate(data,flag,interpVisFreq_p,visFreq,flipdata,flipflag,freqInterpMethod_p, False, False);
  	flipdata.resize();
  	swapyz(flipdata,data);
  	data.resize();
  	data.reference(flipdata);
  	flipflag.resize();
  	swapyz(flipflag,flag);
  	flag.resize();
  	flag.reference(flipflag);
        // Note : 'flag' will get augmented with the flags coming out of weight interpolation
      }
      else
        { // get the flag array to the correct shape.
          // This will get filled at the end of weight-interpolation.
          flag.resize(vb.nCorrelations(), interpVisFreq_p.nelements(), vb.nRows());
          flag.set(false);
        }
        // Now, interpolate the weights also.
        //   (1) Read in the flags from the vb ( setSpectralFlags -> modflagCube )
        //   (2) Collapse the flags along the polarization dimension to match shape of weight.
        //If BriggsWeightor is used weight is already interpolated so we can bypass this
         InterpolateArray1D<casacore::Double,casacore::Complex>::InterpolationMethod weightinterp=freqInterpMethod_p;
  
  if(!briggsWeightor_p.null()){
    weightinterp= InterpolateArray1D<casacore::Double,casacore::Complex>::nearestNeighbour;
  }
      //InterpolateArray1D<casacore::Double,casacore::Complex>::InterpolationMethod weightinterp=InterpolateArray1D<casacore::Double,casacore::Complex>::nearestNeighbour; 
         Matrix<Bool> chanflag(wt.shape());
         AlwaysAssert( chanflag.shape()[0]==modflagCube.shape()[1], AipsError);
         AlwaysAssert( chanflag.shape()[1]==modflagCube.shape()[2], AipsError);
         chanflag=false;
         for(uInt pol=0;pol<modflagCube.shape()[0];pol++)
  	 chanflag = chanflag | modflagCube.yzPlane(pol);

         // (3) Interpolate the weights.
         //      Input flags are the collapsed vb flags : 'chanflag'
         //      Output flags are in tempoutputflag
         //            - contains info about channels that couldn't be interpolated.
         Matrix<Float> flipweight;
         flipweight=transpose(wt);
         Matrix<Bool> flipchanflag;
         flipchanflag=transpose(chanflag);
         Matrix<Bool> tempoutputflag;
         InterpolateArray1D<Double,Float>::
  	 interpolate(weight,tempoutputflag, interpVisFreq_p, visFreq,flipweight,flipchanflag,weightinterp, False, False);
         flipweight.resize();
         flipweight=transpose(weight);
         weight.resize();
         weight.reference(flipweight);
         flipchanflag.resize();
         flipchanflag=transpose(tempoutputflag);
         tempoutputflag.resize();
         tempoutputflag.reference(flipchanflag);

         // (4) Now, fill these flags back into the flag cube
         //                 so that they get USED while gridding the PSF (and data)
         //      Taking the OR of the flags that came out of data-interpolation
         //                         and weight-interpolation, in case they're different.
         //      Expanding flags across polarization.  This will destroy any
         //                          pol-dependent flags for imaging, but msvis::VisImagingWeight
         //                          uses the OR of flags across polarization anyway
         //                          so we don't lose anything.

         AlwaysAssert( tempoutputflag.shape()[0]==flag.shape()[1], AipsError);
         AlwaysAssert( tempoutputflag.shape()[1]==flag.shape()[2], AipsError);
         for(uInt pol=0;pol<flag.shape()[0];pol++)
  	 flag.yzPlane(pol) = tempoutputflag | flag.yzPlane(pol);

         // Fill the output array of image-channel flags.
         flags.resize(flag.shape());
         flags=0;
         flags(flag)=true;

      return true;
    }

  void FTMachine::getInterpolateArrays(const vi::VisBuffer2& vb,
  				       Cube<Complex>& data, Cube<Int>& flags){

	Vector<Double> visFreq(vb.getFrequencies(0).nelements());

      //if(doConversion_p[vb.spectralWindows()[0]]){
      if(freqFrameValid_p){
        convertArray(visFreq, lsrFreq_p);
      }
      else{
        convertArray(visFreq, vb.getFrequencies(0));
      }
    
	
	  
      if((imageFreq_p.nelements()==1) || (freqInterpMethod_p== InterpolateArray1D<Double, Complex>::nearestNeighbour)||  (vb.nChannels()==1) ){
        Cube<Bool> modflagCube;
        setSpectralFlag(vb,modflagCube);
	
		data.reference(vb.visCubeModel());
        //flags.resize(vb.flagCube().shape());
        flags.resize(modflagCube.shape());
        flags=0;
        //flags(vb.flagCube())=true;
        flags(modflagCube)=true;
        interpVisFreq_p.resize();
        interpVisFreq_p=visFreq;
        return;
      }

      data.resize(vb.nCorrelations(), imageFreq_p.nelements(), vb.nRows());
      flags.resize(vb.nCorrelations(), imageFreq_p.nelements(), vb.nRows());
      data.set(Complex(0.0,0.0));
      flags.set(0);
      //no need to degrid channels that does map over this vb
      Int maxchan=max(chanMap);
      for (uInt k =0 ; k < chanMap.nelements() ; ++k){
        if(chanMap(k)==-1)
  	chanMap(k)=maxchan;
      }
      Int minchan=min(chanMap);
      if(minchan==maxchan)
        minchan=-1;


      for(Int k = 0; k < minchan; ++k)
        flags.xzPlane(k).set(1);

      for(uInt k = maxchan + 1; k < imageFreq_p.nelements(); ++k)
        flags.xzPlane(k).set(1);

      interpVisFreq_p.resize(imageFreq_p.nelements());
      convertArray(interpVisFreq_p, imageFreq_p);
      chanMap.resize(imageFreq_p.nelements());
      indgen(chanMap);
    }

  Bool FTMachine::interpolateFrequencyFromgrid(vi::VisBuffer2& vb,
  					       Cube<Complex>& data,
  					       FTMachine::Type type){

      Cube<Complex> *origdata;
      Vector<Double> visFreq(vb.getFrequencies(0).nelements());

      //if(doConversion_p[vb.spectralWindows()[0]]){
      if(freqFrameValid_p){
        convertArray(visFreq, lsrFreq_p);
      }
      else{
        convertArray(visFreq, vb.getFrequencies(0));
      }

      if(type==FTMachine::MODEL){
        origdata=const_cast <Cube<Complex>* > (&(vb.visCubeModel()));
      }
      else if(type==FTMachine::CORRECTED){
        origdata=const_cast<Cube<Complex>* >(&(vb.visCubeCorrected()));
      }
      else{
        origdata=const_cast<Cube<Complex>* >(&(vb.visCube()));
      }
    //
    // If visibility data (vb) has only one channel, or the image cube
    // has only one channel, resort to nearestNeighbour interpolation.
    // Honour user selection of nearestNeighbour.
    //
    
	Double width=fabs(imageFreq_p[1]-imageFreq_p[0])/fabs(visFreq[1]-visFreq[0]);
	
    if((imageFreq_p.nelements()==1) || 
       (vb.nChannels()==1) || 
       (freqInterpMethod_p== InterpolateArray1D<Double, Complex>::nearestNeighbour) ){
        origdata->reference(data);
        return false;
      }
  
      //Need to get  new interpolate functions that interpolate explicitly on the 2nd axis
      //2 swap of axes needed
		Cube<Complex> flipgrid;
		flipgrid.resize();
		swapyz(flipgrid,data);
		Vector<Double> newImFreq;
		newImFreq=imageFreq_p;
		
		//cerr << "width " << width << endl;
                /* if(((width >2.0) && (freqInterpMethod_p==InterpolateArray1D<Double, Complex>::linear)) ||
                   ((width >4.0) && (freqInterpMethod_p !=InterpolateArray1D<Double, Complex>::linear))){*/
                if(width > 1.0){
			Int newNchan=Int(std::floor(width))*imageFreq_p.nelements();
			newImFreq.resize(newNchan);
			Double newIncr= (imageFreq_p[1]-imageFreq_p[0])/std::floor(width);
			Double newStart=imageFreq_p[0]-(imageFreq_p[1]-imageFreq_p[0])/2.0+newIncr/2.0;
			Cube<Complex> newflipgrid(flipgrid.shape()[0], flipgrid.shape()[1], newNchan);
                        
			for (Int k=0; k < newNchan; ++k){
				newImFreq[k]=newStart+k*newIncr;
				Int oldchan=k/Int(std::floor(width));
				newflipgrid.xyPlane(k)=flipgrid.xyPlane(oldchan);
				
			}
			//cerr << std::setprecision(12) << "newfreq " << newImFreq << endl;
			//cerr << "oldfreq " << imageFreq_p << endl;
			//InterpolateArray1D<Double,Complex>::
        //interpolate(newflipgrid,newImFreq, imageFreq_p, flipgrid, InterpolateArray1D<Double, Complex>::nearestNeighbour);
			flipgrid.resize();
			flipgrid.reference(newflipgrid);
			 
		 }
      Cube<Complex> flipdata((origdata->shape())(0),(origdata->shape())(2),
  			   (origdata->shape())(1)) ;
      flipdata.set(Complex(0.0));

      ///TESTOO  
      //Cube<Bool> inflag(flipgrid.shape());
      //inflag.set(False);
      //Cube<Bool> outflag(flipdata.shape());
      //InterpolateArray1D<Double,Complex>::
      //  interpolate(flipdata,outflag,visFreq,newImFreq,flipgrid,inflag,freqInterpMethod_p, False, True);

      //cerr << "OUTFLAG " << anyEQ(True, outflag) << " chanmap " << chanMap << endl;
      /////End TESTOO
      InterpolateArray1D<Double,Complex>::
        interpolate(flipdata,visFreq, newImFreq, flipgrid,freqInterpMethod_p);
      

      
      Cube<Bool>  copyOfFlag;
      //Vector<Int> mychanmap=multiChanMap_p[vb.spectralWindows()[0]];
      matchChannel(vb);
      copyOfFlag.assign(vb.flagCube());
      for (uInt k=0; k< chanMap.nelements(); ++ k)
	if(chanMap(k) == -1)
	  copyOfFlag.xzPlane(k).set(true);
      flipgrid.resize();
      swapyz(flipgrid, copyOfFlag, flipdata);
      //swapyz(flipgrid,flipdata);
      vb.setVisCubeModel(flipgrid);

      return true;
    }


  void FTMachine::girarUVW(Matrix<Double>& uvw, Vector<Double>& dphase,
			 const vi::VisBuffer2& vb)
{
    
    
    
    //the uvw rotation is done for common tangent reprojection or if the 
    //image center is different from the phasecenter
    // UVrotation is false only if field never changes
  if(lastMSId_p != vb.msId())
    romscol_p=new MSColumns(vb.ms());
   if((vb.fieldId()(0)!=lastFieldId_p) || (vb.msId()!=lastMSId_p)){
      doUVWRotation_p=true;
   } 
   else{
     //if above failed it still can be changing if   polynome phasecenter or ephem
     
     if( (vb.subtableColumns().field().numPoly()(lastFieldId_p) >0) ||  (! (vb.subtableColumns().field().ephemerisId().isNull()) && (vb.subtableColumns().field().ephemerisId()(lastFieldId_p) > -1)))
       doUVWRotation_p=True;
   }
   if(doUVWRotation_p ||  fixMovingSource_p){
      
      mFrame_p.epoch() != 0 ? 
	mFrame_p.resetEpoch(MEpoch(Quantity(vb.time()(0), "s"))):
	mFrame_p.set(mLocation_p, MEpoch(Quantity(vb.time()(0), "s"), (romscol_p->timeMeas())(0).getRef()));
      MDirection::Types outType;
      MDirection::getType(outType, mImage_p.getRefString());
      MDirection phasecenter=MDirection::Convert(vbutil_p->getPhaseCenter(vb, phaseCenterTime_p), MDirection::Ref(outType, mFrame_p))();
      MDirection inFieldPhaseCenter=phasecenter;

      if(fixMovingSource_p){
       
      //First convert to HA-DEC or AZEL for parallax correction
	MDirection::Ref outref1(MDirection::AZEL, mFrame_p);
	MDirection tmphadec;
	if(upcase(movingDir_p.getRefString()).contains("APP")){
	  tmphadec=MDirection::Convert((vbutil_p->getEphemDir(vb, phaseCenterTime_p)), outref1)();
	}
	else{
	  tmphadec=MDirection::Convert(movingDir_p, outref1)();
	}
	MDirection::Ref outref(mImage_p.getRef().getType(), mFrame_p);
	MDirection sourcenow=MDirection::Convert(tmphadec, outref)();
	//cerr << "Rotating to fixed moving source " << MVDirection(phasecenter.getAngle()-firstMovingDir_p.getAngle()+sourcenow.getAngle()) << endl;
	//phasecenter.set(MVDirection(phasecenter.getAngle()+firstMovingDir_p.getAngle()-sourcenow.getAngle()));
	 movingDirShift_p=MVDirection(sourcenow.getAngle()-firstMovingDir_p.getAngle());
	 // cerr << "shift " << movingDirShift_p.getAngle() << endl;
	inFieldPhaseCenter.shift(movingDirShift_p, False);
    }


      // Set up the UVWMachine only if the field id has changed. If
      // the tangent plane is specified then we need a UVWMachine that
      // will reproject to that plane iso the image plane
      if(doUVWRotation_p || fixMovingSource_p) {
	
	String observatory=(vb.subtableColumns().observation()).telescopeName()(0);
	if(uvwMachine_p) delete uvwMachine_p; uvwMachine_p=0;
	if(observatory.contains("ATCA") || observatory.contains("WSRT")){
		//Tangent specified is being wrongly used...it should be for a
	    	//Use the safest way  for now.
	  uvwMachine_p=new UVWMachine(inFieldPhaseCenter, vbutil_p->getPhaseCenter(vb, phaseCenterTime_p), mFrame_p,
					true, false);
	    phaseShifter_p=new UVWMachine(mImage_p, phasecenter, mFrame_p,
					true, false);
	}
	else{
	  uvwMachine_p=new UVWMachine(inFieldPhaseCenter, vbutil_p->getPhaseCenter(vb, phaseCenterTime_p),  mFrame_p,
				      false, false);
	  phaseShifter_p=new UVWMachine(mImage_p, phasecenter,  mFrame_p,
				      false, false);
	}
      }

      lastFieldId_p=vb.fieldId()(0);
	lastMSId_p=vb.msId();

      
      AlwaysAssert(uvwMachine_p, AipsError);
      
      // Always force a recalculation 
      uvwMachine_p->reCalculate();
      phaseShifter_p->reCalculate();
      
      // Now do the conversions
      uInt nrows=dphase.nelements();
      Vector<Double> thisRow(3);
      thisRow=0.0;
      //CoordinateSystem csys=image->coordinates();
      //DirectionCoordinate dc=csys.directionCoordinate(0);
      //Vector<Double> thePix(2);
      //dc.toPixel(thePix, phasecenter);
      //cerr << "field id " << vb.fieldId() << "  the Pix " << thePix << endl;
      //Vector<Float> scale(2);
      //scale(0)=dc.increment()(0);
      //scale(1)=dc.increment()(1);
      for (uInt irow=0; irow<nrows;++irow) {
	thisRow.assign(uvw.column(irow));
	//cerr << " uvw " << thisRow ;
	// This is for frame change
	uvwMachine_p->convertUVW(dphase(irow), thisRow);
	// This is for correlator phase center change
	MVPosition rotphase=phaseShifter_p->rotationPhase() ;
	//cerr << " rotPhase " <<  rotphase << " oldphase "<<  rotphase*(uvw.column(irow))  << " newphase " << (rotphase)*thisRow ;
	//	cerr << " phase " << dphase(irow) << " new uvw " << uvw.column(irow);
	//dphase(irow)+= (thePix(0)-nx/2.0)*thisRow(0)*scale(0)+(thePix(1)-ny/2.0)*thisRow(1)*scale(1);
	//Double pixphase=(thePix(0)-nx/2.0)*uvw.column(irow)(0)*scale(0)+(thePix(1)-ny/2.0)*uvw.column(irow)(1)*scale(1);
	//Double pixphase2=(thePix(0)-nx/2.0)*thisRow(0)*scale(0)+(thePix(1)-ny/2.0)*thisRow(1)*scale(1);
	//cerr << " pixphase " <<  pixphase <<  " pixphase2 " << pixphase2<< endl;
	//dphase(irow)=pixphase;
	RotMatrix rotMat=phaseShifter_p->rotationUVW();
	uvw.column(irow)(0)=thisRow(0)*rotMat(0,0)+thisRow(1)*rotMat(1,0);
	uvw.column(irow)(1)=thisRow(1)*rotMat(1,1)+thisRow(0)*rotMat(0,1);
	uvw.column(irow)(2)=thisRow(0)*rotMat(0,2)+thisRow(1)*rotMat(1,2)+thisRow(2)*rotMat(2,2);
	dphase(irow)+= rotphase(0)*uvw.column(irow)(0)+rotphase(1)*uvw.column(irow)(1);
      }
	
      
    }
}


  void FTMachine::rotateUVW(Matrix<Double>& uvw, Vector<Double>& dphase,
  			    const vi::VisBuffer2& vb)
    {

      if(lastMSId_p != vb.msId())
	romscol_p=new MSColumns(vb.ms());
      //the uvw rotation is done for common tangent reprojection or if the
      //image center is different from the phasecenter
      // UVrotation is false only if field never changes

      if((vb.fieldId()(0)!=lastFieldId_p) || (vb.msId()!=lastMSId_p)){
        doUVWRotation_p=true;
	
      }
      else{
	//if above failed it still can be changing if   polynome phasecenter or ephem
	if( (vb.subtableColumns().field().numPoly()(lastFieldId_p) >0) ||  (! (vb.subtableColumns().field().ephemerisId().isNull()) &&(vb.subtableColumns().field().ephemerisId()(lastFieldId_p) > -1)))
	  doUVWRotation_p=True;
	
      }
      if(doUVWRotation_p || tangentSpecified_p || fixMovingSource_p){
        ok();
	
        mFrame_p.epoch() != 0 ?
	  mFrame_p.resetEpoch(MEpoch(Quantity(vb.time()(0), "s"))):
	 
	  mFrame_p.set(mLocation_p, MEpoch(Quantity(vb.time()(0), "s"), (romscol_p->timeMeas())(0).getRef()));

        MDirection phasecenter=mImage_p;
        if(fixMovingSource_p){

	  //First convert to HA-DEC or AZEL for parallax correction
	  MDirection::Ref outref1(MDirection::AZEL, mFrame_p);
	  MDirection tmphadec;
	  if(upcase(movingDir_p.getRefString()).contains("APP")){
	    tmphadec=MDirection::Convert((vbutil_p->getEphemDir(vb, phaseCenterTime_p)), outref1)();
	  }
	  else{
	    tmphadec=MDirection::Convert(movingDir_p, outref1)();
	  }
	  ////TESTOO waiting for CAS-11060
	  //MDirection tmphadec=MDirection::Convert((vbutil_p->getPhaseCenter(vb, phaseCenterTime_p)), outref1)();
	  /////////
	  MDirection::Ref outref(mImage_p.getRef().getType(), mFrame_p);
	  MDirection sourcenow=MDirection::Convert(tmphadec, outref)();
  	//cerr << "Rotating to fixed moving source " << MVDirection(phasecenter.getAngle()-firstMovingDir_p.getAngle()+sourcenow.getAngle()) << endl;
  	//phasecenter.set(MVDirection(phasecenter.getAngle()+firstMovingDir_p.getAngle()-sourcenow.getAngle()));
	  movingDirShift_p=MVDirection(sourcenow.getAngle()-firstMovingDir_p.getAngle());
	  phasecenter.shift(movingDirShift_p, False);
	  //cerr    << sourcenow.toString() <<"  "<<(vbutil_p->getPhaseCenter(vb, phaseCenterTime_p)).toString() <<  " difference " << firstMovingDir_p.getAngle() - sourcenow.getAngle() << endl;
      }


        // Set up the UVWMachine only if the field id has changed. If
        // the tangent plane is specified then we need a UVWMachine that
        // will reproject to that plane iso the image plane
        if(doUVWRotation_p || fixMovingSource_p) {

	  String observatory=(vb.subtableColumns().observation()).telescopeName()(0);
  	if(uvwMachine_p) delete uvwMachine_p; uvwMachine_p=0;
  	if(observatory.contains("ATCA") || observatory.contains("WSRT")){
  		//Tangent specified is being wrongly used...it should be for a
  	    	//Use the safest way  for now.
	  uvwMachine_p=new UVWMachine(phasecenter, vbutil_p->getPhaseCenter(vb, phaseCenterTime_p), mFrame_p,
  					true, false);
  	}
  	else{
	  uvwMachine_p=new UVWMachine(phasecenter, vbutil_p->getPhaseCenter(vb, phaseCenterTime_p), mFrame_p,
  					false,tangentSpecified_p);
  	    }
       }

  	lastFieldId_p=vb.fieldId()(0);
  	lastMSId_p=vb.msId();


        AlwaysAssert(uvwMachine_p, AipsError);

        // Always force a recalculation
        uvwMachine_p->reCalculate();

        // Now do the conversions
        uInt nrows=dphase.nelements();
        Vector<Double> thisRow(3);
        thisRow=0.0;
        uInt irow;
        //#pragma omp parallel default(shared) private(irow,thisRow)
        {
  	//#pragma omp for
  	  for (irow=0; irow<nrows;++irow) {
  	    thisRow.reference(uvw.column(irow));
  	    convUVW(dphase(irow), thisRow);
  	  }

        }//end pragma
      }
    }
  void FTMachine::convUVW(Double& dphase, Vector<Double>& thisrow){
    //for (uInt i=0;i<3;i++) thisRow(i)=uvw(i,row);
    uvwMachine_p->convertUVW(dphase, thisrow);
    //for (uint i=0;i<3;i++) uvw(i,row)=thisRow(i);

  }


  void FTMachine::locateuvw(const Double*& uvw, const Double*& dphase,
			    const Double*& freq, const Int& nvchan,
			    const Double*& scale, const Double*& offset,  const Int& sampling, Int*& loc, Int*& off, Complex*& phasor, const Int& row, const bool& doW){
    
    Int rowoff=row*nvchan;
    Double phase;
    Double pos;
    Int nel= doW ? 3 : 2;
    for (Int f=0; f<nvchan; ++f){
      for (Int k=0; k <2; ++k){
	pos=(scale[k])*uvw[3*row+k]*(freq[f])/C::c+((offset[k])+1.0);
	loc[(rowoff+f)*nel+k]=std::lround(pos);
	off[(rowoff+f)*nel+k]=std::lround((Double(loc[(rowoff+f)*nel+k])-pos)*Double(sampling));
	//off[(rowoff+f)*2+k]=(loc[(rowoff+f)*2+k]-pos(k))*sampling;	
      }
      phase=-Double(2.0)*C::pi*dphase[row]*(freq[f])/C::c;
      phasor[rowoff+f]=Complex(cos(phase), sin(phase));
     
      ///This is for W-Projection
      if(doW){
	pos=sqrt(fabs(scale[2]*uvw[3*row+2]*(freq[f])/C::c))+offset[2]+1.0;
	loc[(rowoff+f)*nel+2]=std::lround(pos);
	off[(rowoff+f)*nel+2]=0;
      }
    }

    


  }

  void FTMachine::setnumthreads(Int num){
    numthreads_p=num;
  }
  Int FTMachine::getnumthreads(){
    return numthreads_p;
  }

  //
  // Refocus the array on a point at finite distance
  //
    // Refocus the array on a point at finite distance
    //
    void FTMachine::refocus(Matrix<Double>& uvw, const Vector<Int>& ant1,
  			  const Vector<Int>& ant2,
  			  Vector<Double>& dphase, const vi::VisBuffer2& vb)
    {

      ok();

      if(abs(distance_p)>0.0) {

        nAntenna_p=max(vb.antenna2())+1;

        // Positions of antennas
        Matrix<Double> antPos(3,nAntenna_p);
        antPos=0.0;
        Vector<Int> nAntPos(nAntenna_p);
        nAntPos=0;

        uInt aref = min(ant1);

        // Now find the antenna locations: for this we just reference to a common
        // point. We ignore the time variation within this buffer.
        uInt nrows=dphase.nelements();
        for (uInt row=0;row<nrows;row++) {
  	uInt a1=ant1(row);
  	uInt a2=ant2(row);
  	for(uInt dim=0;dim<3;dim++) {
  	  antPos(dim, a1)+=uvw(dim, row);
  	  antPos(dim, a2)-=uvw(dim, row);
  	}
  	nAntPos(a1)+=1;
  	nAntPos(a2)+=1;
        }

        // Now remove the reference location
        Vector<Double> center(3);
        for(uInt dim=0;dim<3;dim++) {
  	center(dim) = antPos(dim,aref)/nAntPos(aref);
        }

        // Now normalize
        for (uInt ant=0; ant<nAntenna_p; ant++) {
  	if(nAntPos(ant)>0) {
  	  for(uInt dim=0;dim<3;dim++) {
  	    antPos(dim,ant)/=nAntPos(ant);
  	    antPos(dim,ant)-=center(dim);
  	  }
  	}
        }

        // Now calculate the offset needed to focus the array,
        // including the w term. This must have the correct asymptotic
        // form so that at infinity no net change occurs
        for (uInt row=0;row<nrows;row++) {
  	uInt a1=ant1(row);
  	uInt a2=ant2(row);

  	Double d1=distance_p*distance_p-2*distance_p*antPos(2,a1);
  	Double d2=distance_p*distance_p-2*distance_p*antPos(2,a2);
  	for(uInt dim=0;dim<3;dim++) {
  	  d1+=antPos(dim,a1)*antPos(dim,a1);
  	  d2+=antPos(dim,a2)*antPos(dim,a2);
  	}
  	d1=sqrt(d1);
  	d2=sqrt(d2);
  	for(uInt dim=0;dim<2;dim++) {
  	  dphase(row)-=(antPos(dim,a1)*antPos(dim,a1)-antPos(dim,a2)*antPos(dim,a2))/(2*distance_p);
  	}
  	uvw(0,row)=distance_p*(antPos(0,a1)/d1-antPos(0,a2)/d2);
  	uvw(1,row)=distance_p*(antPos(1,a1)/d1-antPos(1,a2)/d2);
  	uvw(2,row)=distance_p*(antPos(2,a1)/d1-antPos(2,a2)/d2)+dphase(row);
        }
      }
    }

  void FTMachine::ok() {
    AlwaysAssert(image, AipsError);
    AlwaysAssert(uvwMachine_p, AipsError);
  }
  
  Bool FTMachine::toRecord(String& error, RecordInterface& outRecord, 
			   Bool withImage, const String diskimage) {
    // Save the FTMachine to a Record
    //
    outRecord.define("name", this->name());
    if(withImage){
      if(image==nullptr)
        throw(AipsError("Programmer error: saving to record without proper initialization"));
      CoordinateSystem cs=image->coordinates();
      DirectionCoordinate dircoord=cs.directionCoordinate(cs.findCoordinate(Coordinate::DIRECTION));
      dircoord.setReferenceValue(mImage_p.getAngle().getValue());
      if(diskimage != ""){
	try{
	  PagedImage<Complex> imCopy(TiledShape(toVis_p ? griddedData.shape(): image->shape()), image->coordinates(), diskimage);
	  toVis_p ? imCopy.put(griddedData) : imCopy.copyData(*image);
	  ImageUtilities::copyMiscellaneous(imCopy, *image);
	  Vector<Double> pixcen(2);
	  pixcen(0)=Double(imCopy.shape()(0)/2); pixcen(1)=Double(imCopy.shape()(1)/2);
	  dircoord.setReferencePixel(pixcen);
	  cs.replaceCoordinate(dircoord, cs.findCoordinate(Coordinate::DIRECTION));
	  imCopy.setCoordinateInfo(cs);
	}
	catch(...){
	  throw(AipsError(String("Failed to save model image "+diskimage+String(" to disk")))); 
	}
	outRecord.define("diskimage", diskimage);
	
      }
      else{
	Record imrec;
	Vector<Double> pixcen(2);
	pixcen(0)=Double(griddedData.shape()(0)/2); pixcen(1)=Double(griddedData.shape()(1)/2);
	dircoord.setReferencePixel(pixcen);
	cs.replaceCoordinate(dircoord, cs.findCoordinate(Coordinate::DIRECTION));
	TempImage<Complex> imCopy(griddedData.shape(), cs);
	imCopy.put(griddedData) ;
	ImageUtilities::copyMiscellaneous(imCopy, *image);
	if(imCopy.toRecord(error, imrec))
	  outRecord.defineRecord("image", imrec);
      }
    }
    outRecord.define("nantenna", nAntenna_p);
    outRecord.define("distance", distance_p);
    outRecord.define("lastfieldid", lastFieldId_p);
    outRecord.define("lastmsid", lastMSId_p);
    outRecord.define("tangentspecified", tangentSpecified_p);
    saveMeasure(outRecord, String("mtangent_rec"), error, mTangent_p);
    saveMeasure(outRecord, "mimage_rec", error, mImage_p);
    //mFrame_p not necessary to save as it is generated from mLocation_p
    outRecord.define("nx", nx);
    outRecord.define("ny", ny);
    outRecord.define("npol", npol);
    outRecord.define("nchan", nchan);
    outRecord.define("nvischan", nvischan);
    outRecord.define("nvispol", nvispol);
    //no need to save uvwMachine_p
    outRecord.define("douvwrotation", doUVWRotation_p);
    outRecord.define("freqinterpmethod", static_cast<Int>(freqInterpMethod_p));
    outRecord.define("spwchanselflag", spwChanSelFlag_p);
    outRecord.define("freqframevalid", freqFrameValid_p);
    //outRecord.define("selectedspw", selectedSpw_p);
    outRecord.define("imagefreq", imageFreq_p);
    outRecord.define("lsrfreq", lsrFreq_p);
    outRecord.define("interpvisfreq", interpVisFreq_p);
    Record multichmaprec;
    //for (uInt k=0; k < multiChanMap_p.nelements(); ++k)
    //  multichmaprec.define(k, multiChanMap_p[k]);
    //outRecord.defineRecord("multichanmaprec", multichmaprec);
    outRecord.define("chanmap", chanMap);
    outRecord.define("polmap", polMap);
    outRecord.define("nvischanmulti", nVisChan_p);
    //save moving source related variables
    storeMovingSourceState(error, outRecord);
    //outRecord.define("doconversion", doConversion_p);
    outRecord.define("pointingdircol", pointingDirCol_p);
    outRecord.define("usedoublegrid", useDoubleGrid_p);
    outRecord.define("cfstokes", cfStokes_p);
    outRecord.define("polinuse", polInUse_p);
    outRecord.define("tovis", toVis_p);
    outRecord.define("sumweight", sumWeight);
    outRecord.define("numthreads", numthreads_p);
    outRecord.define("phasecentertime", phaseCenterTime_p);
    //Need to serialized sj_p...the user has to set the sj_p after recovering from record
    return true;
  };
  
  Bool FTMachine::saveMeasure(RecordInterface& rec, const String& name, String& err, const Measure& meas){
    Record tmprec;
    MeasureHolder mh(meas);
    if(mh.toRecord(err, tmprec)){
      rec.defineRecord(name, tmprec);
      return true;
    }
    return false;
  }

  Bool FTMachine::fromRecord(String& error,
			     const RecordInterface& inRecord
			     ) {
    // Restore an FTMachine from a Record
    //
    uvwMachine_p=0; //when null it is reconstructed from mImage_p and mFrame_p
    //mFrame_p is not necessary as it is generated in initMaps from mLocation_p
    inRecord.get("nx", nx);
    inRecord.get("ny", ny);
    inRecord.get("npol", npol);
    inRecord.get("nchan", nchan);
    inRecord.get("nvischan", nvischan);
    inRecord.get("nvispol", nvispol);
    cmplxImage_p=NULL;
    inRecord.get("tovis", toVis_p);
    if(inRecord.isDefined("image")){
      cmplxImage_p=new TempImage<Complex>();
      image=&(*cmplxImage_p);
      
      const Record rec=inRecord.asRecord("image");
      if(!cmplxImage_p->fromRecord(error, rec))
	return false;   
      
    }
    else if(inRecord.isDefined("diskimage")){
      String theDiskImage;
      inRecord.get("diskimage", theDiskImage);
      try{
        File eldir(theDiskImage);
	if(! eldir.exists()){
	  String subPathname[30];
	  String sep = "/";
	  uInt nw = split(theDiskImage, subPathname, 20, sep);
	  String theposs=(subPathname[nw-1]);
	  Bool isExistant=File(theposs).exists();
	  if(isExistant) 
	    theDiskImage=theposs;
	  for (uInt i=nw-2 ; i>0; --i){
	    theposs=subPathname[i]+"/"+theposs;
	    File newEldir(theposs);
	    if(newEldir.exists()){
	      isExistant=true;
	      theDiskImage=theposs;
	    }
	  }
	  if(!isExistant)
	    throw(AipsError("Could not locate mage"));
	}
	cmplxImage_p=new PagedImage<Complex> (theDiskImage);
	image=&(*cmplxImage_p);
      }
      catch(...){
	throw(AipsError(String("Failure to load ")+theDiskImage+String(" image from disk")));
      }
    }
    if(toVis_p && !cmplxImage_p.null()) {
	griddedData.resize(image->shape());
	griddedData=image->get();
    }
    else if(!toVis_p){
      IPosition gridShape(4, nx, ny, npol, nchan);
      griddedData.resize(gridShape);
      griddedData=Complex(0.0);
    }

    nAntenna_p=inRecord.asuInt("nantenna");
    distance_p=inRecord.asDouble("distance");
    lastFieldId_p=inRecord.asInt("lastfieldid");
    lastMSId_p=inRecord.asInt("lastmsid");
    inRecord.get("tangentspecified", tangentSpecified_p);
    { const Record rec=inRecord.asRecord("mtangent_rec");
      MeasureHolder mh;
      if(!mh.fromRecord(error, rec))
	return false;
      mTangent_p=mh.asMDirection();
    }
    { const Record rec=inRecord.asRecord("mimage_rec");
      MeasureHolder mh;
      if(!mh.fromRecord(error, rec))
	return false;
      mImage_p=mh.asMDirection();
    }
    
   
   
    inRecord.get("douvwrotation", doUVWRotation_p);
   
    //inRecord.get("spwchanselflag", spwChanSelFlag_p);
    //We won't respect the chanselflag as the vister may have different selections
    spwChanSelFlag_p.resize();
    inRecord.get("freqframevalid", freqFrameValid_p);
    //inRecord.get("selectedspw", selectedSpw_p);
    inRecord.get("imagefreq", imageFreq_p);
    inRecord.get("lsrfreq", lsrFreq_p);
    inRecord.get("interpvisfreq", interpVisFreq_p);
    //const Record multichmaprec=inRecord.asRecord("multichanmaprec");
    //multiChanMap_p.resize(multichmaprec.nfields(), true, false);
    //for (uInt k=0; k < multichmaprec.nfields(); ++k)
    //  multichmaprec.get(k, multiChanMap_p[k]);
    inRecord.get("chanmap", chanMap);
    inRecord.get("polmap", polMap);
    inRecord.get("nvischanmulti", nVisChan_p);
    //inRecord.get("doconversion", doConversion_p);
    inRecord.get("pointingdircol", pointingDirCol_p);
    
    
    inRecord.get("usedoublegrid", useDoubleGrid_p);
    inRecord.get("cfstokes", cfStokes_p);
    inRecord.get("polinuse", polInUse_p);
    
    
    inRecord.get("sumweight", sumWeight);
    if(toVis_p){
      freqInterpMethod_p=InterpolateArray1D<Double, Complex>::nearestNeighbour;
    }
    else{
     Int tmpInt;
      inRecord.get("freqinterpmethod", tmpInt);
      freqInterpMethod_p=static_cast<InterpolateArray1D<Double, Complex >::InterpolationMethod>(tmpInt);
    }
    inRecord.get("numthreads", numthreads_p);
    inRecord.get("phasecentertime", phaseCenterTime_p);
    ///No need to store this...recalculate thread partion because environment 
    ///may have changed.
    doneThreadPartition_p=-1;
    vbutil_p=nullptr;
    briggsWeightor_p=nullptr;
    ft_p=FFT2D(true);
    if(!recoverMovingSourceState(error, inRecord))
      return False;
    return true;
  };
  Bool FTMachine::storeMovingSourceState(String& error, RecordInterface& outRecord){

    Bool retval=True;
    retval=retval && saveMeasure(outRecord, "mlocation_rec", error, mLocation_p);
    spectralCoord_p.save(outRecord, "spectralcoord");
    retval=retval && saveMeasure(outRecord, "movingdir_rec", error, movingDir_p);
    outRecord.define("fixmovingsource", fixMovingSource_p);
    retval=retval && saveMeasure(outRecord, "firstmovingdir_rec", error, firstMovingDir_p);
    movingDirShift_p=MVDirection(0.0);
    if( mFrame_p.comet()){
      String ephemTab=MeasComet(*(mFrame_p.comet())).getTablePath();
      outRecord.define("ephemeristable",ephemTab);
    }
    return retval;
  }
  Bool FTMachine::recoverMovingSourceState(String& error, const RecordInterface& inRecord){
    Bool retval=True;
    inRecord.get("fixmovingsource", fixMovingSource_p);
    { const Record rec=inRecord.asRecord("firstmovingdir_rec");
      MeasureHolder mh;
      if(!mh.fromRecord(error, rec))
	return false;
      firstMovingDir_p=mh.asMDirection();
    }
    { const Record rec=inRecord.asRecord("movingdir_rec");
      MeasureHolder mh;
      if(!mh.fromRecord(error, rec))
	return false;
      movingDir_p=mh.asMDirection();
    }
     { const Record rec=inRecord.asRecord("mlocation_rec");
      MeasureHolder mh;
      if(!mh.fromRecord(error, rec))
	return false;
      mLocation_p=mh.asMPosition();
    }
     SpectralCoordinate *tmpSpec=SpectralCoordinate::restore(inRecord, "spectralcoord");
    if(tmpSpec){
      spectralCoord_p=*tmpSpec;
      delete tmpSpec;
    }
    visPolMap_p.resize();
    if(inRecord.isDefined("ephemeristable")){
      String ephemtab;
      inRecord.get("ephemeristable", ephemtab);
      MeasComet laComet;
      if(Table::isReadable(ephemtab, False)){
	Table laTable(ephemtab);
	Path leSentier(ephemtab);
	laComet=MeasComet(laTable, leSentier.absoluteName());
      }
      else{
        laComet= MeasComet(ephemtab);
      }
      if(!mFrame_p.comet())
	mFrame_p.set(laComet);
      else
	mFrame_p.resetComet(laComet);
    }
    
    return retval;
  }
  
  
  void FTMachine::getImagingWeight(Matrix<Float>& imwgt, const vi::VisBuffer2& vb){
    //cerr << "BRIGGSweightor " << briggsWeightor_p.null()  << " or " << !briggsWeoght_p << endl;
    if(briggsWeightor_p.null()){
      imwgt=vb.imagingWeight();
    }
    else{
      briggsWeightor_p->weightUniform(imwgt, vb);  
    }

  }
  // Make a plain straightforward honest-to-FSM image. This returns
  // a complex image, without conversion to Stokes. The representation
  // is that required for the visibilities.
  //----------------------------------------------------------------------
  void FTMachine::makeImage(FTMachine::Type type, 
			    vi::VisibilityIterator2& vi,
			    ImageInterface<Complex>& theImage,
			    Matrix<Float>& weight) {
    
    
    logIO() << LogOrigin("FTMachine", "makeImage0") << LogIO::NORMAL;
    
    // Loop over all visibilities and pixels
    vi::VisBuffer2* vb=vi.getVisBuffer();
    
    // Initialize put (i.e. transform to Sky) for this model
    vi.origin();
    
    if(vb->polarizationFrame()==MSIter::Linear) {
      StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::LINEAR);
    }
    else {
      StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::CIRCULAR);
    }
    
    initializeToSky(theImage,weight,*vb);
    Bool useCorrected= !(MSColumns(vi.ms()).correctedData().isNull());
    if((type==FTMachine::CORRECTED) && (!useCorrected))
      type=FTMachine::OBSERVED;
    Bool normalize=true;
    if(type==FTMachine::COVERAGE)
      normalize=false;

    // Loop over the visibilities, putting VisBuffers
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin(); vi.more(); vi.next()) {
	
	switch(type) {
	case FTMachine::RESIDUAL:
	  vb->setVisCube(vb->visCubeCorrected());
	  vb->setVisCube(vb->visCube()-vb->visCubeModel());
	  put(*vb, -1, false);
	  break;
	case FTMachine::MODEL:
	  put(*vb, -1, false, FTMachine::MODEL);
	  break;
	case FTMachine::CORRECTED:
	  put(*vb, -1, false, FTMachine::CORRECTED);
	  break;
	case FTMachine::PSF:
	  vb->setVisCube(Complex(1.0,0.0));
	  put(*vb, -1, true, FTMachine::PSF);
	  break;
	case FTMachine::COVERAGE:
	  vb->setVisCube(Complex(1.0));
	  put(*vb, -1, true, FTMachine::COVERAGE);
	  break;
	case FTMachine::OBSERVED:
	default:
	  put(*vb, -1, false, FTMachine::OBSERVED);
	  break;
	}
      }
    }
    finalizeToSky();
    // Normalize by dividing out weights, etc.
    getImage(weight, normalize);
  }
  
  
  
  
  Bool FTMachine::setFrameValidity(Bool validFrame){
    
    freqFrameValid_p=validFrame;
    return true;
  }
  

  Vector<Int> FTMachine::channelMap(const vi::VisBuffer2& vb){
    matchChannel(vb);
    return chanMap;
  }
  Bool FTMachine::matchChannel(const vi::VisBuffer2& vb){
    //Int spw=vb.spectralWindows()[0];
    nvischan  = vb.nChannels();
    chanMap.resize(nvischan);
    chanMap.set(-1);
    Vector<Double> lsrFreq(0);

      //cerr << "doConve " << spw << "   " << doConversion_p[spw] << " freqframeval " << freqFrameValid_p << endl;
//cerr <<"valid frame " << freqFrameValid_p << " polmap "<< polMap << endl;
    //cerr << "spectral coord system " << spectralCoord_p.frequencySystem(False) << endl;
    if (freqFrameValid_p &&spectralCoord_p.frequencySystem(False)!=MFrequency::REST ) {
      lsrFreq=vb.getFrequencies(0,MFrequency::LSRK);
    }
    else {
      lsrFreq=vb.getFrequencies(0);
    }
    if (spectralCoord_p.frequencySystem(False)==MFrequency::REST && fixMovingSource_p) {
      if(lastMSId_p != vb.msId()){
	romscol_p=new MSColumns(vb.ms());
	//if ms changed ...reset ephem table
	if (upcase(movingDir_p.getRefString()).contains("APP")) {
	  MeasComet mcomet(Path((romscol_p->field()).ephemPath(vb.fieldId()(0))).absoluteName());
	  mFrame_p.resetComet(mcomet);
	}
      }
	
      mFrame_p.resetEpoch(MEpoch(Quantity(vb.time()(0), "s")));
      mFrame_p.resetDirection(vbutil_p->getEphemDir(vb, phaseCenterTime_p));
      shiftFreqToSource(lsrFreq);
    }
     //cerr << "lsrFreq " << lsrFreq.shape() << " nvischan " << nvischan << endl;
     //     if(doConversion_p.nelements() < uInt(spw+1))
     //	 doConversion_p.resize(spw+1, true);
     // doConversion_p[spw]=freqFrameValid_p;

      if(lsrFreq.nelements() ==0){
        return false;
      }
      lsrFreq_p.resize(lsrFreq.nelements());
      lsrFreq_p=lsrFreq;
      Vector<Double> c(1);
      c=0.0;
      Vector<Double> f(1);
      Int nFound=0;
      
      Double minFreq;
      Double maxFreq;
      spectralCoord_p.toWorld(minFreq, Double(0));
      spectralCoord_p.toWorld(maxFreq, Double(nchan));
      if(maxFreq < minFreq){
        f(0)=minFreq;
        minFreq=maxFreq;
        maxFreq=f(0);      
      }
        
      
      //cout.precision(10);
      for (Int chan=0;chan<nvischan;chan++) {
        f(0)=lsrFreq[chan];
        if(spectralCoord_p.toPixel(c, f)) {
  	Int pixel=Int(floor(c(0)+0.5));  // round to chan freq at chan center
  	//cerr << " chan " << chan << " f " << f(0) << " pixel "<< c(0) << "  " << pixel << endl;
  	/////////////
  	//c(0)=pixel;
  	//spectralCoord_p.toWorld(f, c);
  	//cerr << "f1 " << f(0) << " pixel "<< c(0) << "  " << pixel << endl;
  	////////////////
  	if(pixel>-1&&pixel<nchan) {
  	  chanMap(chan)=pixel;
  	  nFound++;
  	  if(nvischan>1&&(chan==0||chan==nvischan-1)) {
  	    /*logIO() << LogIO::DEBUGGING
  		    << "Selected visibility channel : " << chan+1
  		    << " has frequency "
  		    <<  MFrequency(Quantity(f(0), "Hz")).get("GHz").getValue()
  		    << " GHz and maps to image pixel " << pixel+1 << LogIO::POST;
  	    */
  	  }
  	}
	else{
	  
	  if(nvischan > 1){
	    Double fwidth=lsrFreq[1]-lsrFreq[0];
	    Double limit=0;
	    //Double where=c(0)*fabs(spectralCoord_p.increment()(0));
	    if( freqInterpMethod_p==InterpolateArray1D<Double,Complex>::linear)
	      limit=2;
	    else if( freqInterpMethod_p==InterpolateArray1D<Double,Complex>::cubic ||  freqInterpMethod_p==InterpolateArray1D<Double,Complex>::spline)
	      limit=4;
            //cerr<< "where " << where << " pixel " << pixel << " fwidth " << fwidth << endl;
            /*
	    if(((pixel<0) && (where >= (0-limit*fabs(fwidth)))) )
	      chanMap(chan)=-2;
	    if((pixel>=nchan) ) {
	      where=f(0);
	      Double fend;
	      spectralCoord_p.toWorld(fend, Double(nchan));
	      if( ( (fwidth >0) &&where < (fend+limit*fwidth))  || ( (fwidth <0) &&where > (fend+limit*fwidth)) )
		chanMap(chan)=-2;
	    }
            */
            
            if((f(0) <  (maxFreq + limit*fabs(fwidth))) && (f(0) >(maxFreq-0.5*fabs(fwidth)))){
              chanMap(chan)=-2;
            }
            if((f(0) < minFreq+0.5*fabs(fwidth)) &&  (f(0) > (minFreq-limit*fabs(fwidth)))){
              chanMap(chan)=-2;
            }
	  }


	}
        }
      }
      //cerr << "chanmap " << chanMap << endl;
      /* if(multiChanMap_p.nelements() < uInt(spw+1))
    	  multiChanMap_p.resize(spw+1);
      multiChanMap_p[spw].resize();
      multiChanMap_p[spw]=chanMap;
      */

      if(nFound==0) {
        /*
  	logIO()  << "Visibility channels in spw " << spw+1
  	<<      " of ms " << vb.msId() << " is not being used "
  	<< LogIO::WARN << LogIO::POST;
        */
        return false;
      }

      return matchPol(vb);
      


    }

  Bool FTMachine::matchPol(const vi::VisBuffer2& vb){
    Vector<Stokes::StokesTypes> visPolMap(vb.getCorrelationTypesSelected());
    if((polMap.nelements() > 0) &&(visPolMap.nelements() == visPolMap_p.nelements()) &&allEQ(visPolMap, visPolMap_p))
      return True;
    Int stokesIndex=image->coordinates().findCoordinate(Coordinate::STOKES);
    AlwaysAssert(stokesIndex>-1, AipsError);
    StokesCoordinate stokesCoord=image->coordinates().stokesCoordinate(stokesIndex);


    visPolMap_p.resize();
    visPolMap_p=visPolMap;
    nvispol=visPolMap.nelements();
    AlwaysAssert(nvispol>0, AipsError);
    polMap.resize(nvispol);
    polMap=-1;
    Int pol=0;
    Bool found=false;
    // First we try matching Stokes in the visibilities to
    // Stokes in the image that we are gridding into.
    for (pol=0;pol<nvispol;pol++) {
      Int p=0;
      if(stokesCoord.toPixel(p, Stokes::type(visPolMap(pol)))) {
        AlwaysAssert(p<npol, AipsError);
        polMap(pol)=p;
        found=true;
      }
    }
      // If this fails then perhaps we were looking to grid I
      // directly. If so then we need to check that the parallel
      // hands are present in the visibilities.
    if(!found) {
      Int p=0;
      if(stokesCoord.toPixel(p, Stokes::I)) {
        polMap=-1;
        if(vb.polarizationFrame()==MSIter::Linear) {
          p=0;
          for (pol=0;pol<nvispol;pol++) {
            if(Stokes::type(visPolMap(pol))==Stokes::XX)
              {polMap(pol)=0;p++;found=true;};
            if(Stokes::type(visPolMap(pol))==Stokes::YY)
              {polMap(pol)=0;p++;found=true;};
          }
        }
        else {
          p=0;
          for (pol=0;pol<nvispol;pol++) {
            if(Stokes::type(visPolMap(pol))==Stokes::LL)
              {polMap(pol)=0;p++;found=true;};
            if(Stokes::type(visPolMap(pol))==Stokes::RR)
              {polMap(pol)=0;p++;found=true;};
          }
        }
        if(!found) {
          logIO() <<  "Cannot find polarization map: visibility polarizations = "
    					<< visPolMap << LogIO::EXCEPTION;
        }
    	else {
    		
    		//logIO() << LogIO::DEBUGGING << "Transforming I only" << LogIO::POST;
    	}
      };
    }
    return True;
  } 

  Vector<String> FTMachine::cleanupTempFiles(const String& mess){
    briggsWeightor_p=nullptr;
    for(uInt k=0; k < tempFileNames_p.nelements(); ++k){
      if(Table::isReadable(tempFileNames_p[k])){
	if(mess.size()==0){
	  try{
	    Table::deleteTable(tempFileNames_p[k]);
	  }
	  catch(AipsError &x){
	    logIO() << LogOrigin("FTMachine", "cleanupTempFiles") << LogIO::NORMAL;
	    logIO() <<  LogIO::WARN<< "YOU may have to delete the temporary file " << tempFileNames_p[k] << " because " << x.getMesg()  << LogIO::POST;

	  }
	}
	else{
	  logIO() << LogOrigin("FTMachine", "cleanupTempFiles") << LogIO::NORMAL;
	  logIO() << "YOU have to delete the temporary file " << tempFileNames_p[k] << " because " << mess << LogIO::DEBUG1 << LogIO::POST;
	}
      }
    }
    return tempFileNames_p;
  }
  void FTMachine::gridOk(Int convSupport){
    
    if (nx <= 2*convSupport) {
      logIO_p 
	<< "number of pixels "
	<< nx << " on x axis is smaller that the gridding support "
	<< 2*convSupport   << " Please use a larger value " 
	<< LogIO::EXCEPTION;
    }
    
    if (ny <= 2*convSupport) {
      logIO_p 
	<< "number of pixels "
	<< ny << " on y axis is smaller that the gridding support "
	<< 2*convSupport   << " Please use a larger value " 
	<< LogIO::EXCEPTION;
    }
    
  }
  
  void FTMachine::setLocation(const MPosition& loc){
    
    mLocation_p=loc;
    
  }
  
  MPosition& FTMachine::getLocation(){
    
    return mLocation_p;
  }
  
  
  void FTMachine::setMovingSource(const String& sname, const String& ephtab){
    String sourcename=sname;
    String ephemtab=ephtab;
    //if a table is given as sourcename...assume ephemerides
    if(Table::isReadable(sourcename, False)){
      sourcename="COMET";
      ephemtab=sname;
      ephemTableName_p = sname;
    }
    ///Special case
    if(upcase(sourcename)=="TRACKFIELD"){
      //if(name().contains("MosaicFT"))
      //	throw(AipsError("Cannot use field phasecenter to track moving source in a Mosaic"));
      fixMovingSource_p=True;
      movingDir_p=MDirection(Quantity(0.0,"deg"), Quantity(90.0, "deg"));
      movingDir_p.setRefString("APP");
      ///Setting it to APP with movingDir_p==True  => should use the phasecenter to track
      ///Discussion in CAS-9004 where users are too lazy to give an ephemtable.
      return;
    }

    MDirection::Types refType;
    Bool  isValid = MDirection::getType(refType, sourcename);
    if(!isValid)
      throw(AipsError("Cannot recognize moving source "+sourcename));
    if(refType < MDirection::N_Types || refType > MDirection:: N_Planets )
      throw(AipsError(sourcename+" is not type of source we can track"));
    if(refType==MDirection::COMET){
      MeasComet laComet;
      if(Table::isReadable(ephemtab, False)){
	Table laTable(ephemtab);
	Path leSentier(ephemtab);
	laComet=MeasComet(laTable, leSentier.absoluteName());
      }
      else{
        laComet= MeasComet(ephemtab);
      }
      if(!mFrame_p.comet())
	mFrame_p.set(laComet);
      else
	mFrame_p.resetComet(laComet);
    }
    fixMovingSource_p=true;
    movingDir_p=MDirection(Quantity(0.0,"deg"), Quantity(90.0, "deg"));
    movingDir_p.setRefString(sourcename);
    // cerr << "movingdir " << movingDir_p.toString() << endl;
  }


  void FTMachine::setMovingSource(const MDirection& mdir){
    
    fixMovingSource_p=true;
    movingDir_p=mdir;
    
  }
  
  void FTMachine::setFreqInterpolation(const String& method){
    
    String meth=method;
    meth.downcase();
    if(meth.contains("linear")){
      freqInterpMethod_p=InterpolateArray1D<Double,Complex>::linear;
    }
    else if(meth.contains("splin")){
      freqInterpMethod_p=InterpolateArray1D<Double,Complex>::spline;  
    }	    
    else if(meth.contains("cub")){
      freqInterpMethod_p=InterpolateArray1D<Double,Complex>::cubic;
    }
    else{
      freqInterpMethod_p=InterpolateArray1D<Double,Complex>::nearestNeighbour;
    }
  
  }
  void FTMachine::setFreqInterpolation(const InterpolateArray1D<Double,Complex>::InterpolationMethod type){
    freqInterpMethod_p=type;
  }
  
  // helper function to swap the y and z axes of a Cube
  void FTMachine::swapyz(Cube<Complex>& out, const Cube<Complex>& in)
  {
    IPosition inShape=in.shape();
    uInt nxx=inShape(0),nyy=inShape(2),nzz=inShape(1);
    //resize breaks  references...so out better have the right shape 
    //if references is not to be broken
    if(out.nelements()==0)
      out.resize(nxx,nyy,nzz);
    Bool deleteIn,deleteOut;
    const Complex* pin = in.getStorage(deleteIn);
    Complex* pout = out.getStorage(deleteOut);
    uInt i=0, zOffset=0;
    for (uInt iz=0; iz<nzz; ++iz, zOffset+=nxx) {
      Int yOffset=zOffset;
      for (uInt iy=0; iy<nyy; ++iy, yOffset+=nxx*nzz) {
	for (uInt ix=0; ix<nxx; ++ix){ 
	  pout[i++] = pin[ix+yOffset];
	}
      }
    }
    out.putStorage(pout,deleteOut);
    in.freeStorage(pin,deleteIn);
  }

  void FTMachine::swapyz(Cube<Complex>& out, const Cube<Bool>& outFlag, const Cube<Complex>& in)
  {
    IPosition inShape=in.shape();
    uInt nxx=inShape(0),nyy=inShape(2),nzz=inShape(1);
    //resize breaks  references...so out better have the right shape 
    //if references is not to be broken
    if(out.nelements()==0)
      out.resize(nxx,nyy,nzz);
    Bool deleteIn,deleteOut, delFlag;
    const Complex* pin = in.getStorage(deleteIn);
    const Bool* poutflag= outFlag.getStorage(delFlag);
    Complex* pout = out.getStorage(deleteOut);
    uInt i=0, zOffset=0;
    for (uInt iz=0; iz<nzz; ++iz, zOffset+=nxx) {
      Int yOffset=zOffset;
      for (uInt iy=0; iy<nyy; ++iy, yOffset+=nxx*nzz) {
	for (uInt ix=0; ix<nxx; ++ix){ 
	  if(!poutflag[i])
	    pout[i] = pin[ix+yOffset];
	  ++i;
	}
      }
    }
    out.putStorage(pout,deleteOut);
    in.freeStorage(pin,deleteIn);
    outFlag.freeStorage(poutflag, delFlag);
  }

  // helper function to swap the y and z axes of a Cube
  void FTMachine::swapyz(Cube<Bool>& out, const Cube<Bool>& in)
  {
    IPosition inShape=in.shape();
    uInt nxx=inShape(0),nyy=inShape(2),nzz=inShape(1);
    if(out.nelements()==0)
      out.resize(nxx,nyy,nzz);
    Bool deleteIn,deleteOut;
    const Bool* pin = in.getStorage(deleteIn);
    Bool* pout = out.getStorage(deleteOut);
    uInt i=0, zOffset=0;
    for (uInt iz=0; iz<nzz; iz++, zOffset+=nxx) {
      Int yOffset=zOffset;
      for (uInt iy=0; iy<nyy; iy++, yOffset+=nxx*nzz) {
	for (uInt ix=0; ix<nxx; ix++) pout[i++] = pin[ix+yOffset];
      }
    }
    out.putStorage(pout,deleteOut);
    in.freeStorage(pin,deleteIn);
  }
  
  void FTMachine::setPointingDirColumn(const String& column){
    pointingDirCol_p=column;
    pointingDirCol_p.upcase();
    if( (pointingDirCol_p != "DIRECTION") &&(pointingDirCol_p != "TARGET") && (pointingDirCol_p != "ENCODER") && (pointingDirCol_p != "POINTING_OFFSET") && (pointingDirCol_p != "SOURCE_OFFSET")){
      
      //basically at this stage you don't know what you're doing...so you get the default
      
      pointingDirCol_p="DIRECTION";
      
    }    
  }
  
  String FTMachine::getPointingDirColumnInUse(){
    
    return pointingDirCol_p;
    
  }
  
  void FTMachine::setSpwChanSelection(const Cube<Int>& spwchansels) {
    spwChanSelFlag_p.resize();
    spwChanSelFlag_p=spwchansels;
  }
  
  void FTMachine::setSpwFreqSelection(const Matrix<Double>& spwFreqs) 
  {
    spwFreqSel_p.assign(spwFreqs);
    SynthesisUtils::expandFreqSelection(spwFreqs,expandedSpwFreqSel_p, expandedSpwConjFreqSel_p);
  }

  void FTMachine::setSpectralFlag(const vi::VisBuffer2& vb, Cube<Bool>& modflagcube){

      modflagcube.resize(vb.flagCube().shape());
      modflagcube=vb.flagCube();
      if(!isPseudoI_p){
	ArrayIterator<Bool> from(modflagcube, IPosition(1, 0));
	while(!from.pastEnd()){
	  if(anyTrue(from.array()))
	    from.array().set(True);
	  from.next();
	}
      }
      uInt nchan = vb.nChannels();
      uInt msid = vb.msId();
      uInt selspw = vb.spectralWindows()[0];
      Bool spwFlagIsSet=( (spwChanSelFlag_p.nelements() > 0) && (spwChanSelFlag_p.shape()(1) > selspw) &&
  			(spwChanSelFlag_p.shape()(0) > msid) &&
  			(spwChanSelFlag_p.shape()(2) >=nchan));
      //cerr << "spwFlagIsSet " << spwFlagIsSet << endl;
      for (uInt i=0;i<nchan;i++) {
        //Flag those channels that  did not get selected...
        //respect the flags from vb  if selected  or
        //if spwChanSelFlag is wrong shape
        if ((spwFlagIsSet) && (spwChanSelFlag_p(msid,selspw,i)!=1)) {
  	modflagcube.xzPlane(i).set(true);
        }
      }
    }

  Matrix<Double> FTMachine::negateUV(const vi::VisBuffer2& vb){
    Matrix<Double> uvw(vb.uvw().shape());
    for (rownr_t i=0;i< vb.nRows() ; ++i) {
      for (Int idim=0;idim<2; ++idim) uvw(idim,i)=-vb.uvw()(idim, i);
      uvw(2,i)=vb.uvw()(2,i);
    }
    return uvw;
  }
  //-----------------------------------------------------------------------------------------------------------------
  //------------  Vectorized versions of initializeToVis, initializeToSky, finalizeToSky  
  //------------  that are called from CubeSkyEquation.
  //------------  They call getImage,getWeightImage, which are implemented in all FTMs
  //------------  Also, Correlation / Stokes conversions and gS/ggS normalizations.
 

  void FTMachine::setSkyJones(Vector<CountedPtr<casa::refim::SkyJones> >& sj){
    sj_p.resize();
    sj_p=sj;
    cout << "FTM : Set Sky Jones of length " << sj_p.nelements() << " and types ";
    for( uInt i=0;i<sj_p.nelements();i++) cout << sj_p[i]->type() << " ";
    cout << endl;
  }
  // Convert complex correlation planes to float Stokes planes
  void FTMachine::correlationToStokes(ImageInterface<Complex>& compImage, 
				      ImageInterface<Float>& resImage, 
				      const Bool dopsf)
  {
    // Convert correlation image to IQUV format
    AlwaysAssert(compImage.shape()[0]==resImage.shape()[0], AipsError);
    AlwaysAssert(compImage.shape()[1]==resImage.shape()[1], AipsError);
    AlwaysAssert(compImage.shape()[3]==resImage.shape()[3], AipsError);
    
    if(dopsf) 
      { 
	// For the PSF, choose only those stokes planes that have a valid PSF
	StokesImageUtil::ToStokesPSF(resImage,compImage);
      }
    else 
      {
	StokesImageUtil::To(resImage,compImage);
      }
  };
  
  // Convert float Stokes planes to complex correlation planes
  void FTMachine::stokesToCorrelation(ImageInterface<Float>& modelImage,
				      ImageInterface<Complex>& compImage)
  {
    /*
    StokesCoordinate stcomp=compImage.coordinates().stokesCoordinate(compImage.coordinates().findCoordinate(Coordinate::STOKES));
    StokesCoordinate stfloat = modelImage.coordinates().stokesCoordinate(modelImage.coordinates().findCoordinate(Coordinate::STOKES));

    cout << "Stokes types : complex : " << stcomp.stokes() << "    float : " << stfloat.stokes() << endl;
    cout << "Shapes : complex : " << compImage.shape() << "   float : " << modelImage.shape() << endl;
    */

    //Pol axis need not be same
    AlwaysAssert(modelImage.shape()[0]==compImage.shape()[0], AipsError);
    AlwaysAssert(modelImage.shape()[1]==compImage.shape()[1], AipsError);
    AlwaysAssert(modelImage.shape()[3]==compImage.shape()[3], AipsError);
    // Convert from Stokes to Complex
    StokesImageUtil::From(compImage, modelImage);
  };
  
  //------------------------------------------------------------------------------------------------------------------
  
  void FTMachine::normalizeImage(ImageInterface<Float>& skyImage,
				 Matrix<Float>& sumOfWts,
				 ImageInterface<Float>& sensitivityImage,
				 Bool dopsf, Float pblimit, Int normtype)
  {
    
    //Normalize the sky Image
    Int nXX=(skyImage).shape()(0);
    Int nYY=(skyImage).shape()(1);
    Int npola= (skyImage).shape()(2);
    Int nchana= (skyImage).shape()(3);
    
      IPosition pcentre(4,nXX/2,nYY/2,0,0);
    // IPosition psource(4,nXX/2+22,nYY/2,0,0);
    
    //    storeImg(String("norm_resimage.im") , skyImage);
    //    storeImg(String("norm_sensitivity.im"), sensitivityImage);
   
      /////    cout << "FTM::norm : pblimit : " << pblimit << endl; 
    
    // Note : This is needed because initial prediction has no info about sumwt.
    // Not a clean solution.  // ForSB -- if you see a better way, go for it.
    if(sumOfWts.shape() != IPosition(2,npola,nchana))
      {
	cout << "Empty SumWt.... resizing and filling with 1.0" << endl;
	sumOfWts.resize(IPosition(2,npola,nchana));
	sumOfWts=1.0;
      }
    
    //    if(dopsf)  cout << "*** FTM::normalizeImage : Image Center : " << skyImage.getAt(pcentre) << "  and weightImage : " << sensitivityImage.getAt(pcentre) << "  SumWt : " << sumOfWts[0,0];  
    //    else  cout << "*** FTM::normalizeImage : Source Loc : " << skyImage.getAt(psource) << "  and weightImage : " << sensitivityImage.getAt(psource) << "  SumWt : " << sumOfWts[0,0]; 
    
    
    
    IPosition blc(4,nXX, nYY, npola, nchana);
    IPosition trc(4, nXX, nYY, npola, nchana);
    blc(0)=0; blc(1)=0; trc(0)=nXX-1; trc(1)=nYY-1;
    //max weights per plane
    for (Int pol=0; pol < npola; ++pol){
      for (Int chan=0; chan < nchana ; ++chan){
	
	blc(2)=pol; trc(2)=pol;
	blc(3)=chan; trc(3)=chan;
	Slicer sl(blc, trc, Slicer::endIsLast);
	SubImage<Float> subSkyImage(skyImage, sl, false);
	SubImage<Float> subSensitivityImage(sensitivityImage, sl, false);
	SubImage<Float> subOutput(skyImage, sl, true);
	Float sumWt = sumOfWts(pol,chan);
	if(sumWt > 0.0){
	  switch(normtype)
	    {
	    case 0: // only sum Of Weights - FTM only (ForSB)
	      subOutput.copyData( (LatticeExpr<Float>) 
				  ((dopsf?1.0:-1.0)*subSkyImage/(sumWt)));
	      break;
	      
	    case 1: // only sensitivityImage   Ic/avgPB  (ForSB)
	      subOutput.copyData( (LatticeExpr<Float>) 
				  (iif(subSensitivityImage > (pblimit), 
				       (subSkyImage/(subSensitivityImage)),
				       (subSkyImage))));
				       // 0.0)));
	      break;
	      
	    case 2: // sum of Weights and sensitivityImage  IGridded/(SoW*avgPB) and PSF --> Id (ForSB)
	      subOutput.copyData( (LatticeExpr<Float>) 
				  (iif(subSensitivityImage > (pblimit), 
				       ((dopsf?1.0:-1.0)*subSkyImage/(subSensitivityImage*sumWt)),
				       //((dopsf?1.0:-1.0)*subSkyImage))));
				       0.0)));		
	      break;
	      
  	    case 3: // MULTIPLY by the sensitivityImage  avgPB
	      subOutput.copyData( (LatticeExpr<Float>) (subSkyImage * subSensitivityImage) );
	      break;
	      
  	    case 4: // DIVIDE by sqrt of sensitivityImage 
	      subOutput.copyData( (LatticeExpr<Float>) 
				  (iif((subSensitivityImage) > (pblimit), 
				       (subSkyImage/(sqrt(subSensitivityImage))),
				       (subSkyImage))));
				       //0.0)));
	      break;
	      
  	    case 5: // MULTIPLY by sqrt of sensitivityImage 
	      subOutput.copyData( (LatticeExpr<Float>) 
				  (iif((subSensitivityImage) > (pblimit), 
				       (subSkyImage * (sqrt(subSensitivityImage))),
				       (subSkyImage))));

	      break;

	    case 6: // divide by non normalized sensitivity image
	      {
		Float elpblimit=max( subSensitivityImage).getFloat() * pblimit;
		subOutput.copyData( (LatticeExpr<Float>) 
				    (iif(subSensitivityImage > (elpblimit), 
					 ((dopsf?1.0:-1.0)*subSkyImage/(subSensitivityImage)),
					 0.0)));
	      }
	      break;
	    default:
	      throw(AipsError("Unrecognized norm-type in FTM::normalizeImage : "+String::toString(normtype)));
	    }
	}
	else{
	  subOutput.set(0.0);
	}
      }
    }
    
    //if(dopsf)  cout << " Normalized (" << normtype << ") Image Center : " << skyImage.getAt(pcentre) << endl; 
     //     else  cout << " Normalized (" << normtype << ") Source Loc : " << skyImage.getAt(psource) << endl; 
    
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  /////  For use with the new framework 
  ///// (Sorry about these copies, but need to keep old system working)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  // Vectorized InitializeToVis
  void FTMachine::initializeToVisNew(const VisBuffer2& vb,
				     CountedPtr<SIImageStore> imstore)
  {
    AlwaysAssert(imstore->getNTaylorTerms(false)==1, AipsError);

    Matrix<Float> tempWts;
    
    if(!(imstore->forwardGrid()).get())
      throw(AipsError("FTMAchine::InitializeToVisNew error imagestore has no valid grid initialized"));
    // Convert from Stokes planes to Correlation planes
    LatticeLocker lock1 (*(imstore->model()), FileLocker::Read);
    stokesToCorrelation(*(imstore->model()), *(imstore->forwardGrid()));

    if(vb.polarizationFrame()==MSIter::Linear) {
      StokesImageUtil::changeCStokesRep(*(imstore->forwardGrid()),
					StokesImageUtil::LINEAR);
    }
    else {
      StokesImageUtil::changeCStokesRep(*(imstore->forwardGrid()),
					StokesImageUtil::CIRCULAR);
    }
   
    //------------------------------------------------------------------------------------
    // Image Mosaic only :  Multiply the input model with the Primary Beam
    if(sj_p.nelements() >0 ){
      for (uInt k=0; k < sj_p.nelements(); ++k){
	(sj_p(k))->apply(*(imstore->forwardGrid()), *(imstore->forwardGrid()), vb, 0, true);
      }
    }
    //------------------------------------------------------------------------------------

    // Call initializeToVis
    initializeToVis(*(imstore->forwardGrid()), vb); // Pure virtual
    
  };
  
  // Vectorized finalizeToVis is not implemented because it does nothing and is never called.
  
  // Vectorized InitializeToSky
  void FTMachine::initializeToSkyNew(const Bool dopsf, 
				     const VisBuffer2& vb,
				     CountedPtr<SIImageStore> imstore)
    
  {
    AlwaysAssert(imstore->getNTaylorTerms(false)==1, AipsError);
    
    // Make the relevant float grid. 
    // This is needed mainly for facetting (to set facet shapes), but is harmless for non-facetting.
    if( dopsf ) { imstore->psf(); } else { imstore->residual(); } 

    // Initialize the complex grid (i.e. tell FTMachine what array to use internally)
    Matrix<Float> sumWeight;
    if(!(imstore->backwardGrid()).get())
      throw(AipsError("FTMAchine::InitializeToSkyNew error imagestore has no valid grid initialized"));
    initializeToSky(*(imstore->backwardGrid()) , sumWeight , vb);

  };
  
  // Vectorized finalizeToSky
  void FTMachine::finalizeToSkyNew(Bool dopsf, 
				   const VisBuffer2& vb,
				   CountedPtr<SIImageStore> imstore  )				   
  {
    // Check vector lengths. 
    AlwaysAssert( imstore->getNTaylorTerms(false)==1, AipsError);

    Matrix<Float> sumWeights;
    finalizeToSky(); 

    //------------------------------------------------------------------------------------
    // Straightforward case. No extra primary beams. No image mosaic
    if(sj_p.nelements() == 0 ) 
      {
        // cerr << "TYPEID " << typeid( *(imstore->psf())).name() << "     " << typeid(typeid( *(imstore->psf())).name()).name() << endl;
        shared_ptr<ImageInterface<Float> > theim=dopsf ? imstore->psf() : imstore->residual();
        //static_cast<decltype(imstore->residual())>(theim)->lock();
        { LatticeLocker lock1 (*theim, FileLocker::Write);
          correlationToStokes( getImage(sumWeights, false) , *theim, dopsf);
        }
	theim->unlock();
        
	if( (useWeightImage() && dopsf) || isSD() ) {
          
          LatticeLocker lock1 (*(imstore->weight()), FileLocker::Write);
	  getWeightImage( *(imstore->weight())  , sumWeights);
          imstore->weight()->unlock();

	  // Fill weight image only once, during PSF generation. Remember.... it is normalized only once
	  // during PSF generation.
	}
	
	// Take sumWeights from corrToStokes here....
        LatticeLocker lock1 (*(imstore->sumwt()), FileLocker::Write);
        Matrix<Float> sumWeightStokes((imstore->sumwt())->shape()(2), (imstore->sumwt())->shape()(3));
        
        //  convertArray(sumWtComp, sumWeights);
        CoordinateSystem incoord=image->coordinates();
        CoordinateSystem outcoord=imstore->sumwt()->coordinates();
        StokesImageUtil::ToStokesSumWt(sumWeightStokes, sumWeights, outcoord, incoord);
        
        
        Array<Float> sumWtArr(IPosition(4,1,1,sumWeights.shape()[0], sumWeights.shape()[1]));
        
        IPosition blc(4, 0, 0, 0, 0);
         IPosition trc(4, 0, 0, sumWeightStokes.shape()[0]-1, sumWeightStokes.shape()[1]-1);
        sumWtArr(blc, trc).reform(sumWeightStokes.shape())=sumWeightStokes;
        
	//StokesImageUtil::ToStokesSumWt( sumWeightStokes, sumWeights );
	AlwaysAssert( ( (imstore->sumwt())->shape()[2] == sumWeightStokes.shape()[0] ) && 
		      ((imstore->sumwt())->shape()[3] == sumWeightStokes.shape()[1] ) , AipsError );

	(imstore->sumwt())->put( sumWeightStokes.reform((imstore->sumwt())->shape()) );
        imstore->sumwt()->unlock();
	
      }
    //------------------------------------------------------------------------------------
    // Image Mosaic only :  Multiply the residual, and weight image by the PB.
    else 
      {
      
      // Take the FT of the gridded values. Writes into backwardGrid(). 
      getImage(sumWeights, false);

      // Multiply complex image grid by PB.
      if( !dopsf )
	{
	  for (uInt k=0; k < sj_p.nelements(); ++k){
	    (sj_p(k))->apply(*(imstore->backwardGrid()), *(imstore->backwardGrid()), vb, 0, true);
	  }
	}

      // Convert from correlation to Stokes onto a new temporary grid.
      SubImage<Float>  targetImage( ( dopsf ? *(imstore->psf()) : *(imstore->residual()) ) , true);
      TempImage<Float> temp( targetImage.shape(), targetImage.coordinates() );
      correlationToStokes( *(imstore->backwardGrid()), temp, false);

      // Add the temporary Stokes image to the residual or PSF, whichever is being made.
      LatticeExpr<Float> addToRes( targetImage + temp );
      targetImage.copyData(addToRes);
      
      // Now, do the same with the weight image and sumwt ( only on the first pass )
      if( dopsf )
	{
	  SubImage<Float>  weightImage(  *(imstore->weight()) , true);
	  TempImage<Float> temp(weightImage.shape(), weightImage.coordinates());
	  getWeightImage(temp, sumWeights);

	  for (uInt k=0; k < sj_p.nelements(); ++k){
	    (sj_p(k))->applySquare(temp,temp, vb, -1);
	  }

	  LatticeExpr<Float> addToWgt( weightImage + temp );
	  weightImage.copyData(addToWgt);
	  
	  AlwaysAssert( ( (imstore->sumwt())->shape()[2] == sumWeights.shape()[0] ) && 
			((imstore->sumwt())->shape()[3] == sumWeights.shape()[1] ) , AipsError );

	  SubImage<Float>  sumwtImage(  *(imstore->sumwt()) , true);
	  TempImage<Float> temp2(sumwtImage.shape(), sumwtImage.coordinates());
	  temp2.put( sumWeights.reform(sumwtImage.shape()) );
	  LatticeExpr<Float> addToWgt2( sumwtImage + temp2 );
	  sumwtImage.copyData(addToWgt2);
	  
	  //cout << "In finalizeGridCoreMos : sumwt : " << sumwtImage.get() << endl;
	  
	}

    }
    //------------------------------------------------------------------------------------


    
    return;
  };

  Bool FTMachine::changedSkyJonesLogic(const vi::VisBuffer2& vb, Bool& firstRow, Bool& internalRow)
  {
    firstRow=false;
    internalRow=false;

    if( sj_p.nelements()==0 ) 
      {throw(AipsError("Internal Error : Checking changedSkyJones, but it is not yet set."));}

    CountedPtr<SkyJones> ej = sj_p[0];
    if(ej.null())
      return false;
    if(ej->changed(vb,0))
      firstRow=true;
    Int row2temp=0;
    if(ej->changedBuffer(vb,0,row2temp)) {
      internalRow=true;
    }
    return (firstRow || internalRow) ;
  }
  void FTMachine::setCFCache(CountedPtr<CFCache>& /*cfc*/, const Bool /*loadCFC*/) 
  {
    throw(AipsError("FTMachine::setCFCache() directly called!"));
  }
  

  
void FTMachine::findGridSector(const Int& nxp, const Int& nyp, const Int& ixsub, const Int& iysub, const Int& minx, const Int& miny, const Int& icounter, Int& x0, Int& y0, Int&  nxsub, Int& nysub, const Bool linear){
  /* Vector<Int> ord(36);
       ord(0)=14; 
      ord(1)=15;
      ord(2)=20;
      ord(3)=21;ord(4)=13;
      ord(5)=16;ord(6)=19;ord(7)=22;ord(8)=8;ord(9)=9;
      ord(10)=26;ord(11)=27;ord(12)=25;ord(13)=28;ord(14)=7;
      ord(15)=10;ord(16)=32;ord(17)=33;ord(18)=2;ord(19)=3;
      ord(20)=18;ord(21)=23;ord(22)=12;ord(23)=17;ord(24)=1;
      ord(25)=4;ord(26)=6;ord(27)=11;ord(28)=24;ord(29)=29;
      ord(30)=31;ord(31)=34;ord(32)=0;ord(33)=5;ord(34)=30;
      ord(35)=35;
      */
  /*
      Int ix= (icounter+1)-((icounter)/ixsub)*ixsub;
      Int iy=(icounter)/ixsub+1;
      y0=(nyp/iysub)*(iy-1)+1+miny;
      nysub=nyp/iysub;
      if( iy == iysub) {
	nysub=nyp-(nyp/iysub)*(iy-1);
      }
      x0=(nxp/ixsub)*(ix-1)+1+minx;
      nxsub=nxp/ixsub;
      if( ix == ixsub){
	nxsub=nxp-(nxp/ixsub)*(ix-1);
      }
  */
         
         
      {
	Int elrow=icounter/ixsub;
	Int elcol=(icounter-elrow*ixsub);
	//cerr << "row "<< elrow << " col " << elcol << endl; 
	//nxsub=Int(floor(((ceil(fabs(float(2*elcol+1-ixsub)/2.0))-1.0)*5 +1)*nxp/36.0 + 0.5));
	Float factor=0;
	if(ixsub > 1){
	  for (Int k=0; k < ixsub/2; ++k)
	    factor= linear ? factor+(k+1): factor+sqrt(Float(k+1));
	  //factor= linear ? factor+(k+1): factor+(k+1)*(k+1)*(k+1);
	  factor *= 2.0;
	  if(linear)
	    nxsub=Int(floor((ceil(fabs(float(2*elcol+1-ixsub)/2.0))/factor)*nxp + 0.5));
	  else
	    //nxsub=Int(floor((ceil(fabs(float(2*elcol+1-ixsub)/2.0))*ceil(fabs(float(2*elcol+1-ixsub)/2.0))*ceil(fabs(float(2*elcol+1-ixsub)/2.0))/factor)*nxp + 0.5));
	    nxsub=Int(floor((sqrt(ceil(fabs(float(2*elcol+1-ixsub)/2.0)))/factor)*nxp + 0.5));
	}
	else{
	  nxsub=nxp;
	}
        //cerr << nxp << " col " << elcol << " nxsub " << nxsub << endl;
	x0=minx;
	elcol-=1;
	while(elcol >= 0){
	  //x0+=Int(floor(((ceil(fabs(float(2*elcol+1-ixsub)/2.0))-1.0)*5 +1)*nxp/36.0+0.5));

	  if(linear)
	    x0+=Int(floor((ceil(fabs(float(2*elcol+1-ixsub)/2.0))/factor)*nxp + 0.5));
	  else
	    //x0+=Int(floor((ceil(fabs(float(2*elcol+1-ixsub)/2.0))*ceil(fabs(float(2*elcol+1-ixsub)/2.0))*ceil(fabs(float(2*elcol+1-ixsub)/2.0))/factor)*nxp + 0.5));
	    x0+=Int(floor((sqrt(ceil(fabs(float(2*elcol+1-ixsub)/2.0)))/factor)*nxp + 0.5));
	  elcol-=1;
	}
	factor=0;
	if(iysub >1){
	  for (Int k=0; k < iysub/2; ++k)
	    //factor=linear ? factor+(k+1): factor+(k+1)*(k+1)*(k+1);
	    factor= linear ? factor+(k+1): factor+sqrt(Float(k+1));
	  factor *= 2.0;
	  //nysub=Int(floor(((ceil(fabs(float(2*elrow+1-iysub)/2.0))-1.0)*5 +1)*nyp/36.0+0.5));
	  if(linear)
	    nysub=Int(floor((ceil(fabs(float(2*elrow+1-iysub)/2.0))/factor)*nyp + 0.5));
	  else
	    nysub=Int(floor((sqrt(ceil(fabs(float(2*elrow+1-iysub)/2.0)))/factor)*nyp + 0.5));
	}
	else{
	  nysub=nyp;
	}
	  //nysub=Int(floor((ceil(fabs(float(2*elrow+1-iysub)/2.0))*ceil(fabs(float(2*elrow+1-iysub)/2.0))*ceil(fabs(float(2*elrow+1-iysub)/2.0))/factor)*nyp + 0.5));
	y0=miny;
	elrow-=1;
	
	while(elrow >=0){
	  //y0+=Int(floor(((ceil(fabs(float(2*elrow+1-iysub)/2.0))-1.0)*5 +1)*nyp/36.0+0.5));
	  if(linear)
	    y0+=Int(floor((ceil(fabs(float(2*elrow+1-iysub)/2.0))/factor)*nyp + 0.5));
	  else
	    y0+=Int(floor((sqrt(ceil(fabs(float(2*elrow+1-iysub)/2.0)))/factor)*nyp + 0.5));
	    //y0+=Int(floor((ceil(fabs(float(2*elrow+1-iysub)/2.0))*ceil(fabs(float(2*elrow+1-iysub)/2.0))*ceil(fabs(float(2*elrow+1-iysub)/2.0))/factor)*nyp + 0.5));
	  elrow-=1;
	}
      }


      y0+=1;
      x0+=1;
      //cerr << icounter << " x0, y0 " << x0 << "  " << y0 << "  ixsub, iysub " <<  nxsub << "   " << nysub << endl;
      if(doneThreadPartition_p < 0)
	doneThreadPartition_p=1;
   
}

  void FTMachine::tweakGridSector(const Int& nx, const Int& ny, const Int& ixsub, const Int& iysub){
    //if(doneThreadPartition_p)
    //  return;
    Vector<Int> x0, y0, nxsub, nysub;
    Vector<Float> xcut(nx/2);
    Vector<Float> ycut(ny/2);
    if(griddedData2.nelements() >0 ){
      //cerr << "shapes " << xcut.shape() << "   gd " << amplitude(griddedData2(IPosition(4, 0, ny/2-1, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0))).shape() << endl;
      convertArray(xcut, amplitude(griddedData2(IPosition(4, 0, ny/2-1, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0))).reform(xcut.shape()));
      convertArray(ycut, amplitude(griddedData2(IPosition(4, nx/2-1, 0, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0))).reform(ycut.shape()));
    }
    else{
      xcut=amplitude(griddedData(IPosition(4, 0, ny/2-1, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0)));
      ycut=amplitude(griddedData(IPosition(4, nx/2-1, 0, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0)));
    }
    //cerr << griddedData2.shape() << "   " << griddedData.shape() << endl;
    Vector<Float> cumSumX(nx/2, 0);
    //Vector<Float> cumSumX2(nx/2,0);
    cumSumX(0)=xcut(0);
    //cumSumX2(0)=cumSumX(0)*cumSumX(0);
    Float sumX=sum(xcut);
    if(sumX==0.0)
      return;
    //cerr << "sumX " << sumX << endl;
    //sumX *= sumX;
    x0.resize(ixsub);
    x0=nx/2-1;
    nxsub.resize(ixsub);
    nxsub=0;
    x0(0)=0;
    Int counter=1;
    for (Int k=1; k < nx/2; ++k){
      cumSumX(k)=cumSumX(k-1)+xcut(k);
      //cumSumX2(k)=cumSumX(k)*cumSumX(k);
      Float nextEdge=sumX/(Float(ixsub/2)*Float(ixsub/2))*Float(counter)*Float(counter);
      if(cumSumX(k-1) < nextEdge && cumSumX(k) >= nextEdge){
	x0(counter)=k;
	//cerr << counter << "    "   << k << " diff " << x0(counter)-x0(counter-1) << endl;
	nxsub(counter-1)=x0(counter)-x0(counter-1);
	++counter;
      }
    } 
    
    x0(ixsub/2)=nx/2-1;
    nxsub(ixsub/2)=nxsub(ixsub/2-1)=nx/2-1-x0(ixsub/2-1);
    for(Int k=ixsub/2+1; k < ixsub; ++k){
      x0(k)=x0(k-1)+ nxsub(ixsub-k);
      nxsub(k)=nxsub(ixsub-k-1);
    }
    nxsub(ixsub-1)+=1;
    
    Vector<Float> cumSumY(ny/2, 0);
    //Vector<Float> cumSumY2(ny/2,0);
    cumSumY(0)=ycut(0);
    //cumSumY2(0)=cumSumY(0)*cumSumY(0);
    Float sumY=sum(ycut);
    if(sumY==0.0)
      return;
    //sumY *=sumY;
    y0.resize(iysub);
    y0=ny/2-1;
    nysub.resize(iysub);
    nysub=0;
    y0(0)=0;
    counter=1;
    for (Int k=1; k < ny/2; ++k){
      cumSumY(k)=cumSumY(k-1)+ycut(k);
      //cumSumY2(k)=cumSumY(k)*cumSumY(k);
      Float nextEdge=sumY/(Float(iysub/2)*Float(iysub/2))*Float(counter)*Float(counter);
      if(cumSumY(k-1) < nextEdge && cumSumY(k) >= nextEdge){
	y0(counter)=k;
	nysub(counter-1)=y0(counter)-y0(counter-1);
	++counter;
      }
    } 
    
    y0(ixsub/2)=ny/2-1;
    nysub(iysub/2)=nysub(iysub/2-1)=ny/2-1-y0(iysub/2-1);
    for(Int k=iysub/2+1; k < iysub; ++k){
      y0(k)=y0(k-1)+ nysub(iysub-k);
      nysub(k)=nysub(iysub-k-1);
    }
    nysub(iysub-1)+=1;
    
    if(anyEQ(nxsub, 0) || anyEQ(nysub, 0))
      return;
    //cerr << " x0 " << x0 << "  nxsub " << nxsub << endl;
    //cerr << " y0 " << y0 << "  nysub " << nysub << endl;
    x0+=1;
    y0+=1;
    xsect_p.resize(ixsub*iysub);
    ysect_p.resize(ixsub*iysub);
    nxsect_p.resize(ixsub*iysub);
    nysect_p.resize(ixsub*iysub);
    for (Int iy=0; iy < iysub; ++iy){
      for (Int ix=0; ix< ixsub; ++ix){
	
	xsect_p(iy*ixsub+ix)=x0[ix];
	ysect_p(iy*ixsub+ix)=y0[iy];
	nxsect_p(iy*ixsub+ix)=nxsub[ix];
	nysect_p(iy*ixsub+ix)=nysub[iy];
      }
    }

    ++doneThreadPartition_p;

  }
 

  /*
  /// Move to individual FTMs............ make it pure virtual.
  Bool FTMachine::useWeightImage()
  {
    if( name() == "GridFT" || name() == "WProjectFT" )  
      { return false; }
    else
      { return true; }
  }
  */

  }//# namespace refim ends
}//namespace CASA ends

