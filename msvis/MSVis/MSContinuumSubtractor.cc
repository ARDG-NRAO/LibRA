//# MSContinuumSubtractor.cc:  Subtract continuum from spectral line data
//# Copyright (C) 2004
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
//#
#include <msvis/MSVis/MSContinuumSubtractor.h>

#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/casa/Containers/RecordFieldId.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MSSel/MSSelector.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/ms/MSSel/MSSelectionTools.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <msvis/MSVis/IteratingParameters.h>
#include <msvis/MSVis/AveragingVi2Factory.h>
#include <msvis/MSVis/LayeredVi2Factory.h>
#include <casacore/scimath/Fitting/LinearFit.h>
#include <casacore/scimath/Functionals/Polynomial.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayUtil.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <iostream>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//
// Constructor assigns pointer (if MS goes out of scope you will get rubbish)
MSContinuumSubtractor::MSContinuumSubtractor (MeasurementSet& ms)
  : ms_p(&ms),itsSolInt(0.0),itsOrder(0),itsMode("subtract")
{

  // Make a VisSet so scratch columns are created
  Block<Int> nosort(0);
  Matrix<Int> noselection;
  Double timeInterval=0.0;
  Bool compress(False);
  VisSet vs(ms,nosort,noselection,timeInterval,compress);

  nSpw_= vs.numberSpw();

}


//
// Assignment operator
//
MSContinuumSubtractor& MSContinuumSubtractor::operator=(MSContinuumSubtractor& other)
{
  if (this==&other) return *this;
  ms_p = other.ms_p;
  return *this;
}


//
// Destructor does nothing
//
MSContinuumSubtractor::~MSContinuumSubtractor()
{}

// Set the required field Ids
void MSContinuumSubtractor::setField(const String& field)
{

  MSSelection mssel;
  mssel.setFieldExpr(field);
  Vector<Int> fldlist;
  fldlist=mssel.getFieldList(ms_p);

  setFields(fldlist);

}

void MSContinuumSubtractor::setFields(const Vector<Int>& fieldIds)
{
  itsFieldIds = fieldIds;
}


// Set the channels to use in the fit
void MSContinuumSubtractor::setFitSpw(const String& fitspw)
{
  // NB: this method assumes spwids == ddids!

  // Using MSSelection  
  MSSelection mssel;
  mssel.setSpwExpr(fitspw);
  Vector<Int> spwlist;
  spwlist=mssel.getSpwList(ms_p);

  if (spwlist.nelements()==0) {
    spwlist.resize(nSpw_);
    indgen(spwlist);
  }

  setDataDescriptionIds(spwlist);

  itsFitChans= mssel.getChanList(ms_p);

}

// Set the channels from which the fit should be subtracted
void MSContinuumSubtractor::setSubSpw(const String& subspw)
{
  // Using MSSelection  
  MSSelection mssel;
  mssel.setSpwExpr(subspw);
  itsSubChans= mssel.getChanList(ms_p);

}

void MSContinuumSubtractor::setDataDescriptionIds(const Vector<Int>& ddIds)
{
  itsDDIds = ddIds;
}

// Set the solution interval in seconds, the value zero implies scan averaging
void MSContinuumSubtractor::setSolutionInterval(Float solInt)
{
  itsSolInt = solInt;
}

// Set the solution interval in seconds, the value zero implies scan averaging
void MSContinuumSubtractor::setSolutionInterval(String solInt)
{

  LogIO os(LogOrigin("MSContinuumSubtractor","setSolutionInterval"));

  os << LogIO::NORMAL << "Fitting continuum on ";

  if (upcase(solInt).contains("INF")) {
    // ~Infinite (this actually means per-scan in UV contsub)
    solInt="inf";
    itsSolInt=0.0;  
    os <<"per-scan ";
  }
  else if (upcase(solInt).contains("INT")) {
    // Per integration
    itsSolInt=-1.0;
    os << "per-integration ";
  }
  else {
    // User-selected timescale
    QuantumHolder qhsolint;
    String error;
    Quantity qsolint;
    qhsolint.fromString(error,solInt);
    if (error.length()!=0)
      throw(AipsError("Unrecognized units for solint."));

    qsolint=qhsolint.asQuantumDouble();

    if (qsolint.isConform("s"))
      itsSolInt=qsolint.get("s").getValue();
    else {
      // assume seconds
      itsSolInt=qsolint.getValue();
    }

    os << itsSolInt <<"-second (per scan) ";
  }

  os << "timescale." << LogIO::POST;
    
}



