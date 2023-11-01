#include <vector>
#include <iostream>

#include <alma/ASDMBinaries/Error.h>
#include <alma/ASDMBinaries/BaselinesSet.h>

using namespace asdm;
using namespace sdmbin;
using namespace std;

using namespace AxisNameMod;
using namespace BasebandNameMod;
using namespace CorrelationModeMod;
using namespace DataContentMod;

// constructors
BaselinesSet::BaselinesSet(){
  bool coutest=false;
  if(coutest)cout<<"Constructor vide BaselinesSet" <<endl; 
}

BaselinesSet::BaselinesSet( vector<Tag>               v_antennaIdArray, 
			    vector<int>               v_feedIdArray,
			    vector<int>               v_phasedArrayList,
			    vector<bool>              v_antennaUsedArray,
			    DataDescriptionsSet       &dataDescriptionsSet
			   ):
  DataDescriptionsSet(dataDescriptionsSet),
  v_AntennaIdArray_(v_antennaIdArray),
  v_FeedIdArray_(v_feedIdArray),
  v_PhasedArrayList_(v_phasedArrayList)
{
  bool coutest=false;
  if(coutest)cout << "Constructor BaselinesSet" << endl;
  if(coutest)cout << "v_AntennaIdArray_.size=" << v_AntennaIdArray_.size() << endl;
      numAntennas_ = v_antennaIdArray.size();              if(coutest)cout << "numAntennas_=" << numAntennas_ << endl;
      numFeeds_    = v_feedIdArray.size()/numAntennas_;    if(coutest)cout << "numFeeds_=   " << numFeeds_    << endl;


  for(unsigned int na=0; na<numAntennas_; na++)
    if(v_antennaUsedArray[na])
      v_effAntennaIdArray_.push_back(v_antennaIdArray[na]);
  numEffAntennas_ = v_effAntennaIdArray_.size();

  if(coutest)cout << "numEffAntennas_=" << numEffAntennas_ << endl;
  numBaselines_     = (numAntennas_*(numAntennas_-1))/2;          // number of cross-correlation baselines
  if(coutest)cout << "numBaselines_=" << numBaselines_ << endl;
  numEffBaselines_  = (numEffAntennas_*(numEffAntennas_-1))/2;    // number of effective cross-correlation baselines
  if(coutest)cout << "numEffBaselines_=" << numEffBaselines_ << endl;
}

BaselinesSet::BaselinesSet(const BaselinesSet & a) : DataDescriptionsSet(a) {
  cout << "Copy constructor BaselinesSet" << endl;

  // attributes inherited from the class SwitchCyclesList:
  m_bn_v_scId_              = a.m_bn_v_scId_;
  m_bn_v_numBin_            = a.m_bn_v_numBin_;
  vv_numBin_                = a.vv_numBin_;
  v_numBin_                 = a.v_numBin_;
  datasetPtr_               = a.datasetPtr_;
  vv_switchCycleId_         = a.vv_switchCycleId_;
  v_switchCycleId_          = a.v_switchCycleId_;

  // attributes inherited from the class DataDescriptionsSet:
  v_numPol_                 = a.v_numPol_;
  v_spwId_                  = a.v_spwId_;
  v_numChan_                = a.v_numChan_;
  v_basebandName_           = a.v_basebandName_;
  vv_nsp_                   = a.vv_nsp_;

  m_bn_v_ddp_               = a.m_bn_v_ddp_;
  m_ddid_bbn_               = a.m_ddid_bbn_;
  //cout << "1" << endl;
  numApc_                   = a.numApc_;
  numDataDescription_       = a.numDataDescription_;
  e_cm_                     = a.e_cm_;
  correlationMode_          = a.correlationMode_;
  es_apc_                   = a.es_apc_;
  v_atmPhaseCorrection_     = a.v_atmPhaseCorrection_;
  //cout << "2" << endl;
  sumMetaDataIndex_         = a.sumMetaDataIndex_;
  sumAutoSize_              = a.sumAutoSize_;
  sumCrossSize_             = a.sumCrossSize_;
  //cout << "3" << endl;
  v_metaDataIndex_          = a.v_metaDataIndex_;
  v_cumulAutoSize_          = a.v_cumulAutoSize_; 
  v_cumulCrossSize_         = a.v_cumulCrossSize_;
  v_numAutoData_            = a.v_numAutoData_;
  v_numCrossData_           = a.v_numCrossData_;
  v_autoSize_               = a.v_autoSize_;
  v_crossSize_              = a.v_crossSize_;
  //cout << "4" << endl;
  v_dataDescriptionIdArray_ = a.v_dataDescriptionIdArray_;
  v_crossDataDescriptionId_ = a.v_crossDataDescriptionId_;
  v_autoDataDescriptionId_  = a.v_autoDataDescriptionId_;
  v_basebandSet_            = a.v_basebandSet_;
  v_numSpwPerBb_            = a.v_numSpwPerBb_;

  // attribute in the present BaselinesSet class:
  v_AntennaIdArray_         = a.v_AntennaIdArray_;
  v_FeedIdArray_            = a.v_FeedIdArray_;
  v_PhasedArrayList_        = a.v_PhasedArrayList_;
  numAntennas_              = a.numAntennas_;
  numFeeds_                 = a.numFeeds_;
  numEffAntennas_           = a.numEffAntennas_;
  numBaselines_             = a.numBaselines_;
  numEffBaselines_          = a.numEffBaselines_;
  v_effAntennaIdArray_      = a.v_effAntennaIdArray_;
}

