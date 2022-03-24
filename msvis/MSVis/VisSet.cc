//# VisSet.cc: Implementation of VisSet
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2005
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
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

#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisModelDataI.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayUtil.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/DataMan/TiledDataStMan.h>
#include <casacore/tables/DataMan/TiledShapeStMan.h>
#include <casacore/tables/DataMan/StandardStMan.h>
#include <casacore/tables/DataMan/TiledDataStManAccessor.h>
#include <casacore/tables/Tables/TableIter.h>
#include <casacore/tables/DataMan/CompressComplex.h>
#include <casacore/tables/DataMan/CompressFloat.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <casa/iostream.h>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  VisSet::VisSet(MeasurementSet& ms,const Block<Int>& columns, 
		 const Matrix<Int>& chanSelection, Double timeInterval,
		 Bool compress, Bool doModelData)
    :ms_p(ms)
  {
    LogSink logSink;
    LogMessage message(LogOrigin("VisSet","VisSet"));
    
    blockOfMS_p= new Block<MeasurementSet> ();
    blockOfMS_p->resize(1);
    (*blockOfMS_p)[0]=ms_p;
    multims_p=false;
    // sort out the channel selection
    Int nSpw=ms_p.spectralWindow().nrow();
    MSSpWindowColumns msSpW(ms_p.spectralWindow());
    if(nSpw==0)
      throw(AipsError(String("There is no valid spectral windows in "+ms_p.tableName())));
    selection_p.resize(2,nSpw);
    // fill in default selection
    selection_p.row(0)=0; //start
    selection_p.row(1)=msSpW.numChan().getColumn(); 
    for (uInt i=0; i<chanSelection.ncolumn(); i++) {
      Int spw=chanSelection(2,i);
      if (spw>=0 && spw<nSpw && chanSelection(0,i)>=0 && 
	  chanSelection(0,i)+chanSelection(1,i)<=selection_p(1,spw)) {
	// looks like a valid selection, implement it
	selection_p(0,spw)=chanSelection(0,i);
	selection_p(1,spw)=chanSelection(1,i);
      }
    }

    Bool init=true;
    if (ms.tableDesc().isColumn("CORRECTED_DATA")) {
      init=false;
    }
    // in case model data exists and the user do not want it anymore
    if(ms.tableDesc().isColumn("MODEL_DATA") && !doModelData){
      init=true;
    }
    // Add scratch columns
    if (init) {
      
      VisSetUtil::removeCalSet(ms);
      addCalSet2(ms, compress, doModelData);
      
      
      // Force re-sort (in VisIter ctor below) by deleting current sort info 
      if (ms.keywordSet().isDefined("SORT_COLUMNS")) 
	ms.rwKeywordSet().removeField("SORT_COLUMNS");
      if (ms.keywordSet().isDefined("SORTED_TABLE")) 
	ms.rwKeywordSet().removeField("SORTED_TABLE");
    }
    
    
    iter_p=new VisIter(ms_p,columns,timeInterval);
    for (uInt spw=0; spw<selection_p.ncolumn(); spw++) {
      iter_p->selectChannel(1,selection_p(0,spw),selection_p(1,spw),0,spw);
    }

    // Now initialize the scratch columns
    if (init)
      initCalSet(0);

  }

  VisSet::VisSet(MeasurementSet& ms,const Block<Int>& columns, 
		 const Matrix<Int>& chanSelection,
		 Bool addScratch,
		 Double timeInterval, Bool compress, Bool doModelData)
    :ms_p(ms)
  {
    LogSink logSink;
    LogMessage message(LogOrigin("VisSet","VisSet"));
    
    blockOfMS_p= new Block<MeasurementSet> ();
    blockOfMS_p->resize(1);
    (*blockOfMS_p)[0]=ms_p;
    multims_p=false;
    // sort out the channel selection
    Int nSpw=ms_p.spectralWindow().nrow();
    MSSpWindowColumns msSpW(ms_p.spectralWindow());
    if(nSpw==0)
      throw(AipsError(String("There is no valid spectral windows in "+ms_p.tableName())));
    selection_p.resize(2,nSpw);
    // fill in default selection
    selection_p.row(0)=0; //start
    selection_p.row(1)=msSpW.numChan().getColumn(); 
    for (uInt i=0; i<chanSelection.ncolumn(); i++) {
      Int spw=chanSelection(2,i);
      if (spw>=0 && spw<nSpw && chanSelection(0,i)>=0 && 
	  chanSelection(0,i)+chanSelection(1,i)<=selection_p(1,spw)) {
	// looks like a valid selection, implement it
	selection_p(0,spw)=chanSelection(0,i);
	selection_p(1,spw)=chanSelection(1,i);
      }
    }

    Bool init=true;

    if (ms.tableDesc().isColumn("CORRECTED_DATA")) {
      init=false;
    }
    // in case model data exists and the user do not want it anymore
    if(ms.tableDesc().isColumn("MODEL_DATA") && !doModelData){
      init=true;
    }
    //    cout << boolalpha << "addScratch = " << addScratch << endl;

    // Add scratch columns
    if (addScratch && init) {

      VisSetUtil::removeCalSet(ms);
      addCalSet2(ms, compress, doModelData);
      
      
      // Force re-sort (in VisIter ctor below) by deleting current sort info 
      if (ms.keywordSet().isDefined("SORT_COLUMNS")) 
	ms.rwKeywordSet().removeField("SORT_COLUMNS");
      if (ms.keywordSet().isDefined("SORTED_TABLE")) 
	ms.rwKeywordSet().removeField("SORTED_TABLE");
    }
    
    // Generate the VisIter
    iter_p=new VisIter(ms_p,columns,timeInterval);
    for (uInt spw=0; spw<selection_p.ncolumn(); spw++) {
      iter_p->selectChannel(1,selection_p(0,spw),selection_p(1,spw),0,spw);
    }

    // Now initialize the scratch columns
    if (addScratch && init)
      initCalSet(0);

  }
  
  VisSet::VisSet(Block<MeasurementSet>& mss,const Block<Int>& columns, 
                 const Block< Matrix<Int> >& chanSelection, Bool addScratch,
		 Double timeInterval,
		 Bool compress, Bool addModelData)
  {
    
    multims_p=true;
    blockOfMS_p = &mss;
    Int numMS=mss.nelements();
    ms_p=mss[numMS-1];
    
    Block<Vector<Int> > blockNGroup(numMS);
    Block<Vector<Int> > blockStart(numMS);
    Block<Vector<Int> > blockWidth(numMS);
    Block<Vector<Int> > blockIncr(numMS);
    Block<Vector<Int> > blockSpw(numMS);
    
    for (Int k=0; k < numMS; ++k){
      // sort out the channel selection
      Int nSpw=mss[k].spectralWindow().nrow();
      if(nSpw==0)
      throw(AipsError(String("There is no valid spectral windows in "+mss[k].tableName())));
      blockNGroup[k]=Vector<Int> (nSpw,1);
      blockIncr[k]=Vector<Int> (nSpw,1);
      // At this stage select all spw
      blockSpw[k].resize(nSpw);
      indgen(blockSpw[k]);
      if(chanSelection[k].nelements()!=0){
	blockStart[k]=chanSelection[k].row(0);
	blockWidth[k]=chanSelection[k].row(1);
      }
      MSSpWindowColumns msSpW(mss[k].spectralWindow());
      selection_p.resize(2,nSpw);
      //Drat...need to figure this one out....
      // fill in default selection
      selection_p.row(0)=0; //start
      selection_p.row(1)=msSpW.numChan().getColumn(); 
      blockStart[k].resize(selection_p.row(0).nelements());
      blockStart[k]=selection_p.row(0);
      blockWidth[k].resize(selection_p.row(1).nelements());
      blockWidth[k]=selection_p.row(1);
      for (uInt i=0; i<chanSelection[k].ncolumn(); i++) {
	Int spw=chanSelection[k](2,i);
	if (spw>=0 && spw<nSpw && chanSelection[k](0,i)>=0 && 
	    chanSelection[k](0,i)+chanSelection[k](1,i)<=selection_p(1,spw)) {
	  // looks like a valid selection, implement it
	  selection_p(0,spw)=chanSelection[k](0,i);
	  selection_p(1,spw)=chanSelection[k](1,i);
	  blockStart[k][spw]=chanSelection[k](0,i);
	  blockWidth[k][spw]=chanSelection[k](1,i);
	}
      }
    }
    iter_p=new VisIter(mss,columns,timeInterval);
    
    iter_p->selectChannel(blockNGroup, blockStart, blockWidth, blockIncr,
			  blockSpw);

    if(addScratch){
        for (Int k=0; k < numMS ; ++k){
	  addScratchCols(mss[k], compress, addModelData);
        }
    }
        
  }


