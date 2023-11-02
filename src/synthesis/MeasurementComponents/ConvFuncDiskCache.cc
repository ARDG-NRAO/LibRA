// -*- C++ -*-
//# ConvFuncDiskCache.cc: Implementation of the ConvFuncDiskCache class
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
#include <synthesis/MeasurementComponents/ConvFuncDiskCache.h>
#include <synthesis/TransformMachines/Utils.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines/Utils.h>
#include <casacore/casa/OS/Directory.h>
#include <fstream>

using namespace casacore;
namespace casa{
  //
  //-------------------------------------------------------------------------
  // Just load the axillary info. if found.  The actual functions are loaded
  // as and when required using loadConvFunction() method.
  //
  void ConvFuncDiskCache::initCache()
  {
    ostringstream name;
    String line;
    Directory dirObj(Dir);

    if (Dir.length() == 0) 
      throw(SynthesisFTMachineError("Got null string for disk cache dir. "
				    "in ConvFuncDiskCache::initCache()"));
    //
    // If the directory does not exist, create it
    //
    if (!dirObj.exists()) dirObj.create();
    else if ((!dirObj.isWritable()) || (!dirObj.isReadable()))
      {
	throw(SynthesisFTMachineError(String("Directory \"")+Dir+String("\"")+
				      String(" for convolution function cache"
					     " exists but is unreadable/unwriteable")));
      }

    try
      {
	name << Dir << "/" << aux;
	File file(name);
	Int Npa=0,Nw=0;
	ifstream aux;
	Bool readFromFile=false;
	if (file.exists() && file.isRegular()) 
	  {
	    readFromFile=true;
	    aux.open(name.str().c_str());
	    if (readFromFile && aux.good()) aux >> Npa >> Nw;
	    else
	      throw(SynthesisFTMachineError(string("Error while reading convolution function cache file ")+name.str( )));
	  }

	if (Npa > 0)
	  {
	    paList.resize(Npa,true);

	    IPosition s(3,Nw,1,Npa);
	    XSup.resize(s,true);
	    YSup.resize(s,true);
	    Sampling.resize(Npa,true);
	    for(Int i=0;i<Npa;i++)
	      {
		Float pa, S;
		Int XS, YS;
		s[2]=i;
		aux >> pa;
		for(Int iw=0;iw<Nw;iw++)
		  {
		    s[0]=iw;
		    aux >> XS >> YS;
		    YS = XS;
		    paList[i] = pa*M_PI/180.0;
		    XSup(iw,0,i)=XS;
		    YSup(iw,0,i)=YS;
		  }
		aux >> S;
		Sampling[i]=S;
	      }
	  }
      }
    catch(AipsError& x)
      {
	throw(SynthesisFTMachineError(String("Error while initializing CF disk cache: ")
				      +x.getMesg()));
      }
  }
  ConvFuncDiskCache& ConvFuncDiskCache::operator=(const ConvFuncDiskCache& other)
  {
    paList = other.paList;
    Sampling = other.Sampling;
    XSup = other.XSup;
    YSup = other.YSup;
    Dir = other.Dir;
    cfPrefix = other.cfPrefix;
    aux = other.aux;
    return *this;
  };
  //
  //-------------------------------------------------------------------------
  // Write the conv. functions from the mem. cache to the disk cache.
  //
  void ConvFuncDiskCache::cacheConvFunction(Int which, Float pa, 
					    Array<Complex>& cf, 
					    CoordinateSystem& coords,
					    CoordinateSystem& ftCoords,
					    Int &convSize,
					    Cube<Int> &convSupport, 
					    Float convSampling,
					    String nameQualifier,
					    Bool savePA)
  {
    Int N=paList.nelements();
    if (Dir.length() == 0) return;

    try
      {
	IPosition newConvShape = cf.shape();
	Int wConvSize = newConvShape(2), directionIndex;
	for(Int iw=0;iw<wConvSize;iw++)
	  {
	    IPosition sliceStart(4,0,0,iw,0), 
	      sliceLength(4,newConvShape(0),newConvShape(1),1,newConvShape(3));
	
	    //	    CoordinateSystem ftCoords(coords);
	    ftCoords = coords;
	    directionIndex=ftCoords.findCoordinate(Coordinate::DIRECTION);
	    DirectionCoordinate dc=coords.directionCoordinate(directionIndex);
	    //	AlwaysAssert(directionIndex>=0, AipsError);
	    dc=coords.directionCoordinate(directionIndex);
	    Vector<Bool> axes(2); axes(0)=axes(1)=true;//axes(2)=true;
	    Vector<Int> shape(2); shape(0)=newConvShape(0);shape(1)=newConvShape(1);
	    shape=convSize;
	    Vector<Double>ref(4);
	    ref(0)=ref(1)=ref(2)=ref(3)=0;
	    dc.setReferencePixel(ref);
	    Coordinate* ftdc=dc.makeFourierCoordinate(axes,shape);
	    Vector<Double> refVal;
	    refVal=ftdc->referenceValue();
	    refVal(0)=refVal(1)=0;
	    ftdc->setReferenceValue(refVal);
	    ref(0)=newConvShape(0)/2-1;
	    ref(1)=newConvShape(1)/2-1;
	    ftdc->setReferencePixel(ref);
	
// 	    cout << ref << endl << refVal << endl << shape << endl;
// 	    cout << dc.increment() << " " << ftdc->increment() << endl;
	    ftCoords.replaceCoordinate(*ftdc, directionIndex);
	    {
	      ostringstream name;
	      name << Dir << "/" << cfPrefix << nameQualifier << iw << "_" << which;
	      
	      IPosition screenShape(4,newConvShape(0),newConvShape(1),newConvShape(3),1);
	      
	      PagedImage<Complex> thisScreen(screenShape, ftCoords, name);
	      
	      Array<Complex> buf;
	      buf=((cf(Slicer(sliceStart,sliceLength)).nonDegenerate()));
	      thisScreen.put(buf);
	    }
	    delete ftdc; ftdc=0;
	  }
	if (savePA)
	  {
	    IPosition s(3,wConvSize,1,N+1);
	    paList.resize(N+1,true);
	    // 	XSup.resize(N+1,true); 
	    // 	YSup.resize(N+1,true); 
	    XSup.resize(s,true);
	    YSup.resize(s,true);
	    Sampling.resize(N+1,true);
	    paList[N] = pa;
	    for(Int iw=0;iw<wConvSize;iw++)
	      {
		YSup(iw,0,N) = convSupport(iw,0,which);
		XSup(iw,0,N) = convSupport(iw,0,which);
	      }
	    Sampling[N]=convSampling;
	  }
      }
    catch (AipsError& x)
      {
	throw(SynthesisFTMachineError("Error while caching CF to disk in "
				      "ConvFuncDiskCache::cacheConvFunction(): "
				      +x.getMesg()));
      }
  }
  //
  //-------------------------------------------------------------------------
  // Write the weight functions from the mem. cache to the disk cache.
  //
  void ConvFuncDiskCache::cacheWeightsFunction(Int which, Float pa, 
					       Array<Complex>& cfWt, 
					       CoordinateSystem& coords,
					       Int &convSize,
					       Cube<Int> &convSupport, 
					       Float convSampling)
  {
    Int N=paList.nelements();
    if (Dir.length() == 0) return;

    try
      {
	IPosition newConvShape = cfWt.shape();
	Int wConvSize = newConvShape(2), directionIndex;
	for(Int iw=0;iw<wConvSize;iw++)
	  {
	    IPosition sliceStart(4,0,0,iw,0), 
	      sliceLength(4,newConvShape(0),newConvShape(1),1,newConvShape(3));
	
	    CoordinateSystem ftCoords(coords);
	    directionIndex=ftCoords.findCoordinate(Coordinate::DIRECTION);
	    DirectionCoordinate dc=coords.directionCoordinate(directionIndex);
	    //	AlwaysAssert(directionIndex>=0, AipsError);
	    dc=coords.directionCoordinate(directionIndex);
	    Vector<Bool> axes(2); axes(0)=axes(1)=true;//axes(2)=true;
	    Vector<Int> shape(2); shape(0)=newConvShape(0);shape(1)=newConvShape(1);
	    shape=convSize;
	    Vector<Double>ref(4);
	    ref(0)=ref(1)=ref(2)=ref(3)=0;
	    dc.setReferencePixel(ref);
	    Coordinate* ftdc=dc.makeFourierCoordinate(axes,shape);
	    Vector<Double> refVal;
	    refVal=ftdc->referenceValue();
	    refVal(0)=refVal(1)=0;
	    ftdc->setReferenceValue(refVal);
	    ref(0)=newConvShape(0)/2-1;
	    ref(1)=newConvShape(1)/2-1;
	    ftdc->setReferencePixel(ref);
	
// 	    cout << ref << endl << refVal << endl << shape << endl;
// 	    cout << dc.increment() << " " << ftdc->increment() << endl;
	    ftCoords.replaceCoordinate(*ftdc, directionIndex);
	    delete ftdc; ftdc=0;
	
	    {
	      ostringstream name;
	      name << Dir << "/" << cfPrefix << "WT" << iw << "_" << which;
	      
	      IPosition screenShape(4,newConvShape(0),newConvShape(1),newConvShape(3),1);
	      
	      PagedImage<Complex> thisScreen(screenShape, ftCoords, name);
	      
	      Array<Complex> buf;
	      buf=((cfWt(Slicer(sliceStart,sliceLength)).nonDegenerate()));
	      thisScreen.put(buf);
	    }
	  }
	IPosition s(3,wConvSize,1,N+1);
	paList.resize(N+1,true);
// 	XSup.resize(N+1,true); 
// 	YSup.resize(N+1,true); 
	XSup.resize(s,true);
	YSup.resize(s,true);
	Sampling.resize(N+1,true);
	paList[N] = pa;
	for(Int iw=0;iw<wConvSize;iw++)
	  {
	    YSup(iw,0,N) = convSupport(iw,0,which);
	    XSup(iw,0,N) = convSupport(iw,0,which);
	  }
	Sampling[N]=convSampling;
      }
    catch (AipsError& x)
      {
	throw(SynthesisFTMachineError("Error while caching CFWT to disk in "
				      "ConvFuncDiskCache::cacheConvFunction(): "
				      +x.getMesg()));
      }
  }
  //
  //-------------------------------------------------------------------------
  //  
  Bool ConvFuncDiskCache::searchConvFunction(const VisBuffer& vb, 
					    VPSkyJones& vpSJ, 
					    Int& which,
					    Float &pa)
  {
    Int i,NPA=paList.nelements(); Bool paFound=false;
    Float iPA, dPA;
    dPA = vpSJ.getPAIncrement().getValue("rad");
    /*
    Vector<Float> antPA = vb.feed_pa(getCurrentTimeStamp(vb));
    pa = sum(antPA)/(antPA.nelements()-1);
    */
    pa = getPA(vb);
    //    cout << dPA*57.295 << " " << pa*57.295 << endl;
    //    pa = 0;
    //    cout << "######CFDC::search: " << pa << " " << getPA(vb) << endl;
    //    if (NPA == 0) return -1;
    
    for(i=0;i<NPA;i++)
      {
	iPA = paList[i];
	if (fabs(iPA - pa) <= dPA)
	  {
	    paFound = true;
	    break;
	  }
      }
    if (paFound) which = i; else which = -i;
    return paFound;
  }
  //
  //-------------------------------------------------------------------------
  //  
  Bool ConvFuncDiskCache::searchConvFunction(const VisBuffer& vb, 
					     ParAngleChangeDetector& vpSJ, 
					     Int& which,
					     Float &pa)
  {
    if (paList.nelements()==0) initCache();
    Int i,NPA=paList.nelements(); Bool paFound=false;
    Float iPA, dPA;
    dPA = vpSJ.getParAngleTolerance().getValue("rad");
    /*
    Vector<Float> antPA = vb.feed_pa(getCurrentTimeStamp(vb));
    pa = sum(antPA)/(antPA.nelements()-1);
    */
    pa = getPA(vb);
    //    pa = 0;
    //    cout << "######CFDC::search: " << pa << " " << getPA(vb) << endl;
    //    if (NPA == 0) return -1;
    
    Float paDiff=2*dPA;
    Int saveNdx=-1;

    saveNdx = -1;
    for(i=0;i<NPA;i++)
      {
	iPA = paList[i];
	if (fabs(iPA - pa) < paDiff)
	  {
	    saveNdx = i;
	    paDiff = fabs(iPA-pa);
	  }
      }
    if (saveNdx > -1)
      {
	iPA = paList[saveNdx];
	if (fabs(iPA - pa) <= dPA)
	  {
	    i = saveNdx;
	    paFound=true;
	  }
      }
    /*      
    for(i=0;i<NPA;i++)
      {
	iPA = paList[i];
	if (fabs(iPA - pa) <= dPA)
	  {
	    paFound = true;
	    break;
	  }
      }
    */
    if (paFound) 
      {
// 	cout << "SEARCH: " << i << " " << paList[i]*180/M_PI << " " << pa*180/M_PI
// 	     << " " << (paList[i]-pa)*180/M_PI << endl;
	which = i; 
      }
    else which = -i;
    return paFound;
  }
  //
  //-------------------------------------------------------------------------
  //Write the aux. info. also in the disk cache (wonder if this should
  //be automatically be called from cacheConvFunction() method).
  //
  void ConvFuncDiskCache::finalize()
  {
    if (Dir.length() == 0) return;
    ostringstream name;
    name << Dir << "/aux.dat";
    try
      {
	//    cout << "Writing to " << name.str() << endl;
	IPosition supportShape=XSup.shape();
	ofstream aux(name.str().c_str());
	aux << paList.nelements() << " " << supportShape[0] << endl;
	for(uInt ipa=0;ipa<paList.nelements();ipa++)
	  {
	    aux << paList[ipa]*180.0/M_PI << " ";
	    for(int iw=0;iw<supportShape[0];iw++)
	      aux << XSup(iw,0,ipa) << " " << YSup(iw,0,ipa) << " ";
	    aux << " " << Sampling[ipa] <<endl;
	  }
      }
    catch(AipsError &x)
      {
	throw(SynthesisFTMachineError(string("Error while writing ")
				      + name.str( ) + (string) x.getMesg()));
      }
  }
  //
  //-------------------------------------------------------------------------
  //Along with the aux. info., also save the average PB in the disk cache.
  //
  void ConvFuncDiskCache::finalize(ImageInterface<Float>& avgPB)
  {
    if (Dir.length() == 0) return;
    finalize();
    ostringstream Name;
    Name << Dir <<"/avgPB";
    try
      {
	IPosition newShape(avgPB.shape());
	PagedImage<Float> tmp(newShape, avgPB.coordinates(), Name);
	
	LatticeExpr<Float> le(avgPB);
	tmp.copyData(le);
      }
    catch(AipsError &x)
      {
	throw(SynthesisFTMachineError(string("Error while writing ")
				      + Name.str( ) + (string) x.getMesg()));
      }
  }
  //
  //-------------------------------------------------------------------------
  //Load the average PB from the disk cache.
  //
  void ConvFuncDiskCache::loadAvgPB(ImageInterface<Float>& avgPB)
  {
    if (Dir.length() == 0) return;
    ostringstream name;
    name << Dir << "/avgPB";
    //    cout << name.str() << endl;
    try
      {
	PagedImage<Float> tmp(name.str().c_str());
	avgPB.resize(tmp.shape());
	avgPB.put(tmp.get());
      }
    catch(AipsError& x) // Just rethrowing the exception for now.
                        // Ultimately, this should be used to make
			// the state of this object consistant.
      {
	throw(SynthesisFTMachineError(string("Error while loading \"")+
				      name.str( ) + string("\": ") + (string) x.getMesg()));
      }
  }
  //
  //-------------------------------------------------------------------------
  //Load a conv. func. from the disk.  This is non-optimal due to the
  //data structure used for the conv. func. in-memory cache (it's an
  //array of pointers where it should really be a List of pointers).
  //The conf. func. index, which is also used as a key to located them
  //in the mem. cache, are not assured to be contiguous.  As a result,
  //in the current implementation there can be gaps in the
  //convFuncCache array.  These gaps are initialized to NULL pointers.
  //It's not much of a memory waste, but still non-optimal!  Leaving
  //it like this for now.
  //
  Bool ConvFuncDiskCache::loadConvFunction(Int where, Int Nw, 
					   PtrBlock < Array<Complex> *> &convFuncCache,
					   Cube<Int> &convSupport,
					   Vector<Float>& convSampling,
					   Double& cfRefFreq, CoordinateSystem& coordSys,
					   String prefix)
  {
    if (Dir.length() == 0) return false;
    if (where < (Int)convFuncCache.nelements() && (convFuncCache[where] != NULL)) return false;

    Int wConvSize, polInUse=2;
    Int N=convFuncCache.nelements();

    //
    // Re-size the conv. func. memory cache if required, and set the
    // new members of the resized cache to NULL.  This is used in the
    // loop below to make a decision about allocating new memory or
    // not.
    //
    convFuncCache.resize(max(where+1,N), true);
    for(Int i=N;i<=where;i++) convFuncCache[i]=NULL;
    //
    // Each w-plan is in a separate disk file.  Each file contains all
    // polarization planes though. Memory cache holds all w-planes and
    // poln-planes in a single complex array.  The loop below read
    // each w-plane image from the disk, and fills in the 3D
    // mem. cache for each computed PA.
    //
    wConvSize = Nw;
    for(Int iw=0;iw<Nw;iw++)
      {
	ostringstream name;
	//	name << Dir << "/CF" << iw << "_" << where;
	name << Dir << prefix << iw << "_" << where;
	//	cout << "CF File name = " << name.str() << endl;
	try
	  {
	    PagedImage<Complex> tmp(name.str().c_str());
	    Int index= tmp.coordinates().findCoordinate(Coordinate::SPECTRAL);
	    coordSys = tmp.coordinates();
	    SpectralCoordinate spCS = coordSys.spectralCoordinate(index);

	    cfRefFreq=spCS.referenceValue()(0);
	
	    polInUse = tmp.shape()(2);
	    IPosition ts=tmp.shape(),ndx(4,0,0,0,0),ts2(4,0,0,0,0);
	    Array<Complex> buf=tmp.get();
	    if (convFuncCache[where] == NULL)
	      convFuncCache[where] = new Array<Complex>(IPosition(4,ts(0),ts(1),
								  wConvSize,polInUse));
	
	    ndx(2)=iw;                              // The w-axis
	    for(ndx(3)=0;ndx(3)<polInUse;ndx(3)++)  // The Poln. axis.
	      for(ndx(0)=0;ndx(0)<ts(0);ndx(0)++)   
		for(ndx(1)=0;ndx(1)<ts(1);ndx(1)++)
		  {
		    ts2(0)=ndx(0);ts2(1)=ndx(1);
		    ts2(2)=ndx(3); // The Poln. axis of the disk-cache
		    ts2(3)=0;      // The freq. axis of the disk-cache
		    (*convFuncCache[where])(ndx)=buf(ts2);
		  }
	  }
	catch(AipsError &x)
	  {
	    throw(SynthesisFTMachineError(string("Error while loading \"")+
					  name.str( ) + string("\": ") + (string) x.getMesg()));
	  }
	
      }
    convSupport.resize(wConvSize,polInUse,where+1,true);
    for(Int i=0;i<wConvSize;i++)
      for(Int j=0;j<polInUse;j++)
	convSupport(i,j,where) = XSup(i,0,where);
    //    cout << "##### " << convFuncCache.nelements() << endl;
    convSampling = Sampling;
    return true;
  }
}
