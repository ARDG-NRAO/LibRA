#include <sstream>
#include <iostream>
#include <stdlib.h> // for atoi()
#include <stdint.h>

#include <alma/ASDMBinaries/Integration.h>
#include <alma/ASDMBinaries/Error.h>

using namespace sdmbin;
using namespace std;

using namespace AxisNameMod;
using namespace CorrelationModeMod;
using namespace NetSidebandMod;

namespace sdmbin{
// ============================================================================================
// DataStructure: the base
// ============================================================================================

  DataStructure::DataStructure()
  {
  }

  DataStructure::DataStructure(  uint32_t      numPolProduct, 
				 uint32_t      numBin,
				 Enum<NetSideband> e_sideband,            
				 uint32_t      numBaseband,   
				 uint32_t      numAnt,
				 CorrelationMode   correlationMode 
				 )
  {
    v_numSpectralWindow_.resize(0); 
    vv_numAutoPolProduct_.resize(numBaseband);
    vv_numSpectralPoint_.resize(numBaseband);
    vv_numBin_.resize(numBaseband);
    vv_e_sideband_.resize(numBaseband);
    for(uint32_t nbb=0; nbb<numBaseband; nbb++){
      v_numSpectralWindow_.push_back(1);
      for(uint32_t nspw=0; nspw<numBaseband; nspw++){
	vv_numAutoPolProduct_[nbb].push_back(numPolProduct);
	vv_numSpectralPoint_[nbb].push_back(1);
	vv_numBin_[nbb].push_back(numBin);
	vv_e_sideband_[nbb].push_back(e_sideband);
      }
    }
    numApc_          = 1;
    numBaseband_     = numBaseband;
    numAnt_          = numAnt;
    correlationMode_ = correlationMode;
    axisSequence_    = setStructureProperties();
  }

  DataStructure::DataStructure(  uint32_t      numPolProduct,
				 uint32_t      numSpectralPoint,
				 uint32_t      numBin,
				 Enum<NetSideband> e_sideband,
				 uint32_t      numBaseband,
				 uint32_t      numAnt,
				 CorrelationMode   correlationMode)
  {
    v_numSpectralWindow_.resize(0); 
    vv_numAutoPolProduct_.resize(numBaseband);
    vv_numSpectralPoint_.resize(numSpectralPoint);
    vv_numBin_.resize(numBaseband);
    vv_e_sideband_.resize(numBaseband);
    for(uint32_t nbb=0; nbb<numBaseband; nbb++){
      v_numSpectralWindow_.push_back(1);
      for(uint32_t nspw=0; nspw<numBaseband; nspw++){
	vv_numAutoPolProduct_[nbb].push_back(numPolProduct);
	vv_numSpectralPoint_[nbb].push_back(1);
	vv_numBin_[nbb].push_back(numBin);
	vv_e_sideband_[nbb].push_back(e_sideband);
      }
    }
    numAnt_          = numAnt;
    numApc_          = 1;
    numBaseband_     = numBaseband;
    correlationMode_ = correlationMode;
    axisSequence_    = setStructureProperties();
  }

  DataStructure::DataStructure( vector<vector<uint32_t> >       vv_numCrossPolProduct,// /bb/spw
				vector<vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
				vector<vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
				vector<vector<uint32_t> >       vv_numBin,            // /bb/spw
				vector<vector<Enum<NetSideband> > > vv_e_sideband,        // /bb/spw
				uint32_t                        numApc,
				vector<uint32_t>                v_numSpectralWindow,  // /bb
				uint32_t                        numBaseband,
				uint32_t                        numAnt,
				CorrelationMode                     correlationMode):
    vv_numCrossPolProduct_(vv_numCrossPolProduct),
    vv_numAutoPolProduct_(vv_numAutoPolProduct),
    vv_numSpectralPoint_(vv_numSpectralPoint),
    vv_numBin_(vv_numBin),
    vv_e_sideband_(vv_e_sideband),
    numApc_(numApc),
    v_numSpectralWindow_(v_numSpectralWindow),
    numBaseband_(numBaseband),
    numAnt_(numAnt),
    correlationMode_(correlationMode)
  {
    axisSequence_ = setStructureProperties();
  }

  DataStructure::DataStructure( vector<vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
				vector<vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
				vector<vector<uint32_t> >       vv_numBin,            // /bb/spw
				vector<vector<Enum<NetSideband> > > vv_e_sideband,        // /bb/spw
				uint32_t                        numApc,
				vector<uint32_t>                v_numSpectralWindow,  // /bb
				uint32_t                        numBaseband,
				uint32_t                        numAnt,
				CorrelationMode                     correlationMode):
    vv_numAutoPolProduct_(vv_numAutoPolProduct),
    vv_numSpectralPoint_(vv_numSpectralPoint),
    vv_numBin_(vv_numBin),
    vv_e_sideband_(vv_e_sideband),
    numApc_(numApc),
    v_numSpectralWindow_(v_numSpectralWindow),
    numBaseband_(numBaseband),
    numAnt_(numAnt),
    correlationMode_(correlationMode)
  {
    vv_numCrossPolProduct_.clear();
    axisSequence_ = setStructureProperties();
  }

  DataStructure::DataStructure( const DataStructure & a)
  {
    vv_numCrossPolProduct_ = a.vv_numCrossPolProduct_;
    vv_numAutoPolProduct_  = a.vv_numAutoPolProduct_;
    vv_numSpectralPoint_   = a.vv_numSpectralPoint_;
    vv_numBin_             = a.vv_numBin_;
    vv_e_sideband_         = a.vv_e_sideband_;
    numApc_                = a.numApc_;
    v_numSpectralWindow_   = a.v_numSpectralWindow_;
    numBaseband_           = a.numBaseband_;
    numAnt_                = a.numAnt_;
    correlationMode_       = a.correlationMode_;
    axisSequence_          = a.axisSequence_;
    v_minSize_             = a.v_minSize_;
    v_maxSize_             = a.v_maxSize_;
  }

  DataStructure::~DataStructure()
  {
  }

