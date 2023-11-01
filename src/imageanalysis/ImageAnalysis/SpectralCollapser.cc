//# SpectralCollapser.cc: Implementation of class SpectralCollapser
//# Copyright (C) 1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: $

#include <imageanalysis/ImageAnalysis/SpectralCollapser.h>
#include <imageanalysis/ImageAnalysis/ImageCollapser.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/OS/RegularFile.h>
#include <casacore/casa/OS/SymLink.h>
#include <casacore/coordinates/Coordinates/QualityCoordinate.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <imageanalysis/ImageAnalysis/ImageMoments.h>
#include <casacore/images/Images/FITSImage.h>
#include <casacore/images/Images/FITSQualityImage.h>
#include <casacore/images/Images/MIRIADImage.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/lattices/LRegions/LCSlicer.h>

#include <imageanalysis/ImageAnalysis/ImageMoments.h>

using namespace casacore;
namespace casa {
SpectralCollapser::SpectralCollapser(const SPCIIF image):
		_image(image), _log(new LogIO()), _storePath(""){
	_setUp();
}

SpectralCollapser::SpectralCollapser(const SPCIIF image, const String storePath):
		_image(image), _log(new LogIO()), _storePath(storePath){
	_setUp();
}

SpectralCollapser::~SpectralCollapser(){delete _log;}

Bool SpectralCollapser::collapse(const Vector<Float> &specVals, const Float startVal, const Float endVal,
		const String &unit, const SpectralCollapser::CollapseType &collType, const SpectralCollapser::CollapseError &collError, String &outname, String &msg){

	//Bool ok;
	String unit_(unit);
	String outnameData;
	String outnameError;

	*_log << LogOrigin("SpectralCollapser", "collapse");

	if (specVals.size() < 1){
		msg = String("No spectral values provided!");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return false;
	}

	// in the unit, replace a "/" with "_p_"
	if (unit_.find(String("/"))!=String::npos)	{
		String::size_type slashPos=unit_.find(String("/"));
		unit_.replace(slashPos, 1, String("_p_"));
	}

	Bool ascending=true;
	if (specVals(specVals.size()-1)<specVals(0))
		ascending=false;

	Int startIndex, endIndex;
	if (ascending){
		if (endVal < specVals(0)){
			msg = String("Start value: ") + String::toString(endVal) + String(" is smaller than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}
		if (startVal > specVals(specVals.size()-1)){
			msg = String("End value: ") + String::toString(startVal) + String(" is larger than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}
		startIndex=0;
		while (specVals(startIndex)<startVal)
			startIndex++;

		endIndex=specVals.size()-1;
		while (specVals(endIndex)>endVal)
			endIndex--;
	}
	else {
		if (endVal < specVals(specVals.size()-1)){
			msg = String("Start value: ") + String::toString(endVal) + String(" is smaller than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}
		if (startVal > specVals(0)){
			msg = String("End value: ") + String::toString(startVal) + String(" is larger than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}
		startIndex=0;
		while (specVals(startIndex)>endVal)
			startIndex++;

		endIndex=specVals.size()-1;
		while (specVals(endIndex)<startVal)
			endIndex--;
	}

	String chanInp;
	chanInp = String::toString(startIndex) + "~" + String::toString(endIndex);

	String wcsInp;
	wcsInp = String::toString(startVal) + "~" + String::toString(endVal) + unit_;

	if (startIndex > endIndex){
		msg = String("Spectral window ") + wcsInp + String(" too narrow!");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return false;
	}

	String dataAggStr, errorAggStr;
	_collTypeToImCollString(collType,   dataAggStr);
	_collErrorToImCollString(collError, errorAggStr);

	// for ImageMoments:
	//Vector<Int> momentVec;
	//collapseTypeToVector(collType, momentVec);

	_getOutputName(wcsInp, outname, outnameData, outnameError);

	if (_hasQualAxis){
		std::shared_ptr<SubImage<Float> > subData;
		std::shared_ptr<SubImage<Float> > subError;
		if (!_getQualitySubImgs(_image, subData, subError)){
			msg = String("Can not split image: ") + _image->name(true) + String(" to data and error array!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}

		switch (collError)
		{
		case SpectralCollapser::PNOERROR:
			// collapse the data
			//ok = _collapse(subData, dataAggStr, chanInp, outname);
			_collapse(subData, dataAggStr, chanInp, outname);
			break;
		case SpectralCollapser::PERMSE:
			// collapse the data
			//ok = _collapse(subData, dataAggStr, chanInp, outnameData);
			_collapse(subData, dataAggStr, chanInp, outnameData);

			// collapse the error
			//ok = _collapse(subData, errorAggStr, chanInp, outnameError);
			_collapse(subData, errorAggStr, chanInp, outnameError);

			// merge the data and the error
			//ok = _mergeDataError(outname, outnameData, outnameError);
			_mergeDataError(outname, outnameData, outnameError);

			// merge the data and the error
			//ok = _cleanTmpData(outnameData, outnameError);
			_cleanTmpData(outnameData, outnameError);

			break;
		case SpectralCollapser::PPROPAG:
			// collapse the data
			//ok = _collapse(subData, dataAggStr, chanInp, outnameData);
			_collapse(subData, dataAggStr, chanInp, outnameData);

			// collapse the error
			//ok = _collapse(subError, errorAggStr, chanInp, outnameError);
			_collapse(subError, errorAggStr, chanInp, outnameError);

			// merge the data and the error
			//ok = _mergeDataError(outname, outnameData, outnameError, Float(endIndex-startIndex));
			_mergeDataError(outname, outnameData, outnameError, Float(endIndex-startIndex));

			// remove the tmp-images
			//ok = _cleanTmpData(outnameData, outnameError);
			_cleanTmpData(outnameData, outnameError);

			break;
		default:
			// this should not happen
			*_log << LogIO::EXCEPTION << "The error type does not fit!" << LogIO::POST;

		}

		// for ImageMoments:
		//ok = _moments(&subData, momentVec, startIndex,  endIndex, outname);
	}
	else {
		switch (collError)
		{
		case SpectralCollapser::PNOERROR:
			// collapse the data
			//ok = _collapse(_image, dataAggStr, chanInp, outname);
			_collapse(_image, dataAggStr, chanInp, outname);
			break;
		case SpectralCollapser::PERMSE:
			// collapse the data
			//ok = _collapse(_image, dataAggStr, chanInp, outnameData);
			_collapse(_image, dataAggStr, chanInp, outnameData);

			// collapse the error
			//ok = _collapse(_image, errorAggStr, chanInp, outnameError);
			_collapse(_image, errorAggStr, chanInp, outnameError);

			// merge the data and the error
			//ok = _mergeDataError(outname, outnameData, outnameError);
			_mergeDataError(outname, outnameData, outnameError);

			// merge the data and the error
			//ok = _cleanTmpData(outnameData, outnameError);
			_cleanTmpData(outnameData, outnameError);
			break;
		default:
			// this should not happen
			*_log << LogIO::EXCEPTION << "The error type does not fit!" << LogIO::POST;
		}
		// for ImageMoments:
		//ok = _moments(_image, momentVec, startIndex,  endIndex, outname);
	}

	_addMiscInfo(outname, wcsInp, chanInp, collType, collError);
	msg = String("Collapsed image: ") + outname;

	return true;
}

String SpectralCollapser::summaryHeader() const {
	ostringstream os;
	os << "Input parameters ---" << endl;
	os << "       --- imagename:           " << _image->name() << endl;
	os << "       --- storage path:        " << _storePath << endl;
	os << "       --- spectral axis:       " << _specAxis << endl;
	os << "       --- quality axis:        " << _hasQualAxis << endl;
	//os << "       --- channels:            " << _chan << endl;
	//os << "       --- stokes:              " << _stokesString << endl;
	//os << "       --- mask:                " << _mask << endl;
	return os.str();
}

void SpectralCollapser::stringToCollapseType(const String &text, SpectralCollapser::CollapseType &collType){
	if (!text.compare(String("mean")))
		collType = SpectralCollapser::PMEAN;
	else if (!text.compare(String("median")))
		collType = SpectralCollapser::PMEDIAN;
	else if (!text.compare(String("sum")))
		collType = SpectralCollapser::PSUM;
	else
		collType = SpectralCollapser::CUNKNOWN;
}

void SpectralCollapser::stringToCollapseError(const String &text, SpectralCollapser::CollapseError &collError){
	if (!text.compare(String("no error")))
		collError = SpectralCollapser::PNOERROR;
	else if (!text.compare(String("rmse")))
		collError = SpectralCollapser::PERMSE;
	else if (!text.compare(String("propagated")))
		collError = SpectralCollapser::PPROPAG;
	else
		collError = SpectralCollapser::EUNKNOWN;
}

void SpectralCollapser::collapseTypeToString(const SpectralCollapser::CollapseType &collType, String &strCollType){
	switch (collType)
	{
	case SpectralCollapser::PMEAN:
		strCollType=String("mean");
		break;
	case SpectralCollapser::PMEDIAN:
		strCollType=String("median");
		break;
	case SpectralCollapser::PSUM:
		strCollType=String("sum");
		break;
	case SpectralCollapser::CUNKNOWN:
		strCollType=String("unknown");
		break;
	default:
		strCollType=String("No Idea!");
	}
}

void SpectralCollapser::collapseErrorToString(const SpectralCollapser::CollapseError &collError, String &strCollError){
	switch (collError)
	{
	case SpectralCollapser::PNOERROR:
		strCollError=String("noerror");
		break;
	case SpectralCollapser::PERMSE:
		strCollError=String("rmse");
		break;
	case SpectralCollapser::PPROPAG:
		strCollError=String("propagated");
		break;
	case SpectralCollapser::EUNKNOWN:
		strCollError=String("unknown");
		break;
	default:
		strCollError=String("No Idea!");
	}
}

void SpectralCollapser::_collTypeToImCollString(const SpectralCollapser::CollapseType &collType, String &strCollType) const {
	switch (collType)
	{
	case SpectralCollapser::PMEAN:
		strCollType=String("mean");
		break;
	case SpectralCollapser::PMEDIAN:
		strCollType=String("median");
		break;
	case SpectralCollapser::PSUM:
		strCollType=String("sum");
		break;
	case SpectralCollapser::CUNKNOWN:
		strCollType=String("unknown");
		break;
	default:
		strCollType=String("No Idea!");
	}
}

void SpectralCollapser::_collErrorToImCollString(const SpectralCollapser::CollapseError &collError, String &strCollError) const {
	switch (collError)
	{
	case SpectralCollapser::PNOERROR:
		strCollError=String("noerror");
		break;
	case SpectralCollapser::PERMSE:
		strCollError=String("variance");
		break;
	case SpectralCollapser::PPROPAG:
		strCollError=String("sum");
		break;
	case SpectralCollapser::EUNKNOWN:
		strCollError=String("unknown");
		break;
	default:
		strCollError=String("No Idea!");
	}
}

// for ImageMoments:
void SpectralCollapser::collapseTypeToVector(const SpectralCollapser::CollapseType &collType, Vector<Int> &momentVec){
	momentVec.resize(1);
	switch (collType)
	{
	case SpectralCollapser::PMEAN:
		momentVec(0) = MomentsBase<Float>::AVERAGE;
		break;
	case SpectralCollapser::PMEDIAN:
		momentVec(0) = MomentsBase<Float>::MEDIAN;
		break;
	case SpectralCollapser::PSUM:
		momentVec(0) = MomentsBase<Float>::INTEGRATED;
		break;
	case SpectralCollapser::CUNKNOWN:
		momentVec(0) = MomentsBase<Float>::DEFAULT ;
		break;
	default:
		momentVec(0) = MomentsBase<Float>::DEFAULT ;
	}
}

void SpectralCollapser::_setUp(){
	*_log << LogOrigin("SpectralCollapser", "_setUp");

	_all = CasacRegionManager::ALL;

	// get the pixel axis number of the spectral axis
	CoordinateSystem cSys = _image->coordinates();
	Int specAx = cSys.findCoordinate(Coordinate::SPECTRAL);
	if (specAx < 0){
		specAx = cSys.findCoordinate(Coordinate::TABULAR);
		if ( specAx < 0 ){
			*_log << LogIO::WARN << "No spectral axis in image: " << _image->name() << LogIO::POST;
			return;
		}
	}
	Vector<Int> nPixelSpec = cSys.pixelAxes(specAx);
	_specAxis = IPosition(1, nPixelSpec(0));

	// check for a quality axis
	_hasQualAxis = (cSys.findCoordinate(Coordinate::QUALITY) < 0) ? false : true;
}

Bool SpectralCollapser::_cleanTmpData(const String &tmpData, const String &tmpError) const {
	// remove the tmp-data
	Bool okData = _cleanTmpData(tmpData);

	// remove the tmp-error
	Bool okError = _cleanTmpData(tmpError);

	return (okData&&okError);
}

Bool SpectralCollapser::_cleanTmpData(const String &tmpFileName) const {
	// get the tmp-data file
	Path tmpFilePath(tmpFileName);
	File tmpFile(tmpFilePath);

	// check its existence
	if (tmpFile.exists ()){
		// delete it as file
		if (tmpFile.isRegular() && tmpFile.isWritable()){
			RegularFile tmpRegFile(tmpFilePath);
			tmpRegFile.remove();
		}
		// delete it as directory
		else if (tmpFile.isWritable()){
			Directory tmpDir(tmpFilePath);
			tmpDir.removeRecursive(false);
		}
		else{
			*_log << LogIO::EXCEPTION << "Can not remove the tmp-image: " << tmpFilePath.absoluteName() << LogIO::POST;
		}
	}
	return true;
}

Bool SpectralCollapser::_getQualitySubImg(const ImageInterface<Float>* image, const Bool &getData, SubImage<Float> &qualitySub){
	Int specAx = (image->coordinates()).findCoordinate(Coordinate::QUALITY);
	Vector<Int> nPixelQual = (image->coordinates()).pixelAxes(specAx);
	uInt nAxisQual=nPixelQual(0);

	// build the appropriate slicer
	IPosition startPos(image->ndim(), 0);
	IPosition lengthPos=image->shape();
	if (!getData)
		startPos(nAxisQual) = 1;
	lengthPos(nAxisQual) = 1;
	Slicer subSlicer(startPos, lengthPos, Slicer::endIsLength);

	qualitySub = SubImage<Float>(*image, subSlicer, AxesSpecifier(false));

	return true;
}

Bool SpectralCollapser::_getQualitySubImgs(SPCIIF image, std::shared_ptr<SubImage<Float> > &subData, std::shared_ptr<SubImage<Float> > &subError) const{

	// check whether the image origin is FITS
	const FITSQualityImage  * const qImg = dynamic_cast<const FITSQualityImage*const>(image.get());
	if (qImg){
		// create the data image from the FITS data extension
		subData.reset(new SubImage<Float>(*(qImg->fitsData())));

		// create the error image from the FITS error extension
		subError.reset(new SubImage<Float>(*(qImg->fitsError())));
	}
	else{
		// get the coordinate system and the
		// info on the quality axis
		Int dataIndex, errorIndex;
		CoordinateSystem qSys = image->coordinates();
		Int qualAx = qSys.findCoordinate(Coordinate::QUALITY);
		Vector<Int> nPixelQual = qSys.pixelAxes(qualAx);
		uInt nAxisQual=nPixelQual(0);

		// get the data and the error index
		(qSys.qualityCoordinate(qualAx)).toPixel(dataIndex,  Quality::DATA);
		(qSys.qualityCoordinate(qualAx)).toPixel(errorIndex, Quality::ERROR);

		// build the slicer for the data and error
		IPosition startPos(image->ndim(), 0);
		IPosition lengthPos=image->shape();
		startPos(nAxisQual) = dataIndex;
		lengthPos(nAxisQual)= 1;
		Slicer sliceData(startPos, lengthPos, Slicer::endIsLength);
		startPos(nAxisQual) = errorIndex;
		Slicer sliceError(startPos, lengthPos, Slicer::endIsLength);

		// create an axis specifier that removes
		// only the degenerated quality axis
		IPosition iposKeep(lengthPos.size(), 1);
		iposKeep(nAxisQual) = 0;
		AxesSpecifier axSpec(iposKeep);

		// create the data sub-image
		subData.reset(new SubImage<Float>(*image, sliceData, axSpec));

		// create the error sub-image
		subError.reset(new SubImage<Float>(*image, sliceError, axSpec));
	}
	return true;
}

Bool SpectralCollapser::_getOutputName(const String &wcsInp, String &outImg, String &outImgData, String &outImgError)const{
	Path   imgPath(_image->name());
	Path   collImgName(_storePath);
	String imgName(imgPath.baseName());

	*_log << LogOrigin("SpectralCollapser", "_getOutputName");

	// check that the storage path is OK
	if (!collImgName.isValid ())
		*_log << LogIO::EXCEPTION << "Not a valid storage path: " << collImgName.absoluteName() << LogIO::POST;

	// strip ".fits" or ".img"
	if ((int)imgName.find(".fits") > -1)
		imgName = imgName(0, imgName.find(".fits"));
	else if ((int)imgName.find(".img") > -1)
		imgName = imgName(0, imgName.find(".img"));

	// build a new name
	imgName += "_" + wcsInp;
	collImgName.append(imgName);
	File imgFile(collImgName);
	File imgFileData(collImgName.absoluteName()+String("DATA"));
	File imgFileError(collImgName.absoluteName()+String("ERROR"));

	// make sure the name is unique
	Int index=1;
	while (imgFile.exists() || imgFileData.exists() || imgFileError.exists()){
		imgFile = File(collImgName.absoluteName() + "(" + String::toString(index)+ ")");
		imgFileData = File(collImgName.absoluteName() + "DATA(" + String::toString(index)+ ")");
		imgFileError = File(collImgName.absoluteName() + "ERROR(" + String::toString(index)+ ")");
		index++;
	}

	// feed the names back
	outImg = (imgFile.path()).absoluteName();
	outImgData  = (imgFileData.path()).absoluteName();
	outImgError = (imgFileError.path()).absoluteName();

	return true;
}

Bool SpectralCollapser::_collapse(const SPCIIF image, const String &aggString,
		const String& chanInp, const String& outname) const {
	CasacRegionManager rm(image->coordinates());
	String diagnostics;
	uInt nSelectedChannels;
	String stokes = _all;
	Record myreg = rm.fromBCS(
		diagnostics, nSelectedChannels, stokes, 0, "", chanInp,
		CasacRegionManager::USE_ALL_STOKES, "", image->shape(), "", false
	);
	// create and execute the imcollapse-class
	ImageCollapser<Float> collapser(
			aggString,                  // String aggString
			image,                      // const ImageInterface<Float> *const image
			&myreg,                          // const Record *const regionRec
			"",                         // const String& maskInp
			_specAxis,                  // const IPosition& axes
			false,                      // do not invert axes selection
			outname,                    // String& outname
			true                        // const Bool overwrite
		);
		collapser.collapse();
		return true;
}

Bool SpectralCollapser::_moments(const ImageInterface<Float> *image, const Vector<Int> &momentVec,
		const Int & startIndex, const Int &endIndex, const String& outname){
   IPosition blc(image->ndim(),0);
   IPosition trc=image->shape()-1;
   blc(_specAxis(0))=(uInt)startIndex;
   trc(_specAxis(0))=(uInt)endIndex;
   const LCSlicer region(blc, trc);
   //cout << "before getregion()" << endl;
   //ImageRegion mask = fitsImage.getRegion(fitsImage.getDefaultMask(), RegionHandler::Masks);
   //cout << "after getregion()" << endl;
   SubImage<Float> subImage(*image, ImageRegion(region));
   ImageMoments<Float> moment(subImage, *_log, true, true);
   if (!moment.setMoments(momentVec)) {
   	*_log << LogIO::SEVERE << moment.errorMessage() << LogIO::POST;
      return false;
   }
   try {
	   moment.setMomentAxis(_specAxis(0));
   }
   catch (const AipsError& exc) {
	   String errorMsg = exc.getMesg();
	   *_log << LogIO::SEVERE << exc.getMesg() << LogIO::POST;
	   return false;
   }
   std::vector<std::shared_ptr<MaskedLattice<Float> > > images;
   try {
	   images = moment.createMoments(false, outname, false);
   }
   catch (const AipsError& exc) {
	   *_log << LogIO::SEVERE << exc.getMesg() << LogIO::POST;
	   return false;
   }
	for (uInt i=0; i<images.size(); i++) {
		cout << "out shape: " << images[i]->shape() << endl;
		//delete images[i];
	}

   //pSubImage2 = new SubImage<Float>(subImage, ImageRegion(region));


		return true;
}

Bool SpectralCollapser::_mergeDataError( const String &outImg, const String &dataImg, const String &errorImg, const Float &normError) const {

	// open the data and the error image
	ImageInterface<Float>  *data  = new PagedImage<Float>(dataImg, TableLock::AutoNoReadLocking);
	ImageInterface<Float>  *error = new PagedImage<Float>(errorImg, TableLock::AutoNoReadLocking);

	// create the tiled shape for the output image
	IPosition newShape=IPosition(data->shape());
	newShape.append(IPosition(1, 2));
	TiledShape tiledShape(newShape);

	// create the coordinate system for the output image
	CoordinateSystem newCSys = data->coordinates();
	Vector<Int> quality(2);
	quality(0) = Quality::DATA;
	quality(1) = Quality::ERROR;
	QualityCoordinate qualAxis(quality);
	newCSys.addCoordinate(qualAxis);

	Array<Float> outData=Array<Float>(newShape, 0.0f);
	Array<Bool>  outMask;

	// get all the data values
	Array<Float> inData, inError;
	Array<Bool> inDataMask, inErrorMask;
	Slicer inSlicer(IPosition((data->shape()).size(), 0), IPosition(data->shape()));
	data->doGetSlice(inData, inSlicer);

	// define in the output array
	// the slicers for data and error
	Int qualCooPos, qualIndex;
	qualCooPos = newCSys.findCoordinate(Coordinate::QUALITY);
	(newCSys.qualityCoordinate(qualCooPos)).toPixel(qualIndex, Quality::DATA);
	IPosition outStart(newShape.size(), 0);
	outStart(newShape.size()-1)=qualIndex;
	IPosition outLength(newShape);
	outLength(newShape.size()-1)=1;
	Slicer outDataSlice(outStart, outLength);
	(newCSys.qualityCoordinate(qualCooPos)).toPixel(qualIndex, Quality::ERROR);
	outStart(newShape.size()-1)=qualIndex;
	Slicer outErrorSlice(outStart, outLength);

	// get all the error values
	error->doGetSlice(inError, inSlicer);

	// check whether a mask is necessary
	if (data->hasPixelMask() || error->hasPixelMask()){
		// create the output mask
		outMask=Array<Bool>(newShape, true);

		// make the mask for the data values
		if (data->hasPixelMask()){
			inDataMask  = (data->pixelMask()).get();
		}
		else{
			inDataMask = Array<Bool>(data->shape(), true);
		}

		// make the mask for the error values
		if (error->hasPixelMask()){
			inErrorMask  = (error->pixelMask()).get();
		}
		else{
			inErrorMask = Array<Bool>(error->shape(), true);
		}
	}

	// normalise the error
	// TODO: check whether for masked arrays there are problems
	if (normError==0.0){
		if (inErrorMask.ndim() > 0){
			inErrorMask = false;
		}
		else{
			outMask=Array<Bool>(newShape, true);

			inDataMask  = Array<Bool>(data->shape(), true);
			inErrorMask = Array<Bool>(error->shape(), false);
		}
	}
	else if (normError>1.0){
		inError = inError / (normError*normError);
	}


	// add the data and error values to the output array
	outData(outDataSlice)  = inData.addDegenerate(1);
	outData(outErrorSlice) = inError.addDegenerate(1);

	// check whether there is a mask
	if (inDataMask.ndim() > 0 && inErrorMask.ndim() > 0){
		// add the data and error mask to the output
		outMask(outDataSlice)  = inDataMask.addDegenerate(1);
		outMask(outErrorSlice) = inErrorMask.addDegenerate(1);
	}

   // write out the combined image
	ImageUtilities::writeImage(tiledShape, newCSys, outImg, outData, *_log, outMask);

	delete data;
	delete error;
	return true;
}


void SpectralCollapser::_addMiscInfo(const String &outName, const String &wcsInput, const String &chanInput,
		const SpectralCollapser::CollapseType &collType, const SpectralCollapser::CollapseError &collError) const {
	ImageInterface<Float>  *outImg  = new PagedImage<Float>(outName, TableLock::AutoNoReadLocking);

	// get the collapse type and error type as strings
	String strCollType, strCollError;
	SpectralCollapser::collapseTypeToString(collType, strCollType);
	SpectralCollapser::collapseErrorToString(collError, strCollError);

	// compile and set the miscInfo
	TableRecord miscInfo=outImg->miscInfo();
	miscInfo.define("inimage", _image->name(true));
	miscInfo.setComment("inimage", "name input image");
	miscInfo.define("specsel", wcsInput);
	miscInfo.setComment("specsel", "spectral selection");
	miscInfo.define("chansel", chanInput);
	miscInfo.setComment("chansel", "channel selection");
	miscInfo.define("colltype", strCollType);
	miscInfo.setComment("colltype", "collapse type");
	miscInfo.define("collerr", strCollError);
	miscInfo.setComment("collerr", "collapse error");
	miscInfo.define("origin", "CASA viewer / 2D recombination");
	miscInfo.setComment("origin", "origin of the image");

	// put to miscInfo
	outImg->setMiscInfo(miscInfo);

   delete outImg;
}
}

