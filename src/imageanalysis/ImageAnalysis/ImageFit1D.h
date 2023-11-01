
//# ImageFit1D.h: Class to fit profiles to vectors from images
//# Copyright (C) 2004
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
//#   $Id: ImageFit1D.h 20229 2008-01-29 15:19:06Z gervandiepen $

#ifndef IMAGES_IMAGEFIT1D_H
#define IMAGES_IMAGEFIT1D_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/scimath/Mathematics/NumericTraits.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MFrequency.h>
#include <casacore/measures/Measures/MDoppler.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <components/SpectralComponents/ProfileFit1D.h>

#include <memory>

namespace casacore{

class ImageRegion;
template<class T> class ImageInterface;
}

namespace casa {

class SpectralElement;
class SpectralList;


// <summary>
// Fit spectral components to a casacore::Vector of data from an image
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tImageFit1D.cc">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="SpectralElement">SpectralElement</linkto> 
//   <li> <linkto class="SpectralList">SpectralList</linkto> 
//   <li> <linkto class="SpectralFit">SpectralFit</linkto> 
//   <li> <linkto class="ProfileFit1D">ProfileFit1D</linkto> 
// </prerequisite>

// <synopsis> 
// Fit lists (held in class SpectralList) of SpectralElements to a casacore::Vector of data 
// from the image.  Each SpectralElement can  be one from a variety of types.
// The values of the parameters for each SpectralElement provide the initial 
// starting guesses for the fitting process.  
//
// You specify the domain in which the fit is to be done via the 
// enum AbcissaType.  The casacore::CoordinateSystem in the image is used
// to convert the pixel coordinates to the desired abcissa.  
// You can change the units of the casacore::CoordinateSystem if you want
// to fit in different units.  If you set an estimate yourself
// (function setElements or addElement) it is the callers responsibility
// that the elements are in the correct abcissa domain.  Function
// setGaussianElements will automatically make an estimate in the 
// correct domain.
//
// Also, a SpectralElement object holds a mask indicating whether 
// a parameter should be held fixed or solved for.   After the 
// fitting is done, a new SpectralList holding SpectralElements with 
// the fitted parameters is created.  
//
// For all the functions that return a status casacore::Bool, true is good. If
// false is returned, an error message can be recovered with function
// <src>errorMessage</src>,  You should not proceed if false is returned.
// 
// Exceptions will be thrown if you do not set the Image and axis 
// via the constructor or <src>setImage</src> function.
// </synopsis> 

// <example>
// <srcblock>
// casacore::PagedImage<casacore::Float> im("myimage");
// casacore::Int axis = 2;
// ImageFit1D<casacore::Float> fitter(image, axis);
// casacore::IPosition pos(in.ndim(),0);
// fitter.setData(pos, ImageFit1D<casacore::Float>::IM_NATIVE);     // Fit in native coordinate space
// fitter.setGaussianElements(3);                      // FIt 3 Gaussians
// if (fitter.fit()) {
//    cerr << fitter.getList() << endl;                // Print result
// }
// 
// </srcblock>
// </example>

// <todo asof="2004/07/10">
//   <li> Add constraints
// </todo>


template <class T> class ImageFit1D {
public:

using FitterType = typename casacore::NumericTraits<T>::PrecisionType;

    enum AbcissaType {
       PIXEL = 0,
       IM_NATIVE = 1,
       VELOCITY = 2,
       N_TYPES};


    // Constructor.  Fitting weights are assumed all unity.
    ImageFit1D(std::shared_ptr<const casacore::ImageInterface<T> > image, casacore::uInt axis=0);

    // Constructor with fitting weights image.  The data and weights images must
    // be the same shape.
    ImageFit1D(
    	std::shared_ptr<const casacore::ImageInterface<T> > image,
    	std::shared_ptr<const casacore::ImageInterface<T> > weights, casacore::uInt axis=0
    );

    // Destructor
    ~ImageFit1D();

    // Copy constructor.  Uses reference semantics.
    ImageFit1D(const ImageFit1D& other);

    // Assignment operator. Uses reference semantics.
    ImageFit1D& operator=(const ImageFit1D& other);


    // Set the data to be fit.  All non-profile axes data are averaged.
    // For the profile axis, the full spectrum is taken.  The abscissa
    // world values are computed when you call these functions unless they
    // have been set previously by a call to setAbscissa() in which case
    // the values that were passed to that method are used. Use the first
    // form of setData() in this case. The domain of the
    // abscissa values is controlled by <src>AbcissaType</src> and
    // <src>doAbs</src> (absolute coordinates).  The casacore::CoordinateSystem in
    // the image is used to convert from pixels to world values.
    // If <src>type</src>=IN_NATIVE and <src>abscissaDivisor</src> is not null,
    // the world abscissa values will be divided by the value pointed to by
    // <src>abscissaDivisor</src>. This mitigates having very large or very small
    // abscissa values when fitting. If xfunc and/or yfunc is not NULL, the x and/or
    // y values are fed to the specified function and the resultant values are what
    // are used for the x and/or y values in the fit. If xfunc is not NULL and
    // setAbscissa values has been called prior, no abscissa value transformation occurs.
    // Thus if you want to apply a function to the abscissa values, the caller should
    // pass the result of that function into setAbscissaValues.
    // <group>
    void setData (
    	const casacore::IPosition& pos, /*const ImageFit1D<T>::AbcissaType type,
        const casacore::Bool doAbs=true, const casacore::Double* const &abscissaDivisor=0,
        casacore::Array<casacore::Double> (*xfunc)(const casacore::Array<casacore::Double>&)=0, */
        casacore::Array<FitterType> (*yfunc)(const casacore::Array<FitterType>&)=0
    );

    void setData (
    	const casacore::IPosition& pos, const ImageFit1D<T>::AbcissaType type,
    	const casacore::Bool doAbs=true, const casacore::Double* const &abscissaDivisor=0,
    	casacore::Array<casacore::Double> (*xfunc)(const casacore::Array<casacore::Double>&)=0,
    	casacore::Array<FitterType> (*yfunc)(const casacore::Array<FitterType>&)=0
    );

    /*
    casacore::Bool setData (
    	const casacore::ImageRegion& region, const ImageFit1D<T>::AbcissaType type,
        casacore::Bool doAbs=true
    );
    */
    // </group>

    // Set a SpectralList of SpectralElements to fit for.    These elements
    // must be in the correct abcissa domain set in function <src>setData</src>.
    // You must have already called <src>setData</src> to call this function.
    // The SpectralElements in the list hold the
    // initial estimates.  They also contain the information about whether
    // specific parameters are to be held fixed or allowed to vary in
    // the fitting process.
    // You can recover the list of elements with function getList.
    void setElements (const SpectralList& list) {_fitter.setElements(list);};

    // Add new SpectralElement(s) to the SpectralList (can be empty)
    // of SpectralElements to be fit for.  
    // You must have already called <src>setData</src> to call this function.
    //<group>
    void addElement (const SpectralElement& el) {_fitter.addElement(el);};
    void addElements (const SpectralList& list) {_fitter.addElements(list);};
    // </group>

    // Set a SpectralList of Gaussian SpectralElements to fit for.  
    // The initial estimates for the Gaussians will be automatically determined
    // in the correct abcissa domain.
    // All of the parameters created by this function will be solved for
    // by default. You can recover the list of elements with function getList.
    // Status is returned, if false, error message can be recovered with <src>errorMessage</src>
    void setGaussianElements (casacore::uInt nGauss);

    // Clear the SpectralList of elements to be fit for
    void clearList () {_fitter.clearList();};

    // Do the fit and return convergence status.  Errors in the fitting
    // process will generate an casacore::AipsError exception and you should catch
    // these yourself.
    casacore::Bool fit ();

    // Get Chi Squared of fit
    casacore::Double getChiSquared () const {return _fitter.getChiSquared();}

    // Get number of iterations for last fit
    casacore::Double getNumberIterations () const {return _fitter.getNumberIterations();}

    // Recover the list of elements.  You can get the elements
    // as initially estimated (fit=false), or after fitting 
    // (fit=true).  In the latter case, the SpectralElements
    // hold the parameters and errors of the fit.
    const SpectralList& getList (casacore::Bool fit=true) const {return _fitter.getList(fit);};

    // Recover vectors for the estimate, fit and residual.
    // If you don't specify which element, all elements are included
    // If the Vectors are returned with zero length, it means an error
    // condition exists (e.g. asking for fit before you do one). In this
    // case an error message can be recovered with function <src>errorMessage</src>.
    //<group>
    casacore::Vector<T> getEstimate (casacore::Int which=-1) const;
    casacore::Vector<T> getFit (casacore::Int which=-1) const;
    casacore::Vector<T> getResidual (casacore::Int which=-1, casacore::Bool fit=true)  const;
    //</group>

    casacore::Bool setXMask(const std::set<casacore::uInt>& indices, casacore::Bool specifiedPixelsAreGood) {
    	return _fitter.setXMask(indices, specifiedPixelsAreGood);
    }

    // get data mask
    casacore::Vector<casacore::Bool> getDataMask () const {return _fitter.getDataMask();};

    // Get Total Mask (data and range mask)
    casacore::Vector<casacore::Bool> getTotalMask () const {return _fitter.getTotalMask();};

    // did the fit succeed? should only be called after fit().
    casacore::Bool succeeded() const;

    // did the fit converge? should only be called after fit().
    casacore::Bool converged() const;

    // Helper function.  Sets up the casacore::CoordinateSystem to reflect the choice of
    // abcissa unit and the doppler (if the axis is spectral).
    static casacore::Bool setAbcissaState (casacore::String& errMsg, ImageFit1D<T>::AbcissaType& type,
                                 casacore::CoordinateSystem& cSys, const casacore::String& xUnit,
                                 const casacore::String& doppler, casacore::uInt pixelAxis);


    // flag the solution as invalid based on external criteria.
    void invalidate();

    // is the solution valid? If false, some external logic has
    // called invalidate()
    casacore::Bool isValid() const;

    // Set the abscissa values prior to running setData. If this is done, then
    // the abscissa values will not be recomputed when setData is called.
    // This can imporove performance if, for example, you are looping over several fitters for
    // which you know the abscissa values do not change.
    void setAbscissa(const casacore::Vector<casacore::Double>& x) { _x.assign(x); }

    // make the abscissa values, <src>x</src>. If <src>type</src>=IN_NATIVE
    // and <src>abscissaDivisor is not null, then divide the native values
    // by the value pointed to by <src>abscissaDivisor</src> in making the abscissa
    // values.
    casacore::Vector<casacore::Double> makeAbscissa (
		   ImageFit1D<T>::AbcissaType type,
		   casacore::Bool doAbs, const casacore::Double* const &abscissaDivisor
   );
private:
   std::shared_ptr<const casacore::ImageInterface<T> > _image, _weights;
   casacore::uInt _axis;

// In the future I will be able to template the fitter on T. For now
// it must be Double.

   ProfileFit1D<FitterType> _fitter;
   casacore::Bool _converged, _success, _isValid;
   casacore::Vector<casacore::Double> _x, _unityWeights, _weightSlice;
   casacore::IPosition _sliceShape;
   
   // Disallow default constructor
   ImageFit1D() {}

   void check() const;
   void checkType() const;
   void _construct();
   void copy (const ImageFit1D<T>& other);

   //void setWeightsImage (const casacore::ImageInterface<T>& im);

   // reset the fitter, for example if we've done a fit and want to move
   // to the next position in the image
   void _resetFitter();


   // Set Image(s) and axis
   // <group>
   // void setImage (const casacore::ImageInterface<T>& im, const casacore::ImageInterface<T>& weights, casacore::uInt pixelAxis);
   // void setImage (const casacore::ImageInterface<T>& im, casacore::uInt pixelAxis);
   // </group>

};

} //#End casa namespace

#ifndef CASACORE_NO_AUTO_TEMPLATES
#include <imageanalysis/ImageAnalysis/ImageFit1D.tcc>
#endif //# CASACORE_NO_AUTO_TEMPLATES
#endif
