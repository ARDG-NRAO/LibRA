// -*- C++ -*-
//# PhaseGrad.cc: Implementation of the PhaseGrad class
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
//

#include <synthesis/TransformMachines2/PhaseGrad.h>
#include <synthesis/TransformMachines/SynthesisMath.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogOrigin.h>

using namespace casacore;
namespace casa{
  using namespace refim;
  //
  //----------------------------------------------------------------------
  //
  PhaseGrad& PhaseGrad::operator=(const PhaseGrad& other)
  {
    if(this!=&other) 
      {
	field_phaseGrad_p = other.field_phaseGrad_p;
	antenna_phaseGrad_p = other.antenna_phaseGrad_p;
	cached_FieldOffset_p = other.cached_FieldOffset_p;
      }
    return *this;
  }
  //
  //----------------------------------------------------------------------
  //
  bool PhaseGrad::needsNewPhaseGrad(const VisBuffer2& vb,
				    const int& /*row*/)
  {
    unsigned int nRow=vb.nRows();
    if (cached_FieldOffset_p.nelements() < nRow) cached_FieldOffset_p.resize(nRow,true);

    return ((field_phaseGrad_p.shape()[0] < maxCFShape_p[0])           ||
	    (field_phaseGrad_p.shape()[1] < maxCFShape_p[1]));
  }
  //
  //----------------------------------------------------------------------
  //
  // bool PhaseGrad::ComputeFieldPointingGrad(const Vector<double>& pointingOffset,
  // 					   const CountedPtr<CFBuffer>& cfb,
  // 					   const Vector<int>&cfShape,
  // 					   const Vector<int>& convOrigin,
  // 					   const double& /*cfRefFreq*/,
  // 					   const double& /*imRefFreq*/,
  // 					   const int& spwID, const int& fieldId)
  // bool PhaseGrad::ComputeFieldPointingGrad(const CountedPtr<PointingOffsets>& pointingOffsets_p,
  // 					   const CountedPtr<CFBuffer>& cfb,
  // 					   const VisBuffer2& vb,
  // 					   const int& row,
  // 					   const pair<int,int> antGrp)
  bool PhaseGrad::ComputeFieldPointingGrad(const CountedPtr<PointingOffsets>& pointingOffsets_p,
					   const VisBuffer2& vb,
					   const int& row,
					   const pair<int,int> antGrp)

