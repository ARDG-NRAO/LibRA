// -*- C++ -*-
//# ImageInformation.h: Definition of ImageInformation.cc
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
#ifndef SYNTHESIS_TRANSFORM2_IMAGEINFORMATION_H
#define SYNTHESIS_TRANSFORM2_IMAGEINFORMATION_H


#include <casacore/casa/aips.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageOpener.h>
//#include <ms/MeasurementSets/MeasurementSet.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <iostream>

namespace casa
{
  namespace refim {

    class ImageInformationError : public casacore::AipsError {
    public:
      ImageInformationError (const casacore::String& message, const casacore::AipsError::Category c=casacore::AipsError::GENERAL):
	casacore::AipsError(message,c)
      {};
      ~ImageInformationError () noexcept {};
    };

    class ImageInformationSaveError : public ImageInformationError {
    public:
      ImageInformationSaveError (const casacore::String& message, const casacore::AipsError::Category c=casacore::AipsError::GENERAL):
	ImageInformationError(message,c)
      {};
      ~ImageInformationSaveError () noexcept {};
    };

    class ImageInformationRecordNotFoundError : public ImageInformationError {
    public:
      ImageInformationRecordNotFoundError (const casacore::String& message, const casacore::AipsError::Category c=casacore::AipsError::GENERAL):
	ImageInformationError(message,c)
      {};
      ~ImageInformationRecordNotFoundError () noexcept {};
    };
    

    template <class T>
    class ImageInformation
    {
    public:
      ImageInformation():
	cimg_p(NULL),
	coordSysFileName("cgrid_csys.rec"),coordSysKey("cgrid_csys"),
	imInfoFileName("iminfo.rec"), imShapeKey("imshape"),
	miscInfoFileName("miscInfo.rec"), miscInfoKey("miscInfo"),
	coordSysRecFileName(), imInfoRecFileName(), miscInfoRecFileName()
      {};
      //
      // Constructor to read existing records save by this class
      //
      ImageInformation(const casacore::String& targetPath):
	cimg_p(NULL),
	coordSysFileName("cgrid_csys.rec"),coordSysKey("cgrid_csys"),
	imInfoFileName("iminfo.rec"), imShapeKey("imshape"),
	miscInfoFileName("miscInfo.rec"), miscInfoKey("miscInfo"),
	coordSysRecFileName(), imInfoRecFileName(), miscInfoRecFileName()
      {
	initPaths(targetPath);
      };
      //
      // Constructor to write and read records saved by this class
      //
      ImageInformation(casacore::ImageInterface<T>& cimg,
		       const casacore::String& targetPath):
	cimg_p(&cimg),
	coordSysFileName("cgrid_csys.rec"),coordSysKey("cgrid_csys"),
	imInfoFileName("iminfo.rec"), imShapeKey("imshape"),
	miscInfoFileName("miscInfo.rec"), miscInfoKey("miscInfo"),
	coordSysRecFileName(), imInfoRecFileName(), miscInfoRecFileName()
      {
	initPaths(targetPath);
      };
      
      void initPaths(const casacore::String& targetPath)
      {
	coordSysRecFileName = targetPath+'/'+coordSysFileName;
	imInfoRecFileName=targetPath+'/'+imInfoFileName;
	miscInfoRecFileName=targetPath+'/'+miscInfoFileName;
      }
      
      void save()
      {
	if (cimg_p == NULL)
	  throw(ImageInformationSaveError("ImageInformation::save():  Use ImageInformation(ImageInterface&, String&) ctor to save image information"));
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
      }
      //
      //------------------------------------------------------------------------
      //
      casacore::CoordinateSystem getCoordinateSystem()
      {
	if (!fileExists(coordSysRecFileName.c_str()))
	  throw(ImageInformationRecordNotFoundError("ImageInformation::getCoordinateSystem(): "+coordSysRecFileName+" does not exist."));

	casacore::IPosition dummyImShape;
	casacore::CoordinateSystem csys;
	
	SynthesisUtils::readFromRecord(csys,
				     dummyImShape,
				     coordSysRecFileName,
				     coordSysKey);
	return csys;
	
      };
      //
      //------------------------------------------------------------------------
      //
      casacore::Vector<int> getImShape()
      {
	if (!fileExists(coordSysRecFileName.c_str()))
	  throw(ImageInformationRecordNotFoundError("ImageInformation::getCoordinateSystem(): "+imInfoRecFileName+" does not exist."));

	casacore::TableRecord imInfo = casacore::TableRecord(SynthesisUtils::readRecord(imInfoRecFileName));
	casacore::Vector<int> imShape;
	imInfo.get(imShapeKey,imShape);
	return imShape;
      };
      //
      //------------------------------------------------------------------------
      //
      casacore::TableRecord getMiscInfo()
      {
	if (!fileExists(coordSysRecFileName.c_str()))
	  throw(ImageInformationRecordNotFoundError("ImageInformation::getCoordinateSystem(): "+miscInfoRecFileName+" does not exist."));

	return casacore::TableRecord(SynthesisUtils::readRecord(miscInfoRecFileName));
      }
      ~ImageInformation() {};
    private:
      casacore::ImageInterface<T> *cimg_p;
      casacore::String coordSysFileName,  coordSysKey;
      casacore::String imInfoFileName, imShapeKey;
      casacore::String miscInfoFileName,  miscInfoKey;
      casacore::String coordSysRecFileName, imInfoRecFileName, miscInfoRecFileName;

      inline bool fileExists(const char* name)
      {
	if (FILE *file = fopen(name, "r"))
	  {
	    fclose(file);
	    return true;
	  }
	else
	  {
	    return false;
	  }
      };
    };
  };
};
#endif
