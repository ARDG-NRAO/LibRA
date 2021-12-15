//# HetArrayConvFunc.cc: Implementation for HetArrayConvFunc
//# Copyright (C) 2008-2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  General Public
//# License for more details.
//#
//# You should have received a copy of the GNU  General Public License
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

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <scimath/Mathematics/FFTServer.h>
#include <measures/Measures/MeasTable.h>
#include <scimath/Mathematics/MathFunc.h>
#include <scimath/Mathematics/ConvolveGridder.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/CompositeNumber.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>

#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/SubImage.h>
#include <images/Images/TempImage.h>
#include <imageanalysis/Utilities/SpectralImageUtil.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/Lattices/LatticeConcat.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/LatticeCache.h>
#include <lattices/LatticeMath/LatticeFFT.h>


#include <ms/MeasurementSets/MSColumns.h>

#include <msvis/MSVis/VisBuffer2.h>

#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines/PBMath1DAiry.h>
#include <synthesis/TransformMachines/PBMath1DNumeric.h>
#include <synthesis/TransformMachines/PBMath2DImage.h>
#include <synthesis/TransformMachines/PBMath.h>
#include <synthesis/TransformMachines2/HetArrayConvFunc.h>
#include <synthesis/MeasurementEquations/VPManager.h>

#include <casa/OS/Timer.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace refim {

using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::refim;


  HetArrayConvFunc::HetArrayConvFunc() : convFunctionMap_p(0), nDefined_p(0),msId_p(-1), actualConvIndex_p(-1), vpTable_p("")
{
    calcFluxScale_p=true;
    init(PBMathInterface::AIRY);
}

HetArrayConvFunc::HetArrayConvFunc(const PBMathInterface::PBClass typeToUse, const String vpTable):
    convFunctionMap_p(0), nDefined_p(0), msId_p(-1), actualConvIndex_p(-1), vpTable_p(vpTable)
{
    calcFluxScale_p=true;
    init(typeToUse);

}

HetArrayConvFunc::HetArrayConvFunc(const RecordInterface& rec, Bool calcFluxneeded):convFunctionMap_p(0), nDefined_p(0), msId_p(-1), actualConvIndex_p(-1) {
    String err;
    fromRecord(err, rec, calcFluxneeded);
}

HetArrayConvFunc::~HetArrayConvFunc() {
    //
}

void HetArrayConvFunc::init(const PBMathInterface::PBClass typeTouse) {
    doneMainConv_p=false;
    filledFluxScale_p=false;
    pbClass_p=typeTouse;
}



void HetArrayConvFunc::findAntennaSizes(const vi::VisBuffer2& vb) {

      if(msId_p != vb.msId()) {
        msId_p=vb.msId();
        //MSColumns mscol(vb.ms());
        const MSAntennaColumns& ac=vb.subtableColumns().antenna();
        antIndexToDiamIndex_p.resize(ac.nrow());
        antIndexToDiamIndex_p.set(-1);
        Int diamIndex=antDiam2IndexMap_p.size( );
        Vector<Double> dishDiam=ac.dishDiameter().getColumn();
        Vector<String>dishName=ac.name().getColumn();
        String telescop=vb.subtableColumns().observation().telescopeName()(0);
        PBMath::CommonPB whichPB;
        if(pbClass_p==PBMathInterface::COMMONPB) {
            String band;
            String commonPBName;
            // This frequency is ONLY required to determine which PB model to use:
            // The VLA, the ATNF, and WSRT have frequency - dependent PB models
            Quantity freq( vb.subtableColumns().spectralWindow().refFrequency()(0), "Hz");


            PBMath::whichCommonPBtoUse( telescop, freq, band, whichPB, commonPBName );
            //Revert to using AIRY for unknown common telescope
            if(whichPB==PBMath::UNKNOWN)
                pbClass_p=PBMathInterface::AIRY;

        }
        if(pbClass_p== PBMathInterface::AIRY) {
	  LogIO os;
	os << LogOrigin("HetArrConvFunc", "findAntennaSizes")  << LogIO::NORMAL;
            ////////We'll be using dish diameter as key
            for (uInt k=0; k < dishDiam.nelements(); ++k) {
                if( (diamIndex !=0) && antDiam2IndexMap_p.find(String::toString(dishDiam(k))) != antDiam2IndexMap_p.end( ) ) {
                    antIndexToDiamIndex_p(k)=antDiam2IndexMap_p[String::toString(dishDiam(k))];
                }
                else {
                    if(dishDiam[k] > 0.0) { //there may be stations with no dish on
                        antDiam2IndexMap_p.insert(std::pair<casacore::String, casacore::Int>(String::toString(dishDiam(k)), diamIndex));
                        antIndexToDiamIndex_p(k)=diamIndex;
                        antMath_p.resize(diamIndex+1);
                        if(pbClass_p== PBMathInterface::AIRY) {
			  //ALMA ratio of blockage to dish
                            Quantity qdiam= Quantity (dishDiam(k),"m");	
                            Quantity blockDiam= Quantity(dishDiam(k)/12.0*.75, "m");
			    Quantity support=Quantity(150, "arcsec");
                            ///For ALMA 12m dish it is effectively 10.7 m according to Todd Hunter
                            ///@ 2011-12-06
                            if((vb.subtableColumns().observation().telescopeName()(0) =="ALMA") || (vb.subtableColumns().observation().telescopeName()(0) =="ACA")){
			      Quantity fov(max(nx_p*dc_p.increment()(0), ny_p*dc_p.increment()(1)), dc_p.worldAxisUnits()(0));
			      if((abs(dishDiam[k] - 12.0) < 0.5)) {
				qdiam= Quantity(10.7, "m");
				blockDiam= Quantity(0.75, "m");
				support=Quantity(max(150.0, fov.getValue("arcsec")/5.0), "arcsec");
                                
			      }
			      else{
                                //2017 the ACA dishes are best represented by 6.25m:
                               
				qdiam= Quantity(6.25,"m");
				blockDiam = Quantity(0.75,"m");
				support=Quantity(max(300.0,fov.getValue("arcsec")/3.0) , "arcsec");
			      }
			    }
			     os << "Overriding PB with Airy of diam,blockage="<<qdiam<<","<<blockDiam<<" starting with antenna "<<k<<LogIO::POST; 
			    
			
		    

			antMath_p[diamIndex]=new PBMath1DAiry(qdiam, blockDiam,
							  support,
							  Quantity(100.0,"GHz"));
			}

		

                        //////Will no longer support this
                        /*else if(pbClass_p== PBMathInterface::IMAGE){
                          //Get the image name by calling code for the antenna name and array name
                          //For now hard wired to ALMA as this part of the code will not be accessed for non-ALMA
                          //see Imager::setMosaicFTMachine
                          // When ready to generalize then code that calls with telescope name, antenna name
                          //(via vb.msColumns) and/or diameter and frequency via vb.frequency (indexing will need to
                          //be upgraded to account for frequency too) should be done to return the
                          //right voltage pattern image.
                          String vpImageName="";
                          if (abs(dishDiam[k]-7.0) < 1.0)
                        Aipsrc::find(vpImageName, "alma.vp.7m", "");
                          else
                        Aipsrc::find(vpImageName, "alma.vp.12m", "") ;
                          //cerr << "first vpImagename " << vpImageName  << endl;
                          if(vpImageName==""){
                        String beamPath;
                        if(!MeasTable::AntennaResponsesPath(beamPath, "ALMA")){
                          throw(AipsError("Alma beam images requested cannot be found "));
                        }
                        else{
                          beamPath=beamPath.before(String("AntennaResponses"));
                          vpImageName= (abs(dishDiam[k]-7.0) < 1.0) ? beamPath
                            +String("/ALMA_AIRY_7M.VP") :
                            beamPath+String("/ALMA_AIRY_12M.VP");
                        }


                          }
                          //cerr << "Using the image VPs " << vpImageName << endl;
                          if(Table::isReadable(vpImageName))
                        antMath_p[diamIndex]=new PBMath2DImage(PagedImage<Complex>(vpImageName));
                          else
                        throw(AipsError(String("Cannot find voltage pattern image ") + vpImageName));
                        }
                        else{

                          throw(AipsError("Do not  deal with non airy dishes or images of VP yet "));
                        }
                        */
                        ++diamIndex;
	    
		    }

		}
	    }

	}
        else if(pbClass_p== PBMathInterface::IMAGE) {

            VPManager *vpman=VPManager::Instance();
            if(vpTable_p != String(""))
                vpman->loadfromtable(vpTable_p);
            ///else it is already loaded in the static object
            Vector<Record> recs;
            Vector<Vector<String> > antnames;

            if(vpman->imagepbinfo(antnames, recs)) {
                Vector<Bool> dishDefined(dishName.nelements(), false);
                Int nbeams=antnames.nelements();
                ///will be keying on file image file name here
                for (uInt k=0; k < dishDiam.nelements(); ++k) {
                    String key;
                    Bool beamDone=false;
                    Int recordToUse=0;
                    for (Int j =0; j < nbeams; ++j) {
                        key=recs[j].isDefined("realimage") ? recs[j].asString("realimage") : recs[j].asString("compleximage");
                        if(antnames[j][0]=="*" || anyEQ(dishName[k], antnames[j])) {
                            dishDefined[k]=true;
                            recordToUse=j;

                            if((diamIndex !=0) && antDiam2IndexMap_p.find(key) != antDiam2IndexMap_p.end( )) {
                                antIndexToDiamIndex_p(k)=antDiam2IndexMap_p[key];
                                beamDone=true;
                            }
                        }
                    }
                    if(!beamDone && dishDefined[k]) {
                        key=recs[recordToUse].isDefined("realimage") ? recs[recordToUse].asString("realimage") : recs[recordToUse].asString("compleximage");
                        antDiam2IndexMap_p.insert(std::pair<casacore::String, casacore::Int>(key, diamIndex));
                        antIndexToDiamIndex_p(k)=diamIndex;
                        antMath_p.resize(diamIndex+1);
                        if(recs[recordToUse].isDefined("realimage") && recs[recordToUse].isDefined("imagimage")) {
			  //PagedImage<Float> realim(recs[recordToUse].asString("realimage"));
			  // PagedImage<Float> imagim(recs[recordToUse].asString("imagim"));
                          //  antMath_p[diamIndex]=new PBMath2DImage(realim, imagim);

                          if(!Table::isReadable(recs[recordToUse].asString("realimage")))
                            throw(AipsError("real part of VP "+recs[recordToUse].asString("realimage")+ " is not readable"));
                          PagedImage<Float> realim(recs[recordToUse].asString("realimage"));
                          CountedPtr<ImageInterface<Float> >imagim;
                          if(recs[recordToUse].asString("imagimage").size()==0){
                            imagim=new TempImage<Float>(realim.shape(), realim.coordinates());
                            imagim->set(0.0);
                          }
                          else{
                            if(!Table::isReadable(recs[recordToUse].asString("imagimage")))
                              throw(AipsError("Imaginary part of VP "+recs[recordToUse].asString("imagimage")+ " is not readable"));
                            imagim= new PagedImage<Float> (recs[recordToUse].asString("imagimage"));
                          }
                            antMath_p[diamIndex]=new PBMath2DImage(realim, *imagim);

                        }
                        else {
                           antMath_p[diamIndex]=new PBMath2DImage(PagedImage<Complex>(recs[recordToUse].asString("compleximage")));
			  
			   //                          if(!Table::isReadable(recs[recordToUse].asString("compleximage")))
			   //                            throw(AipsError("complex image of VP "+recs[recordToUse].asString("compleximage")+ " is not readable"));

			   //                          PagedImage<Complex> compim(recs[recordToUse].asString("compleximage"));
			   //                          antMath_p[diamIndex]=new PBMath2DImage(compim);
			   //                          //antMath_p[diamIndex]=new PBMath2DImage(PagedImage<Complex>(recs[recordToUse].asString("compleximage")));
			 
                        }
                        ++diamIndex;
                    }
                }
                if(!allTrue(dishDefined)) {
                    //cerr << "dishDefined " << dishDefined << endl;
                    throw(AipsError("Some Antennas in the MS did not have a VP defined"));
                }
            }
            else {
                throw(AipsError("Mosaic does not support non-image voltage patterns yet"));
            }

            //Get rid of the static class
            vpman->reset();
        }
	else if(vpTable_p != String("")){
	  ////When we get vpmanager to give beams on antenna name we
	  //should change this key to antenna name and loop over all antenna names
      if((diamIndex !=0) && antDiam2IndexMap_p.find(telescop+String("_")+String::toString(dishDiam(0))) != antDiam2IndexMap_p.end( ) ) {
	    antIndexToDiamIndex_p.set(antDiam2IndexMap_p[telescop+String("_")+String::toString(dishDiam(0))]);
	  }
	  else{
		 antDiam2IndexMap_p.insert(std::pair<casacore::String, casacore::Int>(telescop+"_"+String::toString(dishDiam(0)), diamIndex));
		 antIndexToDiamIndex_p.set(diamIndex);
	     VPManager *vpman=VPManager::Instance();
	     vpman->loadfromtable(vpTable_p);
	     Record rec;
	     vpman->getvp(rec, telescop);
	     antMath_p.resize(diamIndex+1);
	     antMath_p[diamIndex]=PBMath::pbMathInterfaceFromRecord(rec);
	     vpman->reset();
	  }
	  
	}
        else if(pbClass_p==PBMathInterface::COMMONPB) {
            //cerr << "Doing the commonPB thing" << endl;
            ///Have to use telescop part as string as in multims case different
            //telescopes may have same dish size but different commonpb
            // VLA and EVLA for e.g.
            if((diamIndex !=0) && antDiam2IndexMap_p.find(telescop+String("_")+String::toString(dishDiam(0))) != antDiam2IndexMap_p.end( )) {
                antIndexToDiamIndex_p.set(antDiam2IndexMap_p[telescop+String("_")+String::toString(dishDiam(0))]);
            }
            else {
                antDiam2IndexMap_p.insert(std::pair<casacore::String, casacore::Int>(telescop+"_"+String::toString(dishDiam(0)), diamIndex));
                antIndexToDiamIndex_p.set(diamIndex);
                antMath_p.resize(diamIndex+1);
                antMath_p[diamIndex]=PBMath::pbMathInterfaceForCommonPB(whichPB, True);
            }
        }
        else {

            throw(AipsError("Mosaic  supports image based or Airy voltage patterns or known common pb   for now"));

        }

        //cerr << "antIndexTodiamIndex " << antIndexToDiamIndex_p << endl;
    }





}

