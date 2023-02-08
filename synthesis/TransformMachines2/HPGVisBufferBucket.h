// -*- C++ -*-
#ifndef SYNTHESIS_TRANSFORM2_HPGVISBUFFERBUCKET_H
#define SYNTHESIS_TRANSFORM2_HPGVISBUFFERBUCKET_H

// #include <msvis/MSVis/VisBuffer2.h>
// #include <synthesis/TransformMachines2/VBStore.h>
// //#include <synthesis/TransformMachines2/CFBuffer.h>
// #include <synthesis/TransformMachines2/CFDefs.h>
// #include <synthesis/TransformMachines2/FTMachine.h>

#include <hpg/hpg.hpp>
// #include <hpg/hpg_indexing.hpp>
using namespace hpg;
//
// -------------------------------------------------------------------------------
//
template <unsigned NCorr>
class HPGVisBufferBucket
{
public:
  HPGVisBufferBucket(const unsigned& nVBs)
    :hpgVB_p(),nVBs_p(nVBs), nFills_p(0), rowCounter_p(0)
  {};

  ~HPGVisBufferBucket() {};
  //
  // -------------------------------------------------------------------------------
  //
  //  template <unsigned NCorr>
  inline
  void create_blank_vis_data_vector(const unsigned nPol,
				    const unsigned nChan,
				    const unsigned nRow)
  {
    (void)nPol; // Just to remove an annoying compile-time warning ("unused parameter").
    assert(nPol == NCorr);
    hpgVB_p.resize(nRow*nChan);
  }
  //
  // -------------------------------------------------------------------------------
  //
  inline unsigned int resize(const unsigned nPol,
			     const unsigned nChan,
			     const unsigned nRow)
  {
    if (hpgVB_p.size()==0)
      {
	hpgVB_p.resize(nRow*nChan*nVBs_p);
	rowCounter_p=0;
      }
    return hpgVB_p.size();
  }
  //
  // -------------------------------------------------------------------------------
  //
  inline unsigned size(){return hpgVB_p.size();}
  inline unsigned counter(){return rowCounter_p;}
  inline bool isFull() {return (rowCounter_p >= size())||(nFills_p == nVBs_p);}
  //
  // -------------------------------------------------------------------------------
  //
  inline bool append(const hpg::VisData<NCorr>& visData)
  {
    hpgVB_p[rowCounter_p++]=visData;
    nFills_p++;
    return rowCounter_p==size();
  }
  //
  // -------------------------------------------------------------------------------
  //
  std::vector<hpg::VisData<NCorr>> hpgVB_p;  

private:
  unsigned int nVBs_p, nFills_p;
  unsigned int rowCounter_p;
};
#endif
