#include <set>
#include <stdio.h>
#include <iostream>
#include <stdlib.h> // for atoi()
#include <alma/ASDMBinaries/Error.h>
#include <alma/ASDMBinaries/DataDescriptionsSet.h>

#include <alma/Enumtcl/StokesParameter.h>
#include <alma/Enumtcl/PolarizationType.h>

using namespace asdm;
using namespace std;

using namespace AtmPhaseCorrectionMod;
using namespace BasebandNameMod;
using namespace CorrelationModeMod;

namespace sdmbin {

  DataDescriptionsSet::DataDescriptionsSet(){}

  DataDescriptionsSet::DataDescriptionsSet( ASDM* const datasetPtr, vector<Tag> v_switchCycleId,
					    vector<Tag> v_dataDescriptionIdArray,
					    CorrelationMode corrMode, vector<AtmPhaseCorrection> v_atmPhaseCorrection):
    SwitchCyclesList(datasetPtr, v_switchCycleId),
    v_dataDescriptionIdArray_(v_dataDescriptionIdArray),
    correlationMode_(corrMode),
    v_atmPhaseCorrection_(v_atmPhaseCorrection)
  {
    /*
     * Do v_switchCycleId and v_dataDescriptionIdArray have the same size ?
     */
    if ( v_switchCycleId.size() != v_dataDescriptionIdArray.size() )
      Error(FATAL, (char *) "It seems that the arrays 'switchCycleId' and 'dataDescriptionId' do not have the same size in one row of the ConfigDescription table !");

    bool coutest=false;

    e_cm_   = corrMode;

    numApc_ = v_atmPhaseCorrection_.size();
    es_apc_.reset();
    //for(unsigned int n=0; n<numApc_; n++)es_apc_.set(v_atmPhaseCorrection_[n]);
    es_apc_.set(v_atmPhaseCorrection_);

    map<BasebandName,vector<DataDescParams> >::iterator itbnddp, itbnddpb, itbnddpf, itbnddpe;
    numDataDescription_ = v_dataDescriptionIdArray.size();   
    sumAutoSize_        = 0;
    sumCrossSize_       = 0;
    vector<SpectralWindowRow*> v_spwr;
    DataDescriptionTable&      rddSet =  datasetPtr_->getDataDescription();
    SwitchCycleTable&          rscSet =  datasetPtr_->getSwitchCycle();
    for(unsigned int n=0; n<numDataDescription_; n++){
      Enum<BasebandName>  e_bbn;
      DataDescriptionRow* ddPtr=rddSet.getRowByKey(v_dataDescriptionIdArray[n]);
      SwitchCycleRow*     scPtr=rscSet.getRowByKey(v_switchCycleId[n]);
      v_spwr.push_back(ddPtr->getSpectralWindowUsingSpectralWindowId());
      //      if(v_spwr[n]->isBasebandNameExists()){    mandatory since ASDMv1
      BasebandName   bbn = ddPtr->getSpectralWindowUsingSpectralWindowId()->getBasebandName();
      DataDescParams ddp;
      ddp.ddIdx   = n;
      ddp.scId    = v_switchCycleId[n];
      ddp.polId   = ddPtr->getPolOrHoloId();
      ddp.spwId   = ddPtr->getSpectralWindowId();
      ddp.numChan = ddPtr->getSpectralWindowUsingSpectralWindowId()->getNumChan();
      ddp.numCorr = ddPtr->getPolarizationUsingPolOrHoloId()->getNumCorr();
      ddp.numBin  = scPtr->getNumStep();
      itbnddpf = m_bn_v_ddp_.find(bbn);
      itbnddpe = m_bn_v_ddp_.end();
      // a map view, the  DataDescriptions being grouped as a function of their parent babseband name
      if(itbnddpf==itbnddpe){
	vector<DataDescParams> v_ddp; v_ddp.push_back(ddp);
	m_bn_v_ddp_.insert(make_pair(bbn,v_ddp));
      }else{
	itbnddpf->second.push_back(ddp);
      }
      // also a flat description, the DataDescription view:
      v_basebandName_.push_back(bbn);
      v_spwId_.push_back(ddp.spwId);
      v_numChan_.push_back(ddp.numChan);
      v_numPol_.push_back(ddp.numCorr);
      //       }else{
      // 	Error(SERIOUS,"BasebandName ASDM item missing when spectralWindowId=%d",
      // 	      atoi(ddPtr->getSpectralWindowId().toString().c_str()));
      //       }
      /*
	if(v_basebandNum_[n]<1||v_basebandNum_[n]>numBaseband_){
	cout << "ERROR: bbcNo must have a value in the range [1,"<<numBaseband_<<"]"<<endl;
	return;
	}
      */
      e_bbn=v_basebandName_[n];
      if(coutest){
	cout << "v_spwId_["<<n<<"]        = "<< v_spwId_[n].toString() << endl;
	cout << "v_numChan_["<<n<<"]      = "<< v_numChan_[n] << endl;
	cout << "v_numPol_["<<n<<"]       = "<< v_numPol_[n] << endl;
	cout << "v_basebandName_["<<n<<"] = "<< e_bbn.str() << endl;
      }
      mapScToBaseband();
    }
    unsigned int numBaseband = m_bn_v_ddp_.size();
    if(coutest)cout << "The list of dataDescription for configDescriptionId involves "
		    << numBaseband << " basebands" 
		    << endl;


    //     // check that the basebands are in the order constrained by the definition of the BasebandName enumeration:
    //     for(unsigned int n=1; n<v_basebandName_.size(); n++){
    //       if(v_basebandName_[n]<v_basebandName_)
    // 	Error( SERIOUS,"The sequence of spectral windows does not correspond to an ascending\n sequence of basebands");
    //     }
      
    // sequence of basebands, nb of spw/bb for each member of this sequence and nb of spectral point within each of these spw
    bool                  newBaseband;
    EnumSet<BasebandName> es_bn;
    // sequence of basebands v_basebandSet_
    for(unsigned int ndd=0; ndd<v_dataDescriptionIdArray.size(); ndd++){
      newBaseband = true;
      if(es_bn[v_basebandName_[ndd]])newBaseband = false;
      es_bn.set(v_basebandName_[ndd],false);
      if(newBaseband){
	v_basebandSet_.push_back(v_basebandName_[ndd]);
      }
    }
    if(coutest)cout << "The list of dataDescriptions for configDescriptionId involves "
		    << numBaseband << " basebands following the sequence:" 
		    << endl;

    for(unsigned int nbb=0; nbb<numBaseband; nbb++)v_numSpwPerBb_.push_back(0);
    for(unsigned int nbb=0; nbb<numBaseband; nbb++){
      itbnddpf=m_bn_v_ddp_.find(v_basebandSet_[nbb]);    
      // nb of spw/bb for each member of this sequence
      v_numSpwPerBb_[nbb]=itbnddpf->second.size();
    

      Enum<BasebandName> e_bn=v_basebandSet_[nbb];
      if(coutest)cout << " - baseband " << e_bn.str()
		      << " has " << v_numSpwPerBb_[nbb] 
		      << " spectral windows "
		      << endl;
      vector<int> v_ndd;
      // nb of spectral point within each of these spw, baseband organized, following the sequence input order
      for(unsigned int ndd=0; ndd<itbnddpf->second.size(); ndd++)
	v_ndd.push_back(itbnddpf->second[ndd].numChan);
      vv_nsp_.push_back(v_ndd);
    }


    /* Determination of the dataDescription identifiers for the auto and cross-correlation.
       When both are stored the input list of identifiers is the one for the cross correlations.
       The method below searches for the implicit dataDescriptionId corresponding to the 
       auto-correlation coming with the data identifier of the cross-correlation dataDescriptionId.
       Note that in the case of DSB receivers with sideband separation using 90d phase switching
       to the two sideband spectral windows for the cross data correspond only one implicit DSB
       spectral window for the auto-correlation data. 
    */
    Enum<CorrelationMode> e_cm; e_cm=corrMode;
    if(e_cm[CROSS_ONLY]){                        // Case when there are only cross-correlations:
      if(coutest)cout << "Only cross-correlations use-case " << endl;
      for(unsigned int n=0; n<numDataDescription_; n++){
	v_crossDataDescriptionId_.push_back(v_dataDescriptionIdArray[n]);
	v_pairDataDescriptionId_.push_back(false);
      }
    }else if(e_cm[AUTO_ONLY]){                  // Case when there are only auto-correlations:
      if(coutest)cout << "Only auto-correlations use-case" << endl;
      for(unsigned int n=0; n<numDataDescription_; n++){
	v_autoDataDescriptionId_.push_back(v_dataDescriptionIdArray[n]);
	v_pairDataDescriptionId_.push_back(false);
      }
    }else{                                       // Case when there are both auto and cross-correlations
      for(unsigned int n=0; n<numDataDescription_; n++){
	v_crossDataDescriptionId_.push_back(v_dataDescriptionIdArray[n]);
	v_autoDataDescriptionId_.push_back(v_dataDescriptionIdArray[n]);
	v_pairDataDescriptionId_.push_back(true);
      }
    }
    // set the number of data and the sizes in number of primitive data type (PDT) values.
    size();
    if(coutest)cout << "Total size in correlated PDT data values (a complex being 2 PDT values):\n" 
		    << "   number of cross-correlations  " << sumCrossSize_
		    << "   number of auto-correlations   " << sumAutoSize_
		    << endl;

    // set the indices for the binary meta-data (timeCentroid and exposure)
    if(coutest)cout << "Build metadata index" << endl;
    metaDataIndex();

  }

