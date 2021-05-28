//# ALMAAperture.cc: Implementation of the ALMAAperture class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//# Copyright (C) 2011 by ESO (in the framework of the ALMA collaboration)
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
//
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/TransformMachines/ALMAAperture.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <synthesis/TransformMachines/WTerm.h>
#include <synthesis/TransformMachines/ALMACalcIlluminationConvFunc.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>

using namespace casacore;
namespace casa{

  AntennaResponses* ALMAAperture::aR_p = 0;
  Bool ALMAAperture::orderMattersInCFKey = false;

  ALMAAperture::ALMAAperture(): 
    //    ATerm(),
    AzElAperture(),
    polMap_p(),
    antTypeMap_p(),
    respImage_p()
  {

    haveCannedResponses_p = true;
    
    if(!aR_p){
      aR_p = new AntennaResponses();
    }

    if(!aR_p->isInit()){ // the shared antenna responses object was not yet initialised

      cout << "Initialising antenna responses ..." << endl;

      String antRespPath;
      if(!MeasTable::AntennaResponsesPath(antRespPath, "ALMA")) {
	// unknown observatory
	logIO() << LogOrigin("ALMAAperture", "ctor")
		<< LogIO::WARN
		<< "We don't have any precalculated antenna responses for ALMA."
		<< endl << "Will try to generate them using ray tracing instead."
		<< LogIO::POST;
	haveCannedResponses_p = false;
      }
      else if(!aR_p->init(antRespPath)){
	// init failed
	String mesg="Initialisation of antenna responses for observatory ALMA failed using path "+antRespPath;
	SynthesisError err(mesg);
	throw(err);
      }
      else if(MeasTable::AntennaResponsesPath(antRespPath, "ACA")) {
	// also have responses for ACA
	aR_p->append(antRespPath); // dont't throw if this fails because the ACA antennas
	                          // may already be in the ALMA table
      }
    }
	 
  }
  
  ALMAAperture::~ALMAAperture(){ 
    for(uInt i=0;i<respImage_p.nelements();i++){
      if(respImage_p(i)){
	delete respImage_p(i);
      } 
    }
  };


  ALMAAperture& ALMAAperture::operator=(const ALMAAperture& other)
  {
    if(this!=&other) 
      {
        // aR_p does not need to be copied because it is static
	haveCannedResponses_p = other.haveCannedResponses_p;
 	polMap_p.assign(other.polMap_p);
	antTypeMap_p.assign(other.antTypeMap_p);
      }
    return *this;
  }
  void ALMAAperture::cacheVBInfo(const String& telescopeName, 
				 const Float& diameter)
  {
    telescopeName_p=telescopeName;
    Diameter_p=diameter;
  }
  void ALMAAperture::cacheVBInfo(const VisBuffer& vb)
  {
    Vector<String> telescopeNames=vb.msColumns().observation().telescopeName().getColumn();
    for(uInt nt=0;nt<telescopeNames.nelements();nt++){
      if ((telescopeNames(nt) != "ALMA") && 
	  (telescopeNames(nt) != "ACA") && 
	  (telescopeNames(nt) != "OSF")){
	String mesg="Can handle only ALMA, ACA, and OSF antennas.\n";
	mesg += "Erroneous telescope name = " + telescopeNames(nt) + ".";
	SynthesisError err(mesg);
	throw(err);
      }
    }
    telescopeName_p=telescopeNames[0];
  }

  Int ALMAAperture::getBandID(const Double& freq, const String& /*telescopeName*/)
  {
    Int bandID = -1;
    if(haveCannedResponses_p){
      String bandName;
      if(aR_p->getBandName(bandName, "ALMA", freq)){
	bandID = atoi(bandName.substr(bandName.find("and")+3).c_str()); // band names start with "band" 
      }
      else{
	logIO() << LogOrigin("ALMAAperture", "getVisParams")
		<< LogIO::WARN
		<< "We don't have predefined antenna responses for ALMA at "
		<< freq << " Hz. Will try to use raytracing instead."
		<< LogIO::POST;
      }      
    }
    return bandID;
  }

