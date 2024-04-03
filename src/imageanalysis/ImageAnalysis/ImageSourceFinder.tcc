//# ImageSourceFinder.cc:  find sources
//# Copyright (C) 1995-2003,2004
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
//# $Id: ImageSourceFinder.tcc 19940 2007-02-27 05:35:22Z Malte.Marquarding $
//
#include <imageanalysis/ImageAnalysis/ImageSourceFinder.h>

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/casa/Arrays/AxesSpecifier.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/SkyComponentFactory.h>
#include <casacore/scimath/Fitting/LSQaips.h>
#include <casacore/scimath/Fitting/NonLinearFitLM.h>
#include <casacore/lattices/LatticeMath/Fit2D.h>
#include <casacore/scimath/Functionals/Gaussian2D.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageInfo.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/lattices/LatticeMath/LatticeStatistics.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/scimath/Mathematics/AutoDiff.h> 
#include <casacore/scimath/Mathematics/NumericTraits.h> 
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/casa/Quanta/Unit.h>
#include <casacore/casa/Utilities/COWPtr.h>
#include <casacore/casa/BasicMath/Math.h>
#include <iostream>


namespace casa {

template <class T> ImageSourceFinder<T>::ImageSourceFinder(
	SPCIIT image, const casacore::Record *const region, const casacore::String& mask
) : ImageTask<T>(image, region, mask, "", false) {
	this->_construct();
}

template <class T> 
ImageSourceFinder<T>::~ImageSourceFinder () {}

template <class T>
ComponentList ImageSourceFinder<T>::findSources (casacore::Int nMax) {
   return _findSources(nMax);
}

template <class T>
SkyComponent ImageSourceFinder<T>::findSourceInSky (
	casacore::Vector<casacore::Double>& absPixel
) {

	// Find sky
	casacore::Int dC;
	casacore::String errorMessage;
	casacore::Vector<casacore::Int> pixelAxes, worldAxes;
	auto subImage = SubImageFactory<T>::createSubImageRO(
		*this->_getImage(), *this->_getRegion(), this->_getMask(),
		this->_getLog().get(), casacore::AxesSpecifier()
	);

	casacore::CoordinateSystem cSys = subImage->coordinates();
	ThrowIf(
		!casacore::CoordinateUtil::findSky(
			errorMessage, dC, pixelAxes,
			worldAxes, cSys
		), errorMessage
	);
	// Find maximum/minimum
   
	casacore::LatticeStatistics<T> stats(*subImage, *this->_getLog(), true);
	casacore::IPosition minPos, maxPos;
	ThrowIf(
		!stats.getMinMaxPos(minPos, maxPos), stats.errorMessage()
	);
	// Make casacore::SubImage of plane of sky holding maximum or minimum
         
	casacore::IPosition shape = subImage->shape();
	const casacore::uInt nDim = subImage->ndim();

	absPixel.resize(nDim);
	casacore::IPosition blc;
	casacore::IPosition trc;
	if (_absFind) {
		// Find positive only

		blc = maxPos;
		trc = maxPos;
		for (casacore::uInt i=0; i<nDim; ++i) {
			absPixel(i) = maxPos(i);
		}
	}
	else {
		// Find positive or negative only
		auto valueMin = subImage->getAt(minPos);
		auto valueMax = subImage->getAt(maxPos);
		if (abs(valueMax) > abs(valueMin)) {
			blc = maxPos;
			trc = maxPos;
			for (casacore::uInt i=0; i<nDim; ++i) {
				absPixel(i) = maxPos(i);
			}
		}
		else {
			blc = minPos;
			trc = minPos;
			for (casacore::uInt i=0; i<nDim; ++i) {
				absPixel(i) = maxPos(i);
			}
		}
	}
	blc(pixelAxes(0)) = 0;
	blc(pixelAxes(1)) = 0;
	trc(pixelAxes(0)) = shape(pixelAxes(0))-1;
	trc(pixelAxes(1)) = shape(pixelAxes(1))-1;
	casacore::IPosition inc(nDim,1);
	casacore::LCBox::verify (blc, trc, inc, shape);
	casacore::Slicer slicer(blc, trc, inc, casacore::Slicer::endIsLast);
	casacore::AxesSpecifier axesSpec(false);   // drop degenerate
	const casacore::SubImage<T> subImage2(*subImage, slicer, axesSpec);
	SPCIIT myclone(subImage2.cloneII());
	// Find one source

	ImageSourceFinder<T> isf(myclone, nullptr, "");
	isf.setCutoff(_cutoff);
	isf.setAbsFind(_absFind);
	isf.setDoPoint(_doPoint);
	isf.setWidth(_width);
   
	ComponentList list = isf.findSources(1);
	SkyComponent sky = list.component(0);
	casacore::DirectionCoordinate dCoord = cSys.directionCoordinate(dC);
	casacore::MDirection mDir = sky.shape().refDirection();
	casacore::Vector<casacore::Double> dirPixel(2);
	ThrowIf(
		!dCoord.toPixel(dirPixel, mDir), dCoord.errorMessage()
	);
	absPixel(pixelAxes(0)) = dirPixel(0);
	absPixel(pixelAxes(1)) = dirPixel(1);
	return sky;
}

template <class T> ComponentList ImageSourceFinder<T>::_findSources (
	casacore::Int nMax
) {
	ComponentList listOut;

	// Make sure the Image is 2D and that it holds the sky.  Exception if not.

	auto subImage = SubImageFactory<T>::createSubImageRO(
		*this->_getImage(), *this->_getRegion(), this->_getMask(),
   		this->_getLog().get(), casacore::AxesSpecifier(false)
	);

	const auto& cSys = subImage->coordinates();
	casacore::Bool xIsLong = cSys.isDirectionAbscissaLongitude();

	// Width support for fast source finder.
	// Can go to w/off/off2 = 5/2/1 but craps out if bigger.

	casacore::Int w = 3;
	casacore::Int off = 1;
	casacore::Int off2 = 0;

	// Results matrix
	casacore::Matrix<typename casacore::NumericTraits<T>::PrecisionType> mat(w,w);
	casacore::Matrix<typename casacore::NumericTraits<T>::PrecisionType> rs(nMax, 3);  // flux, x, y
	rs = 0.0;
    
	// Assume only positive
    
	casacore::Double asign(1.0);

	// Fitting data
 
	casacore::LSQaips fit(6);
	casacore::Vector<T> gel(6);
	casacore::uInt rank;
	casacore::Vector<T> sol(6);
   
	// casacore::Input data arrays

	casacore::IPosition inShape = subImage->shape();
	casacore::Int nx = inShape(0);
	casacore::Int ny = inShape(1);
	ThrowIf(
		ny <= w,
		"Need at least " + casacore::String::toString(w+1)
   	   	+ " rows in image. Found only " + casacore::String::toString(ny)
	);
	casacore::IPosition inSliceShape(2, nx, 1);
	casacore::Block<casacore::COWPtr<casacore::Array<T> > > inPtr(w);
	casacore::Block<casacore::COWPtr<casacore::Array<casacore::Bool> > > inMaskPtr(w);
	casacore::Matrix<casacore::Bool> inDone(w,nx);
	inDone = false;
	for (casacore::Int j=0; j<w; ++j) {
		inPtr[j] = casacore::COWPtr<casacore::Array<T> >(new casacore::Array<T>(inSliceShape));
		inMaskPtr[j] = casacore::COWPtr<casacore::Array<casacore::Bool> >(new casacore::Array<casacore::Bool>(inSliceShape));
	}
	// Read first w-1 lines
	casacore::Int inp = 0;
	casacore::IPosition start(inShape);
	start = 0;
	casacore::IPosition pos(1,0);
	for (casacore::Int j=0; j<(w-1); ++j) {
		subImage->getSlice(inPtr[inp+j], casacore::Slicer(start, inSliceShape), true);
		subImage->getMaskSlice(inMaskPtr[inp+j], casacore::Slicer(start, inSliceShape), true);
		for (casacore::Int i=0; i<nx; ++i) {
			pos(0) = i;
			inDone(inp+j, i) = inMaskPtr[inp+j].ref()(pos);
		}
		start(1) += 1;
	}
	// Loop through remaining lines
               
	for (casacore::Int j=(w-1); j<ny; ++j) {
		inp++;
		inp %= w;
		subImage->getSlice(inPtr[(inp+1)%w], casacore::Slicer(start, inSliceShape), true);
		subImage->getMaskSlice(inMaskPtr[(inp+1)%w], casacore::Slicer(start, inSliceShape), true);
		for (casacore::Int i=0; i<nx; ++i) {
			pos(0) = i;
			inDone((inp+1)%w, i) = !(inMaskPtr[(inp+1)%w].ref()(pos));
		}
		start(1) += 1;
         
		// All points

		for (casacore::Int i=off; i<(nx-off); ++i) {
			if (inDone(inp, i)) {
				continue;             // point already used or masked
			}
			pos(0) = i;
			typename casacore::NumericTraits<T>::PrecisionType v(inPtr[inp].ref()(pos));
			if (_absFind) {                            // find pos/neg
				asign = (v<0) ? -1.0 : 1.0;
				v = abs(v);
			}
			if (
				v<0.8*_cutoff*abs(rs(0,0))
				|| v<0.8*abs(rs(nMax-1,0))
			) {
				continue;      // too small
			}
         
			// Make local data field
            
			casacore::Bool xt = false;
			for (casacore::Int jj=-off; jj<(off+1); ++jj) {
				for (casacore::Int ii=-off; ii<(off+1); ++ii) {
					if (inDone((inp+jj+w)%w, i+ii)) {    // already used
						xt = true;
						break;
					}

					pos(0) = i+ii;
					mat(jj+off,ii+off) = inPtr[(inp+jj+w)%w].ref()(pos);
					mat(jj+off,ii+off) *= asign;            // make abs
				}
				if (xt) {
					break;
				}
			}
			if (xt) {
				continue;
			}
                     
			// Test if a local peak

			if (
				v<=abs(mat(0+off2,1+off2)) || v<=abs(mat(2+off2,1+off2))
				|| v<=abs(mat(1+off2,0+off2)) || v<=abs(mat(1+off2,2+off2))
			) {
				continue;
			}

			// Solve general ellipsoid
    
			casacore::Int k = 0;
			fit.set(6);
			for (casacore::Int jj=-off; jj<(off+1); ++jj) {
				for (casacore::Int ii=-off; ii<(off+1); ++ii) {
					gel(0)= 1;
					gel(1) = jj;
					gel(2) = ii;
					gel(3) = jj*jj;
					gel(4) = ii*ii;
					gel(5) = jj*ii;
					fit.makeNorm(
						gel.data(),
						1.0 - 0.5*(abs(ii)+abs(jj)) + 0.25*abs(jj*ii),
						mat(jj+off,ii+off)
					);
					++k;
				}
			}

			if (!fit.invert(rank)) {
				continue;        // Cannot solve
			}
			fit.solve(sol);
   
			// Find max

			typename casacore::NumericTraits<T>::PrecisionType r1(sol(5)*sol(5) - 4*sol(3)*sol(4));       // dx
			if (r1 == typename casacore::NumericTraits<T>::PrecisionType(0)) {
				continue;                            // forget
			}
			typename casacore::NumericTraits<T>::PrecisionType r0((2*sol(2)*sol(3) - sol(1)*sol(5))/r1);  // dy
			r1 = (2*sol(1)*sol(4) - sol(2)*sol(5))/r1;
			if (abs(r0)>1 || abs(r1)>1) {
				continue;             // too far away from peak
			}
   
			// Amplitude
   
			sol(0) += sol(1)*r0 + sol(2)*r1 + sol(3)*r0*r0 + sol(4)*r1*r1 + sol(5)*r0*r1;
			v = sol(0);
			if (_absFind) {
				v = abs(v);
				sol(0) = asign*sol(0);
			}
			if (v<_cutoff*abs(rs(0,0))) continue;             // too small

			for (casacore::Int k=0; k<nMax; ++k) {
				if (v>=rs(k,0)) {
					for (casacore::Int l=nMax-1; l>k; --l) {
						for (casacore::uInt i0=0; i0<3; ++i0) {
							rs(l,i0) = rs(l-1,i0);
						}
					}

					rs(k,0) = sol(0);                      // Peak
					rs(k,1) = i+r0;                        // X
					rs(k,2) = j+r1-1;                      // Y

					for (casacore::Int jj=-off; jj<(off+1); ++jj) {
						for (casacore::Int ii=-off; ii<(off+1); ++ii) {
							inDone((inp+jj+w)%w, i+ii) = true;
						}
					}
            	  break;
				}
			}
		}
	}
                       
	// Find the number filled
	casacore::Int nFound = 0;
	auto x = _cutoff*abs(rs(0,0));
	for (casacore::Int k=0; k<nMax; ++k) {
		if (abs(rs(k,0)) < x || rs(k,0) == 0) {
			break;
		}
		++nFound;
	}

	if (nFound==0) {
		*this->_getLog() << casacore::LogIO::WARN << "No sources were found" << casacore::LogIO::POST;
		return listOut;
	}

	// Generate more accurate fit if required giveing shape information

	casacore::Matrix<typename casacore::NumericTraits<T>::PrecisionType> ss(nFound, 3);    // major, minor, pa
	casacore::Matrix<typename casacore::NumericTraits<T>::PrecisionType> rs2(rs.copy());   // copy

	if (! _doPoint) {

		// Loop over found sources

		for (casacore::Int k=0; k<nFound; ++k) {
			if (_width <= 0) {

				// This means we want just the default shape estimates only

				ss(k,0) = _width;
				ss(k,1) = _width;
				ss(k,2) = 0.0;
			}
			else {

				// See if we can do this source
				casacore::Int iCen = casacore::Int(rs(k,1));
				casacore::Int jCen = casacore::Int(rs(k,2));
				casacore::IPosition blc0(subImage->ndim(),0);
				casacore::IPosition trc0(subImage->ndim(),0);
				blc0(0) = iCen - _width;
				blc0(1) = jCen - _width;
				trc0(0) = iCen + _width;
				trc0(1) = jCen + _width;

				if (
					blc0(0)<0 || trc0(0)>=inShape(0)
					|| blc0(1)<0 || trc0(1)>=inShape(1)
				) {
					*this->_getLog() << casacore::LogIO::WARN << "Component " << k << " is too close to the image edge for" << endl;
					*this->_getLog() << "  shape-fitting - resorting to default shape estimates" << casacore::LogIO::POST;
					ss(k,0) = _width;
					ss(k,1) = _width;
					ss(k,2) = 0.0;
				}
				else {

					// Fish out data to fit

					casacore::IPosition shp = trc0 - blc0 + 1;
					casacore::Array<T> dataIn = subImage->getSlice(blc0, shp, false);
					casacore::Array<casacore::Bool> maskIn = subImage->getMaskSlice(blc0, shp, false);
					casacore::Array<T> sigmaIn(dataIn.shape(),1.0F);

					// Make fitter, add model and fit

					casacore::Fit2D fit2d(*this->_getLog());
					auto model =  fit2d.estimate(casacore::Fit2D::GAUSSIAN, dataIn, maskIn);
					model(0) = rs(k,0);
					fit2d.addModel(casacore::Fit2D::GAUSSIAN, model);
					auto ret = fit2d.fit(dataIn, maskIn, sigmaIn);

					if (ret==casacore::Fit2D::OK) {
						auto solution = fit2d.availableSolution();

						rs(k,0) = solution(0);
						rs(k,1) = solution(1) + blc0(0);
						rs(k,2) = solution(2) + blc0(1);

						ss(k,0) = solution(3);
						ss(k,1) = solution(4);
						ss(k,2) = solution(5);
					}
					else {
						*this->_getLog() << casacore::LogIO::WARN << "Fit did not converge, resorting to default shape estimates" << casacore::LogIO::POST;
						ss(k,0) = _width;
						ss(k,1) = _width;
						ss(k,2) = 0.0;
					}
				}
			}
		}
	}

	// Fill SkyComponents
	*this->_getLog() << casacore::LogIO::NORMAL << "Found " << nFound << " sources" << casacore::LogIO::POST;
	const casacore::Unit& bU = subImage->units();
	casacore::Double rat;

	// What casacore::Stokes is the plane we are finding in ?
      
	casacore::Stokes::StokesTypes stokes(casacore::Stokes::Undefined);
	stokes = casacore::CoordinateUtil::findSingleStokes (*this->_getLog(), cSys, 0);

	casacore::Vector<casacore::Double> pars;
	ComponentType::Shape cType(ComponentType::POINT);
	pars.resize(3);
	if (! _doPoint) {
		cType = ComponentType::GAUSSIAN;
		pars.resize(6, true);
	}

	for (casacore::Int k=0; k<nFound; ++k) {
		pars(0) = rs(k,0);
		pars(1) = rs(k,1);
		pars(2) = rs(k,2);

		if (! _doPoint) {
			pars(3) = ss(k,0);
			pars(4) = ss(k,1);
			pars(5) = ss(k,2);
		}

		auto beam = subImage->imageInfo().restoringBeam();
		try {
            // FIXME need to deal with multi beam images
			auto sky = SkyComponentFactory::encodeSkyComponent (
				*this->_getLog(), rat, cSys, bU,
				cType, pars, stokes, xIsLong, beam
			);
			listOut.add(sky);
		}
		catch (const casacore::AipsError& x) {
			*this->_getLog() << casacore::LogIO::WARN << "Could not convert fitted pixel parameters to world for source " << k+1 << endl;
			*this->_getLog() << "Probably this means the fitted parameters were wildly wrong" << endl;
			*this->_getLog() << "Reverting to original POINT source parameters for this source " << casacore::LogIO::POST;

			casacore::Vector<casacore::Double> pars2(3);
			pars2(0) = rs2(k,0);
			pars2(1) = rs2(k,1);
			pars2(2) = rs2(k,2);
			// FIXME need to deal with multi beam images
			auto sky = SkyComponentFactory::encodeSkyComponent (
				*this->_getLog(), rat, cSys, bU, ComponentType::POINT,
				pars2, stokes, xIsLong, beam
			);
			listOut.add(sky);
		}
	}
	return listOut;
}

}

