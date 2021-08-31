//# SynthesisImagerVi2.cc: Implementation of SynthesisImager.h
//# Copyright (C) 1997-2019
//# Associated Universities, Inc. Washington DC, USA.
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//# License for more details.
//#
//# https://www.gnu.org/licenses/
//#
//# You should have received a copy of the GNU  General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Queries concerning CASA should be submitted at
//#        https://help.nrao.edu
//#
//#        Postal address: CASA Project Manager 
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#define CFC_VERBOSE false /* Control the verbosity when building CFCache. */

#include <casa/Exceptions/Error.h>
#include <casa/iostream.h>
#include <casa/sstream.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>


#include <casa/Logging.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>
#include <casa/System/ProgressMeter.h>

#include <casa/OS/DirectoryIterator.h>
#include <casa/OS/File.h>
#include <casa/OS/HostInfo.h>
#include <casa/OS/Path.h>
//#include <casa/OS/Memory.h>

#include <lattices/LRegions/LCBox.h>

#include <measures/Measures/MeasTable.h>

#include <ms/MeasurementSets/MSHistoryHandler.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MSSel/MSSelection.h>


#if ! defined(CASATOOLS)
#include <synthesis/ImagerObjects/SIIterBot.h>
#endif
#include <synthesis/ImagerObjects/SynthesisImagerVi2.h>

#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <synthesis/ImagerObjects/SIImageStore.h>
#include <synthesis/ImagerObjects/SIImageStoreMultiTerm.h>
#include <synthesis/ImagerObjects/CubeMajorCycleAlgorithm.h>
#include <synthesis/ImagerObjects/CubeMakeImageAlgorithm.h>
#include <synthesis/MeasurementEquations/VPManager.h>
#include <imageanalysis/Utilities/SpectralImageUtil.h>
#include <msvis/MSVis/MSUtil.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <msvis/MSVis/VisImagingWeight.h>

#include <synthesis/TransformMachines2/GridFT.h>
#include <synthesis/TransformMachines2/WPConvFunc.h>
#include <synthesis/TransformMachines2/WProjectFT.h>
#include <synthesis/TransformMachines2/VisModelData.h>
#include <synthesis/TransformMachines2/AWProjectFT.h>
#include <synthesis/TransformMachines2/HetArrayConvFunc.h>
#include <synthesis/TransformMachines2/MosaicFTNew.h>
#include <synthesis/TransformMachines2/MultiTermFTNew.h>
#include <synthesis/TransformMachines2/AWProjectWBFTNew.h>
#include <synthesis/TransformMachines2/AWConvFunc.h>
//#include <synthesis/TransformMachines2/AWConvFuncEPJones.h>
#include <synthesis/TransformMachines2/NoOpATerm.h>
#include <synthesis/TransformMachines2/SDGrid.h>
#include <synthesis/TransformMachines/WProjectFT.h>
#include <synthesis/TransformMachines2/BriggsCubeWeightor.h>
#if ! defined(CASATOOLS)
#include <casadbus/viewer/ViewerProxy.h>
#include <casadbus/plotserver/PlotServerProxy.h>
#endif
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/OS/Directory.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>
//#include <casadbus/utilities/BusAccess.h>
//#include <casadbus/session/DBusSession.h>

#include <sys/types.h>
#include <unistd.h>
#include <iomanip>
#include <synthesis/Parallel/Applicator.h>

using namespace std;

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN
  extern Applicator applicator;
  SynthesisImagerVi2::SynthesisImagerVi2() : SynthesisImager(), vi_p(0), fselections_p(nullptr), imparsVec_p(0), gridparsVec_p(0) {
	/*cerr << "is applicator initialized " << applicator.initialized() << endl;
	if(!applicator.initialized()){
	  int argc=1;
        char **argv;
        casa::applicator.init ( argc, argv );
        cerr << "controller ?" <<  applicator.isController() <<  " worker? " <<  applicator.isWorker() <<  " numprocs " << applicator.numProcs() <<  endl;
	}
	*/
    mss_p.resize(0);
  }

  SynthesisImagerVi2::~SynthesisImagerVi2(){
    for (uInt k=0; k < mss_p.nelements(); ++k){
      if(mss_p[k])
	delete mss_p[k];
    }
      SynthesisUtilMethods::getResource("End Run");
}

  Bool SynthesisImagerVi2::selectData(const SynthesisParamsSelect& selpars){
 LogIO os( LogOrigin("SynthesisImagerVi2","selectData",WHERE) );
 Bool retval=True;

    SynthesisUtilMethods::getResource("Start Run");
    
    try
      {


	MeasurementSet thisms;
	{ ///Table system seems to have a bug when running in multi-process as the source table disappears
	  /// temporarily when other processes are updating it 
	  uInt exceptCounter=0;
	  
	  while(true){
	    try{
	      //Respect the readonly flag...necessary for multi-process access
	      thisms=MeasurementSet(selpars.msname, TableLock(TableLock::UserNoReadLocking),
				    selpars.readonly ? Table::Old : Table::Update);
	      break;
	    }
	    catch(AipsError &x){
	      
	      String mes=x.getMesg();
	      if(mes.contains("FilebufIO::readBlock") || mes.contains("SOURCE")){
		sleep(0.05);
		os << LogIO::WARN << "#####CATCHING a sleep because "<< mes<< LogIO::POST;
	      }
	      else
		throw(AipsError("Error in selectdata: "+mes));
	      
	      if(exceptCounter > 100){
		throw(AipsError("Error in selectdata got 100 of this exeception: "+mes));
		
	      }
	      
	    }
	    ++exceptCounter;
	  }
	}//End of work around for table disappearing bug
	

    
    useScratch_p=selpars.usescratch;
    readOnly_p = selpars.readonly;
    lockMS(thisms);	
    thisms.setMemoryResidentSubtables (MrsEligibility::defaultEligible());

    
    //    cout << "**************** usescr : " << useScratch_p << "     readonly : " << readOnly_p << endl;
    //if you want to use scratch col...make sure they are there
    ///Need to clear this in first pass only...child processes or loops for cube ///should skip it
    if(!(impars_p.mode.contains("cube")) || ((impars_p.mode.contains("cube")) && doingCubeGridding_p)){
      if(selpars.usescratch && !selpars.readonly){
	VisSetUtil::addScrCols(thisms, true, false, true, false);
	refim::VisModelData::clearModel(thisms);
      }
    ////TESTOO
    //Int CPUID;
	//MPI_Comm_rank(MPI_COMM_WORLD, &CPUID);
    //cerr  << " SELPARS " << selpars.toRecord()  << endl;
      if(!selpars.incrmodel && !selpars.usescratch && !selpars.readonly)
	refim::VisModelData::clearModel(thisms, selpars.field, selpars.spw);
    }//end of first pass if for cubes
    // unlockMSs();


    os << "MS : " << selpars.msname << " | ";

    //Some MSSelection 
    //If everything is empty (which is valid) it will throw an exception..below
    //So make sure the main defaults are not empy i.e field and spw
    MSSelection thisSelection;
    if(selpars.field != ""){
      thisSelection.setFieldExpr(selpars.field);
      os << "Selecting on fields : " << selpars.field << " | " ;//LogIO::POST;
    }else
      thisSelection.setFieldExpr("*");
    if(selpars.spw != ""){
	thisSelection.setSpwExpr(selpars.spw);
	os << "Selecting on spw :"<< selpars.spw  << " | " ;//LogIO::POST;
    }else
      thisSelection.setSpwExpr("*");
    
    if(selpars.antenna != ""){
      Vector<String> antNames(1, selpars.antenna);
      // thisSelection.setAntennaExpr(MSSelection::nameExprStr( antNames));
      thisSelection.setAntennaExpr(selpars.antenna);
      os << "Selecting on antenna names : " << selpars.antenna << " | " ;//LogIO::POST;
	
    }            
    if(selpars.timestr != ""){
	thisSelection.setTimeExpr(selpars.timestr);
	os << "Selecting on time range : " << selpars.timestr << " | " ;//LogIO::POST;	
      }
    if(selpars.uvdist != ""){
      thisSelection.setUvDistExpr(selpars.uvdist);
      os << "Selecting on uvdist : " << selpars.uvdist << " | " ;//LogIO::POST;	
    }
    if(selpars.scan != ""){
      thisSelection.setScanExpr(selpars.scan);
      os << "Selecting on scan : " << selpars.scan << " | " ;//LogIO::POST;	
    }
    if(selpars.obs != ""){
      thisSelection.setObservationExpr(selpars.obs);
      os << "Selecting on Observation Expr : " << selpars.obs << " | " ;//LogIO::POST;	
    }
    if(selpars.state != ""){
      thisSelection.setStateExpr(selpars.state);
      os << "Selecting on Scan Intent/State : " << selpars.state << " | " ;//LogIO::POST;	
    }
    // if(selpars.taql != ""){
    // 	thisSelection.setTaQLExpr(selpars.taql);
    // 	os << "Selecting via TaQL : " << selpars.taql << " | " ;//LogIO::POST;	
    // }
    os << "[Opened " << (readOnly_p?"in readonly mode":(useScratch_p?"with scratch model column":"with virtual model column"))  << "]" << LogIO::POST;
    TableExprNode exprNode=thisSelection.toTableExprNode(&thisms);
    if(!(exprNode.isNull()))
      {
	
    
	MeasurementSet thisMSSelected0 = MeasurementSet(thisms(exprNode));
	mss_p.resize(mss_p.nelements()+1, false, true);
	if(selpars.taql != "")
	  {
	    MSSelection mss0;
	    mss0.setTaQLExpr(selpars.taql);
	    os << "Selecting via TaQL : " << selpars.taql << " | " ;//LogIO::POST;	

	    TableExprNode tenWithTaQL=mss0.toTableExprNode(&thisMSSelected0);
	    MeasurementSet thisMSSelected1 = MeasurementSet(thisMSSelected0(tenWithTaQL));
	    //mss4vi_p[mss4vi_p.nelements()-1]=MeasurementSet(thisms(exprNode));
	    mss_p[mss_p.nelements()-1]=new MeasurementSet(thisMSSelected1);
	  }
	else
	  mss_p[mss_p.nelements()-1]=new MeasurementSet(thisMSSelected0);
	  
	os << "  NRows selected : " << (mss_p[mss_p.nelements()-1])->nrow() << LogIO::POST;
	//	unlockMSs();
      }
    else{
      throw(AipsError("Selection for given MS "+selpars.msname+" is invalid"));
    }
    if((mss_p[mss_p.nelements()-1])->nrow() ==0){
      delete mss_p[mss_p.nelements()-1];
      mss_p.resize(mss_p.nelements()-1, True, True);
      if(mss_p.nelements()==0)
	throw(AipsError("Data selection ended with 0 rows"));
      //Sill have some valid ms's so return false and do not proceed to do 
      //channel selection
      unlockMSs();
      return False;
    }


    
    ///Channel selection
    {
      if(!fselections_p) fselections_p=new FrequencySelections();
      Matrix<Int> chanlist = thisSelection.getChanList(mss_p[mss_p.nelements()-1]);
      Matrix<Double> freqList=thisSelection.getChanFreqList(mss_p[mss_p.nelements()-1]);
      //cerr << std::setprecision(12) << "FreqList " << freqList << endl;
      IPosition shape = freqList.shape();
      uInt nSelections = shape[0];
      ///temporary variable as we carry that for tunechunk...till we get rid of it
      selFreqFrame_p=selpars.freqframe;
      Bool ignoreframe=False;
      MFrequency::Types freqFrame=MFrequency::castType(MSColumns(*mss_p[mss_p.nelements()-1]).spectralWindow().measFreqRef()(Int(chanlist(0,0))));
  
      if(selpars.freqframe == MFrequency::REST ||selpars.freqframe == MFrequency::Undefined){	
	selFreqFrame_p=freqFrame;
	ignoreframe=True;
      }
      if(selpars.freqbeg==""){
	   // Going round the problem of CAS-8829
        /*vi::FrequencySelectionUsingChannels channelSelector;

        channelSelector.add(thisSelection, mss_p[mss_p.nelements()-1]);

        fselections_p.add(channelSelector);
        */
        ////////////////////////////
        Double lowfreq;
        Double topfreq;
      Vector<Int> fieldList=thisSelection.getFieldList(mss_p[mss_p.nelements()-1]);
	 // cerr << "chanlist " << chanlist.column(0) << "\n fieldList " << fieldList << endl;
        
	//cerr << "selpars.freqframe " << selpars.freqframe << endl;
        vi::FrequencySelectionUsingFrame channelSelector(selFreqFrame_p);
	///temporary variable as we carry that for tunechunk
		
	Bool selectionValid=False;
    	  for(uInt k=0; k < nSelections; ++k){
	    Bool thisSpwSelValid=False;
	    //The getChanfreqList is wrong for beg and end..going round that too.
	    Vector<Double> freqies=MSColumns(*mss_p[mss_p.nelements()-1]).spectralWindow().chanFreq()(Int(chanlist(k,0)));
	    Vector<Double> chanwidth=MSColumns(*mss_p[mss_p.nelements()-1]).spectralWindow().chanWidth()(Int(chanlist(k,0)));
            
	    if(freqList(k,3) < 0.0){
	      topfreq=freqies(chanlist(k,1));//-chanwidth(chanlist(k,1))/2.0;
	      lowfreq=freqies(chanlist(k,2));//+chanwidth(chanlist(k,2))/2.0;
	      //lowfreq=freqList(k,2); //+freqList(k,3)/2.0;
	      //topfreq=freqList(k, 1); //-freqList(k,3)/2.0;
	    }
	    else{
	      lowfreq=freqies(chanlist(k,1));//-chanwidth(chanlist(k,1))/2.0;
	      topfreq=freqies(chanlist(k,2));//+chanwidth(chanlist(k,2))/2.0;
	      //lowfreq=freqList(k,1);//-freqList(k,3)/2.0;
	      //topfreq=freqList(k, 2);//+freqList(k,3)/2.0;
	    }
	    
	    if(!ignoreframe){
		
			
          //cerr << "begin " << lowfreq << "  " << topfreq << endl; 
	      //vi::VisibilityIterator2 tmpvi(mss_p, vi::SortColumns(), false); 
	      //VisBufferUtil::getFreqRangeFromRange(lowfreq, topfreq,  freqFrame, lowfreq,  topfreq, tmpvi, selFreqFrame_p);
	      //    lockMS(*(const_cast<MeasurementSet*> (mss_p[mss_p.nelements()-1])));
              if(MSUtil::getFreqRangeInSpw( lowfreq,
				  topfreq, Vector<Int>(1,chanlist(k,0)), Vector<Int>(1,chanlist(k,1)),
				  Vector<Int>(1, chanlist(k,2)-chanlist(k,1)+1),
				 *mss_p[mss_p.nelements()-1] , 
				  selFreqFrame_p,
						 fieldList, False))
                {
                  selectionValid=True;
                  thisSpwSelValid=True;
                }
	      //  unlockMSs();
		    
	    }
	    
	    if(thisSpwSelValid || ignoreframe){
	      andFreqSelection(mss_p.nelements()-1, Int(freqList(k,0)), lowfreq, topfreq, selFreqFrame_p);
	      andChanSelection(mss_p.nelements()-1, Int(chanlist(k,0)), Int(chanlist(k,1)),Int(chanlist(k,2)));
	    }
          }
	  if(! (selectionValid && !ignoreframe)){
	    //os << "Did not match spw selection in the selected ms " << LogIO::WARN << LogIO::POST;
	    retval=False;
	  }
	    //fselections_p->add(channelSelector);
          //////////////////////////////////
      }
      else{

	//////More workaroung CAS-8829
	//MFrequency::Types freqFrame=MFrequency::castType(MSColumns(*mss_p[mss_p.nelements()-1]).spectralWindow().measFreqRef()(Int(freqList(0,0))));
    	  Quantity freq;
    	  Quantity::read(freq, selpars.freqbeg);
    	  Double lowfreq=freq.getValue("Hz");
    	  Quantity::read(freq, selpars.freqend);
    	  Double topfreq=freq.getValue("Hz");
    	 
	  ////Work aroun CAS-8829
	  // if(vi_p) 
	    //VisBufferUtil::getFreqRangeFromRange(lowfreq, topfreq,  selpars.freqframe, lowfreq,  topfreq, *vi_p, freqFrame);
	  //cerr << "lowFreq "<< lowfreq << " topfreq " << topfreq << endl;
	  //vi::FrequencySelectionUsingFrame channelSelector((vi_p ? freqFrame :selpars.freqframe));
	  //vi::FrequencySelectionUsingFrame channelSelector(selpars.freqframe);
    	  for(uInt k=0; k < nSelections; ++k){
            //channelSelector.add(Int(freqList(k,0)), lowfreq, topfreq);
	    //andFreqSelection((mss_p.nelements()-1), Int(freqList(k,0)), lowfreq, topfreq, vi_p ?freqFrame : selpars.freqframe);
	    andFreqSelection((mss_p.nelements()-1), Int(freqList(k,0)), lowfreq, topfreq, selFreqFrame_p);
          }
    	  //fselections_p->add(channelSelector);

      }
    }//End of channel selection
          
    writeAccess_p=writeAccess_p && !selpars.readonly;
    createVisSet(writeAccess_p);

    /////// Remove this when the new vi/vb is able to get the full freq range.
    mssFreqSel_p.resize();
    mssFreqSel_p  = thisSelection.getChanFreqList(NULL,true);
   
    //// Set the data column on which to operate
    // TT: added checks for the requested data column existace 
    //    cout << "Using col : " << selpars.datacolumn << endl;
    if( selpars.datacolumn.contains("data") || selpars.datacolumn.contains("obs") ) 
      {    if( thisms.tableDesc().isColumn("DATA") ) { datacol_p = FTMachine::OBSERVED; }
           else { os << LogIO::SEVERE <<"DATA column does not exist" << LogIO::EXCEPTION;}
      }
    else if( selpars.datacolumn.contains("corr") ) { datacol_p = FTMachine::CORRECTED; }
    else { os << LogIO::WARN << "Invalid data column : " << selpars.datacolumn << ". Using corrected (or observed if corrected doesn't exist)" << LogIO::POST;  datacol_p =  FTMachine::CORRECTED;}


    dataSel_p.resize(dataSel_p.nelements()+1, true);
    dataSel_p[dataSel_p.nelements()-1]=selpars;
	//cerr << "AT THE end of DATASEL " << selpars.toRecord() << endl;

    unlockMSs();
      }
    catch(AipsError &x)
      {
	unlockMSs();
	throw( AipsError("Error in selectData() : "+x.getMesg()) );
      }

    return retval;



  }
