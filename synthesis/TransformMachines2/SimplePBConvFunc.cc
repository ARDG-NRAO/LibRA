//# SimplePBConvFunc.cc: implementation of SimplePBConvFunc
//# Copyright (C) 2007-2020
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Slice.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/OS/Timer.h>
#include <casa/Utilities/Assert.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/MVAngle.h>
#include <measures/Measures/MeasTable.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>

#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <images/Images/SubImage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <ms/MeasurementSets/MSColumns.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/LatticeCache.h>
#include <lattices/LatticeMath/LatticeFFT.h>

#include <scimath/Mathematics/ConvolveGridder.h>

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisibilityIterator.h>

#include <synthesis/TransformMachines2/SimplePBConvFunc.h>
#include <synthesis/TransformMachines2/SkyJones.h>

#include <casa/Utilities/CompositeNumber.h>
#include <iomanip>
#include <math.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
namespace refim {//# namespace for imaging refactor
using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::refim;

SimplePBConvFunc::SimplePBConvFunc(): nchan_p(-1),
        npol_p(-1), pointToPix_p(), directionIndex_p(-1), thePix_p(0),
        filledFluxScale_p(false),doneMainConv_p(0),
                                      
				      calcFluxScale_p(true), usePointingTable_p(False), actualConvIndex_p(-1), convSize_p(0), convSupport_p(0), pointingPix_p()    {
    //

    pbClass_p=PBMathInterface::COMMONPB;
    ft_p=FFT2D(true);
    usePointingTable_p=False;
}

  SimplePBConvFunc::SimplePBConvFunc(const PBMathInterface::PBClass typeToUse): 
    nchan_p(-1),npol_p(-1),pointToPix_p(),
    directionIndex_p(-1), thePix_p(0), filledFluxScale_p(false),doneMainConv_p(0), 
    calcFluxScale_p(true), usePointingTable_p(False), actualConvIndex_p(-1), convSize_p(0), convSupport_p(0), pointingPix_p() {
    //
    pbClass_p=typeToUse;
    ft_p=FFT2D(true);
    usePointingTable_p=False;
  }
  SimplePBConvFunc::SimplePBConvFunc(const RecordInterface& rec, const Bool calcfluxneeded)
  : nchan_p(-1),npol_p(-1),pointToPix_p(), directionIndex_p(-1), thePix_p(0), filledFluxScale_p(false),
    doneMainConv_p(0), 
    calcFluxScale_p(calcfluxneeded), usePointingTable_p(False), actualConvIndex_p(-1), convSize_p(0), convSupport_p(0), pointingPix_p() 
  {
    String err;
    fromRecord(err, rec, calcfluxneeded);
    ft_p=FFT2D(true);
    usePointingTable_p=False;
  }
  SimplePBConvFunc::~SimplePBConvFunc(){
    //

  }

  void SimplePBConvFunc::storeImageParams(const ImageInterface<Complex>& iimage,
					  const vi::VisBuffer2& vb){
    //image signature changed...rather simplistic for now
    if((iimage.shape().product() != nx_p*ny_p*nchan_p*npol_p) || nchan_p < 1){
      csys_p=iimage.coordinates();
      Int coordIndex=csys_p.findCoordinate(Coordinate::DIRECTION);
      AlwaysAssert(coordIndex>=0, AipsError);
      directionIndex_p=coordIndex;
      dc_p=csys_p.directionCoordinate(directionIndex_p);
      ObsInfo imInfo=csys_p.obsInfo();
      String tel= imInfo.telescope();
      MPosition pos;
      MSColumns mscol(vb.ms());
      if (vb.subtableColumns().observation().nrow() > 0) {
	tel =vb.subtableColumns().observation().telescopeName()(mscol.observationId()(0));
      }
      if (tel.length() == 0 || !tel.contains("VLA") ||
	  !MeasTable::Observatory(pos,tel)) {
	// unknown observatory, use first antenna
    	  Int ant1=vb.antenna1()(0);
    	  pos=vb.subtableColumns().antenna().positionMeas()(ant1);
      }
      imInfo.setTelescope(tel);
      csys_p.setObsInfo(imInfo);
      //Store this to build epochs via the time access of visbuffer later
      timeMType_p=MEpoch::castType(mscol.timeMeas()(0).getRef().getType());
      timeUnit_p=Unit(mscol.timeMeas().measDesc().getUnits()(0).getName());
      // timeUnit_p=Unit("s");
      //cout << "UNIT " << timeUnit_p.getValue() << " name " << timeUnit_p.getName()  << endl;
      pointFrame_p=MeasFrame(imInfo.obsDate(), pos);
      MDirection::Ref elRef(dc_p.directionType(), pointFrame_p);
      //For now we set the conversion from this direction 
      pointToPix_p=MDirection::Convert( MDirection(), elRef);
      nx_p=iimage.shape()(coordIndex);
      ny_p=iimage.shape()(coordIndex+1);
      pointingPix_p.resize(nx_p, ny_p);
      pointingPix_p.set(false);
      coordIndex=csys_p.findCoordinate(Coordinate::SPECTRAL);
      Int pixAxis=csys_p.pixelAxes(coordIndex)[0];
      nchan_p=iimage.shape()(pixAxis);
      coordIndex=csys_p.findCoordinate(Coordinate::STOKES);
      pixAxis=csys_p.pixelAxes(coordIndex)[0];
      npol_p=iimage.shape()(pixAxis);
      if(calcFluxScale_p){
    	  if(fluxScale_p.shape().nelements()==0){
    		  fluxScale_p=TempImage<Float>(IPosition(4,nx_p,ny_p,npol_p,nchan_p), csys_p);
    		  fluxScale_p.set(0.0);
    	  }
    	  filledFluxScale_p=false;
      }
      
    }

  }

  void SimplePBConvFunc::toPix(const vi::VisBuffer2& vb, const MVDirection& extraShift, const Bool useExtraShift){
    thePix_p.resize(2);

    const MDirection& p1=pointingDirAnt1(vb);
    if(dc_p.directionType() !=  MDirection::castType(p1.getRef().getType())){
      //pointToPix_p.setModel(theDir);

    	String tel= csys_p.obsInfo().telescope();
    	if(!tel.contains("VLA")) {
    		//use first antenna as direction1_p is used to calculate pointing
    		// as only VLA uses observatory pos for calculations
    	    	  Int ant1=vb.antenna1()(0);
    	    	  MPosition pos=vb.subtableColumns().antenna().positionMeas()(ant1);
    	    	  pointFrame_p.resetPosition(pos);
    	}
      MEpoch timenow(Quantity(vb.time()(0), timeUnit_p), timeMType_p);
      //cerr << "Ref " << vb.direction1()(0).getRefString() <<  " ep " << timenow.getRefString() << " time " << MVTime(timenow.getValue().getTime()).string(MVTime::YMD) << endl; 
      pointFrame_p.resetEpoch(timenow);
      ///////////////////////////
      //MDirection some=pointToPix_p(vb.direction1()(0));
      //MVAngle mvRa=some.getAngle().getValue()(0);
      //MVAngle mvDec=some.getAngle().getValue()(1);
      
      //cout  << mvRa(0.0).string(MVAngle::TIME,8) << "   ";
      // cout << mvDec.string(MVAngle::DIG2,8) << "   ";
      //cout << MDirection::showType(some.getRefPtr()->getType()) << endl;

      //////////////////////////
      //pointToPix holds pointFrame_p by reference...
      //thus good to go for conversion
      direction1_p=pointToPix_p(p1);
      //direction2_p=pointToPix_p(vb.direction2()(0));
      direction2_p=direction1_p;
      

    }
    else{
      direction1_p=p1;
     
      //direction2_p=vb.direction2()(0);
      //For now 
      direction2_p=direction1_p;
     
    }
    //Should return both antennas direction in the future
    
    if(useExtraShift){
      direction1_p.shift(extraShift, False);
      direction2_p.shift(extraShift, False);
    }
    dc_p.toPixel(thePix_p, direction1_p);
   
  }