void  HetArrayConvFunc::reset() {
    doneMainConv_p=false;
    convFunctions_p.resize(0, true);
    convWeights_p.resize(0, true);
    convSizes_p.resize(0, true);
    convSupportBlock_p.resize(0, true);
    convFunctionMap_p.resize(0);
    vbConvIndex_p.clear();
    ft_p=FFT2D(true);
}



void HetArrayConvFunc::findConvFunction(const ImageInterface<Complex>& iimage,
                                        const vi::VisBuffer2& vb,
                                        const Int& convSamp, const Vector<Double>& visFreq,
                                        Array<Complex>& convFunc,
                                        Array<Complex>& weightConvFunc,
                                        Vector<Int>& convsize,
                                        Vector<Int>& convSupport,
                                        Vector<Int>& convFuncPolMap,
                                        Vector<Int>& convFuncChanMap,
                                        Vector<Int>& convFuncRowMap, Bool getConjConvFunc,
					const MVDirection& extraShift, const Bool useExtraShift)
{

    storeImageParams(iimage,vb);
    convFuncChanMap.resize(vb.nChannels());
    Vector<Double> beamFreqs;
    findUsefulChannels(convFuncChanMap, beamFreqs, vb, visFreq);
    //cerr << "SPW " << vb.spectralWindow() << "   beamFreqs "<< beamFreqs <<  " chamMap " << convFuncChanMap << endl;
    Int nBeamChans=beamFreqs.nelements();
    /////For now not doing beam rotation or squints but to be enabled easily
    convFuncPolMap.resize(vb.nCorrelations());
    Int nBeamPols=1;
    convFuncPolMap.set(0);
    findAntennaSizes(vb);
    uInt ndish=antMath_p.nelements();
    if(ndish==0)
        throw(AipsError("Don't have dishsize"));
    Int ndishpair;
    if(ndish==1)
        ndishpair=1;
    else
        ndishpair=factorial(ndish)/factorial(ndish-2)/2 + ndish;

    convFunc.resize();
    weightConvFunc.resize();
    convFuncRowMap.resize();
    convsize.resize();
    convSupport.resize();

    Int isCached=checkPBOfField(vb, convFuncRowMap, extraShift, useExtraShift);
    //cout << "isCached " << isCached <<  endl;
    if(isCached==1 && (convFuncRowMap.shape()[0]==(ssize_t)vb.nRows())) {
        /*convFunc.reference(convFunc_p);
        weightConvFunc.reference(weightConvFunc_p);
        convsize=*convSizes_p[actualConvIndex_p];
        convSupport=convSupport_p;
         return;
        */
    }
    else if(isCached ==2) {
        convFunc.resize();
        weightConvFunc.resize();
        convsize.resize();
        convSupport.resize();
        convFuncRowMap.resize();
        return;

    }
    actualConvIndex_p=convIndex(vb);
    //cerr << "actual conv index " << actualConvIndex_p << " doneMainconv " << doneMainConv_p << endl;
    if(doneMainConv_p.shape()[0] < (actualConvIndex_p+1)) {
        //cerr << "resizing DONEMAIN " <<   doneMainConv_p.shape()[0] << endl;
        doneMainConv_p.resize(actualConvIndex_p+1, true);
        doneMainConv_p[actualConvIndex_p]=false;
        convFunctions_p.resize(actualConvIndex_p+1);
        convFunctions_p[actualConvIndex_p]=nullptr;
    }
    ///// In multi ms mode ndishpair may change when meeting a new ms
    //// redo the calculation then
    if(msId_p != vb.msId())//doneMainConv_p[actualConvIndex_p] && ((convSupport_p.nelements() != uInt(ndishpair)) || convFunctions_p[actualConvIndex_p]->shape()[3] != nBeamChans))
    {
        doneMainConv_p[actualConvIndex_p]=false;
        //cerr << "invalidating doneMainConv " <<  convFunctions_p[actualConvIndex_p]->shape()[3] << " =? " << nBeamChans << " convsupp " << convSupport_p.nelements() << endl;
    }

    // Get the coordinate system
    CoordinateSystem coords(iimage.coordinates());
    Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
    AlwaysAssert(directionIndex>=0, AipsError);
    // Set up the convolution function.
    Int nx=nx_p;
    Int ny=ny_p;
    Int support=max(nx_p, ny_p)/10;
    Int convSampling=1;
    if(!doneMainConv_p[actualConvIndex_p]) {
        for (uInt ii=0; ii < ndish; ++ii) {
            support=max((antMath_p[ii])->support(coords), support);
        }
	
        support=Int(min(Float(support), max(Float(nx_p), Float(ny_p)))*2.0)/2;
        convSize_p=support*convSampling;
        // Make this a nice composite number, to speed up FFTs
        CompositeNumber cn(uInt(convSize_p*2.0));
        convSize_p  = cn.nextLargerEven(Int(convSize_p));
        convSize_p=(convSize_p/16)*16;  // need it to be divisible by 8 in places
	
    }


    DirectionCoordinate dc=dc_p;
    //where in the image in pixels is this pointing
    Vector<Double> pixFieldDir(2);
    if(doneMainConv_p.shape()[0] < (actualConvIndex_p+1)) {
        //cerr << "resizing DONEMAIN " <<   doneMainConv_p.shape()[0] << endl;
        doneMainConv_p.resize(actualConvIndex_p+1, true);
        doneMainConv_p[actualConvIndex_p]=false;
    }
    //no need to call toPix here as its been done already above in checkOFPB
    //thus the values are still current.
    pixFieldDir=thePix_p;
    //toPix(pixFieldDir, vb);
    MDirection fieldDir=direction1_p;
    //shift from center
    pixFieldDir(0)=pixFieldDir(0)- Double(nx / 2);
    pixFieldDir(1)=pixFieldDir(1)- Double(ny / 2);
    //phase gradient per pixel to apply
    pixFieldDir(0)=-pixFieldDir(0)*2.0*C::pi/Double(nx)/Double(convSamp);
    pixFieldDir(1)=-pixFieldDir(1)*2.0*C::pi/Double(ny)/Double(convSamp);


    if(!doneMainConv_p[actualConvIndex_p]) {
      //cerr << "doneMainConv_p " << actualConvIndex_p << endl;

        Vector<Double> sampling;
	
        sampling = dc.increment();
	sampling*=Double(convSampling);
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
	//cerr << "spcoord " ;
	//spCoord.print(std::cerr);
        coords.replaceCoordinate(spCoord, spind);
	CoordinateSystem conjCoord=coords;
	Double centerFreq=SpectralImageUtil::worldFreq(csys_p, 0.0);
	SpectralCoordinate conjSpCoord=spCoord;
		//cerr << "centreFreq " << centerFreq << " beamFreqs " << beamFreqs(0) << "  " << beamFreqs(1) << endl;
	conjSpCoord.setReferenceValue(Vector<Double>(1,SynthesisUtils::conjFreq(beamFreqs[0], centerFreq)));
	///Increment should go in the reverse direction
	////Do a tabular spectral coordinate for more than 1 channel 
	if(beamFreqs.nelements() >1){
	  Vector<Double> conjFreqs(beamFreqs.nelements());
	  for (uInt kk=0; kk< beamFreqs.nelements(); ++kk){
	    //conjFreqs[kk]=sqrt(2*centerFreq*centerFreq-beamFreqs(kk)*beamFreqs(kk));
	    conjFreqs[kk]=SynthesisUtils::conjFreq(beamFreqs[kk], centerFreq);
	  }
	  conjSpCoord=SpectralCoordinate(spCoord.frequencySystem(), conjFreqs, spCoord.restFrequency());
	  //conjSpCoord.setIncrement(Vector<Double>(1, beamFreqs(0)-beamFreqs(1)));
	}
	conjCoord.replaceCoordinate(conjSpCoord, spind);
        IPosition pbShape(4, convSize_p, convSize_p, 1, nBeamChans);
        //TempImage<Complex> twoDPB(pbShape, coords);
	
	
        TempLattice<Complex> convFuncTemp(TiledShape(IPosition(5, convSize_p/4, convSize_p/4, nBeamPols, nBeamChans, ndishpair), IPosition(5, convSize_p/4, convSize_p/4, 1, 1, 1)), 0);
        TempLattice<Complex> weightConvFuncTemp(TiledShape(IPosition(5, convSize_p/4, convSize_p/4, nBeamPols, nBeamChans, ndishpair), IPosition(5, convSize_p/4, convSize_p/4, 1, 1, 1)), 0);
        //convFunc_p.resize(IPosition(5, convSize_p, convSize_p, nBeamPols, nBeamChans, ndishpair));

        // convFunc_p=0.0;
        //weightConvFunc_p.resize(IPosition(5, convSize_p, convSize_p, nBeamPols, nBeamChans, ndishpair));
        //weightConvFunc_p=0.0;
        IPosition begin(5, 0, 0, 0, 0, 0);
        IPosition end(5, convFuncTemp.shape()[0]-1,  convFuncTemp.shape()[1]-1, nBeamPols-1, nBeamChans-1, 0);
        //FFTServer<Float, Complex> fft(IPosition(2, convSize_p, convSize_p));
        //TempImage<Complex> pBScreen(TiledShape(pbShape, IPosition(4, convSize_p, convSize_p, 1, 1)), coords, 0);
        //TempImage<Complex> pB2Screen(TiledShape(pbShape, IPosition(4, convSize_p, convSize_p, 1, 1)), coords, 0);
        Long memtot=HostInfo::memoryFree();
        Double memtobeused= Double(memtot)*1024.0;
        if(memtot <= 2000000)
            memtobeused=0.0;
        TempImage<Complex> pBScreen(TiledShape(pbShape), coords, memtobeused/2.2);
		
        TempImage<Complex> pB2Screen(TiledShape(pbShape), ((nchan_p==1) && getConjConvFunc) ?conjCoord : coords  , memtobeused/2.2);
        IPosition start(4, 0, 0, 0, 0);
        convSupport_p.resize(ndishpair);
	//////////////////
	/*Double wtime0=0.0;
	Double wtime1=0.0;
	Double wtime2=0.0;
	wtime0=omp_get_wtime()
	*/;
	//////////////
        for (uInt k=0; k < ndish; ++k) {

            for (uInt j =k ; j < ndish; ++j) {
                //Timer tim;
                //Matrix<Complex> screen(convSize_p, convSize_p);
                //screen=1.0;
                //pBScreen.putSlice(screen, start);
                //cerr << "k " << k << " shape " << pBScreen.shape() <<  " direction1 " << direction1_p << " direction2 " << direction2_p << endl;

                //pBScreen.set(Complex(1.0, 0.0));
                //one antenna
		antMath_p[k]->setBandOrFeedName(bandName_p);
		antMath_p[j]->setBandOrFeedName(bandName_p);
                IPosition blcin(4, 0, 0, 0, 0);
                IPosition trcin(4, convSize_p-1, convSize_p-1, 0, 0);
                for (Int kk=0; kk < nBeamChans; ++kk) {
                    blcin[3]=kk;
                    trcin[3]=kk;
      		    //wtime0=omp_get_wtime();
                    Slicer slin(blcin, trcin, Slicer::endIsLast);
                    SubImage<Complex> subim(pBScreen, slin, true);
                    subim.set(Complex(1.0, 0.0));
                    (antMath_p[k])->applyVP(subim, subim, direction1_p);

                    //Then the other
                    (antMath_p[j])->applyVP(subim, subim, direction2_p);
                    //tim.show("After Apply ");
                    //tim.mark();
                    //pB2Screen.set(Complex(1.0,0.0));
                    SubImage<Complex> subim2(pB2Screen, slin, true);
					subim2.set(Complex(1.0,0.0));
                    
					if(nchan_p >1 || !getConjConvFunc){
						//one antenna
						(antMath_p[k])->applyPB(subim2, subim2, direction1_p);
						//Then the other
						(antMath_p[j])->applyPB(subim2, subim2, direction2_p);
					}
					else{
						//direct frequency PB
						//cerr << "orig coords " << subim.coordinates().toWorld(IPosition(4,0,0,0,0)) << " conj coords " <<  subim2.coordinates().toWorld(IPosition(4,0,0,0,0)) << endl;
						//cerr << "incr " << subim.coordinates().increment() << "   " << subim2.coordinates().increment() << endl;
						subim2.copyData(subim);
						//Now do the conjugate freq multiplication
						(antMath_p[k])->applyVP(subim2, subim2, direction1_p);

						//Then the other
						(antMath_p[j])->applyVP(subim2, subim2, direction2_p);
						
						/*
						//one antenna
						(antMath_p[k])->applyPB(subim2, subim2, direction1_p);
						//Then the other
						(antMath_p[j])->applyPB(subim2, subim2, direction2_p);
						*/
					}
                    //tim.show("After Apply2 ");
                    //tim.mark();
					//wtime1+=omp_get_wtime()-wtime0;
                    //subim.copyData((LatticeExpr<Complex>) (iif(abs(subim)> 5e-2, subim, 0)));
                    //subim2.copyData((LatticeExpr<Complex>) (iif(abs(subim2)> 25e-4, subim2, 0)));

					//wtime0=omp_get_wtime();
					ft_p.c2cFFTInDouble(subim);
					ft_p.c2cFFTInDouble(subim2);
					//ft_p.c2cFFT(subim);
					//ft_p.c2cFFT(subim2);
					//wtime2+=omp_get_wtime()-wtime0;
                    //  tim.show("after ffts ");


                }
		//cerr << "Apply " << wtime1 << "  fft " << wtime2 << endl;
                /*
                if(nBeamChans >1){
                  Slicer slin(blcin, trcin, Slicer::endIsLast);
                  SubImage<Complex> origPB(pBScreen, slin, false);
                  IPosition elshape= origPB.shape();
                  Matrix<Complex> i1=origPB.get(true);
                  SubImage<Complex> origPB2(pB2Screen, slin, false);
                  Matrix<Complex> i2=origPB2.get(true);
                  Int cenX=i1.shape()(0)/2;
                  Int cenY=i1.shape()(1)/2;

                  for (Int kk=0; kk < (nBeamChans-1); ++kk){
                    Double fratio=beamFreqs(kk)/beamFreqs(nBeamChans-1);
                    cerr << "fratio " << fratio << endl;
                    blcin[3]=kk;
                    trcin[3]=kk;
                    //Slicer slout(blcin, trcin, Slicer::endIsLast);
                    Matrix<Complex> o1(i1.shape(), Complex(0.0));
                    Matrix<Complex> o2(i2.shape(), Complex(0.0));
                    for (Int yy=0;  yy < i1.shape()(1); ++yy){
                      //Int nyy= (Double(yy-cenY)*fratio) + cenY;
                      Double nyy= (Double(yy-cenY)/fratio) + cenY;
                      Double cyy=ceil(nyy);
                      Double fyy= floor(nyy);
                      Int iy=nyy > fyy+0.5 ? cyy : fyy;
                      if(cyy <2*cenY && fyy >=0.0)
                 for(Int xx=0; xx < i1.shape()(0); ++ xx){
                   //Int nxx= Int(Double(xx-cenX)*fratio) + cenX;
                   Double nxx= Int(Double(xx-cenX)/fratio) + cenX;
                    Double cxx=ceil(nxx);
                    Double fxx= floor(nxx);
                    Int ix=nxx > fxx+0.5 ? cxx : fxx ;
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
                    pBScreen.putSlice(o1.reform(elshape), blcin);
                    pB2Screen.putSlice(o2.reform(elshape), blcin);
                  }

                }
                */

                //tim.show("after apply+apply2+masking+fft ");
                //tim.mark();
                //LatticeFFT::cfft2d(pBScreen);
                //LatticeFFT::cfft2d(pB2Screen);

                //Matrix<Complex> lala=pBScreen.get(true);
                //fft.fft0(lala, true);
                //fft.flip(lala, false, false);
                // pBScreen.put(lala.reform(IPosition(4, convSize_p, convSize_p, 1, 1)));
                //lala=pB2Screen.get(true);
                //fft.fft0(lala, true);
                //fft.flip(lala, false, false);
                //pB2Screen.put(lala.reform(IPosition(4, convSize_p, convSize_p, 1, 1)));

                //////////*****************
                /*if(0){
                  ostringstream os1;
                  os1 << "PB_field_" << Int(thePix_p[0]) << "_" << Int(thePix_p[1]) << "_antpair_" << k <<"_"<<j ;
                  PagedImage<Float> thisScreen(pbShape, coords, String(os1));
                  LatticeExpr<Float> le(abs(pBScreen));
                  thisScreen.copyData(le);
                  }*/
                ////////*****************/

                //tim.show("after FFT ");
                //tim.mark();
                Int plane=0;
                for (uInt jj=0; jj < k; ++jj)
                    plane=plane+ndish-jj-1;
                plane=plane+j;
                begin[4]=plane;
                end[4]=plane;
                Slicer slplane(begin, end, Slicer::endIsLast);
                //cerr <<  "SHAPES " << convFunc_p(begin, end).shape() << "  " << pBScreen.get(false).shape() << " begin and end " << begin << "    " << end << endl;
                //convFunc_p(begin, end).copyMatchingPart(pBScreen.get(false));
                //weightConvFunc_p(begin, end).copyMatchingPart(pB2Screen.get(false));
                IPosition blcQ(4, pbShape(0)/8*3, pbShape(1)/8*3, 0, 0);
                IPosition trcQ(4,  blcQ[0]+ pbShape(0)/4-1, blcQ[1]+pbShape(1)/4-1 , nBeamPols-1, nBeamChans-1);

                //cerr << "blcQ " << blcQ << " trcQ " << trcQ << " pbShape " << pbShape << endl;
                Slicer slQ(blcQ, trcQ, Slicer::endIsLast);
                {
                    SubImage<Complex>  pBSSub(pBScreen, slQ, false);
                    SubLattice<Complex> cFTempSub(convFuncTemp,  slplane, true);
                    LatticeConcat<Complex> lc(4);
                    lc.setLattice(pBSSub);
                    //cerr << "SHAPES " << cFTempSub.shape() << "   " <<  lc.shape() << endl;
                    cFTempSub.copyData(lc);
                    //cFTempSub.copyData(pBScreen);
                }
                {
                    SubImage<Complex>  pB2SSub(pB2Screen, slQ, false);
                    SubLattice<Complex> cFTempSub2(weightConvFuncTemp,  slplane, true);
                    LatticeConcat<Complex> lc(4);
                    lc.setLattice(pB2SSub);
                    cFTempSub2.copyData(lc);
                    // cFTempSub2.copyData(pB2Screen);
                    //weightConvFuncTemp.putSlice(pB2Screen.get(false), begin);

                }
                //	  supportAndNormalize(plane, convSampling);
                supportAndNormalizeLatt( plane, convSampling, convFuncTemp,  weightConvFuncTemp);



                // tim.show("After search of support ");
            }

        }


        doneMainConv_p[actualConvIndex_p]=true;
        convFunctions_p.resize(actualConvIndex_p+1);
        convWeights_p.resize(actualConvIndex_p+1);
        convSupportBlock_p.resize(actualConvIndex_p+1);
	//Using conjugate change support to be larger of either
	if((nchan_p == 1) && getConjConvFunc) {
	  Int conjsupp=conjSupport(beamFreqs) ;
	  if(conjsupp > max(convSupport_p)){
	    convSupport_p=conjsupp;
	  }

	}
        convFunctions_p[actualConvIndex_p]= new Array<Complex>();
        convWeights_p[actualConvIndex_p]= new Array<Complex>();
        convSupportBlock_p[actualConvIndex_p]=new Vector<Int>();
        Int newConvSize=2*(max(convSupport_p)+2)*convSampling;
        Int newRealConvSize=newConvSize* Int(Double(convSamp)/Double(convSampling));
        Int lattSize=convFuncTemp.shape()(0);
        (*convSupportBlock_p[actualConvIndex_p])=convSupport_p;
        LogIO os(LogOrigin("HetArrConvFunc", "findConvFunction", WHERE));
        os << "convolution function support: " << convSupport_p  << LogIO::POST;

        if(newConvSize < lattSize) {
            IPosition blc(5, (lattSize/2)-(newConvSize/2),
                          (lattSize/2)-(newConvSize/2),0,0,0);
            IPosition trc(5, (lattSize/2)+(newConvSize/2-1),
                          (lattSize/2)+(newConvSize/2-1), nBeamPols-1, nBeamChans-1,ndishpair-1);
            IPosition shp(5, newConvSize, newConvSize, nBeamPols, nBeamChans, ndishpair);

            convFunctions_p[actualConvIndex_p]= new Array<Complex>(IPosition(5, newRealConvSize, newRealConvSize, nBeamPols, nBeamChans, ndishpair ));
            convWeights_p[actualConvIndex_p]= new Array<Complex>(IPosition(5, newRealConvSize, newRealConvSize, nBeamPols, nBeamChans, ndishpair ));
            (*convFunctions_p[actualConvIndex_p])=resample(convFuncTemp.getSlice(blc,shp),Double(convSamp)/Double(convSampling));
            convSize_p=newRealConvSize;
            (*convWeights_p[actualConvIndex_p])=resample(weightConvFuncTemp.getSlice(blc, shp),Double(convSamp)/Double(convSampling));
	    //cerr << "nchan " << nchan_p << " getconj " << getConjConvFunc << endl;
       
        }
        else {
            newRealConvSize=lattSize* Int(Double(convSamp)/Double(convSampling));
            convFunctions_p[actualConvIndex_p]= new Array<Complex>(IPosition(5, newRealConvSize, newRealConvSize, nBeamPols, nBeamChans, ndishpair ));
            convWeights_p[actualConvIndex_p]= new Array<Complex>(IPosition(5, newRealConvSize, newRealConvSize, nBeamPols, nBeamChans, ndishpair ));

            (*convFunctions_p[actualConvIndex_p])=resample(convFuncTemp.get(),  Double(convSamp)/Double(convSampling));
            (*convWeights_p[actualConvIndex_p])=resample(weightConvFuncTemp.get(),  Double(convSamp)/Double(convSampling));
            convSize_p=newRealConvSize;
        }


	if((nchan_p == 1) && getConjConvFunc) {
	  fillConjConvFunc(beamFreqs);
	  /////////////////////////TESTOOO
	  /*PagedImage<Complex> SCREEN2(TiledShape(convFunctions_p[actualConvIndex_p]->shape()), TMP, "CONJU"+String::toString(actualConvIndex_p));
	  SCREEN2.put(*convFunctionsConjFreq_p[actualConvIndex_p]  );
	  */
	  ////////////////////////
	}

        convFunc_p.resize();
        weightConvFunc_p.resize();

    }
    else {
        convSize_p=max(*convSizes_p[actualConvIndex_p]);
        convSupport_p.resize();
        convSupport_p=*convSupportBlock_p[actualConvIndex_p];
    }
    /*
    rowMap.resize(vb.nRow());
    for (Int k=0; k < vb.nRow(); ++k){
      //plane of convfunc that match this pair of antennas
      rowMap(k)=antIndexToDiamIndex_p(vb.antenna1()(k))*ndish+
    antIndexToDiamIndex_p(vb.antenna2()(k));

    }
    */
    ////////////////TESTOOO
    //		 CoordinateSystem TMP = coords;
    //	  CoordinateUtil::addLinearAxes(TMP, Vector<String>(1,"gulu"), IPosition(1,nBeamChans)); 
    //	  PagedImage<Complex> SCREEN(TiledShape(convFunctions_p[actualConvIndex_p]->shape()), TMP, "NONCONJUVI2"+String::toString(actualConvIndex_p));
    //	  SCREEN.put(*convFunctions_p[actualConvIndex_p]  );
    //	   PagedImage<Complex> SCREEN3(TiledShape(convWeights_p[actualConvIndex_p]->shape()), TMP, "FTWEIGHTVI2"+String::toString(actualConvIndex_p));
    //	  SCREEN3.put(*convWeights_p[actualConvIndex_p]  );
	
    /////////////////

    makerowmap(vb, convFuncRowMap);
    ///need to deal with only the maximum of different baselines available in this
    ///vb
    ndishpair=max(convFuncRowMap)+1;

    convSupportBlock_p.resize(actualConvIndex_p+1);
    convSizes_p.resize(actualConvIndex_p+1);
    //convSupportBlock_p[actualConvIndex_p]=new Vector<Int>(ndishpair);
    //(*convSupportBlock_p[actualConvIndex_p])=convSupport_p;
    convSizes_p[actualConvIndex_p]=new Vector<Int> (ndishpair);

    /*    convFunctions_p[actualConvIndex_p]->resize(convSize_p, convSize_p, ndishpair);
    *(convFunctions_p[actualConvIndex_p])=convSave_p;
    convWeights_p[actualConvIndex_p]->resize(convSize_p, convSize_p, ndishpair);
    *(convWeights_p[actualConvIndex_p])=weightSave_p;
    */

    convFunc_p.resize();
	if((nchan_p == 1) && getConjConvFunc) {
	  // cerr << this << " recovering " << actualConvIndex_p <<  "   " <<convFunctionsConjFreq_p.size() << endl;
	  if(Int(convFunctionsConjFreq_p.size()) <= actualConvIndex_p){
	    fillConjConvFunc(beamFreqs);
	    
	  }
		convFunc_p=(*convFunctionsConjFreq_p[actualConvIndex_p]);
	}
	else{
		
		convFunc_p=(*convFunctions_p[actualConvIndex_p]);
	}
	
    weightConvFunc_p.resize();
    weightConvFunc_p=(*convWeights_p[actualConvIndex_p]);


    // cerr << "convfunc shapes " <<  convFunc_p.shape() <<  "   " << weightConvFunc_p.shape() << "  " << convSize_p << " pol " << nBeamPols << "  chan " << nBeamChans << " ndishpair " << ndishpair << endl;
    //convSupport_p.resize();
    //convSupport_p=(*convSupportBlock_p[actualConvIndex_p]);
    Bool delc;
    Bool delw;
    Double dirX=pixFieldDir(0);
    Double dirY=pixFieldDir(1);
    Complex *convstor=convFunc_p.getStorage(delc);
    Complex *weightstor=weightConvFunc_p.getStorage(delw);
    Int elconvsize=convSize_p;

    #pragma omp parallel default(none) firstprivate(convstor, weightstor, dirX, dirY, elconvsize, ndishpair, nBeamChans, nBeamPols)
    {
        #pragma omp for
        for(Int iy=0; iy<elconvsize; ++iy) {
            applyGradientToYLine(iy,  convstor, weightstor, dirX, dirY, elconvsize, ndishpair, nBeamChans, nBeamPols);

        }
    }///End of pragma

    convFunc_p.putStorage(convstor, delc);
    weightConvFunc_p.putStorage(weightstor, delw);



    //For now all have the same size convsize;
    convSizes_p[actualConvIndex_p]->set(convSize_p);

    //We have to get the references right now
    //    convFunc_p.resize();
    //convFunc_p.reference(*convFunctions_p[actualConvIndex_p]);
    //weightConvFunc_p.resize();
    //weightConvFunc_p.reference(*convWeights_p[actualConvIndex_p]);

    convFunc.reference(convFunc_p);
    weightConvFunc.reference(weightConvFunc_p);
    convsize=*convSizes_p[actualConvIndex_p];
    convSupport=convSupport_p;


}

