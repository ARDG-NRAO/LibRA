// -*- C++ -*-
//# FortranizedLoopsToGrid.cc: Code to call the gridding inner-loops
//# written in FORTRAN
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
  timer_p.mark();
  //  T *gridPtr;
  Complex *cfPtr, *phaseGradPtr;
  Int *supportPtr, *cfShapePtr,
    *locPtr, *igrdposPtr, *ilocPtr, *tilocPtr,
    *convOriginPtr;
  Float  *samplingPtr;
  Double *offPtr;
  Bool dummy;
  //  gridPtr       = grid.getStorage(dummy);
  cfPtr         = convFuncV;
  phaseGradPtr  = cached_phaseGrad_p.getStorage(dummy);
  supportPtr    = support.getStorage(dummy);
  samplingPtr   = sampling.getStorage(dummy);
  cfShapePtr    = cfShape.getStorage(dummy);
  locPtr        = loc.getStorage(dummy);
  igrdposPtr    = igrdpos.getStorage(dummy);
  ilocPtr       = iloc.getStorage(dummy);
  tilocPtr      = tiloc.getStorage(dummy);
  offPtr        = off.getStorage(dummy);
  convOriginPtr = convOrigin.getStorage(dummy);
  (void)cfPtr;
  (void)ilocPtr;
  (void)tilocPtr;

  Int finitePointingOffsets_int = (finitePointingOffsets?1:0),
    psfOnly_int = (psfOnly?1:0);
  Int cf0=cfShape(0), cf1=cfShape(1), cf2=cfShape(2), cf3=cfShape(3);
  Int gnx = nx, gny = ny, gnp = nGridPol, gnc=nGridChan;
  Int phx=cached_phaseGrad_p.shape()[0], phy=cached_phaseGrad_p.shape()[1];

  runTimeG2_p += timer_p.real();   

  //
  // Call the FORTRAN function with the gridding inner-loops (in synthesis/fortran/faccumulateToGrid.f)
  //
  if (isGridSinglePrecision)
    {
      // Following type cast is so that this code compiles when
      // embedded (via #include) in the templated method
      // AWVisResampler::DataToGridImpl_p().  Without this type cast,
      // a compile-time error occurs (because this code gets embedded
      // in a templated function where the type of gridStore variable
      // is templated).
      Complex *Complex_gridStore=(Complex *)gridStore;
      faccumulatetogrid_(Complex_gridStore, 
			 convFuncV, 
			 &nvalue, 
			 &dataWVal,
			 supportPtr,samplingPtr,offPtr, convOriginPtr,
			 cfShapePtr,locPtr,igrdposPtr, 
			 &sinDPA, &cosDPA,
			 &finitePointingOffsets_int, 
			 &psfOnly_int,
			 &norm, phaseGradPtr,
			 &gnx, &gny, &gnp, &gnc,
			 &cf0, &cf1, &cf2, &cf3,
			 &phx, &phy);
      // if (fpclassify(abs(norm)) == FP_NAN)
      // 	{
      // 	  cerr << "N NAN: " << nvalue << " " << support << " " << sampling << " " << off << " " << convOrigin << " " << loc << " " << igrdpos << " " << cfShape << endl;
      // 	  throw(AipsError("NAN DETECTED IN NORM"));
      // 	}
      // if (fpclassify(vbs.imagingWeight_p(ichan, irow)) == FP_NAN)
      // 	{
      // 	  cerr << "WT NAN: " << nvalue << " " << support << " " << sampling << " " << off << " " << convOrigin << " " << loc << " " << igrdpos << " " << cfShape << endl;
      // 	  throw(AipsError("NAN DETECTED IN WT"));
      // 	}
    }
  else
    {
      DComplex *DComplex_gridStore=(DComplex *)gridStore;
      dfaccumulatetogrid_(DComplex_gridStore, 
			  convFuncV, 
			  &nvalue, 
			  &dataWVal,
			  supportPtr,samplingPtr,offPtr, convOriginPtr,
			  cfShapePtr,locPtr,igrdposPtr, 
			  &sinDPA, &cosDPA,
			  &finitePointingOffsets_int, 
			  &psfOnly_int,
			  &norm, phaseGradPtr,
			  &gnx, &gny, &gnp, &gnc,
			  &cf0, &cf1, &cf2, &cf3,
			  &phx, &phy);
    //   if (fpclassify(abs(norm)) == FP_NAN)
    //   	{
    //   	  cerr << "dN NAN: " << getpid() << " " << nvalue << " " << supportPtr[0] << " " << sampling << " " << off << " " << convOrigin << " " << loc << " " << igrdpos << " " << cfShapePtr[0] << endl;
    //   	  throw(AipsError("NAN DETECTED IN NORM"));
    //   	}
    //   if (fpclassify(vbs.imagingWeight_p(ichan, irow)) == FP_NAN)
    //   	{
    //   	  cerr << "dWT NAN: " << nvalue << " " << support << " " << sampling << " " << off << " " << convOrigin << " " << loc << " " << igrdpos << " " << cfShape << endl;
    //   	  throw(AipsError("NAN DETECTED IN WT"));
    //   	}
    }
}