  DataDescriptionsSet::DataDescriptionsSet(const DataDescriptionsSet & a) : SwitchCyclesList(a){
    //cout << "copy constructor DataDescriptionsSet" << endl;

    // attribute in the 'abstract' class SwitchCyclesList
    m_bn_v_scId_              = a.m_bn_v_scId_;
    m_bn_v_numBin_            = a.m_bn_v_numBin_;
    vv_numBin_                = a.vv_numBin_;
    v_numBin_                 = a.v_numBin_;
    datasetPtr_               = a.datasetPtr_;
    vv_switchCycleId_         = a.vv_switchCycleId_;
    v_switchCycleId_          = a.v_switchCycleId_;


    // attribute in the present DataDescriptionsSet class:
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
    //cout << "deep copy done" << endl;
  }

  DataDescriptionsSet::~DataDescriptionsSet(){}
  
  void DataDescriptionsSet::mapScToBaseband(){
    map<BasebandName,vector<DataDescParams> >::iterator itbnddp, itbnddpb, itbnddpf, itbnddpe;
    itbnddpb = m_bn_v_ddp_.begin();
    itbnddpe = m_bn_v_ddp_.end();
    if(vv_numBin_.size()){
      if(vv_numBin_.size()!=m_bn_v_ddp_.size())
	Error(SERIOUS,(char *)"The input dataDescription and switchCycle identifiers are not defined with the same nb of basebands");
      int nbb=0;
      for(itbnddp=itbnddpb; itbnddp!=itbnddpe; ++itbnddp){
	if(itbnddp->second.size()!=vv_numBin_[nbb].size())
	  Error(SERIOUS,(char *) "Nb of dataDescriptionId for baseband %d not equal to nb (%d) of switchCycleId for that baseband",
		itbnddp->second.size(),vv_numBin_[nbb].size());
	m_bn_v_numBin_.insert( make_pair(itbnddp->first,vv_numBin_[nbb]) );
	m_bn_v_scId_.insert( make_pair(itbnddp->first,vv_switchCycleId_[nbb]) );
      }
    }else{
      for(itbnddp=itbnddpb; itbnddp!=itbnddpe; ++itbnddp){
	vector<int> vi;
	vector<Tag> vt;
	for(unsigned int n=0; n<itbnddp->second.size(); n++){
	  vi.push_back(itbnddp->second[n].numBin);
	  vt.push_back(itbnddp->second[n].scId);
	}
	m_bn_v_numBin_.insert( make_pair(itbnddp->first,vi) );
 	m_bn_v_scId_.insert(   make_pair(itbnddp->first,vt) );
      }
    }
  }

  
  void DataDescriptionsSet::size(){
    /* Determine for every data description:
     *
     * v_numAutoData_, v_autoSize_,  v_cumulAutoSize, sumAutoSize_
     * v_numCrossData_,v_crossSize_, v_cumulCrossSize_, sumCrossSize_
     *
     * The two first vectors describe the leaf for the auto and cross use-cases in term of 
     * number of pixels (num) and number of values (a complex number counting for 2 values).
     * The third vector defines the cumulative distribution of sizes as a function of the 
     * dataDescrition index. The dimensionality of the leaf, 4, is set by the axes APC, 
     * BIN, SPP and POL
     * WARNING: The size of the APC axis is considered to be 1 for auto
     * sumAutoSize_ (sumCrossSize_) is the sum of the elements of v_autoSize_ (v_crossSize_) 
     * Note that the number of values and size of a leaf is null for auto when a cross 
     * dataDescrition has no associated auto  dataDescrition (use-case CROSS_AND_AUTO with 
     * DSB 90d phase switching, the auto having no sideband separation).
     */  
    bool coutest=false;
    sumAutoSize_  = 0;
    sumCrossSize_ = 0;
    unsigned long dataSize, numVis, numAuto, nv;
    unsigned long numAutoPol;
    // int k;   // set in old code, but never actually used, kept in the event I work out what it was originally for and need it
    for(unsigned int ndd=0; ndd<numDataDescription_; ndd++){
      nv = v_numChan_[ndd] * v_numBin_[ndd]; 
      if(correlationMode_!=CROSS_ONLY){   // case there are auto data
	// if(v_numPol_[n]==1)numAutoPol = 1;                    modif Feb 06
	if(e_cm_[AUTO_ONLY]){             // single dish case: the list of dd is the list of single-dish dd
	  numAutoPol = numPol(ndd);
	}else{
	  numAutoPol = numSdPol(ndd);     // both single dish and interfero: the dd list is the list of interfero dd
	}
	v_cumulAutoSize_.push_back(sumAutoSize_);
	if(coutest)cout << "numChan=    " << v_numChan_[ndd] << endl;
	if(coutest)cout << "numAutoPol= " << numAutoPol << endl;
	if(coutest)cout << "numBin=     " << v_numBin_[ndd] << endl;
	numAuto = nv*numAutoPol;                                      // number of autocorr
	if(e_cm_[CROSS_AND_AUTO])
	  if(!v_pairDataDescriptionId_[ndd])numAuto=0;                // case of the image of a DSB sideband already registred

	v_numAutoData_.push_back(numAuto);                            // a re datum as well as a complex data counts for 1 num 
	if(coutest)cout << "numAuto/dd= " << v_numAutoData_[ndd] << endl;   

	if(numAutoPol==3)
	  dataSize = nv*(numAutoPol+1);                               // because XY data have 1re + 1 im)
	else
	  dataSize = numAuto;

	if(e_cm_[CROSS_AND_AUTO])
	  if(!v_pairDataDescriptionId_[ndd])dataSize=0;               // case of the image of a DSB sideband already registred


	v_autoSize_.push_back(dataSize);                              // size in nb of PDT values 
	sumAutoSize_  = sumAutoSize_  + dataSize;                     // grand total size for auto
      }
      if(!e_cm_[AUTO_ONLY]){              // there are cross data
	if(coutest)cout << "numApc=     " << numApc_ << endl;
	if(coutest)cout << "numChan=    " << v_numChan_[ndd] << endl;
	if(coutest)cout << "numPol=     " << v_numPol_[ndd]  << endl;
	if(coutest)cout << "numBin=     " << v_numBin_[ndd]  << endl;
	// k = 1;   // see note above where it was originally declared, set but not used
	// if(e_cm_[CROSS_ONLY])k=0;                                     // case with only cross-correlations, set but not used
	v_cumulCrossSize_.push_back(sumCrossSize_);
	numVis = nv*v_numPol_[ndd]*numApc_;                 
	if(coutest) cout << "numVis=     " << numVis << endl;
	v_numCrossData_.push_back(numVis);                            // number of complex visibilities
	dataSize = numVis*2;                                          // a complex is composed of 2 primitive data type values
	v_crossSize_.push_back(dataSize);
	sumCrossSize_ = sumCrossSize_ + dataSize;                     // grand total size for cross 
      }
    }
    if(coutest)cout << "Total size in auto-correlation data per zero baseline: " << sumAutoSize_  << " PDT values" << endl;
    if(coutest)cout << "Total size in cross-correlation data per baseline:     " << sumCrossSize_ << " PDT values" << endl;

    // instantiate the vector of meta data (timeCentroid and exposure) indices

    return;
  }