void SynthesisImagerVi2::andChanSelection(const Int msId, const Int spwId, const Int startchan, const Int endchan){

	map<Int, Vector<Int> > spwsel;
	auto it=channelSelections_p.find(msId);
	if(it !=channelSelections_p.end())
		spwsel=it->second;
	auto hasspw=spwsel.find(spwId);
	Vector<Int>chansel(2,-1);
	if(hasspw != spwsel.end()){
		chansel.resize();
		chansel=hasspw->second;
	}
	Int nchan=endchan-startchan+1;
	if(chansel(1)== -1)
		chansel(1)=startchan;
	if(chansel(1) >= startchan){
	  if(nchan > (chansel(1)-startchan+chansel(0))){
			chansel(0)=nchan;
	  }
	  else{
			chansel(0)=chansel(1)-startchan+chansel(0);
	  }
	  chansel(1)=startchan;
	}
	else{
		if((chansel(0) -(startchan - chansel(1)+1)) < nchan){	
		  chansel(0)=nchan+(startchan-chansel(1));
		}
	}
	spwsel[spwId]=chansel;
	channelSelections_p[msId]=spwsel;
	//cerr << "chansel "<< channelSelections_p << endl;
	
}
  void SynthesisImagerVi2::andFreqSelection(const Int msId, const Int spwId,  const Double freqBeg, const Double freqEnd, const MFrequency::Types frame){
    
    
    Int key=msId;
   
    Bool isDefined=False;
    FrequencySelectionUsingFrame frameSel(frame);
    for (uInt k =0; k<freqBegs_p.size(); ++k){ 
      //cerr <<freqBegs_p[k].first  << " == " << key << " && " << freqSpws_p[k].second<< " == " << spwId << " && " << freqBeg << " < " << freqEnds_p[k].second<< " && " << freqEnd << " > " << freqBegs_p[k].second << endl;
	if((freqBegs_p[k].first == key || key <0 ) && (freqSpws_p[k].second==spwId || spwId <0)  && (freqBeg < freqEnds_p[k].second) && (freqEnd > freqBegs_p[k].second)){
	isDefined=True;
	//cerr << k << " inside freqBegs " << freqBegs_p[k].second << "  " << freqBeg << endl;  
	if(freqBegs_p[k].second < freqBeg)
	  freqBegs_p[k].second=freqBeg;
	if(freqEnds_p[k].second > freqEnd)
	  freqEnds_p[k].second=freqEnd;
	if(msId < 0) key=freqBegs_p[k].first;
	//cerr << "modified " <<  freqBegs_p[k].second << "   "  <<  freqEnds_p[k].second << endl;
      }
	///add only those that have the same msid
	if(freqBegs_p[k].first == key){
	  //cerr << "added " << k << " freqBegs " << freqBegs_p[k].second << "  " << freqEnds_p[k].second << endl;  
	  frameSel.add(freqSpws_p[k].second ,  freqBegs_p[k].second, freqEnds_p[k].second);
	}
    }
    if(!isDefined && msId >=0){
      //cerr << "undefined " << key << " freqBegs "  << freqBeg << "  " << freqEnd << endl;  
      freqBegs_p.push_back(make_pair(key, freqBeg));
      freqEnds_p.push_back(make_pair(key, freqEnd));
      freqSpws_p.push_back(make_pair(key, spwId));
      frameSel.add(spwId,  freqBeg, freqEnd);
    }
    CountedPtr<vi::FrequencySelections> copyFsels=fselections_p->clone();
    uInt nMSs=copyFsels->size() <=msId ? msId+1 : copyFsels->size();
    //cerr << "nms " << nMSs << endl;
    fselections_p=new FrequencySelections();
    for (uInt k=0;  k < nMSs ; ++k){
      if(k==uInt(key)){
	fselections_p->add(frameSel);
	//cerr <<"adding framesel " << frameSel.toString() << endl;
      }
      else{
	const FrequencySelectionUsingFrame& thissel= static_cast<const FrequencySelectionUsingFrame &> (copyFsels->get(k));
	//cerr <<"framesel orig " << thissel.toString() << endl;
	fselections_p->add(thissel);

      }
    }
    
 

  }

  void SynthesisImagerVi2::tuneChunk(const Int gmap){
    

    CoordinateSystem cs=itsMappers.imageStore(gmap)->getCSys();
    IPosition imshape=itsMappers.imageStore(gmap)->getShape();
    /////For some reason imagestore returns 0 channel image sometimes
    ////
    if(imshape(3) < 1) {
      return;
    }
   
    Double minFreq=SpectralImageUtil::worldFreq(cs, 0.0);
    Double maxFreq=SpectralImageUtil::worldFreq(cs,imshape(3)-1);
   
    if(maxFreq < minFreq){
      Double tmp=minFreq;
      minFreq=maxFreq;
      maxFreq=tmp;
    }
    
    Int spectralIndex=cs.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate spectralCoord=cs.spectralCoordinate(spectralIndex);
    maxFreq+=fabs(spectralCoord.increment()(0))/2.0;
    minFreq-=fabs(spectralCoord.increment()(0))/2.0;
    if(minFreq < 0.0) minFreq=0.0;
    MFrequency::Types intype=spectralCoord.frequencySystem(True);
    
    if(!VisBufferUtil::getFreqRangeFromRange(minFreq, maxFreq,  intype, minFreq,  maxFreq, *vi_p, selFreqFrame_p)){
      //Do not retune if conversion did not happen
      return;
    }
      
    maxFreq+=3.0*fabs(spectralCoord.increment()(0))/2.0;
    minFreq-=3.0*fabs(spectralCoord.increment()(0))/2.0;
    if(minFreq < 0.0) minFreq=0.0;
    
    auto copyFreqBegs=freqBegs_p;
    auto copyFreqEnds=freqEnds_p;
    auto copyFreqSpws=  freqSpws_p;
    ///////////////TESTOO
    //cerr << std::setprecision(12) << "AFTER maxFreq " << maxFreq << "  minFreq " << minFreq << endl;
    //for (Int k =0 ; k < (fselections_p->size()) ; ++k){
    //  cerr << k << (fselections_p->get(k)).toString() << endl;
    // }
    ///////////////////////////////////////	   
    ///TESTOO
    // andFreqSelection(-1, -1, minFreq, maxFreq, MFrequency::TOPO); 
    andFreqSelection(-1, -1, minFreq, maxFreq, selFreqFrame_p);
    
    vi_p->setFrequencySelection (*fselections_p);

    freqBegs_p=copyFreqBegs;
    freqEnds_p=copyFreqEnds;
    freqSpws_p=copyFreqSpws;
    



  }


Bool SynthesisImagerVi2::defineImage(SynthesisParamsImage& impars, 
			   const SynthesisParamsGrid& gridpars)
  {

    LogIO os( LogOrigin("SynthesisImagerVi2","defineImage",WHERE) );
    if(mss_p.nelements() ==0)
      os << "SelectData has to be run before defineImage" << LogIO::EXCEPTION;

    CoordinateSystem csys;
    CountedPtr<refim::FTMachine> ftm, iftm;
    impars_p = impars;
    gridpars_p = gridpars; 

    try
      {
	

	os << "Define image coordinates for [" << impars.imageName << "] : " << LogIO::POST;

	csys = impars_p.buildCoordinateSystem( *vi_p, channelSelections_p, mss_p );
	//use the location defined for coordinates frame;
	mLocation_p=impars_p.obslocation;
	IPosition imshape = impars_p.shp();

	os << "Impars : start " << impars_p.start << LogIO::POST;
	os << "Shape : " << imshape << "Spectral : " << csys.spectralCoordinate().referenceValue() << " at " << csys.spectralCoordinate().referencePixel() << " with increment " << csys.spectralCoordinate().increment() << LogIO::POST;

	if( (itsMappers.nMappers()==0) || 
	    (impars_p.imsize[0]*impars_p.imsize[1] > itsMaxShape[0]*itsMaxShape[1]))
	  {
	    itsMaxShape=imshape;
	    itsMaxCoordSys=csys;
	  }
        itsNchan = imshape[3];
        itsCsysRec = impars_p.getcsys();
	/*
	os << "Define image  [" << impars.imageName << "] : nchan : " << impars.nchan 
	   //<< ", freqstart:" << impars.freqStart.getValue() << impars.freqStart.getUnit() 
	   << ", start:" << impars.start
	   <<  ", imsize:" << impars.imsize 
	   << ", cellsize: [" << impars.cellsize[0].getValue() << impars.cellsize[0].getUnit() 
	   << " , " << impars.cellsize[1].getValue() << impars.cellsize[1].getUnit() 
	   << LogIO::POST;
	*/
        // phasecenter
        if (impars_p.phaseCenterFieldId == -1) {
          // user-specified
          phaseCenter_p = impars_p.phaseCenter;
        } else if (impars_p.phaseCenterFieldId >= 0) {
          // FIELD_ID
          auto const msobj = mss_p[0];
          MSFieldColumns msfield(msobj->field());
          phaseCenter_p=msfield.phaseDirMeas(impars_p.phaseCenterFieldId);
        } else {
          // use default FIELD_ID (0)
          auto const msobj = mss_p[0];
          MSFieldColumns msfield(msobj->field());
          phaseCenter_p=msfield.phaseDirMeas(0);
        }

      }
    catch(AipsError &x)
      {
	os << "Error in building Coordinate System and Image Shape : " << x.getMesg() << LogIO::EXCEPTION;
      }

	
    try
      {
	os << "Set Gridding options for [" << impars_p.imageName << "] with ftmachine : " << gridpars.ftmachine << LogIO::POST;

	itsVpTable=gridpars.vpTable;
	itsMakeVP= ( gridpars.ftmachine.contains("mosaicft") ||
		             gridpars.ftmachine.contains("awprojectft") )?False:True;

	//cerr << "DEFINEimage " << impars_p.toRecord() << endl; 				 
					 
	createFTMachine(ftm, iftm, gridpars.ftmachine, impars_p.nTaylorTerms, gridpars.mType, 
			gridpars.facets, gridpars.wprojplanes,
			gridpars.padding,gridpars.useAutoCorr,gridpars.useDoublePrec,
			gridpars.convFunc,
			gridpars.aTermOn,gridpars.psTermOn, gridpars.mTermOn,
			gridpars.wbAWP,gridpars.cfCache,gridpars.usePointing,gridpars.pointingOffsetSigDev.tovector(),
			gridpars.doPBCorr,gridpars.conjBeams,
			gridpars.computePAStep,gridpars.rotatePAStep,
			gridpars.interpolation, impars_p.freqFrameValid, 1000000000,  16, impars_p.stokes,
			impars_p.imageName, gridpars.pointingDirCol, gridpars.skyPosThreshold,
			gridpars.convSupport, gridpars.truncateSize, gridpars.gwidth, gridpars.jwidth,
			gridpars.minWeight, gridpars.clipMinMax, impars_p.pseudoi);

      }
    catch(AipsError &x)
      {
	os << "Error in setting up FTMachine() : " << x.getMesg() << LogIO::EXCEPTION;
      }

    try
      {

		appendToMapperList(impars_p.imageName,  csys,  impars_p.shp(),
			   ftm, iftm,
			   gridpars.distance, gridpars.facets, gridpars.chanchunks,impars_p.overwrite,
			   gridpars.mType, gridpars.padding, impars_p.nTaylorTerms, impars_p.startModel);
	
	imageDefined_p=true;
      }
    catch(AipsError &x)
      {
	os << "Error in adding Mapper : "+x.getMesg() << LogIO::EXCEPTION;
      }
	imparsVec_p.resize(imparsVec_p.nelements()+1, true);
	imparsVec_p[imparsVec_p.nelements()-1]=impars_p;
	///For now cannot deal with cube and mtmfs in C++ parallel mode
	if(imparsVec_p[0].deconvolver=="mtmfs") setCubeGridding(False);
	//cerr <<"DECONV " << imparsVec_p[0].deconvolver << " cube gridding " << doingCubeGridding_p << endl;
	gridparsVec_p.resize(gridparsVec_p.nelements()+1, true);
	gridparsVec_p[imparsVec_p.nelements()-1]=gridpars_p;
	//For now as awproject does not work with the c++ mpi cube gridding make sure it works the old way as mfs
	if(gridparsVec_p[0].ftmachine.contains("awproject"))
	   setCubeGridding(False);
	
    return true;
  }
Bool SynthesisImagerVi2::defineImage(CountedPtr<SIImageStore> imstor, SynthesisParamsImage& impars, 
			   const SynthesisParamsGrid& gridpars){
	
	Int id=itsMappers.nMappers();
    CoordinateSystem csys =imstor->getCSys();
    IPosition imshape=imstor->getShape();
    Int nx=imshape[0], ny=imshape[1];
    if( (id==0) || (nx*ny > itsMaxShape[0]*itsMaxShape[1]))
      {
	itsMaxShape=imshape;
	itsMaxCoordSys=csys;
      }
    mLocation_p=impars.obslocation;
    // phasecenter
    if (impars.phaseCenterFieldId == -1) {
          // user-specified
          phaseCenter_p = impars.phaseCenter;
        } else if (impars.phaseCenterFieldId >= 0) {
          // FIELD_ID
          auto const msobj = mss_p[0];
          MSFieldColumns msfield(msobj->field());
          phaseCenter_p=msfield.phaseDirMeas(impars.phaseCenterFieldId);
        } else {
          // use default FIELD_ID (0)
          auto const msobj = mss_p[0];
          MSFieldColumns msfield(msobj->field());
          phaseCenter_p=msfield.phaseDirMeas(0);
        }
	itsVpTable=gridpars.vpTable;
	itsMakeVP= ( gridpars.ftmachine.contains("mosaicft") ||
		             gridpars.ftmachine.contains("awprojectft") )?False:True;
	CountedPtr<refim::FTMachine> ftm, iftm;
	createFTMachine(ftm, iftm, gridpars.ftmachine, impars.nTaylorTerms, gridpars.mType, 
			gridpars.facets, gridpars.wprojplanes,
			gridpars.padding,gridpars.useAutoCorr,gridpars.useDoublePrec,
			gridpars.convFunc,
			gridpars.aTermOn,gridpars.psTermOn, gridpars.mTermOn,
			gridpars.wbAWP,gridpars.cfCache,gridpars.usePointing,
			gridpars.pointingOffsetSigDev.tovector(),
			gridpars.doPBCorr,gridpars.conjBeams,
			gridpars.computePAStep,gridpars.rotatePAStep,
			gridpars.interpolation, impars.freqFrameValid, 1000000000,  16, impars.stokes,
			impars.imageName, gridpars.pointingDirCol, gridpars.skyPosThreshold,
			gridpars.convSupport, gridpars.truncateSize, gridpars.gwidth, gridpars.jwidth,
			gridpars.minWeight, gridpars.clipMinMax, impars.pseudoi);  
	
	if(gridpars.facets >1)
	{
	      // Make and connect the list.
		Block<CountedPtr<SIImageStore> > imstorList = createFacetImageStoreList( imstor, gridpars.facets );
		for( uInt facet=0; facet<imstorList.nelements(); facet++)
		{
		  CountedPtr<refim::FTMachine> new_ftm, new_iftm;
		  if(facet==0){ new_ftm = ftm;  new_iftm = iftm; }
		  else{ new_ftm=ftm->cloneFTM();  new_iftm=iftm->cloneFTM(); }
		  itsMappers.addMapper(createSIMapper( gridpars.mType, imstorList[facet], new_ftm, new_iftm));
		}
	}
	else{
		itsMappers.addMapper(  createSIMapper( gridpars.mType, imstor, ftm, iftm ) );	
	}
        impars_p=impars;
        gridpars_p=gridpars;
	imageDefined_p=true;
	return true;
}
Bool SynthesisImagerVi2::defineImage(CountedPtr<SIImageStore> imstor, 
				    const String& ftmachine)
  {
    CountedPtr<refim::FTMachine> ftm, iftm;

    // The following call to createFTMachine() uses the
    // following defaults
    //
    // facets=1, wprojplane=1, padding=1.0, useAutocorr=false, 
    // useDoublePrec=true, gridFunction=String("SF")
    //
    createFTMachine(ftm, iftm, ftmachine);
    
    Int id=itsMappers.nMappers();
    CoordinateSystem csys =imstor->getCSys();
    IPosition imshape=imstor->getShape();
    Int nx=imshape[0], ny=imshape[1];
    if( (id==0) || (nx*ny > itsMaxShape[0]*itsMaxShape[1]))
      {
	itsMaxShape=imshape;
	itsMaxCoordSys=csys;
      }

    itsMappers.addMapper(  createSIMapper( "default", imstor, ftm, iftm ) );
    imageDefined_p=true;
    return true;
  }
