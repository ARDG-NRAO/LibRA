//# WFCleanImageSkyModel.cc: Implementation of WFCleanImageSkyModel class
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

#include <casa/Arrays/ArrayMath.h>
#include <synthesis/MeasurementComponents/WFCleanImageSkyModel.h>
//#include <synthesis/MeasurementComponents/MFCleanImageSkyModel.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageInterface.h>
#include <casa/OS/File.h>
#include <images/Images/SubImage.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LRegions/LCBox.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogIO.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

WFCleanImageSkyModel::WFCleanImageSkyModel():
  MFCleanImageSkyModel(),  nfacets_p(1), facets_p(1) /*,
  largeMem_p(false)*/ {

  imageImage_p=0;
  residualImage_p=0;
  maskImage_p=0;
};

WFCleanImageSkyModel::WFCleanImageSkyModel(const Int facets, Bool ):
  MFCleanImageSkyModel(), nfacets_p(facets*facets),  facets_p(facets) /*,
  largeMem_p(largeMemory) */
{
  imageImage_p=0;
  residualImage_p=0;
  maskImage_p=0;

};

Int WFCleanImageSkyModel::add(ImageInterface<Float>& image,
			      const Int maxNumXfr)
{
  // Is this the first image?
  if(imageImage_p.null()) {
    // Create the facet images and copy the relevant region from the 
    // original image
    imageImage_p=CountedPtr<ImageInterface<Float> >(&image, false);
    facetImages_p.resize(nfacets_p);
    for (Int facet=0;facet<nfacets_p;facet++) {
      facetImages_p[facet] = makeFacet(facet, image);
      AlwaysAssert(!facetImages_p[facet].null(), AipsError);
      AlwaysAssert(MFCleanImageSkyModel::add(*facetImages_p[facet], maxNumXfr)==facet,
		   AipsError);
    }
    return 0;
  }
  else {
    // All other images are added as is
    Int result = MFCleanImageSkyModel::add(image, maxNumXfr);
    return result-nfacets_p+1;
  }
};

Bool WFCleanImageSkyModel::addMask(Int image, ImageInterface<Float>& mask)
{
  // Create the facet images and copy the relevant region from the 
  // original image
  AlwaysAssert(image>-1, AipsError);
  if(maskImage_p.null()) {
    maskImage_p=CountedPtr<ImageInterface<Float> >(&mask, false);
    facetMaskImages_p.resize(nfacets_p);
    for (Int facet=0;facet<nfacets_p;facet++) {
      facetMaskImages_p[facet] = makeFacet(facet, mask);
      AlwaysAssert(!facetMaskImages_p[facet].null(), AipsError);
      AlwaysAssert(MFCleanImageSkyModel::addMask(facet, *facetMaskImages_p[facet]),
		   AipsError);
    }
    return true;
  }
  else {
    // All other images are added as is
    return MFCleanImageSkyModel::addMask(image+nfacets_p-1, mask);
  }
};

Bool WFCleanImageSkyModel::addResidual(Int image,
				       ImageInterface<Float>& residual) 
{
  // Create the facet images and copy the relevant region from the 
  // original image
  AlwaysAssert(image>-1, AipsError);
  if(residualImage_p.null()) {
    residualImage_p=CountedPtr<ImageInterface<Float> >(&residual, false);
    facetResidualImages_p.resize(nfacets_p);
    for (Int facet=0;facet<nfacets_p;facet++) {
      facetResidualImages_p[facet] = makeFacet(facet, residual);
      AlwaysAssert(!facetResidualImages_p[facet].null(), AipsError);
      AlwaysAssert(MFCleanImageSkyModel::addResidual(facet, *facetResidualImages_p[facet]),
		   AipsError);
    }
    return true;
  }
  else {
    // All other images are added as is
    return MFCleanImageSkyModel::addResidual(image+nfacets_p-1, residual);
  }
};

WFCleanImageSkyModel::~WFCleanImageSkyModel()
{
  // Now destroy all the facet images
  /*
  if(imageImage_p !=0){
    for (Int facet=0;facet<nfacets_p;facet++) {
      if(facetImages_p[facet]); delete facetImages_p[facet];
      facetImages_p[facet]=0;
      if(Int(facetMaskImages_p.nelements())>facet) {
	if(facetMaskImages_p[facet]) delete facetMaskImages_p[facet];
	facetMaskImages_p[facet]=0;
      }
      if(Int(facetResidualImages_p.nelements())>facet) {
	if(facetResidualImages_p[facet]) delete facetResidualImages_p[facet];
	facetResidualImages_p[facet]=0;
      }
    }
  }
  */
};

// Clean solver: just call the MF solver
Bool WFCleanImageSkyModel::solve(SkyEquation& se) {

  LogIO os(LogOrigin("WFCleanImageSkyModel","solve"));

  os << "Starting wide-field clean with " << nfacets_p << " facets"
     << " and " << this->numberOfModels()-nfacets_p << " outliers" << LogIO::POST;
  maskImage_p=0; // the masks will be have to be renewed when continuing 
  // The MFCleanImageSkyModel solve cleans all images in
  // parallel.
  if(!MFCleanImageSkyModel::solve(se)) {
    os << "Wide-field clean apparently failed to reach threshold" << LogIO::POST;
    return false;
  }
  
  return(true);
};
  
