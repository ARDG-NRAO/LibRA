// -*- C++ -*-
//# CFBuffer.cc: Implementation of the CFBuffer class
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
#include <synthesis/TransformMachines/CFBuffer.h>
#include <synthesis/TransformMachines/Utils.h>
#include <casacore/casa/Utilities/BinarySearch.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <casa/Utilities/Assert.h>
using namespace casacore;

namespace casa{

  // Instantiate a commonly used extern template

  //
  //---------------------------------------------------------------
  //
  //
  void CFBuffer::setParams(const CFBuffer& other)
  {
    wValues_p.assign(other.wValues_p);
    freqValues_p.assign(other.freqValues_p);
    muellerElements_p.assign(other.muellerElements_p);
    muellerElementsIndex_p.assign(other.muellerElementsIndex_p);
    conjMuellerElements_p.assign(other.conjMuellerElements_p);
    conjMuellerElementsIndex_p.assign(other.conjMuellerElementsIndex_p); 
    wValIncr_p = other.wValIncr_p; 
    freqValIncr_p = other.freqValIncr_p;
    muellerMask_p.assign(other.muellerMask_p);
    pointingOffset_p.assign(other.pointingOffset_p);
    freqNdxMapsReady_p = other.freqNdxMapsReady_p;

    freqNdxMap_p.assign(other.freqNdxMap_p);
    for(uInt i=0;i<freqNdxMap_p.nelements();i++) freqNdxMap_p[i].assign(other.freqNdxMap_p[i]);
    conjFreqNdxMap_p.assign(other.conjFreqNdxMap_p);
    for(uInt i=0;i<conjFreqNdxMap_p.nelements();i++) conjFreqNdxMap_p[i].assign(other.conjFreqNdxMap_p[i]);
    cfCacheDirName_p = other.cfCacheDirName_p;
  }
  //---------------------------------------------------------------
  //
  //
  CountedPtr<CFBuffer> CFBuffer::clone()
  {
    CountedPtr<CFBuffer> clone=new CFBuffer();
    clone->setParams(*this);

    IPosition shp(cfCells_p.shape());
    clone->resize(shp);
    clone->allocCells(cfCells_p);
    // clone->show("####CLONE: ",cerr);
    return clone;
  }
  void CFBuffer::allocCells(const Cube<CountedPtr<CFCell> >& cells)
  {
    IPosition shp(cells.shape());
    for (Int i=0;i < shp[0]; i++)
      for (Int j=0; j < shp[1]; j++)
	for (Int k=0; k < shp[2]; k++)
	  {
	    cfCells_p(i,j,k) = cells(i,j,k)->clone();
	  }
  }
  //---------------------------------------------------------------
  //
  //  template<class T> Int CFBuffer<T>
  Int CFBuffer::noOfMuellerElements(const PolMapType& muellerElements)
  {
    Int n=0,nrows = muellerElements.nelements();
    for (Int i=0;i<nrows;i++)
      n+=muellerElements(i).nelements();
    return n;
    // Int n=0;
    // for(Int i=0;i<muellerElements.nelements();i++)
    //   for(Int j=0;j<muellerElements(i).nelements();j++)
    // 	if (muellerElements(i)(j) > n) n=muellerElements(i)(j);
    // return n;
  }
  //
  //---------------------------------------------------------------
  //
  void CFBuffer::clear()
  {
    IPosition shp(cfCells_p.shape());
    for (Int i=0;i < shp[0]; i++)
      for (Int j=0; j < shp[1]; j++)
	for (Int k=0; k < shp[2]; k++)
	  {
	    cfCells_p(i,j,k)->clear();
	  }
  }
  //
  //---------------------------------------------------------------
  //
  //  template<class T>  void CFBuffer<T>
  void CFBuffer::resize(const Double& wScale, const Double& freqIncr,
			const Vector<Double>& wValues, 
			const Vector<Double>& freqValues,
			const PolMapType& muellerElements,
			const PolMapType& muellerElementsIndex,
			const PolMapType& conjMuellerElements,
			const PolMapType& conjMuellerElementsIndex
			)
  {
    wValues_p.assign(wValues);
    freqValues_p.assign(freqValues);
    wValIncr_p = wScale;
    freqValIncr_p = freqIncr;
    //    muellerMask_p.assign(muellerElements);

    //    nPol_p=noOfMuellerElements(muellerMask_p);
    nPol_p=noOfMuellerElements(muellerElementsIndex);
    nChan_p = freqValues_p.nelements();
    nW_p = wValues_p.nelements();

    //    muellerElements_p.resize(nPol_p);
    muellerElements_p.assign(muellerElements);
    muellerElementsIndex_p.assign(muellerElementsIndex);
    conjMuellerElements_p.assign(conjMuellerElements);
    conjMuellerElementsIndex_p.assign(conjMuellerElementsIndex);
    // Resize the various aux. information storage buffers.
    //
    // Resize the storage.  Retain the value of the existing pixels.
    // New pixels due to resize, if any, are assigned a new Array<T>
    // pointer.
    cfCells_p.resize(nChan_p, nW_p, nPol_p, true);

    for (uInt i=0;i<cfCells_p.shape()(0);i++)      // nChan_p
      for (uInt j=0;j<cfCells_p.shape()(1);j++)    // nW_p
	{
	  Int k=0;                                 // nPol_p
	  for(uInt prow=0;prow<muellerElements_p.nelements();prow++)
	    for(uInt pcol=0;pcol<muellerElements_p(prow).nelements();pcol++)
	      {
		if (cfCells_p(i,j,k).null()) cfCells_p(i,j,k) = new CFCell;
		if (cfCells_p(i,j,k)->storage_p.null()) cfCells_p(i,j,k)->storage_p=new Array<TT>;
		cfCells_p(i,j,k)->freqValue_p = freqValues(i);
		cfCells_p(i,j,k)->freqIncr_p = freqIncr;
		cfCells_p(i,j,k)->wValue_p = wValues(j);
		cfCells_p(i,j,k)->wIncr_p = wValIncr_p;
		cfCells_p(i,j,k)->muellerElement_p = muellerElements_p(prow)(pcol);
		cfCells_p(i,j,k)->xSupport_p = 0;
		cfCells_p(i,j,k)->ySupport_p = 0;
		k++;
	      }
	}
  }
  //
  //---------------------------------------------------------------
  //
  //  template <class T>  void CFBuffer<T>
  // RigidVector<Int, 3> CFBuffer::setParams(const Int& inu, const Int& iw, const Int& muellerElement,
  // 					  const TableRecord& miscInfo)
  // {
  //   RigidVector<Int,3> ndx = setParams(inu, iw, 
  // 				       miscInfo.coordSys, miscInfo.sampling,
  // 				       miscInfo.xSupport,miscInfo.ySupport,
  // 				       miscInfo.freqValue, miscInfo.wValue,
  // 				       muellerElements, 
  // 				       miscInfo.fileName, miscInfo.conjfFreq,
  // 				       miscinfo.conjPoln);
  //   cfCells_p(ndx(0), ndx(1), ndx(2))->telescopeName = micsInfo.telescopeName;
  // }
  //
  //---------------------------------------------------------------
  //
  //  template <class T>  void CFBuffer<T>
  RigidVector<Int, 3> CFBuffer::setParams(const Int& inu, const Int& iw, const Int& ipx, const Int& ipy,
					  const Double& freqValue, 
					  const Double& wValue,
					  const Int& muellerElement,
					  CoordinateSystem& cs,
					  const TableRecord& miscInfo)
  {
    float sampling; miscInfo.get("Sampling",sampling);
    int xSupport, ySupport; miscInfo.get("Xsupport",xSupport);miscInfo.get("Ysupport",ySupport);
    String fileName; miscInfo.get("Name",fileName);
    double conjFreq; miscInfo.get("ConjFreq", conjFreq);
    int conjPoln; miscInfo.get("ConjPoln", conjPoln);
    String telescopeName; miscInfo.get("TelescopeName", telescopeName);
    String bandName; miscInfo.get("BandName", bandName);
    float diameter; miscInfo.get("Diameter", diameter);
    // In the absense of evidence, assume that users are sensible and
    // are using AWProjection where it is really need it and not for
    // using it as a replacement for rotatially symmetric stuff.  So
    // by default, the CFs are assumed to be rotationally asymmetric.
    bool isRotationallySymmetric=True; 
    
    if (miscInfo.isDefined("OpCode"))
	miscInfo.get("OpCode",isRotationallySymmetric);

    RigidVector<Int,3> ndx=setParams(inu, iw, ipx, ipy, freqValue, bandName, wValue, muellerElement, cs,
				     sampling, xSupport, ySupport, fileName, conjFreq, conjPoln, telescopeName,
				     diameter);
    cfCells_p(ndx(0),ndx(1),ndx(2))->isRotationallySymmetric_p = isRotationallySymmetric;
    return ndx;
  }
  RigidVector<Int, 3> CFBuffer::setParams(const Int& inu, const Int& iw, const Int& /*ipx*/, const Int& /*ipy*/,
					  const Double& freqValue,
					  const String& bandName,
					  const Double& wValue,
					  const Int& muellerElement,
					  CoordinateSystem& cs,
					  Float& sampling,
					  Int& xSupport, Int& ySupport, 
					  const String& fileName,
					  const Double& conjFreq,
					  const Int& conjPoln,
					  const String& telescopeName,
					  const Float& diameter)
  {
    RigidVector<Int,3> ndx=getIndex(freqValue, wValue, muellerElement);
    ndx(0)=inu; ndx(1)=iw;//ndx(2) = muellerElements_p(ipx)(ipy);
    cfCells_p(ndx(0),ndx(1),ndx(2))->sampling_p = sampling;
    cfCells_p(ndx(0),ndx(1),ndx(2))->xSupport_p = xSupport;
    cfCells_p(ndx(0),ndx(1),ndx(2))->ySupport_p = ySupport;
    cfCells_p(ndx(0),ndx(1),ndx(2))->muellerElement_p = muellerElement;
    cfCells_p(ndx(0),ndx(1),ndx(2))->conjFreq_p = conjFreq;
    cfCells_p(ndx(0),ndx(1),ndx(2))->conjPoln_p = conjPoln;
    if (fileName != "")
      cfCells_p(ndx(0),ndx(1),ndx(2))->fileName_p = fileName;
    cfCells_p(ndx(0),ndx(1),ndx(2))->telescopeName_p = telescopeName;//String("EVLA");
    cfCells_p(ndx(0),ndx(1),ndx(2))->diameter_p = diameter;//25.0;
    if (bandName != "") cfCells_p(ndx(0),ndx(1),ndx(2))->bandName_p = bandName;

    Int index=cs.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate spCS = cs.spectralCoordinate(index);
    Vector<Double> val; val.resize(1);val(0)=freqValues_p(ndx(0));
    spCS.setReferenceValue(val);
    //val.resize();
    //val = spCS.increment();
    //val = cfCells_p(ndx(0),ndx(1),ndx(2))->freqIncr_p;
    //spCS.setIncrement(val);
    cs.replaceCoordinate(spCS,index);

    // cfCells_p(ndx(0),ndx(1),ndx(2))->freqValue_p = 
    //   cs.spectralCoordinate(cs.findCoordinate(Coordinate::SPECTRAL)).referenceValue()(0);
    // cfCells_p(ndx(0),ndx(1),ndx(2))->freqIncr_p  = 
    //   cs.spectralCoordinate(cs.findCoordinate(Coordinate::SPECTRAL)).increment()(0);

    cfCells_p(ndx(0),ndx(1),ndx(2))->coordSys_p  = cs;
    return ndx;
  }
  //
  //---------------------------------------------------------------
  //
  void CFBuffer::setPA(Float& pa)
  {
    RigidVector<Int,3> ndx(-1);
    Int nF=cfCells_p.shape()(0), nW=cfCells_p.shape()(1), nM=cfCells_p.shape()(2);
    for (Int i=0; i<nF; i++)
      for(Int j=0; j<nW; j++)
	for(Int k=0; k<nM; k++)
	  cfCells_p(i,j,k)->pa_p=Quantity(pa,"deg");
  }