typedef unsigned long long ooLong;

void HetArrayConvFunc::applyGradientToYLine(const Int iy, Complex*& convFunctions, Complex*& convWeights, const Double pixXdir, const Double pixYdir, Int convSize, const Int ndishpair, const Int nChan, const Int nPol) {
    Double cy, sy;

    SINCOS(Double(iy-convSize/2)*pixYdir, sy, cy);
    Complex phy(cy,sy) ;
    for (Int ix=0; ix<convSize; ix++) {
        Double cx, sx;
        SINCOS(Double(ix-convSize/2)*pixXdir, sx, cx);
        Complex phx(cx,sx) ;
        for (Int ipol=0; ipol< nPol; ++ipol) {
            //Int poloffset=ipol*nChan*ndishpair*convSize*convSize;
            for (Int ichan=0; ichan < nChan; ++ichan) {
                //Int chanoffset=ichan*ndishpair*convSize*convSize;
                for(Int iz=0; iz <ndishpair; ++iz) {
                    ooLong index=((ooLong(iz*nChan+ichan)*nPol+ipol)*ooLong(convSize)+ooLong(iy))*ooLong(convSize)+ooLong(ix);
                    convFunctions[index]= convFunctions[index]*phx*phy;
                    convWeights[index]= convWeights[index]*phx*phy;
                }
            }
        }

    }
}
Int  HetArrayConvFunc::conjSupport(const casacore::Vector<casacore::Double>& freqs){
  Double centerFreq=SpectralImageUtil::worldFreq(csys_p, 0.0);
  Double maxRatio=-1.0;
  for (Int k=0; k < freqs.shape()[0]; ++k) {
    //Double conjFreq=(centerFreq-freqs[k])+centerFreq;
    Double conjFreq=SynthesisUtils::conjFreq(freqs[k], centerFreq);
    if(maxRatio < conjFreq/freqs[k] )
      maxRatio=conjFreq/freqs[k];
  }
  return  Int(max(convSupport_p)*sqrt(maxRatio)/2.0)*2;
}
void HetArrayConvFunc::fillConjConvFunc(const Vector<Double>& freqs) {
    //cerr << "Actualconv index " << actualConvIndex_p << endl;
    convFunctionsConjFreq_p.resize(actualConvIndex_p+1);
    Double centerFreq=SpectralImageUtil::worldFreq(csys_p, 0.0);
    IPosition shp=convFunctions_p[actualConvIndex_p]->shape();
    convFunctionsConjFreq_p[actualConvIndex_p]=new Array<Complex>(shp, Complex(0.0));
    //cerr << "convsize " << convSize_p << " convsupport " << convSupport_p << endl;
    /*
    Double maxRatio=-1.0;
    for (Int k=0; k < freqs.shape()[0]; ++k) {
      Double conjFreq=(centerFreq-freqs[k])+centerFreq;
      if(maxRatio < conjFreq/freqs[k] )
	maxRatio=conjFreq/freqs[k];
    }
    */
    IPosition blc(5,0,0,0,0,0);
    IPosition trc=shp-1;
    /*
    IPosition trcOut=trc;
    IPosition trcOut(0)= Int(shp(0)*maxRatio/2.0)*2-1;
    IPosition trcOut(1)= Int(shp(1)*maxRatio/2.0)*2-1;
    */
    for (Int k=0; k < freqs.shape()[0]; ++k) {
      //Double conjFreq=(centerFreq-freqs[k])+centerFreq;
      Double conjFreq=SynthesisUtils::conjFreq(freqs[k], centerFreq);
        blc[3]=k;
        trc[3]=k;
        //cerr << "blc " << blc << " trc "<< trc << " ratio " << conjFreq/freqs[k] << endl; 
        //Matrix<Complex> convSlice((*convFunctions_p[actualConvIndex_p])(blc, trc).reform(IPosition(2, shp[0], shp[1])));
        Array<Complex> convSlice((*convFunctions_p[actualConvIndex_p])(blc, trc));
	//Array<Complex> weightSlice((*convWeights_p[actualConvIndex_p])(blc,trc));
        Array<Complex> conjFreqSlice(resample(convSlice, conjFreq/freqs[k]));
	Double ratio1= Double(Int(Double(convSlice.shape()(0))*conjFreq/freqs[k]/2.0)*2)/Double(convSlice.shape()(0));
	Double ratio2= Double(Int(Double(convSlice.shape()(1))*conjFreq/freqs[k]/2.0)*2)/Double(convSlice.shape()(1));
	//cerr << "resample shape " << conjFreqSlice.shape()  << " ratio " << ratio1*ratio2 << " trc " << trc << endl; 
        Array<Complex> conjSlice=(*convFunctionsConjFreq_p[actualConvIndex_p])(blc, trc);
        if(conjFreq > freqs[k]) {
            IPosition end=shp-1;
            IPosition beg(5,0,0,0,0,0);
            beg(0)=(conjFreqSlice.shape()(0)-shp(0))/2;
            beg(1)=(conjFreqSlice.shape()(1)-shp(1))/2;
			end(0)=beg(0)+shp(0)-1;
			end(1)=beg(1)+shp(1)-1;
            end[3]=0;
            conjSlice=conjFreqSlice(beg, end);
        }
        else {
            IPosition end=conjFreqSlice.shape()-1;
            end[3]=0;
            IPosition beg(5,0,0,0,0,0);
            beg(0)=(shp(0)-conjFreqSlice.shape()(0))/2;
            beg(1)=(shp(1)-conjFreqSlice.shape()(1))/2;
			end(0)+=beg(0);
			end(1)+=beg(1);
            conjSlice(beg, end)=conjFreqSlice;
        }
	//cerr << "SUMS " << sum(real(convSlice)) << "   new " << sum(real(conjSlice))/ratio1/ratio2 << endl; //" weight " << sum(real(weightSlice))/ratio1/ratio2<< endl;
	Complex renorm( 1.0/(ratio1*ratio2),0.0);
	conjSlice=conjSlice*renorm;
	//weightSlice=weightSlice*Complex(1.0/(ratio1*ratio2),0.0);

    }
   

}
Bool HetArrayConvFunc::toRecord(RecordInterface& rec) {

    try {
        rec.define("name", "HetArrayConvFunc");
        Int numConv=convFunctions_p.nelements();
        rec.define("ndefined", numConv);
        //rec.define("convfunctionmap", convFunctionMap_p);
        std::map<String, Int>::iterator it=vbConvIndex_p.begin();
        for (Int64 k=0; k < numConv; ++k) {
            rec.define("convfunctions"+String::toString(k), *(convFunctions_p[k]));
            rec.define("convweights"+String::toString(k), *(convWeights_p[k]));
            rec.define("convsizes"+String::toString(k), *(convSizes_p[k]));
            rec.define("convsupportblock"+String::toString(k), *(convSupportBlock_p[k]));
            rec.define(String("key")+String::toString(k), it->first);
            rec.define(String("val")+String::toString(k), it->second);
            it++;
        }
        rec.define("actualconvindex",  actualConvIndex_p);
        rec.define("donemainconv", doneMainConv_p);
        rec.define("vptable", vpTable_p);
        rec.define("pbclass", Int(pbClass_p));

    }
    catch(AipsError& x) {
        return false;
    }
    return true;

}

