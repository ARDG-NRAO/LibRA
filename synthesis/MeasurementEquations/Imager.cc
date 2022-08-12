//# Imager.cc: Implementation of Imager.h
//# Copyright (C) 1997-2008
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/iostream.h>
#include <synthesis/MeasurementEquations/Imager.h>
#include <synthesis/MeasurementComponents/EPJones.h>
#include <synthesis/TransformMachines/VisModelData.h>

#include <casacore/ms/MeasurementSets/MSHistoryHandler.h>

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>

#include <casacore/casa/Logging.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogMessage.h>

#include <casacore/casa/OS/DirectoryIterator.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>

#include <casacore/casa/OS/HostInfo.h>
#include <casacore/tables/Tables/RefRows.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/TableUtil.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/tables/Tables/TableLock.h>
#include <casacore/tables/TaQL/ExprNode.h>

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/Fallible.h>
#include <casacore/casa/Utilities/CompositeNumber.h>

#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/Utilities/Regex.h>

#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/images/Images/ImageExpr.h>
#include <imageanalysis/ImageAnalysis/ImagePolarimetry.h>
#include <synthesis/MeasurementEquations/ClarkCleanProgress.h>
#include <casacore/lattices/LatticeMath/LatticeCleanProgress.h>
#include <msvis/MSVis/MSUtil.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <msvis/MSVis/VisImagingWeight.h>
/////////#include <msvis/MSVis/VisBufferAsync.h>

// Disabling Imager::correct() (gmoellen 06Nov20)
//#include <synthesis/MeasurementComponents/TimeVarVisJones.h>

#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/Quanta/UnitMap.h>
#include <casacore/casa/Quanta/UnitVal.h>
#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/casa/Quanta/MVEpoch.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/Measures/MeasTable.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/ms/MSSel/MSSelectionTools.h>
#include <casacore/ms/MSSel/MSDataDescIndex.h>
#include <casacore/ms/MeasurementSets/MSDopplerUtil.h>
#include <casacore/ms/MSSel/MSSourceIndex.h>
#include <casacore/ms/MSOper/MSSummary.h>
#include <synthesis/MeasurementEquations/CubeSkyEquation.h>
#include <synthesis/MeasurementEquations/Feather.h>
#include <synthesis/MeasurementComponents/ImageSkyModel.h>
#include <synthesis/MeasurementComponents/CEMemImageSkyModel.h>
#include <synthesis/MeasurementComponents/MFCEMemImageSkyModel.h>
#include <synthesis/MeasurementComponents/MFCleanImageSkyModel.h>
#include <synthesis/MeasurementComponents/CSCleanImageSkyModel.h>
#include <synthesis/MeasurementComponents/MFMSCleanImageSkyModel.h>
#include <synthesis/MeasurementComponents/HogbomCleanImageSkyModel.h>
#include <synthesis/MeasurementComponents/MSCleanImageSkyModel.h>
#include <synthesis/MeasurementComponents/NNLSImageSkyModel.h>
#include <synthesis/MeasurementComponents/WBCleanImageSkyModel.h>
#include <synthesis/MeasurementComponents/GridBoth.h>
#include <synthesis/TransformMachines/SetJyGridFT.h>
#include <synthesis/TransformMachines/MosaicFT.h>
#include <synthesis/TransformMachines/WProjectFT.h>
#include <synthesis/MeasurementComponents/nPBWProjectFT.h>
#include <synthesis/MeasurementComponents/PBMosaicFT.h>
#include <synthesis/TransformMachines/PBMath.h>
#include <synthesis/TransformMachines/SimpleComponentFTMachine.h>
#include <synthesis/TransformMachines/VPSkyJones.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines/HetArrayConvFunc.h>

#include <synthesis/DataSampling/SynDataSampling.h>
#include <synthesis/DataSampling/SDDataSampling.h>
#include <synthesis/DataSampling/ImageDataSampling.h>

#include <casacore/lattices/LRegions/LattRegionHolder.h>
#include <casacore/lattices/Lattices/TiledLineStepper.h> 
#include <casacore/lattices/Lattices/LatticeIterator.h> 
#include <casacore/lattices/LEL/LatticeExpr.h> 
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/lattices/LRegions/LCEllipsoid.h>
#include <casacore/lattices/LRegions/LCRegion.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/lattices/LRegions/LCIntersection.h>
#include <casacore/lattices/LRegions/LCUnion.h>
#include <casacore/lattices/LRegions/LCExtension.h>

#include <casacore/images/Images/ImageRegrid.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Regions/RegionManager.h>
#include <casacore/images/Regions/WCBox.h>
#include <casacore/images/Regions/WCUnion.h>
#include <casacore/images/Regions/WCIntersection.h>
#include <synthesis/TransformMachines/PBMath.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/ImageInfo.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/coordinates/Coordinates/ObsInfo.h>

#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/ConstantSpectrum.h>
#include <components/ComponentModels/SpectralIndex.h>
#include <components/ComponentModels/TabularSpectrum.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/FluxStandard.h>
#include <components/ComponentModels/PointShape.h>
#include <components/ComponentModels/DiskShape.h>

#ifdef USE_GRPC
#include <synthesis/ImagerObjects/grpcInteractiveClean.h>
#endif

#include <casacore/casa/OS/HostInfo.h>

#include <components/ComponentModels/ComponentList.h>

#include <casacore/measures/Measures/UVWMachine.h>

#include <casacore/casa/sstream.h>

#include <sys/types.h>
#include <unistd.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

Imager::Imager() 
  :  msname_p(""), vs_p(0), rvi_p(0), wvi_p(0), ft_p(0), 
     cft_p(0), se_p(0),
     sm_p(0), vp_p(0), gvp_p(0), setimaged_p(false), nullSelect_p(false), 
     mssFreqSel_p(), mssChanSel_p(),
     clean_panel_p(0), image_id_p(0), mask_id_p(0), 
      prev_image_id_p(0), prev_mask_id_p(0), projection_p("SIN")
{
  ms_p=0;
  mssel_p=0;
  lockCounter_p=0;
  numMS_p=0;
  defaults();
};


void Imager::defaults() 
{

  setimaged_p=false;
  nullSelect_p=false;
  nx_p=128; ny_p=128; facets_p=1;
  wprojPlanes_p=-1;
  mcellx_p=Quantity(1, "arcsec"); mcelly_p=Quantity(1, "arcsec");
  shiftx_p=Quantity(0.0, "arcsec"); shifty_p=Quantity(0.0, "arcsec");
  distance_p=Quantity(0.0, "m");
  stokes_p="I"; npol_p=1;
  nscales_p=5;
  ntaylor_p=1;
  reffreq_p=0.0;
  useNewMTFT_p=false;
  scaleMethod_p="nscales";  
  scaleInfoValid_p=false;
  dataMode_p="none";
  imageMode_p="MFS";
  dataNchan_p=0;
  imageNchan_p=0;
  doVP_p=false;
  doDefaultVP_p = true;
  parAngleInc_p=Quantity(360.,"deg");
  skyPosThreshold_p=Quantity(180.,"deg");
  telescope_p="";
  gridfunction_p="SF";
  minWeight_p=0.;
  clipminmax_p=false;
  doMultiFields_p=false;
  doWideBand_p=false;
  multiFields_p=false;
  // Use half the machine memory as cache. The user can override
  // this via the setoptions function().
  cache_p=(HostInfo::memoryTotal(true)/8)*1024;
  //On 32 bit machines with more than 2G of mem this can become negative
  // overriding it to 2 Gb.
  if(cache_p <=0 )
    cache_p=2000000000/8;
  tile_p=16;
  ftmachine_p="ft";
  wfGridding_p=false;
  padding_p=1.2;
  sdScale_p=1.0;
  sdWeight_p=1.0;
  sdConvSupport_p=-1;

  doShift_p=false;
  spectralwindowids_p.resize(1); 
  spectralwindowids_p=0;
  fieldid_p=0;
  dataspectralwindowids_p.resize(0); 
  datadescids_p.resize(0);
  datafieldids_p.resize(0);
  mImageStart_p=MRadialVelocity(Quantity(0.0, "km/s"), MRadialVelocity::LSRK);
  mImageStep_p=MRadialVelocity(Quantity(0.0, "km/s"), MRadialVelocity::LSRK);
  mDataStart_p=MRadialVelocity(Quantity(0.0, "km/s"), MRadialVelocity::LSRK);
  mDataStep_p=MRadialVelocity(Quantity(0.0, "km/s"), MRadialVelocity::LSRK);
  beamValid_p=false;
  beam_p = ImageBeamSet();
  images_p.resize(0);
  masks_p.resize(0);
  fluxMasks_p.resize(0);
  residuals_p.resize(0);
  componentList_p=0;

  cyclefactor_p = 1.5;
  cyclespeedup_p =  -1;
  cyclemaxpsffraction_p = 0.8;
  stoplargenegatives_p = 2;
  stoppointmode_p = -1;
  fluxscale_p.resize(0);
  scaleType_p = "NONE";
  minPB_p = 0.1;
  constPB_p = 0.4;
  redoSkyModel_p=true;
  nmodels_p=0;
  useModelCol_p=false;
  freqFrameValid_p=false;
  doTrackSource_p=false;
  freqInterpMethod_p="nearest";
  pointingDirCol_p="DIRECTION";
  logSink_p=LogSink(LogMessage::NORMAL, false);
  imwgt_p=VisImagingWeight();
  smallScaleBias_p=0.6;
  freqFrame_p=MFrequency::LSRK;
  imageTileVol_p=0;
  singlePrec_p=false;
  spwchansels_p.resize();
  flatnoise_p=true;
  freqrange_p.resize();
  numthreads_p=-1;
  avoidTempLatt_p=false;
  mssFreqSel_p.resize();
  mssChanSel_p.resize();
  projection_p=String("SIN");

}


Imager::Imager(MeasurementSet& theMS,  Bool compress, Bool useModel)
  : msname_p(""), vs_p(0), rvi_p(0), wvi_p(0), 
    ft_p(0), cft_p(0), se_p(0),
    sm_p(0), vp_p(0), gvp_p(0), setimaged_p(false), nullSelect_p(false), 
    mssFreqSel_p(), mssChanSel_p(),
    clean_panel_p(0), image_id_p(0), mask_id_p(0), prev_image_id_p(0), prev_mask_id_p(0),
    projection_p("SIN")

{

  mssel_p=0;
  ms_p=0;
  lockCounter_p=0;
  LogIO os(LogOrigin("Imager", "Imager(MeasurementSet &theMS)", WHERE));
  if(!open(theMS, compress, useModel)) {
    os << LogIO::SEVERE << "Open of MeasurementSet failed" << LogIO::EXCEPTION;
  };

  numMS_p=1;
  defaults();
  latestObsInfo_p=ObsInfo();
}



Imager::Imager(MeasurementSet& theMS, Bool compress)
  :  msname_p(""),  vs_p(0), rvi_p(0), wvi_p(0), ft_p(0), cft_p(0), se_p(0),
     sm_p(0), vp_p(0), gvp_p(0), setimaged_p(false), nullSelect_p(false), 
     mssFreqSel_p(), mssChanSel_p(),
     clean_panel_p(0), image_id_p(0), mask_id_p(0),
     prev_image_id_p(0), prev_mask_id_p(0), projection_p("SIN")
{
  mssel_p=0;
  ms_p=0;
  lockCounter_p=0;
  LogIO os(LogOrigin("Imager", "Imager(MeasurementSet &theMS)", WHERE));
  if(!open(theMS, compress)) {
    os << LogIO::SEVERE << "Open of MeasurementSet failed" << LogIO::EXCEPTION;
  };

  numMS_p=1;
  defaults();

  latestObsInfo_p=ObsInfo();
}

Imager::Imager(const Imager & other)
  :  msname_p(""), vs_p(0), rvi_p(0), wvi_p(0), 
     ft_p(0), cft_p(0), se_p(0),
     sm_p(0), vp_p(0), gvp_p(0), setimaged_p(false), nullSelect_p(false), 
     clean_panel_p(0), image_id_p(0), mask_id_p(0), prev_image_id_p(0), prev_mask_id_p(0)
{
  mssel_p=0;
  ms_p=0;
  operator=(other);
}

Imager &Imager::operator=(const Imager & other)
{
  if (!ms_p.null() && this != &other) {
    *ms_p = *(other.ms_p);
  }
  if(this != &other){
    //Equating the table and ms parameters
    antab_p=other.antab_p;
    datadesctab_p=other.datadesctab_p;
    feedtab_p=other.feedtab_p;
    fieldtab_p=other.fieldtab_p;
    obstab_p=other.obstab_p;
    pointingtab_p=other.pointingtab_p;
    poltab_p=other.poltab_p;
    proctab_p=other.proctab_p;
    spwtab_p=other.spwtab_p;
    statetab_p=other.statetab_p;
    latestObsInfo_p=other.latestObsInfo_p;
    parAngleInc_p=other.parAngleInc_p;
    skyPosThreshold_p=other.skyPosThreshold_p;
    doTrackSource_p=other.doTrackSource_p;
    trackDir_p=other.trackDir_p;
    smallScaleBias_p=other.smallScaleBias_p;
    numMS_p=other.numMS_p;
    if (!mssel_p.null() && this != &other) {
      *mssel_p = *(other.mssel_p);
    }
    if (vs_p && this != &other) {
      *vs_p = *(other.vs_p);
    }
    if (wvi_p && this != &other) {
      *wvi_p = *(other.wvi_p);
      rvi_p=wvi_p;
    }
    else if(rvi_p && this != &other){
      *rvi_p = *(other.rvi_p);
      wvi_p=NULL;
    }
    if (ft_p && this != &other) {
      *ft_p = *(other.ft_p);
    }
    if (cft_p && this != &other) {
      *cft_p = *(other.cft_p);
    }
    if (se_p && this != &other) {
      *se_p = *(other.se_p);
    }
    if (sm_p && this != &other) {
      *sm_p = *(other.sm_p);
    }
    if (vp_p && this != &other) {
      *vp_p = *(other.vp_p);
    }
    if (gvp_p && this != &other) {
      *gvp_p = *(other.gvp_p);
    }
    imageTileVol_p=other.imageTileVol_p;
    flatnoise_p=other.flatnoise_p;
    mssFreqSel_p.assign(other.mssFreqSel_p);
    mssChanSel_p.assign(other.mssChanSel_p);
    projection_p = other.projection_p;
  }
  return *this;
}

Imager::~Imager()
{
  try{
    destroySkyEquation();
    this->unlock(); //unlock things if they are in a locked state
    
    //if (mssel_p) {
    //  delete mssel_p;
    // }
    mssel_p = 0;
    //if (ms_p) {
    //  delete ms_p;
    //}
    ms_p = 0;
    if (vs_p) {
      delete vs_p;
    }
    if(rvi_p)
      delete rvi_p;
    rvi_p=wvi_p=0;

    vs_p = 0;
    if (ft_p) {
      delete ft_p;
    }
    
    
    ft_p = 0;
    if (cft_p) {
      delete cft_p;
    }
    cft_p = 0;

  }
  catch (AipsError x){
    String mess=x.getMesg();
    //This is a bug for wproject and facet together...
    //somebody is erasing a TempLattice before desturctor.
    //will keep this in place till i figure it out...its benign
    if(mess.contains("does not exist") && mess.contains("TempLattice")){
      String rootpath="/"+String(mess.after("/")).before("TempLattice");
      String pid=String(mess.after("TempLattice")).before("_");
      DirectoryIterator dir(rootpath, Regex(Regex::fromPattern("TempLattice"+pid+"*")));
      while(!dir.pastEnd()){
	  Directory ledir(rootpath+"/"+dir.name());
	  ledir.removeRecursive();
	  dir++;
	  
      }

    }
    else{
      throw(AipsError(x));

    }

  }


}


Bool Imager::open(MeasurementSet& theMs, Bool /*compress*/, Bool useModelCol)
{

  LogIO os(LogOrigin("Imager", "open()", WHERE));
  
  if (!ms_p.null()) {
    *ms_p = theMs;
  } else {
    ms_p = new MeasurementSet(theMs);
    AlwaysAssert(!ms_p.null(), AipsError);
  }
  

  try {
    this->openSubTables();
    this->lock();
    msname_p = ms_p->tableName();
    
    os << "Opening MeasurementSet " << msname_p << LogIO::POST;

    // Check for DATA or FLOAT_DATA column
    if(!ms_p->tableDesc().isColumn("DATA") && 
       !ms_p->tableDesc().isColumn("FLOAT_DATA")) {
      ms_p->unlock();
      //delete ms_p; 
      ms_p=0;
      os << LogIO::SEVERE
	 << "Missing DATA or FLOAT_DATA column: imager cannot be run"
	 << LogIO::EXCEPTION;
      return false;
    }
    
    // The unused return value creates a compiler warning, there shouldn't be any side-effect of this statement
    //(!ms_p->tableDesc().isColumn("CORRECTED_DATA")); // if no side effect then delete this statement?
    
    /*if(vs_p) {
      delete vs_p; vs_p=0;
    }
    */
    if(rvi_p){
      delete rvi_p;
      rvi_p=0;
      wvi_p=0;
    }
    
    // Now open the selected MeasurementSet to be initially the
    // same as the original MeasurementSet

    mssel_p=new MeasurementSet(*ms_p);
    useModelCol_p=useModelCol;
    
    // Now create the VisSet
    this->makeVisSet(*mssel_p);
    AlwaysAssert(rvi_p, AipsError);
    
    // Polarization
    MSColumns msc(*mssel_p);
    Vector<String> polType=msc.feed().polarizationType()(0);
    if (polType(0)!="X" && polType(0)!="Y" &&
	polType(0)!="R" && polType(0)!="L") {
      this->unlock();
      os << LogIO::SEVERE << "Warning: Unknown stokes types in feed table: "
	 << polType(0) << endl
	 << "Results open to question!" << LogIO::POST;
    }
    numMS_p=1;

    this->unlock();

    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Caught Exception: "<< x.getMesg() << LogIO::EXCEPTION;

    return false;
  } 

  return true;
}

Bool Imager::close()
{
  if(!valid()) return false;
  if (detached()) return true;
  LogIO os(LogOrigin("imager", "close()", WHERE));
  os << LogIO::NORMAL // Loglevel PROGRESS
     << "Closing MeasurementSet and detaching from imager"
     << LogIO::POST;
  this->unlock();
  if(ft_p) delete ft_p; ft_p = 0;
  if(cft_p) delete cft_p; cft_p = 0;
  if(vs_p) delete vs_p; vs_p = 0;
  if(rvi_p) delete rvi_p; 
  rvi_p=0;
  wvi_p=0;
  //if(mssel_p) delete mssel_p; 
  mssel_p = 0;
  //if(ms_p) delete ms_p; 
  ms_p = 0;

  if(se_p) delete se_p; se_p = 0;

  if(vp_p) delete vp_p; vp_p = 0;
  if(gvp_p) delete gvp_p; gvp_p = 0;

  destroySkyEquation();

  return true;
}

String Imager::name() const
{
  if (detached()) {
    return "none";
  }
  return msname_p;
}




IPosition Imager::imageshape() const
{
  return IPosition(4, nx_p, ny_p, npol_p, imageNchan_p);
}

Bool Imager::summary() 
{
  if(!valid()) return false;
  LogOrigin OR("imager", "Imager::summary()", WHERE);
  
  LogIO los(OR);
  
  los << "Logging summary" << LogIO::POST;
  try {
    
    this->lock();
    MSSummary mss(*ms_p);
    mss.list(los, true);
    
    los << endl << state() << LogIO::POST;
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    los << LogIO::SEVERE << "Caught Exception: " << x.getMesg()
	<< LogIO::EXCEPTION;
    return false;
  } 
  
  return true;
}


Bool Imager::setimage(const Int nx, const Int ny,
		      const Quantity& cellx, const Quantity& celly,
		      const String& stokes,
		      Bool doShift,
		      const MDirection& phaseCenter, 
		      const Quantity& shiftx, const Quantity& shifty,
		      const String& mode, const Int nchan,
		      const Int start, const Int step,
		      const MRadialVelocity& mStart, const MRadialVelocity& mStep,
		      const Vector<Int>& spectralwindowids,
		      const Int fieldid,
		      const Int facets,
		      const Quantity& distance)
{

  if(!valid())
    {
      return false;
    }

  //Clear the sink 
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "setimage()"), logSink_p);

  os << "nx=" << nx << " ny=" << ny
     << " cellx='" << cellx.getValue() << cellx.getUnit()
     << "' celly='" << celly.getValue() << celly.getUnit()
     << "' stokes=" << stokes << " doShift=" << doShift
     << " shiftx='" << shiftx.getValue() << shiftx.getUnit()
     << "' shifty='" << shifty.getValue() << shifty.getUnit()
     << "' mode=" << mode << " nchan=" << nchan
     << " start=" << start << " step=" << step
     << " spwids=" << spectralwindowids
     << " fieldid=" <<   fieldid << " facets=" << facets
     << " distance='" << distance.getValue() << distance.getUnit() <<"'";
  ostringstream clicom;
  clicom << " phaseCenter='" << phaseCenter;
  clicom << "' mStart='" << mStart << "' mStep='" << mStep << "'";
  os << String(clicom);
  
  try {
    
    this->lock();
    this->writeCommand(os);

    os << LogIO::NORMAL << "Defining image properties" << LogIO::POST; // Loglevel INFO
  
    /**** this check is not really needed here especially for SD imaging
    if(2*Int(nx/2)!=nx) {
      this->unlock();
      os << LogIO::SEVERE << "nx must be even" << LogIO::POST;
      return false;
    }
    if(2*Int(ny/2)!=ny) {
      this->unlock();
      os << LogIO::SEVERE << "ny must be even" << LogIO::POST;
      return false;
    }

    */
    {
      CompositeNumber cn(nx);
      if (! cn.isComposite(nx)) {
	Int nxc = (Int)cn.nextLargerEven(nx);
	Int nnxc = (Int)cn.nearestEven(nx);
	if (nxc == nnxc) {
	  os << LogIO::WARN << "nx = " << nx << " is not composite; nx = " 
	     << nxc << " will be more efficient" << LogIO::POST;
	} else {
	  os <<  LogIO::WARN << "nx = " << nx << " is not composite; nx = " 
	     << nxc <<  " or " << nnxc << " will be more efficient for FFTs" << LogIO::POST;
	}
      }
      if (! cn.isComposite(ny)) {
	Int nyc = (Int)cn.nextLargerEven(ny);
	Int nnyc = (Int)cn.nearestEven(ny);
	if (nyc == nnyc) {
	  os <<  LogIO::WARN << "ny = " << ny << " is not composite; ny = " 
	     << nyc << " will be more efficient" << LogIO::POST;
	} else {
	  os <<  LogIO::WARN << "ny = " << ny << " is not composite ; ny = " << nyc << 
	      " or " << nnyc << " will be more efficient for FFTs" << LogIO::POST;
	}
	os << LogIO::WARN 
	   << "You may safely ignore this message for single dish imaging" 
	   << LogIO::POST;

      }
      
    }

  
    nx_p=nx;
    ny_p=ny;
    mcellx_p=cellx;
    mcelly_p=celly;
    distance_p=distance;
    stokes_p=stokes;
    imageMode_p=mode;
    imageMode_p.upcase();
    imageNchan_p=nchan;
    imageStart_p=start;
    imageStep_p=step;
    mImageStart_p=mStart;
    mImageStep_p=mStep;
    spectralwindowids_p.resize(spectralwindowids.nelements());
    spectralwindowids_p=spectralwindowids;
    fieldid_p=fieldid;
    facets_p=facets;
    redoSkyModel_p=true;
    destroySkyEquation();    

     Vector<Int> whichStokes = decideNPolPlanes(false);
     if( whichStokes.nelements()==1 && whichStokes[0]==0 )
      {
      this->unlock();
      os << LogIO::SEVERE << "Stokes selection " << stokes_p << " is currently not supported." << LogIO::EXCEPTION;
      return false;
      }

    

    //THIS NEEDS TO GO
    ////this->setImageParam(nx_p, ny_p, npol_p, imageNchan_p);
    nchan_p = imageNchan_p; // check if this is needed.... it's the only non-redundant thing in setImageParam.

    // Now do the shifts
    //    MSColumns msc(*ms_p);

    doShift_p=doShift;
    if(doShift_p) {
      phaseCenter_p=phaseCenter;
    }
    else {

      MSFieldColumns msfield(ms_p->field());
      phaseCenter_p=msfield.phaseDirMeas(fieldid_p);
      //    phaseCenter_p=msc.field().phaseDirMeas(fieldid_p);
    }
    
    // Now add the optional shifts
    shiftx_p=shiftx;
    shifty_p=shifty;
    if(shiftx_p.get().getValue()!=0.0||shifty_p.get().getValue()!=0.0) {
      Vector<Double> vPhaseCenter(phaseCenter_p.getAngle().getValue());
      if(cos(vPhaseCenter(1))!=0.0) {
	vPhaseCenter(0)+=shiftx_p.get().getValue()/cos(vPhaseCenter(1));
      }
      vPhaseCenter(1)+=shifty_p.get().getValue();
      phaseCenter_p.set(MVDirection(vPhaseCenter));
    }
    
    // Now we have set the image parameters
    setimaged_p=true;
    beamValid_p=false;
    
    this->unlock();

    return true;
  } catch (AipsError x) {
    
    this->unlock();

    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  } 

  return true;
}


Bool Imager::defineImage(const Int nx, const Int ny,
			 const Quantity& cellx, const Quantity& celly,
			 const String& stokes,
			 const MDirection& phaseCenter, const Int fieldid,
			 const String& mode, const Int nchan,
			 const Int start, const Int step,
			 const MFrequency& mFreqStart,
			 const MRadialVelocity& mStart, 
			 const Quantity& qStep,
			 const Vector<Int>& spectralwindowids,
			 const Int facets,
			 const Quantity& restFreq,
                         const MFrequency::Types& mFreqFrame,
			 const Quantity& distance, const Bool dotrackDir, 
			 const MDirection& trackDir,
			 const String& projection)
{




  //Clear the sink 
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "defineimage()"), logSink_p);
  if(cellx.getValue() == 0.0  || celly.getValue()==0.0)
	 throw(AipsError("Infinite resolution not possible... please do let us know what you are drinking"));
  os << LogIO::NORMAL << "Defining image properties:"; // Loglevel INFO
  os << "nx=" << nx << " ny=" << ny
     << " cellx='" << cellx.getValue() << cellx.getUnit()
     << "' celly='" << celly.getValue() << celly.getUnit()
     << "' stokes=" << stokes 
     << "' mode=" << mode << " nchan=" << nchan
     << " start=" << start << " step=" << step
     << " spwids=" << spectralwindowids
     << " fieldid=" <<   fieldid << " facets=" << facets
     << " frame=" << mFreqFrame 
     << " distance='" << distance.getValue() << distance.getUnit() <<"'";
  os << LogIO::POST;
  ostringstream clicom;
  clicom << " phaseCenter='"  ;
  if(fieldid < 0){
    MVAngle mvRA=phaseCenter.getAngle().getValue()(0);
    MVAngle mvDEC=phaseCenter.getAngle().getValue()(1);
    clicom << mvRA(0.0).string(MVAngle::TIME,8) << ", ";
    clicom << mvDEC(0.0).string(MVAngle::ANGLE_CLEAN,8) << ", ";
  }
  else{
    clicom << "field-" << fieldid<< " "; 
  }
  clicom << "' mStart='" << mStart << "' qStep='" << qStep << "'";
  clicom << "' mFreqStart='" << mFreqStart;
  os << String(clicom);
  os << LogIO::POST;
  
  try {
    
    this->lock();
    this->writeCommand(os);
  
    doTrackSource_p=dotrackDir;
    trackDir_p=trackDir;


    /**** this check is not really needed here especially for SD imaging
    if(2*Int(nx/2)!=nx) {
      this->unlock();
      os << LogIO::SEVERE << "nx must be even" << LogIO::POST;
      return false;
    }
    if(2*Int(ny/2)!=ny) {
      this->unlock();
      os << LogIO::SEVERE << "ny must be even" << LogIO::POST;
      return false;
    }

    */
    // RVU : Disabling this Composite Number check, because image sizes are
    //          anyway being re-calculated inside the FTMachines (accounding for padding too).
    if(0)
    {
      CompositeNumber cn(nx);
      if (! cn.isComposite(nx)) {
	Int nxc = (Int)cn.nextLargerEven(nx);
	Int nnxc = (Int)cn.nearestEven(nx);
	if (nxc == nnxc) {
	  os << LogIO::POST << "nx = " << nx << " is not composite; nx = " 
	     << nxc << " will be more efficient" << LogIO::POST;
	} else {
	  os <<  LogIO::POST << "nx = " << nx << " is not composite; nx = " 
	     << nxc <<  " or " << nnxc << " will be more efficient" << LogIO::POST;
	}
      }
      if (! cn.isComposite(ny)) {
	Int nyc = (Int)cn.nextLargerEven(ny);
	Int nnyc = (Int)cn.nearestEven(ny);
	if (nyc == nnyc) {
	  os <<  LogIO::POST << "ny = " << ny << " is not composite; ny = " 
	     << nyc << " will be more efficient" << LogIO::POST;
	} else {
	  os <<  LogIO::POST << "ny = " << ny << " is not composite; ny = " << nyc << 
	      " or " << nnyc << " will be more efficient" << LogIO::POST;
	}
	os << LogIO::POST
	   << "You may safely ignore this message for single dish imaging" 
	   << LogIO::POST;

      }
      
    }


    if((abs(Double(nx)*cellx.getValue("rad")) > C::pi) || (abs(Double(ny)*celly.getValue("rad")) > C::pi))
      throw(AipsError("Cannot image the extent requested for this image;  more that PI ialong one or both of the axes " ));
    


    nx_p=nx;
    ny_p=ny;
    mcellx_p=cellx;
    mcelly_p=celly;
    distance_p=distance;
    stokes_p=stokes;
    imageMode_p=mode;
    imageMode_p.upcase();
    imageNchan_p=nchan;
    imageStart_p=start;
    imageStep_p=step;
    if(mode.contains("VEL")){
      mImageStart_p=mStart;
      mImageStep_p=MRadialVelocity(qStep);
    }
    if(mode.contains("FREQ")){
      mfImageStart_p=mFreqStart;
      mfImageStep_p=MFrequency(qStep);
    }
    restFreq_p=restFreq;
    freqFrame_p=mFreqFrame;
    spectralwindowids_p.resize(spectralwindowids.nelements());
    spectralwindowids_p=spectralwindowids;
    fieldid_p=fieldid;
    facets_p=facets;
    redoSkyModel_p=true;
    destroySkyEquation();    

    // Now make the derived quantities 
    Vector<Int> whichStokes = decideNPolPlanes(false);
    if( whichStokes.nelements()==1 && whichStokes[0]==0 )
      {
      this->unlock();
      os << LogIO::SEVERE << "Stokes selection " << stokes_p << " is currently not supported." << LogIO::EXCEPTION;
      return false;
      }

    // nchan we need to get rid of one of these variables 
    nchan_p=imageNchan_p;
    
    if(fieldid < 0){      
      doShift_p=true;
      phaseCenter_p=phaseCenter;
    }
    else {
      MSFieldColumns msfield(ms_p->field());
      phaseCenter_p=msfield.phaseDirMeas(fieldid_p);
    }
    
    // we only support projections SIN, CAR, TAN, and SFL
    Projection::Type const ptype = Projection::type(projection);
    if (ptype != Projection::SIN && ptype != Projection::CAR
        && ptype != Projection::TAN && ptype != Projection::SFL) {
      this->unlock();
      os << LogIO::SEVERE << "Projection " << projection << " is currently not supported." << LogIO::EXCEPTION;
      return false;
    } else {
      projection_p = projection;
    }

    
    // Now we have set the image parameters
    setimaged_p=true;
    beamValid_p=false;
    
    this->unlock();


    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  } 
  return true;
}


