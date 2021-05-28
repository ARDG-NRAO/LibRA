//# tSubImage.cc: Test program for class SubImage
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


#ifndef IMAGEANALYSIS_IMAGEPROFILEFITTERRESULTS_H
#define IMAGEANALYSIS_IMAGEPROFILEFITTERRESULTS_H

#include <components/SpectralComponents/GaussianMultipletSpectralElement.h>
#include <imageanalysis/ImageAnalysis/ImageFit1D.h>
#include <images/Images/TempImage.h>

#include <casa/namespace.h>

namespace casa {

class LogFile;
class ProfileFitResults;

class ImageProfileFitterResults {
    // <summary>
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // </etymology>

    // <synopsis>
    // This class is the encapsulated IO portion of ImageProfileFitter. It is meant to be
    // used only by that class. It is seperate from ImageProfileFitter for maintenence
    // and compile convenience.
    // </synopsis>

    // <example>
    // <srcblock>
    // </srcblock>
    // </example>

public:
    ImageProfileFitterResults(
		const std::shared_ptr<casacore::LogIO> log, const casacore::CoordinateSystem& csysIm,
		const casacore::Array<std::shared_ptr<ProfileFitResults> >* const &fitters,
        const SpectralList& nonPolyEstimates,
		const std::shared_ptr<const casacore::SubImage<casacore::Float> > subImage, casacore::Int fitAxis, casacore::Int polyOrder,
		casacore::uInt nGaussSinglets, casacore::uInt nGaussMultiplets, casacore::uInt nLorentzSinglets,
		casacore::uInt nPLPCoeffs, casacore::uInt nLTPCoeffs, casacore::Bool logResults, casacore::Bool multiFit,
		const std::shared_ptr<LogFile> logfile, const casacore::String& xUnit,
		const casacore::String& summaryHeader
    );

    ~ImageProfileFitterResults();

    // create all the products. Should be run after the object has been entirely configured
    void createResults();

    // get the result Record
	casacore::Record getResults() const;

    inline casacore::String getClass() const { return _class; };

    // stream results to logger? This can be a lot of output.
    inline void setLogResults(const casacore::Bool logResults) { _logResults = logResults; }

    // <group>
    // gaussian amplitude image name
    inline void setAmpName(const casacore::String& s) { _ampName = s; }
    // gaussian amplitude error image name
    inline void setAmpErrName(const casacore::String& s) { _ampErrName = s; }
    // gaussian center image name
    inline void setCenterName(const casacore::String& s) { _centerName = s; }
    // gaussian center error image name
    inline void setCenterErrName(const casacore::String& s) { _centerErrName = s; }
    // gaussian fwhm image name
    inline void setFWHMName(const casacore::String& s) { _fwhmName = s; }
    // gaussian fwhm error image name
    inline void setFWHMErrName(const casacore::String& s) { _fwhmErrName = s; }
    // gaussian integral image name
    inline void setIntegralName(const casacore::String& s) { _integralName = s; }
    // gaussian integral error image name
    inline void setIntegralErrName(const casacore::String& s) { _integralErrName = s; }
    // </group>

    // set the name of the power logarithmic polynomial image.
    inline void setPLPName(const casacore::String& s) { _plpName = s; }

    // set the name of the power logarithmic polynomial image.
    inline void setPLPErrName(const casacore::String& s) { _plpErrName = s; }

    // set the name of the power logarithmic polynomial image.
    inline void setLTPName(const casacore::String& s) { _ltpName = s; }

    // set the name of the power logarithmic polynomial image.
    inline void setLTPErrName(const casacore::String& s) { _ltpErrName = s; }


    //inline void setOutputSigmaImage(const casacore::String& s) { _sigmaName = s; }
    // </group>


    const static casacore::String _CONVERGED;
    const static casacore::String _SUCCEEDED;
    const static casacore::String _VALID;

    const casacore::Array<ImageFit1D<casacore::Float> >& getFitters() const;
    // Returns the center, in pixels of the indexth fit.
    const casacore::Vector<casacore::Double> getPixelCenter( uint index ) const;
    //Converts a pixel value into a world value either in velocity, wavelength, or
    //frequency units.
    casacore::Double getWorldValue(
    	double pixelVal, const casacore::IPosition& imPos, const casacore::String& units,
        bool velocity, bool wavelength
    ) const;

    void setPLPDivisor(const casacore::String& x) { _plpDivisor = x; }

    std::vector<String> logSummary(
    	casacore::uInt nProfiles, casacore::uInt nAttempted, casacore::uInt nSucceeded,
    	casacore::uInt nConverged, casacore::uInt nValid
    );

    void writeImages(casacore::Bool someConverged) const;

private:
    enum gaussSols {
        AMP, CENTER, FWHM, INTEGRAL, AMPERR, CENTERERR,
        FWHMERR, INTEGRALERR, NGSOLMATRICES
    };

    enum spxSols {
        SPXSOL, SPXERR, NSPXSOLMATRICES
    };

    enum axisType {
        LONGITUDE, LATITUDE, FREQUENCY, POLARIZATION, NAXISTYPES
    };
    
