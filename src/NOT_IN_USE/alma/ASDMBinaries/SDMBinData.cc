#include <dirent.h>
#include <stdio.h>

#include <vector>
#include <map>
#include <iostream>
#include <string>

#include <alma/ASDMBinaries/SDMBinData.h>
#include <alma/ASDMBinaries/Error.h>
#include <alma/ASDMBinaries/DataDescriptionsSet.h>

#include <alma/Enumtcl/AxisName.h>
#include <alma/Enumtcl/DataContent.h>
#include <alma/Enumtcl/NetSideband.h>
#include <alma/Enumtcl/PrimitiveDataType.h>
#include <alma/Enumtcl/CalibrationDevice.h>

using namespace asdm;
using namespace asdmbinaries;
using namespace std;

using namespace AtmPhaseCorrectionMod;
using namespace AxisNameMod;
using namespace CalibrationDeviceMod;
using namespace CorrelationModeMod;
using namespace CorrelatorTypeMod;
using namespace DataContentMod;
using namespace NetSidebandMod;
using namespace PrimitiveDataTypeMod;
using namespace ProcessorTypeMod;
using namespace ScanIntentMod;
using namespace SpectralResolutionTypeMod;
using namespace TimeSamplingMod;

namespace sdmbin {

//   SDMBinData::SDMBinData(){
//     datasetPtr_   = Singleton<ASDM>::instance();
//     execBlockDir_ = Singleton<ExecBlockDir>::instance()->dir();
//   }

  SDMBinData::SDMBinData( ASDM* const datasetPtr, string execBlockDir){
    // Are we going to be verbose ?
    verbose_ = getenv("ASDM_DEBUG") != NULL;

    DIR *dir;
    dir = opendir( execBlockDir.c_str());
    if(!dir)
      Error(FATAL, "In SDMBinData::SDMBinData( ASDM* const datasetPtr, string execBlockDir) : Directory " + execBlockDir + " not present present");
    closedir(dir);
    dir = NULL;

    string xmlfile = execBlockDir + "/Main.xml"; //cout << "xmlfile="<< xmlfile<<endl;
    FILE *pFile;
    pFile = fopen( xmlfile.c_str(),"r");
    if(!pFile)
      Error(FATAL, string("In SDMBinData::SDMBinData( ASDM* const datasetPtr, string execBlockDir) : Main table not present in the SDM dataset"));
    fclose(pFile);
    pFile = NULL;

    execBlockDir_ = execBlockDir;
    mainRowPtr_   = NULL;
    datasetPtr_   = datasetPtr;
    dataOID_      = "";
    canSelect_    = true;
    es_si_.flip();              // default: accept any scan
    es_pt_.flip();              // default: accept any processor type when selecting the rows to be processed
    es_cm_.flip();              // default: accept any correlation mode when selecting the rows to be processed
    es_srt_.flip();             // default: accept any spectral resolution type when selecting the rows to be processed
    es_ts_.flip();              // default: accept any time sampling when selecting the rows to be processed
    e_qcm_.set(CROSS_AND_AUTO); // default: select all data present in the BLOB (the HLA enum model being not set-based...)
    es_qapc_.flip();            // default: select all the full APC axis in the BLOB
    ddfirst_=false;

    bdfMemoryMapped = getenv("BDF_MEMORY_MAPPED") != NULL;
  }

  SDMBinData::~SDMBinData(){
    if(coutDeleteInfo_)cout<<"Destructor SSDMBinData"<<endl;

    for(unsigned int n=0; n<v_msDataPtr_.size();  n++) {
      if(v_msDataPtr_[n]){deleteMsData(v_msDataPtr_[n]);}
    }
    v_msDataPtr_.clear();

    for(unsigned int n=0; n<v_sdmDataPtr_.size(); n++){
      if(v_sdmDataPtr_[n]){delete v_sdmDataPtr_[n]; }
    }
    v_sdmDataPtr_.clear();

    if(vmsDataPtr_) {
      delete vmsDataPtr_;
      vmsDataPtr_ = NULL;
    }

    // msDataPtr_ is used to pass things around
    // the pointers to be deleted are handled where they are found - usually in v_msDataPtr_
    // if(msDataPtr_ ) {
    //   deleteMsData(msDataPtr_);
    //   msDataPtr_ = NULL;
    // }

    // sdmDataPtr_ appears to not be used here, unsure what it is or where it comes from
    // if(sdmDataPtr_) {
    //   delete sdmDataPtr_;
    //   sdmDataPtr_ = NULL;
    // }

    // v_dataDump_
    vector<DataDump*>::reverse_iterator
      it,
      itrb=v_dataDump_.rbegin(),
      itre=v_dataDump_.rend();
    for(it=itrb; it!=itre; it++){
      delete *it;
    }
    v_dataDump_.clear();

    for (map<Tag,BaselinesSet*>::iterator it=m_cdId_baselinesSet_.begin();it!=m_cdId_baselinesSet_.end();++it) {
      if (it->second != NULL) delete(it->second);
    }
    m_cdId_baselinesSet_.clear();

    if(coutDeleteInfo_)cout<<"FIN: SSDMBinData object deleted"<<endl;
  }

  void SDMBinData::select( EnumSet<ScanIntent> es_si){
    if(canSelect_)
      es_si_ = es_si;
    else
      Error(WARNING,string("Selecting a subset of rows in the Main table at this stage is forbidden"));
  }

  void SDMBinData::select( EnumSet<ProcessorType> es_pt){
    if(canSelect_)
      es_pt_ = es_pt;
    else
      Error(WARNING,string("Selecting a subset of rows in the Main table at this stage is forbidden"));
  }

  void SDMBinData::select( EnumSet<CorrelationMode> es_cm){
    if(canSelect_)
      es_cm_ = es_cm;
    else
      Error(WARNING,string("Selecting a subset of rows in the Main table at this stage is forbidden"));
  }

  void SDMBinData::select( EnumSet<SpectralResolutionType> es_srt){
    if(canSelect_)
      es_srt_ = es_srt;
    else
      Error(WARNING,string("Selecting a subset of rows in the Main table at this stage is forbidden"));
  }

  void SDMBinData::SDMBinData::select( EnumSet<TimeSampling> es_ts){
    if(canSelect_)
      es_ts_ = es_ts;
    else
      Error(WARNING,string("Selecting a subset of rows in the Main table at this stage is forbidden"));
  }

  void SDMBinData::select( EnumSet<CorrelationMode>        es_cm,
			   EnumSet<SpectralResolutionType> es_srt,
			   EnumSet<TimeSampling>           es_ts){
    if(canSelect_){
      es_cm_  = es_cm;
      es_srt_ = es_srt;
      es_ts_  = es_ts;
    }else{
      Error(WARNING,string("Selecting a subset of rows in the Main table at this stage is forbidden"));
    }
  }

  void SDMBinData::selectDataSubset(  Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc){
    if(canSelect_){
      e_qcm_   = e_qcm;
      es_qapc_ = es_qapc;
    }else{
      Error(WARNING,string("Selecting a subset of data in the BLOB(s) at this stage is forbidden"));
    }
  }

  string SDMBinData::reasonToReject(MainRow* const mainRowPtr){

    //bool                  coutest=false;
    bool                  ok;
    string                reason = "";
    ConfigDescriptionRow* cdr    = mainRowPtr->getConfigDescriptionUsingConfigDescriptionId();
    ScanRow*              sr;

    if(verbose_){
      cout << "SDMBinData::reasonToReject(MainRow* const mainRowPtr) : entering." << endl;
      cout<<cdr->getConfigDescriptionId().toString()<<endl;
      vector<DataDescriptionRow*> v_ddr=cdr->getDataDescriptionsUsingDataDescriptionId();
      for(unsigned int n=0; n<v_ddr.size(); n++)cout<<v_ddr[n]->getDataDescriptionId().toString()<<" ";
      cout<<endl;
    }

    // the scan intent:
    sr = datasetPtr_->getScan().getRowByKey( mainRowPtr->getExecBlockUsingExecBlockId()->getExecBlockId(),
					     mainRowPtr->getScanNumber() );
    if (sr == (ScanRow*) 0) {
      ostringstream oss;
      oss << "There is no row in the Scan table with 'execBlockId==" << mainRowPtr->getExecBlockId().toString()
	  << "' and 'scanNumber==" << mainRowPtr->getScanNumber() << "'.";
      Error(FATAL, oss.str());
    }
    vector<ScanIntent> v_si=sr->getScanIntent();
    ok=false;
    for(unsigned int n=0 ; n<v_si.size(); n++)if(es_si_[v_si[n]])ok=true;
    if(!ok){
      reason += " no overlap with the scan intent set {"+es_si_.str()+"}";
    }

    // the processor type:
    if(!es_pt_[cdr->getProcessorUsingProcessorId()->getProcessorType()]) {  
      reason +=  " no overlap with the processor type set {"+es_pt_.str()+"}";
    }

    // the correlation mode:
    if(!es_cm_[cdr->getCorrelationMode()]) {
      reason += "no overlap with the corretion mode set {"+es_cm_.str()+"}";
    }

    // the spectral resolution type:
    vector<SpectralResolutionType>  v_srt;
    if (cdr->isAssocNatureExists()) v_srt=cdr->getAssocNature();
    if(verbose_)
      for(unsigned int n=0; n<v_srt.size(); n++)
	cout<<"v_srt["<<n<<"]="<<Enum<SpectralResolutionType>(v_srt[n]).str()<<endl;
    ok=false;
    EnumSet<SpectralResolutionType> es_srt; es_srt.set(v_srt); v_srt = es_srt.flip().toEnumType();
    if(verbose_)
      for(unsigned int n=0; n<v_srt.size(); n++)
	cout<<"v_srt["<<n<<"].flip()="<<Enum<SpectralResolutionType>(v_srt[n]).str()<<endl;
    for(unsigned int n=0; n<v_srt.size(); n++)if(es_srt_[v_srt[n]])ok=true;
    if(!ok) {
      reason += " no overlap with the spectral resolution type set {"+es_srt_.str()+"}";
    }

    // the time sampling:
    if(!es_ts_[mainRowPtr->getTimeSampling()]) {
	reason += " no overlap with the time sampling set {"+es_ts_.str()+"}";
    }

    // the correlation mode data subset
    if(e_qcm_[CROSS_ONLY] && cdr->getCorrelationMode()==AUTO_ONLY) {
      reason += " no cross correlations";
    }

    if(e_qcm_[AUTO_ONLY] && cdr->getCorrelationMode()==CROSS_ONLY) {
      reason += " no auto correlations";
    }


    // the apc axis
    // We pay attention to it if and only if the processor is not a radiometer.
    //
    if (processorType(mainRowPtr) != RADIOMETER) {
      EnumSet<AtmPhaseCorrection> es_apc;
      vector<AtmPhaseCorrection>  v_apc=cdr->getAtmPhaseCorrection();
      ok=false;
      for(unsigned int n=0; n<v_apc.size(); n++)if(es_qapc_[v_apc[n]])ok=true;
      if(!ok) {
	reason += " no overlap with the APC set {"+es_qapc_.str()+"}";
      }
    }

    if(verbose_){
      cout << "SDMBinData::reasonToReject(MainRow* const mainRowPtr) : exiting." << endl;
    }
    return reason;
  }

  void SDMBinData::setPriorityDataDescription(){
    ddfirst_=true;
  }

  bool SDMBinData::dataDescriptionFirst()const{
    return ddfirst_;
  }

  void SDMBinData::applySysCal(){
    syscal_=true;
    return;
  }

  bool SDMBinData::sysCalApplied() const{
    return syscal_;
  }

  bool SDMBinData::openMainRow( MainRow* const mainRowPtr){
    if (verbose_) cout << "SDMBinData::openMainRow : entering" << endl;

    if( reasonToReject(mainRowPtr).length() )return false;

    ConfigDescriptionRow* cdr  = mainRowPtr->getConfigDescriptionUsingConfigDescriptionId();


    // proceed, the row having been accepted:
    // -------------------------------------

    mainRowPtr_ = mainRowPtr;


    // process the config description (if not yet done) for this row:
    Tag cdId = cdr->getConfigDescriptionId();                       if(verbose_)cout<< "ici 1"<<cdId.toString() <<endl;
    set<Tag>::iterator
      itcdIdf=s_cdId_.find(cdId),
      itcdIde=s_cdId_.end();
    if(itcdIdf==itcdIde){
      DataDescriptionsSet dataDescriptionsSet( datasetPtr_,
					       cdr->getSwitchCycleId(),
					       cdr->getDataDescriptionId(),
					       cdr->getCorrelationMode(),
					       cdr->getAtmPhaseCorrection());                    if(verbose_)cout << "ici 3"<<endl;

      unsigned int numAnt=cdr->getNumAntenna();
      vector<int>  v_phasedArrayList;  for(unsigned int na=0; na<numAnt; na++)v_phasedArrayList.push_back(0);
      vector<bool> v_antennaUsedArray; for(unsigned int na=0; na<numAnt; na++)v_antennaUsedArray.push_back(true);
      BaselinesSet* baselinesSetPtr = new BaselinesSet( cdr->getAntennaId(),
							cdr->getFeedId(),
							v_phasedArrayList,
							v_antennaUsedArray,
							dataDescriptionsSet);  if(verbose_)cout << "ici 4"<<endl;
      m_cdId_baselinesSet_.insert(make_pair(cdId,baselinesSetPtr));
      s_cdId_.insert(cdId);
    }else{
      if(verbose_)cout<<"reuse cdId="<<cdId.toString()<<endl;
    }

    if(verbose_)cout << "ici 4 dataOID="<<mainRowPtr->getDataUID().getEntityId().toString()<<endl;

    int iostat = openStreamDataObject(mainRowPtr->getDataUID().getEntityId().toString());
    if(iostat==0){
      if(verbose_){
	cout << "Successfully opened Stream Data Object" << endl;
      }
    }else{
      Error(WARNING,string("No data retrieving for this SDM mainTable row"));
      return false;
    }
    if (verbose_) cout << "SDMBinData::openMainRow : exiting" << endl;
    return true;
  }

  ProcessorType SDMBinData::processorType(MainRow* const mainRowPtr) const {
    if (verbose_) cout << "SDMBinData::processorType(MainRow* const mainRowPtr) : entering" << endl;
    Tag cdId = mainRowPtr->getConfigDescriptionId();
    ConfigDescriptionTable& cT = mainRowPtr->getTable().getContainer().getConfigDescription();
    ConfigDescriptionRow* cR = cT.getRowByKey(cdId);
    Tag pId = cR->getProcessorId();
    
    
    ProcessorType processorType =  mainRowPtr->getTable().getContainer().getProcessor().getRowByKey(pId)->getProcessorType();

    if (verbose_) cout << "SDMBinData::processorType(MainRow* const mainRowPtr) : exiting" << endl;
    return processorType;
  }

  bool SDMBinData::acceptMainRow( MainRow* const mainRowPtr){

    //bool coutest=false;

    if (verbose_) cout << "SDMBinData::acceptMainRow( MainRow* const mainRowPtr) : entering." << endl;

    string whyToReject = reasonToReject(mainRowPtr);
    if( whyToReject.length() ){
      if (verbose_) {
	cout << "Main row rejected , reason : " << whyToReject << endl;
	cout << "SDMBinData::acceptMainRow( MainRow* const mainRowPtr) : exiting." << endl;
      }
      return false;
    }

    ConfigDescriptionRow* cdr  = mainRowPtr->getConfigDescriptionUsingConfigDescriptionId();


    // proceed, the row having been accepted:
    // -------------------------------------

    mainRowPtr_ = mainRowPtr;


    // process the config description (if not yet done) for this row:
    Tag cdId = cdr->getConfigDescriptionId();                       if(verbose_)cout<< "ici 1"<<cdId.toString() <<endl;
    set<Tag>::iterator
      itcdIdf=s_cdId_.find(cdId),
      itcdIde=s_cdId_.end();
    if(itcdIdf==itcdIde){
      DataDescriptionsSet dataDescriptionsSet( datasetPtr_,
					       cdr->getSwitchCycleId(),
					       cdr->getDataDescriptionId(),
					       cdr->getCorrelationMode(),
					       cdr->getAtmPhaseCorrection());                    if(verbose_)cout << "ici 3"<<endl;

      unsigned int numAnt=cdr->getNumAntenna();
      vector<int>  v_phasedArrayList;  for(unsigned int na=0; na<numAnt; na++)v_phasedArrayList.push_back(0);
      vector<bool> v_antennaUsedArray; for(unsigned int na=0; na<numAnt; na++)v_antennaUsedArray.push_back(true);
      BaselinesSet* baselinesSetPtr = new BaselinesSet( cdr->getAntennaId(),
							cdr->getFeedId(),
							v_phasedArrayList,
							v_antennaUsedArray,
							dataDescriptionsSet);  if(verbose_)cout << "ici 4"<<endl;
      m_cdId_baselinesSet_.insert(make_pair(cdId,baselinesSetPtr));
      s_cdId_.insert(cdId);
    }else{
      if(verbose_)cout<<"reuse cdId="<<cdId.toString()<<endl;
    }

    if(verbose_)cout << "ici 4 dataOID="<<mainRowPtr->getDataUID().getEntityId().toString()<<endl;

    int iostat = 0;
    if (bdfMemoryMapped) {
      iostat = attachDataObject(mainRowPtr->getDataUID().getEntityId().toString());
    }
    else {  
      iostat = attachStreamDataObject(mainRowPtr->getDataUID().getEntityId().toString());
    }
    if(iostat==0){
      if(verbose_){
	cout<<"Summary of the data object properties at URI: "<<dataOID_<<endl;
	cout<<"v_dataDump_.size()="<<v_dataDump_.size()<<endl;
	v_dataDump_[0]->summary();
      }
    }else{
      Error(WARNING,string("No data retrieving for this SDM mainTable row"));
      if (verbose_) cout << "SDMBinData::acceptMainRow( MainRow* const mainRowPtr) : exiting." << endl;
      return false;
    }

    if (verbose_) cout << "SDMBinData::acceptMainRow( MainRow* const mainRowPtr) : exiting." << endl;
    return true;
  }