Bool HetArrayConvFunc::fromRecord(String& err, const RecordInterface& rec, Bool calcfluxscale) {
    //Force pbmath stuff and saved image stuff
    nchan_p=0;
    msId_p=-1;
    try {
        if(!rec.isDefined("name") || rec.asString("name") != "HetArrayConvFunc") {
            throw(AipsError("Wrong record to recover HetArray from"));
        }
        nDefined_p=rec.asInt("ndefined");
        //rec.get("convfunctionmap", convFunctionMap_p);
        convFunctions_p.resize(nDefined_p, true, false);
        convSupportBlock_p.resize(nDefined_p, true, false);
        convWeights_p.resize(nDefined_p, true, false);
        convSizes_p.resize(nDefined_p, true, false);
        vbConvIndex_p.erase(vbConvIndex_p.begin(), vbConvIndex_p.end());
        for (Int64 k=0; k < nDefined_p; ++k) {
            convFunctions_p[k]=new Array<Complex>();
            convWeights_p[k]=new Array<Complex>();
            convSizes_p[k]=new Vector<Int>();
            convSupportBlock_p[k]=new Vector<Int>();
            rec.get("convfunctions"+String::toString(k), *(convFunctions_p[k]));
            rec.get("convweights"+String::toString(k), *(convWeights_p[k]));
            rec.get("convsizes"+String::toString(k), *(convSizes_p[k]));
            rec.get("convsupportblock"+String::toString(k), *(convSupportBlock_p[k]));
            String key;
            Int val;
            rec.get(String("key")+String::toString(k), key);
            rec.get(String("val")+String::toString(k), val);
            vbConvIndex_p[key]=val;
        }
        //Now that we are calculating all phase gradients on the fly ..
        ///we should clean up some and get rid of the cached variables

        convSize_p= nDefined_p > 0 ? (*(convSizes_p[0]))[0] : 0;
        //convSave_p.resize();
        //rec.get("convsave", convSave_p);
        //weightSave_p.resize();
        //rec.get("weightsave", weightSave_p);
        rec.get("vptable", vpTable_p);
        rec.get("donemainconv", doneMainConv_p);
        //convSupport_p.resize();
        //rec.get("convsupport", convSupport_p);
        pbClass_p=static_cast<PBMathInterface::PBClass>(rec.asInt("pbclass"));
        calcFluxScale_p=calcfluxscale;
    }
    catch(AipsError& x) {
        err=x.getMesg();
        return false;
    }

    return true;
}


