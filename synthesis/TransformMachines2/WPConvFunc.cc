//# WPConvFunc.cc: implementation of WPConvFunc
//# Copyright (C) 2007-2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#include <sstream>
#include <iostream>
#include <iomanip>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/OS/HostInfo.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/CompositeNumber.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>

#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>

#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/lattices/Lattices/SubLattice.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/Lattices/LatticeCache.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/scimath/Mathematics/ConvolveGridder.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <casacore/scimath/Mathematics/FFTPack.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <synthesis/TransformMachines/SimplePBConvFunc.h> //por SINCOS

#include <synthesis/TransformMachines2/WPConvFunc.h>
#ifdef _OPENMP
#include <omp.h>
#endif




using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
namespace refim{ //namespace for refactoring imager

using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::refim;
  typedef unsigned long long ooLong; 
 WPConvFunc::WPConvFunc(const Double minW, const Double maxW, const Double rmsW):
   actualConvIndex_p(-1), convSize_p(0), convSupport_p(0), minW_p(minW), maxW_p(maxW), rmsW_p(rmsW) {
   //
  }
WPConvFunc::WPConvFunc(const WPConvFunc& other):
   actualConvIndex_p(-1), convSize_p(0), convSupport_p(0){

    operator=(other);
  }

  WPConvFunc& WPConvFunc::operator=(const WPConvFunc& other){
    if(this != &other){
      uInt numConv=other.convFunctions_p.nelements();
      convFunctions_p.resize(numConv, true, false);
      convSupportBlock_p.resize(numConv, true, false);
      for (uInt k=0; k < numConv; ++k){
	convFunctions_p[k]=new Cube<Complex>();
	*(convFunctions_p[k])=*(other.convFunctions_p[k]);
	convSupportBlock_p[k]=new Vector<Int> ();
	*(convSupportBlock_p[k]) = *(other.convSupportBlock_p[k]);
      }
     
      convFunctionMap_p=other.convFunctionMap_p;
      convSizes_p.resize();
      convSizes_p=other.convSizes_p;
      actualConvIndex_p=other.actualConvIndex_p;
      convSize_p=other.convSize_p;
      convSupport_p.resize();
      convSupport_p=other.convSupport_p;
      convFunc_p.resize();
      convFunc_p=other.convFunc_p;
      wScaler_p=other.wScaler_p;
      convSampling_p=other.convSampling_p;
      nx_p=other.nx_p; 
      ny_p=other.ny_p;
      minW_p=other.minW_p;
      maxW_p=other.maxW_p;
      rmsW_p=other.rmsW_p;


      
    }
    return *this;
  }

  WPConvFunc::~WPConvFunc(){
    //usage of CountedPtr keeps this simple

  }