  void SimplePBConvFunc::setWeightImage(CountedPtr<TempImage<Float> >& wgtimage){
    convWeightImage_p=wgtimage;
    calcFluxScale_p=true;

  }
 
  void SimplePBConvFunc::reset(){
    doneMainConv_p.resize();
    convFunctions_p.resize(0, true);
    convWeights_p.resize(0, true);
    convSizes_p.resize(0, true);
    convSupportBlock_p.resize(0, true);
    convFunctionMap_p.clear();
   vbConvIndex_p.clear();
   ft_p=FFT2D(true);
  }



  Int SimplePBConvFunc::convIndex(const vi::VisBuffer2& vb){
	  String elkey=String::toString(vb.msId())+String("_")+String::toString(vb.spectralWindows()[0]);
	  if(vbConvIndex_p.count(elkey) > 0){
		  return vbConvIndex_p[elkey];
	  }
	  Int val=vbConvIndex_p.size();
	  vbConvIndex_p[elkey]=val;
	  return val;
  }

  const MDirection& SimplePBConvFunc::pointingDirAnt1(const vi::VisBuffer2& vb){
   
    
    std::ostringstream oss;
    
    oss << vb.msId() << "_" << vb.antenna1()(0) << "_";
    oss.precision(13);
    oss << vb.time()(0);
    String elkey=oss.str();
    //  String elkey=String::toString(vb.msId())+String("_")+String::toString(vb.antenna1()(0))+String("_")
    //									  +String::toString(vb.time()(0));

    //cerr << "key " << elkey << " count " << ant1PointVal_p.count(elkey)  << " size " << ant1PointVal_p.size() << "  " << ant1PointingCache_p.nelements() << endl;
    if(ant1PointVal_p.count(elkey) > 0){
      return ant1PointingCache_p[ant1PointVal_p[elkey]];

    }
    Bool hasValidPointing=False;
    if(Table::isReadable(vb.ms().pointingTableName())){
      hasValidPointing=usePointingTable_p &&  (vb.ms().pointing().nrow() >0);
    }
   
    Int val=ant1PointingCache_p.nelements();
    ant1PointingCache_p.resize(val+1, true);
    if(hasValidPointing){
      //ant1PointingCache_p[val]=vb.direction1()[0];
      ant1PointingCache_p[val]=vbutil_p->getPointingDir(vb, vb.antenna1()(0), 0, dc_p.directionType());
    }
    else
      ant1PointingCache_p[val]=vbutil_p->getPhaseCenter(vb);
    //ant1PointingCache_p[val]=vbUtil_p.getPointingDir(vb, vb.antenna1()(0), 0);
    ant1PointVal_p[elkey]=val;
    return ant1PointingCache_p[val];

  }
void SimplePBConvFunc::findConvFunction(const ImageInterface<Complex>& iimage, 
					const vi::VisBuffer2& vb,
					const Int& convSampling,
					const Vector<Double>& visFreq, 
					  Array<Complex>& convFunc, 
					  Array<Complex>& weightConvFunc, 
					  Vector<Int>& convsize,
					  Vector<Int>& convSupport,
					  Vector<Int>& convFuncPolMap,
					  Vector<Int>& convFuncChanMap,
					  Vector<Int>& convFuncRowMap,
					const Bool /*getConjFreqConvFunc*/,
					const MVDirection& extraShift, const Bool useExtraShift
					  ){



  Int convSamp=2*convSampling;
  /////////////////////////
  LogIO os1;
  os1<< "msID " << vb.msId()  <<  " ANT1 id" << vb.antenna1()(0) << " direction " << vb.direction1()(0).toString() <<  " ANT2 id" << vb.antenna2()(0) << " direction " << vb.direction2()(0).toString() << LogIO::DEBUG1 << LogIO::POST ; 
    //////////////////////
  storeImageParams(iimage, vb);
  convFuncChanMap.resize(vb.nChannels());
  Vector<Double> beamFreqs;
  findUsefulChannels(convFuncChanMap, beamFreqs, vb, visFreq);
  //cerr << "CHANMAP " << convFuncChanMap << " beamFreqs " << beamFreqs << endl;
  Int nBeamChans=beamFreqs.nelements();
  //indgen(convFuncChanMap);
  convFuncPolMap.resize(vb.nCorrelations());
  convFuncPolMap.set(0);
  //Only one plane in this version
  convFuncRowMap.resize();
  convFuncRowMap=Vector<Int>(vb.nRows(),0);
  //break reference
  convFunc.resize();
  weightConvFunc.resize();
  LogIO os;
  os << LogOrigin("SimplePBConv", "findConvFunction")  << LogIO::NORMAL;
  
  
  // Get the coordinate system
  CoordinateSystem coords(iimage.coordinates());
  
  
  actualConvIndex_p=convIndex(vb);
  //cerr << "In findConv " << actualConvIndex_p << endl;
  // Make a two dimensional image to calculate the
  // primary beam. We want this on a fine grid in the
  // UV plane 
  Int directionIndex=directionIndex_p;
    AlwaysAssert(directionIndex>=0, AipsError);
    
    // Set up the convolution function.
    Int nx=nx_p;
    Int ny=ny_p;
    //    convSize_p=max(nx,ny)*convSampling;
    //cerr << "size " << nx << "  " << ny << endl;
    //3 times the support size
    if(doneMainConv_p.shape()[0] < (actualConvIndex_p+1)){
      // cerr << "resizing DONEMAIN " <<   doneMainConv_p.shape()[0] << endl;
      doneMainConv_p.resize(actualConvIndex_p+1, true);
      doneMainConv_p[actualConvIndex_p]=false;
    }

    if(!(doneMainConv_p[actualConvIndex_p])){

      //convSize_p=4*(sj_p->support(vb, coords));
      convSize_p=Int(max(nx_p, ny_p)/2)*2*convSamp;
      // Make this a nice composite number, to speed up FFTs
      //cerr << "convSize_p 0 " <<  convSize_p << " convSamp " << convSamp<< endl;
      CompositeNumber cn(uInt(convSize_p*2.0));  
     
      convSize_p  = cn.nextLargerEven(Int(convSize_p));
      //cerr << "convSize : " << convSize_p << endl;

    }
    
   
    toPix(vb, extraShift, useExtraShift);
    //Timer tim;
    //tim.mark();
    addPBToFlux(vb);
    //tim.show("After addPBToFlux");
    DirectionCoordinate dc=dc_p;

    //where in the image in pixels is this pointing
    Vector<Double> pixFieldDir(2);
    pixFieldDir=thePix_p;

    //cerr << "pix of pointing " << pixFieldDir << endl;
    MDirection fieldDir=direction1_p;
    //shift from center
    pixFieldDir(0) = pixFieldDir(0) - Double(nx / 2);
    pixFieldDir(1) = pixFieldDir(1) - Double(ny / 2);

    //phase gradient per pixel to apply
    pixFieldDir(0) = -pixFieldDir(0)*2.0*C::pi/Double(nx)/Double(convSampling);
    pixFieldDir(1) = -pixFieldDir(1)*2.0*C::pi/Double(ny)/Double(convSampling);

    //cerr << "DonemainConv " << doneMainConv_p[actualConvIndex_p] << endl;
    if(!doneMainConv_p[actualConvIndex_p]){
      Vector<Double> sampling;
      sampling = dc.increment();
      sampling*=Double(convSamp);
      sampling(0)*=Double(nx)/Double(convSize_p);
      sampling(1)*=Double(ny)/Double(convSize_p);
      dc.setIncrement(sampling);
      
      
      Vector<Double> unitVec(2);
      unitVec=convSize_p/2;
      dc.setReferencePixel(unitVec);
      
      
      //make sure we are using the same units
      fieldDir.set(dc.worldAxisUnits()(0));
      
      dc.setReferenceValue(fieldDir.getAngle().getValue());
      
      coords.replaceCoordinate(dc, directionIndex);
      Int spind=coords.findCoordinate(Coordinate::SPECTRAL);
      SpectralCoordinate spCoord=coords.spectralCoordinate(spind);
      spCoord.setReferencePixel(Vector<Double>(1,0.0));
      spCoord.setReferenceValue(Vector<Double>(1, beamFreqs(0)));
      if(beamFreqs.nelements() >1)
	spCoord.setIncrement(Vector<Double>(1, beamFreqs(1)-beamFreqs(0)));
      coords.replaceCoordinate(spCoord, spind);


      CoordinateSystem coordLastPlane= coords;
      spCoord.setReferenceValue(Vector<Double>(1, beamFreqs(nBeamChans-1)));
      coordLastPlane.replaceCoordinate(spCoord, spind);
      //cerr << "BEAM freqs " << beamFreqs << endl;

      //  coords.list(logIO(), MDoppler::RADIO, IPosition(), IPosition());
      
      Int tempConvSize=((convSize_p/4/(convSamp/convSampling))/2)*2;
      IPosition pbShape(4, tempConvSize, tempConvSize, 1, nBeamChans);
      
      Long memtot=HostInfo::memoryFree();
      Double memtobeused= Double(memtot)*1024.0;
      //check for 32 bit OS and limit it to 2Gbyte
      if( sizeof(void*) == 4){
    	  if(memtot > 2000000)
    		  memtot=2000000;
      }
      if(memtot <= 2000000)
    	  memtobeused=0.0;
      //cerr << "mem to be used " << memtobeused << endl;
      //tim.mark();
      IPosition start(4, 0, 0, 0, 0);
      //IPosition pbSlice(4, convSize_p, convSize_p, 1, 1);
      //cerr << "pbshape " << pbShape << endl;
      TempImage<Complex> twoDPB(TiledShape(pbShape, IPosition(4, pbShape(0), pbShape(1), 1, 1)), coords, memtobeused/10.0);

      //tim.show("after making one image");
      convFunc_p.resize(tempConvSize, tempConvSize);
      convFunc_p=0.0;
      
      

      // Accumulate terms 
      //Matrix<Complex> screen(convSize_p, convSize_p);
      //screen=1.0;
      // Either the SkyJones
      //tim.mark();
      //twoDPB.set(Complex(1.0,0.0));
      //for (Int k=0; k < nBeamChans; ++k){
      //blcin[3]=k;
      //trcin[3]=k;
      //Slicer slin(blcin, trcin, Slicer::endIsLast);
      //SubImage<Complex> subim(twoDPB, slin, true);
      TempImage<Complex> subim(IPosition(4, convSize_p, convSize_p, 1, 1), coordLastPlane, memtobeused/2.2);
      subim.set(Complex(1.0,0.0));
      //twoDPB.putSlice(screen, start);
      sj_p->apply(subim, subim, vb, 0); 
      //LatticeFFT::cfft2d(subim);
      ft_p.c2cFFT(subim);
	//  }
      //tim.show("after an apply" );
      //tim.mark();
      TempImage<Float> screen2(TiledShape(IPosition(4, convSize_p, convSize_p, 1, 1)), coordLastPlane, memtobeused/2.2);
      screen2.set(1.0);
      TempImage<Complex> subout(TiledShape(IPosition(4, convSize_p, convSize_p, 1, 1)), coordLastPlane, memtobeused/2.2);
      //////Making a reference on half of the lattice as on the Mac rcfft is failing for some 
      //////reason
      SubImage<Complex> halfsubout(subout, Slicer(IPosition(4, 0, 0, 0, 0), IPosition(4, convSize_p/2, convSize_p-1, 0, 0), Slicer::endIsLast), true);
      sj_p->applySquare(screen2, screen2, vb, 0); 
      ft_p.r2cFFT(halfsubout, screen2);
      //LatticeFFT::rcfft(halfsubout, screen2, true, false);
      //Real FFT fills only first half of the array
      //making it look like a Complex to Complex FFT
      IPosition iblc(4, 0, 3*subout.shape()(1)/8, 0, 0);
      IPosition itrc(4, 0, 5*subout.shape()(1)/8, 0, 0);
      for(Int x=subout.shape()(0)/2; x <(5*subout.shape()(0)/8); ++x){
	
	iblc[0]=x-subout.shape()(0)/2;
	itrc[0]=x-subout.shape()(0)/2;
	Slicer isl(iblc, itrc, Slicer::endIsLast);
	iblc[0]=x;
	subout.putSlice(subout.getSlice(isl), iblc);
      }
      for(Int x=subout.shape()(0)/2+1; x <(5*subout.shape()(0)/8); ++x){
	
	iblc[0]=x;
	itrc[0]=x;
	Slicer isl(iblc, itrc, Slicer::endIsLast);
	iblc[0]=subout.shape()(0)-x;
	subout.putSlice(subout.getSlice(isl), iblc);
	if(x==(subout.shape()(0)-1)){
	  iblc[0]=0;
	  subout.putSlice(subout.getSlice(isl), iblc);
	}
      }
      //End of FFT's
      //tim.show("After apply2 ");
      TempImage<Complex> twoDPB2(TiledShape(pbShape, IPosition(4, pbShape(0), pbShape(1), 1, 1)), coords, memtobeused/10.0);
      
      IPosition blcout(4, 0, 0, 0, nBeamChans-1);
      IPosition trcout(4, pbShape(0)-1, pbShape(1)-1, 0,nBeamChans-1);
      Slicer outsl(blcout, trcout, Slicer::endIsLast);
      
      IPosition blcin(4, convSize_p/2-pbShape(0)/2, convSize_p/2-pbShape(1)/2, 0, 0);
      IPosition trcin(4, convSize_p/2+pbShape(0)/2-1, convSize_p/2+pbShape(1)/2-1, 0, 0);
      Slicer insl(blcin, trcin, Slicer::endIsLast);
      {
	SubImage<Complex> subtwoDPB(twoDPB, outsl, true);
	SubImage<Complex> intwoDPB(subim, insl, false);
	subtwoDPB.copyData(intwoDPB);
      }
      {
	SubImage<Complex> subtwoDPB2(twoDPB2, outsl, true);
	SubImage<Complex> intwoDPB2(subout, insl, false);
	subtwoDPB2.copyData(intwoDPB2);
      }
      
      if(nBeamChans > 0){
	blcin=IPosition(4,0,0,0, nBeamChans-1);
	trcin=IPosition(4, pbShape(0)-1, pbShape(1)-1, 0, nBeamChans-1);
	Slicer slin(blcin, trcin, Slicer::endIsLast);
	SubImage<Complex> origPB(twoDPB, slin, false);
	IPosition elshape= origPB.shape();
	Matrix<Complex> i1=origPB.get(true);
	SubImage<Complex> origPB2(twoDPB2, slin, false);
	Matrix<Complex> i2=origPB2.get(true);
	Int cenX=i1.shape()(0)/2;
	Int cenY=i1.shape()(1)/2;
	
	   
	for (Int kk=0; kk < nBeamChans; ++kk){
	  Double fratio=beamFreqs(kk)/beamFreqs(nBeamChans-1);
	  //cerr << "fratio " << fratio << endl;
	  Float convRatio=convSamp/convSampling;
	  blcin[3]=kk;
	  trcin[3]=kk;
	  //Slicer slout(blcin, trcin, Slicer::endIsLast);
	  Matrix<Complex> o1(i1.shape(), Complex(0.0));
	  Matrix<Complex> o2(i2.shape(), Complex(0.0));
	  for (Int yy=0;  yy < i1.shape()(1); ++yy){
	    //Int nyy= (Double(yy-cenY)*fratio) + cenY; 
	    Double nyy= (Double((yy-cenY)*convRatio)/fratio) + cenY;
	    Double cyy=ceil(nyy);
	    Double fyy= floor(nyy);
	    Int iy=nyy > fyy+0.5 ? Int(cyy) : Int(fyy); 
	    if(cyy <2*cenY && fyy >=0.0)
	      for(Int xx=0; xx < i1.shape()(0); ++ xx){
		//Int nxx= Int(Double(xx-cenX)*fratio) + cenX; 
		Double nxx= Int(Double((xx-cenX)*convRatio)/fratio) + cenX;
		Double cxx=ceil(nxx);
		Double fxx= floor(nxx);
		Int ix=nxx > fxx+0.5 ? Int(cxx) : Int(fxx) ;
		if(cxx < 2*cenX && fxx >=0.0 ){
		  //Double dist=sqrt((nxx-cxx)*(nxx-cxx)+(nyy-cyy)*(nyy-cyy))/sqrt(2.0);
		  //o1(xx, yy)=float(1-dist)*i1(fxx, fyy)+ dist*i1(cxx,cyy);
		  o1(xx, yy)=i1( ix, iy);
		  //o2(xx, yy)=i2(nxx, nyy);
		  //o2(xx, yy)=float(1-dist)*i2(fxx, fyy)+ dist*i2(cxx,cyy);
		  o2(xx, yy)=i2(ix, iy);
		}
	      }
	  }
	  twoDPB.putSlice(o1.reform(elshape), blcin);
	  twoDPB2.putSlice(o2.reform(elshape), blcin);
	}
	
      }

      /*
      {
	TempImage<Float> screen2(TiledShape(pbShape, IPosition(4, pbShape(0), pbShape(1), 1, 1)), coords, memtobeused);
    	  //	Matrix<Float> screenoo(convSize_p, convSize_p);
    	  //screenoo.set(1.0);
    	  //screen2.putSlice(screenoo,start);
    	  //screen2.set(1.0);
	  for (Int k=0; k < nBeamChans; ++k){
	    blcin[3]=k;
	    trcin[3]=k;
	    Slicer slin(blcin, trcin, Slicer::endIsLast);
	    SubImage<Float> subim(screen2, slin, true);
	    SubImage<Complex> subout(twoDPB2, slin, true);
	    subim.set(1.0);
	    //twoDPB.putSlice(screen, start);
	    sj_p->applySquare(subim, subim, vb, 0); 
	    //// LatticeExpr<Complex> le(subim);
	    //// subout.copyData(le);
	    ///// LatticeFFT::cfft2d(subout);
	   
	    LatticeFFT::rcfft(subout, subim, true, false);
	    IPosition iblc(4, 0, 3*subout.shape()(1)/8, 0, 0);
	    IPosition itrc(4, 0, 5*subout.shape()(1)/8, 0, 0);
	    for(Int x=subout.shape()(0)/2; x <(5*subout.shape()(0)/8); ++x){
	      
	      iblc[0]=x-subout.shape()(0)/2;
	      itrc[0]=x-subout.shape()(0)/2;
	      Slicer isl(iblc, itrc, Slicer::endIsLast);
	      iblc[0]=x;
	      subout.putSlice(subout.getSlice(isl), iblc);
	    }
	    for(Int x=subout.shape()(0)/2+1; x <(5*subout.shape()(0)/8); ++x){
	      
	      iblc[0]=x;
	      itrc[0]=x;
	      Slicer isl(iblc, itrc, Slicer::endIsLast);
	      iblc[0]=subout.shape()(0)-x;
	      subout.putSlice(subout.getSlice(isl), iblc);
	      if(x==(subout.shape()(0)-1)){
		iblc[0]=0;
		subout.putSlice(subout.getSlice(isl), iblc);
	      }
	    }
	    
	  }
      
    	  //sj_p->applySquare(screen2, screen2, vb, 0);
    	  //LatticeExpr<Complex> le(screen2);
    	  //twoDPB2.copyData(le);
      }
      
      */ 
      
      /*
      if(0) {
	CoordinateSystem ftCoords(coords);
	//directionIndex=ftCoords.findCoordinate(Coordinate::DIRECTION);
	//AlwaysAssert(directionIndex>=0, AipsError);
	dc=coords.directionCoordinate(directionIndex);
	Vector<Bool> axes(2); axes(0)=true;axes(1)=true;
	Vector<Int> shape(2); shape(0)=convSize_p;shape(1)=convSize_p;
	Coordinate* ftdc=dc.makeFourierCoordinate(axes,shape);
	//ftCoords.replaceCoordinate(*ftdc, directionIndex);
	delete ftdc; ftdc=0;
	ostringstream os1;
	os1 << "Screen_" << vb.fieldId() ;
	PagedImage<Complex> thisScreen(twoDPB2.shape(), ftCoords, String(os1));
	//LatticeExpr<Float> le(abs(twoDPB2));
	thisScreen.copyData(twoDPB2);
	LatticeFFT::cfft2d(thisScreen, false);
	PagedImage<Complex> thisScreen0(twoDPB.shape(), ftCoords, String("PB_")+String(os1));
	thisScreen0.copyData(twoDPB);
	LatticeFFT::cfft2d(thisScreen0, false);
      }
      */
      /* 
      // Now FFT and get the result back
      //LatticeFFT::cfft2d(twoDPB);
      //LatticeFFT::cfft2d(twoDPB2);
      
      // Write out FT of screen as an image
      if(0) {
	CoordinateSystem ftCoords(coords);
	directionIndex=ftCoords.findCoordinate(Coordinate::DIRECTION);
	AlwaysAssert(directionIndex>=0, AipsError);
	dc=coords.directionCoordinate(directionIndex);
	Vector<Bool> axes(2); axes(0)=true;axes(1)=true;
	Vector<Int> shape(2); shape(0)=convSize_p;shape(1)=convSize_p;
	Coordinate* ftdc=dc.makeFourierCoordinate(axes,shape);
	ftCoords.replaceCoordinate(*ftdc, directionIndex);
	delete ftdc; ftdc=0;
	ostringstream os1;
	os1 << "FTScreen_" << vb.fieldId() ;
	PagedImage<Float> thisScreen(pbShape, ftCoords, String(os1));
	LatticeExpr<Float> le(abs(twoDPB2));
	thisScreen.copyData(le);
      }
      */
      //cerr << "twoDPB shape " << twoDPB.shape() << " slice shape " << IPosition(4, tempConvSize, tempConvSize, 1, 1) << endl;
      convFunc_p=twoDPB.getSlice(IPosition(4,0,0,0,0), IPosition(4, tempConvSize, tempConvSize, 1, 1), true);
      weightConvFunc_p.resize();
      weightConvFunc_p=twoDPB2.getSlice(IPosition(4,0,0,0,nBeamChans-1), IPosition(4, tempConvSize, tempConvSize, 1, 1), true);
      //convFunc/=max(abs(convFunc));
      Float maxAbsConvFunc=max(amplitude(weightConvFunc_p));
      
      Float minAbsConvFunc=min(amplitude(weightConvFunc_p));
      //cerr << "min max " << minAbsConvFunc << "  " <<  maxAbsConvFunc << endl;
      convSupport_p=-1;
      Bool found=false;
      //Bool found2=true;
      //Int trial2=0;
      Int trial=0;
      for (trial=tempConvSize/2-2;trial>0;trial--) {
	//Searching down a diagonal
	if(abs(weightConvFunc_p(tempConvSize/2-trial, tempConvSize/2-trial)) >  (1.0e-3*maxAbsConvFunc)) {
	  found=true;
	  trial=Int(sqrt(2.0*Float(trial*trial)));
	  break;
	}
      }
      if(!found){
	if((maxAbsConvFunc-minAbsConvFunc) > (1.0e-3*maxAbsConvFunc)) 
	  found=true;
	// if it drops by more than 2 magnitudes per pixel
	trial=( tempConvSize > (10*convSampling)) ? 5*convSampling : (tempConvSize/2 - 4*convSampling);
      }

      if(trial < 5*convSampling) 
	trial=( tempConvSize > (10*convSampling)) ? 5*convSampling : (tempConvSize/2 - 4*convSampling);
      
      if(found) {
	convSupport_p=Int(0.5+Float(trial)/Float(convSampling))+1;
      }
      else {
	os << "Convolution function is misbehaved - support seems to be zero\n"
	   << "Reasons can be: \n(1)The image definition not covering one or more of the pointings selected\n"
           << "(2) No unflagged data in a given pointing\n"
	   << "(3) The entries in the POINTING subtable do not match the field being imaged."
	   << "Please check, and try again with an empty POINTING subtable.)\n"
	   << LogIO::EXCEPTION;
      }

      // Normalize such that plane 0 sums to 1 (when jumping in
      // steps of convSampling)
      
      Double pbSum=0.0;
      //Double pbSum2=0.0;
      
      

      for (Int iy=-convSupport_p;iy<=convSupport_p;iy++) {
	for (Int ix=-convSupport_p;ix<=convSupport_p;ix++) {
	  Complex val=convFunc_p(ix*convSampling+tempConvSize/2,
				 iy*convSampling+tempConvSize/2);
	  pbSum+=real(val);
      
	  //pbSum+=sqrt(real(val)*real(val)+ imag(val)*imag(val));
	}
      }
      
 
      //pbSum=sum(amplitude(convFunc_p))/Double(convSampling)/Double(convSampling);

      if(pbSum>0.0) {
	convFunc_p*=Complex(1.0/pbSum,0.0);
      }
      else {
	os << "Convolution function integral is not positive"
	   << LogIO::EXCEPTION;
      }
      
      //##########################################
      os << "Convolution support = " << convSupport_p
	 << " pixels in Fourier plane"
	 << LogIO::POST;
      
      convSupportBlock_p.resize(actualConvIndex_p+1);
      convSizes_p.resize(actualConvIndex_p+1);
      //Only one beam for now...but later this should be able to
      // take all the beams for the different antennas.
      convSupportBlock_p[actualConvIndex_p]=new Vector<Int>(1);
      convSizes_p[actualConvIndex_p]=new Vector<Int> (1);
      (*(convSupportBlock_p[actualConvIndex_p]))[0]=convSupport_p;
      convFunctions_p.resize(actualConvIndex_p+1);
      convWeights_p.resize(actualConvIndex_p+1);
      convFunctions_p[actualConvIndex_p]= new Array<Complex>();
      convWeights_p[actualConvIndex_p]= new Array<Complex>();
      Int newConvSize=2*(convSupport_p+2)*convSampling;
      //NEED to chop this right ...and in the centre
      if(newConvSize >=tempConvSize)
    	  newConvSize=tempConvSize;

      IPosition blc(4, (tempConvSize/2)-(newConvSize/2),
    		  (tempConvSize/2)-(newConvSize/2), 0, 0);
      IPosition trc(4, (tempConvSize/2)+(newConvSize/2-1),
		      (tempConvSize/2)+(newConvSize/2-1), 0, nBeamChans-1);
      convFunctions_p[actualConvIndex_p]->resize(IPosition(5, newConvSize, newConvSize, 1, nBeamChans,1));
      //cerr << "convFunc shape " << (convFunctions_p[actualConvIndex_p])->shape() << 
      //"  " << " twoDPB shape " <<twoDPB.get(false)(blc,trc).shape() << endl;
      convFunctions_p[actualConvIndex_p]->copyMatchingPart(twoDPB.get(false)(blc,trc));//*Complex(1.0/pbSum,0.0));
      convSize_p=newConvSize;
      convWeights_p[actualConvIndex_p]->resize(IPosition(5, newConvSize, newConvSize, 1, nBeamChans,1));
      convWeights_p[actualConvIndex_p]->copyMatchingPart(twoDPB2.get(false)(blc,trc));//*Complex(1.0/pbSum2,0.0));
      blc.resize(5, false);
      trc.resize(5,false);
      blc=IPosition(5, 0, 0, 0, 0,0);
      trc=IPosition(5, newConvSize-1, newConvSize-1, 0, 0,0);
      for (Int bc=0; bc< nBeamChans; ++bc){
	blc[3]=bc;
	trc[3]=bc;
	pbSum=real(sum((*convFunctions_p[actualConvIndex_p])(blc,trc)))/Double(convSampling)/Double(convSampling);
	(*convFunctions_p[actualConvIndex_p])(blc,trc)= 	(*convFunctions_p[actualConvIndex_p])(blc,trc)/pbSum;
	(*convWeights_p[actualConvIndex_p])(blc,trc)= 	(*convWeights_p[actualConvIndex_p])(blc,trc)/pbSum;
      }



      convFunc_p.resize();//break any reference
      (*convSizes_p[actualConvIndex_p])[0]=convSize_p;
      doneMainConv_p[actualConvIndex_p]=true;
      
    }
    else{
      convSize_p=(*convSizes_p[actualConvIndex_p])[0];

    }

    //Apply the shift phase gradient
    convFunc.resize();
    weightConvFunc.resize();
    convFunc.assign(*(convFunctions_p[actualConvIndex_p]));
    weightConvFunc.assign(*(convWeights_p[actualConvIndex_p]));
    Bool copyconv, copywgt;
    Complex *cv=convFunc.getStorage(copyconv);
    Complex *wcv=weightConvFunc.getStorage(copywgt);
    //cerr << "Field " << vb.fieldId() << " spw " << vb.spectralWindow() << " phase grad: " << pixFieldDir << endl;
   
    for (Int nc=0; nc < nBeamChans; ++nc){ 
    	Int planeoffset=nc*convSize_p*convSize_p;
    	for (Int iy=0;iy<convSize_p;iy++) {
    		Double cy, sy;
		Int offset;
	       
    		SINCOS(Double(iy-convSize_p/2)*pixFieldDir(1), sy, cy);
    		Complex phy(cy,sy) ;
    		offset = iy*convSize_p+planeoffset;
    		for (Int ix=0;ix<convSize_p;ix++) {
    			Double cx, sx;
    			SINCOS(Double(ix-convSize_p/2)*pixFieldDir(0), sx, cx);
    			Complex phx(cx,sx) ;
			cv[ix+offset]= cv[ix+offset]*phx*phy;
			wcv[ix+offset]= wcv[ix+offset]*phx*phy;

    		}
    	}
    }
    convFunc.putStorage(cv, copyconv);
    weightConvFunc.putStorage(wcv, copywgt);
    convsize.resize();
    convsize=*(convSizes_p[actualConvIndex_p]);
    convSupport.resize();
    convSupport=(*(convSupportBlock_p[actualConvIndex_p]));
    
    
  }