Bool Imager::advise(const Bool takeAdvice, const Float amplitudeLoss,
		    const Quantity& fieldOfView, Quantity& cell,
		    Int& pixels, Int& facets, MDirection& phaseCenter)
{
  if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "advise()", WHERE));
  
  try {
    
    os << "Advising image properties" << LogIO::POST;
    
    Float maxAbsUV=0.0;
    Float maxWtAbsUV=0.0;
    // To determine the number of facets, we need to fit w to
    // a.u + b.v. The misfit from this (i.e. the dispersion 
    // will determine the error beam due to the non-coplanar
    // baselines. We'll do both cases: where the position
    // errors are important and where they are not. We'll use
    // the latter.
    Double sumWt = 0.0;

    Double sumUU=0.0;
    Double sumUV=0.0;
    Double sumUW=0.0;
    Double sumVV=0.0;
    Double sumVW=0.0;
    Double sumWW=0.0;

    Double sumWtUU=0.0;
    Double sumWtUV=0.0;
    Double sumWtUW=0.0;
    Double sumWtVV=0.0;
    Double sumWtVW=0.0;
    Double sumWtWW=0.0;

    Double sum = 0.0;

    this->lock();
    ROVisIter& vi(*rvi_p);
    VisBuffer vb(vi);
    
    for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	Int nRow=vb.nRow();
	Int nChan=vb.nChannel();
	for (Int row=0; row<nRow; ++row) {
	  for (Int chn=0; chn<nChan; ++chn) {
	    if(!vb.flag()(chn,row)) {
	      Float f=vb.frequency()(chn)/C::c;
	      Float u=vb.uvw()(row)(0)*f;
	      Float v=vb.uvw()(row)(1)*f;
	      Float w=vb.uvw()(row)(2)*f;
              Double wt=vb.imagingWeight()(chn,row);
	      if(wt>0.0) {
		if(abs(u)>maxWtAbsUV) maxWtAbsUV=abs(u);
		if(abs(v)>maxWtAbsUV) maxWtAbsUV=abs(v);
		sumWt += wt;
                sumWtUU += wt * u * u;
                sumWtUV += wt * u * v;
                sumWtUW += wt * u * w;
                sumWtVV += wt * v * v;
                sumWtVW += wt * v * w;
                sumWtWW += wt * w * w;
	      }
	      sum += 1;
	      if(abs(u)>maxAbsUV) maxAbsUV=abs(u);
	      if(abs(v)>maxAbsUV) maxAbsUV=abs(v);
	      sumUU += u * u;
	      sumUV += u * v;
	      sumUW += u * w;
	      sumVV += v * v;
	      sumVW += v * w;
	      sumWW += w * w;
	    }
	  }
	}
      }
    }
    

    if(sumWt==0.0) {
      os << LogIO::WARN << "Visibility data are not yet weighted: using unweighted values" << LogIO::POST;
      sumWt = sum;
    }
    else {
      sumUU = sumWtUU;
      sumUV = sumWtUV;
      sumUW = sumWtUW;
      sumVV = sumWtVV;
      sumVW = sumWtVW;
      sumWW = sumWtWW;
      maxAbsUV = maxWtAbsUV;
    }

    // First find the cell size
    if(maxAbsUV==0.0) {
      this->unlock();
      os << LogIO::SEVERE << "Maximum uv distance is zero" << LogIO::POST;
      return false;
    }
    else {
      cell=Quantity(0.5/maxAbsUV, "rad").get("arcsec");
      os << "Maximum uv distance = " << maxAbsUV << " wavelengths" << endl;
      os << "Recommended cell size < " << cell.get("arcsec").getValue()
	 << " arcsec" << LogIO::POST;
    }

    // Now we can find the number of pixels for the specified field of view
    pixels = 2*Int((fieldOfView.get("rad").getValue()/cell.get("rad").getValue())/2.0);
    {
      CompositeNumber cn(pixels);
      pixels = (Int) (cn.nextLargerEven(pixels));
    }
    if(pixels < 64) pixels = 64;
    os << "Recommended number of pixels = " << pixels << endl;

      // Rough rule for number of facets:
      // For the specified facet size, the loss in amplitude
      // due to the peeling of facets from the sphere should 
      // be equal to the amplitude error.
      Int worstCaseFacets=1;
      if(sumWt<=0.0||sumUU<=0.0||(sumUU+sumVV)<=0.0) {
	this->unlock();
	os << LogIO::SEVERE << "Sum of imaging weights is zero" << LogIO::POST;
	return false;
      }
      else {
	Double rmsUV  = sqrt((sumUU + sumVV)/sumWt);
	Double rmsW = sqrt(sumWW/sumWt);
	os << "Dispersion in uv, w distance = " << rmsUV << ", "<< rmsW
	   << " wavelengths" << endl;
	if(rmsW>0.0&&rmsUV>0.0&&amplitudeLoss>0.0) {
	  worstCaseFacets =
	    Int (pixels * (abs(cell.get("rad").getValue())*
				  sqrt(C::pi*rmsW/(sqrt(32.0*amplitudeLoss)))));
	}
	else {
	  os << LogIO::WARN << "Cannot calculate number of facets: using 1"
	     << LogIO::POST;
	  worstCaseFacets = 1;
	}
	// Solve for the parameters:
	Double Determinant = sumUU * sumVV - square(sumUV);
	Double rmsFittedW = rmsW;
	if(Determinant > 0.0) {
	  Double a = ( sumVV * sumUW - sumUV * sumVW)/Determinant;
	  Double b = (-sumUV * sumUW + sumUU * sumVW)/Determinant;
	  os << "Best fitting plane is w = " << a << " * u + "
	     << b << " * v" << endl;
	  Double FittedWW =
	    sumWW  + square(a) * sumUU + square(b) * sumVV +
	    + 2.0 * a * b * sumUV - 2.0 * (a * sumUW + b * sumVW);
	  rmsFittedW  = sqrt(FittedWW/sumWt);
	  os << "Dispersion in fitted w = " << rmsFittedW
	     << " wavelengths" << endl;
	  facets = Int (pixels * (abs(cell.get("rad").getValue())*
				  sqrt(C::pi*rmsFittedW/(sqrt(32.0*amplitudeLoss)))));
          if (facets<1) facets = 1;
	}
	else {
	  os << "Error in fitting plane to uvw data" << LogIO::POST;
	}
	if(worstCaseFacets<1) worstCaseFacets=1;
	if(worstCaseFacets>1) {
	  os << "imager recommends that you use the wide field clean" << endl
	     << "For accurate positions, use " << worstCaseFacets
	     << " facets on each axis" << endl
	     << "For accurate removal of sources, you only need "
	     << facets << " facets on each axis" << LogIO::POST;
	}
	else {
	  os << "Wide field cleaning is not necessary"
	     << LogIO::POST;
	}
      }

    MSColumns msc(*mssel_p);
    if(datafieldids_p.shape()!=0){
      //If setdata has been used prior to this
    phaseCenter=msc.field().phaseDirMeas(datafieldids_p(0));
    }
    else{
    phaseCenter=msc.field().phaseDirMeas(fieldid_p);   
    }

    
    // Now we have set the image parameters
    if(takeAdvice) {
      os << "Using advised image properties" << LogIO::POST;
      mcellx_p=cell;
      mcelly_p=cell;
      phaseCenter_p=phaseCenter;
      setimaged_p=true;
      beamValid_p=false;
      facets_p=facets;
      nx_p=ny_p=pixels;
    }
    
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  } 
  
  return true;
}



Bool Imager::setDataPerMS(const String& msname, const String& mode, 
			  const Vector<Int>& nchan, 
			  const Vector<Int>& start,
			  const Vector<Int>& step,
			  const Vector<Int>& spectralwindowids,
			  const Vector<Int>& fieldids,
			  const String& msSelect, const String& timerng,
			  const String& fieldnames, 
			  const Vector<Int>& antIndex,
			  const String& antnames,
			  const String& spwstring,
                          const String& uvdist, const String& scan,
                          const String& intent, const String& obs,
                          const Bool useModelCol, const Bool /*readonly*/)
{
  LogIO os(LogOrigin("imager", "setdata()"), logSink_p);
  if(msname != ""){

    LogIO os(LogOrigin("imager", "setdata()"), logSink_p);
    os << LogIO::WARN
       << "Ignoring that ms" << msname << "specified here"
       << LogIO::POST;
    os << LogIO::WARN
       << "Imager was constructed with an ms "
       << LogIO::POST;
    os << LogIO::WARN
       << "if multi-ms are to be used please construct imager without parameters and use setdata to specify the ms's and selection"
       << LogIO::POST;

  }
  MRadialVelocity dummy;
  //Calling the old setdata
  return   setdata(mode, nchan, start, step, dummy, dummy, spectralwindowids, 
		   fieldids, msSelect, timerng, fieldnames, antIndex, 
                   antnames, spwstring, uvdist, scan, intent, obs, useModelCol);

}


Bool Imager::setdata(const String& mode, const Vector<Int>& nchan,
		     const Vector<Int>& start, const Vector<Int>& step,
		     const MRadialVelocity& mStart,
		     const MRadialVelocity& mStep,
		     const Vector<Int>& spectralwindowids,
		     const Vector<Int>& fieldids,
		     const String& msSelect, const String& timerng,
		     const String& fieldnames, const Vector<Int>& antIndex,
		     const String& antnames, const String& spwstring,
                     const String& uvdist, const String& scan,
                     const String& intent,
                     const String& obs, 
                     const Bool /*useModelCol*/,
                     const Bool be_calm)
{
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "data selection"), logSink_p);

  if(ms_p.null()) {
    os << LogIO::SEVERE << "Program logic error: MeasurementSet pointer ms_p not yet set"
       << LogIO::EXCEPTION;
    nullSelect_p=true;
    return false;
  }

  os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
     << "mode=" << mode << " nchan=" << nchan 
     <<  " start=" << start << " step=" << step;
  ostringstream clicom;
  clicom <<  " mstart='" << mStart << "' mstep='" << mStep;
  os << String(clicom) ;
  os <<  "' spectralwindowids=" << spectralwindowids
     << " fieldids=" << fieldids << " msselect=" << msSelect;

  nullSelect_p=false;
  String local_spwstring(spwstring);
  if (local_spwstring  == "") local_spwstring="*";
  try {
    
    this->lock();
    this->writeCommand(os);

    os << (be_calm ? LogIO::NORMAL2 : LogIO::NORMAL)
       << "Performing selection on MeasurementSet : " << ms_p->tableName()
       << LogIO::POST; // Loglevel PROGRESS
    //Some MSSelection 
    MSSelection thisSelection;

    // check that sorted table exists (it should), if not, make it now.
    //this->makeVisSet(*ms_p);
    
    //MeasurementSet sorted=ms_p->keywordSet().asTable("SORTED_TABLE");
    //MSSelection thisSelection (sorted, MSSelection::PARSE_NOW,timerng,antnames,
    //			       fieldnames, local_spwstring,uvdist, msSelect,"",
    //			       scan, obs); 

    datafieldids_p.resize(fieldids.nelements());
    datafieldids_p = fieldids;
    if(datafieldids_p.nelements() > 0){
      thisSelection.setFieldExpr(MSSelection::indexExprStr(datafieldids_p));
      os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
         << "Selecting on field ids : " << datafieldids_p <<  LogIO::POST;
    }
    if(fieldnames != ""){
      thisSelection.setFieldExpr(fieldnames);
      os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
         << "Selecting on fields : " << fieldnames << LogIO::POST;
    }
    
    dataspectralwindowids_p.resize(spectralwindowids.nelements());
    dataspectralwindowids_p = spectralwindowids;
    if(dataspectralwindowids_p.nelements() > 0){
      thisSelection.setSpwExpr(MSSelection::indexExprStr(dataspectralwindowids_p));
      os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
         << "Selecting on spectral windows" << LogIO::POST;
    }
    else if(local_spwstring != ""){
      os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
         << "Selecting on spectral windows expression : " << local_spwstring
	 << LogIO::POST;
      thisSelection.setSpwExpr(local_spwstring);
    }
    
    if(antIndex.nelements() >0){
      thisSelection.setAntennaExpr( MSSelection::indexExprStr(antIndex));
      os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
         << "Selecting on antenna ids : " << antIndex << LogIO::POST;	
    }
    if(antnames != ""){
      Vector<String>antNames(1, antnames);
      //       thisSelection.setAntennaExpr(MSSelection::nameExprStr(antNames));
      thisSelection.setAntennaExpr(antnames);
      os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
         << "Selecting on antenna names : " << antnames << LogIO::POST; 
    } 
               
    if(timerng != ""){
      //	Vector<String>timerange(1, timerng);
	thisSelection.setTimeExpr(timerng);
	os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
           << "Selecting on time range : " << timerng << LogIO::POST;	
    }
    
    if(uvdist != ""){
	thisSelection.setUvDistExpr(uvdist);
	os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
           << "Selecting on uvdist : " << uvdist << LogIO::POST;	
    }
    
    if(scan != ""){
      thisSelection.setScanExpr(scan);
	os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
           << "Selecting on scan : " << scan << LogIO::POST;	
    }
    if(intent != "") {
      thisSelection.setStateExpr(intent);
	os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
           << "Selecting on State Expr : " << intent  << LogIO::POST;	
    }
    if(obs != ""){
      thisSelection.setObservationExpr(obs);
	os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
           << "Selecting on Observation Expr : " << obs << LogIO::POST;	
    }
    if(msSelect != ""){
      thisSelection.setTaQLExpr(msSelect);
	os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
           << "Selecting via TaQL : " << msSelect << LogIO::POST;	
    }
    
    //***************
    TableExprNode exprNode;
    try{
      exprNode = thisSelection.toTableExprNode(&(*ms_p));
    }
    catch(...){
      nullSelect_p = true;
      this->unlock();
      // A "bad selection" warning message could be sent to the logger here,
      // but it should be left to the calling function to do that.  For
      // example, this function is called by setjy, and it would be a mistake
      // to print a logger message for every spw that was not observed for the
      // given field.
      return false;
    }
    //TableExprNode exprNode=thisSelection.getTEN();
    //if(exprNode.isNull()){
      //      throw(AipsError("Selection failed...review ms and selection parameters"));
    //}

    // Need to delete the ft machine as the channel flag may change
    if(ft_p)
      delete ft_p;
    ft_p=0;
    dataMode_p=mode;
    dataNchan_p.resize();
    dataStart_p.resize();
    dataStep_p.resize();
    dataNchan_p=nchan;
    dataStart_p=start;
    dataStep_p=step;
    mDataStart_p=mStart;
    mDataStep_p=mStep;
    //    useModelCol_p=useModelCol;
    
    if(rvi_p)
      delete rvi_p; 
    rvi_p=0;
    wvi_p=0;
    // if(mssel_p) delete mssel_p; 
    mssel_p=0;
    
    datafieldids_p.resize();
    datafieldids_p=thisSelection.getFieldList();
    if(datafieldids_p.nelements()==0){
      Int nf=ms_p->field().nrow();
      datafieldids_p.resize(nf);
      indgen(datafieldids_p);
    }
    //Now lets see what was selected as spw and match it with datadesc
    //
    // getSpwList could return duplicated spw ids
    // when multiple channel ranges are specified.
    //dataspectralwindowids_p.resize();
    //dataspectralwindowids_p=thisSelection.getSpwList();
    //get channel selection in spw
    // TT: Added sorting option in getChanList call 
    //     to accomodate changes related CAS-2521
    Matrix<Int> chansels=thisSelection.getChanList(NULL, 1, true);
    mssChanSel_p.assign(chansels);
    mssFreqSel_p.resize();
    mssFreqSel_p=thisSelection.getChanFreqList(NULL, true);

    //cout<<"chansels="<<chansels<<endl;
    //convert the selection into flag
    uInt nms = 1;
    uInt nrow = chansels.nrow();
    dataspectralwindowids_p.resize();
    const MSSpWindowColumns spwc(ms_p->spectralWindow());
    uInt nspw = spwc.nrow();
    const ScalarColumn<Int> spwNchans(spwc.numChan());
    Vector<Int> nchanvec = spwNchans.getColumn();
    Int maxnchan = 0;
    for (uInt i=0;i<nchanvec.nelements();i++) {
      maxnchan=max(nchanvec[i],maxnchan);
    }
    
    spwchansels_p.resize(nms,nspw,maxnchan);
    spwchansels_p.set(0);
    uInt nselspw=0;
    if (nrow==0) {
      //no channel selection, select all channels
      spwchansels_p=1;
      dataspectralwindowids_p=thisSelection.getSpwList();
    }
    else {
      spwchansels_p=0; //deselect
      Int prvspwid=-1;
      Vector<Int> selspw;
      for (uInt i=0;i<nrow;i++) {
	Vector<Int> sel = chansels.row(i);
	Int spwid = sel[0];
	if((sel[1] >= nchanvec[spwid]) || (sel[2] >=nchanvec[spwid]))
	  throw(AipsError("Unexpected selection  in spw selection of spwid "+String::toString(spwid)));
	if (spwid != prvspwid){
	  nselspw++;
	  selspw.resize(nselspw,true);
	  selspw[nselspw-1]=spwid;
	}
	uInt minc= sel[1];
	uInt maxc = sel[2];
	uInt step = sel[3];
	// step as the same context as in im.selectvis
	// select channels 
	for (uInt k=minc;k<(maxc+1);k+=step) {
	  spwchansels_p(0,spwid,k)=1;
	}
	prvspwid=spwid;
      }
      dataspectralwindowids_p=selspw;
    }
    
    // Map the selected spectral window ids to data description ids
    if(dataspectralwindowids_p.nelements()==0){
      Int nspwinms=ms_p->spectralWindow().nrow();
      dataspectralwindowids_p.resize(nspwinms);
      indgen(dataspectralwindowids_p);
    }
    MSDataDescIndex msDatIndex(ms_p->dataDescription());
    datadescids_p.resize(0);
    datadescids_p=msDatIndex.matchSpwId(dataspectralwindowids_p);
    
    if (datafieldids_p.nelements() > 1) {
      os << LogIO::NORMAL4<< "Multiple fields specified" << LogIO::POST;
      multiFields_p = true;
    }
    
    
    if(mode=="none"){
      // Now channel selection from spw already stored in chansel,
      // no need for this- TT
      //check if we can find channel selection in the spw string
      //Matrix<Int> chanselmat=thisSelection.getChanList();
      //
      // This not correct for multiple channel ranges TT
      //if(chanselmat.nrow()==dataspectralwindowids_p.nelements()){
      if(nselspw==dataspectralwindowids_p.nelements()){
	
	dataMode_p="channel";
	dataStep_p.resize(dataspectralwindowids_p.nelements());
	dataStart_p.resize(dataspectralwindowids_p.nelements());
	dataNchan_p.resize(dataspectralwindowids_p.nelements());
	Cube<Int> spwchansels_tmp=spwchansels_p;
	
	for (uInt k =0 ; k < dataspectralwindowids_p.nelements(); ++k){
	  uInt curspwid=dataspectralwindowids_p[k];
	  //dataStep_p[k]=1;
	  if (nrow > 0) {
	    dataStep_p[k]=chansels.row(k)(3);
	  }
	  else {
	    dataStep_p[k]=1;
	  }
	  //dataStart_p[k]=chanselmat.row(k)(1);
	  dataStart_p[k]=0;
	  dataNchan_p[k]=nchanvec(curspwid);
	  //find start
	  Bool first =true;
	  uInt nchn = 0;
	  uInt lastchan = 0;
	  for (uInt j=0 ; j < uInt(nchanvec(curspwid)); j++) {
	    if (spwchansels_p(0,curspwid,j)==1) {
	      if (first) {
		dataStart_p[k]=j;
		first = false;
	      }
	      lastchan=j;
	      nchn++;
	    }	
	  }
	  dataNchan_p[k]=Int(ceil(Double(lastchan-dataStart_p[k])/Double(dataStep_p[k])))+1;
	  //dataNchan_p[k]=Int(ceil(Double(chanselmat.row(k)(2)-dataStart_p[k])/Double(dataStep_p[k])))+1;
	  
	  //if(dataNchan_p[k]<1)
	  //  dataNchan_p[k]=1;	  
	  
	  //cout<<"modified start="<<dataStart_p[k]<<endl;
	  //cout<<"modified nchan="<<dataNchan_p[k]<<endl;
	  //
	  //Since msselet will be applied to the data before flags from spwchansels_p
	  //are applied to the data in FTMachine, shift spwchansels_p by dataStart_p
	  //for (uInt j=0  ; j < nchanvec(k)-dataStart_p[k]; j++){
	  for (uInt j=0  ; j < uInt(nchanvec(curspwid)); j++){
	    if ( Int(j) < nchanvec(curspwid)-dataStart_p[k]) {
	      spwchansels_tmp(0,curspwid,j) = spwchansels_p(0,curspwid,j+dataStart_p[k]);
	    }
	    else {
	      spwchansels_tmp(0,curspwid,j) = 0;
	    }
	  }
	}
	spwchansels_p = spwchansels_tmp;
      }
    }
    if(!(exprNode.isNull())){
      mssel_p = new MeasurementSet((*ms_p)(exprNode), &* ms_p);
    }
    else{
      // Null take all the ms ...setdata() blank means that
      mssel_p = new MeasurementSet(*ms_p);
    }

    AlwaysAssert(!mssel_p.null(), AipsError);
    if(mssel_p->nrow()==0) {
      //delete mssel_p; 
      mssel_p=0;
      os << (be_calm ? LogIO::NORMAL4 : LogIO::WARN)
         << "Selection is empty: reverting to sorted MeasurementSet"
	 << LogIO::POST;
      mssel_p=new MeasurementSet(*ms_p);
      nullSelect_p=true;
    }
    else {
      mssel_p->flush();
      nullSelect_p=false;
    }
    if (nullSelect_p) {
      if ((mssel_p->field()).nrow() > 1) {
	os << LogIO::NORMAL4 << "Multiple fields selected" << LogIO::POST;
	multiFields_p = true;
      } else {
	os << LogIO::NORMAL4 << "Single field selected" << LogIO::POST;
	multiFields_p = false;
      }
    }
    
    uInt nvis_all = ms_p->nrow();
    
    // Now create the VisSet
    this->makeVisSet(*mssel_p); 
    AlwaysAssert(rvi_p, AipsError);
    uInt nvis_sel = mssel_p->nrow();
    
    if(nvis_sel != nvis_all) {
      os << LogIO::NORMAL // Loglevel INFO
         << "Selected " << nvis_sel << " out of "
         << nvis_all << " rows."
	 << LogIO::POST;
    }
    else {
      os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL)
         << "Selected all " << nvis_sel << " rows" << LogIO::POST; // Loglevel INFO
    }
    //    }

    // Tell the user how many channels have been selected.
    // NOTE : This code is replicated in ImagerMultiMS.cc.
    Vector<Int> chancounts(dataspectralwindowids_p.nelements());
    chancounts=0;
    //    if( local_spwstring == "" ) os << "Selected all spws and channels" << LogIO::POST;
    //else os << "Channel selection : " << local_spwstring << LogIO::POST;
    os << (be_calm ? LogIO::NORMAL4 : LogIO::NORMAL) << "Selected:";
    for(uInt k=0;k<dataspectralwindowids_p.nelements();k++)
      {
	for(uInt ch=0;ch<uInt(nchanvec(dataspectralwindowids_p[k]));ch++) 
	  {if(spwchansels_p(0,dataspectralwindowids_p[k],ch)) chancounts[k]++; }
	os << " [" << chancounts[k] << " chans in spw " << dataspectralwindowids_p[k] << "]";
	//	os << "Selected " << chancounts[k] << " channels in spw " 
	//  << dataspectralwindowids_p[k] << LogIO::POST;
      }
    os << LogIO::POST;

    // Now we do a selection to cut down the amount of information
    // passed around.
  
    this->selectDataChannel(dataspectralwindowids_p, dataMode_p,
			    dataNchan_p, dataStart_p, dataStep_p,
                            mDataStart_p, mDataStep_p);
    ///Tell iterator to use on the fly imaging weights if scratch columns is
    ///not in use
    imwgt_p=VisImagingWeight("natural");
    rvi_p->useImagingWeight(imwgt_p);
    
    // Guess that the beam is no longer valid
    beamValid_p=false;
    destroySkyEquation();
    if(!valid()){ 
      this->unlock();
      os << LogIO::SEVERE << "Check your data selection or Measurement set "
         << LogIO::EXCEPTION;
      return false;
    }
    this->unlock();
    return !nullSelect_p;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
   
    return false;
  } 
  return !nullSelect_p;
}


Bool Imager::setmfcontrol(const Float cyclefactor,
			  const Float cyclespeedup,
                          const Float cyclemaxpsffraction, 
			  const Int stoplargenegatives, 
			  const Int stoppointmode,
			  const String& scaleType,
			  const Float minPB,
			  const Float constPB,
			  const Vector<String>& fluxscale,
			  const Bool flatnoise)
{  
  cyclefactor_p = cyclefactor;
  cyclespeedup_p =  cyclespeedup;
  cyclemaxpsffraction_p = cyclemaxpsffraction;
  stoplargenegatives_p = stoplargenegatives;
  stoppointmode_p = stoppointmode;
  fluxscale_p.resize( fluxscale.nelements() );
  fluxscale_p = fluxscale;
  scaleType_p = scaleType;
  minPB_p = minPB;
  flatnoise_p=flatnoise;

  constPB_p = constPB;
  return true;
}  


Bool Imager::setvp(const Bool dovp,
		   const Bool doDefaultVPs,
		   const String& vpTable,
		   const Bool doSquint,
		   const Quantity &parAngleInc,
		   const Quantity &skyPosThreshold,
		   String defaultTel,
                   const Bool verbose)
{

  LogIO os(LogOrigin("Imager", "setvp()", WHERE));
  
  os << LogIO::NORMAL << "Setting voltage pattern parameters" << LogIO::POST; // Loglevel PROGRESS
  
  if(!dovp && !vp_p)
    delete vp_p;
  vp_p=0;
  doVP_p=dovp;
  doDefaultVP_p = doDefaultVPs;
  vpTableStr_p = vpTable;
  telescope_p= defaultTel;
  if (doSquint) {
    squintType_p = BeamSquint::GOFIGURE;
  } else {
    squintType_p = BeamSquint::NONE;
  }

  parAngleInc_p = parAngleInc;

  skyPosThreshold_p = skyPosThreshold;
  os << (verbose ? LogIO::NORMAL : LogIO::NORMAL3) // Loglevel INFO
     <<"Sky position tolerance is "<<skyPosThreshold_p.getValue("deg")
     << " degrees" << LogIO::POST;

  if (doDefaultVP_p) {
    os << (verbose ? LogIO::NORMAL : LogIO::NORMAL3) // Loglevel INFO
       << "Using system default voltage patterns for each telescope" << LogIO::POST;
  } else {
    os << (verbose ? LogIO::NORMAL : LogIO::NORMAL3) // Loglevel INFO
       << "Using user defined voltage patterns in Table "
       <<  vpTableStr_p << LogIO::POST;
  }
  if (doSquint) {
    os << (verbose ? LogIO::NORMAL : LogIO::NORMAL3) // Loglevel INFO
       << "Beam Squint will be included in the VP model" <<  LogIO::POST;
    os << (verbose ? LogIO::NORMAL : LogIO::NORMAL3)
       << "and the Parallactic Angle increment is "  // Loglevel INFO
       << parAngleInc_p.getValue("deg") << " degrees"  << LogIO::POST;
  }

  // muddled with the state of SkyEquation..so redo it
  destroySkyEquation();
  return true;
}

Bool Imager::setoptions(const String& ftmachine, const Long cache, const Int tile,
			const String& gridfunction, const MPosition& mLocation,
                        const Float padding,
			const Int wprojplanes,
			const String& epJTableName,
			const Bool applyPointingOffsets,
			const Bool doPointingCorrection,
			const String& cfCacheDirName,
			const Float& rotPAStep, 
			const Float& computePAStep, 
			const Float& pbLimit, const String& interpMeth, const Int imageTileVol,
			const Bool singprec,
			const Int numthreads,
			const Bool psTermOn,
			const Bool aTermOn,
			const Bool mTermOn,
			const Bool wbawp,
			const Bool conjBeams)
{
  if(!valid()) 
    {
      return false;
    }
  if(!assertDefinedImageParameters())
    {
      return false;
    }
  LogIO os(LogOrigin("imager", "setoptions()", WHERE));
  
  os << LogIO::NORMAL << "Setting processing options" << LogIO::POST; // Loglevel PROGRESS

  ftmachine_p=downcase(ftmachine);
  if(ftmachine_p=="gridft") {
    os << LogIO::WARN
       << "FT machine gridft is now called ft - please use the new name in future"
       << LogIO::POST;
    ftmachine_p="ft";
  }

  if(ftmachine_p=="wfmemoryft"){
    wfGridding_p=true;
    ftmachine_p="ft";
  }

  wprojPlanes_p=wprojplanes;
  epJTableName_p = epJTableName;
  cfCacheDirName_p = cfCacheDirName;
  rotPAStep_p = rotPAStep;
  computePAStep_p = computePAStep;
  pbLimit_p = pbLimit;
  psTermOn_p=psTermOn;
  aTermOn_p=aTermOn;
  mTermOn_p=mTermOn;
  wbAWP_p=wbawp;
  conjBeams_p=conjBeams;
  freqInterpMethod_p=interpMeth;
  imageTileVol_p=imageTileVol;
  if(imageTileVol_p <= 0){
    avoidTempLatt_p=true;
    imageTileVol_p=-1*imageTileVol_p;
  }
 
  
  singlePrec_p=singprec;

  if(cache>0) cache_p=cache;
  if(tile>0) tile_p=tile;
  gridfunction_p=downcase(gridfunction);
  mLocation_p=mLocation;
  if(padding>=1.0) {
    padding_p=padding;
  }

  // Check if gridfunction is set to gauss or gjinc for non single dish imaging
  if (ftmachine_p!="sd" 
      && (gridfunction_p=="gauss" || gridfunction_p=="gjinc")) {
    os << LogIO::SEVERE
       << "Grid function " << gridfunction << " is available only for single dish imaging" << LogIO::EXCEPTION;
  }

  // Destroy the FTMachine
  if(ft_p) {delete ft_p; ft_p=0;}
  if(gvp_p) {delete gvp_p; gvp_p=0;}
  if(cft_p) {delete cft_p; cft_p=0;}

  doPointing = applyPointingOffsets;
  doPBCorr = doPointingCorrection;
////The set below does not seemed to be remembered later in the process it 
/// under some compiler version so setting a private variable to be used
/// a negative number means use all that is available
  numthreads_p= numthreads;
#ifdef _OPENMP
  if(numthreads > 0){
    if(numthreads <= omp_get_max_threads()){
      omp_set_num_threads(numthreads);
    }
  }
#endif
  return true;
}

Bool Imager::setsdoptions(const Float scale, const Float weight, 
			  const Int convsupport, String pointCol,
			  const Quantity truncate,
			  const Quantity gwidth, const Quantity jwidth,
			  const Float minweight, const Bool clipminmax)
{
  LogIO os(LogOrigin("imager", "setsdoptions()", WHERE));
  
  os << LogIO::NORMAL << "Setting single dish processing options" << LogIO::POST; // Loglevel PROGRESS
  
  sdScale_p=scale;
  sdWeight_p=weight;

  
  sdConvSupport_p=convsupport;
  pointingDirCol_p=pointCol;
  pointingDirCol_p.upcase();
  if( (pointingDirCol_p != "DIRECTION") &&(pointingDirCol_p != "TARGET") && (pointingDirCol_p != "ENCODER") && (pointingDirCol_p != "POINTING_OFFSET") && (pointingDirCol_p != "SOURCE_OFFSET")){
    os << LogIO::SEVERE
       << "No such direction column as "<< pointingDirCol_p
       << " in pointing table "<< LogIO::EXCEPTION;
  }
  qtruncate_p=truncate;
  qgwidth_p=gwidth;
  qjwidth_p=jwidth;
  minWeight_p = minweight;
  clipminmax_p = clipminmax;

  // Destroy the FTMachine
  if(ft_p) {delete ft_p; ft_p=0;}
  if(gvp_p) {delete gvp_p; gvp_p=0;}
  if(cft_p) {delete cft_p; cft_p=0;}

  return true;
}