void HetArrayConvFunc::supportAndNormalize(Int plane, Int convSampling) {

    LogIO os;
    os << LogOrigin("HetArrConvFunc", "suppAndNorm")  << LogIO::NORMAL;
    // Locate support
    Int convSupport=-1;
    IPosition begin(5, 0, 0, 0, 0, plane);
    IPosition end(5, convFunc_p.shape()[0]-1,  convFunc_p.shape()[1]-1, 0, 0, plane);
    Matrix<Complex> convPlane(convFunc_p(begin, end).reform(IPosition(2,convFunc_p.shape()[0], convFunc_p.shape()[1]))) ;
    Float maxAbsConvFunc=max(amplitude(convPlane));
    Float minAbsConvFunc=min(amplitude(convPlane));
    Bool found=false;
    Int trial=0;
    for (trial=convSize_p/2-2; trial>0; trial--) {
        //Searching down a diagonal
        if(abs(convPlane(convSize_p/2-trial,convSize_p/2-trial)) >  (1.0e-2*maxAbsConvFunc) ) {
            found=true;
            trial=Int(sqrt(2.0*Float(trial*trial)));
	   
            break;
        }
    }
    if(!found) {
        if((maxAbsConvFunc-minAbsConvFunc) > (1.0e-2*maxAbsConvFunc))
            found=true;
        // if it drops by more than 2 magnitudes per pixel
        trial=( (10*convSampling) < convSize_p) ? 5*convSampling : (convSize_p/2 - 4*convSampling);
    }


    if(found) {
        if(trial < 5*convSampling)
            trial= ( (10*convSampling) < convSize_p) ? 5*convSampling : (convSize_p/2 - 4*convSampling);
        convSupport=Int(0.5+Float(trial)/Float(convSampling))+1;
        //support is really over the edge
        if( (convSupport*convSampling) >= convSize_p/2) {
            convSupport=convSize_p/2/convSampling-1;
        }
    }
    else {
        /*
        os << "Convolution function is misbehaved - support seems to be zero\n"
           << "Reasons can be: \nThe image definition not covering one or more of the pointings selected \n"
           << "Or no unflagged data in a given pointing"

           << LogIO::EXCEPTION;
        */
        //OTF may have flagged stuff ...
        convSupport=0;
    }
    //cerr << "trial " << trial << " convSupport " << convSupport << " convSize " << convSize_p << endl;
    convSupport_p(plane)=convSupport;
    Double pbSum=0.0;
    /*
    Double pbSum1=0.0;

    for (Int iy=-convSupport;iy<=convSupport;iy++) {
      for (Int ix=-convSupport;ix<=convSupport;ix++) {
        Complex val=convFunc_p.xyPlane(plane)(ix*convSampling+convSize_p/2,
    					  iy*convSampling+convSize_p/2);

        pbSum1+=sqrt(real(val)*real(val)+ imag(val)*imag(val));
      }
    }

    */
    if(convSupport >0) {
        IPosition blc(2, -convSupport*convSampling+convSize_p/2, -convSupport*convSampling+convSize_p/2);
        IPosition trc(2, convSupport*convSampling+convSize_p/2, convSupport*convSampling+convSize_p/2);
        for (Int chan=0; chan < convFunc_p.shape()[3]; ++chan) {
            begin[3]=chan;
            end[3]=chan;
            convPlane.resize();
            convPlane.reference(convFunc_p(begin, end).reform(IPosition(2,convFunc_p.shape()[0], convFunc_p.shape()[1])));
            pbSum=real(sum(convPlane(blc,trc)))/Double(convSampling)/Double(convSampling);
            if(pbSum>0.0) {
                (convPlane)=convPlane*Complex(1.0/pbSum,0.0);
                convPlane.resize();
                convPlane.reference(weightConvFunc_p(begin, end).reform(IPosition(2,convFunc_p.shape()[0], convFunc_p.shape()[1])));
		 
                (convPlane) =(convPlane)*Complex(1.0/pbSum,0.0);
            }
            else {
                os << "Convolution function integral is not positive"
                   << LogIO::EXCEPTION;
            }
        }
    }
    else {
        //no valid convolution for this pointing
        for (Int chan=0; chan < convFunc_p.shape()[3]; ++chan) {
            begin[3]=chan;
            end[3]=chan;
            convFunc_p(begin, end).set(Complex(0.0));
            weightConvFunc_p(begin, end).set(Complex(0.0));
            //convFunc_p.xyPlane(plane).set(0.0);
            //weightConvFunc_p.xyPlane(plane).set(0.0);
        }
    }

}