  void SimplePBConvFunc::setSkyJones(SkyJones* sj){
    sj_p=sj;
  }
  
  
  void SimplePBConvFunc::findUsefulChannels(Vector<Int>& chanMap, Vector<Double>& chanFreqs,  const vi::VisBuffer2& vb, const Vector<Double>& freq){
    
	  
	Int spw=vb.spectralWindows()(0);
	bandName_p=vb.subtableColumns().spectralWindow().name()(spw);
        Vector<Double> spwfreq=vb.subtableColumns().spectralWindow().chanFreq()(spw);
        Double spwfreqwidth=abs(Vector<Double>(vb.subtableColumns().spectralWindow().chanWidth()(spw))(0));
	chanMap.resize(freq.nelements());
    Vector<Double> localfreq=vb.getFrequencies(0, MFrequency::TOPO);
    Double minfreq=min(freq);
    Double maxfreq=max(freq);
    Double origwidth=freq.nelements()==1 ? 1e12 : (maxfreq-minfreq)/(freq.nelements()-1);
    ///Fractional bandwidth which will trigger mutiple PB in one spw
    
    Double tol=(max(spwfreq))*0.5/100;
    if(tol < origwidth/2.0) tol=origwidth/2.0;
    Double topFreq=max(spwfreq);
    while (topFreq > maxfreq){
      topFreq -= tol;
    }
    // For large tol
    if(topFreq < minfreq)
       topFreq += tol;
    //Int nchan=Int(lround((max(freq)-min(freq))/tol));
    Double bottomFreq=topFreq;
    Int nchan=0;
    //cerr << std::setprecision(12) << "top " << bottomFreq << " minFreq " << minfreq << " maxFreq " << maxfreq << endl;
     while(bottomFreq > minfreq){
       ++nchan;
       bottomFreq -= tol;
     }
     if(nchan > 1){
       nchan-=1;
       bottomFreq+=tol;
     }
     
     //cerr  << "TOLERA " << tol << " nchan " << nchan << " bot " << bottomFreq << " vb.nchan " << vb.nChannels() << endl;
    //Number of beams that matters are the ones in the data
     if(nchan > vb.nChannels()){
      nchan=vb.nChannels();
      tol=spwfreqwidth;
      bottomFreq=min(localfreq);
     }
   
    chanFreqs.resize();
    if(nchan >= (Int)(freq.nelements()-1)) { indgen(chanMap); chanFreqs=freq; return;}
    if((nchan==0) || (freq.nelements()==1)) { chanFreqs=Vector<Double>(1, bottomFreq);chanMap.set(0); return;}

    chanFreqs.resize(nchan);
    for(Int k=0; k < nchan; ++k){
      chanFreqs[k]=k*tol+bottomFreq;
    }
    
  
    Int activechan=0;
    chanMap.set(0);
    for (uInt k=0; k < chanMap.nelements(); ++k){
      Double mindiff=DBL_MAX;
      Int nearestchan=0;
      while((activechan< nchan) && Double(abs(freq[k]-chanFreqs[activechan])) > Double(tol/Double(2.0))){
        if(mindiff > Double(abs(freq[k]-chanFreqs[activechan]))){
          mindiff=Double(abs(freq[k]-chanFreqs[activechan]));
          nearestchan=activechan;
        }
          
        //	cerr << "k " << k << " atcivechan " << activechan << " comparison " 
	//     << freq[k] << "    " << chanFreqs[activechan]  << endl;	
	++activechan;
      }
      if(activechan != nchan){
	chanMap[k]=activechan;
      }
      //////////////////
      else{
        //cerr << std::setprecision(10) << "freq diffs " << freq[k]-chanFreqs << "  TOL " << tol/2.0 << endl;
        chanMap[k]=nearestchan;
      }
      ///////////////////////////
      activechan=0;
    }
    //cerr << "chanfreqs "  << chanFreqs << endl;
    //cerr << "USEFULchan " << chanMap << endl;
    return;
  }
  /*
  
    void SimplePBConvFunc::findUsefulChannels(Vector<Int>& chanMap, Vector<Double>& chanFreqs,  const vi::VisBuffer2& vb, const Vector<Double>& freq){
    
	  
	Int spw=vb.spectralWindows()(0);
	bandName_p=vb.subtableColumns().spectralWindow().name()(spw);
	chanMap.resize(freq.nelements());
    Vector<Double> localfreq=vb.getFrequencies(0, MFrequency::TOPO);
    Double minfreq=min(freq);
    
    Double origwidth=freq.nelements()==1 ? 1e12 : (max(freq)-min(freq))/(freq.nelements()-1);
    ///Fractional bandwidth which will trigger mutiple PB in one spw
    Double tol=(max(freq))*0.5/100;
    
    Int nchan=Int(lround((max(freq)-min(freq))/tol));
   
    //cerr  << "TOLERA " << tol << " nchan " << nchan << " vb.nchan " << vb.nChannel() << endl;
    //Number of beams that matters are the ones in the data
    if(nchan > vb.nChannels())
      nchan=vb.nChannels();

    if(tol < origwidth) tol=origwidth;
    chanFreqs.resize();
    if(nchan >= (Int)(freq.nelements()-1)) { indgen(chanMap); chanFreqs=freq; return;}
    if((nchan==0) || (freq.nelements()==1)) { chanFreqs=Vector<Double>(1, freq[0]);chanMap.set(0); return;}

    //readjust the tolerance...
    tol=(max(freq)-min(freq)+origwidth)/Double(nchan);
    chanFreqs.resize(nchan);
    for (Int k=0; k < nchan; ++k)
      chanFreqs[k]=minfreq-origwidth+tol/2.0+tol*Double(k);
    Int activechan=0;
    chanMap.set(0);
    for (uInt k=0; k < chanMap.nelements(); ++k){
      Double mindiff=DBL_MAX;
      Int nearestchan=0;
      while((activechan< nchan) && Double(abs(freq[k]-chanFreqs[activechan])) > Double(tol/Double(2.0))){
        if(mindiff > Double(abs(freq[k]-chanFreqs[activechan]))){
          mindiff=Double(abs(freq[k]-chanFreqs[activechan]));
          nearestchan=activechan;
        }
          
        //	cerr << "k " << k << " atcivechan " << activechan << " comparison " 
	//     << freq[k] << "    " << chanFreqs[activechan]  << endl;	
	++activechan;
      }
      if(activechan != nchan){
	chanMap[k]=activechan;
      }
      //////////////////
      else{
        //cerr << std::setprecision(10) << "freq diffs " << freq[k]-chanFreqs << "  TOL " << tol/2.0 << endl;
        chanMap[k]=nearestchan;
      }
      ///////////////////////////
      activechan=0;
    }
    //cerr << chanMap << endl;
    return;
  }
  */
  Bool SimplePBConvFunc::checkPBOfField(const vi::VisBuffer2& vb){
    //Int fieldid=vb.fieldId();
    String msid=vb.msName(true);
    /*
     if(convFunctionMap_p.ndefined() > 0){
      if (((fluxScale_p.shape()[3] != nchan_p) || (fluxScale_p.shape()[2] != npol_p)) && calcFluxScale_p){
	convFunctionMap_p.clear();
      }
    }
    // if you rename the ms might be a problem
    String mapid=msid+String("_")+String::toString(fieldid);
    if(convFunctionMap_p.ndefined() == 0){
      convFunctionMap_p.define(mapid, 0);    
      actualConvIndex_p=0;
      if(calcFluxScale_p){
	// 0ne channel only is needed to keep track of pb coverage
	if(fluxScale_p.shape().nelements()==0){
	  fluxScale_p=TempImage<Float>(IPosition(4,nx_p,ny_p,npol_p,1), csys_p);
	  fluxScale_p.set(0.0);
	}
	filledFluxScale_p=false;
      }
      return false;
    }
    
    if(!convFunctionMap_p.isDefined(mapid)){
      actualConvIndex_p=convFunctionMap_p.ndefined();
      convFunctionMap_p.define(mapid, actualConvIndex_p);
      return false;
    }
    else{
      actualConvIndex_p=convFunctionMap_p(mapid);
      convFunc_p.resize(); // break any reference
      weightConvFunc_p.resize(); 
      //Here we will need to use the right xyPlane for different PA range.
      //convFunc_p.reference(convFunctions_p[actualConvIndex_p]->xyPlane(0));
      //weightConvFunc_p.reference(convWeights_p[actualConvIndex_p]->xyPlane(0));
      //Again this for one time of antenna only later should be fixed for all 
      // antennas independently
      convSupport_p=(*convSupportBlock_p[actualConvIndex_p])[0];
      convSize_p=(*convSizes_p[actualConvIndex_p])[0];

  }
*/
 
 return true;



  }
  

