// -*- C++ -*-
//# MyCFArray.h: Definition of the MyCFArray class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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

#ifndef SYNTHESIS_TRANSFORM2_MYCFARRAY_H
#define SYNTHESIS_TRANSFORM2_MYCFARRAY_H
#include <hpg/hpg.hpp>

namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{
    struct MyCFArray final: public hpg::CFArray
  {
    std::vector<std::array<hpg::coord_t, 4>> m_extent;
    std::vector<std::vector<std::complex<hpg::cf_fp_t>>> m_values;
    unsigned cf_oversampling=0;
    
    MyCFArray(): m_extent(), m_values(), cf_oversampling(0) {};

    void setSize(const unsigned& nCFs, const unsigned& oversampling)
    {
      cf_oversampling=oversampling;
      m_extent.resize(nCFs);
      m_values.resize(nCFs);
    }

    void resize(const unsigned& i, const unsigned& nx,const unsigned& ny,const unsigned& sto=2, const unsigned& cube=1,
		bool allocate=true)
    {
      m_extent[i][0] = nx;
      m_extent[i][1] = ny;
      m_extent[i][2] = sto;
      m_extent[i][3] = cube;
      if (allocate) m_values[i].resize(nx*ny*sto*cube);
    }
    unsigned num_groups() const override
    {
      return static_cast<unsigned>(m_extent.size());
    }

    std::vector<hpg::CFArray::value_type>
    getGroup(const uint igrp) {return m_values[igrp];}

    void setValues(const casacore::Complex* values, const unsigned igrp,const unsigned nx, const unsigned ny, const unsigned polNdx, const unsigned freqNdx)
    {
      auto& pix = m_values[igrp];
      auto& ext = m_extent[igrp];
      
      unsigned k=0;
      for (unsigned j=0; j<ny; j++)
      	for (unsigned i=0;i<nx; i++)
      	  {
      	    pix[((i * ny + j) * ext[2] + polNdx) * ext[3]+ freqNdx]
      	      =values[k++];
      	  }
    }

    unsigned oversampling() const override
    {
      return cf_oversampling;
    }

    // std::array<unsigned, 4> extents(unsigned i) const 
    // {
    //   return m_extent[i];
    // }

    std::array<hpg::coord_t, 4> extents(unsigned i) const 
    {
      return m_extent[i];
    }

    // !!!!!!!!!!! REQUIRED FOR rcgrid HPG BRANCH !!!!!!!!!!!!!!
    std::array<hpg::coord_t, 4> extents(int i) const 
    {
      return m_extent[i];
    }

    inline std::complex<hpg::cf_fp_t> operator()
    //(HPG_ARRAY_INDEX_TYPE x, HPG_ARRAY_INDEX_TYPE y, HPG_ARRAY_INDEX_TYPE pol, HPG_ARRAY_INDEX_TYPE freq, HPG_ARRAY_INDEX_TYPE i) const 
    (hpg::coord_t x, hpg::coord_t y, hpg::coord_t pol, hpg::coord_t freq, hpg::coord_t i) const 
    {
      {
	auto& pix = m_values[i];
	auto& ext = m_extent[i];
	return pix[((x * ext[1] + y) * ext[2] + pol) * ext[3]+ freq];
      }
    }
  };
  }
}
#endif