  int SDMBinData::attachDataObject(string dataOID ){

    bool coutest=false;

    if(coutest)cout<<"Entree dans attachDataObject"<<endl;

    if(dataOID_.length()){
      cout << "dataOID_ = " << dataOID_ << ", dataOID = " << dataOID <<  endl;
      //Error(WARNING,"Not assumed to have more than one BLOB open read-only");
      //if(coutest)
      cout<<"Close "<<dataOID_<<" before opening "<<dataOID<<endl;
      detachDataObject();
    }
    
    if(coutest)cout<<"attach BLOB "<<dataOID<<endl;


    // transforme le dataOID en file name (re-utilise code ds la methode beginWithMimeHeader de SDMBLOBs)  TODO
    string filename=dataOID;
    string::size_type np;
    np=filename.find("/",0);
    while(np!=string::npos){ np=filename.find("/",np); if(np!=string::npos){ filename.replace(np,1,"_"); np++; } }
    np=filename.find(":",0);
    while(np!=string::npos){ np=filename.find(":",np); if(np!=string::npos){ filename.replace(np,1,"_"); np++; } }
    if(coutest)cout<<"filename="<<filename<<" execBlockDir_="<<execBlockDir_<<endl;
    static DIR* dirp;
//     ostringstream dir; dir<<execBlockDir_<<"/SDMBinaries";
    ostringstream dir; dir<<execBlockDir_<<"/ASDMBinary";
    dirp = opendir(dir.str().c_str());
    if(!dirp)
      Error(FATAL,(char *) "Could not open directory %s",dir.str().c_str());
    closedir(dirp);
    filename=dir.str()+"/"+filename;
    if(coutest)cout<<"filename="<<filename<<endl;

    try {
      const SDMDataObject& dataObject = blob_r_.read(filename);
      dataOID_ = dataOID;
      if(coutest){
	cout<<"============================"<<endl;
	cout<< dataObject.toString()<<endl;
	cout<<"============================"<<endl;
      }
//       dataObject.execBlockUID();
//       dataObject.execBlockNum();
//       dataObject.scanNum();
//       dataObject.subscanNum();

      Enum<CorrelationMode>            e_cm;
      Enum<ProcessorType>              e_pt;
      Enum<CorrelatorType>             e_ct;

      unsigned int                     numTime=0;

      unsigned int                     numAnt              = dataObject.numAntenna();
      CorrelationMode                  correlationMode     = dataObject.correlationMode(); e_cm=correlationMode;
      ProcessorType                    processorType       = dataObject.processorType();   e_pt=processorType;
      CorrelatorType                   correlatorType;

      const SDMDataObject::DataStruct& dataStruct          = dataObject.dataStruct();

      vector<AtmPhaseCorrection>       v_apc               = dataStruct.apc();
      vector<SDMDataObject::Baseband>  v_baseband          = dataStruct.basebands();
      SDMDataObject::ZeroLagsBinaryPart               zeroLagsParameters;

//      bool                             normalized=false;
      if(e_pt[ProcessorTypeMod::CORRELATOR]){
//	if(e_cm[CorrelationModeMod::CROSS_ONLY])
//	  normalized =  dataStruct.autoData().normalized();
	if(dataStruct.zeroLags().size()){
	  zeroLagsParameters = dataStruct.zeroLags();
	  correlatorType = zeroLagsParameters.correlatorType(); e_ct=correlatorType;
	}
      }
	
      unsigned int                     numApc              = v_apc.size();
      unsigned int                     numBaseband         = v_baseband.size();

      vector< vector< unsigned int > > vv_numAutoPolProduct;
      vector< vector< unsigned int > > vv_numCrossPolProduct;
      vector< vector< unsigned int > > vv_numSpectralPoint;
      vector< vector< unsigned int > > vv_numBin;
      vector< vector< float > >        vv_scaleFactor;
      vector< unsigned int >           v_numSpectralWindow;
      vector< vector< Enum<NetSideband> > > vv_e_sideband;
      vector< vector< SDMDataObject::SpectralWindow* > > vv_image;

      if(coutest)cout<<"e_cm="<<e_cm.str()<<endl;

      for(unsigned int nbb=0; nbb<v_baseband.size(); nbb++){
	vector<SDMDataObject::SpectralWindow>  v_spw = v_baseband[nbb].spectralWindows(); v_numSpectralWindow.push_back(v_spw.size());
	vector<SDMDataObject::SpectralWindow*> v_image;
	vector< unsigned int > v_numBin;
	vector< unsigned int > v_numSpectralPoint;
	vector< unsigned int > v_numSdPolProduct;
	vector< unsigned int > v_numCrPolProduct;
	vector< Enum<NetSideband> >  v_e_sideband;
	vector<float>          v_scaleFactor;
	for(unsigned int nspw=0; nspw<v_spw.size(); nspw++){
	  v_e_sideband.push_back(Enum<NetSideband>(v_spw[nspw].sideband()));
	  v_image.push_back(NULL);                                   // TODO waiting for implementation
	  v_numBin.push_back(v_spw[nspw].numBin());
	  if(coutest)cout<<"v_spw["<<nspw<<"].numSpectralPoint()="<<v_spw[nspw].numSpectralPoint()<<endl;
	  v_numSpectralPoint.push_back(v_spw[nspw].numSpectralPoint());
	  if(e_cm[AUTO_ONLY])
	    v_numSdPolProduct.push_back(v_spw[nspw].sdPolProducts().size());
	  else{
	    v_numCrPolProduct.push_back(v_spw[nspw].crossPolProducts().size());
	    v_scaleFactor.push_back(v_spw[nspw].scaleFactor());
	    if(e_cm[CROSS_AND_AUTO])
	      v_numSdPolProduct.push_back(v_spw[nspw].sdPolProducts().size());
	  }
	}
	if(e_cm[CROSS_ONLY]==false)vv_numAutoPolProduct.push_back(v_numSdPolProduct);
	if(e_cm[AUTO_ONLY]==false) vv_numCrossPolProduct.push_back(v_numCrPolProduct);
	vv_numSpectralPoint.push_back(v_numSpectralPoint);
	vv_e_sideband.push_back(v_e_sideband);
	vv_image.push_back(v_image);
	vv_numBin.push_back(v_numBin);
	vv_scaleFactor.push_back(v_scaleFactor);
      }
      if(e_cm[AUTO_ONLY])vv_numCrossPolProduct.clear();
      if(e_cm[CROSS_ONLY])vv_numAutoPolProduct.clear();

      complexData_=false;
      if(vv_numAutoPolProduct.size()){
	for(unsigned int nbb=0; nbb<vv_numAutoPolProduct.size(); nbb++){
	  for(unsigned int nspw=0; nspw<vv_numAutoPolProduct[nbb].size(); nspw++)
	    if(vv_numAutoPolProduct[nbb][nspw]>2)complexData_=true;
	}
      }
      if(vv_numCrossPolProduct.size())complexData_=true;

      EnumSet<AxisName>         es_axisNames;
      DataContent               dataContent;
      SDMDataObject::BinaryPart binaryPart;
      map<DataContent,pair<unsigned int,EnumSet<AxisName> > > m_dc_sizeAndAxes;

      dataContent = FLAGS;
      binaryPart  = dataStruct.flags();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = ACTUAL_DURATIONS;
      binaryPart  = dataStruct.actualDurations();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = ACTUAL_TIMES;
      binaryPart  = dataStruct.actualTimes();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = AUTO_DATA;
      binaryPart  = dataStruct.autoData();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      if(es_axisNames[TIM])numTime=dataObject.numTime();

      dataContent = CROSS_DATA;
      binaryPart  = dataStruct.crossData();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      if(binaryPart.size()){
	dataContent = ZERO_LAGS;
	binaryPart  = dataStruct.zeroLags();
	es_axisNames.set(binaryPart.axes(),true);
	if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
      }

      if(es_axisNames[TIM])numTime=dataObject.numTime();

      map<DataContent,pair<unsigned int,EnumSet<AxisName> > >::iterator
	itf=m_dc_sizeAndAxes.find(AUTO_DATA),
	ite=m_dc_sizeAndAxes.end();
      if(itf==ite)itf=m_dc_sizeAndAxes.find(CROSS_DATA);
      if(itf==ite)Error(FATAL,(char *) "BLOB %s has no declaration for observational data",dataOID.c_str());


      if(coutDeleteInfo_)cout<<"delete  v_dataDump_"<<endl;
      for(unsigned int n=0; n<v_dataDump_.size(); n++)delete v_dataDump_[n];
      v_dataDump_.clear();

      if(coutest)cout<<"numTime="<<numTime<<endl;

      if(numTime){  // MIME content with a structure of dimensionality 0

	const SDMDataSubset      dataSubset         = dataObject.tpDataSubset();
	uint64_t                 obsDuration        = dataSubset.interval();

	int64_t                  interval           = (int64_t)obsDuration/(int64_t)numTime;
	int64_t                  exposure;
	int64_t                  timeOfDump         = (int64_t)dataSubset.time()-(int64_t)obsDuration/2LL-interval/2LL; // mutable
	int64_t                  timeCentroid       = timeOfDump;         // default value for the first dump

	// actual pointer to the data blocks and nb of pdt values found in these blocks:
	const unsigned int*      flagsPtr           = NULL;  unsigned int long numFlags=0; // mcaillat 
	const int64_t*         actualTimesPtr     = NULL;  unsigned int long numActualTimes=0;
	const int64_t*         actualDurationsPtr = NULL;  unsigned int long numActualDurations=0;
	const float*             zeroLagsPtr        = NULL;  unsigned int long numZeroLags=0;
	const float*             autoDataPtr        = NULL;  unsigned int long numAutoData=0;
	const short int*         crossShortDataPtr  = NULL;  unsigned int long numCrossData=0;
	const int*               crossIntDataPtr    = NULL;
	const float*             crossFloatDataPtr  = NULL;


	// size (in nb pdt value per dump) and list of axes using the dataStruct declarations:
	unsigned int flagsSize=0;           EnumSet<AxisName> es_flagsAxes;
	unsigned int actualTimesSize=0;     EnumSet<AxisName> es_actualTimesAxes;
	unsigned int actualDurationsSize=0; EnumSet<AxisName> es_actualDurationsAxes;
        unsigned int zeroLagsSize=0;        EnumSet<AxisName> es_zeroLagsAxes;
        unsigned int autoDataSize=0;        EnumSet<AxisName> es_autoDataAxes;
	unsigned int crossDataSize=0;       EnumSet<AxisName> es_crossDataAxes;

	if((itf=m_dc_sizeAndAxes.find(FLAGS))!=ite){
	  if(coutest)cout<<"Flags have been declared in the main header"<<endl;
	  flagsSize    = itf->second.first;
	  es_flagsAxes = itf->second.second;
	  numFlags     = dataSubset.flags( flagsPtr );
	  if(numFlags!=flagsSize)
	    Error(FATAL, (char *) "Size of flags, %d, not compatible with the declared size of %d",
		  numFlags,flagsSize);
	  flagsSize   /= numTime;
	}

	if((itf=m_dc_sizeAndAxes.find(ACTUAL_TIMES))!=ite){
	  if(coutest)cout<<"ActualTimes have been declared in the main header"<<endl;
	  actualTimesSize    = itf->second.first;
	  es_actualTimesAxes = itf->second.second;
	  numActualTimes     = dataSubset.actualTimes( actualTimesPtr );
	  if(numActualTimes!=actualTimesSize)
	    Error(FATAL, (char *) "Size of actualTimes, %d, not compatible with the declared size of %d",
		  numActualTimes,actualTimesSize);
	  actualTimesSize   /= numTime;
	}

	if((itf=m_dc_sizeAndAxes.find(ACTUAL_DURATIONS))!=ite){
	  if(coutest)cout<<"ActualDurations have been declared in the main header"<<endl;
	  actualDurationsSize    = itf->second.first;
	  es_actualDurationsAxes = itf->second.second;
	  numActualDurations     = dataSubset.actualDurations( actualDurationsPtr );
	  if(numActualDurations!=actualDurationsSize)
	    Error(FATAL, (char *) "Size of actualDurations, %d, not compatible with the declared size of %d",
		  numActualDurations,actualDurationsSize);
	  actualDurationsSize   /= numTime;
	}

	if((itf=m_dc_sizeAndAxes.find(CROSS_DATA))!=ite){
	  if(coutest)cout<<"CrossData have been declared in the main header"<<endl;
	  crossDataSize    = itf->second.first;
	  es_crossDataAxes = itf->second.second;
	  switch(dataSubset.crossDataType()){
	  case INT16_TYPE:
	    numCrossData = dataSubset.crossData( crossShortDataPtr );
	    break;
	  case INT32_TYPE:
	    numCrossData = dataSubset.crossData( crossIntDataPtr );
	    break;
	  case FLOAT32_TYPE:
	    numCrossData = dataSubset.crossData( crossFloatDataPtr );
	    break;
	  default:
	    Enum<PrimitiveDataType> e_pdt=dataSubset.crossDataType();
	    Error(FATAL, (char *) "Cross data with the primitive data type %s are not supported",
		  e_pdt.str().c_str());
	  }
	  if(numCrossData!=crossDataSize)
	    Error(FATAL, (char *) "Size of crossData, %d, not compatible with the declared size of %d",
		  numCrossData,crossDataSize);
	  crossDataSize /= numTime;

	}

	if((itf=m_dc_sizeAndAxes.find(ZERO_LAGS))!=ite && crossDataSize ){
	  if(coutest)cout<<"ZeroLags have been declared in the main header"<<endl;
	  zeroLagsSize = itf->second.first;
	  es_zeroLagsAxes = itf->second.second;
	  numZeroLags = dataSubset.zeroLags( zeroLagsPtr );
	  if(numZeroLags != zeroLagsSize)
	    Error(FATAL,(char *) "Size of zeroLags, %d, not compatible with the declared size of %d",
		    numZeroLags,zeroLagsSize);
	  zeroLagsSize /= numTime;
	}

	if((itf=m_dc_sizeAndAxes.find(AUTO_DATA))!=ite){
	  if(coutest)cout<<"AutoData have been declared in the main header"<<endl;
	  autoDataSize    = itf->second.first;
	  es_autoDataAxes = itf->second.second;
	  numAutoData     = dataSubset.autoData( autoDataPtr );
	  if(numAutoData){
	    if(numAutoData != autoDataSize)
	      Error(FATAL,(char *) "Size of autoData, %d, not compatible with the declared size of %d",
		    numAutoData,itf->second.first);
	  }else if(numAutoData==0){
	    if(!e_cm[CROSS_ONLY])
	      Error(WARNING,string("No autoData! may happen when a subscan is aborted"));
	    return 0;
	  }
	  autoDataSize /= numTime;
	}

	for(unsigned int nt=0; nt<numTime; nt++){

	  timeOfDump   += interval;
	  timeCentroid =  timeOfDump;   // TODO
	  exposure     =  interval;     // TODO

	  DataDump* dataDumpPtr = new DataDump( vv_numCrossPolProduct,
						vv_numAutoPolProduct,
						vv_numSpectralPoint,
						vv_numBin,
						vv_e_sideband,
						numApc,
						v_numSpectralWindow,
						numBaseband,
						numAnt,
						correlationMode,
						timeOfDump,
						timeCentroid,
						interval,
						exposure
					       );
	  if(e_cm[CROSS_ONLY]    )dataDumpPtr->setScaleFactor(vv_scaleFactor);
	  if(e_cm[CROSS_AND_AUTO])dataDumpPtr->setScaleFactor(vv_scaleFactor);


	  // TODO update timeCentroid and exposure would the actualTimes and actualDurations binary blocks be provided.

	  if(flagsPtr)           dataDumpPtr->attachFlags(           flagsSize, es_flagsAxes,
							             numFlags,  flagsPtr + nt*flagsSize);

	  if(actualTimesPtr)     dataDumpPtr->attachActualTimes(     actualTimesSize, es_actualTimesAxes,
								     numActualTimes,  actualTimesPtr + nt*actualTimesSize);

	  if(actualDurationsPtr) dataDumpPtr->attachActualDurations( actualDurationsSize, es_actualDurationsAxes,
								     numActualDurations, actualDurationsPtr + nt*actualDurationsSize);

	  if(zeroLagsPtr)        dataDumpPtr->attachZeroLags(        zeroLagsSize, es_zeroLagsAxes,
								     numZeroLags, zeroLagsPtr + nt*zeroLagsSize);

	  if(crossShortDataPtr)  dataDumpPtr->attachCrossData(       crossDataSize, es_crossDataAxes,
								     numCrossData, crossShortDataPtr + nt*crossDataSize);

	  if(crossIntDataPtr)    dataDumpPtr->attachCrossData(       crossDataSize, es_crossDataAxes,
								     numCrossData, crossIntDataPtr + nt*crossDataSize);

	  if(crossFloatDataPtr)  dataDumpPtr->attachCrossData(       crossDataSize, es_crossDataAxes,
								     numCrossData, crossFloatDataPtr + nt*crossDataSize);

	  if(autoDataPtr)        dataDumpPtr->attachAutoData(        autoDataSize, es_autoDataAxes,
								     numAutoData, autoDataPtr + nt*autoDataSize);


	  dataDumpPtr->setContextUsingProjectPath(dataSubset.projectPath());

	  if(coutest)cout<<"store dataDump"<<endl;
	  v_dataDump_.push_back(dataDumpPtr);
	  if(coutest)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;
	}
	if(coutest)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;

      }else{

	const vector<SDMDataSubset>& v_dataSubset    = dataObject.corrDataSubsets();
// 	uint_64           startTime       = dataObject.startTime();
	int64_t                    interval;
	int64_t                    exposure;
	int64_t                    timeOfInteg;
	int64_t                    timeCentroid;
//	unsigned int                 dumpNum;

// 	interval_    = interval;
// 	exposure     = interval_;
// 	timeOfInteg  = midIntervalTime;
// 	timeCentroid = time_;
// 	dumpNum      = integNumCounter_++;

	if(coutest)cout<<"ici 0 v_dataSubset.size()="<<v_dataSubset.size()<<endl;
	for(unsigned int nt=0; nt<v_dataSubset.size(); nt++){

	  if(coutest){
	    cout<<"filename="<<filename<<endl;
	    cout << v_dataSubset[nt].toString();
	    cout<<"ici 1"<<endl;
	  }

//	  dumpNum     = nt+1;

	  timeOfInteg  = v_dataSubset[nt].time();        if(coutest)cout<<"attachDataObject: "<<timeOfInteg<<endl;
	  timeCentroid = v_dataSubset[nt].time();        if(coutest)cout<<"attachDataObject: "<<timeCentroid<<endl;
	  interval     = v_dataSubset[nt].interval();    if(coutest)cout<<"attachDataObject: "<<interval<<endl;
	  exposure     = interval;
	  if(coutest){
	    for(unsigned int i=0; i<v_numSpectralWindow.size(); i++){
	      for(unsigned int j=0; j<v_numSpectralWindow[i]; j++)
		cout<<"vv_numSpectralPoint[i][j]="<<vv_numSpectralPoint[i][j]<<endl;
	    }
	    for(unsigned int i=0; i<vv_numAutoPolProduct.size(); i++){
	      for(unsigned int j=0; j<vv_numAutoPolProduct[i].size(); j++)
		cout<<"vv_numAutoPolProduct[i][j]="<<vv_numAutoPolProduct[i][j]<<endl;
	    }
	    for(unsigned int i=0; i<vv_numCrossPolProduct.size(); i++){
	      for(unsigned int j=0; j<vv_numCrossPolProduct[i].size(); j++)
		cout<<"vv_numCrossPolProduct[i][j]="<<vv_numCrossPolProduct[i][j]<<endl;
	    }
	  }
	  DataDump* dataDumpPtr = new DataDump( vv_numCrossPolProduct,
						vv_numAutoPolProduct,
						vv_numSpectralPoint,
						vv_numBin,
						vv_e_sideband,
						numApc,
						v_numSpectralWindow,
						numBaseband,
						numAnt,
						correlationMode,
						timeOfInteg,
						timeCentroid,
						interval,
						exposure
					       );
	  if(e_cm[CROSS_ONLY]    )dataDumpPtr->setScaleFactor(vv_scaleFactor);
	  if(e_cm[CROSS_AND_AUTO])dataDumpPtr->setScaleFactor(vv_scaleFactor);

	  if(coutest)cout<<"m_dc_sizeAndAxes.size()="<< m_dc_sizeAndAxes.size() << endl;
	  if((itf=m_dc_sizeAndAxes.find(FLAGS))!=ite){
	    if(coutest)cout<<"Flags have been declared in the main header"<<endl;
	    const unsigned int* flagsPtr;
	    unsigned int long numFlags = v_dataSubset[nt].flags( flagsPtr );
	    if(coutest)cout<<numFlags<<" "<<itf->second.first<<endl;
	    if(numFlags)dataDumpPtr->attachFlags( itf->second.first, itf->second.second,
						  numFlags, flagsPtr);
	  }

	  if((itf=m_dc_sizeAndAxes.find(ACTUAL_TIMES))!=ite){
	    if(coutest)cout<<"ActualTimes have been declared in the main header"<<endl;
	    const int64_t* actualTimesPtr;
	    unsigned long int numActualTimes = v_dataSubset[nt].actualTimes( actualTimesPtr );
	    if(coutest)cout<<numActualTimes<<" "<<itf->second.first<<endl;
	    if(numActualTimes)dataDumpPtr->attachActualTimes( itf->second.first, itf->second.second,
							      numActualTimes, actualTimesPtr);
	  }

	  if((itf=m_dc_sizeAndAxes.find(ACTUAL_DURATIONS))!=ite){
	    if(coutest)cout<<"ActualDurations have been declared in the main header"<<endl;
	    const int64_t* actualDurationsPtr;
	    unsigned long int numActualDurations = v_dataSubset[nt].actualDurations( actualDurationsPtr );
	    if(coutest)cout<<numActualDurations<<" "<<itf->second.first<<endl;
	    if(numActualDurations)dataDumpPtr->attachActualDurations( itf->second.first, itf->second.second,
								      numActualDurations, actualDurationsPtr);
	  }

	  if((itf=m_dc_sizeAndAxes.find(ZERO_LAGS))!=ite){
	    if(coutest)cout<<"ZeroLags have been declared in the main header"<<endl;
	    const float* zeroLagsPtr;
	    unsigned long int numZeroLags = v_dataSubset[nt].zeroLags( zeroLagsPtr );
	    if(coutest)cout<<numZeroLags<<" "<<itf->second.first<<endl;
	    if(numZeroLags)dataDumpPtr->attachZeroLags( itf->second.first, itf->second.second,
							numZeroLags, zeroLagsPtr);
	  }

	  if((itf=m_dc_sizeAndAxes.find(CROSS_DATA))!=ite){
	    if(coutest)cout<<"CrossData have been declared in the main header ";
	    unsigned long int numCrossData;
	    switch(v_dataSubset[nt].crossDataType()){
	    case INT16_TYPE:
	      { const short int* crossDataPtr;
		numCrossData = v_dataSubset[nt].crossData( crossDataPtr );
		if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							      numCrossData, crossDataPtr);
	      }
	      if(coutest)cout<<"SHORT_TYPE"<<endl;
	      if(coutest)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataShort()<<endl;
	      break;
	    case INT32_TYPE:
	      { const int* crossDataPtr;
		numCrossData = v_dataSubset[nt].crossData( crossDataPtr );
		if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							      numCrossData, crossDataPtr);
	      }
	      if(coutest)cout<<"INT_TYPE"<<endl;
	      if(coutest)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataLong()<<endl;
	      break;
	    case FLOAT32_TYPE:
	      { const float* crossDataPtr;
		numCrossData = v_dataSubset[nt].crossData( crossDataPtr );
		if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							      numCrossData, crossDataPtr);
	      }
	      if(coutest)cout<<"FLOAT_TYPE"<<endl;
	      if(coutest)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataFloat()<<endl;
	      break;
	    default:
	      Enum<PrimitiveDataType> e_pdt=v_dataSubset[nt].crossDataType();
	      Error(FATAL, (char *) "Cross data with the primitive data type %s are not supported",
		    e_pdt.str().c_str());
	    }
	  }

	  if((itf=m_dc_sizeAndAxes.find(AUTO_DATA))!=ite){
	    if(coutest)cout<<"AutoData have been declared in the main header"<<endl;
	    const float*      floatDataPtr=NULL;
	    unsigned long int numFloatData = v_dataSubset[nt].autoData( floatDataPtr );
	    if(coutest)cout<<numFloatData<<" "<<itf->second.first<<"  "<<floatDataPtr<<endl;
	    if(numFloatData){
	      if(numFloatData!=itf->second.first)
		Error(FATAL,(char *) "Size of autoData, %d, not compatible with the declared size of %d",
		      numFloatData,itf->second.first);
	      dataDumpPtr->attachAutoData( itf->second.first, itf->second.second,
					   numFloatData, floatDataPtr);
	      if(coutest)cout<<"autoData attached,  const pointer:"<<dataDumpPtr->autoData()<<endl;
// 	      const long unsigned int* aptr=dataDumpPtr->flags();
	    }else if(numFloatData==0){
	       if(!e_cm[CROSS_ONLY])
		 Error(WARNING,string("No autoData! may happen when a subscan is aborted"));
	      break;
	    }
	  }

	  if (coutest) cout << "About to setContextUsingProjectPath" << endl;
	  dataDumpPtr->setContextUsingProjectPath(v_dataSubset[nt].projectPath());
	  if (coutest) cout << "Back from setContextUsingProjectPath" << endl;
	  const unsigned int* bptr=dataDumpPtr->flags(); if(bptr==NULL)if(coutest)cout<<"No flags"<<endl;
	  if(coutest)cout<<"store dataDump"<<endl;
	  v_dataDump_.push_back(dataDumpPtr);
	  if(coutest)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;
	}
	if(coutest)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;
      }
      if(coutest)cout<<"numTime="<<numTime<<", about to return 0"<<endl;

