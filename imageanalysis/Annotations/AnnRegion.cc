#include <imageanalysis/Annotations/AnnRegion.h>

#include <casacore/casa/Exceptions/Error.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
//#include <imageanalysis/Regions/CasacRegionManager.h>
#include <imageanalysis/IO/ParameterParser.h>
#include <casacore/images/Regions/WCExtension.h>
#include <casacore/images/Regions/WCUnion.h>
#include <casacore/tables/Tables/TableRecord.h>

#include <iomanip>

using namespace casacore;
namespace casa {

const String AnnRegion::_class = "AnnRegion";

AnnRegion::AnnRegion(
	const Type shape, const String& dirRefFrameString,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Quantity& beginFreq,
	const Quantity& endFreq,
	const String& freqRefFrame,
	const String& dopplerString,
	const Quantity& restfreq,
	const Vector<Stokes::StokesTypes> stokes,
	const Bool annotationOnly,
	const Bool requireImageRegion
) : AnnotationBase(
		shape, dirRefFrameString, csys, beginFreq, endFreq,
		freqRefFrame, dopplerString, restfreq, stokes
	),
	_requireImageRegion(requireImageRegion),
	_isAnnotationOnly(annotationOnly),
	_isDifference(false), _constructing(true), _imShape(imShape),
	_spectralPixelRange(vector<Double>(0)) {
	_init();
	// just before returning
	_constructing = false;
}

AnnRegion::AnnRegion(
	const Type shape,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) :	AnnotationBase(shape, csys, stokes),
	_requireImageRegion(requireImageRegion),
	_isDifference(false), _constructing(true), _imShape(imShape),
	_spectralPixelRange(vector<Double>(0)) {
	_init();
	// just before returning
	_constructing = false;
}

AnnRegion::AnnRegion(const AnnRegion& other)
	: AnnotationBase(other), 
	  _requireImageRegion(other._requireImageRegion),
	  _imageRegion(other._imageRegion),
	  _directionRegion(other._directionRegion),
	  _isAnnotationOnly(other._isAnnotationOnly),
	  _isDifference(other._isDifference),
	  _constructing(other._constructing),
	  _imShape(other._imShape),
	  _spectralPixelRange(other._spectralPixelRange) {}


AnnRegion::~AnnRegion() {}

AnnRegion& AnnRegion::operator= (const AnnRegion& other) {
	if (&other != this) {
		AnnotationBase::operator= (other);
		_isAnnotationOnly = other._isAnnotationOnly;
		_requireImageRegion = other._requireImageRegion;
		_imageRegion = other._imageRegion;
		_directionRegion = other._directionRegion;
		_isDifference = other._isDifference;
		_constructing = other._constructing;
		_imShape.resize(other._imShape.size());
		_imShape = other._imShape;
		_spectralPixelRange = other._spectralPixelRange;
	}
	return *this;
}

Bool AnnRegion::operator== (const AnnRegion& other) const {
	return &other == this || (
		_isAnnotationOnly == other._isAnnotationOnly
		&& _requireImageRegion == other._requireImageRegion
		&& _imageRegion == other._imageRegion
		&& _directionRegion == other._directionRegion
		&& _isDifference == other._isDifference
		&& _constructing == other._constructing
		&& _imShape == other._imShape
		&& _spectralPixelRange == other._spectralPixelRange
	);
}


void AnnRegion::setAnnotationOnly(const Bool isAnnotationOnly) {
	_isAnnotationOnly = isAnnotationOnly;
}

Bool AnnRegion::isAnnotationOnly() const {
	return _isAnnotationOnly;
}

Bool AnnRegion::_hasDirectionRegion() {
	return (_directionRegion.isWCRegion() || _directionRegion.isLCRegion() || _directionRegion.isLCSlicer());
}

Bool AnnRegion::hasImageRegion() const {
	return (_imageRegion.isWCRegion() || _imageRegion.isLCRegion() || _imageRegion.isLCSlicer());
}

void AnnRegion::setDifference(const Bool difference) {
	_isDifference = difference;
}

Bool AnnRegion::isDifference() const {
	return _isDifference;
}

TableRecord AnnRegion::asRecord() const {
	return _imageRegion.toRecord("");
}

ImageRegion AnnRegion::asImageRegion() const {
	return _imageRegion;
}

CountedPtr<const WCRegion>  AnnRegion::getRegion() const {
	if (hasImageRegion()) {
		return _imageRegion.asWCRegionPtr()->cloneRegion();
	} else {
		return CountedPtr<const WCRegion>(nullptr);
	}
}

std::shared_ptr<const WCRegion>  AnnRegion::getRegion2() const {
	if (hasImageRegion()) {
		return std::shared_ptr<const WCRegion>(_imageRegion.asWCRegionPtr()->cloneRegion());
	} else {
		return std::shared_ptr<const WCRegion>(nullptr);
	}
}

Bool AnnRegion::isRegion() const {
	return true;
}

void AnnRegion::_init() {
	if (_imShape.nelements() != getCsys().nPixelAxes()) {
		ostringstream oss;
		oss << _class << "::" << __FUNCTION__ << ": Number of coordinate axes ("
			<< getCsys().nPixelAxes() << ") differs from number of dimensions in image shape ("
			<< _imShape.nelements() << ")";
		throw AipsError(oss.str());
	}

}

Bool AnnRegion::setFrequencyLimits(
	const Quantity& beginFreq,
	const Quantity& endFreq,
	const String& freqRefFrame,
	const String& dopplerString,
	const Quantity& restfreq
) {
	if (
		AnnotationBase::setFrequencyLimits(
			beginFreq, endFreq, freqRefFrame, dopplerString, restfreq
		)
	) {
		if (! _constructing) {
			// have to re-extend the direction region because of new freq range.
			// but not during object construction
			_extend();
		}
		return true;
	}
	else {
		return false;
	}
}

void AnnRegion::_setDirectionRegion(const ImageRegion& region) {
	_directionRegion = region;
}


vector<Double> AnnRegion::getSpectralPixelRange() const {
	return _spectralPixelRange;
}


void AnnRegion::_extend() {
	if (!_hasDirectionRegion()) {
		return; // nothing to extend
	}
	Int stokesAxis = -1;
	Int spectralAxis = -1;
	Vector<Quantity> freqRange;
	uInt nBoxes = 0;
	Vector<MFrequency> freqLimits = getFrequencyLimits();
	const CoordinateSystem& csys = getCsys();
	if (csys.hasSpectralAxis() && freqLimits.size() == 2) {
		SpectralCoordinate spcoord = csys.spectralCoordinate();
		spectralAxis = csys.spectralAxisNumber();

		if (spectralAxis < 0) {
			throw AipsError(
				String(__FUNCTION__) + ": A spectral range was specified "
				+ "but the spectral pixel axis in the supplied coordinate "
				+ "system is not present."
			);
		}

		String unit = spcoord.worldAxisUnits()[0];
		_spectralPixelRange.resize(2);
		spcoord.toPixel(_spectralPixelRange[0], freqLimits[0]);
		spcoord.toPixel(_spectralPixelRange[1], freqLimits[1]);
		freqRange.resize(2);
		freqRange[0] = freqLimits[0].get(unit);
		freqRange[1] = freqLimits[1].get(unit);
		if (_spectralPixelRange[1] <= _spectralPixelRange[0]) {
			std::swap(_spectralPixelRange[0], _spectralPixelRange[1]);
			std::swap(freqRange[0], freqRange[1]);
		}
		nBoxes = 1;
	}
	vector<Stokes::StokesTypes> stokesRanges;
	Vector<Stokes::StokesTypes> stokes = getStokes();
	if (
		csys.hasPolarizationCoordinate() && stokes.size() > 0
		&& (stokesAxis = csys.polarizationAxisNumber()) >= 0
	) {
		vector<uInt> stokesNumbers(2*stokes.size());
		for (uInt i=0; i<stokes.size(); i++) {
			stokesNumbers[2*i] = (uInt)stokes[i];
			stokesNumbers[2*i + 1] = stokesNumbers[2*i];
		}
        uInt nSel = 0;
		vector<uInt> orderedRanges = ParameterParser::consolidateAndOrderRanges(
			nSel, stokesNumbers
		);
		for (uInt i=0; i<orderedRanges.size(); i++) {
			stokesRanges.push_back(Stokes::type(orderedRanges[i]));
		}
		nBoxes = stokesRanges.size()/2;
	}
	if (nBoxes == 0) {
		_imageRegion = _directionRegion;
	}
	else {
		uInt nExtendAxes = 0;
		if (spectralAxis >= 0) {
			nExtendAxes++;
		}
		if (stokesAxis >= 0) {
			nExtendAxes++;
		}

		IPosition pixelAxes(nExtendAxes);
		uInt n = 0;
		// spectral axis must be first to be consistent with _makeExtensionBox()
		if (spectralAxis > 0) {
			pixelAxes[n] = spectralAxis;
			n++;
		}
		if (stokesAxis > 0) {
			pixelAxes[n] = stokesAxis;
			n++;
		}
		if (nBoxes == 1) {
            Vector<Stokes::StokesTypes> const stokesRangesV(stokesRanges);
			WCBox wbox = _makeExtensionBox(freqRange, stokesRangesV, pixelAxes);
			_imageRegion = ImageRegion(WCExtension(_directionRegion, wbox));
		}
		else {
			PtrBlock<const WCRegion*> regions(nBoxes);
			for (uInt i=0; i<nBoxes; i++) {
				Vector<Stokes::StokesTypes> stokesRange(2);
				stokesRange[0] = stokesRanges[2*i];
				stokesRange[1] = stokesRanges[2*i + 1];
				WCBox wbox = _makeExtensionBox(freqRange, stokesRange, pixelAxes);
				regions[i] = new WCExtension(_directionRegion, wbox);
			}
			_imageRegion = ImageRegion(WCUnion(true, regions));
		}
	}
	try {
		_imageRegion.asWCRegionPtr()->toLCRegion(csys, _imShape);
	}
	catch (const AipsError& x) {
		throw (ToLCRegionConversionError(x.getMesg()));
	}
}

WCBox AnnRegion::_makeExtensionBox(
	const Vector<Quantity>& freqRange,
	const Vector<Stokes::StokesTypes>& stokesRange,
	const IPosition& pixelAxes
) const {
	uInt n = 0;
	Vector<Quantity> blc(pixelAxes.size());
	Vector<Quantity> trc(pixelAxes.size());
	Vector<Int> absRel(pixelAxes.size(), RegionType::Abs);
	if (freqRange.size() == 2) {
		blc[n] = freqRange[0];
		trc[n] = freqRange[1];
		n++;
	}
	if (stokesRange.size() == 2) {
		blc[n] = Quantity(stokesRange[0], "");
		trc[n] = Quantity(stokesRange[1], "");
	}
	WCBox wbox(blc, trc, pixelAxes, getCsys(), absRel);
	return wbox;
}

Quantity AnnRegion::_lengthToAngle(
	const Quantity& quantity, const uInt pixelAxis
) const {
	if(quantity.getUnit() == "pix") {
		return getCsys().toWorldLength(quantity.getValue(), pixelAxis);
	}
	else if (! quantity.isConform("rad")) {
		throw AipsError (
			"Quantity " + String::toString(quantity)
			+ " is not an angular measure nor is it in pixel units."
		);
	}
	else {
		return quantity;
	}
}

void AnnRegion::_printPrefix(ostream& os) const {
	if (isAnnotationOnly()) {
		os << "ann ";
	}
	else if (isDifference()) {
		os << "- ";
	}
}

}