ImageInterface<Float>& WFCleanImageSkyModel::getResidual(Int trueMod){

  if(trueMod==0){
    if(residualImage_p.null()){
      residualImage_p=new TempImage<Float>(imageImage_p->shape(), imageImage_p->coordinates());
      facetResidualImages_p.resize(nfacets_p);
      for (Int facet=0;facet<nfacets_p; ++facet) {
	facetResidualImages_p[facet] = makeFacet(facet, *residualImage_p);
	AlwaysAssert(!facetResidualImages_p[facet].null(), AipsError);	
      }
      
    }
    for (Int facet=0;facet<nfacets_p;++facet) 
      facetResidualImages_p[facet]->copyData(residual(facet));
    return *residualImage_p;
    
  }
  else{
    //return the outlying Multi field residuals 
    return residual(trueMod+nfacets_p-1);

  }



}
    
SubImage<Float>* 
WFCleanImageSkyModel::makeFacet(Int facet, ImageInterface<Float>& image)
{

  LogIO os(LogOrigin("WFCleanImageSkyModel","makeFacet"));

  
  // Make the output image
  Slicer imageSlicer;
  
  // Have to fiddle about with the shape (double it in  x and y)
  // and the coordinates (change the ref pixel).
  IPosition facetShape(image.shape());
  CoordinateSystem coords=image.coordinates();
  Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
  AlwaysAssert(directionIndex>=0, AipsError);
  DirectionCoordinate
    facetDirCoord=coords.directionCoordinate(directionIndex);
  if(!makeSlicers(facet, image.shape(), facetShape, imageSlicer)) {
    os << LogIO::SEVERE << "Cannot create facet image" << LogIO::EXCEPTION;
  }
  // Now we have all the information and we can create the facet image
  SubImage<Float>*  facetImage = new SubImage<Float>(image, imageSlicer, true);
  facetImage->setMiscInfo(image.miscInfo());
  facetImage->setUnits(image.units());
  
  return facetImage;
}

Bool 
WFCleanImageSkyModel::makeSlicers(const Int facet, const IPosition& imageShape,
				  IPosition& facetShape,
				  Slicer& imageSlicer)
{
  LogIO os(LogOrigin("WFCleanImageSkyModel","makeSlicers"));
                    
  if((facet>(nfacets_p-1))||(facet<0)) {
    os << LogIO::SEVERE << "Illegal facet " << facet << LogIO::POST;
    return false;
  }

  IPosition imageBlc(imageShape.nelements(), 0);
  IPosition imageTrc(imageShape.nelements(), 0);
  IPosition inc=IPosition(imageShape.nelements(), 1);

  facetShape=imageShape;
  IPosition facetBlc(facetShape.nelements(), 0);
  IPosition facetTrc(facetShape.nelements(), 0);

  Int facetx = facet % facets_p; 
  Int facety = (facet - facetx) / facets_p;
  Int sizex = imageShape(0) / facets_p;
  Int sizey = imageShape(1) / facets_p;

  // Make the image Slicer
  imageBlc(0) = facetx * sizex; imageTrc(0) = imageBlc(0) + sizex - 1;
  imageBlc(1) = facety * sizey; imageTrc(1) = imageBlc(1) + sizey - 1;
  {
    for (uInt i=2;i<imageShape.nelements();i++) {
      imageTrc(i)=imageBlc(i)+imageShape(i)-1;
      if(imageTrc(i) > (imageShape(i)-1)) imageTrc(i) = imageShape(i) -1;
    }
  }

  LCBox::verify(imageBlc, imageTrc, inc, imageShape);

  imageSlicer=Slicer (imageBlc, imageTrc, inc, Slicer::endIsLast);

  // Make the facet Slicer to place the imageBlc at the 
  // end of first quarter of the facet and the imageTrc at the
  // start of the last quarter.
  facetShape(0) = sizex;
  facetBlc(0) = 0; facetTrc(0) = facetBlc(0) + sizex - 1;
  facetShape(1) = sizey;
  facetBlc(1) = 0; facetTrc(1) = facetBlc(1) + sizey - 1;
  {
    for (uInt i=2;i<facetShape.nelements();i++) {
      facetTrc(i)=facetBlc(i)+facetShape(i)-1;
      if(facetTrc(i) > (facetShape(i)-1)) facetTrc(i) = facetShape(i) -1;
    }
  }

  LCBox::verify(facetBlc, facetTrc, inc, facetShape);

  os << LogIO::DEBUGGING << "Facet " << facet+1 
     << " : from " << imageBlc+1<< " to " << imageTrc+1 << LogIO::POST;
  
  return true;
}



} //# NAMESPACE CASA - END