  void DataDescriptionsSet::metaDataIndex(){
    sumMetaDataIndex_ = 0;

    for(unsigned int ndd=0; ndd<numDataDescription_; ndd++){
      v_metaDataIndex_.push_back(sumMetaDataIndex_);
      sumMetaDataIndex_  = sumMetaDataIndex_  + v_numBin_[ndd];
    }
    return;
  }

  unsigned int DataDescriptionsSet::sumMetaDataIndex(){
    return sumMetaDataIndex_;
  }

  unsigned int DataDescriptionsSet::metaDataIndex( unsigned int ndd) {
    if((unsigned int)ndd>=v_metaDataIndex_.size())
      Error(FATAL,(char *) "The dataDescription index index must be smaller than %d",
	    v_metaDataIndex_.size());
    return v_metaDataIndex_[ndd];
  }


  unsigned long DataDescriptionsSet::getNumAutoData(Tag autoDataDescriptionId) { //throw (Error){
    bool coutest=false;
    int m=-1;
    for(unsigned int n=0; n<v_autoDataDescriptionId_.size(); n++){
      if(coutest)cout << autoDataDescriptionId.toString()
		      << " compared with" 
		      << v_autoDataDescriptionId_[n].toString()
		      << endl;
      if(autoDataDescriptionId==v_autoDataDescriptionId_[n])m=n;
    }
    if(m<0){
      Error(SERIOUS, (char *)"requested autoDataDescriptionId %s not present for the spectro-polarization setup", 
	    autoDataDescriptionId.getTag().c_str());
      return 0;
    }
    return v_numAutoData_[m];
  }

