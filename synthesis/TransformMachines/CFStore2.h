// -*- C++ -*-
//# CFStore2.h: Definition of the CFStore2 class
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
#ifndef SYNTHESIS_CFSTORE2_H
#define SYNTHESIS_CFSTORE2_H
#include <synthesis/TransformMachines/CFDefs.h>
#include <synthesis/TransformMachines/CFBuffer.h>
#include <synthesis/TransformMachines/CFCell.h>
#include <synthesis/TransformMachines/VBStore.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Utilities/CountedPtr.h>
#include <images/Images/ImageInterface.h>
#include <msvis/MSVis/VisBuffer.h>
namespace casa { //# NAMESPACE CASA - BEGIN
  using namespace CFDefs;

  typedef casacore::Cube<casacore::CountedPtr<CFCell > > VBRow2CFMapType;
  typedef casacore::Vector<casacore::CountedPtr<CFBuffer > > VBRow2CFBMapType;
  class CFStore2
  {
  public:
    CFStore2():storage_p(), pa_p(), lazyFillOn_p(False), mosPointingPos_p(0), currentSPWID_p(-1), cfCacheDir_p("") {};

    // CFStore2(CFBuffer<casacore::Complex> *dataPtr, casacore::Quantity PA, casacore::Int mosPointing):
    //   storage_p(), pa_p(PA), mosPointingPos_p(mosPointing)
    // {storage_p = new CFBuffer<casacore::Complex>(*dataPtr);};

    virtual ~CFStore2() {};

    CFStore2& operator=(const CFStore2& other);
    //-------------------------------------------------------------------------
    void show(const char *Mesg=NULL,ostream &os=cerr, const casacore::Bool verbose=false);
    //-------------------------------------------------------------------------
    // This version saves the entire CFStore
    void makePersistent(const char *dir,const char *name="", const char *qualifier="");
    // This version saves only those pixels of CFStore that correspond
    // to [PA,(Ant1,Ant2)] co-ordiantes ([PA, BaselineType]
    // co-ordinate).
    void makePersistent(const char *dir,
			const char *cfName,
			const char *qualifier,
			const casacore::Quantity &pa, const casacore::Quantity& dPA,
			const casacore::Int& ant1, const casacore::Int& ant2);
    //-------------------------------------------------------------------------
    void primeTheCFB();
    //-------------------------------------------------------------------------
    void initMaps(const VisBuffer& vb, const casacore::Matrix<casacore::Double>& freqSelection, 
		  const casacore::Double& imRefFreq);
    //-------------------------------------------------------------------------
    void initPolMaps(PolMapType& polMap, PolMapType& conjPolMap);
    //-------------------------------------------------------------------------
    casacore::Bool null() {return (storage_p.size() == 0);};
    //-------------------------------------------------------------------------
    casacore::Double memUsage();
    //-------------------------------------------------------------------------
    void clear();
    //-------------------------------------------------------------------------
    void set(const CFStore2& other)
    {
      pa_p.assign(other.pa_p);
      ant1_p.assign(other.ant1_p);
      ant2_p.assign(other.ant2_p);
    }
    //-------------------------------------------------------------------------
    void setCFBuffer(CFBuffer *dataPtr, casacore::Quantity pa, 
		     const casacore::Int& ant1, const casacore::Int& ant2);
    //-------------------------------------------------------------------------
    casacore::CountedPtr<CFBuffer>& getCFBuffer(const casacore::Quantity& pa, 
				      const casacore::Quantity& paTol, 
				      const casacore::Int& ant1, const casacore::Int& ant2);
    //-------------------------------------------------------------------------
    void setCFCacheDir(const String& dir);
    casacore::String getCFCacheDir() {return cfCacheDir_p;};
    void setLazyFill(const Bool& val) {lazyFillOn_p=val;}
    casacore::Bool isLazyFillOn() {return lazyFillOn_p;}
    void invokeGC(const casacore::Int& spwID);
    //-------------------------------------------------------------------------
    // Get CFBuffer by directly indexing in the list of CFBuffers
    casacore::CountedPtr<CFBuffer>& getCFBuffer(const casacore::Int& paNdx, const casacore::Int& antNdx);
    CFBuffer& operator()(const casacore::Int& paNdx, const casacore::Int& antNdx) {return *storage_p(paNdx, antNdx);}
    void getParams(casacore::Quantity& pa, 
		   casacore::Int& ant1, casacore::Int& ant2, 
		   const casacore::Int& paNdx, const casacore::Int& antNdx);
    //-------------------------------------------------------------------------
    //
    // Generate a map for the given frequency and Mueller element list
    // to the index in the internal list of CFs.  This can be used in
    // tight loops to get get direct access to the required CF.
    //
    /*
    void makeVBRow2CFMap(VBRow2CFMapType& vbRow2CFMap,
			 const VisBuffer& vb, 
			 const casacore::Quantity& paTol,
			 const casacore::Vector<casacore::Int>& dataChan2ImChanMap,
			 const casacore::Vector<casacore::Int>& dataPol2ImPolMap);
    */
    //-------------------------------------------------------------------------
    casacore::Vector<casacore::Int> resize(const casacore::Quantity& pa, const casacore::Quantity& paTol, 
		       const casacore::Int& ant1,const casacore::Int& ant2, casacore::Bool retainValues=true);
    // void rememberATerm(casacore::CountedPtr<ATerm>& aTerm) {theATermUsed_p = aTerm;}