Bool Imager::mask(const String& mask, const String& image,
		  const Quantity& threshold) 
{
  //if(!valid()) return false;
  LogIO os(LogOrigin("imager", "mask()", WHERE));
  //if(!assertDefinedImageParameters()) return false;
  
  try {
    //this->lock();
    if(image=="") {
      //this->unlock();
      os << LogIO::SEVERE << "Need name for template image" << LogIO::EXCEPTION;
      return false;
    }
    String maskName(mask);
    if(maskName=="") {
      maskName=image+".mask";
    }
    if(!clone(image, maskName)) return false;
    PagedImage<Float> maskImage(maskName);
    maskImage.table().markForDelete();
    PagedImage<Float> imageImage(image);

    if(threshold.check(UnitVal(1.0, "Jy"))){
      os << LogIO::NORMAL // Loglevel INFO
         << "Making mask image " << maskName << ", applying threshold "
         << threshold.get("Jy").getValue() << "Jy, " << endl
         << "to template image " << image << LogIO::POST;
    
      StokesImageUtil::MaskFrom(maskImage, imageImage, threshold);
    }
    else{
      os << LogIO::NORMAL // Loglevel INFO
         << "Making mask image " << maskName << ", applying threshold "
         << threshold.getValue() << " " << threshold.getUnit() << endl
         << "to template image " << image << LogIO::POST;
    
      StokesImageUtil::MaskFrom(maskImage, imageImage, threshold.getValue());
    }
    maskImage.table().unmarkForDelete();

    //this->lock();
    return true;
  } catch (AipsError x) {
    //this->unlock();
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  } 
  //this->unlock();
  return true;
}

Bool Imager::boxmask(const String& mask, const Vector<Int>& blc,
		  const Vector<Int>& trc, const Float value) 
{
  if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "boxmask()", WHERE));
  
  try {
    
    if(!assertDefinedImageParameters()) return false;
    
    if(mask=="") {
      os << LogIO::SEVERE << "Need name for mask image" << LogIO::EXCEPTION;
      return false;
    }
    if(!Table::isWritable(mask)) {
      make(mask);
      this->lock();
    }
    PagedImage<Float> maskImage(mask);
    maskImage.table().markForDelete();
    

    IPosition iblc(blc);
    IPosition itrc(trc);
    IPosition iinc(iblc.nelements(), 1);
    LCBox::verify(iblc, itrc, iinc, maskImage.shape());
    
    os << LogIO::DEBUG1
       << "Setting '" << mask << "' blc=" << iblc
       << " trc=" << itrc << " to " << value << LogIO::POST;
    
    StokesImageUtil::BoxMask(maskImage, iblc, itrc, value);
    
    maskImage.table().unmarkForDelete();

    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  } 
  return true;
}

  Bool Imager::regionmask(const String& maskimage, Record* imageRegRec, 
			  Matrix<Quantity>& blctrcs, Matrix<Float>& circles, 
			  const Float& value){

  //This function does not modify ms(s) so no need of lock 
  LogIO os(LogOrigin("imager", "regionmask()", WHERE));
  if(!Table::isWritable(maskimage)) {
    make(maskimage);
  }
  return Imager::regionToImageMask(maskimage, imageRegRec, blctrcs, circles, value);

}

  Bool Imager::regionToImageMask(const String& maskimage, Record* imageRegRec, Matrix<Quantity>& blctrcs, Matrix<Float>& circles, const Float& value){
  PagedImage<Float> maskImage(maskimage);
  CoordinateSystem cSys=maskImage.coordinates();
  maskImage.table().markForDelete();
  ImageRegion *boxregions=0;
  ImageRegion *circleregions=0;
  RegionManager regMan;
  regMan.setcoordsys(cSys);
  Vector<Quantum<Double> > blc(2);
  Vector<Quantum<Double> > trc(2);
  if(blctrcs.nelements() !=0){
    if(blctrcs.shape()(1) != 4)
      throw(AipsError("Need a list of 4 elements to define a box"));
    Int nrow=blctrcs.shape()(0);
    Vector<Int> absRel(2, RegionType::Abs); 
    PtrBlock<const WCRegion *> lesbox(nrow);
    for (Int k=0; k < nrow; ++k){
      blc(0) = blctrcs(k,0);
      blc(1) = blctrcs(k,1);
      trc(0) = blctrcs(k,2);
      trc(1) = blctrcs(k,3); 
      lesbox[k]= new WCBox (blc, trc, cSys, absRel);
    }
    boxregions=regMan.doUnion(lesbox);
    for (Int k=0; k < nrow; ++k){
      delete lesbox[k];
    }
  }
  if((circles.nelements()) > 0){
    if(circles.shape()(1) != 3)
      throw(AipsError("Need a list of 3 elements to define a circle"));
    Int nrow=circles.shape()(0);
    Vector<Float> cent(2); 
    cent(0)=circles(0,1); cent(1)=circles(0,2);
    Float radius=circles(0,0);
    IPosition xyshape(2,maskImage.shape()(0),maskImage.shape()(1));
    LCEllipsoid *circ= new LCEllipsoid(cent, radius, xyshape);
    //Tell LCUnion to delete the pointers
    LCUnion *elunion= new LCUnion(true, circ);
    //now lets do the remainder
    for (Int k=1; k < nrow; ++k){
      cent(0)=circles(k,1); cent(1)=circles(k,2);
      radius=circles(k,0);
      circ= new LCEllipsoid(cent, radius, xyshape); 
      elunion=new LCUnion(true, elunion, circ);
    }
    //now lets extend that to the whole image
    IPosition trc(2);
    trc(0)=maskImage.shape()(2)-1;
    trc(1)=maskImage.shape()(3)-1;
    LCBox lbox(IPosition(2,0,0), trc, 
	       IPosition(2,maskImage.shape()(2),maskImage.shape()(3)) );
    LCExtension linter(*elunion, IPosition(2,2,3),lbox);
    circleregions=new ImageRegion(linter);
    delete elunion;
  }


  ImageRegion* recordRegion=0;
  if(imageRegRec !=0){
    TableRecord rec1;
    rec1.assign(*imageRegRec);
    recordRegion=ImageRegion::fromRecord(rec1,"");    
  }
  ImageRegion *unionReg=0;
  if(boxregions!=0 && recordRegion!=0){
    unionReg=regMan.doUnion(*boxregions, *recordRegion);
    delete boxregions; boxregions=0;
    delete recordRegion; recordRegion=0;
  }
  else if(boxregions !=0){
    unionReg=boxregions;
  }
  else if(recordRegion !=0){
    unionReg=recordRegion;
  }
      

 
  
  if(unionReg !=0){
    regionToMask(maskImage, *unionReg, value);
    delete unionReg; unionReg=0;
  }
  //As i can't unionize LCRegions and WCRegions;  do circles seperately
  if(circleregions !=0){
    regionToMask(maskImage, *circleregions, value);
    delete circleregions;
    circleregions=0;
  }
  maskImage.table().unmarkForDelete();
  return true;

}


Bool Imager::regionToMask(ImageInterface<Float>& maskImage, ImageRegion& imagreg, const Float& value){

  
  SubImage<Float> partToMask(maskImage, imagreg, true);
  LatticeRegion latReg=imagreg.toLatticeRegion(maskImage.coordinates(), maskImage.shape());
  ArrayLattice<Bool> pixmask(latReg.get());
  LatticeExpr<Float> myexpr(iif(pixmask, value, partToMask) );
  partToMask.copyData(myexpr);

  return true;
}


Bool Imager::clipimage(const String& image, const Quantity& threshold)
{
  if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "clipimage()", WHERE));
  
  this->lock();
  try {
    
    if(!assertDefinedImageParameters()) return false;
    
    if(image=="") {
      this->unlock();
      os << LogIO::SEVERE << "Need name for image" << LogIO::EXCEPTION;
      return false;
    }
    PagedImage<Float> imageImage(image);
    os << LogIO::NORMAL // Loglevel PROGRESS
       << "Zeroing " << image << ", for all pixels where Stokes I < threshold "
       << threshold.get("Jy").getValue() << "Jy " << LogIO::POST;
    
    StokesImageUtil::MaskOnStokesI(imageImage, threshold);
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  } 
  
  return true;
}

// Add together low and high resolution images in the Fourier plane
Bool Imager::feather(const String& image, const String& highRes,
		     const String& lowRes, const String& lowPSF, const Float effDishDiam, const Bool lowPassFilterSD)
{
  
  Float effDiam=effDishDiam;
  LoggerHolder lh (false);
  LogIO os = lh.logio();
  os << LogOrigin("imager", "feather()");
  
  try {
    Bool noStokes=false;
    String outLowRes=lowRes;
    String outHighRes=highRes;
    {
      if ( ! doVP_p ) {
	this->unlock();
	os << LogIO::SEVERE << 
	  "Must invoke setvp() first in order to apply the primary beam" 
	   << LogIO::EXCEPTION;
	return false;
      }
      
      os << LogIO::NORMAL // Loglevel PROGRESS
         << "\nFeathering together high and low resolution images.\n" << LogIO::POST;
      
     
      // Get initial images
      /*{ //Drat lets deal with images that don't have stokes.
	PagedImage<Float> hightemp(highRes);
	PagedImage<Float> lowtemp(lowRes);
	if(hightemp.shape().nelements() != lowtemp.shape().nelements()){
	  this->unlock();
	  os << LogIO::SEVERE << 
	    "High res. image and low res. image donot have same number of axes" 
	     << LogIO::EXCEPTION;
	  return false;
	  
	}
	if ( (hightemp.coordinates().findCoordinate(Coordinate::STOKES) < 0) &&
	     (lowtemp.coordinates().findCoordinate(Coordinate::STOKES) < 0)){
	  noStokes=true;
	  os << LogIO::NORMAL // Loglevel PROGRESS
             << "Making some temporary images as the inputs have no Stokes axis.\n" 
             << LogIO::POST;
	  PtrHolder<ImageInterface<Float> > outImage1;
	  outHighRes= highRes+"_stokes";
	  ImageUtilities::addDegenerateAxes (os, outImage1, hightemp, outHighRes,
					     false, false,
					     "I", false, false,
					     false);

	  PtrHolder<ImageInterface<Float> > outImage2;
	  outLowRes= lowRes+"_stokes";
	  ImageUtilities::addDegenerateAxes (os, outImage2, lowtemp, outLowRes,
					     false, false,
					     "I", false, false,
					     false);
	  
					     }
      }*/
      PagedImage<Float> high(outHighRes);
      PagedImage<Float> low0(outLowRes);
      
      Feather::feather(image, high, low0, sdScale_p, lowPSF, doDefaultVP_p, vpTableStr_p, effDiam, lowPassFilterSD);
    }
  
    if(noStokes){
      TableUtil::deleteTable(outHighRes);
      TableUtil::deleteTable(outLowRes);
    }
    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  } 
  
  return true;
}



Bool Imager::linearmosaic(const String& mosaic,
			  const String& fluxscale,
			  const String& sensitivity,
			  const Vector<String>& images,
			  const Vector<Int>& fieldids)

{
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "linearmosaic()", WHERE));
  try{
    if(mosaic=="") {
      os << LogIO::SEVERE << "Need name for mosaic image" << LogIO::POST;
      return false;
    }
    if(!Table::isWritable( mosaic )) {
      make( mosaic );
    }
    if (images.nelements() == 0) {
      os << LogIO::SEVERE << "Need names of images to mosaic" << LogIO::POST;
      return false;
    }
    if (images.nelements() != fieldids.nelements()) {
      os << LogIO::SEVERE << "number of fieldids doesn\'t match the" 
	 << " number of images" << LogIO::POST;
      return false;
    }
    
    Double meminMB=Double(HostInfo::memoryTotal(true))/1024.0;
    PagedImage<Float> mosaicImage( mosaic );
    CoordinateSystem cs=mosaicImage.coordinates();
    String err;
    //for some reason subimages below  fail if they are in some frames like BARY
    if(cs.setSpectralConversion(err, "LSRK")){
      mosaicImage.setCoordinateInfo(cs);
    }
    mosaicImage.set(0.0);
    TempImage<Float>  numerator( TiledShape(mosaicImage.shape(), mosaicImage.niceCursorShape()), mosaicImage.coordinates(), meminMB/2.0);
    numerator.set(0.0);
    TempImage<Float>  denominator( TiledShape(mosaicImage.shape(), mosaicImage.niceCursorShape()), mosaicImage.coordinates(), meminMB/2.0);
    denominator.set(0.0);
    ImageRegrid<Float> regridder;
    
    MSColumns msc(*ms_p);
    for (uInt i=0; i < images.nelements(); ++i) {
      if(!Table::isReadable(images(i))) {   
	os << LogIO::SEVERE << "Image " << images(i) << 
	  " is not readable" << LogIO::POST;
	return false;
      }
      
      PagedImage<Float> smallImagedisk( images(i) );
      cs=smallImagedisk.coordinates();
      //for some reason subimages below  fail if they are in some frames like BARY
      if(!cs.setSpectralConversion(err, "LSRK")){
	cs=smallImagedisk.coordinates();
      }
      
      TempImage<Float> smallImage(smallImagedisk.shape(), cs, meminMB/8.0);
      smallImage.copyData(smallImagedisk);
      IPosition iblc(smallImage.shape().nelements(),0);
      IPosition itrc(smallImage.shape());
      itrc=itrc-Int(1);
      
      LCBox lbox(iblc, itrc, smallImage.shape());
      ImageRegion imagreg(WCBox(lbox, cs) );
      try{
	// accumulate the images
	SubImage<Float> subNum;
	SubImage<Float> subDen;
	try{
	  subNum=SubImage<Float>(numerator, imagreg, true);
	  subDen=SubImage<Float>(denominator, imagreg, true);
	}
	catch(...){
	  //Failed to make a subimage let us use the full image
	  subNum=SubImage<Float>(numerator, true);
	  subDen=SubImage<Float>(denominator, true);
	  
	}
	
	
	
	TempImage<Float> fullImage(subNum.shape(), subNum.coordinates(), meminMB/8.0);
	
	os  << "Processing Image " << images(i)  << LogIO::POST;
	
	regridder.regrid( fullImage, Interpolate2D::LINEAR,
			  IPosition(2,0,1), smallImage );
	
	TempImage<Float>  PB( subNum.shape(), subNum.coordinates(), meminMB/8.0);
	PB.set(1.0);
	
	MDirection pointingDirection = msc.field().phaseDirMeas( fieldids(i) );
	
	Quantity pa(0.0, "deg");
	pbguts ( PB, PB, pointingDirection, pa);
	
	fullImage.copyData( (LatticeExpr<Float>) (fullImage *  PB ) );
	subNum.copyData( (LatticeExpr<Float>) (subNum + fullImage) );
	subDen.copyData( (LatticeExpr<Float>) (subDen + (PB*PB)) );
	
      }
      catch (AipsError x) {
	os << LogIO::WARN<< "Caught exception while processing  " << images(i) 
	   << "\n"<< x.getMesg()
	   << LogIO::POST;
	continue;
      } 
      catch(...){
	os << LogIO::WARN << "Unknown error processing " << images(i) << LogIO::POST; 
	continue;
      }
    }
    
    LatticeExprNode LEN = max( denominator );
    Float dMax =  LEN.getFloat();
    
    
    if (scaleType_p == "SAULT") {
      
      // truncate denominator at ggSMin1
      denominator.copyData( (LatticeExpr<Float>) 
			    (iif(denominator < (dMax * constPB_p), dMax, 
				 denominator) ) );
      
      if (fluxscale != "") {
	clone( mosaic, fluxscale );
	
	PagedImage<Float> fluxscaleImage( fluxscale );
	fluxscaleImage.copyData( (LatticeExpr<Float>) 
				 (iif(denominator < (dMax*minPB_p), 0.0,
				      (dMax*minPB_p)/(denominator) )) );
	fluxscaleImage.copyData( (LatticeExpr<Float>) 
				 (iif(denominator > (dMax*constPB_p), 1.0,
				      (fluxscaleImage) )) );
	mosaicImage.copyData( (LatticeExpr<Float>)(iif(denominator > (dMax*minPB_p),
						       (numerator/denominator), 0)) );
      }
    } else {
      mosaicImage.copyData( (LatticeExpr<Float>)(iif(denominator > (dMax*minPB_p),
						     (numerator/denominator), 0)) );
      if (fluxscale != "") {
	clone(mosaic, fluxscale );
	PagedImage<Float> fluxscaleImage( fluxscale );
	fluxscaleImage.copyData( (LatticeExpr<Float>)( 1.0 ) );
      }
    }
    if (sensitivity != "") {
      clone(mosaic, sensitivity);
      PagedImage<Float> sensitivityImage( sensitivity );
      sensitivityImage.copyData( (LatticeExpr<Float>)( denominator/dMax ));
    }
  }
  catch (AipsError x) {
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::POST;
    return false;
  } 
  return true;
}

// Weight the MeasurementSet
Bool Imager::weight(const String& type, const String& crmode,
                 const Quantity& noise, const Double robust,
                 const Quantity& fieldofview,
		    const Int npixels, const Bool multiField)
{
  if(!valid()) return false;
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "weight()"),logSink_p);
  
  this->lock();
  try {
    
    String rmode=crmode; // can change it 


    os << LogIO::NORMAL // Loglevel INFO
       << "Weighting MS: Imaging weights will be changed" << LogIO::POST;
    
    if (type=="natural") {
      os << LogIO::NORMAL // Loglevel INFO
         << "Natural weighting" << LogIO::POST;
      imwgt_p=VisImagingWeight("natural");
    }
    else if(type=="superuniform"){
      if(!assertDefinedImageParameters()) return false;
      ///making usage of npixels consistent with uniform, briggs
      /// don't know why this was done seperately which is kind of redundant in the code
      /// one achieves superuniform with  just uniform with npixels or fieldofview 
      /// set to non-defaults in the section below
      Int actualNpix=npixels/2;
      if(actualNpix <=0)
	actualNpix=3;
      os << LogIO::NORMAL // Loglevel INFO
         << "SuperUniform weighting over a square cell spanning [" 
	 << -actualNpix 
	 << ", " << actualNpix << "] in the uv plane" << LogIO::POST;
      imwgt_p=VisImagingWeight(*rvi_p, rmode, noise, robust, nx_p, 
                               ny_p, mcellx_p, mcelly_p, actualNpix, 
                               actualNpix, multiField);
    }
    else if ((type=="robust")||(type=="uniform")||(type=="briggs")) {
      if(!assertDefinedImageParameters()) return false;
      Quantity actualFieldOfView(fieldofview);
      Int actualNPixels(npixels);
      String wtype;
      if(type=="briggs") {
	//The user really meant to use Brigg's weighting and forgot to set norm or abs
	// guessing it should be norm
	if(rmode=="none")
	  rmode="norm";
        wtype = "Briggs";
      }
      else {
        wtype = "Uniform";
      }
      if(actualFieldOfView.get().getValue()==0.0&&actualNPixels==0) {
        actualNPixels=nx_p;
        actualFieldOfView=Quantity(actualNPixels*mcellx_p.get("rad").getValue(),
								   "rad");
        os << LogIO::NORMAL // Loglevel INFO
           << wtype
           << " weighting: sidelobes will be suppressed over full image"
           << LogIO::POST;
      }
      else if(actualFieldOfView.get().getValue()>0.0&&actualNPixels==0) {
        actualNPixels=Int(actualFieldOfView.get("rad").getValue()/mcellx_p.get("rad").getValue());
        os << LogIO::NORMAL // Loglevel INFO
           << wtype
           << " weighting: sidelobes will be suppressed over specified field of view: "
           << actualFieldOfView.get("arcsec").getValue() << " arcsec" << LogIO::POST;
      }
      else if(actualFieldOfView.get().getValue()==0.0&&actualNPixels>0) {
        actualFieldOfView=Quantity(actualNPixels*mcellx_p.get("rad").getValue(),
								   "rad");
        os << LogIO::NORMAL // Loglevel INFO
           << wtype
           << " weighting: sidelobes will be suppressed over full image field of view: "
           << actualFieldOfView.get("arcsec").getValue() << " arcsec" << LogIO::POST;
      }
      else {
        os << LogIO::NORMAL // Loglevel INFO
           << wtype
           << " weighting: sidelobes will be suppressed over specified field of view: "
           << actualFieldOfView.get("arcsec").getValue() << " arcsec" << LogIO::POST;
      }
      os << LogIO::DEBUG1
         << "Weighting used " << actualNPixels << " uv pixels."
         << LogIO::POST;
      Quantity actualCellSize(actualFieldOfView.get("rad").getValue()/actualNPixels, "rad");

      imwgt_p=VisImagingWeight(*rvi_p, rmode, noise, robust, 
                               actualNPixels, actualNPixels, actualCellSize, 
                               actualCellSize, 0, 0, multiField);
      
    }
    else if (type=="radial") {
      os << "Radial weighting" << LogIO::POST;
      imwgt_p=VisImagingWeight("radial");
    }
    else {
      this->unlock();
      os << LogIO::SEVERE << "Unknown weighting " << type
         << LogIO::EXCEPTION;    
      return false;
    }
    
      rvi_p->useImagingWeight(imwgt_p);
    
    // Beam is no longer valid
    beamValid_p=false;
    destroySkyEquation();
    this->writeHistory(os);
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  } 
  
  return true;
}


Bool Imager::getWeightGrid(Block<Matrix<Float> >&weightgrid, const String& type, const Vector<String>&imagenames){

  if(type=="imaging"){
    weightgrid.resize(0, true, false);
    if(imwgt_p.getType()!="uniform")
      return false;
    imwgt_p.getWeightDensity(weightgrid);
    return true;
  }
  if((type=="ftweight") && (sm_p) && (Int(imagenames.nelements())== sm_p->numberOfModels())){
    for (Int model=0; model < sm_p->numberOfModels(); ++model){
      PagedImage<Float> wgtImage(sm_p->image(model).shape(),
				   (sm_p->image(model)).coordinates(),
				   imagenames(model));
      se_p->getWeightImage(model, wgtImage);
      

    }
    return true;
  } 
        
  return false;
}

Bool Imager::setWeightGrid(const Block<Matrix<Float> >& weightgrid, const String& type){

  if(type=="imaging"){
    if(imwgt_p.getType()!="uniform")
      return false;
    imwgt_p.setWeightDensity(weightgrid);
    rvi_p->useImagingWeight(imwgt_p);
  }

  return true;
}

// Filter the MeasurementSet
Bool Imager::filter(const String& type, const Quantity& bmaj,
		 const Quantity& bmin, const Quantity& bpa)
{
  if(!valid()) return false;
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "filter()"),logSink_p);
  
  this->lock();
  try {
      
    os << LogIO::NORMAL // Loglevel INFO
       << "Imaging weights will be tapered" << LogIO::POST;
    imwgt_p.setFilter(type, bmaj, bmin, bpa);
    rvi_p->useImagingWeight(imwgt_p);
      
    // Beam is no longer valid
    beamValid_p=false;
    destroySkyEquation();
    this->writeHistory(os);
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  } 
  
  return true;
}


// Implement a uv range
Bool Imager::uvrange(const Double& uvmin, const Double& uvmax)
{
  if(!valid()) return false;
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "uvrange()"),logSink_p);
  
  try {
    os << LogIO::NORMAL // Loglevel INFO
       << "Selecting data according to  uvrange: setdata will reset this selection"
       << LogIO::POST;

    Double auvmin(uvmin);
    Double auvmax(uvmax);

    if(auvmax<=0.0) auvmax=1e10;
    if(auvmax>auvmin&&(auvmin>=0.0)) {
      os << LogIO::NORMAL // Loglevel INFO
         << "Allowed uv range: " << auvmin << " to " << auvmax
	 << " wavelengths" << LogIO::POST;
    }
    else {
      os << LogIO::SEVERE << "Invalid uvmin and uvmax: "
	 << auvmin << ", " << auvmax
	 << LogIO::EXCEPTION;
      return false;
    }
    Vector<Double> freq;
    ostringstream strUVmax, strUVmin, ostrInvLambda;

    this->lock();
      
    if(mssel_p.null()){ os << "Please setdata first before using uvrange " << LogIO::POST; return false; }


     // use the average wavelength for the selected windows to convert
     // uv-distance from lambda to meters
     ostringstream spwsel;
     spwsel << "select from $1 where ROWID() IN [";
     for(uInt i=0; i < dataspectralwindowids_p.nelements(); ++i) {
	 if (i > 0) spwsel << ", ";
	 spwsel << dataspectralwindowids_p(i);
     }
     spwsel << "]";

     MSSpectralWindow msspw(tableCommand(spwsel.str(), 
					 mssel_p->spectralWindow()));
     MSSpWindowColumns spwc(msspw);

     // This averaging scheme will work even if the spectral windows are
     // of different sizes.  Note, however, that using an average wavelength
     // may not be a good choice when the total range in frequency is 
     // large (e.g. mfs across double sidebands).
     uInt nrows = msspw.nrow();
     Double ftot = 0.0;
     Int nchan = 0;
     for(uInt i=0; i < nrows; ++i) {
	 nchan += (spwc.numChan())(i);
	 ftot += sum((spwc.chanFreq())(i));
     }
     Double invLambda=ftot/(nchan*C::c);

     // This is message may not be helpful as mfs is set with setimage()
     // which may sometimes get called after uvrange()
     if (nrows > 1 && imageMode_p=="MFS") {
 	 os << LogIO::WARN 
 	    << "When using mfs over a broad range of frequencies, It is more "
 	    << "accurate to " << endl 
 	    << "constrain uv-ranges using setdata(); try: " << endl 
 	    << "  msselect='(SQUARE(UVW[1]) + SQUARE(UVW[2])) > uvmin && "
 	    << "(SQUARE(UVW[1]) + SQUARE(UVW[2])) < uvmax'" << endl
 	    << "where [uvmin, uvmax] is the range given in meters." 
 	    << LogIO::POST;
     }

     invLambda=invLambda*invLambda;
     auvmax=auvmax*auvmax;
     auvmin=auvmin*auvmin;
     strUVmax << auvmax; 
     strUVmin << auvmin;
     ostrInvLambda << invLambda; 
     String strInvLambda=ostrInvLambda;
     MeasurementSet* mssel_p2;

     // Apply the TAQL selection string, to remake the selected MS
     String parseString="select from $1 where (SQUARE(UVW[1]) + SQUARE(UVW[2]))*" + (string) strInvLambda + " > " + strUVmin.str( ) + " &&  (SQUARE(UVW[1]) + SQUARE(UVW[2]))*" + (string) strInvLambda + " < " + strUVmax.str( );

     mssel_p2=new MeasurementSet(tableCommand(parseString,*mssel_p));
     AlwaysAssert(mssel_p2, AipsError);
     // Rename the selected MS as */SELECTED_UVRANGE
     //mssel_p2->rename(msname_p+"/SELECTED_UVRANGE", Table::Scratch);
      
     if (mssel_p2->nrow()==0) {
	 os << LogIO::WARN
	    << "Selection string results in empty MS: "
	    << "reverting to sorted MeasurementSet"
	    << LogIO::POST;
	 delete mssel_p2;
     } else {
       if (!mssel_p.null()) {
	     os << LogIO::NORMAL // Loglevel INFO
                << "By UVRANGE selection previously selected number of rows "
                << mssel_p->nrow() << "  are now reduced to "
                << mssel_p2->nrow() << LogIO::POST; 
	     //delete mssel_p; 
	     mssel_p=mssel_p2;
	     mssel_p->flush();
	 }
     }
      
     
     this->makeVisSet(*mssel_p);
     AlwaysAssert(rvi_p, AipsError);

     // NOW WE HAVE TO REDO THE VELOCITY INFO FOR visiter AS IN SETDATA

     this->selectDataChannel(dataspectralwindowids_p, dataMode_p,
                             dataNchan_p, dataStart_p, dataStep_p,
                             mDataStart_p, mDataStep_p);

     this->writeHistory(os);
     this->unlock();
     
     // Beam is no longer valid
     beamValid_p=false;
     return true;    
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  } 
  return true;
}

// Find the sensitivity
Bool Imager::sensitivity(Quantity& pointsourcesens, Double& relativesens,
			 Double& sumwt,
			 Double& effectiveBandwidth,
			 Double& effectiveIntegration,
			 Int& nBaselines,
			 Matrix<Int>& mssChanSel,
			 Vector<Vector<Int> >& nData,
			 Vector<Vector<Double> >& sumwtChan,
			 Vector<Vector<Double> >& sumwtsqChan,
			 Vector<Vector<Double> >& sumInverseVarianceChan)
{
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "sensitivity()", WHERE));
  
  try {
    
    os << LogIO::NORMAL // Loglevel INFO
       << "Calculating sensitivity from imaging weights and from effective bandwidth and integration time"
       << LogIO::POST;
    
    this->lock();
    mssChanSel.assign(mssChanSel_p);

    VisSetUtil::Sensitivity(*rvi_p, mssFreqSel_p, mssChanSel, pointsourcesens, relativesens, sumwt,
			    effectiveBandwidth, effectiveIntegration, nBaselines,nData, sumwtChan, 
			    sumwtsqChan, sumInverseVarianceChan);
    os << LogIO::NORMAL << "RMS Point source sensitivity  : " // Loglevel INFO
       << pointsourcesens.get("Jy").getValue() << " Jy.m2/K"
       << LogIO::POST;
    os << LogIO::NORMAL // Loglevel INFO
       << "Relative to natural weighting : " << relativesens << LogIO::POST;
    os << LogIO::NORMAL // Loglevel INFO
       << "Sum of weights                : " << sumwt << LogIO::POST;
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  } 
  return true;
}

Bool Imager::apparentSensitivity(Double& effSensitivity,
				 Double& relToNat) {
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "apparentSensitivity()", WHERE));
  
  try {

    os << LogIO::NORMAL // Loglevel INFO
       << "Calculating apparent sensitivity from MS weights, as modified by gridding weight function"
       << LogIO::POST;
    os << LogIO::NORMAL // Loglevel INFO
       << "(assuming that MS weights have correct scale and units)"
       << LogIO::POST;
    
    this->lock();
    
    Double sumNatWt=0.0;
    Double sumGridWt=0.0;
    Double sumGridWt2OverNatWt=0.0;
    
    Float iNatWt(0.0),iGridWt(0.0);
    
    ROVisibilityIterator& vi(*rvi_p);
    VisBuffer vb(vi);
    //Bool doWtSp=vb.existsWeightSpectrum();
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	Int nRow=vb.nRow();
	Vector<Bool> rowFlags=vb.flagRow();
	Matrix<Bool> flag = vb.flag();
	
	Vector<Int>& a1(vb.antenna1()), a2(vb.antenna2());
	
	for (Int row=0; row<nRow; row++) {
	  if (!rowFlags(row) && a1(row)!=a2(row)) {  // exclude ACs
	    iNatWt=2.0f*vb.weight()(row);
	    for (Int ich=0;ich<vb.nChannel();++ich) {
	      if(!flag(ich,row)&&(iNatWt>0.0)) {
		iGridWt=2.0f*vb.imagingWeight()(ich,row);
		sumNatWt+=(iNatWt);
		sumGridWt+=(iGridWt);
		sumGridWt2OverNatWt+=(iGridWt*iGridWt/iNatWt);
	      }
	    }
	  }
	}
      }
    }
    
    if (sumNatWt==0.0) {
      os << "Cannot calculate sensitivity: sum of selected natural weights is zero" << LogIO::EXCEPTION;
    }
    if (sumGridWt==0.0) {
      os << "Cannot calculate sensitivity: sum of gridded weights is zero" << LogIO::EXCEPTION;
    }

    effSensitivity = sqrt(sumGridWt2OverNatWt)/sumGridWt;
    Double natSensitivity = 1.0/sqrt(sumNatWt);
    relToNat=effSensitivity/natSensitivity;

    os << LogIO::NORMAL << "RMS Point source sensitivity  : " // Loglevel INFO
       << effSensitivity      //  << " Jy/beam"       // actually, units are arbitrary
       << LogIO::POST;
    os << LogIO::NORMAL // Loglevel INFO
       << "Relative to natural weighting : " << relToNat << LogIO::POST;

    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  } 
  return true;
    
}