  unsigned long DataDescriptionsSet::getNumAutoData(){
    unsigned long numData=0;
    for(unsigned int n=0; n<v_autoDataDescriptionId_.size(); n++)
      numData = numData+getNumAutoData(v_autoDataDescriptionId_[n]);
    return numData;
  }

  unsigned long DataDescriptionsSet::getNumCrossData(Tag crossDataDescriptionId)  {
    int m=-1;
    for(unsigned int n=0; n<v_crossDataDescriptionId_.size(); n++)
      if(crossDataDescriptionId==v_crossDataDescriptionId_[n])m=n;
    if(m<0){
      Error(SERIOUS,(char *) "requested crossDataDescriptionId %s not present in the spectro-polarization setup", 
	    crossDataDescriptionId.getTag().c_str()); 
      return 0;
    }
    return v_numCrossData_[m];
  }


  unsigned long DataDescriptionsSet::getNumCrossData(){
    unsigned long numData=0;
    for(unsigned int n=0; n<v_crossDataDescriptionId_.size(); n++)
      numData = numData+getNumCrossData(v_crossDataDescriptionId_[n]);
    return numData;
  }

  Tag         DataDescriptionsSet::getAutoDataDescriptionId(Tag crossDataDescriptionId) {

    for(unsigned int ndd=0; ndd<v_crossDataDescriptionId_.size(); ndd++){
      if(crossDataDescriptionId==v_crossDataDescriptionId_[ndd])
	return v_autoDataDescriptionId_[ndd];
    }
    Error(SERIOUS,
	  (char *) "The data description identifier %s does not exist in the spectro-polarization setup",
	  crossDataDescriptionId.toString().c_str()); 
    return Tag((unsigned int)32767);
  }


