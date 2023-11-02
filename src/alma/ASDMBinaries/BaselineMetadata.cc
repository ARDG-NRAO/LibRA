#include <vector>
#include <iostream>
using namespace std;
#include <alma/ASDMBinaries/Error.h>
#include <alma/ASDMBinaries/BaselineMetadata.h>
using namespace sdmbin;

// constructors

// Default onstructor
BaselineMetadata::BaselineMetadata(){
  //cout << "Constructor vide BaselineMetadata" << endl;
}

// Destructor
BaselineMetadata::~BaselineMetadata(){}


// method to instantiate the base of the hierarchy:
void BaselineMetadata::mkBaselineMetadata( vector<int> v_npp, vector<int> v_nbin, int nbb, int nant, int correlationMode)
{
  correlationMode_ = correlationMode;
  v_numPolProduct_ = v_npp;
  v_numBin_    = v_nbin;
  numBaseband_ = nbb;
  numAnt_      = nant;
  numBaseline_ = 0;
  if(correlationMode!=1)
    numBaseline_ = (nant*(nant-1))/2;

  sizes();
}


// accessors

int         BaselineMetadata::numBaseband(){ 
  return v_numBin_.size();
}

int         BaselineMetadata::numPolProduct(int basebandNum) throw (Error){ 
  if(wrongBasebandNum(basebandNum))
    return -1; 
  else return v_numPolProduct_[basebandNum-1];
}

vector<int> BaselineMetadata::numPolProduct(){ 
  return v_numPolProduct_;
}

int         BaselineMetadata::numBin(int basebandNum) throw (Error){ 
  if(wrongBasebandNum(basebandNum))
    return -1; 
  else 
    return v_numBin_[basebandNum-1];       
}

vector<int> BaselineMetadata::numBin(){ 
  return v_numBin_; 
}

int         BaselineMetadata::numBaseline(){ 
  return numBaseline_;
}

int         BaselineMetadata::correlationMode(){ 
  return correlationMode_; 
}

vector<int> BaselineMetadata::numAutoPolProduct(){ 
  return v_numAutoPolProduct_;
}

int         BaselineMetadata::numAutoPolProduct(int basebandNum) throw (Error){
  if(wrongBasebandNum(basebandNum))return -1; 
  else 
    return v_numAutoPolProduct_[basebandNum-1];
}


// static data member
int         BaselineMetadata::FLAGLIMIT=0;


// methods to access a node or sequence of nodes in the tree structure
int BaselineMetadata::at(int pp, int /*bin*/, int bb, int i, int j){  // comment to avoid the unused parameter warning.
  int index=0;
  if(wrongPolProductIndex(pp,bb,i,j))return -1;
  if(i==j){                                                        // metadata for single-dish data
    for(int nbb=0; nbb<bb; nbb++)
      index = index+v_numBin_[nbb]*v_numAutoPolProduct_[nbb];
    return crossSize_ + i*numValPerZeroBaseline_ + index + pp;

  }else{                                                           // metadata for interferometric data
    for(int nbb=0; nbb<bb; nbb++){
      index = index+v_numBin_[nbb]*v_numPolProduct_[nbb];
    }
    return (baselineNum(i,j)-1)*numValPerBaseline_ + index + pp;
  }
}

vector<int> BaselineMetadata::at(int pp, int bb, int i, int j){
  int         index=0;
  vector<int> v_index;
  if(wrongPolProductIndex(pp,bb,i,j))return v_index;
  if(i==j){                                                        // metadata for single-dish data
    for(int nbb=0; nbb<bb; nbb++)
      index = index+v_numBin_[nbb]*v_numAutoPolProduct_[nbb];
    for(int nbin=0; nbin<v_numBin_[bb]; nbin++)
      v_index.push_back( crossSize_ + 
			 i*index + 
			 nbin*v_numAutoPolProduct_[bb] + pp);
    return v_index;
  }else{                                                           // metadata for interferometric data
    for(int nbin=0; nbin<v_numBin_[bb]; nbin++)
      v_index.push_back( (baselineNum(i,j)-1)*index +
			 nbin*v_numPolProduct_[bb] + pp);
    return v_index;
  }
}