VisSet::VisSet(ROVisibilityIterator& vi){

  ///Valid for single ms iterators 
  ms_p=vi.ms();
  blockOfMS_p= new Block<MeasurementSet> ();
  blockOfMS_p->resize(1);
  (*blockOfMS_p)[0]=ms_p;
  multims_p=false;
  Block<Int> columns(0);
  Double timeInterval=0.0;
  iter_p=new VisIter(ms_p,columns,timeInterval);
  Block<Vector<Int> > ngroup;
  Block<Vector<Int> > start;
  Block<Vector<Int> > width;
  Block<Vector<Int> > incr;
  Block<Vector<Int> > spws;
  vi.getChannelSelection(ngroup,start,width,incr,spws);
  iter_p->selectChannel(ngroup,start,width,incr,spws);
  
}

VisSet::VisSet(MeasurementSet& ms, const Matrix<Int>& chanSelection, 
	       Double timeInterval)
  :ms_p(ms)
{
    LogSink logSink;
    LogMessage message(LogOrigin("VisSet","VisSet"));

    blockOfMS_p= new Block<MeasurementSet> ();
    blockOfMS_p->resize(1);
    (*blockOfMS_p)[0]=ms_p;
    multims_p=false;
    // sort out the channel selection
    Int nSpw=ms_p.spectralWindow().nrow();
    if(nSpw==0)
      throw(AipsError(String("There is no valid spectral windows in "+ms_p.tableName())));
    MSSpWindowColumns msSpW(ms_p.spectralWindow());
    selection_p.resize(2,nSpw);
    // fill in default selection
    selection_p.row(0)=0; //start
    selection_p.row(1)=msSpW.numChan().getColumn(); 
    for (uInt i=0; i<chanSelection.ncolumn(); i++) {
      Int spw=chanSelection(2,i);
      if (spw>=0 && spw<nSpw && chanSelection(0,i)>=0 && 
	  chanSelection(0,i)+chanSelection(1,i)<=selection_p(1,spw)) {
	// looks like a valid selection, implement it
	selection_p(0,spw)=chanSelection(0,i);
	selection_p(1,spw)=chanSelection(1,i);
      }
    }

    

    Block<Int> columns(0);

    iter_p=new VisIter(ms_p,columns,timeInterval);
    for (uInt spw=0; spw<selection_p.ncolumn(); spw++) {
      iter_p->selectChannel(1,selection_p(0,spw),selection_p(1,spw),0,spw);
    }


}

