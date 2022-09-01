// -*- C++ -*-
//# FortranizedLoopsFromGrid.cc: Code to call the de-gridding
//# inner-loops written in FORTRAN
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
{
  const Complex *gridPtr;
  Complex  *phaseGradPtr;
  Int *supportPtr, *cfShapePtr,
    *locPtr, *igrdposPtr, //*ilocPtr, *tilocPtr, 
    *convOriginPtr;
  Float  *samplingPtr;
  Double *offPtr;
  Bool dummy;

  gridPtr       = grid.getStorage(dummy);
  phaseGradPtr  = cached_phaseGrad_p.getStorage(dummy);
  supportPtr    = support.getStorage(dummy);
  samplingPtr   = sampling.getStorage(dummy);
  cfShapePtr    = cfShape.getStorage(dummy);
  locPtr        = loc.getStorage(dummy);
  igrdposPtr    = igrdpos.getStorage(dummy);
  //ilocPtr       = iloc.getStorage(dummy);
  //tilocPtr      = tiloc.getStorage(dummy);
  offPtr        = off.getStorage(dummy);
  convOriginPtr = convOrigin.getStorage(dummy);
  Int finitePointingOffsets_int = (finitePointingOffset?1:0);
  Int cf0=cfShape(0), cf1=cfShape(1), cf2=cfShape(2), cf3=cfShape(3);
  Int gnx = nx, gny = ny, gnp = nGridPol, gnc=nGridChan;
  Int phx=cached_phaseGrad_p.shape()[0], phy=cached_phaseGrad_p.shape()[1];

  //
  // Call the FORTRAN function with the gridding inner-loops (in synthesis/fortran/faccumulateFromGrid.f)
  //
  faccumulatefromgrid_(&nvalue, 
		       &norm[ipol],
		       gridPtr, 
		       convFuncV, 
		       &dataWVal,
		       supportPtr,samplingPtr,offPtr, convOriginPtr,
		       cfShapePtr,locPtr,igrdposPtr, 
		       &sinDPA, &cosDPA,
		       &finitePointingOffsets_int, 
		       phaseGradPtr,
		       &phasor,
		       &gnx, &gny, &gnp, &gnc,
		       &cf0, &cf1, &cf2, &cf3,
		       &phx, &phy);
		     
		     // &nx,&ny,&nGridPol, &nGridChan,
		     // &unity, &unity, &unity, &unity,
		     // &cfShape(0),&cfShape(1),&cfShape(2),&cfShape(3),
		     // &(cached_phaseGrad_p.shape()[0]),
		     // &(cached_phaseGrad_p.shape()[1]));
}