Bool SynthesisImagerVi2::defineImage(CountedPtr<SIImageStore> imstor, 
				    const Record& ftmRec, const Record& iftmRec)
  {
    CountedPtr<refim::FTMachine> ftm, iftm;
	ftm=refim::VisModelData::NEW_FT(ftmRec);
	iftm=refim::VisModelData::NEW_FT(iftmRec);
    
    Int id=itsMappers.nMappers();
    CoordinateSystem csys =imstor->getCSys();
    IPosition imshape=imstor->getShape();
    Int nx=imshape[0], ny=imshape[1];
    if( (id==0) || (nx*ny > itsMaxShape[0]*itsMaxShape[1]))
      {
	itsMaxShape=imshape;
	itsMaxCoordSys=csys;
      }

    itsMappers.addMapper(  createSIMapper( "default", imstor, ftm, iftm, id ) );
    imageDefined_p=true;
    return true;
  }
 Bool SynthesisImagerVi2::weight(const Record& inrec){
	String type, rmode, filtertype;
	Quantity noise, fieldofview,filterbmaj,filterbmin, filterbpa;  
	Double robust, fracBW;
	Int npixels;
	Bool multiField, useCubeBriggs;
	SynthesisUtilMethods::getFromWeightRecord(type, rmode,noise, robust,fieldofview,npixels, multiField, useCubeBriggs,
				  filtertype, filterbmaj,filterbmin, filterbpa, fracBW, inrec);
	return weight(type, rmode,noise, robust,fieldofview,npixels, multiField, useCubeBriggs,
				  filtertype, filterbmaj,filterbmin, filterbpa, fracBW );
				
	 
 }
 Bool SynthesisImagerVi2::weight(const String& type, const String& rmode,
			       const Quantity& noise, const Double robust,
			       const Quantity& fieldofview,
				 const Int npixels, const Bool multiField, const Bool useCubeBriggs,
			       const String& filtertype, const Quantity& filterbmaj,
			       const Quantity& filterbmin, const Quantity& filterbpa, Double fracBW)
  {
      LogIO os(LogOrigin("SynthesisImagerVi2", "weight()", WHERE));
      if(rmode=="bwtaper") //See CAS-13021 for bwtaper algorithm details
      {
          if(fracBW == 0.0)
          {
              Double minFreq = 0.0;
              Double maxFreq = 0.0;
              
              if(itsMaxShape(3) < 1) {
                cout << "SynthesisImagerVi2::weight Only one channel in image " << endl;
              }
              else{
                  minFreq=abs(SpectralImageUtil::worldFreq(itsMaxCoordSys, 0.0));
                  maxFreq=abs(SpectralImageUtil::worldFreq(itsMaxCoordSys,itsMaxShape(3)-1));

                  if(maxFreq < minFreq){
                    Double tmp=minFreq;
                    minFreq=maxFreq;
                    maxFreq=tmp;
                  }
                  
                  if((maxFreq != 0.0) || (minFreq != 0.0)) fracBW = 2*(maxFreq - minFreq)/(maxFreq + minFreq);
                  
                  os << LogIO::NORMAL << " Fractional bandwidth used by briggsbwtaper " << fracBW << endl;  //<< LogIO::POST;
                  
              }
          }
      }
      
	weightParams_p=SynthesisUtilMethods::fillWeightRecord(type, rmode,noise, robust,fieldofview,
				 npixels, multiField, useCubeBriggs,filtertype, filterbmaj,filterbmin, filterbpa, fracBW);

       try {
    	//Int nx=itsMaxShape[0];
    	//Int ny=itsMaxShape[1];
	 Quantity cellx=Quantity(itsMaxCoordSys.increment()[0], itsMaxCoordSys.worldAxisUnits()[0]);
	 Quantity celly=Quantity(itsMaxCoordSys.increment()[1], itsMaxCoordSys.worldAxisUnits()[1]);
	 os << LogIO::NORMAL // Loglevel INFO
	    << "Set imaging weights : " ; //<< LogIO::POST;
	 
	 if (type=="natural") {
	   os << LogIO::NORMAL // Loglevel INFO
	      << "Natural weighting" << LogIO::POST;
	   imwgt_p=VisImagingWeight("natural");
	 }
      else if (type=="radial") {
	os << "Radial weighting" << LogIO::POST;
    	  imwgt_p=VisImagingWeight("radial");
      }
      else{
	vi_p->originChunks();
	vi_p->origin();
    	  if(!imageDefined_p)
    		  throw(AipsError("Need to define image"));
    	  Int nx=itsMaxShape[0];
    	  Int ny=itsMaxShape[1];
    	  Quantity cellx=Quantity(itsMaxCoordSys.increment()[0], itsMaxCoordSys.worldAxisUnits()[0]);
    	  Quantity celly=Quantity(itsMaxCoordSys.increment()[1], itsMaxCoordSys.worldAxisUnits()[1]);
    	  if(type=="superuniform"){
    		  if(!imageDefined_p) throw(AipsError("Please define image first"));
    		  Int actualNpix=npixels;
    		  if(actualNpix <=0)
    			  actualNpix=3;
    		  os << LogIO::NORMAL // Loglevel INFO
    				  << "SuperUniform weighting over a square cell spanning ["
    				  << -actualNpix
    				  << ", " << actualNpix << "] in the uv plane" << LogIO::POST;
    		  imwgt_p=VisImagingWeight(*vi_p, rmode, noise, robust, nx,
    				  ny, cellx, celly, actualNpix,
    				  actualNpix, multiField);
    	  }
    	  else if ((type=="robust")||(type=="uniform")||(type=="briggs")) {
   		  if(!imageDefined_p) throw(AipsError("Please define image first"));
    		  Quantity actualFieldOfView_x(fieldofview), actualFieldOfView_y(fieldofview) ;
    		  Int actualNPixels_x(npixels),actualNPixels_y(npixels) ;
    		  String wtype;
    		  if(type=="briggs") {
    			  wtype = "Briggs";
    		  }
    		  else {
    			  wtype = "Uniform";
    		  }
    		  if(actualFieldOfView_x.get().getValue()==0.0&&actualNPixels_x==0) {
    			  actualNPixels_x=nx;
    			  actualFieldOfView_x=Quantity(actualNPixels_x*cellx.get("rad").getValue(),"rad");
    			  actualNPixels_y=ny;
    			  actualFieldOfView_y=Quantity(actualNPixels_y*celly.get("rad").getValue(),"rad");
    			  os << LogIO::NORMAL // Loglevel INFO
    					  << wtype
    					  << " weighting: sidelobes will be suppressed over full image"
    					  << LogIO::POST;
    		  }
    		  else if(actualFieldOfView_x.get().getValue()>0.0&&actualNPixels_x==0) {
    			  actualNPixels_x=nx;
    			  actualNPixels_y=ny;
    			  os << LogIO::NORMAL // Loglevel INFO
    					  << wtype
    					  << " weighting: sidelobes will be suppressed over specified field of view: "
			                  << actualFieldOfView_x.get("arcsec").getValue() << " arcsec by " 
			                  << actualFieldOfView_y.get("arcsec").getValue()  << " arcsec" << LogIO::POST;
    		  }
    		  else if(actualFieldOfView_x.get().getValue()==0.0&&actualNPixels_x>0) {
    			  actualFieldOfView_x=Quantity(actualNPixels_x*cellx.get("rad").getValue(),"rad");
    			  actualFieldOfView_y=Quantity(actualNPixels_y*celly.get("rad").getValue(),"rad");
    			  os << LogIO::NORMAL // Loglevel INFO
    					  << wtype
    					  << " weighting: sidelobes will be suppressed over full image field of view: "
			                  << actualFieldOfView_x.get("arcsec").getValue() << " arcsec by " 
    					  << actualFieldOfView_y.get("arcsec").getValue() << " arcsec" << LogIO::POST;
    		  }
    		  else {
    			  os << LogIO::NORMAL // Loglevel INFO
    					  << wtype
    					  << " weighting: sidelobes will be suppressed over specified field of view: "
			                  << actualFieldOfView_x.get("arcsec").getValue() << " arcsec by " 
    					  << actualFieldOfView_y.get("arcsec").getValue() << " arcsec" << LogIO::POST;
    		  }
    		  os << LogIO::DEBUG1
		     << "Weighting used " << actualNPixels_x << " by " << actualNPixels_y << " uv pixels."
		     << LogIO::POST;
    		  Quantity actualCellSize_x(actualFieldOfView_x.get("rad").getValue()/actualNPixels_x, "rad");
    		  Quantity actualCellSize_y(actualFieldOfView_y.get("rad").getValue()/actualNPixels_y, "rad");


		  //		  cerr << "rmode " << rmode << " noise " << noise << " robust " << robust << " npixels " << actualNPixels << " cellsize " << actualCellSize << " multifield " << multiField << endl;
		  //		  Timer timer;
		  //timer.mark();
		  //Construct imwgt_p with old vi for now if old vi is in use as constructing with vi2 is slower
		  //Determine if any image is cube
		  if(useCubeBriggs){
		    String outstr=String("Doing spectral cube Briggs weighting formula --  " + rmode + (rmode=="abs" ? " with estimated noise "+ String::toString(noise.getValue())+noise.getUnit()  : "")); 
		    os << outstr << LogIO::POST;
		    //VisImagingWeight nat("natural");
		    //vi_p->useImagingWeight(nat);
		    if(rmode=="abs" && robust==0.0 && noise.getValue()==0.0)
		      throw(AipsError("Absolute Briggs formula does not allow for robust 0 and estimated noise per visibility 0"));
                
            CountedPtr<refim::BriggsCubeWeightor> bwgt=new refim::BriggsCubeWeightor(wtype=="Uniform" ? "none" : rmode, noise, robust,fracBW, npixels, multiField);
            for (Int k=0; k < itsMappers.nMappers(); ++k){
                      itsMappers.getFTM2(k)->setBriggsCubeWeight(bwgt);
                    }
              
              
		  }
		  else
		  {
		    imwgt_p=VisImagingWeight(*vi_p, wtype=="Uniform" ? "none" : rmode, noise, robust,
					     actualNPixels_x, actualNPixels_y, actualCellSize_x,
					     actualCellSize_y, 0, 0, multiField);
		  }

		  /*
		  if(rvi_p !=NULL){
		    imwgt_p=VisImagingWeight(*rvi_p, rmode, noise, robust,
                                 actualNPixels, actualNPixels, actualCellSize,
                                 actualCellSize, 0, 0, multiField);
		  }
		  else{
		    ////This is slower by orders of magnitude as of 2014/06/25
		    imwgt_p=VisImagingWeight(*vi_p, rmode, noise, robust,
                                 actualNPixels, actualNPixels, actualCellSize,
                                 actualCellSize, 0, 0, multiField);
		  }
		  */
		    //timer.show("After making visweight ");

    	  }
    	  else {
    		  //this->unlock();
    		  os << LogIO::SEVERE << "Unknown weighting " << type
    				  << LogIO::EXCEPTION;
    		  return false;
    	  }
      }
	 
	 //// UV-Tapering
	 //cout << "Taper type : " << filtertype << " : " << (filtertype=="gaussian") <<  endl;
	 if( filtertype == "gaussian" ) {
	   //	   os << "Setting uv-taper" << LogIO::POST;
	   imwgt_p.setFilter( filtertype,  filterbmaj, filterbmin, filterbpa );
	 }
	 vi_p->useImagingWeight(imwgt_p);
      ///////////////////////////////
	 
	     SynthesisUtilMethods::getResource("Set Weighting");

	 ///	 return true;
	 
       }
       catch(AipsError &x)
	 {
	   throw( AipsError("Error in Weighting : "+x.getMesg()) );
	 }
       
       return true;
  }