VisSet::VisSet(const VisSet& vs,const Block<Int>& columns, 
	       Double timeInterval)
{
    ms_p=vs.ms_p;
    blockOfMS_p=new Block<MeasurementSet>();
    blockOfMS_p->resize(1);
    (*blockOfMS_p)[0]=ms_p;
    multims_p=false;
    selection_p.resize(vs.selection_p.shape());
    selection_p=vs.selection_p;

    iter_p=new VisIter(ms_p,columns,timeInterval);
    for (uInt spw=0; spw<selection_p.ncolumn(); spw++) {
      iter_p->selectChannel(1,selection_p(0,spw),selection_p(1,spw),0,spw);
    }
}

VisSet& VisSet::operator=(const VisSet& other) 
{
    if (this == &other) return *this;
    ms_p=other.ms_p;

    blockOfMS_p= new Block<MeasurementSet> ();
    blockOfMS_p->resize(other.blockOfMS_p->nelements());
    for (uInt k=0; k < blockOfMS_p->nelements() ; ++k)
      (*blockOfMS_p)[k]= (*(other.blockOfMS_p))[k];

    selection_p.resize(other.selection_p.shape());
    selection_p=other.selection_p;
    *iter_p=*(other.iter_p);
    return *this;
}

VisSet::~VisSet() {
  ms_p.flush();
  ms_p=MeasurementSet();// breaking reference

  delete iter_p; iter_p=0;
  //Only one ms then most probably did not use the multi ms constructor
  if(!multims_p){
    (*blockOfMS_p)[0]=MeasurementSet();//breaking reference
  }
  delete blockOfMS_p;
};


void
VisSet::resetVisIter(const Block<Int>& columns, Double timeInterval,
                     asyncio::PrefetchColumns * prefetchColumns)
{
  if (iter_p){
      delete iter_p;   // Delete existing VisIter:
  }

  // Make new VisIter with existing ms_p, and new sort/interval

  iter_p = new VisIter (prefetchColumns, ms_p, columns, true, timeInterval);

  // Inform new VisIter of channel selection

  for (uInt spw=0; spw<selection_p.ncolumn(); spw++) {
    iter_p->selectChannel(1,selection_p(0,spw),selection_p(1,spw),0,spw);
  }
}