  void CFBuffer::setParams(Int& /*nx*/, Int& /*ny*/, CoordinateSystem& /*cs*/, Float& /*sampling*/, 
			   Int& /*xSupport*/, Int& /*ySupport*/, const Double& /*freqValue*/, 
			   const Double& /*wValue*/, const Int& /*muellerElement*/,
			   const String& /*fileName*/)
  {
    /*
    RigidVector<Int,3> ndx=setParams(cs, sampling, xSupport, ySupport,
				     freqValue, wValue, muellerElement);
    // Adding degenerate axis here to make life easier when using these
    // buffers to write them as Images later (with a 4D co-ordinate
    // system - the degenerate axis become the Freq. and Polarization
    // axis).

    //    cfCells_p(ndx(0),ndx(1),ndx(2))->storage_p->resize(IPosition(4,nx,ny,1,1));
    cfCells_p(ndx(0),ndx(1),ndx(2))->shape_p=IPosition(4,nx,ny,1,1);
    */
  }
  //
  //---------------------------------------------------------------
  //
  //  template<class T>  Array<T>& CFBuffer<T>::
  CFCell& CFBuffer::getCFCell(const Int& i, const Int& j, const Int& k)
  {
    return *cfCells_p(i,j,k); // Nfreq x Nw x Nmueller
  }
  //
  //---------------------------------------------------------------
  //
  //  template <class T>  Array<T>& CFBuffer<T>
  CFCell& CFBuffer::getCFCell(const Double& freqVal, 
			     const Double& wValue, 
			     const Int& muellerElement) // muellerElement: (i,j) of the Mueller Matrix
  {
    RigidVector<Int,3> ndx=getIndex(freqVal, wValue, muellerElement);
    return getCFCell(ndx(0), ndx(1),ndx(2));
  }
  //
  //---------------------------------------------------------------
  //
  //  template<class T>  Array<T>& CFBuffer<T>::
  CountedPtr<CFCell>& CFBuffer::getCFCellPtr(const Int& i/*FreqNdx*/, const Int& j/*WNdx*/, const Int& k/*MuellerNdx*/)
  {
    // IPosition shp=cfCells_p.shape();
    // if (cfHitsStats.shape().product()==0)
    //   {
    // 	cfHitsStats.resize(shp);
    // 	cfHitsStats = 0;
    //   }
    // try
    //   {
    // 	AlwaysAssert(((i<shp[0]) && (j<shp[1]) && (k<shp[2])) , AipsError);
    //   }
    // catch (AipsError x)
    //   {
    // 	cerr << "#### " << i << " " << j << " " << k << " " << shp << endl;
    // 	throw(x);
    //   }

    // cfHitsStats(i,j,k)++;
    // //    cerr << "CFBuffer: Cell: " << i << " " << j << " " << k << " " << cfCells_p(i,j,k)->xSupport_p << endl;
    return cfCells_p(i,j,k); // Nfreq x Nw x Nmueller
  }
  //
  //---------------------------------------------------------------
  //
  //  template <class T>  Array<T>& CFBuffer<T>
  CountedPtr<CFCell>& CFBuffer::getCFCellPtr(const Double& freqVal, 
					     const Double& wValue, 
					     const Int & muellerElement)
  {
    RigidVector<Int,3> ndx=getIndex(freqVal, wValue, muellerElement);
    return cfCells_p(ndx(0), ndx(1),ndx(2));
  }
  //
  //---------------------------------------------------------------
  //
  //  template<class T>  Vector<Int>& CFBuffer<T>
  // This should be made more efficient with binary search.
  RigidVector<Int,3> CFBuffer::getIndex(const Double& freqValue, 
					const Double& wValue, 
					const Int& muellerElement)
  {
    RigidVector<Int,3> ndx(-1);
    Int nF=cfCells_p.shape()(0), nW=cfCells_p.shape()(1), nM=cfCells_p.shape()(2);
    Int i,j,k;
    //UNUSED: Int di;

    // Double dfMin=0;di=0;
    // for (i=0;i<nF;i++)
    //   {
    // 	Double df=fabs(cfCells_p(i,0,0)->freqValue_p - freqValue);
    // 	if (df < dfMin) {dfMin=df; di=i;}
    //   }
    // i=di;
    for (i=0; i<nF; i++)
      //      if ((fabs(cfCells_p(i,0,0)->freqValue_p - freqValue) < cfCells_p(i,0,0)->freqIncr_p))
      if (cfCells_p(i,0,0)->freqValue_p == freqValue) break;
    for(j=0; j<nW; j++)   if (cfCells_p(i,j,0)->wValue_p == wValue) break;
    for (k=0; k<nM; k++)  if (cfCells_p(i,j,k)->muellerElement_p == muellerElement) break;

    ndx(0)=i;ndx(1)=j;ndx(2)=k;
    // for (Int i=0;i<nF;i++)
    //   for(Int j=0;j<nW;j++)
    // 	for(Int k=0;k<nM;k++)
    // 	  {
    // 	    if (
    // 		(fabs(cfCells_p(i,j,k)->freqValue_p - freqValue) < cfCells_p(i,j,k)->freqIncr_p) &&
    // 		(cfCells_p(i,j,k)->wValue_p         == wValue) &&
    // 		(cfCells_p(i,j,k)->muellerElement_p == muellerElement)
    // 		)
    // 	      {ndx(0)=i;ndx(1)=j;ndx(2)=k;break;}
    // 	  }

    // cerr << "@#$%#@ " << freqValue 
    // 	 << " " << cfCells_p(ndx(0),ndx(1),ndx(2))->freqValue_p
    // 	 << " " << cfCells_p(ndx(0),ndx(1),ndx(2))->freqIncr_p 
    // 	 << " " << ndx
    // 	 << endl;
    // for(uInt i=0;i<nF;i++)
    //   if (freqValue==cfCells_pfreqValues_p(i)) {ndx(0)=i;break;}

    // for(uInt i=0;i<wValues_p.nelements();i++)
    //   if (wValue==wValues_p(i)) {ndx(1)=i;break;}

    // ndx(2)=muellerElements_p(muellerElement(0),muellerElement(1));

    return ndx;
  }
  //
  //---------------------------------------------------------------
  //
  void CFBuffer::getParams(CoordinateSystem& cs, Float& sampling, 
			   Int& xSupport, Int& ySupport, 
			   const Double& freqVal, const Double& wValue, 
			   const Int& muellerElement)
  {
    RigidVector<Int,3> ndx=getIndex(freqVal, wValue, muellerElement);
    getParams(cs, sampling, xSupport, ySupport, ndx(0), ndx(1), ndx(2));
  }
  //
  //---------------------------------------------------------------
  //
  Double CFBuffer::nearest(Bool& found, const Double& val,
			   const Vector<Double>& valList,
			   const Double& /*incr*/)
  {
    Int n=valList.nelements();
    if (n==1) {found=true;return valList[0];}
    Int where = binarySearch(found, valList, val, n);
    if (found) return valList(where);
    else return -1.0;
  }
  //
  //---------------------------------------------------------------
  //
  Int CFBuffer::nearestNdx(const Double& val,
			   const Vector<Double>& /*valList*/,
			   const Double& incr)
  {
    return SynthesisUtils::nint(incr*val);

    // Int n=valList.nelements();
    // if (n==1) return 0;
    // else return -1;

    // Int where = binarySearch(found, valList, val, n);
    // if (found) return valList(where);
    // else return -1.0;
  }
  //
  //---------------------------------------------------------------
  //
  //  template <class T>  void CFBuffer<T>
  void CFBuffer::show(const char *Mesg,ostream &os)
  {
    LogIO log_l(LogOrigin("CFBuffer","show[R&D]"));

    if (Mesg != NULL) os << Mesg << endl;
    os<< "---------------------------------------------------------" << endl
      << "Shapes: " << cfCells_p.shape() << endl;
    for (Int i=0;i<cfCells_p.shape()(0);i++)
      for (Int j=0;j<cfCells_p.shape()(1);j++)
	for (Int k=0;k<cfCells_p.shape()(2);k++)
	  {
	    os << "CFCell["<<i<<","<<j<<","<<k<<"]:" << endl;
	    cfCells_p(i,j,k)->show(Mesg, os);
	    os << "Pointing offset: " << pointingOffset_p << endl;
	  }
    os << "---------------------------------------------------------" << endl;
  }

