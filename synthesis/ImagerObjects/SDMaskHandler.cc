//# SDMaskHandler.cc: Implementation of SDMaskHandler classes
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
#include <stack>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/OS/HostInfo.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentList.h>
#include <casacore/images/Images/ImageRegrid.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Regions/RegionManager.h>
#include <casacore/images/Regions/RegionHandler.h>
#include <casacore/images/Regions/WCBox.h>
#include <casacore/images/Regions/WCUnion.h>
#include <imageanalysis/ImageAnalysis/CasaImageBeamSet.h>
#include <imageanalysis/ImageAnalysis/ImageDecomposer.h>
#include <imageanalysis/ImageAnalysis/ImageStatsCalculator.h>
#include <imageanalysis/ImageAnalysis/Image2DConvolver.h>
#include <imageanalysis/ImageAnalysis/ImageRegridder.h>
#include <casacore/casa/OS/File.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/Lattices/TiledLineStepper.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/Lattices/LatticeUtilities.h>
#include <casacore/lattices/LRegions/LCEllipsoid.h>
#include <casacore/lattices/LRegions/LCUnion.h>
#include <casacore/lattices/LRegions/LCExtension.h>
#include <casacore/lattices/LRegions/LCPagedMask.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/tables/Tables/TableLock.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

#include <imageanalysis/Annotations/RegionTextList.h>
#include <synthesis/ImagerObjects/SDMaskHandler.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  SDMaskHandler::SDMaskHandler()
  {
    itsMax = DBL_MAX;
    itsRms = DBL_MAX;
    itsSidelobeLevel = 0.0;
    //itsPBMaskLevel = 0.0;
    //itsTooLongForFname=false;
#ifdef NAME_MAX
   itsNAME_MAX = NAME_MAX;
#else
   itsNAME_MAX = 255;
#endif

itsTooLongForFname = false;

  }


  SDMaskHandler::~SDMaskHandler()
  {
  }
  
  void SDMaskHandler::resetMask(std::shared_ptr<SIImageStore> imstore)
  {
    imstore->mask()->set(1.0);
    imstore->mask()->unlock();
  }