void VisSet::initCalSet(Int /*calSet*/)
{
  LogSink logSink;
  LogMessage message(LogOrigin("VisSet","VisSet"));
  Bool doModel=iter_p->ms().tableDesc().isColumn("MODEL_DATA");
  doModel ? message.message("Initializing MODEL_DATA (to unity) and CORRECTED_DATA (to DATA)"):
  message.message("Initializing CORRECTED_DATA (to DATA)");
  logSink.post(message);

  Vector<Int> lastCorrType;
  Vector<Bool> zero;
  Int nRows(0);
  iter_p->setRowBlocking(10000);
  for (iter_p->originChunks(); iter_p->moreChunks(); iter_p->nextChunk()) {

      // figure out which correlations to set to 1. and 0. for the model.
      Vector<Int> corrType; iter_p->corrType(corrType);
      uInt nCorr = corrType.nelements();
      if (nCorr != lastCorrType.nelements() ||
          !allEQ(corrType, lastCorrType)) {
	
        lastCorrType.resize(nCorr); 
        lastCorrType=corrType;
        zero.resize(nCorr);
	
        for (uInt i=0; i<nCorr; i++) 
          {
            zero[i]=(corrType[i]==Stokes::RL || corrType[i]==Stokes::LR ||
                     corrType[i]==Stokes::XY || corrType[i]==Stokes::YX);
          }
      }
      for (iter_p->origin(); iter_p->more(); (*iter_p)++) {
          nRows+=iter_p->nRow();

	  // Read DATA
          Cube<Complex> data;
	  iter_p->visibility(data, VisibilityIterator::Observed);

	  // Set CORRECTED_DATA
          iter_p->setVis(data,VisibilityIterator::Corrected);

	  if(doModel){
	    // Set MODEL_DATA
	    data.set(1.0);
	    if (ntrue(zero)>0) {
	      for (uInt i=0; i < nCorr; i++) {
		if (zero[i]) data(Slice(i), Slice(), Slice()) = Complex(0.0,0.0);
	      }
	    }
	    iter_p->setVis(data,VisibilityIterator::Model);
	  }
      }
  }
  flush();

  iter_p->originChunks();
  iter_p->setRowBlocking(0);

  ostringstream os;
  os << "Initialized " << nRows << " rows."; 
  message.message(os.str());
  logSink.post(message);

}

void VisSet::flush() {
  if(iter_p->newMS()){
      Int msId = iter_p->msId();

      ms_p=(*blockOfMS_p)[msId];
  }
  ms_p.flush();
};

VisIter& VisSet::iter() { return *iter_p; }

// Set or reset the channel selection on all iterators
void VisSet::selectChannel(Int nGroup,Int start, Int width, Int increment, 
			   Int spectralWindow) {

  // Delegate, with callOrigin=true:
  VisSet::selectChannel(nGroup,start,width,increment,spectralWindow,true);

}

void VisSet::selectChannel(Int nGroup,Int start, Int width, Int increment, 
			   Int spectralWindow,
			   Bool callOrigin)
{
  iter_p->selectChannel(nGroup,start,width,increment,spectralWindow); 
  if (callOrigin) iter_p->origin();

  selection_p(0,spectralWindow) = start;
  selection_p(1,spectralWindow) = width;

}

void VisSet::selectChannel(const Matrix<Int>& chansel) {

  // This doesn't handle multiple selections per spw!

  LogSink logSink;
  LogMessage message(LogOrigin("VisSet","selectChannel"));

  for (uInt ispw=0;ispw<chansel.nrow();++ispw) {
    const Int& spw=chansel(ispw,0);
    const Int& start=chansel(ispw,1);
    const Int& end=chansel(ispw,2);
    Int nchan=end-start+1;
    const Int& step=chansel(ispw,3);
    
    ostringstream os;
    os << ".  Spw " << spw << ":" << start << "~" << end 
       << " (" << nchan << " channels, step by " << step << ")";
    message.message(os);
    logSink.post(message);
    
    this->selectChannel(1,start,nchan,step,spw,false);
  }

}

void VisSet::selectAllChans() {

  LogSink logSink;
  LogMessage message(LogOrigin("VisSet","selectAllChans"));
  ostringstream os;
  os << ".  Selecting all channels in selected spws.";
  message.message(os);
  logSink.post(message);

  MSSpWindowColumns msSpW(ms_p.spectralWindow());
  Int nSpw=msSpW.nrow();
  if(nSpw==0)
    throw(AipsError(String("There is no valid spectral windows in "+ms_p.tableName())));
  selection_p.resize(2,nSpw);
  // fill in default selection
  selection_p.row(0)=0; //start
  selection_p.row(1)=msSpW.numChan().getColumn(); 

  // Pass to the VisIter... 
  for (uInt spw=0; spw<selection_p.ncolumn(); ++spw) {
    iter_p->selectChannel(1,selection_p(0,spw),selection_p(1,spw),0,spw);
  }
}

Int VisSet::numberAnt()  
{  
  if(iter_p->newMS()){
    ms_p=(*blockOfMS_p)[iter_p->msId()];
  }
  
  return ((MeasurementSet&)ms_p).antenna().nrow(); // for single (sub)array only..
}
Int VisSet::numberFld()  
{

  if(iter_p->newMS()){
    ms_p=(*blockOfMS_p)[iter_p->msId()];
  }
  return ((MeasurementSet&)ms_p).field().nrow(); 
}
Int VisSet::numberSpw()  
{

  if(iter_p->newMS()){
    ms_p=(*blockOfMS_p)[iter_p->msId()];
  }
  return ((MeasurementSet&)ms_p).spectralWindow().nrow(); 
}
Vector<Int> VisSet::startChan() const
{

  return selection_p.row(0);
}
Vector<Int> VisSet::numberChan() const 
{
  return selection_p.row(1); 
}
Int VisSet::numberCoh() const 
{

  Int numcoh=0;
  for (uInt k=0; k < blockOfMS_p->nelements(); ++k){
    numcoh+=(*blockOfMS_p)[k].nrow();
  }
  return numcoh;
}