void SynthesisImagerVi2::appendToMapperList(String imagename,  
					   CoordinateSystem& csys, 
					   IPosition imshape,
					    CountedPtr<refim::FTMachine>& ftm,
					    CountedPtr<refim::FTMachine>& iftm,
					   Quantity distance,
					   Int facets,
					   Int chanchunks,
					   const Bool overwrite,
					   String mappertype,
					   Float padding,
					   uInt ntaylorterms,
					   const Vector<String> &startmodel)
    {
      LogIO log_l(LogOrigin("SynthesisImagerVi2", "appendToMapperList(ftm)"));
      //---------------------------------------------
      // Some checks..
      
      if(facets > 1 && itsMappers.nMappers() > 0)
	log_l << "Facetted image has to be the first of multifields" << LogIO::EXCEPTION;

     TcleanProcessingInfo procInfo;
     CompositeNumber cn(uInt(imshape[0] * 2));
     // heuristic factors multiplied to imshape based on gridder
     size_t fudge_factor = 15;
     if (ftm->name()=="MosaicFTNew") {
         fudge_factor = 20;
     }
     else if (ftm->name()=="GridFT") {
         fudge_factor = 9;
     }
     std::tie(procInfo, std::ignore, std::ignore) =
         nSubCubeFitInMemory(fudge_factor, imshape, padding);

     // chanchunks auto-calculation block, for now still here for awproject (CAS-12204)
     if(chanchunks<1)
	{
	  log_l << "Automatically calculated chanchunks";
	  log_l << " using imshape : " << imshape << LogIO::POST;

	  // Do calculation here.
	  // This runs once per image field (for multi-field imaging)
	  // This runs once per cube partition, and will see only its own partition's shape
		//chanchunks=1;

                chanchunks = procInfo.chnchnks;

		/*log_l << "Required memory " << required_mem / nlocal_procs / 1024. / 1024. / 1024.
                 << "\nAvailable memory " << memory_avail / 1024. / 1024 / 1024.
                 << " (rc: memory fraction " << usr_memfrac << "% rc memory " << usr_mem / 1024.
                 << ")\n" << nlocal_procs << " other processes on node\n"
                 << "Setting chanchunks to " << chanchunks << LogIO::POST;
		*/
	}
	//record this in gridpars_p
	gridpars_p.chanchunks=chanchunks;
      if( imshape.nelements()==4 && imshape[3]<chanchunks )
	{
	  log_l << LogIO::WARN << "An image with " << imshape[3] << " channel(s) cannot be divided into " << chanchunks << " chunks. Please set chanchunks=1 or choose chanchunks<nchan." << LogIO::EXCEPTION;
	}

      if(chanchunks > 1 && itsMappers.nMappers() > 0)
	log_l << "Channel chunking is currently not supported with multi(outlier)-fields. Please submit a feature request if needed." << LogIO::EXCEPTION;

      if(chanchunks > 1) itsDataLoopPerMapper=true;
      
      AlwaysAssert( ( ( ! (ftm->name()=="MosaicFTNew" && mappertype=="imagemosaic") )  && 
      		      ( ! (ftm->name()=="AWProjectWBFTNew" && mappertype=="imagemosaic") )) ,
		    AipsError );
      //---------------------------------------------

      // Create the ImageStore object
      CountedPtr<SIImageStore> imstor;
      MSColumns msc(*(mss_p[0]));
      imstor = createIMStore(imagename, csys, imshape, overwrite,msc, mappertype, ntaylorterms, distance, procInfo, facets, iftm->useWeightImage(), startmodel );

      // Create the Mappers
      if( facets<2 && chanchunks<2) // One facet. Just add the above imagestore to the mapper list.
	{
	  itsMappers.addMapper(  createSIMapper( mappertype, imstor, ftm, iftm, ntaylorterms) );
	}
      else // This field is facetted. Make a list of reference imstores, and add all to the mapper list.
	{

	  if ( facets>1 && chanchunks==1 )
	    {
	      // Make and connect the list.
	      Block<CountedPtr<SIImageStore> > imstorList = createFacetImageStoreList( imstor, facets );
	      for( uInt facet=0; facet<imstorList.nelements(); facet++)
		{
		  CountedPtr<refim::FTMachine> new_ftm, new_iftm;
		  if(facet==0){ new_ftm = ftm;  new_iftm = iftm; }
		  else{ new_ftm=ftm->cloneFTM();  new_iftm=iftm->cloneFTM(); }
// 		  imstorList[facet]->setDataPolFrame(imstor->getDataPolFrame());
		  itsMappers.addMapper(createSIMapper( mappertype, imstorList[facet], new_ftm, new_iftm, ntaylorterms));
		}
	    }// facets
	  else if ( facets==1 && chanchunks>1 )
	    {
	      // Make and connect the list.
	      Block<CountedPtr<SIImageStore> > imstorList = createChanChunkImageStoreList( imstor, chanchunks );
	      for( uInt chunk=0; chunk<imstorList.nelements(); chunk++)
		{
		  
		  CountedPtr<refim::FTMachine> new_ftm, new_iftm;
		  if(chunk==0){ 
		    new_ftm = ftm;  
		    new_iftm = iftm; }
		  else{ 
		    new_ftm=ftm->cloneFTM();  
		    new_iftm=iftm->cloneFTM(); }
		  imstorList[chunk]->setDataPolFrame(imstor->getDataPolFrame());
		  itsMappers.addMapper(createSIMapper( mappertype, imstorList[chunk], new_ftm, new_iftm, ntaylorterms));
		}
	    }// chanchunks
	  else
	    {
	      throw( AipsError("Error in requesting "+String::toString(facets)+" facets on a side with " + String::toString(chanchunks) + " channel chunks.  Support for faceting along with channel chunking is not yet available. Please submit a feature-request if you need multiple facets as well as chanchunks. ") );
	    }

	}// facets or chunks

    }

  /////////////////////////
  /**
   * Calculations of memory required / available -> nchunks .
   *
   * Returns a tuple with a TcleanProcessingInfo, vector of start channels per subchunk,
   * vector of end channels.
   */
  std::tuple<TcleanProcessingInfo, Vector<Int>, Vector<Int> > SynthesisImagerVi2::nSubCubeFitInMemory(const Int fudge_factor, const IPosition& imshape, const Float padding){
	LogIO log_l(LogOrigin("SynthesisImagerVi2", "nSubCubeFitInMemory"));

	Double required_mem = fudge_factor * sizeof(Float);
	int nsubcube=1;
	CompositeNumber cn(uInt(imshape[0] * 2));
	for (size_t i = 0; i < imshape.nelements(); i++) {
			// gridding pads image and increases to composite number
			if (i < 2) {
				required_mem *= cn.nextLargerEven(Int(padding*Float(imshape[i])-0.5));
			}
			else {
				required_mem *= imshape[i];
			}
	}

	// get number of tclean processes running on the same machine
	size_t nlocal_procs = 1;
	if (getenv("OMPI_COMM_WORLD_LOCAL_SIZE")) {
		std::stringstream ss(getenv("OMPI_COMM_WORLD_LOCAL_SIZE"));
		ss >> nlocal_procs;
	}
        //cerr << "NUM_PROC " << nlocal_procs << endl;
	// assumes all processes need the same amount of memory
	required_mem *= nlocal_procs;
	Double usr_memfrac, usr_mem;
	AipsrcValue<Double>::find(usr_memfrac, "system.resources.memfrac", 80.);
	AipsrcValue<Double>::find(usr_mem, "system.resources.memory", -1.);
	Double memory_avail;
	if (usr_mem > 0.) {
		memory_avail = usr_mem * 1024. * 1024.;
	}
	else {
	    memory_avail = Double(HostInfo::memoryFree()) * (usr_memfrac / 100.) * 1024.;
	}
	// compute required chanchunks to fit into the available memory
	nsubcube = (int)std::ceil((Double)required_mem / memory_avail);
        Int nworkers= applicator.numProcs() < 2 ? 1 : applicator.numProcs()-1;
        if((nsubcube/nworkers) >1 && nworkers !=1){
          nsubcube=(Int(std::floor(Float(nsubcube)/Float(nworkers)))+1)*nworkers;

        }
	if (imshape.nelements() == 4 && imshape[3] < nsubcube) {
		nsubcube = imshape[3];
              // TODO make chanchunks a divisor of nchannels?
	}
	nsubcube = nsubcube < 1 ? 1 : nsubcube;
        if( (imshape[3] >= nworkers) && (nsubcube < nworkers)){
          nsubcube=nworkers;
          ///TESTOO
          //if(imshape[3] > 2*nworkers)
          //  nsubcube=2*nworkers;

        }
	 else if(imshape[3] < (applicator.numProcs()-1)){
		nsubcube=imshape[3]; 
	 }
	Int chunksize=imshape[3]/nsubcube;
	Int rem=imshape[3] % nsubcube;
	//case of nchan < numprocs
	if(chunksize==0 && rem > 0){
		nsubcube=rem;
		chunksize=1;
		rem=0;
	}
	///Avoid an extra chunk with 1 channel as it cause bumps in linear interpolation
	///See CAS-12625
        /*
	while((rem==1) && (chunksize >1)){
		nsubcube +=1;
		chunksize=imshape[3]/nsubcube;
		rem=imshape[3] % nsubcube;
	}
	if(rem !=0) ++nsubcube;
        . */
	Vector<Int> start(nsubcube,0);
	Vector<Int> end(nsubcube,chunksize-1);
        if(rem >0){
          end(0)+=1;
          --rem;
        }
	for (Int k=1; k < nsubcube; ++k){
		start(k)=end(k-1)+1;
                //	end(k)=((k !=nsubcube-1) || rem==0)? (start(k)+chunksize-1) : (start(k)+rem-1);
                end(k)=(start(k)+chunksize-1);
                if(rem > 0){
                  end(k)+=1;
                  --rem;
                }
	}

        // print mem related info to log
        const float toGB = 1024.0 * 1024.0 * 1024.0;
        std::ostringstream usr_mem_msg;
        if (usr_mem > 0.) {
            usr_mem_msg << usr_mem / 1024.;
        } else {
            usr_mem_msg << "-";
        }
        std::ostringstream oss;
        oss << setprecision(4);
        oss << "Required memory: " << required_mem / toGB
            << " GB. Available mem.: " << memory_avail / toGB
            << " GB (rc, mem. fraction: " << usr_memfrac
            << "%, memory: " << usr_mem_msg.str()
            << ") => Subcubes: " << nsubcube
            << ". Processes on node: " << nlocal_procs << ".\n";
        log_l << oss.str() << LogIO::POST;

        TcleanProcessingInfo procInfo;
        procInfo.mpiprocs = nlocal_procs;
        procInfo.chnchnks = nsubcube;
        procInfo.memavail = memory_avail / toGB;
        procInfo.memreq = required_mem / toGB;
        return make_tuple(procInfo, start, end);
  }
  
 void SynthesisImagerVi2::runMajorCycleCube( const Bool dopsf, 
				      const Record inpcontrol) {
	LogIO os( LogOrigin("SynthesisImagerVi2","runMajorCycleCube",WHERE) );		  
	if(dopsf){
	  runCubeGridding(True);
	  ///Store the beamsets in ImageInfo
	  for(Int k=0; k < itsMappers.nMappers(); ++k){
	   
	    (itsMappers.imageStore(k))->getBeamSet();
	  }
	}
	else
		runCubeGridding(False, inpcontrol);
	
			  
			  
	}
 void SynthesisImagerVi2::runMajorCycle(const Bool dopsf, 
				      const Bool savemodel)
  {
    LogIO os( LogOrigin("SynthesisImagerVi2","runMajorCycle",WHERE) );

    //    cout << "Savemodel : " << savemodel << "   readonly : " << readOnly_p << "   usescratch : " << useScratch_p << endl;
    Bool savemodelcolumn = savemodel && !readOnly_p && useScratch_p;
    Bool savevirtualmodel = savemodel && !readOnly_p && !useScratch_p;

    if( savemodelcolumn ) os << "Saving model column" << LogIO::POST;
    if( savevirtualmodel ) os << "Saving virtual model" << LogIO::POST;

    SynthesisUtilMethods::getResource("Start Major Cycle");

    itsMappers.checkOverlappingModels("blank");

    {
      vi::VisBuffer2* vb=vi_p->getVisBuffer();
      vi_p->originChunks();
      vi_p->origin();
      Double numcoh=0;
      for (uInt k=0; k< mss_p.nelements(); ++k)
	numcoh+=Double(mss_p[k]->nrow());
      ProgressMeter pm(1.0, numcoh, 
			 dopsf?"Gridding Weights and PSF":"Major Cycle", "","","",true);
      rownr_t cohDone=0;


    	if(!dopsf)itsMappers.initializeDegrid(*vb);
    	itsMappers.initializeGrid(*vi_p,dopsf);
	SynthesisUtilMethods::getResource("After initGrid for all mappers");
        ////Under some peculiar selection criterion and low channel ms  vb2 seems to return more channels than in spw
        {
          vi_p->originChunks();
          vi_p->origin();
          Int nchannow=vb->nChannels();
          Int spwnow=vb->spectralWindows()[0];
          Int nchaninms=MSColumns(vb->ms()).spectralWindow().numChan()(spwnow);
          //cerr << "chans " << nchaninms << "   " << nchannow << endl;
          if (nchaninms < nchannow)
            throw(AipsError("A nasty Visbuffer2 error occured...wait for CNGI"));
        }
          //////
    	for (vi_p->originChunks(); vi_p->moreChunks();vi_p->nextChunk())
    	{

	  for (vi_p->origin(); vi_p->more(); vi_p->next())
    		{
		  //if (SynthesisUtilMethods::validate(*vb)==SynthesisUtilMethods::NOVALIDROWS) break; // No valid rows in this VB
		  //		  cerr << "nRows "<< vb->nRow() << "   " << max(vb->visCube()) <<  endl;
		  if (SynthesisUtilMethods::validate(*vb)!=SynthesisUtilMethods::NOVALIDROWS)
		    {
    			if(!dopsf) {
			  { Cube<Complex> mod(vb->nCorrelations(), vb->nChannels(), vb->nRows(), Complex(0.0));
			    vb->setVisCubeModel(mod); 
			  }
			  itsMappers.degrid(*vb, savevirtualmodel );
			  if(savemodelcolumn && writeAccess_p ){	
				const_cast<MeasurementSet& >((vi_p->ms())).lock(true);
			    vi_p->writeVisModel(vb->visCubeModel());
				const_cast<MeasurementSet& >((vi_p->ms())).unlock();
				
			    //static_cast<VisibilityIteratorImpl2 *> (vi_p->getImpl())->writeVisModel(vb->visCubeModel());

			    // Cube<Complex> tt=vb->visCubeModel();
			    // tt = 20.0;
			    // cout << "Vis:" << tt << endl;
			    // static_cast<VisibilityIteratorImpl2 *> (vi_p->getImpl())->writeVisModel(tt);
			  }
    			}
    			itsMappers.grid(*vb, dopsf, (refim::FTMachine::Type)datacol_p);
			
			cohDone += vb->nRows();
			pm.update(Double(cohDone));
		    }
    		}
    	}

	// cerr << "VI2 data: " << cohDone << endl;
	// exit(0);
    	//cerr << "IN SYNTHE_IMA" << endl;
    	//VisModelData::listModel(rvi_p->getMeasurementSet());
	SynthesisUtilMethods::getResource("Before finalize for all mappers");
    	if(!dopsf) itsMappers.finalizeDegrid(*vb);
    	itsMappers.finalizeGrid(*vb, dopsf);

    }

    itsMappers.checkOverlappingModels("restore");

    unlockMSs();

    SynthesisUtilMethods::getResource("End Major Cycle");

  }// end runMajorCycle

 
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /// The mapper loop is outside the data iterator loop.
  /// This is for cases where the image size is large compared to the RAM and
  /// where data I/O is the relatively minor cost.
  void SynthesisImagerVi2::runMajorCycle2(const Bool dopsf, 
				      const Bool savemodel)
  {
    LogIO os( LogOrigin("SynthesisImagerVi2","runMajorCycle2",WHERE) );

    //    cout << "Savemodel : " << savemodel << "   readonly : " << readOnly_p << "   usescratch : " << useScratch_p << endl;

    Bool savemodelcolumn = savemodel && !readOnly_p && useScratch_p;
    Bool savevirtualmodel = savemodel && !readOnly_p && !useScratch_p;

    if( savemodelcolumn ) os << "Saving model column" << LogIO::POST;
    if( savevirtualmodel ) os << "Saving virtual model" << LogIO::POST;

    itsMappers.checkOverlappingModels("blank");

    Bool resetModel=False;
    if( savemodelcolumn && writeAccess_p)
      {
	resetModel=True;
	os << "Iterating through the model column to reset it to zero" << LogIO::POST;
	vi::VisBuffer2* vb=vi_p->getVisBuffer();
    	vi_p->originChunks();
    	vi_p->origin();
	Double numcoh=0;
	for (uInt k=0; k< mss_p.nelements(); ++k)
	  numcoh+=Double(mss_p[k]->nrow());
	ProgressMeter pm(1.0, numcoh, 
			 dopsf?"Seting model column to zero":"pre-Major Cycle", "","","",True);
        rownr_t cohDone=0;
    	for (vi_p->originChunks(); vi_p->moreChunks();vi_p->nextChunk())
	  {
	    
	    for (vi_p->origin(); vi_p->more(); vi_p->next())
	      {
		if (SynthesisUtilMethods::validate(*vb)!=SynthesisUtilMethods::NOVALIDROWS)
		  {
		    { Cube<Complex> mod(vb->nCorrelations(), vb->nChannels(), vb->nRows(), Complex(0.0));
			    vb->setVisCubeModel(mod); 
		    }
		    vi_p->writeVisModel(vb->visCubeModel());
		    
		  }
		cohDone += vb->nRows();;
		pm.update(Double(cohDone));
	      }
	  }
      }// setting model to zero

    //Need to inialize the the forward ft machine to save the virtual model on first pass of each ms.
    if(!dopsf && savevirtualmodel){
      vi::VisBuffer2* vb=vi_p->getVisBuffer();
      vi_p->originChunks();
      vi_p->origin();
      itsMappers.initializeDegrid(*vb, -1);
    }
    
    for(Int gmap=0;gmap<itsMappers.nMappers();gmap++)
       {
	 os << "Running major cycle for chunk : " << gmap << LogIO::POST;

	 SynthesisUtilMethods::getResource("Start Major Cycle for mapper"+String::toString(gmap));
	 CountedPtr<vi::FrequencySelections> copyFsels=fselections_p->clone();
	 ///CAS-12132  create a new visiter for each chunk
	 createVisSet(writeAccess_p);
	 ////////////////////////
	 vi::VisBuffer2* vb=vi_p->getVisBuffer();
	 /// Careful where tunechunk 
	 tuneChunk(gmap);

	 vi_p->originChunks();
	 vi_p->origin();

	 Double numcoh=0;
	 for (uInt k=0; k< mss_p.nelements(); ++k)
	   numcoh+=Double(mss_p[k]->nrow());


	 ProgressMeter pm(1.0, numcoh, 
			  dopsf?"Gridding Weights and PSF":"Major Cycle", "","","",true);
	rownr_t cohDone=0;


	itsMappers.getFTM2(gmap, False)->reset();
	itsMappers.getFTM2(gmap, True)->reset();

    	if(!dopsf){
	  itsMappers.initializeDegrid(*vb, gmap);
		  //itsMappers.getMapper(gmap)->initializeDegrid(*vb);
	}
	itsMappers.initializeGrid(*vi_p,dopsf, gmap);
		//itsMappers.getMapper(gmap)->initializeGrid(*vb,dopsf);

	SynthesisUtilMethods::getResource("After initialize for mapper"+String::toString(gmap));
	Int iterNum=0;

	
    	for (vi_p->originChunks(); vi_p->moreChunks();vi_p->nextChunk())
    	{

	  for (vi_p->origin(); vi_p->more(); vi_p->next())
	    {
	      //if (SynthesisUtilMethods::validate(*vb)==SynthesisUtilMethods::NOVALIDROWS) break; // No valid rows in this VB
	      //		  cerr << "nRows "<< vb->nRow() << "   " << max(vb->visCube()) <<  endl;
	      if (SynthesisUtilMethods::validate(*vb)!=SynthesisUtilMethods::NOVALIDROWS)
		{
		  
		  if(!dopsf) {
		    if(resetModel==False) 
		      { 
			Cube<Complex> mod(vb->nCorrelations(), vb->nChannels(), vb->nRows(), Complex(0.0));
			vb->setVisCubeModel(mod); 
		      }
		    itsMappers.degrid(*vb, savevirtualmodel, gmap );
		    //itsMappers.getMapper(gmap)->degrid(*vb); //, savevirtualmodel );
		    if(savemodelcolumn && writeAccess_p ){
		      vi_p->writeVisModel(vb->visCubeModel());
		      //vi_p->writeBackChanges(vb);
		      // static_cast<VisibilityIteratorImpl2 *> (vi_p->getImpl())->writeVisModel(vb->visCubeModel());
		    }

		  }
		  itsMappers.grid(*vb, dopsf, (refim::FTMachine::Type)(datacol_p), gmap);
		  //itsMappers.getMapper(gmap)->grid(*vb, dopsf, datacol_p);
		  cohDone += vb->nRows();
		  ++iterNum;
		  pm.update(Double(cohDone));
		}
	    }
    	}
    	//cerr << "IN SYNTHE_IMA" << endl;
    	//VisModelData::listModel(rvi_p->getMeasurementSet());

	SynthesisUtilMethods::getResource("Before finalize for mapper"+String::toString(gmap));
	
    	if(!dopsf) 
	  {
	    itsMappers.finalizeDegrid(*vb,gmap);
	    //itsMappers.getMapper(gmap)->finalizeDegrid();
	  }
	itsMappers.finalizeGrid(*vb, dopsf,gmap);
    	//itsMappers.getMapper(gmap)->finalizeGrid(*vb, dopsf);
	
	//	itsMappers.getMapper(gmap)->releaseImageLocks();
	itsMappers.getMapper(gmap)->imageStore()->releaseComplexGrids();        
	
	SynthesisUtilMethods::getResource("End Major Cycle for mapper"+String::toString(gmap));
	fselections_p=copyFsels;
       }// end of mapper loop
    ///CAS-12132  create a new visiter for each chunk
    createVisSet(writeAccess_p);
    ////////////////////////
    //////vi_p->setFrequencySelection(*fselections_p);

    itsMappers.checkOverlappingModels("restore");

    unlockMSs();

    SynthesisUtilMethods::getResource("End Major Cycle");

  }// end runMajorCycle2

