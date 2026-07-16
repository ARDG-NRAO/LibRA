// -*- C++ -*-
//# ImageInformation.h: Definition of ImageInformation.cc
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
#ifndef SYNTHESIS_TRANSFORM2_IMAGEINFORMATION_H
#define SYNTHESIS_TRANSFORM2_IMAGEINFORMATION_H


#include <casacore/casa/aips.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageOpener.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <iostream>

namespace casa
{
  namespace refim
  {
    namespace SynthesisUtils
    {
      //-----------------------------------------------------------------------------------------------------------------------
      class ImageInformationError : public casacore::AipsError
      {
      public:
	ImageInformationError (const casacore::String& message, const casacore::AipsError::Category c=casacore::AipsError::GENERAL):
	  casacore::AipsError(message,c)
	{};
	~ImageInformationError () noexcept {};
      };
      
      class ImageInformationSaveError : public ImageInformationError
      {
      public:
	ImageInformationSaveError (const casacore::String& message, const casacore::AipsError::Category c=casacore::AipsError::GENERAL):
	  ImageInformationError(message,c)
	{};
	~ImageInformationSaveError () noexcept {};
      };
      
      class ImageInformationRecordNotFoundError : public ImageInformationError
      {
      public:
	ImageInformationRecordNotFoundError (const casacore::String& message, const casacore::AipsError::Category c=casacore::AipsError::GENERAL):
	  ImageInformationError(message,c)
	{};
	~ImageInformationRecordNotFoundError () noexcept {};
      };
      //-----------------------------------------------------------------------------------------------------------------------
      
      
      template <typename T>
      class ImageInformation
      {
      public:
	ImageInformation():
	  cimg_p(NULL),
	  coordSysFileName("cgrid_csys.rec"),coordSysKey("cgrid_csys"),
	  imInfoFileName("iminfo.rec"), imShapeKey("imshape"),
	  miscInfoFileName("miscInfo.rec"), miscInfoKey("miscInfo"),
	  coordSysRecFileName(), imInfoRecFileName(), miscInfoRecFileName(),
	  inMemory_p(false),isPersistent_p(false),isCached_p(false),
	  imShape_p(), coordinateSystem_p(), miscInfo_p()
	{};
	//
	//------------------------------------------------------------------------
	// Constructor to read existing records save by this class
	//
	ImageInformation(const casacore::String& targetPath, bool load_miscinfo=true):
	  cimg_p(NULL),
	  coordSysFileName("cgrid_csys.rec"),coordSysKey("cgrid_csys"),
	  imInfoFileName("iminfo.rec"), imShapeKey("imshape"),
	  miscInfoFileName("miscInfo.rec"), miscInfoKey("miscInfo"),
	  coordSysRecFileName(), imInfoRecFileName(), miscInfoRecFileName(),
	  inMemory_p(false),isPersistent_p(false),isCached_p(false),
	  imShape_p(), coordinateSystem_p(), miscInfo_p()
	{
	  initPaths(targetPath);
	  imShape_p=getImShape();
	  coordinateSystem_p=getCoordinateSystem();
	  if (load_miscinfo) miscInfo_p=getMiscInfo();
	  isCached_p=true;
	};
	//------------------------------------------------------------------------
	// Constructor to write and read records saved by this class
	// This creates an in-memory-only object
	ImageInformation(casacore::ImageInterface<T>& cimg):
	  cimg_p(&cimg),
	  coordSysFileName("cgrid_csys.rec"),coordSysKey("cgrid_csys"),
	  imInfoFileName("iminfo.rec"), imShapeKey("imshape"),
	  miscInfoFileName("miscInfo.rec"), miscInfoKey("miscInfo"),
	  coordSysRecFileName(), imInfoRecFileName(), miscInfoRecFileName(),
	  inMemory_p(true),isPersistent_p(false),isCached_p(false),
	  imShape_p(), coordinateSystem_p(), miscInfo_p()
	{
	  imShape_p=getImShape();
	  coordinateSystem_p=getCoordinateSystem();
	  miscInfo_p=getMiscInfo();
	  isCached_p=true;
	};
	//
	//------------------------------------------------------------------------
	// Constructor to write and read records saved by this class
	// This creates an in-memory-only object
	ImageInformation(casacore::ImageInterface<T>& cimg,
			 const casacore::String& targetPath):
	  cimg_p(&cimg),
	  coordSysFileName("cgrid_csys.rec"),coordSysKey("cgrid_csys"),
	  imInfoFileName("iminfo.rec"), imShapeKey("imshape"),
	  miscInfoFileName("miscInfo.rec"), miscInfoKey("miscInfo"),
	  coordSysRecFileName(), imInfoRecFileName(), miscInfoRecFileName(),
	  inMemory_p(true),isPersistent_p(false),isCached_p(false),
	  imShape_p(), coordinateSystem_p(), miscInfo_p()
	{
	  initPaths(targetPath);
	  imShape_p=getImShape();
	  coordinateSystem_p=getCoordinateSystem();
	  miscInfo_p=getMiscInfo();
	  isCached_p=true;
	};
	//
	//------------------------------------------------------------------------
	//
	~ImageInformation() {};
	//
	//------------------------------------------------------------------------
	//
	void initPaths(const casacore::String& targetPath)
	{
	  coordSysRecFileName = targetPath+'/'+coordSysFileName;
	  imInfoRecFileName=targetPath+'/'+imInfoFileName;
	  miscInfoRecFileName=targetPath+'/'+miscInfoFileName;
	}
	//
	//------------------------------------------------------------------------
	//
	bool isInMemory() {return inMemory_p;}
	bool isPersistent() {return isPersistent_p;}
	//------------------------------------------------------------------------
	// Saving makes the object both in memory and persistent
	//
	void save(const casacore::String& targetPath);
	void save();
	void save(const casacore::CoordinateSystem& coordinates,
		  const casacore::IPosition& shape,
		  const casacore::Record& miscinfo);
	//
	//------------------------------------------------------------------------
	//
	casacore::CoordinateSystem getCoordinateSystem();
	casacore::Vector<int> getImShape();
	casacore::TableRecord getMiscInfo();
	//
	//------------------------------------------------------------------------
	//
	// inline double getPAValue() {getMiscInfo().get("ParallacticAngle", paVal);
	// miscinfo.get("MuellerElement", mVal);
	// miscinfo.get("WValue", wVal);
	// miscinfo.get("Xsupport", xSupport);
	// miscinfo.get("Ysupport", ySupport);
	// miscinfo.get("Sampling", sampling);
	// miscinfo.get("ConjFreq", conjFreq);
	// miscinfo.get("ConjPoln", conjPoln);
      private:
	casacore::ImageInterface<T> *cimg_p;
	casacore::String coordSysFileName,  coordSysKey;
	casacore::String imInfoFileName, imShapeKey;
	casacore::String miscInfoFileName,  miscInfoKey;
	casacore::String coordSysRecFileName, imInfoRecFileName, miscInfoRecFileName;
	bool inMemory_p,isPersistent_p,isCached_p;

	casacore::Vector<int> imShape_p;
	casacore::CoordinateSystem coordinateSystem_p;
	casacore::TableRecord miscInfo_p;
      };
    };
  };
};
#endif