void VisSet::addCalSet(MeasurementSet& ms, Bool compress, Bool doModelData) {

    // Add and initialize calibration set (comprising a set of CORRECTED_DATA 
    // and MODEL_DATA columns) to the MeasurementSet.
    
    LogSink logSink;
    LogMessage message(LogOrigin("VisSet","addCalSet"));
    
    message.message("Adding MODEL_DATA (initialized to unity) and CORRECTED_DATA (initialized to DATA) columns");
    logSink.post(message);
    
    {
      // Define a column accessor to the observed data
        TableColumn* data;
        const bool data_is_float = ms.tableDesc().isColumn(MS::columnName(MS::FLOAT_DATA));
        if (data_is_float) {
            data = new ArrayColumn<Float> (ms, MS::columnName(MS::FLOAT_DATA));
        } else {
            data = new ArrayColumn<Complex> (ms, MS::columnName(MS::DATA));
        };

        // Check if the data column is tiled and, if so, the
        // smallest tile shape used.
        TableDesc td = ms.actualTableDesc();
        const ColumnDesc& cdesc = td[data->columnDesc().name()];
        String dataManType = cdesc.dataManagerType();
        String dataManGroup = cdesc.dataManagerGroup();

        IPosition dataTileShape;
        Bool tiled = (dataManType.contains("Tiled"));
        Bool simpleTiling = false;
  

        if (tiled) {
            ROTiledStManAccessor tsm(ms, dataManGroup);
            uInt nHyper = tsm.nhypercubes();
            // Find smallest tile shape
            Int lowestProduct=INT_MAX,highestProduct=-INT_MAX;
            Int highestId=0;
            for (uInt id=0; id < nHyper; id++) {
                Int product = tsm.getTileShape(id).product();
                if (product > 0 && (product < lowestProduct)) {
                    lowestProduct = product;
                    //lowestId = id;
                };
                if (product > 0 && (product > highestProduct)) {
                    highestProduct = product;
                    highestId = id;
                };
            };

	    // 2010Oct07 (gmoellen) We will try using
	    //  maximum volumne intput tile shape, as this 
	    //  improves things drastically for ALMA data with
	    //  an enormous range of nchan (e.g., 3840+:1), and
	    //  will have no impact on data with a single shape
	    //	    dataTileShape = tsm.getTileShape(lowestId);
	    dataTileShape = tsm.getTileShape(highestId);
            simpleTiling = (dataTileShape.nelements() == 3);

        };

        if (!tiled || !simpleTiling) {
            // Untiled, or tiled at a higher than expected dimensionality
            // Use a canonical tile shape of 1 MB size

            Int maxNchan = max (numberChan());
            Int tileSize = maxNchan/10 + 1;
            Int nCorr = data->shape(0)(0);
            dataTileShape = IPosition(3, nCorr, tileSize, 131072/nCorr/tileSize + 1);
        };
  
	if(doModelData){
	  // Add the MODEL_DATA column
	  TableDesc tdModel, tdModelComp, tdModelScale;
	  CompressComplex* ccModel=NULL;
	  String colModel=MS::columnName(MS::MODEL_DATA);
	  
	  tdModel.addColumn(ArrayColumnDesc<Complex>(colModel,"model data", 2));
	  td.addColumn(ArrayColumnDesc<Complex>(colModel,"model data", 2));
	  IPosition modelTileShape = dataTileShape;
	  if (compress) {
            tdModelComp.addColumn(ArrayColumnDesc<Int>(colModel+"_COMPRESSED",
                                                       "model data compressed",2));
            tdModelScale.addColumn(ScalarColumnDesc<Float>(colModel+"_SCALE"));
            tdModelScale.addColumn(ScalarColumnDesc<Float>(colModel+"_OFFSET"));
            ccModel = new CompressComplex(colModel, colModel+"_COMPRESSED",
                                          colModel+"_SCALE", colModel+"_OFFSET", true);
	    
            StandardStMan modelScaleStMan("ModelScaleOffset");
            ms.addColumn(tdModelScale, modelScaleStMan);
	    

            TiledShapeStMan modelCompStMan("ModelCompTiled", modelTileShape);
            ms.addColumn(tdModelComp, modelCompStMan);
            ms.addColumn(tdModel, *ccModel);
	    
	  } else {
            MeasurementSet::addColumnToDesc(tdModel, MeasurementSet::MODEL_DATA, 2);
            //MeasurementSet::addColumnToDesc(td, MeasurementSet::MODEL_DATA, 2);
            TiledShapeStMan modelStMan("ModelTiled", modelTileShape);
            //String hcolName=String("Tiled")+String("MODEL_DATA");
            //tdModel.defineHypercolumn(hcolName,3,
            //			     stringToVector(colModel));
            //td.defineHypercolumn(hcolName,3,
            //		     stringToVector(colModel));
            ms.addColumn(tdModel, modelStMan);
	  };
	   if (ccModel) delete ccModel;
	}
 

        // Add the CORRECTED_DATA column
        TableDesc tdCorr, tdCorrComp, tdCorrScale;
        CompressComplex* ccCorr=NULL;
        String colCorr=MS::columnName(MS::CORRECTED_DATA);

        tdCorr.addColumn(ArrayColumnDesc<Complex>(colCorr,"corrected data", 2));
        IPosition corrTileShape = dataTileShape;
        if (compress) {
            tdCorrComp.addColumn(ArrayColumnDesc<Int>(colCorr+"_COMPRESSED",
                                                      "corrected data compressed",2));
            tdCorrScale.addColumn(ScalarColumnDesc<Float>(colCorr+"_SCALE"));
            tdCorrScale.addColumn(ScalarColumnDesc<Float>(colCorr+"_OFFSET"));
            ccCorr = new CompressComplex(colCorr, colCorr+"_COMPRESSED",
                                         colCorr+"_SCALE", colCorr+"_OFFSET", true);

            StandardStMan corrScaleStMan("CorrScaleOffset");
            ms.addColumn(tdCorrScale, corrScaleStMan);

            TiledShapeStMan corrCompStMan("CorrectedCompTiled", corrTileShape);
            ms.addColumn(tdCorrComp, corrCompStMan);
            ms.addColumn(tdCorr, *ccCorr);

        } else {
            TiledShapeStMan corrStMan("CorrectedTiled", corrTileShape);
            ms.addColumn(tdCorr, corrStMan);
        };
        //MeasurementSet::addColumnToDesc(td, MeasurementSet::CORRECTED_DATA, 2);

       
        if (ccCorr) delete ccCorr;

        /* Set the shapes for each row
           and initialize CORRECTED_DATA to DATA
           and MODEL_DATA to one 
        */
        ArrayColumn<Complex> modelData;
	if(doModelData)
	  modelData.attach(ms, "MODEL_DATA");
        ArrayColumn<Complex> correctedData(ms, "CORRECTED_DATA");

        // Get data description column
        ScalarColumn<Int> dd_col = MSMainColumns(ms).dataDescId();

        // Get polarization column in dd table
        ScalarColumn<Int> pol_col = MSDataDescColumns(ms.dataDescription()).polarizationId();

        // Get correlation column
        ArrayColumn<Int> corr_col(MSColumns(ms).polarization().corrType());

        Matrix<Complex> model_vis;
        Int last_dd_id = 0;
        /* Initialize last_dd_id to something that
           causes the model_vis to be (re-)computed
           for the first MS row
        */
        if (ms.nrow() > 0) {
            last_dd_id = dd_col(0) + 1;
        }

        for (uInt row=0; row < ms.nrow(); row++) {

            IPosition rowShape=data->shape(row);  // shape of (FLOAT_)DATA column

            correctedData.setShape(row, rowShape);
	    if(doModelData)
	      modelData.setShape(row, rowShape);

            Matrix<Complex> vis(rowShape);

            if (data_is_float) {
                /* Convert to complex for the CORRECTED_DATA column */
                Matrix<Float> f(rowShape);
                dynamic_cast<ArrayColumn<Float>*>(data)->get(row, f);

                for (unsigned i = 0; (int)i < f.shape()(0); i++)
                    for (unsigned j = 0; (int)j < f.shape()(1); j++)
                        vis(i, j) = f(i, j);
            } 
            else {
                dynamic_cast<ArrayColumn<Complex>*>(data)->get(row, vis);
            }

            correctedData.put(row, vis);

            // figure out which correlations to set to 1. and 0. for the model.
            // Only do that, if the ddid changed since the last row
            Int dd_id = dd_col(row);
	    if(doModelData){
	      if (dd_id != last_dd_id) {
		
                last_dd_id = dd_id;
		model_vis.resize(rowShape);
		
                Int pol_id = pol_col(dd_id);
                Vector<Int> corrType = corr_col(pol_id);
		
                //cerr << "row = " << row << ", dd = " << dd_id << ", pol = " << pol_id << ", corr = " << corrType << endl;
		
                Vector<Int> lastCorrType;
                Vector<Bool> zero;
		
                uInt nCorr = corrType.nelements();
                if (nCorr != lastCorrType.nelements() ||
                    !allEQ(corrType, lastCorrType)) {
		  
		  lastCorrType.resize(nCorr); 
		  lastCorrType = corrType;
		  zero.resize(nCorr);
		  
		  for (uInt i=0; i<nCorr; i++) 
		    {
		      zero[i]=(corrType[i]==Stokes::RL || corrType[i]==Stokes::LR ||
			       corrType[i]==Stokes::XY || corrType[i]==Stokes::YX);
		    }
                }
      
                model_vis = Complex(1.0,0.0);
      
                for (uInt i=0; i < nCorr; i++) {
		  if (zero[i]) {
		    model_vis(Slice(i), Slice()) = Complex(0.0, 0.0);
		  }
                }
	      } // endif ddid changed
	      modelData.put(row, model_vis);
	    }
        } 

        delete data;
    }

  ostringstream os;
  os << "Initialized " << ms.nrow() << " rows"; 
  message.message(os.str());
  logSink.post(message);

  return;
}

