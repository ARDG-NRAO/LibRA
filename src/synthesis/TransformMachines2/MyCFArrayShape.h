// -*- C++ -*-
//# MyCFArrayShape.h: Definition of the MyCFArrayShape class
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

#ifndef SYNTHESIS_TRANSFORM2_MYCFARRAYSHAPE_H
#define SYNTHESIS_TRANSFORM2_MYCFARRAYSHAPE_H
#include <hpg/hpg.hpp>

namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{
  struct MyCFArrayShape final: public hpg::CFArrayShape
  {
    //    std::vector<std::array<hpg::coord_t, 4>> m_extent;
    std::vector<std::array<HPG_ARRAY_INDEX_TYPE, 4>> m_extent;
    unsigned cf_oversampling=0;
    
    MyCFArrayShape(): m_extent(), cf_oversampling(0) {};

    void setSize(const unsigned& nCFs, const unsigned& oversampling)
    {
      cf_oversampling=oversampling;
      m_extent.resize(nCFs);
    }

    void resize(const unsigned& i, const unsigned& nx,const unsigned& ny,const unsigned& sto=2, const unsigned& cube=1)
    {
      m_extent[i][0] = nx;
      m_extent[i][1] = ny;
      m_extent[i][2] = sto;
      m_extent[i][3] = cube;
    }
    unsigned num_groups() const override
    {
      return static_cast<unsigned>(m_extent.size());
    }

    unsigned oversampling() const override
    {
      return cf_oversampling;
    }

    // std::array<unsigned, 4> extents(unsigned i) const 
    // {
    //   return m_extent[i];
    // }
    std::array<HPG_ARRAY_INDEX_TYPE, 4> extents(HPG_ARRAY_INDEX_TYPE i) const 
    {
      return m_extent[i];
    }

  };
  }
}
#endif