// Calculate various sorts of image. Only one image
// can be calculated at a time. The complex Image make
// be retained if a name is given. This does not use
// the SkyEquation.
Bool Imager::makeimage(const String& type, const String& image,
                       const String& compleximage, const Bool verbose)
{
  if(!valid()) 
    {
      return false;
    }
  LogIO os(LogOrigin("imager", "makeimage()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters())
      {
	return false;
      }
    
    os << LogIO::NORMAL // Loglevel INFO
       << "Calculating image (without full skyequation)" << LogIO::POST;
    
    FTMachine::Type seType(FTMachine::OBSERVED);
    Bool doSD(false);

    if(type=="observed") {
      seType=FTMachine::OBSERVED;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making dirty image from " << type << " data "
	 << LogIO::POST;
    }
    else if (type=="model") {
      seType=FTMachine::MODEL;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making dirty image from " << type << " data "
	 << LogIO::POST;
    }
    else if (type=="corrected") {
      seType=FTMachine::CORRECTED;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making dirty image from " << type << " data "
	 << LogIO::POST;
    }
    else if (type=="psf") {
      seType=FTMachine::PSF;
      os << "Making point spread function "
	 << LogIO::POST;
    }
    else if (type=="residual") {
      seType=FTMachine::RESIDUAL;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making dirty image from " << type << " data "
	 << LogIO::POST;
    }
    else if (type=="singledish-observed") {
      doSD = true;
      seType=FTMachine::OBSERVED;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making single dish image from observed data" << LogIO::POST;
    }
    else if (type=="singledish") {
      doSD = true;
      seType=FTMachine::CORRECTED;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making single dish image from corrected data" << LogIO::POST;
    }
    else if (type=="coverage") {
      doSD = true;
      seType=FTMachine::COVERAGE;
      os << LogIO::NORMAL // Loglevel PROGRESS
         << "Making single dish coverage function "
	 << LogIO::POST;
    }
    else if (type=="holography") {
      doSD = true;
      seType=FTMachine::CORRECTED;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making complex holographic image from corrected data "
	 << LogIO::POST;
    }
    else if (type=="holography-observed") {
      doSD = true;
      seType=FTMachine::OBSERVED;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making complex holographic image from observed data "
	 << LogIO::POST;
    }
    else if (type=="pb"){
      if ( ! doVP_p ) {
        if( ftmachine_p == "pbwproject" ){
	   os << LogIO::WARN << "Using pb from ft-machines" << LogIO::POST;
	}
	else{
	  this->unlock();
	  os << LogIO::SEVERE << 
	    "Must invoke setvp() first in order to make its image" 
	     << LogIO::EXCEPTION;
	  return false;
	}
      }
      CoordinateSystem coordsys;
      //imagecoordinates(coordsys, verbose);
      imagecoordinates2(coordsys, verbose);
      if (doDefaultVP_p) {
	if(telescope_p!=""){
	  ObsInfo myobsinfo=this->latestObsInfo();
	  myobsinfo.setTelescope(telescope_p);
	  coordsys.setObsInfo(myobsinfo);
	  
	}
	else{
	  telescope_p=coordsys.obsInfo().telescope();
	}
	this->unlock();
	MSAntennaColumns ac(ms_p->antenna());
	Double dishDiam=ac.dishDiameter()(0);
	if(!allEQ(ac.dishDiameter().getColumn(), dishDiam))
	  os << LogIO::WARN
         << "The MS has multiple antenna diameters ..PB could be wrong "
	 << LogIO::POST;
        return this->makePBImage(coordsys, telescope_p, image, false, dishDiam);
      }
      else{
	Table vpTable(vpTableStr_p);
	this->unlock();
        return this->makePBImage(coordsys, vpTable, image);	
      }

    }
    else {
      this->unlock();
      os << LogIO::SEVERE << "Unknown image type " << type << LogIO::EXCEPTION;

      return false;
    }

    if(doSD && (ftmachine_p == "ft")){
      os << LogIO::SEVERE
         << "To make single dish images, ftmachine in setoptions must be set to either sd or both"
	 << LogIO::EXCEPTION;
    }
    
    // Now make the images. If we didn't specify the names then
    // delete on exit.
    String imageName(image);
    if(image=="") {
      imageName=Imager::imageName()+".image";
    }
    os << LogIO::NORMAL << "Image is : " << imageName << LogIO::POST; // Loglevel INFO
    Bool keepImage=(image!="");
    Bool keepComplexImage=(compleximage!="")||(type=="holography")||(type=="holography-observed");
    String cImageName(compleximage);

    if(compleximage=="") {
      cImageName=imageName+".compleximage";
    }

    if(keepComplexImage) {
      os << "Retaining complex image: " << compleximage << LogIO::POST;
    }

    CoordinateSystem imagecoords;
    //if(!imagecoordinates(imagecoords, false))
    if(!imagecoordinates2(imagecoords, false))
      {
	return false;
      }
    make(imageName);
    PagedImage<Float> imageImage(imageName);
    imageImage.set(0.0);
    imageImage.table().markForDelete();
    
    // Now set up the tile size, here we guess only
    IPosition cimageShape(imageshape());
    Int tilex=32;
    if(imageTileVol_p >0){
      tilex=static_cast<Int>(ceil(sqrt(imageTileVol_p/min(4,
                                                          cimageShape(2))/min(32,
                                                                              cimageShape(3)))));
      if(tilex >0){
	if(tilex > min(Int(cimageShape(0)), Int(cimageShape(1))))
	  tilex=min(Int(cimageShape(0)), Int(cimageShape(1)));
	else
	  tilex=cimageShape(0)/Int(cimageShape(0)/tilex);
      }
      //Not too small in x-y tile
      if(tilex < 10)
	tilex=10;
   
    }
    IPosition tileShape(4, min(tilex, cimageShape(0)), min(tilex, cimageShape(1)),
			min(4, cimageShape(2)), min(32, cimageShape(3)));
    CoordinateSystem cimagecoords;
    //if(!imagecoordinates(cimagecoords, false))
    if(!imagecoordinates2(cimagecoords, false))
      {
	return false;
      }


    
    PagedImage<Complex> cImageImage(TiledShape(cimageShape, tileShape),
				    cimagecoords,
				    cImageName);
    cImageImage.set(Complex(0.0));
    cImageImage.setMaximumCacheSize(cache_p/2);
    cImageImage.table().markForDelete();
    //
    // Add the distance to the object: this is not nice. We should define the
    // coordinates properly.
    //
    Record info(imageImage.miscInfo());
    info.define("distance", distance_p.get("m").getValue());
    cImageImage.setMiscInfo(info);

    
    String ftmachine(ftmachine_p);
    if (!ft_p)
      createFTMachine();
    
    // Now make the required image
    Matrix<Float> weight;
    ft_p->makeImage(seType, *rvi_p, cImageImage, weight);
    StokesImageUtil::To(imageImage, cImageImage);
    //
    // Dirty way to set the proper unit to SD image
    //
    String msunit("");
    String imunit;
    if ( ms_p->tableDesc().isColumn("DATA") ){
      msunit = ms_p->columnUnit(MS::DATA);
      if (msunit == String("")) {
	ColumnDesc dataColDesc(ms_p->tableDesc().columnDesc("DATA"));
	if (dataColDesc.keywordSet().isDefined("UNIT"))
	  msunit = dataColDesc.keywordSet().asString("UNIT");
      }
    } else if ( ms_p->tableDesc().isColumn("FLOAT_DATA")) {
      msunit = ms_p->columnUnit(MS::FLOAT_DATA);
      if (msunit == String("")) {
	ColumnDesc dataColDesc(ms_p->tableDesc().columnDesc("FLOAT_DATA"));
	if (dataColDesc.keywordSet().isDefined("UNIT"))
	  msunit = dataColDesc.keywordSet().asString("UNIT");
      }
    }
    msunit.upcase();
    if (msunit == String("K"))
      imunit = "K";
    else
      imunit = "Jy/beam";
    imageImage.setUnits(Unit(imunit));
    cImageImage.setUnits(Unit(imunit));
//     imageImage.setUnits(Unit("Jy/beam"));
//     cImageImage.setUnits(Unit("Jy/beam"));

    if(keepImage) {
      imageImage.table().unmarkForDelete();
    }
    if(keepComplexImage) {
      cImageImage.table().unmarkForDelete();
    }
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  }
  catch(...){
    //Unknown exception...
    throw(AipsError("Unknown exception caught ...imager/casa may need to be exited"));
  }
  this->unlock();

  return true;
}  

// Restore: at least one model must be supplied
Bool Imager::restore(const Vector<String>& model,
		     const String& complist,
		     const Vector<String>& image,
		     const Vector<String>& residual)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "restore()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) return false;
    
    if(image.nelements()>model.nelements()) {
      this->unlock();
      os << LogIO::SEVERE << "Cannot specify more output images than models"
	 << LogIO::EXCEPTION;
      return false;
    }
    else {
      os << LogIO::NORMAL // Loglevel PROGRESS
         << "Restoring " << model.nelements() << " models" << LogIO::POST;
    }

    ///if the skymodel is already set...no need to get rid of the psf and ftmachine state
    //as long as the images match
    if(!redoSkyModel_p){
      Bool coordMatch=true; 
      for (Int thismodel=0;thismodel<Int(model.nelements());++thismodel) {
	CoordinateSystem cs=(sm_p->image(thismodel)).coordinates();
	coordMatch= coordMatch || checkCoord(cs, model(thismodel));
      }
      if(!coordMatch)
	destroySkyEquation();
    }
    
    if(redoSkyModel_p){
      Vector<String> imageNames(image);
      if(image.nelements()<model.nelements()) {
	imageNames.resize(model.nelements());
	for(Int i=0;i<Int(model.nelements()); ++i) {
	  imageNames(i)="";
	}
      }
      
      for (Int thismodel=0;thismodel<Int(model.nelements());++thismodel) {
	if(imageNames(thismodel)=="") {
	  imageNames(thismodel)=model(thismodel)+".restored";
	}
	removeTable(imageNames(thismodel));
	if(imageNames(thismodel)=="") {
	  this->unlock();
	  os << LogIO::SEVERE << "Illegal name for output image "
	     << imageNames(thismodel) << LogIO::EXCEPTION;
	  return false;
	}
	if(!clone(model(thismodel), imageNames(thismodel))) return false;
      }
      
      Vector<String> residualNames(residual);
      if(residual.nelements()<model.nelements()) {
	residualNames.resize(model.nelements());
	for(Int i=0;i<Int(model.nelements());++i) {
	  residualNames(i)="";
	}
      }

      for (Int thismodel=0;thismodel<Int(model.nelements()); ++thismodel) {
	if(residualNames(thismodel)=="")
	  residualNames(thismodel)=model(thismodel)+".residual";
	removeTable(residualNames(thismodel));
	if(residualNames(thismodel)=="") {
	  this->unlock();
	  os << LogIO::SEVERE << "Illegal name for output residual "
	     << residualNames(thismodel) << LogIO::EXCEPTION;
	  return false;
	}
	if(!clone(model(thismodel), residualNames(thismodel))) return false;
      }
    
      if(beamValid_p) {
	os << LogIO::NORMAL << "Using previous beam fit" << LogIO::POST; // Loglevel INFO
      }
      else {
	os << LogIO::NORMAL // Loglevel INFO
           << "Calculating PSF using current parameters" << LogIO::POST;
	String psf;
	psf=imageNames(0)+".psf";
	if(!clone(imageNames(0), psf)) return false;
	Imager::makeimage("psf", psf);
	fitpsf(psf, beam_p);
	beamValid_p=true;
      }

      //      if (!se_p)
      if(!createSkyEquation(model, complist)) return false;
      
      addResiduals(residualNames);
    }
    sm_p->solveResiduals(*se_p);
    for (uInt k=0 ; k < residuals_p.nelements(); ++k){
      residuals_p[k]->copyData(sm_p->getResidual(k));
    }
    restoreImages(image);
    
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  } 
  this->unlock();
  return true;
}

Bool Imager::updateresidual(const Vector<String>& model,
		     const String& complist,
		     const Vector<String>& image,
		     const Vector<String>& residual)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "updateresidual()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) return false;
    
    if(image.nelements()>model.nelements()) {
      this->unlock();
      os << LogIO::SEVERE << "Cannot specify more output images than models"
	 << LogIO::EXCEPTION;
      return false;
    }
    else {
      os << LogIO::NORMAL // Loglevel PROGRESS
         << "updating and restoring " << model.nelements() << " models" << LogIO::POST;
    }

    if(redoSkyModel_p)
      throw(AipsError("use restore instead of updateresidual"));
    if(!updateSkyModel(model, complist))
	throw(AipsError("Could not do an updateresidual please use restore"));      
    addResiduals(residual);
    for (Int thismodel=0;thismodel<Int(residuals_p.nelements());++thismodel) {
      if(!residuals_p[thismodel].null()) 
	sm_p->addResidual(thismodel, *residuals_p[thismodel]);   
    }
    sm_p->solveResiduals(*se_p);
    /*for (uInt k=0 ; k < residuals_p.nelements(); ++k){
      residuals_p[k]->copyData(sm_p->getResidual(k));
    }
    */
    restoreImages(image);
    

    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  } 
  this->unlock();
  return true;
}

// Residual
Bool Imager::residual(const Vector<String>& model,
		      const String& complist,
		      const Vector<String>& image)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "residual()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) return false;
    os << LogIO::NORMAL // Loglevel INFO
       << "Calculating residual image using full sky equation" << LogIO::POST;
    Vector<String> theModels=model;

    Bool deleteModel=false;

    if(model.nelements()==1 && model[0]=="" && complist != "" 
       && image.nelements()==1){

      //      A component list with no model passed...
      theModels.resize(1);
      theModels[0]="Imager_Scratch_model";
      make(theModels[0]);
      deleteModel=true;
    }

    if(image.nelements()>theModels.nelements()) {
      this->unlock();
      os << LogIO::SEVERE << "Cannot specify more output images than models"
	 << LogIO::EXCEPTION;
      return false;
    }
    else {
      os << LogIO::NORMAL << "Finding residuals for " << theModels.nelements() // Loglevel INFO
	 << " models" << LogIO::POST;
    }
    
    Vector<String> imageNames(image);
    if(image.nelements()<theModels.nelements()) {
      imageNames.resize(model.nelements());
      for(Int i=Int(image.nelements());i<Int(theModels.nelements());++i) {
	imageNames(i)="";
      }
    }

    for (Int thismodel=0;thismodel<Int(theModels.nelements()); ++thismodel) {
      if(imageNames(thismodel)=="")
	imageNames(thismodel)=model(thismodel)+".residual";
      removeTable(imageNames(thismodel));
      if(imageNames(thismodel)=="") {
	this->unlock();
	os << LogIO::SEVERE << "Illegal name for output image "
	   << imageNames(thismodel) << LogIO::EXCEPTION;
	return false;
      }
      if(!clone(theModels(thismodel), imageNames(thismodel))) return false;
    }
    destroySkyEquation();
    if(!createSkyEquation(theModels, complist)) return false;
    
    addResidualsToSkyEquation(imageNames);
    
    sm_p->solveResiduals(*se_p);
    destroySkyEquation();
    if(deleteModel) 
      removeTable(theModels[0]);
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  } 
  this->unlock();
  return true;
}

// Residual
Bool Imager::approximatepsf(const String& psf)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "approximatepsfs()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) return false;
    os << LogIO::NORMAL // Loglevel INFO
       << "Calculating approximate PSFs using full sky equation" << LogIO::POST;
    
 
    if(psf==""){
      this->unlock();
      os << LogIO::SEVERE << "Illegal name for output psf "
	 << psf << LogIO::EXCEPTION;
      return false;
    } 
    removeTable(psf);
    make(psf);
    
    Vector<String>onepsf(1,psf);
    // Previous SkyEquation if they exist is not useful
    destroySkyEquation();
    //    if (!se_p)
    // As we are not going to make any use of a useful model and to economize 
    // temporary image...using the psf itself as model...
    // need to change this if you donot destroy the skyequation after you're done.
    if(!createSkyEquation(onepsf)) return false;
    
    sm_p->makeApproxPSFs(*se_p);
    

    PagedImage<Float> elpsf(psf);
    elpsf.copyData(sm_p->PSF(0));
    ImageBeamSet mbeam;
    StokesImageUtil::FitGaussianPSF(elpsf, mbeam);
    LatticeExprNode sumPSF = sum(elpsf);
    Float volume=sumPSF.getFloat();
    GaussianBeam elbeam=mbeam(0,0);
    os << LogIO::NORMAL << "Approximate PSF  "  << ": size " // Loglevel INFO
       << elbeam.getMajor("arcsec") << " by "
       << elbeam.getMinor("arcsec") << " (arcsec) at pa "
       << elbeam.getPA(Unit("deg")) << " (deg)" << endl
       << "and volume = " << volume << " pixels " << LogIO::POST;
    
    
    destroySkyEquation();
    if(ft_p)
      delete ft_p;
    ft_p=0;

    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    throw(x);
    return false;
  } 
  this->unlock();
  return true;
}

Bool Imager::smooth(const Vector<String>& model, 
		    const Vector<String>& image, Bool usefit, 
		    ImageBeamSet& mbeam,
		    Bool normalizeVolume)
{
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "smooth()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) return false;
    
    os << LogIO::NORMAL << "Smoothing image" << LogIO::POST; // Loglevel PROGRESS
    
    if(model.nelements()>0) {
      for ( uInt thismodel=0;thismodel<model.nelements(); ++thismodel) {
	if(model(thismodel)=="") {
	  this->unlock();
	  os << LogIO::SEVERE << "Need a name for model " << thismodel << LogIO::POST;
	  return false;
	}
      }
    }
    
    if(image.nelements()>model.nelements()) {
      this->unlock();
      os << LogIO::SEVERE << "Cannot specify more output images than models" << LogIO::POST;
      return false;
    }
    
    if(usefit) {
      if(beamValid_p) {
	os << LogIO::NORMAL << "Using previous beam" << LogIO::POST; // Loglevel INFO
	mbeam = beam_p;

      }
      else {
	os << LogIO::NORMAL // Loglevel INFO
           << "Calculating PSF using current parameters" << LogIO::POST;
	String psf;
	psf=model(0)+".psf";
	if(!clone(model(0), psf)) return false;
	Imager::makeimage("psf", psf);
	fitpsf(psf, mbeam);
	beam_p = mbeam;
	beamValid_p=true;
      }
    }
    
    // Smooth all the images
    Vector<String> imageNames(image);
    for (Int thismodel=0;thismodel<Int(image.nelements()); ++thismodel) {
      if(imageNames(thismodel)=="") {
        imageNames(thismodel)=model(thismodel)+".smoothed";
      }
      PagedImage<Float> modelImage(model(thismodel));
      PagedImage<Float> imageImage(TiledShape(modelImage.shape(), 
					      modelImage.niceCursorShape()),
				   modelImage.coordinates(),
				   imageNames(thismodel));
      imageImage.table().markForDelete();
      imageImage.copyData(modelImage);
      StokesImageUtil::Convolve(imageImage, mbeam,
				normalizeVolume);
      
      ImageInfo ii = imageImage.imageInfo();
      //ii.setRestoringBeam(mbeam);
      ii.setBeams(mbeam);
      imageImage.setImageInfo(ii);
      imageImage.setUnits(Unit("Jy/beam"));
      imageImage.table().unmarkForDelete();
    }
    
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
  this->unlock();
  return true;
}

// Clean algorithm
Record Imager::clean(const String& algorithm,
		   const Int niter, 
		   const Float gain,
		   const Quantity& threshold, 
		   const Bool /*displayProgress*/,
		   const Vector<String>& model, const Vector<Bool>& fixed,
		   const String& complist,
		   const Vector<String>& mask,
		   const Vector<String>& image,
		   const Vector<String>& residual,
		   const Vector<String>& psfnames,
                   const Bool firstrun)
{
  ////////////////////////
  //Double wtime0=omp_get_wtime();
  //////////////////////


  Record retval;
  Bool converged=true;
  retval.define("converged", false);
  retval.define("iterations", Int(0));
  retval.define("maxresidual", Float(0.0));
  


  //ROVisibilityIterator::AsyncEnabler enabler (rvi_p);

  if(!valid())
    {
      return retval;
    }
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "clean()"),logSink_p);
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) 
      {
	return retval;
      }
    
    Int nmodels=model.nelements();
    os << LogIO::DEBUG1
       << "Found " << nmodels << " specified model images" << LogIO::POST;
    
    if(model.nelements()>0) {
      for (uInt thismodel=0;thismodel<model.nelements(); ++thismodel) {
	if(model(thismodel)=="") {
	  this->unlock();
	  os << LogIO::SEVERE << "Need a name for model "
	     << thismodel << LogIO::POST;

	  return retval;
	}
      }
    }
    
    Vector<String> modelNames=model;
    // Make first image with the required shape and coordinates only if
    // it doesn't exist yet. Otherwise we'll throw an exception later
    if(modelNames(0)=="") modelNames(0)=imageName()+".clean";
    if(!Table::isWritable(modelNames(0))) {
      make(modelNames(0));
    }
    else{
      Bool coordMatch=false;
      CoordinateSystem coordsys;
      //imagecoordinates(coordsys, firstrun);
      imagecoordinates2(coordsys, firstrun);
      for (uInt modelNum=0; modelNum < modelNames.nelements(); ++modelNum){
	if(Table::isWritable(modelNames(modelNum))){
	  coordMatch= coordMatch || 
	    (this->checkCoord(coordsys, modelNames(modelNum)));
				     
	}
	  
      } 
      if(!coordMatch){
	os << LogIO::WARN << "The model(s) image exists on disk " 
	   << LogIO::POST;
	os << LogIO::WARN 
	   << "The coordinates or shape were found not to match the one "
	   << "defined by setimage " 
	   << LogIO::POST;

	os << LogIO::WARN 
	   << "Cleaning process is going to ignore setimage parameters and "
	   << "continue cleaning from from model on disk " 
	   << LogIO::POST;
      }
    }
    Vector<String> maskNames(nmodels);
    if(Int(mask.nelements())==nmodels) {
      maskNames=mask;
    }
    else {
      /* For msmfs, the one input mask PER FIELD must be replicated for all 
	 Taylor-planes PER FIELD */
      if(algorithm=="msmfs" && (Int(mask.nelements())>=(nmodels/ntaylor_p)) ){
       for(Int tay=0;tay<nmodels;tay++)
	 {
	   maskNames[tay] = mask[ tay%(nmodels/ntaylor_p)  ];
	 }
      }
      else {
	 /* No mask */
	 maskNames="";
      }
    }

    if(sm_p){
      if( sm_p->getAlgorithm() != "clean") destroySkyEquation();
      if(images_p.nelements() != uInt(nmodels)){
	destroySkyEquation();
      }
      else{
	for (Int k=0; k < nmodels ; ++k){
	  if(!(images_p[k]->name().contains(modelNames[k]))) destroySkyEquation();
	}
      }
    }

    // Always fill in the residual images
    Vector<String> residualNames(nmodels);
    if(Int(residual.nelements())==nmodels) {
	residualNames=residual;
    }
    else {
      residualNames="";	
    }
    for (Int thismodel=0;thismodel<Int(model.nelements());++thismodel) {
      if(residualNames[thismodel]=="")
	residualNames(thismodel)=modelNames(thismodel)+".residual";
    }
    if(redoSkyModel_p){
      for (Int thismodel=0;thismodel<Int(model.nelements());++thismodel) {
	removeTable(residualNames(thismodel));
	if(!clone(model(thismodel), residualNames(thismodel)))
	  {
	    return retval;
	  }
      }
    }

    
    // Make an ImageSkyModel with the specified polarization representation
    // (i.e. circular or linear)

    if( redoSkyModel_p || !sm_p){
      if(sm_p) delete sm_p;
      if(algorithm.substr(0,5)=="clark") {
	// Support serial and parallel specializations
	setClarkCleanImageSkyModel();
	if(algorithm.contains("stokes"))
          sm_p->setJointStokesClean(false);
	os << LogIO::NORMAL // Loglevel INFO.        Stating the algo is more for
           << "Using Clark clean" << LogIO::POST; // the logfile than the window.
      }
      else if (algorithm=="hogbom") {
	sm_p = new HogbomCleanImageSkyModel();
	os << LogIO::NORMAL // Loglevel INFO.         Stating the algo is more for
           << "Using Hogbom clean" << LogIO::POST; // the logfile than the window.
      }
      else if (algorithm=="wfhogbom") {
	setWFCleanImageSkyModel();
	sm_p->setSubAlgorithm("hogbom");
	doMultiFields_p = true;
	doMultiFields_p = false;
	os << LogIO::NORMAL // Loglevel INFO
           << "Using wide-field algorithm with Hogbom clean" << LogIO::POST;
      }
      else if (algorithm=="multiscale") {
	if (!scaleInfoValid_p) {
	  this->unlock();
	  os << LogIO::SEVERE << "Scales not yet set" << LogIO::POST;
	  return retval;
	}
	if (scaleMethod_p=="uservector") {	
	  sm_p = new MSCleanImageSkyModel(userScaleSizes_p, stoplargenegatives_p, 
					    stoppointmode_p, smallScaleBias_p);
	} else {
	  sm_p = new MSCleanImageSkyModel(nscales_p, stoplargenegatives_p, 
					    stoppointmode_p, smallScaleBias_p);
	}
	if(ftmachine_p=="mosaic" ||ftmachine_p=="wproject" )
	  sm_p->setSubAlgorithm("full");
	os << LogIO::NORMAL // Loglevel INFO.             Stating the algo is more for
           << "Using multiscale clean" << LogIO::POST; // the logfile than the window.
      }
      else if (algorithm.substr(0,7)=="mfclark" || algorithm=="mf") {
	sm_p = new MFCleanImageSkyModel();
	sm_p->setSubAlgorithm("clark");
	if(algorithm.contains("stokes"))
          sm_p->setJointStokesClean(false);

	doMultiFields_p = true;
	os << LogIO::NORMAL << "Using multifield Clark clean" << LogIO::POST; // Loglevel INFO
      }
      else if (algorithm=="csclean" || algorithm=="cs") {
	sm_p = new CSCleanImageSkyModel();
	doMultiFields_p = true;
	os << LogIO::NORMAL << "Using Cotton-Schwab Clean" << LogIO::POST; // Loglevel INFO
      }
      else if (algorithm=="csfast" || algorithm=="csf") {
	sm_p = new CSCleanImageSkyModel();
	sm_p->setSubAlgorithm("fast");
	doMultiFields_p = true;
	os << LogIO::NORMAL // Loglevel INFO
           << "Using Cotton-Schwab Clean (optimized)" << LogIO::POST;
      }
      else if (algorithm=="mfhogbom") {
	sm_p = new MFCleanImageSkyModel();
	sm_p->setSubAlgorithm("hogbom");
	doMultiFields_p = true;
	os << LogIO::NORMAL << "Using multifield Hogbom clean" << LogIO::POST; // Loglevel INFO
      }
      else if (algorithm=="mfmultiscale") {
	if (!scaleInfoValid_p) {
	  this->unlock();
	  os << LogIO::SEVERE << "Scales not yet set" << LogIO::POST;
	  return retval;
	}
	if (scaleMethod_p=="uservector") {
	  sm_p = new MFMSCleanImageSkyModel(userScaleSizes_p, 
					    stoplargenegatives_p, 
					    stoppointmode_p,
                                            smallScaleBias_p);
	} else {
	  sm_p = new MFMSCleanImageSkyModel(nscales_p, 
					    stoplargenegatives_p, 
					    stoppointmode_p,
                                            smallScaleBias_p);
	}
	//	if(ftmachine_p=="mosaic"|| ftmachine_p=="wproject")
	// For some reason  this does not seem to work without full
	sm_p->setSubAlgorithm("full");

	doMultiFields_p = true;
	os << LogIO::NORMAL << "Using multifield multi-scale clean"  // Loglevel INFO
	   << LogIO::POST;
      } 
      else if (algorithm=="wfclark" || algorithm=="wf") {
	// Support serial and parallel specializations
	setWFCleanImageSkyModel();
	sm_p->setSubAlgorithm("clark");
	doMultiFields_p = false;
	os << LogIO::NORMAL // Loglevel INFO
           << "Using wide-field algorithm with Clark clean" << LogIO::POST;
      }
      else if (algorithm=="wfhogbom") {
	// Support serial and parallel specializations
	setWFCleanImageSkyModel();
	sm_p->setSubAlgorithm("hogbom");
	doMultiFields_p = false;
	os << LogIO::NORMAL // Loglevel INFO
           << "Using wide-field algorithm with Hogbom clean" << LogIO::POST;
      }
      else if (algorithm=="msmfs") {
	doMultiFields_p = false;
	doWideBand_p = true;

        // check for wrong ftmachine specs.
	if ( (ftmachine_p != "ft") && (ftmachine_p != "wproject") && 
             (ftmachine_p != "wbawp") && (ftmachine_p != "nift") &&
             (ftmachine_p != "mosaic") && (ftmachine_p != "awproject") ) {
	  os << LogIO::SEVERE
             << "Multi-scale Multi-frequency Clean currently works only with ft, wproject and mosaic (and wbawp,nift,awproject)"
             << LogIO::POST;
	  return retval;
	}
	
	useNewMTFT_p=false;
	if( ftmachine_p == "awproject" ) { useNewMTFT_p=true; }


	if (!scaleInfoValid_p) {
          this->unlock();
          os << LogIO::WARN << "Scales not yet set, using power law" << LogIO::POST;
          sm_p = new WBCleanImageSkyModel(ntaylor_p, 1 ,reffreq_p);
	}
	if (scaleMethod_p=="uservector") {	
          sm_p = new WBCleanImageSkyModel(ntaylor_p,userScaleSizes_p,reffreq_p);
	} else {
          sm_p = new WBCleanImageSkyModel(ntaylor_p,nscales_p,reffreq_p);
	}
	os << LogIO::NORMAL // Loglevel INFO
           << "Using multi frequency synthesis algorithm" << LogIO::POST;
	((WBCleanImageSkyModel*)sm_p)->imageNames = Vector<String>(image);
	/* Check masks. Should be only one per field. Duplicate the name ntaylor_p times 
	   Note : To store taylor-coefficients, msmfs uses the same data structure as for
	          multi-field imaging. In the case of multifield and msmfs, the list of 
		  images is nested and follows a field-major ordering.
		  All taylor-coeffs for a single field should have the same mask (for now).
	   For now, since only single-field is allowed for msmfs, we have the following.*/
      }
      else {
	this->unlock();
	os << LogIO::SEVERE << "Unknown algorithm: " << algorithm 
	   << LogIO::POST;
	
	return retval;
      }
   
      AlwaysAssert(sm_p, AipsError);
      sm_p->setAlgorithm("clean");

      //    if (!se_p)
      if(!createSkyEquation(modelNames, fixed, maskNames, complist)) 
        {
	
          return retval;
        }
      os << LogIO::NORMAL3 << "Created Sky Equation" << LogIO::POST;
    }
    else{
      //adding or modifying mask associated with skyModel
      addMasksToSkyEquation(maskNames,fixed);
    }
    //No need to add residuals will let sm_p use tmpimage ones and we'll copy them in restore 
    if(!addResiduals(residualNames))
       throw(AipsError("Problem in attaching to residual images")); 
    // The old plot that showed how much flux was being incorporated in each
    // scale.   No longer available, slated for removal.
    // if (displayProgress) {
    //   sm_p->setDisplayProgress(true);
    //   sm_p->setPGPlotter( getPGPlotter() );
    // }



    sm_p->setGain(gain);
    sm_p->setNumberIterations(niter);
    sm_p->setThreshold(threshold.get("Jy").getValue());
    sm_p->setCycleFactor(cyclefactor_p);
    sm_p->setCycleSpeedup(cyclespeedup_p);
    sm_p->setCycleMaxPsfFraction(cyclemaxpsffraction_p);
    {
      ostringstream oos;
      oos << "Clean gain = " <<gain<<", Niter = "<<niter<<", Threshold = "
	  << threshold;
      os << LogIO::NORMAL << String(oos) << LogIO::POST; // More for the
                                                         // logfile than the
                                                         // log window.
    }

    os << LogIO::NORMAL << (firstrun ? "Start" : "Continu")
       << "ing deconvolution" << LogIO::POST; // Loglevel PROGRESS
    if(se_p->solveSkyModel()) {
      os << LogIO::NORMAL
         << (niter == 0 ? "Image OK" : "Successfully deconvolved image")
         << LogIO::POST; // Loglevel PROGRESS
    }
    else {
      converged=false;
      os << LogIO::NORMAL << "Threshhold not reached yet." << LogIO::POST; // Loglevel PROGRESS
    }
    

    printbeam(sm_p, os, firstrun);
    
    if(((algorithm.substr(0,5)=="clark") || algorithm=="hogbom" ||
        algorithm=="multiscale") && (niter != 0))
      //write the model visibility to ms for now 
      sm_p->solveResiduals(*se_p, true);
    
    for (uInt k=0 ; k < residuals_p.nelements(); ++k){
      (residuals_p[k])->copyData(sm_p->getResidual(k));
    }
    /////////////

    //cerr << "Time taken " << omp_get_wtime()-wtime0 << endl;
    ////////////
    retval.define("maxresidual", (sm_p->threshold()));
    retval.define("iterations", (sm_p->numberIterations()));
    retval.define("converged", converged);
    savePSF(psfnames);
    redoSkyModel_p=false;
    writeFluxScales(fluxscale_p);
    // restoreImages(image); // Moved to iClean so that it happens only once.
    
    
    this->unlock();

    return retval;
  }
  catch (PSFZero&  x)
    {
      //os << LogIO::WARN << x.what() << LogIO::POST;
      savePSF(psfnames);
      this->unlock();
      throw(AipsError(String("PSFZero  ")+ x.getMesg() + String(" : Please check that the required data exists and is not flagged.")));
      return retval;
    }  
  catch (exception &x) { 
    this->unlock();
    destroySkyEquation();
    throw(AipsError(x.what()));

    return retval;
  } 

  catch(...){
    this->unlock();
    destroySkyEquation();
    //Unknown exception...
    throw(AipsError("Unknown exception caught ...imager/casa may need to be exited"));
  }
  this->unlock();

  os << LogIO::NORMAL << "Exiting Imager::clean" << LogIO::POST; // Loglevel PROGRESS
  return retval;
}