//       for (unsigned long int nt = 0; nt <v_dataSubset.size(); nt++) {
// 	if(coutest)cout << v_dataSubset[nt].toString();
//       }
      return 0;
    }
    catch (SDMDataObjectReaderException e) { cout << e.getMessage()          << endl; }
    catch (SDMDataObjectParserException e) { cout << e.getMessage()          << endl; }
    catch (SDMDataObjectException e)       { cout << e.getMessage()          << endl; }
    catch (std::exception e)                    { cout << e.what()                << endl; }
    catch (...)                            { cout << "Unexpected exception." << endl; }
    return 1;
  }

  int SDMBinData::openStreamDataObject(const string& dataOID) {
    if (verbose_) cout << "SDMBinData::openStreamDataObject : entering" << endl;
    
    sdmdosr.close();

    if(verbose_)cout<<"open a BDF and consumes its global header. "<<dataOID<<endl;

    // transforme le dataOID en file name (re-utilise code ds la methode beginWithMimeHeader de SDMBLOBs)  TODO
    string filename=dataOID;
    string::size_type np;
    np=filename.find("/",0);
    while(np!=string::npos){ np=filename.find("/",np); if(np!=string::npos){ filename.replace(np,1,"_"); np++; } }
    np=filename.find(":",0);
    while(np!=string::npos){ np=filename.find(":",np); if(np!=string::npos){ filename.replace(np,1,"_"); np++; } }

    static DIR* dirp;
//     ostringstream dir; dir<<execBlockDir_<<"/SDMBinaries";
    ostringstream dir; dir<<execBlockDir_<<"/ASDMBinary";
    dirp = opendir(dir.str().c_str());
    if(!dirp)
      Error(FATAL,(char *) "Could not open directory %s",dir.str().c_str());
    closedir(dirp);
    filename=dir.str()+"/"+filename;
    if(verbose_)cout<<"filename="<<filename<<endl;

    
    sdmdosr.open(filename);
    dataOID_ = dataOID;
    if(verbose_){
      cout<<"============================"<<endl;
      cout<< sdmdosr.toString()<<endl;
      cout<<"============================"<<endl;
    }
    return 0;

    /*    
      Enum<CorrelationMode>            e_cm;
      Enum<ProcessorType>              e_pt;
      Enum<CorrelatorType>             e_ct;

      unsigned int                     numTime=0;

      CorrelationMode                  correlationMode     = sdmdosr.correlationMode(); e_cm=correlationMode;
      ProcessorType                    processorType       = sdmdosr.processorType();   e_pt=processorType;
      CorrelatorType                   correlatorType;

      const SDMDataObject::DataStruct& dataStruct          = sdmdosr.dataStruct();

      vector<AtmPhaseCorrection>       v_apc               = dataStruct.apc();
      vector<SDMDataObject::Baseband>  v_baseband          = dataStruct.basebands();
      SDMDataObject::ZeroLagsBinaryPart               zeroLagsParameters;

      bool                             normalized=false;
      if(e_pt[ProcessorTypeMod::CORRELATOR]){
	if(e_cm[CorrelationModeMod::CROSS_ONLY])
	  normalized =  dataStruct.autoData().normalized();
	if(dataStruct.zeroLags().size()){
	  zeroLagsParameters = dataStruct.zeroLags();
	  correlatorType = zeroLagsParameters.correlatorType(); e_ct=correlatorType;
	}
      }
	
      vector< vector< unsigned int > > vv_numAutoPolProduct;
      vector< vector< unsigned int > > vv_numCrossPolProduct;
      vector< vector< unsigned int > > vv_numSpectralPoint;
      vector< vector< unsigned int > > vv_numBin;
      vector< vector< float > >        vv_scaleFactor;
      vector< unsigned int >           v_numSpectralWindow;
      vector< vector< Enum<NetSideband> > > vv_e_sideband;
      vector< vector< SDMDataObject::SpectralWindow* > > vv_image;

      if(verbose_)cout<<"e_cm="<<e_cm.str()<<endl;

      for(unsigned int nbb=0; nbb<v_baseband.size(); nbb++){
	vector<SDMDataObject::SpectralWindow>  v_spw = v_baseband[nbb].spectralWindows(); v_numSpectralWindow.push_back(v_spw.size());
	vector<SDMDataObject::SpectralWindow*> v_image;
	vector< unsigned int > v_numBin;
	vector< unsigned int > v_numSpectralPoint;
	vector< unsigned int > v_numSdPolProduct;
	vector< unsigned int > v_numCrPolProduct;
	vector< Enum<NetSideband> >  v_e_sideband;
	vector<float>          v_scaleFactor;
	for(unsigned int nspw=0; nspw<v_spw.size(); nspw++){
	  v_e_sideband.push_back(Enum<NetSideband>(v_spw[nspw].sideband()));
	  v_image.push_back(NULL);                                   // TODO waiting for implementation
	  v_numBin.push_back(v_spw[nspw].numBin());
	  if(verbose_)cout<<"v_spw["<<nspw<<"].numSpectralPoint()="<<v_spw[nspw].numSpectralPoint()<<endl;
	  v_numSpectralPoint.push_back(v_spw[nspw].numSpectralPoint());
	  if(e_cm[AUTO_ONLY])
	    v_numSdPolProduct.push_back(v_spw[nspw].sdPolProducts().size());
	  else{
	    v_numCrPolProduct.push_back(v_spw[nspw].crossPolProducts().size());
	    v_scaleFactor.push_back(v_spw[nspw].scaleFactor());
	    if(e_cm[CROSS_AND_AUTO])
	      v_numSdPolProduct.push_back(v_spw[nspw].sdPolProducts().size());
	  }
	}
	if(e_cm[CROSS_ONLY]==false)vv_numAutoPolProduct.push_back(v_numSdPolProduct);
	if(e_cm[AUTO_ONLY]==false) vv_numCrossPolProduct.push_back(v_numCrPolProduct);
	vv_numSpectralPoint.push_back(v_numSpectralPoint);
	vv_e_sideband.push_back(v_e_sideband);
	vv_image.push_back(v_image);
	vv_numBin.push_back(v_numBin);
	vv_scaleFactor.push_back(v_scaleFactor);
      }
      if(e_cm[AUTO_ONLY])vv_numCrossPolProduct.clear();
      if(e_cm[CROSS_ONLY])vv_numAutoPolProduct.clear();

      complexData_=false;
      if(vv_numAutoPolProduct.size()){
	for(unsigned int nbb=0; nbb<vv_numAutoPolProduct.size(); nbb++){
	  for(unsigned int nspw=0; nspw<vv_numAutoPolProduct[nbb].size(); nspw++)
	    if(vv_numAutoPolProduct[nbb][nspw]>2)complexData_=true;
	}
      }
      if(vv_numCrossPolProduct.size())complexData_=true;

      EnumSet<AxisName>         es_axisNames;
      DataContent               dataContent;
      SDMDataObject::BinaryPart binaryPart;
      map<DataContent,pair<unsigned int,EnumSet<AxisName> > > m_dc_sizeAndAxes;

      dataContent = FLAGS;
      binaryPart  = dataStruct.flags();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = ACTUAL_DURATIONS;
      binaryPart  = dataStruct.actualDurations();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = ACTUAL_TIMES;
      binaryPart  = dataStruct.actualTimes();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = AUTO_DATA;
      binaryPart  = dataStruct.autoData();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      if(es_axisNames[TIM])numTime=sdmdosr.numTime();

      dataContent = CROSS_DATA;
      binaryPart  = dataStruct.crossData();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      if(binaryPart.size()){
	dataContent = ZERO_LAGS;
	binaryPart  = dataStruct.zeroLags();
	es_axisNames.set(binaryPart.axes(),true);
	if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
      }

      if(es_axisNames[TIM])numTime=sdmdosr.numTime();

      map<DataContent,pair<unsigned int,EnumSet<AxisName> > >::iterator
	itf=m_dc_sizeAndAxes.find(AUTO_DATA),
	ite=m_dc_sizeAndAxes.end();
      if(itf==ite)itf=m_dc_sizeAndAxes.find(CROSS_DATA);
      if(itf==ite)Error(FATAL,"BLOB %s has no declaration for observational data",dataOID.c_str());
    }
    catch (SDMDataObjectStreamReaderException e) { cout << e.getMessage()          << endl; }
    catch (SDMDataObjectReaderException e) { cout << e.getMessage()          << endl; }
    catch (SDMDataObjectParserException e) { cout << e.getMessage()          << endl; }
    catch (SDMDataObjectException e)       { cout << e.getMessage()          << endl; }
    catch (std::exception e)                    { cout << e.what()                << endl; }
    catch (...)                            { cout << "Unexpected exception." << endl; }
    if (verbose_) cout << "SDMBinData::openStreamDataObject : exiting" << endl;
    return 0;*/
  }

  int SDMBinData::attachStreamDataObject(const string& dataOID ){
    
    if(verbose_)cout<<"SDMBinData::attachStreamDataObject(const string& dataOID ): entering"<<endl;
    
    sdmdosr.close();

    if(verbose_)cout<<"attach BLOB "<<dataOID<<endl;

    // transforme le dataOID en file name (re-utilise code ds la methode beginWithMimeHeader de SDMBLOBs)  TODO
    string filename=dataOID;
    string::size_type np;
    np=filename.find("/",0);
    while(np!=string::npos){ np=filename.find("/",np); if(np!=string::npos){ filename.replace(np,1,"_"); np++; } }
    np=filename.find(":",0);
    while(np!=string::npos){ np=filename.find(":",np); if(np!=string::npos){ filename.replace(np,1,"_"); np++; } }
    if(verbose_)cout<<"filename="<<filename<<" execBlockDir_="<<execBlockDir_<<endl;
    static DIR* dirp;
//     ostringstream dir; dir<<execBlockDir_<<"/SDMBinaries";
    ostringstream dir; dir<<execBlockDir_<<"/ASDMBinary";
    dirp = opendir(dir.str().c_str());
    if(!dirp)
      Error(FATAL,(char *) "Could not open directory %s",dir.str().c_str());
    closedir(dirp);
    filename=dir.str()+"/"+filename;
    if(verbose_)cout<<"filename="<<filename<<endl;

    try {
      sdmdosr.open(filename);
      dataOID_ = dataOID;
      if(verbose_){
	cout<<"============================"<<endl;
	cout<< sdmdosr.toString()<<endl;
	cout<<"============================"<<endl;
      }

      Enum<CorrelationMode>            e_cm;
      Enum<ProcessorType>              e_pt;
      Enum<CorrelatorType>             e_ct;

      unsigned int                     numTime=0;

      unsigned int                     numAnt              = sdmdosr.numAntenna();
      CorrelationMode                  correlationMode     = sdmdosr.correlationMode(); e_cm=correlationMode;
      ProcessorType                    processorType       = sdmdosr.processorType();   e_pt=processorType;
      CorrelatorType                   correlatorType;

      const SDMDataObject::DataStruct& dataStruct          = sdmdosr.dataStruct();

      vector<AtmPhaseCorrection>       v_apc               = dataStruct.apc();
      vector<SDMDataObject::Baseband>  v_baseband          = dataStruct.basebands();
      SDMDataObject::ZeroLagsBinaryPart               zeroLagsParameters;

//      bool                             normalized=false;
      if(e_pt[ProcessorTypeMod::CORRELATOR]){
//	if(e_cm[CorrelationModeMod::CROSS_ONLY])
//	  normalized =  dataStruct.autoData().normalized();
	if(dataStruct.zeroLags().size()){
	  zeroLagsParameters = dataStruct.zeroLags();
	  correlatorType = zeroLagsParameters.correlatorType(); e_ct=correlatorType;
	}
      }
	
      

      unsigned int                     numApc              = v_apc.size();
      unsigned int                     numBaseband         = v_baseband.size();

      vector< vector< unsigned int > > vv_numAutoPolProduct;
      vector< vector< unsigned int > > vv_numCrossPolProduct;
      vector< vector< unsigned int > > vv_numSpectralPoint;
      vector< vector< unsigned int > > vv_numBin;
      vector< vector< float > >        vv_scaleFactor;
      vector< unsigned int >           v_numSpectralWindow;
      vector< vector< Enum<NetSideband> > > vv_e_sideband;
      vector< vector< SDMDataObject::SpectralWindow* > > vv_image;

      if(verbose_)cout<<"e_cm="<<e_cm.str()<<endl;

      for(unsigned int nbb=0; nbb<v_baseband.size(); nbb++){
	vector<SDMDataObject::SpectralWindow>  v_spw = v_baseband[nbb].spectralWindows(); v_numSpectralWindow.push_back(v_spw.size());
	vector<SDMDataObject::SpectralWindow*> v_image;
	vector< unsigned int > v_numBin;
	vector< unsigned int > v_numSpectralPoint;
	vector< unsigned int > v_numSdPolProduct;
	vector< unsigned int > v_numCrPolProduct;
	vector< Enum<NetSideband> >  v_e_sideband;
	vector<float>          v_scaleFactor;
	for(unsigned int nspw=0; nspw<v_spw.size(); nspw++){
	  v_e_sideband.push_back(Enum<NetSideband>(v_spw[nspw].sideband()));
	  v_image.push_back(NULL);                                   // TODO waiting for implementation
	  v_numBin.push_back(v_spw[nspw].numBin());
	  if(verbose_)cout<<"v_spw["<<nspw<<"].numSpectralPoint()="<<v_spw[nspw].numSpectralPoint()<<endl;
	  v_numSpectralPoint.push_back(v_spw[nspw].numSpectralPoint());
	  if(e_cm[AUTO_ONLY])
	    v_numSdPolProduct.push_back(v_spw[nspw].sdPolProducts().size());
	  else{
	    v_numCrPolProduct.push_back(v_spw[nspw].crossPolProducts().size());
	    v_scaleFactor.push_back(v_spw[nspw].scaleFactor());
	    if(e_cm[CROSS_AND_AUTO])
	      v_numSdPolProduct.push_back(v_spw[nspw].sdPolProducts().size());
	  }
	}
	if(e_cm[CROSS_ONLY]==false)vv_numAutoPolProduct.push_back(v_numSdPolProduct);
	if(e_cm[AUTO_ONLY]==false) vv_numCrossPolProduct.push_back(v_numCrPolProduct);
	vv_numSpectralPoint.push_back(v_numSpectralPoint);
	vv_e_sideband.push_back(v_e_sideband);
	vv_image.push_back(v_image);
	vv_numBin.push_back(v_numBin);
	vv_scaleFactor.push_back(v_scaleFactor);
      }
      if(e_cm[AUTO_ONLY])vv_numCrossPolProduct.clear();
      if(e_cm[CROSS_ONLY])vv_numAutoPolProduct.clear();

      complexData_=false;
      if(vv_numAutoPolProduct.size()){
	for(unsigned int nbb=0; nbb<vv_numAutoPolProduct.size(); nbb++){
	  for(unsigned int nspw=0; nspw<vv_numAutoPolProduct[nbb].size(); nspw++)
	    if(vv_numAutoPolProduct[nbb][nspw]>2)complexData_=true;
	}
      }
      if(vv_numCrossPolProduct.size())complexData_=true;

      EnumSet<AxisName>         es_axisNames;
      DataContent               dataContent;
      SDMDataObject::BinaryPart binaryPart;
      map<DataContent,pair<unsigned int,EnumSet<AxisName> > > m_dc_sizeAndAxes;

      dataContent = FLAGS;
      binaryPart  = dataStruct.flags();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = ACTUAL_DURATIONS;
      binaryPart  = dataStruct.actualDurations();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = ACTUAL_TIMES;
      binaryPart  = dataStruct.actualTimes();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      dataContent = AUTO_DATA;
      binaryPart  = dataStruct.autoData();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      if(es_axisNames[TIM])numTime=sdmdosr.numTime();

      dataContent = CROSS_DATA;
      binaryPart  = dataStruct.crossData();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));

      if(binaryPart.size()){
	dataContent = ZERO_LAGS;
	binaryPart  = dataStruct.zeroLags();
	es_axisNames.set(binaryPart.axes(),true);
	if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
      }

      if(es_axisNames[TIM])numTime=sdmdosr.numTime();

      map<DataContent,pair<unsigned int,EnumSet<AxisName> > >::iterator
	itf=m_dc_sizeAndAxes.find(AUTO_DATA),
	ite=m_dc_sizeAndAxes.end();
      if(itf==ite)itf=m_dc_sizeAndAxes.find(CROSS_DATA);
      if(itf==ite)Error(FATAL,(char *) "BLOB %s has no declaration for observational data",dataOID.c_str());


      if(coutDeleteInfo_)cout<<"delete  v_dataDump_"<<endl;
      for(unsigned int n=0; n<v_dataDump_.size(); n++)delete v_dataDump_[n];
      v_dataDump_.clear();

      if(verbose_)cout<<"numTime="<<numTime<<endl;

      if(numTime){  // MIME content with a structure of dimensionality 0

	const SDMDataSubset&     dataSubset         = sdmdosr.getSubset();
	uint64_t                 obsDuration        = dataSubset.interval();

	int64_t                  interval           = (int64_t)obsDuration/(int64_t)numTime;
	int64_t                  exposure;
	int64_t                  timeOfDump         = (int64_t)dataSubset.time()-(int64_t)obsDuration/2LL-interval/2LL; // mutable
	int64_t                  timeCentroid       = timeOfDump;         // default value for the first dump

	// actual pointer to the data blocks and nb of pdt values found in these blocks:
	const unsigned int*      flagsPtr           = NULL;  unsigned int long numFlags=0; // mcaillat 
	const int64_t*         actualTimesPtr     = NULL;  unsigned int long numActualTimes=0;
	const int64_t*         actualDurationsPtr = NULL;  unsigned int long numActualDurations=0;
	const float*             zeroLagsPtr        = NULL;  unsigned int long numZeroLags=0;
	const float*             autoDataPtr        = NULL;  unsigned int long numAutoData=0;
	const short int*         crossShortDataPtr  = NULL;  unsigned int long numCrossData=0;
	const int*               crossIntDataPtr    = NULL;
	const float*             crossFloatDataPtr  = NULL;


	// size (in nb pdt value per dump) and list of axes using the dataStruct declarations:
	unsigned int flagsSize=0;           EnumSet<AxisName> es_flagsAxes;
	unsigned int actualTimesSize=0;     EnumSet<AxisName> es_actualTimesAxes;
	unsigned int actualDurationsSize=0; EnumSet<AxisName> es_actualDurationsAxes;
        unsigned int zeroLagsSize=0;        EnumSet<AxisName> es_zeroLagsAxes;
        unsigned int autoDataSize=0;        EnumSet<AxisName> es_autoDataAxes;
	unsigned int crossDataSize=0;       EnumSet<AxisName> es_crossDataAxes;

	if((itf=m_dc_sizeAndAxes.find(FLAGS))!=ite){
	  if(verbose_)cout<<"Flags have been declared in the main header"<<endl;
	  flagsSize    = itf->second.first;
	  es_flagsAxes = itf->second.second;
	  numFlags     = dataSubset.flags( flagsPtr );
	  if(numFlags!=flagsSize)
	    Error(FATAL, (char *) "Size of flags, %d, not compatible with the declared size of %d",
		  numFlags,flagsSize);
	  flagsSize   /= numTime;
	}

	if((itf=m_dc_sizeAndAxes.find(ACTUAL_TIMES))!=ite){
	  if(verbose_)cout<<"ActualTimes have been declared in the main header"<<endl;
	  actualTimesSize    = itf->second.first;
	  es_actualTimesAxes = itf->second.second;
	  numActualTimes     = dataSubset.actualTimes( actualTimesPtr );
	  if(numActualTimes!=actualTimesSize)
	    Error(FATAL, (char *) "Size of actualTimes, %d, not compatible with the declared size of %d",
		  numActualTimes,actualTimesSize);
	  actualTimesSize   /= numTime;
	}

	if((itf=m_dc_sizeAndAxes.find(ACTUAL_DURATIONS))!=ite){
	  if(verbose_)cout<<"ActualDurations have been declared in the main header"<<endl;
	  actualDurationsSize    = itf->second.first;
	  es_actualDurationsAxes = itf->second.second;
	  numActualDurations     = dataSubset.actualDurations( actualDurationsPtr );
	  if(numActualDurations!=actualDurationsSize)
	    Error(FATAL, (char *) "Size of actualDurations, %d, not compatible with the declared size of %d",
		  numActualDurations,actualDurationsSize);
	  actualDurationsSize   /= numTime;
	}

	if((itf=m_dc_sizeAndAxes.find(CROSS_DATA))!=ite){
	  if(verbose_)cout<<"CrossData have been declared in the main header"<<endl;
	  crossDataSize    = itf->second.first;
	  es_crossDataAxes = itf->second.second;
	  switch(dataSubset.crossDataType()){
	  case INT16_TYPE:
	    numCrossData = dataSubset.crossData( crossShortDataPtr );
	    break;
	  case INT32_TYPE:
	    numCrossData = dataSubset.crossData( crossIntDataPtr );
	    break;
	  case FLOAT32_TYPE:
	    numCrossData = dataSubset.crossData( crossFloatDataPtr );
	    break;
	  default:
	    Enum<PrimitiveDataType> e_pdt=dataSubset.crossDataType();
	    Error(FATAL, (char *) "Cross data with the primitive data type %s are not supported",
		  e_pdt.str().c_str());
	  }
	  if(numCrossData!=crossDataSize)
	    Error(FATAL, (char *) "Size of crossData, %d, not compatible with the declared size of %d",
		  numCrossData,crossDataSize);
	  crossDataSize /= numTime;

	}

	if((itf=m_dc_sizeAndAxes.find(ZERO_LAGS))!=ite && crossDataSize ){
	  if(verbose_)cout<<"ZeroLags have been declared in the main header"<<endl;
	  zeroLagsSize = itf->second.first;
	  es_zeroLagsAxes = itf->second.second;
	  numZeroLags = dataSubset.zeroLags( zeroLagsPtr );
	  if(numZeroLags != zeroLagsSize)
	    Error(FATAL,(char *) "Size of zeroLags, %d, not compatible with the declared size of %d",
		    numZeroLags,zeroLagsSize);
	  zeroLagsSize /= numTime;
	}

	if((itf=m_dc_sizeAndAxes.find(AUTO_DATA))!=ite){
	  if(verbose_)cout<<"AutoData have been declared in the main header"<<endl;
	  autoDataSize    = itf->second.first;
	  es_autoDataAxes = itf->second.second;
	  numAutoData     = dataSubset.autoData( autoDataPtr );
	  if(numAutoData){
	    if(numAutoData != autoDataSize)
	      Error(FATAL,(char *) "Size of autoData, %d, not compatible with the declared size of %d",
		    numAutoData,itf->second.first);
	  }else if(numAutoData==0){
	    if(!e_cm[CROSS_ONLY])
	      Error(WARNING,string("No autoData! may happen when a subscan is aborted"));
	    return 0;
	  }
	  autoDataSize /= numTime;
	}

	for(unsigned int nt=0; nt<numTime; nt++){

	  timeOfDump   += interval;
	  timeCentroid =  timeOfDump;   // TODO
	  exposure     =  interval;     // TODO

	  DataDump* dataDumpPtr = new DataDump( vv_numCrossPolProduct,
						vv_numAutoPolProduct,
						vv_numSpectralPoint,
						vv_numBin,
						vv_e_sideband,
						numApc,
						v_numSpectralWindow,
						numBaseband,
						numAnt,
						correlationMode,
						timeOfDump,
						timeCentroid,
						interval,
						exposure
					       );
	  if(e_cm[CROSS_ONLY]    )dataDumpPtr->setScaleFactor(vv_scaleFactor);
	  if(e_cm[CROSS_AND_AUTO])dataDumpPtr->setScaleFactor(vv_scaleFactor);


	  // TODO update timeCentroid and exposure would the actualTimes and actualDurations binary blocks be provided.

	  if(flagsPtr)           dataDumpPtr->attachFlags(           flagsSize, es_flagsAxes,
							             numFlags,  flagsPtr + nt*flagsSize);

	  if(actualTimesPtr)     dataDumpPtr->attachActualTimes(     actualTimesSize, es_actualTimesAxes,
								     numActualTimes,  actualTimesPtr + nt*actualTimesSize);

	  if(actualDurationsPtr) dataDumpPtr->attachActualDurations( actualDurationsSize, es_actualDurationsAxes,
								     numActualDurations, actualDurationsPtr + nt*actualDurationsSize);

	  if(zeroLagsPtr)        dataDumpPtr->attachZeroLags(        zeroLagsSize, es_zeroLagsAxes,
								     numZeroLags, zeroLagsPtr + nt*zeroLagsSize);

	  if(crossShortDataPtr)  dataDumpPtr->attachCrossData(       crossDataSize, es_crossDataAxes,
								     numCrossData, crossShortDataPtr + nt*crossDataSize);

	  if(crossIntDataPtr)    dataDumpPtr->attachCrossData(       crossDataSize, es_crossDataAxes,
								     numCrossData, crossIntDataPtr + nt*crossDataSize);

	  if(crossFloatDataPtr)  dataDumpPtr->attachCrossData(       crossDataSize, es_crossDataAxes,
								     numCrossData, crossFloatDataPtr + nt*crossDataSize);

	  if(autoDataPtr)        dataDumpPtr->attachAutoData(        autoDataSize, es_autoDataAxes,
								     numAutoData, autoDataPtr + nt*autoDataSize);


	  dataDumpPtr->setContextUsingProjectPath(dataSubset.projectPath());

	  if(verbose_)cout<<"store dataDump"<<endl;
	  v_dataDump_.push_back(dataDumpPtr);
	  if(verbose_)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;
	}
	if(verbose_)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;

      }else{



	int64_t                    interval;
	int64_t                    exposure;
	int64_t                    timeOfInteg;
	int64_t                    timeCentroid;
//	unsigned int                 dumpNum;

// 	interval_    = interval;
// 	exposure     = interval_;
// 	timeOfInteg  = midIntervalTime;
// 	timeCentroid = time_;
// 	dumpNum      = integNumCounter_++;

	const vector<SDMDataSubset>& v_dataSubset = sdmdosr.allRemainingSubsets();	
	if(verbose_)cout<<"ici 0 v_dataSubset.size()="<<v_dataSubset.size()<<endl;
	for(unsigned int nt=0; nt<v_dataSubset.size(); nt++){

	  //int nt = 0;
	  //while (sdmdosr.hasSubset() ) {
	  //const SDMDataSubset& currentSubset = sdmdosr.getSubset();
	  const SDMDataSubset& currentSubset = v_dataSubset[nt];
	  if(verbose_){
	    cout<<"filename="<<filename<<endl;
	    cout << currentSubset.toString();
	    cout<<"ici 1"<<endl;
	  }

//	  dumpNum     = nt+1;  //nt++;

	  timeOfInteg  = currentSubset.time();        if(verbose_)cout<<"attachDataObject: "<<timeOfInteg<<endl;
	  timeCentroid = currentSubset.time();        if(verbose_)cout<<"attachDataObject: "<<timeCentroid<<endl;
	  interval     = currentSubset.interval();    if(verbose_)cout<<"attachDataObject: "<<interval<<endl;
	  exposure     = interval;
	  if(verbose_){
	    for(unsigned int i=0; i<v_numSpectralWindow.size(); i++){
	      for(unsigned int j=0; j<v_numSpectralWindow[i]; j++)
		cout<<"vv_numSpectralPoint[i][j]="<<vv_numSpectralPoint[i][j]<<endl;
	    }
	    for(unsigned int i=0; i<vv_numAutoPolProduct.size(); i++){
	      for(unsigned int j=0; j<vv_numAutoPolProduct[i].size(); j++)
		cout<<"vv_numAutoPolProduct[i][j]="<<vv_numAutoPolProduct[i][j]<<endl;
	    }
	    for(unsigned int i=0; i<vv_numCrossPolProduct.size(); i++){
	      for(unsigned int j=0; j<vv_numCrossPolProduct[i].size(); j++)
		cout<<"vv_numCrossPolProduct[i][j]="<<vv_numCrossPolProduct[i][j]<<endl;
	    }
	  }
	  DataDump* dataDumpPtr = new DataDump( vv_numCrossPolProduct,
						vv_numAutoPolProduct,
						vv_numSpectralPoint,
						vv_numBin,
						vv_e_sideband,
						numApc,
						v_numSpectralWindow,
						numBaseband,
						numAnt,
						correlationMode,
						timeOfInteg,
						timeCentroid,
						interval,
						exposure
					       );
	  if(e_cm[CROSS_ONLY]    )dataDumpPtr->setScaleFactor(vv_scaleFactor);
	  if(e_cm[CROSS_AND_AUTO])dataDumpPtr->setScaleFactor(vv_scaleFactor);

	  if(verbose_)cout<<"m_dc_sizeAndAxes.size()="<< m_dc_sizeAndAxes.size() << endl;
	  if((itf=m_dc_sizeAndAxes.find(FLAGS))!=ite){
	    if(verbose_)cout<<"Flags have been declared in the main header"<<endl;
	    const unsigned int* flagsPtr;
	    unsigned int long numFlags = currentSubset.flags( flagsPtr );
	    if(verbose_)cout<<numFlags<<" "<<itf->second.first<<endl;
	    if(numFlags)dataDumpPtr->attachFlags( itf->second.first, itf->second.second,
						  numFlags, flagsPtr);
	  }

	  if((itf=m_dc_sizeAndAxes.find(ACTUAL_TIMES))!=ite){
	    if(verbose_)cout<<"ActualTimes have been declared in the main header"<<endl;
	    const int64_t* actualTimesPtr;
	    unsigned long int numActualTimes = currentSubset.actualTimes( actualTimesPtr );
	    if(verbose_)cout<<numActualTimes<<" "<<itf->second.first<<endl;
	    if(numActualTimes)dataDumpPtr->attachActualTimes( itf->second.first, itf->second.second,
							      numActualTimes, actualTimesPtr);
	  }

	  if((itf=m_dc_sizeAndAxes.find(ACTUAL_DURATIONS))!=ite){
	    if(verbose_)cout<<"ActualDurations have been declared in the main header"<<endl;
	    const int64_t* actualDurationsPtr;
	    unsigned long int numActualDurations = currentSubset.actualDurations( actualDurationsPtr );
	    if(verbose_)cout<<numActualDurations<<" "<<itf->second.first<<endl;
	    if(numActualDurations)dataDumpPtr->attachActualDurations( itf->second.first, itf->second.second,
								      numActualDurations, actualDurationsPtr);
	  }

	  if((itf=m_dc_sizeAndAxes.find(ZERO_LAGS))!=ite){
	    if(verbose_)cout<<"ZeroLags have been declared in the main header"<<endl;
	    const float* zeroLagsPtr;
	    unsigned long int numZeroLags = currentSubset.zeroLags( zeroLagsPtr );
	    if(verbose_)cout<<numZeroLags<<" "<<itf->second.first<<endl;
	    if(numZeroLags)dataDumpPtr->attachZeroLags( itf->second.first, itf->second.second,
							numZeroLags, zeroLagsPtr);
	  }

	  if((itf=m_dc_sizeAndAxes.find(CROSS_DATA))!=ite){
	    if(verbose_)cout<<"CrossData have been declared in the main header ";
	    unsigned long int numCrossData;
	    switch(currentSubset.crossDataType()){
	    case INT16_TYPE:
	      { const short int* crossDataPtr;
		numCrossData = currentSubset.crossData( crossDataPtr );
		if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							      numCrossData, crossDataPtr);
	      }
	      if(verbose_)cout<<"SHORT_TYPE"<<endl;
	      if(verbose_)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataShort()<<endl;
	      break;
	    case INT32_TYPE:
	      { const int* crossDataPtr;
		numCrossData = currentSubset.crossData( crossDataPtr );
		if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							      numCrossData, crossDataPtr);
	      }
	      if(verbose_)cout<<"INT_TYPE"<<endl;
	      if(verbose_)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataLong()<<endl;
	      break;
	    case FLOAT32_TYPE:
	      { const float* crossDataPtr;
		numCrossData = currentSubset.crossData( crossDataPtr );
		if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							      numCrossData, crossDataPtr);
	      }
	      if(verbose_)cout<<"FLOAT_TYPE"<<endl;
	      if(verbose_)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataFloat()<<endl;
	      break;
	    default:
	      Enum<PrimitiveDataType> e_pdt=currentSubset.crossDataType();
	      Error(FATAL, (char *) "Cross data with the primitive data type %s are not supported",
		    e_pdt.str().c_str());
	    }
	  }

	  if((itf=m_dc_sizeAndAxes.find(AUTO_DATA))!=ite){
	    if(verbose_)cout<<"AutoData have been declared in the main header"<<endl;
	    const float*      floatDataPtr=NULL;
	    unsigned long int numFloatData = currentSubset.autoData( floatDataPtr );
	    if(verbose_)cout<<numFloatData<<" "<<itf->second.first<<"  "<<floatDataPtr<<endl;
	    if(numFloatData){
	      if(numFloatData!=itf->second.first)
		Error(FATAL,(char *) "Size of autoData, %d, not compatible with the declared size of %d",
		      numFloatData,itf->second.first);
	      dataDumpPtr->attachAutoData( itf->second.first, itf->second.second,
					   numFloatData, floatDataPtr);
	      if(verbose_)cout<<"autoData attached,  const pointer:"<<dataDumpPtr->autoData()<<endl;
// 	      const long unsigned int* aptr=dataDumpPtr->flags();
	    }else if(numFloatData==0){
	       if(!e_cm[CROSS_ONLY])
		 Error(WARNING,string("No autoData! may happen when a subscan is aborted"));
	      break;
	    }
	  }

	  if (verbose_) cout << "About to setContextUsingProjectPath" << endl;
	  dataDumpPtr->setContextUsingProjectPath(currentSubset.projectPath());
	  if (verbose_) cout << "Back from setContextUsingProjectPath" << endl;
	  const unsigned int* bptr=dataDumpPtr->flags(); if(bptr==NULL)if(verbose_)cout<<"No flags"<<endl;
	  if(verbose_)cout<<"store dataDump"<<endl;
	  v_dataDump_.push_back(dataDumpPtr);
	  if(verbose_)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;
	}
	if(verbose_)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;
      }
      if(verbose_)cout<<"numTime="<<numTime<<", about to return 0"<<endl;