  void CFBuffer::makePersistent(const char *dir, const char *cfName)
  {
    for (Int i=0;i<cfCells_p.shape()(0);i++)
      for (Int j=0;j<cfCells_p.shape()(1);j++)
	for (Int k=0;k<cfCells_p.shape()(2);k++)
	  {
	    ostringstream name;
	    name << String(cfName) << "_CF_" << i << "_" << j << "_" << k << ".im";
	    //cerr << "CFB Name : " << name.str() << endl;
	    // const char *formedName;
	    // if (cfName != "" ) formedName = name.str().c_str();
	    // else               formedName = cfName;
	    // cerr << "Formed name = " << formedName << endl;
	    cfCells_p(i,j,k)->makePersistent(dir, name.str().c_str());
	  }
  }

  Int CFBuffer::nearestFreqNdx(const Double& freqVal) 
  {
    Int index;
    SynthesisUtils::nearestValue(freqValues_p, freqVal, index);
    return index;
    // // The algorithm below has a N*log(N) cost.
    // Vector<Double> diff = fabs(freqValues_p - freqVal);
    // Bool dummy;
    // Sort sorter(diff.getStorage(dummy), sizeof(Double));
    // sorter.sortKey((uInt)0,TpDouble);
    // Int nch=freqValues_p.nelements();
    // Vector<uInt> sortIndx;
    // sorter.sort(sortIndx, nch);
    
    // return sortIndx(0);

    // Int ndx=min(freqValues_p.nelements()-1,max(0,SynthesisUtils::nint((freqVal-freqValues_p[0])/freqValIncr_p)));
    // return ndx;
  }