  Int ALMAAperture::getVisParams(const VisBuffer& vb, 
                                 const CoordinateSystem& /*skyCoord*/)
  {
    cacheVBInfo(vb);

    MVFrequency FreqQ(vb.msColumns().spectralWindow().refFrequencyQuant()(0));
    Double Freq = FreqQ.getValue();

    return getBandID(Freq, telescopeName_p);

    // Int bandID = -1;
    // if(haveCannedResponses_p){
    //   String bandName;
    //   if(aR_p->getBandName(bandName, "ALMA", FreqQ)){
    // 	bandID = atoi(bandName.substr(bandName.find("and")+3).c_str()); // band names start with "band" 
    //   }
    //   else{
    // 	logIO() << LogOrigin("ALMAAperture", "getVisParams")
    // 		<< LogIO::WARN
    // 		<< "We don't have predefined antenna responses for ALMA at "
    // 		<< Freq << " Hz. Will try to use raytracing instead."
    // 		<< LogIO::POST;
    //   }      
    // }
    // return bandID;
  }
  
  void ALMAAperture::makeFullJones(ImageInterface<Complex>& /*pbImage*/,
				   const VisBuffer& /*vb*/,
				   Bool /*doSquint*/, Int& /*bandID*/, Double /*freqVal*/)
  {
    throw(AipsError("ALMAAperture::makeJones() not yet implemented"));
  }
  