  string DataStructure::setStructureProperties()
  {
    uint32_t  minSize,maxSize;
    bool          newLevel;
    ostringstream axisSequence;
  
    newLevel = false;
    minSize  = 100000;
    maxSize  = 0;
    if(vv_numAutoPolProduct_.size()){
      for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
	for(uint32_t nspw=0; nspw<v_numSpectralWindow_[nbb]; nspw++){
	  if(vv_numAutoPolProduct_[nbb][nspw]<minSize)minSize=vv_numAutoPolProduct_[nbb][nspw];
	  if(vv_numAutoPolProduct_[nbb][nspw]>maxSize)maxSize=vv_numAutoPolProduct_[nbb][nspw];
	  if(vv_numAutoPolProduct_[nbb][nspw]>1)newLevel=true;
	}
      }
    }
    if(vv_numCrossPolProduct_.size()){
      for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
	for(uint32_t nspw=0; nspw<v_numSpectralWindow_[nbb]; nspw++){
	  if(vv_numCrossPolProduct_[nbb][nspw]<minSize)minSize=vv_numCrossPolProduct_[nbb][nspw];
	  if(vv_numCrossPolProduct_[nbb][nspw]>maxSize)maxSize=vv_numCrossPolProduct_[nbb][nspw];
	  if(vv_numCrossPolProduct_[nbb][nspw]>1)newLevel=true;
	}
      }
    }
    if( newLevel ){
      v_minSize_.push_back(minSize);
      v_maxSize_.push_back(maxSize);
      axisSequence << "1";
    }

    newLevel=false;
    minSize  = 1000000;
    maxSize  = 0;
    for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
      for(uint32_t nspw=0; nspw<v_numSpectralWindow_[nbb]; nspw++){
	if(vv_numSpectralPoint_[nbb][nspw]<minSize)minSize=vv_numSpectralPoint_[nbb][nspw];
	if(vv_numSpectralPoint_[nbb][nspw]>maxSize)maxSize=vv_numSpectralPoint_[nbb][nspw];
	if(vv_numSpectralPoint_[nbb][nspw]>1)newLevel=true;
      }
    }
    if( newLevel ){
      v_minSize_.push_back(minSize);
      v_maxSize_.push_back(maxSize);
      axisSequence << "2";
    }

    if( numApc_>1 ){
      v_minSize_.push_back(numApc_);
      v_maxSize_.push_back(numApc_);
      axisSequence << "3";
    }

    newLevel=false;
    minSize  = 100000;
    maxSize  = 0;
    for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
      for(uint32_t nspw=0; nspw<v_numSpectralWindow_[nbb]; nspw++){
	if(vv_numBin_[nbb][nspw]<minSize)minSize=vv_numBin_[nbb][nspw];
	if(vv_numBin_[nbb][nspw]>maxSize)maxSize=vv_numBin_[nbb][nspw];
	if(vv_numBin_[nbb][nspw]>1)newLevel=true;
      }
    }
    if( newLevel ){
      v_minSize_.push_back(minSize);
      v_maxSize_.push_back(maxSize);
      axisSequence << "4";
    }

    newLevel=false;
    minSize  = 100000;
    maxSize  = 0;
    for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
      if(v_numSpectralWindow_[nbb]<minSize)minSize=v_numSpectralWindow_[nbb];
      if(v_numSpectralWindow_[nbb]>maxSize)maxSize=v_numSpectralWindow_[nbb];
      if(v_numSpectralWindow_[nbb]>1)newLevel=true;
    }
    if( newLevel ){
      v_minSize_.push_back(minSize);
      v_maxSize_.push_back(maxSize);
      axisSequence << "5";
    }

    if( numBaseband_>1 ){
      v_minSize_.push_back(numBaseband_);
      v_maxSize_.push_back(numBaseband_);
      axisSequence << "6";
    }

    if(correlationMode_!=AUTO_ONLY){
      v_minSize_.push_back(numAnt_);
      v_maxSize_.push_back(numAnt_);
      axisSequence << "7";
    }

    if(correlationMode_!=CROSS_ONLY){
      v_minSize_.push_back(numAnt_);
      v_maxSize_.push_back(numAnt_);
      axisSequence << "8";
    }

    return axisSequence.str();

  }

  vector<vector<uint32_t> >       DataStructure::numCrossPolProducts(){ return vv_numCrossPolProduct_; }

  vector<vector<uint32_t> >       DataStructure::numAutoPolProducts() { return vv_numAutoPolProduct_;  }

  vector<vector<uint32_t> >       DataStructure::numSpectralPoints()  { return vv_numSpectralPoint_;   }

  vector<vector<uint32_t> >       DataStructure::numBins()            { return vv_numBin_;             }

  vector<vector<Enum<NetSideband> > > DataStructure::sidebands()          { return vv_e_sideband_;         }

  uint32_t                        DataStructure::numApc()             { return numApc_;                }

  vector<uint32_t>                DataStructure::numSpectralWindows() { return v_numSpectralWindow_;   }

  uint32_t                        DataStructure::numBaseband()        { return numBaseband_;           }

  uint32_t                        DataStructure::numAnt()             { return  numAnt_;               }

  CorrelationMode                     DataStructure::correlationMode()    { return correlationMode_;       }


  vector<uint32_t> DataStructure::leafAxisSizes(){
    if(!isIndexible()){
      vector<uint32_t> las;
      return las;
    }
    return leafAxisSizes(0,0);
  }

  vector<uint32_t> DataStructure::leafAxisSizes(uint32_t basebandIndex, uint32_t spectralWindowIndex){
    vector<uint32_t> las;
    if(vv_numBin_[basebandIndex][spectralWindowIndex]!=1)
      las.push_back(vv_numBin_[basebandIndex][spectralWindowIndex]);
    if(numApc_!=1)
      las.push_back(numApc_);
    if(vv_numSpectralPoint_[basebandIndex][spectralWindowIndex]!=1)
      las.push_back(vv_numSpectralPoint_[basebandIndex][spectralWindowIndex]);
    if(vv_numAutoPolProduct_[basebandIndex][spectralWindowIndex]!=1)
      las.push_back(vv_numAutoPolProduct_[basebandIndex][spectralWindowIndex]);
    return las;
  }

  vector<uint32_t> DataStructure::leafAxisSizes(uint32_t ndd){
    vector<uint32_t> las;
    if(ndd==0){
      return leafAxisSizes(0,0);
    }else{
      uint32_t k=0;
      uint32_t nb;
      uint32_t nspw=0;
      for(nb=0; nb<numBaseband_; nb++){
	for(nspw=0; nspw<v_numSpectralWindow_[nb]; nspw++){
	  if(k==ndd)break;
	  k++;
	}
      }
      return leafAxisSizes(nb,nspw);
    }
  }


  uint32_t DataStructure::isIndexible() const
  {
    uint32_t multidim=0;
    for(uint32_t n=0; n<v_minSize_.size(); n++)
      if(v_minSize_[n]==v_maxSize_[n])multidim++;
    if(multidim==v_minSize_.size())return multidim;
    return 0;
  }

  // todo
  vector<uint32_t> DataStructure::eAxisSizes() const
  {
    vector<uint32_t> v_size;
    if(!isIndexible())return v_size;         // 
    // 12345678 
    //v_size.push_back(vv_numCrossPolProduct_[0][0]);
    v_size.push_back(vv_numAutoPolProduct_[0][0]);
    v_size.push_back(vv_numSpectralPoint_[0][0]);
    v_size.push_back(numApc_);
    v_size.push_back(vv_numBin_[0][0]);
    v_size.push_back(v_numSpectralWindow_[0]);
    v_size.push_back(numBaseband_);
    if(correlationMode_!=AUTO_ONLY) v_size.push_back((numAnt_*(numAnt_-1))/2); 
    if(correlationMode_!=CROSS_ONLY)v_size.push_back(numAnt_);
    return v_size;
  }

  // todo
  vector<uint32_t> DataStructure::axisSizes() const
  {
    vector<uint32_t> v_size;
    for(uint32_t n=0; n<v_minSize_.size(); n++)
      if(v_minSize_[n]==v_maxSize_[n])v_size.push_back(v_minSize_[n]);     
    return v_size;
  }

  uint32_t DataStructure::dimension() const
  {
    return (uint32_t)axisSequence_.length();
  }

  vector<uint32_t> DataStructure::minAxSize() const
  {
    return v_minSize_;
  }

  vector<uint32_t> DataStructure::maxAxSize() const
  {
    return v_maxSize_;
  }

  uint32_t DataStructure::numCrossData() const
  {
    uint32_t numCrossData = 1;
    for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
      for(uint32_t nspw=0; nspw<v_numSpectralWindow_[nbb]; nspw++){
	numCrossData *= 
	  vv_numBin_[nbb][nspw] *
	  vv_numSpectralPoint_[nbb][nspw] *
	  vv_numCrossPolProduct_[nbb][nspw]; 
      }
    }
    return numCrossData;
  }

  uint32_t DataStructure::numAutoData() const
  {
    uint32_t numAutoData = 1;
    for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
      for(uint32_t nspw=0; nspw<v_numSpectralWindow_[nbb]; nspw++){
	numAutoData *= 
	  vv_numBin_[nbb][nspw] *
	  vv_numSpectralPoint_[nbb][nspw] *
	  vv_numAutoPolProduct_[nbb][nspw]; 
      }
    }

    //   if( numBaseline_>1 ){
    //     v_minSize_.push_back(numBaseline_);
    //     v_maxSize_.push_back(numBaseline_);
    //     axisSequence << "7";
    //   }
    //   numData *= numApc_*numAnt_;         // for total power only!

    return numAutoData;
  }

  string DataStructure::axisSequence() const
  {
    return axisSequence_;
  }

  void DataStructure::summary() const
  {
    cout<<"Data for "<<numAnt_<<" antenna."<<endl;
    for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
      cout<<"  Baseband "<<nbb+1
	  <<" has "<<v_numSpectralWindow_[nbb]
	  <<" spectral windows."<<endl;
      for(uint32_t nspw=0; nspw<v_numSpectralWindow_[nbb]; nspw++){
	cout<<"    Spectral window "<<nspw+1
	    <<"  num spectral points: "<<vv_numSpectralPoint_[nbb][nspw]
	    <<"  num bins: "<<vv_numBin_[nbb][nspw];
	if(vv_numAutoPolProduct_.size())
	  if(vv_numAutoPolProduct_[nbb].size())
	    cout<<"  num sd pol.: "<<vv_numAutoPolProduct_[nbb][nspw];
	if(vv_numCrossPolProduct_.size())
	  if(vv_numCrossPolProduct_[nbb].size())
	    cout<<"  num cross pol.: "<<vv_numCrossPolProduct_[nbb][nspw];
	cout<<endl;
      }
    }
    uint32_t multidim = isIndexible();
    if( multidim ){
      cout<<"The data structure is  multi-dimensional with the dimensionality "
	  <<multidim<<endl;
      cout<<"  axis order: "<<axisSequence_<<endl;
      vector<uint32_t>v_size=axisSizes();
      ostringstream os; 
      for(uint32_t n=0; n<v_size.size()-1; n++)os<<v_size[n]<<",";
      os<<v_size[v_size.size()-1];
      cout<<"  axis sizes: "<<os.str()<<endl;;
    }
  }

  // ============================================================================================
  // DataDump -> DataStructure
  // ============================================================================================

  DataDump::DataDump()
  {
    //cout<<"Default constructor DataDump"<<endl;
  }

  DataDump::DataDump( uint32_t      numPolProduct,
		      uint32_t      numBin,
		      Enum<NetSideband> e_sideband,
		      uint32_t      numBaseband,
		      uint32_t      numAnt,
		      CorrelationMode   correlationMode,
		      uint64_t         time,
		      uint64_t         timeCentroid,
		      uint64_t         interval, 
		      uint64_t         exposure, 
		      const float*      floatData):
    DataStructure( numPolProduct,
		   numBin,
		   e_sideband,
		   numBaseband,
		   numAnt,
		   correlationMode),
    cuintFlagsPtr_(NULL),
    clonlonActualTimesPtr_(NULL),
    clonlonActualDurationsPtr_(NULL),
    cfloatWeightsPtr_(NULL),
    cfloatZeroLagsPtr_(NULL),
    cfloatAutoDataPtr_(floatData),
    cshortCrossDataPtr_(NULL),
    cintCrossDataPtr_(NULL),
    cfloatCrossDataPtr_(NULL)
  {
    integrationNum_    = 0;
    subintegrationNum_ = 0;
    time_         = time;
    timeCentroid_ = timeCentroid;
    interval_     = interval;
    exposure_     = exposure;

    uintFlagsPtr_             = NULL;
    lonlonActualTimesPtr_     = NULL;
    lonlonActualDurationsPtr_ = NULL;
    floatWeightsPtr_          = NULL;
    floatZeroLagsPtr_         = NULL;
    floatAutoDataPtr_         = NULL;
    shortCrossDataPtr_        = NULL;
    intCrossDataPtr_          = NULL;
    floatCrossDataPtr_        = NULL;

  }


  DataDump::DataDump( uint32_t      numPolProduct,
		      uint32_t      numSpectralPoint,
		      uint32_t      numBin,
		      Enum<NetSideband> e_sideband,
		      uint32_t      numBaseband,
		      uint32_t      numAnt,
		      CorrelationMode   correlationMode,
		      uint64_t         time,
		      uint64_t         timeCentroid, 
		      uint64_t         interval,
		      uint64_t         exposure,
		      const float*             floatData, 
		      const uint32_t* dataFlags):
    DataStructure( numPolProduct,
		   numSpectralPoint,
		   numBin,
		   e_sideband,
		   numBaseband,
		   numAnt,
		   correlationMode),
    cuintFlagsPtr_(dataFlags),
    clonlonActualTimesPtr_(NULL),
    clonlonActualDurationsPtr_(NULL),
    cfloatWeightsPtr_(NULL),
    cfloatZeroLagsPtr_(NULL),
    cfloatAutoDataPtr_(floatData),
    cshortCrossDataPtr_(NULL),
    cintCrossDataPtr_(NULL),
    cfloatCrossDataPtr_(NULL)
  {   
    integrationNum_    = 0;
    subintegrationNum_ = 0;
    time_              = time;
    timeCentroid_      = timeCentroid;  cout<<"timeCentroid="<<timeCentroid<<endl;
    interval_          = interval;       
    exposure_          = exposure;

    uintFlagsPtr_             = NULL;
    lonlonActualTimesPtr_     = NULL;
    lonlonActualDurationsPtr_ = NULL;
    floatWeightsPtr_          = NULL;
    floatZeroLagsPtr_         = NULL;
    floatAutoDataPtr_         = NULL;
    shortCrossDataPtr_        = NULL;
    intCrossDataPtr_          = NULL;
    floatCrossDataPtr_        = NULL;
  }

  DataDump::DataDump( vector<vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
		      vector<vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		      vector<vector<uint32_t> >       vv_numBin,            // /bb/spw
		      vector<vector<Enum<NetSideband> > > vv_e_sideband,        // /bb/spw
		      uint32_t                        numApc,
		      vector<uint32_t>                v_numSpectralWindow,  // /bb
		      uint32_t                        numBaseband,
		      uint32_t                        numAnt,
		      CorrelationMode                     correlationMode,
		      uint64_t                           time,
		      uint64_t                           timeCentroid,  
		      uint64_t                           interval,
		      uint64_t                           exposure,  
		      const float*                        floatData, 
		      const uint32_t*                 dataFlags):
    DataStructure( vv_numAutoPolProduct,
		   vv_numSpectralPoint,
		   vv_numBin,
		   vv_e_sideband,
		   numApc,
		   v_numSpectralWindow,
		   numBaseband,
		   numAnt,
		   correlationMode),
    integrationNum_(0),
    subintegrationNum_(0),
    time_(time),
    timeCentroid_(timeCentroid),
    interval_(interval),
    exposure_(exposure),
    cuintFlagsPtr_(dataFlags),
    clonlonActualTimesPtr_(NULL),
    clonlonActualDurationsPtr_(NULL),
    cfloatWeightsPtr_(NULL),
    cfloatZeroLagsPtr_(NULL),
    cfloatAutoDataPtr_(floatData),
    cshortCrossDataPtr_(NULL),
    cintCrossDataPtr_(NULL),
    cfloatCrossDataPtr_(NULL)
  {
    uintFlagsPtr_             = NULL;
    lonlonActualTimesPtr_     = NULL;
    lonlonActualDurationsPtr_ = NULL;
    floatWeightsPtr_          = NULL;
    floatZeroLagsPtr_         = NULL;
    floatAutoDataPtr_         = NULL;
    shortCrossDataPtr_        = NULL;
    intCrossDataPtr_          = NULL;
    floatCrossDataPtr_        = NULL;
  }

  DataDump::DataDump( vector<vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
		      vector<vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		      vector<vector<uint32_t> >       vv_numBin,            // /bb/spw
		      vector<vector<Enum<NetSideband> > > vv_e_sideband,        // /bb/spw
		      uint32_t                        numApc,
		      vector<uint32_t>                v_numSpectralWindow,  // /bb
		      uint32_t                        numBaseband,
		      uint32_t                        numAnt,
		      CorrelationMode                     correlationMode,
		      uint64_t                           time,
		      uint64_t                           timeCentroid,  
		      uint64_t                           interval,
		      uint64_t                           exposure,  
		      const float*                        floatData):
    DataStructure( vv_numAutoPolProduct,    
		   vv_numSpectralPoint,
		   vv_numBin,
		   vv_e_sideband,
		   numApc,
		   v_numSpectralWindow,
		   numBaseband,
		   numAnt,
		   correlationMode),
    integrationNum_(0),
    subintegrationNum_(0),
    time_(time),
    timeCentroid_(timeCentroid),
    interval_(interval),
    exposure_(exposure),
    cuintFlagsPtr_(NULL),
    clonlonActualTimesPtr_(NULL),
    clonlonActualDurationsPtr_(NULL),
    cfloatWeightsPtr_(NULL),
    cfloatZeroLagsPtr_(NULL),
    cfloatAutoDataPtr_(floatData),
    cshortCrossDataPtr_(NULL),
    cintCrossDataPtr_(NULL),
    cfloatCrossDataPtr_(NULL)
  {
    uintFlagsPtr_             = NULL;
    lonlonActualTimesPtr_     = NULL;
    lonlonActualDurationsPtr_ = NULL;
    floatWeightsPtr_          = NULL;
    floatZeroLagsPtr_         = NULL;
    floatAutoDataPtr_         = NULL;
    shortCrossDataPtr_        = NULL;
    intCrossDataPtr_          = NULL;
    floatCrossDataPtr_        = NULL;
  }

  DataDump::DataDump( vector<vector<uint32_t> >       vv_numCrossPolProduct,// /bb/spw
		      vector<vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
		      vector<vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		      vector<vector<uint32_t> >       vv_numBin,            // /bb/spw
		      vector<vector<Enum<NetSideband> > > vv_e_sideband,        // /bb/spw
		      uint32_t                        numApc,
		      vector<uint32_t>                v_numSpectralWindow,  // /bb
		      uint32_t                        numBaseband,
		      uint32_t                        numAnt,
		      CorrelationMode                     correlationMode,
		      uint64_t                           time,
		      uint64_t                           timeCentroid,  
		      uint64_t                           interval, 
		      uint64_t                           exposure):
    DataStructure( vv_numCrossPolProduct,
		   vv_numAutoPolProduct,
		   vv_numSpectralPoint,
		   vv_numBin,
		   vv_e_sideband,
		   numApc,
		   v_numSpectralWindow,
		   numBaseband,
		   numAnt,
		   correlationMode),
    integrationNum_(0),
    subintegrationNum_(0),
    time_(time),
    timeCentroid_(timeCentroid),
    interval_(interval),
    exposure_(exposure),
    es_flagsAxes_(), 
    es_actualTimesAxes_(),   
    es_actualDurationsAxes_(),   
    es_weightsAxes_(), 
    es_zeroLagsAxes_(), 
    es_autoDataAxes_(), 
    es_crossDataAxes_(),
    numFlags_(0),     
    numActualTimes_(0), 
    numActualDurations_(0), 
    numWeights_(0),     
    numZeroLags_(0),     
    numAutoData_(0),     
    numCrossData_(0),
    cuintFlagsPtr_(NULL),
    clonlonActualTimesPtr_(NULL),
    clonlonActualDurationsPtr_(NULL),
    cfloatWeightsPtr_(NULL),
    cfloatZeroLagsPtr_(NULL),
    cfloatAutoDataPtr_(NULL),
    cshortCrossDataPtr_(NULL),
    cintCrossDataPtr_(NULL),
    cfloatCrossDataPtr_(NULL)
  {

    // By default the scale factor is set to 1 for all spw. It must be updated subsequently if required
    // using the method setScaleFactor().
    if(vv_numCrossPolProduct.size()){
      for(uint32_t nbb=0; nbb<numBaseband; nbb++){
	vector<float> v_f;
	for(uint32_t nspw=0; nspw<vv_numCrossPolProduct[nbb].size(); nspw++)v_f.push_back(1.0);
	vv_scaleFactor_.push_back(v_f);
      }
    }
										
    uintFlagsPtr_             = NULL;
    lonlonActualTimesPtr_     = NULL;
    lonlonActualDurationsPtr_ = NULL;
    floatWeightsPtr_          = NULL;
    floatZeroLagsPtr_         = NULL;
    floatAutoDataPtr_         = NULL;
    shortCrossDataPtr_        = NULL;
    intCrossDataPtr_          = NULL;
    floatCrossDataPtr_        = NULL;

  }


  DataDump::DataDump( vector<vector<uint32_t> >       vv_numCrossPolProduct,	// /bb/spw
		      vector<vector<uint32_t> >       vv_numAutoPolProduct,	// /bb/spw
		      vector<vector<uint32_t> >       vv_numSpectralPoint,	// /bb/spw
		      vector<vector<uint32_t> >       vv_numBin,	// /bb/spw
		      vector<vector<Enum<NetSideband> > > vv_e_sideband,	// /bb/spw
		      uint32_t          numApc,
		      vector<uint32_t>  v_numSpectralWindow,	// /bb
		      uint32_t          numBaseband,
		      uint32_t          numAnt,
		      CorrelationMode   correlationMode,
		      uint64_t          time,
		      uint64_t          timeCentroid,  
		      uint64_t          interval,
		      uint64_t          exposure,
		      uint32_t          /*numVal*/, // comment to avoid the unused parameter warning
 		      const int*        /*crossData*/, // comment to avoid the unused parameter warning
		      uint32_t          numAutoData,
		      const float*      autoData,
		      uint32_t          numFlags,
		      const uint32_t*                 flags):
    DataStructure( vv_numCrossPolProduct,
		   vv_numAutoPolProduct,
		   vv_numSpectralPoint,
		   vv_numBin,
		   vv_e_sideband,
		   numApc,
		   v_numSpectralWindow,
		   numBaseband,
		   numAnt,
		   correlationMode),
    integrationNum_(0),
    subintegrationNum_(0),
    time_(time),
    timeCentroid_(timeCentroid),
    interval_(interval),
    exposure_(exposure),
    cuintFlagsPtr_(NULL),
    clonlonActualTimesPtr_(NULL),
    clonlonActualDurationsPtr_(NULL),
    cfloatWeightsPtr_(NULL),
    cfloatZeroLagsPtr_(NULL),
    cfloatAutoDataPtr_(NULL),
    cshortCrossDataPtr_(NULL),
    cintCrossDataPtr_(NULL),
    cfloatCrossDataPtr_(NULL)
  {
    uintFlagsPtr_     = new uint32_t[numFlags]; for(uint32_t n=0; n<numFlags; n++)uintFlagsPtr_[n] = flags[n];
    lonlonActualTimesPtr_     = NULL;
    lonlonActualDurationsPtr_ = NULL;
    floatWeightsPtr_          = NULL;

    floatZeroLagsPtr_         = NULL;

    floatAutoDataPtr_ = new float[numAutoData]; for(uint32_t n=0; n<numAutoData; n++)floatAutoDataPtr_[n] = autoData[n];
    
    shortCrossDataPtr_        = NULL;
    intCrossDataPtr_          = NULL;
    floatCrossDataPtr_        = NULL;

  }


  DataDump::~DataDump()
  {
    bool coutest=false;
    if(coutest){
      cout<<"Destructor DataDump"<<endl;
      if(uintFlagsPtr_             ) cout<<"delete uintFlagsPtr_"<<endl;
      if(lonlonActualTimesPtr_     ) cout<<"delete lonlonActualTimesPtr_"<<endl;
      if(lonlonActualDurationsPtr_ ) cout<<"delete lonlonActualDurationsPtr_"<<endl;
      if(floatWeightsPtr_          ) cout<<"delete floatWeightsPtr_"<<endl;
      if(floatZeroLagsPtr_         ) cout<<"delete floatZeroLagsPtr_"<<endl;
      if(floatAutoDataPtr_         ) cout<<"delete floatAutoDataPtr_"<<endl;
      if(shortCrossDataPtr_        ) cout<<"delete shortCrossDataPtr_"<<endl;
      if(intCrossDataPtr_          ) cout<<"delete intCrossDataPtr_"<<endl;
      if(floatCrossDataPtr_        ) cout<<"delete floatCrossDataPtr_"<<endl;
    }
    if(uintFlagsPtr_             ) delete uintFlagsPtr_;
    if(lonlonActualTimesPtr_     ) delete lonlonActualTimesPtr_;
    if(lonlonActualDurationsPtr_ ) delete lonlonActualDurationsPtr_;
    if(floatWeightsPtr_          ) delete floatWeightsPtr_;
    if(floatZeroLagsPtr_         ) delete floatZeroLagsPtr_;
    if(floatAutoDataPtr_         ) delete floatAutoDataPtr_;
    if(shortCrossDataPtr_        ) delete shortCrossDataPtr_;
    if(intCrossDataPtr_          ) delete intCrossDataPtr_;
    if(floatCrossDataPtr_        ) delete floatCrossDataPtr_;
  }

  DataDump::DataDump(const DataDump& a) : DataStructure(a)
  {
    cout<<"Copy constructor DataDump (deep copy)"<<endl;
    cout<<"ici a.uintFlagsPtr_="<<a.uintFlagsPtr_<<endl;
    vv_numCrossPolProduct_= a.vv_numCrossPolProduct_;
    vv_numAutoPolProduct_ = a.vv_numAutoPolProduct_;
    vv_numSpectralPoint_  = a.vv_numSpectralPoint_;
    vv_numBin_            = a.vv_numBin_;
    vv_e_sideband_        = a.vv_e_sideband_;
    numApc_               = a.numApc_;
    v_numSpectralWindow_  = a.v_numSpectralWindow_;
    numBaseband_          = a.numBaseband_;
    numAnt_               = a.numAnt_;
    correlationMode_      = a.correlationMode_;
    axisSequence_         = a.axisSequence_;
    v_minSize_            = a.v_minSize_;
    v_maxSize_            = a.v_maxSize_;

    integrationNum_       = a.integrationNum_;
    subintegrationNum_    = a.subintegrationNum_;
    time_                 = a.time_;
    timeCentroid_         = a.timeCentroid_;
    interval_             = a.interval_;
    exposure_             = a.exposure_;

    vv_scaleFactor_       = a.vv_scaleFactor_;

    es_flagsAxes_           = a.es_flagsAxes_; 
    es_actualTimesAxes_     = a.es_actualTimesAxes_;   
    es_actualDurationsAxes_ = a.es_actualDurationsAxes_;   
    es_weightsAxes_         = a.es_weightsAxes_; 

    es_zeroLagsAxes_        = a.es_zeroLagsAxes_; 

    es_autoDataAxes_        = a.es_autoDataAxes_; 
    es_crossDataAxes_       = a.es_crossDataAxes_;


    numFlags_               = a.numFlags_;
    numActualTimes_         = a.numActualTimes_;
    numActualDurations_     = a.numActualDurations_;
    numWeights_             = a.numWeights_;
    
    numZeroLags_            = a.numZeroLags_;
     
    numAutoData_            = a.numAutoData_; 
    numCrossData_           = a.numCrossData_;
    cout<<"A"<<endl;
    cout<<"a.cuintFlagsPtr_"<<a.cuintFlagsPtr_<<endl;
    cout<<"a.flags()="<<a.flags()<<endl;
    cuintFlagsPtr_              = a.cuintFlagsPtr_;
    cout<<"B"<<endl;
    clonlonActualTimesPtr_      = a.clonlonActualTimesPtr_;
    clonlonActualDurationsPtr_  = a.clonlonActualDurationsPtr_;
    cfloatWeightsPtr_           = a.cfloatWeightsPtr_;

    cfloatZeroLagsPtr_          = a.cfloatZeroLagsPtr_;

    cfloatAutoDataPtr_          = a.cfloatAutoDataPtr_;
    cshortCrossDataPtr_         = a.cshortCrossDataPtr_;
    cintCrossDataPtr_           = a.cintCrossDataPtr_;
    cout<<"C"<<endl;
    cout<<"a.uintFlagsPtr_"<<a.uintFlagsPtr_<<endl;
    if(a.uintFlagsPtr_){
      cout<<"a.numFlags_="<<a.numFlags_<<endl;
      uintFlagsPtr_ = new uint32_t[a.numFlags_]; 
      for(uint32_t n=0; n<a.numFlags_; n++) uintFlagsPtr_[n] = a.uintFlagsPtr_[n]; }
    cout<<"D"<<endl;
    if(a.lonlonActualTimesPtr_){
      cout<<"a.numActualTimes_="<<a.numActualTimes_<<endl;
      lonlonActualTimesPtr_ = new int64_t[numActualTimes_]; 
      for(uint32_t n=0; n<numActualTimes_; n++)lonlonActualTimesPtr_[n] = a.lonlonActualTimesPtr_[n]; }
    if(a.lonlonActualDurationsPtr_){
      lonlonActualDurationsPtr_ = new int64_t[numActualDurations_]; 
      for(uint32_t n=0; n<numActualDurations_; n++)lonlonActualDurationsPtr_[n] = a.lonlonActualDurationsPtr_[n]; }
    if(a.floatWeightsPtr_){
      floatWeightsPtr_ = new float[numWeights_]; 
      for(uint32_t n=0; n<numWeights_; n++)floatWeightsPtr_[n] = a.floatWeightsPtr_[n]; }

    if(a.floatZeroLagsPtr_){
      floatZeroLagsPtr_ = new float[numZeroLags_]; 
      for(uint32_t n=0; n<numZeroLags_; n++)floatZeroLagsPtr_[n] = a.floatZeroLagsPtr_[n]; }
    if(a.shortCrossDataPtr_){
      cout<<"a.numCrossData_="<<a.numCrossData_<<endl;
      cout<<"numCrossData_="<<numCrossData_<<endl;
      shortCrossDataPtr_ = new short int[numCrossData_]; 
      for(uint32_t n=0; n<numCrossData_; n++)shortCrossDataPtr_[n] = a.shortCrossDataPtr_[n]; }
    if(a.intCrossDataPtr_){
      intCrossDataPtr_ = new int[numCrossData_]; 
      for(uint32_t n=0; n<numCrossData_; n++)intCrossDataPtr_[n] = a.intCrossDataPtr_[n]; }
    if(a.floatCrossDataPtr_){
      floatCrossDataPtr_ = new float[numCrossData_]; 
      for(uint32_t n=0; n<numCrossData_; n++)floatCrossDataPtr_[n] = a.floatCrossDataPtr_[n]; }
    cout<<"E"<<endl;
    cout<<"floatAutoDataPtr_ ="<<floatAutoDataPtr_<<endl;
    cout<<"a.floatAutoDataPtr_ ="<<a.floatAutoDataPtr_<<endl;
    if(a.floatAutoDataPtr_){
      cout<<"a.numAutoData_="<<a.numAutoData_<<endl;
      cout<<"numAutoData_="<<numAutoData_<<endl;
      floatAutoDataPtr_ = new float[numAutoData_]; 
      for(uint32_t n=0; n<numAutoData_; n++)floatAutoDataPtr_[n] = a.floatAutoDataPtr_[n]; }

     cout<<"F"<<endl;
    
  }

  DataDump& DataDump::operator = (const DataDump & a)
  {
    if(this == &a)return *this;
    vv_numCrossPolProduct_= a.vv_numCrossPolProduct_;
    vv_numAutoPolProduct_ = a.vv_numAutoPolProduct_;
    vv_numSpectralPoint_  = a.vv_numSpectralPoint_;
    vv_numBin_            = a.vv_numBin_;
    vv_e_sideband_        = a.vv_e_sideband_;
    numApc_               = a.numApc_;
    v_numSpectralWindow_  = a.v_numSpectralWindow_;
    numBaseband_          = a.numBaseband_;
    numAnt_               = a.numAnt_;
    correlationMode_      = a.correlationMode_;
    axisSequence_         = a.axisSequence_;
    v_minSize_            = a.v_minSize_;
    v_maxSize_            = a.v_maxSize_;

    integrationNum_       = a.integrationNum_;
    subintegrationNum_    = a.subintegrationNum_;
    time_                 = a.time_;
    timeCentroid_         = a.timeCentroid_;
    interval_             = a.interval_;
    exposure_             = a.exposure_;

    return *this;
  }

  // shape unchanged
  DataDump DataDump::operator - (const DataDump &rhs)
  {
    uint32_t numD=numAutoData();
    if(rhs.numAutoData()!=numD){
      ostringstream os;
      os << "Cannot subtract a data dump which has " << rhs.numAutoData()
	 <<" data from an other which has "<<numD
	 <<" data";
      Error( FATAL, os.str());
    }
    uint64_t st1 = (time_-interval_)/(uint64_t)2;
    uint64_t et1 = (time_+interval_)/(uint64_t)2; 
    uint64_t st2 = (rhs.time()-rhs.interval())/(uint64_t)2;
    uint64_t et2 = (rhs.time()+rhs.interval())/(uint64_t)2; 
    if( st1>et2 || et1<st2)
      Error( WARNING, (char *) "data difference of dumps overlaping in time");
    uint64_t st,et;
    if(st1<st2)
      st = st1;
    else
      st = st2;
    if(et1<et2)
      et = et2;
    else
      et = et1;

    uint64_t timeCentroid;
    if(interval_==rhs.interval())
      timeCentroid = (time_+rhs.time())/(uint64_t)2;
    else{
      double w=interval_/rhs.interval();
      timeCentroid = (uint64_t) ( (w*time_ + rhs.time())/(w+1.) );
    }

    uint32_t       numF = numD;                                  // TODO (more complex than that!)
    float*             diffFloatData = new float[numD];
    uint32_t*      sumUintFlags  = new uint32_t[numF];

    if(floatAutoDataPtr_){
      if(rhs.floatAutoDataPtr_){
	for(uint32_t nd=0; nd<numD; nd++)
	  diffFloatData[nd] = floatAutoDataPtr_[nd]-rhs.floatAutoDataPtr_[nd];
	for(uint32_t nf=0; nf<numF; nf++){
	  if(uintFlagsPtr_[nf]!=rhs.uintFlagsPtr_[nf])
	    sumUintFlags[nf]=uintFlagsPtr_[nf]+rhs.uintFlagsPtr_[nf];  // TODO (more complex than that!)
	  else
	    sumUintFlags[nf]=uintFlagsPtr_[nf];
	}
      }else{
	for(uint32_t nd=0; nd<numD; nd++)
	  diffFloatData[nd] = floatAutoDataPtr_[nd]-rhs.cfloatAutoDataPtr_[nd];
	for(uint32_t nf=0; nf<numF; nf++){
	  if(uintFlagsPtr_[nf]!=rhs.cuintFlagsPtr_[nf])
	    sumUintFlags[nf]=uintFlagsPtr_[nf]+rhs.cuintFlagsPtr_[nf];  // TODO (more complex than that!)
	  else
	    sumUintFlags[nf]=uintFlagsPtr_[nf];
	}
      }
    }else{
      if(rhs.floatAutoDataPtr_){
	for(uint32_t nd=0; nd<numD; nd++)
	  diffFloatData[nd] = cfloatAutoDataPtr_[nd]-rhs.floatAutoDataPtr_[nd];
	for(uint32_t nf=0; nf<numF; nf++){
	  if(cuintFlagsPtr_[nf]!=rhs.uintFlagsPtr_[nf])
	    sumUintFlags[nf]=cuintFlagsPtr_[nf]+rhs.uintFlagsPtr_[nf];  // TODO (more complex than that!)
	  else
	    sumUintFlags[nf]=cuintFlagsPtr_[nf];
	}
      }else{
	for(uint32_t nd=0; nd<numD; nd++)
	  diffFloatData[nd] = cfloatAutoDataPtr_[nd]-rhs.cfloatAutoDataPtr_[nd];
	for(uint32_t nf=0; nf<numF; nf++){
	  if(cuintFlagsPtr_[nf]!=rhs.cuintFlagsPtr_[nf])
	    sumUintFlags[nf]=cuintFlagsPtr_[nf]+rhs.cuintFlagsPtr_[nf];  // TODO (more complex than that!)
	  else
	    sumUintFlags[nf]=cuintFlagsPtr_[nf];
	}
      }
    }    
    uint32_t numV=0;  // TODO
    int*         diffCorrData=NULL;
    return DataDump( vv_numCrossPolProduct_,
		     vv_numAutoPolProduct_,
		     vv_numSpectralPoint_,
		     vv_numBin_,
		     vv_e_sideband_,
		     numApc_,
		     v_numSpectralWindow_,
		     numBaseband_,
		     numAnt_,
		     correlationMode_,
		     (time_+rhs.time())/(uint64_t) 2,
		     timeCentroid,
		     et-st,
		     exposure_+rhs.exposure(),
		     numV, diffCorrData,
		     numD, diffFloatData,
		     numF, sumUintFlags
		     );
  }

  DataDump DataDump::operator + (const DataDump &rhs)
  {
    uint32_t numD=numAutoData();
    if(rhs.numAutoData()!=numD){
      ostringstream os;
      os << "Cannot add a data dump which has " << numD
	 <<" data with an other which has "<<rhs.numAutoData()
	 <<" data";
      Error( FATAL, os.str());
    }
    uint64_t st1 = (time_-interval_)/(uint64_t)2;
    uint64_t et1 = (time_+interval_)/(uint64_t)2; 
    uint64_t st2 = (rhs.time()-rhs.interval())/(uint64_t)2;
    uint64_t et2 = (rhs.time()+rhs.interval())/(uint64_t)2; 
    if( st1>et2 || et1<st2)
      Error( WARNING, (char *) "sum of data dumps which overlap in time");
    uint64_t st,et;
    if(st1<st2)
      st = st1;
    else
      st = st2;
    if(et1<et2)
      et = et2;
    else
      et = et1;

    uint64_t timeCentroid;
    if(interval_==rhs.interval())
      timeCentroid = (time_+rhs.time())/(uint64_t)2;
    else{
      double w=interval_/rhs.interval();
      timeCentroid = (uint64_t) ( (w*time_ + rhs.time())/(w+1.) );
    }
    uint32_t numF = numD;                                           // TODO (more complex than that!)
    float*             sumFloatData = new float[numD];
    uint32_t*      sumUintFlags = new uint32_t[numF];

    if(floatAutoDataPtr_){
      if(rhs.floatAutoDataPtr_){
	for(uint32_t nd=0; nd<numD; nd++)
	  sumFloatData[nd] = 0.5*(floatAutoDataPtr_[nd]+rhs.floatAutoDataPtr_[nd]);
	for(uint32_t nf=0; nf<numF; nf++){
	  if(uintFlagsPtr_[nf]!=rhs.uintFlagsPtr_[nf])
	    sumUintFlags[nf]=uintFlagsPtr_[nf]+rhs.uintFlagsPtr_[nf];  // TODO (more complex than that!)
	  else
	    sumUintFlags[nf]=uintFlagsPtr_[nf];
	}
      }else{
	for(uint32_t nd=0; nd<numD; nd++)
	  sumFloatData[nd] = 0.5*(floatAutoDataPtr_[nd]+rhs.cfloatAutoDataPtr_[nd]);
	for(uint32_t nf=0; nf<numF; nf++){
	  if(uintFlagsPtr_[nf]!=rhs.cuintFlagsPtr_[nf])
	    sumUintFlags[nf]=uintFlagsPtr_[nf]+rhs.cuintFlagsPtr_[nf];  // TODO (more complex than that!)
	  else
	    sumUintFlags[nf]=uintFlagsPtr_[nf];
	}
      }
    }else{
      if(rhs.floatAutoDataPtr_){
	for(uint32_t nd=0; nd<numD; nd++)
	  sumFloatData[nd] = 0.5*(cfloatAutoDataPtr_[nd]+rhs.floatAutoDataPtr_[nd]);
	for(uint32_t nf=0; nf<numF; nf++){
	  if(cuintFlagsPtr_[nf]!=rhs.uintFlagsPtr_[nf])
	    sumUintFlags[nf]=cuintFlagsPtr_[nf]+rhs.uintFlagsPtr_[nf];  // TODO (more complex than that!)
	  else
	    sumUintFlags[nf]=cuintFlagsPtr_[nf];
	}
      }else{
	for(uint32_t nd=0; nd<numD; nd++)
	  sumFloatData[nd] = 0.5*(cfloatAutoDataPtr_[nd]+rhs.cfloatAutoDataPtr_[nd]);
	for(uint32_t nf=0; nf<numF; nf++){
	  if(cuintFlagsPtr_[nf]!=rhs.cuintFlagsPtr_[nf])
	    sumUintFlags[nf]=cuintFlagsPtr_[nf]+rhs.cuintFlagsPtr_[nf];  // TODO (more complex than that!)
	  else
	    sumUintFlags[nf]=cuintFlagsPtr_[nf];
	}
      }
    }
    uint32_t numVal=0;  // TODO
    int*         sumVisData=NULL;
    return DataDump (vv_numCrossPolProduct_,
		     vv_numAutoPolProduct_,
		     vv_numSpectralPoint_,
		     vv_numBin_,
		     vv_e_sideband_,
		     numApc_,
		     v_numSpectralWindow_,
		     numBaseband_,
		     numAnt_,
		     correlationMode_,
		     (time_+rhs.time())/(uint64_t) 2,
		     timeCentroid,
		     et-st,
		     exposure_+rhs.exposure(),
		     numVal, sumVisData,
		     numD, sumFloatData,
		     numF, sumUintFlags
		     );
  }


  // operations modifying the structure