  ImageInterface<Float>&  SimplePBConvFunc::getFluxScaleImage(){

    if(!calcFluxScale_p)
      throw(AipsError("Programmer error: Cannot get flux scale"));
    if(!filledFluxScale_p){
      IPosition blc=fluxScale_p.shape();
      IPosition trc=fluxScale_p.shape();
      blc(0)=0; blc(1)=0; trc(0)=nx_p-1; trc(1)=ny_p-1;
      
      for (Int j=0; j < fluxScale_p.shape()(2); ++j){
	for (Int k=0; k < fluxScale_p.shape()(3) ; ++k){
	  
	  blc(2)=j; trc(2)=j;
	  blc(3)=k; trc(3)=k;
	  Slicer sl(blc, trc, Slicer::endIsLast);
	  SubImage<Float> fscalesub(fluxScale_p, sl, true);
	  Float planeMax;
	  LatticeExprNode LEN = max( fscalesub );
	  planeMax =  LEN.getFloat();
	  if(planeMax !=0){
	    fscalesub.copyData( (LatticeExpr<Float>) (fscalesub/planeMax));
	    
	  }
	}
      }
      /*
      if(0) {
	ostringstream os2;
	os2 << "ALL_" << "BEAMS" ;
	PagedImage<Float> thisScreen2(fluxScale_p.shape(), fluxScale_p.coordinates(), String(os2));
	thisScreen2.copyData(fluxScale_p);
      }
      */

      filledFluxScale_p=true;
    }
      

    return fluxScale_p;
    
  }


