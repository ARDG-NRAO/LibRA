//# MomentCalcBase.h:
//# Copyright (C) 1997,1999,2000,2001,2002
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
//# $Id: MomentCalculator.h 20299 2008-04-03 05:56:44Z gervandiepen $

#ifndef IMAGEANALYSIS_MOMENTCALCBASE_H
#define IMAGEANALYSIS_MOMENTCALCBASE_H

namespace casa {

//# Forward Declarations
template <class T> class MomentsBase;
// <summary>
// Abstract base class for moment calculator classes
// </summary>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
//
// <prerequisite>
//   <li> <linkto class="MomentsBase">MomentsBase</linkto>
//   <li> <linkto class="ImageMoments">ImageMoments</linkto>
//   <li> <linkto class="casacore::LatticeApply">casacore::LatticeApply</linkto>
//   <li> <linkto class="casacore::LineCollapser">casacore::LineCollapser</linkto>
// </prerequisite>
//
// <synopsis>
//  This class, its concrete derived classes, and the classes casacore::LineCollapser,
//  ImageMoments, MSMoments, and casacore::LatticeApply are connected as follows.   casacore::LatticeApply offers 
//  functions so that the application programmer does not need to worry about how 
//  to optimally iterate through a casacore::Lattice; it deals with tiling and to a 
//  lesser extent memory.    casacore::LatticeApply functions are used by offering a class 
//  object to them that has a member function with a name and signature 
//  specified by an abstract base class that casacore::LatticeApply uses and the 
//  offered class inherits from.   Specifically, in this case, MomentCalcBase
//  inherits from casacore::LineCollapser and casacore::LatticeApply uses objects and methods of this
//  class (but does not inherit from it).  This defines the functions
//  <src>collapse</src> and <src>multiProcess</src> which operate on a vector
//  extracted from a Lattice.  The former returns one number, the latter a vector
//  of numbers from that profile.  MomentCalcBase is a base class for
//  for moment calculation and the <src>multiProcess</src>
//  functions are used to compute moments  (e.g., mean, sum, sum squared, 
//  intensity weighted velocity etc).
//
//  It is actually the concrete classes derived from MomentCalcBase (call them,
//  as a group, the MomentCalculator classes) that implement the <src>multiProcess</src> 
//  functions.  These derived classes allow different 
//  algorithms to be written with which moments of the vector can be computed. 
//
//  Now, so far, we have a casacore::LatticeApply function which iterates through Lattices,
//  extracts vectors, and offers them up to functions implemented in the derived 
//  MomentCalculator classes to compute the moments.   As well as that, we need some
//  class to actually construct the MomentCalculator classes and to feed them to 
//  LatticeApply.   This is the role of the ImageMoments or MSMoments classes.  
//  They are a high level 
//  class which takes control information from users specifying which moments they 
//  would like to calculate and how. They also provide the ancilliary masking lattice to 
//  the MomentCalculator constructors. The actual computational work is done by the 
//  MomentCalculator classes. So MomentsBase, MomentCalcBase and their derived 
//  MomentCalculator classes are really one unit; none of them are useful without 
//  the others.  The separation of functionality is caused by having the
//  casacore::LatticeApply class that knows all about optimally iterating through Lattices.
//
//  The coupling between these classes is done partly by the "friendship".   MomentsBase and 
//  its inheritances 
//  grant friendship to MomentCalcBase so that the latter has access to the private data and 
//  private functions of the formers.  MomentCalcBase then operates as an interface between 
//  its derived MomentCalculator classes and ImageMoments or MSMoments. It retrieves private data 
//  from these classes, and also activates private functions in these classes, on behalf 
//  of the MomentCalculator classes. The rest of the coupling is done via the constructors 
//  of the derived MomentCalculator classes.  
//
//  Finally, MomentCalcBase also has a number of protected functions that are common to its
//  derived classes (e.g. fitting, accumulating sums etc).  It also has protected
//  data that is common to all the MomentCalculator classes.  This protected data is accessed 
//  directly by name rather than with interface functions as there is too much of it.  Of 
//  course, since MomentCalcBase is an abstract base class, it is up to the MomentCalculator 
//  classes to give the MomentCalcBase protected data objects values.
//
//  For discussion about different moments and algorithms to compute them see the 
//  discussion in <linkto class="MomentsBase">MomentsBase</linkto>, 
//  <linkto class="ImageMoments">ImageMoments</linkto>, 
//  <linkto class="MSMoments">MSMoments</linkto> and also in
//  the derived classes documentation.
// </synopsis>
//
// <example>
//  Since MomentCalcBase is an abstract class, we defer code examples to
//  the derived classes.
// </example>
//
// <motivation>
// We were desirous of writing functions to optimally iterate through Lattices
// so that the application programmer did not have to know anything about tiling
// or memory if possible.   These are the casacore::LatticeApply functions. To incorporate 
// MomentsBase and its inheritances into this scheme required some of it to be shifted into 
// MomentCalcBase and its derived classes.
// </motivation>
//
// <note role=tip>
// Note that there are is assignment operator or copy constructor.
// Do not use the ones the system would generate either.
// </note>
//
// <todo asof="yyyy/mm/dd">
//  <li> Derive more classes !
// </todo>


template <class T> class MomentCalcBase : public casacore::LineCollapser<T,T> {
public:

    using AccumType = typename casacore::NumericTraits<T>::PrecisionType;
    using DataIterator = typename casacore::Vector<T>::const_iterator;
    using MaskIterator = casacore::Vector<casacore::Bool>::const_iterator;

    virtual ~MomentCalcBase();

    // Returns the number of failed fits if doing fitting
    virtual inline casacore::uInt nFailedFits() const { return nFailed_p; }

protected:

    // A number of private data members are kept here in the base class
    // as they are common to the derived classes.  Since this class
    // is abstract, they have to be filled by the derived classes.

    // CoordinateSystem
    casacore::CoordinateSystem cSys_p;

    // This vector is a container for all the possible moments that
    // can be calculated.  They are in the order given by the MomentsBase
    // enum MomentTypes
    casacore::Vector<T> calcMoments_p;
    casacore::Vector<casacore::Bool> calcMomentsMask_p;

    // This vector tells us which elements of the calcMoments_p vector
    // we wish to select
    casacore::Vector<casacore::Int> selectMoments_p;

    // Although the general philosophy of these classes is to compute
    // all the posisble moments and then select the ones we want,
    // some of them are too expensive to calculate unless they are
    // really wanted.  These are the median moments and those that
    // require a second pass.  These control Bools tell us whether
    // we really want to compute the expensive ones.
    casacore::Bool doMedianI_p, doMedianV_p, doAbsDev_p;

    // These vectors are used to transform coordinates between pixel and world
    casacore::Vector<casacore::Double> pixelIn_p, worldOut_p;

    // All computations involving casacore::Coordinate conversions are relatively expensive
    // These Bools signifies whether we need coordinate calculations or not for
    // the full profile, and for some occaisional calculations
    casacore::Bool doCoordProfile_p, doCoordRandom_p;

    // This vector houses the world coordinate values for the profile if it
    // was from a separable axis. This means this vector can be pre computed
    // just once, instead of working out the coordinates for each profile
    // (expensive).  It should only be filled if doCoordCalc_p is true
    casacore::Vector<casacore::Double> sepWorldCoord_p;

    // This vector is used to hold the abscissa values
    casacore::Vector<T> abcissa_p;

    // This string tells us the name of the moment axis (VELO or FREQ etc)
    casacore::String momAxisType_p;

    // This is the number of Gaussian fits that failed.
    casacore::uInt nFailed_p;

    // This scale factor is the increment along the moment axis
    // applied so that units for the Integrated moment are like
    // Jy/beam.km/s (or whatever is needed for the moment axis units)
    // For non-linear velocities (e.g. optical) this is approximate
    // only and is computed at the reference pixel
    casacore::Double integratedScaleFactor_p;

    // Accumulate statistical sums from a vector
    inline void accumSums(
        typename casacore::NumericTraits<T>::PrecisionType& s0,
        typename casacore::NumericTraits<T>::PrecisionType& s0Sq,
        typename casacore::NumericTraits<T>::PrecisionType& s1,
        typename casacore::NumericTraits<T>::PrecisionType& s2,
        casacore::Int& iMin, casacore::Int& iMax, T& dMin, T& dMax,
        casacore::Int i, T datum, casacore::Double coord
    ) const {
        // Accumulate statistical sums from this datum
        //
        // casacore::Input:
        //  i              Index
        //  datum          Pixel value
        //  coord          casacore::Coordinate value on moment axis
        // casacore::Input/output:
        //  iMin,max       index of dMin and dMax
        //  dMin,dMax      minimum and maximum value
        // Output:
        //  s0             sum (I)
        //  s0Sq           sum (I*I)
        //  s1             sum (I*v)
        //  s2             sum (I*v*v)
        typename casacore::NumericTraits<T>::PrecisionType dDatum = datum;
        s0 += dDatum;
        s0Sq += dDatum*dDatum;
        s1 += dDatum*coord;
        s2 += dDatum*coord*coord;
        if (datum < dMin) {
            iMin = i;
            dMin = datum;
        }
        if (datum > dMax) {
            iMax = i;
            dMax = datum;
        }
    }

    // Determine if the spectrum is pure noise
    casacore::uInt allNoise(T& dMean,
        const casacore::Vector<T>& data,
        const casacore::Vector<casacore::Bool>& mask,
        T peakSNR,
        T stdDeviation
    ) const;

    // Check validity of constructor inputs
    void constructorCheck(
        casacore::Vector<T>& calcMoments,
        casacore::Vector<casacore::Bool>& calcMomentsMask,
        const casacore::Vector<casacore::Int>& selectMoments,
        casacore::uInt nLatticeOut
    ) const;

    // Find out from the selectMoments array whether we want
    // to compute the more expensive moments
    void costlyMoments(
        MomentsBase<T>& iMom, casacore::Bool& doMedianI,
        casacore::Bool& doMedianV, casacore::Bool& doAbsDev
    ) const;

    // Return the casacore::Bool saying whether we need to compute coordinates
    // or not for the requested moments
    void doCoordCalc(
        casacore::Bool& doCoordProfile,
        casacore::Bool& doCoordRandom,
        const MomentsBase<T>& iMom
    ) const;

    // Return the casacore::Bool from the ImageMoments or MSMoments object saying whether we
    // are going to fit Gaussians to the profiles or not.
    casacore::Bool doFit(const MomentsBase<T>& iMom) const;

    // Find the next masked or unmasked point in a vector
    casacore::Bool findNextDatum(
        casacore::uInt& iFound, const casacore::uInt& n,
        const casacore::Vector<casacore::Bool>& mask, const casacore::uInt& iStart,
        const casacore::Bool& findGood
    ) const;

    // Fit a Gaussian to x and y arrays given guesses for the gaussian parameters
    casacore::Bool fitGaussian(
        casacore::uInt& nFailed, T& peak, T& pos,
        T& width, T& level, const casacore::Vector<T>& x,
        const casacore::Vector<T>& y, const casacore::Vector<casacore::Bool>& mask,
        T peakGuess, T posGuess, T widthGuess,
        T levelGuess
    ) const;

    // Automatically fit a Gaussian to a spectrum, including finding the
    // starting guesses.
    casacore::Bool getAutoGaussianFit(
        casacore::uInt& nFailed, casacore::Vector<T>& gaussPars,
        const casacore::Vector<T>& x, const casacore::Vector<T>& y,
        const casacore::Vector<casacore::Bool>& mask, T peakSNR,
        T stdDeviation
    ) const;

    // Automatically work out a guess for the Gaussian parameters
    // Returns false if all pixels masked.
    casacore::Bool getAutoGaussianGuess(
        T& peakGuess, T& posGuess,
        T& widthGuess, T& levelGuess,
        const casacore::Vector<T>& x, const casacore::Vector<T>& y,
        const casacore::Vector<casacore::Bool>& mask
    ) const;

    // Compute the world coordinate for the given moment axis pixel
    inline casacore::Double getMomentCoord(
        const MomentsBase<T>& iMom, casacore::Vector<casacore::Double>& pixelIn,
        casacore::Vector<casacore::Double>& worldOut, casacore::Double momentPixel,
        casacore::Bool asVelocity=false
    ) const {
        // Find the value of the world coordinate on the moment axis
        // for the given moment axis pixel value.
        //
        // Input
        //   momentPixel   is the index in the profile extracted from the data
        // casacore::Input/output
        //   pixelIn       Pixels to convert.  Must all be filled in except for
        //                 pixelIn(momentPixel).
        //   worldOut      casacore::Vector to hold result
        //
        // Should really return a casacore::Fallible as I don't check and see
        // if the coordinate transformation fails or not
        //
        // Should really check the result is true, but for speed ...
        pixelIn[iMom.momentAxis_p] = momentPixel;
        cSys_p.toWorld(worldOut, pixelIn);
        if (asVelocity) {
            casacore::Double velocity;
            cSys_p.spectralCoordinate().frequencyToVelocity(
                velocity, worldOut(iMom.worldMomentAxis_p)
            );
            return velocity;
        }
        return worldOut(iMom.worldMomentAxis_p);
    }

    // Examine a mask and determine how many segments of unmasked points
    // it consists of.
    void lineSegments (
        casacore::uInt& nSeg, casacore::Vector<casacore::uInt>& start,
        casacore::Vector<casacore::uInt>& nPts, const casacore::Vector<casacore::Bool>& mask
    ) const;

    // Return the moment axis from the ImageMoments object
    casacore::Int& momentAxis(MomentsBase<T>& iMom) const;

    // Return the name of the moment/profile axis
    casacore::String momentAxisName(
        const casacore::CoordinateSystem&,
        const MomentsBase<T>& iMom
    ) const;

    // Return the peak SNR for determination of all noise spectra from
    // the ImageMoments or MSMoments object
    T& peakSNR(MomentsBase<T>& iMom) const;

    // Return the selected pixel intensity range from the ImageMoments or MSMoments
    // object and the Bools describing whether it is inclusion or exclusion
    void selectRange(
        casacore::Vector<T>& pixelRange,
        casacore::Bool& doInclude,
        casacore::Bool& doExlude,
        MomentsBase<T>& iMom
    ) const;

    // The MomentCalculators compute a vector of all possible moments.
    // This function returns a vector which selects the desired moments from that
    // "all moment" vector.
    casacore::Vector<casacore::Int> selectMoments(MomentsBase<T>& iMom) const;

    // Fill the ouput moments array
    void setCalcMoments(
        const MomentsBase<T>& iMom, casacore::Vector<T>& calcMoments,
        casacore::Vector<casacore::Bool>& calcMomentsMask, casacore::Vector<casacore::Double>& pixelIn,
        casacore::Vector<casacore::Double>& worldOut, casacore::Bool doCoord,
        casacore::Double integratedScaleFactor, T dMedian,
        T vMedian, casacore::Int nPts,
        typename casacore::NumericTraits<T>::PrecisionType s0,
        typename casacore::NumericTraits<T>::PrecisionType s1,
        typename casacore::NumericTraits<T>::PrecisionType s2,
        typename casacore::NumericTraits<T>::PrecisionType s0Sq,
        typename casacore::NumericTraits<T>::PrecisionType sumAbsDev,
        T dMin, T dMax, casacore::Int iMin, casacore::Int iMax
    ) const;

    // Fill a string with the position of the cursor
    void setPosLabel(casacore::String& title, const casacore::IPosition& pos) const;

    // Install casacore::CoordinateSystem and SpectralCoordinate
    // in protected data members
    void setCoordinateSystem(
        casacore::CoordinateSystem& cSys, MomentsBase<T>& iMom
    );

    // Set up separable moment axis coordinate vector and
    // conversion vectors if not separable
    void setUpCoords(
        const MomentsBase<T>& iMom, casacore::Vector<casacore::Double>& pixelIn,
        casacore::Vector<casacore::Double>& worldOut, casacore::Vector<casacore::Double>& sepWorldCoord,
        casacore::LogIO& os, casacore::Double& integratedScaleFactor,
        const casacore::CoordinateSystem& cSys, casacore::Bool doCoordProfile,
        casacore::Bool doCoordRandom
    ) const;

    // Return standard deviation of image from ImageMoments or MSMoments object
    T& stdDeviation(MomentsBase<T>& iMom) const;

protected:
    // Check if #pixels is indeed 1.
    virtual void init (casacore::uInt nOutPixelsPerCollapse);
};

}

#ifndef CASACORE_NO_AUTO_TEMPLATES
#include <imageanalysis/ImageAnalysis/MomentCalcBase.tcc>
#endif
#endif
