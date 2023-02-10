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
    :hpgVB_p()
  {
    //    cerr << "HPGVBB.ctor()" << endl;
    nVBs_p=nVBs;
    nFills_p=rowCounter_p=nPol_p=nChan_p=nRow_p=0;
  };

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
    //    cerr << "HPGVBB resizing...." << nPol << " " << nChan << " " << nRow << endl;
    //    if (hpgVB_p.size()==0)
      {
	hpgVB_p.resize(nRow*nChan*nVBs_p);
	rowCounter_p=0;
	nFills_p=0;
	// Save the dimensions for use in reset() later.  Think of a better mechanism!
	nPol_p = nPol;
	nChan_p = nChan;
	nRow_p = nRow;
      }
    return hpgVB_p.size();
  }
  //
  // -------------------------------------------------------------------------------
  //
  inline unsigned size()
  {return hpgVB_p.size();}

  inline unsigned counter()
  {return rowCounter_p;}

  inline unsigned totalUnits()
  {return nVBs_p;}

  inline unsigned filledUnits()
  {return nFills_p;}

  inline unsigned incrementFills()
  {return nFills_p++;}

  inline bool isFull()
  {return (rowCounter_p >= size())||(nFills_p == nVBs_p);}

  inline unsigned reset()
  {return resize(nPol_p, nChan_p, nRow_p);}

  inline unsigned shrink()
  {hpgVB_p.resize(rowCounter_p-1); return hpgVB_p.size();}
  //
  // -------------------------------------------------------------------------------
  //
  inline bool append(const hpg::VisData<NCorr>& visData)
  {
    hpgVB_p[rowCounter_p++]=visData;
    return rowCounter_p==size();
  }
  //
  // -------------------------------------------------------------------------------
  //
  std::vector<hpg::VisData<NCorr>> hpgVB_p;  
private:
  unsigned int nVBs_p, nFills_p;
  unsigned int rowCounter_p;
  unsigned int nPol_p, nChan_p, nRow_p;
};
#endif