  Bool SimplePBConvFunc::toRecord(RecordInterface& rec){
    Int numConv=convFunctions_p.nelements();
    // not saving the protected variables as they are generated by
    // the first  call to storeImageParams 
    try{
      rec.define("name", "SimplePBConvFunc");
      rec.define("numconv", numConv);
      //cerr << "num of conv " << numConv << "  " << convFunctionMap_p.ndefined() << "  " <<convFunctions_p.nelements() << endl;
      std::map<String, Int>::iterator it=vbConvIndex_p.begin();
      for (Int k=0; k < numConv; ++k){
	rec.define("convfunctions"+String::toString(k), *(convFunctions_p[k]));
	rec.define("convweights"+String::toString(k), *(convWeights_p[k]));
	rec.define("convsizes"+String::toString(k), *(convSizes_p[k]));
	rec.define("convsupportblock"+String::toString(k), *(convSupportBlock_p[k]));
	//cerr << "k " << k << " key " << convFunctionMap_p.getKey(k) << " val " << convFunctionMap_p.getVal(k) << endl;
	rec.define(String("key")+String::toString(k), it->first);
	rec.define(String("val")+String::toString(k), it->second);
	it++;
      }
      rec.define("pbclass", Int(pbClass_p));
      rec.define("actualconvindex",  actualConvIndex_p);
      rec.define("donemainconv", doneMainConv_p);
      rec.define("usepointingtable", usePointingTable_p);
      //The following is not needed ..can be regenerated
      //rec.define("pointingpix", pointingPix_p);
    }
    catch(AipsError &x) {
      return false;
    }
    return true;
  }