  vector<Tag> DataDescriptionsSet::getAutoDataDescriptionId(){
    return v_autoDataDescriptionId_;
  }



  vector<Tag> DataDescriptionsSet::getCrossDataDescriptionId(){
    return v_crossDataDescriptionId_;
  }

  unsigned int DataDescriptionsSet::numBaseband(){
    return v_basebandSet_.size();
  }

  unsigned int DataDescriptionsSet::getNumChan(unsigned int ndd) {
    if((unsigned int)ndd>=v_numChan_.size())
      Error(SERIOUS,(char *) "The dataDescription number exceeds %d",
	    v_numChan_.size());
    return v_numChan_[ndd];
  }

  unsigned int DataDescriptionsSet::getNumPol(unsigned int basebandIndex) {
    if(basebandIndex>=v_basebandSet_.size())Error(FATAL,(char *) "No baseband with this index");

    return getNumPol(v_basebandSet_[basebandIndex]);
  }

  unsigned int DataDescriptionsSet::getNumPol(BasebandName basebandName) {


    unsigned int numpp=0;
  
    map<BasebandName,vector<DataDescParams> >::iterator 
      itbbnddpf=m_bn_v_ddp_.find(basebandName),
      itbbnddpe=m_bn_v_ddp_.end();
    if(itbbnddpf==itbbnddpe)return numpp;

    numpp =itbbnddpf->second[0].numCorr;
    if(itbbnddpf->second.size()==1)return numpp;
    
    Enum<BasebandName> e_bn=itbbnddpf->first;
    for(unsigned int n=1; n<itbbnddpf->second.size(); n++)
      if(itbbnddpf->second[n].numCorr!=numpp)
	Error(FATAL,(char *) "The dataDescriptions in %s do not have a common nb of pol. cross product",
	      e_bn.str().c_str());
    return numpp;
  }

