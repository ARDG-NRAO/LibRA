// -*- C++ -*-
//# Utils.cc: Implementation of global functions from Utils.h 
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
#include <ms/MeasurementSets/MSRange.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casa/Logging/LogIO.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MeasTable.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casa/Utilities/Assert.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>
#include <lattices/LEL/LatticeExpr.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageRegrid.h>
#include <casa/Containers/Record.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/TiledLineStepper.h> 
#include <lattices/Lattices/LatticeStepper.h> 
#include <lattices/LatticeMath/LatticeFFT.h>
#include <casa/System/Aipsrc.h>
#include <msvis/MSVis/VisibilityIterator2.h>

using namespace casacore;
namespace casa{
  using namespace vi;
  namespace refim{
  //
  //--------------------------------------------------------------------------------------------
  //  
  void storeImg(String fileName,ImageInterface<Complex>& theImg, Bool writeReIm)
  {
    PagedImage<Complex> ctmp(theImg.shape(), theImg.coordinates(), fileName);
    LatticeExpr<Complex> le(theImg);
    ctmp.copyData(le);
    if (writeReIm)
      {
	ostringstream reName,imName;
	reName << "re" << fileName;
	imName << "im" << fileName;
	{
	  PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), reName);
	  LatticeExpr<Float> le(abs(theImg));
	  tmp.copyData(le);
	}
	{
	  PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), imName);
	  LatticeExpr<Float> le(arg(theImg));
	  tmp.copyData(le);
	}
      }
  }
  
  void storeImg(String fileName,ImageInterface<Float>& theImg)
  {
    PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), fileName);
    LatticeExpr<Float> le(theImg);
    tmp.copyData(le);
  }

  void storeArrayAsImage(String fileName, const CoordinateSystem& coord,
			 const Array<Complex>& theImg)
  {
    PagedImage<Complex> ctmp(theImg.shape(), coord, fileName);
    ctmp.put(theImg);
  }
  void storeArrayAsImage(String fileName, const CoordinateSystem& coord,
			 const Array<DComplex>& theImg)
  {
    PagedImage<DComplex> ctmp(theImg.shape(), coord, fileName);
    ctmp.put(theImg);
  }
  void storeArrayAsImage(String fileName, const CoordinateSystem& coord,
			 const Array<Float>& theImg)
  {
    PagedImage<Float> ctmp(theImg.shape(), coord, fileName);
    ctmp.put(theImg);
  }
  //
  //---------------------------------------------------------------------
  //
  // Get the time stamp for the for the current visibility integration.
  // Since VisTimeAverager() does not accumulate auto-correlations (it
  // should though!), and the VisBuffer can potentially have
  // auto-correlation placeholders, vb.time()(0) may not be correct (it
  // will be in fact zero when AC rows are present).  So look for the
  // first timestamp of a row corresponding to an unflagged
  // cross-correlation.
  //
  Double getCurrentTimeStamp(const VisBuffer2& vb)
  {
    //LogIO os(LogOrigin("Utils", "getCurrentTimeStamp", WHERE));

    Int N=vb.nRows();
    for(Int i=0;i<N;i++)
      {
	if ((!vb.flagRow()(i)) && (vb.antenna1()(i) != vb.antenna2()(i)))
	  return vb.time()(i);
      }
    //os << "No unflagged row found!  This is a major problem/bug" << LogIO::WARN;
    return 0.0;
  }
  //
  //---------------------------------------------------------------------
  // Compute the Parallactic Angle for the give VisBuffer
  //
  void getHADec(MeasurementSet& ms, const VisBuffer2& vb, 
		Double& HA, Double& RA, Double& Dec)
  {
    MEpoch last;
    Double time = getCurrentTimeStamp(vb);
    
    Unit Second("s"), Day("d");
    MEpoch epoch(Quantity(time,Second),MEpoch::TAI);
    MPosition pos;
    MSObservationColumns msoc(ms.observation());
    String ObsName=msoc.telescopeName()(vb.arrayId()(0));
    
    if (!MeasTable::Observatory(pos,ObsName))
      throw(AipsError("Observatory position for "+ ObsName + " not found"));
    
    MeasFrame frame(epoch,pos);
    MEpoch::Convert toLAST = MEpoch::Convert(MEpoch::Ref(MEpoch::TAI,frame),
					     MEpoch::Ref(MEpoch::LAST,frame));
    RA=vb.direction1()(0).getAngle().getValue()(0);    
    if (RA < 0.0) RA += M_PI*2.0;
    Dec=vb.direction1()(0).getAngle().getValue()(1);    

    last = toLAST(epoch);
    Double LST   = last.get(Day).getValue();
    LST -= floor(LST); // Extract the fractional day
    LST *= 2*C::pi;// Convert to Raidan

    cout << "LST = " << LST << " " << LST/(2*C::pi) << endl;
    
    HA = LST - RA;
  }
  //
  //---------------------------------------------------------------------
  // Compute the Parallactic Angle for the give VisBuffer
  //
  Double getPA(const vi::VisBuffer2& vb)
  {
    return (Double)(vb.feedPa(getCurrentTimeStamp(vb))(0));
    // Double pa=0;
    // Int n=0;
    // Vector<Float> antPA = vb.feed_pa(getCurrentTimeStamp(vb));
    // for (uInt i=0;i<antPA.nelements();i++)
    //   {
    // 	if (!vb.msColumns().antenna().flagRow()(i))
    // 	  {pa += antPA(i); n++;break;}
    //   }
    // //    pa = sum(antPA)/(antPA.nelements()-1);
    // if (n==0) 
    //   throw(AipsError("No unflagged antenna found in getPA()."));
    // return pa/n;
  }
  //
  //---------------------------------------------------------------------
  //
  // Make stokes axis, given the polarization types.
  //
  void makeStokesAxis(Int npol_p, Vector<String>& polType, Vector<Int>& whichStokes)
  {
    //    Vector<String> polType=msc.feed().polarizationType()(0);
    StokesImageUtil::PolRep polRep_p;
    LogIO os(LogOrigin("Utils", "makeStokesAxis", WHERE));

    if (polType(0)!="X" && polType(0)!="Y" &&
	polType(0)!="R" && polType(0)!="L") 
      {
	os << "Warning: Unknown stokes types in feed table: ["
	   << polType(0) << ", " << polType(1) << "]" << endl
	   << "Results open to question!" << LogIO::POST;
      }
  
    if (polType(0)=="X" || polType(0)=="Y") 
      {
	polRep_p=StokesImageUtil::LINEAR;
	os << "Preferred polarization representation is linear" << LogIO::POST;
      }
    else 
      {
	polRep_p=StokesImageUtil::CIRCULAR;
	os << "Preferred polarization representation is circular" << LogIO::POST;
      }

    //    Vector<Int> whichStokes(npol_p);
    switch(npol_p) 
      {
      case 1:
	whichStokes.resize(1);
	whichStokes(0)=Stokes::I;
	os <<  "Image polarization = Stokes I" << LogIO::POST;
	break;
      case 2:
	whichStokes.resize(2);
	whichStokes(0)=Stokes::I;
	if (polRep_p==StokesImageUtil::LINEAR) 
	  {
	    whichStokes(1)=Stokes::Q;
	    os <<  "Image polarization = Stokes I,Q" << LogIO::POST;
	  }
      else 
	{
	  whichStokes(1)=Stokes::V;
	  os <<  "Image polarization = Stokes I,V" << LogIO::POST;
	}
	break;
      case 3:
	whichStokes.resize(3);
	whichStokes(0)=Stokes::I;
	whichStokes(1)=Stokes::Q;
	whichStokes(1)=Stokes::U;
	os <<  "Image polarization = Stokes I,Q,U" << LogIO::POST;
	break;
      case 4:
	whichStokes.resize(4);
	whichStokes(0)=Stokes::I;
	whichStokes(1)=Stokes::Q;
	whichStokes(2)=Stokes::U;
	whichStokes(3)=Stokes::V;
	os <<  "Image polarization = Stokes I,Q,U,V" << LogIO::POST;
	break;
      default:
	os << LogIO::SEVERE << "Illegal number of Stokes parameters: " << npol_p
	   << LogIO::POST;
      };
  }
  //
  //--------------------------------------------------------------------------------------------
  //  
  Bool isVBNaN(const VisBuffer2 &vb,String& mesg)
  {
    IPosition ndx(3,0);
    Int N = vb.nRows();
    for(ndx(2)=0;ndx(2)<N;ndx(2)++)
      if (std::isnan(vb.visCubeModel()(ndx).real()) ||
	  std::isnan(vb.visCubeModel()(ndx).imag())
	  )
	{
	  ostringstream os;
	  os << ndx(2) << " " << vb.antenna1()(ndx(2)) << "-" << vb.antenna2()(ndx(2)) << " "
	     << vb.flagCube()(ndx) << " " 
	    //<< vb.flag()(0,ndx(2)) << " " 
	     << vb.weight();
	  mesg = os.str().c_str();
	  return true;
	}
    return false;
  }
  //
  //--------------------------------------------------------------------------------------------
  //  
  /////////////////////////////////////////////////////////////////////////////
  //
  // IChangeDetector  - an interface class to detect changes in the VisBuffer
  //     Exact meaning of the "change" is defined in the derived classes
  //     (e.g. a change in the parallactic angle)
  
  // return true if a change occurs somewhere in the buffer
  using namespace vi;
  Bool IChangeDetector::changed(const VisBuffer2 &vb) const
  {
     for (rownr_t i=0;i<vb.nRows();++i)
          if (changed(vb,i)) return true;
     return false;
  }

  // return true if a change occurs somewhere in the buffer starting from row1
  // up to row2 (row2=-1 means up to the end of the buffer). The row number,
  // where the change occurs is returned in the row2 parameter
  Bool IChangeDetector::changedBuffer(const VisBuffer2 &vb, Int row1, 
		   Int &row2) const
  {
    if (row1<0) row1=0;
    Int jrow = row2;
    if (jrow < 0) jrow = vb.nRows()-1;
    DebugAssert(jrow<vb.nRows(),AipsError);
    
    // It is not important now to have a separate function for a "block"
    // operation. Because an appropriate caching is implemented inside
    // VisBuffer, changed(vb,row) can be called in the
    // loop without a perfomance penalty. This method returns the 
    // first row where the change occured rather than the last unchanged 
    // row as it was for BeamSkyJones::changedBuffer
      
    for (Int ii=row1;ii<=jrow;++ii)
         if (changed(vb,ii)) {
             row2 = ii;
             return true;
         }
    return false;
  }
  
  // a virtual destructor to make the compiler happy
  IChangeDetector::~IChangeDetector() {}
  
  //
  /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  //
  // ParAngleChangeDetector - a class to detect a change in the parallatic 
  //                          angle
  //
  
  // set up the tolerance, which determines how much the position angle should
  // change to report the change by this class
  ParAngleChangeDetector::ParAngleChangeDetector(const Quantity &pa_tolerance) 
               : pa_tolerance_p(pa_tolerance.getValue("rad")),
		    last_pa_p(1000.) {}  // 1000 is >> 2pi, so it is changed
                                         // after construction
  
  // Set the value of the PA tolerance
  void ParAngleChangeDetector::setTolerance(const Quantity &pa_tolerance)
  {
    pa_tolerance_p = (pa_tolerance.getValue("rad"));
  }
  // reset to the state which exist just after construction
  void ParAngleChangeDetector::reset()
  {
      last_pa_p=1000.; // it is >> 2pi, which would force a changed state
  }
     
  // return parallactic angle tolerance
  Quantity ParAngleChangeDetector::getParAngleTolerance() const
  {
      return Quantity(pa_tolerance_p,"rad");
  }
  
  // return true if a change occurs in the given row since the last call 
  // of update
  Bool ParAngleChangeDetector::changed(const VisBuffer2 &vb, Int row) const
  {
     if (row<0) row=0;
     //     const Double feed1_pa=vb.feed1_pa()[row];
     Double feed1_pa=getPA(vb);
     if (abs(feed1_pa-last_pa_p) > pa_tolerance_p) 
       {
//  	 cout << "Utils: " << feed1_pa*57.295 << " " << last_pa_p*57.295 << " " << abs(feed1_pa-last_pa_p)*57.295 << " " << ttt*57.295 << " " << vb.time()(0)-4.51738e+09 << endl;
	 return true;
       }
     //     const Double feed2_pa=vb.feed2_pa()[row];
     Double feed2_pa = getPA(vb);
     if (abs(feed2_pa-last_pa_p) > pa_tolerance_p) 
       {
//  	 cout << "Utils: " << feed2_pa*57.295 << " " 
//  	      << last_pa_p*57.295 << " " 
//  	      << abs(feed2_pa-last_pa_p)*57.295 << " " << ttt*57.295 << vb.time()(0)-4.51738e+09 <<endl;
	 return true;
       }
     return false;
  }
  
  // start looking for a change from the given row of the VisBuffer
  void ParAngleChangeDetector::update(const VisBuffer2 &vb, Int row) 
  {
     if (row<0) row=0;
     const Double feed1_pa=vb.feedPa1()[row];
     const Double feed2_pa=vb.feedPa2()[row];
     if (abs(feed1_pa-feed2_pa)>pa_tolerance_p) {
	 LogIO os;
	 os<<LogIO::WARN << LogOrigin("ParAngleChangeDetector","update") 
           <<"The parallactic angle is different at different stations"
	   <<LogIO::POST<<LogIO::WARN <<LogOrigin("ParAngleChangeDetector","update")
	   <<"The result may be incorrect. Continuing anyway."<<LogIO::POST;
     }
     last_pa_p=(feed1_pa+feed2_pa)/2.;
  }

  Int getPhaseCenter(MeasurementSet& ms, MDirection& dir0, Int whichField)
  {
    MSFieldColumns msfc(ms.field());
    if (whichField < 0)
      {
	MSRange msr(ms);
	//
	// An array of shape [2,1,1]!
	//
	Vector<Int> fieldId;
	fieldId = msr.range(MSS::FIELD_ID).asArrayInt(RecordFieldId(0));
	
	Array<Double> phaseDir = msfc.phaseDir().getColumn();
	
	if (fieldId.nelements() <= 0)
	  throw(AipsError("getPhaseCenter: No fields found in the selected MS."));
	
	IPosition ndx0(3,0,0,0),ndx1(3,1,0,0);
	
	Double maxRA, maxDec, RA,Dec,RA0,Dec0, dist0;
	maxRA = maxDec=0;
	for(uInt i=0;i<fieldId.nelements();i++)
	  {
	    RA   = phaseDir(IPosition(3,0,0,fieldId(i)));
	    Dec  = phaseDir(IPosition(3,1,0,fieldId(i)));
	    maxRA += RA; maxDec += Dec;
	  }
	RA0=maxRA/fieldId.nelements(); Dec0=maxDec/fieldId.nelements();
	
	dist0=10000;
	Int field0=0;
	for(uInt i=0;i<fieldId.nelements();i++)
	  {
	    RA   = RA0-phaseDir(IPosition(3,0,0,fieldId(i)));
	    Dec  = Dec0-phaseDir(IPosition(3,1,0,fieldId(i)));
	    Double dist=sqrt(RA*RA + Dec*Dec);
	    if (dist < dist0) {field0=fieldId(i);dist0=dist;};
	  }
	dir0=msfc.phaseDirMeasCol()(field0)(IPosition(1,0));
	//dir0=msfc.phaseDirMeasCol()(6)(IPosition(1,0));
	return field0;
      }
    else
      {
	dir0=msfc.phaseDirMeasCol()(whichField)(IPosition(1,0));
	return whichField;
      }
  }
  //
  //------------------------------------------------------------------
  //
  Bool findMaxAbsLattice(const ImageInterface<Float>& lattice,
			 Float& maxAbs,IPosition& posMaxAbs) 
  {
    posMaxAbs = IPosition(lattice.shape().nelements(), 0);
    maxAbs=0.0;

    const IPosition tileShape = lattice.niceCursorShape();
    TiledLineStepper ls(lattice.shape(), tileShape, 0);
    {
      RO_LatticeIterator<Float> li(lattice, ls);
      for(li.reset();!li.atEnd();li++)
	{
	  IPosition posMax=li.position();
	  IPosition posMin=li.position();
	  Float maxVal=0.0;
	  Float minVal=0.0;
	  
	  minMax(minVal, maxVal, posMin, posMax, li.cursor());
	  if((maxVal)>(maxAbs)) 
	    {
	      maxAbs=maxVal;
	      posMaxAbs=li.position();
	      posMaxAbs(0)=posMax(0);
	    }
	}
    }

    return true;
  };
  //
  //------------------------------------------------------------------
  //
  Bool findMaxAbsLattice(const ImageInterface<Float>& masklat,
			 const Lattice<Float>& lattice,
			 Float& maxAbs,IPosition& posMaxAbs, 
			 Bool flip)
  {
    
    AlwaysAssert(masklat.shape()==lattice.shape(), AipsError);

    Array<Float> msk;
  
    posMaxAbs = IPosition(lattice.shape().nelements(), 0);
    maxAbs=0.0;
    //maxAbs=-1.0e+10;
    const IPosition tileShape = lattice.niceCursorShape();
    TiledLineStepper ls(lattice.shape(), tileShape, 0);
    TiledLineStepper lsm(masklat.shape(), tileShape, 0);
    {
      RO_LatticeIterator<Float> li(lattice, ls);
      RO_LatticeIterator<Float> lim(masklat, lsm);
      for(li.reset(),lim.reset();!li.atEnd();li++,lim++) 
	{
	  IPosition posMax=li.position();
	  IPosition posMin=li.position();
	  Float maxVal=0.0;
	  Float minVal=0.0;
	  
	  msk = lim.cursor();
	  if(flip) msk = (Float)1.0 - msk;
	  
	  //minMaxMasked(minVal, maxVal, posMin, posMax, li.cursor(),lim.cursor());
	  minMaxMasked(minVal, maxVal, posMin, posMax, li.cursor(),msk);
	  
	  
	  if((maxVal)>(maxAbs)) 
	    {
	      maxAbs=maxVal;
	      posMaxAbs=li.position();
	      posMaxAbs(0)=posMax(0);
	    }
	}
    }

    return true;
  }
  //
  //---------------------------------------------------------------
  //Rotate a complex array using a the given coordinate system and the
  //angle in radians.  Default interpolation method is "CUBIC".
  //Axeses corresponding to Linear coordinates in the given
  //CoordinateSystem object are rotated.  Rotation is done using
  //ImageRegrid object, about the pixel given by (N+1)/2 where N is
  //the number of pixels along the axis.
  //
  void SynthesisUtils::rotateComplexArray(LogIO& logio, Array<Complex>& inArray, 
					  CoordinateSystem& inCS,
					  Array<Complex>& outArray,
					  Double dAngleRad,
					  String interpMethod,
					  Bool modifyInCS)
  {
//     logio << LogOrigin("SynthesisUtils", "rotateComplexArray")
// 	  << "Rotating CF using " << interpMethod << " interpolation." 
// 	  << LogIO::POST;
    (void)logio;
    //
    // If no rotation required, just copy the inArray to outArray.
    //
    //    if (abs(dAngleRad) < 0.1)
    
    // IPosition tt;
    // inCS.list(logio,MDoppler::RADIO,tt,tt);

    if (abs(dAngleRad) == 0.0)
      {
	outArray.reference(inArray);
	//	outArray.assign(inArray);
	return;
      }
    //
    // Re-grid inImage onto outImage
    //
    Vector<Int> pixelAxes;
    Int coordInd = -1;
    // Extract LINRAR coords from inCS.
    // Extract axes2

    if(modifyInCS){
      Vector<Double> refPix = inCS.referencePixel();
      refPix(0) = (Int)((inArray.shape()(0))/2.0);
      refPix(1) = (Int)((inArray.shape()(1))/2.0);
      inCS.setReferencePixel(refPix);
    }

    coordInd = inCS.findCoordinate(Coordinate::LINEAR);
    Bool haveLinear = true;

    if(coordInd == -1){ // no linear coordinate found, look for DIRECTION instead
      coordInd = inCS.findCoordinate(Coordinate::DIRECTION);
      haveLinear = false;
    }

    pixelAxes=inCS.pixelAxes(coordInd);
    IPosition axes2(pixelAxes);
    // Set linear transformation matrix in inCS.
//     CoordinateSystem outCS =
//       ImageRegrid<Complex>::makeCoordinateSystem (logio, outCS, inCS, axes2);

    CoordinateSystem outCS(inCS);

    Matrix<Double> xf = outCS.coordinate(coordInd).linearTransform();
    Matrix<Double> rotm(2,2);
    rotm(0,0) = cos(dAngleRad); rotm(0,1) = sin(dAngleRad);
    rotm(1,0) = -rotm(0,1);     rotm(1,1) = rotm(0,0);

    // Double s = sin(dAngleRad);
    // Double c = cos(dAngleRad);
    // rotm(0,0) =  c; rotm(0,1) = s;
    // rotm(1,0) = -s; rotm(1,1) = c;

    // Create new linear transform matrix
    Matrix<Double> xform(2,2);
    xform(0,0) = rotm(0,0)*xf(0,0)+rotm(0,1)*xf(1,0);
    xform(0,1) = rotm(0,0)*xf(0,1)+rotm(0,1)*xf(1,1);
    xform(1,0) = rotm(1,0)*xf(0,0)+rotm(1,1)*xf(1,0);
    xform(1,1) = rotm(1,0)*xf(0,1)+rotm(1,1)*xf(1,1);

    if(haveLinear){
      LinearCoordinate linCoords = outCS.linearCoordinate(coordInd);
      linCoords.setLinearTransform(xform);
      outCS.replaceCoordinate(linCoords, coordInd);
    }
    else{
      DirectionCoordinate dirCoords = outCS.directionCoordinate(coordInd);
      dirCoords.setLinearTransform(xform);
      outCS.replaceCoordinate(dirCoords, coordInd);
    }      
    
    outArray.resize(inArray.shape());
    outArray.set(0);
    //
    // Make an image out of inArray and inCS --> inImage
    //
    //    TempImage<Complex> inImage(inArray.shape(), inCS);
    {
      TempImage<Float> inImage(inArray.shape(),inCS);
      TempImage<Float> outImage(outArray.shape(), outCS);
      ImageRegrid<Float> ir;
      Interpolate2D::Method interpolationMethod = Interpolate2D::stringToMethod(interpMethod);
      //------------------------------------------------------------------------
      // Rotated the real part
      //
      inImage.copyData(LatticeExpr<Float>(real(ArrayLattice<Complex>(inArray))));
      outImage.set(0.0);

      ir.regrid(outImage, interpolationMethod, axes2, inImage);
      setReal(outArray,outImage.get());
      //------------------------------------------------------------------------
      // Rotated the imaginary part
      //
      inImage.copyData(LatticeExpr<Float>(imag(ArrayLattice<Complex>(inArray))));
      outImage.set(0.0);

      ir.regrid(outImage, interpolationMethod, axes2, inImage);
      setImag(outArray,outImage.get());
    }
  }
  //
  //---------------------------------------------------------------
  //
  void SynthesisUtils::findLatticeMax(const ImageInterface<Complex>& lattice,
				      Vector<Float>& maxAbs,
				      Vector<IPosition>& posMaxAbs) 
  {
    IPosition lshape(lattice.shape());
    IPosition ndx(lshape);
    Int nPol=lshape(2);
    posMaxAbs.resize(nPol);
    for(Int i=0;i<nPol;i++)
      posMaxAbs(i)=IPosition(lattice.shape().nelements(), 0);
    maxAbs.resize(nPol);
    ndx=0;
    
    for(Int s2=0;s2<lshape(2);s2++)
      for(Int s3=0;s3<lshape(3);s3++)
	{
	  ndx(2) = s2; ndx(3)=s3;
	  {
	    //
	    // Locate the pixel with the peak value.  That's the
	    // origin in pixel co-ordinates.
	    //
	    maxAbs(s2)=0;
	    posMaxAbs(s2) = 0;
	    for(ndx(1)=0;ndx(1)<lshape(1);ndx(1)++)
	      for(ndx(0)=0;ndx(0)<lshape(0);ndx(0)++)
		if (abs(lattice(ndx)) > maxAbs(s2)) 
		  {posMaxAbs(s2) = ndx;maxAbs(s2)=abs(lattice(ndx));}
	  }
	}
  }
  //
  //---------------------------------------------------------------
  //
  void SynthesisUtils::findLatticeMax(const Array<Complex>& lattice,
				      Vector<Float>& maxAbs,
				      Vector<IPosition>& posMaxAbs) 
  {
    IPosition lshape(lattice.shape());
    IPosition ndx(lshape);
    Int nPol=lshape(2);
    posMaxAbs.resize(nPol);
    for(Int i=0;i<nPol;i++)
      posMaxAbs(i)=IPosition(lattice.shape().nelements(), 0);
    maxAbs.resize(nPol);
    ndx=0;
    
    for(Int s2=0;s2<lshape(2);s2++)
      for(Int s3=0;s3<lshape(3);s3++)
	{
	  ndx(2) = s2; ndx(3)=s3;
	  {
	    //
	    // Locate the pixel with the peak value.  That's the
	    // origin in pixel co-ordinates.
	    //
	    maxAbs(s2)=0;
	    posMaxAbs(s2) = 0;
	    for(ndx(1)=0;ndx(1)<lshape(1);ndx(1)++)
	      for(ndx(0)=0;ndx(0)<lshape(0);ndx(0)++)
		if (abs(lattice(ndx)) > maxAbs(s2)) 
		  {posMaxAbs(s2) = ndx;maxAbs(s2)=abs(lattice(ndx));}
	  }
	}
  }
  //
  //---------------------------------------------------------------
  //
  void SynthesisUtils::findLatticeMax(const ImageInterface<Float>& lattice,
				      Vector<Float>& maxAbs,
				      Vector<IPosition>& posMaxAbs) 
  {
    IPosition lshape(lattice.shape());
    IPosition ndx(lshape);
    Int nPol=lshape(2);
    posMaxAbs.resize(nPol);
    for(Int i=0;i<nPol;i++)
      posMaxAbs(i)=IPosition(lattice.shape().nelements(), 0);
    maxAbs.resize(nPol);
    ndx=0;
    
    for(Int s2=0;s2<lshape(2);s2++)
      for(Int s3=0;s3<lshape(3);s3++)
	{
	  ndx(2) = s2; ndx(3)=s3;
	  {
	    //
	    // Locate the pixel with the peak value.  That's the
	    // origin in pixel co-ordinates.
	    //
	    maxAbs(s2)=0;
	    posMaxAbs(s2) = 0;
	    for(ndx(1)=0;ndx(1)<lshape(1);ndx(1)++)
	      for(ndx(0)=0;ndx(0)<lshape(0);ndx(0)++)
		if (abs(lattice(ndx)) > maxAbs(s2)) 
		  {posMaxAbs(s2) = ndx;maxAbs(s2)=abs(lattice(ndx));}
	  }
	}
  }
  //
  //---------------------------------------------------------------
  // Get the value of the named variable from ~/.aipsrc (or ~/.casarc)
  // or from a env. variable (in this precidence order).
  //
  template <class T>
    T SynthesisUtils::getenv(const char *name, const T defaultVal)
    {
      T val=defaultVal;
      stringstream defaultStr;
      defaultStr << defaultVal;
      {
	char *valStr=NULL;
	std::string tt(name);
	unsigned long pos;
	while((pos=tt.find(".")) != tt.npos)
	  tt.replace(pos, 1, "_");

	if ((valStr = std::getenv(tt.c_str())) != NULL)
	  {
	    stringstream toT2(valStr);
	    toT2 >> val;
	  }
      }
      // If environment variable was not found (val remained set to the
      // defaulVal), look in ~/.aipsrc.
      if (val==defaultVal)
	{
	  uint handle = Aipsrc::registerRC(name, defaultStr.str().c_str());    
	  String strVal = Aipsrc::get(handle);
	  stringstream toT(strVal);
	  toT >> val;
	}
      return val;
    }
    template 
    Int SynthesisUtils::getenv(const char *name, const Int defaultVal);
    template 
    Bool SynthesisUtils::getenv(const char *name, const Bool defaultVal);
    template 
    Float SynthesisUtils::getenv(const char *name, const Float defaultVal);
    template 
    double SynthesisUtils::getenv(const char *name, const double defaultVal);

  Float SynthesisUtils::libreSpheroidal(Float nu) 
  {
    Double top, bot, nuend, delnusq;
    uInt part;
    if (nu <= 0) return 1.0;
    else 
      if (nu >= 1.0) 
	return 0.0;
      else 
	{
	  uInt np = 5;
	  uInt nq = 3;
	  Matrix<Double> p(np, 2);
	  Matrix<Double> q(nq, 2);
	  p(0,0) = 8.203343e-2;
	  p(1,0) = -3.644705e-1;
	  p(2,0) =  6.278660e-1;
	  p(3,0) = -5.335581e-1; 
	  p(4,0) =  2.312756e-1;
	  p(0,1) =  4.028559e-3;
	  p(1,1) = -3.697768e-2; 
	  p(2,1) = 1.021332e-1;
	  p(3,1) = -1.201436e-1;
	  p(4,1) = 6.412774e-2;
	  q(0,0) = 1.0000000e0;
	  q(1,0) = 8.212018e-1;
	  q(2,0) = 2.078043e-1;
	  q(0,1) = 1.0000000e0;
	  q(1,1) = 9.599102e-1;
	  q(2,1) = 2.918724e-1;

	  part = 0;
	  nuend = 0.0;
	  if ((nu >= 0.0) && (nu < 0.75)) 
	    {
	      part = 0;
	      nuend = 0.75;
	    } 
	  else if ((nu >= 0.75) && (nu <= 1.00)) 
	    {
	      part = 1;
	      nuend = 1.0;
	    }
	  
	  top = p(0,part);
	  delnusq = pow(nu,2.0) - pow(nuend,2.0);
	  for (uInt k=1; k<np; k++) 
	    top += p(k, part) * pow(delnusq, (Double)k);

	  bot = q(0, part);
	  for (uInt k=1; k<nq; k++) 
	    bot += q(k,part) * pow(delnusq, (Double)k);
	  
	  if (bot != 0.0) return (top/bot);
	  else            return 0.0;
	}
  }
    Double SynthesisUtils::getRefFreq(const VisBuffer2& /*vb*/)
  {
    throw(AipsError("SynthesisUtils::getRefFreq() depricated due to VI2/VB2 move"));
    // return max((vb.getVi()->ms())//vb.msColumns()
    // 	      .spectralWindow().chanFreq().getColumn());
  }
  
  void SynthesisUtils::makeFTCoordSys(const CoordinateSystem& coords,
				      const Int& convSize,
				      const Vector<Double>& ftRef,
				      CoordinateSystem& ftCoords)
  {
    Int directionIndex;

    ftCoords = coords;
    directionIndex=ftCoords.findCoordinate(Coordinate::DIRECTION);
    //  The following line follows the (lame) logic that if a
    //  DIRECTION axis was not found, the coordinate system must be of
    //  the FT domain already
    if (directionIndex == -1) return;

    DirectionCoordinate dc;//=coords.directionCoordinate(directionIndex);
    //	AlwaysAssert(directionIndex>=0, AipsError);
    dc=coords.directionCoordinate(directionIndex);
    Vector<Bool> axes(2); axes(0)=axes(1)=true;//axes(2)=true;
    Vector<Int> shape(2,convSize);

    Vector<Double>ref(4);
    ref(0)=ref(1)=ref(2)=ref(3)=0;
    dc.setReferencePixel(ref);
    Coordinate* ftdc=dc.makeFourierCoordinate(axes,shape);
    Vector<Double> refVal;
    refVal=ftdc->referenceValue();
    //    refVal(0)=refVal(1)=0;
    //    ftdc->setReferenceValue(refVal);
    ref(0)=ftRef(0);
    ref(1)=ftRef(1);
    ftdc->setReferencePixel(ref);

    ftCoords.replaceCoordinate(*ftdc, directionIndex);
    // LogIO logio;
    // IPosition tt;
    // coords.list(logio,MDoppler::RADIO,tt,tt);
    // ftCoords.list(logio,MDoppler::RADIO,tt,tt);

    delete ftdc; ftdc=0;
  }

  //
  // Given a list of Spw,MinFreq,MaxFreq,FreqStep (the output product
  // of MSSelection), expand the list to a list of channel freqs. and
  // conjugate freqs. per SPW.
  //
  void SynthesisUtils::expandFreqSelection(const Matrix<Double>& freqSelection, 
					   Matrix<Double>& expandedFreqList,
					   Matrix<Double>& expandedConjFreqList)
  {
    Int nSpw = freqSelection.shape()(0), maxSlots=0;
    Double freq;

    for (Int s=0;s<nSpw;s++)
	maxSlots=max(maxSlots,SynthesisUtils::nint((freqSelection(s,2)-freqSelection(s,1))/freqSelection(s,3))+1);

    expandedFreqList.resize(nSpw,maxSlots);
    expandedConjFreqList.resize(nSpw,maxSlots);

    for (Int s=0,cs=(nSpw-1);s<nSpw;s++,cs--)
      for (Int i=0,ci=(maxSlots-1);i<maxSlots;i++,ci--)
	{
	  freq = freqSelection(s,1)+i*freqSelection(s,3);
	  expandedFreqList(s,i) = (freq <= freqSelection(s,2)) ? freq : 0;
	  freq = freqSelection(cs,2) - ci*freqSelection(cs,3);
	  expandedConjFreqList(s,ci) = (freq >= freqSelection(cs,1)) ? freq : 0;
	}
  }

  //
  // The result will be in-place in c1
  //
  template
  void SynthesisUtils::libreConvolver(Array<Complex>& c1, const Array<Complex>& c2);
  

  template <class T>
  void SynthesisUtils::libreConvolver(Array<T>& c1, const Array<T>& c2)
  {
    Array<T> c2tmp;
    c2tmp.assign(c2);

    if (c1.shape().product() > c2tmp.shape().product())
      c2tmp.resize(c1.shape(),true);
    else
      c1.resize(c2tmp.shape(),true);


    ArrayLattice<T> c2tmp_lat(c2tmp), c1_lat(c1);

    LatticeFFT::cfft2d(c1_lat,false);
    LatticeFFT::cfft2d(c2tmp_lat,false);
    //cerr << "########## " << c1.shape() << " " << c2tmp.shape() << endl;
    c1 = sqrt(c1);
    c2tmp=sqrt(c2tmp);
    c1 *= conj(c2tmp);
    LatticeFFT::cfft2d(c1_lat);
  }


  Double SynthesisUtils::nearestValue(const Vector<Double>& list, const Double& val, Int& index)
  {
    Vector<Double> diff = fabs(list - val);
    Double minVal=1e20; 
    Int i=0;

    for (index=0;index<(Int)diff.nelements();index++)
      if (diff[index] < minVal) {minVal=diff[index];i=index;}
    index=i;
    return list(index);

    // The algorithm below has a N*log(N) cost.
    //
    // Bool dummy;
    // Sort sorter(diff.getStorage(dummy), sizeof(Double));
    // sorter.sortKey((uInt)0,TpDouble);
    // Int nch=list.nelements();
    // Vector<uInt> sortIndx;
    // sorter.sort(sortIndx, nch);
    
    // index=sortIndx(0);
    // return list(index);


    // Works only for regularly samplied list
    //
    // Int ndx=min(freqValues_p.nelements()-1,max(0,SynthesisUtils::nint((freqVal-freqValues_p[0])/freqValIncr_p)));
    // return ndx;
  }

  template <class T>
  T SynthesisUtils::stdNearestValue(const vector<T>& list, const T& val, Int& index)
  {
    vector<T> diff=list;
    for (uInt i=0;i<list.size();i++)
      diff[i] = fabs(list[i] - val);
    
    T minVal=std::numeric_limits<T>::max();//1e20; 
    Int i=0;

    for (index=0;index<(Int)diff.size();index++)
      if (diff[index] < minVal) {minVal=diff[index];i=index;}
    index=i;
    return list[index];
  }

  CoordinateSystem SynthesisUtils::makeUVCoords(CoordinateSystem& imageCoordSys,
						IPosition& shape)
  {
    CoordinateSystem FTCoords = imageCoordSys;
    
    Int dirIndex=FTCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=imageCoordSys.directionCoordinate(dirIndex);
    Vector<Bool> axes(2); axes=true;
    Vector<Int> dirShape(2); dirShape(0)=shape(0);dirShape(1)=shape(1);
    Coordinate* FTdc=dc.makeFourierCoordinate(axes,dirShape);
    FTCoords.replaceCoordinate(*FTdc,dirIndex);
    delete FTdc;
    
    return FTCoords;
  }

  Vector<Int> SynthesisUtils::mapSpwIDToDDID(const VisBuffer2& vb, const Int& spwID)
  {
    Vector<Int> ddidList;
    //Int nDDRows = vb.msColumns().dataDescription().nrow();
    Int nDDRows = (vb.ms()).dataDescription().nrow();
    for (Int i=0; i<nDDRows; i++)
      {
	if((vb.subtableColumns()).dataDescription().spectralWindowId().get(i) == spwID)
	  {
	    Int n=ddidList.nelements();
	    ddidList.resize(n+1,true);
	    ddidList(n) = i;
	  }
      }
    return ddidList;
  }

  Vector<Int> SynthesisUtils::mapSpwIDToPolID(const VisBuffer2& vb, const Int& spwID)
  {
    Vector<Int> polIDList, ddIDList;
    ddIDList = SynthesisUtils::mapSpwIDToDDID(vb, spwID);
    Int n=ddIDList.nelements();
    polIDList.resize(n);
    for (Int i=0; i<n; i++)
      polIDList(i) = (vb.subtableColumns()).dataDescription().polarizationId().get(ddIDList(i));
      
    return polIDList;
  }


  // 
  // Calcualte the BLC, TRC of the intersection of two rectangles (courtesy U.Rau)
  //
  void SynthesisUtils::calcIntersection(const Int blc1[2], const Int trc1[2], 
					const Float blc2[2], const Float trc2[2],
					Float blc[2], Float trc[2])
  {
    //    cerr << blc1[0] << " " << blc1[1] << " " << trc1[0] << " " << trc1[1] << " " << blc2[0] << " " << blc2[1] << " " << trc2[0] << " " << trc2[1] << endl;
    Float dblc, dtrc;
    for (Int i=0;i<2;i++)
      {
        dblc = blc2[i] - blc1[i];
        dtrc = trc2[i] - trc1[i];

        if ((dblc >= 0) and (dtrc >= 0))
	  {
            blc[i] = blc1[i] + dblc;
            trc[i] = trc2[i] - dtrc;
	  }
        else if ((dblc >= 0) and (dtrc < 0))
	  {
            blc[i] = blc1[i] + dblc;
            trc[i] = trc1[i] + dtrc;
	  }
        else if ((dblc < 0) and (dtrc >= 0))
	  {
            blc[i] = blc2[i] - dblc;
            trc[i] = trc2[i] - dtrc;
	  }
        else
	  {
            blc[i] = blc2[i] - dblc;
            trc[i] = trc1[i] + dtrc;
	  }
      }
  }
  //
  // Check if the two rectangles interset (courtesy U.Rau).
  //
  Bool SynthesisUtils::checkIntersection(const Int blc1[2], const Int trc1[2], const Float blc2[2], const Float trc2[2])
  {
    // blc1[2] = {xmin1, ymin1}; 
    // blc2[2] = {xmin2, ymin2};
    // trc1[2] = {xmax1, ymax1};
    // trc2[2] = {xmax2, ymax2};

    if ((blc1[0] > trc2[0]) || (trc1[0] < blc2[0]) || (blc1[1] > trc2[1]) || (trc1[1] < blc2[1])) 
      return false;
    else
      return true;
// def checkintersect(  xmin1, ymin1, xmax1, ymax1,   xmin2, ymin2, xmax2, ymax2 ):
//     if  xmin1 > xmax2  or xmax1 < xmin2 or ymin1 > ymax2 or ymax1 < ymin2 :
//         return false
//     else : 
//         return true
  }

  // template<class Iterator>
  // Iterator SynthesisUtils::Unique(Iterator first, Iterator last)
  // {
  //   while (first != last)
  //     {
  //       Iterator next(first);
  //       last = std::remove(++next, last, *first);
  //       first = next;
  //     }
    
  //   return last;
  // }

  String SynthesisUtils::mjdToString(casacore::Time& time)
  {
    String tStr;
    tStr = String::toString(time.year()) + "/" +
      String::toString(time.month()) + "/" +
      String::toString(time.dayOfMonth()) + "/" +
      String::toString(time.hours()) + ":" +
      String::toString(time.minutes()) + ":";
    ostringstream fsec;
    fsec << setprecision(2) << time.dseconds();
    tStr = tStr + String(fsec.str());
    //      String::toString(time.dseconds());
    return tStr;
  }

  template <class Iterator>
  Iterator SynthesisUtils::Unique(Iterator first, Iterator last)
  {
    while (first != last)
      {
	Iterator next(first);
        last = std::remove(++next, last, *first);
        first = next;
      }
    
    return last;
  }

  void SynthesisUtils::showCS(const CoordinateSystem& cs, std::ostream &os, const String& msg)
  {
    LogIO log_l(LogOrigin("SynthesisUtils","showCS"));
    IPosition dummy;
    Vector<String> csList;
    if (msg!="")
      os << "CoordSys: ";
    csList = cs.list(log_l,MDoppler::RADIO,dummy,dummy);
    os << csList << std::endl;
  }
 const casacore::Array<Complex> SynthesisUtils::getCFPixels(const casacore::String& Dir,
							    const casacore::String& fileName)
  {
    try
      {
	casacore::PagedImage<casacore::Complex> thisCF(Dir+'/'+fileName);
	return thisCF.get();
      }
    catch (AipsError &x)
      {
	LogIO log_l(LogOrigin("SynthesisUtils","getCFPixels"));
	log_l << x.getMesg() << LogIO::EXCEPTION;
      }
    return casacore::Array<Complex>(); // Just to keep the complier happy.  Program control should never get here.
  }
  
 const casacore::IPosition SynthesisUtils::getCFShape(const casacore::String& Dir,
						      const casacore::String& fileName)
  {
    try
      {
	casacore::PagedImage<casacore::Complex> thisCF(Dir+'/'+fileName);
	return thisCF.shape();
      }
    catch (AipsError &x)
      {
	LogIO log_l(LogOrigin("SynthesisUtils","getCFShape"));
	log_l << x.getMesg() << LogIO::EXCEPTION;
      }
    return casacore::IPosition(); // Just to keep the complier happy.  Program control should never get here.
  }
  
  casacore::TableRecord SynthesisUtils::getCFParams(const casacore::String& Dir,
						    const casacore::String& fileName,
						    casacore::Array<Complex>& pixelBuffer,
						    casacore::CoordinateSystem& coordSys, 
						    casacore::Double& sampling,
						    casacore::Double& paVal,
						    casacore::Int& xSupport, casacore::Int& ySupport,
						    casacore::Double& fVal, casacore::Double& wVal, casacore::Int& mVal,
						    casacore::Double& conjFreq, casacore::Int& conjPoln,
						    casacore::Bool loadPixels,
						    casacore::Bool loadMiscInfo)
  {
    try
      {
	casacore::PagedImage<casacore::Complex> thisCF(Dir+'/'+fileName);
	if (loadPixels) pixelBuffer.assign(thisCF.get());
	casacore::TableRecord miscinfo;
	if (loadMiscInfo)
	  {
	    miscinfo= thisCF.miscInfo();

	    miscinfo.get("ParallacticAngle", paVal);
	    miscinfo.get("MuellerElement", mVal);
	    miscinfo.get("WValue", wVal);
	    miscinfo.get("Xsupport", xSupport);
	    miscinfo.get("Ysupport", ySupport);
	    miscinfo.get("Sampling", sampling);
	    miscinfo.get("ConjFreq", conjFreq);
	    miscinfo.get("ConjPoln", conjPoln);
	    casacore::Int index= thisCF.coordinates().findCoordinate(casacore::Coordinate::SPECTRAL);
	    coordSys = thisCF.coordinates();
	    casacore::SpectralCoordinate spCS = coordSys.spectralCoordinate(index);
	    fVal=static_cast<Float>(spCS.referenceValue()(0));
	  }
	return miscinfo;
      }
    catch(AipsError& x)
      {
	throw(AipsError(String("Error in SynthesisUtils::getCFParams(): ")
				      +x.getMesg()));
      }
  };


  void SynthesisUtils::rotate2(const double& actualPA, CFCell& baseCFC, CFCell& cfc, const double& rotAngleIncr)
    {
      LogIO log_l(LogOrigin("SynthesisUtils", "rotate2"));

      // // If the A-Term is a No-Op, the resulting CF is rotationally
      // // symmetric.
      // if (isNoOp()) return;

      (void)baseCFC;

      //double actualPA = getPA(vb);
      double currentCFPA = cfc.pa_p.getValue("rad");
      //Double baseCFCPA=baseCFC.pa_p.getValue("rad");

      double dPA = currentCFPA-actualPA;

      if (fabs(dPA) > fabs(rotAngleIncr))
	{
	  casacore::Array<TT> inData;
	  //inData.assign(*baseCFC.getStorage());
	  //dPA = baseCFCPA-actualPA;
	  dPA = currentCFPA-actualPA;
	  inData.assign(*cfc.getStorage());
	  try
	    {
	      SynthesisUtils::rotateComplexArray(log_l, inData, cfc.coordSys_p,
						 *cfc.getStorage(),
						 dPA);//,"LINEAR");
	      // currentCFPA-actualPA);//,"LINEAR");
	    }
	  catch (AipsError &x)
	    {
	      log_l << x.getMesg() << LogIO::EXCEPTION;
	    }
	  cfc.pa_p=Quantity(actualPA, "rad");
	}
    };


    //
    // Parser for parsing the NAME field of the SPECTRAL_WINDOW sub-table.
    // Returns a vector of string containing tokens separated by "#"
    // in the supplied band name string.
    //
    casacore::Vector<casacore::String> SynthesisUtils::parseBandName(const casacore::String& bandName)
    {
      casacore::Vector<casacore::String> tokens;

      // Allow a blank band name for older MSes where this is sometimes left blank.
      if (bandName == "")
	{
	  tokens.resize(1);tokens="";
	  return tokens;
	}

      char *tok, *name;
      int i=0;

      name = (char *)bandName.c_str();
      if ((tok = std::strtok(name,"#"))!=NULL)
	{
	  tokens.resize(i+1,true); tokens(i)="";
	  tokens(i++).assign(tok);
	}

      while ((tok = std::strtok(NULL,"#"))!=NULL)
	{
	  tokens.resize(i+1,true); tokens(i)="";
	  tokens(i++).assign(tok);
	}
      if (tokens(0)=="")
	{
	  LogIO log_l(LogOrigin("SynthesisUtils", "parseBandName"));
	  log_l << "Error while parsing band name \"" << bandName << "\"" << LogIO::EXCEPTION;
	}

      // for (i=0;i<3;i++)
      // 	if (tokens(i)=="") log_l << "Error while parsing band name \"" << bandName << LogIO::EXCEPTION;
      return tokens;
    }


  
  
  template
  std::vector<Double>::iterator SynthesisUtils::Unique(std::vector<Double>::iterator first, std::vector<Double>::iterator last);
  template
  std::vector<Float>::iterator SynthesisUtils::Unique(std::vector<Float>::iterator first, std::vector<Float>::iterator last);
  template
  std::vector<Int>::iterator SynthesisUtils::Unique(std::vector<Int>::iterator first, std::vector<Int>::iterator last);

  template 
  Double SynthesisUtils::stdNearestValue(const vector<Double>& list, const Double& val, Int& index);
  template 
  Float SynthesisUtils::stdNearestValue(const vector<Float>& list, const Float& val, Int& index);
  template 
  Int SynthesisUtils::stdNearestValue(const vector<Int>& list, const Int& val, Int& index);


  }  
   
    //using namespace casacore;
  } // namespace casa