  WPConvFunc::WPConvFunc(const RecordInterface& rec):
   actualConvIndex_p(-1), convSize_p(0), convSupport_p(0){
    String error;
    if (!fromRecord(error, rec)) {
      throw (AipsError("Failed to create WPConvFunc: " + error));
    }

  }

void WPConvFunc::findConvFunction(const ImageInterface<Complex>& image, 
				  const vi::VisBuffer2& vb,
				    const Int& wConvSizeUser,
				    const Vector<Double>& uvScale,
				    const Vector<Double>& uvOffset, 
				    const Float& padding,
				    Int& convSampling,
				    Cube<Complex>& convFunc, 
				    Int& convSize,
				    Vector<Int>& convSupport, Double& wScale){
   if(checkCenterPix(image)){ 
     convFunc.resize();
     convFunc.reference(convFunc_p);
     convSize=convSize_p;
     convSampling=convSampling_p;
     convSupport.resize();
     convSupport=convSupport_p;
    wScale=Float((convFunc.shape()(2)-1)*(convFunc.shape()(2)-1))/wScaler_p;
    return;
   }
   LogIO os;
   os << LogOrigin("WPConvFunc", "findConvFunction")  << LogIO::NORMAL;
   
   
  // Get the coordinate system
   CoordinateSystem coords(image.coordinates());
   Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
   nx_p=Int(image.shape()(directionIndex)); 
   ny_p=Int(image.shape()(directionIndex+1));
   
   Int wConvSize=wConvSizeUser;
   ////Automatic mode
   Double maxUVW;
   if(wConvSize < 1){
     maxUVW=rmsW_p < 0.5*(minW_p+maxW_p) ? 1.05*maxW_p: (rmsW_p /(0.5*((minW_p)+maxW_p))*1.05*maxW_p) ;
     wConvSize=Int(maxUVW*fabs(sin(fabs(image.coordinates().increment()(0))*max(nx_p, ny_p)/2.0)));  
     convSupport.resize(wConvSize);
   }
   else{    
     if(maxW_p> 0.0)
       maxUVW= 1.05*maxW_p;
     else
       maxUVW=0.25/abs(image.coordinates().increment()(0));
     
   }
   if(wConvSize>1) {
     os << "W projection using " << wConvSize << " planes" << LogIO::POST;
     
     os << "Using maximum possible W = " << maxUVW
	<< " (wavelengths)" << LogIO::POST;
    
     Double invLambdaC=vb.getFrequency(0,0)/C::c;
     os << "Typical wavelength = " << 1.0/invLambdaC
	<< " (m)" << LogIO::POST;
     
    
     wScale=Float((wConvSize-1)*(wConvSize-1))/maxUVW;
     //wScale=Float(wConvSize-1)/maxUVW;
     wScaler_p=maxUVW;;
     os << "Scaling in W (at maximum W) = " << 1.0/wScale
	<< " wavelengths per pixel" << LogIO::POST;
   }

   Int planesPerChunk=100;


   if(wConvSize>1) {
     Int maxMemoryMB=min(uInt64(HostInfo::memoryTotal(true)), uInt64(HostInfo::memoryFree()))/1024;
     ////Common you have to have 4 GB...memoryFree sometimes is whacky
     if(maxMemoryMB < 4000)
       maxMemoryMB=4000;
     convSize=max(Int(nx_p*padding),Int(ny_p*padding));
     ///Do the same thing as in WProject::init
     CompositeNumber cn(convSize);    
     convSize    = cn.nextLargerEven(convSize);
    //nominal  100 wprojplanes above that you may (or not) go swapping
     
     planesPerChunk=Int((Double(maxMemoryMB)/8.0*1024.0*1024.0)/Double(convSize)/Double(convSize));
     //cerr << "planesPerChunk" << planesPerChunk << endl;
     planesPerChunk=min(planesPerChunk, 100);
     //    CompositeNumber cn(Int(maxConvSizeConsidered/2.0)*2);
    convSampling_p=4;
    
    //convSize=min(convSize,(Int)cn.nearestEven(Int(maxConvSizeConsidered/2.0)*2));

   }
   else{
     convSampling_p=1;
    convSize=max(Int(nx_p*padding),Int(ny_p*padding));
   }
   convSampling=convSampling_p;
   Int maxConvSize=convSize;
   convSupport.resize(wConvSize);
   convSupport.set(-1);
   ////set sampling
   AlwaysAssert(directionIndex>=0, AipsError);
   DirectionCoordinate dc=coords.directionCoordinate(directionIndex);
   Vector<Double> sampling;
   sampling = dc.increment();
   sampling*=Double(convSampling_p);
   //sampling*=Double(max(nx,ny))/Double(convSize);
   sampling[0]*=Double(padding*Float(nx_p))/Double(convSize);
   sampling[1]*=Double(padding*Float(ny_p))/Double(convSize);
   /////
   Int inner=convSize/convSampling_p;
   ConvolveGridder<Double, Complex>
     ggridder(IPosition(2, inner, inner), uvScale, uvOffset, "SF");
   
   Int nchunk=wConvSize/planesPerChunk;
   if((wConvSize%planesPerChunk) !=0)
     nchunk +=1;
   Vector<Int> chunksize(nchunk, planesPerChunk);
   if((wConvSize%planesPerChunk) !=0)
    chunksize(nchunk-1)=wConvSize%planesPerChunk;
   
   Int warner=0;
   Vector<Complex> maxes(wConvSize);
  // Bool maxdel;
  // Complex* maxptr=maxes.getStorage(maxdel);
  Matrix<Complex> corr(inner, inner);
  Vector<Complex> correction(inner);
   for (Int iy=-inner/2;iy<inner/2;iy++) {
     
     ggridder.correctX1D(correction, iy+inner/2);
     corr.row(iy+inner/2)=correction;
   }
   Bool cpcor;
  
   Complex *cor=corr.getStorage(cpcor);
   Vector<Int>pcsupp;
   pcsupp=convSupport;
   Bool delsupstor;
   Int* suppstor=pcsupp.getStorage(delsupstor);
   Double s1=sampling(1);
   Double s0=sampling(0);
   ///////////Por FFTPack
   Vector<Float> wsave(2*convSize*convSize+15);
   Int lsav=2*convSize*convSize+15;
   Bool wsavesave;
   Float *wsaveptr=wsave.getStorage(wsavesave);
   Int ier;
   FFTPack::cfft2i(convSize, convSize, wsaveptr, lsav, ier);
   ////////// 
   Matrix<Complex> screen(convSize, convSize);
   makeGWplane(screen, 0, s0, s1, wsaveptr, lsav, inner, cor, wScale);
   Float maxconv=abs(screen(0,0));
   for (Int chunkId=nchunk-1; chunkId >= 0;  --chunkId){
     //cerr << "chunkId " << chunkId << endl;
     Cube<Complex> convFuncSect( maxConvSize/2-1, maxConvSize/2-1, chunksize(chunkId));
     convFuncSect.set(0.0);
     Bool convFuncStor=false;
     Complex *convFuncPtr=convFuncSect.getStorage(convFuncStor);
      //////openmp like to share reference param ...but i don't like to share
     Int cpConvSize=maxConvSize;
     //cerr << "orig convsize " << convSize << endl;
     // Int cpWConvSize=wConvSize;
     Double cpWscale=wScale;
     Int wstart=planesPerChunk*chunkId;
     Int wend=wstart+chunksize(chunkId)-1;
     
#ifdef _OPENMP
     omp_set_nested(0);
     Int nthreads=omp_get_max_threads();
     nthreads=min(nthreads, chunksize(chunkId));
     omp_set_num_threads(nthreads);
     
#endif
#pragma omp parallel for default(none) firstprivate(/*cpWConvSize,*/ cpConvSize, convFuncPtr, s0, s1, wsaveptr, lsav, cor, inner, cpWscale,  wstart, wend) schedule(dynamic, 1)
     

     for (Int iw=wstart; iw < (wend+1)  ; ++iw) {
       Matrix<Complex> screen1(cpConvSize, cpConvSize);
       makeGWplane(screen1, iw, s0, s1, wsaveptr, lsav, inner, cor, cpWscale);
       ooLong offset=ooLong(ooLong(iw-wstart)*ooLong(cpConvSize/2-1)*ooLong(cpConvSize/2-1));
       for (ooLong y=0; y< ooLong(cpConvSize/2)-1; ++y){
	 for (ooLong x=0; x< ooLong(cpConvSize/2)-1; ++x){
	   ////////convFuncPtr[offset+y*(convSize/2-1)+x] = quarter(x,y);
	   convFuncPtr[offset+y*ooLong(cpConvSize/2-1)+x] = screen1(x,y);
	 }
       }

     }//iw

     convFuncSect.putStorage(convFuncPtr, convFuncStor);
     for (uInt iw=0; iw< uInt(chunksize(chunkId)); ++iw){
       convFuncSect.xyPlane(iw)=convFuncSect.xyPlane(iw)/(maxconv);
     }
     convFuncPtr=convFuncSect.getStorage(convFuncStor);
     Int thischunkSize=chunksize(chunkId);
     //cerr << "chunkId* planesPerChunk " << chunkId* planesPerChunk << "  chunksize " << thischunkSize << endl;
#pragma omp parallel for default(none) firstprivate(suppstor, cpConvSize, /*cpWConvSize,*/ convFuncPtr, maxConvSize, thischunkSize, wstart, planesPerChunk, chunkId) reduction(+: warner)      
     for (Int iw=0; iw<thischunkSize; iw++) {
       Bool found=false;
       Int trial=0;
       ooLong ploffset=(ooLong)(cpConvSize/2-1)*(ooLong)(cpConvSize/2-1)*(ooLong)iw;
       for (trial=cpConvSize/2-2;trial>0;trial--) {
	 // if((abs(convFunc(trial,0,iw))>1e-3)||(abs(convFunc(0,trial,iw))>1e-3) ) {
	 if((abs(convFuncPtr[(ooLong)(trial)+ploffset])>1e-3)||(abs(convFuncPtr[(ooLong)(trial*(cpConvSize/2-1))+ploffset])>1e-3) ) {
	   //cout <<"iw " << iw << " x " << abs(convFunc(trial,0,iw)) << " y " 
	   //   <<abs(convFunc(0,trial,iw)) << endl; 
	   found=true;
	   break;
	 }
       }
       Int trueIw=iw+chunkId* planesPerChunk;
       if(found) {
	 suppstor[trueIw]=Int(0.5+Float(trial)/Float(convSampling_p))+1;
	 if(suppstor[trueIw]*convSampling_p*2 >= maxConvSize){
	   suppstor[trueIw]=cpConvSize/2/convSampling_p-1;
	   ++warner;
	 }
       }
       else{
	 suppstor[trueIw]=cpConvSize/2/convSampling_p-1;
	 ++warner;
       }
     }
     convFuncSect.putStorage(convFuncPtr, convFuncStor);
     if(chunkId==(nchunk-1)){
	 Int newConvSize=2*(suppstor[wConvSize-1]+2)*convSampling;
	 if(newConvSize < convSize){
	   convFunc.resize((newConvSize/2-1),
		  (newConvSize/2-1),
			   wConvSize);
	   convSize=newConvSize;
	 }
	 else{
	   convFunc.resize((convSize/2-1),
		  (convSize/2-1),
			   wConvSize);
	 }	 	 
     }
    IPosition blc(3, 0,0,wstart);
    IPosition trc(3, (convSize/2-2),(convSize/2-2),wend);
    convFunc(blc, trc)=convFuncSect(IPosition(3, 0,0,0), IPosition(3, (convSize/2-2),
								  (convSize/2-2), thischunkSize-1));


   }//chunkId
   corr.putStorage(cor, cpcor);
   pcsupp.putStorage(suppstor, delsupstor);
   convSupport=pcsupp;
   Double pbSum=0.0;
   for (Int iy=-convSupport(0);iy<=convSupport(0);iy++) {
     for (Int ix=-convSupport(0);ix<=convSupport(0);ix++) {
       pbSum+=real(convFunc(abs(ix)*convSampling_p,abs(iy)*convSampling_p,0));
     }
   }
   if(pbSum>0.0) {
     convFunc*=Complex(1.0/pbSum,0.0);
   }
   else {
     os << "Convolution function integral is not positive"
	<< LogIO::EXCEPTION;
  } 


    os << "Convolution support = " << convSupport*convSampling_p
	  << " pixels in Fourier plane"
	  << LogIO::POST;
    convSupportBlock_p.resize(actualConvIndex_p+1);
    convSupportBlock_p[actualConvIndex_p]= new Vector<Int>();
    convSupportBlock_p[actualConvIndex_p]->assign(convSupport);
    convFunctions_p.resize(actualConvIndex_p+1);
    convFunctions_p[actualConvIndex_p]= new Cube<Complex>(convFunc);
    convSizes_p.resize(actualConvIndex_p+1, true);
    convSizes_p(actualConvIndex_p)=convSize;
    Int maxMemoryMB=HostInfo::memoryTotal(true)/1024;
  Int memoryMB;
  memoryMB = Int(Double(convSize/2-1)*Double(convSize/2-1)*
		 Double(wConvSize)*8.0/1024.0/1024.0);
  os << "Memory used in gridding function = "
     << memoryMB << " MB from maximum "
	  << maxMemoryMB << " MB" << LogIO::POST;
  convSampling=convSampling_p;
  wScale=Float((wConvSize-1)*(wConvSize-1))/wScaler_p;

}//end func