//////////////////////////////
 
  ////////////////////////////////////////////////////////////////////////////////////////////////

  bool SynthesisImagerVi2::runCubeGridding(Bool dopsf, const Record inpcontrol){

	LogIO logger(LogOrigin("SynthesisImagerVi2", "runCubeGridding", WHERE));

	  //dummy for now as init is overloaded on this signature
        int argc=1;
        char **argv=nullptr;
        casa::applicator.init ( argc, argv );
	  //For serial or master only
	if ( applicator.isController() ) {
		CubeMajorCycleAlgorithm cmc;
		//casa::applicator.defineAlgorithm(cmc);
		//Initialize everything to get the setup as serial
		{
                
			vi_p->originChunks();
			vi_p->origin();
		
		}
		///Break things into chunks for parallelization or memory availabbility
		Vector<Int> startchan;
		Vector<Int> endchan; 
		Int numchunks;
		Int fudge_factor = 15;
		if ((itsMappers.getFTM2(0))->name()=="MosaicFTNew") {
			fudge_factor = 20;
		}
		else if ((itsMappers.getFTM2(0))->name()=="GridFT") {
			fudge_factor = 9;
		}
                TcleanProcessingInfo procInfo;
		std::tie(procInfo, startchan, endchan)=nSubCubeFitInMemory(fudge_factor, itsMaxShape, gridpars_p.padding);
                numchunks = procInfo.chnchnks;
		////TESTOO
		//numchunks=2;
		//startchan.resize(2);startchan[0]=0; startchan[1]=2;
		//endchan.resize(2); endchan[0]=1; endchan[1]=2;
		
		/////END TESTOO
		//cerr << "NUMCHUNKS " << numchunks << " start " <<  startchan << " end " << endchan << endl;
		Record controlRecord=inpcontrol;
		//For now just field 0 but should loop over all
		///This is to pass in explicit model, residual names etc
		controlRecord.define("nfields", Int(imparsVec_p.nelements()));
        //CountedPtr<SIImageStore> imstor = imageStore ( 0 );
        // checking that psf,  residual and sumwt is allDone
        //cerr << "shapes "  <<  imstor->residual()->shape() <<  " " <<  imstor->sumwt()->shape() <<  endl;
		if(!dopsf){
		        
		  //controlRecord.define("lastcycle",  savemodel);
		  controlRecord.define("nmajorcycles", nMajorCycles);
			Vector<String> modelnames(Int(imparsVec_p.nelements()),"");
			for(uInt k=0; k < imparsVec_p.nelements(); ++k){
				Int imageStoreId=k;
				if(k>0){
					if(gridparsVec_p[0].chanchunks >1 && uInt(itsMappers.nMappers()) >=    (gridparsVec_p[0].chanchunks + gridparsVec_p.nelements()))
						imageStoreId+=gridparsVec_p[0].chanchunks-1;
					if(gridparsVec_p[0].facets >1)
						imageStoreId+=gridparsVec_p[0].facets*gridparsVec_p[0].facets-1;
				}
				if((itsMappers.imageStore(imageStoreId))->hasModel()){
					modelnames(k)=imparsVec_p[k].imageName+".model";
					(itsMappers.imageStore(k))->model()->unlock();
					controlRecord.define("modelnames", modelnames);
				}
			}
			
		}
		Vector<String> weightnames(Int(imparsVec_p.nelements()),"");
                Vector<String> pbnames(Int(imparsVec_p.nelements()),"");
		for(uInt k=0; k < imparsVec_p.nelements(); ++k){
			Int imageStoreId=k;
			if(k>0){
					if(gridparsVec_p[0].chanchunks >1 && uInt(itsMappers.nMappers()) >=    (gridparsVec_p[0].chanchunks + gridparsVec_p.nelements()))
						imageStoreId+=gridparsVec_p[0].chanchunks-1;
					if(gridparsVec_p[0].facets >1)
						imageStoreId+=gridparsVec_p[0].facets*gridparsVec_p[0].facets-1;
				}
			//cerr << "FTMachine name " << (itsMappers.getFTM2(imageStoreId))->name() << endl;
			if((itsMappers.getFTM2(imageStoreId))->useWeightImage()){
                          //cerr << "Mosaic weight image " << max(itsMappers.imageStore(imageStoreId)->weight(k)->get()) << endl;
				weightnames(k)=itsMappers.imageStore(imageStoreId)->weight(k)->name();
                                
                                
			}
			if(itsMakeVP)
			  pbnames(k)=itsMappers.imageStore(imageStoreId)->pb(k)->name();
		}
		controlRecord.define("weightnames", weightnames);
                controlRecord.define("pbnames", pbnames);
        // Tell the child processes not to do the dividebyweight process as this is done
		// tell each child to do the normars stuff
		controlRecord.define("dividebyweight",  True);
                controlRecord.defineRecord("normpars", normpars_p); 
		///Let's see if no per chan weight density was chosen
		String weightdensityimage=getWeightDensity();
		if(weightdensityimage != "")
			controlRecord.define("weightdensity", weightdensityimage);
        
		Record vecSelParsRec, vecImParsRec, vecGridParsRec;
		Vector<Int>vecRange(2);
		for (uInt k = 0; k < dataSel_p.nelements(); ++k) {
			Record selparsRec = dataSel_p[k].toRecord();
			vecSelParsRec.defineRecord(String::toString(k), selparsRec);
		}
		for (uInt k=0; k < imparsVec_p.nelements(); ++k){
			Record imparsRec = imparsVec_p[k].toRecord();
			//need to send polrep
			imparsRec.define("polrep", Int((itsMappers.imageStore(k))->getDataPolFrame()));
                        //need to send movingSourceName if any
                        imparsRec.define("movingsource", movingSource_p);
			Record gridparsRec = gridparsVec_p[k].toRecord();
			/* Might need this to pass the state of the global ftmachines...test for parallel when needed
			String err;
			Record ftmrec, iftmrec;
			if(!( (itsMappers.getFTM2(k)->toRecord(err, iftmrec,False)) && (itsMappers.getFTM2(k, false)->toRecord(err, ftmrec,False))))
				throw(AipsError("FTMachines serialization failed"));
			gridparsRec.defineRecord("ftmachine", ftmrec);
			gridparsRec.defineRecord("iftmachine", iftmrec);
			*/
			vecImParsRec.defineRecord(String::toString(k), imparsRec);
			vecGridParsRec.defineRecord(String::toString(k), gridparsRec);
		}
		String workingdir="";
		//Int numchan=(dopsf) ? imstor->psf()->shape()[3] : imstor->residual()->shape() [3];
		//copy the imageinfo of main image here
		if(dopsf)
		  cubePsfImageInfo_p=(itsMappers.imageStore(0)->psf())->imageInfo();
        for(Int k=0; k < itsMappers.nMappers(); ++k){
	 
			if(dopsf){
			   
				for(uInt j =0; j <(itsMappers.imageStore(k)->getNTaylorTerms(true)); ++j){
                                  ///TESTOO
                                  //(itsMappers.imageStore(k))->psf(j)->set(0.0);
                                  /////////
					(itsMappers.imageStore(k))->psf(j)->unlock();
				}
			}
			else{
				for(uInt j =0; j <(itsMappers.imageStore(k)->getNTaylorTerms(false)); ++j){
                                  /////////TESTOO
                                  //(itsMappers.imageStore(k))->residual(j)->set(0.0);
                                    ///////
					(itsMappers.imageStore(k))->residual(j)->unlock();
				}
			}
			for(uInt j =0; j <(itsMappers.imageStore(k)->getNTaylorTerms(true)); ++j){
			//cerr << k << " type " << (itsMappers.imageStore(k))->sumwt(j)->imageType() << " name " << (itsMappers.imageStore(k))->sumwt(j)->name() << endl;
				
			       
				Path namewgt( (itsMappers.imageStore(k))->sumwt(j)->name());
				workingdir=namewgt.dirName();
                                ///TESTOO
                                //(itsMappers.imageStore(k))->sumwt(j)->set(0.0);
                                ////
				(itsMappers.imageStore(k))->sumwt(j)->unlock();
				(itsMappers.imageStore(k))->releaseLocks();
			}
	}		
		//Send the working directory as the child and master may be at different places
		
		controlRecord.define("workingdirectory", workingdir);
		// For now this contains lastcycle if necessary in the future this
		// should come from the master control record
        
                //Int numprocs = applicator.numProcs();
        //cerr << "Number of procs: " << numprocs << endl;
        Int rank ( 0 );
        Bool assigned; //(casa::casa::applicator.nextAvailProcess(pwrite, rank));
        Bool allDone ( false );
        Vector<Bool> retvals(numchunks, False);
        Int indexofretval=0;
        for ( Int k=0; k < numchunks; ++k ) {
            assigned=casa::applicator.nextAvailProcess ( cmc, rank );
            //cerr << "assigned "<< assigned << endl;
            while ( !assigned ) {
                rank = casa::applicator.nextProcessDone ( cmc, allDone );
                //cerr << "while rank " << rank << endl;
                Bool status;
		Record returnRec;
		casa::applicator.get(returnRec);
                casa::applicator.get ( status );
                retvals(indexofretval)=status;
		if(dopsf)
		  updateImageBeamSet(returnRec);
		if(returnRec.isDefined("tempfilenames")){
		  std::vector<String> b=returnRec.asArrayString("tempfilenames").tovector();
		  tempFileNames_p.insert(std::end(tempFileNames_p), std::begin(b), std::end(b));
		}
		  
                ++indexofretval;
                if ( status )
                  //cerr << k << " rank " << rank << " successful " << endl;
                  cerr << "" ;
                else
                    logger << k << " rank " << rank << " failed " << LogIO::SEVERE;
                assigned = casa::applicator.nextAvailProcess ( cmc, rank );

            }

            ///send process info
            // put data sel params #1
            applicator.put ( vecSelParsRec );
            // put image sel params #2
            applicator.put ( vecImParsRec );
            // put gridders params #3
            applicator.put ( vecGridParsRec );
            // put which channel to process #4
			vecRange(0)=startchan(k);
			vecRange(1)=endchan(k);
            applicator.put ( vecRange );
            // psf or residual CubeMajorCycleAlgorithm #5
            applicator.put ( dopsf );
            // store modelvis and other controls #6
            applicator.put ( controlRecord );
			// weighting scheme #7
			applicator.put( weightParams_p);
            /// Tell worker to process it
            applicator.apply ( cmc );

        }
        // Wait for all outstanding processes to return
        rank = casa::applicator.nextProcessDone ( cmc, allDone );
        while ( !allDone ) {
            Bool status;
	    Record returnRec;
	    casa::applicator.get(returnRec);
            casa::applicator.get ( status );
	    if(dopsf)
	      updateImageBeamSet(returnRec);
	    if(returnRec.isDefined("tempfilenames")){
	      std::vector<String> b=returnRec.asArrayString("tempfilenames").tovector();
	      tempFileNames_p.insert(std::end(tempFileNames_p), std::begin(b), std::end(b));
	    }
            retvals(indexofretval)=status;
            ++indexofretval;
            if ( status )
              //cerr << "remainder rank " << rank << " successful " << endl;
              cerr << "";
            else
                logger << "remainder rank " << rank << " failed " << LogIO::SEVERE;

            rank = casa::applicator.nextProcessDone ( cmc, allDone );
			if(casa::applicator.isSerial())
				allDone=true;
        }
        if(anyEQ(retvals, False)){
          //cerr << retvals << endl;
          ostringstream oss;
          oss << "One or more  of the cube section failed in de/gridding. Return values for "
              "the sections: " << retvals;
          throw(AipsError(oss));
        }
        if(!dopsf && normpars_p.isDefined("pblimit") && (normpars_p.asFloat("pblimit") > 0.0) ){
          try{
	    SIImageStore::copyMask(itsMappers.imageStore(0)->pb(), itsMappers.imageStore(0)->residual());
          }
          catch(AipsError &x) {
            if(!String(x.getMesg()).contains("T/F"))
              throw(AipsError(x.getMesg()));
	    else{
	      logger << "Error : " << x.getMesg() << LogIO::WARN << LogIO::POST;
	      //cout << "x.getMesg() " << endl;
	    }
            ///ignore copy mask error and proceed as this happens with interactive
          }
        }
	else{
	  LatticeLocker lock1 (*(itsMappers.imageStore(0)->psf()), FileLocker::Write);
	  itsMappers.imageStore(0)->psf()->setImageInfo(cubePsfImageInfo_p);
	  itsMappers.imageStore(0)->psf()->unlock();
	}

        }  
	  
  
	return true;
  
  }
  /////////////////////////
  void SynthesisImagerVi2::predictModel(){
    LogIO os( LogOrigin("SynthesisImagerVi2","predictModel ",WHERE) );

    os << "---------------------------------------------------- Predict Model ---------------------------------------------" << LogIO::POST;
    
    Bool savemodelcolumn = !readOnly_p && useScratch_p;
    Bool savevirtualmodel = !readOnly_p && !useScratch_p;
    //os<<"PREDICTMODEL: readOnly_p=="<<readOnly_p<<" useScratch_p=="<<useScratch_p<<LogIO::POST;
    if( savemodelcolumn ) os << "Saving model column" << LogIO::POST;
    if( savevirtualmodel ) os << "Saving virtual model" << LogIO::POST;

    itsMappers.checkOverlappingModels("blank");


    {
      vi::VisBuffer2* vb = vi_p->getVisBuffer();;
      vi_p->originChunks();
      vi_p->origin();
      Double numberCoh=0;
      for (uInt k=0; k< mss_p.nelements(); ++k)
	numberCoh+=Double(mss_p[k]->nrow());

      ProgressMeter pm(1.0, numberCoh, "Predict Model", "","","",true);
      rownr_t cohDone=0;

      itsMappers.initializeDegrid(*vb);
      for (vi_p->originChunks(); vi_p->moreChunks();vi_p->nextChunk())
	{
	  
	  for (vi_p->origin(); vi_p->more(); vi_p->next())
	    {
	      //if (SynthesisUtilMethods::validate(*vb)==SynthesisUtilMethods::NOVALIDROWS) break; //No valid rows in this MS
	      //if !usescratch ...just save
	      vb->setVisCubeModel(Complex(0.0, 0.0));
	      itsMappers.degrid(*vb, savevirtualmodel);

	      if(savemodelcolumn && writeAccess_p ){

		const_cast<MeasurementSet& >((vi_p->ms())).lock(true);

		vi_p->writeVisModel(vb->visCubeModel());

	      //cerr << "nRows "<< vb->nRows() << "   " << max(vb->visCubeModel()) <<  endl;
		const_cast<MeasurementSet& >((vi_p->ms())).unlock();
	      }

	      cohDone += vb->nRows();
	      pm.update(Double(cohDone));

	    }
	}
      itsMappers.finalizeDegrid(*vb);
    }

    itsMappers.checkOverlappingModels("restore");
    itsMappers.releaseImageLocks();
    unlockMSs();
   
  }// end of predictModel

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SynthesisImagerVi2::makeSdImage(Bool dopsf)
  {
    LogIO os( LogOrigin("SynthesisImagerVi2","makeSdImage",WHERE) );

//    Bool dopsf=false;
    if(datacol_p==FTMachine::PSF) dopsf=true;

    {
      vi::VisBuffer2* vb = vi_p->getVisBuffer();;
      vi_p->originChunks();
      vi_p->origin();

      Double numberCoh=0;
      for (uInt k=0; k< mss_p.nelements(); ++k)
        numberCoh+=Double(mss_p[k]->nrow());

      ProgressMeter pm(1.0, numberCoh, "Predict Model", "","","",true);
      rownr_t cohDone=0;

      itsMappers.initializeGrid(*vi_p,dopsf);
      for (vi_p->originChunks(); vi_p->moreChunks(); vi_p->nextChunk())
      {

        for (vi_p->origin(); vi_p->more(); vi_p->next())
        {
          itsMappers.grid(*vb, dopsf, (refim::FTMachine::Type)datacol_p);
          cohDone += vb->nRows();
          pm.update(Double(cohDone));
        }
      }
      itsMappers.finalizeGrid(*vb, dopsf);

    }

    unlockMSs();

  }// end makeSDImage
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SynthesisImagerVi2::makeImage(String type, const String& imagename, const String& complexImage, const Int whichModel)
  {
    LogIO os( LogOrigin("SynthesisImager","makeImage",WHERE) );

    
    refim::FTMachine::Type seType(refim::FTMachine::OBSERVED);
    if(type=="observed") {
      seType=refim::FTMachine::OBSERVED;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making dirty image from " << type << " data "
	 << LogIO::POST;
    }
    else if (type=="model") {
      seType=refim::FTMachine::MODEL;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making dirty image from " << type << " data "
	 << LogIO::POST;
    }
    else if (type=="corrected") {
      seType=refim::FTMachine::CORRECTED;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making dirty image from " << type << " data "
	 << LogIO::POST;
    }
    else if (type=="psf") {
      seType=refim::FTMachine::PSF;
      os << "Making point spread function "
	 << LogIO::POST;
    }
    else if (type=="residual") {
      seType=refim::FTMachine::RESIDUAL;
      os << LogIO::NORMAL // Loglevel INFO
         << "Making dirty image from " << type << " data "
	 << LogIO::POST;
    }
    else if (type=="singledish-observed") {
      seType=refim::FTMachine::OBSERVED;
      os << LogIO::NORMAL 
         << "Making single dish image from observed data" << LogIO::POST;
    }
    else if (type=="singledish") {
      seType=refim::FTMachine::CORRECTED;
      os << LogIO::NORMAL 
         << "Making single dish image from corrected data" << LogIO::POST;
    }
    else if (type=="coverage") {
      seType=refim::FTMachine::COVERAGE;
      os << LogIO::NORMAL 
         << "Making single dish coverage function "
	 << LogIO::POST;
    }
    else if (type=="holography") {
      seType=refim::FTMachine::CORRECTED;
      os << LogIO::NORMAL
         << "Making complex holographic image from corrected data "
	 << LogIO::POST;
    }
    else if (type=="holography-observed") {
      seType=refim::FTMachine::OBSERVED;
      os << LogIO::NORMAL 
         << "Making complex holographic image from observed data "
	 << LogIO::POST;
    }

    String imageName=(itsMappers.imageStore(whichModel))->getName();
    String cImageName(complexImage);
    if(complexImage=="") {
      cImageName=imageName+".compleximage";
    }
    Bool keepComplexImage=(complexImage!="")||(type.contains("holography"));
    //cerr << "name " << (itsMappers.getFTM2(whichModel))->name() << endl;
 if(((itsMappers.getFTM2(whichModel))->name())!="MultiTermFTNew"){
   ////Non multiterm case    
	//cerr << "whichModel " << whichModel << " itsMappers num " << itsMappers.nMappers() << " shape " << (itsMappers.imageStore(whichModel))->getShape() << endl;
	///the SIImageStore sometimes return 0 shape...
	CoordinateSystem csys=itsMaxCoordSys;
	IPosition shp=itsMaxShape;
	if((itsMappers.imageStore(whichModel))->getShape().product()!=0 ){
		csys=	(itsMappers.imageStore(whichModel))->getCSys();
		shp=(itsMappers.imageStore(whichModel))->getShape();
	}
	itsMappers.releaseImageLocks();
    PagedImage<Float> theImage( shp, csys, imagename);
    PagedImage<Complex> cImageImage(theImage.shape(),
				    theImage.coordinates(),
				    cImageName);
    if(!keepComplexImage)
      cImageImage.table().markForDelete();


    Matrix<Float> weight;
    if(cImageImage.shape()[3] > 1){
		cImageImage.set(Complex(0.0));
		cImageImage.tempClose();
		makeComplexCubeImage(cImageName, seType, whichModel);
	}
	else{
    (itsMappers.getFTM2(whichModel))->makeImage(seType, *vi_p, cImageImage, weight);
	}
    if(seType==refim::FTMachine::PSF){
       StokesImageUtil::ToStokesPSF(theImage, cImageImage);
       StokesImageUtil::normalizePSF(theImage);
    }
    else{
      StokesImageUtil::To(theImage, cImageImage);
    }
 }
 else{
   ///Multiterm
   //refim::MultiTermFTNew *theft=static_cast<refim::MultiTermFTNew *>( (itsMappers.getFTM2(whichModel))->cloneFTM());
   refim::MultiTermFTNew *theft=static_cast<refim::MultiTermFTNew *>( (itsMappers.getFTM2(whichModel)).get());
   Int ntaylor= seType==refim::FTMachine::PSF ? theft->psfNTerms() : theft->nTerms();
   if(ntaylor<2)
     throw(AipsError("some issue with muti term setting "));
   Vector<CountedPtr<ImageInterface<Float> > >theImage(ntaylor);
   Vector<CountedPtr<ImageInterface<Complex> > >cImageImage(ntaylor);
   Vector<CountedPtr<Matrix<Float> > >weight(ntaylor);
   for (Int taylor=0; taylor < ntaylor; ++taylor){
     theImage[taylor]=new PagedImage<Float>((itsMappers.imageStore(whichModel))->getShape(), (itsMappers.imageStore(whichModel))->getCSys(), imagename+".tt"+String::toString(taylor));
     cImageImage[taylor]=new PagedImage<Complex> (theImage[taylor]->shape(),
						  theImage[taylor]->coordinates(),
						  cImageName+".tt"+String::toString(taylor));
      if(!keepComplexImage)
	static_cast<PagedImage<Complex> *> (cImageImage[taylor].get())->table().markForDelete();
      weight[taylor]=new Matrix<Float>();

   }
   theft->makeMTImages(seType, *vi_p, cImageImage, weight);
   Float maxpsf=1.0;
   for (Int taylor=0; taylor < ntaylor; ++taylor){
     if(seType==refim::FTMachine::PSF){
       StokesImageUtil::ToStokesPSF(*(theImage[taylor]), *(cImageImage[taylor]));
       if(taylor==0){
	 maxpsf=StokesImageUtil::normalizePSF(*theImage[taylor]);
	 //cerr << "maxpsf " << maxpsf << endl;
       }
       else{
	 ///divide by max;
	 (*theImage[taylor]).copyData((LatticeExpr<Float>)((*theImage[taylor])/maxpsf));
       }
    }
    else{
      StokesImageUtil::To(*(theImage[taylor]), *(cImageImage[taylor]));
    }
   }
   //delete theft;
     
 }
    unlockMSs();

  }// end makeImage
  /////////////////////////////////////////////////////

void SynthesisImagerVi2::makeComplexCubeImage(const String& cimage, const refim::FTMachine::Type imtype, const Int whichmodel){
	CubeMakeImageAlgorithm *cmi=new CubeMakeImageAlgorithm();
	// Dummies for using the right signature for init
	Path cimpath(cimage);
	String cimname=cimpath.absoluteName();
	//cerr << "ABSOLUTE path " << cimname << endl;
	Int argc = 1;
	char **argv = nullptr;
	casa::applicator.init(argc, argv);
	if(applicator.isController())
    {
		Record vecSelParsRec;
		for (uInt k = 0; k < dataSel_p.nelements(); ++k) {
			Record selparsRec = dataSel_p[k].toRecord();
			vecSelParsRec.defineRecord(String::toString(k), selparsRec);
		}
		Record imparsRec = imparsVec_p[whichmodel].toRecord();
		//cerr << "which model " << whichmodel << " record " << imparsRec << endl;
		Record gridparsRec = gridparsVec_p[whichmodel].toRecord();
        ///Break things into chunks for parallelization or memory availabbility
        Vector<Int> startchan;
        Vector<Int> endchan;
        Int numchunks;
        Int fudge_factor = 15;
        if ((itsMappers.getFTM2(0))->name()=="MosaicFTNew") {
            fudge_factor = 20;
        }
        else if ((itsMappers.getFTM2(0))->name()=="GridFT") {
            fudge_factor = 9;
        }
        TcleanProcessingInfo  procInfo;
        std::tie(procInfo, startchan, endchan)=nSubCubeFitInMemory(fudge_factor, itsMaxShape, gridpars_p.padding);
        numchunks = procInfo.chnchnks;
        
		Int imageType=Int(imtype);
		Int rank(0);
		Bool assigned; 
		Bool allDone(false);
		Vector<Int> chanRange(2);
		for (Int k=0; k < numchunks; ++k) {
			assigned=casa::applicator.nextAvailProcess ( *cmi, rank );
            //cerr << "assigned "<< assigned << endl;
            while ( !assigned ) {
                rank = casa::applicator.nextProcessDone ( *cmi, allDone );
                //cerr << "while rank " << rank << endl;
                Bool status;
                casa::applicator.get(status);
                /*if ( status )
                  cerr << k << " rank " << rank << " successful " << endl;
                else
                    cerr << k << " rank " << rank << " failed " << endl;
                */
                assigned = casa::applicator.nextAvailProcess ( *cmi, rank );

            }
            applicator.put(vecSelParsRec);
			// put image sel params #2
			applicator.put(imparsRec);
			// put gridder params #3
			applicator.put(gridparsRec);
			// put which channel to process #4
			chanRange(0)=startchan(k);
			chanRange(1)=endchan(k);
			applicator.put(chanRange);
			//Type of image #5
			applicator.put(imageType);
			// weighting parameters #6
			applicator.put( weightParams_p);
			// complex imagename #7
			applicator.put(cimname);
			applicator.apply(*cmi);
		}
		// Wait for all outstanding processes to return
        rank = casa::applicator.nextProcessDone(*cmi, allDone);
        while (!allDone) {
            Bool status;
            casa::applicator.get(status);
            /*
            if(status)
                cerr << "remainder rank " << rank << " successful " << endl;
            else
                cerr << "remainder rank " << rank << " failed " << endl;
            */
            rank = casa::applicator.nextProcessDone(*cmi, allDone);
			if(casa::applicator.isSerial())
				allDone=true;
        }
    }//applicator controller section
}