  void CFBuffer::primeTheCache()
  {
    //
    // In each CFCell of this CFBuffer, cache things that might be
    // required in tight loops and can be expensive to extract
    // otherwise.
    //
    IPosition cfCShape=cfCells_p.shape();
    for (Int i=0; i < cfCShape(0); i++)
      for (Int j=0; j < cfCShape(1); j++)
	for (Int k=0; k < cfCShape(2); k++)
	  getCFCellPtr(i,j,k)->initCache();
  }

  void CFBuffer::initMaps(const VisBuffer&, // vb, 
			  const Matrix<Double>& freqSelection, const Double& imRefFreq)
  {
    Vector<Double> spwList=freqSelection.column(0);
    Int maxSpw=(Int)(max(spwList));
    freqNdxMap_p.resize(maxSpw+1);
    conjFreqNdxMap_p.resize(maxSpw+1);

    for (Int i=0;i<(Int)spwList.nelements(); i++)
      {
	Int spw=(Int)freqSelection(i,0);
	Double fmin=freqSelection(i,1), fmax=freqSelection(i,2), finc=freqSelection(i,3);
	Int nchan = (Int)((fmax-fmin)/finc + 1);
	freqNdxMap_p[spw].resize(nchan);
	conjFreqNdxMap_p[spw].resize(nchan);
	for (Int c=0;c<nchan;c++)
	  {
	    Double freq=fmin+c*finc;
	    Double conjFreq=sqrt(2*imRefFreq*imRefFreq - freq*freq);
	    freqNdxMap_p[spw][c]=nearestFreqNdx(freq);
	    conjFreqNdxMap_p[spw][c]=nearestFreqNdx(conjFreq);
	  }
      }

    
    // cerr << "CFBuffer::initMaps: " 
    // 	 << freqSelection << endl
    // 	 << freqValues_p << endl
    // 	 << freqNdxMap_p << endl
    // 	 << conjFreqNdxMap_p << endl;

  }