  void ALMAAperture::applySky(ImageInterface<Complex>& outImage,  // the image (cube if there is more than one pol)
			                                           // upon return: FTed convolution pair of AIFs given by the cfKey  
			                                           // rotated by the PA and regridded to the image
			      const VisBuffer& vb, // for the parallactic angle
			      const Bool doSquint,
			      const Int& cfKey,
			      const Bool raytrace)
  {

    if(getVisParams(vb)==-1 || raytrace){ // need to use ray tracing
      ALMACalcIlluminationConvFunc almaPB;
      Long cachesize=(HostInfo::memoryTotal(true)/8)*1024;
      almaPB.setMaximumCacheSize(cachesize);
      almaPB.applyPB(outImage, vb, doSquint, cfKey);
    }
    else{ // use canned antenna responses

      //   (issue warning if image too coarse => will be in method ATerm::OK(vb, PAtolerance, timetolerance)
      
      //   identify the right response image based on antenna, freq band, polarizations
      Vector<ALMAAntennaType> aTypes = antennaTypesFromCFKey(cfKey);
      uInt nAntTypes = aTypes.nelements();
      Int spwId = vb.spectralWindow();

      Vector<String> respImageName(nAntTypes);
      Vector<uInt> respImageChannel(nAntTypes);
      Vector<MFrequency> respImageNomFreq(nAntTypes);
      Vector<AntennaResponses::FuncTypes> respImageFType(nAntTypes);
      Vector<MVAngle> respImageRotOffset(nAntTypes);
      Vector<Vector<Array<Complex> > > respByPol(nAntTypes); 

      MFrequency refFreq = vb.msColumns().spectralWindow().refFrequencyMeas()(spwId);
      MEpoch obsTime = vb.msColumns().observation().timeRangeMeas()(vb.observationId()(0))(IPosition(1,0)); // start time
      MDirection obsDir = vb.msColumns().field().phaseDirMeas(vb.fieldId()); 

      for(uInt i=0; i<nAntTypes; i++){
	cout << "Looking for ant type \"" << antTypeStrFromType(aTypes(i)) << "\", freq " 
	     << refFreq.get(Unit("Hz")).getValue() << " Hz" << endl;
	if(!aR_p->getImageName(respImageName(i),
			       respImageChannel(i),
			       respImageNomFreq(i),
			       respImageFType(i),
			       respImageRotOffset(i),
			       "ALMA",
			       obsTime,
			       refFreq,
			       AntennaResponses::EFP, 
			       antTypeStrFromType(aTypes(i)),
			       obsDir,
			       "", // receiver type
			       0 // beam number
			       )){ // no matching response found
	  ostringstream oss;
	  oss << "No matching antenna response found for frequency "
	      << refFreq.get(Unit("Hz")).getValue() << " Hz, and antenna type "
	      << antTypeStrFromType(aTypes(i));
	  throw(SynthesisError(oss.str()));
	}
      }

      // load all necessary images (max. two)
      for(uInt i=0; i<nAntTypes; i++){
	if(respImageFType(i)!=AntennaResponses::EFP){
	  throw(SynthesisError(String("Can only process antenna responses of type EFP.")));
	}
      }
      respImage_p.resize(nAntTypes);
      for(uInt i=0; i<nAntTypes; i++){
	cout << "Loading " << respImageName(i) << endl;
   	
	try{
	  respImage_p(i) = new PagedImage<Complex>(respImageName(i));
	}
	catch(std::exception x){
	  ostringstream oss;
	  oss << "Error reading antenna response image from path \""
	      << respImageName(i) << "\": " << x.what();
	  respImage_p.resize(i,true);
	  throw(SynthesisError(oss.str()));
	} 
      }
      cout << "Loaded " << nAntTypes << " images." << endl;



      // check if there are spectral and stokes axes in the output image
      Int pIndex = outImage.coordinates().findCoordinate(Coordinate::STOKES);
      if(pIndex==-1){
	  ostringstream oss;
	  oss << "Error: input image does not contain the necessary polarisation axis.";
	  throw(SynthesisError(oss.str()));
      }	
      Int pSIndex = outImage.coordinates().findCoordinate(Coordinate::SPECTRAL);

      // identify polarisations in the response image and fill into respByPol(i)
      Int rIndex = respImage_p(0)->coordinates().findCoordinate(Coordinate::STOKES);
      Int rAxis = respImage_p(0)->coordinates().pixelAxes(rIndex)(0);
      Vector<Int> rStokes = respImage_p(0)->coordinates().stokesCoordinate(rIndex).stokes();
      IPosition rShape = respImage_p(0)->shape();
      if( (nAntTypes>1) && rShape != respImage_p(1)->shape()){
	ostringstream oss;
	oss << "Error: response images for different antenna types (but otherwise identical parameters)"
	    << endl << "need to have the same shape:"
	    << "Resp. image 1 shape " << rShape << endl
	    << "Resp. image 2 shape " << respImage_p(1)->shape() << endl;
	throw(SynthesisError(oss.str()));
      }	
	  
      const uInt rNDim = rShape.size();

      IPosition rSkyShape(rNDim,1);
      rSkyShape(0) = rShape(0);
      rSkyShape(1) = rShape(1);
      uInt nRPol = rStokes.nelements();
      if(!(nRPol==4 && 
	   rStokes(0)==Stokes::XX && rStokes(1)==Stokes::XY && rStokes(2)==Stokes::YX && rStokes(3)==Stokes::YY)){
	  ostringstream oss;
	  oss << "Error: Antenna response image from path \""
	      << respImageName(0) 
	      << "\" does not contain the necessary polarisation products or they are in the wrong order.\n"
	      << "Order should be XX, XY, YX, YY.";
	  throw(SynthesisError(oss.str()));
      }	
      for(uInt i=0;i<nAntTypes; i++){ 
	respByPol(i).resize(nRPol); 
      }
      for(uInt i=0; i<nAntTypes; i++){
	for(uInt iPol=0; iPol<nRPol; iPol++){
	  IPosition start(rNDim,0);
	  start(rAxis) = iPol;
	  respImage_p(i)->getSlice(respByPol(i)(iPol), start, rSkyShape); 
	}
      }

      // identify polarizations in the input image, put them into polToDoIndex()
      Vector<Int> inStokes;
      inStokes = outImage.coordinates().stokesCoordinate(pIndex).stokes();
      uInt nPol = inStokes.nelements();
      Vector<uInt> polToDoIndex(nPol);
      for(uInt i=0; i<nPol; i++){
	uInt ival=-1;
	switch(inStokes(i)){
	case Stokes::XX:
	  ival = 0;
	  break;
	case Stokes::XY:
	  ival = 1;
	  break;
	case Stokes::YX:
	  ival = 2;
	  break;
	case Stokes::YY:
	  ival = 3;
	  break;
	default:
	  ostringstream oss;
	  oss << "Error processing input image: polarization not valid for ALMA: " 
	      << Stokes::name(Stokes::type(inStokes(i)));
	  throw(SynthesisError(oss.str()));
	  break;
	}
	polToDoIndex(i) = ival;
      }

      // Calculate the primary beam for the given baseline type for each polarization
      LogIO os(LogOrigin("ALMAAperture", "applySky", WHERE));
      
      CoordinateSystem dCoord = respImage_p(0)->coordinates(); // assume both response images have same coordsys

      CoordinateSystem dCoordFinal(dCoord);
      uInt rNDimFinal = rNDim;
      IPosition dShapeFinal = respImage_p(0)->shape();
      dShapeFinal(rAxis) = nPol; // set the number of stokes pixels to that of the output image

      // check if we need to add a degenerate spectral axis to the response image
      Int rSIndex = dCoord.findCoordinate(Coordinate::SPECTRAL);
      if(rSIndex==-1 && pSIndex!=-1){// no spectral coordinate in resp. image but input has one
	SpectralCoordinate sC;
	dCoordFinal.addCoordinate(sC);
	rSIndex = dCoordFinal.findCoordinate(Coordinate::SPECTRAL);
	dShapeFinal.resize(4,true);
	dShapeFinal(3)=1;
	rNDimFinal +=1;
      }
      else if(rSIndex!=-1 && pSIndex==-1){ // no spectral coordinate in input image but response has one
	dCoordFinal = CoordinateSystem();
	dCoordFinal.addCoordinate(dCoord.directionCoordinate(dCoord.findCoordinate(Coordinate::DIRECTION))); 
	dCoordFinal.addCoordinate(dCoord.stokesCoordinate(dCoord.findCoordinate(Coordinate::STOKES))); 
	dShapeFinal.resize(3,true);
	rNDimFinal -=1;
	rSIndex = -1;
      }

      TempImage<Complex> nearFinal(dShapeFinal, dCoordFinal);

      for(uInt iPol=0; iPol<nPol; iPol++){

	Array<Complex> pB( respByPol(0)(polToDoIndex(iPol)).shape() );
	Array<Complex> fact1( pB.shape() );
	Array<Complex> fact2( pB.shape() );

	// rotate the two factor arrays into the right PA
	Double dAngleRad = getPA(vb);
	cout << "PA = " << dAngleRad << " rad" << endl;

	Int fact1Index=-1, fact2Index=-1;
	Double pA1, pA2;

	// apply the rotation offset from the response table
	pA1 = dAngleRad + respImageRotOffset(0).radian();
	pA2 = dAngleRad + respImageRotOffset(nAntTypes-1).radian();

	switch(polToDoIndex(iPol)){
	case 0: // XX
	  fact1Index = fact2Index = 0;
	  break;
	case 1: //XY
	  fact1Index = 0;
	  fact2Index = 3;
	  break;
	case 2: //YX
	  fact1Index = 3;
	  fact2Index = 0;
	  break;
	case 3: //YY
	  fact1Index = fact2Index = 3;
	  break;
	}
	  
	if(pA1 != pA2){ // rotate individual factors before multiplication

	  // rotate factor 1
	  SynthesisUtils::rotateComplexArray(os, respByPol(0)(fact1Index), dCoord, fact1, 
					     pA1, "LINEAR", 
					     false); // don't modify dCoord
	  // if necessary rotate factor 2 
	  if((nAntTypes-1)==0 &&  fact2Index==fact1Index){ // also implies that pA1==PA2
	    fact2.assign(fact1);
	  }
	  else{
	    SynthesisUtils::rotateComplexArray(os, respByPol(nAntTypes-1)(fact2Index), dCoord, fact2, 
					       pA2, "LINEAR", 
					       false); // don't modify dCoord
	  }
	}
	else{ // rotate PB later
	  fact1.assign(respByPol(0)(fact1Index));
	  fact2.assign(respByPol(nAntTypes-1)(fact2Index));
	}

	// multiply EFPs (equivalent to convolution of AIFs) to get primary beam
	if(doSquint){
	  pB = fact1 * conj(fact2);
	}
	else{
	  pB = abs(fact1) * abs(fact2);
	}

	// now have the primary beam for polarization iPol in pB

	// combine all PBs into one image
	IPosition pos(rNDimFinal,0);
	pos(rAxis) = iPol;

	if(pA1 == pA2){ // still need to rotate pB by PA

	  Array<Complex> pBrot( pB.shape() );
	  SynthesisUtils::rotateComplexArray(os, pB, dCoord, pBrot, 
					     pA1, "LINEAR", 
					     false); // don't modify dCoord
	  nearFinal.putSlice(pBrot, pos);  

	}
	else{ // pB was already rotated above
	  nearFinal.putSlice(pB, pos);  
	}      

      }

      // Then regrid it to the image

      // The following mess is necessary since ImageRegrid does not work for Complex images
      // (Interpolate2D is not templated)

      Array<Complex> nearFinalArray = nearFinal.get();

      CoordinateSystem outCS(outImage.coordinates());
      Vector<Int> pixAxes=outCS.pixelAxes(outCS.findCoordinate(Coordinate::DIRECTION));
      IPosition whichOutPixelAxes(pixAxes);

      // get the world coordinates of the center of outImage
      Vector<Double> wCenterOut(2);
      Vector<Double> pCenterOut(2);
      pCenterOut(0) = outImage.shape()(whichOutPixelAxes(0))/2.-0.5;
      pCenterOut(1) = outImage.shape()(whichOutPixelAxes(1))/2.-0.5;
      Vector<String> wAU = outCS.directionCoordinate(0).worldAxisUnits();
      outCS.directionCoordinate(0).toWorld(wCenterOut, pCenterOut);
      //cout << "pixel center " << pCenterOut << " world center " << wCenterOut << " " << wAU(0) << endl;

      uInt dirCoordIndex = dCoordFinal.findCoordinate(Coordinate::DIRECTION);

      // convert direction coordinate to J2000 if necessary
      if(dCoordFinal.directionCoordinate(dirCoordIndex).directionType() != MDirection::J2000){
	Vector<Double> incrV = dCoordFinal.directionCoordinate(dirCoordIndex).increment();
	Vector<Double> refPV = dCoordFinal.directionCoordinate(dirCoordIndex).referencePixel();
	Vector<String> wAxUnitsV =  dCoordFinal.directionCoordinate(dirCoordIndex).worldAxisUnits();
	Projection pproj = dCoordFinal.directionCoordinate(dirCoordIndex).projection();
	Matrix<Double> xxform = dCoordFinal.directionCoordinate(dirCoordIndex).linearTransform();
	Quantity incr0(incrV(0),wAxUnitsV(0)); 
	Quantity incr1(incrV(1),wAxUnitsV(1)); 

	DirectionCoordinate newDC(MDirection::J2000, pproj, 0., 0., 
				  incr0.getValue(Unit("rad")), incr1.getValue(Unit("rad")),
				  xxform, refPV(0), refPV(1), 0., 90.);
	if(!dCoordFinal.replaceCoordinate(newDC, dirCoordIndex)){
	  ostringstream oss;
	  oss << "Error: response image direction coordinate cannot be set to direction type J2000.";
	  throw(SynthesisError(oss.str()));
	}
      }

      DirectionCoordinate dCoordFinalDir = dCoordFinal.directionCoordinate(0);

      // Scale response image with frequency
      Unit uHz("Hz");
      Double refFreqHz = MFrequency::Convert(refFreq, MFrequency::TOPO)().get(uHz).getValue();
      Double nomFreqHz = MFrequency::Convert(respImageNomFreq(0), MFrequency::TOPO)().get(uHz).getValue();
      if(refFreqHz<=0.){
	 throw(SynthesisError("Reference frequency in input image is <=0."));
      }
      Double fScale = nomFreqHz/refFreqHz;
      Vector<Double> newIncr = dCoordFinalDir.increment();
      cout << "Image increment before freq scaling " << newIncr << ", after scaling " << newIncr * fScale << endl;
      dCoordFinalDir.setIncrement(newIncr * fScale);

      // and set the reference Value of inImage to the world center of the outImage
      Vector<String> wAUI = dCoordFinalDir.worldAxisUnits();
      if(!(wAU(0)==wAUI(0))){
	Unit uIn(wAU(0));
	Unit uOut(wAUI(0));
	Quantity q0(wCenterOut(0), uIn);
	Quantity q1(wCenterOut(1), uIn);
	wCenterOut(0) = q0.getValue(uOut);
	wCenterOut(1) = q1.getValue(uOut);
      }

      cout << "Image pixel center " << pCenterOut << ", world center " << wCenterOut << " " << wAUI(0) << endl;
      dCoordFinalDir.setReferenceValue(wCenterOut);

      dCoordFinal.replaceCoordinate(dCoordFinalDir, dirCoordIndex);

      //PagedImage<Float> im1(nearFinalArray.shape(),dCoordFinal, "inImageReal.im");
      //im1.copyData(LatticeExpr<Float>(real(ArrayLattice<Complex>(nearFinalArray))));  
      //PagedImage<Float> im2(nearFinalArray.shape(),dCoordFinal, "inImageImag.im");
      //im2.copyData(LatticeExpr<Float>(imag(ArrayLattice<Complex>(nearFinalArray))));  

      TempImage<Float> inImage(nearFinalArray.shape(),dCoordFinal);

      TempImage<Float> tOutImage(outImage.shape(), outCS);
      Array<Complex> outArray(outImage.shape(), Complex(0.,0.));

      ImageRegrid<Float> iR;
      //iR.showDebugInfo(1);

      inImage.copyData(LatticeExpr<Float>(real(ArrayLattice<Complex>(nearFinalArray))));
      tOutImage.set(0.0);
      
      iR.regrid(tOutImage, Interpolate2D::LINEAR, whichOutPixelAxes, inImage); 
      setReal(outArray,tOutImage.get());
      
      inImage.copyData(LatticeExpr<Float>(imag(ArrayLattice<Complex>(nearFinalArray))));
      tOutImage.set(0.0);
      
      iR.regrid(tOutImage, Interpolate2D::LINEAR, whichOutPixelAxes, inImage);
      setImag(outArray,tOutImage.get());

      outImage.put(outArray);

      // tidy up
      for(uInt i=0; i<nAntTypes; i++){
	delete respImage_p(i);
      }
      respImage_p.resize(0);	  

    }  
  }