void HetArrayConvFunc::supportAndNormalizeLatt(Int plane, Int convSampling, TempLattice<Complex>& convFuncLat,
        TempLattice<Complex>& weightConvFuncLat) {

    LogIO os;
    os << LogOrigin("HetArrConvFunc", "suppAndNorm")  << LogIO::NORMAL;
    // Locate support
    Int convSupport=-1;
    ///Use largest channel as highest freq thus largest conv func
    IPosition begin(5, 0, 0, 0, convFuncLat.shape()(3)-1, plane);
    IPosition shape(5, convFuncLat.shape()[0],  convFuncLat.shape()[1], 1, 1, 1);
    //Int convSize=convSize_p;
    Int convSize=shape(0);
    ///use FT weightconvlat as it is wider
    Matrix<Complex> convPlane=weightConvFuncLat.getSlice(begin, shape, true);
    Float maxAbsConvFunc, minAbsConvFunc;
    IPosition minpos, maxpos;
    minMax(minAbsConvFunc, maxAbsConvFunc, minpos, maxpos, amplitude(convPlane));
     Bool found=false;
    Int trial=0;
    Float cutlevel=2.5e-2;
    //numeric needs a larger ft
    for (uInt k=0; k < antMath_p.nelements() ; ++k){
      if((antMath_p[k]->whichPBClass()) == PBMathInterface::NUMERIC)
	cutlevel=5e-3;
    }

    for (trial=0; trial< (convSize-max(maxpos.asVector())-2); ++trial) {
      ///largest along either axis
      //cerr << "rat1 " << abs(convPlane(maxpos[0]-trial,maxpos[1]))/maxAbsConvFunc << " rat2 " << abs(convPlane(maxpos[0],maxpos[1]-trial))/maxAbsConvFunc << endl;
      if((abs(convPlane(maxpos[0]-trial, maxpos[1])) <  (cutlevel*maxAbsConvFunc)) &&(abs(convPlane(maxpos[0],maxpos[1]-trial)) <  (cutlevel*maxAbsConvFunc)) )
	{

            found=true;
            //trial=Int(sqrt(2.0*Float(trial*trial)));
	    
            break;
        }
    }
    if(!found) {
      if((maxAbsConvFunc-minAbsConvFunc) > (cutlevel*maxAbsConvFunc))
            found=true;
        // if it drops by more than 2 magnitudes per pixel
        //trial=( (10*convSampling) < convSize) ? 5*convSampling : (convSize/2 - 4*convSampling);
      trial=convSize/2 - 4*convSampling;
    }

    if(found) {
        if(trial < 5*convSampling)
            trial= ( (10*convSampling) < convSize) ? 5*convSampling : (convSize/2 - 4*convSampling);
        convSupport=(Int(0.5+Float(trial)/Float(convSampling)))+1 ;
	//cerr << "convsupp " << convSupport << endl;
        //support is really over the edge
        if( (convSupport*convSampling) >= convSize/2) {
            convSupport=convSize/2/convSampling-1;
        }
    }
    else {
        /*
        os << "Convolution function is misbehaved - support seems to be zero\n"
           << "Reasons can be: \nThe image definition not covering one or more of the pointings selected \n"
           << "Or no unflagged data in a given pointing"

           << LogIO::EXCEPTION;
        */
        //OTF may have flagged stuff ...
        convSupport=0;
    }
    convSupport_p(plane)=convSupport;
    Double pbSum=0.0;
    /*
    Double pbSum1=0.0;

    for (Int iy=-convSupport;iy<=convSupport;iy++) {
      for (Int ix=-convSupport;ix<=convSupport;ix++) {
        Complex val=convFunc_p.xyPlane(plane)(ix*convSampling+convSize_p/2,
    					  iy*convSampling+convSize_p/2);

        pbSum1+=sqrt(real(val)*real(val)+ imag(val)*imag(val));
      }
    }

    */
    //cerr << "convSize_p " << convSize_p <<  " convSize " << convSize << endl;
    if(convSupport >0) {
        IPosition blc(2, -convSupport*convSampling+convSize/2, -convSupport*convSampling+convSize/2);
        IPosition trc(2, convSupport*convSampling+convSize/2, convSupport*convSampling+convSize/2);
        for (Int chan=0; chan < convFuncLat.shape()[3]; ++chan) {
            begin[3]=chan;
            //end[3]=chan;
            convPlane.resize();
            convPlane=convFuncLat.getSlice(begin, shape, true);
            pbSum=real(sum(convPlane(blc,trc)))/Double(convSampling)/Double(convSampling);
            if(pbSum>0.0) {
                (convPlane)=convPlane*Complex(1.0/pbSum,0.0);
                convFuncLat.putSlice(convPlane, begin);
                convPlane.resize();
                convPlane=weightConvFuncLat.getSlice(begin, shape, true);
		Double pbSum1=0.0;
		pbSum1=real(sum(convPlane(blc,trc)))/Double(convSampling)/Double(convSampling);
                (convPlane) =(convPlane)*Complex(1.0/pbSum1,0.0);
                weightConvFuncLat.putSlice(convPlane, begin);
            }
            else {
                os << "Convolution function integral is not positive"
                   << LogIO::EXCEPTION;
            }
        }
    }
    else {
        //no valid convolution for this pointing
        for (Int chan=0; chan < convFuncLat.shape()[3]; ++chan) {
            begin[3]=chan;
            //end[3]=chan;
            convPlane.resize(shape[0], shape[1]);
            convPlane.set(Complex(0.0));
            convFuncLat.putSlice(convPlane, begin);
            weightConvFuncLat.putSlice(convPlane, begin);
            //convFunc_p.xyPlane(plane).set(0.0);
            //weightConvFunc_p.xyPlane(plane).set(0.0);
        }
    }

}

