// -*- C++ -*-
// -*- C++ -*-
//# HPGVisBufferBucket.h: Header-only implementation of the HPGVisBufferBucket class
//# Copyright (C) 2024
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
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
  HPGVisBufferBucket(const int& nVis)
    :VBB_p(), VBB_SOBuf_p(),nFills_p(0), rowCounter_p(0), nVis_p(nVis)
  {
    resize(nVis_p);
  };

  // Move constructor
  HPGVisBufferBucket(HPGVisBufferBucket&& other) noexcept
  {
    VBB_p=std::move(other.VBB_p);
    //    VBB_SOBuf_p = std::move(other.VBB_SOBuf_p);
    //    cout << "HPGVBB.move constructor\n";
  }

  // assignment operator
  // HPGVisBufferBucket& operator=(const HPGVisBufferBucket& rhs)
  // {
  //   cout << "HPGVBB.copy operator\n";
  //   if (&rhs != this) {
  //     HPGVisBufferBucket tmp(rhs);
  //     this->hpgVB_p.swap(rhs);
  //   }
  //   return *this;
  // }
  
  // move assignment operator
  std::vector<hpg::VisData<NCorr>>& operator=(HPGVisBufferBucket&& rhs) noexcept
  {
    //    cout << "HPGVBB.move= operator\n";
    std::swap(VBB_p, rhs.VBB_p);
    //    this->VBB_SOBuf_p.swap(rhs.VBB_SOBuf_p);
    return VBB_p;
  }


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
    VBB_p.resize(nRow*nChan);
  }
  //
  // -------------------------------------------------------------------------------
  //
  inline int resize(const int n=-1)
  {
    if (n != -1)
      {
	//	cout << "###### Resizein VBB " << n << endl;
	VBB_p.resize(n);
      }

    rowCounter_p=0;
    nFills_p=0;

    return VBB_p.size();
  }
  //
  // -------------------------------------------------------------------------------
  //
  inline unsigned size()
  {return VBB_p.size();}

  inline unsigned counter()
  {return rowCounter_p;}

  inline unsigned filledUnits()
  {return nFills_p;}

  inline unsigned incrementFills()
  {return nFills_p++;}

  inline bool isFull()
  {return (rowCounter_p >= size());}

  inline bool isEmpty() {return counter()==0;}

  // Copy the contents of the over flow buffer
  inline uint moveSOBuf()
  {
    uint n=vbbSOBuf().size();
    for (auto v : vbbSOBuf()) append(v);
    vbbSOBuf().resize(0);
    return n;
  }

  inline unsigned reset()
  {
    // Resize internal storage. Copy the overflow buffer to the main storage.
    resize(nVis_p);
    moveSOBuf();
    // cout << "ResetVBB: " << size()
    // 	 << " " << vbbSOBuf().size()
    // 	 << " " << counter() 
    // 	 << endl;
    return size();
  }

  inline unsigned shrink()
  {
    if (rowCounter_p > 0) VBB_p.resize(rowCounter_p);
    return VBB_p.size();
  }
  //
  // -------------------------------------------------------------------------------
  //
  inline bool append(const hpg::VisData<NCorr>& visData)
  {
    //    cout << size() << " " << rowCounter_p << " " << VBB_SOBuf_p.size() << " " << isFull() << endl;
    if (isFull())
      VBB_SOBuf_p.push_back(visData); //!! Optimization required?
    else
      VBB_p[rowCounter_p++]=visData;

    return rowCounter_p==size();
  }
  //
  // -------------------------------------------------------------------------------
  //
  std::vector<hpg::VisData<NCorr>>& vbbBuf() {return VBB_p;}
  std::vector<hpg::VisData<NCorr>>& vbbSOBuf() {return VBB_SOBuf_p;}
  //
  // -------------------------------------------------------------------------------
  //

  std::vector<hpg::VisData<NCorr>> VBB_p;  
  std::vector<hpg::VisData<NCorr>> VBB_SOBuf_p;  
private:
  unsigned int nFills_p;
  unsigned int rowCounter_p;
  uint nVis_p;
};
#endif