  void CFBuffer::initPolMaps(PolMapType& polMap, PolMapType& conjPolMap) 
  {
    muellerElementsIndex_p.assign(polMap);
    conjMuellerElementsIndex_p.assign(conjPolMap);
  }

  void CFBuffer::getFreqNdxMaps(Vector<Vector<Int> >& freqNdx, Vector<Vector<Int> >& conjFreqNdx)
  {
    Int nspw;
    nspw=freqNdxMap_p.nelements();
    freqNdx.resize(nspw);
    for (Int s=0;s<nspw;s++)
      freqNdx[s].assign(freqNdxMap_p[s]);

    nspw=conjFreqNdxMap_p.nelements();
    conjFreqNdx.resize(nspw);
    for (Int s=0;s<nspw;s++)
      conjFreqNdx[s].assign(conjFreqNdxMap_p[s]);
  }

  void CFBuffer::ASSIGNVVofI(Int** &target,Vector<Vector<Int> >& source, Bool& doAlloc)
  {
    // cerr << "Source: " << target << " " << source << endl;

    Int nx,ny;					
    Bool b=(doAlloc||(target==NULL));
    nx=source.nelements();
    if (b) target=(int **)malloc(nx*sizeof(int*));
    for (int i=0;i<nx;i++)
      {
	ny = source[i].nelements();
	if (b) (target[i]) = (int *)malloc(ny*sizeof(int));
	for (int j=0;j<ny;j++)
	  (target)[i][j]=source[i][j];
      }

    // cerr << "Target: ";
    // for (int ii=0;ii<source.nelements();ii++)
    //   {
    // 	Int ny=source[ii].nelements();
    // 	for(Int jj=0;jj<ny;jj++)
    // 	  cerr << target[ii][jj] << " ";
    // 	cerr << endl;
    //   }
  }