void VisSet::addCalSet2(MeasurementSet& ms, Bool compress, Bool doModelData) {

    // Add but DO NOT INITIALIZE calibration set (comprising a set of CORRECTED_DATA 
    // and MODEL_DATA columns) to the MeasurementSet.
    
    LogSink logSink;
    LogMessage message(LogOrigin("VisSet","addCalSet"));
    doModelData ?
      message.message("Adding MODEL_DATA and CORRECTED_DATA columns"):
      message.message("Adding CORRECTED_DATA columns");
    logSink.post(message);
    
    {
      // Define a column accessor to the observed data
      TableColumn* data;
      const bool data_is_float = ms.tableDesc().isColumn(MS::columnName(MS::FLOAT_DATA));
      if (data_is_float) {
	data = new ArrayColumn<Float> (ms, MS::columnName(MS::FLOAT_DATA));
      } else {
	data = new ArrayColumn<Complex> (ms, MS::columnName(MS::DATA));
      };
      
      // Check if the data column is tiled and, if so, the
      // smallest tile shape used.
      TableDesc td = ms.actualTableDesc();
      const ColumnDesc& cdesc = td[data->columnDesc().name()];
      String dataManType = cdesc.dataManagerType();
      String dataManGroup = cdesc.dataManagerGroup();
      
      IPosition dataTileShape;
      Bool tiled = (dataManType.contains("Tiled"));
      Bool simpleTiling = false;
      
      
      if (tiled) {
	ROTiledStManAccessor tsm(ms, dataManGroup);
	uInt nHyper = tsm.nhypercubes();
	// Find smallest tile shape
	Int lowestProduct=INT_MAX,highestProduct=-INT_MAX;
	Int highestId=0;
	for (uInt id=0; id < nHyper; id++) {
	  Int product = tsm.getTileShape(id).product();
	  if (product > 0 && (product < lowestProduct)) {
	    lowestProduct = product;
	    //lowestId = id;
	  };
	  if (product > 0 && (product > highestProduct)) {
	    highestProduct = product;
	    highestId = id;
	  };
	};
	
	// 2010Oct07 (gmoellen) We will try using
	//  maximum volumne intput tile shape, as this 
	//  improves things drastically for ALMA data with
	//  an enormous range of nchan (e.g., 3840+:1), and
	//  will have no impact on data with a single shape
	//	    dataTileShape = tsm.getTileShape(lowestId);
	dataTileShape = tsm.getTileShape(highestId);
	simpleTiling = (dataTileShape.nelements() == 3);
	
      };
      
      if (!tiled || !simpleTiling) {
	// Untiled, or tiled at a higher than expected dimensionality
	// Use a canonical tile shape of 1 MB size
	
	Int maxNchan = max (numberChan());
	Int tileSize = maxNchan/10 + 1;
	Int nCorr = data->shape(0)(0);
	dataTileShape = IPosition(3, nCorr, tileSize, 131072/nCorr/tileSize + 1);
      };
      
      delete data;

      if(doModelData){
	// Add the MODEL_DATA column
	TableDesc tdModel, tdModelComp, tdModelScale;
	CompressComplex* ccModel=NULL;
	String colModel=MS::columnName(MS::MODEL_DATA);
	
	tdModel.addColumn(ArrayColumnDesc<Complex>(colModel,"model data", 2));
	td.addColumn(ArrayColumnDesc<Complex>(colModel,"model data", 2));
	IPosition modelTileShape = dataTileShape;
	if (compress) {
	  tdModelComp.addColumn(ArrayColumnDesc<Int>(colModel+"_COMPRESSED",
						     "model data compressed",2));
	  tdModelScale.addColumn(ScalarColumnDesc<Float>(colModel+"_SCALE"));
	  tdModelScale.addColumn(ScalarColumnDesc<Float>(colModel+"_OFFSET"));
	  ccModel = new CompressComplex(colModel, colModel+"_COMPRESSED",
					colModel+"_SCALE", colModel+"_OFFSET", true);
	
	  StandardStMan modelScaleStMan("ModelScaleOffset");
	  ms.addColumn(tdModelScale, modelScaleStMan);
	
	
	  TiledShapeStMan modelCompStMan("ModelCompTiled", modelTileShape);
	  ms.addColumn(tdModelComp, modelCompStMan);
	  ms.addColumn(tdModel, *ccModel);
	
	} else {
	  MeasurementSet::addColumnToDesc(tdModel, MeasurementSet::MODEL_DATA, 2);
	  //MeasurementSet::addColumnToDesc(td, MeasurementSet::MODEL_DATA, 2);
	  TiledShapeStMan modelStMan("ModelTiled", modelTileShape);
	  //String hcolName=String("Tiled")+String("MODEL_DATA");
	  //tdModel.defineHypercolumn(hcolName,3,
	  //			     stringToVector(colModel));
	  //td.defineHypercolumn(hcolName,3,
	  //		     stringToVector(colModel));
	  ms.addColumn(tdModel, modelStMan);
	};
	if (ccModel) delete ccModel;
      }
      // Add the CORRECTED_DATA column
      TableDesc tdCorr, tdCorrComp, tdCorrScale;
      CompressComplex* ccCorr=NULL;
      String colCorr=MS::columnName(MS::CORRECTED_DATA);
      
      tdCorr.addColumn(ArrayColumnDesc<Complex>(colCorr,"corrected data", 2));
      IPosition corrTileShape = dataTileShape;
      if (compress) {
	tdCorrComp.addColumn(ArrayColumnDesc<Int>(colCorr+"_COMPRESSED",
						  "corrected data compressed",2));
	tdCorrScale.addColumn(ScalarColumnDesc<Float>(colCorr+"_SCALE"));
	tdCorrScale.addColumn(ScalarColumnDesc<Float>(colCorr+"_OFFSET"));
	ccCorr = new CompressComplex(colCorr, colCorr+"_COMPRESSED",
				     colCorr+"_SCALE", colCorr+"_OFFSET", true);
	
	StandardStMan corrScaleStMan("CorrScaleOffset");
	ms.addColumn(tdCorrScale, corrScaleStMan);
	
	TiledShapeStMan corrCompStMan("CorrectedCompTiled", corrTileShape);
	ms.addColumn(tdCorrComp, corrCompStMan);
	ms.addColumn(tdCorr, *ccCorr);
	
      } else {
	TiledShapeStMan corrStMan("CorrectedTiled", corrTileShape);
	ms.addColumn(tdCorr, corrStMan);
      };
      //MeasurementSet::addColumnToDesc(td, MeasurementSet::CORRECTED_DATA, 2);
      
    
      if (ccCorr) delete ccCorr;
      
      ms.flush();

    }

  return;
}