vector<int> BaselineMetadata::at(int /*pp*/, int /*bin*/, int /*bb*/){ // comment to avoid the unused parameter warning
  vector<int> v_index;
  Error(WARNING, (char *) " This method has not yet been implemented! contact fviallef@maat.obspm.fr");
  // TODO
  return v_index;
}


vector<vector<int> > BaselineMetadata::at(int /*nbin*/, int /*nbb*/){ // comment to avoid the unused parameter warning
  vector<vector<int> > vv_index;
  Error(WARNING, (char *) "This method has not yet been implemented! contact fviallef@maat.obspm.fr");
  // TODO
  return vv_index;
}


vector<vector<vector<int> > > BaselineMetadata::at(bool cross, int antIndex){

  vector<vector<vector<int> > > vvv_index;
  vector<vector<int> >          vv_index;
  vector<int>                   v_index;

  if(wrongAntennaIndex(antIndex))return vvv_index;

  int                  i,n,k,na,nbb;
  int                  imax;

  if(cross){

    if(correlationMode_==1){
      Error(WARNING, (char *) "No cross metadata when correlationMode=1");
      return vvv_index;
    }
    vv_index.resize(numBaseband_);
    for(na=0; na<numAnt_; na++){
      if(na!=antIndex){
	k = (baselineNum(na,antIndex)-1)*numValPerBaseline_;
	n = 0;
	for(nbb=0; nbb<numBaseband_; nbb++){
	  imax=v_numBin_[nbb]*v_numPolProduct_[nbb];
	  v_index.resize(imax);
	  for(i=0;i<imax; i++)v_index[i]=k+n++;
	  vv_index[nbb]=v_index;
	}
	vvv_index.push_back(vv_index);
      }
    }

  }else{

    /* Since 3APr06 we also have antenna-based metadata when correlationMode=0
    if(correlationMode_==0){
      Error(WARNING, "no auto metadata when correlationMode=0");
      return vvv_index;
    }
    */
    vv_index.resize(numBaseband_);
    k = crossSize_ + antIndex*numValPerZeroBaseline_;
    n=0;
    for(nbb=0; nbb<numBaseband_; nbb++){
      imax=v_numBin_[nbb]*v_numAutoPolProduct_[nbb];
      v_index.resize(imax);
      for(i=0;i<imax; i++)v_index[i]=k+n++;
      vv_index[nbb]=v_index;
    }
    vvv_index.push_back(vv_index);
  }

  return vvv_index;

}


// method to get the size (in number of value of a given type)
void BaselineMetadata::sizes(){

  numValPerBaseline_ = 0;
  for(int nbb=0; nbb<numBaseband(); nbb++)
    numValPerBaseline_ = numValPerBaseline_ + v_numBin_[nbb]*v_numPolProduct_[nbb];

  crossSize_ = 0;
  if(numBaseline_!=0){    
    crossSize_ = numBaseline_*numValPerBaseline_;
  }

  int numAutoPP;
  numValPerZeroBaseline_ = 0;
  for(int nbb=0; nbb<numBaseband(); nbb++){
    numAutoPP = v_numPolProduct_[nbb];
    if(v_numPolProduct_[nbb]==4){
      numAutoPP=3;
      if(correlationMode_==0)numAutoPP=2;  // exclusively for BaselineMetadata children!
    }
    v_numAutoPolProduct_.push_back(numAutoPP);
    numValPerZeroBaseline_ = numValPerZeroBaseline_ + v_numBin_[nbb]*numAutoPP;
  }
  autoSize_  = 0;
  if(correlationMode_==1){
    autoSize_ = numAnt_*numValPerBaseline_;
  }else{
    autoSize_ = numAnt_*numValPerZeroBaseline_;
  }
  //cout << "autoSize_=" << autoSize_ << endl;
  //cout << "crossSize_=" << crossSize_ << endl;
}

int BaselineMetadata::numValue(){
  return crossSize_+autoSize_;
}