//       for (unsigned long int nt = 0; nt <v_dataSubset.size(); nt++) {
// 	if(verbose_)cout << currentSubset.toString();
//       }
      return 0;
    }
    catch (SDMDataObjectStreamReaderException e) { throw; } // Rethrow the exception, it'll be handled at a higher level.
    catch (SDMDataObjectReaderException e) {  cout << e.getMessage()          << endl; }  
    catch (SDMDataObjectParserException e) { cout << e.getMessage()          << endl; }
    catch (SDMDataObjectException e)       { cout << e.getMessage()          << endl; }
    catch (std::exception e)                    { cout << e.what()                << endl; }
    catch (...)                            { cout << "Unexpected exception." << endl; }
    if(verbose_)cout<<"SDMBinData::attachStreamDataObject(const string& dataOID ): exiting"<<endl;
    return 1;
}

  void SDMBinData::detachDataObject(){
//     if(dataOID_.length())cout<<"detachDataObject() using done()"<<endl;
    bool coutest = false;
    if (coutest) cout << "SDMBinData::detachDataObject: entering" << endl;
    if (bdfMemoryMapped) {
      if(dataOID_.length())blob_r_.done();
      dataOID_="";
    }
    else 
      sdmdosr.close();
    if (coutest) cout << "SDMBinData::detachDataObject: exiting" << endl;
    return;
  }

  vector<SDMData*> SDMBinData::getData(){

    detachDataObject();
    return  v_sdmDataPtr_;

  }

  vector<MSData*>  SDMBinData::getData( Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc){
    
    if (verbose_) cout << "SDMBinData::getData( Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc): entering" << endl;
    if(verbose_)
      cout<<"Enter in method getData(e_qcm="<<e_qcm.str()
	  <<",es_qapc="<<es_qapc.str()<<")"
	  << endl;

    // note, this is a LOCAL msDataPtr_ here
    MSData* msDataPtr_ = 0;

    if(v_msDataPtr_.size()>0){
      if(coutDeleteInfo_)cout<<"About to delete "<<v_msDataPtr_.size()<<" msDataPtr objects"<<endl;
      for(vector<MSData*>::reverse_iterator it=v_msDataPtr_.rbegin(); it!=v_msDataPtr_.rend(); ++it)
	deleteMsData(*it);
      
      v_msDataPtr_.clear();    if(coutDeleteInfo_)cout<<"v_msDataPtr_ cleared"<<endl;
    }

    ConfigDescriptionRow*       cdPtr = mainRowPtr_->getConfigDescriptionUsingConfigDescriptionId();
    vector<AtmPhaseCorrection>  v_apc = cdPtr->getAtmPhaseCorrection();
    Enum<CorrelationMode>       e_cm; e_cm = cdPtr->getCorrelationMode();

    EnumSet<AtmPhaseCorrection> es_qapc_uncorrected, es_qapc_old;
;
    es_qapc_uncorrected.fromString("AP_UNCORRECTED");
    if (processorType(mainRowPtr_) == RADIOMETER) {
      es_qapc_old = es_qapc_;
      es_qapc_ = es_qapc_uncorrected;
    }
    
    if(!canSelect_ && (e_qcm.count() || es_qapc.count()) ){

      Error(FATAL,string("This method cannot be used in this context!\n Use the method with no argument getData()"));
      return v_msDataPtr_;

    }

    if(canSelect_){

      // set the new defaults
      e_qcm_   = e_qcm;
      es_qapc_ = es_qapc;

    }

    if(verbose_){
      cout<<"e_qcm=  "<<e_qcm.str()  <<" e_qcm_=  "<<e_qcm_.str()  <<" e_cm="<<e_cm.str()<<endl;
      cout<<"es_qapc="<<es_qapc.str()<<" es_qapc_="<<es_qapc_.str();
      for(unsigned int n=0; n<v_apc.size(); n++)
	cout<<" v_apc["<<n<<"]="<<Enum<AtmPhaseCorrection>(v_apc[n]).str()<<" ";
      cout<<endl;
    }

    if(e_qcm_[CROSS_ONLY]) if(e_cm[AUTO_ONLY])  return v_msDataPtr_;
    if(e_qcm_[AUTO_ONLY])  if(e_cm[CROSS_ONLY]) return v_msDataPtr_;


    vector<unsigned int>       v_selected_napc;
    vector<AtmPhaseCorrection> v_selected_apc;
    for(unsigned int n=0; n<v_apc.size(); n++){
      if(es_qapc_[v_apc[n]]){
	v_selected_napc.push_back(n);
	v_selected_apc.push_back(v_apc[n]);
      }
    }
    if(!v_selected_napc.size())return v_msDataPtr_;

    Tag configDescriptionId = mainRowPtr_->getConfigDescriptionId();


    std::set<Tag>::iterator
      itsf=s_cdId_.find(configDescriptionId),
      itse=s_cdId_.end();
    if(itsf==itse)
      Error(FATAL,"Tree hierarchy not present for configDescId " + configDescriptionId.toString());
    std::map<Tag,BaselinesSet*>::iterator
      itf(m_cdId_baselinesSet_.find(configDescriptionId)),
      ite(m_cdId_baselinesSet_.end());
    if(itf==ite)
      Error(FATAL,"Tree hierarchy not present for configDescId " + configDescriptionId.toString());
    baselinesSet_=itf->second;

    if(verbose_)cout<<configDescriptionId.toString()<<endl;



    unsigned int               stateId;
    vector<StateRow*>          v_statePtr = mainRowPtr_->getStatesUsingStateId();

    vector<int>                v_feedSet  = cdPtr->getFeedId();
    vector<Tag>                v_antSet   = cdPtr->getAntennaId();
    vector<Tag>                v_ddList   = cdPtr->getDataDescriptionId();


    unsigned int               numFeed    = v_feedSet.size()/v_antSet.size();
    unsigned int               numBin;
    unsigned int               numPol;
    vector<unsigned int>       v_dataShape;  v_dataShape.resize(3);  // POL.SPP.APC 3D

    double                     timeMJD;
    double                     timeCentroidMJD;
    double                     interval;
    double                     exposure;
    ArrayTime                  timeOfDump;
    ArrayTime                  timeCentroidOfDump;

    int                        fieldId    = mainRowPtr_->getFieldUsingFieldId()->getFieldId().getTagValue();
    vector<vector<Angle> >     phaseDir   = mainRowPtr_->getFieldUsingFieldId()->getPhaseDir();


    vector<Tag>                v_stateId;

//     unsigned int nd;

//     vector<unsigned int> v_dataShape;
//     v_msDataPtr_[n]->v_dataShape = v_dataShape;

    if(verbose_)cout <<"ici AA: e_qcm_="<<e_qcm_.str()<<endl;

    for(unsigned int na=0; na<v_antSet.size(); na++)
      v_stateId.push_back(v_statePtr[na]->getStateId());

    vector<vector<float> >                      vv_t; // dummy empty vector would there be no tsysSpectrum for a given syscal row
    SysCalTable&                                sct = datasetPtr_->getSysCal();
    vector<Tag>                                 v_spwid;
    vector<pair<bool,vector<vector<float> > > > v_tsys;
    vector<vector<pair<bool,vector<vector<float> > > > > vv_tsys;


    if(SDMBinData::syscal_){
      v_tsys.resize(v_antSet.size()*v_ddList.size()*numFeed);
      for(unsigned int nt=0; nt<v_dataDump_.size(); nt++){
	vector<pair<bool,vector<vector<float> > > > v_tsys;
	timeCentroidOfDump= ArrayTime((int64_t)v_dataDump_[nt]->timeCentroid());
	unsigned int scn=0;
	for(unsigned int na=0; na<v_antSet.size(); na++){
	  for(unsigned int nfe=0; nfe<numFeed; nfe++){
	    for(unsigned int nsw=0; nsw<v_ddList.size(); nsw++){
	      SysCalRow* scr = sct.getRowByKey( v_antSet[na],
						baselinesSet_->getSpwId(nsw),
						ArrayTimeInterval(timeCentroidOfDump),
						v_feedSet[na*numFeed+nfe]
						);
	      if(scr->isTsysSpectrumExists()){
		vector<vector<float> > vv_flt;
		v_tsys[scn].first  = scr->getTsysFlag();
		vector<vector<Temperature> > vv=scr->getTsysSpectrum();
		for(unsigned int np=0; np<vv.size(); np++){
		  vector<float> v;
		  for(unsigned int ns=0; ns<vv[np].size(); ns++)v.push_back(vv[np][ns].get());
		  vv_flt.push_back(v);
		}
		v_tsys[scn].second = vv_flt;
	      }else{
		v_tsys[scn].first  = false;
		v_tsys[scn].second = vv_t;
	      }
	      scn++;
	    }
	  }
	}
	vv_tsys.push_back(v_tsys);
      }
    }

    if(e_cm[CROSS_ONLY]==false && e_qcm_[CROSS_ONLY]==false ){       // retrieve only AUTO_DATA
      for(unsigned int nt=0; nt<v_dataDump_.size(); nt++){
	timeOfDump        = ArrayTime((int64_t)v_dataDump_[nt]->time());               if(verbose_)cout<<timeOfDump<<" ns"<<endl;
	timeCentroidOfDump= ArrayTime((int64_t)v_dataDump_[nt]->timeCentroid());       if(verbose_)cout<<timeCentroidOfDump.toString()<<" ns"<<endl;
	timeMJD           = timeOfDump.getMJD();                              if(verbose_)cout<<timeMJD<<" h = "<<86400.*timeMJD<<" s"<<endl;
	interval          = (double)v_dataDump_[nt]->interval()/1000000000LL; if(verbose_)cout<<interval<<" s"<<endl;
	timeCentroidMJD   = timeCentroidOfDump.getMJD();                      if(verbose_)cout<< timeCentroidMJD<<endl;
	exposure          = (double)v_dataDump_[nt]->exposure()/1000000000LL;
	floatDataDumpPtr_ = v_dataDump_[nt]->autoData();             // used by getData(na, nfe, ndd, nbi)
	//cout << "floatDataDumpPtr_ = " << floatDataDumpPtr_ << endl;
	//cout << "nt = " << nt << endl;
 	//cout<<"1st & 2nd data for nt="<<nt<<": "<<floatDataDumpPtr_[0]<<" "<<floatDataDumpPtr_[1]<<endl;


// 	for(unsigned int n=0; n<v_integrationPtr_.size();n++){
// 	  nd = v_integrationPtr_[n]->numData();
// 	  float* toto=v_integrationPtr_[n]->floatData();
// 	  cout<<"ET LA with nt="<<nt
// 	      <<" toto[0]="<<toto[0]
// 	      <<" toto[1]="<<toto[1]
// 	      <<" toto["<<nd-1<<"]="<<toto[nd-1]<<endl;
// 	  cout<<"ET LA with nt="<<nt
// 	      <<" &toto[0]="<<&toto[0]
// 	      <<" &toto[1]="<<&toto[1]
// 	      <<" &toto["<<nd-1<<"]="<<&toto[nd-1]<<endl;
// 	}


	if(verbose_)cout <<"ici BB"<<endl;

	vector<AtmPhaseCorrection> v_uapc; v_uapc.push_back(AP_UNCORRECTED);

	unsigned int scn=0;
	for(unsigned int na=0; na<v_antSet.size(); na++){
	  stateId = v_statePtr[na]->getStateId().getTagValue();
	  for(unsigned int nfe=0; nfe<numFeed; nfe++){
	    for(unsigned int ndd=0; ndd<v_ddList.size(); ndd++){
	      numBin = baselinesSet_->numBin(ndd);
	      numPol = baselinesSet_->numPol(baselinesSet_->getBasebandIndex(ndd));
	      if(numPol==4)numPol=3;                                 // if XX,XY,YX,YY then auto XX,XY,YY 
	      if(verbose_)cout<<"numPol="<<numPol
			     <<"  na="  <<na
			     <<" ant="  <<v_antSet[na].getTagValue()
			     <<endl;
	      v_dataShape[0]=numPol;
	      //	      if(complexData_)v_dataShape[0]*=2;
	      v_dataShape[1]=baselinesSet_->numChan(ndd);
	      v_dataShape[2]=1;                                      // auto-correlation never atm phase corrected
	      for(unsigned int nbi=0; nbi<numBin; nbi++){
		// the data and binary meta-data
		if(SDMBinData::syscal_)
		  msDataPtr_ = getCalibratedData( na, nfe,
						  ndd, nbi, 
						  vv_tsys[nt][scn]);
		else
		  msDataPtr_ = getData( na, nfe,
					ndd, nbi);
		scn++;
		msDataPtr_->timeCentroid   = 86400.*timeCentroidMJD; // default value would there be no bin actualTimes
		msDataPtr_->exposure       = exposure;               // default value would there be no bin actualDurations
		msDataPtr_->flag           = 0;                      // default value is "false"  would there be no bin flags
		// calibrate and associate SDM meta-data
		if(msDataPtr_->numData>0){
		  msDataPtr_->processorId        = cdPtr->getProcessorId().getTagValue();
		  msDataPtr_->antennaId1         = v_antSet[na].getTagValue();
		  msDataPtr_->antennaId2         = v_antSet[na].getTagValue();
		  msDataPtr_->feedId1            = v_feedSet[na*numFeed+nfe];
		  msDataPtr_->feedId2            = v_feedSet[na*numFeed+nfe];
		  if(e_cm[CROSS_AND_AUTO])
		    msDataPtr_->dataDescId       = baselinesSet_->getAutoDataDescriptionId(v_ddList[ndd]).getTagValue();
		  else
		    msDataPtr_->dataDescId       = v_ddList[ndd].getTagValue();
		  msDataPtr_->phaseDir           = phaseDir;                     // TODO Apply the polynomial formula
		  msDataPtr_->stateId            = stateId;
		  /* This part is now done in the filler itself -  MC 23 Jul 2012
		    msDataPtr_->msState            = getMSState( subscanNum, v_stateId,
							       v_antSet, v_feedSet, v_ddList,
							       na, nfe, ndd, timeOfDump);
		  */
		  msDataPtr_->v_dataShape        = v_dataShape;
		  msDataPtr_->time               = 86400.*timeMJD;
		  msDataPtr_->interval           = interval;
		  msDataPtr_->v_atmPhaseCorrection = v_uapc;         // It is assumed that this is always the uncorrected case
		  msDataPtr_->binNum             = nbi+1;
		  msDataPtr_->fieldId            = fieldId;
		  vector<unsigned int> v_projectNodes;
		  if(v_dataDump_[nt]->integrationNum())v_projectNodes.push_back(v_dataDump_[nt]->integrationNum());
		  if(v_dataDump_[nt]->subintegrationNum())v_projectNodes.push_back(v_dataDump_[nt]->subintegrationNum());
		  msDataPtr_->projectPath        = v_projectNodes;
		  // this pointer ends up in v_msDataPtr_ and is deleted from there
		  v_msDataPtr_.push_back(msDataPtr_);                      // store in vector for export
		}
		if(verbose_)cout << "B/ msDataPtr_->numData=" <<msDataPtr_->numData << endl;
	      }
	    }
	  }
	}
      }
    }

    if(verbose_)cout <<"ici CC: "<<e_qcm_.str()<<endl;
    vector<AtmPhaseCorrection> v_atmPhaseCorrection = cdPtr->getAtmPhaseCorrection();
    bool queryCrossData = false;
    if(e_qcm_[CROSS_ONLY])     queryCrossData=true;
    if(e_qcm_[CROSS_AND_AUTO]) queryCrossData=true;

    if(e_cm[AUTO_ONLY]==false && queryCrossData ){

      // select the queried apc
      vector<unsigned int> v_napc;
      EnumSet<AtmPhaseCorrection> es_apc; es_apc.set(v_atmPhaseCorrection);    if(verbose_)cout<<es_apc.str()<<endl;
      if(verbose_)cout <<"es_apc from BLOB: " << es_apc.str()   << endl;
      if(verbose_)cout <<"es_qapc_ queried: " << es_qapc_.str() << endl;
      for(unsigned int napc=0; napc<v_atmPhaseCorrection.size(); napc++)
	if(es_qapc_[v_atmPhaseCorrection[napc]])v_napc.push_back(napc);
      if(verbose_)for(unsigned int n=0; n<v_napc.size(); n++)cout<<"v_napc["<<n<<"]="<<v_napc[n]<<endl;

      if(!v_napc.size()){
	Error(WARNING,(char *) "No visibilities with AtmPhaseCorrection in the set {%s}",
	      es_qapc_.str().c_str());
	return v_msDataPtr_;
      }

      if(verbose_)cout<<"ici DD"<<endl;


      for(unsigned int nt=0; nt<v_dataDump_.size(); nt++){
	timeOfDump        = ArrayTime((int64_t)v_dataDump_[nt]->time());                if(verbose_)cout<<timeOfDump<<endl;
	timeCentroidOfDump= ArrayTime((int64_t)v_dataDump_[nt]->timeCentroid());        if(verbose_)cout<< timeCentroidOfDump.toString() <<endl;
	timeMJD           = timeOfDump.getMJD();                               if(verbose_)cout<<timeMJD<<" h"<<endl;
	interval          = (double)v_dataDump_[nt]->interval()/1000000000LL;
	timeCentroidMJD   = timeCentroidOfDump.getMJD();                       if(verbose_)cout<< timeCentroidMJD<<" h"<<endl;
	exposure          = (double)v_dataDump_[nt]->exposure()/1000000000LL;;
	if(verbose_)cout<<"ici DD 1"<<endl;
	shortDataPtr_ = NULL;
	longDataPtr_  = NULL;
	floatDataPtr_ = NULL;
	if(v_dataDump_[nt]->crossDataShort())
	  shortDataPtr_   = v_dataDump_[nt]->crossDataShort();       // used by getData(na, na2, nfe, ndd, nbi, napc)
	else if(v_dataDump_[nt]->crossDataLong())
	  longDataPtr_    = v_dataDump_[nt]->crossDataLong();
	else if(v_dataDump_[nt]->crossDataFloat())
	  floatDataPtr_    = v_dataDump_[nt]->crossDataFloat();
	else
	  Error(FATAL, string("Cross data typed float not yet supported"));
	if(verbose_)cout<<"ici DD 2"<<endl;
	unsigned int scn=0;
	for(unsigned int na1=0; na1<v_antSet.size(); na1++){
	  // we will assume that na2 has the same stateId (perhaps we should return a vector?)!
	  stateId = v_statePtr[na1]->getStateId().getTagValue();
	  for(unsigned int na2=na1+1; na2<v_antSet.size(); na2++){
	    for(unsigned int nfe=0; nfe<numFeed; nfe++){
	      for(unsigned int ndd=0; ndd<v_ddList.size(); ndd++){
		if(verbose_)cout<<"ici DD 3   numApc="<<baselinesSet_->numApc()<<endl;
		numBin = baselinesSet_->numBin(ndd);
		numPol = baselinesSet_->numPol(baselinesSet_->getBasebandIndex(ndd));   // nb of pol product (max is 4)
		if(verbose_)cout<<"ici DD 4   numBin="<<numBin<<"  numPol="<<numPol<<endl;
		v_dataShape[0]=baselinesSet_->numPol(ndd);
		v_dataShape[1]=baselinesSet_->numChan(ndd);
		v_dataShape[2]=baselinesSet_->numApc();
		v_dataShape[2]=1;               // qapc being not an EnumSet (MS limitation for floatData column)
		for(unsigned int nbi=0; nbi<numBin; nbi++){
		  if(verbose_){
		    cout<<"timeCentroidMJD="<<timeCentroidMJD<<endl;
		  }
		  // the data and binary meta-data
		  if (verbose_) {
		    cout << "nt = " << nt << endl;
		    cout << "size of v_dataDump_ = " << v_dataDump_.size() << endl;
		  }

		  if(SDMBinData::syscal_) {
		    msDataPtr_ = getCalibratedData( na1, nfe, na2, nfe,
					  ndd,  nbi, v_napc,
					  v_dataDump_[nt]->scaleFactor(ndd),
					  vv_tsys[nt][scn]);
		  }
		  else {
		    msDataPtr_ = getData( na1, nfe, na2, nfe,
					  ndd,  nbi, v_napc,
					  v_dataDump_[nt]->scaleFactor(ndd));
		  }

		  msDataPtr_->timeCentroid  = 86400.*timeCentroidMJD; // default value would there be no bin actualTimes
		  msDataPtr_->exposure      = exposure;               // default value would there be no bin actualDurations
		  msDataPtr_->flag          = 0;                      // default value is "false"  would there be no bin flags
		  if(verbose_)cout<<"ici DD 7 msDataPtr_->numData="<<msDataPtr_->numData<<endl;
		  // the associated SDM meta-data
		  if(msDataPtr_->numData>0){
		    msDataPtr_->processorId          = cdPtr->getProcessorId().getTagValue();
		    msDataPtr_->time                 = 86400.*timeMJD;
		    msDataPtr_->antennaId1           = v_antSet[na1].getTagValue();
		    msDataPtr_->antennaId2           = v_antSet[na2].getTagValue();
		    msDataPtr_->feedId1              = v_feedSet[na1*numFeed+nfe];
		    msDataPtr_->feedId2              = v_feedSet[na2*numFeed+nfe];
		    msDataPtr_->dataDescId           = v_ddList[ndd].getTagValue();
		    msDataPtr_->fieldId              = fieldId;
		    msDataPtr_->phaseDir             = phaseDir;
		    /* This part is now done in the filler itself -  MC 23 Jul 2012
		    msDataPtr_->msState              = getMSState( subscanNum, v_stateId,
								   v_antSet, v_feedSet, v_ddList,
								   na1, nfe, ndd, timeOfDump);
		    */
                     // TODO Apply the polynomial formula
		    msDataPtr_->v_dataShape          = v_dataShape;
		    msDataPtr_->interval             = interval;
		    msDataPtr_->v_atmPhaseCorrection = v_selected_apc;
		    msDataPtr_->binNum               = nbi+1;              // a number is one-based.
		    vector<unsigned int> v_projectNodes;
		    if(v_dataDump_[nt]->integrationNum())v_projectNodes.push_back(v_dataDump_[nt]->integrationNum());
		    if(v_dataDump_[nt]->subintegrationNum())v_projectNodes.push_back(v_dataDump_[nt]->subintegrationNum());
		    msDataPtr_->projectPath          = v_projectNodes;
		  }
		  // store in vector for export
		  // this point is put into v_msDataPtr_ and should be deleted from there
		  if(msDataPtr_->numData>0)v_msDataPtr_.push_back(msDataPtr_);
		  if(verbose_)cout << "A/ msDataPtr_->numData=" << msDataPtr_->numData << endl;
		  scn++;
		}
	      }
	    }
	  }
	}
      }
      if(verbose_)cout<<"v_msDataPtr_.size()="<< v_msDataPtr_.size() << endl;
    }

    detachDataObject();
    if (processorType(mainRowPtr_) == RADIOMETER) {
      es_qapc_ = es_qapc_old;
    }

    if (verbose_) cout << "SDMBinData::getData( Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc): exiting" << endl;
    return v_msDataPtr_;
  }

  MSData* SDMBinData::getData( unsigned int na, unsigned int nfe,
			       unsigned int ndd, unsigned int nbin) throw (Error)
  {
    if (verbose_) cout << "SDMBinData::getData : entering" << endl;
    unsigned int numBin = baselinesSet_->numBin(ndd);                  if(verbose_)cout << "numBin=" << numBin << endl;
    if((nbin+1)>numBin)return 0;


    msDataPtr_ = new MSData;


    msDataPtr_->numData =  baselinesSet_->numAutoData(ndd)/numBin;  // number of measurements (NOT nb values)
    msDataPtr_->v_data.resize(1);                                   // autoData have always an APC axis of 1

    
    if(verbose_)cout << "About to retrieve " <<  msDataPtr_->numData
		    << " auto-correlation measurements (auto correlations) encoded in floats" << endl;
    if(verbose_)cout<<"SDMBinaryData::getData: Query for na="<<na<<" nfe="<<nfe<<" ndd="<<ndd<<" nbin="<<nbin<<endl;

    // ff    const float* autoData =  &floatDataDumpPtr_[baselinesSet_->transferId(na,ndd,nbin)/sizeof(float)];
    const float* autoData =  &floatDataDumpPtr_[baselinesSet_->transferId(na,ndd,nbin)];
    if(verbose_)cout <<"transferId(na,ndd,nbin)="<<baselinesSet_->transferId(na,ndd,nbin);
    //if(baselinesSet_->numAutoData(ndd)>2){
      
    //   if(verbose_)
    // 	cout <<" up to "<<baselinesSet_->transferId(na,ndd,nbin)+(4*msDataPtr_->numData)/3<<" float values"<<endl;
    // }else{
    //   if(verbose_)
    // 	cout <<" up to "<<baselinesSet_->transferId(na,ndd,nbin)+msDataPtr_->numData<<" float values"<<endl;
    // }
//     if(baselinesSet_->transferId(na,ndd,nbin)+msDataPtr_->numData*sizeof(float)>autoCorrContainerSize_){
//       Error(FATAL,"TOO MANY DATA for the Container");
//       return 0;
//     }

    if (baselinesSet_->numSdPol(ndd)>2)  msDataPtr_->numData =  (msDataPtr_->numData / 3 ) * 4; // Attention !!! the value of numData returned by baselinesSet_->numAutoData
                                                                                                // is equal to the size of sdPolProducts, e.g. 3 in the case of "XX XY YX",
                                                                                                // But the number of values to be retrieved in that specific case is 4
                                                                                                // since XX and YY are encoded as simple reals while XY is encoded as a complex.
                                                                                                // Hence this calculation. This tricky detail was discovered while I was working
                                                                                                // on CAS-6935 and ICT-3617.
                                                                                                // Michel Caillat - 24/09/2014
                                                               
    if(verbose_)cout << "numData=" << msDataPtr_->numData << endl;
    if(verbose_)cout << "numSdPol("<<ndd<<")="<<baselinesSet_->numSdPol(ndd)<<endl;
    if(baselinesSet_->numSdPol(ndd)>2){        // shape in complex to have the same shape as the visibilities
      int nmax = msDataPtr_->numData / 4 * 6;  // Attention we assume that the auto data are stored on 1 float + 1 complex + 1 float
                                               // (e.g. XX on one float, XY on 1 complex and YY on one float)
                                               // and the idea is to reshape all this in three complexes.
      msDataPtr_->v_data[0] = new float[nmax];
      int k=0;
      for (int n = 0; n < msDataPtr_->numData; n = n + 4) {
	msDataPtr_->v_data[0][k++]=autoData[n];        // XX real
	msDataPtr_->v_data[0][k++]=0.0;                // XX imaginary
	msDataPtr_->v_data[0][k++]=autoData[n+1];      // XY real
	msDataPtr_->v_data[0][k++]=autoData[n+2];      // XY imaginary
	msDataPtr_->v_data[0][k++]=autoData[n+3];      // YY real
	msDataPtr_->v_data[0][k++]=0.0;                // YY imaginary
      }
      if(verbose_){
	cout << "1st auto: r " << msDataPtr_->v_data[0][0] << " i " << msDataPtr_->v_data[0][1] << endl;
	cout << "2nd auto: r " << msDataPtr_->v_data[0][2] << " i " << msDataPtr_->v_data[0][3] << endl;
	cout << "3rd auto: r " << msDataPtr_->v_data[0][4] << " i " << msDataPtr_->v_data[0][5] << endl;
      }

      msDataPtr_->numData = nmax;
      if(verbose_)cout << "k=" << k << endl;
    }else if(forceComplex_){                  // original was 100% real data, transform shape in complex to have shape of visibilities
      msDataPtr_->v_data[0] = new float[2*msDataPtr_->numData];
      for(int n=0; n<msDataPtr_->numData; n++){
	msDataPtr_->v_data[0][2*n]   = autoData[n];
	msDataPtr_->v_data[0][2*n+1] = 0.0;
      }
      if(verbose_)cout << "1st auto: r " << msDataPtr_->v_data[0][0] << " i " << msDataPtr_->v_data[0][1] << endl;
    }else{
      msDataPtr_->v_data[0] = new float[msDataPtr_->numData];
      for(int n=0; n<msDataPtr_->numData; n++)msDataPtr_->v_data[0][n]=autoData[n];
      if(verbose_)cout << "1st auto: " <<(int) msDataPtr_->v_data[0][0] << " (a real value)" << endl;
    }

    if(verbose_)
      cout << " na="   << na
	   << " ndd="  << ndd
	   << " nbin=" << nbin
	   << endl;

    /*
      This following in this method one needs is the location where one retrieve from the
      the actualTimes and actualDurations the timeCentroid and exposure.
      In the current design of the TotalPower binary there is no way to know that these
      container are not present. Idem for the baselineFlags. Hence we drop this part but this
      will have to be recovered when we will support both total power and correlator data in
      this method.
      In this context, timeCentroid and exposure are set equal to time and interval; they are
      retrieved from v_integration.
      NB: fieldId is now retrieved in getData(queryCorrmode)
          interval: idem, from v_integration_
    */

    if (verbose_) cout << "SDMBinData::getData : exiting for SD" << endl;
    return msDataPtr_;
  }


  MSData* SDMBinData::getCalibratedData( unsigned int na, unsigned int nfe,
					 unsigned int ndd, unsigned int nbin,
					 pair<bool,vector<vector<float> > > p_tsys) throw (Error)
  {
    bool coutest=false;
    unsigned int numBin = baselinesSet_->numBin(ndd);                  if(coutest)cout << "numBin=" << numBin << endl;
    if((nbin+1)>numBin)return 0;


    msDataPtr_ = new MSData;


    msDataPtr_->numData =  baselinesSet_->numAutoData(ndd)/numBin;  // number of measurements (NOT nb values)
    msDataPtr_->v_data.resize(1);                                   // autoData have always an APC axis of 1

    if(coutest)cout << "About to retrieve " <<  msDataPtr_->numData
		    << " auto-correlation measurements (auto correlations) encoded in floats" << endl;
    if(coutest)cout<<"SDMBinaryData::getData: Query for na="<<na<<" nfe="<<nfe<<" ndd="<<ndd<<" nbin="<<nbin<<endl;

    // ff    const float* autoData =  &floatDataDumpPtr_[baselinesSet_->transferId(na,ndd,nbin)/sizeof(float)];
    const float* autoData =  &floatDataDumpPtr_[baselinesSet_->transferId(na,ndd,nbin)];
    if(coutest)cout <<"transferId(na,ndd,nbin)="<<baselinesSet_->transferId(na,ndd,nbin);
    if(baselinesSet_->numAutoData(ndd)>2){
      if(coutest)
	cout <<" up to "<<baselinesSet_->transferId(na,ndd,nbin)+(4*msDataPtr_->numData)/3<<" float values"<<endl;
    }else{
      if(coutest)
	cout <<" up to "<<baselinesSet_->transferId(na,ndd,nbin)+msDataPtr_->numData<<" float values"<<endl;
    }
//     if(baselinesSet_->transferId(na,ndd,nbin)+msDataPtr_->numData*sizeof(float)>autoCorrContainerSize_){
//       Error(FATAL,"TOO MANY DATA for the Container");
//       return 0;
//     }

    unsigned int numSdPol=baselinesSet_->numSdPol(ndd);
    if(coutest)cout << "numData=" << msDataPtr_->numData << endl;
    if(coutest)cout << "numSdPol("<<ndd<<")="<<baselinesSet_->numAutoData(ndd)<<endl;
    if(numSdPol>2){        // shape in complex to have the same shape as the visibilities
      int nmax=msDataPtr_->numData*2;     // {[(numdata*4)/3]*6}/4=numData*6/3=numData*2
      msDataPtr_->v_data[0] = new float[nmax];
      int k=0;
      nmax=(msDataPtr_->numData*4)/3;
      unsigned int nsp=0;
      for(int n=0; n<nmax; n=n+4){
	msDataPtr_->v_data[0][k++] = p_tsys.second[0][nsp]*autoData[n];                                // XX real
	msDataPtr_->v_data[0][k++] = 0.0;                                                              // XX imaginary
	msDataPtr_->v_data[0][k++] = sqrtf(p_tsys.second[0][nsp]*p_tsys.second[1][nsp])*autoData[n+1]; // XY real
	msDataPtr_->v_data[0][k++] = sqrtf(p_tsys.second[0][nsp]*p_tsys.second[1][nsp])*autoData[n+2]; // XY imaginary
	msDataPtr_->v_data[0][k++] = p_tsys.second[1][nsp]*autoData[n+3];                              // YY real
	msDataPtr_->v_data[0][k++] = 0.0;                                                              // YY imaginary
	nsp++;
      }
      if(coutest){
	cout << "1st auto: r " << msDataPtr_->v_data[0][0] << " i " << msDataPtr_->v_data[0][1] << endl;
	cout << "2nd auto: r " << msDataPtr_->v_data[0][2] << " i " << msDataPtr_->v_data[0][3] << endl;
	cout << "3rd auto: r " << msDataPtr_->v_data[0][4] << " i " << msDataPtr_->v_data[0][5] << endl;
	cout << "4th auto: r " << msDataPtr_->v_data[0][6] << " i " << msDataPtr_->v_data[0][7] << endl;
	cout << "5th auto: r " << msDataPtr_->v_data[0][8] << " i " << msDataPtr_->v_data[0][9] << endl;
	cout << "6th auto: r " << msDataPtr_->v_data[0][10]<< " i " << msDataPtr_->v_data[0][11]<< endl;
      }
      if(coutest)cout << "k=" << k << endl;
    }else if(forceComplex_){                  // original was 100% real data, transform shape in complex to have shape of visibilities
      msDataPtr_->v_data[0] = new float[2*msDataPtr_->numData];
      if(numSdPol==1){
	for(int n=0; n<msDataPtr_->numData; n++){
	  msDataPtr_->v_data[0][2*n]   = p_tsys.second[0][n]*autoData[n];
	  msDataPtr_->v_data[0][2*n+1] = 0.0;
	}
      }else{
	for(int n=0; n<msDataPtr_->numData; n=n+2){
	  msDataPtr_->v_data[0][2*n]   = p_tsys.second[0][n]*autoData[n];
	  msDataPtr_->v_data[0][2*n+1] = 0.0;
	  msDataPtr_->v_data[0][2*n+2] = p_tsys.second[1][n]*autoData[n+1];
	  msDataPtr_->v_data[0][2*n+3] = 0.0;
	}
      }
      if(coutest)cout << "1st auto: r " << msDataPtr_->v_data[0][0] << " i " << msDataPtr_->v_data[0][1] << endl;
    }else{
      msDataPtr_->v_data[0] = new float[msDataPtr_->numData];
      if(numSdPol==1){
	for(int n=0; n<msDataPtr_->numData; n++)
	  msDataPtr_->v_data[0][n] = p_tsys.second[0][n]*autoData[n];
      }else{
	for(int n=0; n<msDataPtr_->numData; n+=2){
	  msDataPtr_->v_data[0][n]   = p_tsys.second[0][n]*autoData[n];
	  msDataPtr_->v_data[0][n+1] = p_tsys.second[1][n]*autoData[n+1];
	}
      }
      if(coutest)cout << "1st auto: " <<(int) msDataPtr_->v_data[0][0] << " (a calibrated real value)" << endl;
    }

    if(coutest)
      cout << " na="   << na
	   << " ndd="  << ndd
	   << " nbin=" << nbin
	   << endl;

    /*
      This following in this method one needs is the location where one retrieve from the
      the actualTimes and actualDurations the timeCentroid and exposure.
      In the current design of the TotalPower binary ther is no way to know that these
      container are not present. Idem for the baselineFlags. Hence we drop this part but this
      will have to be recovered when we will support both total power and correlator data in
      this method.
      In this context, timeCentroid and exposure are set equal to time and interval; they are
      retrieve from v_integration.
      NB: fieldId is now retrieved in getData(queryCorrmode)
          interval: idem, from v_integration_
    */



    if(coutest)cout << "Exit getData for SD" << endl;

    return msDataPtr_;
  }


  MSData* SDMBinData::getData( unsigned int na1, unsigned int nfe1, unsigned int na2, unsigned int /*nfe2*/, // comment to avoid the unused parameter warning.
 			       unsigned int ndd, unsigned int nbin, vector<unsigned int> v_napc,
			       float scaleFactor){
    unsigned int nfe=nfe1;                                  // TODO multi-beam

    unsigned int numBin = baselinesSet_->numBin(ndd);                            
    if((nbin+1)>numBin)return 0;

    unsigned int numApc = baselinesSet_->numApc();                               
    for(unsigned int i=0; i<v_napc.size(); i++){
      if((v_napc[i]+1)>numApc){
	Error(FATAL,(char *) "error in the program: apc index exceeds  %d",numApc);
	return 0;
      }
    }

    msDataPtr_ = new MSData;

    msDataPtr_->numData = baselinesSet_->numCrossData(ndd)/(numBin*numApc);  

    msDataPtr_->v_data.resize(v_napc.size());

    // the data (visibilities)

    for(unsigned int n=0; n<v_napc.size(); n++)
      msDataPtr_->v_data[n] = new float[2*msDataPtr_->numData];               // a complex is composed of 2 floats
    //float oneOverSF = 1./scaleFactor;
    int base = 0;

    if(shortDataPtr_){ // case short ints 2 bytes
      for(unsigned int i=0; i<v_napc.size(); i++){
	base = baselinesSet_->transferId(na1,na2,nfe,ndd,nbin,v_napc[i]);
	for(int n=0; n<2*msDataPtr_->numData; n++)
	  msDataPtr_->v_data[i][n] = shortDataPtr_[base+n]/scaleFactor; //oneOverSF * (float)shortDataPtr_[base+n];
      }
    }

    else if (longDataPtr_){   // case ints 4 bytes
      for(unsigned int i=0; i<v_napc.size(); i++){
	base = baselinesSet_->transferId(na1,na2,nfe,ndd,nbin,v_napc[i]);
	for(int n=0; n<2*msDataPtr_->numData; n++)
	  msDataPtr_->v_data[i][n] = longDataPtr_[base+n]/scaleFactor; //oneOverSF * (float)longDataPtr_[base+n];
      }
    }
    else{       // case floats
      for(unsigned int i=0; i<v_napc.size(); i++){
	base = baselinesSet_->transferId(na1,na2,nfe,ndd,nbin,v_napc[i]);
	for(int n=0; n<2*msDataPtr_->numData; n++){
	  msDataPtr_->v_data[i][n] = floatDataPtr_[base+n]/scaleFactor; //oneOverSF * floatDataPtr_[base+n];
	}
      }
    }
  
    return msDataPtr_;
  }

  MSData* SDMBinData::getCalibratedData( unsigned int /*na1*/, unsigned int /*nfe1*/, unsigned int /*na2*/, unsigned int /*nfe2*/,
					 unsigned int /*ndd*/, unsigned int /*nbin*/, vector<unsigned int> /*v_napc*/,
					 float /*scaleFactor*/,
					 pair<bool,vector<vector<float> > > /*p_tsys*/){  // comment to avoid the unused parameter warning.
#if 1
    return 0;
#else
    bool coutest=false;                                   // temporary: to check

    if(coutest)cout<<"scaleFactor="<<scaleFactor<<endl;

    // cout<<"v_napc.size()="<<v_napc.size()<<endl;

    unsigned int nfe=nfe1;                                  // TODO multi-beam

    unsigned int numBin = baselinesSet_->numBin(ndd);                            if(coutest)cout << "numBin=" << numBin << endl;
    if((nbin+1)>numBin)return 0;

    unsigned int numApc = baselinesSet_->numApc();                               if(coutest)cout << "numApc=" << numApc << endl;
    for(unsigned int i=0; i<v_napc.size(); i++){
      if((v_napc[i]+1)>numApc){
	Error(FATAL,"error in the program: apc index exceeds  %d",numApc);
	return 0;
      }
    }


    msDataPtr_ = new MSData;

    msDataPtr_->numData = baselinesSet_->numCrossData(ndd)/(numBin*numApc);  if(coutest)cout << "numCrossData="
											     << baselinesSet_->numCrossData(ndd) << endl;
    msDataPtr_->v_data.resize(v_napc.size());
    // the data (visibilities)
    if(coutest)cout << "About to retrieve " <<  msDataPtr_->numData << " visibilities per dump"<< endl;


    for(unsigned int n=0; n<v_napc.size(); n++)
      msDataPtr_->v_data[n] = new float[2*msDataPtr_->numData];               // a complex is composed of 2 floats

    if(shortDataPtr_){                                                        // case 2 bytes
      for(unsigned int i=0; i<v_napc.size(); i++){
	if(coutest){
	  cout << "2 bytes case: baselinesSet_->transferId(na1="<<na1
	       <<",na2="<<na2<<",ndd="<<ndd<<",nbin="<<nbin<<",napc="<<v_napc[i]<<")="
	       << baselinesSet_->transferId(na1,na2,ndd,nbin,v_napc[i])
	       << endl;
	  cout<<"msDataPtr_->numData="<<msDataPtr_->numData<<" scaleFactor="<<scaleFactor<<endl;
	}
	int np=0;
	for(int n=0; n<2*msDataPtr_->numData; n++){
	  msDataPtr_->v_data[i][n] = p_tsys.second[np%numPol][] *
	    (1./scaleFactor) * (float)shortDataPtr_[baselinesSet_->transferId(na1,na2,nfe,ndd,nbin,v_napc[i])+n];
	}
      }
    }else{   // case 4 bytes
      for(unsigned int i=0; i<v_napc.size(); i++){
	if(coutest)
	  cout << "4 bytes case: baselinesSet_->transferId(na="<<na1<<",na2="<<na2
	       <<",ndd="<<ndd<<",nbin="<<nbin<<",napc="<<v_napc[i]<<")="
	       << baselinesSet_->transferId(na1,na2,ndd,nbin,v_napc[i])
	       << endl;
	if(longDataPtr_){
	  for(int n=0; n<2*msDataPtr_->numData; n++){
	    msDataPtr_->v_data[i][n] =
	      (1./scaleFactor) * (float)longDataPtr_[baselinesSet_->transferId(na1,na2,nfe,ndd,nbin,v_napc[i])+n];
	  }
	}else{
	  for(int n=0; n<2*msDataPtr_->numData; n++){
	    msDataPtr_->v_data[i][n] =
	      (1./scaleFactor) * floatDataPtr_[baselinesSet_->transferId(na1,na2,nfe,ndd,nbin,v_napc[i])+n];
	  }
	}
      }
    }
    if(coutest){
      cout << "numData=" << msDataPtr_->numData << endl;
      for(unsigned int i=0; i<v_napc.size(); i++)
	cout << "1st vis apc=v_napc[i]: re " << msDataPtr_->v_data[i][0]
	     << " im " <<  msDataPtr_->v_data[i][1] << endl;
      cout << " na1=" << na1
	   << " na2=" << na2
	   << " ndd=" << ndd
	   << " napc="; for(unsigned int i=0; i< v_napc.size(); i++)cout<<v_napc[i]<<" ";
      cout << " nbin=" << nbin
	   << endl;
    }
    return msDataPtr_;
#endif
  }



  MSData*          SDMBinData::getData( Tag /*antId*/, int /*feedId*/,
					Tag /*dataDescId*/,
					AtmPhaseCorrection /*apc*/,
					unsigned int  /*binNum*/){ // comment to avoid the unused parameter warning.
    return msDataPtr_;
  }

  MSData*          SDMBinData::getData( Tag /*antId1*/, int /*feedId1*/,
					Tag /*antId2*/, int /*feedId2*/,
					Tag /*dataDescId*/,
					vector<AtmPhaseCorrection> /*v_apc*/,
					unsigned int  /*binNum*/){
    return msDataPtr_;
  }

  vector<pair<unsigned int,double> > SDMBinData::timeSequence()const{
    return v_tci_;
  }

  const VMSData* SDMBinData::getDataCols(Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc){

    if (verbose_) cout << "SDMBinData::getDataCols(Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc) : entering." << endl;

    if(v_msDataPtr_.size()>0){
      if(coutDeleteInfo_)cout << "delete v_msDataPtr_[n]->xData" << endl;
      for(vector<MSData*>::reverse_iterator it=v_msDataPtr_.rbegin(); it!=v_msDataPtr_.rend(); ++it)deleteMsData(*it);
      
      if(vmsDataPtr_){
	if(coutDeleteInfo_)cout<<"about to delete vmsDataPtr_"<<endl;
	for(unsigned int n=0; n<vmsDataPtr_->v_m_data.size(); n++)vmsDataPtr_->v_m_data[n].clear();
	vmsDataPtr_->v_m_data.clear();    // a priori inutile
	delete vmsDataPtr_;
	vmsDataPtr_=NULL;
      }
    }

    v_msDataPtr_.clear();
    v_msDataPtr_ = getData( e_qcm, es_qapc );

    int numRows=v_msDataPtr_.size();
    if(verbose_)cout<<"Number of MS row for this SDM BLOB: " << numRows <<endl;

    if(coutDeleteInfo_&&vmsDataPtr_!=0)cout << "delete vmsDataPtr_" << endl;
    if(vmsDataPtr_!=0)delete vmsDataPtr_;
    vmsDataPtr_ = new VMSData;

    //VMSData* vmsDataPtr_ = new VMSData;
    if(!vmsDataPtr_)
      Error(FATAL,string("Fail to allocate memory for a new VMSData type structure"));


    // the fields of the SDM key shared by all MS rows for this SDM Main entry:
    if(numRows){
      vmsDataPtr_->processorId          = v_msDataPtr_[0]->processorId;
      vmsDataPtr_->binNum               = v_msDataPtr_[0]->binNum;
    }

    unsigned int numApcMax=0;
    for(int n=0; n<numRows; n++){
      if(v_msDataPtr_[n]->v_atmPhaseCorrection.size()>numApcMax)
	numApcMax=v_msDataPtr_[n]->v_atmPhaseCorrection.size();
    }
    if(verbose_)cout<<"numApcMax="<<numApcMax<<endl;

    multimap<int,unsigned int> mm_dd;
    if (ddfirst_) {
      vector<int> ddId_v;
      vector<int>::iterator ddId_i;
      int ddIdOld = -1;
      int ddIndex = -1;
 
      for(unsigned int n=0; n<v_msDataPtr_.size(); n++) {
	if (v_msDataPtr_[n]->dataDescId != ddIdOld) {
	  ddIdOld = v_msDataPtr_[n]->dataDescId ;
	  ddId_i = find(ddId_v.begin(), ddId_v.end(),ddIdOld);
	  if (ddId_i == ddId_v.end()) {
	    ddId_v.push_back(ddIdOld);
	    ddIndex = ddId_v.size()-1;
	  }
	  else 
	    ddIndex = ddId_i - ddId_v.begin();
	}
	mm_dd.insert(make_pair(ddIndex,n));	  
      }
    }

    /*
    multimap<int,unsigned int> mm_dd;
    if(ddfirst_){
      for(unsigned int n=0; n<v_msDataPtr_.size(); n++)
	mm_dd.insert(make_pair(v_msDataPtr_[n]->dataDescId,n));
    }
    */

    v_tci_.clear();  
    v_tci_.resize(numRows);

    if(ddfirst_){   // the dataDescription/time/baseline expansion

      unsigned int n, i=0;
      multimap<int,unsigned int>::const_iterator it;
      for(it=mm_dd.begin(); it!=mm_dd.end(); ++it){
	n=it->second;
	vmsDataPtr_->v_time.push_back(v_msDataPtr_[n]->time);
	vmsDataPtr_->v_fieldId.push_back(v_msDataPtr_[n]->fieldId);
	vmsDataPtr_->v_interval.push_back(v_msDataPtr_[0]->interval);
	vmsDataPtr_->v_antennaId1.push_back(v_msDataPtr_[n]->antennaId1);
	vmsDataPtr_->v_antennaId2.push_back(v_msDataPtr_[n]->antennaId2);
	vmsDataPtr_->v_feedId1.push_back(v_msDataPtr_[n]->feedId1);
	vmsDataPtr_->v_feedId2.push_back(v_msDataPtr_[n]->feedId2);
	vmsDataPtr_->v_dataDescId.push_back(v_msDataPtr_[n]->dataDescId);
	vmsDataPtr_->v_timeCentroid.push_back(v_msDataPtr_[n]->timeCentroid);
	vmsDataPtr_->v_exposure.push_back(v_msDataPtr_[n]->exposure);
	vmsDataPtr_->v_numData.push_back(v_msDataPtr_[n]->numData);
	vmsDataPtr_->vv_dataShape.push_back(v_msDataPtr_[n]->v_dataShape);
	vmsDataPtr_->v_phaseDir.push_back(v_msDataPtr_[n]->phaseDir);
	vmsDataPtr_->v_stateId.push_back(v_msDataPtr_[n]->stateId);
	vmsDataPtr_->v_msState.push_back(v_msDataPtr_[n]->msState);
	vmsDataPtr_->v_flag.push_back(v_msDataPtr_[n]->flag);

	vmsDataPtr_->v_atmPhaseCorrection = v_msDataPtr_[n]->v_atmPhaseCorrection;
	map<AtmPhaseCorrection,float*> m_vdata;
	for(unsigned int napc=0; napc<vmsDataPtr_->v_atmPhaseCorrection.size(); napc++){
	  float* d=v_msDataPtr_[n]->v_data[napc];
	  m_vdata.insert(make_pair(vmsDataPtr_->v_atmPhaseCorrection[napc],d));
	}
	vmsDataPtr_->v_m_data.push_back(m_vdata);
	//cout<<"dataDescriptionId="<<it->first<<" row="<<it->second<<endl;

	pair<unsigned int,double> p=make_pair(i++,v_msDataPtr_[n]->timeCentroid);
	v_tci_[n]=p;
      }

    }else{    // the baseline/dataDescription expansion

      
      for(int n=0; n<numRows; n++){

	vmsDataPtr_->v_time.push_back(v_msDataPtr_[n]->time);
	vmsDataPtr_->v_fieldId.push_back(v_msDataPtr_[n]->fieldId);
	vmsDataPtr_->v_interval.push_back(v_msDataPtr_[0]->interval);
	vmsDataPtr_->v_antennaId1.push_back(v_msDataPtr_[n]->antennaId1);
	vmsDataPtr_->v_antennaId2.push_back(v_msDataPtr_[n]->antennaId2);
	vmsDataPtr_->v_feedId1.push_back(v_msDataPtr_[n]->feedId1);
	vmsDataPtr_->v_feedId2.push_back(v_msDataPtr_[n]->feedId2);
	vmsDataPtr_->v_dataDescId.push_back(v_msDataPtr_[n]->dataDescId);
	vmsDataPtr_->v_timeCentroid.push_back(v_msDataPtr_[n]->timeCentroid);
	vmsDataPtr_->v_exposure.push_back(v_msDataPtr_[n]->exposure);
	vmsDataPtr_->v_numData.push_back(v_msDataPtr_[n]->numData);
	vmsDataPtr_->vv_dataShape.push_back(v_msDataPtr_[n]->v_dataShape);
	vmsDataPtr_->v_phaseDir.push_back(v_msDataPtr_[n]->phaseDir);
	vmsDataPtr_->v_stateId.push_back(v_msDataPtr_[n]->stateId);
	vmsDataPtr_->v_msState.push_back(v_msDataPtr_[n]->msState);
	vmsDataPtr_->v_flag.push_back(v_msDataPtr_[n]->flag);

	vmsDataPtr_->v_atmPhaseCorrection = v_msDataPtr_[n]->v_atmPhaseCorrection;
	map<AtmPhaseCorrection,float*> m_vdata;
	for(unsigned int napc=0; napc<vmsDataPtr_->v_atmPhaseCorrection.size(); napc++){
	  float* d=v_msDataPtr_[n]->v_data[napc];
	  m_vdata.insert(make_pair(vmsDataPtr_->v_atmPhaseCorrection[napc],d));
	}
	vmsDataPtr_->v_m_data.push_back(m_vdata);

	v_tci_[n] = make_pair(n,v_msDataPtr_[n]->timeCentroid);

      }

    }

    detachDataObject();
    if (verbose_) cout << "SDMBinData::getDataCols(Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc) : exiting." << endl;
    return vmsDataPtr_;
  }


  const VMSData* SDMBinData::getDataCols(){
    if (verbose_) cout << "SDMBinData::getDataCols() : entering." << endl; 
    Enum<CorrelationMode>       e_qcm;
    EnumSet<AtmPhaseCorrection> es_qapc;
    if(canSelect_){
      //cout<<"INFORM: context allow to select"<<endl;
      e_qcm   = e_qcm_;
      es_qapc = es_qapc_;
    }
    const VMSData* result = getDataCols( e_qcm, es_qapc );
    if (verbose_) cout << "SDMBinData::getDataCols() : exiting." << endl; 
    return result;
  }

  void  SDMBinData::deleteMsData(MSData* msDataPtr){
    if(msDataPtr){
      for(unsigned int i=0; i<msDataPtr->v_data.size(); i++){
	if(msDataPtr->v_data[i]){
	  delete[] msDataPtr->v_data[i];
	  //if(coutDeleteInfo_)cout << "data in MS data object deleted" << endl;
	  msDataPtr->v_data[i] = 0;
	}
      }

      delete msDataPtr;   //  if(coutDeleteInfo_)cout<<"MS data object deleted"<<endl;
      msDataPtr=0;
    }
    return;
  }

  const VMSData* SDMBinData::getNextMSMainCols(unsigned int nDataSubset) {
    Enum<CorrelationMode>       e_qcm;
    EnumSet<AtmPhaseCorrection> es_qapc;
    if(canSelect_){
      cout<<"INFORM: context allow to select"<<endl;
      e_qcm   = e_qcm_;
      es_qapc = es_qapc_;
    }
    return getNextMSMainCols( e_qcm, es_qapc, nDataSubset);
  }

  const VMSData* SDMBinData::getNextMSMainCols(Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc, unsigned int nDataSubset) {
    if (verbose_) cout << "SDMBinData::getNextMSMainCols : entering" << endl;

    // Delete the content of v_msDataPtr if any
    if ( v_msDataPtr_.size() > 0 ){
      if(verbose_)cout << "delete v_msDataPtr_[n]->xData" << endl;
      for(vector<MSData*>::reverse_iterator it=v_msDataPtr_.rbegin(); it!=v_msDataPtr_.rend(); ++it)deleteMsData(*it);
      
      if(vmsDataPtr_){
	if(verbose_)cout<<"about to delete vmsDataPtr_"<<endl;
	for(unsigned int n=0; n<vmsDataPtr_->v_m_data.size(); n++)
	  vmsDataPtr_->v_m_data[n].clear();
	vmsDataPtr_->v_m_data.clear();    // a priori inutile
	delete vmsDataPtr_;
	vmsDataPtr_=NULL;
      }
    }

    v_msDataPtr_.clear(); // I doubt this is really useful ... (MC 08/12/2011)
    
    // No more data available... then return empty content.
    if  (!sdmdosr.hasSubset()) {
      return NULL;
    }

    v_msDataPtr_ = getMSDataFromBDFData(e_qcm, es_qapc, nDataSubset);
    int numRows = v_msDataPtr_.size();
    if (verbose_) cout << "Number of MS Main rows for this block of " << nDataSubset << " [sub]integrations in '" << sdmdosr.dataOID() << "' = " << numRows << endl; 

    if(verbose_&&vmsDataPtr_!=0) cout << "delete vmsDataPtr_" << endl;
    if(vmsDataPtr_!=0)delete vmsDataPtr_;
    vmsDataPtr_ = new VMSData;

    if(!vmsDataPtr_)
      Error(FATAL, string("Fail to allocate memory for a new VMSData type structure"));

   if(numRows){
      vmsDataPtr_->processorId          = v_msDataPtr_[0]->processorId;
      vmsDataPtr_->binNum               = v_msDataPtr_[0]->binNum;
    }

    unsigned int numApcMax=0;
    for(int n=0; n<numRows; n++){
      if(v_msDataPtr_[n]->v_atmPhaseCorrection.size()>numApcMax)
	numApcMax=v_msDataPtr_[n]->v_atmPhaseCorrection.size();
    }
    if(verbose_)cout<<"numApcMax="<<numApcMax<<endl;

    multimap<int,unsigned int> mm_dd;
    if (ddfirst_) {
      vector<int> ddId_v;
      vector<int>::iterator ddId_i;
      int ddIdOld = -1;
      int ddIndex = -1;
 
      for(unsigned int n=0; n<v_msDataPtr_.size(); n++) {
	if (v_msDataPtr_[n]->dataDescId != ddIdOld) {
	  ddIdOld = v_msDataPtr_[n]->dataDescId ;
	  if ((ddId_i = find(ddId_v.begin(), ddId_v.end(),ddIdOld)) == ddId_v.end()) {
	    ddId_v.push_back(ddIdOld);
	    ddIndex = ddId_v.size()-1;
	  }
	  else 
	    ddIndex = ddId_i - ddId_v.begin();
	}
	mm_dd.insert(make_pair(ddIndex,n));	  
      }
    }
/*
    multimap<int,unsigned int> mm_dd;
    if(ddfirst_){
      for(unsigned int n=0; n<v_msDataPtr_.size(); n++)
	mm_dd.insert(make_pair(v_msDataPtr_[n]->dataDescId,n));
    }
*/
    v_tci_.clear();  
    v_tci_.resize(numRows);

    if(ddfirst_){   // the dataDescription/time/baseline expansion

      unsigned int n, i=0;
      multimap<int,unsigned int>::const_iterator it;
      for(it=mm_dd.begin(); it!=mm_dd.end(); ++it){
	n=it->second;
	vmsDataPtr_->v_time.push_back(v_msDataPtr_[n]->time);
	vmsDataPtr_->v_fieldId.push_back(v_msDataPtr_[n]->fieldId);
	vmsDataPtr_->v_interval.push_back(v_msDataPtr_[0]->interval);
	vmsDataPtr_->v_antennaId1.push_back(v_msDataPtr_[n]->antennaId1);
	vmsDataPtr_->v_antennaId2.push_back(v_msDataPtr_[n]->antennaId2);
	vmsDataPtr_->v_feedId1.push_back(v_msDataPtr_[n]->feedId1);
	vmsDataPtr_->v_feedId2.push_back(v_msDataPtr_[n]->feedId2);
	vmsDataPtr_->v_dataDescId.push_back(v_msDataPtr_[n]->dataDescId);
	vmsDataPtr_->v_timeCentroid.push_back(v_msDataPtr_[n]->timeCentroid);
	vmsDataPtr_->v_exposure.push_back(v_msDataPtr_[n]->exposure);
	vmsDataPtr_->v_numData.push_back(v_msDataPtr_[n]->numData);
	vmsDataPtr_->vv_dataShape.push_back(v_msDataPtr_[n]->v_dataShape);
	vmsDataPtr_->v_phaseDir.push_back(v_msDataPtr_[n]->phaseDir);
	vmsDataPtr_->v_stateId.push_back(v_msDataPtr_[n]->stateId);
	vmsDataPtr_->v_msState.push_back(v_msDataPtr_[n]->msState);
	vmsDataPtr_->v_flag.push_back(v_msDataPtr_[n]->flag);

	vmsDataPtr_->v_atmPhaseCorrection = v_msDataPtr_[n]->v_atmPhaseCorrection;
	map<AtmPhaseCorrection,float*> m_vdata;
	for(unsigned int napc=0; napc<vmsDataPtr_->v_atmPhaseCorrection.size(); napc++){
	  float* d=v_msDataPtr_[n]->v_data[napc];
	  m_vdata.insert(make_pair(vmsDataPtr_->v_atmPhaseCorrection[napc],d));
	}
	vmsDataPtr_->v_m_data.push_back(m_vdata);
	//cout<<"dataDescriptionId="<<it->first<<" row="<<it->second<<endl;

	pair<unsigned int,double> p=make_pair(i++,v_msDataPtr_[n]->timeCentroid);
	v_tci_[n]=p;
      }

    }else{    // the baseline/dataDescription expansion

      
      for(int n=0; n<numRows; n++){

	vmsDataPtr_->v_time.push_back(v_msDataPtr_[n]->time);
	vmsDataPtr_->v_fieldId.push_back(v_msDataPtr_[n]->fieldId);
	vmsDataPtr_->v_interval.push_back(v_msDataPtr_[0]->interval);
	vmsDataPtr_->v_antennaId1.push_back(v_msDataPtr_[n]->antennaId1);
	vmsDataPtr_->v_antennaId2.push_back(v_msDataPtr_[n]->antennaId2);
	vmsDataPtr_->v_feedId1.push_back(v_msDataPtr_[n]->feedId1);
	vmsDataPtr_->v_feedId2.push_back(v_msDataPtr_[n]->feedId2);
	vmsDataPtr_->v_dataDescId.push_back(v_msDataPtr_[n]->dataDescId);
	vmsDataPtr_->v_timeCentroid.push_back(v_msDataPtr_[n]->timeCentroid);
	vmsDataPtr_->v_exposure.push_back(v_msDataPtr_[n]->exposure);
	vmsDataPtr_->v_numData.push_back(v_msDataPtr_[n]->numData);
	vmsDataPtr_->vv_dataShape.push_back(v_msDataPtr_[n]->v_dataShape);
	vmsDataPtr_->v_phaseDir.push_back(v_msDataPtr_[n]->phaseDir);
	vmsDataPtr_->v_stateId.push_back(v_msDataPtr_[n]->stateId);
	vmsDataPtr_->v_msState.push_back(v_msDataPtr_[n]->msState);
	vmsDataPtr_->v_flag.push_back(v_msDataPtr_[n]->flag);

	vmsDataPtr_->v_atmPhaseCorrection = v_msDataPtr_[n]->v_atmPhaseCorrection;
	map<AtmPhaseCorrection,float*> m_vdata;
	for(unsigned int napc=0; napc<vmsDataPtr_->v_atmPhaseCorrection.size(); napc++){
	  float* d=v_msDataPtr_[n]->v_data[napc];
	  m_vdata.insert(make_pair(vmsDataPtr_->v_atmPhaseCorrection[napc],d));
	}
	vmsDataPtr_->v_m_data.push_back(m_vdata);

	v_tci_[n] = make_pair(n,v_msDataPtr_[n]->timeCentroid);
      }
    }
    if (verbose_) cout << "SDMBinData::getNextMSMainCols : exiting" << endl; 
    return vmsDataPtr_;
  }

  // shared_ptr not needed by CASA, do not compile this for the WITHOUT_BOOST option
  // It may be needed by ALMA code, so do not elimiate it yet.
