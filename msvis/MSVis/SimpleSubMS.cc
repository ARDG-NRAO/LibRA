//# SimpleSubMS.cc 
//# Copyright (C) 2010
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify
//# it under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or
//# (at your option) any later version.
//#
//# This library is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//# 
//# You should have received a copy of the GNU General Public License
//# along with this library; if not, write to the Free Software
//# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: $
#include <casa/System/AppInfo.h>
#include <casa/Utilities/GenSort.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/SimpleSubMS.h>

using namespace casacore;
namespace casa {

  SimpleSubMS::SimpleSubMS(MeasurementSet& ms) : SubMS(ms){

  }

  SimpleSubMS::~SimpleSubMS(){

  }

  MeasurementSet* SimpleSubMS::makeMemSubMS(const MS::PredefinedColumns& whichcol, const String& name){
    LogIO os(LogOrigin("SimpleSubMS", "makeMemSubMS()"));
    
    if(max(fieldid_p) >= Int(ms_p.field().nrow())){
      os << LogIO::SEVERE 
	 << "Field selection contains elements that do not exist in "
	 << "this MS"
	 << LogIO::POST;
      ms_p=MeasurementSet();
      return 0;  
    }
    if(max(spw_p) >= Int(ms_p.spectralWindow().nrow())){
      os << LogIO::SEVERE 
         << "SpectralWindow selection contains elements that do not exist in "
         << "this MS"
         << LogIO::POST;
      ms_p=MeasurementSet();
      return 0;
    }
    
    if(!makeSelection()){
      os << LogIO::SEVERE 
         << "Failed on selection: combination of spw and/or field and/or time "
         << "chosen may be invalid."
         << LogIO::POST;
      ms_p=MeasurementSet();
      return 0;
    }
    //Make sure the damn ms is sorted the way we want ...to copy the meta-columns as is.
    Block<String> sortcol(4);
    sortcol[0]=MS::columnName(MS::ARRAY_ID);
    sortcol[1]=MS::columnName(MS::FIELD_ID);
    sortcol[2]=MS::columnName(MS::DATA_DESC_ID);
    sortcol[3]=MS::columnName(MS::TIME);
    MeasurementSet msselsort(mssel_p.sort(sortcol, Sort::Ascending, Sort::QuickSort));

    mscIn_p=new MSColumns(msselsort);
    String msname=name;
    if(msname==""){
      msname=AppInfo::workFileName(100, "TempSubMS");
    }
    MeasurementSet* ah=setupMS(msname, nchan_p[0], ncorr_p[0],  
			       mscIn_p->observation().telescopeName()(0),
			       Vector<MS::PredefinedColumns>(1,whichcol));
    if(!ah)
      return 0;
    
    MeasurementSet * outpointer;
    if(name==""){
      ah->markForDelete();
      outpointer= new MeasurementSet(ah->copyToMemoryTable("TmpMemoryMS"));
      delete ah;
    }
    else{
      outpointer=ah;
    }
    if(!outpointer)
      return 0;
    outpointer->initRefs();
    
    msOut_p = *outpointer;
    msc_p = new MSColumns(msOut_p);
    
    //Do the subtables
    msc_p->setEpochRef(MEpoch::castType(mscIn_p->timeMeas().getMeasRef().getType()));
    
    // UVW is the only other Measures column in the main table.
    msc_p->uvwMeas().setDescRefCode(Muvw::castType(mscIn_p->uvwMeas().getMeasRef().getType()));
    
    // fill or update
    if(!fillDDTables()){
      return 0;
    }

    // SourceIDs need to be remapped around here.  It could not be done in
    // selectSource() because mssel_p was not setup yet.
    relabelSources();
    
    fillFieldTable();
    copySource();
    
    copyAntenna();
    if(!copyFeed())         // Feed table writing has to be after antenna 
      return 0;
    
    copyObservation();
    copyPointing();
    // copyWeather();
    // copySyscal();   // ?
    /////////////Done with the sub tables...now to the main
    fillAccessoryMainCols();
    msc_p->weight().putColumn(mscIn_p->weight());
    msc_p->sigma().putColumn(mscIn_p->sigma());
    Block<Int> sort(4);
    sort[0]=MS::ARRAY_ID;
    sort[1]=MS::FIELD_ID;
    sort[2]=MS::DATA_DESC_ID;
    sort[3]=MS::TIME;
    ROVisibilityIterator vi(msselsort, sort);
    for (Int k=0; k < spw_p.shape()(0) ; ++k){ 
       os << LogIO::NORMAL
	  << "Selecting "<< nchan_p[k] << " channels, starting at " <<chanStart_p[k] << " for spw " << spw_p[k] << LogIO::POST; ;
       
       vi.selectChannel(1, chanStart_p[k], nchan_p[k],
			chanStep_p[k], spw_p[k]);
    }
    /////slurp test
    //vi.slurp();
    ///
    Bool doSpWeight = !(mscIn_p->weightSpectrum().isNull()) &&
                      mscIn_p->weightSpectrum().isDefined(0);
    Int rowsdone=0;
    Int rowsnow=0;
    VisBuffer vb(vi);
    Vector<Int> spwindex(max(spw_p)+1);
    spwindex.set(-1);
    for(uInt k = 0; k < spw_p.nelements(); ++k)
      spwindex[spw_p[k]] = k;
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin(); vi.more(); vi++) {
	Int spw=spwindex[vb.spectralWindow()];
	if(spw <0){
	  cerr << "Huh ?: The programmer calling this code is useless.." << endl;
	  return 0;
	}
	rowsnow=vb.nRow();	
	RefRows rowstoadd(rowsdone, rowsdone+rowsnow-1);
	if(whichcol==MS::DATA)
	  msc_p->data().putColumnCells(rowstoadd, vb.visCube());
	else if(whichcol==MS::MODEL_DATA)
	  msc_p->data().putColumnCells(rowstoadd, vb.modelVisCube());
	else if(whichcol==MS::CORRECTED_DATA)
	  msc_p->data().putColumnCells(rowstoadd, vb.correctedVisCube());
	else
	  cerr << "Column not requested not yet supported to be loaded to memory" << endl;
	msc_p->flag().putColumnCells(rowstoadd, vb.flagCube());
	if(doSpWeight)
	  msc_p->weightSpectrum().putColumnCells(rowstoadd, vb.weightSpectrum());
	rowsdone += rowsnow;
      }
    }
    return outpointer;
  }


} //#End casa namespace