    const static casacore::String _class;
    casacore::Bool _logResults, _multiFit, _doVelocity;
   	casacore::String _xUnit, _centerName, _centerErrName, _fwhmName,
        _fwhmErrName, _ampName, _ampErrName,
        _integralName, _integralErrName, _plpName, _plpErrName,
        _ltpName, _ltpErrName, /*_sigmaName, */ _summaryHeader;
    casacore::uInt _nGaussSinglets, _nGaussMultiplets, _nLorentzSinglets,
        _nPLPCoeffs, _nLTPCoeffs;
    const casacore::Array<std::shared_ptr<ProfileFitResults> >* const  _fitters;
    SpectralList _nonPolyEstimates;
    // subimage contains the region of the original image
    // on which the fit is performed.
	const std::shared_ptr<const casacore::SubImage<casacore::Float> > _subImage;
	casacore::Int _polyOrder, _fitAxis;
    std::vector<axisType> _axisTypes;
	casacore::Array<casacore::String> _worldCoords;
	casacore::Record _results;
	const static casacore::uInt _nOthers, _gsPlane, _lsPlane;
    std::shared_ptr<LogFile> _logfile;
	std::shared_ptr<casacore::LogIO> _log;
   	casacore::Vector<casacore::Double> _goodAmpRange, _goodCenterRange, _goodFWHMRange;
   	const casacore::CoordinateSystem _csysIm;
   	casacore::String _plpDivisor;

    void _setResults();

    void _resultsToLog();

    casacore::String _getTag(const casacore::uInt i) const;

    std::unique_ptr<std::vector<vector<casacore::Array<casacore::Double> > > > _createPCFArrays() const;

    casacore::String _elementToString(
    	const casacore::Double value, const casacore::Double error,
    	const casacore::String& unit, casacore::Bool isFixed
    ) const;

    casacore::String _pcfToString(
    	const PCFSpectralElement *const &pcf, const casacore::CoordinateSystem& csys,
    	const casacore::Vector<casacore::Double>& world, const casacore::IPosition& imPos, casacore::Bool showTypeString=true,
    	const casacore::String& indent=""
    ) const;

    casacore::String _gaussianMultipletToString(
        const GaussianMultipletSpectralElement& gm,
    	const casacore::CoordinateSystem& csys, const casacore::Vector<casacore::Double>& world,
    	const casacore::IPosition& imPos
    ) const;

    casacore::Bool _setAxisTypes();

    casacore::String _polynomialToString(
    	const PolynomialSpectralElement& poly, const casacore::CoordinateSystem& csys,
    	const casacore::Vector<casacore::Double>& imPix, const casacore::Vector<casacore::Double>& world
    ) const;

    casacore::String _powerLogPolyToString(
        const PowerLogPolynomialSpectralElement& plp
    ) const;

    casacore::String _logTransPolyToString(
        const LogTransformedPolynomialSpectralElement& ltp
    ) const;

    void _marshalFitResults(
    	casacore::Array<casacore::Bool>& attemptedArr, casacore::Array<casacore::Bool>& successArr,
        casacore::Array<casacore::Bool>& convergedArr, casacore::Array<casacore::Bool>& validArr,
        casacore::Array<casacore::String>& typeMat, casacore::Array<casacore::Int>& niterArr,
        casacore::Array<casacore::Int>& nCompArr, std::unique_ptr<std::vector<vector<casacore::Array<casacore::Double> > > >& pcfArrays,
        std::vector<casacore::Array<casacore::Double> >& plpArrayss, std::vector<casacore::Array<casacore::Double> >& ltpArrays, casacore::Bool returnDirection,
        casacore::Array<casacore::String>& directionInfo /*, casacore::Array<casacore::Bool>& mask */
    ); 

    static void _makeSolutionImages(
    	const casacore::String& name, const casacore::CoordinateSystem& csys,
		const casacore::Array<casacore::Double>& values, const casacore::String& unit,
		const casacore::Array<casacore::Bool>& mask
    );

    void _insertPCF(
        vector<vector<casacore::Array<casacore::Double> > >& pcfArrays, /*casacore::Bool& isSolutionSane,*/
        const casacore::IPosition& pixel, const PCFSpectralElement& pcf, 
        const casacore::uInt row, const casacore::uInt col, 
        const casacore::Double increment/*, const casacore::uInt npix*/
    ) const;

    void _writeImages(
    	const casacore::CoordinateSystem& csys,
    	const casacore::Array<casacore::Bool>& mask, const casacore::String& yUnit
    ) const;

    casacore::Double _fitAxisIncrement() const;

    casacore::Double _centerWorld(
    	const PCFSpectralElement& solution, const casacore::IPosition& imPos
    ) const;

    //casacore::Bool _inVelocitySpace() const;

    casacore::Vector< casacore::Vector<casacore::Double> > _pixelPositions;

    void _writeLogfile(const casacore::String& str, casacore::Bool open, casacore::Bool close);

    // the input array must have a degenerate last axis. It will be replicated
    // along this axis n times to form an array with a last axis laength of n.
    static casacore::Array<casacore::Bool> _replicateMask(const casacore::Array<casacore::Bool>& array, casacore::Int n);

    void _doWorldCoords(
    	casacore::Array<casacore::String>& directionInfo, const casacore::CoordinateSystem& csysSub,
    	const casacore::IPosition& pixel, const casacore::DirectionCoordinate* const &dcoord,
    	const casacore::SpectralCoordinate* const &spcoord, const casacore::StokesCoordinate* const &polcoord,
    	casacore::Bool returnDirection, const casacore::String& directionType
    );

    void _processSolutions(
    	/* casacore::Array<casacore::Bool>& mask, */ casacore::Array<casacore::String>& typeMat, casacore::Array<casacore::Int>& niterArr,
    	casacore::Array<casacore::Int>& nCompArr, const casacore::IPosition& pixel,
        std::shared_ptr<const ProfileFitResults> fitter,
    	/* const casacore::RO_MaskedLatticeIterator<casacore::Float>& inIter, */
    	std::unique_ptr<std::vector<vector<casacore::Array<casacore::Double> > > >& pcfArrays,
    	std::vector<casacore::Array<casacore::Double> >& plpArrays, std::vector<casacore::Array<casacore::Double> >& ltpArrays,
    	casacore::Double increment
    );
};
}

#endif
