// -*- C++ -*-
//# ImageInformation.c: Implementation of ImageInformation class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003,2026
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

#include <synthesis/TransformMachines2/ImageInformation.h>
#include <synthesis/TransformMachines2/Utils.h>
//
//------------------------------------------------------------------------
// Saving makes the object both in memory and persistent
//
namespace casa
{
  using namespace vi;
  namespace refim
  {
    namespace SynthesisUtils
    {
      template <typename T>
      void ImageInformation<T>::save(const casacore::String& targetPath)
      {
	initPaths(targetPath);
	save();
      }
      
      template <typename T>
      void ImageInformation<T>::save()
      {
	if (cimg_p == NULL)
	  throw(ImageInformationSaveError("ImageInformation::save():  "
					  "Use ImageInformation(ImageInterface&, String&) "
					  "ctor to save image information"));
	//
	// Save image coordinate system as a casacore::Record
	//
	SynthesisUtils::saveAsRecord(cimg_p->coordinates(),
				     cimg_p->shape(),
				     coordSysRecFileName,
				     coordSysKey);
	//
	// Save cimg.shape() as a casacore::Record
	//
	casacore::RecordDesc imInfoDesc;
	// Add image shape to the record.
	imInfoDesc.addField(imShapeKey,casacore::TpArrayInt);
	casacore::Vector<int> shp=cimg_p->shape().asVector();
	
	casacore::Record imInfoRec(imInfoDesc);
	int imsId=imInfoDesc.fieldNumber(imShapeKey);
	imInfoRec.define(imsId,shp);
	SynthesisUtils::writeRecord(imInfoRecFileName, imInfoRec);
	
	//
	// Save miscinfo
	//
	casacore::TableRecord miscinfo= cimg_p->miscInfo();
	if (miscinfo.nfields() > 0)
	  {
	    SynthesisUtils::writeRecord(miscInfoRecFileName,
					miscinfo);
	  }
	
	// The info has been made persistent on the disk. The get*() functions
	// can return the info from the persistent copy.
	inMemory_p=true;
	isPersistent_p=true;
      }
      //
      //------------------------------------------------------------------------
      //
      template <typename T>
      void ImageInformation<T>::save(const casacore::CoordinateSystem& coordinates,
				     const casacore::IPosition& shape,
				     const casacore::Record& miscinfo)
      {
	//
	// Save image coordinate system as a casacore::Record
	//
	SynthesisUtils::saveAsRecord(coordinates,
				     shape,
				     coordSysRecFileName,
				     coordSysKey);
	//
	// Save cimg.shape() as a casacore::Record
	//
	casacore::RecordDesc imInfoDesc;
	// Add image shape to the record.
	imInfoDesc.addField(imShapeKey,casacore::TpArrayInt);
	casacore::Vector<int> shp=shape.asVector();
	
	casacore::Record imInfoRec(imInfoDesc);
	int imsId=imInfoDesc.fieldNumber(imShapeKey);
	imInfoRec.define(imsId,shp);
	SynthesisUtils::writeRecord(imInfoRecFileName, imInfoRec);
	
	//
	// Save miscinfo
	//
	if (miscinfo.nfields() > 0)
	  {
	    SynthesisUtils::writeRecord(miscInfoRecFileName,
					miscinfo);
	  }
      }
      //
      //------------------------------------------------------------------------
      //
      template <typename T>
      casacore::CoordinateSystem ImageInformation<T>::getCoordinateSystem()
      {
	if (isCached_p) return coordinateSystem_p;
	if (inMemory_p) return cimg_p->coordinates();
	//
	// Use the supplied filename as-is here.  The logic to
	// modify the name for reading as a Record or a Table
	// are in SynthesisUtils::read/writeRecord.  At this level,
	// the interface is via casacore::Record only.
	//
	casacore::String fileName = coordSysRecFileName;
	// if (!fileExists(fileName.c_str()))
	//   {
	//     fileName = coordSysRecFileName + casacore::String(".tab");
	//     if (!fileExists(fileName.c_str()))
	//       throw(ImageInformationRecordNotFoundError("ImageInformation::getCoordinateSystem(): "+fileName+" does not exist."));
	//   }
	
	casacore::IPosition dummyImShape;
	casacore::CoordinateSystem csys;
	
	SynthesisUtils::readFromRecord(csys,
				       dummyImShape,
				       fileName,
				       coordSysKey);
	return csys;
	
      };
      //
      //------------------------------------------------------------------------
      //
      template <typename T>
      casacore::Vector<int> ImageInformation<T>::getImShape()
      {
	if (isCached_p) return imShape_p;
	if (inMemory_p) return cimg_p->shape().asVector();
	//
	// Use the supplied filename as-is here.  The logic to
	// modify the name for reading as a Record or a Table
	// are in SynthesisUtils::read/writeRecord.  At this level,
	// the interface is via casacore::Record only.
	//
	casacore::String fileName=imInfoRecFileName;
	casacore::TableRecord imInfo = casacore::TableRecord(SynthesisUtils::readRecord(fileName));
	casacore::Vector<int> imShape;
	imInfo.get(imShapeKey,imShape);
	return imShape;
      };
      //
      //------------------------------------------------------------------------
      //
      template <typename T>
      casacore::TableRecord ImageInformation<T>::getMiscInfo()
      {
	if (isCached_p) return miscInfo_p;
	if (inMemory_p) return cimg_p->miscInfo();
	//
	// Use the supplied filename as-is here.  The logic to
	// modify the name for reading as a Record or a Table
	// are in SynthesisUtils::read/writeRecord.  At this level,
	// the interface is via casacore::Record only.
	//
	casacore::String fileName=miscInfoRecFileName;
	return casacore::TableRecord(SynthesisUtils::readRecord(fileName));
      }
      template class ImageInformation<casacore::Complex>;
      template class ImageInformation<casacore::DComplex>;
      template class ImageInformation<double>;
      template class ImageInformation<float>;
    };
  };
};