#ifndef WITHOUT_BOOST
  void SDMBinData::getNextMSMainCols(unsigned int nDataSubset, boost::shared_ptr<VMSDataWithSharedPtr>& vmsData_p_sp) {
    Enum<CorrelationMode>       e_qcm;
    EnumSet<AtmPhaseCorrection> es_qapc;
    if(canSelect_){
      cout<<"INFORM: context allow to select"<<endl;
      e_qcm   = e_qcm_;
      es_qapc = es_qapc_;
    }
    getNextMSMainCols( e_qcm, es_qapc, nDataSubset, vmsData_p_sp );
  }

  void  SDMBinData::getNextMSMainCols(Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc, unsigned int nDataSubset,  boost::shared_ptr<VMSDataWithSharedPtr>& vmsData_p_sp ) {
    if (verbose_) cout << "SDMBinData::getNextMSMainCols (with VMSDataSharedPtr) : entering" << endl;

    VMSDataWithSharedPtr* vmsData_p = vmsData_p_sp.get();

    // Delete the content of v_msDataPtr (but do not delete "deeply" i.e. do not delete the visibilities referred to by pointers stored into v_msDataPtr since the 
    // memory they occupy is going to be managed by boost::shared_ptr s.
    //
    if ( v_msDataPtr_.size() > 0 ){
      for(vector<MSData*>::reverse_iterator it=v_msDataPtr_.rbegin(); it!=v_msDataPtr_.rend(); ++it) delete (*it);
    }
    v_msDataPtr_.clear();

    v_msDataPtr_ = getMSDataFromBDFData(e_qcm, es_qapc, nDataSubset);
    int numRows = v_msDataPtr_.size();
    if (verbose_) cout << "Number of MS Main rows for this block of " << nDataSubset << " [sub]integrations in '" << sdmdosr.dataOID() << "' = " << numRows << endl; 

   if(numRows){
      vmsData_p->processorId          = v_msDataPtr_[0]->processorId;
      vmsData_p->binNum               = v_msDataPtr_[0]->binNum;
    }

    unsigned int numApcMax=0;
    for(int n=0; n<numRows; n++){
      if(v_msDataPtr_[n]->v_atmPhaseCorrection.size()>numApcMax)
	numApcMax=v_msDataPtr_[n]->v_atmPhaseCorrection.size();
    }
    if(verbose_)cout<<"numApcMax="<<numApcMax<<endl;

    multimap<int,unsigned int> mm_dd;
    if (ddfirst_) {
      vector<int> ddId_v;
      vector<int>::iterator ddId_i;
      int ddIdOld = -1;
      int ddIndex = -1;
 
      for(unsigned int n=0; n<v_msDataPtr_.size(); n++) {
	if (v_msDataPtr_[n]->dataDescId != ddIdOld) {
	  ddIdOld = v_msDataPtr_[n]->dataDescId ;
	  if ((ddId_i = find(ddId_v.begin(), ddId_v.end(),ddIdOld)) == ddId_v.end()) {
	    ddId_v.push_back(ddIdOld);
	    ddIndex = ddId_v.size()-1;
	  }
	  else 
	    ddIndex = ddId_i - ddId_v.begin();
	}
	mm_dd.insert(make_pair(ddIndex,n));	  
      }
    }


    /*
    multimap<int,unsigned int> mm_dd;
    if(ddfirst_){
      for(unsigned int n=0; n<v_msDataPtr_.size(); n++)
	mm_dd.insert(make_pair(v_msDataPtr_[n]->dataDescId,n));
    }
    */

    v_tci_.clear();  
    v_tci_.resize(numRows);

    if(ddfirst_){   // the dataDescription/time/baseline expansion

      unsigned int n, i=0;
      multimap<int,unsigned int>::const_iterator it;
      for(it=mm_dd.begin(); it!=mm_dd.end(); ++it){
	n=it->second;
	vmsData_p->v_time.push_back(v_msDataPtr_[n]->time);
	vmsData_p->v_fieldId.push_back(v_msDataPtr_[n]->fieldId);
	vmsData_p->v_interval.push_back(v_msDataPtr_[0]->interval);
	vmsData_p->v_antennaId1.push_back(v_msDataPtr_[n]->antennaId1);
	vmsData_p->v_antennaId2.push_back(v_msDataPtr_[n]->antennaId2);
	vmsData_p->v_feedId1.push_back(v_msDataPtr_[n]->feedId1);
	vmsData_p->v_feedId2.push_back(v_msDataPtr_[n]->feedId2);
	vmsData_p->v_dataDescId.push_back(v_msDataPtr_[n]->dataDescId);
	vmsData_p->v_timeCentroid.push_back(v_msDataPtr_[n]->timeCentroid);
	vmsData_p->v_exposure.push_back(v_msDataPtr_[n]->exposure);
	vmsData_p->v_numData.push_back(v_msDataPtr_[n]->numData);
	vmsData_p->vv_dataShape.push_back(v_msDataPtr_[n]->v_dataShape);
	vmsData_p->v_phaseDir.push_back(v_msDataPtr_[n]->phaseDir);
	vmsData_p->v_stateId.push_back(v_msDataPtr_[n]->stateId);
	vmsData_p->v_msState.push_back(v_msDataPtr_[n]->msState);
	vmsData_p->v_flag.push_back(v_msDataPtr_[n]->flag);

	vmsData_p->v_atmPhaseCorrection = v_msDataPtr_[n]->v_atmPhaseCorrection;
	map<AtmPhaseCorrection,boost::shared_array<float> > m_vdata;
	for(unsigned int napc=0; napc<vmsData_p->v_atmPhaseCorrection.size(); napc++){
	  float* d=v_msDataPtr_[n]->v_data[napc];
	  boost::shared_array<float> d_sp(d);
	  m_vdata.insert(make_pair(vmsData_p->v_atmPhaseCorrection[napc],d_sp));
	}
	vmsData_p->v_m_data.push_back(m_vdata);
	//cout<<"dataDescriptionId="<<it->first<<" row="<<it->second<<endl;

	pair<unsigned int,double> p=make_pair(i++,v_msDataPtr_[n]->timeCentroid);
	v_tci_[n]=p;
      }

    }else{    // the baseline/dataDescription expansion

      
      for(int n=0; n<numRows; n++){

	vmsData_p->v_time.push_back(v_msDataPtr_[n]->time);
	vmsData_p->v_fieldId.push_back(v_msDataPtr_[n]->fieldId);
	vmsData_p->v_interval.push_back(v_msDataPtr_[0]->interval);
	vmsData_p->v_antennaId1.push_back(v_msDataPtr_[n]->antennaId1);
	vmsData_p->v_antennaId2.push_back(v_msDataPtr_[n]->antennaId2);
	vmsData_p->v_feedId1.push_back(v_msDataPtr_[n]->feedId1);
	vmsData_p->v_feedId2.push_back(v_msDataPtr_[n]->feedId2);
	vmsData_p->v_dataDescId.push_back(v_msDataPtr_[n]->dataDescId);
	vmsData_p->v_timeCentroid.push_back(v_msDataPtr_[n]->timeCentroid);
	vmsData_p->v_exposure.push_back(v_msDataPtr_[n]->exposure);
	vmsData_p->v_numData.push_back(v_msDataPtr_[n]->numData);
	vmsData_p->vv_dataShape.push_back(v_msDataPtr_[n]->v_dataShape);
	vmsData_p->v_phaseDir.push_back(v_msDataPtr_[n]->phaseDir);
	vmsData_p->v_stateId.push_back(v_msDataPtr_[n]->stateId);
	vmsData_p->v_msState.push_back(v_msDataPtr_[n]->msState);
	vmsData_p->v_flag.push_back(v_msDataPtr_[n]->flag);

	vmsData_p->v_atmPhaseCorrection = v_msDataPtr_[n]->v_atmPhaseCorrection;
	map<AtmPhaseCorrection, boost::shared_array<float> > m_vdata;
	for(unsigned int napc=0; napc<vmsData_p->v_atmPhaseCorrection.size(); napc++){
	  float* d=v_msDataPtr_[n]->v_data[napc];
	  boost::shared_array<float> d_sp(d);
	  m_vdata.insert(make_pair(vmsData_p->v_atmPhaseCorrection[napc],d_sp));
	}
	vmsData_p->v_m_data.push_back(m_vdata);

	v_tci_[n] = make_pair(n,v_msDataPtr_[n]->timeCentroid);
      }
    }
    if (verbose_) cout << "SDMBinData::getNextMSMainCols : exiting" << endl; 
  }