  void WPConvFunc::makeGWplane(Matrix<Complex>& screen, const Int iw, Double s0, Double s1, Float *& wsaveptr, Int& lsav, Int& inner, Complex*& cor, Double&cpWscale){

    Int cpConvSize=screen.shape()(0);
    //cerr << "cpConvSize " << cpConvSize << "   shape " << screen.shape() << endl;
    screen.set(0.0);
     Bool cpscr;
     Complex *scr=screen.getStorage(cpscr);
      Double twoPiW=2.0*C::pi*Double(iw*iw)/cpWscale;
	 for (Int iy=-inner/2;iy<inner/2;iy++) {
	   Double m=s1*Double(iy);
	   Double msq=m*m;
	   //////Int offset= (iy+convSize/2)*convSize;
	   ///fftpack likes it flipped
	   ooLong offset= (iy>-1 ? ooLong(iy) : ooLong(iy+cpConvSize))*ooLong(cpConvSize);
	   for (Int ix=-inner/2;ix<inner/2;ix++) {
	     //////	  Int ind=offset+ix+convSize/2;
	     ///fftpack likes it flipped
	     ooLong ind=offset+(ix > -1 ? ooLong(ix) : ooLong(ix+cpConvSize));
	     Double l=s0*Double(ix);
	     Double rsq=l*l+msq;
	     if(rsq<1.0) {
	       Double phase=twoPiW*(sqrt(1.0-rsq)-1.0);
	       Double cval, sval;
	       SINCOS(phase, sval, cval);
	       Complex comval(cval, sval);
	       scr[ind]=(cor[ix+inner/2+ (iy+inner/2)*inner])*comval;
	       //screen(ix+convSize/2, iy+convSize/2)=comval; 
	     }
	   }
	 }
	 ////Por FFTPack
	 Vector<Float>work(2*cpConvSize*cpConvSize);
	 Int lenwrk=2*cpConvSize*cpConvSize;
	 Bool worksave;
	 Float *workptr=work.getStorage(worksave);
	 Int ier;
	 FFTPack::cfft2f(cpConvSize, cpConvSize, cpConvSize, scr, wsaveptr, lsav, workptr, lenwrk, ier);
       
       screen.putStorage(scr, cpscr);

  } 


Bool WPConvFunc::checkCenterPix(const ImageInterface<Complex>& image){

  CoordinateSystem imageCoord=image.coordinates();
  MDirection wcenter;  
  Int directionIndex=imageCoord.findCoordinate(Coordinate::DIRECTION);
  DirectionCoordinate
    directionCoord=imageCoord.directionCoordinate(directionIndex);
  Vector<Double> incr=directionCoord.increment();
  nx_p=image.shape()(directionIndex);
  ny_p=image.shape()(directionIndex+1);


  //Images with same number of pixels and increments can have the same conv functions
  ostringstream oos;
  oos << setprecision(6);

  oos << nx_p << "_"<< fabs(incr(0)) << "_";
  oos << ny_p << "_"<< fabs(incr(1));
  String imageKey(oos);

  if(convFunctionMap_p.size( ) == 0){
    convFunctionMap_p.insert(std::pair<casacore::String, casacore::Int>(imageKey, 0));
    actualConvIndex_p=0;
    return false;
  }
   
  if( convFunctionMap_p.find(imageKey) == convFunctionMap_p.end( ) ){
    actualConvIndex_p=convFunctionMap_p.size( );
    convFunctionMap_p.insert(std::pair<casacore::String, casacore::Int>(imageKey,actualConvIndex_p));
    return false;
  }
  else{
    actualConvIndex_p=convFunctionMap_p[imageKey];
    convFunc_p.resize(); // break any reference
    convFunc_p.reference(*convFunctions_p[actualConvIndex_p]);
    convSupport_p.resize();
    convSupport_p.reference(*convSupportBlock_p[actualConvIndex_p]);
    convSize_p=convSizes_p[actualConvIndex_p];

  }

  return true;
}

Bool WPConvFunc::toRecord(RecordInterface& rec){

  Int numConv=convFunctions_p.nelements();
  try{
    rec.define("numconv", numConv);
    auto convptr = convFunctionMap_p.begin( );
    for (Int k=0; k < numConv; ++k, ++convptr){
      rec.define("convfunctions"+String::toString(k), *(convFunctions_p[k]));
      rec.define("convsupportblock"+String::toString(k), *(convSupportBlock_p[k]));
      rec.define("key"+String::toString(k), convptr->first);
      rec.define("val"+String::toString(k), convptr->second);
    }
    rec.define("convsizes", convSizes_p);
    rec.define("actualconvIndex",actualConvIndex_p);
    rec.define("convsize", convSize_p);
    rec.define("convsupport", convSupport_p);
    rec.define("convfunc",convFunc_p);
    rec.define("wscaler", wScaler_p);
    rec.define("convsampling", convSampling_p);
    rec.define("nx", nx_p);
    rec.define("ny", ny_p);
  }
  catch(AipsError x) {
    return false;
  }
  return true;

 

}