  void ALMAAperture::applySky(ImageInterface<Float>& outImage,
			      const VisBuffer& vb, 
			      const Bool doSquint,
			      const Int& cfKey,
			      const Bool raytrace)
  {
    if(getVisParams(vb)==-1 || raytrace){ // need to use ray tracing
      ALMACalcIlluminationConvFunc almaPB;
      Long cachesize=(HostInfo::memoryTotal(true)/8)*1024;
      almaPB.setMaximumCacheSize(cachesize);
      almaPB.applyPB(outImage, vb, doSquint, cfKey);
    }
    else{ // use canned antenna responses
      TempImage<Complex> tI(outImage.shape(), outImage.coordinates());
      tI.set(Complex(0.,0.));
      applySky(tI, vb, doSquint, cfKey, false);
      outImage.put(real(abs(tI.get())));
    }
  }

  Int ALMAAperture::makePBPolnCoords(const VisBuffer&vb,
				     const Int& convSize,
				     const Int& convSampling,
				     const CoordinateSystem& skyCoord,
				     const Int& skyNx, const Int& /*skyNy*/,
				     CoordinateSystem& feedCoord)
  {
    feedCoord = skyCoord;
    //
    // Make a two dimensional image to calculate auto-correlation of
    // the ideal illumination pattern. We want this on a fine grid in
    // the UV plane
    //
    Int directionIndex=skyCoord.findCoordinate(Coordinate::DIRECTION);
    AlwaysAssert(directionIndex>=0, AipsError);
    DirectionCoordinate dc=skyCoord.directionCoordinate(directionIndex);
    Vector<Double> sampling;
    sampling = dc.increment();
    sampling*=Double(convSampling);
    sampling*=Double(skyNx)/Double(convSize);
    dc.setIncrement(sampling);
    
    
    Vector<Double> unitVec(2);
    unitVec=convSize/2;
    dc.setReferencePixel(unitVec);
    
    // Set the reference value to that of the image
    feedCoord.replaceCoordinate(dc, directionIndex);

    //
    // Make an image with linear polarization axis.
    //
    Int NPol=0,M,N=0;
    M=polMap_p.nelements();
    for(Int i=0;i<M;i++){
      if (polMap_p(i) > -1) NPol++;
    }
    Vector<Int> poln(NPol);
    
    Int index;
    Vector<Int> inStokes;
    index = feedCoord.findCoordinate(Coordinate::STOKES);
    inStokes = feedCoord.stokesCoordinate(index).stokes();
    N = 0;
    try{
      for(Int i=0;i<M;i++) if (polMap_p(i) > -1) {poln(N) = vb.corrType()(i);N++;}
      StokesCoordinate polnCoord(poln);
      Int StokesIndex = feedCoord.findCoordinate(Coordinate::STOKES);
      feedCoord.replaceCoordinate(polnCoord,StokesIndex);
    }
    catch(AipsError& x){
      throw(SynthesisFTMachineError("Likely cause: Discrepancy between the poln. "
				    "axis of the data and the image specifications."));
    }
    
    return NPol;
  }