#endif

  vector<MSData*> SDMBinData::getMSDataFromBDFData(Enum<CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrection> es_qapc, unsigned int nDataSubsets) {
    if (verbose_) cout << "SDMBinData::getMSDataFromBDFData: entering (e_qcm="<<e_qcm.str()
		      <<",es_qapc="<<es_qapc.str()
		      <<", nDataSubsets="<< nDataSubsets 
		      <<")"
		      << endl;

    MSData* msDataPtr_ = (MSData *) NULL;
    
    if (v_msDataPtr_.size() > 0) {
      if (verbose_) cout<<"About to delete "<<v_msDataPtr_.size()<<" msDataPtr objects"<<endl;
      for(vector<MSData*>::reverse_iterator it=v_msDataPtr_.rbegin(); it!=v_msDataPtr_.rend(); ++it)
	deleteMsData(*it);
      
      v_msDataPtr_.clear();
    }

    ConfigDescriptionRow*       cdPtr = mainRowPtr_->getConfigDescriptionUsingConfigDescriptionId();
    vector<AtmPhaseCorrection>  v_apc = cdPtr->getAtmPhaseCorrection();
    Enum<CorrelationMode>       e_cm; e_cm = cdPtr->getCorrelationMode();

    if(!canSelect_ && (e_qcm.count() || es_qapc.count()) ){
      Error(FATAL, string("This method cannot be used in this context!\n Use the method with no argument getData()"));
      return v_msDataPtr_;
    }

    if(canSelect_){
      // set the new defaults
      e_qcm_   = e_qcm;
      es_qapc_ = es_qapc;
    }

    if (verbose_) {
      cout<<"e_qcm=  "<<e_qcm.str()  <<" e_qcm_=  "<<e_qcm_.str()  <<" e_cm="<<e_cm.str()<<endl;
      cout<<"es_qapc="<<es_qapc.str()<<" es_qapc_="<<es_qapc_.str();
      for(unsigned int n=0; n<v_apc.size(); n++)
	cout<<" v_apc["<<n<<"]="<<Enum<AtmPhaseCorrection>(v_apc[n]).str()<<" ";
      cout<<endl;
    }

    if(e_qcm_[CROSS_ONLY]) if(e_cm[AUTO_ONLY])  return v_msDataPtr_;
    if(e_qcm_[AUTO_ONLY])  if(e_cm[CROSS_ONLY]) return v_msDataPtr_;

    vector<unsigned int>       v_selected_napc;
    vector<AtmPhaseCorrection> v_selected_apc;
    for(unsigned int n=0; n<v_apc.size(); n++){
      if(es_qapc_[v_apc[n]]){
	v_selected_napc.push_back(n);
	v_selected_apc.push_back(v_apc[n]);
      }
    }
    if(!v_selected_napc.size())return v_msDataPtr_;

    Tag configDescriptionId = mainRowPtr_->getConfigDescriptionId();
    std::set<Tag>::iterator
      itsf=s_cdId_.find(configDescriptionId),
      itse=s_cdId_.end();
    if(itsf==itse)
      Error(FATAL, string("Tree hierarchy not present for configDescId ") + configDescriptionId.toString());
    std::map<Tag,BaselinesSet*>::iterator
      itf(m_cdId_baselinesSet_.find(configDescriptionId)),
      ite(m_cdId_baselinesSet_.end());
    if(itf==ite)
      Error(FATAL, string("Tree hierarchy not present for configDescId ") + configDescriptionId.toString());
    baselinesSet_=itf->second;

    if (verbose_) cout<<"ConfigDescriptionId = " << configDescriptionId.toString()<<endl;
//    unsigned int               stateId;
    vector<StateRow*>          v_statePtr = mainRowPtr_->getStatesUsingStateId();

    vector<int>                v_feedSet  = cdPtr->getFeedId();
    vector<Tag>                v_antSet   = cdPtr->getAntennaId();
    vector<Tag>                v_ddList   = cdPtr->getDataDescriptionId();


    unsigned int               numFeed    = v_feedSet.size()/v_antSet.size();
    unsigned int               numBin;
    unsigned int               numPol;
    vector<unsigned int>       v_dataShape;  v_dataShape.resize(3);  // POL.SPP.APC 3D

    double                     timeMJD;
    double                     timeCentroidMJD;
    double                     interval;
    double                     exposure;
    ArrayTime                  timeOfDump;
    ArrayTime                  timeCentroidOfDump;

    int                        fieldId    = mainRowPtr_->getFieldUsingFieldId()->getFieldId().getTagValue();
    vector<vector<Angle> >     phaseDir   = mainRowPtr_->getFieldUsingFieldId()->getPhaseDir();

    
    vector<Tag>                v_stateId;

    if ( verbose_ ) cout <<"ici AA: e_qcm_="<<e_qcm_.str()<<endl;

    for(unsigned int na=0; na<v_antSet.size(); na++)
      v_stateId.push_back(v_statePtr[na]->getStateId());

    vector<vector<float> >                      vv_t; // dummy empty vector would there be no tsysSpectrum for a given syscal row
    SysCalTable&                                sct = datasetPtr_->getSysCal();
    vector<Tag>                                 v_spwid;
    vector<pair<bool,vector<vector<float> > > > v_tsys;
    vector<vector<pair<bool,vector<vector<float> > > > > vv_tsys;

    if(SDMBinData::syscal_){
      v_tsys.resize(v_antSet.size()*v_ddList.size()*numFeed);
      for(unsigned int nt=0; nt<v_dataDump_.size(); nt++){
	vector<pair<bool,vector<vector<float> > > > v_tsys;
	timeCentroidOfDump= ArrayTime((int64_t)v_dataDump_[nt]->timeCentroid());
	unsigned int scn=0;
	for(unsigned int na=0; na<v_antSet.size(); na++){
	  for(unsigned int nfe=0; nfe<numFeed; nfe++){
	    for(unsigned int nsw=0; nsw<v_ddList.size(); nsw++){
	      SysCalRow* scr = sct.getRowByKey( v_antSet[na],
						baselinesSet_->getSpwId(nsw),
						ArrayTimeInterval(timeCentroidOfDump),
						v_feedSet[na*numFeed+nfe]
						);
	      if(scr->isTsysSpectrumExists()){
		vector<vector<float> > vv_flt;
		v_tsys[scn].first  = scr->getTsysFlag();
		vector<vector<Temperature> > vv=scr->getTsysSpectrum();
		for(unsigned int np=0; np<vv.size(); np++){
		  vector<float> v;
		  for(unsigned int ns=0; ns<vv[np].size(); ns++)v.push_back(vv[np][ns].get());
		  vv_flt.push_back(v);
		}
		v_tsys[scn].second = vv_flt;
	      }else{
		v_tsys[scn].first  = false;
		v_tsys[scn].second = vv_t;
	      }
	      scn++;
	    }
	  }
	}
	vv_tsys.push_back(v_tsys);
      }
    }

    // Okay now it's time to obtain at most n dataDumps.
    getNextDataDumps(nDataSubsets);

    // And now use what's in v_dataDump.
    if(e_cm[CROSS_ONLY]==false && e_qcm_[CROSS_ONLY]==false ){       // retrieve only AUTO_DATA
      for(unsigned int nt=0; nt<v_dataDump_.size(); nt++){
	timeOfDump        = ArrayTime((int64_t)v_dataDump_[nt]->time());               if(verbose_)cout<<timeOfDump<<" ns"<<endl;
	timeCentroidOfDump= ArrayTime((int64_t)v_dataDump_[nt]->timeCentroid());       if(verbose_)cout<<timeCentroidOfDump.toString()<<" ns"<<endl;
	timeMJD           = timeOfDump.getMJD();                              if(verbose_)cout<<timeMJD<<" h = "<<86400.*timeMJD<<" s"<<endl;
	interval          = (double)v_dataDump_[nt]->interval()/1000000000LL; if(verbose_)cout<<interval<<" s"<<endl;
	timeCentroidMJD   = timeCentroidOfDump.getMJD();                      if(verbose_)cout<< timeCentroidMJD<<endl;
	exposure          = (double)v_dataDump_[nt]->exposure()/1000000000LL;
	floatDataDumpPtr_ = v_dataDump_[nt]->autoData();             // used by getData(na, nfe, ndd, nbi)
	
	if(verbose_)cout <<"ici BB"<<endl;
	
	vector<AtmPhaseCorrection> v_uapc; v_uapc.push_back(AP_UNCORRECTED);
	
	unsigned int scn=0;
	for(unsigned int na=0; na<v_antSet.size(); na++){
//	  stateId = v_statePtr[na]->getStateId().getTagValue();
	  for(unsigned int nfe=0; nfe<numFeed; nfe++){
	    for(unsigned int ndd=0; ndd<v_ddList.size(); ndd++){
	      numBin = baselinesSet_->numBin(ndd);
	      numPol = baselinesSet_->numPol(baselinesSet_->getBasebandIndex(ndd));
	      if(numPol==4)numPol=3;                                 // if XX,XY,YX,YY then auto XX,XY,YY 
	      if(verbose_)cout<<"numPol="<<numPol
			     <<"  na="  <<na
			     <<" ant="  <<v_antSet[na].getTagValue()
			     <<endl;
	      v_dataShape[0]=numPol;
	      //	      if(complexData_)v_dataShape[0]*=2;
	      v_dataShape[1]=baselinesSet_->numChan(ndd);
	      v_dataShape[2]=1;                                     // auto-correlation never atm phase corrected
	      if (verbose_) cout << "CC" << endl;
	      for(unsigned int nbi=0; nbi<numBin; nbi++){
		// the data and binary meta-data
		if(SDMBinData::syscal_)
		  msDataPtr_ = getCalibratedData( na, nfe,
						  ndd, nbi, 
						  vv_tsys[nt][scn]);
		else
		  msDataPtr_ = getData( na, nfe,
					ndd, nbi);
		scn++;
		msDataPtr_->timeCentroid   = 86400.*timeCentroidMJD; // default value would there be no bin actualTimes
		msDataPtr_->exposure       = exposure;               // default value would there be no bin actualDurations
		msDataPtr_->flag           = 0;                      // default value is "false"  would there be no bin flags
		// calibrate and associate SDM meta-data
		if(msDataPtr_->numData>0){
		  msDataPtr_->processorId        = cdPtr->getProcessorId().getTagValue();
		  msDataPtr_->antennaId1         = v_antSet[na].getTagValue();
		  msDataPtr_->antennaId2         = v_antSet[na].getTagValue();
		  msDataPtr_->feedId1            = v_feedSet[na*numFeed+nfe];
		  msDataPtr_->feedId2            = v_feedSet[na*numFeed+nfe];
		  // if(e_cm[CROSS_AND_AUTO])
		  //   msDataPtr_->dataDescId       = baselinesSet_->getAutoDataDescriptionId(v_ddList[ndd]).getTagValue();
		  // else
		  //   msDataPtr_->dataDescId       = v_ddList[ndd].getTagValue();
		  msDataPtr_->dataDescId         = v_ddList[ndd].getTagValue();   // This comes in place of the 4 commented statements above,
		                                                                  // in order to avoid that auto data and cross data of one same 
                                                                                  // integration appear with different data description id in the 
                                                                                  // case when two different data descriptions are declared while
                                                                                  // would be enough (typically one for AUTO : XX, XY, YY one for 
		                                                                  // CROSS : XX XY YX YY) while in my view CROSS : XX, XY, YX, YY 
                                                                                  // *implicitely* means AUTo : XX, XY, YY. 
		                                                                  // The motivation for this is to keep as simple as possible
		                                                                  // the logic by which CROSS DATA will follow AUTO DATA in the resulting MS
                                                                                  // with a UNIQUE (MS) data description id. Michel Caillat 7th of Sep, 2015.

		  msDataPtr_->phaseDir           = phaseDir;                     // TODO Apply the polynomial formula
		  /*
		  msDataPtr_->stateId            = stateId;
		  msDataPtr_->msState            = getMSState( subscanNum, v_stateId,
							       v_antSet, v_feedSet, v_ddList,
							       na, nfe, ndd, timeOfDump);
		  */
		  msDataPtr_->v_dataShape        = v_dataShape;
		  msDataPtr_->time               = 86400.*timeMJD;
		  msDataPtr_->interval           = interval;
		  msDataPtr_->v_atmPhaseCorrection = v_uapc;         // It is assumed that this is always the uncorrected case
		  msDataPtr_->binNum             = nbi+1;
		  msDataPtr_->fieldId            = fieldId;
		  vector<unsigned int> v_projectNodes;
		  if(v_dataDump_[nt]->integrationNum())v_projectNodes.push_back(v_dataDump_[nt]->integrationNum());
		  if(v_dataDump_[nt]->subintegrationNum())v_projectNodes.push_back(v_dataDump_[nt]->subintegrationNum());
		  msDataPtr_->projectPath        = v_projectNodes;
		  v_msDataPtr_.push_back(msDataPtr_);                      // store in vector for export
		}
		if(verbose_)cout << "B/ msDataPtr_->numData=" <<msDataPtr_->numData << endl;
	      }
	    }
	  }
	}
      }
    }
    if(verbose_)cout <<"ici CC: "<<e_qcm_.str()<<endl;
    vector<AtmPhaseCorrection> v_atmPhaseCorrection = cdPtr->getAtmPhaseCorrection();
    bool queryCrossData = false;
    if(e_qcm_[CROSS_ONLY])     queryCrossData=true;
    if(e_qcm_[CROSS_AND_AUTO]) queryCrossData=true;

    if(e_cm[AUTO_ONLY]==false && queryCrossData ){
      
      // select the queried apc
      vector<unsigned int> v_napc;
      EnumSet<AtmPhaseCorrection> es_apc; es_apc.set(v_atmPhaseCorrection);    if(verbose_)cout<<es_apc.str()<<endl;
      if(verbose_)cout <<"es_apc from BLOB: " << es_apc.str()   << endl;
      if(verbose_)cout <<"es_qapc_ queried: " << es_qapc_.str() << endl;
      for(unsigned int napc=0; napc<v_atmPhaseCorrection.size(); napc++)
	if(es_qapc_[v_atmPhaseCorrection[napc]])v_napc.push_back(napc);
      if(verbose_)for(unsigned int n=0; n<v_napc.size(); n++)cout<<"v_napc["<<n<<"]="<<v_napc[n]<<endl;
      
      if(!v_napc.size()){
	Error(WARNING,(char *) "No visibilities with AtmPhaseCorrection in the set {%s}",
	      es_qapc_.str().c_str());
	return v_msDataPtr_;
      }
      
      if(verbose_)cout<<"ici DD"<<endl;
      
      
      for(unsigned int nt=0; nt<v_dataDump_.size(); nt++){
	timeOfDump        = ArrayTime((int64_t)v_dataDump_[nt]->time());                if(verbose_)cout<<timeOfDump<<endl;
	timeCentroidOfDump= ArrayTime((int64_t)v_dataDump_[nt]->timeCentroid());        if(verbose_)cout<< timeCentroidOfDump.toString() <<endl;
	timeMJD           = timeOfDump.getMJD();                               if(verbose_)cout<<timeMJD<<" h"<<endl;
	interval          = (double)v_dataDump_[nt]->interval()/1000000000LL;
	timeCentroidMJD   = timeCentroidOfDump.getMJD();                       if(verbose_)cout<< timeCentroidMJD<<" h"<<endl;
	exposure          = (double)v_dataDump_[nt]->exposure()/1000000000LL;;
	if(verbose_)cout<<"ici DD 1"<<endl;
	shortDataPtr_ = NULL;
	longDataPtr_  = NULL;
	floatDataPtr_ = NULL;
	if(v_dataDump_[nt]->crossDataShort())
	  shortDataPtr_   = v_dataDump_[nt]->crossDataShort();       // used by getData(na, na2, nfe, ndd, nbi, napc)
	else if(v_dataDump_[nt]->crossDataLong())
	  longDataPtr_    = v_dataDump_[nt]->crossDataLong();
	else if(v_dataDump_[nt]->crossDataFloat())
	  floatDataPtr_    = v_dataDump_[nt]->crossDataFloat();
	else
	  Error(FATAL, string("Cross data typed float not yet supported"));
	if(verbose_)cout<<"ici DD 2"<<endl;
	unsigned int scn=0;
	for(unsigned int na1=0; na1<v_antSet.size(); na1++){
	  // we will assume that na2 has the same stateId (perhaps we should return a vector?)!
 //	  stateId = v_statePtr[na1]->getStateId().getTagValue();
	  for(unsigned int na2=na1+1; na2<v_antSet.size(); na2++){
	    for(unsigned int nfe=0; nfe<numFeed; nfe++){
	      for(unsigned int ndd=0; ndd<v_ddList.size(); ndd++){
		if(verbose_)cout<<"ici DD 3   numApc="<<baselinesSet_->numApc()<<endl;
		numBin = baselinesSet_->numBin(ndd);
		numPol = baselinesSet_->numPol(baselinesSet_->getBasebandIndex(ndd));   // nb of pol product (max is 4)
		if(verbose_)cout<<"ici DD 4   numBin="<<numBin<<"  numPol="<<numPol<<endl;
		v_dataShape[0]=baselinesSet_->numPol(ndd);
		v_dataShape[1]=baselinesSet_->numChan(ndd);
		v_dataShape[2]=baselinesSet_->numApc();
		v_dataShape[2]=1;               // qapc being not an EnumSet (MS limitation for floatData column)
		for(unsigned int nbi=0; nbi<numBin; nbi++){
		  if(verbose_){
		    cout<<"timeCentroidMJD="<<timeCentroidMJD<<endl;
		  }
		  // the data and binary meta-data
		  if (verbose_) {
		    cout << "nt = " << nt << endl;
		    cout << "size of v_dataDump_ = " << v_dataDump_.size() << endl;
		  }
		  
		  if(SDMBinData::syscal_) {
		    msDataPtr_ = getCalibratedData( na1, nfe, na2, nfe,
						    ndd,  nbi, v_napc,
						    v_dataDump_[nt]->scaleFactor(ndd),
						    vv_tsys[nt][scn]);
		  }
		  else {
		    msDataPtr_ = getData( na1, nfe, na2, nfe,
					  ndd,  nbi, v_napc,
					  v_dataDump_[nt]->scaleFactor(ndd));
		  }
		  
		  msDataPtr_->timeCentroid  = 86400.*timeCentroidMJD; // default value would there be no bin actualTimes
		  msDataPtr_->exposure      = exposure;               // default value would there be no bin actualDurations
		  msDataPtr_->flag          = 0;                      // default value is "false"  would there be no bin flags
		  if(verbose_)cout<<"ici DD 7 msDataPtr_->numData="<<msDataPtr_->numData<<endl;
		  // the associated SDM meta-data
		  if(msDataPtr_->numData>0){
		    msDataPtr_->processorId          = cdPtr->getProcessorId().getTagValue();
		    msDataPtr_->time                 = 86400.*timeMJD;
		    msDataPtr_->antennaId1           = v_antSet[na1].getTagValue();
		    msDataPtr_->antennaId2           = v_antSet[na2].getTagValue();
		    msDataPtr_->feedId1              = v_feedSet[na1*numFeed+nfe];
		    msDataPtr_->feedId2              = v_feedSet[na2*numFeed+nfe];
		    msDataPtr_->dataDescId           = v_ddList[ndd].getTagValue();
		    msDataPtr_->fieldId              = fieldId;
		    msDataPtr_->phaseDir             = phaseDir;
		    /* This part is now done in the filler itself -  MC 23 Jul 2012
		    msDataPtr_->msState              = getMSState( subscanNum, v_stateId,
								   v_antSet, v_feedSet, v_ddList,
								   na1, nfe, ndd, timeOfDump);
		    */
                     // TODO Apply the polynomial formula
		    msDataPtr_->v_dataShape          = v_dataShape;
		    msDataPtr_->interval             = interval;
		    msDataPtr_->v_atmPhaseCorrection = v_selected_apc;
		    msDataPtr_->binNum               = nbi+1;              // a number is one-based.
		    vector<unsigned int> v_projectNodes;
		    if(v_dataDump_[nt]->integrationNum())v_projectNodes.push_back(v_dataDump_[nt]->integrationNum());
		    if(v_dataDump_[nt]->subintegrationNum())v_projectNodes.push_back(v_dataDump_[nt]->subintegrationNum());
		    msDataPtr_->projectPath          = v_projectNodes;
		  }
		  // store in vector for export
		  if(msDataPtr_->numData>0)v_msDataPtr_.push_back(msDataPtr_);
		  if(verbose_)cout << "A/ msDataPtr_->numData=" << msDataPtr_->numData << endl;
		  scn++;
		}
	      }
	    }
	  }
	}
      }
      if(verbose_)cout<<"v_msDataPtr_.size()="<< v_msDataPtr_.size() << endl;
    }
    if (verbose_) cout << "SDMBinData::getMSDataFromBDFData: exiting" << endl;
    return v_msDataPtr_;
  }

  void SDMBinData::getNextDataDumps(unsigned int nDataSubsets) {
    if (verbose_) cout << "SDMBinData::getNextDataDumps : entering" << endl;

    /*
     * Possibly we must empty the vector of pointers on DataDump objects containing
     * elements from a previous call.
     */
    vector<DataDump*>::reverse_iterator it, itrb=v_dataDump_.rbegin(), itre=v_dataDump_.rend();
    for(it=itrb; it!=itre; it++){
      delete *it;
    }
    v_dataDump_.clear();

    Enum<CorrelationMode>            e_cm;
    Enum<ProcessorType>              e_pt;
    Enum<CorrelatorType>             e_ct;
 
    if (verbose_) cout << "*" << endl;
//    unsigned int                     numTime=0;
 
    unsigned int                     numAnt              = sdmdosr.numAntenna();   
    CorrelationMode                  correlationMode     = sdmdosr.correlationMode(); e_cm=correlationMode;
    ProcessorType                    processorType       = sdmdosr.processorType();   e_pt=processorType;
    CorrelatorType                   correlatorType;
    
    const SDMDataObject::DataStruct& dataStruct          = sdmdosr.dataStruct();
    
    const vector<AtmPhaseCorrection>&       v_apc               = dataStruct.apc();
    const vector<SDMDataObject::Baseband>&  v_baseband          = dataStruct.basebands();
    SDMDataObject::ZeroLagsBinaryPart               zeroLagsParameters;
    
//    bool                             normalized=false;
    if(e_pt[ProcessorTypeMod::CORRELATOR]){
//      if(e_cm[CorrelationModeMod::CROSS_ONLY])
//	normalized =  dataStruct.autoData().normalized();
      if(dataStruct.zeroLags().size()){
	zeroLagsParameters = dataStruct.zeroLags();
	correlatorType = zeroLagsParameters.correlatorType(); e_ct=correlatorType;
      }
    }
 
    unsigned int                     numApc              = v_apc.size();
    unsigned int                     numBaseband         = v_baseband.size();
   
    vector< vector< unsigned int > > vv_numAutoPolProduct;
    vector< vector< unsigned int > > vv_numCrossPolProduct;
    vector< vector< unsigned int > > vv_numSpectralPoint;
    vector< vector< unsigned int > > vv_numBin;
    vector< vector< float > >        vv_scaleFactor;
    vector< unsigned int >           v_numSpectralWindow;
    vector< vector< Enum<NetSideband> > > vv_e_sideband;
    vector< vector< SDMDataObject::SpectralWindow* > > vv_image;
    
    if(verbose_)cout<<"e_cm="<<e_cm.str()<<endl;
    
    for(unsigned int nbb=0; nbb<v_baseband.size(); nbb++){
      vector<SDMDataObject::SpectralWindow>  v_spw = v_baseband[nbb].spectralWindows(); v_numSpectralWindow.push_back(v_spw.size());
      vector<SDMDataObject::SpectralWindow*> v_image;
      vector< unsigned int > v_numBin;
      vector< unsigned int > v_numSpectralPoint;
      vector< unsigned int > v_numSdPolProduct;
      vector< unsigned int > v_numCrPolProduct;
      vector< Enum<NetSideband> >  v_e_sideband;
      vector<float>          v_scaleFactor;
      for(unsigned int nspw=0; nspw<v_spw.size(); nspw++){
	v_e_sideband.push_back(Enum<NetSideband>(v_spw[nspw].sideband()));
	v_image.push_back(NULL);                                   // TODO waiting for implementation
	v_numBin.push_back(v_spw[nspw].numBin());
	if(verbose_)cout<<"v_spw["<<nspw<<"].numSpectralPoint()="<<v_spw[nspw].numSpectralPoint()<<endl;
	v_numSpectralPoint.push_back(v_spw[nspw].numSpectralPoint());
	if(e_cm[AUTO_ONLY])
	  v_numSdPolProduct.push_back(v_spw[nspw].sdPolProducts().size());
	else{
	  v_numCrPolProduct.push_back(v_spw[nspw].crossPolProducts().size());
	  v_scaleFactor.push_back(v_spw[nspw].scaleFactor());
	  if(e_cm[CROSS_AND_AUTO])
	    v_numSdPolProduct.push_back(v_spw[nspw].sdPolProducts().size());
	}
      }
      if(e_cm[CROSS_ONLY]==false)vv_numAutoPolProduct.push_back(v_numSdPolProduct);
      if(e_cm[AUTO_ONLY]==false) vv_numCrossPolProduct.push_back(v_numCrPolProduct);
      vv_numSpectralPoint.push_back(v_numSpectralPoint);
      vv_e_sideband.push_back(v_e_sideband);
      vv_image.push_back(v_image);
      vv_numBin.push_back(v_numBin);
      vv_scaleFactor.push_back(v_scaleFactor);
    }
    if(e_cm[AUTO_ONLY])vv_numCrossPolProduct.clear();
    if(e_cm[CROSS_ONLY])vv_numAutoPolProduct.clear();
    
    complexData_=false;
    if(vv_numAutoPolProduct.size()){
      for(unsigned int nbb=0; nbb<vv_numAutoPolProduct.size(); nbb++){
	for(unsigned int nspw=0; nspw<vv_numAutoPolProduct[nbb].size(); nspw++)
	  if(vv_numAutoPolProduct[nbb][nspw]>2)complexData_=true;
      }
    }
    if(vv_numCrossPolProduct.size())complexData_=true;
    
    EnumSet<AxisName>         es_axisNames;
    DataContent               dataContent;
    SDMDataObject::BinaryPart binaryPart;
    map<DataContent,pair<unsigned int,EnumSet<AxisName> > > m_dc_sizeAndAxes;
    
    dataContent = FLAGS;
    binaryPart  = dataStruct.flags();
    es_axisNames.set(binaryPart.axes(),true);
    if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
    
    dataContent = ACTUAL_DURATIONS;
    binaryPart  = dataStruct.actualDurations();
    es_axisNames.set(binaryPart.axes(),true);
    if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
    
    dataContent = ACTUAL_TIMES;
    binaryPart  = dataStruct.actualTimes();
    es_axisNames.set(binaryPart.axes(),true);
    if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
    
    dataContent = AUTO_DATA;
    binaryPart  = dataStruct.autoData();
    es_axisNames.set(binaryPart.axes(),true);
    if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
    
//    if(es_axisNames[TIM])numTime=sdmdosr.numTime();
    
    dataContent = CROSS_DATA;
    binaryPart  = dataStruct.crossData();
    es_axisNames.set(binaryPart.axes(),true);
    if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
    
    if(binaryPart.size()){
      dataContent = ZERO_LAGS;
      binaryPart  = dataStruct.zeroLags();
      es_axisNames.set(binaryPart.axes(),true);
      if(es_axisNames.count())m_dc_sizeAndAxes.insert(make_pair(dataContent,make_pair(binaryPart.size(),es_axisNames)));
    }
    
//    if(es_axisNames[TIM])numTime=sdmdosr.numTime();
    
    map<DataContent,pair<unsigned int,EnumSet<AxisName> > >::iterator
      itf=m_dc_sizeAndAxes.find(AUTO_DATA),
      ite=m_dc_sizeAndAxes.end();
    if(itf==ite)itf=m_dc_sizeAndAxes.find(CROSS_DATA);
    if(itf==ite)Error(FATAL,(char *) "BLOB %s has no declaration for observational data",sdmdosr.dataOID().c_str());
    
    // Okay then we can start to read.
    unsigned int nread = 0 ;
    int64_t                    interval;
    int64_t                    exposure;
    int64_t                    timeOfInteg;
    int64_t                    timeCentroid;
    const vector<SDMDataSubset>& sdmDataSubsets = sdmdosr.nextSubsets(nDataSubsets);

    for (vector<SDMDataSubset>::const_iterator iter = sdmDataSubsets.begin(); iter != sdmDataSubsets.end(); iter++) {
      const SDMDataSubset& currentSubset = *iter;
      nread++; 
      
      timeOfInteg  = currentSubset.time();        
      timeCentroid = currentSubset.time();        
      interval     = currentSubset.interval();    
      exposure     = interval;
      if(verbose_){
	for(unsigned int i=0; i<v_numSpectralWindow.size(); i++){
	  for(unsigned int j=0; j<v_numSpectralWindow[i]; j++)
	    cout<<"vv_numSpectralPoint[i][j]="<<vv_numSpectralPoint[i][j]<<endl;
	}
	for(unsigned int i=0; i<vv_numAutoPolProduct.size(); i++){
	  for(unsigned int j=0; j<vv_numAutoPolProduct[i].size(); j++)
	    cout<<"vv_numAutoPolProduct[i][j]="<<vv_numAutoPolProduct[i][j]<<endl;
	}
	for(unsigned int i=0; i<vv_numCrossPolProduct.size(); i++){
	  for(unsigned int j=0; j<vv_numCrossPolProduct[i].size(); j++)
	    cout<<"vv_numCrossPolProduct[i][j]="<<vv_numCrossPolProduct[i][j]<<endl;
	}
      }
      DataDump* dataDumpPtr = new DataDump( vv_numCrossPolProduct,
					    vv_numAutoPolProduct,
					    vv_numSpectralPoint,
					    vv_numBin,
					    vv_e_sideband,
					    numApc,
					    v_numSpectralWindow,
					    numBaseband,
					    numAnt,
					    correlationMode,
					    timeOfInteg,
					    timeCentroid,
					    interval,
					    exposure
					    );
      if(e_cm[CROSS_ONLY]    )dataDumpPtr->setScaleFactor(vv_scaleFactor);
      if(e_cm[CROSS_AND_AUTO])dataDumpPtr->setScaleFactor(vv_scaleFactor);

      if(verbose_)cout<<"m_dc_sizeAndAxes.size()="<< m_dc_sizeAndAxes.size() << endl;
      if((itf=m_dc_sizeAndAxes.find(FLAGS))!=ite){
	if(verbose_)cout<<"Flags have been declared in the main header"<<endl;
	const unsigned int* flagsPtr;
	unsigned int long numFlags = currentSubset.flags( flagsPtr );
	if(verbose_)cout<<numFlags<<" "<<itf->second.first<<endl;
	if(numFlags)dataDumpPtr->attachFlags( itf->second.first, itf->second.second,
					      numFlags, flagsPtr);
      }

      if((itf=m_dc_sizeAndAxes.find(ACTUAL_TIMES))!=ite){
	if(verbose_)cout<<"ActualTimes have been declared in the main header"<<endl;
	const int64_t* actualTimesPtr;
	unsigned long int numActualTimes = currentSubset.actualTimes( actualTimesPtr );
	if(verbose_)cout<<numActualTimes<<" "<<itf->second.first<<endl;
	if(numActualTimes)dataDumpPtr->attachActualTimes( itf->second.first, itf->second.second,
							  numActualTimes, actualTimesPtr);
      }

      if((itf=m_dc_sizeAndAxes.find(ACTUAL_DURATIONS))!=ite){
	if(verbose_)cout<<"ActualDurations have been declared in the main header"<<endl;
	const int64_t* actualDurationsPtr;
	unsigned long int numActualDurations = currentSubset.actualDurations( actualDurationsPtr );
	if(verbose_)cout<<numActualDurations<<" "<<itf->second.first<<endl;
	if(numActualDurations)dataDumpPtr->attachActualDurations( itf->second.first, itf->second.second,
								  numActualDurations, actualDurationsPtr);
      }

      if((itf=m_dc_sizeAndAxes.find(ZERO_LAGS))!=ite){
	if(verbose_)cout<<"ZeroLags have been declared in the main header"<<endl;
	const float* zeroLagsPtr;
	unsigned long int numZeroLags = currentSubset.zeroLags( zeroLagsPtr );
	if(verbose_)cout<<numZeroLags<<" "<<itf->second.first<<endl;
	if(numZeroLags)dataDumpPtr->attachZeroLags( itf->second.first, itf->second.second,
						    numZeroLags, zeroLagsPtr);
      }

      if((itf=m_dc_sizeAndAxes.find(CROSS_DATA))!=ite){
	if(verbose_)cout<<"CrossData have been declared in the main header ";
	unsigned long int numCrossData;
	switch(currentSubset.crossDataType()){
	case INT16_TYPE:
	  { const short int* crossDataPtr;
	    numCrossData = currentSubset.crossData( crossDataPtr );
	    if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							  numCrossData, crossDataPtr);
	  }
	  if(verbose_)cout<<"SHORT_TYPE"<<endl;
	  if(verbose_)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataShort()<<endl;
	  break;
	case INT32_TYPE:
	  { const int* crossDataPtr;
	    numCrossData = currentSubset.crossData( crossDataPtr );
	    if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							  numCrossData, crossDataPtr);
	  }
	  if(verbose_)cout<<"INT_TYPE"<<endl;
	  if(verbose_)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataLong()<<endl;
	  break;
	case FLOAT32_TYPE:
	  { const float* crossDataPtr;
	    numCrossData = currentSubset.crossData( crossDataPtr );
	    if(numCrossData)dataDumpPtr->attachCrossData( itf->second.first, itf->second.second,
							  numCrossData, crossDataPtr);
	  }
	  if(verbose_)cout<<"FLOAT_TYPE"<<endl;
	  if(verbose_)cout<<"crossData attached,  const pointer:"<<dataDumpPtr->crossDataFloat()<<endl;
	  break;
	default:
	  Enum<PrimitiveDataType> e_pdt=currentSubset.crossDataType();
	  Error(FATAL, (char *) "Cross data with the primitive data type %s are not supported",
		e_pdt.str().c_str());
	}
      }

      if((itf=m_dc_sizeAndAxes.find(AUTO_DATA))!=ite){
	if(verbose_)cout<<"AutoData have been declared in the main header"<<endl;
	const float*      floatDataPtr=NULL;
	unsigned long int numFloatData = currentSubset.autoData( floatDataPtr );
	if(verbose_)cout<<numFloatData<<" "<<itf->second.first<<"  "<<floatDataPtr<<endl;
	if(numFloatData){
	  if(numFloatData!=itf->second.first)
	    Error(FATAL,(char *) "Size of autoData, %d, not compatible with the declared size of %d",
		  numFloatData,itf->second.first);
	  dataDumpPtr->attachAutoData( itf->second.first, itf->second.second,
				       numFloatData, floatDataPtr);
	  if(verbose_)cout<<"autoData attached,  const pointer:"<<dataDumpPtr->autoData()<<endl;
	  // 	      const long unsigned int* aptr=dataDumpPtr->flags();
	}else if(numFloatData==0){
	  if(!e_cm[CROSS_ONLY])
	    Error(WARNING, string("No autoData! may happen when a subscan is aborted"));
	  break;
	}
      }

      if (verbose_) cout << "About to setContextUsingProjectPath" << endl;
      dataDumpPtr->setContextUsingProjectPath(currentSubset.projectPath());
      if (verbose_) cout << "Back from setContextUsingProjectPath" << endl;
      const unsigned int* bptr=dataDumpPtr->flags(); if(bptr==NULL)if(verbose_)cout<<"No flags"<<endl;
      if(verbose_)cout<<"store dataDump"<<endl;
      v_dataDump_.push_back(dataDumpPtr);
      if(verbose_)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;
    }
    if(verbose_)cout<<"v_dataDumpPtr_.size()="<< v_dataDump_.size()<<endl;
    if (verbose_) cout << "SDMBinData::getNextDataDumps : exiting" << endl;
    return;
  }

  MSState SDMBinData::getMSState( unsigned int subscanNum,
				  vector<Tag>  v_stateId,
				  vector<Tag>  v_antennaId, vector<int> v_feedId, vector<Tag> v_ddId,
				  unsigned int na, unsigned int nfe, unsigned int nspw, ArrayTime timeOfDump)
  {
    if (verbose_) cout << "SDMBinData::getMSState : entering." << endl;
    static unsigned int                                     subscan    = 0;
    static vector<StateRow*>                                v_sdmState;
    static vector<vector<vector<vector<CalDeviceRow*> > > > vvvv_calDevice;
    static vector<Tag>                                      v_spwId;
    MSState                                                 msState;
    //    if(subscan!=subscanNum){
      StateTable&  sdmStates  = datasetPtr_->getState();
      v_sdmState.resize(v_stateId.size());
      for(unsigned int n=0 ; n<v_stateId.size(); n++)
	v_sdmState[n] = sdmStates.getRowByKey(v_stateId[n]);
      //}
    if(verbose_)cout<<"v_sdmState="<<v_sdmState.size()<<endl;

    msState.sig  = v_sdmState[na]->getSig();
    msState.ref  = v_sdmState[na]->getRef();
    msState.cal  = 0;
    msState.load = 0;

    if(!v_sdmState[na]->getOnSky()){
      if(verbose_)cout<<"subscan="<<subscan<<" subscanNum="<<subscanNum<<endl;
      //if(subscan!=subscanNum){
	vector<CalDeviceRow*> v_calDev;
	vector<CalDeviceRow*>* v_calDevPtr = 0;
	CalDeviceTable&       calDevices       = datasetPtr_->getCalDevice();
	DataDescriptionTable& dataDescriptions = datasetPtr_->getDataDescription();
	for(unsigned int n_d=0; n_d<v_ddId.size(); n_d++)
	  v_spwId.push_back(dataDescriptions.getRowByKey(v_ddId[n_d])->getSpectralWindowId());

	vvvv_calDevice.resize(v_antennaId.size());
	for(unsigned int n_a=0; n_a<v_antennaId.size(); n_a++){
	  vvvv_calDevice[n_a].resize(v_feedId.size());
	  for(unsigned int n_f=0; n_f<v_feedId.size(); n_f++){
	    vvvv_calDevice[n_a][n_f].resize(v_spwId.size());
	    for(unsigned int n_s=0; n_s<v_spwId.size(); n_s++){
	      if(verbose_)cout<<" antId=" << v_antennaId[n_a].toString()
			     <<" feedId="<< v_feedId[n_f]
			     <<" spwId=" << v_spwId[n_s].toString()
			     <<endl;
	      vvvv_calDevice[n_a][n_f][n_s] = v_calDev;
	      if ( (v_calDevPtr = calDevices.getByContext( v_antennaId[n_a],
							   v_spwId[n_s],
							   (int) v_feedId[n_f])) != 0)
		vvvv_calDevice[n_a][n_f][n_s] = *v_calDevPtr;
	      else {
		ostringstream oss;
		oss << "Could not find a row in the table CalDevice for antennaId = "
		     << v_antennaId[n_a].getTagValue()
		     <<", spectralWindowId = "
		     << v_spwId[n_s].getTagValue()
		     << ", feedId = "
		     << v_feedId[n_f]
		     << endl;
		Error(FATAL, oss.str());
	      }
	    }
	  }
	}
	//}
      int nt=-1;
      vector<CalDeviceRow*> v_ts=vvvv_calDevice[na][nfe][nspw];   // the time series
      if(!v_ts.size())Error(FATAL, string("The CalDevice is empty, I can't go further."));
      for(unsigned int n=0; n<v_ts.size(); n++){
	if( nt==-1 && v_ts[n]->getTimeInterval().contains(timeOfDump) ){
	  nt=n;
	  break;
	}
      }
      if(nt==-1)
	Error( FATAL,
	       (char *) "Failed to find the CalDevice row for na=%d nfe=%d nspw=%d at the given time of dump",
	       na,nfe,nspw);

      CalDeviceRow*             caldevr      = vvvv_calDevice[na][nfe][nspw][nt];
      vector<CalibrationDevice> v_calDevice                                                                 = caldevr->getCalLoadNames();
      vector<double>            v_noiseCal;        if(caldevr->isNoiseCalExists())        v_noiseCal        = caldevr->getNoiseCal();
      vector<Temperature>       v_temperatureLoad; if(caldevr->isTemperatureLoadExists()) v_temperatureLoad = caldevr->getTemperatureLoad();
      vector<vector<float> >    v_calEff;          if(caldevr->isCalEffExists())          v_calEff          = caldevr->getCalEff();

      CalibrationDevice         calDevName        = v_sdmState[na]->getCalDeviceName();
      for(unsigned int n_cd=0; n_cd<v_calDevice.size(); n_cd++){
	switch(v_calDevice[n_cd]) {
	case AMBIENT_LOAD:
	  if(v_calDevice[n_cd]==calDevName)
	    msState.load = v_temperatureLoad[n_cd].get();
	  break;
	case COLD_LOAD:
	  if(v_calDevice[n_cd]==calDevName)
	    msState.load = v_temperatureLoad[n_cd].get();
	  break;
	case HOT_LOAD:
	  if(v_calDevice[n_cd]==calDevName)
	    msState.load = v_temperatureLoad[n_cd].get();
	  break;
	case NOISE_TUBE_LOAD:
	  if(v_calDevice[n_cd]==calDevName)
	    msState.cal = v_noiseCal[n_cd];
	  break;
	case QUARTER_WAVE_PLATE:
	  Error(FATAL, string("QUATER_WAVE_PLATE use-case not yet supported"));
	  break;
	case SOLAR_FILTER:
	  Error(FATAL, string("SOLAR_FILTER use-case not yet supported"));
	  break;
	default:
	  Error(FATAL, string("Illegal calibration device name"));
	}
      }
    }
    msState.subscanNum = subscanNum;

    if(verbose_){
      cout<<"State: ";
      if(msState.sig)cout<<" sig Y"; else cout<<" sig N";
      if(msState.sig)cout<<" ref Y"; else cout<<" ref N";
      cout<<" cal="       <<msState.cal
	  <<" load="      <<msState.load
	  <<" subscanNum="<<msState.subscanNum<<endl;
    }

    // TODO the OBS_MODE

    subscan = subscanNum;
    if (verbose_) cout << "SDMBinData::getMSState : exiting." << endl;
    return msState;
  }

  bool  SDMBinData::isComplexData(){
  
    if(mainRowPtr_){

      if(canSelect_){
	return complexData_;    // the state of complexData_ is set in the method acceptMainRow(mainRowPtr)
      }else{

	cerr<<"Forbidden to use the method isComplexData() in the context of a specific"<<endl;
	cerr<<"row of the Main table because it was already used in the context of the Main"<<endl;
	cerr<<"table as a whole"<<endl;
	Error(FATAL, string("Use-case not valid in the perpective of a filling a MeasurementSet"));
	return true;  // never pass here
      }

    }else{
      /*
	 This part is intended to know if a DATA or FLOAT_DATA column has to be created before

	 considering the SDM main table rows one by one when building a MS main table.
      */

      canSelect_    = false;
      forceComplex_ = true;

      if (e_qcm_[CROSS_ONLY] && es_cm_[CROSS_ONLY]) return forceComplex_;

      vector<MainRow*>            v_mr = datasetPtr_->getMain().get();
      vector<DataDescriptionRow*> v_ddr;
      ConfigDescriptionRow*       cdr;
      Enum<CorrelationMode>       e_cm;
      for(unsigned int n=0; n<v_mr.size(); n++){
	if(!reasonToReject(v_mr[n]).length()){
	  cdr  = v_mr[n]->getConfigDescriptionUsingConfigDescriptionId();
	  e_cm = cdr->getCorrelationMode();
	  if(e_cm[AUTO_ONLY]){
	    v_ddr = cdr->getDataDescriptionsUsingDataDescriptionId();
	    for(unsigned int ndd=0; ndd<v_ddr.size(); ndd++)
	      if(v_ddr[ndd]->getPolarizationUsingPolOrHoloId()->getCorrType().size()>2)return forceComplex_;
	  }
	  else if(e_cm[CROSS_AND_AUTO]){
	    if(e_qcm_[CROSS_AND_AUTO]){
	      return true;
	    }else{
	      v_ddr = cdr->getDataDescriptionsUsingDataDescriptionId();
	      for(unsigned int ndd=0; ndd<v_ddr.size(); ndd++)
		if(v_ddr[ndd]->getPolarizationUsingPolOrHoloId()->getCorrType().size()>2)return forceComplex_;
	    }
	  }
	  else if (e_cm[CROSS_ONLY]) {
	    return true;
	  }
	}
      }
      forceComplex_ = false;
      return forceComplex_;
    }
  }

  bool SDMBinData::baselineReverse(){
    return SDMBinData::baselineReverse_;
  }

  bool SDMBinData::autoTrailing(){
    return SDMBinData::autoTrailing_;
  }

  pair<bool,bool> SDMBinData::dataOrder(){
    pair<bool,bool> p=make_pair( baselineReverse_, autoTrailing_ );
    return p;
  }

  string           SDMBinData::execBlockDir_  = "";
  ASDM*            SDMBinData::datasetPtr_    = 0;
  bool             SDMBinData::canSelect_     = true;
  bool             SDMBinData::forceComplex_  = false;
  MSData*          SDMBinData::msDataPtr_     = 0;
  SDMData*         SDMBinData::sdmDataPtr_    = 0;
  VMSData*         SDMBinData::vmsDataPtr_    = 0;
  vector<MSData*>  SDMBinData::v_msDataPtr_;
  vector<SDMData*> SDMBinData::v_sdmDataPtr_;
  bool             SDMBinData::coutDeleteInfo_=false;
  BaselinesSet*    SDMBinData::baselinesSet_  =0;
  bool             SDMBinData::baselineReverse_ = false;
  bool             SDMBinData::autoTrailing_    = false;
  bool             SDMBinData::syscal_          = false;
}