 Bool WPConvFunc::fromRecord(String& err, const RecordInterface& rec){
  
  Int numConv=0;
  try{
    rec.get("numconv", numConv);
    convFunctions_p.resize(numConv, true, false);
    convSupportBlock_p.resize(numConv, true, false);
    convFunctionMap_p.clear( );
    for (Int k=0; k < numConv; ++k){
      convFunctions_p[k]=new Cube<Complex>();
      convSupportBlock_p[k]=new Vector<Int>();
      rec.get("convfunctions"+String::toString(k), *(convFunctions_p[k]));
      rec.get("convsupportblock"+String::toString(k), *(convSupportBlock_p[k]));
      String key;
      Int val;
      rec.get("key"+String::toString(k), key);
      rec.get("val"+String::toString(k), val);
      convFunctionMap_p.insert(std::pair<casacore::String, casacore::Int>(key,val));
    }
    rec.get("convsizes", convSizes_p);
    rec.get("actualconvIndex",actualConvIndex_p);
    rec.get("convsize", convSize_p);
    rec.get("convsupport", convSupport_p);
    rec.get("convfunc",convFunc_p);
    if(rec.isDefined("wscaler"))
      rec.get("wscaler", wScaler_p);
    rec.get("convsampling", convSampling_p);
    rec.get("nx", nx_p);
    rec.get("ny", ny_p);
  }
  catch(AipsError x) {
    err=x.getMesg();
    return false;
  }
  return true;

  }




} //end of namespace refim
} //# NAMESPACE CASA - END
