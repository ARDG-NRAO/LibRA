// -*- C++ -*-
//# FortranizedLoops.h: Forward decleration of FORTRAN functions
//# with the gridding/de-gridding inner loops.
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
namespace casa{
  extern "C" 
  {
    void faccumulatetogrid_(casacore::Complex *grid, casacore::Complex *CF, 
			    casacore::Complex *nvalue, casacore::Double *wVal, 
			    casacore::Int *scaledSupport, casacore::Float *scaledSampling,
			    casacore::Double *off, casacore::Int *convOrigin, casacore::Int *cfShape, casacore::Int *loc,
			    casacore::Int *igrdpos, casacore::Double *sinDPA, casacore::Double *cosDPA,
			    casacore::Int *finitePointingOffset,
			    casacore::Int *doPSFOnly,
			    casacore::Complex *norm,
			    casacore::Complex *phaseGrad,
			    casacore::Int *imNX, casacore::Int *imNY, casacore::Int *imNP, casacore::Int *imNC,
			    casacore::Int *cfNX, casacore::Int *cfNY, casacore::Int *cfNP, casacore::Int *cfNC,
			    casacore::Int *phNX, casacore::Int *phNY);
    void dfaccumulatetogrid_(casacore::DComplex *grid, casacore::Complex *CF, 
			    casacore::Complex *nvalue, casacore::Double *wVal, 
			    casacore::Int *scaledSupport, casacore::Float *scaledSampling,
			    casacore::Double *off, casacore::Int *convOrigin, casacore::Int *cfShape, casacore::Int *loc,
			    casacore::Int *igrdpos, casacore::Double *sinDPA, casacore::Double *cosDPA,
			    casacore::Int *finitePointingOffset,
			    casacore::Int *doPSFOnly,
			    casacore::Complex *norm,
			    casacore::Complex *phaseGrad,
			    casacore::Int *imNX, casacore::Int *imNY, casacore::Int *imNP, casacore::Int *imNC,
			    casacore::Int *cfNX, casacore::Int *cfNY, casacore::Int *cfNP, casacore::Int *cfNC,
			    casacore::Int *phNX, casacore::Int *phNY);
    void faccumulatefromgrid_(casacore::Complex *nvalue,casacore::Complex *norm,
			      const casacore::Complex *grid, casacore::Complex *CF, 
			      casacore::Double *wVal, 
			      casacore::Int *scaledSupport, casacore::Float *scaledSampling,
			      casacore::Double *off, casacore::Int *convOrigin, casacore::Int *cfShape, casacore::Int *loc,
			      casacore::Int *igrdpos, casacore::Double *sinDPA, casacore::Double *cosDPA,
			      casacore::Int *finitePointingOffset,
			      casacore::Complex *phaseGrad,
			      casacore::Complex *phasor,
			      casacore::Int *imNX, casacore::Int *imNY, casacore::Int *imNP, casacore::Int *imNC,
			      casacore::Int *cfNX, casacore::Int *cfNY, casacore::Int *cfNP, casacore::Int *cfNC,
			      casacore::Int *phNX, casacore::Int *phNY);
  };
}