  unsigned int DataDescriptionsSet::getNumSdPol(unsigned int basebandIndex) {
    if(basebandIndex>=v_basebandSet_.size())Error(FATAL, (char *) "No baseband with this index");

    return getNumSdPol(v_basebandSet_[basebandIndex]);
  }

  unsigned int DataDescriptionsSet::getNumSdPol(BasebandName basebandName) {

    int numpp=0;
  
    map<BasebandName,vector<DataDescParams> >::iterator 
      itbbnddpf=m_bn_v_ddp_.find(basebandName),
      itbbnddpe=m_bn_v_ddp_.end();
    if(itbbnddpf==itbbnddpe)return numpp;

    PolarizationTable& rpolSet =  datasetPtr_->getPolarization(); 
    numpp = rpolSet.getRowByKey(itbbnddpf->second[0].polId)->getNumCorr();

    if(itbbnddpf->second.size()==1)return numpp;
    
    Enum<BasebandName> e_bn=itbbnddpf->first;
    int                numCorr;
    for(unsigned int n=1; n<itbbnddpf->second.size(); n++){
      numCorr = rpolSet.getRowByKey(itbbnddpf->second[n].polId)->getNumCorr();
      if(numCorr!=numpp)
	Error(FATAL,(char *) "The dataDescriptions in %s do not have a common nb of sd pol. cross product",
	      e_bn.str().c_str());
    }
    return numpp;
  }

