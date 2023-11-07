#include <vector>
#include <stdio.h>
#include <iostream>

#include <alma/ASDMBinaries/SwitchCyclesList.h>

using namespace asdm;
using namespace std;

using namespace BasebandNameMod;

namespace sdmbin {

SwitchCyclesList::SwitchCyclesList(){}

SwitchCyclesList::SwitchCyclesList(ASDM* const datasetPtr, vector<Tag> v_switchCycleId):
  datasetPtr_(datasetPtr),
  v_switchCycleId_(v_switchCycleId)
{
  SwitchCycleTable& rscSet = datasetPtr->getSwitchCycle();
  for(unsigned int nbb=0; nbb<v_switchCycleId.size(); nbb++)
    v_numBin_.push_back(rscSet.getRowByKey(v_switchCycleId[nbb])->getNumStep());  
}

SwitchCyclesList::SwitchCyclesList(ASDM* const datasetPtr, vector<vector<Tag> > vv_switchCycleId):
  datasetPtr_(datasetPtr),
  vv_switchCycleId_(vv_switchCycleId)
{
  SwitchCycleTable& rscSet = datasetPtr->getSwitchCycle();
  // sub-class DataDescriptionsSet will check that v_switchCycleId.size() = numBaseband
  int  numConstStep;
  bool perBaseband=true;
  vv_numBin_.resize(vv_switchCycleId.size());
  for(unsigned int nbb=0; nbb<vv_switchCycleId.size(); nbb++){
    for(unsigned int nspw=0; nspw<vv_switchCycleId[nbb].size(); nspw++){
      int numStep=rscSet.getRowByKey(vv_switchCycleId[nbb][nspw])->getNumStep();
      if(nspw){
	if(numStep!=numConstStep)perBaseband=false;
      }else{
	numConstStep = numStep;
      }
      vv_numBin_[nbb].push_back(numStep);
    }
    if(perBaseband)v_numBin_.push_back(numConstStep);
  }
  if(!perBaseband)v_numBin_.resize(0);
}

SwitchCyclesList::SwitchCyclesList(ASDM* const datasetPtr, vector<vector<int> > vv_switchCycleId):
  datasetPtr_(datasetPtr)
{
  SwitchCycleTable& rscSet = datasetPtr->getSwitchCycle();
  // sub-class DataDescriptionsSet will check that v_switchCycleId.size() = numBaseband
  vv_switchCycleId_.resize(vv_switchCycleId.size());
  vv_numBin_.resize(vv_switchCycleId.size());
  for(unsigned int nbb=0; nbb<vv_switchCycleId.size(); nbb++)
    for(unsigned int nspw=0; nspw<vv_switchCycleId[nbb].size(); nspw++){
      vv_switchCycleId_[nbb].push_back(Tag((unsigned int)vv_switchCycleId[nbb][nspw]));
      vv_numBin_[nbb].push_back(rscSet.getRowByKey(vv_switchCycleId[nbb][nspw])->getNumStep());  
    }
}

SwitchCyclesList::SwitchCyclesList(const SwitchCyclesList & a){
  m_bn_v_scId_      = a.m_bn_v_scId_;
  m_bn_v_numBin_    = a.m_bn_v_numBin_;
  vv_numBin_        = a.vv_numBin_;
  v_numBin_         = a.v_numBin_;
  datasetPtr_       = a.datasetPtr_;
  vv_switchCycleId_ = a.vv_switchCycleId_;
  v_switchCycleId_  = a.v_switchCycleId_;
}

SwitchCyclesList::~SwitchCyclesList(){}

void SwitchCyclesList::mapScToBaseband(){}

vector<Tag> SwitchCyclesList::getSwitchCyclesList(BasebandName basebandName){
  map<BasebandName,vector<Tag> >::iterator 
    itf=m_bn_v_scId_.find(basebandName),
    ite=m_bn_v_scId_.end();
  vector<Tag> v;
  if(itf==ite)return v;
  return itf->second;
} 

vector<Tag> SwitchCyclesList::getSwitchCyclesList(){
  map<BasebandName,vector<Tag> >::iterator 
    it,
    itb=m_bn_v_scId_.begin(),
    ite=m_bn_v_scId_.end();
  vector<Tag> v;
  for(it=itb; it!=ite; ++it)
    for(unsigned int n=0; n<it->second.size(); n++)
      v.push_back(it->second[n]);
  return v;
} 

vector<int> SwitchCyclesList::getNumBin(){
  map<BasebandName,vector<int> >::iterator 
    it,
    itb=m_bn_v_numBin_.begin(), 
    ite=m_bn_v_numBin_.end();
  vector<int> v;
  if(itb==ite)return v;
  for(it=itb; it!=ite; ++it)
    for(unsigned int n=0; n<it->second.size(); n++)
      v.push_back(it->second[n]);
  return v;
}

vector<int> SwitchCyclesList::getNumBin(BasebandName basebandName){
  map<BasebandName,vector<int> >::iterator 
    itf=m_bn_v_numBin_.find(basebandName), 
    ite=m_bn_v_numBin_.end();
  vector<int> v;
  if(itf==ite)
    return v;
  else 
    return itf->second;
}

}