CountedPtr<SIMapper> SynthesisImagerVi2::createSIMapper(String mappertype,  
							   CountedPtr<SIImageStore> imagestore,
							CountedPtr<refim::FTMachine> ftmachine,
							CountedPtr<refim::FTMachine> iftmachine,
						       uInt /*ntaylorterms*/)
  {
    LogIO os( LogOrigin("SynthesisImagerVi2","createSIMapper",WHERE) );
    
    CountedPtr<SIMapper> localMapper;

    try
      {
	
	if( mappertype == "default" || mappertype == "multiterm" )
	  {
	    localMapper = new SIMapper( imagestore, ftmachine, iftmachine );
	  }
	else if( mappertype == "imagemosaic") // || mappertype == "mtimagemosaic" )
	  {
	    localMapper = new SIMapperImageMosaic( imagestore, ftmachine, iftmachine );
	  }
	else
	  {
	    throw(AipsError("Unknown mapper type : " + mappertype));
	  }

      }
    catch(AipsError &x) {
	throw(AipsError("Error in createSIMapper : " + x.getMesg() ) );
      }
    return localMapper;
  }
  
void SynthesisImagerVi2::lockMS(MeasurementSet& thisms){
  thisms.lock(!readOnly_p);
    thisms.antenna().lock(false);
	thisms.dataDescription().lock(false);
    thisms.feed().lock(false);
    thisms.field().lock(false);
    thisms.observation().lock(false);
    thisms.polarization().lock(false);
    thisms.processor().lock(false);
	thisms.spectralWindow().lock(false);
    thisms.state().lock(false);
    if(!thisms.doppler().isNull())
      thisms.doppler().lock(false);
    if(!thisms.flagCmd().isNull())
      thisms.flagCmd().lock(false);
    if(!thisms.freqOffset().isNull())
      thisms.freqOffset().lock(false);
	//True here as we can write in that
    if(!thisms.history().isNull())
    // thisms.history().lock(!readOnly_p);
      thisms.history().lock(false);
    if(!thisms.pointing().isNull())
      thisms.pointing().lock(false);
	//we write virtual model here
    if(!thisms.source().isNull())
      thisms.source().lock(!readOnly_p);
    if(!thisms.sysCal().isNull())
      thisms.sysCal().lock(false);
    if(!thisms.weather().isNull())
      thisms.weather().lock(false);
	
}
///////////////////////
  
  Vector<SynthesisParamsSelect> SynthesisImagerVi2::tuneSelectData(){
    vi_p->originChunks();
    vi_p->origin();
    vi::VisBuffer2* vb=vi_p->getVisBuffer();
    Int spwnow=vb->spectralWindows()[0];
    Int nchaninms=MSColumns(vb->ms()).spectralWindow().numChan()(spwnow);
    //For some small number of channels in the ms vi/vb2 retuning the selection
    //will sometimes return more channels than what is in the ms...this is a
    //kludge here to bypass it...mostly seen in test_tclean
    /// write to the test !!  till someboody fixes this is vi2 or wait for cngi
    //if savescratch column we have tune...otherwise some channel may be 0
    // when chunking or in parallel
    if(nchaninms <30 && !(!readOnly_p && useScratch_p))
      return dataSel_p;
    
    return SynthesisImager::tuneSelectData();
  }
//////////////////////
  void SynthesisImagerVi2::lockMSs(){
    for(uInt i=0;i<mss_p.nelements();i++)
      { 
       
	MeasurementSet *ms_l = 	const_cast<MeasurementSet* >(mss_p[i]);
	lockMS(*ms_l);
      }

  }