int BaselineMetadata::baselineNum( int na1, int na2) throw(Error){
  if(na1==na2){
    return numBaseline_+na1+1;
  }else{
    int n;
    if(na1>na2){ n=na2; na2=na1; na1=n; }
    int iNum = na1+1;
    int jNum = na2+1;
    n=iNum; if(jNum>iNum)n=jNum; 
    if( ((n*(n-1))/2)>numBaseline_){
      Error(SERIOUS, 
	    (char *) "Cannot find a baselineNum with the antenna indices %d and %d",
	    na1,na2);
      return -1;
    }
    int m = 0;
    for(int n=1; n<jNum-1; n++)m=m+n;
    return m+iNum;         // the returned index, a "num", is 1 based.         
  }          
}                


bool BaselineMetadata::notFor(bool interfero){
  if(interfero){
    if(correlationMode_==1){
      Error(WARNING,
	    (char *) " The baseline metadata were build for single-dish; queries not expected for interferomtry!");
      return true;
    }
  }else{
    /* There are now, even in this correlation mode 0, antenna-based metadata (since 3Apr06) 
    if(correlationMode_==0){
      cout<<"ERROR: shaped for pure interferometric metadata; query failed!"<<endl;
      return true;
    }
    */
  }
  return false;
}

bool BaselineMetadata::wrongBasebandNum(int basebandNum){
  if(basebandNum<numBaseband_)return false;
  else
    Error(SERIOUS,
	  (char *) "The input baseband number, %d, exceeds the number of baseband %d in the configuration",
	  basebandNum,numBaseband_);
  if(basebandNum<1)
    Error(FATAL,
	  (char *) "A baseband number must be one-based");
  return true;
} 

bool BaselineMetadata::wrongAntennaIndex(int anti){
  if(anti<0){
    Error(WARNING, (char *) "An antenna index must be a non-negative integer!");
    return true;
  }
  if(anti>=numAnt_){
    Error(WARNING, (char *) "Wrong antenna index; it must be < %d; query failed!",numAnt_);
    return true;
  }
  return false;
} 

bool BaselineMetadata::wrongBasebandIndex(int bb){
  if(bb<0){
    Error(WARNING, (char *) "A baseband index must be a non-negative integer!");
    return true;
  }
  if(bb>=numBaseband_){
    Error(WARNING, (char *) "Wrong baseband index: it must be < %d; query failed!",numBaseband_);
    return true;
  }
  return false;
} 

bool BaselineMetadata::wrongPolProductIndex(int pp,int bb,int i,int j){

  if(wrongAntennaIndex(i))return true;
  if(wrongAntennaIndex(j))return true;
  if(wrongBasebandIndex(bb))return true;
  if(pp<0){
    Error(WARNING, (char *) "A polProduct index must be a non-negative integer!");
    return true;
  }
  if(i==j){
    if(pp>=v_numAutoPolProduct_[bb]){
      Error(WARNING, (char *) "Wrong polProduct index: it must be < %d; query failed!",v_numAutoPolProduct_[bb]);
      return true;
    }
  }else{
    if(pp>=v_numPolProduct_[bb]){
      Error(WARNING, (char *) "Wrong polProduct index: it must be < %d; query failed!",v_numPolProduct_[bb]);
      return true;
    }
  }
  return false;
}


void BaselineMetadata::display(){
  if(numAnt_){                        // with single-dish
    for(int n=0; n<numBaseband_; n++)
      cout <<"Baseband num.: "     << n+1
	   <<"  Nb pol.: "         << v_numPolProduct_[n] 
	   <<"  Nb sd pol.: "      << v_numAutoPolProduct_[n] 
	   <<"  Nb bins: "         << v_numBin_[n] 
	   <<"  Nb ant.: "         << numAnt_
	   <<"  Nb baselines: "    << numBaseline_ << endl;

  }else{                              // pure interfero
    for(int n=0; n<numBaseband_; n++)
      cout <<"Baseband num.: "     << n+1
	   <<"  Nb pol. product: " << v_numPolProduct_[n] 
	   <<"  Nb bins: "         << v_numBin_[n] 
	   <<"  Nb ant.: "         << numAnt_
	   <<"  Nb baselines: "    << numBaseline_ << endl;
  }  
}