  Vector<Int> ALMAAperture::vbRow2CFKeyMap(const VisBuffer& vb, Int& nUnique) 
  {
    // return index to outputImages for each row in vb
    Vector<Int> map; 
    map.resize(vb.nRow()); 

    //cout << "vb rows: " << vb.nRow() << endl;

    if(haveCannedResponses_p){
      // distinguish different antenna types
      if(antTypeMap_p.nelements()!=(uInt)vb.numberAnt()){
	antTypeMap_p.assign(antTypeMap(vb));
	cout << "initialising antTypeMap to " << antTypeMap_p << endl;
      }

      std::map<Int, Int > cFKeysEncountered;
      Int cfKeyCount = 0;
      for(uInt i=0; i<(uInt)vb.nRow(); i++){
	Int cfKey = cFKeyFromAntennaTypes(antTypeMap_p(vb.antenna1()(i)),
					  antTypeMap_p(vb.antenna2()(i)));
	map(i) = cfKey;
	if(cFKeysEncountered.find(cfKey) == cFKeysEncountered.end( )){ // new cFKey
	  cFKeysEncountered.insert(std::pair<Int, Int >(cfKey, cfKeyCount));
	  cfKeyCount++;
	}
      }  
      nUnique = cfKeyCount;
    }
    else{ // raytracing knows only one antenna type for the moment
      map=0; 
      nUnique=1; 
    }
    return map;
  }
  