  void CFBuffer::getAsStruct(CFBStruct& st)
  {
    Vector<Int> shp=cfCells_p.shape().asVector();

    Bool doAlloc=(st.CFBStorage == NULL);

    st.shape[0]=shp[0];
    st.shape[1]=shp[1];
    st.shape[2]=shp[2];
    st.fIncr = freqValIncr_p; st.wIncr = wValIncr_p;


    Bool dummy;
    Int n=cfCells_p.nelements();
    if (doAlloc) st.CFBStorage = (CFCStruct *)malloc(n*sizeof(CFCStruct));
    CountedPtr<CFCell> *cfstore=cfCells_p.getStorage(dummy);
    for (int i=0;i<n;i++)
      {
	//	  st.CFBStorage[i]=(CFCStruct *)malloc(sizeof(CFCStruct));
	(cfstore[i]).operator->()->getAsStruct(st.CFBStorage[i]);
      }
    //	st.CFBStorage[i] = (cfstore[i]).operator->()->getStorage()->getStorage(dummy);
    
    if (doAlloc) st.pointingOffset=(Double *)malloc(pointingOffset_p.nelements()*sizeof(Double));
    for (uInt i=0;i<pointingOffset_p.nelements();i++) st.pointingOffset[i]=pointingOffset_p[i];

    if (doAlloc) st.freqValues=(Double *)malloc(freqValues_p.nelements()*sizeof(Double));
    for (uInt i=0;i<freqValues_p.nelements();i++) st.freqValues[i]=freqValues_p[i];

    if (doAlloc) st.wValues=(Double *)malloc(wValues_p.nelements()*sizeof(Double));
    for (uInt i=0;i<wValues_p.nelements();i++) st.wValues[i]=wValues_p[i];

    ASSIGNVVofI(st.muellerElements, muellerElements_p, doAlloc);
    ASSIGNVVofI(st.muellerElementsIndex, muellerElementsIndex_p,doAlloc);
    ASSIGNVVofI(st.conjMuellerElements, conjMuellerElements_p,doAlloc);
    ASSIGNVVofI(st.conjMuellerElementsIndex, conjMuellerElementsIndex_p,doAlloc);

    st.nMueller = muellerElements_p.nelements();
    
    Vector<Vector<Int> > freqNdx, conjFreqNdx;
    getFreqNdxMaps(freqNdx, conjFreqNdx);
    ASSIGNVVofI(st.freqNdxMap, freqNdx, doAlloc);
    ASSIGNVVofI(st.conjFreqNdxMap, conjFreqNdx, doAlloc);
  }
  
