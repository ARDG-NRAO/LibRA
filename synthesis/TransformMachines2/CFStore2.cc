// -*- C++ -*-
//# CFStore2.cc: Implementation of the CFStore2 class
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
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/CFBuffer.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines2/PolOuterProduct.h>

using namespace casacore;
namespace casa{
  using namespace refim;
  //
  //---------------------------------------------------------------
  //
  CFStore2 & CFStore2::operator=(const CFStore2& other)
  {
    if (&other != this)
      {
	storage_p.assign(other.storage_p); 
	ant1_p.assign(other.ant1_p);
	ant2_p.assign(other.ant2_p);
	pa_p.assign(other.pa_p);
	lazyFillOn_p=other.lazyFillOn_p;
	currentSPWID_p = other.currentSPWID_p;
	cfCacheDir_p = other.cfCacheDir_p;
      }
    return *this;
  };
  //
  //---------------------------------------------------------------
  //
  void CFStore2::show(const char *Mesg, ostream& os, const Bool verbose)
  {
    if (!null())
      {
	if (Mesg != NULL) os << Mesg << endl;
	os << "Data Shape: " << storage_p.shape() << endl;
	os << "Ant1: " << ant1_p << endl;
	os << "Ant2: " << ant2_p << endl;
	os << "PA = "; for (uInt i=0;i<pa_p.nelements();i++)
			 os << pa_p[i].get("deg") << endl;

	if (verbose)
	  for (int i=0; i<storage_p.shape()(0); i++)
	    for(int j=0;j<storage_p.shape()(1);j++)
	      storage_p(i,j)->show(Mesg,os);
      }
  };
  //
  //---------------------------------------------------------------
  //
  Vector<Int> CFStore2::resize(const Quantity& pa,
			       const Quantity& paTol,
			       const Int& ant1, const Int& ant2,
			       Bool retainValues)
  {
    Int paNdx, antNdx;
    //    getIndex(pa,wValue,ant1,ant2,paNdx, antNdx, wNdx);
    getIndex(pa,paTol,ant1,ant2,paNdx, antNdx);

    // If a index along any axis is negative, resize the correponding
    // vector holding the values along that axis.
    if (paNdx < 0) 
      {paNdx=pa_p.nelements();pa_p.resize(paNdx+1,retainValues);};
    if (antNdx < 0) 
      {antNdx=ant1_p.nelements();ant1_p.resize(antNdx+1,true);ant2_p.resize(antNdx+1,retainValues);}; 
    // if (wNdx < 0) 
    //   {wNdx=wValue_p.nelements();wValue_p.resize(wNdx+1,retainValues);}
    // Resize the storage_p Cube to the current size of required along
    // the 3 axis.  This is a NoOp if the axis lengths did not change.
    storage_p.resize(pa_p.nelements(), ant1_p.nelements(), retainValues);
    
    pa_p[paNdx] = pa;
    ant1_p[antNdx] = ant1;
    ant2_p[antNdx] = ant2;

    Vector<Int> pos(2);pos(0)=paNdx;pos(1)=antNdx;
    if (storage_p(pos(0),pos(1)).null())
      {
      storage_p(pos(0),pos(1)) = new CFBuffer;
      storage_p(pos(0),pos(1))->setDir(cfCacheDir_p);
      }
    return pos;
  };
  //
  //---------------------------------------------------------------
  //
  void CFStore2::setCFBuffer(CFBuffer* /*dataPtr*/, 
			     Quantity /*pa*/, 
			     const Int& /*ant1*/, const Int& /*ant2*/)
  {
    throw(AipsError("setCFBuffer called!"));
    // Vector<Int> pos=resize(pa,ant1,ant2);
    // storage_p(pos[0], pos[1]) = dataPtr;
  }
  //
  //---------------------------------------------------------------
  //
  CountedPtr<CFBuffer>& CFStore2::getCFBuffer(const Quantity& pa, 
					      const Quantity& paTol, 
					      const Int& ant1, const Int& ant2)
  {
    Int paNdx, antNdx;
    getIndex(pa,paTol,ant1,ant2,paNdx, antNdx);
    if ((paNdx < 0) || (antNdx < 0)) 
	throw(CFNotCached("CFStore2::getIndex: Index not found"));
      
    return getCFBuffer(paNdx, antNdx);
  }
  //
  //---------------------------------------------------------------
  //
  // Get CFBuffer by direct indexing
  CountedPtr<CFBuffer>& CFStore2::getCFBuffer(const Int& paNdx, const Int& antNdx)
  {
    return storage_p(paNdx, antNdx);
  }
  //
  //---------------------------------------------------------------
  //
  void CFStore2::getParams(Quantity& pa,
			   Int& ant1, Int& ant2, 
			   const Int& paNdx, const Int& antNdx)
  {
    pa     = pa_p[paNdx];
    ant1   = ant1_p[antNdx];
    ant2   = ant2_p[antNdx];
  }
  //
  //---------------------------------------------------------------
  // This version saves the CFBuffer that corresponds to the
  // [PA,(Ant1,Ant2)] pixel of the CFStore storage.
  void CFStore2::makePersistent(const char *dir,
				const char *cfName,
				const char *qualifier,
				const Quantity &pa, const Quantity& dPA,
				const Int& ant1, const Int& ant2)
  {
    LogIO log_l(LogOrigin("CFStore2", "makePersistent(pa)[R&D]"));
    (void)cfName;
    Int paNdx, antNdx;
    getIndex(pa,dPA,ant1,ant2,paNdx, antNdx);
    
    log_l << "Saving CFStore of shape " << storage_p(paNdx,antNdx)->shape() 
	  << " for PA = " << pa.getValue("deg") 
	  << " BaselineType(antType1, antType2) = (" << ant1 << "," << ant2 << ")" 
	  << LogIO::POST;
    ostringstream name;
    name << String(qualifier) << "CFS_" << paNdx << "_" << antNdx;
    storage_p(paNdx,antNdx)->makePersistent(dir, name.str().c_str());
  }
  //
  //---------------------------------------------------------------
  //
  void CFStore2::makePersistent(const char *dir,
				const char *cfName,
				const char *qualifier)
  {
    (void)cfName;
    LogIO log_l(LogOrigin("CFStore2.2", "makePersistent[R&D]"));
    //const char *formedName;

    for (Int i=0;i<storage_p.shape()(0);i++)
      for (Int j=0;j<storage_p.shape()(1);j++)
	{
	  log_l << "Writing CFStore("<<i<<","<<j<<") of shape " << storage_p(i,j)->shape() << LogIO::POST;
	  ostringstream name;
	  //name << dir << "/" << qualifier << "CFS_" << i << "_" << j;
	  name << String(qualifier) << "CFS_" << i << "_" << j;
	  // if (String(cfName) == "") formedName = name.str().c_str();
	  // else              formedName = cfName;
	  storage_p(i,j)->makePersistent(dir,name.str().c_str());
	}
  }
  //
  //---------------------------------------------------------------
  //
  void CFStore2::primeTheCFB()
  {
    IPosition shp=getShape();
    for (Int i=0; i<shp(0); i++)
      for (Int j=0; j<shp(1); j++)
	getCFBuffer(i,j)->primeTheCache();
  }
  //
  //---------------------------------------------------------------
  //
  void CFStore2::initMaps(const VisBuffer2& vb, const Matrix<Double>& freqSelection, const Double& imRefFreq)
  {
    for (Int i=0;i<storage_p.shape()(0);i++)
      for (Int j=0;j<storage_p.shape()(1);j++)
	storage_p(i,j)->initMaps(vb, freqSelection, imRefFreq);
  }
  //
  //---------------------------------------------------------------
  //
  void CFStore2::initPolMaps(PolMapType& polMap, PolMapType& conjPolMap)
  {
    IPosition shp=getShape();
    for (Int i=0; i<shp(0); i++)
      for (Int j=0; j<shp(1); j++)
	getCFBuffer(i,j)->initPolMaps(polMap, conjPolMap);
  }
  //
void CFStore2::clear()
  {
    IPosition cfsShape = getStorage().shape();

    for (Int ib=0;ib<cfsShape(0); ib++)
      for(int it=0;it<cfsShape(1); it++)
	  getStorage()(ib,it)->clear();
  }
  //
  //---------------------------------------------------------------
  //
  //---------------------------------------------------------------
  //
  Double CFStore2::memUsage()
  {
    IPosition cfsShape = getStorage().shape();
    Double memUsed=0;
    for (Int ib=0;ib<cfsShape(0); ib++)
      for(int it=0;it<cfsShape(1); it++)
	{
	  IPosition cfbShape = getStorage()(ib,it)->getStorage().shape();
	  for(int ip=0;ip<cfbShape(2); ip++)
	    for(Int ich=0;ich<cfbShape(0);ich++)
	      for(Int iw=0;iw<cfbShape(1); iw++)
		memUsed += getStorage()(ib,it)->getStorage()(ich,iw,ip)->getStorage()->shape()
		  .product()*sizeof(Complex);
	}
    return memUsed;
  }
  //
  //---------------------------------------------------------------
  //
  Int CFStore2::nearestPA(const Quantity& pa, const Quantity& paTol)
    {
      Int n=pa_p.nelements(), junk=-1;
      Float dpa=paTol.getValue("rad"),
	paVal = pa.getValue("rad"), cpa;

      for(Int i=0;i<n;i++)
	{
	  cpa = pa_p(i).getValue("rad");
	  //	  cerr << "##### " << i << " " << cpa*57.2956 << " " << paVal*57.2956 << " " << dpa*57.2956 << " " << (fabs(cpa - paVal))*57.2956 << endl; 
	  // if (fabs(cpa - paVal) > dpa) 
	  //   {cout << "%%%%% "; cin >> junk;}
	  if (fabs(cpa - paVal) < dpa) {junk=i;break;}
	}
      return junk;
    }
  //
  //---------------------------------------------------------------
  //
  void CFStore2::setCFCacheDir(const String& dir)
  {
    IPosition cfsShape = getStorage().shape();

    for (Int ib=0;ib<cfsShape(0); ib++)
      for(int it=0;it<cfsShape(1); it++)
	  getStorage()(ib,it)->setDir(dir);

    cfCacheDir_p=dir;
  }
  //
  //---------------------------------------------------------------
  //
  void CFStore2::invokeGC(const Int& spwID)
  {
    if (isLazyFillOn())
      {
    	if (spwID != currentSPWID_p)
	  {
	    // LogIO log_l(LogOrigin("CFStore2", "invokeGC"));
	    // log_l << "Invoking Garbage Collector: ";
	    // // The reason for invoking GC
	    // if (currentSPWID_p < 0)      log_l << "Initial mopping-up";
	    // else     log_l << "SPW" << currentSPWID_p << "->SPW" << spwID;
	    // log_l << LogIO::DEBUGGING;

	    clear(); currentSPWID_p=spwID;
	  }
      }
  }
}; // end casa namespace