    // casacore::Int mapAntIDToAntType(const casacore::Int& ant) {return theATermUsed_p->mapAntIDToAntType(ant);};
    casacore::Matrix<casacore::CountedPtr<CFBuffer> >& getStorage() {return storage_p;}
    casacore::Vector<casacore::Int>& getAnt1List() {return ant1_p;};
    casacore::Vector<casacore::Int>& getAnt2List() {return ant2_p;};
    casacore::Vector<casacore::Quantity> getPAList() {return pa_p;};
    casacore::IPosition getShape() {return storage_p.shape();}


  protected:

    casacore::Matrix<casacore::CountedPtr<CFBuffer > > storage_p;
    casacore::Vector<casacore::Int> ant1_p, ant2_p;
    casacore::Vector<casacore::Quantity> pa_p;
    casacore::Bool lazyFillOn_p;
    casacore::Int mosPointingPos_p, currentSPWID_p;
    casacore::String cfCacheDir_p;

    virtual void getIndex(const casacore::Quantity& pa, 
			  const casacore::Quantity& paTol, 
			  const casacore::Int& ant1, const casacore::Int& ant2,
			  casacore::Int& paNdx, casacore::Int& antNdx)
    {
      paNdx  = paHashFunction(pa,paTol);
      antNdx = antHashFunction(ant1,ant2);
    }

    // virtual casacore::Int wHashFunction(const casacore::Double& wValue)
    // {
    //   casacore::Int ndx=-1;
    //   for(casacore::uInt i=0;i<wValue_p.nelements(); i++)
    // 	if (wValue_p[i] == wValue)
    // 	  {ndx=i;break;}
    //   return ndx;
    // }
    virtual casacore::Int antHashFunction(const casacore::Int& ant1, const casacore::Int& ant2)
    {
      casacore::Int ndx=-1;
      for (casacore::uInt i=0;i<ant1_p.nelements(); i++)
	if ((ant1_p[i]==ant1) && (ant2_p[i]==ant2))
	  {ndx=i;break;}
      return ndx;
    };

    virtual casacore::Int paHashFunction(const casacore::Quantity& pa,const casacore::Quantity& paTol)
    {
      // for (casacore::uInt i=0; i<pa_p.nelements(); i++)
      // 	if ( pa_p[i] == pa)
      // 	  {ndx=i;break;}
      return nearestPA(pa, paTol);
    }

    virtual casacore::Int nearestPA(const casacore::Quantity& pa, const casacore::Quantity& paTol);
  };
} //# NAMESPACE CASA - END

#endif