void VisSet::addScratchCols(MeasurementSet& ms, Bool compress, Bool doModelData){

  LogSink logSink;
  LogMessage message(LogOrigin("VisSet","VisSet"));

  //function to add scratchy column
  Bool init=true;

  Int nSpw=ms.spectralWindow().nrow();
  MSSpWindowColumns msSpW(ms.spectralWindow());
  selection_p.resize(2,nSpw);
  // fill in default selection
  selection_p.row(0)=0; //start
  selection_p.row(1)=msSpW.numChan().getColumn(); 

  // Add scratch columns
  if (init) {
    
    VisSetUtil::removeCalSet(ms);
    addCalSet(ms, compress, doModelData);
      

    // Force re-sort if it was sorted 
    if (ms.keywordSet().isDefined("SORT_COLUMNS")) 
      ms.rwKeywordSet().removeField("SORT_COLUMNS");
    if (ms.keywordSet().isDefined("SORTED_TABLE")) 
      ms.rwKeywordSet().removeField("SORTED_TABLE");
  }
}

String VisSet::msName(){

  String name;
  if(ms_p.isMarkedForDelete()){ // this is a temporary selected table
    Block<String> refTables = ms_p.getPartNames(false);
    name = refTables[0];
  }
  else{
    name = ms_p.tableName();
  }
  return name;

}

String VisSet::sysCalTableName(){

  return ms_p.sysCalTableName();
}

String VisSet::spectralWindowTableName(){
  
  return ms_p.spectralWindowTableName();
}

String VisSet::fieldTableName(){
  
  return ms_p.fieldTableName();
}

String VisSet::syspowerTableName(){
  
  return ms_p.rwKeywordSet().asTable("SYSPOWER").tableName();
}

String VisSet::caldeviceTableName(){
  
  return ms_p.rwKeywordSet().asTable("CALDEVICE").tableName();
}




} //# NAMESPACE CASA - END

