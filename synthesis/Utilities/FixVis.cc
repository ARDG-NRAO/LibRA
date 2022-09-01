#include <synthesis/Utilities/FixVis.h>
#include <msvis/MSVis/MSUVWGenerator.h>
#include <msvis/MSVis/SubMS.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/measures/Measures/UVWMachine.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/ObsInfo.h>
#include <casacore/images/Images/PagedImage.h>           // Used to pass coords
#include <casacore/images/Images/ImageInfo.h>            // to FTMachine.
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MSDopplerUtil.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/ms/MSSel/MSSelectionTools.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/BasicSL/String.h>	// for parseColumnNames()

#include <iostream>

using namespace casacore;
namespace casa {

FixVis::FixVis(MeasurementSet& ms, const String& dataColName) :
  FTMachine(),
  ms_p(ms),
  msc_p(NULL),
  nsel_p(0),
  nAllFields_p(1),
  npix_p(2),
  cimageShape_p(4, npix_p, npix_p, 1, 1), // Can we get away with
  tileShape_p(4, npix_p, npix_p, 1, 1),   // (1, 1, 1, 1)?  Does it matter?
  tiledShape_p(cimageShape_p, tileShape_p),
  antennaSel_p(false),
  freqFrameValid_p(false)
  //  obsString_p("")
{
  logSink() << LogOrigin("FixVis", "") << LogIO::NORMAL3;

  antennaId_p.resize();
  antennaSelStr_p.resize();
  distances_p.resize();
  dataCols_p = SubMS::parseColumnNames(dataColName, ms);
  nDataCols_p = dataCols_p.nelements();

  nchan_p = 1; // imageNchan_p;

  spectralwindowids_p.resize(ms_p.spectralWindow().nrow());
  indgen(spectralwindowids_p);

  lockCounter_p = 0;
}
  
// Destructor
FixVis::~FixVis()
{
  if(!ms_p.isNull())
    ms_p.flush();

  delete msc_p;
}

// Interpret field indices (MSSelection)
Vector<Int> FixVis::getFieldIdx(const String& fields)
{
  MSSelection mssel;

  mssel.setFieldExpr(fields);
  return mssel.getFieldList(&ms_p);
}

uInt FixVis::setFields(const Vector<Int>& fieldIds)
{
  logSink() << LogOrigin("FixVis", "setFields");
  logSink() << LogIO::NORMAL << "Selecting fields ";

  nsel_p = fieldIds.nelements();
  nAllFields_p = ms_p.field().nrow();
  FieldIds_p.resize(nAllFields_p);

  for(Int i = 0; i < static_cast<Int>(nAllFields_p); ++i){
    FieldIds_p(i) = -1;
    for(uInt j = 0; j < nsel_p; ++j){
      if(fieldIds[j] == i){
	FieldIds_p(i) = i;
	logSink() << i << " " << LogIO::NORMAL;
	break;
      }
    }
  }
  logSink() << LogIO::POST;

  return nsel_p;
}

void FixVis::setPhaseDirs(const Vector<MDirection>& phaseDirs)
{
  phaseDirs_p = phaseDirs;

  // Do a consistency check between fieldIds and FieldIds_p.
  logSink() << LogOrigin("FixVis", "setPhaseDirs");
  uInt n2change = phaseDirs.nelements();
  if(n2change != nsel_p){
    logSink() << LogIO::SEVERE
              << "Mismatch between the number of new directions and the fields to change"
              << LogIO::POST;
  }
}


void FixVis::convertFieldDirs(const MDirection::Types outType)
{
  logSink() << LogOrigin("FixVis", "convertFieldDirs");

  // Note that the each direction column in the FIELD table only allows one
  // reference frame for the entire column, but polynomials can be assigned on
  // a row-wise basis for objects moving in that frame.

  Muvw::Types uvwtype;
  Muvw::getType(uvwtype, MDirection::showType(outType));

  ready_msc_p();
  
  msc_p->uvw().rwKeywordSet().asrwRecord("MEASINFO").define("Ref", MDirection::showType(outType));

  MSFieldColumns& msfcs = msc_p->field();

  MDirection pd0(msfcs.phaseDirMeas(0));
  logSink() << LogIO::DEBUG1
            << "PHASE_DIR[0] before = " << pd0.tellMe() << " ";
  {
    ostringstream os;
    os << *(pd0.getData()) << endl;
    logSink() << os.str() << LogIO::POST;
  }
  
  // There is no physical or known programming need to change the delay and
  // reference direction frames as well, but for aesthetic reasons we keep them
  // all in the same frame if they start in the same frame.
  //ArrayMeasColumn<MDirection> msDelayDirCol;
  //msDelayDirCol.reference(msfcs.delayDirMeasCol());
  //ArrayMeasColumn<MDirection> msReferenceDirCol;
  //msReferenceDirCol.reference(msfcs.referenceDirMeasCol());
  Bool doAll3 = (msfcs.phaseDirMeasCol().getMeasRef().getType() ==
                 msfcs.delayDirMeasCol().getMeasRef().getType() &&
                 msfcs.phaseDirMeasCol().getMeasRef().getType() ==
                 msfcs.referenceDirMeasCol().getMeasRef().getType());
  
  // Setup conversion machines.
  // Set the frame - choose the first antenna. For e.g. VLBI, we
  // will need to reset the frame per antenna
  mLocation_p = msc_p->antenna().positionMeas()(0);

  // Expect problems if a moving object is involved!
  mFrame_p = MeasFrame(msfcs.timeMeas()(0), mLocation_p);

  //MDirection::Ref startref(msfcs.phaseDirMeasCol().getMeasRef());
  // If the either of the start or destination frames refers to a finite
  // distance, then the conversion has to be done in two steps:
  // MDirection::Convert start2app(msfcs.phaseDirMeasCol()(0), MDirection::APP);
  // 
  // Otherwise the conversion can be done directly.
  Bool haveMovingFrame = (MDirection::castType(msfcs.phaseDirMeasCol().getMeasRef().getType()) > MDirection::N_Types ||
                          outType > MDirection::N_Types);

  const MDirection::Ref newFrame(haveMovingFrame ? MDirection::APP : outType,
                                 mFrame_p);

  convertFieldCols(msfcs, newFrame, doAll3);
  
  if(haveMovingFrame){
    // Since ArrayMeasCol most likely uses one frame for the whole column, do
    // the second half of the conversion with a second pass through the column
    // instead of on a row-by-row basis.
    logSink() << LogIO::WARN
              << "Switching to or from accelerating frames is not well tested."
              << LogIO::POST;

    // Using msfcs.phaseDirMeasCol()(0)[0] to initialize converter will only
    // work if msfcs.phaseDirMeasCol()'s type has been set to APP.
    const MDirection::Ref newerFrame(outType, mFrame_p);

    convertFieldCols(msfcs, newerFrame, doAll3);
  }

  pd0 = msfcs.phaseDirMeas(0);
  logSink() << LogIO::DEBUG1
            << "PHASE_DIR[0] after =  " << pd0.tellMe() << " ";
  {
    ostringstream os;
    os << *(pd0.getData()) << endl;
    logSink() << os.str() << LogIO::POST;
  }
}


void FixVis::convertFieldCols(MSFieldColumns& msfcs,
                              const MDirection::Ref& newFrame,
                              const Bool doAll3)
{
  logSink() << LogOrigin("FixVis", "convertFieldCols");
  // Unfortunately ArrayMeasColumn::doConvert() is private, which moots the
  // point of making a conversion machine here.
//   Vector<MDirection> dummyV;
//   dummyV.assign(pdc(0));
//   MDirection::Convert *converter = new MDirection::Convert(dummyV[0],
//                                                            newFrame);
//   if(!converter)
//     logSink() << "Cannot make direction conversion machine"
//               << LogIO::SEVERE;

  uInt nrows = msfcs.nrow();

  // Convert each phase tracking center.  This will make them numerically
  // correct in the new frame, but the column will still be labelled with the
  // old frame.
  
  uInt nOrders;
  Vector<MDirection> mdarr;              // direction for each order
  Array<Double>     darr;               // longitude and latitude for each order
  Vector<Double> dirV;
  for(uInt i = 0; i < nrows; ++i){
    nOrders = msfcs.numPoly()(i) + 1;
    logSink() << LogIO::DEBUG1 << "numPoly(" << i << ") = " << nOrders - 1
              << LogIO::POST;

    //pdc.put(i, pdc.doConvert(i, *converter));
    mdarr = msfcs.phaseDirMeasCol().convert(i, newFrame);
    darr.resize(IPosition(2, 2, nOrders));
    for(uInt orderNumber = 0; orderNumber < nOrders; ++orderNumber){
      dirV = mdarr[orderNumber].getAngle().getValue();
      darr(IPosition(2, 0, orderNumber)) = dirV[0];
      darr(IPosition(2, 1, orderNumber)) = dirV[1];
    }
    msfcs.phaseDir().put(i, darr);
    
    //msfcs.phaseDirMeasCol().put(i, mdarr);
    
    if(doAll3){
      //ddc.put(i, ddc.doConvert(i, *converter));
      mdarr = msfcs.delayDirMeasCol().convert(i, newFrame);
      for(uInt orderNumber = 0; orderNumber < nOrders; ++orderNumber){
        dirV = mdarr[orderNumber].getAngle().getValue();
        darr(IPosition(2, 0, orderNumber)) = dirV[0];
        darr(IPosition(2, 1, orderNumber)) = dirV[1];
      }
      msfcs.delayDir().put(i, darr);
      //rdc.put(i, rdc.doConvert(i, *converter));
      //msfcs.referenceDirMeasCol().put(i,
      //         msfcs.referenceDirMeasCol().convert(i, newFrame));
      mdarr = msfcs.referenceDirMeasCol().convert(i, newFrame);
      for(uInt orderNumber = 0; orderNumber < nOrders; ++orderNumber){
        dirV = mdarr(IPosition(1, orderNumber)).getAngle().getValue();
        darr(IPosition(2, 0, orderNumber)) = dirV[0];
        darr(IPosition(2, 1, orderNumber)) = dirV[1];
      }
      msfcs.referenceDir().put(i, darr);      
    }
  }
  
  // Update the reference frame label(s).
  msfcs.phaseDirMeasCol().setDescRefCode(newFrame.getType(), false);
  if(doAll3){
    msfcs.delayDirMeasCol().setDescRefCode(newFrame.getType(), false);
    msfcs.referenceDirMeasCol().setDescRefCode(newFrame.getType(), false);
  }

  //delete converter;
}


void FixVis::setDistances(const Vector<Double>& distances)
{
  logSink() << LogOrigin("FixVis", "setDistances");
  if(distances.nelements() != nsel_p)
    logSink() << LogIO::SEVERE
              << "Mismatch between the # of specified distances and selected fields."
              << LogIO::POST;
  distances_p = distances;
}

// Calculate the (u, v, w)s and store them in ms_p.
Bool FixVis::calc_uvw(const String& refcode, const Bool reuse)
{
  Bool retval = false;
  
  logSink() << LogOrigin("FixVis", "calc_uvw");

  if(!ready_msc_p())
    return false;
  
  if(nsel_p > 0){

    // Get the PHASE_DIR reference frame type for the input ms.
    MSFieldColumns& msfcs(msc_p->field());
    MDirection startDir = msfcs.phaseDirMeas(0);
    MDirection::Types startDirType = MDirection::castType(msfcs.phaseDirMeasCol().getMeasRef().getType());
    MDirection::Types wantedDirType;
    MDirection::getType(wantedDirType, refcode);

    if(startDirType != wantedDirType){
      if(nsel_p < nAllFields_p){
        logSink() << LogIO::SEVERE
                  << "The reference frame must either be changed for all fields or not at all."
                  << LogIO::POST;
        return false;
      }
      else
        convertFieldDirs(wantedDirType);
    }
    else if(reuse){
      logSink() << LogIO::NORMAL
                << "The UVWs are already in the desired frame - leaving them as is."
                << LogIO::POST;
      return true;
    }
    
    try{
      if(reuse){
        const MDirection::Ref outref(wantedDirType);
        
        rotateUVW(startDir, outref);
      }
      else{
        Muvw::Types uvwtype;
        MBaseline::Types bltype;
    
        try{
          MBaseline::getType(bltype, refcode);
          Muvw::getType(uvwtype, refcode);
        }
        catch(AipsError x){
          logSink() << LogIO::SEVERE
                    << "refcode \"" << refcode << "\" is not valid for baselines."
                    << LogIO::POST;
          return false;
        }
             
        MSUVWGenerator uvwgen(*msc_p, bltype, uvwtype);
        retval = uvwgen.make_uvws(FieldIds_p);
      }
      
    }
    catch(AipsError x){
      logSink() << LogIO::SEVERE << "Error " << x.getMesg() << LogIO::POST;
    }

  }
  else{
    logSink() << LogIO::SEVERE
	      << "There is a problem with the selected field IDs and phase tracking centers."
	      << LogIO::POST;
  }
  return retval;
}

// Convert the UVW column to a new reference frame by rotating the old
// baselines instead of calculating fresh ones.
//
// oldref must be supplied instead of extracted from msc_p->uvw(), because
// the latter might be wrong (use the field direction).
void FixVis::rotateUVW(const MDirection &indir, const MDirection::Ref& newref)
{
  ArrayColumn<Double>& UVWcol = msc_p->uvw();

  // Setup a machine for converting a UVW vector from the old frame to
  // uvwtype's frame
  UVWMachine uvm(newref, indir);
  RotMatrix rm(uvm.rotationUVW());

  uInt nRows = UVWcol.nrow();
  for(uInt row = 0; row < nRows; ++row){
    UVWcol.put(row, (rm * MVuvw(UVWcol(row))).getVector());
  }
  return;
}

// Don't just calculate the (u, v, w)s, do everything and store them in ms_p.
Bool FixVis::fixvis(const String& refcode)
{
  logSink() << LogOrigin("FixVis", "fixvis");

  Bool retval = false;

  if(!ready_msc_p())
    return false;

  if(nsel_p > 0){
    if(phaseDirs_p.nelements() == static_cast<uInt>(nsel_p)){ 

      String telescop = msc_p->observation().telescopeName()(0);
      MPosition obsPosition;
      if(!(MeasTable::Observatory(obsPosition, telescop))){
	logSink() << LogIO::WARN << "Did not get the position of " << telescop 
		  << " from data repository" << LogIO::POST;
	logSink() << LogIO::WARN << "Please contact CASA to add it to the repository."
		  << LogIO::POST;
	logSink() << LogIO::WARN << "Frequency conversion will not work " << LogIO::POST;
	freqFrameValid_p = false;
      }
      else{
	mLocation_p = obsPosition;
	freqFrameValid_p = true;
      }

      MSFieldColumns& msfcs = msc_p->field();

      mFrame_p = MeasFrame(msfcs.timeMeas()(0), mLocation_p);

      msc_p->uvw().rwKeywordSet().asrwRecord("MEASINFO").define("Ref", refcode);

      //**** Adjust the phase tracking centers and distances. ****
      // Loop through each selected field.
      Int selectedField;
      Int selFldCounter=0;
      for(uInt fldCounter = 0; fldCounter < nAllFields_p; ++fldCounter){
        selectedField = FieldIds_p[fldCounter];
	if(selectedField<0){
	  continue;
	}
        setImageField(selectedField);
        if(makeSelection(selectedField)){
	  logSink() << LogIO::NORMAL << "Processing field " << selectedField << LogIO::POST;
          processSelected(selFldCounter);
	  selFldCounter++;

	  // Update FIELD (and/or optional tables SOURCE, OBSERVATION, but not
	  // POINTING?) to new PTC.

          retval = true;
        }
        else{
          logSink() << LogIO::SEVERE
                    << "Field " << selectedField << " could not be selected for phase tracking center or"
                    << " distance adjustment."
                    << LogIO::POST;
        }
      }
    }
    else if(phaseDirs_p.nelements() > 0){
      logSink() << LogIO::SEVERE
		<< "There is a problem with the selected field IDs and phase tracking centers.\n"
		<< "No adjustments of phase tracking centers or distances will be done."
		<< LogIO::POST;
    }
  }
  else{
    logSink() << LogIO::SEVERE << "No fields are selected." << LogIO::POST;
  }  
  return retval;
}


Bool FixVis::setImageField(const Int fieldid,
                           const Bool dotrackDir //, const MDirection& trackDir
                           )
{
  logSink() << LogOrigin("FixVis", "setImageField()");

  try{

    doTrackSource_p = dotrackDir;

    fieldid_p = fieldid;

    return true;
  }
  catch(AipsError x){
    this->unlock();
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::POST;
    return false;
  } 
  return true;
}

Bool FixVis::lock()
{
  Bool ok = true;

  if(lockCounter_p == 0)
    ok = ms_p.lock();
  ++lockCounter_p;

  return ok;
}

void FixVis::unlock()
{
  if(lockCounter_p == 1)
    ms_p.unlock();

  if(lockCounter_p > 0)
    --lockCounter_p;
}


Bool FixVis::makeSelection(const Int selectedField)
{
  logSink() << LogOrigin("FixVis", "makeSelection()");
    
  //Vis/MSIter will check if SORTED_TABLE exists and resort if necessary.
  MSSelection thisSelection;
  if(selectedField >= 0 && nAllFields_p > 1){
    Vector<Int> wrapper;
    wrapper.resize(1);
    wrapper[0] = selectedField;
    thisSelection.setFieldExpr(MSSelection::indexExprStr(wrapper));
  }
  else if(antennaSel_p){
    if(antennaId_p.nelements() > 0)
      thisSelection.setAntennaExpr(MSSelection::indexExprStr(antennaId_p));
    if(antennaSelStr_p[0] != "")
      thisSelection.setAntennaExpr(MSSelection::nameExprStr(antennaSelStr_p));
  }
  //  if(obsString_p != "")
  //  thisSelection.setObservationExpr(obsString_p);
    
  TableExprNode exprNode = thisSelection.toTableExprNode(&ms_p);    
    
  // Now remake the selected ms
  if(!(exprNode.isNull())){
    mssel_p = MeasurementSet(ms_p(exprNode));
  }
  else if(selectedField < 0 || nsel_p == nAllFields_p){
    // Null take all the ms ...setdata() blank means that
    mssel_p = MeasurementSet(ms_p);
  }
  else{
    logSink() << LogIO::SEVERE
              << "Error selecting field " << selectedField << ".  "
              << "Are you trying to adjust antenna positions at the same time?"
              << LogIO::POST;
    return false;
  }

  //mssel_p.rename(ms_p.tableName()+"/SELECTED_TABLE", Table::Scratch);
  if(mssel_p.nrow() == 0)
    return false;

  if(mssel_p.nrow() < ms_p.nrow())
    logSink() << LogIO::NORMAL
              << mssel_p.nrow() << " rows selected out of " << ms_p.nrow() << "." 
              << LogIO::POST;

  delete msc_p;
  msc_p = NULL;
  return ready_msc_p();
}

Bool FixVis::ready_msc_p()
{
  Bool retval = false;

  if(!msc_p){
    try{
      msc_p = new MSColumns(mssel_p.isNull() ? ms_p : mssel_p);
      retval = true; // Assume msc_p is OK.
    }
    catch(AipsError& e){
      logSink() << LogIO::SEVERE
                << "Error getting the columns from the MS."
                << LogIO::POST;
    }
    catch(std::bad_alloc& e){
      logSink() << LogIO::SEVERE
                << "Error allocating memory for the MS columns."
                << LogIO::POST;
    }
    // Just let any other exceptions, of the unexpected kind,
    // propagate up where they can be seen.
  }
  else
    retval = true; // Assume msc_p is OK.

  return retval;
}


void FixVis::processSelected(uInt numInSel)
{
  logSink() << LogOrigin("FixVis", "processSelected()");

  mImage_p = phaseDirs_p[numInSel];

  ArrayColumn<Double>& UVWcol = msc_p->uvw();

  Block<Int> sort(0);
  sort.resize(4);
  sort[0] = MS::ARRAY_ID; 		    // use default sort order
  sort[1] = MS::FIELD_ID;
  sort[2] = MS::DATA_DESC_ID;
  sort[3] = MS::TIME;

  VisibilityIterator vi(mssel_p, sort);	
	  
  // Loop over all visibilities in the selected field.
  VisBuffer vb(vi);
  
  vi.origin();

  for(vi.originChunks(); vi.moreChunks(); vi.nextChunk()){
    for(vi.origin(); vi.more(); ++vi){

      uInt numRows = vb.nRow();

      Matrix<Double> uvw(3, numRows);
      uvw=0.0;
      Vector<Double> dphase(numRows);
      dphase=0.0;

      for(uInt i = 0; i < numRows; ++i){
 	for (Int idim=0;idim<3;idim++){
 	  uvw(idim,i)=vb.uvw()(i)(idim);
 	}
      }

      // the following call requires the member variables
      //   lastFieldId_p
      //   lastMSId_p
      //   tangentSpecified_p
      //   MeasFrame mFrame_p == output ref frame for the UVW coordinates
      //   MDirection mImage_p == output phase center
      //      (input phase center is taken from the visbuffer, i.e. from the FIELD table)
      
      FTMachine::rotateUVW(uvw, dphase, vb);

      // Immediately returns if not needed.
      refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);

      // update vis buffer cache
      for(uInt datacol = 0; datacol < nDataCols_p; datacol++){
	if(dataCols_p[datacol] == MS::DATA){
	  vb.visCube(); // return value not needed
	}
        else if(dataCols_p[datacol] == MS::CORRECTED_DATA){
	  vb.correctedVisCube();
	}
        else if(dataCols_p[datacol] == MS::MODEL_DATA){
	  vb.modelVisCube();
	}
      }

      // apply phase center shift to vis buffer
      vb.phaseCenterShift(dphase);

      // write changed UVWs
      auto origRows = vb.rowIds();
      for(uInt row = 0; row < numRows; row++){
	UVWcol.put(origRows(row), uvw.column(row));
      }

      // write changed visibilities
      for(uInt datacol = 0; datacol < nDataCols_p; datacol++){
	if(dataCols_p[datacol] == MS::DATA){
	  vi.setVis(vb.visCube(),VisibilityIterator::Observed);
	}
        else if(dataCols_p[datacol] == MS::CORRECTED_DATA){
	  vi.setVis(vb.correctedVisCube(),VisibilityIterator::Corrected);
	}
        else if(dataCols_p[datacol] == MS::MODEL_DATA){
	  vi.setVis(vb.modelVisCube(),VisibilityIterator::Model);
	}
      }


    }
    
  }

}
  

void FixVis::ok() {
  //  AlwaysAssert(image, AipsError);
}

void FixVis::init()
{
  logSink() << LogOrigin("FixVis", "init")  << LogIO::NORMAL;

  //ok();

  //npol  = image->shape()(2);
  //nchan = image->shape()(3);
}

// Initialize FTMachine.
void FixVis::initializeToVis(ImageInterface<Complex>& iimage,
			     const VisBuffer&)
{
  image = &iimage;

  ok();
  init();

  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  //RR initMaps(vb);
}

void FixVis::finalizeToVis()
{
}

// Initialize the FFT to the Sky. Here we have to setup and initialize the
// grid. 
void FixVis::initializeToSky(ImageInterface<Complex>& iimage,
			     Matrix<Float>&, //weight,
                             const VisBuffer&)
{
  // image always points to the image
  image = &iimage;

  init();

  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  //RR initMaps(vb);
}

}  // End of casa namespace.