    {
      //
      // Re-find the max. CF size if the CFB changed.
      //
      // CFBuffer *thisCFB = cfb.get();
      // if (thisCFB != cachedCFBPtr_p)
      // 	{
      // 	  maxCFShape_p[0] = maxCFShape_p[1] = cfb->getMaxCFSize();
      // 	  {
      // 	    // LogIO log_l(LogOrigin("PhaseGrad","computeFieldPointingGrad"));
      // 	    //cerr << "CFB changed: "<< thisCFB << " " << cachedCFBPtr_p << " " << vb.spectralWindows()(0) << " " << vb.fieldId()(0) << " " << maxCFShape_p << endl;
      // 	  }
      // 	  cachedCFBPtr_p = thisCFB;
      // 	}
      //
      // If the pointing or the max. CF size changed, recompute the phase gradient.
      //
      LogIO log_l(LogOrigin("PhaseGrad","computeAntennaPointingGrad"));
      Bool needsCFShape_l = (needsNewPhaseGrad(vb, row));
      if (needsCFShape_l || needsNewFieldPG_p || needsNewPOPG_p )
	{
	  Vector<Vector<double> > pointingOffset;
	  if(pointingOffsets_p->getDoPointing())
	    {
	      pointingOffset.assign(pointingOffsets_p->pullAntGridPointingOffsets());
	      // cerr << "Pointing Offsets in PG: "<<pointingOffset<<endl;
	      // cerr << "NeedsCFShape_l " << needsCFShape_l <<" NeedsNewFieldPG_p " << needsNewFieldPG_p << " NeedsNewPOPG_p " << needsNewPOPG_p <<" row " <<row << endl;
	    }
	  else
	    pointingOffset = pointingOffsets_p->pullPointingOffsets();
	  if(needsNewFieldPG_p)
	    {
	       log_l << "Computing Phase Grad for Field : " << vb.fieldId()(0) << "  for spw :" 
		     << vb.spectralWindows()(0)
		     << LogIO::POST;
	    }
	  // else if(needsCFShape_l)
	  //   {
	  //     log_l << "Computing Phase Grad for change of max CF size : " << maxCFShape_p[0]
	  // 	    << LogIO::POST;
	  //   }
	  // else if(needsNewPOPG_p)
	  //   {

	  //     // cerr << "NeedsNewPOPG_p for row "<< row << " " << vb.fieldId()(0)<< " " <<  vb.spectralWindows()(0) << endl;
	  //     // log_l << "Computing Phase Grad for Antenna Pointing Offset for row : " << row << " " << pointingOffset[row][0] << " " << pointingOffset[row][1] << " spw :"
	  //     // 	    << vb.spectralWindows()(0) << " and field " << vb.fieldId()(0)
	  //     // 	    << LogIO::POST;
	  //   }
	    
	  int nx=maxCFShape_p(0), ny=maxCFShape_p(1);
	  double grad;
	  Complex phx,phy;
	  Vector<int> convOrigin = maxCFShape_p/2;
	  
	  field_phaseGrad_p.resize(nx,ny);
	  // cached_FieldOffset_p[row] = pointingOffset[row];
	  vector<double> ant1PO_l, ant2PO_l, blPO_l;
	  Vector<double> tmpblPO_l;
	  ant1PO_l.resize(2,0);
	  ant2PO_l.resize(2,0);
	  blPO_l.resize(2,0);

	  if(pointingOffsets_p->getDoPointing())
	    {
	      ant1PO_l[0] = (pointingOffset[0][antGrp.first]);
	      ant1PO_l[1] = (pointingOffset[1][antGrp.first]);
	      ant2PO_l[0] = (pointingOffset[0][antGrp.second]);
	      ant2PO_l[1] = (pointingOffset[1][antGrp.second]);

	      // cerr<<"Ant1: "<<antGrp.first << " "<< ant1PO_l[0] << " " << ant1PO_l[1] << endl;
	      // cerr<<"Ant2: "<<antGrp.second << " "<< ant2PO_l[0] << " " << ant2PO_l[1] << endl;
	      blPO_l[0] = (ant1PO_l[0] + ant2PO_l[0])/2;
	      blPO_l[1] = (ant1PO_l[1] + ant2PO_l[1])/2;
          Vector<double> const blPO_l_casavec(blPO_l);
	      tmpblPO_l = pointingOffsets_p->gradPerPixel(blPO_l_casavec);
	      cached_FieldOffset_p[row](0) = tmpblPO_l[0];
	      cached_FieldOffset_p[row](1) = tmpblPO_l[1];
	    }
	  else
	    {
	      cached_FieldOffset_p[row](0) = pointingOffset[row][0];
	      cached_FieldOffset_p[row](1) = pointingOffset[row][1];

	      // cached_FieldOffset_p[row](0) = pointingOffsets_p->gradPerPixel((ant1PO_l[0] + ant2PO_l[0])/2);
	      // cached_FieldOffset_p[row](1) = pointingOffsets_p->gradPerPixel((ant1PO_l[1] + ant2PO_l[1])/2);
	    }
 	  // cerr << "Cached Field Offset is : " << cached_FieldOffset_p[row](0) << " " << cached_FieldOffset_p[row](1)<< " for row " << row << " field id " << vb.fieldId()(0) << endl;
	  for(int ix=0;ix<nx;ix++)
	    {
	      grad = (ix-convOrigin[0])*cached_FieldOffset_p[row](0);
	      double sx,cx;
	      SINCOS(grad,sx,cx);
	      phx = Complex(cx,sx);
	      for(int iy=0;iy<ny;iy++)
		{
		  grad = (iy-convOrigin[1])*cached_FieldOffset_p[row](1);
		  Double sy,cy;
		  SINCOS(grad,sy,cy);
		  phy = Complex(cy,sy);
		  field_phaseGrad_p(ix,iy)=phx*phy;
		}
	    }
	  needsNewPOPG_p = false;
	  return true; // New phase gradient was computed
	}
      return false;
    }
}