BaselinesSet::~BaselinesSet(){}

unsigned int BaselinesSet::getNumAntennas(){ return numAntennas_; }

unsigned int BaselinesSet::getNumEffAntennas(){ return numEffAntennas_; }

Tag BaselinesSet::getEffAntennaId(unsigned int na) {
  if(na>=v_effAntennaIdArray_.size())
    Error(SERIOUS,
	  (char *) "The antenna index, %d, exceeds the maximum limit of %d",
	  na,v_effAntennaIdArray_.size()-1); 
  return v_effAntennaIdArray_[na];
}

int BaselinesSet::getFeedId(unsigned int na, unsigned int nfe) {
  if(na>=v_effAntennaIdArray_.size())
    Error(SERIOUS,
	  (char *) "The antenna index, %d, exceeds the maximum limit of %d",
	  na,v_effAntennaIdArray_.size()-1);
  if(nfe>=numFeeds_)
    Error(SERIOUS,
	  (char *) "The feed index, %d, exceeds the maximum limit of %d deriving from the number of feeds in the configuration",
	  nfe,numFeeds_-1); 
  return v_FeedIdArray_[na*numFeeds_+nfe];
}

unsigned int BaselinesSet::getNumBaselines(){ return numBaselines_; }

unsigned int BaselinesSet::getNumEffBaselines(){ return numEffBaselines_; }

unsigned int BaselinesSet::baselineIndex( Tag antennaId1, Tag antennaId2) {
//   int iNum = 0;          // iNum will be the (1 based) antennaNum in the list for antennaId1 
//   int jNum = 0;          // jNum will be the (1 based) antennaNum in the list for antennaId2 
//   for(int n=0; n<numEffAntennas_; n++)if(v_effAntennaIdArray_[n]==antennaId1)iNum=n+1; if(iNum==0)return 0;
//   for(int n=0; n<numEffAntennas_; n++)if(v_effAntennaIdArray_[n]==antennaId2)jNum=n+1; if(jNum==0)return 0;

  unsigned int m = 0;
  unsigned int iIdx=baselineIndex(antennaId1);
  unsigned int jIdx=baselineIndex(antennaId2);
  if(iIdx>jIdx){
    m=jIdx; jIdx=iIdx; iIdx=m;
  }
  m = 0;
  for(unsigned int n=0; n<jIdx; n++)m=m+n;
  return m+iIdx;
}

unsigned int BaselinesSet::baselineIndex( Tag antennaId ) {
  for(unsigned int n=0; n<numEffAntennas_; n++)
    if(v_effAntennaIdArray_[n]==antennaId)return n;
  Error(FATAL,(char *) "No baseline index for antennaId=%s",antennaId.toString().c_str());
  return 0;
}


// unsigned int BaselinesSet::baselineIndex( unsigned int na1, unsigned int na2) {

//   // if(na1>=v_effAntennaIdArray_.size()){
//   //   Error(FATAL,"The antenna index, %d, exceeds the limit of %d",
//   // 	  na1,v_effAntennaIdArray_.size()-1);
//   //   return 0;
//   // }
//   // if(na2>=v_effAntennaIdArray_.size()){
//   //   Error(FATAL,"The antenna index, %d, exceeds the limit of %d",
//   // 	  na2,v_effAntennaIdArray_.size()-1);
//   //   return 0;
//   // }

//   unsigned int iIdx = min (na1, na2);
//   unsigned int jIdx = max (na1, na2);
//   /*
//   if(na1<na2){
//     iIdx = na1;
//     jIdx = na2;
//   }else{
//     iIdx = na2;
//     jIdx = na1;
//   }
//   unsigned int m = 0;
//   for(unsigned int n=0; n<jIdx; n++)m=m+n;
//   */
//   return jIdx * (jIdx - 1) / 2 + iIdx;
// }