// Mem algorithm
Bool Imager::mem(const String& algorithm,
		 const Int niter, 
		 const Quantity& sigma, 
		 const Quantity& targetFlux,
		 const Bool constrainFlux,
		 const Bool displayProgress, 
		 const Vector<String>& model, 
		 const Vector<Bool>& fixed,
		 const String& complist,
		 const Vector<String>& prior,
		 const Vector<String>& mask,
		 const Vector<String>& image,
		 const Vector<String>& residual)
{
   if(!valid())
    {
      return false;
    }
   logSink_p.clearLocally();
   LogIO os(LogOrigin("imager", "mem()"), logSink_p);
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) 
      {
	return false;
      }
    os << LogIO::NORMAL << "Deconvolving images with MEM" << LogIO::POST; // Loglevel PROGRESS
    
    Int nmodels=model.nelements();
    os << LogIO::NORMAL  // Loglevel INFO
       << "Found " << nmodels << " specified model images" << LogIO::POST;
    
    if(model.nelements()>0) {
      for (uInt thismodel=0;thismodel<model.nelements();++thismodel) {
	if(model(thismodel)=="") {
	  this->unlock();
	  os << LogIO::SEVERE << "Need a name for model "
	     << thismodel << LogIO::POST;
	  return false;
	}
      }
    }
    
    Vector<String> modelNames=model;
    // Make first image with the required shape and coordinates only if
    // it doesn't exist yet. Otherwise we'll throw an exception later
    if(modelNames(0)=="") modelNames(0)=imageName()+".mem";
    if(!Table::isWritable(modelNames(0))) {
      make(modelNames(0));
    }
    
    Vector<String> maskNames(nmodels);
    if(Int(mask.nelements())==nmodels) {
      maskNames=mask;
      for(Int k=0; k < nmodels; ++k){
	if(mask(k)!=""&& !Table::isReadable(mask(k))) {
	  os << LogIO::WARN 
	     << "Mask" << mask(k) 
	     << " is unreadable; ignoring masks altogether " 
	     << LogIO::POST;
	  maskNames.resize(1);
	  maskNames(0)="";
	}
      }
    }
    else {
      maskNames.resize(1);
      maskNames(0)="";
    }
    
    // Always fill in the residual images
    Vector<String> residualNames(nmodels);
    if(Int(residual.nelements())==nmodels) {
      residualNames=residual;
    }
    else {
      residualNames="";
    }
    for (Int thismodel=0;thismodel<Int(model.nelements());++thismodel) {
      if(residualNames(thismodel)=="") {
	residualNames(thismodel)=modelNames(thismodel)+".residual";
      }
      removeTable(residualNames(thismodel));
      if(!clone(model(thismodel), residualNames(thismodel)))
	{
	  return false;
	}
    }
    
    // Make an ImageSkyModel with the specified polarization representation
    // (i.e. circular or linear)
    if(algorithm=="entropy") {
      sm_p = new CEMemImageSkyModel(sigma.get("Jy").getValue(),
				    targetFlux.get("Jy").getValue(),
				    constrainFlux,
				    prior,
				    algorithm);
      os << LogIO::NORMAL // Loglevel INFO
         << "Using single-field algorithm with Maximum Entropy" << LogIO::POST;
      if(ftmachine_p=="mosaic" ||ftmachine_p=="wproject" )
	sm_p->setSubAlgorithm("full");
    }
    else if (algorithm=="emptiness") {
      sm_p = new CEMemImageSkyModel(sigma.get("Jy").getValue(),
				    targetFlux.get("Jy").getValue(),
				    constrainFlux,
				    prior,
				    algorithm);
      os << LogIO::NORMAL // Loglevel INFO
         << "Using single-field algorithm with Maximum Emptiness" << LogIO::POST;
      if(ftmachine_p=="mosaic" ||ftmachine_p=="wproject" )
	sm_p->setSubAlgorithm("full");
    }
    else if (algorithm=="mfentropy") {
      sm_p = new MFCEMemImageSkyModel(sigma.get("Jy").getValue(),
				      targetFlux.get("Jy").getValue(),
				      constrainFlux,
				      prior,
				      algorithm);
      doMultiFields_p = true;
      os << LogIO::NORMAL << "Using Maximum Entropy" << LogIO::POST; // Loglevel INFO
      //   if(ftmachine_p=="mosaic" ||ftmachine_p=="wproject" )
      sm_p->setSubAlgorithm("full");
    } else if (algorithm=="mfemptiness") {
      sm_p = new MFCEMemImageSkyModel(sigma.get("Jy").getValue(),
				      targetFlux.get("Jy").getValue(),
				      constrainFlux,
				      prior,
				      algorithm);
      doMultiFields_p = true;
      os << LogIO::NORMAL << "Using Maximum Emptiness" << LogIO::POST; // Loglevel INFO
      // if(ftmachine_p=="mosaic" ||ftmachine_p=="wproject" )
      sm_p->setSubAlgorithm("full");
    } else {
      this->unlock();
      os << LogIO::SEVERE << "Unknown algorithm: " << algorithm << LogIO::POST;
      return false;
    }
    AlwaysAssert(sm_p, AipsError);
    sm_p->setAlgorithm("mem");
    if (displayProgress) {
      sm_p->setDisplayProgress(true);
    }
    sm_p->setNumberIterations(niter);
    sm_p->setCycleFactor(cyclefactor_p);   // used by mf algs
    sm_p->setCycleSpeedup(cyclespeedup_p); // used by mf algs
    sm_p->setCycleMaxPsfFraction(cyclemaxpsffraction_p); // used by mf algs

    {
      ostringstream oos;
      oos << "MEM algorithm = " <<algorithm<<", Niter = "<<niter<<", Sigma = "
	  <<sigma << ", Target Flux = " << targetFlux;
      os << LogIO::DEBUG1 << String(oos) << LogIO::POST;
    }
 
    //    if (!se_p)
    if(!createSkyEquation(modelNames, fixed, maskNames, complist)) 
      {
	return false;
      }
    os << LogIO::NORMAL3 << "Created Sky Equation" << LogIO::POST;
    
    addResidualsToSkyEquation(residualNames);

    os << LogIO::NORMAL << "Starting deconvolution" << LogIO::POST; // Loglevel PROGRESS
    if(se_p->solveSkyModel()) {
      os << LogIO::NORMAL << "Successfully deconvolved image" << LogIO::POST; // Loglevel INFO
    }
    else {
      os << LogIO::NORMAL << "Nominally failed deconvolution" << LogIO::POST; // Loglevel INFO
    }

    // Get the PSF fit while we are here
    if(!beamValid_p){
      ImageBeamSet beam=sm_p->beam(0);
      if(beam.nelements() > 0){
	/*beam_p.setMajorMinor(
    			Quantity(abs(beam(0)), "arcsec"), Quantity(abs(beam(1)), "arcsec")
    		);
    	  beam_p.setPA(Quantity(beam(2), "deg"));
	*/
	beam_p=beam;
	beamValid_p=true;
      }
    }
    if(algorithm=="entropy" || algorithm=="emptiness" )
      sm_p->solveResiduals(*se_p, true);
    writeFluxScales(fluxscale_p); 
    restoreImages(image);
    destroySkyEquation();  
    this->writeHistory(os);
    try{
      { // write data processing history into image logtable
	LoggerHolder imagelog (false);
	LogSink& sink = imagelog.sink();
	LogOrigin lor( String("imager"), String("mem()") );
	LogMessage msg(lor);
	sink.postLocally(msg);
	MSHistoryColumns msHis(ms_p->history());
	transferHistory(imagelog, msHis);
	for (Int thismodel=0;thismodel<Int(model.nelements());++thismodel) {
	  PagedImage<Float> restoredImage(image(thismodel),
					  TableLock(TableLock::UserLocking));
	  LoggerHolder& log = restoredImage.logger();
	  log.append(imagelog);
	  log.flush();
	}
      }
    }
    catch(exception& x){
      
      os << LogIO::WARN << "Caught exception: " << x.what()
	 << LogIO::POST;
      os << LogIO::SEVERE << "This means your MS/HISTORY table may be corrupted; you may consider deleting all the rows from this table"
	 <<LogIO::POST; 
      //continue and wrap up this function as normal
      
    }
    catch(...){
      //Unknown exception...
      throw(AipsError("Unknown exception caught ...imager/casa may need to be exited"));
    }
    
    this->unlock();

    return true;
  } catch (exception& x) {
    this->unlock();
    throw(AipsError(x.what()));

    return false;
  } 
  this->unlock();
  return true;

}

    
// NNLS algorithm
Bool Imager::nnls(const String&,  const Int niter, const Float tolerance, 
		  const Vector<String>& model, const Vector<Bool>& fixed,
		  const String& complist,
		  const Vector<String>& fluxMask,
		  const Vector<String>& dataMask,
		  const Vector<String>& residual,
		  const Vector<String>& image)
{
  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "nnls()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) return false;
    
    os << LogIO::NORMAL << "Performing NNLS deconvolution" << LogIO::POST; // Loglevel PROGRESS
    
    if(niter<0) {
      this->unlock();
      os << LogIO::SEVERE << "Number of iterations must be positive" << LogIO::POST;
      return false;
    }
    if(tolerance<0.0) {
      this->unlock();
      os << LogIO::SEVERE << LogIO::SEVERE << "Tolerance must be positive" << LogIO::POST;
      return false;
    }
    
    // Add the images to the ImageSkyModel
    Int nmodels=model.nelements();
    if(nmodels>1) os<< "Can only process one model" << LogIO::POST;
    
    if(model(0)=="") {
      this->unlock();
      os << LogIO::SEVERE << "Need a name for model " << LogIO::POST;
      return false;
    }
    
    if(!Table::isWritable(model(0))) {
      make(model(0));
      this->lock();
    }
    
    // Always fill in the residual images
    Vector<String> residualNames(nmodels);
    if(Int(residual.nelements())==nmodels) {
      residualNames=residual;
    }
    else {
      residualNames="";
    }
    for (Int thismodel=0;thismodel<Int(model.nelements());++thismodel) {
      if(residualNames(thismodel)=="") {
	residualNames(thismodel)=model(thismodel)+".residual";
      }
      removeTable(residualNames(thismodel));
      if(!clone(model(thismodel), residualNames(thismodel))) return false;
    }
    
    // Now make the NNLS ImageSkyModel
    sm_p= new NNLSImageSkyModel();
    sm_p->setNumberIterations(niter);
    sm_p->setTolerance(tolerance);
    sm_p->setAlgorithm("nnls");
    os << LogIO::DEBUG1
       << "NNLS Niter = " << niter << ", Tolerance = " << tolerance << LogIO::POST;

    //    if (!se_p)
    if(!createSkyEquation(model, fixed, dataMask, fluxMask, complist)) return false;

    addResidualsToSkyEquation(residualNames);
    
    os << LogIO::NORMAL << "Starting deconvolution" << LogIO::POST; // Loglevel PROGRESS

    if(se_p->solveSkyModel()) {
      os << LogIO::NORMAL << "Successfully deconvolved image" << LogIO::POST; // Loglevel INFO
    }
    else {
      os << LogIO::NORMAL << "Nominally failed deconvolution" << LogIO::POST; // Loglevel INFO
    }
    
    // Get the PSF fit while we are here
    StokesImageUtil::FitGaussianPSF(sm_p->PSF(0), beam_p);
    beamValid_p=true;
   
    
    // Restore the image
    restoreImages(image);

    destroySkyEquation();
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
  this->unlock();
  return true;
}

// Fourier transform the model and componentlist
Bool Imager::ft(const Vector<String>& model, const String& complist,
		const Bool incremental, const Double phaseCenTime)
{
  if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "ft()", WHERE));

  if (wvi_p==NULL)
    os << LogIO::WARN << "Please make sure MS is writable when using Imager::ft" << LogIO::EXCEPTION;
  
  this->lock();
  try {
    
    if(!redoSkyModel_p){
      //let us try to update the sm_p then
      //so as to keep the state and psf's etc if they have been calculated
      //useful when cleaning, modify/clip model then predict, selfcal and clean again
      if(!updateSkyModel(model, complist))
	destroySkyEquation();
    }

    os << LogIO::NORMAL // Loglevel INFO
       << String("Fourier transforming: ") + 
      (incremental ? String("adding to "): String("replacing "))+ 
      (useModelCol_p ? String("MODEL_DATA column") : String("visibility model header")) << LogIO::POST;
   
    if (redoSkyModel_p){
      if(!createSkyEquation(model, complist)) return false;
    }
    if(incremental){
      for (Int mod=0; mod < (sm_p->numberOfModels()); ++mod){
	(sm_p->deltaImage(mod)).copyData(sm_p->image(mod));
      }
    }
    se_p->setPhaseCenterTime(phaseCenTime);
    se_p->predict(incremental);
    
    // destroySkyEquation();
    
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
  this->unlock();
  return true;
}

Bool Imager::setjy(const Int fieldid, 
		   const Int spectralwindowid,
		   const Vector<Double>& fluxDensity, const String& standard)
{
  // the old interface to new interface
  String fieldnames="";
  String spwstring="";
  Vector<Int>fldids(1,fieldid);
  Vector<Int>spwids(1,spectralwindowid);
  return setjy(fldids, spwids, fieldnames, spwstring, fluxDensity, standard);

}

Bool Imager::setjy(const Vector<Int>& /*fieldid*/,
		   const Vector<Int>& /*spectralwindowid*/,
		   const String& fieldnames, const String& spwstring,
		   const Vector<Double>& fluxDensity, const String& standard)
{
  if(!valid()) return false;
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "setjy()"), logSink_p);
  this->lock();

  String tempCL;
  try {
    Bool precompute=(fluxDensity(0) <= 0);

    // Figure out which fields/spws to treat
    Record selrec=ms_p->msseltoindex(spwstring, fieldnames);
    Vector<Int> fldids(selrec.asArrayInt("field"));
    Vector<Int> spwids(selrec.asArrayInt("spw"));

    expand_blank_sel(spwids, ms_p->spectralWindow().nrow());
    expand_blank_sel(fldids, ms_p->field().nrow());

    // Loop over field id. and spectral window id.
    Vector<Double> fluxUsed(4);
    String fluxScaleName;
    Bool matchedScale=false;
    Int spwid, fldid;
    MSColumns msc(*ms_p);
    ConstantSpectrum cspectrum;
    // TT
    Double meantime = msc.time()(0);
    meantime += 0.5 * (msc.time()(msc.nrow() - 1) - meantime);
    MEpoch mtime(msc.timeMeas()(0));
    mtime.set(Quantity(meantime, "s"));


    for (uInt kk=0; kk<fldids.nelements(); ++kk) {
      fldid=fldids[kk];
      // Extract field name and field center position 
      MDirection position=msc.field().phaseDirMeas(fldid, meantime);
      String fieldName=msc.field().name()(fldid);

      for (uInt jj=0; jj< spwids.nelements(); ++jj) {
	spwid=spwids[jj];

	// Determine spectral window center frequency
	IPosition ipos(1,0);
	MFrequency mfreq=msc.spectralWindow().chanFreqMeas()(spwid)(ipos);
	Array<Double> freqArray;
	msc.spectralWindow().chanFreq().get(spwid, freqArray, true);
	Double medianFreq=median(freqArray);
	mfreq.set(MVFrequency(medianFreq));

	fluxUsed=fluxDensity;
	fluxScaleName="user-specified";
	if (precompute) {
	  // Pre-compute flux density for standard sources if not specified
	  // using the specified flux scale standard or catalog.


	  FluxStandard::FluxScale fluxScaleEnum;
	  matchedScale=FluxStandard::matchStandard(standard, fluxScaleEnum, 
						   fluxScaleName);
	  (void)matchedScale;
	  FluxStandard fluxStd(fluxScaleEnum);
	  Flux<Double> returnFlux, returnFluxErr;

	  if (fluxStd.compute(fieldName, position, mfreq, mtime, returnFlux, returnFluxErr)) {
	    // Standard reference source identified
	    returnFlux.value(fluxUsed);
	  } 

	  // dgoscha, NCSA, 02 May, 2002
	  // this else condtion is to handle the case where the user
	  // specifies standard='SOURCE' in the setjy argument.  This will
	  // then look into the SOURCE_MODEL column of the SOURCE subtable
	  // for a table-record entry that points to a component list with the
	  // model information in it.


	  else if (standard==String("SOURCE")) {
		// Look in the SOURCE_MODEL column of the SOURCE subtable for 
		// the name of the CL which contains the model.

		// First test to make sure the SOURCE_MODEL column exists.
		if (ms_p->source().tableDesc().isColumn("SOURCE_MODEL")) {
			TableRecord modelRecord;
			msc.source().sourceModel().get(0, modelRecord);
	
			// Get the name of the model component list from the table record
			Table modelRecordTable = 
				modelRecord.asTable(modelRecord.fieldNumber(String ("model")));
			String modelCLName = modelRecordTable.tableName();
			modelRecord.closeTable(modelRecord.fieldNumber(String ("model")));

			// Now grab the flux from the model component list and use.
			ComponentList modelCL = ComponentList(Path(modelCLName), true);
			SkyComponent fluxComponent = modelCL.component(fldid);

			fluxUsed = 0;
			fluxUsed = real(fluxComponent.flux().value());
			fluxScaleName = modelCLName;
		}
		else {
			os << LogIO::SEVERE << "Missing SOURCE_MODEL column."
			   << LogIO::SEVERE << "Using default, I=1.0"
			   << LogIO::POST;
			fluxUsed = 0;
			fluxUsed(0) = 1.0;
		}
	  }

	  else {
	    // Source not found; use Stokes I=1.0 Jy for now
	    fluxUsed=0;
	    fluxUsed(0)=1.0;
	    fluxScaleName="default";
	  };
	}

	// Set the component flux density
	Flux<Double> fluxval;
	fluxval.setValue(fluxUsed);

	// Create a point component at the field center
	// with the specified flux density
	PointShape point(position);
	SkyComponent skycomp(fluxval, point, cspectrum);

	// Create a component list containing this entry
	String baseString=msname_p + "." + fieldName + ".spw" +
	  String::toString(spwid);
	tempCL=baseString + ".tempcl";

	// Force a call to the ComponentList destructor
	// using scoping rules.
	{ 
	  ComponentList cl;
	  cl.add(skycomp);
	  cl.rename(tempCL, Table::New);
	}

	// Select the uv-data for this field and spw. id.;
	// all frequency channels selected.
	Vector<Int> selectSpw(1), selectField(1);
	selectSpw(0)=spwid;
	selectField(0)=fldid;
	String msSelectString = "";
	Vector<Int> numDeChan(1);
	numDeChan[0]=0;
	Vector<Int> begin(1);
	begin[0]=0;
	Vector<Int> stepsize(1);
	stepsize[0]=1;
	setdata("channel", numDeChan, begin, stepsize, MRadialVelocity(), 
		MRadialVelocity(),
		selectSpw, selectField, msSelectString, "", "", Vector<Int>(), 
		"", "", "", "", "", "",true);

	if (!nullSelect_p) {

	  // Transform the component model table
	  Vector<String> model;
	  ft(model, tempCL, false);

	  // Log flux density used for this field and spectral window
	  os.output().width(12);
	  os << fieldName << "  spwid=";
	  os.output().width(3);
	  os << (spwid) << "  ";
	  os.output().width(0);
	  os.output().precision(4);
	  os << LogIO::NORMAL << "[I=" << fluxUsed(0) << ", "; // Loglevel INFO
	  os << "Q=" << fluxUsed(1) << ", ";
	  os << "U=" << fluxUsed(2) << ", ";
	  os << "V=" << fluxUsed(3) << "] Jy, ";
	  os << ("(" + fluxScaleName + ")") << LogIO::POST;
	};
	  
	// Delete the temporary component list and image tables
	TableUtil::deleteTable(tempCL);

      }
    }
    this->writeHistory(os);
    this->unlock();
    return true;

  } catch (AipsError x) {
    this->unlock();
    if(TableUtil::canDeleteTable(tempCL)) TableUtil::deleteTable(tempCL);
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
  return true;
}

// This is the one used by im.setjy() (because it has a model arg).
// CURRENT SETJY CODE
Record Imager::setjy(const Vector<Int>& /*fieldid*/,
                   const Vector<Int>& /*spectralwindowid*/,
                   const String& fieldnames, const String& spwstring,
                   const String& model,
                   const Vector<Double>& fluxDensity, 
                   const String& standard, const Bool chanDep,
                   //const Double spix, const MFrequency& reffreq,
                   const Vector<Double>& spix, const MFrequency& reffreq,
                   const Vector<Double>& pipars,const Vector<Double>& papars, 
                   const Double& rotMeas,
                   const String& timerange, const String& scanstr,
                   const String& intentstr, const String& obsidstr,
                   const String& interpolation)
{
  //if(!valid())
    //return false;

  //Bool didAnything = false;
  
  Record retval;
  if(!valid()) {
    retval.define("process",false);
    return retval;
  }

  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "setjy()"), logSink_p);
  this->lock();

  // user specified flux densities (IQUV), global to the spws and fields
  Vector<Double> fluxdens = fluxDensity;
  if(fluxDensity.nelements() < 4){
    fluxdens.resize(4,true);
    for(Int i = fluxDensity.nelements(); i < 4; ++i)
      fluxdens[i] = 0.0;
  }

  Vector<String> tempCLs;
  TempImage<Float> *tmodimage(NULL);

  try{
    Bool precompute = fluxdens[0] < 0.0;

    // Figure out which fields/spws to treat
    // including intent info 
    MSSelection mssel;
    mssel.setFieldExpr(fieldnames);
    mssel.setSpwExpr(spwstring);
    mssel.setStateExpr(intentstr);
    TableExprNode exprNode = mssel.toTableExprNode(&(*ms_p));
    //Vector<Int> fldids;
    Vector<Int> fldids(mssel.getFieldList());
    Vector<Int> selToRawSpwIds(mssel.getSpwList());
    // if intent is given try to do AND with fieldIds
    if (intentstr!="") {
      mssel_p = new MeasurementSet((*ms_p)(exprNode), &(*ms_p));
      MSColumns tmpmsc(*mssel_p);
      Vector<Int> fldidv=tmpmsc.fieldId().getColumn();
      if (fldidv.nelements()==0) 
        throw(AipsError("No field ids were selected, please check input parameters"));
      std::set<Int> ufldids(fldidv.begin(),fldidv.end());
      std::vector<Int> tmpv(ufldids.begin(), ufldids.end());
      fldids.resize(tmpv.size());
      uInt count=0;
      for (std::vector<int>::const_iterator it=tmpv.begin();it != tmpv.end(); it++)
      {
         fldids(count) = *it;
         count++;
      }
    }
    //else {
    //  fldids(mssel.getFieldList());
    //}
    //cerr<<"fldids.nelements()="<<fldids.nelements()<<endl;
    //for (uInt i = 0; i < fldids.nelements(); i++) {
    //    cerr<<"fldids="<<fldids(i)<<endl;
    //}
    //Record selrec = ms_p->msseltoindex(spwstring, fieldnames);
    //Vector<Int> fldids(selrec.asArrayInt("field"));
    //Vector<Int> selToRawSpwIds(selrec.asArrayInt("spw"));
 
    expand_blank_sel(selToRawSpwIds, ms_p->spectralWindow().nrow());
    expand_blank_sel(fldids, ms_p->field().nrow());

    // Warn against multiple fields in some circumstances.
    if (fldids.nelements() > 1 && (model != "" || !precompute)) {
      String errmsg("setjy is applying a single ");

      if(model != ""){
	errmsg += "modimage";
        if(!precompute)
          errmsg += " or ";
      }

      if(!precompute)
        errmsg += "fluxdensity";

      errmsg += " to multiple fields!\n";
      os << LogIO::WARN
         << errmsg
         << "This could be a user error, but sometimes a single name will\n"
         << "resolve to > 1 field index.\n"
         << LogIO::POST;
      //throw(AipsError(errmsg));
    }

    os << LogIO::NORMAL;
    if(precompute || spix[0] != 0.0)
      os << "Using " << ((chanDep || (!precompute && spix[0] != 0.0)) ? "channel" : 
                         "spw") << " dependent flux densities";
    else
      os << "The applied flux density does not depend on frequency.";
    os << LogIO::POST;

    // Ignore user polarization if using an image.
    if(model != "" &&
       (fluxdens[1] != 0.0 || fluxdens[2] != 0.0 || fluxdens[3] != 0.0)){
      os << LogIO::WARN
         << "Using model image, so zeroing user QUV flux densities."
         << LogIO::POST;
      fluxdens[1] = fluxdens[2] = fluxdens[3] = 0.0;
      writeHistory(os);
    }

    // Loop over field id. and spectral window id.
    //Vector<Double> fluxUsed(4);
    String fluxScaleName("user-specified");
    FluxStandard::FluxScale fluxScaleEnum;
    if(!FluxStandard::matchStandard(standard, fluxScaleEnum, fluxScaleName))
      throw(AipsError(standard + " is not a recognized flux density scale"));

    FluxStandard fluxStd(fluxScaleEnum);
    if (fluxScaleEnum==FluxStandard::PERLEY_BUTLER_2013 || 
        fluxScaleEnum==FluxStandard::PERLEY_BUTLER_2017 ) {
      fluxStd.setInterpMethod(interpolation);
    }

    // Setup the frequency, Flux, and ComponentList arrays.
    uInt nspws = selToRawSpwIds.nelements();
    Vector<Vector<Flux<Double> > > returnFluxes(nspws), returnFluxErrs(nspws);
    Vector<Vector<MFrequency> > mfreqs(nspws);
    Vector<Vector<Double> > fluxUsed(nspws); // fluxesUsed(nspws,4) 
    MSColumns msc(*ms_p);
    MEpoch aveEpoch=MEpoch(msc.timeMeas()(0));
    const Unit freqUnit = sjy_setup_arrs(returnFluxes, returnFluxErrs, fluxUsed, tempCLs, mfreqs,
                                         msc.spectralWindow(), nspws, selToRawSpwIds,
                                         chanDep);
    // Processing for each field ***************************************************
    for(Int fldInd = fldids.nelements(); fldInd--;){
      Int fldid = fldids[fldInd];
      // Extract field name and field center position 
      MDirection fieldDir = msc.field().phaseDirMeas(fldid, msc.time()(0));
      String fieldName = msc.field().name()(fldid);
      Bool foundSrc = false;
    
      //for returned flux densities
      
      Record retvalperField;
      //
      //fluxUsed = fluxdens;
      fluxUsed(0) = fluxdens;
      //if(precompute){
        // Pre-compute flux density for standard sources if not specified
        // using the specified flux scale standard or catalog.
        //
        // The flux densities are calculated for all spws at once to avoid
        // repeatedly digging up the flux model (and possibly the ephemeris).
        //
        // TT: moving this outside of if(precompute) since selected ms (mssel_p)
        // will be needed for other cases to clear the model using 
        // VisModelData::ClearModel()
        
        Vector<Int> selectField(1);
        selectField[0] = fldid;
        Vector<Int> numDeChan(1);
        numDeChan[0] = 0;
        Vector<Int> begin(1);
        begin[0] = 0;
        Vector<Int> stepsize(1);
        stepsize[0] = 1;
        String msSelectString = "";
        setdata("none", numDeChan, begin, stepsize, MRadialVelocity(), 
                MRadialVelocity(),
                selToRawSpwIds, selectField, msSelectString, timerange, "",
                Vector<Int>(), "", "", "", scanstr, intentstr, obsidstr, true, true);
        if(nullSelect_p){
          os << ((timerange == "" && scanstr == ""
                  && obsidstr == "") ? LogIO::WARN : LogIO::NORMAL)
             << "No data was selected for field " << fldid << "."
             << LogIO::POST;
          continue;
        }
      if(precompute){
        // Make componentlist for each spw.
        // Pre-compute flux density for standard sources if not specified
        // using the specified flux scale standard or catalog.
        //
        // The flux densities are calculated for all spws at once to avoid
        // repeatedly digging up the flux model (and possibly the ephemeris).
        //
        MSColumns msselc(*mssel_p);
        //if(nullSelect_p || msselc.nrow() < 1){
        if(!nullSelect_p and  msselc.nrow() < 1){
          os << ((timerange == "" && scanstr == ""
                  && obsidstr == "") ? LogIO::WARN : LogIO::NORMAL)
             << "No data was selected for field " << fldid << "."
             << LogIO::POST;
          continue;
        }

        // chnage to return cocantenated CL?
        foundSrc = sjy_computeFlux(os, fluxStd, returnFluxes, returnFluxErrs, tempCLs,
                                   fluxUsed, fluxScaleName, aveEpoch, mfreqs, model, fieldName,
                                   msselc, fldid, fieldDir, selToRawSpwIds, standard);
	(void)foundSrc;
      }
      
      //*************** For loop about spw starts here ****************************
      Vector<Double>  freqscaling;
      Vector<Double> freqsOfScale;
      Vector<Int> rawspwids(nspws);
      // make raw spw id list
      for(uInt selspw = 0; selspw < nspws; ++selspw){
        Int rawspwid = selToRawSpwIds[selspw];
        rawspwids[selspw]=rawspwid;
      } 

        // move inside sjy_computeFlux  - TT, 2014.06.13 
        /***
        if(foundSrc){
          // Log fluxes found from the standard catalog database to HISTORY table 
          // get I-flux density for the selected spw (returnFluxes[nspw][4]) 
          // Read this as fluxUsed = returnFluxes[selspw][0].value().
          os << "CHECK: foundSrc....."<<LogIO::POST;
          returnFluxes[selspw][0].value(fluxUsed);
            
          // Log flux density found for this field and spectral window
          os.output().width(12);
          os << fieldName;
          os.output().width(2);
          os << " (fld ind " << fldid << ") spw ";
          os << rawspwid << "  ";
          os.output().width(0);
          os.output().precision(5);
          os << LogIO::NORMAL << "[I=" << fluxUsed(0) << ", "; // Loglevel INFO
          os << "Q=" << fluxUsed(1) << ", ";
          os << "U=" << fluxUsed(2) << ", ";
          os << "V=" << fluxUsed(3) << "] Jy, ";
          os << ("(" + fluxScaleName + ")") << LogIO::POST;
          writeHistory(os);
        }
          
        // If a model image has been specified, 
        //  rescale it according to the I f.d. determined above

	//Vector<Double> freqscaling;
	//Vector<Double> freqsOfScale;

        //MEpoch mtime = msc.field().timeMeas()(fldid);

      } //spw for-loop end
      ****/
      
      MEpoch mtime = msc.field().timeMeas()(fldid);

      // model image prep. changed to do all spws at once - TT, 2014.06.13
      if(model != ""){

        tmodimage = sjy_prepImage(os, fluxStd, fluxUsed[0], freqsOfScale, freqscaling, model, msc.spectralWindow(),
       // tmodimage = sjy_prepImage(os, fluxStd, fluxUsed, freqsOfScale, freqscaling, model, msc.spectralWindow(),
      //                          rawspwid, chanDep, mfreqs, selspw, fieldName,
                                  selToRawSpwIds, chanDep, mfreqs, fieldName,
                                  fieldDir, freqUnit, fluxdens, precompute, spix,
                                  reffreq, aveEpoch, fldid);
          
      }
      else if (!precompute) {
        // do it in sjy_makeComponentList()
        // TODO: add polindex, polangle, rm handling
        // for now ignore circular polarization
        //Vector<Double> cppars(1,0.0);
        Vector<Double> checkfluxes;
        sjy_makeComponentList(os, tempCLs, returnFluxes, fluxUsed[0], selToRawSpwIds, mfreqs, fieldName, fieldDir, 
                            spix, pipars, papars, rotMeas, reffreq, aveEpoch, fldid);
        returnFluxes[0][0].value(checkfluxes);
      }
      /*** moved to sjy_makeComponentList()
      // make componentlist using flux densities from the user specfied fluxdensity(per-spw) 
      for(uInt selspw = 0; selspw < nspws; ++selspw){
        Int rawspwid = selToRawSpwIds[selspw];
        rawspwids[selspw]=rawspwid;
        if(model == "" && !precompute){
          // **** inside spw for-loop
          // fluxUsed was supplied by the user instead of FluxStandard, so
          // make a component list for it now, for use in ft.

          // Set the component flux density
          Flux<Double> fluxval;
          Flux<Double> fluxerr;
          fluxval.setValue(fluxUsed[0]);
          // Create a point component at the field center
          // with the specified flux density 
          // - obviously this does not correct for solar objects... 
          PointShape point(fieldDir);
          SpectralIndex siModel;
          if(reffreq.getValue().getValue() > 0.0){
	    MeasFrame mFrame(MEpoch(msc.timeMeas()(0)), mLocation_p, fieldDir);
	    MFrequency::Convert cvt(mfreqs[selspw][0].getRef(), MFrequency::Ref(MFrequency::castType(reffreq.getRef().getType()), mFrame));
            siModel.setRefFrequency(reffreq);
            siModel.setIndex(spix);
            returnFluxes[selspw][0].setValue(fluxUsed[0] * siModel.sample(cvt(mfreqs[selspw][0])));
          }
          else{
            if(spix != 0.0){            // If not the default, complain and quit.
              os << LogIO::SEVERE
                 << "spix cannot be nonzero with reffreq = 0!"
                 << LogIO::POST;
              //return false;
            }
            siModel.setRefFrequency(MFrequency(Quantity(1.0, "GHz")));
            siModel.setIndex(0.0);
          }

          // No worries about varying fluxes or sizes here, so any time will do.
          // Moved this line up (TT 2013/05/09) 
          //MEpoch mtime = msc.field().timeMeas()(fldid);
          tempCLs[selspw] = FluxStandard::makeComponentList(fieldName,
                                                            mfreqs[selspw][0],
                                                            mtime, fluxval, point,
                                                            siModel,
          // jagonzal (CAS-4109): Specify table name to avoid clashing between different CASA engines when running vs a MMS
                                                            ms_p->tableName() +
                                                            "_setjy_spw" +
                                                            String::toString(selspw) +
                                                            "_");
        }
        ***/

        // clear existing model for the selected field and for all selected spws 
        // outside spw loop
        //if (!useModelCol_p && selspw==0) {
        if (!useModelCol_p) {
          String tmpspwstring=spwstring;
          if (tmpspwstring=="") tmpspwstring="*";
          os << LogIO::NORMAL
             << "Will clear any existing model with matching field="
             << fieldName
             << " and spw=" << tmpspwstring
             << LogIO::POST;

          String fldidstr = String::toString(fldid); 
          // use field id due to possible MSSelection bug for handing field name with blanks
          //VisModelData::clearModel(*mssel_p, fieldName, spwstring)
          VisModelData::clearModel(*mssel_p, fldidstr, spwstring);
        }
        // TODO: do it for all spw at once............
        //sjy_make_visibilities(tmodimage, os, rawspwid, fldid, tempCLs[selspw],
        //                      timerange, scanstr, intentstr, obsidstr, freqsOfScale, freqscaling);
	
        /***
        if(tmodimage)
          delete tmodimage;
        tmodimage = NULL;
        //	if (Table::canDeleteTable("temp.setjy.image")) Table::deleteTable("temp.setjy.image");

	if(tempCLs[selspw] != ""){
          String errmsg;

          //didAnything = true;
          // commentted out for testing of concatCLs, may need to uncommentted later!!!!!!

          if(Table::canDeleteTable(errmsg, tempCLs[selspw]))
            Table::deleteTable(tempCLs[selspw]);
          else
            os << LogIO::WARN
               << "Could not rm " << tempCLs[selspw]
               << " because the " << errmsg << "."
               << LogIO::POST;
        }
        ***/

      for(uInt selspw = 0; selspw < nspws; ++selspw){
        Record subrec;
        //store fluxd actually used to scale (not input fluxdensity)
        Vector<Double> finalFluxUsed;
        // Flux of first chan
        returnFluxes[selspw][0].value(finalFluxUsed);
        subrec.define("fluxd",finalFluxUsed);
        // TODO: add fluxd error when the flux density uncertainties 
        //       are corrrectly filled.
        //
        //retvalperField.defineRecord(String::toString(rawspwid),subrec);
        retvalperField.defineRecord(String::toString(selToRawSpwIds[selspw]),subrec);
      }   // for selspw end **********************************************
      //retval.defineRecord(fieldName,retvalperField);
      retvalperField.define("fieldName",fieldName);
      retval.defineRecord(String::toString(fldid),retvalperField);

      // cocatenate componentlists - not yet used....
      //if (tempCLs[0]!="") {
        // concatcl name should contains field name mjd etc...
      //  ostringstream oss;
      //  oss<< ms_p->tableName() << "_setjy_"
      //     << fieldName << "_" << mtime.get("d").getValue()
      //     << "d.cl";
      //  String concatcl(oss);
      //  sjy_concatComponentLists(os, tempCLs, concatcl);
      //}

      //sjy_make_visibilities(tmodimage, os, rawspwids, fldid, concatcl,
      //                      timerange, scanstr, intentstr, obsidstr, freqsOfScale, freqscaling);
      //
      //### Uncomment above once setjyFTMachine can handle multi-row componentlist ############# 

      for(uInt selspw = 0; selspw < nspws; ++selspw){
        sjy_make_visibilities(tmodimage, os, rawspwids[selspw], fldid, tempCLs[selspw],
                            timerange, scanstr, intentstr, obsidstr, freqsOfScale, freqscaling);
      }
      // #######################################################################################
      // clean-up
      //  
      if(tmodimage)
        delete tmodimage;
      tmodimage = NULL;

      for(uInt selspw = 0; selspw < nspws; ++selspw){
	if(tempCLs[selspw] != ""){
          String errmsg;

          //didAnything = true;

          if(TableUtil::canDeleteTable(errmsg, tempCLs[selspw]))
            TableUtil::deleteTable(tempCLs[selspw]);
          else
            os << LogIO::WARN
               << "Could not rm " << tempCLs[selspw]
               << " because the " << errmsg << "."
               << LogIO::POST;

          //if(Table::canDeleteTable(errmsg, concatcl))
          //  Table::deleteTable(concatcl);
        }
      }
      
    }   // End of loop over fields.
    // add a format info for the returned flux densities (Record)
    //retval.define("format","{field name: {spw Id: {fluxd: [I,Q,U,V] in Jy}}}");
    retval.define("format","{field Id: {spw Id: {fluxd: [I,Q,U,V] in Jy}, 'fieldName':field name }}");

    if(!precompute && spix[0] != 0.0 && reffreq.getValue().getValue() > 0.0){
      os << LogIO::NORMAL
         << "Flux density as a function of frequency (channel 0 of each spw):\n"
         << "  Frequency (GHz)    Flux Density (Jy, Stokes I)"
         << LogIO::POST;
      for(uInt selspw = 0; selspw < nspws; ++selspw)
        os << "     " << mfreqs[selspw][0].get("GHz").getValue() << "         "
           << returnFluxes[selspw][0].value(Stokes::I).getValue()
           << LogIO::POST;
    }

    this->writeHistory(os);
    this->unlock();
    //return true;
  }
  catch (AipsError x){
    this->unlock();
    for(Int i = tempCLs.nelements(); i--;){
      if(tempCLs[i] != "")
        TableUtil::deleteTable(tempCLs[i]);
    }
    if (tmodimage) delete tmodimage; tmodimage=NULL;
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    //return false;
  } 
  //return didAnything;
  return retval;
}