Int HetArrayConvFunc::factorial(Int n) {
    Int fact=1;
    for (Int k=1; k<=n; ++k)
        fact *=k;
    return fact;
}


Int HetArrayConvFunc::checkPBOfField(const vi::VisBuffer2& vb,
                                     Vector<Int>& /*rowMap*/, const MVDirection& extraShift, const Bool useExtraShift) {

  toPix(vb, extraShift, useExtraShift);
    Vector<Int> pixdepoint(2);
    convertArray(pixdepoint, thePix_p);
    if((pixdepoint(0) < 0) ||  pixdepoint(0) >= nx_p || pixdepoint(1) < 0 ||
            pixdepoint(1) >=ny_p) {
        //cout << "in pix de point off " << pixdepoint << endl;
        return 2;
    }
    String pointingid=String::toString(pixdepoint(0))+"_"+String::toString(pixdepoint(1));
    //Int fieldid=vb.fieldId();
    String msid=vb.msName(true);
    //If channel or pol length has changed underneath...then its time to
    //restart the map
    /*
    if(convFunctionMap_p.ndefined() > 0){
      if ((fluxScale_p.shape()[3] != nchan_p) || (fluxScale_p.shape()[2] != npol_p)){
    convFunctionMap_p.clear();
      }
    }

    */
    if(convFunctionMap_p.nelements() > 0) {
        if (calcFluxScale_p && ((fluxScale_p.shape()[3] != nchan_p) || (fluxScale_p.shape()[2] != npol_p))) {
            convFunctionMap_p.resize();
            nDefined_p=0;
        }
    }
    //String mapid=msid+String("_")+pointingid;
    /*
    if(convFunctionMap_p.ndefined() == 0){
      convFunctionMap_p.define(mapid, 0);
      actualConvIndex_p=0;
      fluxScale_p=TempImage<Float>(IPosition(4,nx_p,ny_p,npol_p,nchan_p), csys_p);
      filledFluxScale_p=false;
      fluxScale_p.set(0.0);
      return -1;
    }
    */
    if(convFunctionMap_p.nelements() == 0) {
        convFunctionMap_p.resize(nx_p*ny_p);
        convFunctionMap_p.set(-1);
        convFunctionMap_p[pixdepoint[1]*nx_p+pixdepoint[0]]=0;
        nDefined_p=1;
        actualConvIndex_p=0;
        if(calcFluxScale_p) {
            fluxScale_p=TempImage<Float>(IPosition(4,nx_p,ny_p,npol_p,nchan_p), csys_p);
            filledFluxScale_p=false;
            fluxScale_p.set(0.0);
        }
        return -1;
    }

    // if(!convFunctionMap_p.isDefined(mapid)){
    //  actualConvIndex_p=convFunctionMap_p.ndefined();
    //  convFunctionMap_p.define(mapid, actualConvIndex_p);
    if(convFunctionMap_p[pixdepoint[1]*nx_p+pixdepoint[0]] <0) {
        actualConvIndex_p=nDefined_p;
        convFunctionMap_p[pixdepoint[1]*nx_p+pixdepoint[0]]=nDefined_p;
        // ++nDefined_p;
        nDefined_p=1;
        return -1;
    }
    else {
        /*
        actualConvIndex_p=convFunctionMap_p[pixdepoint[1]*nx_p+pixdepoint[0]];
        convFunc_p.resize(); // break any reference
        weightConvFunc_p.resize();
        convSupport_p.resize();
        //Here we will need to use the right xyPlane for different PA range
        //and frequency may be
        convFunc_p.reference(*convFunctions_p[actualConvIndex_p]);
        weightConvFunc_p.reference(*convWeights_p[actualConvIndex_p]);
        //Again this for one time of antenna only later should be fixed for all
        // antennas independently
        //these are not really needed right now
        convSupport_p=(*convSupportBlock_p[actualConvIndex_p]);
        convSize_p=(*convSizes_p[actualConvIndex_p])[0];
        makerowmap(vb, rowMap);
        */
        actualConvIndex_p=0;
        return -1;
    }

    return 1;


}