/***
  void SDMaskHandler::fillMask(std::shared_ptr<SIImageStore> imstore, Vector<String> maskStrings)
  {
      String maskString;
      if (maskStrings.nelements()) {
        for (uInt imsk = 0; imsk < maskStrings.nelements(); imsk++) {
          maskString = maskStrings[imsk];
          fillMask(imstore, maskString);
        }
      }
      else {
        fillMask(imstore, String(""));
      }
  }
***/

  void SDMaskHandler::fillMask(std::shared_ptr<SIImageStore> imstore, Vector<String> maskStrings)
  {
    LogIO os( LogOrigin("SDMaskHandler","fillMask",WHERE) );
    String maskString;
    try {
      TempImage<Float> tempAllMaskImage(imstore->mask()->shape(), imstore->mask()->coordinates(), memoryToUse());
      tempAllMaskImage.set(0.0);
      if (maskStrings.nelements()) {
        //working temp mask image
        TempImage<Float> tempMaskImage(imstore->mask()->shape(), imstore->mask()->coordinates(), memoryToUse());
        copyMask(*(imstore->mask()), tempMaskImage);
        for (uInt imsk = 0; imsk < maskStrings.nelements(); imsk++) {
          maskString = maskStrings[imsk];
          bool isTable(false);
          bool isCasaImage(false);
          // test
          Path testPath(maskString);
          if (testPath.baseName()==maskString) {
              if (int(maskString.length()) > itsNAME_MAX) 
                  // the string is too long if it is file name or could be region text
                  itsTooLongForFname=true;
          }
          if (maskString!="") {
            if(!itsTooLongForFname)
              isTable = Table::isReadable(maskString); 
            if (!isTable) {
              try {
                if (ImageOpener::imageType(maskString)==ImageOpener::IMAGECONCAT) isCasaImage=true;
                //os <<"IT'S A CONCATImage"<<LogIO::POST;
              } 
              catch (AipsError &x) {
                os <<"cannot find imageType... "<<LogIO::POST;
              }
            }
            else {
              Table imtab = Table(maskString, Table::Old);
              Vector<String> colnames = imtab.tableDesc().columnNames();
              if ( colnames[0]=="map" ) isCasaImage=true;
            }  
            if (isCasaImage) {
	      std::shared_ptr<ImageInterface<Float> > inmaskptr;
              LatticeBase* latt =ImageOpener::openImage(maskString);
              inmaskptr.reset(dynamic_cast<ImageInterface<Float>* >(latt));
              IPosition inShape = inmaskptr->shape();
              IPosition outShape = imstore->mask()->shape();
              Int specAxis = CoordinateUtil::findSpectralAxis(inmaskptr->coordinates());
              Int inNchan = (specAxis==-1? 1: inShape(specAxis) );
              Int outSpecAxis = CoordinateUtil::findSpectralAxis(imstore->mask()->coordinates());
              Vector <Stokes::StokesTypes> inWhichPols, outWhichPols;
              Int stokesAxis = CoordinateUtil::findStokesAxis(inWhichPols, inmaskptr->coordinates());
              Int inNstokes = (stokesAxis==-1? 1: inShape(stokesAxis) );
              Int outStokesAxis = CoordinateUtil::findStokesAxis(outWhichPols, imstore->mask()->coordinates());
              if (inNchan == 1 && outShape(outSpecAxis)>1) {
                os << "Extending mask image: " << maskString << LogIO::POST;
                expandMask(*inmaskptr, tempMaskImage);
              }
              else if(inNstokes == 1 && outShape(outStokesAxis) > 1 ) {
                os << "Extending mask image along Stokes axis: " << maskString << LogIO::POST;
                expandMask(*inmaskptr, tempMaskImage);
              }
              else {
                os << "Copying mask image: " << maskString << LogIO::POST;
                copyMask(*inmaskptr, tempMaskImage);
              }
            }// end of readable table
            else {
              //
              std::unique_ptr<Record> myrec(nullptr);
              try {
                if (!itsTooLongForFname) {
                  myrec.reset(RegionManager::readImageFile(maskString,String("temprgrec")));
                  if (myrec!=0) {
                    Bool ret(false);
                    Matrix<Quantity> dummyqmat;
                    Matrix<Float> dummyfmat;
                    ret=SDMaskHandler::regionToImageMask(tempMaskImage, myrec.get(), dummyqmat, dummyfmat);
                    if (!ret) cout<<"regionToImageMask failed..."<<endl;
                      os << "Reading region record mask: " << maskString << LogIO::POST;
                  }
                }
                else {
                    // skip to check the mask as a direct region text input
                    throw(AipsError("Too long for filename. Try the string as a direct region text"));
                }
              }
              catch (AipsError &x) {
                try {
                  ImageRegion* imageRegion=0;
                  os << LogIO::NORMAL3<< "Reading text mask: " << maskString << LogIO::POST;
                  SDMaskHandler::regionTextToImageRegion(maskString, tempMaskImage, imageRegion);
                  if (imageRegion!=0)
                    SDMaskHandler::regionToMask(tempMaskImage,*imageRegion, Float(1.0));
                }
                catch (AipsError &x) {
                  os << LogIO::WARN << maskString << "is invalid mask. Skipping this mask..." << LogIO::POST;
                }
              }
            }// end of region string
          }// end of non-emtpy maskstring
         
          LatticeExpr<Float> addedmask( tempMaskImage + tempAllMaskImage ); 
          tempAllMaskImage.copyData( LatticeExpr<Float>( iif(addedmask > 0.0, 1.0, 0.0) ) );
        }
        imstore->mask()->copyData(tempAllMaskImage);
        imstore->mask()->unlock();
      }
    } catch( AipsError &x )
      {
	throw(AipsError("Error in constructing "+ imstore->getName() +".mask from " + maskString + " : " + x.getMesg()));
      }
  }


  
  void SDMaskHandler::fillMask(std::shared_ptr<SIImageStore> imstore, String maskString)
  {

    try {
      
      //// imstore->mask() will return a pointer to an ImageInterface (allocation happens on first access). 
      
      //    cout << "Call makeMask here to fill in " << imstore->mask()->name() << " from " << maskString <<  ". For now, set mask to 1 inside a central box" << endl;
      
      //interpret maskString 
      if (maskString !="") {
        bool isTable(false);
        Path testPath(maskString);
        if (testPath.baseName() == maskString) { 
            if (int(maskString.length()) > itsNAME_MAX)  
                itsTooLongForFname=true;
        }
        if (!itsTooLongForFname) 
          isTable = Table::isReadable(maskString);
	//if (!itsTooLongForFname && Table::isReadable(maskString) ) {
	if ( isTable ) {
	  Table imtab = Table(maskString, Table::Old);
	  Vector<String> colnames = imtab.tableDesc().columnNames();
	  if ( colnames[0]=="map" ) {
	    
	    // looks like a CASA image ... probably should check coord exists in the keyword also...
	    //          cout << "copy this input mask...."<<endl;
	    PagedImage<Float> inmask(maskString); 
	    IPosition inShape = inmask.shape();
	    IPosition outShape = imstore->mask()->shape();
	    Int specAxis = CoordinateUtil::findSpectralAxis(inmask.coordinates());
	    Int outSpecAxis = CoordinateUtil::findSpectralAxis(imstore->mask()->coordinates());
	    if (inShape(specAxis) == 1 && outShape(outSpecAxis)>1) {
	      expandMask(inmask, *(imstore->mask()));
	    }
	    else {
	      copyMask(inmask, *(imstore->mask()));
	    }
	  }// end of ''map''
	}// end of readable table
	else {
	  ImageRegion* imageRegion=0;
	  SDMaskHandler::regionTextToImageRegion(maskString, *(imstore->mask()), imageRegion);
	  if (imageRegion!=0)
	    SDMaskHandler::regionToMask(*(imstore->mask()),*imageRegion, Float(1.0));
	}// end of region string
      }
      else { 
	/////// Temporary code to set a mask in the inner quarter.
	/////// This is only for testing... should go when 'maskString' can be used to fill it in properly. 
	IPosition imshp = imstore->mask()->shape();
	AlwaysAssert( imshp.nelements() >=2 , AipsError );
	
	Slicer themask;
	IPosition blc(imshp.nelements(), 0);
	IPosition trc = imshp-1;
	IPosition inc(imshp.nelements(), 1);
	
	blc(0)=int(imshp[0]*0.25);
	blc(1)=int(imshp[1]*0.25);
	trc(0)=int(imshp[0]*0.75);
	trc(1)=int(imshp[1]*0.75);
	
	LCBox::verify(blc, trc, inc, imshp);
	Slicer imslice(blc, trc, inc, Slicer::endIsLast);
	
	std::shared_ptr<ImageInterface<Float> >  referenceImage( new SubImage<Float>(*(imstore->mask()), imslice, true) );
	referenceImage->set(1.0);
      }

      imstore->mask()->unlock();
   
    } catch( AipsError &x )
      {
	throw(AipsError("Error in constructing "+ imstore->getName() +".mask from " + maskString + " : " + x.getMesg()));
      }
  }
  
  //void SDMaskHandler::makeMask()
   std::shared_ptr<ImageInterface<Float> > SDMaskHandler::makeMask(const String& maskName, const Quantity threshold,
   //void SDMaskHandler::makeMask(const String& maskName, const Quantity threshold,
                               ImageInterface<Float>& tempim)
   //                             ImageInterface<Float>& tempim,
   //                           ImageInterface<Float> *newMaskImage)
  {
    LogIO os( LogOrigin("SDMaskHandler","makeMask",WHERE) );
    //
    // create mask from a threshold... Imager::mask()...
    //default handling?
    String maskFileName(maskName);
    if ( maskFileName=="" ) { 
      maskFileName = tempim.name() + ".mask";
    }
    if (!cloneImShape(tempim, maskFileName)) {
      throw(AipsError("Cannot make a mask from "+tempim.name()));
    }
    PagedImage<Float> *newMaskImage = new PagedImage<Float>(maskFileName, TableLock::AutoNoReadLocking);
    //newMaskImage = PagedImage<Float>(maskFileName, TableLock::AutoNoReadLocking);
    //PagedImage<Float>(maskFileName, TableLock::AutoNoReadLocking);
    StokesImageUtil::MaskFrom(*newMaskImage, tempim, threshold);
    return std::shared_ptr<ImageInterface<Float> >(newMaskImage);
  }

  //Bool SDMaskHandler::regionToImageMask(const String& maskName, Record* regionRec, Matrix<Quantity>& blctrcs,
  Bool SDMaskHandler::regionToImageMask(ImageInterface<Float>& maskImage, const Record* regionRec, const Matrix<Quantity>& blctrcs,
            const Matrix<Float>& circles, const Float& value) {

    LogIO os(LogOrigin("imager", "regionToImageMask", WHERE));

    try {
      //PagedImage<Float> tempmask(TiledShape(maskImage->shape(),
      //                                    maskImage->niceCursorShape()), maskImage->coordinates(), tempfname);
      std::shared_ptr<ImageInterface<Float> > tempmask;
      tempmask.reset( new TempImage<Float>(TiledShape(maskImage.shape(),maskImage.niceCursorShape()), maskImage.coordinates(), memoryToUse() ) );
      //tempmask = new PagedImage<Float>(maskImage.shape(), maskImage.coordinates(),"__tmp_rgmask");
      tempmask->copyData(maskImage);

      CoordinateSystem cSys=tempmask->coordinates();
      //maskImage.table().markForDelete();
      ImageRegion *boxregions=0;
      ImageRegion *circleregions=0;
      RegionManager regMan;
      regMan.setcoordsys(cSys);
      if (blctrcs.nelements()!=0){
        boxRegionToImageRegion(*tempmask, blctrcs, boxregions);
      }
      if (circles.nelements()!=0) {
        circleRegionToImageRegion(*tempmask, circles, circleregions);
      } 
      ImageRegion* recordRegion=0;
      if(regionRec !=0){
      //if(regionRec.nfields() !=0){
        recordRegionToImageRegion(regionRec, recordRegion);
      }
   
      ImageRegion *unionReg=0;
      if(boxregions!=0 && recordRegion!=0){
        unionReg=regMan.doUnion(*boxregions, *recordRegion);
        delete boxregions; boxregions=0;
        delete recordRegion; recordRegion=0;
      }
      else if(boxregions !=0){
        unionReg=boxregions;
      }
      else if(recordRegion !=0){
        unionReg=recordRegion;
      } 

      if(unionReg !=0){
        regionToMask(*tempmask, *unionReg, value);
        delete unionReg; unionReg=0;
      }
      //As i can't unionize LCRegions and WCRegions;  do circles seperately
      if(circleregions !=0){
        regionToMask(*tempmask, *circleregions, value);
        delete circleregions;
        circleregions=0;
      }
      //maskImage.table().unmarkForDelete();
      maskImage.copyData(*tempmask); 
    }
    catch (AipsError& x) {
      os << "Error in regionToMaskImage() : " << x.getMesg() << LogIO::EXCEPTION;
    }
    return true;
  }

  Bool SDMaskHandler::regionToMask(ImageInterface<Float>& maskImage, ImageRegion& imageregion, const Float& value) 
  {
    SubImage<Float> partToMask(maskImage, imageregion, true);
    LatticeRegion latReg=imageregion.toLatticeRegion(maskImage.coordinates(), maskImage.shape());
    ArrayLattice<Bool> pixmask(latReg.get());
    LatticeExpr<Float> myexpr(iif(pixmask, value, partToMask) );
    partToMask.copyData(myexpr);
    return true;
  }

  void SDMaskHandler::boxRegionToImageRegion(const ImageInterface<Float>& maskImage, const Matrix<Quantity>& blctrcs, ImageRegion*& boxImageRegions)
  {
    if(blctrcs.shape()(1) != 4)
      throw(AipsError("Need a list of 4 elements to define a box"));

    CoordinateSystem cSys=maskImage.coordinates();
    RegionManager regMan;
    regMan.setcoordsys(cSys);
    Vector<Quantum<Double> > blc(2);
    Vector<Quantum<Double> > trc(2);
    Int nrow=blctrcs.shape()(0);
    Vector<Int> absRel(2, RegionType::Abs);
    PtrBlock<const WCRegion *> lesbox(nrow);
    for (Int k=0; k < nrow; ++k){
      blc(0) = blctrcs(k,0);
      blc(1) = blctrcs(k,1);
      trc(0) = blctrcs(k,2);
      trc(1) = blctrcs(k,3);
      lesbox[k]= new WCBox (blc, trc, cSys, absRel);
    }
    boxImageRegions=regMan.doUnion(lesbox);
    if (boxImageRegions!=0) {
    }
    for (Int k=0; k < nrow; ++k){
      delete lesbox[k];
    }
  }

  void SDMaskHandler::circleRegionToImageRegion(const ImageInterface<Float>& maskImage, const Matrix<Float>& circles, 
                                         ImageRegion*& circleImageRegions)
  {
    if(circles.shape()(1) != 3)
      throw(AipsError("Need a list of 3 elements to define a circle"));

    CoordinateSystem cSys=maskImage.coordinates();
    RegionManager regMan;
    regMan.setcoordsys(cSys);
    Int nrow=circles.shape()(0);
    Vector<Float> cent(2);
    cent(0)=circles(0,1); cent(1)=circles(0,2);
    Float radius=circles(0,0);
    IPosition xyshape(2,maskImage.shape()(0),maskImage.shape()(1));
    LCEllipsoid *circ= new LCEllipsoid(cent, radius, xyshape);
    //Tell LCUnion to delete the pointers
    LCUnion *elunion= new LCUnion(true, circ);
    //now lets do the remainder
    for (Int k=1; k < nrow; ++k){
      cent(0)=circles(k,1); cent(1)=circles(k,2);
      radius=circles(k,0);
      circ= new LCEllipsoid(cent, radius, xyshape);
      elunion=new LCUnion(true, elunion, circ);
    }
    //now lets extend that to the whole image
    IPosition trc(2);
    trc(0)=maskImage.shape()(2)-1;
    trc(1)=maskImage.shape()(3)-1;
    LCBox lbox(IPosition(2,0,0), trc,
               IPosition(2,maskImage.shape()(2),maskImage.shape()(3)) );
    LCExtension linter(*elunion, IPosition(2,2,3),lbox);
    circleImageRegions=new ImageRegion(linter);
    delete elunion;
  }
 
  void SDMaskHandler::recordRegionToImageRegion(const Record* imageRegRec, ImageRegion*& imageRegion )
  //void SDMaskHandler::recordRegionToImageRegion(Record& imageRegRec, ImageRegion*& imageRegion ) 
  {
    if(imageRegRec !=0){
      TableRecord rec1;
      rec1.assign(*imageRegRec);
      imageRegion=ImageRegion::fromRecord(rec1,"");
    }
  }


  void SDMaskHandler::regionTextToImageRegion(const String& text, const ImageInterface<Float>& regionImage,
                                            ImageRegion*& imageRegion)
  {
    LogIO os( LogOrigin("SDMaskHandler", "regionTextToImageRegion",WHERE) );

     try {
       IPosition imshape = regionImage.shape();
       CoordinateSystem csys = regionImage.coordinates();
       File fname; 
       int maxFileName;
       bool skipfnamecheck(false);
       #ifdef NAME_MAX
         maxFileName=NAME_MAX;
       #else
         maxFileName=255;
       #endif

       Path testPath(text);
       if (testPath.baseName() == text) {
           if (int(text.length()) > maxFileName) 
              // could be direct region text 
              skipfnamecheck=true;
       }
       fname=text; 
       Record* imageRegRec=0;
       Record myrec;

       //Record imageRegRec;
       if (!skipfnamecheck && fname.exists() && fname.isRegular()) {
         RegionTextList  CRTFList(text, csys, imshape);
         myrec = CRTFList.regionAsRecord();
       }
       else { // direct text input....
         Regex rx (Regex::fromPattern("*\\[*\\]*"));
         if (text.matches(rx)) {
           RegionTextList CRTFList(csys, text, imshape);
           myrec = CRTFList.regionAsRecord();
           //cerr<<"myrec.nfields()="<<myrec.nfields()<<endl;
         }
         else {
           throw(AipsError("Input mask, '"+text+"' does not exist if it is inteded as a mask file name."+
                 "Or invalid CRTF syntax if it is intended as a direct region specification."));
         }
       }
       imageRegRec = new Record();
       imageRegRec->assign(myrec);
       recordRegionToImageRegion(imageRegRec, imageRegion);
       delete imageRegRec;
     }
     catch (AipsError& x) {
       os << LogIO::SEVERE << "Exception: "<< x.getMesg() << LogIO::POST;
     }  
  }

  void SDMaskHandler::copyAllMasks(const Vector< std::shared_ptr<ImageInterface<Float> > > inImageMasks, ImageInterface<Float>& outImageMask)
  {
     LogIO os( LogOrigin("SDMaskHandler", "copyAllMasks", WHERE) );

     TempImage<Float> tempoutmask(outImageMask.shape(), outImageMask.coordinates(), memoryToUse());
     
     for (uInt i = 0; i < inImageMasks.nelements(); i++) {
       copyMask(*inImageMasks(i), tempoutmask);
        outImageMask.copyData( (LatticeExpr<Float>)(tempoutmask+outImageMask) );
     }
  }

  void SDMaskHandler::copyMask(const ImageInterface<Float>& inImageMask, ImageInterface<Float>& outImageMask) 
  {
    LogIO os( LogOrigin("SDMaskHandler", "copyMask", WHERE) );
  
    //output mask coords
    IPosition outshape = outImageMask.shape();
    CoordinateSystem outcsys = outImageMask.coordinates();
    DirectionCoordinate outDirCsys = outcsys.directionCoordinate();
    SpectralCoordinate outSpecCsys = outcsys.spectralCoordinate();
     
    // do regrid   
    IPosition axes(3,0,1,2);
    IPosition inshape = inImageMask.shape();
    CoordinateSystem incsys = inImageMask.coordinates(); 
    DirectionCoordinate inDirCsys = incsys.directionCoordinate();
    SpectralCoordinate inSpecCsys = incsys.spectralCoordinate();
    //Check the conversion layer consistentcy between input and output.
    //Change the frame of the convesion layer in incsys to that of outcsys if different.
    if (inSpecCsys.frequencySystem(True)!=outSpecCsys.frequencySystem(True)) {
      incsys.setSpectralConversion(MFrequency::showType(outSpecCsys.frequencySystem(True)));
    }

    Vector<Int> dirAxes = CoordinateUtil::findDirectionAxes(incsys);
    axes(0) = dirAxes(0); 
    axes(1) = dirAxes(1);
    axes(2) = CoordinateUtil::findSpectralAxis(incsys);

    //const String outfilename = outImageMask.name()+"_"+String::toString(HostInfo::processID());

    try {
      // Since regrid along the spectral axis does not seem to work
      // properly, replacing with ImageRegridder 
      //ImageRegrid<Float> imregrid;
      //imregrid.showDebugInfo(1);
      //imregrid.regrid(outImageMask, Interpolate2D::LINEAR, axes, inImageMask); 
      //
      TempImage<Float>* inImageMaskptr = new TempImage<Float>(inshape,incsys,memoryToUse());
      ArrayLattice<Bool> inimmask(inImageMask.getMask());
      inImageMaskptr->copyData((LatticeExpr<Float>)(inImageMask * iif(inimmask,1.0,0.0)) );
      //
      SPCIIF tempim(inImageMaskptr);
      SPCIIF templateim(new TempImage<Float>(outshape,outcsys, memoryToUse()));
      Record* dummyrec = 0;
      //ImageRegridder regridder(tempim, outfilename, templateim, axes, dummyrec, "", true, outshape);
      //TTDEBUG
      ImageRegridder<Float> regridder(tempim, "", templateim, axes, dummyrec, "", true, outshape);
      regridder.setMethod(Interpolate2D::LINEAR);
      SPIIF retim = regridder.regrid();
      //outImageMask.copyData( (LatticeExpr<Float>) iif(*retim > 0.1, 1.0, 0.0)  );
      ArrayLattice<Bool> retimmask(retim->getMask());
      //LatticeExpr<Float> withmask( (*retim) * iif(retimmask,1.0,0.0) );
      //outImageMask.copyData( withmask );
      outImageMask.copyData( (LatticeExpr<Float>)((*retim) * iif(retimmask,1.0,0.0))  );
      //LatticeUtilities::copyDataAndMask(os, outImageMask, *retim );
    //} catch (AipsError &x) {
//	throw(AipsError("Image regrid error : "+ x.getMesg()));
 //     }
    } catch (AipsError &x) {
        os<<LogIO::WARN <<x.getMesg()<<LogIO::POST;
        os<<LogIO::WARN <<" Cannot regrid the input mask to output mask, it will be empty."<<LogIO::POST;
        outImageMask.set(0);
    } 

    // no longer needed (output file in regrid is now set to "" so no need of this clean-up)
    //try
    //  {
	// delete the outfilename image on disk
    //	Directory dd(outfilename);
    //	dd.removeRecursive();
    //  }
    //catch (AipsError &x) {
      //      throw(AipsError("Cannot delete temporary mask image : "+ x.getMesg()));
    //  os << LogIO::WARN << "Cannot  delete temporary mask image : " << x.getMesg() << LogIO::POST;
    //}
    
  } 

  void SDMaskHandler::expandMask(const ImageInterface<Float>& inImageMask, ImageInterface<Float>& outImageMask)
  {
    LogIO os( LogOrigin("SDMaskHandler", "expandMask", WHERE) );

    // expand mask with input range (in spectral axis and stokes?) ... to output range on outimage
    // currently expand a continuum mask to a cube mask in channels (to all channels) 
    // or continuum Stokes I mask to multi-Stokes mask
    // or continuum with multi-Stokes mask to cube with multi-Stokes mask
    IPosition inShape = inImageMask.shape();
    CoordinateSystem inCsys = inImageMask.coordinates();
    Vector<Int> dirAxes = CoordinateUtil::findDirectionAxes(inCsys);
    Int inSpecAxis = CoordinateUtil::findSpectralAxis(inCsys);
    Int inNchan; 
    if (inSpecAxis==-1) {
      inNchan=1;
    }
    else {
      inNchan = inShape(inSpecAxis); 
    }
      
    Vector<Stokes::StokesTypes> inWhichPols;
    Int inStokesAxis = CoordinateUtil::findStokesAxis(inWhichPols,inCsys);
    Int inNpol; 
    if (inStokesAxis==-1) {
      inNpol=1;
    }
    else {
      inNpol = inShape(inStokesAxis); 
    }
    
    //
    IPosition outShape = outImageMask.shape();
    CoordinateSystem outCsys = outImageMask.coordinates();
    Vector<Int> outDirAxes = CoordinateUtil::findDirectionAxes(outCsys);
    Int outSpecAxis = CoordinateUtil::findSpectralAxis(outCsys);
    Int outNchan = outShape(outSpecAxis);
    Vector<Stokes::StokesTypes> outWhichPols;
    Int outStokesAxis = CoordinateUtil::findStokesAxis(outWhichPols,outCsys);
    Int outNpol = outShape(outStokesAxis);

    IPosition start(4,0,0,0,0);
    IPosition length(4,outShape(outDirAxes(0)), outShape(outDirAxes(1)),1,1);
    //Slicer sl(start, length); 
    
    Int stokesInc = 1;
    // for fixing removed degenerate axis
    Bool addSpecAxis = (inSpecAxis == -1? True: False);
    // Do expansion for input mask with single channel (continuum)
    if (inNchan==1 ) {
      if (inNpol == 1 ) { 
        stokesInc = 1;
      }
      else if (inShape(inStokesAxis)==outShape(outStokesAxis)) {
        stokesInc = inShape(inStokesAxis);
      }
      else {
        throw(AipsError("Cannot extend the input mask of "+String::toString(inNpol)+
              " Stokes planes to the output mask of "+String::toString(outNpol)+
              " Stokes planes. Please modify the input mask to make it a Stokes I mask or a mask with the same number of Stokes planes as the output.") );
      }

      length(outStokesAxis) = stokesInc;

      // I stokes cont -> cube: regrid ra.dec on the input single plane 
      // I stokes cont -> cont multi-stokes: regrid ra.dec on the input 
      // I stokes cont ->  cube multi-stokes: regid ra.dec on input 

      Slicer sl(start, length);
      // make a subImage for regridding output       
      SubImage<Float> chanMask(outImageMask, sl, true);
      ImageRegrid<Float> imregrid;
      TempImage<Float> tempInImageMask(chanMask.shape(), chanMask.coordinates(),memoryToUse());
      PtrHolder<ImageInterface<Float> > outmaskim_ptr;
      if ( chanMask.shape().nelements() >  inImageMask.shape().nelements() ) {
        String stokesStr;
        if (inNpol==1) {
          stokesStr="I";
        }
        else {
          stokesStr="";
          //for (uInt ipol=0; ipol < inWhichPols.nelements(); ipol++) {
          //  stokesStr+=Stokes::name(inWhichPols(ipol));
          //}
        }  
        //os<<"Adding degenerate axes: addSpecAxis="<<addSpecAxis<<" stokes="<<stokesStr<<LogIO::POST;
        casacore::ImageUtilities::addDegenerateAxes(os, outmaskim_ptr, inImageMask,"",False, addSpecAxis, stokesStr, False, False, True); 
        tempInImageMask.copyData(*outmaskim_ptr);
      }
      else {
        tempInImageMask.copyData(inImageMask);
      }

      try {
        imregrid.regrid(chanMask, Interpolate2D::LINEAR, dirAxes, tempInImageMask);
      } catch (AipsError& x) {
        os<<LogIO::WARN<<"Regridding of the input mask image failed: "<<x.getMesg()<<LogIO::POST;
      }
      // extract input mask (first stokes plane) 
      Array<Float> inMaskData;
      IPosition end2(4,outShape(outDirAxes(0)), outShape(outDirAxes(1)), 1, 1);
      if ( inNpol==outNpol ) {
        end2(outStokesAxis) = inNpol;
      } 
      chanMask.doGetSlice(inMaskData, Slicer(start,end2));
      IPosition stride(4,1,1,1,1);
      //
      // continuum output mask case
      if (outNchan==1) { 
        //No copying over channels, just do copying over all Stokes if input mask is a single Stokes
        if (inNpol == 1 && outNpol > 1) {
          for (Int ipol = 0; ipol < outNpol; ipol++) {
            start(outStokesAxis) = ipol;
            os<<"Copying input mask to Stokes plane="<<ipol<<LogIO::POST;
            outImageMask.putSlice(inMaskData,start,stride);
          }
        }
      }
      else {  // for cube 
        for (Int ich = 0; ich < outNchan; ich++) {
          start(outSpecAxis) = ich;
          IPosition inStart(4,0,0,0,0);
          if (inNpol == 1 && outNpol > 1) {
            // extend to other Stokes 
            for (Int ipol = 0; ipol < outNpol; ipol++) {
              os<<"Copying input mask to Stokes plane="<<ipol<<LogIO::POST;
              start(outStokesAxis) = ipol;
              outImageMask.putSlice(inMaskData,start,stride);
            }
          }
          else {
            // copy Stokes plane as is (but expand it to all channels)
            stride(outStokesAxis) = stokesInc; 
            if (inNpol == outNpol) {
              for (Int ipol = 0; ipol < outNpol; ipol++) {
                // need to slice mask from each stokes plane
                inMaskData.resize();
                inStart(outStokesAxis) = ipol;
                start(outStokesAxis) = ipol;
                end2(outStokesAxis) = 1;
                stride(outStokesAxis) = 1;
                chanMask.doGetSlice(inMaskData, Slicer(inStart,end2));
                outImageMask.putSlice(inMaskData,start,stride); 
              }

            }
            else {
              outImageMask.putSlice(inMaskData,start,stride); 
            }
          }
        } //for loop
      } // else
    }
    // Stokes I (a single Stokes plane mask with cube)
    else if (inNpol == 1) {
      if (inNpol != 1 ) {
        if (inShape(inStokesAxis)==outShape(outStokesAxis)) {
          stokesInc = inShape(inStokesAxis);
        }
        else {
          throw(AipsError("Cannot extend the input mask of "+String::toString(inNpol)+
              " Stokes planes to the output mask of "+String::toString(outNpol)+
              " Stokes planes. Please modify the input mask to make it a Stokes I mask or a mask with the same number of Stokes planes as the output.") );
        }
      }
      length(outStokesAxis) = stokesInc;
      length(outSpecAxis) = outNchan;
      Slicer slStokes(start, length);
      // make a subImage for regridding output (all channels)    
      SubImage<Float> stokesMask(outImageMask, slStokes, true);
      ImageRegrid<Float> imregrid2;
      TempImage<Float> tempInStokesImageMask(stokesMask.shape(), stokesMask.coordinates(),memoryToUse());
      PtrHolder<ImageInterface<Float> > outstokesmaskim_ptr;
      Vector<Stokes::StokesTypes> outWhichPols;
      if ( stokesMask.shape().nelements() >  inImageMask.shape().nelements() ) {
        casacore::ImageUtilities::addDegenerateAxes(os, outstokesmaskim_ptr, inImageMask,"",False, addSpecAxis, "I", False, False, True); 
        Vector<Int> outWorldOrder(4);
        Vector<Int> outPixelOrder(4);
        outWorldOrder(0)=0;
        outWorldOrder(1)=1;
        outWorldOrder(2)=3;
        outWorldOrder(3)=2;
        outPixelOrder(0)=0;
        outPixelOrder(1)=1;
        outPixelOrder(2)=3;
        outPixelOrder(3)=2;
        CoordinateSystem modcsys=tempInStokesImageMask.coordinates();
        IPosition inMaskShape = inImageMask.shape();
        Array<Float> inData = outstokesmaskim_ptr->get();
        IPosition newAxisOrder(4,0,1,3,2);
        Array<Float> reorderedData = reorderArray(inData, newAxisOrder);
        IPosition newShape=reorderedData.shape();
        //os<< "reoderedData shape="<<reorderedData.shape()<<LogIO::POST;
        TempImage<Float> modTempInStokesMask(TiledShape(newShape), modcsys);
        modTempInStokesMask.put(reorderedData);
        
        if (compareSpectralCoordinate(inImageMask,tempInStokesImageMask) ) {
          tempInStokesImageMask.copyData(modTempInStokesMask);
        }
      }
      else {
        if (compareSpectralCoordinate(inImageMask,tempInStokesImageMask) ) {
          tempInStokesImageMask.copyData(inImageMask);
        }
         
      }
      try {
        imregrid2.regrid(stokesMask, Interpolate2D::LINEAR, dirAxes, tempInStokesImageMask);
      } catch (AipsError& x) {
        os<<LogIO::WARN<<"Regridding of the input mask image failed: "<<x.getMesg()<<LogIO::POST;
      }
      os <<"Slicing data..."<<LogIO::POST;
      Array<Float> inMaskData2;
      IPosition end3(4,outShape(outDirAxes(0)), outShape(outDirAxes(1)), 1, 1);
      end3(outStokesAxis) = inNpol;
      end3(outSpecAxis) = inNchan;
      stokesMask.doGetSlice(inMaskData2,slStokes);    
      IPosition stride(4,1,1,1,1);
      IPosition inStart2(4,0,0,0,0);
      for (Int ipol = 0; ipol < outNpol; ipol++) {
        // need to slice mask from each stokes plane
        inMaskData2.resize();
        inStart2(outStokesAxis) = 0;
        start(outStokesAxis) = ipol;
        end3(outStokesAxis) = 1;
        stride(outStokesAxis) = 1;
        stride(outSpecAxis) = 1; // assume here inNchan == outNchan
        stokesMask.doGetSlice(inMaskData2, Slicer(inStart2,end3));
        outImageMask.putSlice(inMaskData2,start); 
      }
            
    }
    else {
      throw(AipsError("Input mask,"+inImageMask.name()+" does not conform with the number of channels in output mask"));
    }
  }

  Bool SDMaskHandler::compareSpectralCoordinate(const ImageInterface<Float>& inImage, const ImageInterface<Float>& outImage)
  { 
    LogIO os( LogOrigin("SDMaskHandler", "checkSpectralCoord",WHERE) );
    
    SpectralCoordinate outSpecCoord = outImage.coordinates().spectralCoordinate();
    IPosition inshape = inImage.shape();
    IPosition outshape = outImage.shape();
    CoordinateSystem incys = inImage.coordinates();
    CoordinateSystem outcsys = outImage.coordinates();
    Int inSpecAxis = CoordinateUtil::findSpectralAxis(incys);
    Int outSpecAxis = CoordinateUtil::findSpectralAxis(outcsys);
    Bool nchanMatch(true);
    if (inSpecAxis != -1 and outSpecAxis != -1 ) 
      nchanMatch = inshape(inSpecAxis) == outshape(outSpecAxis)? true: false;
    if (!nchanMatch) {
      if (!outSpecCoord.near(inImage.coordinates().spectralCoordinate())) {
        throw(AipsError("Cannot extend the input mask. Spectral coordiante and the number of channels of the input mask does not match with those of the output mask. Use a single channel mask or a mask that matches the spectral coordiante of the output. "));
      } 
      else {
        throw(AipsError("Cannot extend the input mask. The number of the channels in Input mask,"+inImage.name()+"does not match with that of the output mask. Use a single channel mask or a mask that matches the spectral coordiante of the output. "));
      }
      return false;
    } 
    else {
      if (!outSpecCoord.near(inImage.coordinates().spectralCoordinate())) {
        throw(AipsError("Cannot extend the input mask. Spectral coordiante of Input mask,"+inImage.name()+"does not match with that of the output mask. Use a single channel mask or a mask that matches the spectral coordiante of the output. "));
        return false;
      }
    } 
    return true;
  }


  // was Imager::clone()...
  //static Bool cloneImShape(const ImageInterface<Float>& inImage, ImageInterface<Float>& outImage)
  Bool SDMaskHandler::cloneImShape(const ImageInterface<Float>& inImage, const String& outImageName)
  { 
    LogIO os( LogOrigin("SDMaskHandler", "cloneImShape",WHERE) );
    
    try {
      PagedImage<Float> newImage(TiledShape(inImage.shape(),
                                          inImage.niceCursorShape()), inImage.coordinates(),
    //                           outImage.name());
                               outImageName);
      newImage.set(0.0);
      newImage.table().flush(true, true);
    } catch (AipsError& x) {
      os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
      return false;
    }
    return true;
  }


  Int SDMaskHandler::makeInteractiveMask(std::shared_ptr<SIImageStore>& imstore,
					 Int& niter, Int& cycleniter, 
					 String& threshold, String& cyclethreshold)
  {
    Int ret;
    // Int niter=1000, ncycles=100;
    // String thresh="0.001mJy";
    String imageName = imstore->getName()+".residual"+(imstore->getNTaylorTerms()>1?".tt0":"");
    String maskName = imstore->getName() + ".mask";
    imstore->mask()->unlock();
    cout << "Before interaction : niter : " << niter << " cycleniter : " << cycleniter << " thresh : " << threshold << "  cyclethresh : " << cyclethreshold << endl;
    //    ret = interactiveMasker_p->interactivemask(imageName, maskName,
    //                                            niter, ncycles, threshold);
    return ret;
  }

  void SDMaskHandler::makeAutoMask(std::shared_ptr<SIImageStore> imstore)
  {
    LogIO os( LogOrigin("SDMaskHandler","makeAutoMask",WHERE) );

    Array<Float> localres;
    // Modification to be able to work with a cube (TT 2014-12-09)
    //imstore->residual()->get( localres , true );
    imstore->residual()->get( localres );

    Array<Float> localmask;
    //imstore->mask()->get( localmask , true );
    imstore->mask()->get( localmask );
   
    Int specAxis = CoordinateUtil::findSpectralAxis(imstore->mask()->coordinates());
    IPosition maskShape = localmask.shape();
    Int ndim = maskShape.nelements();
    IPosition pos(ndim,0);
    IPosition blc(ndim,0);
    IPosition trc(ndim,0);
    trc[0] = maskShape[0]-1; 
    trc[1] = maskShape[1]-1;
    // added per channel mask setting
    for (pos[specAxis] = 0; pos[specAxis]<localmask.shape()[specAxis]; pos[specAxis]++) 
      { 
        IPosition posMaxAbs( localmask.shape().nelements(), 0);
        blc[specAxis]=pos[specAxis];
        trc[specAxis]=pos[specAxis];
        Float maxAbs=0.0;
        Float minVal;
        IPosition posmin(localmask.shape().nelements(), 0);
        //minMax(minVal, maxAbs, posmin, posMaxAbs, localres);
        minMax(minVal, maxAbs, posmin, posMaxAbs, localres(blc,trc));

    //    cout << "Max position : " << posMaxAbs << endl;

        Int dist=5;
     
        //IPosition pos(2,0,0); // Deal with the input shapes properly......
        for (pos[0]=posMaxAbs[0]-dist; pos[0]<posMaxAbs[0]+dist; pos[0]++)
          {
	    for (pos[1]=posMaxAbs[1]-dist; pos[1]<posMaxAbs[1]+dist; pos[1]++)
	      {
	        if( pos[0]>0 && pos[0]<localmask.shape()[0] && pos[1]>0 && pos[1]<localmask.shape()[1] )
	          {
		    localmask( pos ) = 1.0;
	          }
	      }
          }
      } // over channels
    //cout << "Sum of mask : " << sum(localmask) << endl;
    Float summask = sum(localmask);
    if( summask==0.0 ) { localmask=1.0; summask = sum(localmask); }
    os << LogIO::NORMAL1 << "Make Autobox mask with " << summask << " available pixels " << LogIO::POST;

    imstore->mask()->put( localmask );

    //    imstore->mask()->get( localmask , true );
    //    cout << "Sum of imstore mask : " << sum( localmask ) << endl;

  }

  void SDMaskHandler::autoMask(std::shared_ptr<SIImageStore> imstore, 
                               ImageInterface<Float>& posmask,
                               const Int iterdone,
                               Vector<Bool>& chanflag,
                               Record& robuststatsrec,
                               const String& alg, 
                               const String& threshold, 
                               const Float& fracofpeak, 
                               const String& resolution,
                               const Float& resbybeam,
                               const Int nmask,
                               const Bool autoadjust,
                               // new params for the multithreshold alg.
                               const Float& sidelobethreshold,
                               const Float& noisethreshold, 
                               const Float& lownoisethreshold,
                               const Float& negativethreshold,
                               const Float& cutthreshold,
                               const Float& smoothfactor,
                               const Float& minbeamfrac, 
                               const Int growiterations,
                               const Bool dogrowprune,
                               const Float& minpercentchange,
                               const Bool verbose, 
                               const Bool fastnoise,
                               const Bool isthresholdreached,
                               Float pblimit)
  {
    LogIO os( LogOrigin("SDMaskHandler","autoMask",WHERE) );

    //currently supported alg:
    //  onebox: a box around a max (box size +/-5pix around max position)
    //  thresh: threshold based auto masking (uses threshold or fracofpeak, and resolution)
    //      
    // create a working copy of residual image (including pixel masks) 
    os << LogIO::NORMAL2 <<"algorithm:"<<alg<<LogIO::POST;
    TempImage<Float>* tempres = new TempImage<Float>(imstore->residual()->shape(), imstore->residual()->coordinates(), memoryToUse()); 
    //Array<Float> maskdata;
    //Array<Float> psfdata;
    {
      Array<Float> resdata;
      imstore->residual()->get(resdata);
      tempres->put(resdata);
      tempres->setImageInfo(imstore->residual()->imageInfo());
      tempres->attachMask(ArrayLattice<Bool> (imstore->residual()->getMask()));
    }

    TempImage<Float>* tempmask = new TempImage<Float>(imstore->mask()->shape(), imstore->mask()->coordinates(), memoryToUse());
    // get current mask and apply pbmask
    {
      Array<Float> maskdata;
      imstore->mask()->get(maskdata);
      String maskname = imstore->getName()+".mask";
      tempmask->put(maskdata);
      //For debug
      //PagedImage<Float> tempmaskInit(tempmask->shape(), tempmask->coordinates(), "tempMaskInit"+String::toString(iterdone));
      //tempmaskInit.copyData(*tempmask);

      // 
    
      if (pblimit>0.0 && imstore->hasPB()) {
        //Determine if there is mask and set pixel mask to True for the mask to check by ntrue 
        LatticeExpr<Bool> pixmask( iif(*tempmask > 0.0, True, False));
        TempImage<Float>* dummy = new TempImage<Float>(tempres->shape(), tempres->coordinates(), memoryToUse());
        dummy->attachMask(pixmask);
        LatticeExpr<Float> themask;
        if (!ntrue(dummy->getMask())) { // initial zero mask
          //os<<"INITIAL Zero Mask ...."<<LogIO::POST;
          //themask = LatticeExpr<Float>( iif( (*(imstore->pb())) > pblimit , 1.0 , 0.0 ));
          themask = LatticeExpr<Float>( *tempmask);
        }
        else {
          //os<<"INITIAL NON-Zero Mask ...."<<LogIO::POST;
          themask = LatticeExpr<Float>( iif( (*(imstore->pb())) > pblimit, *(imstore->mask()), 0.0));
        } 
        // attache pixmask to temp res image to be used in stats etc
        tempres->attachMask(LatticeExpr<Bool> ( iif(*(imstore->pb()) > pblimit, True, False)));
        imstore->mask()->copyData( themask );
        imstore->mask()->get(maskdata);
        tempmask->put(maskdata);
        delete dummy;
      }
    } 
    //for debug
    //String tempresname="initialRes_"+String::toString(iterdone)+".im";
    //PagedImage<Float> initialRes(tempres->shape(), tempres->coordinates(), tempresname);
    //initialRes.copyData(*tempres);
     
    // Not use this way for now. Got an issue on removing pixel mask from *.mask image
    // retrieve pixelmask (i.e.  pb mask)
    //LatticeExpr<Bool> pixmasyyk;
    //if (imstore->mask()->hasPixelMask()) {
    //  pixmask = LatticeExpr<Bool> (imstore->mask()->pixelMask()); 
      //
      // create pixel mask (set to True for the previous selected region(s) to exclude the region from the stats/masking )
      //LatticeExpr<Bool> prevmask( iif(*tempmask > 0.0 || pixmask, True, False) );
    //  TempImage<Float>* dummy = new TempImage<Float>(tempres->shape(), tempres->coordinates());
    //  dummy->attachMask(pixmask);
      //if (ntrue(dummy->getMask())) tempres->attachMask(pixmask);
    //  if (ntrue(dummy->getMask())) {
        //tempmask->removeMask();
    //  }
    //  else {
    //    os<<LogIO::DEBUG1<<"No pixel mask"<<LogIO::POST;
    //  }  
    //  delete dummy; dummy=0;
    //}
    //
    //input 
    Quantity qthresh(0,"");
    Quantity qreso(0,"");
    Quantity::read(qreso,resolution);
    Float sigma = 0.0;
    // if fracofpeak (fraction of a peak) is specified, use it to set a threshold
    if ( fracofpeak != 0.0 ) {
      if (fracofpeak > 1.0 )
         throw(AipsError("Fracofpeak must be < 1.0"));
      sigma = 0.0;
    }
    else if(Quantity::read(qthresh,threshold) ) {
      // evaluate threshold input 
      //cerr<<"qthresh="<<qthresh.get().getValue()<<" unit="<<qthresh.getUnit()<<endl;
      if (qthresh.getUnit()!="") {
        // use qthresh and set sigma =0.0 to ignore
        sigma = 0.0;
      }
      else {
        sigma = String::toFloat(threshold);
        if (sigma==0.0) {
            // default case: threshold, fracofpeak unset => use default (3sigma)
            sigma = 3.0;
        }
      }
    }
    else {
      if (!sigma) {
        throw(AipsError("Unrecognized automask threshold="+threshold));
      }
    }
       
    //TempImage<Float>* resforstats = new TempImage<Float>(imstore->residual()->shape(), imstore->residual()->coordinates()); 
    //Array<Float> resdata2;
    //imstore->residual()->get(resdata2);
    //resforstats->put(resdata2);
    //resforstats->setImageInfo(imstore->residual()->imageInfo());
    //LatticeExpr<Bool> prevmask( iif(*tempmask > 0.0 , True, False) );
    //resforstats->attachMask(prevmask);
    //std::shared_ptr<casacore::ImageInterface<float> > tempres_ptr(resforstats);
    /**
    std::shared_ptr<casacore::ImageInterface<float> > tempres_ptr(tempres);
    //cerr<<" tempres->hasPixelMask? "<<tempres->hasPixelMask()<<endl;
    ImageStatsCalculator imcalc( tempres_ptr, 0, "", False); 
    Vector<Int> axes(2);
    axes[0] = 0;
    axes[1] = 1;
    imcalc.setAxes(axes);
    // for now just for new autobox alg.
    if (alg.contains("newauto") ) {
       imcalc.setRobust(true);
    }
    Record thestats = imcalc.statistics();
    ***/

    Record*  region_ptr=0;
    String LELmask("");
    // note: tempres is res image with pblimit applied
    Bool robust(false); // robust is false by default (turn it on for 'multithresh') 
    Bool doAnnulus(false); // turn off stats on an annulus
    if (alg.contains("multithresh") ) {
       robust=true;
       // define an annulus 
       Float outerRadius=0.0;
       Float innerRadius=1.0;
       if (imstore->hasPB()) {
          LatticeExpr<Bool> blat;
          //use pb based annulus for statistics
          if (doAnnulus) {
              outerRadius = 0.2;
              innerRadius = 0.3; 
              blat=LatticeExpr<Bool> (iif(( *imstore->pb() < innerRadius && *imstore->pb() > outerRadius) , True, False) );
          }
          else {
              blat=LatticeExpr<Bool> ((*imstore->pb()).pixelMask());
          }
          TempImage<Float>* testres = new TempImage<Float>(imstore->residual()->shape(), imstore->residual()->coordinates(), memoryToUse()); 
          testres->set(1.0);
          testres->attachMask(blat);
              
          if (ntrue(testres->getMask()) > 0 ) {
              String pbname = imstore->getName()+".pb";
              if (doAnnulus) { 
                  LELmask=pbname+"<"+String::toString(innerRadius)+" && "+pbname+">"+String::toString(outerRadius);   
              }
          }
          delete testres; testres=0;
       }
    }

    //use new noise calc.
    //Bool useoldstats(False);
 
    // at this point tempres has pbmask applied
    LatticeExpr<Bool> pbmask(tempres->pixelMask());

    Record thestats = calcImageStatistics(*tempres, LELmask, region_ptr, robust);
    Array<Double> maxs, mins, rmss, mads;
    thestats.get(RecordFieldId("max"), maxs);
    thestats.get(RecordFieldId("rms"), rmss);

    Record thenewstats; 
    if (!(iterdone==0 && robuststatsrec.nfields()) ) { // this is an indirect way to check if initial stats by nsigma threshold is already run.
    if (fastnoise) { // use a faster but less robust noise determination
      thenewstats = thestats;
      os<< LogIO::DEBUG1 << " *** Using classic image statistics ***"<<LogIO::POST;
      os<< LogIO::DEBUG1 << "All rms's on the input image -- rms.nelements()="<<rmss.nelements()<<" rms="<<rmss<<LogIO::POST;
      //os<< LogIO::DEBUG1 << "All max's on the input image -- max.nelements()="<<maxs.nelements()<<" max="<<maxs<<LogIO::POST;
      if (alg.contains("multithresh")) {
        thestats.get(RecordFieldId("medabsdevmed"), mads);
        os<< LogIO::DEBUG1 << "All MAD's on the input image -- mad.nelements()="<<mads.nelements()<<" mad="<<mads<<LogIO::POST;
      }
    }
    else {
    // Revised version of calcRobustImageStatistics  (previous one- rename to calcRobustImageStatisticsOld)
    //Record thenewstats = calcRobustImageStatistics(*tempres, *tempmask, pbmask, LELmask, region_ptr, robust, chanflag);
      try{
	thenewstats = calcRobustImageStatistics(*tempres, *tempmask, pbmask, LELmask, region_ptr, robust, chanflag);
      }
      catch( AipsError &x )
      {
	//now that there are part images that are masked...should just not proceed rather than throw an exception
	if(x.getMesg().contains("zero element data"))
	  return;
	else
	  throw(x);
      }
      Array<Double> newmaxs, newmins, newrmss, newmads;
      thenewstats.get(RecordFieldId("max"), newmaxs);
      thenewstats.get(RecordFieldId("rms"), newrmss);
      os<< LogIO::DEBUG1 << "*** Using the new image statistics *** "<<LogIO::POST;
      os<< LogIO::DEBUG1 << "All NEW rms's on the input image -- rms.nelements()="<<newrmss.nelements()<<" rms="<<newrmss<<LogIO::POST;
      //os<< LogIO::DEBUG1 << "All NEW max's on the input image -- max.nelements()="<<newmaxs.nelements()<<" max="<<newmaxs<<LogIO::POST;
      if (alg.contains("multithresh")) {
         thenewstats.get(RecordFieldId("medabsdevmed"), newmads);
         os<< LogIO::DEBUG1 << "All NEW MAD's on the input image -- mad.nelements()="<<newmads.nelements()<<" mad="<<newmads<<LogIO::POST;
      }
    }
    os<<LogIO::NORMAL3<<" set the stats to what is calculated here" <<LogIO::POST;
    robuststatsrec=thenewstats;
    } 
    else {
      thenewstats=robuststatsrec; // use existing
    }
    //os<<" current robuststatsrec nfields="<<robuststatsrec.nfields()<<LogIO::POST;
    

    os<<LogIO::NORMAL <<"SidelobeLevel = "<<imstore->getPSFSidelobeLevel()<<LogIO::POST;
    itsSidelobeLevel = imstore->getPSFSidelobeLevel();
    //os<< "mask algortihm ="<<alg<< LogIO::POST;
    if (alg==String("") || alg==String("onebox")) {
      //cerr<<" calling makeAutoMask(), simple 1 cleanbox around the max"<<endl;
      makeAutoMask(imstore);
    }
    else if (alg==String("thresh")) {
      autoMaskByThreshold(*tempmask, *tempres, *imstore->psf(), qreso, resbybeam, qthresh, fracofpeak, 
                          thestats, sigma, nmask, autoadjust);
    }
    else if (alg==String("thresh2")) {
      autoMaskByThreshold2(*tempmask, *tempres, *imstore->psf(), qreso, resbybeam, qthresh, fracofpeak, thestats, sigma, nmask);
    }
    else if (alg==String("multithresh")) {
      autoMaskByMultiThreshold(*tempmask, posmask, *tempres, *imstore->psf(), thestats, thenewstats, iterdone, chanflag, minpercentchange, itsSidelobeLevel, sidelobethreshold, noisethreshold, lownoisethreshold, negativethreshold, cutthreshold, smoothfactor, minbeamfrac, growiterations, dogrowprune, verbose, isthresholdreached);
    }

    // this did not work (it won't physically remove the mask from the image 
    /***
    if (imstore->mask()->hasPixelMask()) {
      imstore.get()->mask()->removeRegion(fname, RegionHandler::Any, False);
      cerr<<"imstore->mask()->name()="<<imstore->mask()->name()<<endl;
      cerr<<" mask: "<<fname<<" exist on disk? ="<<File(fname).isDirectory()<<endl;
    }
    ***/
    {
      Array<Float> updatedMaskData;
      tempmask->get(updatedMaskData);
      imstore->mask()->put(updatedMaskData);
    }
    //tempmask->get(maskdata);
    //imstore->mask()->put(maskdata);
    delete tempmask; tempmask=0;
    //delete temppsf; temppsf=0;
    delete tempres; tempres=0;
  }

  Record SDMaskHandler::calcImageStatistics(ImageInterface<Float>& res, String& LELmask,  Record* regionPtr, const Bool robust )
  { 
    LogIO os( LogOrigin("SDMaskHandler","calcImageStatistics",WHERE) );
    TempImage<Float>* tempres = new TempImage<Float>(res.shape(), res.coordinates(), memoryToUse()); 
    Array<Float> resdata;
    //
    
    res.get(resdata);
    tempres->put(resdata);
    // if input image (res) has a pixel mask, make sure to honor it so the region is exclude from statistics
    if (res.hasPixelMask()) {
      tempres->attachMask(res.pixelMask());
    }
      
    tempres->setImageInfo(res.imageInfo());
    std::shared_ptr<casacore::ImageInterface<Float> > tempres_ptr(tempres);
    
    // 2nd arg is regionRecord, 3rd is LELmask expression and those will be AND 
    // to define a region to be get statistics
    //ImageStatsCalculator imcalc( tempres_ptr, 0, "", False); 
    //String lelstring = pbname+">0.92 && "+pbname+"<0.98";
    //cerr<<"lelstring = "<<lelstring<<endl;
    //cerr<<"LELMask="<<LELmask<<endl;
    ImageStatsCalculator<Float> imcalc( tempres_ptr, regionPtr, LELmask, False); 
    Vector<Int> axes(2);
    axes[0] = 0;
    axes[1] = 1;
    imcalc.setAxes(axes);
    imcalc.setRobust(robust);
    Record thestats = imcalc.statistics();

    //cerr<<"thestats="<<thestats<<endl;
    //Array<Double> max, min, rms, mad;
    //thestats.get(RecordFieldId("max"), max);
    return thestats;
  }

  // robust image statistics for better noise estimation
  Record SDMaskHandler::calcRobustImageStatisticsOld(ImageInterface<Float>& res, ImageInterface<Float>&  prevmask , LatticeExpr<Bool>& pbmask, String& LELmask,  Record* regionPtr, const Bool robust, Vector<Bool>& chanflag )
  { 
    LogIO os( LogOrigin("SDMaskHandler","calcRobustImageStatisticsOld",WHERE) );

    //Bool debugStats(true);

    Array<Float> wholemaskdata;
    IPosition maskshp = prevmask.shape();
    IPosition maskstart(4,0);
    IPosition masklength(4,maskshp(0),maskshp(1), 1, 1);
    Slicer masksl(maskstart, masklength);
    prevmask.doGetSlice(wholemaskdata,masksl); // single plane
    Float npixwholemask = sum(wholemaskdata); 
    Bool fullmask(False);
    if (npixwholemask == (Float) maskshp(0) * (Float) maskshp(1) ) {
       fullmask=True;
       os<<LogIO::DEBUG1 <<"Appears to be fully masked! npix for the whole mask ="<<npixwholemask<<LogIO::POST;
     }
    //TempImage<Float>* tempres = new TempImage<Float>(res.shape(), res.coordinates(), memoryToUse()); 
    //Array<Float> resdata;
    
    //res.get(resdata);
    //tempres->put(resdata);
    // if input image (res) has a pixel mask, make sure to honor it so the region is exclude from statistics
    //if (res.hasPixelMask()) {
    //  tempres->attachMask(res.pixelMask());
    //}
    if (nfalse(pbmask.getMask())) {
      os<<LogIO::DEBUG1<<"has pbmask"<<LogIO::POST;
    }
   
    //check Stokes and spectral axes
    //IPosition shp = tempres->shape();
    IPosition shp = res.shape();
    //Int specaxis = CoordinateUtil::findSpectralAxis(tempres->coordinates());
    Int specaxis = CoordinateUtil::findSpectralAxis(res.coordinates());
    uInt nchan = shp(specaxis);
    Vector<Stokes::StokesTypes> whichPols;
    //Int stokesaxis = CoordinateUtil::findStokesAxis(whichPols, tempres->coordinates());
    Int stokesaxis = CoordinateUtil::findStokesAxis(whichPols, res.coordinates());
    uInt nStokes= shp(stokesaxis);
    uInt iaxis2;
    uInt iaxis3;
   
    //TempImage<Bool> pbmaskim(shp, tempres->coordinates(), memoryToUse());
    TempImage<Bool> pbmaskim(shp, res.coordinates(), memoryToUse());
    pbmaskim.copyData(pbmask);
 
    //check dimensions of tempres, prevmask, pbmask 
    if (prevmask.shape()!=shp) {
      throw(AipsError("Mismatch in shapes of the previous mask and residual image"));
    }
    else if (pbmask.shape()!=shp) {
      throw(AipsError("Mismatch in shapes of pbmask and residual image"));
    }

    //for stats storage
    IPosition statshape(2, shp(2), shp(3));
    Array<Double> outMins(statshape);
    Array<Double> outMaxs(statshape);
    Array<Double> outRmss(statshape);
    Array<Double> outMads(statshape);
    Array<Double> outMdns(statshape);

    for (uInt istokes = 0; istokes < nStokes; istokes++) {
      std::vector<double> mins, maxs, rmss, mads, mdns;
      for (uInt ichan = 0; ichan < nchan; ichan++ ) {
        if (stokesaxis==3) {
          iaxis2 = ichan;
          iaxis3 = istokes;
        }
        else {
          iaxis2 = istokes;
          iaxis3 = ichan;
        } 
        if (chanflag.nelements()==0 || !chanflag(ichan)) { // get new stats
        // check if mask is empty (evaulated as the whole input mask )
        Array<Float> maskdata; 
        //IPosition maskshape = prevmask.shape();
        //Int naxis = maskshape.size();
        //IPosition blc(naxis,0);
        //IPosition trc=maskshape-1;
        //Slicer sl(blc,trc,Slicer::endIsLast);
        IPosition start(4, 0, 0, iaxis2, iaxis3);
        IPosition length(4, shp(0),shp(1), 1, 1);
        Slicer sl(start, length);
        //os<<"slicer for subimage start="<<start<<" length="<<length<<LogIO::POST;
        // create subimage (slice) of tempres
        AxesSpecifier aspec(False);
        SubImage<Float>* subRes = new SubImage<Float>(res, sl, aspec, True);
        TempImage<Float>* tempSubRes = new TempImage<Float>(subRes->shape(), subRes->coordinates(), memoryToUse());
        tempSubRes->copyData(*subRes);
        SubImage<Float>* subprevmask = new SubImage<Float>(prevmask, sl, aspec, True);
        SubImage<Bool>* subpbmask = new SubImage<Bool>(pbmaskim, sl, aspec, True);

        prevmask.doGetSlice(maskdata,sl); // single plane
        Float nmaskpix = sum(maskdata);
        Array<Bool> booldata;
        pbmask.doGetSlice(booldata,sl); // single plane pbmask
        //os<<"valid pix ntrue(booldata)="<<ntrue(booldata)<<LogIO::POST;

        if (nmaskpix==0 ) {
          if (ntrue(booldata)) {
            os<<LogIO::DEBUG1<<"No existing mask but apply pbmask..."<<LogIO::POST;
             tempSubRes->attachMask(*subpbmask);
             //os<<"ntrue tempres pixelmask=="<<ntrue(tempres->getMask())<<LogIO::POST;
          }
        }
        else if (!fullmask) {
          os<<LogIO::DEBUG1<<"Do the image statitistics in a region outside the mask..."<<LogIO::POST;
          LatticeExpr<Bool> outsideMaskReg;
          //if (res.hasPixelMask()) {
          if (nfalse(booldata)) {
            //LatticeExpr<Bool> pbmask(iif(res.pixelMask());
            outsideMaskReg = LatticeExpr<Bool> (iif(*subprevmask == 1.0 || !*subpbmask, False, True));
            // for debug
            //if (debugStats) {
            //  PagedImage<Float> pbmaskSave(res.shape(), res.coordinates(), "pbmasksaved.im");
            //  LatticeExpr<Float> temppbmasksave(iif(subpbmask, 1.0, 0.0));
            //  pbmaskSave.copyData(temppbmasksave);
            //}
          }
          else {
            outsideMaskReg = LatticeExpr<Bool> (iif(*subprevmask == 1.0, False, True));
            //LatticeExpr<Bool> outsideMaskReg(iif((*subprevmask == 1.0 || !*subpbmask, False, True));
          }
          //tempres->attachMask(outsideMaskReg);
          tempSubRes->attachMask(outsideMaskReg);
        }
    
        //DEBUG
        //if(debugStats) {
        //  PagedImage<Float> temptempIm(res.shape(), res.coordinates(), "temptempmask.im");
        //  temptempIm.copyData(prevmask);
        //  PagedImage<Float> temptempResIm(res.shape(), res.coordinates(), "temptempres.im");
        //  temptempResIm.copyData(*tempres);
        //  temptempResIm.makeMask("maskcopy",True, True);
        //  if (tempres->hasPixelMask()) {
        //    temptempResIm.pixelMask().put((tempres->pixelMask()).get());
        //  }
        //} //for debug

        std::shared_ptr<casacore::ImageInterface<Float> > tempres_ptr(tempSubRes);
    
        // 2nd arg is regionRecord, 3rd is LELmask expression and those will be AND 
        // to define a region to be get statistics
        //ImageStatsCalculator imcalc( tempres_ptr, 0, "", False); 
        ImageStatsCalculator<Float> imcalc( tempres_ptr, regionPtr, LELmask, True); 
        Vector<Int> axes(2);
        axes[0] = 0;
        axes[1] = 1;
        imcalc.setAxes(axes);

        // for an empty (= no mask) mask, use Chauvenet algorithm with maxiter=5 and zscore=-1
        if (nmaskpix==0.0 || fullmask ) {
          os<<"Using Chauvenet algorithm for image statistics"<<LogIO::POST;
          imcalc.configureChauvenet(Double(-1.0), Int(5));
        }
        imcalc.setRobust(robust);
        Record thestats = imcalc.statistics();

        Array<Double> arrmins, arrmaxs, arrrmss, arrmads, arrmdns;  
    
        // repack 
        thestats.get(RecordFieldId("min"), arrmins);
        thestats.get(RecordFieldId("max"), arrmaxs);
        thestats.get(RecordFieldId("rms"), arrrmss);
        //robust = True only 
        if (robust) { 
          thestats.get(RecordFieldId("medabsdevmed"), arrmads);
          thestats.get(RecordFieldId("median"), arrmdns);
        }
        if (arrmaxs.nelements()==0 ){
          throw(AipsError("No image statisitics is returned. Possible the whole image is masked."));
        }
        IPosition zeroindx(arrmins.ndim(), 0);

        mins.push_back(arrmins(zeroindx));
        maxs.push_back(arrmaxs(zeroindx));
        rmss.push_back(arrrmss(zeroindx));
        if (robust) { 
          mads.push_back(arrmads(zeroindx));
          mdns.push_back(arrmdns(zeroindx));
        }
        //os<<"deleting tempSubRes"<<LogIO::POST;
        //delete tempSubRes; tempSubRes=0;
        delete subRes; subRes=0;
        delete subprevmask; subprevmask=0;
        delete subpbmask; subpbmask=0;

        }// if-ichanflag end
        else {
          mins.push_back(Double(0.0));
          maxs.push_back(Double(0.0));
          rmss.push_back(Double(0.0));
          if (robust) { 
            mads.push_back(Double(0.0));
            mdns.push_back(Double(0.0));
          }
        } 
      } // chan-for-loop end
      //os<<" rms vector for stokes="<<istokes<<" : "<<rmss<<LogIO::POST;
      //os<<"outMins.shape="<<outMins.shape()<<LogIO::POST;
        
      IPosition start(2, istokes, 0);
      IPosition length(2, 1, nchan); // slicer per Stokes
      //Slicer sl(blc, trc,Slicer::endIsLast );
      Slicer slstokes(start, length);
      //cerr<<"set outMin slstokes="<<slstokes<<" to the mins vector"<<endl;
      Vector<Double> minvec(mins);
      Vector<Double> maxvec(maxs);
      Vector<Double> rmsvec(rmss);
      //os<<"stl vector rmss"<<rmss<<LogIO::POST;
      //os<<"rmsvec.shape="<<rmsvec.shape()<<" rmsvec="<<rmsvec<<LogIO::POST;
      Matrix<Double> minmat(minvec);
      Matrix<Double> maxmat(maxvec);
      //Matrix<Double> rmsmat((Vector<Double>) rmss);
      Matrix<Double> rmsmat(rmsvec);
      //os<<"Intial shape of rmsmat="<<rmsmat.shape()<<LogIO::POST;
      //os<<"Intial content of rmsmat="<<rmsmat<<LogIO::POST;
      // copyValues do not work if there is a degerate axis in front
      minmat.resize(IPosition(2, nchan, 1),True);
      maxmat.resize(IPosition(2, nchan, 1),True);
      rmsmat.resize(IPosition(2, nchan, 1),True);
      Array<Double> minarr = transpose(minmat);
      Array<Double> maxarr = transpose(maxmat);
      Array<Double> rmsarr = transpose(rmsmat);
      if (robust) {
        Vector<Double> madvec(mads);
        Vector<Double> mdnvec(mdns);
        Matrix<Double> madmat(madvec);
        Matrix<Double> mdnmat(mdnvec);
        madmat.resize(IPosition(2, nchan, 1),True);
        mdnmat.resize(IPosition(2, nchan, 1),True);
        Array<Double> madarr = transpose(madmat);
        Array<Double> mdnarr = transpose(mdnmat);
        outMads(slstokes) = madarr;
        outMdns(slstokes) = mdnarr;
        //os<<"madarr="<<madarr<<LogIO::POST;
        //os<<"mdnarr="<<mdnarr<<LogIO::POST;
      }
      //os<<"rmsarr="<<rmsarr<<LogIO::POST;
      outMins(slstokes) = minarr;
      outMaxs(slstokes) = maxarr;
      outRmss(slstokes) = rmsarr;
      //cerr<<"Done setting outMin sltokes="<<slstokes<<" to the mins vector"<<endl;
    } // stokes-for-loop end

    Record theOutStatRec;
    if (shp(2) == 1) { //Single Stokes plane
      // remove degenerate axis 
      theOutStatRec.define("min", outMins.nonDegenerate(IPosition(1,1)));
      theOutStatRec.define("max", outMaxs.nonDegenerate(IPosition(1,1)));
      theOutStatRec.define("rms", outRmss.nonDegenerate(IPosition(1,1)));
      if (robust) {
        theOutStatRec.define("medabsdevmed", outMads.nonDegenerate(IPosition(1,1)));
        theOutStatRec.define("median", outMdns.nonDegenerate(IPosition(1,1)));
      }
    }
    else if(shp(3) == 1) { //Single channel plane
      theOutStatRec.define("min", outMins.nonDegenerate(IPosition(1,0)));
      theOutStatRec.define("max", outMaxs.nonDegenerate(IPosition(1,0)));
      theOutStatRec.define("rms", outRmss.nonDegenerate(IPosition(1,0)));
      if (robust) {
        theOutStatRec.define("medabsdevmed", outMads.nonDegenerate(IPosition(1,0)));
        theOutStatRec.define("median", outMdns.nonDegenerate(IPosition(1,0)));
      }
    }
    else {
      theOutStatRec.define("min", outMins);
      theOutStatRec.define("max", outMaxs);
      theOutStatRec.define("rms", outRmss);
      if (robust) {
        theOutStatRec.define("medabsdevmed", outMads);
        theOutStatRec.define("median", outMdns);
      }
    }
    return theOutStatRec;
  }

  // robust image statistics for better noise estimation - revised for making it faster 
  // Modified version to do stats all at once
  Record SDMaskHandler::calcRobustImageStatistics(ImageInterface<Float>& res, ImageInterface<Float>&  prevmask , LatticeExpr<Bool>& pbmask, String& LELmask,  Record* regionPtr, const Bool robust, Vector<Bool>& chanflag )
  { 
    LogIO os( LogOrigin("SDMaskHandler","calcRobustImageStatistics",WHERE) );

    //Bool debugStats(true);

    Array<Float> wholemaskdata;
    IPosition maskshp = prevmask.shape();
    IPosition maskstart(4,0);
    IPosition masklength(4,maskshp(0),maskshp(1), 1, 1);
    Slicer masksl(maskstart, masklength);
    prevmask.doGetSlice(wholemaskdata,masksl); // single plane
    Float npixwholemask = sum(wholemaskdata); 
    Bool fullmask(False);

    if (npixwholemask == (Float) maskshp(0) * (Float) maskshp(1) ) {
       fullmask=True;
       os<<LogIO::DEBUG1 <<"Appears to be fully masked! npix for the whole mask ="<<npixwholemask<<LogIO::POST;
    }
    
    //res.get(resdata);
    //tempres->put(resdata);
    // if input image (res) has a pixel mask, make sure to honor it so the region is exclude from statistics
    //if (res.hasPixelMask()) {
    //  tempres->attachMask(res.pixelMask());
    //}
    if (nfalse(pbmask.getMask())) {
      os<<LogIO::DEBUG1<<"has pbmask..."<<LogIO::POST;
      os<<LogIO::DEBUG1<<"-> nfalse(pbmask)="<<nfalse(pbmask.getMask())<<LogIO::POST;
    }
   
    // do stats on a whole cube at once for each algrithms
    //check Stokes and spectral axes
    IPosition shp = res.shape();
    Int specaxis = CoordinateUtil::findSpectralAxis(res.coordinates());
    uInt nchan = shp(specaxis);
    Vector<Stokes::StokesTypes> whichPols;
    Int stokesaxis = CoordinateUtil::findStokesAxis(whichPols, res.coordinates());
    uInt nStokes= shp(stokesaxis);
    uInt iaxis2;
    uInt iaxis3;
   
    //TempImage<Bool> pbmaskim(shp, tempres->coordinates(), memoryToUse());
    //TempImage<Bool> pbmaskim(shp, res.coordinates(), memoryToUse());
    //pbmaskim.copyData(pbmask);
 
    //check dimensions of tempres, prevmask, pbmask 
    if (prevmask.shape()!=shp) {
      throw(AipsError("Mismatch in shapes of the previous mask and residual image"));
    }
    else if (pbmask.shape()!=shp) {
      throw(AipsError("Mismatch in shapes of pbmask and residual image"));
    }

    //for stats storage
    IPosition statshape(2, shp(2), shp(3));
    Array<Double> outMins(statshape);
    Array<Double> outMaxs(statshape);
    Array<Double> outRmss(statshape);
    Array<Double> outMads(statshape);
    Array<Double> outMdns(statshape);

    //do stats for each algortims all at once.
    // 2nd arg is regionRecord, 3rd is LELmask expression and those will be AND 
    // to define a region to be get statistics
    //ImageStatsCalculator imcalc( tempres_ptr, 0, "", False); 
    // Chauvenet over a full image  
    // for an empty (= no mask) mask, use Chauvenet algorithm with maxiter=5 and zscore=-1
    TempImage<Float>* tempRes = new TempImage<Float>(res.shape(), res.coordinates(), memoryToUse());
    tempRes->copyData(res);
    tempRes->attachMask(pbmask);
    std::shared_ptr<casacore::ImageInterface<Float> > tempres_ptr(tempRes);
    ImageStatsCalculator<Float> imcalc( tempres_ptr, regionPtr, LELmask, False); 
    Vector<Int> axes(2);
    axes[0] = 0;
    axes[1] = 1;
    imcalc.setAxes(axes);
    os<<LogIO::DEBUG1<<"Using Chauvenet algorithm for image statistics for a whole cube"<<LogIO::POST;
    imcalc.configureChauvenet(Double(-1.0), Int(5));
    imcalc.setRobust(robust);
    Record thestatsNoMask = imcalc.statistics();

    // do stats outside the mask
    //tempres_ptr.reset();
    Record thestatsWithMask;
    if(!fullmask) {
      TempImage<Float>* tempRes2 = new TempImage<Float>(res.shape(), res.coordinates(), memoryToUse());
      tempRes2->copyData(res);
      os<<LogIO::DEBUG1<<"Do the image statitistics in a region outside the mask..."<<LogIO::POST;
      LatticeExpr<Bool> outsideMaskReg;
      outsideMaskReg = LatticeExpr<Bool> (iif(prevmask == 1.0 || !pbmask, False, True));
      // need this case? for no existance of pb mask? 
      //outsideMaskReg = LatticeExpr<Bool> (iif(prevmask == 1.0, False, True));
      tempRes2->attachMask(outsideMaskReg);
      std::shared_ptr<casacore::ImageInterface<Float> > tempres_ptr2(tempRes2);
      ImageStatsCalculator<Float> imcalc2( tempres_ptr2, regionPtr, LELmask, False);
      imcalc2.setAxes(axes);
      imcalc2.setRobust(robust);
      thestatsWithMask = imcalc2.statistics(); 
    }

    Array<Double> arrminsNoMask, arrmaxsNoMask, arrrmssNoMask, arrmadsNoMask, arrmdnsNoMask;  
    Array<Double> arrmins, arrmaxs, arrrmss, arrmads, arrmdns;  
    thestatsNoMask.get(RecordFieldId("min"), arrminsNoMask);
    thestatsNoMask.get(RecordFieldId("max"), arrmaxsNoMask);
    thestatsNoMask.get(RecordFieldId("rms"), arrrmssNoMask);
    IPosition statsind(arrmins.ndim(), 0);
    //robust = True only 
    if (robust) { 
      thestatsNoMask.get(RecordFieldId("medabsdevmed"), arrmadsNoMask);
      thestatsNoMask.get(RecordFieldId("median"), arrmdnsNoMask);
    } 
    if (!fullmask) { // with Mask stats 
      thestatsWithMask.get(RecordFieldId("min"), arrmins);
      thestatsWithMask.get(RecordFieldId("max"), arrmaxs);
      thestatsWithMask.get(RecordFieldId("rms"), arrrmss);
      if (robust) {
        thestatsWithMask.get(RecordFieldId("medabsdevmed"), arrmads);
        thestatsWithMask.get(RecordFieldId("median"), arrmdns);
      }
    }

    for (uInt istokes = 0; istokes < nStokes; istokes++) {
      std::vector<double> mins, maxs, rmss, mads, mdns;
      for (uInt ichan = 0; ichan < nchan; ichan++ ) {
        if (stokesaxis==3) {
          iaxis2 = ichan;
          iaxis3 = istokes;
          statsind(0) = iaxis2;
          if (nStokes>1) {
            statsind(1) = iaxis3;
          }
        }
        else {
          iaxis2 = istokes;
          iaxis3 = ichan;
          if (nStokes>1) {
            statsind(0) = iaxis2;
            statsind(1) = iaxis3;
          }
          else { // Stokes axis is degenerate
            statsind(0) = iaxis3;
          } 
        }
        
        if (chanflag.nelements()==0 || !chanflag(ichan)) { // get new stats
        // check if mask is empty (evaulated as the whole input mask )
        Array<Float> maskdata; 
        IPosition start(4, 0, 0, iaxis2, iaxis3);
        IPosition length(4, shp(0),shp(1), 1, 1);
        Slicer sl(start, length);
        //os<<"slicer for subimage start="<<start<<" length="<<length<<LogIO::POST;
        // create subimage (slice) of tempres
        //AxesSpecifier aspec(False);
        //SubImage<Float>* subprevmask = new SubImage<Float>(prevmask, sl, aspec, True);
        
        // get chan mask data to evaulate no mask 
        prevmask.doGetSlice(maskdata,sl); // single plane
        Float nmaskpix = sum(maskdata);
        Array<Bool> booldata;

    
        Double min, max, rms, mad, mdn;
        // for an empty (= no mask) mask, use Chauvenet algorithm with maxiter=5 and zscore=-1
        if (nmaskpix==0.0 || fullmask ) {
          os<<"[C"<<ichan<<"] Using Chauvenet algorithm for the image statistics "<<LogIO::POST;
          if(arrminsNoMask.nelements()==0 || arrmaxsNoMask.nelements() ==0 || arrrmssNoMask.nelements() ==0){
            throw(AipsError("No image statistics possible on zero element data"));
          }
          min = arrminsNoMask(statsind);
          max = arrmaxsNoMask(statsind);
          rms = arrrmssNoMask(statsind);
          if (robust) {
              if(arrmadsNoMask.nelements()==0 || arrmdnsNoMask.nelements() ==0 || arrrmssNoMask.nelements() ==0)               
                throw(AipsError("No robust image statistics possible on zero element data"));
             mad = arrmadsNoMask(statsind);
             mdn = arrmdnsNoMask(statsind);
          }
        }
        else {
          //os<<"[C"<<ichan<<"] Using the image statitistics in a region outside the mask"<<LogIO::POST;
          min = arrmins(statsind);
          max = arrmaxs(statsind);
          rms = arrrmss(statsind);
          if (robust) { 
             mad = arrmads(statsind);
             mdn = arrmdns(statsind);
          }
        }

    
        // repack 
        //if (arrmaxs.nelements()==0 ){
        //  throw(AipsError("No image statisitics is returned. Possible the whole image is masked."));
        //}

        mins.push_back(min);
        maxs.push_back(max);
        rmss.push_back(rms);
        if (robust) { 
          mads.push_back(mad);
          mdns.push_back(mdn);
        }
        }// if-ichanflag end
        else {
          mins.push_back(Double(0.0));
          maxs.push_back(Double(0.0));
          rmss.push_back(Double(0.0));
          if (robust) { 
            mads.push_back(Double(0.0));
            mdns.push_back(Double(0.0));
          }
        } 
      } // chan-for-loop end
      //os<<" rms vector for stokes="<<istokes<<" : "<<rmss<<LogIO::POST;
      //os<<"outMins.shape="<<outMins.shape()<<LogIO::POST;
        
      IPosition start(2, istokes, 0);
      IPosition length(2, 1, nchan); // slicer per Stokes
      //Slicer sl(blc, trc,Slicer::endIsLast );
      Slicer slstokes(start, length);
      //cerr<<"set outMin slstokes="<<slstokes<<" to the mins vector"<<endl;
      Vector<Double> minvec(mins);
      Vector<Double> maxvec(maxs);
      Vector<Double> rmsvec(rmss);
      //os<<"stl vector rmss"<<rmss<<LogIO::POST;
      //os<<"rmsvec.shape="<<rmsvec.shape()<<" rmsvec="<<rmsvec<<LogIO::POST;
      Matrix<Double> minmat(minvec);
      Matrix<Double> maxmat(maxvec);
      //Matrix<Double> rmsmat((Vector<Double>) rmss);
      Matrix<Double> rmsmat(rmsvec);
      //os<<"Intial shape of rmsmat="<<rmsmat.shape()<<LogIO::POST;
      //os<<"Intial content of rmsmat="<<rmsmat<<LogIO::POST;
      // copyValues do not work if there is a degerate axis in front
      minmat.resize(IPosition(2, nchan, 1),True);
      maxmat.resize(IPosition(2, nchan, 1),True);
      rmsmat.resize(IPosition(2, nchan, 1),True);
      Array<Double> minarr = transpose(minmat);
      Array<Double> maxarr = transpose(maxmat);
      Array<Double> rmsarr = transpose(rmsmat);
      if (robust) {
        Vector<Double> madvec(mads);
        Vector<Double> mdnvec(mdns);
        Matrix<Double> madmat(madvec);
        Matrix<Double> mdnmat(mdnvec);
        madmat.resize(IPosition(2, nchan, 1),True);
        mdnmat.resize(IPosition(2, nchan, 1),True);
        Array<Double> madarr = transpose(madmat);
        Array<Double> mdnarr = transpose(mdnmat);
        outMads(slstokes) = madarr;
        outMdns(slstokes) = mdnarr;
        //os<<"madarr="<<madarr<<LogIO::POST;
        //os<<"mdnarr="<<mdnarr<<LogIO::POST;
      }
      outMins(slstokes) = minarr;
      outMaxs(slstokes) = maxarr;
      outRmss(slstokes) = rmsarr;
    } // stokes-for-loop end

    Record theOutStatRec;
    if (shp(2) == 1) { //Single Stokes plane
      // remove degenerate axis 
      //os<<"Stokes axis has a single Stokes"<<LogIO::POST;
      theOutStatRec.define("min", outMins.nonDegenerate(IPosition(1,1)));
      theOutStatRec.define("max", outMaxs.nonDegenerate(IPosition(1,1)));
      theOutStatRec.define("rms", outRmss.nonDegenerate(IPosition(1,1)));
      if (robust) {
        theOutStatRec.define("medabsdevmed", outMads.nonDegenerate(IPosition(1,1)));
        theOutStatRec.define("median", outMdns.nonDegenerate(IPosition(1,1)));
      }
    }
    else if(shp(3) == 1) { //Single channel plane
      theOutStatRec.define("min", outMins.nonDegenerate(IPosition(1,0)));
      theOutStatRec.define("max", outMaxs.nonDegenerate(IPosition(1,0)));
      theOutStatRec.define("rms", outRmss.nonDegenerate(IPosition(1,0)));
      if (robust) {
        theOutStatRec.define("medabsdevmed", outMads.nonDegenerate(IPosition(1,0)));
        theOutStatRec.define("median", outMdns.nonDegenerate(IPosition(1,0)));
      }
    }
    else {
      theOutStatRec.define("min", outMins);
      theOutStatRec.define("max", outMaxs);
      theOutStatRec.define("rms", outRmss);
      if (robust) {
        theOutStatRec.define("medabsdevmed", outMads);
        theOutStatRec.define("median", outMdns);
      }
    }
    
    return theOutStatRec;
  }

  void SDMaskHandler::autoMaskByThreshold(ImageInterface<Float>& mask,
                                          const ImageInterface<Float>& res, 
                                          const ImageInterface<Float>& psf, 
                                          const Quantity& resolution,
                                          const Float& resbybeam,
                                          const Quantity& qthresh, 
                                          const Float& fracofpeak,
                                          const Record& stats, 
                                          const Float& sigma, 
                                          const Int nmask,
                                          const Bool autoadjust) 
  {
    LogIO os( LogOrigin("SDMaskHandler","autoMaskByThreshold",WHERE) );
    Array<Double> rms, max;
    Double rmsthresh, minrmsval, maxrmsval, minmaxval, maxmaxval;
    IPosition minrmspos, maxrmspos, minmaxpos, maxmaxpos;
    Int npix;
    //for debug set to True to save intermediate mask images on disk
    Bool debug(False);

    //automask stage selecitons
    Bool dobin(True);
    Bool doregrid(True);
    Bool doconvolve(True);

    // taking account for beam or input resolution
    TempImage<Float> tempmask(mask.shape(), mask.coordinates(), memoryToUse());
    IPosition shp = mask.shape();
    CoordinateSystem incsys = res.coordinates();
    Vector<Double> incVal = incsys.increment(); 
    Vector<String> incUnit = incsys.worldAxisUnits();
    Quantity qinc(incVal[0],incUnit[0]);
    if (resolution.get().getValue() ) {
      //npix = 2*Int(abs( resolution/(qinc.convert(resolution),qinc) ).getValue() );
      npix = Int(abs( resolution/(qinc.convert(resolution),qinc) ).getValue() );
      os << LogIO::NORMAL2 << "Use the input resolution:"<<resolution<<" fo binning "<< LogIO::POST;
      os << LogIO::DEBUG1 << "inc = "<<qinc.getValue(resolution.getUnit())<<LogIO::POST;
    }
    else {
      //use beam from residual or psf
      ImageInfo resInfo = res.imageInfo();
      ImageInfo psfInfo = psf.imageInfo();
      GaussianBeam beam;
      if (resInfo.hasBeam() || psfInfo.hasBeam()) {
        if (resInfo.hasSingleBeam()) {
          beam = resInfo.restoringBeam();  
        }
        else if (resInfo.hasMultipleBeams()) {
          beam = CasaImageBeamSet(resInfo.getBeamSet()).getCommonBeam(); 
        }
        else if (psfInfo.hasSingleBeam()) {
          beam = psfInfo.restoringBeam();  
        }
        else {
          beam = CasaImageBeamSet(psfInfo.getBeamSet()).getCommonBeam(); 
        }
        Quantity bmaj = beam.getMajor();
        Quantity bmin = beam.getMinor();
        if (resbybeam > 0.0 ) {
          //npix = 2*Int( Double(resbybeam) * abs( (bmaj/(qinc.convert(bmaj),qinc)).get().getValue() ) );
          npix = Int( Double(resbybeam) * abs( (bmaj/(qinc.convert(bmaj),qinc)).get().getValue() ) );
          os << LogIO::NORMAL2 << "Use "<< resbybeam <<" x  beam size(maj)="<< Double(resbybeam)*bmaj <<" for binning."<< LogIO::POST;
        }
        else {
          //npix = 2*Int( abs( (bmaj/(qinc.convert(bmaj),qinc)).get().getValue() ) );
          npix = Int( abs( (bmaj/(qinc.convert(bmaj),qinc)).get().getValue() ) );
          os << LogIO::NORMAL2 << "Use a beam size(maj):"<<bmaj<<" for binning."<< LogIO::POST;
        } 
      }
      else {
         throw(AipsError("No restoring beam(s) in the input image/psf or resolution is given"));
      }
    }
    os << LogIO::DEBUG1 << "Acutal bin size used: npix="<<npix<< LogIO::POST;
    if (npix==0) {
      os << "Resolution too small. No binning (nbin=1)  is applied input image to evaluate the threshold." << LogIO::POST;
      npix=1;
    }

    // Determine threshold from input image stats
    stats.get(RecordFieldId("max"), max);
    stats.get(RecordFieldId("rms"), rms);
    minMax(minmaxval,maxmaxval,minmaxpos, maxmaxpos, max);
    minMax(minrmsval,maxrmsval,minrmspos, maxrmspos, rms); 
    os << LogIO::DEBUG1 <<"stats on the image: max="<<maxmaxval<<" rms="<<maxrmsval<<endl;
    if (fracofpeak) {
      rmsthresh = maxmaxval * fracofpeak; 
      //os << LogIO::NORMAL <<"Threshold by fraction of the peak(="<<fracofpeak<<") * max: "<<rmsthresh<< LogIO::POST;
      os << LogIO::DEBUG1 <<"max at "<<maxmaxpos<<", dynamic range = "<<maxmaxval/rms(maxmaxpos) << LogIO::POST;
    }
    else if (sigma) {
      //cerr<<"minval="<<minval<<" maxval="<<maxval<<endl;
      rmsthresh = maxrmsval * sigma;
      //os << LogIO::NORMAL <<"Threshold by sigma(="<<sigma<<")* rms (="<<maxrmsval<<") :"<<rmsthresh<< LogIO::POST;
      os << LogIO::DEBUG1 <<"max rms at "<<maxrmspos<<", dynamic range = "<<max(maxrmspos)/maxrmsval << LogIO::POST;
    }      
    else {
      rmsthresh = qthresh.getValue(Unit("Jy"));
      if ( rmsthresh==0.0 ) 
        { throw(AipsError("Threshold for automask is not set"));}
    }
    //os << LogIO::NORMAL2 <<" thresh="<<rmsthresh<<LogIO::POST;


    TempImage<Float>* tempIm2 = new TempImage<Float>(res.shape(), res.coordinates(), memoryToUse());
    TempImage<Float>* tempIm = new TempImage<Float>(res.shape(), res.coordinates(), memoryToUse());
    tempIm->copyData(res);    

    SPCIIF tempIm2_ptr(tempIm2);
    SPIIF tempIm3_ptr(tempIm);
    SPIIF tempIm_ptr;
    //
    //binning stage
    if (dobin) {
      tempIm_ptr =  makeMaskFromBinnedImage(res, npix, npix, fracofpeak, sigma, nmask, autoadjust, rmsthresh);
      //for debugging: save the mask at this stage
      if (debug) {
        PagedImage<Float> tempBinIm(TiledShape(tempIm_ptr.get()->shape()), tempIm_ptr.get()->coordinates(), "binnedThresh.Im");
        tempBinIm.copyData(*(tempIm_ptr.get()));
      }
    }
    if (doregrid) {
      //regrid
      os << LogIO::DEBUG1 <<" now regridding..."<<LogIO::POST;
      IPosition axes(3,0, 1, 2);
      Vector<Int> dirAxes = CoordinateUtil::findDirectionAxes(incsys);
      axes(0) = dirAxes(0);
      axes(1) = dirAxes(1);
      axes(2) = CoordinateUtil::findSpectralAxis(incsys);
      Record* dummyrec = 0;
      SPCIIF inmask_ptr(tempIm_ptr);
      ImageRegridder<Float> regridder(inmask_ptr, "", tempIm2_ptr, axes, dummyrec, "", True, shp);
      regridder.setMethod(Interpolate2D::LINEAR);
      tempIm_ptr = regridder.regrid();
      //for debugging: save the mask at this stage
      if (debug) {
        PagedImage<Float> tempGridded(TiledShape(tempIm_ptr.get()->shape()), tempIm_ptr.get()->coordinates(), "binAndGridded.Im");
        tempGridded.copyData(*(tempIm_ptr.get()));
      }
    }
    else {
      tempIm_ptr = tempIm3_ptr;
    }
    if (doconvolve) {
    //
      SPIIF outmask = convolveMask(*(tempIm_ptr.get()), npix, npix);
      tempIm_ptr = outmask;
      //
      //for debugging: save the mask at this stage
      if (debug) { 
        PagedImage<Float> tempconvIm(TiledShape(tempIm_ptr.get()->shape()), tempIm_ptr.get()->coordinates(),"convolved.Im");
        tempconvIm.copyData(*(tempIm_ptr.get()));
      }
      //os<<"done convolving the mask "<<LogIO::POST;
    }

    //os <<"Final thresholding with rmsthresh/afactor="<< rmsthresh/afactor <<LogIO::POST;
    //LatticeExpr<Float> themask( iif( *(tempIm_ptr.get()) > rmsthresh/afactor, 1.0, 0.0 ));
    // previous 1/0 mask (regridded), max pix value should be <1.0, take arbitary cut off at 0.1
    LatticeExpr<Float> themask( iif( *(tempIm_ptr.get()) > 0.1, 1.0, 0.0 ));
    if (res.hasPixelMask()) {
      LatticeExpr<Bool>  pixmask(res.pixelMask()); 
      mask.copyData( (LatticeExpr<Float>)( iif((mask + themask) > 0.0 && pixmask, 1.0, 0.0  ) ) );
      os <<LogIO::DEBUG1 <<"add previous mask, pbmask and the new mask.."<<LogIO::POST;
    }
    else {
      //for debug
      /***
      PagedImage<Float> tempthemask(TiledShape(tempIm_ptr.get()->shape()), tempIm_ptr.get()->coordinates(),"tempthemask.Im");
      tempthemask.copyData(themask);
      ***/

      //os <<"Lattice themask is created..."<<LogIO::POST;
      //LatticeExpr<Float> themask( iif( tempconvim > rmsthresh/afactor, 1.0, 0.0 ));
      mask.copyData( (LatticeExpr<Float>)( iif((mask + themask) > 0.0, 1.0, 0.0  ) ) );
      os <<LogIO::DEBUG1 <<"add previous mask and the new mask.."<<LogIO::POST;
    }
  }

  void SDMaskHandler::autoMaskByThreshold2(ImageInterface<Float>& mask,
                                          const ImageInterface<Float>& res, 
                                          const ImageInterface<Float>& psf, 
                                          const Quantity& resolution,
                                          const Float& resbybeam,
                                          const Quantity& qthresh, 
                                          const Float& fracofpeak,
                                          const Record& stats, 
                                          const Float& sigma, 
                                          const Int nmask) 
  {
    LogIO os( LogOrigin("SDMaskHandler","autoMaskByThreshold2",WHERE) );
    Array<Double> rms, max;
    Double rmsthresh, minrmsval, maxrmsval, minmaxval, maxmaxval;
    IPosition minrmspos, maxrmspos, minmaxpos, maxmaxpos;
    Int npix;
    Int beampix;

    //for debug set to True to save intermediate mask images on disk
    //Bool debug(False);

    // taking account for beam or input resolution
    TempImage<Float> tempmask(mask.shape(), mask.coordinates(), memoryToUse());
    IPosition shp = mask.shape();
    CoordinateSystem incsys = res.coordinates();
    Vector<Double> incVal = incsys.increment(); 
    Vector<String> incUnit = incsys.worldAxisUnits();
    Quantity qinc(incVal[0],incUnit[0]);
    if (resolution.get().getValue() ) {
      npix = Int(abs( resolution/(qinc.convert(resolution),qinc) ).getValue() );
      beampix = Int( C::pi * npix * npix /(4.*C::ln2)); 
      os << LogIO::NORMAL2 << "Use the input resolution:"<<resolution<<" for pruning "<< LogIO::POST;
      os << LogIO::DEBUG1 << "inc = "<<qinc.getValue(resolution.getUnit())<<LogIO::POST;
    }
    else {
      //use beam from residual or psf
      ImageInfo resInfo = res.imageInfo();
      ImageInfo psfInfo = psf.imageInfo();
      GaussianBeam beam;
      Int npixmin;
      if (resInfo.hasBeam() || psfInfo.hasBeam()) {
        if (resInfo.hasSingleBeam()) {
          beam = resInfo.restoringBeam();  
        }
        else if (resInfo.hasMultipleBeams()) {
          beam = CasaImageBeamSet(resInfo.getBeamSet()).getCommonBeam(); 
        }
        else if (psfInfo.hasSingleBeam()) {
          beam = psfInfo.restoringBeam();  
        }
        else {
          beam = CasaImageBeamSet(psfInfo.getBeamSet()).getCommonBeam(); 
        }
        Quantity bmaj = beam.getMajor();
        Quantity bmin = beam.getMinor();
        if (resbybeam > 0.0 ) {
          npix = Int( Double(resbybeam) * abs( (bmaj/(qinc.convert(bmaj),qinc)).get().getValue() ) );
          npixmin = Int( Double(resbybeam) * abs( (bmin/(qinc.convert(bmin),qinc)).get().getValue() ) );
          beampix = Int(C::pi * npix * npixmin / (4. * C::ln2));
          
          os << LogIO::NORMAL2 << "Use "<< resbybeam <<" x  beam size(maj)="<< Double(resbybeam)*bmaj <<" for pruning."<< LogIO::POST;
        }
        else {
          npix = Int( abs( (bmaj/(qinc.convert(bmaj),qinc)).get().getValue() ) );
          npixmin = Int(  abs( (bmin/(qinc.convert(bmin),qinc)).get().getValue() ) );
          beampix = Int(C::pi * npix * npixmin / (4. * C::ln2));
          os << LogIO::NORMAL2 << "Use a beam size(maj):"<<bmaj<<" for pruning."<< LogIO::POST;
        } 
      }
      else {
         throw(AipsError("No restoring beam(s) in the input image/psf or resolution is given"));
      }
    }
    os << LogIO::DEBUG1 << "Acutal bin size used: npix="<<npix<< LogIO::POST;

    // Determine threshold from input image stats
    stats.get(RecordFieldId("max"), max);
    stats.get(RecordFieldId("rms"), rms);
    minMax(minmaxval,maxmaxval,minmaxpos, maxmaxpos, max);
    minMax(minrmsval,maxrmsval,minrmspos, maxrmspos, rms); 
    os << LogIO::DEBUG1 <<"stats on the image: max="<<maxmaxval<<" rms="<<maxrmsval<<endl;
    if (fracofpeak) {
      rmsthresh = maxmaxval * fracofpeak; 
      os << LogIO::NORMAL <<"Threshold by fraction of the peak(="<<fracofpeak<<") * max: "<<rmsthresh<< LogIO::POST;
      os << LogIO::DEBUG1 <<"max at "<<maxmaxpos<<", dynamic range = "<<maxmaxval/rms(maxmaxpos) << LogIO::POST;
    }
    else if (sigma) {
      //cerr<<"minval="<<minval<<" maxval="<<maxval<<endl;
      rmsthresh = maxrmsval * sigma;
      os << LogIO::NORMAL <<"Threshold by sigma(="<<sigma<<")* rms (="<<maxrmsval<<") :"<<rmsthresh<< LogIO::POST;
      os << LogIO::DEBUG1 <<"max rms at "<<maxrmspos<<", dynamic range = "<<max(maxrmspos)/maxrmsval << LogIO::POST;
    }      
    else {
      rmsthresh = qthresh.getValue(Unit("Jy"));
      if ( rmsthresh==0.0 ) 
        { throw(AipsError("Threshold for automask is not set"));}
    }
    os << LogIO::NORMAL2 <<" thresh="<<rmsthresh<<LogIO::POST;

    std::shared_ptr<ImageInterface<Float> > tempIm_ptr = pruneRegions(res, rmsthresh, nmask, beampix);
    LatticeExpr<Float> themask( iif( *(tempIm_ptr.get()) > rmsthresh, 1.0, 0.0 ));

    //for debug
    /***
    PagedImage<Float> tempthemask(TiledShape(tempIm_ptr.get()->shape()), tempIm_ptr.get()->coordinates(),"tempthemask.Im");
    tempthemask.copyData(themask);
    ***/
    if (res.hasPixelMask()) {
      LatticeExpr<Bool>  pixmask(res.pixelMask()); 
      mask.copyData( (LatticeExpr<Float>)( iif((mask + themask) > 0.0 && pixmask, 1.0, 0.0  ) ) );
      mask.clearCache();
      mask.unlock();
      mask.tempClose();
      os <<LogIO::DEBUG1 <<"Add previous mask, pbmask and the new mask.."<<LogIO::POST;
    }
    else {
      //os <<"Lattice themask is created..."<<LogIO::POST;
      //LatticeExpr<Float> themask( iif( tempconvim > rmsthresh/afactor, 1.0, 0.0 ));
      mask.copyData( (LatticeExpr<Float>)( iif((mask + themask) > 0.0, 1.0, 0.0  ) ) );
      os <<LogIO::DEBUG1 <<"Add previous mask and the new mask.."<<LogIO::POST;
    }
  }//end of makeAutoMaskByThreshold2

  // *** auto-multithresh  ***
  // for implemtation of Amanda's algorithm
  void SDMaskHandler::autoMaskByMultiThreshold(ImageInterface<Float>& mask,
                                          ImageInterface<Float>& posmask,
                                          const ImageInterface<Float>& res, 
                                          const ImageInterface<Float>& psf, 
                                          const Record& stats, 
                                          const Record& robuststats, 
                                          const Int iterdone,
                                          Vector<Bool>& chanFlag,
                                          const Float& minPercentChange,
                                          const Float& sidelobeLevel,
                                          const Float& sidelobeThresholdFactor,
                                          const Float& noiseThresholdFactor,
                                          const Float& lowNoiseThresholdFactor,
                                          const Float& negativeThresholdFactor,
                                          const Float& cutThreshold,
                                          const Float& smoothFactor,
                                          const Float& minBeamFrac, 
                                          const Int growIterations,
                                          const Bool doGrowPrune,
                                          const Bool verbose,
                                          const Bool isthresholdreached)
  {
    LogIO os( LogOrigin("SDMaskHandler","autoMaskByMultiThreshold",WHERE) );
    Array<Double> rmss, maxs, mins, mads, mdns;
    Array<Double> resRmss;
    IPosition minrmspos, maxrmspos, minmaxpos, maxmaxpos, minmadpos, maxmadpos;
    Int nxpix, nypix;

    // % min mask pixel change (to trigger new automask creation per chan plane) to a fractional change 
    Float fracChange = minPercentChange/100.0;

    //store summary info
    Record summaryRec;
    summaryRec.define("sidelobelevel",sidelobeLevel); 

    //for debug set to True to save intermediate mask images on disk
    Bool debug(false); // create additional temp masks for debugging
    Bool debug2(false); // debug2 saves masks before/after prune and binary dilation
    
    //set true to use calcImageStatistics2 and thresholds adjusted for the location (median)
    //Bool newstats(True); // turn on new stats definition of threshold calc.

    //Timer
    Timer timer;

    //debug
    if (debug2) {
      PagedImage<Float> tempcurinmask(mask.shape(), mask.coordinates(), "currrent-in-mask-"+String::toString(iterdone)+".im");
      tempcurinmask.copyData(mask);
    }
      
    // tempmsk: working image for the curret mask
    TempImage<Float> tempmask(mask.shape(), mask.coordinates(), memoryToUse());
    tempmask.set(0);
    // prevmask: mask from previous iter.
    TempImage<Float> prevmask(mask.shape(), mask.coordinates(), memoryToUse());
    // use positive only previous mask
    //prevmask.copyData(LatticeExpr<Float>(mask) );
    prevmask.copyData(LatticeExpr<Float>(posmask) );
    // set up a container for a full cube negative mask
    //if (negativeThresholdFactor > 0) {
    TempImage<Float> thenegmask(mask.shape(), mask.coordinates(), memoryToUse());
    thenegmask.set(0);  
    //}
    // taking account for beam or input resolution
    //IPosition shp = mask.shape();
    CoordinateSystem incsys = res.coordinates();
    Vector<Double> incVal = incsys.increment(); 
    Vector<String> incUnit = incsys.worldAxisUnits();
    Quantity qinc(incVal[0],incUnit[0]);
    //use beam from residual or psf
    ImageInfo resInfo = res.imageInfo();
    ImageInfo psfInfo = psf.imageInfo();

    GaussianBeam beam, modbeam; // modbeam for smooth
    Double pruneSize; 
    if (resInfo.hasBeam() || psfInfo.hasBeam()) {
      if (resInfo.hasSingleBeam()) {
        beam = resInfo.restoringBeam();  
      }
      else if (resInfo.hasMultipleBeams()) {
        beam = CasaImageBeamSet(resInfo.getBeamSet()).getCommonBeam(); 
      }
      else if (psfInfo.hasSingleBeam()) {
        beam = psfInfo.restoringBeam();  
      }
      else {
        beam = CasaImageBeamSet(psfInfo.getBeamSet()).getCommonBeam(); 
      }
      // check
      if(std::isinf( beam.getMajor(Unit("arcsec"))) || std::isinf( beam.getMinor(Unit("arcsec"))) ){
        throw(AipsError("A bad common beam, which is used to set smoothing and pruning sizes for automask. At least one of the axes of the beam is infinite."));
      }
      Quantity bmaj = beam.getMajor();
      Quantity bmin = beam.getMinor();
      //for pruning for now
      // minBeamFrac * beamarea 
      Double beamfrac=1.0;
      if (minBeamFrac > 0.0) {
          beamfrac = (Double) minBeamFrac; 
      }
      Double beampix = pixelBeamArea(beam, incsys);
      pruneSize = beamfrac * beampix;
      //beam in pixels
      nxpix = Int( Double(smoothFactor) * abs( (bmaj/(qinc.convert(bmaj),qinc)).get().getValue() ) );
      nypix = Int( Double(smoothFactor) * abs( (bmin/(qinc.convert(bmin),qinc)).get().getValue() ) );
      modbeam.setMajorMinor(Double(smoothFactor) * bmaj, Double(smoothFactor) * bmin);
      modbeam.setPA(beam.getPA());
      
      os<<LogIO::DEBUG1<<"beam in pixels: B_maj="<<nxpix<<" B_min="<<nypix<<" beam area="<<beampix<<LogIO::POST;
      os<<LogIO::NORMAL <<"prune size="<<pruneSize<<"(minbeamfrac="<<minBeamFrac<<" * beampix="<<beampix<<")"<<LogIO::POST;
      summaryRec.define("pruneregionsize",pruneSize);
    }
    else {
      throw(AipsError("No restoring beam(s) in the input image/psf"));
    }


    //One time parameter checks 
    if (!iterdone) {
        if (cutThreshold >=0.2) {
            os<<LogIO::WARN<<"Faint regions may not be included in the final mask. Consider decreasing cutthreshold."<<LogIO::POST;
        }
    }

    // Determine threshold from input image stats
    stats.get(RecordFieldId("max"), maxs);
    stats.get(RecordFieldId("min"), mins);
    // robuststats may contains only robustrms and medians if it is filled by calcRobustRMS
    if (robuststats.isDefined("rms")) {
      robuststats.get(RecordFieldId("rms"), rmss);
      robuststats.get(RecordFieldId("medabsdevmed"), mads);
      resRmss = mads * 1.4826;
      os<<LogIO::DEBUG1<<" rms from MAD (mads*1.4826)= "<<resRmss<<LogIO::POST;
    }
    else if (robuststats.isDefined("robustrms") ) {
      robuststats.get(RecordFieldId("robustrms"),resRmss); // already converted from MAD to rms  
      os<<LogIO::DEBUG1<<" robustrms from MAD (mads*1.4826)= "<<resRmss<<LogIO::POST;
    }
    os<<LogIO::DEBUG1<<"get mdns"<<LogIO::POST;
    robuststats.get(RecordFieldId("median"), mdns);
    
    //check for pbmask
    IPosition imshp=res.shape();
    IPosition imstart(4, 0, 0, 0, 0);
    IPosition imlength(4, imshp(0),imshp(1), imshp(2), imshp(3));

    Float sidelobeThreshold;
    Float noiseThreshold;
    Float lowNoiseThreshold;
    Float negativeThreshold;
    //
    //determine shape, nchan, npol from residual image
    CoordinateSystem imcoord = res.coordinates(); 
    Int specAxis = CoordinateUtil::findSpectralAxis(imcoord);
    Vector <Stokes::StokesTypes> whichPols;
    Int polAxis = CoordinateUtil::findStokesAxis(whichPols, imcoord);
    Int nchan = -1;
    Int npol = -1;
    if (specAxis != -1) {
      nchan = imshp(specAxis);
    }
    if (polAxis != -1 ) {
      npol = imshp(polAxis);
    }  
    //Int specAxis = CoordinateUtil::findSpectralAxis(res.coordinates());
    // here, now chindx really means index to extract per-plane
    //
    IPosition statshp = mdns.shape();
    IPosition chindx = statshp;
    Int poldim = (npol == -1? 1:npol); 
    Int chandim = (nchan == -1? 1:nchan); 
    Matrix<Float> maskThreshold(poldim, chandim);
    Matrix<Float> lowMaskThreshold(poldim, chandim);
    Matrix<Float> negativeMaskThreshold(poldim, chandim);
    Matrix<String> ThresholdType(poldim, chandim);
    Matrix<Bool> pruned(poldim, chandim);

    //for (uInt ich=0; ich < mads.nelements(); ich++) {
    for (uInt ich=0; ich < (uInt)nchan; ich++) {
    // for loop for Stokes as well?
      for (uInt ipol=0; ipol < (uInt)npol; ipol++) {
        if (nchan!=-1) {
          if(npol==-1 || npol==1) {
            chindx(0) = ich;
          }
          else {
            chindx(0) = ipol;
            chindx(1) = ich;
          }
        }
        else { // pol only
          chindx(0) = ipol;
        }
      
        // turn on a new definition for new stats --- remove old one once tested
      //if (newstats) {
      //  os<<LogIO::DEBUG1<<"Using the new statistics ..."<<LogIO::POST;
      //  sidelobeThreshold = (Float)mdns(chindx) + sidelobeLevel * sidelobeThresholdFactor * (Float)maxs(chindx); 
      //}
      //else {
      //  sidelobeThreshold = sidelobeLevel * sidelobeThresholdFactor * (Float)maxs(chindx); 
      //} 

      // turn on a new definition for new stats --- remove old one once tested
      //if (newstats) {
      //  noiseThreshold = (Float)mdns(chindx) + noiseThresholdFactor * (Float)resRmss(chindx);
      //  lowNoiseThreshold = (Float)mdns(chindx) + lowNoiseThresholdFactor * (Float)resRmss(chindx); 
      //  negativeThreshold = (Float)mdns(chindx) + negativeThresholdFactor * (Float)resRmss(chindx);
      //}
      //else { 
      //  noiseThreshold = noiseThresholdFactor * (Float)resRmss(chindx);
      //  lowNoiseThreshold = lowNoiseThresholdFactor * (Float)resRmss(chindx); 
      //  negativeThreshold = negativeThresholdFactor * (Float)resRmss(chindx);
      //}
      
        // include location (=median)  for both fastnoise=true and false
        Double absmax = max(abs(maxs(chindx)), abs(mins(chindx)));
        // start with no offset 
        sidelobeThreshold = sidelobeLevel * sidelobeThresholdFactor * (Float)absmax; 
        noiseThreshold = noiseThresholdFactor * (Float)resRmss(chindx);
        lowNoiseThreshold = lowNoiseThresholdFactor * (Float)resRmss(chindx); 
        negativeThreshold = negativeThresholdFactor * (Float)resRmss(chindx);
        //negativeMaskThreshold(ich) = (-1.0)*max(sidelobeThreshold, negativeThreshold) + (Float)mdns(chindx);
        negativeMaskThreshold(ipol, ich) = (-1.0)*max(sidelobeThreshold, negativeThreshold) + (Float)mdns(chindx);
        // add the offset
        sidelobeThreshold += (Float)mdns(chindx); 
        noiseThreshold += (Float)mdns(chindx);
        lowNoiseThreshold += (Float)mdns(chindx); 
        negativeThreshold += (Float)mdns(chindx);
        //maskThreshold(ich) = max(sidelobeThreshold, noiseThreshold);
        maskThreshold(ipol, ich) = max(sidelobeThreshold, noiseThreshold);
        //lowMaskThreshold(ich) = max(sidelobeThreshold, lowNoiseThreshold);
        lowMaskThreshold(ipol, ich) = max(sidelobeThreshold, lowNoiseThreshold);
        //ThresholdType(ich) = (maskThreshold(ich) == sidelobeThreshold? "sidelobe": "noise");
        ThresholdType(ipol, ich) = (maskThreshold(ipol, ich) == sidelobeThreshold? "sidelobe": "noise");

        os << LogIO::DEBUG1 <<" sidelobeTreshold="<<sidelobeThreshold<<" noiseThreshold="<<noiseThreshold<<" lowNoiseThreshold="<<lowNoiseThreshold<<LogIO::POST;
        os << LogIO::DEBUG1 <<" negativeThreshold(abs)="<<negativeThreshold<<", all thresholds include  location ="<<(Float)mdns(chindx)<<LogIO::POST;
        //os << LogIO::DEBUG1 <<" Using "<<ThresholdType(ich)<<" threshold for chan "<<String::toString(ich)<<" threshold="<<maskThreshold(ich)<<LogIO::POST;
        os << LogIO::DEBUG1 <<" Using "<<ThresholdType(ipol, ich)<<" threshold for pol "<<String::toString(ipol)<<",  chan "<< String::toString(ich)<<" threshold="<<maskThreshold(ipol, ich)<<LogIO::POST;
      } // for-ipol
    } // for-ich


    //main per plane loop start here
    IPosition planeshp(imshp.nelements(), 1);
    planeshp(0) = imshp(0); 
    planeshp(1) = imshp(1); 
   
    // store in matrix instead of vector to support full pols 
    Matrix<uInt> nreg(poldim, chandim,0);
    Matrix<uInt> npruned(poldim, chandim, 0);
    Vector<Float> dummysizes;
    Matrix<uInt> ngrowreg(poldim, chandim, 0);
    Matrix<uInt> ngrowpruned(poldim, chandim, 0);
    Matrix<Float> negmaskpixs(poldim, chandim ,0);
    Matrix<Bool> allPruned(poldim, chandim, False);
    // for timing info storage
    Vector<Double> timeInitThresh(3,0.0);
    Vector<Double> timePrune(3,0.0);
    Vector<Double> timeSmooth(3,0.0);
    Vector<Double> timeGrow(3,0.0);
    Vector<Double> timePruneGrow(3,0.0);
    Vector<Double> timeSmoothGrow(3,0.0);
    Vector<Double> timeNegThresh(3,0.0);

    Bool perplanetiming(True);

    for (uInt ich=0; ich < (uInt)nchan; ich++) {
      if (npol==1) {
        os << LogIO::NORMAL<< "*** Start auto-multithresh processing for Channel "<<ich<<"***"<<LogIO::POST;
      }
      for (uInt ipol=0; ipol < (uInt)npol; ipol++ ) {  
        if (npol!=1) {
          os << LogIO::NORMAL<< "*** Start auto-multithresh processing for Channel "<<ich<<", Polarization "<<ipol<<"***"<<LogIO::POST;
        }
        // channel skip check
        if (chanFlag(ich)) {
          os << LogIO::NORMAL<<" Skip this channel "<<LogIO::POST;
        }
        else {
          // Below corresponds to createThresholdMask in Amanda's Python code.
            IPosition start(planeshp.nelements(),0);
            if (specAxis != -1) {
              start(specAxis)=ich;
            }
            if (polAxis != -1) {
              start(polAxis)=ipol; 
            } 

            IPosition length(planeshp.nelements(), planeshp(0), planeshp(1), 1, 1);
            Slicer sl(start, length);
            AxesSpecifier aspec(True); // keep degenerate axes
            SubImage<Float> planeResImage(res, sl, aspec, true);    
            TempImage<Float> planeTempMask(planeResImage.shape(), planeResImage.coordinates(), memoryToUse());
            planeTempMask.set(0); // initialize
            //SubImage<Float> subprevmask(prevmask, sl, true, aspec, true);
            // working copy of per-plane previous total mask to be modified. Started with en empty mask. 
            // For an un-touched version of per-plane previous mask, subMask is created at the grow mask step.
            TempImage<Float> subprevmask(planeshp, planeResImage.coordinates(), memoryToUse());
            subprevmask.set(0);
            SubImage<Float> subposmask(posmask, sl, true, aspec, true);
            //Vector<Bool> allPruned(nchan);
            // sigle element vectors for input
            Vector<Bool> chanFlag1elem(1);
            chanFlag1elem(0) = chanFlag(ich);
            Vector<Float> maskThreshold1elem(1);
            maskThreshold1elem(0) = maskThreshold(ipol,ich);
            Vector<Float> lowMaskThreshold1elem(1);
            lowMaskThreshold1elem(0) = lowMaskThreshold(ipol,ich);
            Vector<Float> negativeMaskThreshold1elem(1);
            negativeMaskThreshold1elem(0) = negativeMaskThreshold(ipol,ich);

            // *** Pruning *** 
            if (minBeamFrac > 0.0 ) {
              // do pruning...
              //os<<LogIO::NORMAL<<"Pruning the current mask"<<LogIO::POST;
              os << LogIO::NORMAL3 << "Start thresholding: create an initial mask by threshold" << LogIO::POST;
              timer.mark();
              // make temp mask image consist of the original pix value and below the threshold is set to 0 
              //TempImage<Float> maskedRes(res.shape(), res.coordinates(), memoryToUse());
              // single plane
              TempImage<Float> maskedRes(planeshp, planeResImage.coordinates(), memoryToUse());
              maskedRes.set(0);
              //makeMaskByPerChanThreshold(res, chanFlag, maskedRes, maskThreshold, dummysizes); 
              makeMaskByPerChanThreshold(planeResImage, chanFlag1elem, maskedRes, maskThreshold1elem, dummysizes); 
              timeInitThresh(0)+=timer.real(); timeInitThresh(1)+=timer.user(); timeInitThresh(2)+=timer.system();
              if (perplanetiming) {
                os << LogIO::NORMAL3 << "End thresholding: time to create the initial threshold mask:  real "<< timer.real() 
                   << "s ( user " << timer.user() <<"s, system "<< timer.system() << "s)" << LogIO::POST;
              }
              //if (res.hasPixelMask()) {
              if (planeResImage.hasPixelMask()) {
                //os << LogIO::DEBUG1 <<" HAS MASK....ich="<<ich<<LogIO::POST;
                ArrayLattice<Bool> pixmasklat(planeResImage.getMask()); 
                maskedRes.copyData( (LatticeExpr<Float>)( iif(pixmasklat, maskedRes, 0.0 ) ) );
                //for debug
                //Array<Float> testdata;
                //maskedRes.get(testdata);
                //os<<" current total of pix values="<<sum(testdata)<<LogIO::POST;
              }
              //TODO MOVE THIS SECTION outside the for-loop -DONE 
              //this section need to be move to the end of automask outside of the main chan loop
              //Vector<Bool> allPruned(nchan);
              //if (!iterdone) noMaskCheck(maskedRes, ThresholdType(ipol,ich));
              if (debug2) {
                os<<LogIO::DEBUG2<<"Saving intermediate masks for this cycle: with name tmp****-"<<iterdone<<".im"<<LogIO::POST;
                String tmpfname1="tmpInitThresh-ch"+String::toString(ich)+"pol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im";
                PagedImage<Float> savedPreMask(planeResImage.shape(),planeResImage.coordinates(),tmpfname1);
                savedPreMask.copyData(maskedRes);
              }

              os << LogIO::NORMAL3 << "Start pruning: the initial threshold mask" << LogIO::POST;
              timer.mark();
              //std::shared_ptr<ImageInterface<Float> > tempIm_ptr = pruneRegions2(maskedRes, tempthresh,  -1, pruneSize);
              //temporary storage for single plane results
              Vector<uInt> nreg1elem, npruned1elem;
              Vector<Bool> allPruned1elem(1);
              std::shared_ptr<ImageInterface<Float> > tempIm_ptr = YAPruneRegions(maskedRes, chanFlag1elem, allPruned1elem, nreg1elem, npruned1elem, pruneSize, false);
              nreg(ipol,ich) = nreg1elem(0);
              npruned(ipol, ich) = npruned1elem(0);
              allPruned(ipol, ich) = allPruned1elem(0);
              //tempmask.copyData(*(tempIm_ptr.get()));
              planeTempMask.copyData(*(tempIm_ptr.get()));
              // now this timing for single plane... need to accumlate and report it later????
              timePrune(0)+=timer.real(); timePrune(1)+=timer.user(); timePrune(2)+=timer.system();
              if (perplanetiming) {
                os << LogIO::NORMAL3 << "End pruning: time to prune the initial threshold mask: real " 
                   << timer.real()<< "s (user " << timer.user() <<"s, system "<< timer.system() << "s)" << LogIO::POST;
              }
            
              //if (debug2) {
              //  String tmpfname2="tmpAfterPrune-"+String::toString(iterdone)+".im";
              //  PagedImage<Float> savedPrunedPreThreshMask(res.shape(),res.coordinates(),tmpfname2);
              //  savedPrunedPreThreshMask.copyData(*(tempIm_ptr.get()));
              //}
              //themask = LatticeExpr<Float> ( iif( *(tempIm_ptr.get()) > maskThreshold, 1.0, 0.0 ));
              // Need this?
              //makeMaskByPerChanThreshold(*(tempIm_ptr.get()), tempmask, maskThreshold, dummysizes); 
              //if (debug) {
              //  PagedImage<Float> savedPostPrunedMask(res.shape(),res.coordinates(),"tmp-postPruningPostThreshMask.im");
              //  savedPostPrunedMask.copyData(tempmask);
              //}
            }
            else { // ***** No pruning case ******
              os << LogIO::NORMAL3 << "Start thresholding: create an initial threshold mask" << LogIO::POST;
              timer.mark();
              //tempmask.set(0);
              planeTempMask.set(0);
              //makeMaskByPerChanThreshold(res, chanFlag, tempmask, maskThreshold, dummysizes); 
              makeMaskByPerChanThreshold(planeResImage, chanFlag1elem, planeTempMask, maskThreshold1elem, dummysizes);
              if (debug2) {
                String tmpnopruneinitmask="tmpInitThreshNoprune-ch"+String::toString(ich)+"pol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im";
                 PagedImage<Float> savedThreshmask(planeResImage.shape(), planeResImage.coordinates(), tmpnopruneinitmask);
                 savedThreshmask.copyData(planeTempMask);
              }

              //if (!iterdone) noMaskCheck(tempmask, ThresholdType);
              timePrune(0)+=timer.real(); timePrune(1)+=timer.user(); timePrune(2)+=timer.system();
              if (perplanetiming) {
                os << LogIO::NORMAL3 << "End trehsholding: time to create the initial threshold mask: real "
                   << timer.real()<<"s (user " << timer.user() <<"s, system "<< timer.system() << "s)" << LogIO::POST;
              }
              //tempmask.copyData(themask);
            } // DONE PRUNING STAGE 

            // ***** SMOOTHING *******
            os << LogIO::NORMAL3 << "Start smoothing: the initial threshold mask" << LogIO::POST;
            timer.mark();
            SPIIF outmask = convolveMask(planeTempMask, modbeam );
            if (debug2 ) {
                String tmpfname3="tmp-postSmoothMask-"+String::toString(ich)+"pol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im";
                PagedImage<Float> savedSmoothedMask(planeResImage.shape(),planeResImage.coordinates(),tmpfname3);
                savedSmoothedMask.copyData(*(outmask.get()));
            }


            //clean up (appy cutThreshold to convolved mask image)
            String lelmask("");
            //use standard stats
            Record  smmaskstats = calcImageStatistics(*outmask, lelmask, 0, false);
            Array<Double> smmaskmaxs;
            smmaskstats.get(RecordFieldId("max"),smmaskmaxs);
            Vector<Float> cutThresholdValue(1);

            //if (npol<=1) {
            //  chindx(0) = 0;
            //}
            //else {
            //  chindx(1) = 0;
            //}
            //  cutThresholdValue(ich) = cutThreshold * smmaskmaxs(chindx);
            cutThresholdValue(0) = cutThreshold * smmaskmaxs(IPosition(1,0));
            //os<<LogIO::DEBUG1<<" cutThreshVal("<<ich<<")="<<cutThresholdValue(ich)<<LogIO::POST;
          
            //TempImage<Float> thenewmask(res.shape(),res.coordinates(), memoryToUse());
            //thenewmask.set(0);
            TempImage<Float> thenewmask(planeshp,planeResImage.coordinates(), memoryToUse());
            thenewmask.set(0);
            //makeMaskByPerChanThreshold(*outmask, chanFlag, thenewmask, cutThresholdValue, dummysizes); 
            makeMaskByPerChanThreshold(*outmask, chanFlag1elem, thenewmask, cutThresholdValue, dummysizes); 
            // Per-plane timing
            timeSmooth(0) += timer.real(); timeSmooth(1) += timer.user(); timeSmooth(2) += timer.system(); 
            if (perplanetiming) {
            os << LogIO::NORMAL3 << "End smoothing: time to create the smoothed initial threshold mask: real "<< timer.real()
               <<"s (user " << timer.user() <<"s, system "<< timer.system() << "s)" <<  LogIO::POST;
            }

          /***
          if (!iterdone) {
            if (!isEmptyMask(*(outmask.get())) && isEmptyMask(thenewmask)) os<<LogIO::WARN<<"Removed all regions based by cutthreshold applied to the smoothed mask."<<LogIO::POST;
          }
          ***/
          if (debug2 ) {
              String tmpnewmask="tmp-AfterSmooth-"+String::toString(ich)+"pol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im";
              PagedImage<Float> savedthenewmask(planeResImage.shape(), planeResImage.coordinates(), tmpnewmask);
              savedthenewmask.copyData(thenewmask);
          }

            // ***** GROW STAGE *****
            //
            // take stats on the current mask for setting flags for grow mask : if max < 1 for any spectral plane it will grow the previous mask
            //
            //  Mod: 2017.07.26: modified get stats for prev mask, if channel contains no mask in prev mask it will set flag to skip the channel 
            //Record maskstats = calcImageStatistics(thenewmask, thenewmask, lelmask, 0, false);
            //
            SubImage<Float>  subMask(mask,sl, true, aspec, true);
            if(debug2) {
              String tmpsubmaskName = "tmp-submask-"+String::toString(ich)+"pol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im";
              PagedImage<Float> tmpsubmask(planeResImage.shape(), planeResImage.coordinates(), tmpsubmaskName);
              tmpsubmask.copyData(subMask);
            }
            //Record  maskstats = calcImageStatistics(mask, lelmask, 0, false);
            // per-plane stats now
            Record  maskstats = calcImageStatistics(subMask, lelmask, 0, false);
            Array<Double> maskmaxs;
            maskstats.get(RecordFieldId("max"),maskmaxs);
            IPosition arrshape = maskmaxs.shape();
            uInt naxis=arrshape.size();
            IPosition indx(naxis,0);
            //os<<LogIO::NORMAL<<"arrshape="<<arrshape<<" indx="<<indx<<LogIO::POST;
            //os<<LogIO::NORMAL<<"statshp="<<statshp<<LogIO::POST;
            // ignoring corr for now and assume first axis is channel
            Array<Bool> dogrow(arrshape);
            dogrow.set(false);
            for (uInt i=0; i < arrshape(0); i++) {
              indx(0) = i;
              if (maskmaxs(indx) == 1.0 && !chanFlag1elem(0)) {
                dogrow(indx) = true;
              }
              //For debug
              //if (chanFlag(i)) {
              //  os<<LogIO::NORMAL<<"For dogrow: skipping channel: "<<i<<" chanFlag(i)="<<chanFlag(i)<<" dogrow("<< indx << ")=" <<dogrow(indx)<<LogIO::POST;
              //}
              // set dogrow true for all chans (contraintMask should be able to handle skipping channels )
              //  dogrow(indx) = true;
            }   

            if (iterdone && growIterations>0) { // enter to acutal grow process
              //per-plane timing
              os << LogIO::NORMAL3 << "Start grow mask: growing the previous mask " << LogIO::POST;
              timer.mark();
              //call growMask
              // corresponds to calcThresholdMask with lowNoiseThreshold...
              //TempImage<Float> constraintMaskImage(res.shape(), res.coordinates(), memoryToUse()); 
              // per-plane constraint mask image
              TempImage<Float> constraintMaskImage(planeshp, planeResImage.coordinates(), memoryToUse()); 
              constraintMaskImage.set(0);
              // constrainMask is 1/0 mask
              //makeMaskByPerChanThreshold(res, chanFlag, constraintMaskImage, lowMaskThreshold, dummysizes);
              makeMaskByPerChanThreshold(planeResImage, chanFlag1elem, constraintMaskImage, lowMaskThreshold1elem, dummysizes);
              if(debug2 && ipol==0 && ich==0) {
                os<< LogIO::NORMAL3<<"saving constraint mask " << LogIO::POST;
                PagedImage<Float> beforepruneconstIm(planeResImage.shape(), planeResImage.coordinates(),"tmpConstraint-"+String::toString(iterdone)+".im");
                beforepruneconstIm.copyData(constraintMaskImage);
              }

              // 2017.05.05: should done after multiply by binary dilation 
              //
              // prune the constraintImage
              //if (minBeamFrac > 0.0 ) {
              //  //Double thethresh=0.1;
              // os<<LogIO::NORMAL << "Pruning the constraint mask "<<LogIO::POST;
              // //std::shared_ptr<ImageInterface<Float> > tempPrunedMask_ptr = pruneRegions2(constraintMaskImage, thethresh,  -1, pruneSize);
              //  Vector<Bool> dummy(0);
              //  std::shared_ptr<ImageInterface<Float> > tempPrunedMask_ptr = YAPruneRegions(constraintMaskImage, dummy, pruneSize);
              //  constraintMaskImage.copyData( *(tempPrunedMask_ptr.get()) );
              //}
              //if(debug2) {
              //  PagedImage<Float> afterpruneconstIm(res.shape(), res.coordinates(),"tmpAfterPruneConstraint-"+String::toString(iterdone)+".im");
              //  afterpruneconstIm.copyData(constraintMaskImage);
              //}

              // for mask in binaryDilation, translate it to T/F (if T it will grow the mask region (NOTE currently binary dilation 
              // does opposite T/F interpretation NEED to CHANGE)
              //TempImage<Bool> constraintMask(res.shape(),res.coordinates(), memoryToUse());
              //constraintMask.copyData( LatticeExpr<Bool> (iif(constraintMaskImage > 0, true, false)) );
              TempImage<Bool> constraintMask(planeshp, planeResImage.coordinates(), memoryToUse());
              constraintMask.copyData( LatticeExpr<Bool> (iif(constraintMaskImage > 0, true, false)) );
              // simple structure element for binary dilation
              IPosition axislen(2, 3, 3);
              Array<Float> se(axislen);
              se.set(0);
              se(IPosition(2,1,0))=1.0;
              se(IPosition(2,0,1))=1.0;
              se(IPosition(2,1,1))=1.0;
              se(IPosition(2,2,1))=1.0;
              se(IPosition(2,1,2))=1.0;
              if(debug2 && ich==0 && ipol == 0) {
                String tmpbeforeBD="tmp-BeforeBD-"+String::toString(ich)+"pol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im";
                PagedImage<Float> beforeBinaryDilationIm(planeResImage.shape(), planeResImage.coordinates(),tmpbeforeBD);
                //`beforeBinaryDilationIm.copyData(constraintMaskImage);
                beforeBinaryDilationIm.copyData(subMask);
              }
              // CHECK THIS works for a single plane but 4 dim image 

              subprevmask.set(0);
              binaryDilation(subMask, se, growIterations, constraintMask, dogrow, subprevmask); 
              if(debug2) {
                PagedImage<Float> afterBinaryDilationIm(planeResImage.shape(), planeResImage.coordinates(),"tmpAfterBinaryDilation-"+String::toString(ich)+"pol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im");
                afterBinaryDilationIm.copyData(subprevmask);
              }
              // multiply binary dilated mask by constraintmask
              //prevmask.copyData( LatticeExpr<Float> (constraintMaskImage*prevmask));
              subprevmask.copyData( LatticeExpr<Float> (constraintMaskImage*subprevmask));
              if(debug2) {
                PagedImage<Float> beforepruneconstIm(planeResImage.shape(), planeResImage.coordinates(),"tmpBeforePruneGrowMask-"+String::toString(ich)+"ipol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im");
                beforepruneconstIm.copyData(subprevmask);
              }
              timeGrow(0) += timer.real(); timeGrow(1) += timer.user(); timeGrow(2) += timer.system(); 
              if (perplanetiming) {
                os << LogIO::NORMAL3 << "End grow mask: time to grow the previous mask: real " 
                 << timer.real() <<"s (user "<< timer.user() << "s, system " << timer.system() << "s)" << LogIO::POST;
              }

              // **** pruning on grow mask ****
              if (minBeamFrac > 0.0 && doGrowPrune) {
                //os<<LogIO::NORMAL << "Pruning the growed previous mask "<<LogIO::POST;
                os << LogIO::NORMAL3 << "Start pruning: on the grow mask "<< LogIO::POST;
                timer.mark();
                Vector<Bool> dummy(0);
                Vector<uInt> ngrowreg1elem, ngrowpruned1elem;
                //std::shared_ptr<ImageInterface<Float> > tempPrunedMask_ptr = YAPruneRegions(prevmask, chanFlag, dummy, ngrowreg, ngrowpruned, pruneSize);
             
                std::shared_ptr<ImageInterface<Float> > tempPrunedMask_ptr = YAPruneRegions(subprevmask, chanFlag1elem, dummy, ngrowreg1elem, ngrowpruned1elem, pruneSize, false);
                ngrowreg(ipol,ich) = ngrowreg1elem(0);
                ngrowpruned(ipol,ich) = ngrowpruned1elem(0);
                //prevmask.copyData( *(tempPrunedMask_ptr.get()) );
                subprevmask.copyData( *(tempPrunedMask_ptr.get()) );
                timePruneGrow(0) += timer.real(); timePruneGrow(1) += timer.user(); timePruneGrow(2) += timer.system();
                if (perplanetiming) {
                  os << LogIO::NORMAL3 << "End pruning: time to prune the grow mask: real " 
                    << timer.real() <<"s (user "<< timer.user() << "s, system "<< timer.system() << "s)" << LogIO::POST;
                }
                if(debug2) {
                  String tmpafterprunegrowname="tmpAfterPruneGrowMask-"+String::toString(ich)+"pol"+String::toString(ipol)+"iter"+String::toString(iterdone)+".im";
                  PagedImage<Float> afterpruneconstIm(planeResImage.shape(), planeResImage.coordinates(),tmpafterprunegrowname);
                  afterpruneconstIm.copyData(subprevmask);
               }
              }

              // ***** smoothing on grow mask *****
              os << LogIO::NORMAL3 << "Start smoothing: the grow mask " << LogIO::POST;
              timer.mark();
              ///SPIIF outprevmask = convolveMask( prevmask, modbeam);
              SPIIF outprevmask = convolveMask( subprevmask, modbeam);
              //if (debug) {
              //  PagedImage<Float> postSmoothGrowedMask(res.shape(), res.coordinates(),"tmpPostSmoothGrowMask-"+String::toString(iterdone)+".im");
              //  postSmoothGrowedMask.copyData(*outprevmask);
              //}
              //prevmask.copyData( LatticeExpr<Float> (iif( *(outprevmask.get()) > cutThreshold, 1.0, 0.0 )) );
              // single plane
              Record constmaskstats = calcImageStatistics(*outprevmask, lelmask, 0, false);
              Array<Double> constmaskmaxs;
              constmaskstats.get(RecordFieldId("max"),constmaskmaxs);
              //Vector<Float> constCutThresholdValue(nchan);
              Vector<Float> constCutThresholdValue(1);
              // stats on a single plane now, so no need of chindx
              //if (npol <=1) {
              //  chindx(0) = 0;
              //}
              //else {
              //  chindx(1) = 0;
              //}
              //constCutThresholdValue(0) = cutThreshold * constmaskmaxs(chindx);
              constCutThresholdValue(0) = cutThreshold * constmaskmaxs(IPosition(1,0));
              //prevmask.set(0);
              subprevmask.set(0);
              //makeMaskByPerChanThreshold(*outprevmask, chanFlag, prevmask, constCutThresholdValue, dummysizes); 
              makeMaskByPerChanThreshold(*outprevmask, chanFlag1elem, subprevmask, constCutThresholdValue, dummysizes); 
              //if (debug) {
              //  PagedImage<Float> smoothedGrowedMask(res.shape(), res.coordinates(),"tmpSmoothedGrowMask-"+String::toString(iterdone)+".im");
              //  smoothedGrowedMask.copyData(prevmask);
              //}
              timeSmoothGrow(0) +=  timer.real(); timeSmoothGrow(1) += timer.user(); timeSmoothGrow(2) += timer.system(); 
              if (perplanetiming) {
                os << LogIO::NORMAL3 << "End smoothing: time to create the smoothed grow mask: real " 
                  << timer.real() <<"s (user "<< timer.user() << "s, system " << timer.system() << "s)" << LogIO::POST;
              }
            } //end - GROW (iterdone && dogrowiteration)
          
            // ****** save positive (emission) mask only ******

            // temporary save negative mask from the previous one
            //TempImage<Float> prevnegmask(res.shape(), res.coordinates(), memoryToUse());
            //prevnegmask.copyData( (LatticeExpr<Float>)( iif( (mask - posmask ) > 0.0, 1.0, 0.0 ) ) );

            //
            if (debug2 ) {
              String beforesumSPmaskname= "beforesumSPmask-ch"+String::toString(ich)+"pol"+String::toString(ipol)+"-iter"+String::toString(iterdone)+".im";
              PagedImage<Float> tempsubposmask(TiledShape(subposmask.shape()), subposmask.coordinates(), beforesumSPmaskname);
              tempsubposmask.copyData(subposmask);
              String beforesumSPrevmaskname = "beforesumSPrevmask-ch"+String::toString(ich)+"pol"+String::toString(ipol)+"-iter"+String::toString(iterdone)+".im";
              PagedImage<Float> tempsubprevmask(TiledShape(subposmask.shape()), subposmask.coordinates(), beforesumSPrevmaskname);
              tempsubprevmask.copyData(subprevmask);
            } 



            if (planeResImage.hasPixelMask()) {
              //LatticeExpr<Bool>  pixmask(res.pixelMask()); 
              LatticeExpr<Bool>  pixmask(planeResImage.pixelMask()); 
              // add all positive masks (previous one, grow mask, current thresh mask)
              // mask = untouched prev mask, prevmask=modified prev mask by the grow func, thenewmask=mask by thresh on current residual 
              //posmask.copyData( (LatticeExpr<Float>)( iif((posmask + prevmask + thenewmask ) > 0.0 && pixmask, 1.0, 0.0  ) ) );
              subposmask.copyData( (LatticeExpr<Float>)( iif((subposmask + subprevmask + thenewmask ) > 0.0 && pixmask, 1.0, 0.0  ) ) );
              os <<LogIO::DEBUG1 <<"Add positive previous mask, pbmask and the new mask for this plane"<<LogIO::POST;
            }
            else {
              //posmask.copyData( (LatticeExpr<Float>)( iif((posmask + prevmask + thenewmask ) > 0.0, 1.0, 0.0  ) ) );
              subposmask.copyData( (LatticeExpr<Float>)( iif((subposmask + subprevmask + thenewmask ) > 0.0, 1.0, 0.0  ) ) );
              os <<LogIO::DEBUG1 <<"Add positive previous mask and the new mask.."<<LogIO::POST;
            }

            // **** NEGATIVE MASK creation *****
            //TempImage<Float> thenegmask(res.shape(),res.coordinates(), memoryToUse());
            TempImage<Float> subnegmask(planeshp, planeResImage.coordinates(), memoryToUse());
            subnegmask.set(0);
            //Vector<Float> negmaskpixs;
            Vector<Float> negmaskpixs1elem;
            if (negativeThresholdFactor > 0) { 
              os << LogIO::NORMAL3 << "Start thresholding: create a negative mask" << LogIO::POST;
              timer.mark();
              //os<<LogIO::NORMAL<<"Creating a mask for negative features. "<<LogIO::POST;
              //TempImage<Float> negativeMaskImage(res.shape(), res.coordinates(), memoryToUse()); 
              TempImage<Float> negativeSubMaskImage(planeshp, planeResImage.coordinates(), memoryToUse()); 
              negativeSubMaskImage.set(0);
              //makeMaskByPerChanThreshold(res, chanFlag, negativeMaskImage , negativeMaskThreshold, dummysizes);
              makeMaskByPerChanThreshold(planeResImage, chanFlag1elem, negativeSubMaskImage, negativeMaskThreshold1elem, dummysizes);
              // SPIIF negmask = convolveMask( negativeMaskImage, modbeam);
              SPIIF negmask = convolveMask( negativeSubMaskImage, modbeam);
              // determine the cutthreshold value for negative mask
              Record negmaskstats = calcImageStatistics(*negmask, lelmask, 0, false);
              Array<Double> negmaskmaxs;
              negmaskstats.get(RecordFieldId("max"),negmaskmaxs);
              //Vector<Float> negCutThresholdValue(nchan);
              Vector<Float> negCutThresholdValue(1);
              // 1 dim stats now, so no need of chindx
              //if (npol <= 1) {
              //  chindx(0) = 0;
              //}
              //else {
              //  chindx(1) = 0;
              //}
              negCutThresholdValue(0) = cutThreshold * negmaskmaxs(IPosition(1,0));
              //makeMaskByPerChanThreshold(*negmask, chanFlag, thenegmask, negCutThresholdValue, negmaskpixs); 
              makeMaskByPerChanThreshold(*negmask, chanFlag1elem, subnegmask, negCutThresholdValue, negmaskpixs1elem); 
              negmaskpixs(ipol,ich) = negmaskpixs1elem(0);
              if (isEmptyMask(subnegmask) ){
                os<<"No negative region was found by auotmask."<<LogIO::POST;
              }
              //if (debug) {
              //  PagedImage<Float> temppresmoothnegmask(TiledShape(negativeMaskImage.shape()), negativeMaskImage.coordinates(),"tmpPreSmoNegMask.im");
              // temppresmoothnegmask.copyData(negativeMaskImage); 
              //PagedImage<Float> tempnegmask(TiledShape(thenegmask.shape()), thenegmask.coordinates(),"tmpNegMask.im");
              // tempnegmask.copyData(thenegmask);
              // PagedImage<Float> tempsmonegmask(TiledShape(thenegmask.shape()), thenegmask.coordinates(),"tmpSmoNegMask.im");
              // tempsmonegmask.copyData(*negmask);
              //}
              timeNegThresh(0) += timer.real(); timeNegThresh(1) += timer.user(); timeNegThresh(2) += timer.system(); 
              if (perplanetiming) {
                os << LogIO::NORMAL3 << "End thresholding: time to create the negative mask: real " 
                 << timer.real() <<"s (user " << timer.user() << "s, system " << timer.system() << "s)" << LogIO::POST;
              }
            }
             
            // store per plane masks to full cube mask images
            // subMask to mask, posmask, thenegmask
            Array<Float> maskdata, posmaskdata, negmaskdata;
            // all images are single planes
            IPosition stride(4,1,1,1,1);
            IPosition plstart(4,0,0,0,0); 
            Slicer plsl(plstart, length); 
            subMask.doGetSlice(maskdata, plsl); 
            subposmask.doGetSlice(posmaskdata, plsl); 
            subnegmask.getSlice(negmaskdata, plsl); 
            // put to full mask images 
            mask.putSlice(maskdata,start,stride);
            posmask.putSlice(posmaskdata,start,stride);
            thenegmask.putSlice(negmaskdata,start,stride);
            if (debug2) {
              PagedImage<Float> tempfinalmask(TiledShape(mask.shape()), mask.coordinates(), "tmpfinalmask"+String::toString(iterdone)+".im");
              tempfinalmask.copyData(mask);
            }
        } // if not chanFlag=True
      } // ipol iter
    } // the main per plane for-loop end  for-ich

    //print tot. timing for each step
    if (npol*nchan > 1) {
      os << LogIO::NORMAL << "*** Timing summary for whole planes ***" << LogIO::POST;
      os << LogIO::NORMAL << "Total time to create the initial threshold mask:  real "<< timeInitThresh(0) 
         << "s ( user " << timeInitThresh(1) <<"s, system "<< timeInitThresh(2) << "s)" << LogIO::POST;
      os << LogIO::NORMAL << "Total time to prune the initial threshold mask: real " 
	 << timePrune(0)<< "s (user " << timePrune(1) <<"s, system "<< timePrune(2) << "s)" << LogIO::POST;
      os << LogIO::NORMAL << "Total time to create the smoothed initial threshold mask: real "<< timeSmooth(0)
         <<"s (user " << timeSmooth(1)<<"s, system "<< timeSmooth(2) << "s)" <<  LogIO::POST;
      os << LogIO::NORMAL << "Total time to grow the previous mask: real " 
	 << timeGrow(0) <<"s (user "<< timeGrow(1) << "s, system " << timeGrow(2) << "s)" << LogIO::POST;
      os << LogIO::NORMAL << "Total time to prune the grow mask: real " 
	 << timePruneGrow(0) <<"s (user "<< timePruneGrow(1) << "s, system "<< timePruneGrow(2) << "s)" << LogIO::POST;
      os << LogIO::NORMAL << "Total time to create the smoothed grow mask: real " 
	 << timeSmoothGrow(0) <<"s (user "<< timeSmoothGrow(1) << "s, system " << timeSmoothGrow(2) << "s)" << LogIO::POST;
    }
    
    if (!iterdone) noMaskCheck(posmask, ThresholdType);
    // "Allpruned check here"
    Int nAllPruned=ntrue(allPruned);
    // pruning is done only on positive mask
    if(!iterdone && isEmptyMask(posmask) && nAllPruned) {
      os<<LogIO::WARN<<nAllPruned<<" of "<<nchan<<" channels had all regions removed by pruning."
        <<" Try decreasing minbeamfrac to remove fewer regions"<<LogIO::POST;
    }

    //for debug
    /***
    PagedImage<Float> tempthemask(TiledShape(tempIm_ptr.get()->shape()), tempIm_ptr.get()->coordinates(),"tempthemask.Im");
    tempthemask.copyData(themask);
    ***/

    // In the initial iteration, if no mask is created (all spectral planes) by automask it will fall back to full clean mask
    /***
    if (!iterdone) {
      Array<Float> maskdata; 
      IPosition maskshape = thenewmask.shape();
      Int naxis = maskshape.size();
      IPosition blc(naxis,0);
      IPosition trc=maskshape-1;
      Slicer sl(blc,trc,Slicer::endIsLast);
      thenewmask.doGetSlice(maskdata,sl);
      if (sum(maskdata)==0.0) {
         mask.set(1);
         //os<<LogIO::WARN<<"No mask was created by automask, set a clean mask to the entire image."<<LogIO::POST;
         os<<LogIO::WARN<<"No mask was created by automask."<<LogIO::POST;
      }
    }
    ***/
    if (debug) {
        //saved prev unmodified mask
        PagedImage<Float> tmpUntouchedPrevMask(res.shape(), res.coordinates(),"tmpUnmodPrevMask"+String::toString(iterdone)+".im");
        tmpUntouchedPrevMask.copyData(mask);

    }
    // make a copy of unmodified previous mask 
    TempImage<Float> unmodifiedprevmask(res.shape(),res.coordinates(), memoryToUse());
    unmodifiedprevmask.copyData(mask);
     
    if (res.hasPixelMask()) {
      LatticeExpr<Bool>  pixmask(res.pixelMask()); 
      //mask.copyData( (LatticeExpr<Float>)( iif((mask + thenewmask) > 0.0 && pixmask, 1.0, 0.0  ) ) );
      // add all masks (previous one, grow mask, current thresh mask)
      // mask = untouched prev mask, prevmask=modified prev mask by the grow func, thenewmask=mask by thresh on current residual 
      //mask.copyData( (LatticeExpr<Float>)( iif((mask+prevmask + thenewmask + thenegmask) > 0.0 && pixmask, 1.0, 0.0  ) ) );
      if(debug) {
         PagedImage<Float> savedUnmod(res.shape(), res.coordinates(), "savedUmmod"+String::toString(iterdone)+".im");
         savedUnmod.copyData(mask);
         PagedImage<Float> savedPosmask(res.shape(), res.coordinates(), "savedPosmask"+String::toString(iterdone)+".im");
         savedPosmask.copyData(posmask);
         PagedImage<Float> savedNegmask(res.shape(), res.coordinates(), "savedNegmask"+String::toString(iterdone)+".im");
         savedPosmask.copyData(thenegmask);
      }   
      mask.copyData( (LatticeExpr<Float>)( iif((mask + posmask + thenegmask ) > 0.0 && pixmask, 1.0, 0.0  ) ) );

      mask.clearCache();
      mask.unlock();
      mask.tempClose();
      os <<LogIO::DEBUG1 <<"Add previous mask, pbmask and the new mask.."<<LogIO::POST;
    }
    else {
      //os <<"Lattice themask is created..."<<LogIO::POST;
      //LatticeExpr<Float> themask( iif( tempconvim > rmsthresh/afactor, 1.0, 0.0 ));

      //mask.copyData( (LatticeExpr<Float>)( iif((mask + prevmask + thenewmask + thenegmask ) > 0.0, 1.0, 0.0  ) ) );
      mask.copyData( (LatticeExpr<Float>)( iif((mask + posmask + thenegmask ) > 0.0, 1.0, 0.0  ) ) );

      os <<LogIO::DEBUG1 <<"Add previous mask and the new mask.."<<LogIO::POST;
    }
    // test the curent final mask with the previous mask 
    Vector<Bool> zeroChanMask;
    skipChannels(fracChange,unmodifiedprevmask, mask, ThresholdType, isthresholdreached, chanFlag, zeroChanMask);

    if (verbose) 
      printAutomaskSummary(resRmss, maxs, mins, mdns,  maskThreshold, ThresholdType, chanFlag, zeroChanMask, nreg, npruned, ngrowreg, ngrowpruned, negmaskpixs, summaryRec);
  }//end of autoMaskByMultiThreshold

  Bool SDMaskHandler::isEmptyMask(ImageInterface<Float>& mask) 
  {
      Array<Float> maskdata;
      IPosition maskshape = mask.shape();
      Int naxis = maskshape.size();
      IPosition blc(naxis,0);
      IPosition trc=maskshape-1;
      Slicer sl(blc,trc,Slicer::endIsLast);
      mask.doGetSlice(maskdata,sl);
      return (sum(maskdata)==0.0);
      
  }
 
  //void SDMaskHandler::noMaskCheck(ImageInterface<Float>& mask, Vector<String>& thresholdType)
  void SDMaskHandler::noMaskCheck(ImageInterface<Float>& mask, Matrix<String>& thresholdType)
  {
      // checkType and thresholdType will determine the exact messages to print out in the log
      LogIO os( LogOrigin("SDMaskHandler","autoMaskByMultiThreshold",WHERE) );
      // for waring messsages
      /***
      Array<Float> maskdata;
      IPosition maskshape = mask.shape();
      Int naxis = maskshape.size();
      IPosition blc(naxis,0);
      IPosition trc=maskshape-1;
      Slicer sl(blc,trc,Slicer::endIsLast);
      mask.doGetSlice(maskdata,sl);
      ***/
      //if (sum(maskdata)==0.0) {
      if (isEmptyMask(mask)) {
         os << LogIO::WARN <<"No regions found by automasking" <<LogIO::POST;
         // checktype
         //Int nThresholdType = thresholdType.nelements();
         Int nrow = thresholdType.nrow();
         Int ncol = thresholdType.ncolumn();
         //Int nThresholdType = thresholdType.nelements();
         Int nThresholdType = nrow*ncol; 
         Int nsidelobethresh=0;
         Int nnoisethresh=0;
         if (nThresholdType>1 ) {
            //for (uInt j=0; j<(uInt) nThresholdType; j++) {
            for (uInt j=0; j<(uInt) ncol; j++) {
             for (uInt i=0; i<(uInt) nrow; i++) {
                if (thresholdType(i,j)=="sidelobe") {
                   nsidelobethresh++;
                }
                if (thresholdType(i,j)=="noise") {
                   nnoisethresh++;
                }
              }
            }
            if (nsidelobethresh) {
                os << LogIO::WARN <<nsidelobethresh<<" of "<<nThresholdType
                   <<" channels used the sidelobe threshold to mask, but no emission was found."
                   <<" Try decreasing your sidelobethreshold parameter if you want to capture emission in these channels."<< LogIO::POST;
            }
            if (nnoisethresh) {
               os << LogIO::WARN <<nnoisethresh<<" of "<<nThresholdType<<" channels used the noise threshold to mask, but no emission was found."
                  << " Try decreasing your noisethreshold parameter if you want to capture emission in these channels."<< LogIO::POST;
            }
         }
         else {

            os << LogIO::WARN << "Used "<<thresholdType(0,0)<<" threshold to mask, but no emission was found."
               << "Try decreasing your "<<thresholdType(0,0)<<"threshold parameter if you want to capture emission in these channels."<< LogIO::POST;
         }
      }
  }

  void SDMaskHandler::skipChannels(const Float& fracChange, 
                                  ImageInterface<Float>& prevmask, 
                                  ImageInterface<Float>& curmask, 
                                  //const Vector<String>& thresholdtype,
                                  const Matrix<String>& thresholdtype,
                                  const Bool isthresholdreached,
                                  Vector<Bool>& chanFlag,
                                  Vector<Bool>& zeroChanMask)
  {
    LogIO os( LogOrigin("SDMaskHandler","skipChannels",WHERE) );
    // debug
    os<<LogIO::DEBUG1<<"Inside skipChannels...."<<LogIO::POST;
    IPosition shp = curmask.shape();
    Int naxis = shp.size();
    CoordinateSystem csys = curmask.coordinates();
    Int specaxis = CoordinateUtil::findSpectralAxis(csys); 
    Int nchan = shp(specaxis);
    // Assumption here is skipChannels applied to stokes I only to keep track which channels 
    // to skip for new automasking
    IPosition blc(naxis,0);
    IPosition trc=shp-1;
    zeroChanMask.resize(nchan);
    os<<LogIO::DEBUG1<<"Inside skipChannels....after zeroChanMask init"<<LogIO::POST;
    for (Int ichan=0; ichan<nchan; ichan++) {
      blc(specaxis)=ichan;
      trc(specaxis)=ichan;
      Slicer sl(blc,trc,Slicer::endIsLast);
      Array<Float> curmaskdata;
      curmask.doGetSlice(curmaskdata,sl);
      Float curmaskpix = sum(curmaskdata);
      // sepearately store zero channel mask info maybe combined in future to streamline
      if (curmaskpix==0) {
         zeroChanMask(ichan) = True; 
      }
      else {
         zeroChanMask(ichan) = False;
      }

      //if (thresholdtype(ichan).contains("noise") && isthresholdreached && !chanFlag(ichan)) {
      if (thresholdtype(0, ichan).contains("noise") && !chanFlag(ichan)) {
        Array<Float> prevmaskdata;
        prevmask.doGetSlice(prevmaskdata,sl);
        Float prevmaskpix = sum(prevmaskdata);
	//cerr<<"prevmaskpix="<<prevmaskpix<<" curemaskpix="<<curmaskpix<<endl;
	//cerr<<"fracChnage="<<fracChange<<endl;
	Float diffpix = abs(curmaskpix-prevmaskpix);
        // stopmask is true if one of the followings is satified
        // 1) if current mask is zero (curmaskpix==0.0)
        // 2) if cyclethreshold==threshold (i.e. isthresholdreached=True) and diffpix is zero or 
        //    less than user-specified fractinal change
	//if ( curmaskpix==0.0 || (diffpix == 0.0 && prevmaskpix!=0.0) || diffpix < fracChange*prevmaskpix) {
	//if ( curmaskpix==0.0 || (isthresholdreached && ((diffpix == 0.0 && prevmaskpix!=0.0) || diffpix < fracChange*prevmaskpix)) ) {
	if ( curmaskpix==0.0 || 
             (fracChange >=0.0 && isthresholdreached && ( diffpix == 0.0 || diffpix < fracChange*prevmaskpix) ) ) {
	  chanFlag(ichan) = True;
	  os<<LogIO::NORMAL<<"Stopping masking for chan="<<ichan<<LogIO::POST;
	}       
      }
    } // for loop end
  }

  std::shared_ptr<ImageInterface<Float> >  SDMaskHandler::makeMaskFromBinnedImage(const ImageInterface<Float>& image, 
                                                                             const Int nx, 
                                                                             const Int ny,  
                                                                             const Float& fracofpeak,
                                                                             const Float& sigma, 
                                                                             const Int nmask,
                                                                             const Bool autoadjust,
                                                                             Double thresh)
  {
    Bool debug(False);
    Bool firstrun(False);
    LogIO os( LogOrigin("SDMaskHandler","makeMaskfromBinnedImage",WHERE) );
    RebinImage<Float> tempRebinnedIm( image, IPosition(4,nx, ny,1,1) );
    // for debug
    if (debug) {
      PagedImage<Float> copyRebinnedIm(TiledShape(tempRebinnedIm.shape()), tempRebinnedIm.coordinates(), "binned.Im");
      copyRebinnedIm.copyData(tempRebinnedIm);
    }

    // modified threshold
    // original algortihm
    //thresh = 3.0*thresh / sqrt(npix);
    // modified by bin size only
    //thresh = thresh / sqrt(nx);

    //stats on the binned image (the info not used for mask criteria yet)
    Array<Double> rmses, maxes, mins;
    // vars to store min,max values of extrema and rms in all planes
    Double minRmsVal, maxRmsVal, minMaxVal, maxMaxVal, minMinVal, maxMinVal;
    IPosition minRmsPos, maxRmsPos, minMaxPos, maxMaxPos, minMinPos, maxMinPos;
    TempImage<Float>* tempImForStat = new TempImage<Float>(tempRebinnedIm.shape(), tempRebinnedIm.coordinates(), memoryToUse() );
    tempImForStat->copyData(tempRebinnedIm);
    std::shared_ptr<casacore::ImageInterface<float> > temprebin_ptr(tempImForStat);
    //os<<" temprebin_ptr.get()->hasPixelMask()="<<temprebin_ptr.get()->hasPixelMask()<<LogIO::POST;
    ImageStatsCalculator<Float> imcalc( temprebin_ptr, 0, "", False);
    Vector<Int> stataxes(2);
    stataxes[0] = 0;
    stataxes[1] = 1;
    imcalc.setAxes(stataxes);
    Record imstats = imcalc.statistics();
    imstats.get(RecordFieldId("rms"),rmses);
    imstats.get(RecordFieldId("max"),maxes);
    imstats.get(RecordFieldId("min"),mins);
    minMax(minRmsVal,maxRmsVal,minRmsPos, maxRmsPos,rmses);
    minMax(minMaxVal,maxMaxVal,minMaxPos, maxMaxPos,maxes);
    minMax(minMinVal,maxMinVal,minMinPos, maxMinPos,mins);
    

    //os << LogIO::NORMAL <<"Statistics on binned image: max="<<maxMaxVal<<" rms="<<maxRmsVal<<LogIO::POST;
    os << LogIO::NORMAL <<"Statistics on binned image: Peak (max)="<<maxMaxVal<<"@"<<maxMaxPos
                        <<"rms (max) ="<<maxRmsVal<<"@"<<maxRmsPos<<LogIO::POST;
    //os << LogIO::NORMAL <<"Statistics on binned image: min of Max="<<minMaxVal<<"@"<<minMaxPos<<LogIO::POST;
    //os << LogIO::NORMAL <<"Statistics on binned image: min of rms="<<minRmsVal<<"@"<<minRmsPos<<LogIO::POST;

    TempImage<Float>* tempMask = new TempImage<Float> (tempRebinnedIm.shape(), tempRebinnedIm.coordinates(), memoryToUse() );


    //Double dr =  maxMaxVal/rmses(maxMaxPos);
    
    // save only the first time
    if (itsMax==DBL_MAX) { 
        itsMax = maxMaxVal;
        firstrun = True;
    }
    else {
        firstrun = False;
    }

    Float adjFact = 0.0;

    //Float fracDiffMax = (itsMax - maxMaxVal)/itsMax;
    Float fracDiffRms = (itsRms - maxRmsVal)/itsRms;
    //os<<"fractional changes in max:"<<fracDiffMax<<" in rms:"<<fracDiffRms<<LogIO::POST;
    os<<LogIO::DEBUG1<<"fractional changes in rms from previous one:"<<fracDiffRms<<LogIO::POST;
    if (autoadjust) {
      //os <<"autoAdjust is on "<<LogIO::POST;
      // automatically adjust threshold for the initial round and when fractional rms change is
      // is less than 10%
      if (fracDiffRms < 0.1 ) {
        adjFact = (Float) Int(log(1.0/abs(fracDiffRms))-1.0);
      }
     // else if (dr < 10.0) {
     //   os<<LogIO::DEBUG1<<"dynamic range:max/rms = "<<dr<<LogIO::POST;
     //   adjFact = sigma!=0 && sigma <= 3.0? 2: 0;
     // }
      else if (firstrun) {
        adjFact = 3;
      }
    }
    if (fracofpeak) {
      thresh = fracofpeak * maxMaxVal;
      Double prevthresh = thresh;
      if (adjFact >0.0 ) {
        thresh = max((adjFact + 3) * maxRmsVal,thresh);
        if (firstrun) {
          // adjustment at 1st iteration cycle, if threshold is too big, make cutoff at 50% peak 
          if (thresh < itsMax) {
            if (prevthresh != thresh) {
              os << LogIO::NORMAL <<"first iteration automatically adjusted thresh="<<thresh<<"( "<<" * (adj fact.="<<adjFact+3<<") * rms )"<<LogIO::POST;
            }
          }
          else {
            thresh=prevthresh;
          }
        } 
        else {
          if (prevthresh != thresh) {
            os << LogIO::NORMAL <<"thresh="<<thresh<<" ( adj fact.="<<adjFact+3<<") * rms )"<<LogIO::POST;
          }
        }
      }
      // if sidelobe level is set and if it is larger than the current thresh use that value
      //thresh = max( itsSidelobeLevel*itsMax, thresh );
      if (adjFact != 0.0) {
      }
      else {
        os << LogIO::NORMAL <<"thresh="<<thresh<<" ( "<<fracofpeak<<"* max peak )"<<LogIO::POST;
      }
    } 
    else if (sigma) {
      thresh = (sigma + adjFact)* maxRmsVal;
      // if sidelobe level is set and if it is larger than the current thresh use that value
      //thresh = max( itsSidelobeLevel*itsMax, thresh);
      if (firstrun && adjFact != 0.0) {
        if (thresh < itsMax) { 
          os << LogIO::NORMAL <<"first iteration automatically adjusted thresh="<<thresh
             <<" ( "<<sigma<<"+adjustment factor:"<<adjFact<<")* rms )"<<LogIO::POST;
        }
        else {
          thresh = 0.5*itsMax;
          os << LogIO::NORMAL <<"first iteration automatically adjusted thresh="<<thresh
             <<" (0.5*peak )"<<LogIO::POST;
        }
      } 
      if (adjFact != 0.0) {
        os << LogIO::NORMAL <<"thresh="<<thresh<<" ( "<<sigma<<"+adjustment factor:"<<adjFact<<")* rms )"<<LogIO::POST;
      }
      else {
        os << LogIO::NORMAL <<"thresh="<<thresh<<" ( "<<sigma<<"* rms )"<<LogIO::POST;
      }
    }


    itsRms = maxRmsVal;

    if (thresh > maxMaxVal) {
      os << LogIO::WARN <<" The threshold value,"<<thresh<<" for making a mask is greater than max value in the image. No new mask will be added by automask."<< LogIO::POST;
      tempMask->set(0.0);
    }
    else {
    
      // apply threshold to rebinned image to generate a temp image mask
      // first run pruning by limiting n masks (npix=1 as it is already binned)
      std::shared_ptr<ImageInterface<Float> > dummyim = pruneRegions(tempRebinnedIm, thresh, nmask, 1);

      os << LogIO::DEBUG1<<" threshold applied ="<<thresh<<LogIO::POST;
      //cerr<<"dummyim shape="<<dummyim.get()->shape()<<endl;
      //cerr<<"temprebinned shape="<<tempRebinnedIm.shape()<<endl;
      //
      //LatticeExpr<Float> tempthresh( iif( abs(tempRebinnedIm) > thresh, 1.0, 0.0) );
      LatticeExpr<Float> tempthresh( iif( abs( *(dummyim.get()) ) > thresh, 1.0, 0.0) );
      //os << LogIO::DEBUG1<<" copying the threshold image....."<<LogIO::POST;
      tempMask->copyData(tempthresh);
    }
    return std::shared_ptr<ImageInterface<Float> >(tempMask);
  }

  std::shared_ptr<ImageInterface<Float> > SDMaskHandler::convolveMask(const ImageInterface<Float>& inmask, const GaussianBeam& beam)
  {
    LogIO os( LogOrigin("SDMaskHandler","convolveMask",WHERE) );
    TempImage<Float>* tempIm = new TempImage<Float>(inmask.shape(), inmask.coordinates(), memoryToUse() );
    tempIm->copyData(inmask);
    std::shared_ptr<casacore::ImageInterface<float> > tempIm2_ptr(tempIm);
    //DEBUG will be removed 

    Vector<Quantity> convbeam(3);
    convbeam[0] = beam.getMajor();
    convbeam[1] = beam.getMinor();
    convbeam[2] = beam.getPA();
    os << LogIO::DEBUG1<<"convolve with a gaussian: bmaj="<<convbeam[0]<<"bmin="<<convbeam[1]<<" pa="<<convbeam[2]<<LogIO::POST;
    Record dammyRec=Record();
    //String convimname("temp_convim");
    Image2DConvolver<Float> convolver(tempIm2_ptr, &dammyRec, String(""), String(""), True);
    convolver.setKernel("GAUSSIAN", convbeam[0], convbeam[1], convbeam[2]);
    convolver.setAxes(std::make_pair(0,1));
    convolver.setScale(Double(-1.0));
    convolver.setSuppressWarnings(True);
    auto outmask = convolver.convolve();
    return outmask;
  } 

  std::shared_ptr<ImageInterface<Float> > SDMaskHandler::convolveMask(const ImageInterface<Float>& inmask, Int nxpix, Int nypix)
  {
    LogIO os( LogOrigin("SDMaskHandler","convolveMask",WHERE) );
    TempImage<Float>* tempIm = new TempImage<Float>(inmask.shape(), inmask.coordinates(), memoryToUse() );
    tempIm->copyData(inmask);
    std::shared_ptr<casacore::ImageInterface<float> > tempIm2_ptr(tempIm);
    //DEBUG will be removed 
    os << LogIO::DEBUG1<<"convolve with "<<nxpix<<" pix x "<<nypix<<" pix gaussian"<<LogIO::POST;

    Vector<Quantity> convbeam(3);
    convbeam[0] = Quantity(nxpix, "pix");
    convbeam[1] = Quantity(nypix, "pix");
    convbeam[2] = Quantity(0.0, "deg");
    Record dammyRec=Record();
    //String convimname("temp_convim");
    Image2DConvolver<Float> convolver(tempIm2_ptr, &dammyRec, String(""), String(""), True);
    convolver.setKernel("GAUSSIAN", convbeam[0], convbeam[1], convbeam[2]);
    convolver.setAxes(std::make_pair(0,1));
    convolver.setScale(Double(-1.0));
    convolver.setSuppressWarnings(True);
    auto outmask = convolver.convolve();
    return outmask;
  } 

  std::shared_ptr<casacore::ImageInterface<Float> >  SDMaskHandler::pruneRegions(const ImageInterface<Float>& image, Double& thresh, Int nmask, Int npix)
  {
    LogIO os( LogOrigin("SDMaskHandler", "pruneRegions",WHERE) );
    Bool debug(False);

    IPosition fullimShape=image.shape();
    TempImage<Float>* fullIm = new TempImage<Float>(TiledShape(fullimShape, image.niceCursorShape()), image.coordinates(), memoryToUse());

    if (nmask==0 && npix==0 ) {
      //No-op
      os<<LogIO::DEBUG1<<"Skip pruning of mask regions"<<LogIO::POST;
      fullIm->copyData(image);
      return std::shared_ptr<ImageInterface<Float> >(fullIm);
    }  
    os <<LogIO::DEBUG1<< "pruneRegions with nmask="<<nmask<<", size="<<npix<<" is applied"<<LogIO::POST;
    
    IPosition shp = image.shape();
    //cerr<<"shp = "<<shp<<endl;
    IPosition blc(4,0);
    IPosition trc = shp-1; 
    Slicer sl(blc,trc,Slicer::endIsLast);
    AxesSpecifier aspec(False);
    // decomposer can only work for 2 and 3-dim images so need
    // some checks the case for stokes axis > 1
    // following works if stokes axis dim = 1
    SubImage<Float>* subIm = new SubImage<Float>(image, sl, aspec, True); 
    RegionManager regMan;
    CoordinateSystem cSys=subIm->coordinates(); 
    regMan.setcoordsys(cSys);
    //String strReg = "box[["+String::toString(blc[0])+"pix,"+String::toString(blc[1])+"pix], ["+String::toString(shp[0])+"pix,"+String::toString(shp[1])+"pix]]";
    //cerr<<"strReg="<<strReg<<endl;
    //RegionTextList CRTFList(cSys, strReg, shp);
    //Record regRec = CRTFList.regionAsRecord();
    //std::shared_ptr<casacore::SubImage<Float> > subIm = SubImageFactory<Float>::createSubImageRW(image, regRec, "", &os, aspec, False, False);

    //cerr <<" subIm.shape="<<subIm->shape()<<endl;
    IPosition subimShape=subIm->shape();
    uInt ndim = subimShape.nelements();
    //std::shared_ptr<casacore::ImageInterface<float> > tempIm_ptr(subIm);
    TempImage<Float>* tempIm = new TempImage<Float> (TiledShape(subIm->shape(), subIm->niceCursorShape()), subIm->coordinates(), memoryToUse() );
    // to search for both positive and negative components
    tempIm->copyData(LatticeExpr<Float> (abs(*subIm)));
    os << LogIO::NORMAL2 <<"Finding regions by ImageDecomposer..."<<LogIO::POST;
    //use ImageDecomposer
    Matrix<Quantity> blctrcs;
    ImageDecomposer<Float> id(*tempIm);
    id.setDeblend(True);
    os << LogIO::DEBUG1<< "Deblend threshold="<<thresh<<LogIO::POST;
    id.setDeblendOptions(thresh, 3, 1, 2); //nContour=3
    //id.setDeblendOptions(thresh, 3, 1, 1); //nContour=3, naxis=1
    id.setFit(False);
    os << LogIO::DEBUG1<<"Now calling decomposeImage()..."<<LogIO::POST;
    id.decomposeImage();
    if (debug) 
      id.printComponents();
    uInt nRegion = id.numRegions();
    os << "Found " << nRegion <<" regions"<<LogIO::POST;
    Block<IPosition> blcs(nRegion);
    Block<IPosition> trcs(nRegion);
    id.boundRegions(blcs, trcs);
    //os << "Get comp list.."<<LogIO::POST;
    Matrix<Float> clmat=id.componentList();
    //os << "Get peaks.."<<LogIO::POST;
    Vector<Float> peaks = clmat.column(0);
    //cerr<<"peaks="<<peaks<<endl;
    os << LogIO::DEBUG1<< "Sorting by peak fluxes..."<<LogIO::POST;
    // sort 
    Vector<uInt> sortedindx;
    Sort sorter;
    //cerr<<"Setup sortKey..."<<endl;
    sorter.sortKey(peaks.data(),TpFloat,0, Sort::Descending);
    //cerr<<"do sort..."<<endl;
    sorter.sort(sortedindx, peaks.nelements());
    //os << "Sorting py peak flux DONE..."<<LogIO::POST;
    os<< LogIO::DEBUG1<<"sortedindx="<<sortedindx<<LogIO::POST;    
    // FOR DEBUGGING
    for (uInt j = 0; j < blcs.nelements(); j++) {
      os<<" j="<<j<<" blcs="<<blcs[j]<<" trcs="<<trcs[j]<<LogIO::POST;
    }
    Vector<Int> absRel(ndim, RegionType::Abs);
    PtrBlock<const WCRegion *> wbox;
    uInt iSubComp=0;
    uInt removeByNMask=0;
    uInt removeBySize=0;
    for (uInt icomp=0; icomp < sortedindx.nelements(); icomp++) {
      Bool removeit(False);
      Vector<Quantum<Double> > qblc(ndim);
      Vector<Quantum<Double> > qtrc(ndim);      
      Vector<Double> wblc(ndim);
      Vector<Double> wtrc(ndim);
      Vector<Double> pblc(ndim);
      Vector<Double> ptrc(ndim);
      // pixel blcs and trcs
      for (uInt i=0; i < ndim; i++) {
        pblc[i] = (Double) blcs[sortedindx[icomp]][i]; 
        ptrc[i] = (Double) trcs[sortedindx[icomp]][i];
      }
      // get blcs and trcs in world coord.
      cSys.toWorld(wblc,pblc);
      cSys.toWorld(wtrc,ptrc);
      for (uInt i=0; i < ndim; i++) {
        qblc[i] = Quantum<Double>(wblc[i], cSys.worldAxisUnits()(cSys.pixelAxisToWorldAxis(i)));
        qtrc[i] = Quantum<Double>(wtrc[i], cSys.worldAxisUnits()(cSys.pixelAxisToWorldAxis(i)));
      }

      if (npix > 0) {
        //npix = area 
        //os<<"pruning regions by size < "<<npix<<LogIO::POST;
        Int xboxdim = ptrc[0] - pblc[0];
        Int yboxdim = ptrc[1] - pblc[1];
        //if (( xboxdim < npix || yboxdim < npix ) && xboxdim*yboxdim < npix*npix ) {
        if ( xboxdim*yboxdim < npix ) {
          removeit = True;
          removeBySize++;
        }
      }
      if (nmask > 0 && icomp >= (uInt)nmask ) {
        removeit=True; 
        removeByNMask++;
      }
      
      if (removeit) {
        wbox.resize(iSubComp+1);
        wbox[iSubComp]= new WCBox (qblc, qtrc, cSys, absRel);
        iSubComp++;
        os << LogIO::DEBUG1<<"*** Removed region: "<<icomp<<" pblc="<<pblc<<" ptrc="<<ptrc<<LogIO::POST;
      }
      else {
        os << LogIO::DEBUG1<<"Saved region: "<<icomp<<" pblc="<<pblc<<" ptrc="<<ptrc<<LogIO::POST;
      }
    } // for icomp  

    //cerr<<"iSubComp="<<iSubComp<<endl;
    //cerr<<"wbox.nelements="<<wbox.nelements()<<endl;
    if (iSubComp>0) {
      ImageRegion* boxImageRegion=regMan.doUnion(wbox);
      //cerr<<"regionToMask ..."<<endl;
      tempIm->copyData(*subIm);
      regionToMask(*tempIm,*boxImageRegion, Float(0.0)); 
      //cerr<<"Done regionToMask..."<<endl;
      os <<"pruneRegions removed "<<iSubComp<<" regions from the mask image"<<LogIO::POST;
      os <<"  (reasons: "<< removeBySize<<" by size "<<", "<<removeByNMask<<" by nmask)" <<LogIO::POST;
      for (uInt k=0; k < wbox.nelements(); k++) {
        delete wbox[k];
      }
    }
    else {
      os <<"No regions are removed by pruning" << LogIO::POST;
    }
    // Debug
    if (debug) {
      PagedImage<Float> debugPrunedIm(tempIm->shape(),tempIm->coordinates(),"prunedSub.Im");
      debugPrunedIm.copyData(*tempIm);
    }
    //
    // Inserting pruned result image to the input image
    Array<Float> subimData;
    //IPosition fullimShape=image.shape();
    //TempImage<Float>* fullIm = new TempImage<Float>(TiledShape(fullimShape, image.niceCursorShape()), image.coordinates());
    fullIm->set(0);
    IPosition start(fullimShape.nelements(),0);
    IPosition stride(fullimShape.nelements(),1);
    if (ndim ==3) {
      IPosition substart(3,0);
      IPosition subshape(3,subimShape(0),subimShape(1),1);
      IPosition substride(3,1,1,1);
      uInt nchan=subimShape(2);
      //cerr<<"shape tempIm ="<<tempIm->shape()<<endl;
      //cerr<<"shape fullIm ="<<fullIm->shape()<<endl;
      for (uInt ich=0; ich < nchan; ich++) {
        substart(2) = ich;
        //tempIm->getSlice(subimData,Slicer(substart,subend),True);
        tempIm->getSlice(subimData,substart,subshape,substride,True);
        start(3) = ich;
        fullIm->putSlice(subimData,start,stride);  
      }
    }
    else if (ndim==2) {
      subimData = tempIm->get();
      //cerr<<"subimData shape="<<subimData.shape()<<endl;
      //cerr<<"shape tempIm ="<<tempIm->shape()<<endl;
      fullIm->putSlice(subimData,start,stride);
      //cerr<<"shape fullIm ="<<fullIm->shape()<<endl;
    }
    delete subIm; subIm=0;
    delete tempIm; tempIm=0;
    return std::shared_ptr<ImageInterface<Float> >(fullIm);
  }
 
   
  std::shared_ptr<casacore::ImageInterface<Float> >  SDMaskHandler::pruneRegions2(const ImageInterface<Float>& image, Double& thresh, Int nmask, Double prunesize)
  {
    LogIO os( LogOrigin("SDMaskHandler", "pruneRegions2",WHERE) );
    Bool debug(False);

    IPosition fullimShape=image.shape();
    TempImage<Float>* fullIm = new TempImage<Float>(TiledShape(fullimShape, image.niceCursorShape()), image.coordinates(), memoryToUse());
    fullIm->set(0);

    if (nmask==0 && prunesize==0.0 ) {
      //No-op
      os<<LogIO::DEBUG1<<"Skip pruning of mask regions"<<LogIO::POST;
      fullIm->copyData(image);
      return std::shared_ptr<ImageInterface<Float> >(fullIm);
    }
    os <<LogIO::NORMAL<< "pruneRegions with nmask="<<nmask<<", size="<<prunesize<<" is applied"<<LogIO::POST;

    IPosition shp = image.shape();
    IPosition trc = shp-1;
    Int specaxis = CoordinateUtil::findSpectralAxis(image.coordinates());
    uInt nchan = shp(specaxis); 
    // do a single channel plane at time
    for (uInt ich = 0; ich < nchan; ich++) { 
      IPosition start(4, 0, 0, 0,ich);
      IPosition length(4, shp(0),shp(1),shp(2),1);
      Slicer sl(start, length);
      //cerr<<"slicer sl ="<<sl<<endl;
      AxesSpecifier aspec(False);
      // decomposer can only work for 2 and 3-dim images so need
      // some checks the case for stokes axis > 1
      // following works if stokes axis dim = 1
      SubImage<Float>* subIm = new SubImage<Float>(image, sl, aspec, True);
      RegionManager regMan;
      CoordinateSystem cSys=subIm->coordinates();
      regMan.setcoordsys(cSys);
      IPosition subimShape=subIm->shape();
      uInt ndim = subimShape.nelements();
      TempImage<Float>* tempIm = new TempImage<Float> (TiledShape(subIm->shape(), subIm->niceCursorShape()), subIm->coordinates(), memoryToUse() );
      // to search for both positive and negative components
      tempIm->copyData(LatticeExpr<Float> (abs(*subIm)));
      //use ImageDecomposer
      Matrix<Quantity> blctrcs;
      ImageDecomposer<Float> id(*tempIm);
      id.setDeblend(True);
      os << LogIO::DEBUG1<< "Deblend threshold="<<thresh<<LogIO::POST;
      id.setDeblendOptions(thresh, 3, 1, 2); //nContour=3
      id.setFit(False);
      os << LogIO::DEBUG1<<"Now calling decomposeImage()..."<<LogIO::POST;
      id.decomposeImage();
      if (debug)
        id.printComponents();
      uInt nRegion = id.numRegions();
      os << "Found " << nRegion <<" regions for channel plane "<<ich<<LogIO::POST;
      if (nRegion) {
      Block<IPosition> blcs(nRegion);
      Block<IPosition> trcs(nRegion);
      id.boundRegions(blcs, trcs);
      //os << "Get comp list.."<<LogIO::POST;
      Matrix<Float> clmat=id.componentList();
      //os << "Get peaks.."<<LogIO::POST;
      Vector<Float> peaks = clmat.column(0);
      //cerr<<"peaks="<<peaks<<endl;
      os << LogIO::DEBUG1<< "Sorting by peak fluxes..."<<LogIO::POST;
      // sort
      Vector<uInt> sortedindx;
      Sort sorter;
      sorter.sortKey(peaks.data(),TpFloat,0, Sort::Descending);
      sorter.sort(sortedindx, peaks.nelements());
      //os << "Sorting py peak flux DONE..."<<LogIO::POST;
      os<< LogIO::DEBUG1<<"sortedindx="<<sortedindx<<LogIO::POST;
      // FOR DEBUGGING
      if (debug) {
        for (uInt j = 0; j < blcs.nelements(); j++) {
          os<<" j="<<j<<" blcs="<<blcs[j]<<" trcs="<<trcs[j]<<LogIO::POST;
        }
      }
      Vector<Int> absRel(ndim, RegionType::Abs);
      PtrBlock<const WCRegion *> wbox;
      uInt iSubComp=0;
      uInt removeByNMask=0;
      uInt removeBySize=0;
      for (uInt icomp=0; icomp < sortedindx.nelements(); icomp++) {
        Bool removeit(False);
        if (debug) {
          cerr<<"sortedindx="<<sortedindx[icomp]<<" comp="<<clmat.row(sortedindx[icomp])<<endl;
        }
        Vector<Quantum<Double> > qblc(ndim);
        Vector<Quantum<Double> > qtrc(ndim);
        Vector<Double> wblc(ndim);
        Vector<Double> wtrc(ndim);
        Vector<Double> pblc(ndim);
        Vector<Double> ptrc(ndim);
        // pixel blcs and trcs
        for (uInt i=0; i < ndim; i++) {
          pblc[i] = (Double) blcs[sortedindx[icomp]][i];
          ptrc[i] = (Double) trcs[sortedindx[icomp]][i];
        }
        // get blcs and trcs in world coord.
        cSys.toWorld(wblc,pblc);
        cSys.toWorld(wtrc,ptrc);
        if (debug) {
          cerr<<"cSys.workdAxisUnits=="<<cSys.worldAxisUnits()<<endl;
          cerr<<"cSys increment = "<<cSys.increment()<<endl;
        }
        for (uInt i=0; i < ndim; i++) {
          qblc[i] = Quantum<Double>(wblc[i], cSys.worldAxisUnits()(cSys.pixelAxisToWorldAxis(i)));
          qtrc[i] = Quantum<Double>(wtrc[i], cSys.worldAxisUnits()(cSys.pixelAxisToWorldAxis(i)));
        }
        if (prunesize > 0.0) {
          //npix = area
          Int xboxdim = ptrc[0] - pblc[0] +1;
          Int yboxdim = ptrc[1] - pblc[1] +1;
          // get size from component : these seem to be in deg
          Double ax1 = clmat.row(sortedindx[icomp])[3];
          Double ax2 = ax1*clmat.row(sortedindx[icomp])[4];
          Quantity qax1(ax1,"deg");
          Quantity qax2(ax2,"deg");
          Vector<Double> incVal = cSys.increment();
          Vector<String> incUnit = cSys.worldAxisUnits();
          Quantity qinc1(incVal[0],incUnit[0]);
          Quantity qinc2(incVal[1],incUnit[1]);
          Double pixAreaInRad = abs(qinc1.get(Unit("rad")).getValue() * qinc2.get(Unit("rad")).getValue());
          Double regionInSR = C::pi * qax1.get(Unit("rad")).getValue()  * qax2.get(Unit("rad")).getValue() / (4. * C::ln2);
          Double regpix = regionInSR/pixAreaInRad;
          //Double axpix1 = ceil(abs(qax1/(qinc1.convert(qax1),qinc1)).get().getValue()); 
          //Double axpix2 = ceil(abs(qax2/(qinc2.convert(qax2),qinc2)).get().getValue()); 
          //Int regpix = Int(C::pi * axpix1 * axpix2 /(4. * C::ln2)); 
          if (debug) {
            cerr<<"regpix="<<regpix<<" prunesize="<<prunesize<<" xboxdim="<<xboxdim<<" yboxdim="<<yboxdim<<endl;
          }
          // regpix seems to be a bit too small ..., try pruning by blc, trc ...
          if ( xboxdim*yboxdim < Int(ceil(prunesize)) ) {
          //if ( regpix < prunesize ) {
            removeit = True;
            removeBySize++;
          }
        }
        if (nmask > 0 && icomp >= (uInt)nmask ) {
          removeit=True;
          removeByNMask++;
        }

        if (removeit) {
          wbox.resize(iSubComp+1);
          wbox[iSubComp]= new WCBox (qblc, qtrc, cSys, absRel);
          iSubComp++;
          os << LogIO::DEBUG1<<"*** Removed region: "<<icomp<<" pblc="<<pblc<<" ptrc="<<ptrc<<LogIO::POST;
        }
        else {
          os << LogIO::DEBUG1<<"Saved region: "<<icomp<<" pblc="<<pblc<<" ptrc="<<ptrc<<LogIO::POST;
        }
      } // for icomp

      //cerr<<"iSubComp="<<iSubComp<<endl;
      //cerr<<"wbox.nelements="<<wbox.nelements()<<endl;
      if (iSubComp>0) {
        ImageRegion* boxImageRegion=regMan.doUnion(wbox);
        //cerr<<"regionToMask ..."<<endl;
        tempIm->copyData(*subIm);
        regionToMask(*tempIm,*boxImageRegion, Float(0.0));
        //cerr<<"Done regionToMask..."<<endl;
        os <<"pruneRegions removed "<<iSubComp<<" regions from the mask image"<<LogIO::POST;
        os <<"  (reasons: "<< removeBySize<<" by size "<<", "<<removeByNMask<<" by nmask)" <<LogIO::POST;
        for (uInt k=0; k < wbox.nelements(); k++) {
          delete wbox[k];
        }
      }
      else {
        os <<"No regions are removed by pruning" << LogIO::POST;
      }
      // Debug
      if (debug) {
        PagedImage<Float> debugPrunedIm(tempIm->shape(),tempIm->coordinates(),"tmp-prunedSub.im");
        debugPrunedIm.copyData(*tempIm);
      }
      //
      // Inserting pruned result image to the input image
      Array<Float> subimData;
      //IPosition fullimShape=image.shape();
      //TempImage<Float>* fullIm = new TempImage<Float>(TiledShape(fullimShape, image.niceCursorShape()), image.coordinates());

      //IPosition start(fullimShape.nelements(),0);
      //IPosition stride(fullimShape.nelements(),1);
      //IPosition substart(3,0);
      //IPosition subshape(3,subimShape(0),subimShape(1),1);
      //IPosition substride(3,1,1,1);
      //tempIm->getSlice(subimData,Slicer(substart,subend),True);
      tempIm->getSlice(subimData,IPosition(2,0), tempIm->shape(), IPosition(2,1,1));
      fullIm->putSlice(subimData,start,IPosition(4,1,1,1,1));
      delete tempIm; tempIm=0;
      delete subIm; subIm=0;
      }// if(nRegion) end 
    }
    return std::shared_ptr<ImageInterface<Float> >(fullIm);
  }

  //yet another pruneRegions - using connect component labelling with depth first search alogirthm ..
  std::shared_ptr<casacore::ImageInterface<Float> >  SDMaskHandler::YAPruneRegions(const ImageInterface<Float>& image, Vector<Bool>& chanflag, Vector<Bool>& allpruned, Vector<uInt>& nreg, Vector<uInt>& npruned, Double prunesize, Bool showchanlabel)
  {
    LogIO os( LogOrigin("SDMaskHandler", "YAPruneRegions",WHERE) );
    Timer timer;
    Bool debug(False);
    Bool recordPruned(False);
    if (allpruned.nelements()>0) {
       recordPruned=True;
       allpruned.set(False);
    }
   
    IPosition fullimShape=image.shape();
    TempImage<Float>* fullIm = new TempImage<Float>(TiledShape(fullimShape, image.niceCursorShape()), image.coordinates(), memoryToUse());
    fullIm->set(0);

    if (prunesize==0.0 ) {
      //No-op
      os<<LogIO::DEBUG1<<"Skip pruning of mask regions"<<LogIO::POST;
      fullIm->copyData(image);
      return std::shared_ptr<ImageInterface<Float> >(fullIm);
    }
    os <<LogIO::DEBUG1<< "pruneRegions with size="<<prunesize<<" is applied"<<LogIO::POST;

    IPosition shp = image.shape();
    Int specaxis = CoordinateUtil::findSpectralAxis(image.coordinates());
    uInt nchan = shp(specaxis);
    nreg.resize(nchan);
    npruned.resize(nchan);
    // do a single channel plane at time
    //  - assumes standard CASA image axis ordering (ra,dec,stokes,chan)
    for (uInt ich = 0; ich < nchan; ich++) {
      if (!chanflag(ich)) {
	IPosition start(4, 0, 0, 0,ich);
	IPosition length(4, shp(0),shp(1),shp(2),1);
	Slicer sl(start, length);
	//cerr<<"ich="<<ich<<" slicer sl ="<<sl<<endl;
	AxesSpecifier aspec(False);
	// following works if stokes axis dim = 1
	SubImage<Float>* subIm = new SubImage<Float>(image, sl, aspec, True);

	IPosition subimShape=subIm->shape();
	TempImage<Float>* tempIm = new TempImage<Float> (TiledShape(subIm->shape(), subIm->niceCursorShape()), subIm->coordinates(), memoryToUse() );
	// to search for both positive and negative components
	tempIm->copyData(LatticeExpr<Float> (abs(*subIm)));

	TempImage<Float>* blobMap = new TempImage<Float> (TiledShape(subIm->shape(), subIm->niceCursorShape()), subIm->coordinates(), memoryToUse() );
	blobMap->set(0);

	// connected componet labelling
	os<<LogIO::DEBUG1<<"Calling labelRegions..."<<LogIO::POST;
	Array<Float> tempImarr;
	tempIm->get(tempImarr);
	Float sumMaskVal=sum(tempImarr);
	uInt removeBySize=0;
	uInt nBlob=0; 
	os<<LogIO::DEBUG1<<" total pix of 1s="<< sumMaskVal <<LogIO::POST;
	if ( sumMaskVal !=0.0 ) {
	  timer.mark();
	  labelRegions(*tempIm, *blobMap);
	  os<< LogIO::DEBUG1 << "Processing time for labelRegions: real "<< timer.real()<< "s ; user "<< timer.user() <<"s"<< LogIO::POST;
	  Array<Float> tempblobarr;
	  blobMap->get(tempblobarr);
	  os<<LogIO::DEBUG1<<" total pix of 1s="<< sum(tempblobarr) <<LogIO::POST;
	  os<<LogIO::DEBUG1<<"Calling findBlobSize..."<<LogIO::POST;
	  // get blobsizes (the vector contains each labeled region size (label # = ith element+1)
	  //timer.mark();
	  Vector<Float> blobsizes = findBlobSize(*blobMap);
	  //cerr<<"blobsizes="<<blobsizes<<endl;
	  //use ImageDecomposer
	  // book keeping of no of  removed components`
	  //cerr<<"blobsizes.nelements()="<<blobsizes.nelements()<<endl; 
	  //removing operations
	  nBlob = blobsizes.nelements();
	  if (blobsizes.nelements()) {
	    if (prunesize > 0.0) {
	      for (uInt icomp = 0; icomp < blobsizes.nelements(); ++icomp) {
		if ( blobsizes[icomp] < prunesize ) {
		  Float blobid = Float(icomp+1);
		  removeBySize++;
		  tempIm->copyData( (LatticeExpr<Float>)( iif(*blobMap == blobid, 0.0, *tempIm  ) ) );
		}
	      }//for-loop
	    }
	  }
	} // if-sumMaskVal!=0
	// log reporting ...
        String chanlabel("");
        if (showchanlabel) {
	   chanlabel = "[C"+String::toString(ich)+"]";
        }
	if (removeBySize>0) {
	  os <<LogIO::DEBUG1<<chanlabel<<" pruneRegions removed "<<removeBySize<<" regions (out of "<<nBlob<<" ) from the mask image. "<<LogIO::POST;
	  if (recordPruned) {
	    if (removeBySize==nBlob) allpruned(ich) = True;
	  } 
	}
	else {
	  if (sumMaskVal!=0.0) {
	    os <<LogIO::NORMAL<<chanlabel<<" No regions are removed in pruning process." << LogIO::POST;
	  }
	  else {
	    os <<LogIO::NORMAL<<chanlabel<<" No regions are found in this plane."<< LogIO::POST;
	  }
	}
        nreg[ich] = nBlob;
        npruned[ich] = removeBySize;
	// Debug
	if (debug) {
	  PagedImage<Float> tempBlobMap(blobMap->shape(), blobMap->coordinates(), "tmp-Blob.map");
	  tempBlobMap.copyData(*blobMap);
	}
	Array<Float> subimData;
	tempIm->getSlice(subimData,IPosition(2,0), tempIm->shape(), IPosition(2,1,1));
	fullIm->putSlice(subimData,start,IPosition(4,1,1,1,1));
	delete tempIm; tempIm=0;
	delete subIm; subIm=0;
	delete blobMap; blobMap=0;
      } // if-skipmask
      else {
        nreg[ich] = 0;
        npruned[ich] = 0;
        if (showchanlabel) {
          os<<LogIO::DEBUG1<<"Skipping chan "<<ich<<" from pruning"<<LogIO::POST;
        }
        else {
          os<<LogIO::DEBUG1<<"Skipping this plane from pruning"<<LogIO::POST;
        }
           
      }
    } 
    return std::shared_ptr<ImageInterface<Float> >(fullIm);
  }


  Float SDMaskHandler::pixelBeamArea(const GaussianBeam& beam, const CoordinateSystem& csys) 
  {

      Quantity bmaj = beam.getMajor();
      Quantity bmin = beam.getMinor();
      Vector<Double> incVal = csys.increment();
      Vector<String> incUnit = csys.worldAxisUnits();
      Quantity qinc1(incVal[0],incUnit[0]);
      Quantity qinc2(incVal[1],incUnit[1]);
      // should in rad but make sure...
      Double pixArea = abs(qinc1.get(Unit("rad")).getValue() * qinc2.get(Unit("rad")).getValue()); 
      Double solidAngle = C::pi * bmaj.get(Unit("rad")).getValue() * bmin.get(Unit("rad")).getValue()/(4.* C::ln2);
      return (Float) solidAngle/pixArea;

  }

  void SDMaskHandler::makePBMask(std::shared_ptr<SIImageStore> imstore, Float pblimit, Bool combinemask)
  {
    LogIO os( LogOrigin("SDMaskHandler","makePBMask",WHERE) );

    if( imstore->hasPB() ) // Projection algorithms will have this.
      {
        LatticeExpr<Float> themask;
        if (combinemask && imstore->hasMask()) { 
          os<<"MAKE combined PB mask"<<LogIO::POST;
	  themask = LatticeExpr<Float> ( iif( (*(imstore->pb())) > pblimit, *(imstore->mask()), 0.0 ) );
        }
        else {
          os<<"MAKE PB mask"<<LogIO::POST;
	  themask = LatticeExpr<Float> ( iif( (*(imstore->pb())) > pblimit , 1.0, 0.0 ) );
        }
	imstore->mask()->copyData( themask );
      }
    else // Calculate it here...
      {
	// Get antenna diameter
	// Get frequency
	// Assuming a Gaussian, construct a circle region at pblimit.

	// But for now...
	throw(AipsError("Need PB/Sensitivity/Weight image before a PB-based mask can be made for "+imstore->getName())); 

      }
    // Also add option to just use the vpmanager or whatever centralized PB repository there will be (sometime in the distant future...).

  }// end of makePBMask

  //apply per channel plane threshold
  void SDMaskHandler::makeMaskByPerChanThreshold(const ImageInterface<Float>& image, Vector<Bool>& chanflag, ImageInterface<Float>& mask, Vector<Float>& thresholds, Vector<Float>& masksizes)
  {
    IPosition imshape = image.shape();

    CoordinateSystem imcsys = image.coordinates();
    Vector<Int> diraxes = CoordinateUtil::findDirectionAxes(imcsys);
    Int specaxis = CoordinateUtil::findSpectralAxis(imcsys);
    uInt nchan = imshape (specaxis);
    masksizes.resize(nchan); 
    if (nchan != thresholds.nelements()) {
      throw(AipsError("Mismatch in the number of threshold values and the number of chan planes."));
    }
    for (uInt ich=0; ich < nchan; ich++) {
      if (!chanflag(ich)) {
        IPosition start(4, 0, 0, 0,ich);
        IPosition length(4, imshape(diraxes(0)),imshape(diraxes(1)),imshape(2),1);
        Slicer sl(start, length);

        // make a subImage for  a channel slice      
        AxesSpecifier aspec(False);
        SubImage<Float> chanImage(image, sl, aspec, true);
        TempImage<Float>* tempChanImage = new TempImage<Float> (chanImage.shape(), chanImage.coordinates(), memoryToUse() );
        Array<Float> chanImageArr;
        LatticeExpr<Float> chanMask;
        if (thresholds(ich) < 0) {
          //LatticeExpr<Float> chanMask(iif(chanImage < thresholds(ich),1.0, 0.0)); 
          chanMask = LatticeExpr<Float> (iif(chanImage < thresholds(ich),1.0, 0.0)); 
        }
        else {
          //LatticeExpr<Float> chanMask(iif(chanImage > thresholds(ich),1.0, 0.0)); 
          chanMask = LatticeExpr<Float> (iif(chanImage > thresholds(ich),1.0, 0.0)); 
        }
        tempChanImage->copyData(chanMask);
        //tempChanImage->getSlice(chanImageArr, IPosition(4,0), chanImage.shape(),IPosition(4,1,1,1,1));
        tempChanImage->getSlice(chanImageArr, IPosition(2,0), chanImage.shape(),IPosition(2,1,1));
        mask.putSlice(chanImageArr,start,IPosition(4,1,1,1,1)); 
        masksizes[ich]=sum(chanImageArr);
        delete tempChanImage; tempChanImage=0;
      }
      else {
        masksizes[ich]=0;
      //  cerr<<"makeMaskByPerChanThresh: skipping chan="<<ich<<endl;
      }
    } // loop over chans
  }

  void SDMaskHandler::binaryDilationCore(Lattice<Float>& inlattice,
                      Array<Float>& structure,
                      Lattice<Bool>& mask,
                      Array<Bool>& chanmask,
                      Lattice<Float>& outlattice)
  {
    LogIO os( LogOrigin("SDMaskHandler", "binaryDilation", WHERE) );
    //
    //IPosition cursorShape=inlattice.niceCursorShape();
    IPosition inshape = inlattice.shape();
    Int nx = inshape(0);
    Int ny = inshape(1);
    // assume here 3x3 structure elements (connectivity of either 1 or 2)
    IPosition seshape = structure.shape();
    Int se_nx =seshape(0); 
    Int se_ny =seshape(1); 

    if (mask.shape()!=inshape) {
      throw(AipsError("Incompartible mask shape. Need to be the same as the input image."));
    } 
    // assume the origin of structure element is the center  se(1,1)
    IPosition cursorShape(4, nx, ny, 1, 1);
    IPosition axisPath(4, 0, 1, 3, 2);
    //cerr<<"cursorShape="<<cursorShape<<endl;
    //cerr<<"structure="<<structure<<endl;
    LatticeStepper tls(inlattice.shape(), cursorShape, axisPath); 
    RO_LatticeIterator<Float> li(inlattice, tls);
    RO_LatticeIterator<Bool> mi(mask, tls);
    LatticeIterator<Float> oli(outlattice,tls);
    Int ich;
    IPosition ipch(chanmask.shape().size(),0);

    // for debug
    //Array<Float> initarr=inlattice.get();
    //cerr<<"initarr sum pix="<<sum(initarr)<<endl;
    
    for (li.reset(), mi.reset(), oli.reset(), ich=0; !li.atEnd(); li++, mi++, oli++, ich++) {
      //Array<Float> planeImage(li.cursor());
      Array<Float> inMask(li.cursor());
      //cerr<<"sum of inMask="<<sum(inMask)<<endl;
      Array<Float> planeImage(inMask.shape());
      planeImage.set(0);
      planeImage=inMask;
      //cerr<<"sum of planeImage before grow ="<<sum(planeImage)<<endl;
      Array<Bool> planeMask(mi.cursor());
      ipch(0)=ich;
      // if masks are true do binary dilation...
      if (ntrue(planeMask)>0 && chanmask(ipch)) {
      //cerr<<"planeImage.shape()="<<planeImage.shape()<<endl;
      for (Int i=0; i < nx; i++) {
        for (Int j=0; j < ny; j++) {
          if (planeImage(IPosition(4,i,j,0,0))==1.0 && planeMask(IPosition(4,i,j,0,0)) ) {
            //cerr<<"if planeImage ==1 i="<<i<<" j="<<j<<endl;
            // Set the value for se(1,1)
            planeImage(IPosition(4,i,j,0,0)) = 2.0;
            for (Int ise=0; ise < se_nx; ise++) {
              for (Int jse = 0; jse < se_ny; jse++) {
                Int relx_se = ise - 1;
                Int rely_se = jse - 1;
                if (structure(IPosition(2,ise,jse)) && !(ise==1.0 && jse==1.0)) {
                  //cerr<<"structure("<<ise<<","<<jse<<")="<<structure(IPosition(2,ise,jse))<<endl; 
                  if ((i + relx_se) >= 0 && (i + relx_se) < nx &&
                      (j + rely_se) >= 0 && (j + rely_se) < ny) {
                    if (planeImage(IPosition(4,i+relx_se,j+rely_se,0,0))==0 ) {
                      // set to 2.0 to make distinction with the orignal 1.0 pixels
                      planeImage(IPosition(4, i+relx_se, j+rely_se,0,0))=2.0;
                      //cerr<<" i+relx_se="<<i+relx_se<<" j+rely_se="<<j+rely_se<<endl;
                    }                   
                  }
                } // if(se(ise,jse)
              } // if planeImage(i,j)=1.0
            } // S.E. col loop
          } // S.E. row loop
        } // image col loop
      } //inage row loop


      for (Int ii=0; ii < nx; ii++) {
        for (Int jj=0; jj < ny; jj++) {
          if (planeImage(IPosition(4,ii,jj,0,0))==2) 
            planeImage(IPosition(4,ii,jj,0,0))=1;
        }
      }
      } // if ntrure() ...
      oli.woCursor() = planeImage;
    }
    //For debug
    //Array<Float> afterinarr=inlattice.get();
    //cerr<<"afaterinarr sum pix ="<<sum(afterinarr)<<endl;
    //Array<Float> outarr = outlattice.get();
    //cerr<<"outlattice sum pix ="<<sum(outarr)<<endl;
  }

  void SDMaskHandler::binaryDilation(ImageInterface<Float>& inImage,
                      Array<Float>& structure,
                      Int niteration,
                      Lattice<Bool>& mask,
                      Array<Bool>& chanmask,
                      ImageInterface<Float>& outImage)
  {
      LogIO os( LogOrigin("SDMaskHandler", "binaryDilation", WHERE) );
      ArrayLattice<Float> templattice(inImage.shape());
      templattice.copyData(inImage);
      TempImage<Float> diffTempImage(outImage.shape(), outImage.coordinates(), memoryToUse());
      diffTempImage.set(1);
      // initial grow mask
      binaryDilationCore(inImage,structure,mask,chanmask,outImage);
      LatticeExpr<Float> diffIm0( abs(templattice - outImage ) );

      // if the initial grow does not change mask (i.e. diffIm0 = 0)
      // then it won't enter the while loop below. 
      diffTempImage.copyData(diffIm0);
      Int iter = 1;
      while (iter < niteration && !isEmptyMask(diffTempImage)) {
        templattice.copyData(outImage);
        binaryDilationCore(templattice,structure,mask,chanmask,outImage); 
        LatticeExpr<Float> diffIm( abs(templattice - outImage ) );
        diffTempImage.copyData(diffIm);
        /***
        if (isEmptyMask(diffTempImage)) { 
          cerr<<"current iter"<<iter<<" diffim is 0 "<<endl;
        }
        else {
          cerr<<"current iter"<<iter<<endl;
        } 
        ***/
        iter++;
      }
      os<<"grow iter done="<<iter<<LogIO::POST;
  }

 
  void SDMaskHandler::autoMaskWithinPB(std::shared_ptr<SIImageStore> imstore, 
                                       ImageInterface<Float>& posmask,
                                       const Int iterdone,
                                       Vector<Bool>& chanflag,
                                       Record& robuststatsrec,
                                       const String& alg, 
                                       const String& threshold, 
                                       const Float& fracofpeak, 
                                       const String& resolution,
                                       const Float& resbybeam, 
                                       const Int nmask,
                                       const Bool autoadjust,
                                       const Float& sidelobethreshold,
                                       const Float& noisethreshold, 
                                       const Float& lownoisethreshold,
                                       const Float& negativethreshold,
                                       const Float& cutthreshold, 
                                       const Float& smoothfactor,
                                       const Float& minbeamfrac,
                                       const Int growiterations,
                                       const Bool dogrowprune,
                                       const Float& minpercentchange,
                                       const Bool verbose,
                                       const Bool fastnoise,
                                       const Bool isthresholdreached,
                                       Float pblimit)
  { 
    LogIO os( LogOrigin("SDMaskHandler","autoMaskWithinPB",WHERE) );
    //Bool debug(False);

    os <<LogIO::DEBUG1<<"Calling autoMaskWithinPB .."<<LogIO::POST;
    // changed to do automask ater pb mask is generated so automask do stats within pb mask
    autoMask( imstore, posmask, iterdone, chanflag, robuststatsrec, alg, threshold, fracofpeak, resolution, resbybeam, nmask, autoadjust, 
              sidelobethreshold, noisethreshold, lownoisethreshold, negativethreshold, cutthreshold, smoothfactor, 
              minbeamfrac, growiterations, dogrowprune, minpercentchange, verbose, fastnoise, isthresholdreached, pblimit);

    if( imstore->hasPB() )
      {
        LatticeExpr<Float> themask( iif( (*(imstore->pb())) > pblimit , (*(imstore->mask())), 0.0 ) );
	imstore->mask()->copyData( themask );

        /**** 
        // apply pb mask as pixel mask for now. This will be converted to 1/0 image later
        LatticeExpr<Bool> mask( iif(*(imstore->pb()) > pblimit, True, False));
        os <<"calling MakeMask, hasPixelMask? "<<imstore->mask()->hasPixelMask()<<LogIO::POST;
        os <<"calling MakeMask, hasRegion mask0? "<<imstore->mask()->hasRegion("mask0")<<LogIO::POST;
        os <<"defaultMask "<<imstore->mask()->getDefaultMask()<<LogIO::POST;
        //ImageRegion outreg=imstore->mask()->makeMask("mask0", False, True);
        ImageRegion outreg=imstore.get()->mask()->makeMask("mask0", True, True);
        LCRegion& outmask=outreg.asMask();
        outmask.copyData(mask);
        os <<"Before defineRegion"<<LogIO::POST;
        imstore.get()->mask()->defineRegion("mask0", outreg, RegionHandler::Masks, True);
        os <<"setDefMask"<<LogIO::POST;
        imstore.get()->mask()->setDefaultMask("mask0");
        imstore.get()->releaseImage(imstore.get()->mask());
        if (debug) {
	  cerr<<"Make a copy"<<endl;
          PagedImage<Float> temppb(imstore->mask()->shape(), imstore->mask()->coordinates(),"tempPB.Im");
          temppb.copyData(*(imstore->mask()));
          temppb.defineRegion("mask0", outreg, RegionHandler::Masks, True);
          temppb.setDefaultMask("mask0");
        }
        ***/
        
      }
    //autoMask( imstore, alg, threshold, fracofpeak, resolution, resbybeam, nmask);

    // else... same options as makePBMask (put it into a helper function)
  }// end of autoMaskWithinPB

  //region labelling code
  void SDMaskHandler::depthFirstSearch(Int x, 
                                       Int y, 
                                       Int cur_label, 
                                       Array<Float>& inlatarr,
                                       Array<Float>& lablatarr)
  {
    Vector<Int> dx(4);
    Vector<Int> dy(4);
    // 4-direction connectivity
    dx(0) = 1; dx(1)=0;dx(2)=-1;dx(3)=0;
    dy(0) = 0; dy(1)=1;dy(2)=0;dy(3)=-1;
    //IPosition inshape = inlat.shape();
    IPosition inshape = inlatarr.shape();
    Int nrow = inshape(0);
    Int ncol = inshape(1);
    // out of bound condition
    if(x < 0 || x == nrow) return;
    if(y < 0 || y == ncol) return;
    //2d lattice is assumed
    IPosition loc(2,x,y);
    // already labelled or not value 1 pixel 
    if(lablatarr(loc) || !inlatarr(loc)) return;
   
    //cerr<<"cur_label="<<cur_label<<" loc="<<loc<<endl;

    lablatarr(loc) = Float(cur_label);
    //lablat.putAt(Float(cur_label), loc);
    //
    //
    //recursively check the neighbor 
    for (uInt inc = 0; inc < 4; ++inc)  
      depthFirstSearch(x + dx[inc], y + dy[inc], cur_label, inlatarr, lablatarr);
  }

  void SDMaskHandler::depthFirstSearch2(Int x,
                                       Int y,
                                       Int cur_label,
                                       Array<Float>& inlatarr,
                                       Array<Float>& lablatarr)

  {
    std::stack<IPosition> mystack;
    IPosition inshape = inlatarr.shape();
    Int nrow = inshape(0);
    Int ncol = inshape(1);
    // out of bound condition
    if(x < 0 || x == nrow) return;
    if(y < 0 || y == ncol) return;
    //2d lattice is assumed
    IPosition loc(2,x,y);
    // if already visited or not mask region, return 
    if(lablatarr(loc) || !inlatarr(loc)) return;

    IPosition curloc;
    mystack.push(loc);
    while (!mystack.empty()) {
      curloc = mystack.top();
      mystack.pop( );
      //cerr<<"curloc="<<curloc<<" cur_label="<<cur_label<<endl;
      lablatarr(curloc) = Float(cur_label);
      Vector<IPosition> loclist = defineNeighbors(curloc, nrow, ncol);
      //cerr<<"defineNeighbors done nelements="<<loclist.nelements()<<endl;
      if (loclist.nelements()) {
        for (uInt i=0; i < loclist.nelements(); ++i) 
        {
          if (inlatarr(loclist(i)) == 1 && lablatarr(loclist(i)) == 0 ) 
          {
            mystack.push(loclist(i));
          }
        }
      }
    } 
  }      

  Vector<IPosition> SDMaskHandler::defineNeighbors(IPosition& pos, Int nx, Int ny) 
  {
    Vector<IPosition> neighbors(0);
    Int nelement=0;
    // 4-direction connectivity
    Vector<Int> dx(4);
    Vector<Int> dy(4);
    dx(0) = 1; dx(1)=0;dx(2)=-1;dx(3)=0;
    dy(0) = 0; dy(1)=1;dy(2)=0;dy(3)=-1;
    for (uInt inc = 0; inc < 4; ++inc) {
      IPosition newpos(2,0);
      newpos(0) = pos(0)+dx(inc);
      newpos(1) = pos(1)+dy(inc);
      if (newpos(0) >= 0 && newpos(0) < nx && newpos(1) >= 0 &&  newpos(1) < ny)
      {
        nelement++;
        neighbors.resize(nelement,True);  
        neighbors(nelement-1)=newpos;
      }
    } 
    return neighbors;
  }

  void SDMaskHandler::labelRegions(Lattice<Float>& inlat, Lattice<Float>& lablat) 
  {
    Int blobId = 0;
    IPosition inshape = inlat.shape();
    Int nrow = inshape(0);
    Int ncol = inshape(1);
    Array<Float> inlatarr;
    Array<Float> lablatarr;
    inlat.get(inlatarr);
    lablat.get(lablatarr);
    //cerr<<"IN labelRegions:: inlat.shape="<<inlat.shape()<<" lablat.shape="<<lablat.shape()<<" nrow="<<nrow<<" ncol="<<ncol<<endl;

    if ( sum(inlatarr) !=0.0 ) {
      for (Int i = 0; i < nrow; ++i)
      { 
        for (Int j = 0; j < ncol; ++j) 
        {
          // evaluating elements with lattice seems to be very slow... 
          // changed to use Arrarys
          //if (!lablat(IPosition(2,i,j)) && inlat(IPosition(2,i,j) ) ) 
          if (!lablatarr(IPosition(2,i,j)) && inlatarr(IPosition(2,i,j) ) ) 
            //depthFirstSearch(i, j, ++blobId, inlatarr, lablatarr);
            // Use non-recursive version
            depthFirstSearch2(i, j, ++blobId, inlatarr, lablatarr);
        }
      }
      lablat.put(lablatarr);
    }
    //cerr<<"done blobId="<<blobId<<endl;
  }

  Vector<Float> SDMaskHandler::findBlobSize(Lattice<Float>& lablat) 
  {
  // iterate through lablat (2D)
  // find max label in lablat
  // create groupsize list vector gsize(max-1)
  // get val at each pixel in lablat (ival=lablat.get(loc)) and add 1 to gsize(ival-1) 
  // print each labelled comp's size...

    LogIO os( LogOrigin("SDMaskHandler","findBlobSize",WHERE) );
    IPosition inshape = lablat.shape();
    Int nrow = inshape(0);
    Int ncol = inshape(1);
    // getting max value via LatticeExprNode seems to be slower
    //LatticeExprNode leMax=max(lablat);
    //Float maxlab = leMax.getFloat();
    Array<Float> lablatarr;
    lablat.get(lablatarr);
    Float maxlab = max(lablatarr);
    //os<<LogIO::DEBUG1<<"maxlab="<<maxlab<<LogIO::POST;
    
    if (maxlab < 1.0) {
      return Vector<Float>();  
    }
    Vector<Float> blobsizes(Int(maxlab),0);
    for (Int i = 0; i < nrow; ++i) 
    { 
      for (Int j =0; j < ncol; ++j)
      {
        //IPosition loc(4, i, j, 0, 0);
        IPosition loc(2, i, j);
        //os<<LogIO::DEBUG1<<"i="<<i<<" j="<<j<<" labelat(loc)="<<lablat(loc)<<LogIO::POST;
        //if (lablat(loc)) blobsizes[Int(lablat(loc))-1]+=1;
        if (lablatarr(loc)) blobsizes[Int(lablatarr(loc))-1]+=1;
      }
    }

    //for debug
    for (Int k = 0;k < maxlab; ++k) 
    {
        os<<LogIO::DEBUG1<<"blobsizes["<<k<<"]="<<blobsizes[k]<<LogIO::POST;
    } 

    return blobsizes;
  }

  void SDMaskHandler::printAutomaskSummary (const Array<Double>& rmss, 
                                            const Array<Double>& maxs, 
                                            const Array<Double>& mins, 
                                            const Array<Double>& mdns, 
                                            //const Vector<Float>& maskthreshold, 
                                            const Matrix<Float>& maskthreshold, 
                                            //const Vector<String>& thresholdtype, 
                                            const Matrix<String>& thresholdtype, 
                                            //const Vector<Bool>& chanflag, 
                                            const Vector<Bool>& chanflag, 
                                            //const Vector<Bool>& /* zeroChanMask */,
                                            const Matrix<Bool>& /* zeroChanMask */,
                                            //const Vector<uInt>& nreg, 
                                            const Matrix<uInt>& nreg, 
                                            //const Vector<uInt>& npruned,
                                            const Matrix<uInt>& npruned,
                                            //const Vector<uInt>& ngrowreg,
                                            const Matrix<uInt>& ngrowreg,
                                            //const Vector<uInt>& ngrowpruned,
                                            const Matrix<uInt>& ngrowpruned,
                                            //const Vector<Float>& negmaskpixs, 
                                            const Matrix<Float>& negmaskpixs, 
                                            const Record& miscSummaryInfo) 

  {
    LogIO os( LogOrigin("SDMaskHandler","printAutomaskSummary",WHERE) );
   
    // miscSummaryInfo currently contains sidelobe level and pruneregionsize
    // but these won't be printed out here now (these are printed out in the beginning). 
    // or alll the arguments maybe packed into record...
    Float sidelobelevel;
    miscSummaryInfo.get("sidelobelevel", sidelobelevel); 
    Float prunesize;
    miscSummaryInfo.get("pruneregionsize", prunesize);

    os << LogIO::NORMAL <<"========== automask summary ==========" << LogIO::POST;  
    os << LogIO::NORMAL <<"chan masking? median   RMS"<<"         "
                        <<"peak   thresh_type   thresh_value   "
                        <<"N_reg N_pruned N_grow N_grow_pruned N_neg_pix"<<LogIO::POST;

    //use maskthreshold shape to find npol and nchan. Maskthreshold(npol, nchan)
    Int npol = maskthreshold.nrow();
    Int nchan = maskthreshold.ncolumn(); 
    os << LogIO::DEBUG1 << "npol="<<npol<< " nchan="<<nchan<<LogIO::POST;
    IPosition statshp = rmss.shape();
    // Note: stats record collapse the axis with 1
    os<<LogIO::DEBUG1 <<"rmss shape="<< String::toString(statshp) <<LogIO::POST;
    // For the historical reason it is called chanidx but it is an index for a single plane stats
    IPosition chanidx = statshp;
    //uInt ndim = rmss.ndim();

    //Int nchan = maskthreshold.nelements(); 
    for (uInt ich = 0; ich < (uInt) nchan; ich++) {
      if (chanidx.nelements()==1 ) {
          chanidx(0) = ich;
        }
      else if(chanidx.nelements()==2) {
          // to include stats in all stokes in a single line
          chanidx(1) = ich;
	  chanidx(0) =0;
      }
        Vector<Double> peaks(npol);
        for (uInt ipol = 0; ipol < (uInt) npol; ipol++) {
        //Double peak = abs(maxs(chanidx))> abs( mins(chanidx))? maxs(chanidx): mins(chanidx);
          if (npol!=1) {
            chanidx(0) = ipol;
          }
          peaks(ipol) = abs(maxs(chanidx))> abs( mins(chanidx))? maxs(chanidx): mins(chanidx);
          //os << LogIO::DEBUG1 << "chanidx="<<chanidx<< " peaks("<<ipol<<")="<<peaks(ipol)<<LogIO::POST;
          
        }
        String peakStr = npol==1? String::toString(peaks(0)):String::toString(peaks);
        // only tested for single pol (normally stokes I)
        String domasking = chanflag(ich)==0? "T":"F";
        //String domasking = zeroChanMask[ich]==1? "F":"T";
        String mdnsStr, rmssStr, maskthresholdStr;
        String Nreg, Npruned, Ngrowreg, NgrowPruned, Nnegpix;
        String NAstr("--");
      
        //if masking is skipped median, rms, thresholdvalue are
        //set to ---
        if (domasking=="F") {
          mdnsStr=NAstr;
          rmssStr=NAstr;
          maskthresholdStr=NAstr;
        }
        else {
          //mdnsStr=String::toString(mdns(chanidx));
          //rmssStr=String::toString(rmss(chanidx));
          //maskthresholdStr=String::toString(maskthreshold[ich]);
          //reset pol axis of chanidx
          IPosition trc = chanidx;
          if (npol > 1) {
            chanidx(0)=0;
            trc(0)=npol-1;
          }

          os<<LogIO::DEBUG1<<" mdns.shape="<<mdns.shape()<<"chanidx="<<chanidx<<" trc="<<trc<<LogIO::POST;
          if (chanidx.nelements()==1) {
            if (nchan==1) {
              mdnsStr=String::toString(mdns);
              rmssStr=String::toString(rmss);
            }
            else {
              mdnsStr=String::toString(mdns(chanidx));
              rmssStr=String::toString(rmss(chanidx));
            } 
          }
          else {
            Matrix<Double> mdnsChan=mdns(chanidx, trc);
            Matrix<Double> rmssChan=rmss(chanidx, trc);
            //mdnsStr=String::toString(mdns(chanidx,trc));
            //rmssStr=String::toString(rmss(chanidx,trc));
            mdnsStr=String::toString(mdnsChan.column(0));
            rmssStr=String::toString(rmssChan.column(0));
          }
          Vector<Float> maskthreshvec = maskthreshold.column(ich); 
          if (maskthreshvec.nelements()==1) {
            maskthresholdStr=String::toString(maskthreshvec(0));
          }
          else {
            maskthresholdStr=String::toString(maskthreshvec);
          }
        }

      if (!nreg.nelements()) {
        Nreg = NAstr;
      }
      else {
        //Nreg = String::toString(nreg[ich]);  
        Vector<uInt> nregvec = nreg.column(ich);  
        if (nregvec.nelements()==1) {
          Nreg = String::toString(nregvec(0));  
        }
        else {
          Nreg = String::toString(nregvec);  
        }
      }
      if (!npruned.nelements()) {
        Npruned = NAstr;
      }
      else {
        //Npruned = String::toString(npruned[ich]);
        Vector<uInt> nprunedvec = npruned.column(ich);  
        if (nprunedvec.nelements()==1) {
          Npruned = String::toString(nprunedvec(0));
        }
        else {
          Npruned = String::toString(nprunedvec);
        }
      }
      if (!ngrowreg.nelements()) {
        Ngrowreg = NAstr;
      }
      else {
        //Ngrowreg = String::toString(ngrowreg[ich]);
        Vector<uInt> ngrowregvec = ngrowreg.column(ich);
        if (ngrowregvec.nelements()==1) {
          Ngrowreg = String::toString(ngrowregvec(0));
        }
        else {
          Ngrowreg = String::toString(ngrowreg.column(ich));
        }
      }
      if (!ngrowpruned.nelements()) {
        NgrowPruned = NAstr;
      }
      else {
        //NgrowPruned = String::toString(ngrowpruned[ich]);
        Vector<uInt> ngrowprunedvec=ngrowpruned.column(ich);
        if (ngrowprunedvec.nelements()==1) {
          NgrowPruned = String::toString(ngrowprunedvec(0));
        }
        else {
          NgrowPruned = String::toString(ngrowprunedvec);
        }
      }
      if (!negmaskpixs.nelements()) {
        Nnegpix = NAstr;
      }
      else {
        //Nnegpix = String::toString(negmaskpixs[ich]);
        Vector<Float> negmaskpixsvec=negmaskpixs.column(ich);
        if (negmaskpixsvec.nelements()==1) {
          Nnegpix = String::toString(negmaskpixsvec(0));
        }
        else {
          Nnegpix = String::toString(negmaskpixsvec);
        }
      }
      Vector<String> threshtypevec=thresholdtype.column(ich);
      String threshtypeStr;
      if (threshtypevec.nelements()==1) {
        threshtypeStr=threshtypevec(0); 
      }
      else {
        threshtypeStr=String::toString(threshtypevec); 
      }
 
      os << LogIO::NORMAL << "[C" << ich << "] " 
                          << domasking << "        " 
                          //<< mdns(chanidx) << "  "
                          //<< rmss(chanidx) << "  " 
                          << mdnsStr << "  "
                          << rmssStr << "  " 
                          << peakStr << "  " 
                          << threshtypeStr << "  " 
                          //<< maskthreshold[ich] << "        "
                          << maskthresholdStr << "        "
                          << Nreg << "  " 
                          << Npruned << "  "
                          << Ngrowreg << "  "
                          << NgrowPruned << "  "
                          << Nnegpix 
                          << LogIO::POST;
    }
    os << LogIO::NORMAL <<"========== END of automask summary ==========" << LogIO::POST;  
  }

  void SDMaskHandler::setPBMaskLevel (const Float pbmasklevel) {
    itsPBMaskLevel = pbmasklevel;
  } 
  
  Float SDMaskHandler::getPBMaskLevel() {
    return itsPBMaskLevel;
  }
     
} //# NAMESPACE CASA - END