unsigned int BaselinesSet::antenna1( unsigned int baselineIdx)  {
  unsigned int iIdx;
  unsigned int jIdx;
  unsigned int m;
  for(unsigned int na1=0; na1<numEffAntennas_; na1++){
    for(unsigned int na2=na1; na2<numEffAntennas_; na2++){
      iIdx = na1;
      jIdx = na2;
      m    = 0;
      for(unsigned int n=0; n<jIdx; n++)m += n;
      if((m+iIdx)==baselineIdx)return na1;
    }
  }
  Error(FATAL,(char *) "No antenna1 index for the requested baseline index %d",baselineIdx);
  return 0;
}

unsigned int BaselinesSet::antenna2( unsigned int baselineIdx)  {
  unsigned int iIdx;
  unsigned int jIdx;
  unsigned int m;
  if(baselineIdx==0)return 0;
  for(unsigned int na1=0; na1<numEffAntennas_; na1++){
    for(unsigned int na2=na1; na2<numEffAntennas_; na2++){
      iIdx = na1;
      jIdx = na2;
      m = 0;
      for(unsigned int n=0; n<jIdx; n++)m += n;
      if((m+iIdx)==baselineIdx)return na2;
    }
  }
  Error(FATAL,(char *) "No antenna2 index for the requested baseline index %d",baselineIdx);
  return 0;
}

unsigned int BaselinesSet::feedIndex(Tag antId, int feedId) {
  int na  = -1;
  for(unsigned int n=0; n<numAntennas_; n++)if(antId==v_AntennaIdArray_[n])na=n;
  if(na<0){
    Error(FATAL,
	  (char *) "Antenna with identifier %s  not in the configuration",
	  antId.toString().c_str()); 
    return 0;
  }
  int nfe = -2;
  for(unsigned int n=0; n<numFeeds_; n++)if(feedId==v_FeedIdArray_[na*numFeeds_+n])nfe=n;
  if(nfe<0){
    Error(FATAL,
	  (char *) "Feed identifier %d for antenna with the identifier %s not in the configuration",
	  feedId,antId.toString().c_str()); 
    return nfe;
  }
  return (unsigned int) nfe;
}

 unsigned int BaselinesSet::getNumPDTvalue(Enum<DataContent> e_dc, EnumSet<AxisName> es_an, bool effective){

   unsigned int nAntennas =1; if(es_an[ANT])nAntennas = numAntennas_;  if(e_dc[CROSS_DATA])nAntennas =0;
   unsigned int nBaselines=1; if(es_an[BAL])nBaselines= numBaselines_; if(e_dc[AUTO_DATA]) nBaselines=0;
   // nBaseband is not currently used
   // unsigned int nBaseband =1; if(es_an[BAB])nBaseband = numBaseband();
   unsigned int nBin      =1;
   unsigned int nApc      =1; if(es_an[APC])nApc      = es_apc_.count();
   unsigned int nSpp      =1;
   unsigned int nPol      =1;
   unsigned int npv;
   unsigned int nv1=0, nv2=0;

   // The concept of having in the binary the data only for those actualy producing data has been dropped
   // Hence, in practice effective should be alway set to false with the consequence that nAntennas and
   // nBaselines in general will reflect what has been actually scheduled in antenna resource to do the
   // observations. 
   if(effective && e_dc[FLAGS]==false){
     if(es_an[ANT])nAntennas = numEffAntennas_;; if(e_dc[CROSS_DATA])nAntennas =0;
     if(es_an[BAL])nBaselines= numEffBaselines_; if(e_dc[AUTO_DATA]) nBaselines=0;
   }

   map<BasebandName,vector<DataDescParams> >::iterator 
     itbnddp, 
     itbnddpb=m_bn_v_ddp_.begin(), 
     itbnddpe=m_bn_v_ddp_.end();
   for(itbnddp=itbnddpb;itbnddp!=itbnddpe; ++itbnddp){
     for(unsigned int nspw=0; nspw<itbnddp->second.size(); nspw++){
       if(es_an[POL]){
	 nPol = itbnddp->second[nspw].numCorr;
	 if(e_dc[FLAGS]||e_dc[ZERO_LAGS])if(nPol>2)nPol=2;
       }
       npv = nPol;
       if(e_dc[AUTO_DATA]){
	 if(nPol==3)npv=4;  // XX XY YY    ==> 4 primitive values, 3 real values and 1 imaginary value
	 if(nPol==4)npv=6;  // XX XY YX YY ==> 6 primitive values (use-case not expected for ALMA)
       }
       if(e_dc[CROSS_DATA]){
	 npv *=2;             // 2 primitive values, all the data being complex numbers
       }
       if(es_an[SPP])nSpp=itbnddp->second[nspw].numChan;
       if(es_an[BIN])nBin=itbnddp->second[nspw].numBin;
       nv1 += npv*nBin*nSpp;
     }
   }
   if(e_cm_[AUTO_ONLY]){     // (multi) single-dish use-case
     nv1 = nv1*nAntennas*nApc;
     return nv1;
   }
   if(e_cm_[CROSS_ONLY]){    // non-standard use-case for ALMA, standard use-case for EVLA
     nv1 = nv1*nBaselines*nApc;
     return nv1;
   }
   if(e_dc[ZERO_LAGS]){      // these auxiliary data can be only antenna-based
     nv1 = nv1*nAntennas*nApc;
     return nv1;
   }

   // From here we are now only in the use-case CROSS_AND_AUTO (standard and almost always used for ALMA)
   unsigned int nv=0;

   // Comments about structural constraints:
   // FLAGS:
   //    For the FLAGS meta-data the POL axis, if any, corresponds effectively to a PolarizationType 
   // axis; the size of this POL axis must be the same for both the non-zero and zero baselines.
   // APC axis: 
   //    The following implementation assumes that the APC axis has a size which is the same for
   // the baseline-based and the antenna-based parts of the metadata (i.e. of ACTUAL_TIMES or
   // ACTUAL_DURATION or WEIGHTS or FLAGS). In practice we do not expect an APC axis for these
   // metadata. Would one WVRadiometer have a problem for one antenna, this will be known by looking
   // at the nature of the flags set in FLAGS; In such situations, for all the baselines which involve 
   // the antenna(s) with WVR problems, the filler will retrieve ONLY the uncorrected data.
   // The implication is that actualTimes, actualDurations, weights and flags ARE NOT EXPECTED TO
   // HAVE "APC" IN THEIR SEQUENCE OF AXES in order to concisely describe the data. 
   if(e_dc[FLAGS]){
     if(es_an[BAL])nv = nv1*nBaselines*nApc;
     if(es_an[ANT])nv += nv1*nAntennas*nApc;
     return nv;
   }

   if(e_dc[CROSS_DATA]){       // the query is for the size of crossData 
     return nv1*nBaselines*nApc;
   }

   // From here we must also account for the implicit numPol of the zero-baselines because, when in the 
   // case of CROSS_AND_AUTO, only the numPol for the non-zero baselines are explicit.
   nSpp=1;
   nBin=1;
   nPol=1;
   for(itbnddp=itbnddpb;itbnddp!=itbnddpe; ++itbnddp){
     for(unsigned int nspw=0; nspw<itbnddp->second.size(); nspw++){
       if(es_an[SPP])nSpp=itbnddp->second[nspw].numChan;
       if(es_an[BIN])nBin=itbnddp->second[nspw].numBin;
       if(es_an[POL])nPol=numPol(itbnddp->second[nspw].ddIdx); 
       npv = nPol;
       if(es_an[POL] && e_dc[AUTO_DATA]){
	 if(nPol==3)npv=4;  // XX XY YY    ==> 4 primitive values, 3 real values and 1 imaginary value
	 if(nPol==4)npv=6;  // XX XY YX YY ==> 6 primitive values (use-case not expected for ALMA)
       }
       nv2 += npv*nBin*nSpp;
     }
   }
   nv2 *= nAntennas*nApc;
   if(e_dc[AUTO_DATA])       // the query is for the size of autoData 
     return nv2;

   if(es_an[BAL])nv += nv1;
   if(es_an[ANT])nv += nv2;
   return nv;                 // the query is for the size of actualTimes or actualDurations

 }
 
 