void SynthesisImagerVi2::unlockMSs()
  {
    LogIO os( LogOrigin("SynthesisImagerVi2","unlockMSs",WHERE) );
    for(uInt i=0;i<mss_p.nelements();i++)
      { 
	os << LogIO::NORMAL2 << "Unlocking : " << (mss_p[i])->tableName() << LogIO::POST;
	MeasurementSet *ms_l = 	const_cast<MeasurementSet* >(mss_p[i]);
	ms_l->unlock(); 
	ms_l->antenna().unlock();
	ms_l->dataDescription().unlock();
	ms_l->feed().unlock();
	ms_l->field().unlock();
	ms_l->observation().unlock();
	ms_l->polarization().unlock();
	ms_l->processor().unlock();
	ms_l->spectralWindow().unlock();
	ms_l->state().unlock();
	//
	// Unlock the optional sub-tables as well, if they are present
	//
	if(!(ms_l->source().isNull()))     ms_l->source().unlock();
	if(!(ms_l->doppler().isNull()))    ms_l->doppler().unlock();
	if(!(ms_l->flagCmd().isNull()))    ms_l->flagCmd().unlock();
	if(!(ms_l->freqOffset().isNull())) ms_l->freqOffset().unlock();
	if(!(ms_l->history().isNull()))    ms_l->history().unlock();
	if(!(ms_l->pointing().isNull()))   ms_l->pointing().unlock();
	if(!(ms_l->sysCal().isNull()))     ms_l->sysCal().unlock();
	if(!(ms_l->weather().isNull()))    ms_l->weather().unlock();
      }
  }
  void SynthesisImagerVi2::createFTMachine(CountedPtr<refim::FTMachine>& theFT, 
					   CountedPtr<refim::FTMachine>& theIFT, 
					   const String& ftname,
					   const uInt nTaylorTerms,
					   const String mType,
					   const Int facets,            //=1
					   //------------------------------
					   const Int wprojplane,        //=1,
					   const Float padding,         //=1.0,
					   const Bool useAutocorr,      //=false,
					   const Bool useDoublePrec,    //=true,
					   const String gridFunction,   //=String("SF"),
					//------------------------------
					   const Bool aTermOn,          //= true,
					   const Bool psTermOn,         //= true,
					   const Bool mTermOn,          //= false,
					const Bool wbAWP,            //= true,
					   const String cfCache,        //= "",
					   const Bool usePointing,       //= false,
					   // const Vector<Float> pointingOffsetSigDev, //= 10.0,
					   const vector<float> pointingOffsetSigDev,// = {10,10}
					   const Bool doPBCorr,         //= true,
					   const Bool conjBeams,        //= true,
					const Float computePAStep,         //=360.0
					   const Float rotatePAStep,          //=5.0
					   const String interpolation,  //="linear"
					   const Bool freqFrameValid, //=true
					   const Int cache,             //=1000000000,
					   const Int tile,               //=16
					   const String stokes, //=I
					   const String imageNamePrefix,
					   //---------------------------
					   const String &pointingDirCol,
					   const Float skyPosThreshold,
					   const Int convSupport,
					   const Quantity &truncateSize,
					   const Quantity &gwidth,
					   const Quantity &jwidth,
					   const Float minWeight,
					   const Bool clipMinMax,
					   const Bool pseudoI
					   )

  {
    LogIO os( LogOrigin("SynthesisImagerVi2","createFTMachine",WHERE));

    if(ftname=="gridft"){
      if(facets >1){
	theFT=new refim::GridFT(cache, tile, gridFunction, mLocation_p, phaseCenter_p, padding, useAutocorr, useDoublePrec);
	theIFT=new refim::GridFT(cache, tile, gridFunction, mLocation_p, phaseCenter_p, padding, useAutocorr, useDoublePrec);

      }
      else{
	theFT=new refim::GridFT(cache, tile, gridFunction, mLocation_p, padding, useAutocorr, useDoublePrec);
	theIFT=new refim::GridFT(cache, tile, gridFunction, mLocation_p, padding, useAutocorr, useDoublePrec);
      }
    }
    else if(ftname== "wprojectft"){
     Double maxW=-1.0;
     Double minW=-1.0;
     Double rmsW=-1.0;
     if(wprojplane <1)
       casa::refim::WProjectFT::wStat(*vi_p, minW, maxW, rmsW);
    if(facets >1){
      theFT=new refim::WProjectFT(wprojplane,  phaseCenter_p, mLocation_p,
			   cache/2, tile, useAutocorr, padding, useDoublePrec, minW, maxW, rmsW);
      theIFT=new refim::WProjectFT(wprojplane,  phaseCenter_p, mLocation_p,
			    cache/2, tile, useAutocorr, padding, useDoublePrec, minW, maxW, rmsW);
    }
    else{
      theFT=new refim::WProjectFT(wprojplane,  mLocation_p,
			   cache/2, tile, useAutocorr, padding, useDoublePrec, minW, maxW, rmsW);
      theIFT=new refim::WProjectFT(wprojplane,  mLocation_p,
			    cache/2, tile, useAutocorr, padding, useDoublePrec, minW, maxW, rmsW);
    }
    CountedPtr<refim::WPConvFunc> sharedconvFunc=static_cast<refim::WProjectFT &>(*theFT).getConvFunc();
      //static_cast<WProjectFT &>(*theFT).setConvFunc(sharedconvFunc);
    static_cast<refim::WProjectFT &>(*theIFT).setConvFunc(sharedconvFunc);
    }
    else if ((ftname == "awprojectft") || (ftname== "mawprojectft") || (ftname == "protoft")) {
      createAWPFTMachine(theFT, theIFT, ftname, facets, wprojplane, 
			 padding, useAutocorr, useDoublePrec, gridFunction,
			 aTermOn, psTermOn, mTermOn, wbAWP, cfCache, 
			 usePointing, pointingOffsetSigDev, doPBCorr, conjBeams, computePAStep,
			 rotatePAStep, cache,tile,imageNamePrefix);
    }
    else if ( ftname == "mosaic" || ftname== "mosft" || ftname == "mosaicft" || ftname== "MosaicFT"){

      createMosFTMachine(theFT, theIFT, padding, useAutocorr, useDoublePrec, rotatePAStep, stokes, conjBeams);
    } else if (ftname == "sd") {
      createSDFTMachine(theFT, theIFT, pointingDirCol, skyPosThreshold, doPBCorr, rotatePAStep,
          gridFunction, convSupport, truncateSize, gwidth, jwidth,
          minWeight, clipMinMax, cache, tile, stokes);
    }
    else
      {
	throw( AipsError( "Invalid FTMachine name : " + ftname ) );
      }
    /* else if(ftname== "MosaicFT"){

       }*/



    ///////// Now, clone and pack the chosen FT into a MultiTermFT if needed.
    if( mType=="multiterm" )
      {
	AlwaysAssert( nTaylorTerms>=1 , AipsError );

	CountedPtr<refim::FTMachine> theMTFT = new refim::MultiTermFTNew( theFT , nTaylorTerms, true/*forward*/ );
	CountedPtr<refim::FTMachine> theMTIFT = new refim::MultiTermFTNew( theIFT , nTaylorTerms, false/*forward*/ );

	theFT = theMTFT;
	theIFT = theMTIFT;
      }




    ////// Now, set the SkyJones if needed, and if not internally generated.
    if( mType=="imagemosaic" && 
	(ftname != "awprojectft" && ftname != "mawprojectft" && ftname != "proroft") )
      {
	CountedPtr<refim::SkyJones> vp;
	MSColumns msc(*(mss_p[0]));
	Quantity parang(0.0,"deg");
	Quantity skyposthreshold(0.0,"deg");
	vp = new refim::VPSkyJones(msc, true,  parang, BeamSquint::NONE,skyposthreshold);

	Vector<CountedPtr<refim::SkyJones> > skyJonesList(1);
	skyJonesList(0) = vp;
	theFT->setSkyJones(  skyJonesList );
	theIFT->setSkyJones(  skyJonesList );

      }

    //// For mode=cubedata, set the freq frame to invalid..
    // get this info from buildCoordSystem
    //theFT->setSpw( tspws, false );
    //theIFT->setSpw( tspws, false );
    theFT->setFrameValidity( freqFrameValid );
    theIFT->setFrameValidity( freqFrameValid );

    //// Set interpolation mode
    theFT->setFreqInterpolation( interpolation );
    theIFT->setFreqInterpolation( interpolation );

    ///Set tracking of moving source if any
    if(movingSource_p != ""){
      theFT->setMovingSource(movingSource_p);
      theIFT->setMovingSource(movingSource_p);
    }
    /* vi_p has chanselection now
    //channel selections from spw param
    theFT->setSpwChanSelection(chanSel_p);
    theIFT->setSpwChanSelection(chanSel_p);
    */

    // Set pseudo-I if requested.
    if(pseudoI==true)
    {
      os << "Turning on Pseudo-I gridding" << LogIO::POST;
      theFT->setPseudoIStokes(true);
      theIFT->setPseudoIStokes(true);
    }

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SynthesisImagerVi2::createAWPFTMachine(CountedPtr<refim::FTMachine>& theFT, CountedPtr<refim::FTMachine>& theIFT, 
					      const String&,// ftmName,
					      const Int,// facets,            //=1
					      //------------------------------
					      const Int wprojPlane,        //=1,
					      const Float,// padding,         //=1.0,
					      const Bool,// useAutocorr,      //=false,
					      const Bool useDoublePrec,    //=true,
					      const String,// gridFunction,   //=String("SF"),
					      //------------------------------
					      const Bool aTermOn,          //= true,
					      const Bool psTermOn,         //= true,
					      const Bool mTermOn,          //= false,
					      const Bool wbAWP,            //= true,
					      const String cfCache,        //= "",
					      const Bool usePointing,       //= false,
					      const vector<Float> pointingOffsetSigDev,//={10,10},
					      const Bool doPBCorr,         //= true,
					      const Bool conjBeams,        //= true,
					      const Float computePAStep,   //=360.0
					      const Float rotatePAStep,    //=5.0
					      const Int cache,             //=1000000000,
					      const Int tile,               //=16
					      const String imageNamePrefix
					      )

  {
    LogIO os( LogOrigin("SynthesisImagerVi2","createAWPFTMachine",WHERE));

    if (wprojPlane<=1)
      {
	os << LogIO::NORMAL
	   << "You are using wprojplanes=1. Doing co-planar imaging (no w-projection needed)" 
	   << LogIO::POST;
	os << LogIO::NORMAL << "Performing WBA-Projection" << LogIO::POST; // Loglevel PROGRESS
      }
    // if((wprojPlane>1)&&(wprojPlane<64)) 
    //   {
    // 	os << LogIO::WARN
    // 	   << "No. of w-planes set too low for W projection - recommend at least 128"
    // 	   << LogIO::POST;
    // 	os << LogIO::NORMAL << "Performing WBAW-Projection" << LogIO::POST; // Loglevel PROGRESS
    //   }

    // CountedPtr<ATerm> apertureFunction = createTelescopeATerm(mss4vi_p[0], aTermOn);
    // CountedPtr<PSTerm> psTerm = new PSTerm();
    // CountedPtr<WTerm> wTerm = new WTerm();
    
    // //
    // // Selectively switch off CFTerms.
    // //
    // if (aTermOn == false) {apertureFunction->setOpCode(CFTerms::NOOP);}
    // if (psTermOn == false) psTerm->setOpCode(CFTerms::NOOP);

    // //
    // // Construct the CF object with appropriate CFTerms.
    // //
    // CountedPtr<ConvolutionFunction> tt;
    // tt = AWProjectFT::makeCFObject(aTermOn, psTermOn, true, mTermOn, wbAWP);
    // CountedPtr<ConvolutionFunction> awConvFunc;
    // //    awConvFunc = new AWConvFunc(apertureFunction,psTerm,wTerm, !wbAWP);
    // if ((ftmName=="mawprojectft") || (mTermOn))
    //   awConvFunc = new AWConvFuncEPJones(apertureFunction,psTerm,wTerm,wbAWP);
    // else
    //   awConvFunc = new AWConvFunc(apertureFunction,psTerm,wTerm,wbAWP);

    MSObservationColumns msoc((mss_p[0])->observation());
    String telescopeName=msoc.telescopeName()(0);
    CountedPtr<refim::ConvolutionFunction> awConvFunc = refim::AWProjectFT::makeCFObject(telescopeName, 
									   aTermOn,
									   psTermOn, (wprojPlane > 1),
									   mTermOn, wbAWP, conjBeams);

    //
    // Construct the appropriate re-sampler.
    //
    CountedPtr<refim::VisibilityResamplerBase> visResampler;
    //    if (ftmName=="protoft") visResampler = new ProtoVR();
    //elsef
    visResampler = new refim::AWVisResampler();
    //    CountedPtr<VisibilityResamplerBase> visResampler = new VisibilityResampler();

    //
    // Construct and initialize the CF cache object.
    //


    // CountedPtr<CFCache> cfCacheObj = new CFCache();
    // cfCacheObj->setCacheDir(cfCache.data());
    // //    cerr << "Setting wtImagePrefix to " << imageNamePrefix.c_str() << endl;
    // cfCacheObj->setWtImagePrefix(imageNamePrefix.c_str());
    // cfCacheObj->initCache2();

    CountedPtr<refim::CFCache> cfCacheObj;
      

    //
    // Finally construct the FTMachine with the CFCache, ConvFunc and
    // Re-sampler objects.  
    //
    Float pbLimit_l=1e-3;

    theFT = new refim::AWProjectWBFTNew(wprojPlane, cache/2, 
			      cfCacheObj, awConvFunc, 
			      visResampler,
					/*true */usePointing, pointingOffsetSigDev ,doPBCorr, 
			      tile, computePAStep, pbLimit_l, true,conjBeams,
			      useDoublePrec);

    cfCacheObj = new refim::CFCache();
    cfCacheObj->setCacheDir(cfCache.data());
    // Get the LAZYFILL setting from the user configuration.  If not
    // found, default to False.
    //
    // With lazy fill ON, CFCache loads the required CFs on-demand
    // from the disk.  And periodically triggers garbage collection to
    // release CFs that aren't required immediately.
    cfCacheObj->setLazyFill(refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1);
    //    cerr << "Setting wtImagePrefix to " << imageNamePrefix.c_str() << endl;
    cfCacheObj->setWtImagePrefix(imageNamePrefix.c_str());
    cfCacheObj->initCache2(CFC_VERBOSE);

    theFT->setCFCache(cfCacheObj);
    

    Quantity rotateOTF(rotatePAStep,"deg");
    static_cast<refim::AWProjectWBFTNew &>(*theFT).setObservatoryLocation(mLocation_p);
    static_cast<refim::AWProjectWBFTNew &>(*theFT).setPAIncrement(Quantity(computePAStep,"deg"),rotateOTF);

    // theIFT = new AWProjectWBFT(wprojPlane, cache/2, 
    // 			       cfCacheObj, awConvFunc, 
    // 			       visResampler,
    // 			       /*true */usePointing, doPBCorr, 
    // 			       tile, computePAStep, pbLimit_l, true,conjBeams,
    // 			       useDoublePrec);

    // static_cast<AWProjectWBFT &>(*theIFT).setObservatoryLocation(mLocation_p);
    // static_cast<AWProjectWBFT &>(*theIFT).setPAIncrement(Quantity(computePAStep,"deg"),rotateOTF);

    theIFT = new refim::AWProjectWBFTNew(static_cast<refim::AWProjectWBFTNew &>(*theFT));

    os << "Sending frequency selection information " <<  mssFreqSel_p  <<  " to AWP FTM." << LogIO::POST;
    theFT->setSpwFreqSelection( mssFreqSel_p );
    theIFT->setSpwFreqSelection( mssFreqSel_p );
    

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void SynthesisImagerVi2:: createMosFTMachine(CountedPtr<refim::FTMachine>& theFT,CountedPtr<refim::FTMachine>&  theIFT, const Float /*padding*/, const Bool useAutoCorr, const Bool useDoublePrec, const Float rotatePAStep, const String stokes, const Bool doConjBeams){
    
    LogIO os(LogOrigin("SynthesisImagerVi2", "createMosFTMachine",WHERE));
   
    MSColumns msc(vi_p->ms());
    String telescop=msc.observation().telescopeName()(0);
    Bool multiTel=False;
    Int msid=0;
     for(vi_p->originChunks(); vi_p->moreChunks(); vi_p->nextChunk()){
       if(((vi_p->getVisBuffer())->msId() != msid) && telescop !=  MSColumns(vi_p->ms()).observation().telescopeName()(0)){
	 msid=(vi_p->getVisBuffer())->msId();
	 multiTel=True;
       }
     }
    vi_p->originChunks();
  
  

    PBMath::CommonPB kpb;
    Record rec;
    getVPRecord( rec, kpb, telescop );
   

    if(rec.empty()){os << LogIO::SEVERE << "Cannot proceed with mosaicft gridder without a valid PB model" << LogIO::POST; }
    
    /*
   VPManager *vpman=VPManager::Instance();
    PBMath::CommonPB kpb;
    PBMath::enumerateCommonPB(telescop, kpb);
    Record rec;
    vpman->getvp(rec, telescop);
    */

   refim::VPSkyJones* vps=NULL;
   //cerr << "rec " << rec << " kpb " << kpb << endl;
    if(rec.asString("name")=="COMMONPB" && kpb !=PBMath::UNKNOWN ){
      vps= new refim::VPSkyJones(msc, true, Quantity(rotatePAStep, "deg"), BeamSquint::GOFIGURE, Quantity(360.0, "deg"));
      /////Don't know which parameter has pb threshold cutoff that the user want 
      ////leaving at default
      ////vps.setThreshold(minPB);
      
    }
    else{
      PBMath myPB(rec);
      String whichPBMath;
      PBMathInterface::namePBClass(myPB.whichPBClass(), whichPBMath);
      os  << "Using the PB defined by " << whichPBMath << " for beam calculation for telescope " << telescop << LogIO::POST;
      vps= new refim::VPSkyJones(telescop, myPB, Quantity(rotatePAStep, "deg"), BeamSquint::GOFIGURE, Quantity(360.0, "deg"));
      //kpb=PBMath::DEFAULT;
    }
   
    theFT = new refim::MosaicFTNew(vps, mLocation_p, stokes, 1000000000, 16, useAutoCorr, 
				   useDoublePrec, doConjBeams, gridpars_p.usePointing);
    PBMathInterface::PBClass pbtype=((kpb==PBMath::EVLA) || multiTel)? PBMathInterface::COMMONPB: PBMathInterface::AIRY;
    if(rec.asString("name")=="IMAGE")
       pbtype=PBMathInterface::IMAGE;
    ///Use Heterogenous array mode for the following
    if((kpb == PBMath::UNKNOWN) || (kpb==PBMath::OVRO) || (kpb==PBMath::ACA)
       || (kpb==PBMath::ALMA) || (kpb==PBMath::EVLA) || multiTel){
      CountedPtr<refim::SimplePBConvFunc> mospb=new refim::HetArrayConvFunc(pbtype, itsVpTable);
      static_cast<refim::MosaicFTNew &>(*theFT).setConvFunc(mospb);
    }
    ///////////////////make sure both FTMachine share the same conv functions.
    theIFT= new refim::MosaicFTNew(static_cast<refim::MosaicFTNew &>(*theFT));

    
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void SynthesisImagerVi2::createSDFTMachine(CountedPtr<refim::FTMachine>& theFT,
      CountedPtr<refim::FTMachine>& theIFT,
      const String &pointingDirCol,
      const Float skyPosThreshold,
      const Bool /*doPBCorr*/,
      const Float rotatePAStep,
      const String& gridFunction,
      const Int convSupport,
      const Quantity& truncateSize,
      const Quantity& gwidth,
      const Quantity& jwidth,
      const Float minWeight,
      const Bool clipMinMax,
      const Int cache,
      const Int tile,
      const String &stokes,
      const Bool pseudoI) {
//    // member variable itsVPTable is VP table name
    LogIO os(LogOrigin("SynthesisImagerVi2", "createSDFTMachine", WHERE));
    os << LogIO::NORMAL // Loglevel INFO
       << "Performing single dish gridding..." << LogIO::POST;
    os << LogIO::NORMAL1 // gridFunction is too cryptic for most users.
       << "with convolution function " << gridFunction << LogIO::POST;

    // Now make the Single Dish Gridding
    os << LogIO::NORMAL // Loglevel INFO
       << "Gridding will use specified common tangent point:" << LogIO::POST;
    os << LogIO::NORMAL << SynthesisUtilMethods::asComprehensibleDirectionString(phaseCenter_p)
        << LogIO::POST; // Loglevel INFO
    String const gridfunclower = downcase(gridFunction);
    if(gridfunclower=="pb") {
      refim::SkyJones *vp = nullptr;
      MSColumns msc(*mss_p[0]);
      // todo: NONE is OK?
      BeamSquint::SquintType squintType = BeamSquint::NONE;
      Quantity skyPosThresholdQuant((Double)skyPosThreshold+180.0, "deg");
      Quantity parAngleStepQuant((Double)rotatePAStep, "deg");
      if (itsVpTable.empty()) {
        os << LogIO::NORMAL // Loglevel INFO
            << "Using defaults for primary beams used in gridding" << LogIO::POST;
        vp=new refim::VPSkyJones(msc, true, parAngleStepQuant, squintType,
            skyPosThresholdQuant);
      } else {
        os << LogIO::NORMAL // Loglevel INFO
            << "Using VP as defined in " << itsVpTable <<  LogIO::POST;
        Table vpTable( itsVpTable );
        vp=new refim::VPSkyJones(msc, vpTable, parAngleStepQuant, squintType,
            skyPosThresholdQuant);
      }
      theFT = new refim::SDGrid(mLocation_p, *vp, cache/2, tile, gridfunclower,
          convSupport, minWeight, clipMinMax);
      theIFT = new refim::SDGrid(mLocation_p, *vp, cache/2, tile, gridfunclower,
          convSupport, minWeight, clipMinMax);
    } else if (gridfunclower=="gauss" || gridfunclower=="gjinc") {
      DirectionCoordinate dirCoord = itsMaxCoordSys.directionCoordinate();
      Vector<String> units = dirCoord.worldAxisUnits();
      Vector<Double> increments = dirCoord.increment();
      Quantity cellx(increments[0], units[0]);
      Quantity celly(increments[1], units[1]);
      if (cellx != celly &&
          ((!truncateSize.getUnit().empty()||truncateSize.getUnit()=="pixel")
              || (!gwidth.getUnit().empty()||gwidth.getUnit()=="pixel")
              || (!jwidth.getUnit().empty()||jwidth.getUnit()=="pixel"))) {
        os << LogIO::WARN
            << "The " << gridFunction << " gridding doesn't support non-square grid." << endl
            << "Result may be wrong." << LogIO::POST;
      }
      Float truncateValue, gwidthValue, jwidthValue;
      if (truncateSize.getUnit().empty() || truncateSize.getUnit()=="pixel")
        truncateValue = truncateSize.getValue();
      else
        truncateValue = truncateSize.getValue("rad")/celly.getValue("rad");
      if (gwidth.getUnit().empty() || gwidth.getUnit()=="pixel")
        gwidthValue = gwidth.getValue();
      else
        gwidthValue = gwidth.getValue("rad")/celly.getValue("rad");
      if (jwidth.getUnit().empty() || jwidth.getUnit()=="pixel")
        jwidthValue = jwidth.getValue();
      else
        jwidthValue = jwidth.getValue("rad")/celly.getValue("rad");
      theFT = new refim::SDGrid(mLocation_p, cache/2, tile, gridfunclower,
                        truncateValue, gwidthValue, jwidthValue, minWeight, clipMinMax);
      theIFT = new refim::SDGrid(mLocation_p, cache/2, tile, gridfunclower,
                        truncateValue, gwidthValue, jwidthValue, minWeight, clipMinMax);
    }
    else {
      theFT = new refim::SDGrid(mLocation_p, cache/2, tile, gridfunclower,
                        convSupport, minWeight, clipMinMax);
      theIFT = new refim::SDGrid(mLocation_p, cache/2, tile, gridfunclower,
                        convSupport, minWeight, clipMinMax);
    }
    theFT->setPointingDirColumn(pointingDirCol);

    // turn on Pseudo Stokes mode if necessary
    if (pseudoI || stokes == "XX" || stokes == "YY" || stokes == "XXYY"
        || stokes == "RR" || stokes == "LL" || stokes == "RRLL") {
      theFT->setPseudoIStokes(True);
      theIFT->setPseudoIStokes(True);
    }
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //// Get/Set Weight Grid.... write to disk and read

  /// todo : do for full mapper list, and taylor terms.
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Bool SynthesisImagerVi2::setWeightDensity( const String& weightimagename)
  {
    LogIO os(LogOrigin("SynthesisImagerVi2", "setWeightDensity()", WHERE));
    try
      {
	if(weightimagename.size() !=0){
	  Table::isReadable(weightimagename, True);
	  PagedImage<Float> im(weightimagename);
	  imwgt_p=VisImagingWeight(im);
          im.unlock();
	}
	else{
          ////In memory weight densities is being deprecated...we should get rid of this bit
	  Int ndensities=1;
	  if((itsMappers.imageStore(0)->gridwt()->nelements())==5)
	    ndensities=(itsMappers.imageStore(0)->gridwt())->shape()[4];
	  Int nx=(itsMappers.imageStore(0)->gridwt())->shape()[0];
	  Int ny=(itsMappers.imageStore(0)->gridwt())->shape()[1];
	  Block<Matrix<Float> > densitymatrices(ndensities);
	  if(((itsMappers.imageStore(0)->gridwt())->shape().nelements())==5){
	    IPosition blc(Vector<Int>(5,0));
	    for (uInt fid=0;fid<densitymatrices.nelements();fid++)
	      {
		densitymatrices[fid].resize();
		Array<Float> lala;
		blc[4]=fid;
		itsMappers.imageStore(0)->gridwt()->getSlice(lala, blc, IPosition(5, nx, ny,1,1,1), True);
		densitymatrices[fid].reference( lala.reform(IPosition(2, nx, ny)));
	      }
	  }
	  else{
	    Array<Float> lala;
	    itsMappers.imageStore(0)->gridwt()->get(lala, True);
	    densitymatrices[0].reference( lala.reform(IPosition(2, nx, ny)));
	  }
	  imwgt_p.setWeightDensity( densitymatrices );
	}

	vi_p->useImagingWeight(imwgt_p);
	itsMappers.releaseImageLocks();

      }
    catch (AipsError &x)
      {
	throw(AipsError("In setWeightDensity : "+x.getMesg()));
      }
    return true;
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SynthesisImagerVi2::createVisSet(const Bool /*writeAccess*/)
  {
    LogIO os( LogOrigin("SynthesisImagerVi2","createVisSet",WHERE) );
    //cerr << "mss_p num" << mss_p.nelements() <<  " sel  " << fselections_p->size() << endl;
    lockMSs();
    if(mss_p.nelements() > uInt(fselections_p->size()) && (fselections_p->size() !=0)){
      throw(AipsError("Discrepancy between Number of MSs and Frequency selections"));
    }
    vi_p=new vi::VisibilityIterator2(mss_p, vi::SortColumns(), true); //writeAccess);

    if(fselections_p->size() !=0){
      CountedPtr<vi::FrequencySelections> tmpfselections=new FrequencySelections();
      //Temporary fix till we get rid of old vi and we can get rid of tuneSelect
      if(uInt(fselections_p->size()) > mss_p.nelements()){
	for(uInt k=0 ; k <  mss_p.nelements(); ++k){
	  tmpfselections->add(fselections_p->get(k));
	}
      }
      else{
	tmpfselections=fselections_p;
      }
      ////end of fix for tuneSelectdata 
      vi_p->setFrequencySelection (*tmpfselections);

    }
    //
    vi_p->originChunks();
    vi_p->origin();
    ////make sure to use the latest imaging weight scheme
    vi_p->useImagingWeight(imwgt_p);
    unlockMSs();
  }// end of createVisSet

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Method to run the AWProjectFT machine to seperate the CFCache
  // construction from imaging.  This is done by splitting the
  // operation in two steps: (1) run the FTM in "dry" mode to create a
  // "blank" CFCache, and (2) re-load the "blank" CFCache and "fill"
  // it.
  //
  // If someone can get me (SB) out of the horrible statc_casts in the
  // code below, I will be most grateful (we are out of it! :-)).
  //
  void SynthesisImagerVi2::dryGridding(const Vector<String>& cfList)
  {
    LogIO os( LogOrigin("SynthesisImagerVi2","dryGridding",WHERE) );

    rownr_t cohDone=0;
    Int whichFTM=0;
    (void)cfList;
    // If not an AWProject-class FTM, make this call a NoOp.  Might be
    // useful to extend it to other projection FTMs -- but later.
    String ftmName = ((*(itsMappers.getFTM2(whichFTM)))).name();

    if (!((itsMappers.getFTM2(whichFTM,true))->isUsingCFCache())) return;

    os << "---------------------------------------------------- Dry Gridding ---------------------------------------------" << LogIO::POST;

    //
    // Go through the entire MS in "dry" mode to set up a "blank"
    // CFCache.  This is done by setting the AWPWBFT in dryrun mode
    // and gridding.  The process of gridding emits CFCache, which
    // will be "blank" in a dry run.
    {
      vi::VisBuffer2* vb=vi_p->getVisBuffer();
      vi_p->originChunks();
      vi_p->origin();
      Double numberCoh=0;
      for (uInt k=0; k< mss_p.nelements(); ++k)
	numberCoh+=Double(mss_p[k]->nrow());

      ProgressMeter pm(1.0, numberCoh, "dryGridding", "","","",true);

      itsMappers.initializeGrid(*vi_p);
    
      // Set the gridder (iFTM) to run in dry-gridding mode
      (itsMappers.getFTM2(whichFTM,true))->setDryRun(true);

      Bool aTermIsOff=False;
      {
	CountedPtr<refim::FTMachine> ftm=itsMappers.getFTM2(whichFTM,True);
	CountedPtr<refim::ConvolutionFunction> cf=ftm->getAWConvFunc();
	aTermIsOff = cf->getTerm("ATerm")->isNoOp();
      }

      os << "Making a \"blank\" CFCache"
	 << (aTermIsOff?" (without the A-Term)":"")
	 << LogIO::WARN << LogIO::POST;

      // Step through the MS.  This triggers the logic in the Gridder
      // to determine all the CFs that will be required.  These empty
      // CFs are written to the CFCache which can then be filled via
      // a call to fillCFCache().
      for (vi_p->originChunks(); vi_p->moreChunks();vi_p->nextChunk())
	{
	  for (vi_p->origin(); vi_p->more(); vi_p->next())
	    {
	      if (SynthesisUtilMethods::validate(*vb)!=SynthesisUtilMethods::NOVALIDROWS) 
		{
		  itsMappers.grid(*vb, true, refim::FTMachine::OBSERVED, whichFTM);
		  cohDone += vb->nRows();
		  pm.update(Double(cohDone));
		  // If there is no term that depends on time, don't iterate over the entire data base
		  if (aTermIsOff) break;
		}
	    }
	}
    }
    if (cohDone == 0) os << "No valid rows found in dryGridding." << LogIO::EXCEPTION << LogIO::POST;
    // Unset the dry-gridding mode.
    (itsMappers.getFTM2(whichFTM,true))->setDryRun(false);

    //itsMappers.checkOverlappingModels("restore");
    unlockMSs();
    //fillCFCache(cfList);
  }
  //
  // Re-load the CFCache from the disk using the supplied list of CFs
  // (as cfList).  Then extract the ConvFunc object (which was setup
  // in the FTM) and call it's makeConvFunction2() to fill the CF.
  // Finally, unset the dry-run mode of the FTM.
  //
  void SynthesisImagerVi2::fillCFCache(const Vector<String>& cfList,
				       const String& ftmName,
				       const String& cfcPath,
				       const Bool& psTermOn,
				       const Bool& aTermOn,
				       const Bool& conjBeams)
    {
      LogIO os( LogOrigin("SynthesisImagerVi2","fillCFCache",WHERE) );
      // If not an AWProject-class FTM, make this call a NoOp.  Might be
      // useful to extend it to other projection FTMs -- but later.
      // String ftmName = ((*(itsMappers.getFTM(whichFTM)))).name();

      if (!ftmName.contains("awproject") and
	  !ftmName.contains("multitermftnew")) return;
      //if (!ftmName.contains("awproject")) return;
      
      os << "---------------------------------------------------- fillCFCache ---------------------------------------------" << LogIO::POST;

      //String cfcPath = itsMappers.getFTM(whichFTM)->getCacheDir();
      //String imageNamePrefix=itsMappers.getFTM(whichFTM)->getCFCache()->getWtImagePrefix();

      //cerr << "Path = " << path << endl;

      // CountedPtr<AWProjectWBFTNew> tmpFT = new AWProjectWBFTNew(static_cast<AWProjectWBFTNew &> (*(itsMappers.getFTM(whichFTM))));


      Float dPA=360.0,selectedPA=2*360.0;
      if (cfList.nelements() > 0)
	{
	  CountedPtr<refim::CFCache> cfCacheObj = new refim::CFCache();
	  //Vector<String> wtCFList; wtCFList.resize(cfList.nelements());
	  //for (Int i=0; i<wtCFList.nelements(); i++) wtCFList[i] = "WT"+cfList[i];
	  //Directory dir(path);
	  Vector<String> cfList_p=cfList;//dir.find(Regex(Regex::fromPattern("CFS*")));
	  Vector<String> wtCFList_p;
	  wtCFList_p.resize(cfList_p.nelements());
	  for (Int i=0; i<(Int)wtCFList_p.nelements(); i++) wtCFList_p[i]="WT"+cfList_p[i];

	  //cerr << cfList_p << endl;
      	  cfCacheObj->setCacheDir(cfcPath.data());

	  os << "Re-loading the \"blank\" CFCache for filling" << LogIO::WARN << LogIO::POST;

      	  cfCacheObj->initCacheFromList2(cfcPath, cfList_p, wtCFList_p,
      					 selectedPA, dPA,CFC_VERBOSE);

	  // tmpFT->setCFCache(cfCacheObj);
	  Vector<Double> uvScale, uvOffset;
	  Matrix<Double> vbFreqSelection;
	  CountedPtr<refim::CFStore2> cfs2 = CountedPtr<refim::CFStore2>(&cfCacheObj->memCache2_p[0],false);//new CFStore2;
	  CountedPtr<refim::CFStore2> cfwts2 =  CountedPtr<refim::CFStore2>(&cfCacheObj->memCacheWt2_p[0],false);//new CFStore2;

	  //
	  // Get whichFTM from itsMappers (SIMapperCollection) and
	  // cast it as AWProjectWBFTNew.  Then get the ConvFunc from
	  // the FTM and cast it as AWConvFunc.  Finally call
	  // AWConvFunc::makeConvFunction2().
	  //
	  // (static_cast<AWConvFunc &> 
	  //  (*(static_cast<AWProjectWBFTNew &> (*(itsMappers.getFTM(whichFTM)))).getAWConvFunc())
	  //  ).makeConvFunction2(String(path), uvScale, uvOffset, vbFreqSelection,
	  // 		       *cfs2, *cfwts2);

	  // This is a global methond in AWConvFunc.  Does not require
	  // FTM to be constructed (which can be expensive in terms of
	  // memory footprint).
	  refim::AWConvFunc::makeConvFunction2(String(cfcPath), uvScale, uvOffset, vbFreqSelection,
					       *cfs2, *cfwts2, psTermOn, aTermOn, conjBeams);
      	}
      //cerr << "Mem used = " << itsMappers.getFTM(whichFTM)->getCFCache()->memCache2_p[0].memUsage() << endl;
      //(static_cast<AWProjectWBFTNew &> (*(itsMappers.getFTM(whichFTM)))).getCFCache()->initCache2();
    }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SynthesisImagerVi2::reloadCFCache()
  {
      LogIO os( LogOrigin("SynthesisImagerVi2","reloadCFCache",WHERE) );
      Int whichFTM=0; 
      CountedPtr<refim::FTMachine> ftm=itsMappers.getFTM2(whichFTM,true);

      // Proceed only if FMTs uses the CFCache mechanism. The first FTM
      // in the Mapper is used to make this decision.  Not sure if the
      // framework pipes allow other FTMs in SIMapper to be
      // fundamentally different. If it does, and if that is
      // triggered, the current decision may be insufficient.
      if (!(ftm->isUsingCFCache())) return; // Better check than checking against FTM name
      
      os << "-------------------------------------------- Re-load CFCache ---------------------------------------------" << LogIO::POST;

      // Following code that distinguishes between MultiTermFTM and
      // all others should ideally be replaced with a polymorphic
      // solution.  I.e. all FTMs should have a working getFTM2(bool)
      // method.  This is required since MultiTermFTM is a container
      // FTM and it's getFTM2() returns the internal (per-MTMFS term)
      // FTMs.  Non-container FTMs must return a pointer to
      // themselves.  The if-else below is because attempts to make
      // AWProjectFT::getFTM2() work have failed.
      //
      // Control reaches this stage only if the isUsingCFCache() test
      // above return True.  The only FTMs what will pass that test
      // for now are AWProjectFT (and its derivatives) and
      // MultiTermFTM if it is constructed with AWP.
      //
      CountedPtr<refim::CFCache> cfc;
      if (ftm->name().contains("MultiTerm")) cfc = ftm->getFTM2(true)->getCFCache();
      else                                   cfc = ftm->getCFCache();
      cfc->setLazyFill((refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1));
      cfc->initCache2();


      // String path,imageNamePrefix;
      // if (ftm->name().contains("MultiTerm"))
      // 	{
      // 	  path = ftm->getFTM2(true)->getCacheDir();
      // 	  imageNamePrefix = ftm->getFTM2(true)->getCFCache()->getWtImagePrefix();
      // 	}
      // else
      // 	{
      // 	  path = ftm->getCacheDir();
      // 	  imageNamePrefix = ftm->getCFCache()->getWtImagePrefix();
      // 	}
	

      // CountedPtr<refim::CFCache> cfCacheObj = new refim::CFCache();
      // cfCacheObj->setCacheDir(path.c_str());
      // cfCacheObj->setWtImagePrefix(imageNamePrefix.c_str());
      // cfCacheObj->setLazyFill((refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1));
      // cfCacheObj->initCache2();

      // // This assumes the itsMappers is always SIMapperCollection.
      // for (whichFTM = 0; whichFTM < itsMappers.nMappers(); whichFTM++)
      // 	{
      // 	  CountedPtr<refim::FTMachine> ifftm=itsMappers.getFTM2(whichFTM,true),
      // 	    fftm=itsMappers.getFTM2(whichFTM,false);
	
      // 	  ifftm->setCFCache(cfCacheObj,true);
      // 	  fftm->setCFCache(cfCacheObj,true);
      // 	}
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SynthesisImagerVi2::loadMosaicSensitivity(){
    String ftmname=itsMappers.getFTM2(0)->name();
    
    if(ftmname.contains("Mosaic")){
      //sumwt has been calcuated
      Bool donesumwt=(max(itsMappers.imageStore(0)->sumwt()->get()) > 0.0);
      //cerr << "Done sumwght " << donesumwt << max(itsMappers.imageStore(0)->sumwt()->get()) << endl;
      if(donesumwt){
        IPosition shp=itsMappers.imageStore(0)->weight()->shape();
        CoordinateSystem cs=itsMappers.imageStore(0)->weight()->coordinates();
        CountedPtr<TempImage<Float> > wgtim=new TempImage<Float>(shp, cs);
        wgtim->copyData(*(itsMappers.imageStore(0)->weight()));
        (static_cast<refim::MosaicFTNew &>( *(itsMappers.getFTM2(0,False)))).setWeightImage(wgtim);
        static_cast<refim::MosaicFTNew &>( *(itsMappers.getFTM2(0,True))).setWeightImage(wgtim);

      }


    }

  }
  /////////////////////////////////////////////////
  Record SynthesisImagerVi2::apparentSensitivity() 
  {
    LogIO os(LogOrigin("SynthesisImagerVi2", "apparentSensitivity()", WHERE));
    
    Record outrec;
    try {

      os << LogIO::NORMAL // Loglevel INFO
	 << "Calculating apparent sensitivity from MS weights, as modified by gridding weight function"
	 << LogIO::POST;
      os << LogIO::NORMAL // Loglevel INFO
	 << "(assuming that MS weights have correct scale and units)"
	 << LogIO::POST;
      
      Double sumNatWt=0.0;
      Double sumGridWt=0.0;
      Double sumGridWt2OverNatWt=0.0;
    
      Float iNatWt(0.0),iGridWt(0.0);
      
      vi::VisBuffer2* vb = vi_p->getVisBuffer();
      vi_p->originChunks();
      vi_p->origin();
      
      // Discover if weightSpectrum non-trivially available
      Bool doWtSp=vi_p->weightSpectrumExists();

      //////
      for(vi_p->originChunks(); vi_p->moreChunks(); vi_p->nextChunk())
	{
	  for (vi_p->origin(); vi_p->more(); vi_p->next())
	    {
	      auto nRow=vb->nRows();
	      const Vector<Bool>& rowFlags(vb->flagRow());

	      const Vector<Int>& a1(vb->antenna1()), a2(vb->antenna2());

              // Extract weights correctly (use WEIGHT_SPECTRUM, if available)
	      Int nCorr=vb->nCorrelations();
              Matrix<Float> wtm;
              Cube<Float> wtc;
              if (doWtSp)
                // WS available [nCorr,nChan,nRow]
                wtc.reference(vb->weightSpectrum());       
              else {
                // WS UNavailable weight()[nCorr,nRow] --> [nCorr,nChan,nRow]
                wtc.reference(vb->weight().reform(IPosition(3,nCorr,1,nRow)));  // unchan'd weight as single-chan
              }
	      Int nChanWt=wtc.shape()(1);  // Might be 1 (no WtSp)

	      Cube<Bool> flagCube(vb->flagCube());
	      for (rownr_t row=0; row<nRow; row++) {
		if (!rowFlags(row) && a1(row)!=a2(row)) {  // exclude ACs

		  for (Int ich=0;ich<vb->nChannels();++ich) {
		    if( !flagCube(0,ich,row) && !flagCube(nCorr-1,ich,row)) {  // p-hands unflagged

		      // Accumulate relevant info

		      // Simple sum of p-hand for now
		      iNatWt=wtc(0,ich%nChanWt,row)+wtc(nCorr-1,ich%nChanWt,row);

		      iGridWt=2.0f*vb->imagingWeight()(ich,row);

		      if (iGridWt>0.0 && iNatWt>0.0) {
			sumNatWt+=(iNatWt);
			sumGridWt+=(iGridWt);
			sumGridWt2OverNatWt+=(iGridWt*iGridWt/iNatWt);
		      }
		    }
		  }
		}
	      } // row
	    } // vb
	} // chunks
      
      if (sumNatWt==0.0) {
	os << "Cannot calculate sensitivity: sum of selected natural weights is zero" << LogIO::EXCEPTION;
      }
      if (sumGridWt==0.0) {
	os << "Cannot calculate sensitivity: sum of gridded weights is zero" << LogIO::EXCEPTION;
      }

      Double effSensitivity = sqrt(sumGridWt2OverNatWt)/sumGridWt;

      Double natSensitivity = 1.0/sqrt(sumNatWt);
      Double relToNat=effSensitivity/natSensitivity;

      os << LogIO::NORMAL << "RMS Point source sensitivity  : " // Loglevel INFO
	 << effSensitivity      //  << " Jy/beam"       // actually, units are arbitrary
	 << LogIO::POST;
      os << LogIO::NORMAL // Loglevel INFO
	 << "Relative to natural weighting : " << relToNat << LogIO::POST;

      // Fill output Record
      outrec.define("relToNat",relToNat);
      outrec.define("effSens",effSensitivity);

    } catch (AipsError x) {
      throw(x);
      return outrec;
    } 
    return outrec;

  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Bool SynthesisImagerVi2::makePB()
  {
      LogIO os( LogOrigin("SynthesisImagerVi2","makePB",WHERE) );

      if( itsMakeVP==False )
	{
	  os << LogIO::NORMAL1 << "Not making .pb by direct evaluation. The gridder will make a .weight and a .pb will be computed from it." << LogIO::POST;
	  // Check that the .weight exists.. ?

	  return False;
	}
      else
	{
	  Bool doDefaultVP = itsVpTable.length()>0 ? False : True;

	  CoordinateSystem coordsys=itsMappers.imageStore(0)->getCSys();
	  String telescope=coordsys.obsInfo().telescope();
	  
	  if (doDefaultVP) {
	    
	    MSAntennaColumns ac(mss_p[0]->antenna());
	    Double dishDiam=ac.dishDiameter()(0);
	    if(!allEQ(ac.dishDiameter().getColumn(), dishDiam))
	      os << LogIO::WARN
		 << "The MS has multiple antenna diameters ..PB could be wrong "
		 << LogIO::POST;
	    return makePBImage( telescope, False, dishDiam);
	  }
	  else{
	    return makePBImage(telescope );	
	  }
	  
	}
 
      return False;
  }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Bool SynthesisImagerVi2::makePrimaryBeam(PBMath& pbMath)
  {
    LogIO os( LogOrigin("SynthesisImagerVi2","makePrimaryBeam",WHERE) );

    os << "vi2 : Evaluating Primary Beam model onto image grid(s)" << LogIO::POST;

    itsMappers.initPB();

    vi::VisBuffer2* vb = vi_p->getVisBuffer();
    vi_p->originChunks();
    vi_p->origin();
    std::map<Int, std::set<Int>> fieldsDone;
    VisBufferUtil vbU(*vb);
    ///////if tracking a moving source
    MDirection origMovingDir;
    MDirection newPhaseCenter;
    Bool trackBeam=getMovingDirection(*vb, origMovingDir, True);
    //////
    for(vi_p->originChunks(); vi_p->moreChunks(); vi_p->nextChunk())
      {
	for (vi_p->origin(); vi_p->more(); vi_p->next())
	  {
	    Bool fieldDone=False;
	    if(fieldsDone.count(vb->msId() >0)){
	      fieldDone=fieldDone || (fieldsDone[vb->msId()].count(vb->fieldId()(0)) > 0);
	    }
	    else{
	      fieldsDone[vb->msId()]=std::set<int>();
	    }
	    if(!fieldDone){
	      fieldsDone[vb->msId()].insert(vb->fieldId()(0));
	      if(trackBeam){
		MDirection newMovingDir;
		getMovingDirection(*vb, newMovingDir);
		//newPhaseCenter=vb->phaseCenter();
                newPhaseCenter=vbU.getPhaseCenter(*vb);
		newPhaseCenter.shift(MVDirection(-newMovingDir.getAngle()+origMovingDir.getAngle()), False);
	      }
	      itsMappers.addPB(*vb,pbMath, newPhaseCenter, trackBeam);
	      
	    }
	  }
      }
    itsMappers.releaseImageLocks();
    unlockMSs();

    return True;
  }// end makePB

  Bool SynthesisImagerVi2::getMovingDirection(const vi::VisBuffer2& vb,  MDirection& outDir, const Bool useImageEpoch){
    MDirection movingDir;
    Bool trackBeam=False;
      
    MeasFrame mFrame(MEpoch(Quantity(vb.time()(0), "s"), MSColumns(vb.ms()).timeMeas()(0).getRef()), mLocation_p);
    if(useImageEpoch){
      mFrame.resetEpoch((itsMappers.imageStore(0))->getCSys().obsInfo().obsDate());

    }
    if(movingSource_p != ""){
      MDirection::Types refType;
      trackBeam=True;
      if(Table::isReadable(movingSource_p, False)){
	//seems to be a table so assuming ephemerides table
	Table laTable(movingSource_p);
	Path leSentier(movingSource_p);
	MeasComet laComet(laTable, leSentier.absoluteName());
	movingDir.setRefString("COMET");
	mFrame.set(laComet);
      }
      ///if not a table 
      else  if(casacore::MDirection::getType(refType, movingSource_p)){
	if(refType > casacore::MDirection::N_Types && refType < casacore::MDirection:: N_Planets ){
	  ///A known planet
	  movingDir.setRefString(movingSource_p);
	}
      }
      else if(upcase(movingSource_p)=="TRACKFIELD"){
        VisBufferUtil vbU(vb);
	movingDir=vbU.getEphemDir(vb, MEpoch(mFrame.epoch()).get("s").getValue());
      }
      else{
	throw(AipsError("Erroneous tracking direction set to make pb"));
      }
      MDirection::Ref outref1(MDirection::AZEL, mFrame);
      MDirection tmphazel=MDirection::Convert(movingDir, outref1)();
      MDirection::Ref outref(vb.phaseCenter().getRef().getType(), mFrame);
      outDir=MDirection::Convert(tmphazel, outref)();  
    }
    else{
      outDir=vb.phaseCenter();
      trackBeam=False;
    }
      return trackBeam;


  }
  CountedPtr<vi::VisibilityIterator2> SynthesisImagerVi2::getVi(){
	return vi_p;  
  }
  CountedPtr<refim::FTMachine> SynthesisImagerVi2::getFTM(const Int fid, Bool ift){
	if(itsMappers.nMappers() <=fid)
		throw(AipsError("Mappers have not been set for field "+String::toString(fid)));
	return (itsMappers.getFTM2(fid, ift));
	  
  }
  void SynthesisImagerVi2::updateImageBeamSet(Record& returnRec){
    if(returnRec.isDefined("imageid") && returnRec.asInt("imageid")==0){
      //ImageInfo ii=(itsMappers.imageStore(0)->psf())->imageInfo();
      Vector<Int> chanRange(0);
      if(returnRec.isDefined("chanrange"))
	chanRange=returnRec.asArrayInt("chanrange");
      Int npol=(itsMappers.imageStore(0)->getShape())(2);
      Int nchan=(itsMappers.imageStore(0)->getShape())(3);
      if(chanRange.nelements() ==2 && chanRange(1) < nchan){

	ImageBeamSet iibeamset=cubePsfImageInfo_p.getBeamSet();
	Matrix<GaussianBeam> mbeams=iibeamset.getBeams();
	if(mbeams.nelements()==0){
	  mbeams.resize(itsMappers.imageStore(0)->getShape()(3), itsMappers.imageStore(0)->getShape()(2));
	  mbeams.set(GaussianBeam(Vector<Quantity>(3, Quantity(1e-12, "arcsec"))));
	}
	Cube<Float> recbeams(returnRec.asArrayFloat("beams"));
	for(Int c=chanRange[0]; c <= chanRange[1]; ++c){
	  for(Int p=0; p < npol; ++p){
	    mbeams(c,p)=GaussianBeam(Quantity(recbeams(c-chanRange[0], p, 0),"arcsec"), Quantity(recbeams(c-chanRange[0], p, 1),"arcsec"), Quantity(recbeams(c-chanRange[0], p, 2),"deg"));

	  }
	}
	cubePsfImageInfo_p.setBeams(ImageBeamSet(mbeams));

      }
      //itsMappers.imageStore(0)->psf()->setImageInfo(ii);
      //itsMappers.imageStore(0)->psf()->unlock();

      
    }
  }

} //# NAMESPACE CASA - END