String Imager::make_comp(const String& objName,
			 const String& standard,
			 const MEpoch& mtime, const Vector<MFrequency>& freqv,
			 const String& prefix)
{
  Bool foundSrc = false;
  logSink_p.clearLocally();
  LogIO os(LogOrigin("imager", "setjy()"), logSink_p);

  Vector<String> clistnames(1);
  try{
    FluxStandard::FluxScale fluxScaleEnum;
    String fluxScaleName("user-specified");

    if(!FluxStandard::matchStandard(standard, fluxScaleEnum, fluxScaleName))
      throw(AipsError(standard + " is not a recognized flux density scale"));

    FluxStandard fluxStd(fluxScaleEnum);

    Vector<Vector<Flux<Double> > > returnFluxes(1), returnFluxErrs(1);
    Vector<Vector<MFrequency> > mfreqs(1);
    uInt nfreqs = freqv.nelements();

    mfreqs[0] = freqv;
    returnFluxes[0].resize(nfreqs);
    returnFluxErrs[0].resize(nfreqs);

    MDirection objDir;
    
    if (fluxScaleEnum==FluxStandard::PERLEY_BUTLER_2013 || 
        fluxScaleEnum==FluxStandard::PERLEY_BUTLER_2017) 
    { 
      fluxStd.setInterpMethod("nearest");
    }
    foundSrc = fluxStd.computeCL(objName, mfreqs, mtime, objDir,
				 returnFluxes, returnFluxErrs,
				 clistnames, prefix);
  }
  catch(AipsError x){
    os << LogIO::SEVERE << "Exception Reported: " << x.getMesg() << LogIO::POST;
    RETHROW(x);
  }  
  return foundSrc ? clistnames[0] : "";
}

Unit Imager::sjy_setup_arrs(Vector<Vector<Flux<Double> > >& returnFluxes,
                            Vector<Vector<Flux<Double> > >& returnFluxErrs,
                            Vector<Vector<Double> >& fluxUsed,
                            Vector<String>& tempCLs,
                            Vector<Vector<MFrequency> >& mfreqs,
                            const MSSpWindowColumns& spwcols, const uInt nspws,
                            const Vector<Int>& selToRawSpwIds, const Bool chanDep)
{
  // .getUnits() is a little confusing - it seems to return a Vector which is
  // a list of all the units, not the unit for each row.

  
  const Unit freqUnit(spwcols.chanFreqQuant().getUnits()[0]);

  IPosition ipos(1, 0);

  tempCLs.resize(nspws);
  for(uInt selspw = 0; selspw < nspws; ++selspw){
    Int rawspwid = selToRawSpwIds[selspw];

    if(chanDep){
      mfreqs[selspw] = spwcols.chanFreqMeas()(rawspwid);
      uInt nchan = mfreqs[selspw].nelements();
      returnFluxes[selspw].resize(nchan);
      returnFluxErrs[selspw].resize(nchan);
    }
    else{
      mfreqs[selspw].resize(1);
      returnFluxes[selspw].resize(1);
      returnFluxErrs[selspw].resize(1);

      // Determine spectral window center frequency
      Double medianFreq = median(spwcols.chanFreq()(rawspwid));
      mfreqs[selspw] = spwcols.chanFreqMeas()(rawspwid)(ipos);
      mfreqs[selspw].set(MVFrequency(Quantum<Double>(medianFreq, freqUnit)));
    }
    // initialize fluxUsed to 0 
    Vector<Double> iquvF(4,0.0);
    fluxUsed[selspw]=iquvF;
  }

  return freqUnit;
}
// new signature should be...
/***
Bool Imager::sjy_make_visibilities(TempImage<Float> *tmodimage, LogIO& os,
                                   const Vector<Int>& rawspwids, const Int fldid,
                                   const String& clname, const String& timerange,
                                   const String& scanstr, const String& intentstr, const String& obsidstr,
                                   const Vector<Double>& freqsOfScale, const Vector<Double>& freqscaling)
***/
Bool Imager::sjy_make_visibilities(TempImage<Float> *tmodimage, LogIO& os,
                                   const Int rawspwid, const Int fldid,
                                   const String& clname, const String& timerange,
                                   const String& scanstr, const String& intentstr, const String& obsidstr,
                                   const Vector<Double>& freqsOfScale, const Vector<Double>& freqscaling)
{
  Bool made_visibilities = false;

  // Select the uv-data for this field and spw. id.;
  // all frequency channels selected.
  Vector<Int> selectSpw(1), selectField(1);
  // for the new 
  //Vector<Int> selectSpw, selectField(1);
  //selectSpw.resize(rawspwids.nelements());
  //selectSpw = rawspwids;
  //
  selectSpw[0] = rawspwid;
  selectField[0] = fldid;
  String msSelectString = "";
  Vector<Int> numDeChan(1);
  numDeChan[0] = 0;
  Vector<Int> begin(1);
  begin[0] = 0;
  Vector<Int> stepsize(1);
  stepsize[0] = 1;
  //TempImage<Float> tmodimage = tmodimages[0];
  if(tmodimage || clname != "") {
  //if(!tmodimages[0].null() || clname != "")
    setdata("channel", numDeChan, begin, stepsize, MRadialVelocity(), 
            MRadialVelocity(),
            selectSpw, selectField, msSelectString, timerange, "",
            Vector<Int>(), "", "", "", scanstr, intentstr, obsidstr, true, true);
  }
  if(!nullSelect_p){
    // Use ft to form visibilities
    Vector<String> modelv;
 
    if(tmodimage){
    //if(!tmodimages[0].null()){
      //for (uInt imod=0; imod<tmodimages.nelements();imod++) {
      //if(!tmodimages[imod].null()) {
      if(sm_p)
        destroySkyEquation();
      // do if for each spw?
      if(freqsOfScale.nelements() > 0){
      //if(freqsOfScales[imod].nelements() > 0){
	delete ft_p;
	ft_p=NULL;
	ftmachine_p="SetJyGridFT";
	createFTMachine();
	(static_cast<SetJyGridFT*>(ft_p))->setScale(freqsOfScale, freqscaling);
	//(static_cast<SetJyGridFT*>(ft_p))->setScale(freqsOfScales[imod], freqscalings[imod]);
      }
      if(!ft_p)
        createFTMachine();
      sm_p = new CleanImageSkyModel();
      // loop over for multiple spw model images
      sm_p->add(*tmodimage, 1);
      //sm_p->add(*(tmodimages[imod]), 1);
      //} //if-tmodimages..
      //} //for loop
      // this has no effect (SetJyGridFT hardcoded to use linear now, Aug. 2016)
      //ft_p->setFreqInterpolation("nearest");
      setSkyEquation();
      se_p->predict(false);
      destroySkyEquation();

      made_visibilities = true;
    }
    else if(clname != ""){
      //made_visibilities = ft(modelv, clname, false);
      made_visibilities = ft(modelv, clname, false);
      destroySkyEquation();
    }
    else
      os << LogIO::NORMAL
         << "Skipping an empty component list for spw " << rawspwid
     // for new one
     //    << "Skipping an empty component list for spw " << rawspwids
         << LogIO::POST;
  }
  return made_visibilities;
}


Bool Imager::sjy_concatComponentLists(LogIO& os, const Vector<String>& tempCLs, const String& outTempCL)
{
  ComponentList concatCL;
  try {
    for (uInt icl=0; icl<tempCLs.nelements(); icl++) {
      if (tempCLs[icl]!="") {
        // expected format _setjy_spw#_...
        String::size_type spos=tempCLs[icl].find(String("spw"));
        String::size_type epos=tempCLs[icl].find_first_of("_",spos);
        String clab = tempCLs[icl].substr(spos,epos-spos);
        Path clname(tempCLs[icl]);
        os <<LogIO::DEBUG1 << " tempCLs["<<icl<<"]="<<tempCLs[icl]<<LogIO::POST;
        ComponentList tempcl(clname, true);
        Vector<Int> which(1,0);
        tempcl.setLabel(which,clab);
        os << LogIO::DEBUG1 << "adding "<<tempCLs[icl]<<" to "<<outTempCL<<LogIO::POST;
        concatCL.addList(tempcl);
      }
    }  
    concatCL.rename(outTempCL, Table::New);
    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::EXCEPTION;
    return false;
  }
}

Bool Imager::sjy_computeFlux(LogIO& os, FluxStandard& fluxStd,
                             Vector<Vector<Flux<Double> > >& returnFluxes,
                             Vector<Vector<Flux<Double> > >& returnFluxErrs,
                             Vector<String>& tempCLs, 
                             //Vector<Double>& fluxUsed,
                             Vector<Vector<Double> >& fluxUsed,
                             String& fluxScaleName, MEpoch& aveEpoch,
                             const Vector<Vector<MFrequency> >& mfreqs,
                             const String& model, const String& fieldName, 
                             const MSColumns& msc, const Int fldid, 
                             const MDirection& fieldDir, const Vector<Int>& selToRawSpwIds,
                             const String& standard)
{
  Bool foundSrc = false;
   
  Double meantime = msc.time()(0);
  meantime += 0.5 * (msc.time()(msc.nrow() - 1) - meantime);
  MEpoch mtime(msc.timeMeas()(0));
  mtime.set(Quantity(meantime, "s"));
  if(model != ""){
    // Just get the fluxes and their uncertainties for scaling the image.
    //foundSrc = fluxStd.compute(fieldName, mfreqs, returnFluxes,
     //                          returnFluxErrs);
    foundSrc = fluxStd.compute(fieldName, fieldDir, mfreqs, mtime, returnFluxes,
                              returnFluxErrs);
  }
  else{
    // Go ahead and get FluxStandard to make the ComponentList, since
    // it knows what type of component to use. 

    // This is _a_ time.  It would be more accurate and safer, but
    // slower, to use the weighted average of the times at which this
    // source was observed, and to use the range of times in the
    // estimate of the error introduced by using a single time.
    //
    // Obviously that would be overkill if the source does not vary.
    //
    /***
    Double meantime = msc.time()(0);
    meantime += 0.5 * (msc.time()(msc.nrow() - 1) - meantime);
    MEpoch mtime(msc.timeMeas()(0));
    mtime.set(Quantity(meantime, "s"));
    ***/
            
    aveEpoch=mtime;

    foundSrc = fluxStd.computeCL(fieldName, mfreqs, mtime, fieldDir,
                                 returnFluxes, returnFluxErrs,
                                 tempCLs, ms_p->tableName()+"_setjy_");
  }
  if(!foundSrc){
    if(standard == String("SOURCE")){
      // *** THIS MODE IS NOT USED IN CURRENT SETJY ***
      // dgoscha, NCSA, 02 May, 2002
      // this else condtion is to handle the case where the user
      // specifies standard='SOURCE' in the setjy argument.  This will
      // then look into the SOURCE_MODEL column of the SOURCE subtable
      // for a table-record entry that points to a component list with the
      // model information in it.

      // Look in the SOURCE_MODEL column of the SOURCE subtable for 
      // the name of the CL which contains the model.

      // First test to make sure the SOURCE_MODEL column exists.
      if(ms_p->source().tableDesc().isColumn("SOURCE_MODEL")){
        TableRecord modelRecord;
        msc.source().sourceModel().get(0, modelRecord);
	
        // Get the name of the model component list from the table record
        Table modelRecordTable = 
          modelRecord.asTable(modelRecord.fieldNumber(String ("model")));
        String modelCLName = modelRecordTable.tableName();
        modelRecord.closeTable(modelRecord.fieldNumber(String ("model")));

        // Now grab the flux from the model component list and use.
        ComponentList modelCL = ComponentList(Path(modelCLName), true);
        SkyComponent fluxComponent = modelCL.component(fldid);

        //fluxUsed = 0;
        fluxUsed(0) = 0;
        //fluxUsed = real(fluxComponent.flux().value());
        fluxUsed(0) = real(fluxComponent.flux().value());
        fluxScaleName = modelCLName;
      }
      else {
        os << LogIO::SEVERE << "Missing SOURCE_MODEL column."
           << LogIO::SEVERE << "Continuing with the default, I = 1.0 Jy"
           << LogIO::POST;
        //fluxUsed = 0;
        fluxUsed(0) = 0;
        //fluxUsed(0) = 1.0;
        fluxUsed(0)(0) = 1.0;
      }
    }
    else {
      // Source not found; use Stokes I=1.0 Jy for now
      // (The flux standard already issued a complaint like this...)
      // os << LogIO::WARN
      //    << fieldName << " was not recognized by " << standard
      //    << ".\nContinuing with the default, I = 1.0 Jy"
      //    << LogIO::POST;
      //fluxUsed = 0;
      fluxUsed(0) = 0;
      //fluxUsed(0) = 1.0;
      fluxUsed(0)(0) = 1.0;
      fluxScaleName = "default";
    }

    // Currently, if !foundSrc, then the flux density is the same for all
    // spws.
    // Log the flux density found for this field.
    os.output().width(12);
    os << fieldName << "  ";
    os.output().width(0);
    os.output().precision(4);
    /***
    os << LogIO::NORMAL << "[I=" << fluxUsed(0) << ", "; // Loglevel INFO
    os << "Q=" << fluxUsed(1) << ", ";
    os << "U=" << fluxUsed(2) << ", ";
    os << "V=" << fluxUsed(3) << "] Jy, ";
    ***/
    os << LogIO::NORMAL << "[I=" << fluxUsed(0)(0) << ", "; // Loglevel INFO
    os << "Q=" << fluxUsed(0)(1) << ", ";
    os << "U=" << fluxUsed(0)(2) << ", ";
    os << "V=" << fluxUsed(0)(3) << "] Jy @ ";
    os << mfreqs(0)(0).getValue()<<"Hz, ";
    os << ("(" + fluxScaleName + ")") << LogIO::POST;
    writeHistory(os);
  }  // End of if(!foundSrc).
  else {
    // Logging/History for foundSrc=true (moved from the im.setjy method)
    for (uInt selspw=0; selspw<selToRawSpwIds.nelements(); selspw++) {
      returnFluxes[selspw][0].value(fluxUsed[selspw]);
      // Log flux density found for this field and spectral window
      os.output().width(12);
      os << fieldName;
      os.output().width(2);
      os << " (fld ind " << fldid << ") spw ";
      os << selToRawSpwIds[selspw] << "  ";
      os.output().width(0);
      os.output().precision(5);
      os << LogIO::NORMAL << "[I=" << fluxUsed(selspw)(0) << ", "; // Loglevel INFO
      os << "Q=" << fluxUsed(selspw)(1) << ", ";
      os << "U=" << fluxUsed(selspw)(2) << ", ";
      os << "V=" << fluxUsed(selspw)(3) << "] Jy @ ";
      os << mfreqs(selspw)(0).getValue()<<"Hz, ";
      os << ("(" + fluxScaleName + ")") << LogIO::POST;
      writeHistory(os);
    } 
  }
  return foundSrc;
}

//make componentlist(s) with user specified flux density
void Imager::sjy_makeComponentList(LogIO& os, Vector<String>& tempCLs,
                              Vector<Vector<Flux<Double> > >& returnFluxes,
                              const Vector<Double>& fluxUsed, 
                              const Vector<Int>& selToRawSpwIds, 
                              const Vector<Vector<MFrequency> >& mfreqs,
                              const String& fieldName,
                              const MDirection& fieldDir, 
                              const Vector<Double>& spix,
                              const Vector<Double>& pipars,
                              const Vector<Double>& papars,
                              const Double& rotMeas,
                              // circ pol parameters
                              //const Vector<Double>& cppars,
                              const MFrequency& reffreq,
                              const MEpoch& mtime,
                              const Int /*fldid*/)
{

  for(uInt selspw = 0; selspw < selToRawSpwIds.nelements(); ++selspw){
    // fluxUsed was supplied by the user instead of FluxStandard, so
    // make a component list for it now, for use in ft.

    // if spix is a float/double and q,u fluxes not set or pipars and papars not
    // set => SpectralIndex 
    // if spix is a float/double and q,u fluxes is set but not pipars and papers
    // => spectralindex
    // if spix is a float/double and q,u fluxes and pipars and papars are set
    // => tabular
    // if spix is a vector but qu fluxes nor pipars and papers are not set 
    // => tabular
    // if spix is a vector => tabular form
    //
    // Set the component flux density
    Flux<Double> fluxval;
    Flux<Double> fluxerr;
    fluxval.setValue(fluxUsed);
    // Create a point component at the field center
    // with the specified flux density
    // - obviously this does not correct for solar objects...
    PointShape point(fieldDir);

    Bool useTabularFlux(false);
    //check if to use tabular form or SpectralIndex model
    if ( spix.nelements() > 1 ) {
      useTabularFlux=true; 
    }
    else {
      //if (pipars.nelements() > 1 || papars.nelements() > 1 || rotMeas != 0.0) {
      if (pipars.nelements() > 1 || papars.nelements() > 1 ) {
        useTabularFlux=true;
      }
    }
    SpectralIndex siModel;
    //Vector<Double> iflux;
    //Vector<Double> qflux;
    //Vector<Double> uflux;
    Vector<Flux<Double> > fluxvalvec; 
    Bool gotQUFlux(false);
    Bool useFluxAsIs(false);
    // 
    if(reffreq.getValue().getValue() > 0.0){
      //original code uses first time of the data but shouldn't be using the same time as for
      //FluxStandard::makeComponentList?
      //MeasFrame mFrame(MEpoch(msc.timeMeas()(0)), mLocation_p, fieldDir);
      MeasFrame mFrame(mtime, mLocation_p, fieldDir);
      MFrequency::Convert cvt(mfreqs[selspw][0].getRef(), MFrequency::Ref(MFrequency::castType(reffreq.getRef().getType()), mFrame));
      siModel.setRefFrequency(reffreq);
      // if spix is not array of double,do this otherwise need to set flux densities by tabular... 
      //
      Int nchn = mfreqs[selspw].nelements(); 
      fluxvalvec.resize(nchn);
      Vector<Double> iflux(nchn);
      Vector<Double> qflux(nchn);
      Vector<Double> uflux(nchn);
      Vector<Double> vflux(nchn,0.0);
      // circular polarization fraction
      Double circpolFraction=0.0;
      if ( fluxUsed[0] !=0.0 && fluxUsed[3] != 0.0 ) {
        circpolFraction = fluxUsed[3]/fluxUsed[0]; 
      }

      if (spix.nelements()==1) {
        //siModel.setIndex(spix[0]);
        Vector<Double> stokesindex(4);
        stokesindex[0]=spix[0];
        stokesindex[1]=0.0;
        stokesindex[2]=rotMeas!=0.0? rotMeas: 0.0;
        stokesindex[3]=0.0;
        siModel.setStokesIndex(stokesindex);
        // still use iflux if q,u flux=0 but polindex and polangle is set
        for (uInt ichn = 0; ichn < uInt(nchn); ichn++) { 
          iflux[ichn] = fluxUsed[0] * siModel.sample(cvt(mfreqs[selspw][ichn]));
        }
      }
      else {
      // tabular case   
        sjy_calciflux(mfreqs[selspw],reffreq,fluxUsed[0],spix,iflux);
      }
      // linear pol
      Vector<Double> inpipars;
      Vector<Double> inpapars;
      if ( pipars.nelements() > 0 || papars.nelements() > 0 ) {
        inpipars.resize(pipars.nelements());
        inpipars=pipars;
        inpapars.resize(papars.nelements());
        inpapars=papars;
      }

      // Either Q or U non-zero, so use them
      if (fluxUsed[1] != 0.0 || fluxUsed[2] != 0.0) {
        // if Q U flux densities are given use that as 0th coefficient
        Double pi0 = sqrt(fluxUsed[1] * fluxUsed[1] + fluxUsed[2] * fluxUsed[2]) / fluxUsed[0];
        Double pa0 = 0.5 * atan2(fluxUsed[2],fluxUsed[1]);
        os<<LogIO::DEBUG1<<"Polindex c0="<<pi0<<", polangle c0="<<pa0
          <<" determined from input flux densities are used"<<LogIO::POST;
        if ( pipars.nelements() == 0 || papars.nelements() == 0 ) {
          inpipars.resize(1);
          inpapars.resize(1);
        }
        inpipars[0] = pi0;
        inpapars[0] = pa0;
      }
      //if (useTabularFlux) { 
      if (inpipars.nelements()!=0 && inpapars.nelements()!=0) { 
      //  cerr<<"running sjy_calcquflux...."<<endl;
      //     - returns qflux and uflux
        gotQUFlux = sjy_calcquflux(inpipars, inpapars, iflux, rotMeas, mfreqs[selspw], reffreq, qflux, uflux);
      }
      else if (fluxUsed[1] != 0.0 || fluxUsed[2] != 0.0 || fluxUsed[3] != 0.0) {
        gotQUFlux=true;
        useFluxAsIs=true;
      }
        /***
        if ( !useTabularFlux ) {
          Vector<Double> stokesIndex(4);
          stokesIndex[0] = index[0];
          //need to translate polindex ...etc to stokesIndex[1,2] 
          siModel.setStokesIndex(spix);
        }
        ***/ 
      for (uInt ichn=0; ichn < iflux.nelements(); ichn++) {
        if (!gotQUFlux) {
          qflux[ichn] = 0.0; 
          uflux[ichn] = 0.0;
        } 
        else if(useFluxAsIs) {
          qflux[ichn] = fluxUsed[1];
          uflux[ichn] = fluxUsed[2];
          vflux[ichn] = fluxUsed[3];
        }
        if ( circpolFraction != 0.0) vflux[ichn] = iflux[ichn]*circpolFraction;
        Flux<Double> iquvflux(iflux[ichn],qflux[ichn],uflux[ichn],vflux[ichn]);
        fluxvalvec[ichn] = iquvflux; 
      }
      returnFluxes[selspw][0]=fluxvalvec[0];
    }
    else{
      if(spix[0] != 0.0){            // If not the default, complain and quit.
        os << LogIO::SEVERE
           << "spix cannot be nonzero with reffreq = 0!"
           << LogIO::POST;
           //return false;
      }
      siModel.setRefFrequency(MFrequency(Quantity(1.0, "GHz")));
      siModel.setIndex(0.0);
    }
    // TODO: call tabular form method for full pol specification....
    //
    // No worries about varying fluxes or sizes here, so any time will do.
    if ( useTabularFlux ) {
      tempCLs[selspw] = FluxStandard::makeComponentList(fieldName,
                                                     mfreqs[selspw],
                                                     mtime, fluxvalvec, point,
                                                     ms_p->tableName() +
                                                     "_setjy_spw" +
                                                     String::toString(selspw) +
                                                     "_");
    }
    else {
    //if simodel is set use this 
      //cerr<<"NON-Tabular makeComponentList..."<<endl;
      //if (fluxval.value(1) ==0.0 && fluxval.value(2) == 0.0 && gotQUFlux) {

      // fluxval is @ thefreq, make sure proper freq is used 
      // Note that refreq in siModel will be overriden by thefreq 
      MFrequency thefreq = reffreq;
      if ( gotQUFlux) {
        fluxval=fluxvalvec[0]; 
        thefreq=mfreqs[selspw][0];
      }   
      tempCLs[selspw] = FluxStandard::makeComponentList(fieldName,
                                 //                    mfreqs[selspw][0],
                                                     thefreq,
                                                     mtime, fluxval, point,
                                                     siModel,
    // jagonzal (CAS-4109): Specify table name to avoid clashing between different CASA engines when running vs a MMS
                                                     ms_p->tableName() +
                                                     "_setjy_spw" +
                                                     String::toString(selspw) +
                                                     "_");
    }
  }
}