  unsigned int DataDescriptionsSet::getNumSpw(unsigned int basebandIndex) {
    if(basebandIndex>=v_basebandSet_.size())Error(FATAL,(char *) "No baseband with this index");
    return v_basebandSet_.size() ;
  }

  unsigned int DataDescriptionsSet::getNumSpw(BasebandName basebandName){
    map<BasebandName,vector<DataDescParams> >::iterator 
      itbbnddpf=m_bn_v_ddp_.find(basebandName),
      itbbnddpe=m_bn_v_ddp_.end();
    if(itbbnddpf==itbbnddpe)return 0;
    return itbbnddpf->second.size();
  }

  unsigned int DataDescriptionsSet::getNdd(unsigned int basebandIndex, unsigned int i) {
    if(basebandIndex>=v_basebandSet_.size())Error(FATAL,(char *) "No baseband with this index");

    return getNdd(v_basebandSet_[basebandIndex],i);
  }

  unsigned int DataDescriptionsSet::getNdd(BasebandName basebandName, unsigned int i) {

    map<BasebandName,vector<DataDescParams> >::iterator 
      itbbnddpf=m_bn_v_ddp_.find(basebandName),
      itbbnddpe=m_bn_v_ddp_.end();
    Enum<BasebandName> e_bn; e_bn=basebandName;
    if(itbbnddpf==itbbnddpe)Error(FATAL,(char *) "No baseband with the name %s for this configuration",
				  e_bn.str().c_str() );

    if(i>=itbbnddpf->second.size())
      Error(FATAL,(char *) "Index %d too large; it cannot exceed %d",i,itbbnddpf->second.size());
    Tag spwId = itbbnddpf->second[i].spwId;
    for(unsigned int ndd=0; ndd<v_spwId_.size(); ndd++)if(v_spwId_[ndd]==spwId)return ndd;
    Error(FATAL,(char *)"error in the algorithm");
    return 0;
  }

  unsigned long     DataDescriptionsSet::getAutoSize(){
    return sumAutoSize_; 
  }

  unsigned long     DataDescriptionsSet::getCrossSize(){
    return sumCrossSize_;
  }

  BasebandName      DataDescriptionsSet::getBasebandName(unsigned int ndd) {
    if(ndd>v_basebandName_.size()-1)
      Error(SERIOUS,(char *) "The dataDescription index index must be smaller than %d",
	    v_basebandName_.size());
    return v_basebandName_[ndd];
  }

  unsigned int      DataDescriptionsSet::getBasebandIndex(unsigned int ndd) {
    if(ndd>v_basebandName_.size()-1)
      Error(SERIOUS,(char *) "The dataDescription index index must be smaller than %d",
	    v_basebandName_.size());
    for(unsigned int nbb=0; nbb<v_basebandSet_.size(); nbb++)
      if(v_basebandName_[ndd]==v_basebandSet_[nbb])return nbb;
    Error(FATAL,(char *) "Problem in the algorithm");
    return 0; // should never happen
  }

  CorrelationMode   DataDescriptionsSet::getCorrelationMode(){
    return correlationMode_;
  }

  unsigned int    DataDescriptionsSet::getNumDataDescription(){
    return numDataDescription_;
  }

  unsigned int    DataDescriptionsSet::getDataDescriptionIndex( Tag dataDescriptionId) {
    int ndd = -1;
    for(unsigned int n=0; n<numDataDescription_; n++)
      if(dataDescriptionId==v_dataDescriptionIdArray_[n])ndd=n; // numbers are 1 based
    if(ndd==-1)Error(FATAL,
		     (char *) "The data description identifier %s is not in the spectro-polarization setup",
		     dataDescriptionId.toString().c_str());
    return ndd;
  }

  unsigned int    DataDescriptionsSet::numApc(){
    return es_apc_.count();
  }