void HetArrayConvFunc::makerowmap(const vi::VisBuffer2& vb,
                                  Vector<Int>& rowMap) {

    uInt ndish=antMath_p.nelements();
    rowMap.resize(vb.nRows());
    for (rownr_t k=0; k < vb.nRows(); ++k) {
        Int index1=antIndexToDiamIndex_p(vb.antenna1()(k));
        Int index2=antIndexToDiamIndex_p(vb.antenna2()(k));
        if(index2 < index1) {
            index1=index2;
            index2=antIndexToDiamIndex_p(vb.antenna1()(k));
        }
        Int plane=0;
        for (Int jj=0; jj < index1; ++jj)
            plane=plane+ndish-jj-1;
        plane=plane+index2;
        //plane of convfunc that match this pair of antennas
        rowMap(k)=plane;

    }

}

Array<Complex> HetArrayConvFunc::resample(const Array<Complex>& inarray, const Double factor) {

    Double nx=Double(inarray.shape()(0));
    Double ny=Double(inarray.shape()(1));
    IPosition shp=inarray.shape();
    shp(0)=Int(nx*factor/2.0)*2;
    shp(1)=Int(ny*factor/2.0)*2;
    Int newNx=shp(0);
    Int newNy=shp(1);
    
    Array<Complex> out(shp, Complex(0.0));
   // cerr << "SHP " << shp << endl;
    
   IPosition incursor=IPosition(inarray.shape().nelements(),1);
    incursor[0]=nx;
    incursor[1]=ny;
    IPosition outcursor=IPosition(inarray.shape().nelements(),1);
    outcursor[0]=shp[0];
    outcursor[1]=shp[1];
    ArrayIterator<Complex> inIt(inarray, IPosition(2,0,1), True);
    ArrayIterator<Complex> outIt(out, IPosition(2,0,1),True);
    inIt.origin();
    outIt.origin();
    //for (zzz=0; zzz< shp.(4); ++zzz){
    //  for(yyy=0; yyy< shp.(3); ++yyy){
    // for(xxx=0; xxx< shp.(2); ++xxx){
    while(!inIt.pastEnd()) {
       // cerr << "Iter shape " << inIt.array().shape() << endl;
        Matrix<Complex> inmat;
        inmat=inIt.array();    
        //Matrix<Float> leReal=real(Matrix<Complex>(inIt.array()));
        //Matrix<Float> leImag=imag(Matrix<Complex>(inIt.array()));
        Matrix<Float> leReal=real(inmat);
        Matrix<Float> leImag=imag(inmat);
        Bool leRealCopy, leImagCopy;
        Float *realptr=leReal.getStorage(leRealCopy);
        Float *imagptr=leImag.getStorage(leImagCopy);
        Bool isCopy;
        Matrix<Complex> outMat(outIt.array());
        Complex *intPtr=outMat.getStorage(isCopy);
        Float realval, imagval;
#ifdef _OPENMP
        omp_set_nested(0);
#endif
        #pragma omp parallel for default(none) private(realval, imagval) firstprivate(intPtr, realptr, imagptr, nx, ny, newNx, newNy) shared(leReal, leImag)

        for (Int k =0; k < newNy; ++k) {
            Double y =Double(k)/Double(newNy)*Double(ny);

            for (Int j=0; j < newNx; ++j) {
                //      Interpolate2D interp(Interpolate2D::LANCZOS);
                Double x=Double(j)/Double(newNx)*Double(nx);
                //interp.interp(realval, where, leReal);
                realval=interpLanczos(x , y, nx, ny,
                                      realptr);
                imagval=interpLanczos(x , y, nx, ny,
                                      imagptr);
                //interp.interp(imagval, where, leImag);
                intPtr[k*Int(newNx)+j]=Complex(realval, imagval);
            }

        }
        outMat.putStorage(intPtr, isCopy);
        leReal.putStorage(realptr, leRealCopy);
        leImag.putStorage(imagptr, leImagCopy);
        inIt.next();
        outIt.next();
    }
    return out;
}
Matrix <Complex> HetArrayConvFunc::resample2(const Matrix<Complex>& inarray, const Double factor) {

    Double nx=Double(inarray.shape()(0));
    Double ny=Double(inarray.shape()(1));
    IPosition shp=inarray.shape();
    shp(0)=Int(nx*factor/2.0)*2;
    shp(1)=Int(ny*factor/2.0)*2;

    
    Matrix<Complex> outMat(shp, Complex(0.0));
    
    
   
     {
        //cerr << "Iter shape " << inarray.shape() << endl;
        
        Matrix<Float> leReal=real(inarray);
        Matrix<Float> leImag=imag(inarray);
        Bool leRealCopy, leImagCopy;
        Float *realptr=leReal.getStorage(leRealCopy);
        Float *imagptr=leImag.getStorage(leImagCopy);
        Bool isCopy;
        Complex *intPtr=outMat.getStorage(isCopy);
        Float realval, imagval;
#ifdef _OPENMP
//        omp_set_nested(0);
#endif
 //       #pragma omp parallel for default(none) private(realval, imagval) firstprivate(intPtr, realptr, imagptr, nx, ny) shared(leReal, leImag)

        for (Int k =0; k < shp(1); ++k) {
            Double y =Double(k)/Double(shp(1))*Double(ny);

            for (Int j=0; j < Int(nx*factor); ++j) {
                //      Interpolate2D interp(Interpolate2D::LANCZOS);
                Double x=Double(j)/Double(factor);
                //interp.interp(realval, where, leReal);
                realval=interpLanczos(x , y, nx, ny,
                                      realptr);
                imagval=interpLanczos(x , y, nx, ny,
                                      imagptr);
                //interp.interp(imagval, where, leImag);
                intPtr[k*Int(nx*factor)+j]=Complex(realval, imagval);
            }

        }
        outMat.putStorage(intPtr, isCopy);
        leReal.putStorage(realptr, leRealCopy);
        leImag.putStorage(imagptr, leImagCopy);
        
     
    }
    return outMat;
}
Float HetArrayConvFunc::sinc(const Float x)  {
    if (x == 0) {
        return 1;
    }
    return sin(C::pi * x) / (C::pi * x);
}
Float HetArrayConvFunc::interpLanczos( const Double& x , const Double& y, const Double& nx, const Double& ny,   const Float* data, const Float a) {
    Double floorx = floor(x);
    Double floory = floor(y);
    Float result=0.0;
    if (floorx < a || floorx >= nx - a || floory < a || floory >= ny - a) {
        result = 0;
        return result;
    }
    for (Float i = floorx - a + 1; i <= floorx + a; ++i) {
        for (Float j = floory - a + 1; j <= floory + a; ++j) {
            result += Float(Double(data[Int(j*nx+i)]) * sinc(x - i)*sinc((x-i)/ a) * sinc(y - j)*sinc((y-j)/ a));
        }
    }
    return result;
}

ImageInterface<Float>&  HetArrayConvFunc::getFluxScaleImage() {
    if(!calcFluxScale_p)
        throw(AipsError("Programmer Error: flux image cannot be retrieved"));
    if(!filledFluxScale_p) {
        //The best flux image for a heterogenous array is the weighted coverage
        fluxScale_p.copyData(*(convWeightImage_p));
        IPosition blc(4,nx_p, ny_p, npol_p, nchan_p);
        IPosition trc(4, ny_p, ny_p, npol_p, nchan_p);
        blc(0)=0;
        blc(1)=0;
        trc(0)=nx_p-1;
        trc(1)=ny_p-1;

        for (Int j=0; j < npol_p; ++j) {
            for (Int k=0; k < nchan_p ; ++k) {

                blc(2)=j;
                trc(2)=j;
                blc(3)=k;
                trc(3)=k;
                Slicer sl(blc, trc, Slicer::endIsLast);
                SubImage<Float> fscalesub(fluxScale_p, sl, true);
                Float planeMax;
                LatticeExprNode LEN = max( fscalesub );
                planeMax =  LEN.getFloat();
                if(planeMax !=0) {
                    fscalesub.copyData( (LatticeExpr<Float>) (fscalesub/planeMax));

                }
            }
        }
        filledFluxScale_p=true;
    }


    return fluxScale_p;

}

void HetArrayConvFunc::sliceFluxScale(Int npol) {
    IPosition fshp=fluxScale_p.shape();
    if (fshp(2)>npol) {
        npol_p=npol;
        // use first npol planes...
        IPosition blc(4,0,0,0,0);
        IPosition trc(4,fluxScale_p.shape()(0)-1, fluxScale_p.shape()(1)-1,npol-1,fluxScale_p.shape()(3)-1);
        Slicer sl=Slicer(blc, trc, Slicer::endIsLast);
        //writeable if possible
        SubImage<Float> fluxScaleSub = SubImage<Float> (fluxScale_p, sl, true);
        SubImage<Float> convWeightImageSub = SubImage<Float> (*convWeightImage_p, sl, true);
        fluxScale_p = TempImage<Float>(fluxScaleSub.shape(),fluxScaleSub.coordinates());
        convWeightImage_p = new TempImage<Float> (convWeightImageSub.shape(),convWeightImageSub.coordinates());
        LatticeExpr<Float> le(fluxScaleSub);
        fluxScale_p.copyData(le);
        LatticeExpr<Float> le2(convWeightImageSub);
        convWeightImage_p->copyData(le2);
    }
}
} // namespace refim end
} //# NAMESPACE CASA - END