//   DataDump      DataDump::spectralAverage()
//   {

//   }

  // Attachers and importers

  void DataDump::attachFlags( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, // comment to avoid the unused parameter warning 
			      uint32_t numData, const uint32_t* flagsPtr){
    cuintFlagsPtr_ = flagsPtr; 
    es_flagsAxes_  = es_an; 
    numFlags_      = numData; 
  }
  void DataDump::importFlags( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, // comment to avoid the unused parameter warning 
			      uint32_t numData, const uint32_t* flagsPtr){
    cuintFlagsPtr_ = NULL;
    if(uintFlagsPtr_){
      if(numFlags_!=numData){
	delete uintFlagsPtr_;
	uintFlagsPtr_ = new uint32_t[numData];
      }
    }else{
      uintFlagsPtr_ = new uint32_t[numData];
    }
    for(uint32_t n=0; n<numData; n++)uintFlagsPtr_[n] = flagsPtr[n]; 
    es_flagsAxes_ = es_an;
    numFlags_     = numData; 
  }

  void DataDump::attachActualTimes( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, // comment to avoid the unused parameter warning 
				    uint32_t numData, const int64_t * actualTimesPtr){
    clonlonActualTimesPtr_ = actualTimesPtr; 
    es_actualTimesAxes_    = es_an;
    numActualTimes_        = numData;
  }
  void DataDump::importActualTimes( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, // comment to avoid the unused parameter warning 
				    uint32_t numData, const int64_t * actualTimesPtr){
    clonlonActualTimesPtr_ = NULL;
    if(lonlonActualTimesPtr_){
      if(numActualTimes_!=numData){
	delete lonlonActualTimesPtr_;
	lonlonActualTimesPtr_ = new int64_t[numData];
      }
    }else{
      lonlonActualTimesPtr_ = new int64_t[numData];
    }
    for(uint32_t n=0; n<numData; n++)lonlonActualTimesPtr_[n] = actualTimesPtr[n]; 
    es_actualTimesAxes_ = es_an;
    numActualTimes_     = numData; 
  }


  void DataDump::attachActualDurations( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, // comment to avoid the unused parameter warning 
					uint32_t numData, const int64_t * actualDurationsPtr){
    clonlonActualDurationsPtr_ = actualDurationsPtr; 
    es_actualDurationsAxes_     = es_an;
    numActualDurations_         = numData;
  }
  void DataDump::importActualDurations( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, // comment to avoid the unused parameter warning 
					uint32_t numData, const int64_t * actualDurationsPtr){
    clonlonActualDurationsPtr_ = NULL;
    if(lonlonActualDurationsPtr_){
      if(numActualDurations_!=numData){
	delete lonlonActualDurationsPtr_;
	lonlonActualDurationsPtr_ = new int64_t[numData];
      }
    }else{
      lonlonActualDurationsPtr_ = new int64_t[numData];
    }
    for(uint32_t n=0; n<numData; n++)lonlonActualDurationsPtr_[n] = actualDurationsPtr[n]; 
    es_actualDurationsAxes_ = es_an;
    numActualDurations_     = numData; 
  }

  void DataDump::attachZeroLags( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, // comment to avoid the unused parameter warning 
				 uint32_t numData, const float* zeroLagsPtr){
    cfloatZeroLagsPtr_ = zeroLagsPtr; 
    es_zeroLagsAxes_   = es_an;
    numZeroLags_       = numData;
  }
  void DataDump::importZeroLags( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, // comment to avoid the unused parameter warning
				 uint32_t numData, const float* zeroLagsPtr){
    cfloatZeroLagsPtr_ = NULL;
    if(floatZeroLagsPtr_){
      if(numZeroLags_!=numData){
	delete floatZeroLagsPtr_;
	floatZeroLagsPtr_ = new float[numData];
      }
    }else{
      floatZeroLagsPtr_ = new float[numData];
    }
    for(uint32_t n=0; n<numData; n++)floatZeroLagsPtr_[n] = zeroLagsPtr[n]; 
    es_zeroLagsAxes_ = es_an;
    numZeroLags_     = numData;
  }

  void DataDump::attachAutoData( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, 
				 uint32_t numData, const float* autoDataPtr){
    cfloatAutoDataPtr_ = autoDataPtr; 
    es_autoDataAxes_   = es_an;
    numAutoData_       = numData;

    if(uintFlagsPtr_)cout<<"there are imported flags"<<endl;
    //cout<<"uintFlagsPtr_="<<    uintFlagsPtr_<<endl;

  }
  void DataDump::importAutoData( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, 
				 uint32_t numData, const float* autoDataPtr){
    cfloatAutoDataPtr_ = NULL;
    if(floatAutoDataPtr_){
      if(numAutoData_!=numData){
	delete floatAutoDataPtr_;
	floatAutoDataPtr_ = new float[numData];
      }
    }else{
      floatAutoDataPtr_ = new float[numData];
    }
    for(uint32_t n=0; n<numData; n++)floatAutoDataPtr_[n] = autoDataPtr[n]; 
    es_autoDataAxes_ = es_an;
    numAutoData_     = numData;
  }

  void DataDump::attachCrossData( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, 
				  uint32_t numData, const short int* crossDataPtr){
    cintCrossDataPtr_   = NULL; 
    cfloatCrossDataPtr_ = NULL;
    cshortCrossDataPtr_ = crossDataPtr; 
    es_crossDataAxes_   = es_an;
    numCrossData_       = numData;
  }
  void DataDump::importCrossData( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, 
				  uint32_t numData, const short int* crossDataPtr){
    cintCrossDataPtr_   = NULL; 
    cfloatCrossDataPtr_ = NULL;
    cshortCrossDataPtr_ = NULL;
    if(shortCrossDataPtr_){
      if(numCrossData_!=numData){
	delete shortCrossDataPtr_;
	shortCrossDataPtr_ = new short int[numData];
      }
    }else{
      shortCrossDataPtr_ = new short int[numData];
    }
    for(uint32_t n=0; n<numData; n++)shortCrossDataPtr_[n] = crossDataPtr[n]; 
    es_crossDataAxes_ = es_an;
    numCrossData_     = numData;
  }

  void DataDump::attachCrossData( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, 
				  uint32_t numData, const  int* crossDataPtr){
    cshortCrossDataPtr_ = NULL; 
    cintCrossDataPtr_   = crossDataPtr; 
    cfloatCrossDataPtr_ = NULL;
    es_crossDataAxes_   = es_an;
    numCrossData_       = numData;
  }
  void DataDump::importCrossData( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, 
				  uint32_t numData, const int* crossDataPtr){
    cshortCrossDataPtr_ = NULL; 
    cintCrossDataPtr_   = NULL;
    cfloatCrossDataPtr_ = NULL;
    if(intCrossDataPtr_){
      if(numCrossData_!=numData){
	delete intCrossDataPtr_;
	intCrossDataPtr_ = new int[numData];
      }
    }else{
      intCrossDataPtr_ = new int[numData];
    }
    for(uint32_t n=0; n<numData; n++)intCrossDataPtr_[n] = crossDataPtr[n]; 
    es_crossDataAxes_ = es_an;
    numCrossData_     = numData;
  }


  void DataDump::attachCrossData( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, 
				  uint32_t numData, const float* crossDataPtr){
    cshortCrossDataPtr_ = NULL; 
    cintCrossDataPtr_   = NULL;
    cfloatCrossDataPtr_ = crossDataPtr; 
    es_crossDataAxes_   = es_an;
    numCrossData_       = numData;
  }
  void DataDump::importCrossData( uint32_t /*declaredSize*/, EnumSet<AxisName> es_an, 
				  uint32_t numData, const float* crossDataPtr){
    cshortCrossDataPtr_ = NULL; 
    cintCrossDataPtr_   = NULL;
    cfloatCrossDataPtr_ = NULL;
    if(floatCrossDataPtr_){
      if(numCrossData_!=numData){
	delete floatCrossDataPtr_;
	floatCrossDataPtr_ = new float[numData];
      }
    }else{
      floatCrossDataPtr_ = new float[numData];
    }
    for(uint32_t n=0; n<numData; n++)floatCrossDataPtr_[n] = crossDataPtr[n]; 
    es_crossDataAxes_ = es_an;
    numCrossData_     = numData;
  }

  void DataDump::setScaleFactor(vector<vector<float> > vv_scaleFactor){
    if(vv_scaleFactor.size()!=vv_numCrossPolProduct_.size())
      Error(FATAL,
	    (char *) "vv_scaleFactor, of size %d, does not have  a size equal to %d (i.e. numBaseband)",
	    vv_numCrossPolProduct_.size(),numBaseband_);
    for(uint32_t nbb=0; nbb<vv_numCrossPolProduct_.size(); nbb++)
      if(vv_scaleFactor[nbb].size()!=vv_numCrossPolProduct_[nbb].size())
	Error(FATAL,
	      (char *) "vv_scaleFactor[%d], of size %d, does not have  a size equal to the nb on spw for that baseband",
	    nbb,vv_numCrossPolProduct_[nbb].size());
    vv_scaleFactor_ = vv_scaleFactor;
    // linearization for fast access:
    for(uint32_t nbb=0; nbb<vv_scaleFactor.size(); nbb++){
      for(uint32_t nspw=0; nspw<vv_scaleFactor[nbb].size(); nspw++){
	v_scaleFactor_.push_back(vv_scaleFactor[nbb][nspw]);
      }
    }
    return;
  }

  // Setters to identify the project structure context:
  uint32_t DataDump::setIntegration(uint32_t integNum){
    if(integrationNum_){
      if(integNum==integrationNum_)return integrationNum_;
      Error(WARNING,
	    (char *) "This dump has already been identified with the integration number %d;\n it is not allowed to tag it again",
	    integrationNum_);
      return 0;
    }
    integrationNum_=integNum;
    return integrationNum_;
  }

  uint32_t DataDump::setSubintegration(uint32_t integNum, uint32_t subintegNum){
    if(setIntegration(integNum)){
      if(subintegNum==subintegrationNum_)return integrationNum_;
      if(subintegrationNum_)
	Error(WARNING,
	      (char *) "This dump has already been identified with the subintegration number %d;\n it is not allowed to tag it again",
	      subintegrationNum_);
      subintegrationNum_=subintegNum;
      return subintegNum;
    }
    return 0;
  }

  uint32_t DataDump::setContextUsingProjectPath(string projectPathUri){
    string::size_type i=0, p=0 ;
    //    uint32_t i=0, p=0;
    vector<uint32_t> v_node;
    while(p!=string::npos){
      p = projectPathUri.find("/",i);
      if(p!=string::npos){
	v_node.push_back((uint32_t)atoi(projectPathUri.substr(i,p-i).c_str()));
	i = p+1;
      }else{
	v_node.push_back((uint32_t)atoi(projectPathUri.substr(i,projectPathUri.length()).c_str()));
      }
    }
    if(v_node.size()==5)
      //return setSubintegration(v_node[4],v_node[5]);
      return setSubintegration(v_node[3],v_node[4]);
    else if(v_node.size()==4) 
      // return setIntegration(v_node[4]);
      return setIntegration(v_node[3]);
    return 0;
  }


  // Accessors
  uint64_t    DataDump::time() const
  {
    return time_;
  }

  uint64_t    DataDump::timeCentroid() const
  {
    return timeCentroid_;
  }

  uint64_t    DataDump::interval() const
  {
    return interval_;
  }

  uint64_t    DataDump::exposure() const
  {
    return exposure_;
  }


  uint32_t DataDump::integrationNum()
  {
    return integrationNum_;
  }

  uint32_t DataDump::subintegrationNum()
  {
    return subintegrationNum_;
  }

  float DataDump::scaleFactor(uint32_t nbb, uint32_t nspw){
    if(nbb<vv_scaleFactor_.size()){
      if(nspw<vv_scaleFactor_[nbb].size()){
	return vv_scaleFactor_[nbb][nspw];
      }else{
	Error(FATAL,(char *) "spectral window index %d too large for %d spectral windows for baseband index %d",
	      nspw,vv_scaleFactor_[nbb].size(),nbb);
      }
    }else{
      Error(FATAL,(char *) "baseband index %d too large for %d basebands",
	    nbb,vv_scaleFactor_.size());
    }
    return 0;
  }

  float DataDump::scaleFactor(uint32_t ndd){
    bool coutest=false;
    if (coutest) cout << "size of v_scaleFactor_ = " << v_scaleFactor_.size() << ", ndd = " << ndd << endl;
    return v_scaleFactor_.at(ndd);
  }


  const float*              DataDump::autoData() const{
    if(floatAutoDataPtr_)               // imported data made available read-only
      return floatAutoDataPtr_;
    else
      return cfloatAutoDataPtr_;        // attached data made available read-only
  }

  const short*             DataDump::crossDataShort() const{
    if(shortCrossDataPtr_)
      return shortCrossDataPtr_;
    else
      return cshortCrossDataPtr_;
  }

  const int*              DataDump::crossDataLong() const{
    if(intCrossDataPtr_)
      return intCrossDataPtr_;
    else
      return cintCrossDataPtr_;
  }

  const float*              DataDump::crossDataFloat() const{
    if(floatCrossDataPtr_)
      return floatCrossDataPtr_;
    else
      return cfloatCrossDataPtr_;
  }


  const uint32_t* DataDump::flags() const
  {
    bool coutest=false;
    if(coutest){
      if(uintFlagsPtr_)
	cout<<"return flags owned by datadump"<<endl;
      if(cuintFlagsPtr_)
	cout<<"return flags which have been attached to datadump"<<endl;
    }
    if(uintFlagsPtr_)
      return uintFlagsPtr_;
    else
      return cuintFlagsPtr_;
    return NULL;
  }


  const int64_t*     DataDump::actualTimes() const
  {
    if(lonlonActualTimesPtr_)
      return lonlonActualTimesPtr_;
    else
      return clonlonActualTimesPtr_;
    return NULL;
  }

  const int64_t*         DataDump::actualDurations() const
  {
    bool coutest=false;
    if(coutest){
      if(lonlonActualDurationsPtr_)
	cout<<"return actualDurations owned by datadump"<<endl;
      if(clonlonActualDurationsPtr_)
	cout<<"return actualDurations which have been attached to datadump"<<endl;
    }
    if(lonlonActualDurationsPtr_)
      return lonlonActualDurationsPtr_;
    else
      return clonlonActualDurationsPtr_;
    return NULL;
  }

  const float*             DataDump::zeroLags() const
  {
    if(floatZeroLagsPtr_)
      return floatZeroLagsPtr_;
    else
      return cfloatZeroLagsPtr_;
    return NULL;
  }

  uint32_t DataDump::floatData(vector<vector<vector<float> > >&)
  {
  
//     for(uint32_t nbb=0; nbb<numBaseband_; nbb++){
//       v_sizes[nbb].push_back(numSpectralWindow[nbb]);
//     }
//     if(numPolProduct>1)     v_sizes.push_back(numPolProduct);
//     if(numSpectralPoint>1)  v_sizes.push_back(numSpectralPoint);
//     if(numBin>1)            v_sizes.push_back(numBin);
//     if(numApc>1)            v_sizes.push_back(numApc);
//     if(numSpectralWindow>1) v_sizes.push_back(numSpectralWindow);
//     if(numBaseband>1)       v_sizes.push_back(numBaseband);
//     if(numAnt>1)            v_sizes.push_back(numAnt);  
    uint32_t dim=0;
    return dim;
  }
  uint32_t DataDump::floatData(vector<vector<vector<float*> > >& /*vvv*/) // comment to avoid the unused parameter warning
  {
    uint32_t dim=0;
    return dim;
  }

  uint32_t DataDump::floatData(vector<vector<vector<vector<float> > > >& /*vvvv*/) // comment to avoid the unused parameter warning
  {
    uint32_t dim=0;
    return dim;
  }


  uint32_t DataDump::floatData(vector<vector<vector<vector<vector<float> > > > >& /*vvvvv*/)
  {
//     uint32_t nd;
//     vvvvvvv.resize(numAnt_);
//     for(uint32_t na=0; na<numAnt_; na++){
//       vvvvvvv[na].resize(numBaseband_);

//       for(uint32_t nbb=0; nbb<numBaseband_; nbb){
// 	vvvvvvv[na][nbb].resize(v_numSpectralWindow_[nbb]);

// 	for(uint32_t nspw=0; nspw<v_numSpectralWindow_[nbb]; nspw++){
// 	  vvvvvvv[na][nbb][nspw].resize(vv_numBin_[nbb][nspw]);

// 	  for(uint32_t nb=0; nb<vv_numBin_[nbb][nspw]; nb++){
// 	    vvvvvvv[na][nbb][nspw][nb].resize(vv_numBin_[nbb][nspw]);

// 	    for(uint32_t napc=0; napc<numApc_; napc++){
// 	      vvvvvvv[na][nbb][nb][nspw][nb].resize(numApc_);


// 	      for(uint32_t nsp=0; nsp<vv_numSpectralPoint_[nbb][nspw] nsp++){
// 		vvvvvvv[na][nbb][nb][nspw][nb][nsp].resize(vv_numPolProduct_[nbb][nspw]);

// 		for(uint32_t np=0; np<vv_numPolProduct_[nbb][nspw]; np++)
// 		  vvvvvvv[na][nbb][nb][nspw][nb][nsp].push-back(*floatDataPtr_[nd++]);

// 	      }
// 	    }
// 	  }
// 	}
//       }
//     }
    
    uint32_t dim=0;
    return dim;

  }