  Bool SimplePBConvFunc::fromRecord(String& err, const RecordInterface& rec, Bool calcFluxneeded){
     Int numConv=0;
     //make sure storeImageParams is triggered
     nchan_p=0;
     
     try{
       if(!rec.isDefined("name") || rec.asString("name") != "SimplePBConvFunc"){
	 throw(AipsError("Wrong record to recover SimplePBConvFunc from"));
	}
       rec.get("numconv", numConv);
       convFunctions_p.resize(numConv, true, false);
       convSupportBlock_p.resize(numConv, true, false);
       convWeights_p.resize(numConv, true, false);
       convSizes_p.resize(numConv, true, false);
       convFunctionMap_p.clear( );
       vbConvIndex_p.erase(vbConvIndex_p.begin(), vbConvIndex_p.end());
       for (Int k=0; k < numConv; ++k){
	 convFunctions_p[k]=new Array<Complex>();
	 convWeights_p[k]=new Array<Complex>();
	 convSizes_p[k]=new Vector<Int>();
	 convSupportBlock_p[k]=new Vector<Int>();
	 rec.get("convfunctions"+String::toString(k), *(convFunctions_p[k]));
	 rec.get("convsupportblock"+String::toString(k), *(convSupportBlock_p[k]));
	 rec.get("convweights"+String::toString(k), *(convWeights_p[k]));
	 rec.get("convsizes"+String::toString(k), *(convSizes_p[k]));
	 String key;
	 Int val;
	 rec.get(String("key")+String::toString(k), key);
	 rec.get(String("val")+String::toString(k), val);
	 vbConvIndex_p[key]=val;
	 ant1PointVal_p.clear();
	 ant1PointingCache_p.resize();
	 //convFunctionMap_p.define(key,val);
       }
       pbClass_p=static_cast<PBMathInterface::PBClass>(rec.asInt("pbclass"));
       rec.get("actualconvindex",  actualConvIndex_p);
       rec.get("usepointingtable", usePointingTable_p);
       pointingPix_p.resize();
       //rec.get("pointingpix", pointingPix_p);
       calcFluxScale_p=calcFluxneeded;

     }
     catch(AipsError & x) {
       err=x.getMesg();
       return false;
     }
     return true;
     
  }
  void SimplePBConvFunc::addPBToFlux(const vi::VisBuffer2& vb){
    if(calcFluxScale_p){
      if(fluxScale_p.shape().nelements()==0)
	{ //cerr << "nx_p " << nx_p << endl;
	calcFluxScale_p=False;
	return;
      }
      Vector<Int> pixdepoint(2, -100000);
      convertArray(pixdepoint, thePix_p);
      if((pixdepoint(0) >=0) && (pixdepoint(0) < pointingPix_p.shape()[0]) && (pixdepoint(1) >=0) && (pixdepoint(1) < pointingPix_p.shape()[1])  && !pointingPix_p(pixdepoint(0), pixdepoint(1))){
	 TempImage<Float> thispb(fluxScale_p.shape(), fluxScale_p.coordinates());
	 thispb.set(1.0);
	 sj_p->applySquare(thispb, thispb, vb, 0);
	 LatticeExpr<Float> le(fluxScale_p+thispb);
	 fluxScale_p.copyData(le);
	 pointingPix_p(pixdepoint(0), pixdepoint(1))=true;
	 //LatticeExprNode LEN = max(fluxScale_p);
	 //Float maxsca=LEN.getFloat();
	 //Tempporary fix when cubesky is chunking...do not add on 
	 //already defined position
	 //if(maxsca > 1.98){
	 //  cerr << "avoiding subtract " << endl;
	//fluxScale_p.copyData(LatticeExpr<Float>(fluxScale_p-thispb));

	 //}      
      /*
      if(0) {
	ostringstream os1;
	os1 << "SINGLE_" << vb.fieldId() ;
	PagedImage<Float> thisScreen(fluxScale_p.shape(), fluxScale_p.coordinates(), String(os1));
	thisScreen.copyData(thispb);
	ostringstream os2;
	os2 << "ALL_" << vb.fieldId() ;
	PagedImage<Float> thisScreen2(fluxScale_p.shape(), fluxScale_p.coordinates(), String(os2));
	thisScreen2.copyData(fluxScale_p);
      }
      */
       }
    }

  }

  void SimplePBConvFunc::sliceFluxScale(Int npol) {
     IPosition fshp=fluxScale_p.shape();
     if (fshp(2)>npol){
       npol_p=npol;
       // use first npol planes...
       IPosition blc(4,0,0,0,0);
       IPosition trc(4,fluxScale_p.shape()(0)-1, fluxScale_p.shape()(1)-1,npol-1,fluxScale_p.shape()(3)-1);
       Slicer sl=Slicer(blc, trc, Slicer::endIsLast);
       //writeable if possible
       SubImage<Float> fluxScaleSub = SubImage<Float> (fluxScale_p, sl, true);
       fluxScale_p = TempImage<Float>(fluxScaleSub.shape(),fluxScaleSub.coordinates());
       LatticeExpr<Float> le(fluxScaleSub);
       fluxScale_p.copyData(le);
     }
  }
} //# END of name space REFIM
} //# NAMESPACE CASA - END