  Vector<ALMAAntennaType> ALMAAperture::antTypeMap(const VisBuffer& vb){
    Vector<ALMAAntennaType> map(vb.numberAnt(),ALMA_INVALID);
    for(uInt i=0; i<map.nelements(); i++){
      map(i) = antTypeFromName(vb.msColumns().antenna().name()(i));
      cout << vb.msColumns().antenna().name()(i) << " " << map(i) << endl;
      if(map(i)==ALMA_INVALID){
	logIO() << LogOrigin("ALMAAperture", "antTypeMap")
		<< LogIO::WARN 
		<< "Unrecognised antenna type for antenna \"" 
		<< vb.msColumns().antenna().name()(i) << "\""  
		<< LogIO::POST;
      }
    }
    return map;
  }

  ALMAAntennaType ALMAAperture::antTypeFromName(const String& name){
    ALMAAntennaType rval = ALMA_INVALID;
    String typeN = name.substr(0,2);
    if(typeN=="DV") rval=ALMA_DV;
    else if(typeN=="DA") rval=ALMA_DA;
    else if(typeN=="PM") rval=ALMA_PM;
    else if(typeN=="CM") rval=ALMA_CM;
    return rval;
  }

  String ALMAAperture::antTypeStrFromType(const ALMAAntennaType& aType){
    String tS;
    switch(aType){
    case ALMA_DV:
      tS = "DV";
      break;
    case ALMA_DA:
      tS = "DA";
      break;
    case ALMA_PM:
      tS = "PM";
      break;
    case ALMA_CM:
      tS = "CM";
      break;
    case ALMA_INVALID:
    default:
      tS = "UNKOWN";
      break;
    }
    return tS;
  }


