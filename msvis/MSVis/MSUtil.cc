//# MSUtil.cc: Some MS specific Utilities
//# Copyright (C) 2011
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#include <casa/Utilities/Sort.h>
#include <measures/Measures/MeasTable.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MSSel/MSSpwIndex.h>
#include <ms/MSSel/MSDataDescIndex.h>
#include <msvis/MSVis/MSUtil.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/OS/Path.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <iomanip>
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  MSUtil::MSUtil(){};
  void MSUtil::getSpwInFreqRange(Vector<Int>& spw, Vector<Int>& start,
				  Vector<Int>& nchan,
				  const MeasurementSet& ms, 
				  const Double freqStart,
				  const Double freqEnd,
				  const Double freqStep,
			    const MFrequency::Types freqframe,
			    const Int fieldId)
  {
    spw.resize();
    start.resize();
    nchan.resize();
    Vector<Double> t;
    ScalarColumn<Double> (ms,MS::columnName(MS::TIME)).getColumn(t);
    //Vector<Int> ddId;
    //Vector<Int> fldId;
    
    MSFieldColumns fieldCol(ms.field());
    MSDataDescColumns ddCol(ms.dataDescription());
    MSSpWindowColumns spwCol(ms.spectralWindow());
    ROScalarMeasColumn<MEpoch> timeCol(ms, MS::columnName(MS::TIME));
    Vector<uInt>  uniqIndx;
    uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, t, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);

    t.resize(0);
    //ScalarColumn<Int> (ms,MS::columnName(MS::DATA_DESC_ID)).getColumn(ddId);
    //ScalarColumn<Int> (ms,MS::columnName(MS::FIELD_ID)).getColumn(fldId);
    ScalarColumn<Int> ddId(ms,MS::columnName(MS::DATA_DESC_ID));
    ScalarColumn<Int> fldId(ms,MS::columnName(MS::FIELD_ID));
    //now need to do the conversion to data frame from requested frame
    //Get the epoch mesasures of the first row
    MEpoch ep;
    timeCol.get(0, ep);
    String observatory;
    MPosition obsPos;
    /////observatory position
    MSColumns msc(ms);
    if (ms.observation().nrow() > 0) {
      observatory = msc.observation().telescopeName()(msc.observationId()(0));
    }
    if (observatory.length() == 0 || 
	!MeasTable::Observatory(obsPos,observatory)) {
      // unknown observatory, use first antenna
      obsPos=msc.antenna().positionMeas()(0);
    }
    //////
    Int oldDD=ddId(0);
    Int newDD=oldDD;
    //For now we will assume that the field is not moving very far from polynome 0
    MDirection dir =fieldCol.phaseDirMeas(fieldId);
    MFrequency::Types obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(ddCol.spectralWindowId()(ddId(0))));
    //cout << "nTimes " << nTimes << endl;
    //cout << " obsframe " << obsMFreqType << " reqFrame " << freqframe << endl; 
    MeasFrame frame(ep, obsPos, dir);
    MFrequency::Convert toObs(freqframe,
                              MFrequency::Ref(obsMFreqType, frame));
    Double freqEndMax=freqEnd;
    Double freqStartMin=freqStart;
    if(freqframe != obsMFreqType){
      freqEndMax=0.0;
      freqStartMin=C::dbl_max;
    }
    for (uInt j=0; j< nTimes; ++j){
      if(fldId(uniqIndx[j]) ==fieldId){
	timeCol.get(uniqIndx[j], ep);
	newDD=ddId(uniqIndx[j]);
	if(oldDD != newDD){
	  oldDD=newDD;
	  if(spwCol.measFreqRef()(ddCol.spectralWindowId()(newDD)) != obsMFreqType){
	    obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(ddCol.spectralWindowId()(newDD)));
	    toObs.setOut(MFrequency::Ref(obsMFreqType, frame));
	  }
	}
	if(obsMFreqType != freqframe){
	  frame.resetEpoch(ep);
	  Double freqTmp=toObs(Quantity(freqStart, "Hz")).get("Hz").getValue();
	  freqStartMin=(freqStartMin > freqTmp) ? freqTmp : freqStartMin;
	  freqTmp=toObs(Quantity(freqEnd, "Hz")).get("Hz").getValue();
	  freqEndMax=(freqEndMax < freqTmp) ? freqTmp : freqEndMax; 
	}
      }
    }

    //cout << "freqStartMin " << freqStartMin << " freqEndMax " << freqEndMax << endl;
    MSSpwIndex spwIn(ms.spectralWindow());
    spwIn.matchFrequencyRange(freqStartMin-0.5*freqStep, freqEndMax+0.5*freqStep, spw, start, nchan);


 
  }

    void MSUtil::getSpwInSourceFreqRange(Vector<Int>& spw, Vector<Int>& start,
					 Vector<Int>& nchan,
					 const MeasurementSet& ms, 
					 const Double freqStart,
					 const Double freqEnd,
					 const Double freqStep,
					 const String& ephemtab,
					 const Int fieldId)
  {
    spw.resize();
    start.resize();
    nchan.resize();
    Vector<Double> t;
    ScalarColumn<Double> (ms,MS::columnName(MS::TIME)).getColumn(t);
    //Vector<Int> ddId;
    //Vector<Int> fldId;
    
    MSFieldColumns fieldCol(ms.field());
    MSDataDescColumns ddCol(ms.dataDescription());
    MSSpWindowColumns spwCol(ms.spectralWindow());
    ROScalarMeasColumn<MEpoch> timeCol(ms, MS::columnName(MS::TIME));
    Vector<uInt>  uniqIndx;
    uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, t, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);

    t.resize(0);
    //ScalarColumn<Int> (ms,MS::columnName(MS::DATA_DESC_ID)).getColumn(ddId);
    //ScalarColumn<Int> (ms,MS::columnName(MS::FIELD_ID)).getColumn(fldId);
    ScalarColumn<Int> ddId(ms,MS::columnName(MS::DATA_DESC_ID));
    ScalarColumn<Int> fldId(ms,MS::columnName(MS::FIELD_ID));
    //now need to do the conversion to data frame from requested frame
    //Get the epoch mesasures of the first row
    MEpoch ep;
    timeCol.get(0, ep);
    String observatory;
    MPosition obsPos;
    /////observatory position
    MSColumns msc(ms);
    if (ms.observation().nrow() > 0) {
      observatory = msc.observation().telescopeName()(msc.observationId()(0));
    }
    if (observatory.length() == 0 || 
	!MeasTable::Observatory(obsPos,observatory)) {
      // unknown observatory, use first antenna
      obsPos=msc.antenna().positionMeas()(0);
    }
    //////
    // Int oldDD=ddId(0); // unused/commented out below in the oldDD!=newDD if
    // Int newDD=oldDD;   // unused/commented out below in the oldDD!=newDD if
    //For now we will assume that the field is not moving very far from polynome 0
    MDirection dir =fieldCol.phaseDirMeas(fieldId);
    MFrequency::Types obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(ddCol.spectralWindowId()(ddId(0))));
    if( obsMFreqType != MFrequency::TOPO)
      throw(AipsError("No dealing with non topo data for moving source yet"));
    //cout << "nTimes " << nTimes << endl;
    //cout << " obsframe " << obsMFreqType << " reqFrame " << freqframe << endl; 
    MeasFrame frame(ep, obsPos, dir);
    // MFrequency::Convert toObs(freqframe,MFrequency::Ref(obsMFreqType, frame);
    MDoppler toObs;
    MDoppler toSource;
    setupSourceObsVelSystem(ephemtab, ms, fieldId, toSource, toObs,frame);
    Double  freqEndMax=0.0;
    Double freqStartMin=C::dbl_max;
    
    for (uInt j=0; j< nTimes; ++j){
      if(fldId(uniqIndx[j]) ==fieldId){
	timeCol.get(uniqIndx[j], ep);
	// newDD=ddId(uniqIndx[j]);  // unused below
	/*if(oldDD != newDD){
	  oldDD=newDD;
	  if(spwCol.measFreqRef()(ddCol.spectralWindowId()(newDD)) != obsMFreqType){
	    obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(ddCol.spectralWindowId()(newDD)));
	    toObs.setOut(MFrequency::Ref(obsMFreqType, frame));
	  }
	  }
	*/
	//if(obsMFreqType != freqframe){
	  frame.resetEpoch(ep);
	  Vector<Double> freqTmp(2);
	  freqTmp[0]=freqStart;
	  freqTmp[1]=freqEnd;
	  Vector<Double> newFreqs=toObs.shiftFrequency(freqTmp);
	  //Double freqTmp=toObs(Quantity(freqStart, "Hz")).get("Hz").getValue();
	  freqStartMin=(freqStartMin > newFreqs[0]) ? newFreqs[0] : freqStartMin;
	  //freqTmp=toObs(Quantity(freqEnd, "Hz")).get("Hz").getValue();
	  freqEndMax=(freqEndMax < newFreqs[1]) ? newFreqs[1] : freqEndMax; 
	  //}
      }
    }

    //cout << "freqStartMin " << freqStartMin << " freqEndMax " << freqEndMax << endl;
    MSSpwIndex spwIn(ms.spectralWindow());
    spwIn.matchFrequencyRange(freqStartMin-0.5*freqStep, freqEndMax+0.5*freqStep, spw, start, nchan);


 
  }
  void MSUtil:: setupSourceObsVelSystem(const String& ephemTable, const MeasurementSet& ms,   const Int& fieldid, MDoppler& toSource, MDoppler& toObs, MeasFrame& mFrame){
    String ephemtab("");
    const MSColumns mscol(ms);
    if(Table::isReadable(ephemTable)){
      ephemtab=ephemTable;
    }
    else if(ephemTable=="TRACKFIELD"){
      ephemtab=(mscol.field()).ephemPath(fieldid);
      
    }
    MRadialVelocity::Types refvel=MRadialVelocity::GEO;
    MEpoch ep(mFrame.epoch());
    if(ephemtab != ""){

      MeasComet mcomet(Path(ephemtab).absoluteName());
      if(mFrame.comet())
	mFrame.resetComet(mcomet);
      else
	mFrame.set(mcomet);
      if(mcomet.getTopo().getLength("km").getValue() > 1.0e-3){
	refvel=MRadialVelocity::TOPO;
      }
      ////Will use UT for now for ephem tables as it is not clear that they are being
      ///filled with TDB as intended in MeasComet.h
      MEpoch::Convert toUT(ep, MEpoch::UT);
      MVRadialVelocity cometvel;
      mcomet.getRadVel(cometvel, toUT(ep).get("d").getValue());
      MRadialVelocity::Convert obsvelconv(MRadialVelocity(MVRadialVelocity(0.0),
							  MRadialVelocity::Ref(MRadialVelocity::TOPO, mFrame)),  MRadialVelocity::Ref(refvel));
      toSource=MDoppler(Quantity(-cometvel.get("km/s").getValue("km/s")+obsvelconv().get("km/s").getValue("km/s") , "km/s"), MDoppler::RELATIVISTIC);
      toObs=MDoppler(Quantity(cometvel.get("km/s").getValue("km/s")-obsvelconv().get("km/s").getValue("km/s") , "km/s"), MDoppler::RELATIVISTIC);
      
					  
    }
    else{//Must be a DE-200 canned source that measures know
      ephemtab=upcase(ephemTable);
      MeasTable::Types mtype=MeasTable::BARYEARTH;
      MDirection::Types planettype;
      if(!MDirection::getType(planettype, ephemtab))
	throw(AipsError("Did not understand sourcename as a known solar system object"));
      switch(planettype){
      case MDirection::MERCURY :
	mtype=MeasTable::MERCURY;
	break;
      case MDirection::VENUS :
	mtype=MeasTable::VENUS;
	break;	
      case MDirection::MARS :
	mtype=MeasTable::MARS;
	break;
      case MDirection::JUPITER :
	mtype=MeasTable::JUPITER;
	break;
      case MDirection::SATURN :
	mtype=MeasTable::SATURN;
	break;
      case MDirection::URANUS :
	mtype=MeasTable::URANUS;
	break;
      case MDirection::NEPTUNE :
	mtype=MeasTable::NEPTUNE;
	break;
      case MDirection::PLUTO :
	mtype=MeasTable::PLUTO;
	break;
      case MDirection::MOON :
	mtype=MeasTable::MOON;
	break;
      case MDirection::SUN :
	mtype=MeasTable::SUN;
	break;
      default:
	throw(AipsError("Cannot translate to known major solar system object"));
      }

      Vector<Double> planetparam;
       Vector<Double> earthparam;
       MEpoch::Convert toTDB(ep, MEpoch::TDB);
       earthparam=MeasTable::Planetary(MeasTable::EARTH, toTDB(ep).get("d").getValue());
       planetparam=MeasTable::Planetary(mtype, toTDB(ep).get("d").getValue());
       //GEOcentric param
       planetparam=planetparam-earthparam;
       Vector<Double> unitdirvec(3);
       Double dist=sqrt(planetparam(0)*planetparam(0)+planetparam(1)*planetparam(1)+planetparam(2)*planetparam(2));
       unitdirvec(0)=planetparam(0)/dist;
       unitdirvec(1)=planetparam(1)/dist;
       unitdirvec(2)=planetparam(2)/dist;
       MRadialVelocity::Convert obsvelconv(MRadialVelocity(MVRadialVelocity(0.0),
							  MRadialVelocity::Ref(MRadialVelocity::TOPO, mFrame)),  MRadialVelocity::Ref(refvel));
       Quantity planetradvel(planetparam(3)*unitdirvec(0)+planetparam(4)*unitdirvec(1)+planetparam(5)*unitdirvec(2), "AU/d");
	toSource=MDoppler(Quantity(-planetradvel.getValue("km/s")+obsvelconv().get("km/s").getValue("km/s") , "km/s"), MDoppler::RELATIVISTIC);
	toObs=MDoppler(Quantity(planetradvel.getValue("km/s")-obsvelconv().get("km/s").getValue("km/s") , "km/s"), MDoppler::RELATIVISTIC);
    }
  }

  
  void MSUtil::getSpwInFreqRangeAllFields(Vector<Int>& outspw, Vector<Int>& outstart,
  			  Vector<Int>& outnchan,
  			  const MeasurementSet& ms,
  			  const Double freqStart,
  			  const Double freqEnd,
  			  const Double freqStep,
  			  const MFrequency::Types freqframe){
	  Vector<Int> fldId;
	  ScalarColumn<Int> (ms, MS::columnName (MS::FIELD_ID)).getColumn (fldId);
	  const Int option = Sort::HeapSort | Sort::NoDuplicates;
	  const Sort::Order order = Sort::Ascending;

	  Int nfields = GenSort<Int>::sort (fldId, order, option);

	  fldId.resize(nfields, true);
	  outspw.resize();
	  outstart.resize();
	  outnchan.resize();
	  for (Int k=0; k < nfields; ++k){
		  Vector<Int> locspw, locstart, locnchan;
		  MSUtil::getSpwInFreqRange(locspw, locstart, locnchan, ms, freqStart, freqEnd, freqStep, freqframe, fldId[k]);
		  for (Int j=0; j< locspw.shape()(0); ++j ){
			  Bool hasthisspw=false;
			  for (Int i=0; i< outspw.shape()(0); ++i){
				  if(outspw[i]==locspw[j]){
					  hasthisspw=true;
					  if(locstart[j] < outstart[i]){
						  Int endchan=outstart[i]+outnchan[i]-1;
						  outstart[i]=locstart[j];
						  if(endchan < (locstart[j]+ locnchan[j]-1))
							  endchan=locstart[j]+ locnchan[j]-1;
						  outnchan[i]=endchan+outstart[i]+1;
					  }
				  }
			  }
			  if(!hasthisspw){
			    uInt nout=outspw.nelements();
				  outspw.resize(nout+1, true);
				  outnchan.resize(nout+1, true);
				  outstart.resize(nout+1, true);
				  outspw[nout]=locspw[j];
				  outstart[nout]=locstart[j];
				  outnchan[nout]=locnchan[j];
				 


			  }
		  }

	  }


  }
  void MSUtil::getChannelRangeFromFreqRange(Int& start,
				  Int& nchan,
				  const MeasurementSet& ms,
				  const Int spw,
				  const Double freqStart,
				  const Double freqEnd,
				  const Double freqStep,
			    const MFrequency::Types freqframe)
  {
    start=-1;
    nchan=-1;
    MSFieldColumns fieldCol(ms.field());
    MSDataDescColumns ddCol(ms.dataDescription());
	Vector<Int> dataDescSel=MSDataDescIndex(ms.dataDescription()).matchSpwId(spw);
	//cerr << "dataDescSel " << dataDescSel << endl;
	if(dataDescSel.nelements()==0)
		return;
	Vector<Double> t;
    ScalarColumn<Double> (ms,MS::columnName(MS::TIME)).getColumn(t);
	Vector<Int> ddId;
    ScalarColumn<Int> (ms,MS::columnName(MS::DATA_DESC_ID)).getColumn(ddId);
    MSSpWindowColumns spwCol(ms.spectralWindow());
    Vector<Double> ddIdD(ddId.shape());
    convertArray(ddIdD, ddId);
    ddIdD+= 1.0; //no zero id
     //we have to do this as one can have multiple dd for the same time. 
    t*=ddIdD;
    //t(fldId != fieldId)=-1.0;
    Vector<Double> elt;
    Vector<Int> elindx;
    //rejecting the large blocks of same time for all baselines
    //this speeds up by a lot GenSort::sort
    rejectConsecutive(t, elt, elindx);
    
    ROScalarMeasColumn<MEpoch> timeCol(ms, MS::columnName(MS::TIME));
    Vector<uInt>  uniqIndx;
    uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, elt, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);

    t.resize(0);
    
    //ScalarColumn<Int> ddId(ms,MS::columnName(MS::DATA_DESC_ID));
    ScalarColumn<Int> fldId(ms,MS::columnName(MS::FIELD_ID));
    //now need to do the conversion to data frame from requested frame
    //Get the epoch mesasures of the first row
    MEpoch ep;
    timeCol.get(0, ep);
    String observatory;
    MPosition obsPos;
    /////observatory position
    MSColumns msc(ms);
    if (ms.observation().nrow() > 0) {
      observatory = msc.observation().telescopeName()(msc.observationId()(0));
    }
    if (observatory.length() == 0 || 
	!MeasTable::Observatory(obsPos,observatory)) {
      // unknown observatory, use first antenna
      obsPos=msc.antenna().positionMeas()(0);
    }
    //////
    Int oldDD=dataDescSel(0);
    Int newDD=oldDD;
    //For now we will assume that the field is not moving very far from polynome 0
    MDirection dir =fieldCol.phaseDirMeas(0);
    MFrequency::Types obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(ddCol.spectralWindowId()(dataDescSel(0))));
    MeasFrame frame(ep, obsPos, dir);
    MFrequency::Convert toObs(freqframe,
                              MFrequency::Ref(obsMFreqType, frame));
    Double freqEndMax=freqEnd+0.5*fabs(freqStep);
    Double freqStartMin=freqStart-0.5*fabs(freqStep);
    if(freqframe != obsMFreqType){
      freqEndMax=0.0;
      freqStartMin=C::dbl_max;
    }
    for (uInt j=0; j< nTimes; ++j) {
        if(anyEQ(dataDescSel, ddId(elindx[uniqIndx[j]]))){
            timeCol.get(elindx[uniqIndx[j]], ep);
            newDD=ddId(elindx[uniqIndx[j]]);
            if(oldDD != newDD) {
				
                oldDD=newDD;
                if(spwCol.measFreqRef()(ddCol.spectralWindowId()(newDD)) != obsMFreqType) {
                    obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(ddCol.spectralWindowId()(newDD)));
                    toObs.setOut(MFrequency::Ref(obsMFreqType, frame));
                }
            }
            if(obsMFreqType != freqframe) {
                dir=fieldCol.phaseDirMeas(fldId(elindx[uniqIndx[j]]));
                frame.resetEpoch(ep);
                frame.resetDirection(dir);
                Double freqTmp=toObs(Quantity(freqStart, "Hz")).get("Hz").getValue();
                freqStartMin=(freqStartMin > freqTmp) ? freqTmp : freqStartMin;
                freqTmp=toObs(Quantity(freqEnd, "Hz")).get("Hz").getValue();
                freqEndMax=(freqEndMax < freqTmp) ? freqTmp : freqEndMax;
            }
        }
    }

    
    MSSpwIndex spwIn(ms.spectralWindow());
	Vector<Int> spws;
	Vector<Int> starts;
	Vector<Int> nchans;
    if(!spwIn.matchFrequencyRange(freqStartMin-0.5*freqStep, freqEndMax+0.5*freqStep, spws, starts, nchans)){
			return;
	}
	for (uInt k=0; k < spws.nelements(); ++k){
			if(spws[k]==spw){
					start=starts[k];
					nchan=nchans[k];
			}
	
	}
  }
  Bool MSUtil::getFreqRangeInSpw( Double& freqStart,
				  Double& freqEnd, const Vector<Int>& spw, const Vector<Int>& start,
				  const Vector<Int>& nchan,
				  const MeasurementSet& ms, 
				  const MFrequency::Types freqframe,
				  const Int fieldId, const Bool fromEdge){
    Vector<Int> fields(1, fieldId);
    return MSUtil::getFreqRangeInSpw( freqStart, freqEnd, spw, start,
				      nchan,ms, freqframe,fields, fromEdge);


  }
  Bool MSUtil::getFreqRangeInSpw( Double& freqStart,
				  Double& freqEnd, const Vector<Int>& spw, const Vector<Int>& start,
				  const Vector<Int>& nchan,
				  const MeasurementSet& ms, 
				  const MFrequency::Types freqframe,
				  const Bool fromEdge){
    Vector<Int> fields(0);
    return MSUtil::getFreqRangeInSpw( freqStart, freqEnd, spw, start,
				      nchan,ms, freqframe,fields, fromEdge, True);


  }
  Bool MSUtil::getFreqRangeInSpw( Double& freqStart,
				  Double& freqEnd, const Vector<Int>& spw, const Vector<Int>& start,
				  const Vector<Int>& nchan,
				  const MeasurementSet& ms, 
				  const MFrequency::Types freqframe,
				  const Vector<Int>&  fieldIds, const Bool fromEdge, const Bool useFieldsInMS){
    
    Bool retval=False;
    freqStart=C::dbl_max;
    freqEnd=0.0;
    Vector<Double> t;
    ScalarColumn<Double> (ms,MS::columnName(MS::TIME)).getColumn(t);
    Vector<Int> ddId;
    Vector<Int> fldId;
    ScalarColumn<Int> (ms,MS::columnName(MS::DATA_DESC_ID)).getColumn(ddId);
    ScalarColumn<Int> (ms,MS::columnName(MS::FIELD_ID)).getColumn(fldId);
    MSFieldColumns fieldCol(ms.field());
    MSDataDescColumns ddCol(ms.dataDescription());
    MSSpWindowColumns spwCol(ms.spectralWindow());
    ROScalarMeasColumn<MEpoch> timeCol(ms, MS::columnName(MS::TIME));
    Vector<Double> ddIdD(ddId.shape());
    convertArray(ddIdD, ddId);
    ddIdD+= 1.0; //no zero id
    //we have to do this as one can have multiple dd for the same time. 
    t*=ddIdD;
    //t(fldId != fieldId)=-1.0;
    Vector<Double> elt;
    Vector<Int> elindx;
    //rejecting the large blocks of same time for all baselines
    //this speeds up by a lot GenSort::sort
    rejectConsecutive(t, elt, elindx);
    Vector<uInt>  uniqIndx;
    
    uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, elt, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
    
    MDirection dir;
    if(useFieldsInMS)
      dir=fieldCol.phaseDirMeas(fldId[0]);
    else
      dir=fieldCol.phaseDirMeas(fieldIds[0]);
    MSDataDescIndex mddin(ms.dataDescription());
    MFrequency::Types obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(0));
    MEpoch ep;
    timeCol.get(0, ep);
    String observatory;
    MPosition obsPos;
    /////observatory position
    MSColumns msc(ms);
    if (ms.observation().nrow() > 0) {
      observatory = msc.observation().telescopeName()(msc.observationId()(0));
    }
    if (observatory.length() == 0 || 
	!MeasTable::Observatory(obsPos,observatory)) {
      // unknown observatory, use first antenna
      obsPos=msc.antenna().positionMeas()(0);
    }
    //////
    MeasFrame frame(ep, obsPos, dir);
    
						
    for (uInt ispw =0 ; ispw < spw.nelements() ; ++ispw){
		if(nchan[ispw]>0 && start[ispw] >-1){
      Double freqStartObs=C::dbl_max;
      Double freqEndObs=0.0;
      Vector<Double> chanfreq=spwCol.chanFreq()(spw[ispw]);
      Vector<Double> chanwid=spwCol.chanWidth()(spw[ispw]);
      Vector<Int> ddOfSpw=mddin.matchSpwId(spw[ispw]);
      for (Int ichan=start[ispw]; ichan<start[ispw]+nchan[ispw]; ++ichan){ 
		  if(fromEdge){
			if(freqStartObs > (chanfreq[ichan]-fabs(chanwid[ichan])/2.0)) freqStartObs=chanfreq[ichan]-fabs(chanwid[ichan])/2.0;
			if(freqEndObs < (chanfreq[ichan]+fabs(chanwid[ichan])/2.0)) freqEndObs=chanfreq[ichan]+fabs(chanwid[ichan])/2.0;   
		  }
		  else{
			if(freqStartObs > (chanfreq[ichan])) freqStartObs=chanfreq[ichan];
			if(freqEndObs < (chanfreq[ichan])) freqEndObs=chanfreq[ichan];   
		  }
      }
      obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(spw[ispw]));
      if((obsMFreqType==MFrequency::REST) || (obsMFreqType==freqframe && obsMFreqType != MFrequency::TOPO)){
	if(freqStart > freqStartObs)  freqStart=freqStartObs;
	if(freqEnd < freqStartObs)  freqEnd=freqStartObs;
	if(freqStart > freqEndObs)  freqStart=freqEndObs;
	if(freqEnd < freqEndObs)  freqEnd=freqEndObs;
	retval=True;
      }
      else{
	MFrequency::Convert toframe(obsMFreqType,
				    MFrequency::Ref(freqframe, frame));
	for (uInt j=0; j< nTimes; ++j){
	  if((useFieldsInMS || anyEQ(fieldIds, fldId[elindx[uniqIndx[j]]])) && anyEQ(ddOfSpw, ddId[elindx[uniqIndx[j]]])){
	    timeCol.get(elindx[uniqIndx[j]], ep);
	    dir=fieldCol.phaseDirMeas(fldId[elindx[uniqIndx[j]]]);
	    frame.resetEpoch(ep);
	    frame.resetDirection(dir);
	    Double freqTmp=toframe(Quantity(freqStartObs, "Hz")).get("Hz").getValue();
	    if(freqStart > freqTmp)  freqStart=freqTmp;
	    if(freqEnd < freqTmp)  freqEnd=freqTmp;
	    freqTmp=toframe(Quantity(freqEndObs, "Hz")).get("Hz").getValue();
	    if(freqStart > freqTmp)  freqStart=freqTmp;
	    if(freqEnd < freqTmp)  freqEnd=freqTmp;
	    retval=True;
	  }
	}
      }
		}
    }
    return retval;
  }


  Bool MSUtil::getFreqRangeAndRefFreqShift( Double& freqStart,
					    Double& freqEnd, Quantity& sysvel, const MEpoch& refEp, const Vector<Int>& spw, const Vector<Int>& start,
				  const Vector<Int>& nchan,
				  const MeasurementSet& ms, 
				  const String& ephemPath,   const MDirection& trackDir, 
				  const Bool fromEdge){

    casacore::MDirection::Types planetType=MDirection::castType(trackDir.getRef().getType());
    if( (! Table::isReadable(ephemPath)) &&   ( (planetType <= MDirection::N_Types) || (planetType >= MDirection::COMET)))
      throw(AipsError("getFreqRange in SOURCE frame has to have a valid ephemeris table or major solar system object defined"));
    Bool isephem=False;
    MeasComet mcomet;
    MeasTable::Types mtype=MeasTable::BARYEARTH;
    if(Table::isReadable(ephemPath)){
      mcomet=MeasComet(Path(ephemPath).absoluteName());
      isephem=True;
    }
    else{
      
      if(planetType >=MDirection::MERCURY && planetType < MDirection::COMET){
	//Damn these enums are not in the same order
	switch(planetType){
	case MDirection::MERCURY :
	  mtype=MeasTable::MERCURY;
	  break;
	case MDirection::VENUS :
	  mtype=MeasTable::VENUS;
	  break;	
	case MDirection::MARS :
	  mtype=MeasTable::MARS;
	  break;
	case MDirection::JUPITER :
	  mtype=MeasTable::JUPITER;
	  break;
	case MDirection::SATURN :
	  mtype=MeasTable::SATURN;
	  break;
	case MDirection::URANUS :
	  mtype=MeasTable::URANUS;
	  break;
	case MDirection::NEPTUNE :
	  mtype=MeasTable::NEPTUNE;
	  break;
	case MDirection::PLUTO :
	  mtype=MeasTable::PLUTO;
	  break;
	case MDirection::MOON :
	  mtype=MeasTable::MOON;
	  break;
	case MDirection::SUN :
	  mtype=MeasTable::SUN;
	  break;
	default:
	  throw(AipsError("Cannot translate to known major solar system object"));
	}
      }

    }


    Vector<Double> planetparam;
    Vector<Double> earthparam;
    

  


    
    Bool retval=False;
    freqStart=C::dbl_max;
    freqEnd=0.0;
    Vector<Double> t;
    ScalarColumn<Double> (ms,MS::columnName(MS::TIME)).getColumn(t);
    Vector<Int> ddId;
    Vector<Int> fldId;
    ScalarColumn<Int> (ms,MS::columnName(MS::DATA_DESC_ID)).getColumn(ddId);
    ScalarColumn<Int> (ms,MS::columnName(MS::FIELD_ID)).getColumn(fldId);
    MSFieldColumns fieldCol(ms.field());
    MSDataDescColumns ddCol(ms.dataDescription());
    MSSpWindowColumns spwCol(ms.spectralWindow());
    ROScalarMeasColumn<MEpoch> timeCol(ms, MS::columnName(MS::TIME));
    Vector<Double> ddIdD(ddId.shape());
    convertArray(ddIdD, ddId);
    ddIdD+= 1.0; //no zero id
    //we have to do this as one can have multiple dd for the same time. 
    t*=ddIdD;
    //t(fldId != fieldId)=-1.0;
    Vector<Double> elt;
    Vector<Int> elindx;
    //rejecting the large blocks of same time for all baselines
    //this speeds up by a lot GenSort::sort
    rejectConsecutive(t, elt, elindx);
    Vector<uInt>  uniqIndx;
    
    uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, elt, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
    
    MDirection dir;
    dir=fieldCol.phaseDirMeas(fldId[0]);
    MSDataDescIndex mddin(ms.dataDescription());
    MEpoch ep;
    timeCol.get(0, ep);
    String observatory;
    MPosition obsPos;
    /////observatory position
    MSColumns msc(ms);
    if (ms.observation().nrow() > 0) {
      observatory = msc.observation().telescopeName()(msc.observationId()(0));
    }
    if (observatory.length() == 0 || 
	!MeasTable::Observatory(obsPos,observatory)) {
      // unknown observatory, use first antenna
      obsPos=msc.antenna().positionMeas()(0);
    }
    //////
    //cerr << "obspos " << obsPos << endl;
    MeasFrame mframe(ep, obsPos, dir);
    ////Will use UT for now for ephem tables as it is not clear that they are being
    ///filled with TDB as intended in MeasComet.h 
    MEpoch::Convert toUT(ep, MEpoch::UT);
    MEpoch::Convert toTDB(ep, MEpoch::TDB);
    MRadialVelocity::Types refvel=MRadialVelocity::GEO;
    if(isephem){
      //cerr << "dist " << mcomet.getTopo().getLength("km") << endl;
      if(mcomet.getTopo().getLength("km").getValue() > 1.0e-3){
	refvel=MRadialVelocity::TOPO;
      }
    }
    MRadialVelocity::Convert obsvelconv (MRadialVelocity(MVRadialVelocity(0.0),
					       MRadialVelocity::Ref(MRadialVelocity::TOPO, mframe)),
					 MRadialVelocity::Ref(refvel));
    MVRadialVelocity cometvel;
    
    for (uInt ispw =0 ; ispw < spw.nelements() ; ++ispw){
      if(nchan[ispw]>0 && start[ispw] >-1){
	Double freqStartObs=C::dbl_max;
	Double freqEndObs=0.0;
	Vector<Double> chanfreq=spwCol.chanFreq()(spw[ispw]);
	Vector<Double> chanwid=spwCol.chanWidth()(spw[ispw]);
	Vector<Int> ddOfSpw=mddin.matchSpwId(spw[ispw]);
	for (Int ichan=start[ispw]; ichan<start[ispw]+nchan[ispw]; ++ichan){ 
	  if(fromEdge){
	    if(freqStartObs > (chanfreq[ichan]-fabs(chanwid[ichan])/2.0)) freqStartObs=chanfreq[ichan]-fabs(chanwid[ichan])/2.0;
	    if(freqEndObs < (chanfreq[ichan]+fabs(chanwid[ichan])/2.0)) freqEndObs=chanfreq[ichan]+fabs(chanwid[ichan])/2.0;   
	  }
	  else{
	    if(freqStartObs > (chanfreq[ichan])) freqStartObs=chanfreq[ichan];
	    if(freqEndObs < (chanfreq[ichan])) freqEndObs=chanfreq[ichan];   
	  }
	}
      
      MFrequency::Types obsMFreqType= (MFrequency::Types) (spwCol.measFreqRef()(spw[ispw]));
      if(obsMFreqType==MFrequency::REST)
	throw(AipsError("cannot do Source frame conversion from REST"));
      MFrequency::Convert toTOPO(obsMFreqType,
				 MFrequency::Ref(MFrequency::TOPO, mframe));
      Double diffepoch=1e37;
      sysvel=Quantity(0.0,"m/s");
	
      for (uInt j=0; j< nTimes; ++j){
	  if(anyEQ(ddOfSpw, ddId[elindx[uniqIndx[j]]])){
	    timeCol.get(elindx[uniqIndx[j]], ep);
	    dir=fieldCol.phaseDirMeas(fldId[elindx[uniqIndx[j]]], ep.get("s").getValue());
	    mframe.resetEpoch(ep);
	    mframe.resetDirection(dir);
	    if(obsMFreqType != MFrequency::TOPO){
	      freqStartObs=toTOPO(Quantity(freqStartObs, "Hz")).get("Hz").getValue();
	      freqEndObs=toTOPO(Quantity(freqEndObs, "Hz")).get("Hz").getValue();
	    }
	    MDoppler mdop;
	    if(isephem){
	      mcomet.getRadVel(cometvel, toUT(ep).get("d").getValue());
	      mdop=MDoppler(Quantity(-cometvel.get("km/s").getValue("km/s")+obsvelconv().get("km/s").getValue("km/s") , "km/s"), MDoppler::RELATIVISTIC);
	      //	      cerr << std::setprecision(10) <<  toUT(ep).get("d").getValue() << " fieldid " << fldId[elindx[uniqIndx[j]]] << " cometvel " << cometvel.get("km/s").getValue("km/s") << " obsvel " << obsvelconv().get("km/s").getValue("km/s") << endl;
	    }
	    else{
	      earthparam=MeasTable::Planetary(MeasTable::EARTH, toTDB(ep).get("d").getValue());
	      planetparam=MeasTable::Planetary(mtype, toTDB(ep).get("d").getValue());
	      //GEOcentric param
	      planetparam=planetparam-earthparam;
	      Vector<Double> unitdirvec(3);
	      Double dist=sqrt(planetparam(0)*planetparam(0)+planetparam(1)*planetparam(1)+planetparam(2)*planetparam(2));
	      unitdirvec(0)=planetparam(0)/dist;
	      unitdirvec(1)=planetparam(1)/dist;
	      unitdirvec(2)=planetparam(2)/dist;
	      Quantity planetradvel(planetparam(3)*unitdirvec(0)+planetparam(4)*unitdirvec(1)+planetparam(5)*unitdirvec(2), "AU/d");
	      mdop=MDoppler(Quantity(-planetradvel.getValue("km/s")+obsvelconv().get("km/s").getValue("km/s") , "km/s"), MDoppler::RELATIVISTIC);

	    }
	    Vector<Double> range(2); range(0)=freqStartObs; range(1)=freqEndObs;
	    range=mdop.shiftFrequency(range);
	    if(diffepoch > fabs(ep.get("s").getValue("s")-refEp.get("s").getValue("s"))){
	      diffepoch= fabs(ep.get("s").getValue("s")-refEp.get("s").getValue("s"));
	      sysvel=mdop.get("km/s");
	      //cerr << std::setprecision(10) << "shifts " << range(0)-freqStartObs << "   " <<  range(1)-freqEndObs << endl;
	    }
	      
	    
	    if(freqStart > range[0])  freqStart=range[0];
	    if(freqEnd < range[0])  freqEnd=range[0];
	
	    if(freqStart > range[1])  freqStart=range[1];
	    if(freqEnd < range[1])  freqEnd=range[1];
	    retval=True;
	  }
      }
      }

    }
    return retval;
  }
  void MSUtil::rejectConsecutive(const Vector<Double>& t, Vector<Double>& retval, Vector<Int>& indx){
    uInt n=t.nelements();
    if(n >0){
      retval.resize(n);
      indx.resize(n);
      retval[0]=t[0];
      indx[0]=0;
    }
    else
      return;
    Int prev=0;
    for (uInt k=1; k < n; ++k){ 
      if(t[k] != retval(prev)){
	++prev;
	//retval.resize(prev+1, true);
	retval[prev]=t[k];
	//indx.resize(prev+1, true);
	indx[prev]=k;
      }
    }
    retval.resize(prev+1, true);
    indx.resize(prev+1, true);
    
  }

  Vector<String> MSUtil::getSpectralFrames(Vector<MFrequency::Types>& types, const MeasurementSet& ms)
  {
	  Vector<String> retval;
	  Vector<Int> typesAsInt=MSSpWindowColumns(ms.spectralWindow()).measFreqRef().getColumn();
	  if(ms.nrow()==Table(ms.getPartNames()).nrow()){
		  types.resize(typesAsInt.nelements());
		  for (uInt k=0; k < types.nelements(); ++k)
			  types[k]=MFrequency::castType(typesAsInt[k]);
  	  }
	  else{
		  Vector<Int> ddId;
		  ScalarColumn<Int> (ms,MS::columnName(MS::DATA_DESC_ID)).getColumn(ddId);
		  Vector<uInt>  uniqIndx;
		  uInt nTimes=GenSort<Int>::sort (ddId, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
		  ddId.resize(nTimes, true);
		  Vector<Int> spwids(nTimes);
		  Vector<Int> spwInDD=MSDataDescColumns(ms.dataDescription()).spectralWindowId().getColumn();
		  for (uInt k=0; k < nTimes; ++k)
			  spwids[k]=spwInDD[ddId[k]];

		  nTimes=GenSort<Int>::sort (spwids, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
		  spwids.resize(nTimes, true);
		  types.resize(nTimes);
		  for(uInt k=0; k <nTimes; ++k)
			  types[k]=MFrequency::castType(typesAsInt[spwids[k]]);
	  }

	  retval.resize(types.nelements());
	  for (uInt k=0; k < types.nelements(); ++k){
		retval[k]=MFrequency::showType(types[k]);
	  }


	  return retval;

  }
  void MSUtil::getIndexCombination(const MSColumns& mscol, Matrix<Int>& retval2){
    Vector<Vector<Int> >retval;
    Vector<Int> state = mscol.stateId().getColumn();
    Vector<Int> scan=mscol.scanNumber().getColumn();
    Vector<Double> t=mscol.time().getColumn();
    Vector<Int> fldid=mscol.fieldId().getColumn();
    Vector<Int> ddId=mscol.dataDescId().getColumn();
    Vector<Int> spwid=mscol.dataDescription().spectralWindowId().getColumn();
    Vector<uInt>  uniqIndx;
    {
      Vector<Double> ddIdD(ddId.shape());
      convertArray(ddIdD, ddId);
      ddIdD+= 1.0; //no zero id
      //we have to do this as one can have multiple dd for the same time. 
      t*=ddIdD;
    }
    uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, t, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
	   Vector<Int> comb(4);
	   
	   for (uInt k=0; k < nTimes; ++k){
	     comb(0)=fldid[uniqIndx[k]];
	     comb(1)=spwid[ddId[uniqIndx[k]]];
	     comb(2)=scan(uniqIndx[k]);
	     comb(3)=state(uniqIndx[k]);
	     Bool hasComb=False;
	     if(retval.nelements() >0){
	       for (uInt j=0; j < retval.nelements(); ++j){
		 if(allEQ(retval[j], comb))
		   hasComb=True;
	       }
	       
	     }
	     if(!hasComb){
	       retval.resize(retval.nelements()+1, True);
	       retval[retval.nelements()-1]=comb;
	     }
	   }
	   retval2.resize(retval.nelements(),4);
	   for (uInt j=0; j < retval.nelements(); ++j)
	     retval2.row(j)=retval[j];

  }


} //# NAMESPACE CASA - END