  //
  //----------------------------------------------------------------------
  //
  void CFBuffer::fill(const Int& /*nx*/, const Int& /*ny*/, 
		      const Vector<Double>& freqValues,
		      const Vector<Double>& wValues,
		      const PolMapType& muellerElements)
  {

    LogIO log_l(LogOrigin("CFBuffer", "fillBuffer[R&D]"));
    for (uInt imx=0;imx<muellerElements.nelements();imx++) // Loop over all MuellerElements
      for (uInt imy=0;imy<muellerElements(imx).nelements();imy++)
    	{
	    for (uInt inu=0;inu<freqValues.nelements();inu++) // All freq. channels
	      {
    		for (uInt iw=0;iw<wValues.nelements();iw++)     // All w-planes
    		  {
		    log_l << " CF("
			  << "M:"<<muellerElements(imx)(imy) 
			  << ",C:" << inu 
			  << ",W:" << iw << "): ";
		    Vector<Double> ftRef(2);

		    // ftRef(0)=cfWtBuf.shape()(0)/2.0;
		    // ftRef(1)=cfWtBuf.shape()(1)/2.0;
		    CoordinateSystem ftCoords;//=cs_l;
		    // SynthesisUtils::makeFTCoordSys(cs_l, cfWtBuf.shape()(0), ftRef, ftCoords);

		    // cfb.setParams(inu,iw,imx,imy,//muellerElements(imx)(imy),
		    // 		  ftCoords, sampling, xSupport, ySupport,
		    // 		  freqValues(inu), wValues(iw), muellerElements(imx)(imy));
		    // cfb.getCFCellPtr(freqValues(inu), wValues(iw), 
		    // 		     muellerElements(imx)(imy))->pa_p=Quantity(vbPA,"rad");
		    //
		    // Now tha the CFs have been computed, cache its
		    // paramters in CFCell for quick access in tight
		    // loops (in the re-sampler, e.g.).
		    //
		    (getCFCellPtr(freqValues(inu), wValues(iw), 
				  muellerElements(imx)(imy)))->initCache();
    		  }
	      }
    	}
  }

} // end casa namespace