// Set the order of the fit (1=linear)
void MSContinuumSubtractor::setOrder(Int order)
{
  itsOrder = order;
}

// Set the processing mode: subtract, model or replace
void MSContinuumSubtractor::setMode(const String& mode)
{
  itsMode = mode;
}

// Do the subtraction (or save the model)
void MSContinuumSubtractor::subtract()
{
  
  LogIO os(LogOrigin("MSContinuumSubtractor","subtract"));
  os << LogIO::NORMAL<< "MSContinuumSubtractor::subtract() - parameters:"
     << "ddIds="<<itsDDIds<<", fieldIds="<<itsFieldIds
     << ", order="<<itsOrder
     << ", mode="<<itsMode<<LogIO::POST;

  if (itsFitChans.nelements()>0) {
    os<<"fit channels: " << LogIO::POST;
    for (uInt i=0;i<itsFitChans.nrow();++i)
      os<<" spw="<<itsFitChans.row(i)(0)
	<<": "<<itsFitChans.row(i)(1)<<"~"<<itsFitChans.row(i)(2)
	<< LogIO::POST;
  }

  ostringstream select;
  select <<"select from $1 where ANTENNA1!=ANTENNA2";
  if (itsFieldIds.nelements()>0) {
    select<<" && FIELD_ID IN ["<<itsFieldIds(0);
    for (uInt j=1; j<itsFieldIds.nelements(); j++) select<<", "<<itsFieldIds(j);
    select<<"]";
  }
  if (itsDDIds.nelements()>0) {
    select<<" && DATA_DESC_ID IN ["<<itsDDIds(0);
    for (uInt j=1; j<itsDDIds.nelements(); j++) select<<", "<<itsDDIds(j);
    select<<"]";
  }
  //os <<"Selection string: "<<select.str()<<LogIO::POST;
  //os <<" nrow="<<ms_p->nrow()<<LogIO::POST;
  MeasurementSet selectedMS(tableCommand(select,*ms_p));
  MSSelector msSel(selectedMS);
  //os <<" nrow="<<msSel.nrow()<<LogIO::POST;
  MSColumns msc(selectedMS);
  if (itsDDIds.nelements()>1) {
    cout<<"Processing "<<itsDDIds.nelements()<<" spectral windows"<<endl;
  }
  for (uInt iDD=0; iDD<itsDDIds.nelements(); iDD++) {
    Vector<Int> ddIDs(1,itsDDIds(iDD));
    msSel.initSelection(ddIDs);
    //os <<" nrow="<<msSel.nrow()<<LogIO::POST;
    if (msSel.nrow()==0) continue;
    Int nChan=msc.spectralWindow()
                  .numChan()(msc.dataDescription().spectralWindowId()(ddIDs(0)));
    Vector<Int> corrTypes=msc.polarization().
          corrType()(msc.dataDescription().polarizationId()(ddIDs(0)));


    // default to fit all channels
    Vector<Bool> fitChanMask(nChan,True);

    // Handle non-trivial channel selection:

    if (itsFitChans.nelements()>0 && anyEQ(itsFitChans.column(0),itsDDIds(iDD))) {
      // If subset of channels selected, set mask all False...
      fitChanMask=False;
      
      IPosition blc(1,0);
      IPosition trc(1,0);

      // ... and set only selected channels True:
      for (uInt i=0;i<itsFitChans.nrow();++i) {
	Vector<Int> chansel(itsFitChans.row(i));

	// match current spwId/DDid
	if (chansel(0)==itsDDIds(iDD)) {
	  blc(0)=chansel(1);
	  trc(0)=chansel(2);
	  fitChanMask(blc,trc)=True;
	}
      }
    }

    //  cout << "fitChanMask = " << fitChanMask << endl;

    // default to subtract from all channels
    Vector<Bool> subChanMask(nChan,True);
    if (itsSubChans.nelements()>0 && anyEQ(itsSubChans.column(0),itsDDIds(iDD))) {
      // If subset of channels selected, set sub mask all False...
      subChanMask=False;
      
      IPosition blc(1,0);
      IPosition trc(1,0);

      // ... and set only selected sub channels True:
      for (uInt i=0;i<itsSubChans.nrow();++i) {
	Vector<Int> chansel(itsSubChans.row(i));

	// match current spwId/DDid
	if (chansel(0)==itsDDIds(iDD)) {
	  blc(0)=chansel(1);
	  trc(0)=chansel(2);
	  subChanMask(blc,trc)=True;
	}
      }
    }

    // select parallel hand polarizations
    Vector<String> polSel(corrTypes.nelements()); 
    Int nPol = 0;
    for (uInt j=0; j<corrTypes.nelements(); j++) {
      if (corrTypes(j)==Stokes::XX||corrTypes(j)==Stokes::YY||
          corrTypes(j)==Stokes::RR||corrTypes(j)==Stokes::LL) {
        polSel(nPol++)=Stokes::name(Stokes::type(corrTypes(j)));
      }
    }
    polSel.resize(nPol,True);
    msSel.selectPolarization(polSel);

    msSel.iterInit(
        stringToVector("ARRAY_ID,DATA_DESC_ID,SCAN_NUMBER,FIELD_ID,TIME"),
        itsSolInt,0,False);
    msSel.iterOrigin();
    Int nIter=1;
    while (msSel.iterNext()) nIter++;
    os<<"Processing "<<nIter<<" slots."<< LogIO::POST;
    msSel.iterOrigin();
    do {
      Record avRec = msSel.getData(stringToVector("corrected_data"),True,0,1,True);
      Record dataRec = msSel.getData(stringToVector("model_data,corrected_data"),
                                     True,0,1);
      Array<Complex> avCorData(avRec.asArrayComplex("corrected_data"));
      Array<Complex> modelData(dataRec.asArrayComplex("model_data"));
      Array<Complex> correctedData(dataRec.asArrayComplex("corrected_data"));
      Int nTime=modelData.shape()[3];
      Int nIfr=modelData.shape()[2];

      // fit
      Vector<Float> x(nChan);
      for (Int i=0; i<nChan; i++) x(i)=i;
      LinearFit<Float> fitter;
      Polynomial<AutoDiff<Float> > apoly(itsOrder);
      fitter.setFunction(apoly);
      Polynomial<Float> poly(itsOrder);
      
      Vector<Float> y1(nChan),y2(nChan);
      Vector<Float> sol(itsOrder+1);
      Vector<Complex> tmp(nChan);
      IPosition start(3,0,0,0),end(3,0,nChan-1,0);
      for (; start[2]<nIfr; start[2]++,end[2]++) {
        for (start[0]=end[0]=0; start[0]<nPol; start[0]++,end[0]++) {
          Vector<Complex> c(avCorData(start,end).nonDegenerate());
          tmp=c; // copy into contiguous storage
          c.set(Complex(0.0));  // zero the input
          real(y1,tmp);
          imag(y2,tmp);
          sol = fitter.fit(x,y1,&fitChanMask);
          poly.setCoefficients(sol);
          y1=x;
          y1.apply(poly);
          sol = fitter.fit(x,y2,&fitChanMask);
          poly.setCoefficients(sol);
          y2=x;
          y2.apply(poly);
          for (Int chn=0; chn<nChan; chn++) 
            if (subChanMask(chn))
              c(chn)=Complex(y1(chn),y2(chn));
        }
      }
        
      IPosition start4(4,0,0,0,0),end4(4,nPol-1,nChan-1,nIfr-1,0);
      for (Int iTime=0; iTime<nTime; iTime++,start4[3]++,end4[3]++) {
        if  (itsMode!="replace") {
          Array<Complex> model(modelData(start4,end4).nonDegenerate(3));
          model=avCorData;
        }
        if (itsMode!="model") {
          Array<Complex> corr(correctedData(start4,end4).nonDegenerate(3));
          if (itsMode=="replace") corr=avCorData;
          if (itsMode=="subtract" || itsMode=="sub") corr-=avCorData;
        }
      }  
                    
      Record newDataRec;
      if (itsMode=="model"||itsMode=="subtract"||itsMode=="sub") {
        newDataRec.define("model_data",modelData);
      }
      if (itsMode=="replace"||itsMode=="subtract"||itsMode=="sub") {
        newDataRec.define("corrected_data",correctedData);
      }
      msSel.putData(newDataRec);
      
    } while (msSel.iterNext());
    
  }
}