// modified the input model image by regridding, scaling with a flux standard
TempImage<Float>* Imager::sjy_prepImage(LogIO& os, FluxStandard& fluxStd,
                                        Vector<Double>& fluxUsed, Vector<Double>& freqsOfScale, 
                                        Vector<Double>& freqscale, const String& model,
                                        const MSSpWindowColumns& spwcols,
                                        //const Int rawspwid, const Bool chanDep,
                                        const Vector<Int> rawspwids, const Bool chanDep,
                                        const Vector<Vector<MFrequency> >& mfreqs,
                                        //const uInt selspw, const String& fieldName,
                                        const String& fieldName,
                                        const MDirection& fieldDir,
                                        const Unit& freqUnit,
                                        const Vector<Double>& fluxdens,
                                        const Bool precompute, 
                                        //const Double spix,
                                        const Vector<Double>& spix,
                                        const MFrequency& reffreq,
					const MEpoch& aveEpoch,
					const Int fieldId)
{
  TempImage<Float>* tmodimage = NULL;
  
  Double freqMax, freqMin;
  Vector<Vector<Int> >dummy;
  String msname=mssel_p->antenna().tableName();
  msname.erase(msname.length()-8);
  //adviseChanSelex(freqMin, freqMax, 0.0, MFrequency::LSRK, dummy, dummy, dummy, msname, fieldId, true, String::toString(rawspwid));
  // Get freqmin and freqmax in LSRK for the entire span of the selected spws
  String selSpwsStr;
  for (uInt ispw=0; ispw < rawspwids.nelements(); ispw++) {
    if (selSpwsStr!="") selSpwsStr += ", ";
    selSpwsStr += String::toString(rawspwids(ispw));
  }
  adviseChanSelex(freqMin, freqMax, 0.0, MFrequency::LSRK, dummy, dummy, dummy, msname, fieldId, true, selSpwsStr);

  // Find min channel width to increment to construct freqsofScale 
  Double freqWidth = 0;
  //
  for (uInt ispw = 0; ispw<rawspwids.nelements(); ispw++) {
    Vector<Double> freqWidths = spwcols.chanWidth()(rawspwids(ispw));

    Double minChanWidth = min(fabs(freqWidths));
    // freqWidth init....
    if (freqWidth == 0) 
      freqWidth = minChanWidth;
    else 
      freqWidth = min(freqWidth,minChanWidth); 
  }
  
  // ADDED for debug
  //Int rawspwid = rawspwids[0];
  //Vector<Double> freqArray = spwcols.chanFreq()(rawspwid);
  //Int nchan=freqArray.shape()[0]   ;
  Int nchan = Int(ceil(fabs(freqMax - freqMin)/freqWidth))+1;
  //cerr<<"nchan="<<nchan<<" freqMax="<<freqMax<<" freqMin="<<freqMin<<" freqWidth="<<freqWidth<<endl; 
  //Filling it with the LSRK values
  // SetJyGridFT will trigger nearestNeighbour interpolation for nchan>=2
  Vector<Double> freqArray(nchan);
  if (nchan==1) {
    freqArray[0] = freqMin;
  }
  else if (nchan==2) {
    freqArray[0] = freqMin;
    freqArray[1] = freqMax;
  }
  else if (nchan==3) {
    freqArray[0] = freqMin - freqWidth;
    freqArray[1] = freqMin;
    freqArray[2] = freqMin + freqWidth;
  }
  else {
    for (Int k =0;k < nchan; ++k){
      freqArray[k]=freqMin+k*freqWidth;
    }
  }
  //Vector<Double> freqInc = spwcols.chanWidth()(rawspwid);
  Double medianFreq = median(freqArray);
  freqsOfScale.resize();
  freqscale.resize();

  // 2 bw channel extra
  // UNCOMMENTED for debug
  //freqWidth = fabs(freqMax - freqMin) + 2 * max(freqInc);
  Matrix<Double> fluxUsedPerChan; // 4 rows nchan col ...will resize when needed

  // Set fluxUsedPerChan to the flux densities for each chan.
  if(chanDep || (spix[0] != 0.0 && fluxdens[0] != 0.0)){
    IPosition whichChan(1, 0);
    Flux<Double> returnFlux;
    Flux<Double> returnFluxErr;
    Double reffreqInGHz = 1.0;

    Unit ghz("GHz");
    if(!precompute)
      reffreqInGHz = reffreq.get(ghz).getValue();

    fluxUsedPerChan.resize(4, freqArray.nelements());
    for(uInt k = 0; k < freqArray.nelements(); ++k){
      whichChan[0] = k;
      if(precompute){
        //fluxStd.compute(fieldName, spwcols.chanFreqMeas()(rawspwid)(whichChan),
        //                returnFlux, returnFluxErr);
	fluxStd.compute(fieldName, fieldDir, MFrequency(Quantity(freqArray[k], "Hz"), MFrequency::LSRK),
			aveEpoch, returnFlux, returnFluxErr);
        returnFlux.value(fluxUsed);
      }
      else{
        // spix: index = c0 + c1*log(f/fo) + c2*log(f/fo)^2+ ... 
        //             = log(So) + alpha*log(f/fo) + curv1*log(f/f0)^2 ....
        uInt order = spix.nelements();
        Polynomial<Double> spixfunc(order);
        Vector<Double> coeffs(order+1);
        coeffs[0] = log10(fluxdens[0]); 
        for (uInt ispix = 1; ispix < order+1; ispix++) {
          coeffs[ispix] = spix[ispix-1];
        }
        spixfunc.setCoefficients(coeffs);

        //Double freq = spwcols.chanFreqMeas()(rawspwids(0))(whichChan).get(ghz).getValue();
        //Double specfac = pow(freq / reffreqInGHz, spix);
        // TT mod-06/11/14
        // freqArray may or may not be exactly match with data chan frequencies
        // so probably make sense to use  freqArray instead 
        //Double specfac = pow((freqArray[k]/1.e+09) / reffreqInGHz, spix);
        Double specfac = pow((freqArray[k]/1.e+09) / reffreqInGHz, spixfunc(freqArray[k]));
        
        for(uInt stokes = 0; stokes < 4; ++stokes)
          fluxUsed[stokes] = fluxdens[stokes] * specfac;
      }
      fluxUsedPerChan.column(k) = fluxUsed;
    }
  }
  PagedImage<Float> modimage(model);
  modimage.table().unmarkForDelete();
  IPosition imshape = modimage.shape();
  CoordinateSystem csys(modimage.coordinates());
  Int freqAxis = CoordinateUtil::findSpectralAxis(csys);
  Vector<Stokes::StokesTypes> whichPols;
  Int polAxis = CoordinateUtil::findStokesAxis(whichPols, csys);
  Int icoord = csys.findCoordinate(Coordinate::SPECTRAL);
  SpectralCoordinate spcsys = csys.spectralCoordinate(icoord);
  MEpoch elEpoch; MDirection elDir; MFrequency::Types elTypes; MPosition elPos;
  spcsys.getReferenceConversion(elTypes, elEpoch, elPos, elDir);
  spcsys.setReferenceConversion(MFrequency::LSRK, aveEpoch, elPos, elDir);  
  spcsys.setReferenceValue(Vector<Double>(1, medianFreq));
  spcsys.setReferencePixel(Vector<Double>(1, 0.0));
  spcsys.setWorldAxisUnits(Vector<String>(1,
					  //mfreqs[selspw][0].getUnit().getName()));
					  mfreqs[0][0].getUnit().getName()));
  //make image freq. width wide enough for FTMachine to work correctly 
  spcsys.setIncrement(Vector<Double>(1, 2*fabs(freqMax-freqMin)));
  // make a cube model if the model is a cube already
  if(modimage.shape()(freqAxis) >1){
    // model image is a cube...just regrid it then
    os << LogIO::NORMAL
       << "The model image is a cube, so it is being regridded but without scaling the flux density."
       << LogIO::POST;
    spcsys = SpectralCoordinate(
				//MFrequency::castType(mfreqs[selspw][0].getRef().getType()),
				MFrequency::castType(mfreqs[0][0].getRef().getType()),
                                freqArray, spcsys.restFrequency());
    imshape(freqAxis)=freqArray.nelements();
    csys.replaceCoordinate(spcsys, icoord);
    tmodimage = new TempImage<Float>(imshape, csys);
    sjy_regridCubeChans(tmodimage, modimage, freqAxis);
    //return from here itself
    return tmodimage;
  }
 
  if(chanDep && (fluxUsedPerChan.ncolumn() > 1)){
    //spcsys = SpectralCoordinate(
    //                  MFrequency::castType(mfreqs[selspw][0].getRef().getType()),
    //                            freqArray, spcsys.restFrequency());
    if(freqAxis < 2 || polAxis < 2)
      throw(AipsError("Cannot setjy with a model that has spectral or stokes axis before direction axes.\n Please reorder the axes of the image"));
    freqscale.resize(freqArray.nelements());
    freqsOfScale.resize(freqArray.nelements());
    freqsOfScale=freqArray;
    freqscale=1.0;
    if(freqAxis == 2) {//pol and freq are swapped
      imshape[2]=imshape[3];
      imshape[3] = 1;      
      Vector<Int> trans(4);
      trans[0] = 0; trans[1] = 1; trans[2] = 3; trans[3] = 2;
      csys.transpose(trans, trans);
    }
    else{
      imshape(freqAxis) = 1;
    }
  } 
  csys.replaceCoordinate(spcsys, icoord);
  tmodimage = new TempImage<Float>(imshape, csys);
  IPosition blcin(modimage.shape().nelements(), 0);
  IPosition trcin=modimage.shape()-1;
  IPosition blcout(imshape.nelements(), 0); 
  IPosition trcout=imshape-1;
  
  for (uInt ipol=0; ipol < imshape[2]; ++ipol){
    blcin[polAxis]=ipol;
    trcin[polAxis]=ipol;
    blcout[2]=ipol;
    trcout[2]=ipol;
    Slicer slin(blcin, trcin, Slicer::endIsLast);
    Slicer slout(blcout, trcout, Slicer::endIsLast);
    SubImage<Float> subimout(*tmodimage, slout, true);
    SubImage<Float> subimin(modimage, slin, false);
    subimout.copyData(subimin);
  }
  os << LogIO::DEBUG1
     << "freqUnit.getName() = " << freqUnit.getName()
     << LogIO::POST;
  os << LogIO::DEBUG1
     //<< "mfreqs[selspw].get(freqUnit).getValue() = "
     //<< mfreqs[selspw][0].get(freqUnit).getValue()
     << "mfreqs[0][0].get(freqUnit).getValue() = "
     << mfreqs[0][0].get(freqUnit).getValue()
     << LogIO::POST;

  // Check direction consistency (reported in log message below)
  String err;
  if(!CoordinateUtil::setDirectionConversion(err, csys, fieldDir.getRefString())){
    os << "LogIO::WARN " 
       << "Could not set direction conversion between flux image and " 
       << fieldDir.getRefString() << LogIO::POST;
  }
  Int dircoord(csys.findCoordinate(Coordinate::DIRECTION));
  DirectionCoordinate dircsys=csys.directionCoordinate(dircoord);
  MVDirection mvd;
  dircsys.toWorld(mvd,dircsys.referencePixel());
  Double sep = fieldDir.getValue().separation(mvd,"\"").getValue();
	  
  //Apply radius limit for 3C286,3C48,3C147 and 3C138
  sjy_setRadiusLimit(tmodimage, modimage, model, dircsys);

  // for debugging
  //PagedImage<Float> checkIm(TiledShape(modimage.shape(),
  //                                        modimage.niceCursorShape()),
  //                                  modimage.coordinates(),
  //                                  "checkImage");
  //checkIm.copyData((LatticeExpr<Float>)(*tmodimage));

  if(fluxdens[0] != 0.0){
    Float sumI = 1.0;

    // ?: can't handle the different return types.
    if(whichPols.nelements() > 1)
      //sumI = sum(ImagePolarimetry(modimage).stokesI()).getFloat();
      sumI = sum(ImagePolarimetry(*tmodimage).stokesI()).getFloat();
    else
      //sumI = sum(modimage).getFloat();
      sumI = sum(*tmodimage).getFloat();

    //if(selspw == 0)
      os << LogIO::NORMAL
         << "Using model image " << modimage.name() // Loglevel INFO
         << LogIO::POST;

    // scale the image
    if(freqscale.nelements() > 0){
      Int midchan = freqArray.nelements()/2;
      if(modimage.shape()(freqAxis) == 1){
	//     IPosition blc(imshape.nelements(), 0);
        //IPosition trc = imshape - 1;
        os << LogIO::NORMAL
           //<< "Scaling spw " << selspw << "'s model image by channel to I = " 
           << "Scaling spw(s) " << String::toString(rawspwids) << "'s model image by channel to  I = " 
           << fluxUsedPerChan.row(0)(0)<<", "
           << fluxUsedPerChan.row(0)(midchan)<<", "
           << fluxUsedPerChan.row(0)(nchan-1)
           << " Jy @("
           << freqArray(0)<<", "
           << freqArray(midchan)<<", "
           << freqArray(nchan-1)
           <<")Hz (LSRK) for visibility prediction (a few representative values are shown)."
           << LogIO::POST;
        writeHistory(os);
        for(uInt k = 0; k < fluxUsedPerChan.ncolumn(); ++k){
          freqscale[k] = fluxUsedPerChan.column(k)(0)/sumI;
          //blc[3] = k;
          //trc[3] = k;
          //Slicer sl(blc, trc, Slicer::endIsLast);
          //SubImage<Float> subim(*tmodimage, sl, true);
          //subim.copyData((LatticeExpr<Float>)(modimage*scale));
        }
        // for debug
        //cerr<<"freqscale="<<freqscale<<endl;
        //cerr<<"freqsOfScale="<<freqsOfScale<<endl;
      }
    }
    else{
      // Scale factor
      Float scale = fluxUsed[0] / sumI;
      //for addition of sjy_setRadiusLimit
      //tmodimage->copyData( (LatticeExpr<Float>)(modimage * scale) );	
      tmodimage->copyData( (LatticeExpr<Float>)(*tmodimage * scale) );	
      os << LogIO::NORMAL
     //    << "Scaling spw " << selspw << "'s model image to I = "
         << "Scaling spw(s) " << String::toString(rawspwids) << "'s model image to I = "
         << fluxUsed[0] // Loglevel INFO
         << " Jy @ "
         //<< mfreqs[0][0].getValue()
         << freqArray(0) 
         << "Hz (LSRK) for visibility prediction."
         << LogIO::POST;
      writeHistory(os);
    }
  }
  else{
    os << LogIO::NORMAL                                  // Loglevel INFO
 << "Using the model image's original unscaled flux density for visibility prediction."
       << LogIO::POST;
    writeHistory(os);
    // included in sjy_setRadiusLimit
    //tmodimage->copyData( (LatticeExpr<Float>)(modimage) );
  }
            
  //if(selspw == 0){
    os << LogIO::NORMAL // Loglevel INFO
       << "The model image's reference pixel is " << sep 
       << " arcsec from " << fieldName << "'s phase center."
       << LogIO::POST;
    writeHistory(os);
  //}

  return tmodimage;
}

Bool Imager::sjy_regridCubeChans(TempImage<Float>* tmodimage,
                                 PagedImage<Float>& modimage, Int freqAxis)
{
  if(freqAxis != 3)
    throw(AipsError("Cannot setjy with a cube model that does not have the spectral axis as the last one.\n Please reorder the axes of the image"));
  ImageRegrid<Float> ir;
  IPosition axes(1, freqAxis);   // regrid the spectral only
  ir.regrid(*tmodimage, Interpolate2D::LINEAR, axes, modimage);
  return true;
}

Bool Imager::sjy_setRadiusLimit(TempImage<Float>* tmodimage,
                                PagedImage<Float>& modimage, const String& model, DirectionCoordinate& dircsys)
{
  Path path(model);
  String basename=path.baseName();
  Float arad;
  // radius limit in arcsec from AIPS
  if (basename.find("3C286")==0) {
      arad=3.0;
  }
  else if (basename.find("3C48")==0) {
      arad=0.95;
  }
  else if (basename.find("3C147")==0) {
      arad=0.85;
  }
  else if (basename.find("3C138")==0) {
      arad=0.75;
  }
  else {
      arad=0;
      tmodimage->copyData(modimage);
      return true;
  }
  try {
    Quantity qrad(arad,"arcsec");
    Float prad=Float(qrad.get(Unit("rad")).getValue()/abs(dircsys.increment()(0)));
    Float radius = (prad >0.5 ? prad: 0.5);
    //Add current ms name to avoid file access clash for MMS processing
    String tempmaskname=ms_p->tableName()+"__tmp_mask_setjy_radiuslimit";
    PagedImage<Float> maskImage(TiledShape(modimage.shape(),
                                           modimage.niceCursorShape()),
                                modimage.coordinates(), tempmaskname);
    maskImage.table().markForDelete();
    Matrix<Float> circ(1,3);
    Record *imrec=0;
    Matrix<Quantity> blctrcs;
    circ(0,0)=radius;
    circ(0,1)=dircsys.referencePixel()(0);
    circ(0,2)=dircsys.referencePixel()(1);
    Imager::regionToImageMask(tempmaskname,imrec,blctrcs,circ,1.0);
    PagedImage<Float> tmpmask(tempmaskname);
    tmpmask.table().markForDelete();
    tmodimage->copyData( (LatticeExpr<Float>)(tmpmask*modimage));
  }
  catch (...) {
    return false;
  }
  return true;
}

Bool Imager::sjy_calciflux(const Vector<MFrequency>& freqs, const MFrequency& reffreq, 
                           const Double refflux, const Vector<Double>& vspix, Vector<Double>& iflux)
{
  try {
    // assume polynomical is log(S) = c0 + alpha*log(f/fo) + curv1*log(f/fo)^2+
    // vspix should contains [alpha, curv1, etc..] and c0 is calculated from refflux 
    uInt porder = vspix.nelements();
    Polynomial<Double> lf(porder);
    Vector<Double> coeffs(porder+1);
    coeffs[0] = log10(refflux);
    for (uInt i = 1; i < vspix.nelements()+1; i++ ) {
      coeffs[i] = vspix[i-1];
    }
    lf.setCoefficients(coeffs);
    Int nf = freqs.nelements();
    iflux.resize(nf);
    Unit frequnit("GHz");
    Double reffreqval = reffreq.get(frequnit).getValue();
    for (uInt cfidx = 0; cfidx < (uInt)nf; cfidx++) {
      iflux[cfidx] = pow(10.0,lf(log10(freqs[cfidx].get(frequnit).getValue()/reffreqval))); 
    }
  }
  catch (...) {
    return false;
  } 
  return true; 
}

Bool Imager::sjy_calcquflux(const Vector<Double>& pipars, const Vector<Double>& papars,
                            const Vector<Double>& iflux, const Double rotMeas,
                            const Vector<MFrequency>& freqs, 
                            const MFrequency& reffreq, Vector<Double>& qflux,
                            Vector<Double>& uflux)
{

  try {
    Int nf = freqs.nelements();
    //polindex
    // of the form, pi_o + c1*(f-fo)/fo + c2*(f-fo)/fo
    Polynomial<Double> pipoly(pipars.nelements());
    pipoly.setCoefficients(pipars);
    //pangle
    Polynomial<Double> papoly(papars.nelements());
    papoly.setCoefficients(papars);
    qflux.resize(nf);
    uflux.resize(nf);
    Unit ghz("Hz"); 
    Double f0 = reffreq.get(ghz).getValue();

    for (uInt cfidx = 0; cfidx < (uInt)nf; cfidx++) {
      Double f = freqs[cfidx].get(ghz).getValue();
      Double ipi = pipoly((f-f0)/f0);
      Double ipa = papoly((f-f0)/f0);
      Double iiflux = iflux[cfidx];
      Double qfluxval = ipi * iiflux * cos(2.0*ipa);
      Double ufluxval = ipi * iiflux * sin(2.0*ipa);
      //debug
      //if (cfidx<10) cerr<<"sjy_calcquflux:: poli="<<ipi<<" pola="<<ipa<<" qflux="<<qfluxval<<" uflux="<<ufluxval<<endl;
      if (rotMeas!=0.0 ) {
        Double rotangle = 2*rotMeas * C::c * C::c * (f0*f0-f*f)/(f*f*f0*f0);
        //if (cfidx<10) cerr<<"rotangle="<<rotangle<<endl;
        qflux[cfidx] = qfluxval*cos(rotangle) - ufluxval*sin(rotangle);
        uflux[cfidx] = qfluxval*sin(rotangle) + ufluxval*cos(rotangle); 
      }
      else { 
        qflux[cfidx] = qfluxval; 
        uflux[cfidx] = ufluxval; 
      }
      //if (cfidx<10) cerr<<"uflux/qflux["<<cfidx<<"]="<<uflux[cfidx]/qflux[cfidx]<<endl;
    }
  }
  catch (...) {
    return false;
  }
  return true;
}


Bool Imager::clone(const String& imageName, const String& newImageName)
{
  //if(!valid()) return false;
  // This is not needed if(!assertDefinedImageParameters()) return false;
  LogIO os(LogOrigin("imager", "clone()", WHERE));
  try {
    PagedImage<Float> oldImage(imageName);
    PagedImage<Float> newImage(TiledShape(oldImage.shape(), 
					  oldImage.niceCursorShape()), oldImage.coordinates(),
			       newImageName);
    newImage.set(0.0);
    newImage.table().flush(true, true);
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
  return true;
}

// Make an empty image
Bool Imager::make(const String& model)
{

  if(!valid())
    {
      return false;
    }
  LogIO os(LogOrigin("imager", "make()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters())
      {
	return false;
      }
    
    // Make an image with the required shape and coordinates
    String modelName(model);
    if(modelName=="") modelName=imageName()+".model";
    os << LogIO::DEBUG1
       << "Making empty image: " << modelName << LogIO::POST;
    
    removeTable(modelName);
    CoordinateSystem coords;
    //if(!imagecoordinates(coords, false)) 
    if(!imagecoordinates2(coords, false)) 
      {
	this->unlock();
	return false;
      }
    this->makeEmptyImage(coords, modelName, fieldid_p);
    this->unlock();
    
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;

    return false;    

  } 
  this->unlock();

  return true;
}

// Fit the psf. If psf is blank then make the psf first.
Bool Imager::fitpsf(const String& psf, ImageBeamSet& mbeam) {

  if(!valid()) 
    {
      return false;
    }
  LogIO os(LogOrigin("imager", "fitpsf()", WHERE));
  
  this->lock();
  try {
    if(!assertDefinedImageParameters()) 
      {
	this->unlock();
	return false;
      }
    
    os << LogIO::NORMAL << "Fitting to psf" << LogIO::POST; // Loglevel PROGRESS
    
    String lpsf; lpsf=psf;
    if(lpsf=="") {
      lpsf=imageName()+".psf";
      makeimage("psf", lpsf);
    }

    if(!Table::isReadable(lpsf)) {
      this->unlock();
      os << LogIO::SEVERE << "PSF image " << lpsf << " does not exist"
	 << LogIO::POST;

      return false;
    }

    PagedImage<Float> psfImage(lpsf);
    StokesImageUtil::FitGaussianPSF(psfImage, mbeam);
    beam_p = mbeam;
    beamValid_p=true;
    
    GaussianBeam elbeam=beam_p(0,0);
    os << LogIO::NORMAL // Loglevel INFO
       << "  Beam fit: " << elbeam.getMajor("arcsec") << " by "
       << elbeam.getMinor("arcsec") << " (arcsec) at pa "
       << elbeam.getPA(Unit("deg")) << " (deg) " << endl;

    this->unlock();
    
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;

     return false;
  } 
  this->unlock();

  return true;
}


Bool Imager::setscales(const String& scaleMethod,
			    const Int inscales,
			    const Vector<Float>& userScaleSizes)
{
  scaleMethod_p = scaleMethod;
  userScaleSizes_p.resize(userScaleSizes.nelements());
  userScaleSizes_p = userScaleSizes;
  if (scaleMethod_p == "uservector") {
    nscales_p =  userScaleSizes.nelements();
  } else {
    nscales_p = inscales;
  }
  //Force the creation of a new sm_p with the new scales
  destroySkyEquation();
  scaleInfoValid_p = true;  
  return true;
};

Bool Imager::setSmallScaleBias(const Float inbias)
{ 
  smallScaleBias_p = inbias;
  return true;
}

// Added for wb algo.
Bool Imager::settaylorterms(const Int intaylor,const Double inreffreq)
{
  ntaylor_p = intaylor;
  reffreq_p = inreffreq;
  return true;
};

// Set the beam
Bool Imager::setbeam(const ImageBeamSet& mbeam)
{
  if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "setbeam()", WHERE));
  beam_p = ImageBeamSet(mbeam);
  beamValid_p=true;
    
  return true;
}

// Plot the uv plane
Bool Imager::plotuv(const Bool rotate) 
{

  if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "plotuv()", WHERE));
  
  this->lock();
  try {
    os << LogIO::NORMAL // Loglevel PROGRESS
       << "Plotting uv coverage for currently selected data" << LogIO::POST;
    
    ROVisIter& vi(*rvi_p);
    VisBuffer vb(vi);
    
    uInt nVis = count_visibilities(rvi_p, true, true);
    
    if(nVis==0) {
      this->unlock();
      os << LogIO::SEVERE << "No unflagged visibilities" << LogIO::POST;
      return false;
    }
    
    if(rotate) {
      os << LogIO::NORMAL // Loglevel INFO
         << "UVW will be rotated to specified phase center" << LogIO::POST;    
    }
    
    
    Vector<Float> u(nVis); u=0.0;
    Vector<Float> v(nVis); v=0.0;
    Vector<Float> uRotated(nVis); uRotated=0.0;
    Vector<Float> vRotated(nVis); vRotated=0.0;
    Float maxAbsUV=0.0;
    
    Int iVis=0;
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	Int nRow=vb.nRow();
	Int nChan=vb.nChannel();
	Vector<Double> uvwRotated(3);
	MeasFrame mFrame((MEpoch(Quantity(vb.time()(0), "s"))), mLocation_p);
	UVWMachine uvwMachine(phaseCenter_p, vb.phaseCenter(), mFrame);
	for (Int row=0; row<nRow; ++row) {
	  if(rotate) {
	    for (Int dim=0;dim<3;++dim) {
	      uvwRotated(dim)=vb.uvw()(row)(dim);
	    }
	    uvwMachine.convertUVW(uvwRotated);
	  }
	  
	  for (Int chn=0; chn<nChan; ++chn) {
	    if(!vb.flag()(chn,row)&&vb.imagingWeight()(chn,row)>0.0) {
	      Float f=vb.frequency()(chn)/C::c;
	      u(iVis)=vb.uvw()(row)(0)*f;
	      v(iVis)=vb.uvw()(row)(1)*f;
	      if(abs(u(iVis))>maxAbsUV) maxAbsUV=abs(u(iVis));
	      if(abs(v(iVis))>maxAbsUV) maxAbsUV=abs(v(iVis));
	      if(rotate) {
		uRotated(iVis)=uvwRotated(0)*f;
		vRotated(iVis)=uvwRotated(1)*f;
		if(abs(uRotated(iVis))>maxAbsUV) maxAbsUV=abs(uRotated(iVis));
		if(abs(vRotated(iVis))>maxAbsUV) maxAbsUV=abs(vRotated(iVis));
	      }
	      ++iVis;
	    }
	  }
	}
      }
    }
    
    if(maxAbsUV==0.0) {
      this->unlock();
      os << LogIO::SEVERE << "Maximum uv distance is zero" << LogIO::POST;
      return false;
    }
    else {
      Quantity cell(0.5/maxAbsUV, "rad");
      os << LogIO::NORMAL // Loglevel INFO
         << "Maximum uv distance = " << maxAbsUV << " wavelengths" << endl;
      os << LogIO::NORMAL // Loglevel INFO
         << "Recommended cell size < " << cell.get("arcsec").getValue()
	 << " arcsec" << LogIO::POST;
    }
    
   
    return false;

    this->unlock();
  
  } 
  catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
    
  } 
  catch (...) {
    this->unlock();  
  } 
  this->unlock();
  
  

  return true;
}