  Int ALMAAperture::cFKeyFromAntennaTypes(const ALMAAntennaType aT1, const ALMAAntennaType aT2){
    if(orderMattersInCFKey){
      return (Int)aT2+1 + 10000*((Int)aT1+1); 
    }
    else{
      return min((Int)aT1+1, (Int)aT2+1) + 10000*max((Int)aT1+1, (Int)aT2+1); // assume order doesn't matter
    }
  }

  Vector<ALMAAntennaType> ALMAAperture::antennaTypesFromCFKey(const Int& cFKey){
    Int t1 = (Int) floor(cFKey/10000.);
    Int t2 = cFKey - 10000*t1 - 1;
    t1--;
    Vector<ALMAAntennaType> rval(1);
    if(t1==t2){
      rval(0) = (ALMAAntennaType) t1;
    }
    else{
      rval.resize(2);
      if(orderMattersInCFKey){
	rval(0) = (ALMAAntennaType)t1;
	rval(1) = (ALMAAntennaType)t2;
      }
      else{
	rval(0) = (ALMAAntennaType) min(t1,t2);
	rval(1) = (ALMAAntennaType) max(t1,t2);
      }
    }
    return rval;
  }

  Vector<ALMAAntennaType> ALMAAperture::antTypeList(const VisBuffer& vb){
    Vector<ALMAAntennaType> aTypeMap = antTypeMap(vb);
    Vector<Bool> encountered(ALMA_numAntTypes, false);
    uInt typeCount = 0;
    for(uInt i=0; i<aTypeMap.nelements(); i++){
      uInt index = (uInt)aTypeMap(i);
      if(!encountered(index)){
	encountered(index) = true;
	typeCount++;
      }
    }
    Vector<ALMAAntennaType> aTypeList(typeCount);
    uInt index = 0;
    for(uInt i=0; i<ALMA_numAntTypes; i++){
      if(encountered(i)){
	aTypeList(index) = (ALMAAntennaType)i;
	index++;
      }
    }
    return aTypeList;
  }


};
