#include <imageanalysis/ImageAnalysis/ImageMaxFitter.h>

#include <imageanalysis/ImageAnalysis/ImageSourceFinder.h>


namespace casa {

template<class T> ImageMaxFitter<T>::ImageMaxFitter(
	SPCIIT image, const casacore::Record *const &region
) : ImageTask<T>(
		image, "", region, "", "", "", "", "", false
	) {
	this->_construct();
}

template<class T> casacore::Record ImageMaxFitter<T>::fit(
	casacore::Bool doPoint, casacore::Int width, casacore::Bool absFind, casacore::Bool list
) const {
	casacore::Vector<casacore::Double> absPixel;
	casacore::AxesSpecifier axesSpec(false);
	casacore::String mask;
	auto subImage = SubImageFactory<T>::createSubImageRO(
		*this->_getImage(), *this->_getRegion(), mask,
		this->_getLog().get(), axesSpec
	);
	*this->_getLog() << casacore::LogOrigin(getClass(), __func__);
	ImageSourceFinder<T> sf(subImage, nullptr, "");
	sf.setCutoff(0.1);
	sf.setAbsFind(absFind);
	sf.setDoPoint(doPoint);
	sf.setWidth(width);
	//auto sky = sf.findSourceInSky(*this->_getLog(), absPixel, cutoff, absFind, doPoint, width);
	auto sky = sf.findSourceInSky(absPixel);
	// modify to show dropped degenerate axes values???
	if (list) {
		*this->_getLog() << casacore::LogIO::NORMAL << "Brightness     = " << sky.flux().value()
			<< " " << sky.flux().unit().getName() << casacore::LogIO::POST;
		const auto& cSys = subImage->coordinates();
		*this->_getLog() << "World Axis Units: " << cSys.worldAxisUnits() << casacore::LogIO::POST;
		casacore::Vector<casacore::Double> wPix;
		if (!cSys.toWorld(wPix, absPixel)) {
			*this->_getLog() << casacore::LogIO::WARN
				<< "Failed to convert to absolute world coordinates "
				<< cSys.errorMessage() << casacore::LogIO::POST;
		}
		else {
			*this->_getLog() << "Absolute world = " << wPix << casacore::LogIO::POST;
		}
		casacore::Vector<casacore::Double> wRel = wPix.copy();
		cSys.makeWorldRelative(wRel);
		*this->_getLog() << "Relative world = " << wRel << casacore::LogIO::POST;
		*this->_getLog() << casacore::LogIO::NORMAL << "Absolute pixel = " << absPixel << endl;
		casacore::Vector<casacore::Double> pRel = absPixel.copy();
		cSys.makePixelRelative(pRel);
		*this->_getLog() << "Relative pixel = " << pRel << casacore::LogIO::POST;
	}
	ComponentList mycomp;
	mycomp.add(sky);
	casacore::String error;
	casacore::Record outrec;
	ThrowIf(
		! mycomp.toRecord(error, outrec),
		"Cannot convert SkyComponent to output record"
	);
	return outrec;
}

}
