// -*- C++ -*-
//# CFCell.h: Definition of the CFCell class
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
#ifndef SYNTHESIS_CFCELL_H
#define SYNTHESIS_CFCELL_H
#include <synthesis/TransformMachines/CFDefs.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Utilities/CountedPtr.h>
#include <images/Images/ImageInterface.h>
#include <msvis/MSVis/VisBuffer.h>
//
// <summary> 
//
// A light-weight container to carray all the information required for
// a single convolution function plan.
//
//</summary>

// <prerequisite>
// </prerequisite>
//
// <etymology> 
//
//  CFCell is basic in-memory representation of a single
//  monochromatic, single polarization cross-product and single
//  w-plane convoluion function .
//
//</etymology>
//
// <synopsis> 
//
// Mostly a conveniance class to pass around related information.  
//
//</synopsis>
//
// <example>
// </example>
//
// <motivation>
//
// To avoid parameter-bloat to passing around a number of related objects seperately.
//
// </motivation>
//

namespace casa { //# NAMESPACE CASA - BEGIN
  typedef casacore::Complex TT;

  struct  CFCStruct{
    casacore::CoordinateSystem coordSys;
    TT * CFCStorage;
    casacore::Int shape[2];
    casacore::Float sampling,diameter;
    casacore::Int xSupport, ySupport;
    casacore::Double wValue, wIncr, freqValue,freqIncr, conjFreq;    
    casacore::Int muellerElement, conjPoln;
    casacore::String fileName, telescopeName, bandName;
    bool isRotationallySymmetric;
  };

  using namespace CFDefs;
  using namespace std;
  //  template <class T>
  class CFCell
  {
  void initCFCStruct(CFCStruct& cfcSt) 
  {
    cfcSt.CFCStorage=NULL;
    cfcSt.xSupport = cfcSt.ySupport=0;
    cfcSt.sampling=0.0;
    cfcSt.shape[0]=cfcSt.shape[1]=0;
  }

  public:
    //
    //========================= Administrative Parts ==========================
    //------------------------------------------------------------------
    //
    CFCell():cfShape_p(),isRotationallySymmetric_p(false){};

    CFCell(casacore::Array<TT> &dataPtr, casacore::CoordinateSystem& cs, casacore::Float& /*samp*/):
      isRotationallySymmetric_p(false)
    {
      if (storage_p.null()) storage_p = new casacore::Array<TT>(dataPtr);
      coordSys_p = cs;
      cfShape_p.assign(storage_p->shape().asVector());
    };

    ~CFCell() 
    {
      if (!storage_p.null()) 
	{
	  //cerr << "############### " << "~CFCell() called " << storage_p->shape() << endl;
	  storage_p->resize();
	}
    };

    void getAsStruct(CFCStruct& cfst) 
    {
      casacore::Bool dummy;
      cfst.CFCStorage = getStorage()->getStorage(dummy);
      cfst.coordSys = coordSys_p;
      cfst.shape[0]=cfShape_p[0];
      cfst.shape[1]=cfShape_p[1];
      cfst.sampling=sampling_p;
      cfst.xSupport=xSupport_p;
      cfst.ySupport=ySupport_p;
      cfst.wValue=wValue_p;
      cfst.wIncr=wIncr_p;
      cfst.freqValue=freqValue_p;
      cfst.freqIncr=freqIncr_p;
      cfst.muellerElement=muellerElement_p;
      cfst.conjFreq = conjFreq_p;
      cfst.conjPoln = conjPoln_p;
      cfst.diameter=diameter_p;
      cfst.fileName = fileName_p;
      cfst.telescopeName=telescopeName_p;
      cfst.bandName = bandName_p;
      cfst.isRotationallySymmetric=isRotationallySymmetric_p;
    }
    casacore::CountedPtr<casacore::Array<TT> >& getStorage() {return storage_p;}
    void setStorage(casacore::Array<TT>& val) {getStorage()->assign(val); cfShape_p=val.shape().asVector();};
    void clear();
    void makePersistent(const char *dir, const char *cfName="");
    casacore::CountedPtr<CFCell> clone();
    void setParams(const CFCell& other);
    void initCache(const casacore::Bool& releaseSpace=false) {shape_p=getShape(); cfShape_p.assign(shape_p.asVector());casacore::IPosition tt=shape_p;tt=0;tt[0]=tt[1]=0;if (releaseSpace) storage_p->resize(tt);};
    casacore::IPosition getShape() {return storage_p->shape();}
    //
    //============================= casacore::Functional Parts ============================
    //------------------------------------------------------------------
    //
    void show(const char *Mesg,ostream &os);

    casacore::IPosition shape_p;
    casacore::CountedPtr<casacore::Array<TT> > storage_p; // Nx x Ny
    casacore::CoordinateSystem coordSys_p;
    casacore::Float sampling_p, diameter_p;
    casacore::Int xSupport_p,ySupport_p, conjPoln_p;
    casacore::Double wValue_p, wIncr_p, freqValue_p,freqIncr_p, conjFreq_p;
    //    MuellerElementType muellerElement_p;
    casacore::Int muellerElement_p;
    casacore::Quantity pa_p;
    casacore::Vector<casacore::Int> cfShape_p;
    casacore::String fileName_p,telescopeName_p, bandName_p;
    bool isRotationallySymmetric_p;
  };
} //# NAMESPACE CASA - END
#endif