  Enum<AtmPhaseCorrection> DataDescriptionsSet::atmPhaseCorrection(unsigned int atmPhaseCorrectionIndex){
    Enum<AtmPhaseCorrection> e_apc;
    e_apc=v_atmPhaseCorrection_[atmPhaseCorrectionIndex];
    //     for(unsigned int n=0; n<v_atmPhaseCorrection_.size(); n++)
    //       if(v_atmPhaseCorrection_[n]==atmPhaseCorrectionIndex)e_apc = v_atmPhaseCorrection_[n];
    return e_apc;
  }

  unsigned int             DataDescriptionsSet::atmPhaseCorrectionIndex(AtmPhaseCorrection apc) {
    for(unsigned int n=0; n<v_atmPhaseCorrection_.size(); n++)
      if(v_atmPhaseCorrection_[n]==apc)return n;
    Enum<AtmPhaseCorrection> e_apc=apc;
    Error(FATAL, (char *) "No %s along the APC axis",e_apc.str().c_str());
    return 0;
  }

  unsigned int    DataDescriptionsSet::numPol(unsigned int ndd) {
    if((unsigned int)ndd>=v_numPol_.size())
      Error(SERIOUS,(char *) "The dataDescription index index must be smaller than %d",
	    v_numPol_.size());
    return v_numPol_[ndd];
  }

  unsigned int    DataDescriptionsSet::numSdPol(unsigned int ndd) {
    if(e_cm_[CROSS_ONLY])return 0;
    if(ndd>=v_numPol_.size())
      Error(SERIOUS,(char *) "The dataDescription index index must be smaller than %d",
	    v_numPol_.size());
    if(e_cm_[AUTO_ONLY])return v_numPol_[ndd];	
    if(v_numPol_[ndd]<=2)return v_numPol_[ndd];
    return 3;                                        // could be 2 but dropping XY not supported yet!
  }

  unsigned int  DataDescriptionsSet::numChan(unsigned int ndd) {
    //if(ndd<0)Error(FATAL,(char *) "DataDescription index must be 0 based");
    return v_numChan_[ndd];
  }

  Tag DataDescriptionsSet::getSpwId(unsigned int ndd) {
    //if(ndd<0)Error(FATAL,(char *) "DataDescription index must be 0 based");
    return v_spwId_[ndd];
  }

  Frequency DataDescriptionsSet::totBandwidth(unsigned int ndd) {
    //if(ndd<0)Error(FATAL,(char *) "DataDescription index must be 0 based");
    if(ndd>=numDataDescription_){
      Error(SERIOUS,(char *) "The index for a DataDescriptions cannot exceed %d",
	    numDataDescription_);
      return 0;
    }
    DataDescriptionTable&  rddSet =  datasetPtr_->getDataDescription();
    return rddSet.getRowByKey(v_dataDescriptionIdArray_[ndd])->getSpectralWindowUsingSpectralWindowId()->getTotBandwidth();
  }


  int               DataDescriptionsSet::numAutoData(unsigned int ndd) {
    //if( ndd <= 0)Error(FATAL, (char *) "DataDescription index must be 0 based");
    if(v_numAutoData_.size()==0){
      Error(WARNING, (char *) "no auto data expected");
      return 0;
    }else if((unsigned int)ndd>=v_numAutoData_.size()){
      Error(SERIOUS, (char *) "The index for a DataDescriptions cannot exceed %d",
	    v_numAutoData_.size());
    }
    return v_numAutoData_[ndd];
  }

  unsigned int      DataDescriptionsSet::numCrossData(unsigned int ndd) {
    if(!v_numCrossData_.size()){
      Error(WARNING, (char *) "no cross data expected");
      return 0;
    }else if((unsigned int)ndd>=v_numCrossData_.size()){
      Error(SERIOUS, (char *) "The index for a DataDescriptions cannot exceed %d",
	    v_numCrossData_.size());
    }
    return v_numCrossData_[ndd];
  }

}