// ============================================================================================
// Integration -> DataDump -> DataStructure
// ============================================================================================

//   Integration::Integration()
//   {
//   }

  Integration::Integration( vector<vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
			    vector<vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
			    vector<vector<uint32_t> >       vv_numBin,            // /bb/spw
			    vector<vector<Enum<NetSideband> > > vv_e_sideband,        // /bb/spw
			    uint32_t                        numApc,
			    vector<uint32_t>                v_numSpectralWindow,  // /bb
			    uint32_t                        numBaseband,
			    uint32_t                        numAnt,
			    CorrelationMode                     correlationMode,
			    uint64_t                           time, 
			    uint64_t                           timeCentroid, 
			    uint64_t                           interval, 
			    uint64_t                           exposure, 
			    float*                              floatData,
			    uint32_t                        integrationNum):
    DataDump( vv_numPolProduct,    
	      vv_numSpectralPoint,
	      vv_numBin,
	      vv_e_sideband,
	      numApc,
	      v_numSpectralWindow,
	      numBaseband,
	      numAnt,
	      correlationMode,
	      time,
	      timeCentroid,
	      interval, 
	      exposure,
	      floatData)
  {
    integrationNum_    = integrationNum;
    subintegrationNum_ = 0;
  }

  Integration::Integration( vector<vector<uint32_t> >       vv_numPolProduct,	// /bb/spw
			    vector<vector<uint32_t> >       vv_numSpectralPoint,	// /bb/spw
			    vector<vector<uint32_t> >       vv_numBin,	// /bb/spw
			    vector<vector<Enum<NetSideband> > > vv_e_sideband,	// /bb/spw
			    uint32_t            numApc,
			    vector<uint32_t>    v_numSpectralWindow,	// /bb
			    uint32_t            numBaseband,
			    uint32_t            numAnt,
			    CorrelationMode     correlationMode,
			    uint64_t            time,
			    uint64_t            timeCentroid,  
			    uint64_t            interval,
			    uint64_t            exposure,  
			    float*              floatData, 
			    uint32_t*           dataFlags,
			    uint32_t                        integrationNum):
    DataDump( vv_numPolProduct,
	      vv_numSpectralPoint,
	      vv_numBin,
	      vv_e_sideband,
	      numApc,
	      v_numSpectralWindow,
	      numBaseband,
	      numAnt,
	      correlationMode,
	      time,
	      timeCentroid,
	      interval, 
	      exposure,
	      floatData,
	      dataFlags)
  {
    integrationNum_    = integrationNum;
    subintegrationNum_ = 0;
  }

  Integration::~Integration()
  {
  }

  Integration::Integration( const Integration & a) : DataDump(a)
  {
    cout<<"Copy constructor Integration"<<endl;
    vv_numCrossPolProduct_= a.vv_numCrossPolProduct_;
    vv_numAutoPolProduct_ = a.vv_numAutoPolProduct_;
    vv_numSpectralPoint_  = a.vv_numSpectralPoint_;
    vv_numBin_            = a.vv_numBin_;
    vv_e_sideband_        = a.vv_e_sideband_;
    numApc_               = a.numApc_;
    v_numSpectralWindow_  = a.v_numSpectralWindow_;
    numBaseband_          = a.numBaseband_;
    numAnt_               = a.numAnt_;
    correlationMode_      = a.correlationMode_;
    axisSequence_         = a.axisSequence_;
    v_minSize_            = a.v_minSize_;
    v_maxSize_            = a.v_maxSize_;

    integrationNum_       = a.integrationNum_;
    subintegrationNum_    = a.subintegrationNum_;
    time_                 = a.time_;
    timeCentroid_         = a.timeCentroid_;
    interval_             = a.interval_;
    exposure_             = a.exposure_;


//     int nd=a.numData();
//     if(a.floatDataPtr_){
//       floatDataPtr_ = new        float[nd]; for(int n=0; n<nd; n++)floatDataPtr_[n] = a.floatDataPtr_[n];
//     }else{
//       floatDataPtr_=a.floatDataPtr_;
//     }
//     if(a.dataFlagsPtr_){
//       dataFlagsPtr_ = new uint32_t[nd]; for(int n=0; n<nd; n++)dataFlagsPtr_[n] = a.dataFlagsPtr_[n];
//     }else{
//       dataFlagsPtr_=a.dataFlagsPtr_;
//     }

    integrationNum_       = a.integrationNum_;
  }



}