// Do the subtraction (or save the model)
void MSContinuumSubtractor::subtract2()
{
  // Log selections for subtraction
  LogIO os(LogOrigin("MSContinuumSubtractor","subtract2"));
  os << LogIO::NORMAL << "parameters:" 
     << LogIO::POST;
  os << LogIO::NORMAL << "   ddIds=" << itsDDIds
     << ", fieldIds="<< itsFieldIds
     << ", order="<< itsOrder
     << ", mode="<< itsMode 
     << LogIO::POST;
  if (itsFitChans.nelements()>0) {
    os << "fit channels: " << LogIO::POST;
    for (uInt i=0; i<itsFitChans.nrow(); ++i)
      os << " spw=" << itsFitChans.row(i)(0)
	     << ": " << itsFitChans.row(i)(1) << "~" << itsFitChans.row(i)(2)
	     << LogIO::POST;
  }

  // Use taQL for selection (no DDiD sel in MSSelection)
  ostringstream select;
  select << "select from $1 where ANTENNA1!=ANTENNA2";
  if (itsFieldIds.nelements() > 0) {
    select << " && FIELD_ID IN [" << itsFieldIds(0);
    for (uInt j=1; j<itsFieldIds.nelements(); j++) 
        select << ", " << itsFieldIds(j);
    select << "]";
  }
  if (itsDDIds.nelements() > 0) {
    select << " && DATA_DESC_ID IN [" << itsDDIds(0);
    for (uInt j=1; j<itsDDIds.nelements(); j++)
        select << ", " << itsDDIds(j);
    select << "]";
  }
  MeasurementSet selectedMS(tableCommand(select, *ms_p));
  MSColumns msc(selectedMS);
  MSSelection mssel;  // for channel and poln selection

  for (uInt iDD=0; iDD<itsDDIds.nelements(); iDD++) {
    Int thisDDId(itsDDIds(iDD));
    Int nChan = msc.spectralWindow().numChan()
        (msc.dataDescription().spectralWindowId()(thisDDId));
    Vector<Int> corrTypes = msc.polarization().corrType()
        (msc.dataDescription().polarizationId()(thisDDId));

    // default to fit all channels
    Vector<Bool> fitChanMask(nChan,True);
    // Handle non-trivial channel selection:
    if (itsFitChans.nelements()>0 && anyEQ(itsFitChans.column(0),thisDDId)) {
      // If subset of channels selected, set fit mask all False...
      fitChanMask = False;
      IPosition blc(1,0);
      IPosition trc(1,0);
      // ... and set only selected fit channels True:
      for (uInt i=0; i<itsFitChans.nrow(); ++i) {
	    Vector<Int> chansel(itsFitChans.row(i));  // [spw, startChan, endChan]
	    // match current spwId/DDid
	    if (chansel(0) == thisDDId) {
	        blc(0) = chansel(1);
	        trc(0) = chansel(2);
	        fitChanMask(blc,trc) = True;
	    }
      }
    }

    // default to subtract from all channels
    Vector<Bool> subChanMask(nChan,True);
    // Handle non-trivial channel selection:
    if (itsSubChans.nelements()>0 && anyEQ(itsSubChans.column(0),thisDDId)) {
      // If subset of channels selected, set sub mask all False...
      subChanMask = False;
      IPosition blc(1,0);
      IPosition trc(1,0);
      // ... and set only selected sub channels True:
      for (uInt i=0; i<itsSubChans.nrow(); ++i) {
	    Vector<Int> chansel(itsSubChans.row(i));
	    // match current spwId/DDid
	    if (chansel(0) == thisDDId) {
	        blc(0) = chansel(1);
	        trc(0) = chansel(2);
	        subChanMask(blc,trc) = True;
	    }
      }
    }

    // select parallel hand polarizations with MSSelection
    Vector<String> polSel(corrTypes.nelements()); 
    Int nPol = 0;
    for (uInt j=0; j<corrTypes.nelements(); j++) {
      if (corrTypes(j)==Stokes::XX || corrTypes(j)==Stokes::YY ||
          corrTypes(j)==Stokes::RR || corrTypes(j)==Stokes::LL) {
        polSel(nPol++) = Stokes::name(Stokes::type(corrTypes(j)));
      }
    }
    polSel.resize(nPol,True);
    String polnExpr("");
    if (nPol > 0) {
        polnExpr = polSel[0];
        for (Int np=1; np<nPol; ++np) {
            polnExpr += "," + polSel[np];
        }
    }
    // apply selection to get correlation slices
    String spwExpr = String::toString(thisDDId);
    MeasurementSet spwSelMS(selectedMS);
    Vector<Vector<Slice> > chansel;
    Vector<Vector<Slice> > corrsel;
    mssel.clear();
    mssSetData(selectedMS, spwSelMS, chansel, corrsel,
               "", "", "", "", spwExpr, "", "",
               polnExpr, "", "", "", "", 1, &mssel);

    casacore::Float chunkInt(itsSolInt);
    if (chunkInt < 0.0) {
        MSMainColumns msmain(spwSelMS);
        chunkInt = msmain.interval().getColumn()(0);
    }

    // Set up FrequencySelection with chansel and corrsel for VisibilityIterator2
    vi::FrequencySelectionUsingChannels freqSel = vi::FrequencySelectionUsingChannels();
    freqSel.add(mssel, &spwSelMS);
    freqSel.addCorrelationSlices(corrsel);

    // Sort Columns
    Block<Int> columns(5);
    Int i(0);
    columns[i++] = MS::ARRAY_ID;
    columns[i++] = MS::DATA_DESC_ID;
    columns[i++] = MS::SCAN_NUMBER;
    columns[i++] = MS::FIELD_ID;
    columns[i++] = MS::TIME;
    vi::SortColumns sortcols(columns, false);
      
    // Set up averaged non-calibrating vi
    vi::IteratingParameters iterpar(chunkInt, sortcols);
    vi::AveragingOptions avgopt(vi::AveragingOptions::AverageCorrected |
            vi::AveragingOptions::CorrectedWeightAvgFromWEIGHT);
    vi::AveragingParameters avgpar(1e8, 0.0, sortcols, avgopt);
    vi::LayeredVi2Factory factory(&spwSelMS, &iterpar, &avgpar);
    vi::VisibilityIterator2* vi2 = new vi::VisibilityIterator2(factory);
    vi2->setFrequencySelection(freqSel);
    vi::VisBuffer2* vb2 = vi2->getVisBuffer();
    vi2->originChunks();
    vi2->origin();

    // Get continuum model
    // fit
    Vector<Float> x(nChan);
    for (Int i=0; i<nChan; i++) x(i)=i;
    LinearFit<Float> fitter;
    Polynomial<AutoDiff<Float> > apoly(itsOrder);
    fitter.setFunction(apoly);
    Polynomial<Float> poly(itsOrder);
    Vector<Float> y1(nChan),y2(nChan);
    Vector<Float> sol(itsOrder+1);
    Vector<Complex> tmp(nChan);
    Cube<Complex> avgCorrData;
    // map is <scan, fit model>
    map<Int, Cube<Complex>> fitCorrData;
    map<Int, Vector<Int>> fitCorrIfr;
    Int nIter(0);
    IPosition start(3,0,0,0), end(3,0,nChan-1,0);
    for (vi2->originChunks(); vi2->moreChunks(); vi2->nextChunk()){
        for (vi2->origin(); vi2->more(); vi2->next()){
            avgCorrData.resize();
            avgCorrData = vb2->visCubeCorrected();
            for (start[2]=end[2]=0; start[2]<avgCorrData.shape()[2];
                    start[2]++,end[2]++) {
                for (start[0]=end[0]=0; start[0]<nPol; start[0]++,end[0]++) {
                    Vector<Complex> c(avgCorrData(start,end).nonDegenerate());
                    tmp=c; // copy into contiguous storage
                    c.set(Complex(0.0));  // zero the input
                    real(y1,tmp);
                    imag(y2,tmp);
                    sol = fitter.fit(x,y1,&fitChanMask);
                    poly.setCoefficients(sol);
                    y1=x;
                    y1.apply(poly);
                    sol = fitter.fit(x,y2,&fitChanMask);
                    poly.setCoefficients(sol);
                    y2=x;
                    y2.apply(poly);
                    for (Int chn=0; chn<nChan; chn++) {
                        if (subChanMask(chn))
                            c(chn)=Complex(y1(chn),y2(chn));
                    }
                }
            }
            fitCorrData[nIter] = avgCorrData;
            Vector<Int> ant1 = vb2->antenna1();
            Vector<Int> ant2 = vb2->antenna2();
            fitCorrIfr[nIter] = ant1*1000 + ant2;
            nIter++;
        }
    }
    os << "Processing " << nIter << " slots for ddId " << iDD << LogIO::POST;
    delete vi2;

    // unaveraged non-calibrating vi
    vi::LayeredVi2Factory factory2(&spwSelMS, &iterpar);
    vi2 = new vi::VisibilityIterator2(factory2);
    vi2->setFrequencySelection(freqSel);
    vb2 = vi2->getVisBuffer();
    vi2->originChunks();
    vi2->origin();
    Cube<Complex> modelData, correctedData, fitModel;
    nIter=0;
    for (vi2->originChunks(); vi2->moreChunks(); vi2->nextChunk()){
        for (vi2->origin(); vi2->more(); vi2->next()){
            // get model and corrected data with poln selection applied
            modelData.resize();
            modelData = vb2->visCubeModel();
            correctedData.resize();
            correctedData = vb2->visCubeCorrected();
            IPosition datashape = correctedData.shape();
            fitModel.resize();
            fitModel = fitCorrData[nIter];

            // Unaveraged data may have different shape
            if (fitModel.shape() != datashape) {
                // only use baselines in this chunk
                Cube<Complex> newFitModel(datashape);
                Vector<Int> ant1 = vb2->antenna1();
                Vector<Int> ant2 = vb2->antenna2();
                Vector<Int> ifrs = ant1*1000 + ant2;
                Vector<Int> fitIfr = fitCorrIfr[nIter];
                IPosition start(3,0,0,0), start2(3,0,0,0);
                Int nPol(datashape[0]), nChan(datashape[1]), nIfr(datashape[2]);
                IPosition end(3, nPol-1, nChan-1, nIfr-1);
                IPosition end2(3, nPol-1, nChan-1, nIfr-1);
                for (uInt i=0; i<ifrs.size(); ++i) {
                    start[2] = end[2] = i;
                    Int ifr = ifrs[i];
                    // now find ifr in fitIfr
                    for (uInt j=0; j<fitIfr.size(); ++j) {
                        if (fitIfr[j]==ifr) {
                            start2[2] = end2[2] = j;
                            newFitModel(start,end) = fitModel(start2,end2);
                            break;
                        }
                    }
                }
                fitModel.resize(datashape);
                fitModel = newFitModel;
            }

            // write out cubes to MS
            if (itsMode=="subtract" || itsMode=="model" || itsMode=="sub") {
                modelData = fitModel;
                vi2->getImpl()->writeVisModel(modelData);
            }
            if (itsMode=="replace") {
                correctedData = fitModel;
                vi2->getImpl()->writeVisCorrected(correctedData);
            }
            if (itsMode=="subtract" || itsMode=="sub") {
                correctedData -= fitModel;
                vi2->getImpl()->writeVisCorrected(correctedData);
            }
        }
        nIter++;
    }
    delete vi2;
  } // end DDId loop
}


} //# NAMESPACE CASA - END