// Plot the visibilities
Bool Imager::plotvis(const String& type, const Int increment) 
{

  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "plotvis()", WHERE));
  
  this->lock();
  try {
    
    os << LogIO::NORMAL // Loglevel PROGRESS
       << "Plotting Stokes I visibility for currently selected data"
       << LogIO::POST;
    
    
    MSColumns msc(*mssel_p);
    Bool hasCorrected=!(msc.correctedData().isNull());
    Bool hasModel= true; //with virtual model data service model data is always there
    //why bother if it is not requested
    if(!((type == "all") || (type=="model") || (type == "residual")))
      hasModel=false;
    if(!((type == "all") || (type=="corrected") || (type == "residual")))
      hasCorrected=false;
    


    Bool twoPol=true;
    Vector<String> polType=msc.feed().polarizationType()(0);
    if (polType(0)!="X" && polType(0)!="Y" &&
	polType(0)!="R" && polType(0)!="L") {
      twoPol=false;
    }
    
    ROVisIter& vi(*rvi_p);
    VisBuffer vb(vi);
    
    Int nVis=0;
    Int counter=0;
    Float maxWeight=0;
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	Int nRow=vb.nRow();
	Int nChan=vb.nChannel();
	maxWeight=max(maxWeight, max(vb.imagingWeight()));
	for (Int row=0; row<nRow; ++row) {
	  for (Int chn=0; chn<nChan; ++chn) {
	    if(!vb.flag()(chn,row)&&vb.imagingWeight()(chn,row)>0.0) {
	      ++counter;
	      if(counter==increment) {
		counter=0;
		++nVis;
	      }
	    }
	  }
	}
      }
    }
    
    if(nVis==0) {
      os << LogIO::SEVERE << "No unflagged visibilities" << LogIO::POST;
      if(maxWeight <=0){
	os << LogIO::SEVERE << "Max of imaging-weight is " << maxWeight 
	   << LogIO::POST;
	os << LogIO::SEVERE << "Try setting it with the function weight"  
	   << LogIO::POST;
      }
      this->unlock();
      return false;
    }
    
    if(increment>1) {
      os << LogIO::NORMAL << "For increment = " << increment << ", found " << nVis // Loglevel INFO
	 << " points for plotting" << endl;
    }
    else {
      os << LogIO::NORMAL << "Found " << nVis << " points for plotting" << endl; // Loglevel INFO
    }
    Vector<Float> amp(nVis); amp=0.0;
    Vector<Float> correctedAmp(nVis); correctedAmp=0.0;
    Vector<Float> modelAmp(nVis); modelAmp=0.0;
    Vector<Float> residualAmp(nVis); residualAmp=0.0;
    Vector<Float> uvDistance(nVis); uvDistance=0.0;
   
    if(!hasModel)
      modelAmp.resize();
    if(!hasCorrected)
      correctedAmp.resize();
    if(!hasCorrected || !hasModel)
      residualAmp.resize();
    

    Float maxuvDistance=0.0;
    Float maxAmp=0.0;
    Float maxCorrectedAmp=0.0;
    Float maxModelAmp=0.0;
    Float maxResidualAmp=0.0;
    Int iVis=0;
    counter=0;
    vi.originChunks();
    vi.origin();
    uInt numCorrPol=vb.nCorr();
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	Int nRow=vb.nRow();
	Int nChan=vb.nChannel();
	for (Int row=0; row<nRow; ++row) {
	  for (Int chn=0; chn<nChan; ++chn) {
	    if(!vb.flag()(chn,row)&&vb.imagingWeight()(chn,row)>0.0) {
	      ++counter;
	      if(counter==increment) {
		counter=0;
		Float f=vb.frequency()(chn)/C::c;
		Float u=vb.uvw()(row)(0)*f; 
		Float v=vb.uvw()(row)(1)*f;
		uvDistance(iVis)=sqrt(square(u)+square(v));
		if(twoPol) {
		  amp(iVis)=sqrt((square(abs(vb.visCube()(0,chn,row)))+
				  square(abs(vb.visCube()(numCorrPol,chn,row))))/2.0);
		  if(hasCorrected)
		    correctedAmp(iVis)=
		      sqrt((square(abs(vb.correctedVisCube()(0,chn,row)))+
			    square(abs(vb.correctedVisCube()(numCorrPol,chn,row))))/2.0);
		  if(hasModel)
		    modelAmp(iVis)=
		      sqrt((square(abs(vb.modelVisCube()(0,chn,row)))+
			    square(abs(vb.modelVisCube()(numCorrPol,chn,row))))/2.0);
		  if(hasCorrected && hasModel)
		    residualAmp(iVis)=
		      sqrt((square(abs(vb.modelVisCube()(0,chn,row)-
				       vb.correctedVisCube()(0,chn,row)))+
			    square(abs(vb.modelVisCube()(numCorrPol,chn,row)-
				       vb.correctedVisCube()(numCorrPol,chn,row))))/2.0);
		}
		else {
		  amp(iVis)=abs(vb.visCube()(0,chn,row));
		  if(hasCorrected)
		    correctedAmp(iVis)=abs(vb.correctedVisCube()(0,chn,row));
		   if(hasModel)
		     modelAmp(iVis)=abs(vb.modelVisCube()(0,chn,row));
		  if(hasCorrected && hasModel) 
		    residualAmp(iVis)=
		      abs(vb.modelVisCube()(0,chn,row)-
			  vb.correctedVisCube()(0,chn,row));
		}
		if(uvDistance(iVis)>maxuvDistance) {
		  maxuvDistance=uvDistance(iVis);
		}
		if(amp(iVis)>maxAmp) {
		  maxAmp=amp(iVis);
		}
		if(hasCorrected && (correctedAmp(iVis)>maxCorrectedAmp)) {
		  maxCorrectedAmp=correctedAmp(iVis);
		}
		if(hasModel && (modelAmp(iVis)>maxModelAmp)) {
		  maxModelAmp=modelAmp(iVis);
		}
		if((hasModel&&hasCorrected) && (residualAmp(iVis)>maxResidualAmp)) {
		  maxResidualAmp=residualAmp(iVis);
		}
		++iVis;
	      }
	    }
	  }
	}
      }
    }
    


    if(maxuvDistance==0.0) {
      os << LogIO::SEVERE << "Maximum uv distance is zero" << LogIO::POST;
      this->unlock();
      return false;
    }
    


    

    Float Ymax(0.0);

    if (type.contains("corrected") && hasCorrected)
      if(maxCorrectedAmp>Ymax) Ymax = maxCorrectedAmp;

    if (type.contains("model") && hasModel)
      if(maxModelAmp>Ymax)     Ymax = maxModelAmp;

    if (type.contains("residual") && (hasModel && hasCorrected))
      if(maxResidualAmp>Ymax)  Ymax = maxResidualAmp;

    if (type.contains("observed"))
      if(maxAmp>Ymax)          Ymax = maxAmp;

    if ((type=="all") || (type == ""))
      {
	if (maxAmp > Ymax)       Ymax = maxAmp;
	if(hasCorrected && (maxCorrectedAmp>Ymax)) Ymax = maxCorrectedAmp;
	if(hasModel && (maxModelAmp>Ymax))     Ymax = maxModelAmp;
	if((hasModel && hasCorrected) && maxResidualAmp>Ymax)  Ymax = maxResidualAmp;
      }
   

    return false;

    
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
  this->unlock();

  return true;
}

// Plot the weights
Bool Imager::plotweights(const Bool gridded, const Int increment) 
{

  if(!valid()) return false;
  LogIO os(LogOrigin("imager", "plotweights()", WHERE));
  
  this->lock();
  try {
    
    
    os << LogIO::NORMAL // Loglevel PROGRESS
       << "Plotting imaging weights for currently selected data"
       << LogIO::POST;
    
    ROVisIter& vi(*rvi_p);
    VisBuffer vb(vi);
    
    if(gridded) {
      if(!assertDefinedImageParameters()) {this->unlock(); return false;}
      // First find the gridded weights
      Float uscale, vscale;
      Int uorigin, vorigin;
      uscale=(nx_p*mcellx_p.get("rad").getValue())/2.0;
      vscale=(ny_p*mcelly_p.get("rad").getValue())/2.0;
      uorigin=nx_p/2;
      vorigin=ny_p/2;
      
      // Simply declare a big matrix 
      Float maxWeight=0.0;
      Matrix<Float> gwt(nx_p,ny_p);
      gwt=0.0;
      
      Float u, v;
      Float sumwt=0.0;
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	for (vi.origin();vi.more();vi++) {
	  Int nRow=vb.nRow();
	  Int nChan=vb.nChannel();
	  for (Int row=0; row<nRow; ++row) {
	    for (Int chn=0; chn<nChan; ++chn) {
	      if(!vb.flag()(chn,row)&&vb.imagingWeight()(chn,row)>0.0) {
		Float f=vb.frequency()(chn)/C::c;
		u=vb.uvw()(row)(0)*f; 
		v=vb.uvw()(row)(1)*f;
		Int ucell=Int(uscale*u+uorigin);
		Int vcell=Int(vscale*v+vorigin);
		if((ucell>0)&&(ucell<nx_p)&&(vcell>0)&&(vcell<ny_p)) {
		  gwt(ucell,vcell)+=vb.imagingWeight()(chn,row);
		  sumwt+=vb.imagingWeight()(chn,row);
		  if(vb.imagingWeight()(chn,row)>maxWeight) {
		    maxWeight=vb.imagingWeight()(chn,row);
		  }
		}
		ucell=Int(-uscale*u+uorigin);
		vcell=Int(-vscale*v+vorigin);
		if((ucell>0)&&(ucell<nx_p)&&(vcell>0)&&(vcell<ny_p)) {
		  gwt(ucell,vcell)+=vb.imagingWeight()(chn,row);
		}
	      }
	    }
	  }
	}
      }
      
      if(sumwt>0.0) {
	os << LogIO::NORMAL << "Sum of weights = " << sumwt << endl; // Loglevel INFO
      }
      else {
	this->unlock();
	os << LogIO::SEVERE << "Sum of weights is zero: perhaps you need to weight the data"
	   << LogIO::POST;
	  return false;
      }
     
      
      //Float umax=Float(nx_p/2)/uscale;
      //Float vmax=Float(ny_p/2)/vscale;


      return false;
    }
    else {
      
      // Now do the points plot
      Int nVis=0;
      Int counter=0;
      Float maxWeight=0.0;
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	for (vi.origin();vi.more();vi++) {
	  Int nRow=vb.nRow();
	  Int nChan=vb.nChannel();
	  for (Int row=0; row<nRow; ++row) {
	    for (Int chn=0; chn<nChan; ++chn) {
	      if(!vb.flag()(chn,row)&&vb.imagingWeight()(chn,row)>0.0) {
		++counter;
		if(counter==increment) {
		  counter=0;
		  ++nVis;
		}
	      }
	    }
	  }
	}
      }
      
      if(increment>1) {
	os << LogIO::NORMAL // Loglevel INFO
           << "For increment = " << increment << ", found " << nVis
	   << " points for plotting" << endl;
      }
      else {
	os << LogIO::NORMAL // Loglevel INFO
           << "Found " << nVis << " points for plotting" << endl;
      }
      
      Float maxuvDistance=0.0;
      Vector<Float> weights(nVis);
      Vector<Float> uvDistance(nVis);
      weights=0.0;
      uvDistance=0.0;
      
      Int iVis=0;
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	for (vi.origin();vi.more();vi++) {
	  Int nRow=vb.nRow();
	  Int nChan=vb.nChannel();
	  for (Int row=0; row<nRow; ++row) {
	    for (Int chn=0; chn<nChan; ++chn) {
	      if(!vb.flag()(chn,row)&&vb.imagingWeight()(chn,row)>0.0) {
		++counter;
		if(counter==increment) {
		  Float f=vb.frequency()(chn)/C::c;
		  Float u=vb.uvw()(row)(0)*f; 
		  Float v=vb.uvw()(row)(1)*f;
		  uvDistance(iVis)=sqrt(square(u)+square(v));
		  weights(iVis)=vb.imagingWeight()(chn,row);
		  if(vb.imagingWeight()(chn,row)>maxWeight) {
		    maxWeight=vb.imagingWeight()(chn,row);
		  }
		  if(uvDistance(iVis)>maxuvDistance) {
		    maxuvDistance=uvDistance(iVis);
		  }
		  counter=0;
		  ++iVis;
		}
	      }
	    }
	  }
	}
      }
      
      if(maxuvDistance==0.0) {
	this->unlock();
	os << LogIO::SEVERE << "Maximum uv distance is zero" << LogIO::POST;
	return false;
      }


      return false;
    }
    

    
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
  this->unlock();

  return true;
}

Bool Imager::clipvis(const Quantity& threshold) 
{

  if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "clipvis()", WHERE));
  
  this->lock();
  try {
    
    Float thres=threshold.get("Jy").getValue();
    
    os << LogIO::NORMAL // Loglevel PROGRESS
       << "Clipping visibilities where residual visibility > "
       << thres << " Jy" << LogIO::POST;
    if(!wvi_p){
      os << LogIO::WARN
         << "Cannot clip visibilities in read only mode of ms" 
	 << LogIO::POST;
      return false;
    }
    VisIter& vi(*wvi_p);
    VisBuffer vb(vi);
    

    vi.originChunks();
    vi.origin();
// Making sure picking LL for [RR RL LR LL] correlations or [RR LL] 
    uInt numCorrPol=vb.modelVisCube().shape()(0) - 1 ; 
    Int nBad=0;
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	Int nRow=vb.nRow();
	Int nChan=vb.nChannel();
	for (Int row=0; row<nRow; ++row) {
	  for (Int chn=0; chn<nChan; ++chn) {
	    if(!vb.flag()(chn,row)) {
	      Float residualAmp=
		sqrt((square(abs(vb.modelVisCube()(0,chn,row)-
				 vb.correctedVisCube()(0,chn,row)))+
		      square(abs(vb.modelVisCube()(numCorrPol,chn,row)-
				 vb.correctedVisCube()(numCorrPol,chn,row))))/2.0);
	      if(residualAmp>thres) {
		vb.flag()(chn,row)=true;
		++nBad;
	      }
	    }
	  }
	}
	vi.setFlag(vb.flag());
      }
    }
    
    os << LogIO::NORMAL << "Flagged " << nBad << " points" << LogIO::POST; // Loglevel INFO
    
    this->unlock();
    return true;
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  this->unlock();

  return true;
}

// Plot various ids
Bool Imager::plotsummary() 
{
  
  if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "plotsummary()", WHERE));
  
  os << LogIO::WARN << "NOT implemented " << LogIO::POST;
  return false;

  this->lock();
  try {
    /*
    os << "Plotting field and spectral window ids for currently selected data" << LogIO::POST;
    
    ROVisIter& vi(*rvi_p);
    VisBuffer vb(vi);
    
    Int nVis=0;
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	Int nRow=vb.nRow();
	for (Int row=0; row<nRow; ++row) {
	  ++nVis;
	}
      }
    }
    
    os << "Found " << nVis << " selected records" << LogIO::POST;
    
    Vector<Float> fieldId(nVis);
    Vector<Float> spectralWindowId(nVis);
    Vector<Double> t(nVis);
    
    Int maxFieldId=0;
    Int maxSpectralWindowId=0;
    Int iVis=0;
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	Int nRow=vb.nRow();
	for (Int row=0; row<nRow; ++row) {
	  t(iVis)=vb.time()(row);
	  fieldId(iVis)=vb.fieldId()+1.0;
	  spectralWindowId(iVis)=vb.spectralWindow()+1.003;
	  if(Int(fieldId(iVis))>maxFieldId) maxFieldId=Int(fieldId(iVis));
	  if(Int(spectralWindowId(iVis))>maxSpectralWindowId)
	    maxSpectralWindowId=Int(spectralWindowId(iVis));
	  ++iVis;
	}
      }
    }
    
    Double tStart=t(0);
    Vector<Float> timeFloat(nVis);
    for(Int i=0;i<nVis;++i) {
      timeFloat(i)=Float(t(i)-tStart);
    }
    
    MSColumns msc(*ms_p);
    PGPlotter plotter=getPGPlotter();
    plotter.subp(1, 2);
    plotter.page();
    plotter.swin(timeFloat(0), timeFloat(nVis-1)*1.20, 0, Float(maxFieldId)*1.1);
    plotter.tbox("BCSNTZHFO", 0.0, 0, "ABCNTS", 0.0, 0);
    String xLabel="Time (offset from " + MVTime(tStart/86400.0).string() + ")";
    plotter.lab(xLabel, "ID", "Field IDs for " +imageName());
    plotter.sci(1);
    for (Int fid=0;fid<maxFieldId;++fid) {
      String fieldName=msc.field().name()(fid);
      plotter.text(1.02*timeFloat(nVis-1), Float(fid+1), fieldName);
    }
    plotter.pt(timeFloat,fieldId,-1);
    plotter.page();
    plotter.swin(timeFloat(0), timeFloat(nVis-1)*1.20, 0,
		 Float(maxSpectralWindowId)*1.1);
    plotter.tbox("BCSNTZHFO", 0.0, 0, "ABCNTS", 0.0, 0);
    xLabel="Time (offset from " + MVTime(tStart/86400.0).string() + ")";
    plotter.lab(xLabel, "ID", "Spectral Window IDs for " +imageName());
    plotter.sci(1);
    for(Int spwId=0;spwId<maxSpectralWindowId;++spwId) {
      Vector<Double> chanFreq=msc.spectralWindow().chanFreq()(spwId); 
      ostringstream spwString;
      spwString<<chanFreq(0)/1.0e9<<" GHz";
      plotter.text(1.02*timeFloat(nVis-1), Float(spwId+1),
		   spwString);
    }
    plotter.pt(timeFloat,spectralWindowId,-1);
    plotter.iden();
    this->unlock();
    return true;
    */
  } catch (AipsError x) {
    this->unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
    
  this->unlock();

  return true;
}


Bool Imager::detached() const
{
  if (ms_p.null()) {
    LogIO os(LogOrigin("imager", "detached()", WHERE));
    os << LogIO::SEVERE << 
      "imager is detached - cannot perform operation." << endl <<
      "Call imager.open('filename') to reattach." << LogIO::POST;
    return true;
  }
  return false;
}

Bool Imager::makemodelfromsd(const String& sdImage, const String& modelImage, 
			     const String& lowPSF, String& maskImage)
{

 if(!valid()) return false;
  
  LogIO os(LogOrigin("imager", "makemodelfromsd()", WHERE));
  
  try {
    
    if(!Table::isReadable(sdImage)){
      os << LogIO::SEVERE  << "Single Dish " << sdImage 
	 << "  image is not readable" << LogIO::EXCEPTION;
      
      return false;
    }

    os << LogIO::NORMAL << "Creating an initial model image " << modelImage  // Loglevel INFO
       << " from single dish image " << sdImage << LogIO::POST;
    
    CoordinateSystem coordsys;
    //imagecoordinates(coordsys);
    imagecoordinates2(coordsys);
    String modelName=modelImage;
    this->makeEmptyImage(coordsys, modelName, fieldid_p);
    
    PagedImage<Float> model(modelImage);
    PagedImage<Float> low0(sdImage);
    String sdObs=low0.coordinates().obsInfo().telescope();

    GaussianBeam lBeam;
    ImageInfo lowInfo=low0.imageInfo();
    lBeam=lowInfo.restoringBeam();
  
    Float beamFactor=-1.0;

    
    // regrid the single dish image
    {
      ImageRegrid<Float> ir;
      IPosition axes(3,0,1,3);   // if its a cube, regrid the spectral too
      ir.regrid(model, Interpolate2D::LINEAR, axes, low0);
    }
    
  

    // Will need to make a complex image to apply the beam
    TempImage<Complex> ctemp(model.shape(), model.coordinates());
    if(lowPSF=="") {
      os << LogIO::NORMAL // Loglevel INFO
         << "Using primary beam of single dish to determine flux scale"
         << LogIO::POST;

      TempImage<Float> beamTemp(model.shape(), model.coordinates());
      //Make the PB accordingly
      if(lBeam.isNull()) {
      
	if (doDefaultVP_p) { 
	  if(telescope_p!=""){
	    ObsInfo myobsinfo=this->latestObsInfo();
	    myobsinfo.setTelescope(telescope_p);
	    coordsys.setObsInfo(myobsinfo);
	    
	  }
	  else{
	    if(sdObs != ""){
	      telescope_p=sdObs;
	      ObsInfo myobsinfo=this->latestObsInfo();
	      myobsinfo.setTelescope(telescope_p);
	      coordsys.setObsInfo(myobsinfo);
	    }
	    else{
	      telescope_p=coordsys.obsInfo().telescope();
	    }
	  }
	  beamTemp.setCoordinateInfo(coordsys);
	  this->makePBImage(beamTemp);
	 
	}
	else{
	  Table vpTable(vpTableStr_p);
	  this->makePBImage(vpTable, beamTemp);	
	}
	StokesImageUtil::FitGaussianPSF(beamTemp, lBeam);
	LatticeExprNode sumImage = sum(beamTemp);
	beamFactor=sumImage.getFloat();
	
      }
      
      
    }
    else {
      os << LogIO::NORMAL // Loglevel INFO
         << "Using specified low resolution PSF to determine sd flux scale"
         << LogIO::POST;
      // regrid the single dish psf
      PagedImage<Float> lowpsf0(lowPSF);
      TempImage<Float> lowpsf(model.shape(), model.coordinates());
      {
	ImageRegrid<Float> ir;
	IPosition axes(2,0,1);   //
	ir.regrid(lowpsf, Interpolate2D::LINEAR, axes, lowpsf0);
      }
      LatticeExprNode sumImage = sum(lowpsf);
      beamFactor=sumImage.getFloat();
      if(lBeam.isNull()) {
	os << LogIO::NORMAL << "Finding SD beam from given PSF" << LogIO::POST; // Loglevel PROGRESS
	StokesImageUtil::FitGaussianPSF(lowpsf0, lBeam);
      }
    }
    

    // This factor comes from the beam volumes
    if(sdScale_p!=1.0)
      os << LogIO::DEBUG1
         << "Multiplying single dish data by user specified factor "
         << sdScale_p << LogIO::POST;
    Float sdScaling  = sdScale_p;
    if(! lBeam.isNull()) {
      Int directionIndex=model.coordinates().findCoordinate(Coordinate::DIRECTION);
      DirectionCoordinate
	directionCoord=model.coordinates().directionCoordinate(directionIndex);
      Vector<String> units(2); units.set("arcsec");
      directionCoord.setWorldAxisUnits(units); 
      Vector<Double> incr= directionCoord.increment();
      if(beamFactor >0.0) {
	beamFactor=1.0/beamFactor;
      }
      else{
	//	beamFactor=
	//	  abs(incr(0)*incr(1))/(lBeam(0).get("arcsec").getValue()*lBeam(1).get("arcsec").getValue()*1.162);
	//Brute Force for now.
	IPosition imshape(4, nx_p, ny_p, 1, 1);
	TempImage<Float> lowpsf(imshape, coordsys);
	lowpsf.set(0.0);
	IPosition center(4, Int((nx_p/4)*2), Int((ny_p/4)*2),0,0);
        lowpsf.putAt(1.0, center);
	StokesImageUtil::Convolve(lowpsf, lBeam, false);
	LatticeExprNode sumImage = sum(lowpsf);
	beamFactor=1.0/sumImage.getFloat();

	
      }
      os << LogIO::NORMAL << "Beam volume factor  " // Loglevel INFO
	 <<  beamFactor << LogIO::POST;
      sdScaling*=beamFactor;
    }
    else {
      os << LogIO::WARN << "Insufficient information to scale correctly" << LogIO::POST;
    }
 
    //Convert to Jy/pixel
    model.copyData(  (LatticeExpr<Float>)((model * sdScaling)));
    model.setUnits(Unit("Jy/pixel"));
    
    //make a mask image
    this->makeEmptyImage(coordsys, maskImage, fieldid_p);
    PagedImage<Float> mask(maskImage);
    mask.set(1.0);
    ArrayLattice<Bool> sdMask(model.getMask());
    mask.copyData( LatticeExpr<Float> (mask* ntrue(sdMask)*model));
    StokesImageUtil::MaskFrom(mask, mask, Quantity(0.0, "Jy"));
    model.copyData( LatticeExpr<Float> (mask*model));
    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::POST;
    return false;
  } 
  
  return true;


}


Int Imager::interactivemask(const String& image, const String& mask, 
			    Int& niter, Int& ncycles, String& thresh, const Bool forceReload){

  LogIO os(LogOrigin("Imager", "interactivemask()", WHERE));
   if(Table::isReadable(mask)) {
    if (! Table::isWritable(mask)) {
      os << LogIO::WARN << "Mask image is not modifiable " << LogIO::POST;
      return false;
    }
    //we should regrid here if image and mask do not match
   }
   else{
     clone(image, mask);
   }
#ifdef USE_GRPC
   Quantity thr;
   if ( ! Quantity::read(thr,thresh) ) thr = Quantity(0,"Jy");
   float thold = (float) thr.get("Jy").getValue( );
   grpcInteractiveClean::getManager( ).setControls( niter, ncycles, thold);
#endif
   return false;
}

  Record Imager::iClean(const String& algorithm, const Int niter, const Double gain, 
		      const Quantity& threshold,
		      const Bool displayprogress,
		      const Vector<String>& model,
		      const Vector<Bool>& keepfixed, const String& complist,
		      const Vector<String>& mask,
		      const Vector<String>& image,
		      const Vector<String>& residual,
		      const Vector<String>& psfnames,
		      const Bool interactive, const Int npercycle,
		      const String& masktemplate)
  {
    Record rstat;
      
    logSink_p.clearLocally();
    LogIO os(LogOrigin("imager", "iClean()"), logSink_p);

    if(!ms_p.null()) {
      //try
       {
       
	Vector<String> amodel(model);
	Vector<Bool>   fixed(keepfixed);
	Vector<String> amask(mask);
	Vector<String> aimage(image);
	Vector<String> aresidual(residual);
	Vector<String> apsf(psfnames);
	
	if(String(algorithm) != "msmfs") ntaylor_p=1; /* masks increment by ntaylor_p only for msmfs */
	uInt nmods = aresidual.nelements()/ntaylor_p;

	if( (apsf.nelements()==1) && apsf[0]==String(""))
	  apsf.resize();
	if(!interactive){
	  rstat = clean(String(algorithm), niter, gain,  
			threshold, displayprogress, 
			amodel, fixed, String(complist), amask,  
			aimage, aresidual, apsf);
	}
	else{
	  if((amask.nelements()==0) || (amask[0]==String(""))){
	    amask.resize(amodel.nelements());
	    for (uInt k=0; k < amask.nelements(); ++k){
	      amask[k]=amodel[k]+String(".mask");
	    }
	  }
	  Vector<Bool> nointerac(nmods);
	  nointerac.set(false);
	  if(fixed.nelements() != nmods){
	    fixed.resize(nmods);
	    fixed.set(false);
	  }
	  Bool forceReload=true;
	  Int nloop=0;
	  if(npercycle != 0)
	    nloop=niter/npercycle;
	  Int continter=0;
	  Int elniter=npercycle;
	  ostringstream oos;
	  threshold.print(oos);
	  String thresh=String(oos);
	  if(String(masktemplate) != String("")){
	    continter=interactivemask(masktemplate, amask[0], 
						 elniter, nloop, thresh);
	  }
	  else {
	    // do a zero component clean to get started
	    rstat=clean(String(algorithm), 0, gain, 
		  threshold, displayprogress,
		  amodel, fixed, String(complist), amask,  
		  aimage, aresidual, Vector<String>(0), false);
	    
	    for (uInt nIm=0; nIm < nmods; nIm++){ //=ntaylor_p){
	      if(Table::isReadable(aimage[nIm]) && Table::isWritable(aresidual[nIm]) ){
		PagedImage<Float> rest(aimage[nIm]);
		PagedImage<Float> resi(aresidual[nIm]);
		copyMask(resi, rest, "mask0");
	      }
	      forceReload=forceReload || (aresidual.nelements() >1);
	      continter=interactivemask(aresidual[nIm], amask[nIm], 
						   elniter, nloop,thresh, forceReload);
	      forceReload=false;
	      if(continter>=1)
	        nointerac(nIm)=true;
	      if(continter==2)
	        fixed(nIm)=true;
	      
	    }
	    if(allEQ(nointerac, true)){
	      elniter=niter;
	      //make it do one more loop/clean but with all niter 
	      nloop=1;
	    }
	  }
	  for (Int k=0; k < nloop; ++k){
	    
	    casacore::Quantity thrsh;
	    if(!casacore::Quantity::read(thrsh, thresh)){
	      os << LogIO::WARN << "Error interpreting threshold" 
		      << LogIO::POST;
	      thrsh=casacore::Quantity(0, "Jy");
	      thresh="0.0Jy";
	    }
	    Vector<String> elpsf(0);
	    //Need to save psfs in interactive only once and lets do it the 
	    //first time
	    if(k==0)
	      elpsf=apsf;
	    if(anyEQ(fixed, false)){
	      rstat = clean(String(algorithm), elniter, gain, 
			    thrsh, 
			    displayprogress,
			    amodel, fixed, String(complist), 
			    amask,  
			    aimage, aresidual, elpsf, k == 0);
	      //if clean converged... equivalent to stop
	      if(rstat.asBool("converged")){
		continter=2;
		fixed.set(true);
	      }
	      if(anyEQ(fixed, false) && anyEQ(nointerac,false)){
		Int remainloop=nloop-k-1;
		for (uInt nIm=0; nIm < nmods; nIm++){ //=ntaylor_p){
		  if(!nointerac(nIm)){
		    continter=interactivemask(aresidual[nIm], amask[nIm],
					      
					      elniter, remainloop, 
					      thresh, (aresidual.nelements() >1));
		    if(continter>=1)
		      nointerac(nIm)=true;
		    if(continter==2)
		      fixed(nIm)=true;
		  }
		}
		k=nloop-remainloop-1;
		if(allEQ(nointerac,true)){
		  elniter=niter-(k+1)*npercycle;
		  //make it do one more loop/clean but with remaining niter 
		  k=nloop-2;
		}
	      } 
	    }
	  }
	  //Unset the mask in the residual 
	  // Cause as requested in CAS-1768...
	  for (uInt nIm=0; nIm < aresidual.nelements(); ++nIm){
	    if(Table::isWritable(aresidual[nIm]) ){
	      PagedImage<Float> resi(aresidual[nIm]);
	      if(resi.hasRegion("mask0", RegionHandler::Masks)){
		resi.setDefaultMask("");
	      }
	    }
	  }
	  
	}

	os << "Restoring Image(s) with the clean-beam" << LogIO::POST;
	restoreImages(aimage, (niter>=0) );
	this->writeHistory(os);
	try{
	  // write data processing history into image logtable
	  LoggerHolder imagelog (false);
	  LogSink& sink = imagelog.sink();
	  LogOrigin lor( String("imager"), String("clean()") );
	  LogMessage msg(lor);
	  sink.postLocally(msg);
	  MSHistoryColumns msHis(ms_p->history());
	  transferHistory(imagelog, msHis);
	  for (Int thismodel=0;thismodel<Int(aimage.nelements());++thismodel) {
	    if(Table::isWritable(aimage(thismodel))){
	      PagedImage<Float> restoredImage(aimage(thismodel),
					      TableLock(TableLock::AutoNoReadLocking));
	      LoggerHolder& log = restoredImage.logger();
	      log.append(imagelog);
	      log.flush();
	      restoredImage.table().relinquishAutoLocks(true);
	    }
	  }
	}
	catch(exception& x){
      
	  this->unlock();
	  destroySkyEquation();
	  os << LogIO::WARN << "Caught exception: " << x.what()
	     << LogIO::POST;
	  os << LogIO::SEVERE << "This means your MS/HISTORY table may be corrupted;  you may consider deleting all the rows from this table"
	     <<LogIO::POST; 
	  //continue and wrap up this function
	  
	}
	catch(...){
	  this->unlock();
	  destroySkyEquation();
	  os << LogIO::WARN << "Caught unknown exception" <<  LogIO::POST;
	  os << LogIO::SEVERE << "The MS/HISTORY table may be corrupted;  you may consider deleting all the rows from this table"
	     <<LogIO::POST;
	  
	}


       } //catch  (AipsError x) {
       //os << LogIO::SEVERE << "Exception Reported: " << x.getMesg() << LogIO::POST;
       //	RETHROW(x);
       //  }
    } else {
      os << LogIO::SEVERE << "No MeasurementSet has been assigned, please run open." << LogIO::POST;
    }
    return rstat;
  }

Bool Imager::adviseChanSelex(Double& freqStart, Double& freqEnd, 
		       const Double& freqStep,  const MFrequency::Types& freqframe,
		       Vector< Vector<Int> >& spw, Vector< Vector<Int> >& start,
			     Vector< Vector<Int> >& nchan, const String& ms, const Int field_id, const Bool getFreqRange, const String spwselection){

  LogIO os(LogOrigin("imager", "adviseChanSelex"));
  if(ms==String("")){
    if(numMS_p < 1 || !rvi_p){
      os << LogIO::SEVERE << "Data selection incomplete" 
	 << LogIO::POST;
      return false;
    }
  }
  spw.resize();
  start.resize();
  nchan.resize();
  try {
    if(!getFreqRange){
      Block<Vector<Int> > bnchan;
      Block<Vector<Int> > bstart;
      Block<Vector<Int> > bspw;
      Double fS, fE;
      fS=freqStart;
      fE=freqEnd;
      if(freqEnd < freqStart){
	fS=freqEnd;
	fE=freqStart;
      }
    
      if(ms==String("")){
	rvi_p->getSpwInFreqRange(bspw, bstart, bnchan, fS, fE, fabs(freqStep), freqframe);
      }
      else{
	bnchan.resize(1);
	bstart.resize(1);
	bspw.resize(1);
	MeasurementSet elms(String(ms), TableLock(TableLock::AutoNoReadLocking), Table::Old);
	MSUtil::getSpwInFreqRange(bspw[0], bstart[0], bnchan[0], elms, fS, fE, fabs(freqStep), freqframe, field_id);
	elms.relinquishAutoLocks(true);

      }
      spw=Vector<Vector<Int> >(bspw.begin( ), bspw.end( ));
      start=Vector<Vector<Int> >(bstart.begin( ), bstart.end( ));
      nchan=Vector<Vector<Int> >(bnchan.begin( ), bnchan.end( ));
    }
    else{
      if(ms==String("")){
	rvi_p->getFreqInSpwRange(freqStart, freqEnd, freqframe);
      }
      else{
	MeasurementSet elms(ms, TableLock(TableLock::AutoNoReadLocking), Table::Old);
	MSSelection thisSelection;
	String spsel=spwselection;
	if(spsel=="")spsel="*";
	thisSelection.setSpwExpr(spsel);
	TableExprNode exprNode=thisSelection.toTableExprNode(&elms);
	Matrix<Int> chanlist=thisSelection.getChanList();
	if(chanlist.ncolumn() <3){
	  freqStart=-1.0;
	  freqEnd=-1.0;
	  return false;
	}
	Vector<Int> elspw=chanlist.column(0);
	Vector<Int> elstart=chanlist.column(1);
	Vector<Int> elnchan=Vector<Int> (chanlist.column(2)-elstart)+1;
	MSUtil::getFreqRangeInSpw(freqStart, freqEnd, elspw, elstart, elnchan, elms, freqframe, field_id);
      }

    }



        
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::POST;
    return false;
  } 
  catch (...){
    os << LogIO::SEVERE << "Unknown  exception handled" 
       << LogIO::POST;
    return false;

  }

  return true;

}



} //# NAMESPACE CASA - END