unsigned int BaselinesSet::transferId(unsigned int na, unsigned int ndd, unsigned int nbin){
  unsigned int nfe=0;
  return transferId(na,nfe,ndd,nbin);
}

unsigned int BaselinesSet::transferId(unsigned int na, unsigned int nfe, unsigned int ndd, unsigned int nbin){
  unsigned int v_cumulAutoSize_ndd=0; 
  if(v_cumulAutoSize_.size()!=0)v_cumulAutoSize_ndd=v_cumulAutoSize_[ndd];
  unsigned int v_autoSize_ndd=0;
  if(v_autoSize_.size()!=0)v_autoSize_ndd=v_autoSize_[ndd];
  return 
    na*numFeeds_*sumAutoSize_ +                                     // /antenna
    nfe*sumAutoSize_ +                                              // /antenna/feed
    v_cumulAutoSize_ndd +                                           // /antenna/feed/datadesc
    nbin*(v_autoSize_ndd/numBin(ndd));                              // /antenna/feed/datadesc/bin
}



unsigned int BaselinesSet::transferId(unsigned int na1, unsigned int na2, unsigned int ndd, unsigned int nbin, unsigned int napc){
  unsigned int nfe=0;
  return transferId(na1,na2, nfe, ndd, nbin, napc);
}
