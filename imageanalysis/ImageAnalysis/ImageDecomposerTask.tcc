#include <imageanalysis/ImageAnalysis/ImageDecomposerTask.h>

#include <imageanalysis/ImageAnalysis/ImageDecomposer.h>

namespace casa {

template<class T> ImageDecomposerTask<T>::ImageDecomposerTask(
	const SPCIIT image, const casacore::Record *const region,
	const casacore::String& maskInp
) : ImageTask<T>(image, region, maskInp, "", false) {
	this->_construct();
}

template<class T> void ImageDecomposerTask<T>::setDeblendOptions(
	casacore::Double threshold, casacore::Int nContour, casacore::Int minRange, casacore::Int nAxis
) {
	ThrowIf(threshold < 0, "Threshold cannot be negative");
	_threshold = threshold;
	_ncontour = nContour;
	_minrange = minRange;
	_naxis = nAxis;
}

template<class T> void ImageDecomposerTask<T>::setFitOptions(
	casacore::Double maxrms, casacore::Int maxRetry, casacore::Int maxIter, casacore::Double convCriteria
) {
	_maxrms = maxrms;
	_maxretry = maxRetry;
	_maxiter = maxIter;
	_convcriteria = convCriteria;
}

template<class T> casacore::Matrix<T> ImageDecomposerTask<T>::decompose(
	casacore::Matrix<casacore::Int>& blcs, casacore::Matrix<casacore::Int>& trcs
) {
	casacore::AxesSpecifier axesSpec(false);
	auto subImage = SubImageFactory<T>::createSubImageRO(
		*this->_getImage(), *this->_getRegion(), this->_getMask(),
		this->_getLog().get(), axesSpec, this->_getStretch()
	);
	// Make finder
	ImageDecomposer<T> decomposer(*subImage);
	decomposer.setDeblend(! _simple);
	decomposer.setDeblendOptions(_threshold, _ncontour, _minrange, _naxis);
	decomposer.setFit(_fit);
	decomposer.setFitOptions(_maxrms, _maxretry, _maxiter, _convcriteria);
	decomposer.decomposeImage();
	decomposer.printComponents();
	casacore::Block<casacore::IPosition> blcspos(decomposer.numRegions());
	casacore::Block<casacore::IPosition> trcspos(decomposer.numRegions());
	decomposer.boundRegions(blcspos, trcspos);
	if(! blcspos.empty()) {
		auto n = blcspos.nelements();
		blcs.resize(n, blcspos[0].asVector().size());
		trcs.resize(n, trcspos[0].asVector().size());
		for (casacore::uInt k=0; k < n; ++k){
			blcs.row(k)=blcspos[k].asVector();
			trcs.row(k)=trcspos[k].asVector();
		}
	}
	// As yet no methods to put the results into an output container
	// (Note: component list can be output as a Matrix.)
	return decomposer.componentList();
}


}
