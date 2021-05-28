//# SingleDishSkyCal.h: Single dish sky calibration
//# Copyright (C) 2014
//# Associated Universities, Inc. Washington DC, USA.
//# National Astronomical Observatory of Japan, Japan.
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
//#
//# $Id$

//#! Create an include 'guard', containing your class name in the all
//#! upper case format implied below.  This prevents multiple inclusion
//#! of this header file during pre-processing.
//#!
//#! Note that the leading "AIPS_" identifies the package to which your class
//#! belongs.  Other packages include dish, vlbi, nfra, synthesis, atnf...
//#! If you are contributing a new class to one of these packages, be
//#! sure to replace "AIPS_" with (for instance) "DISH_" or "ATNF_".

#ifndef _SYNTHESIS_SINGLEDISH_SKY_CAL_H_
#define _SYNTHESIS_SINGLEDISH_SKY_CAL_H_

//#! Includes go here
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/SkyCal.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations

// <summary>
//#! A one line summary of the class.   This summary (shortened a bit
//#! if necessary so that it fits along with the "ClassFileName.h" in 75
//#! characters) should also appear as the first line of this file.
//#! Be sure to use the word "abstract" here if this class is, indeed,
//#! an abstract base class.
// </summary>

// <use visibility=local>   or   <use visibility=export>
//#! If a class is intended for use by application programmers, or
//#! people writing other libraries, then specify that this class
//#! has an "export" visibility:  it defines an interface that
//#! will be seen outside of its module.  On the other hand, if the
//#! class has a "local" visibility, then it is known and used only
//#! within its module.

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
//#! for example:
//#!  <reviewed reviewer="pshannon@nrao.edu" date="1994/10/10" tests="tMyClass, t1MyClass" demos="dMyClass, d1MyClass">
//#!  This is a well-designed class, without any obvious problems.
//#!  However, the addition of several more demo programs would
//#!  go a *long* way towards making it more usable.
//#!  </reviewed>
//#!
//#! (In time, the documentation extractor will be able handle reviewed
//#! attributes spread over more than one line.)
//#!
//#! see "Coding Standards and Guidelines"  (AIPS++ note 167) and
//#! "AIPS++ Code Review Process" (note 170) for a full explanation
//#! It is up to the class author (the programmer) to fill in these fields:
//#!     tests, demos
//#! The reviewer fills in
//#!     reviewer, date
// </reviewed>

// <prerequisite>
//#! Classes or concepts you should understand before using this class.
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
//#! Except when it is obvious (e.g., "casacore::Array") explain how the class name
//#! expresses the role of this class.  Example: casacore::IPosition is short for
//#! "Integral Position" - a specialized integer vector for specifying
//#! array dimensions and indices.
// </etymology>
//
// <synopsis>
//#! What does the class do?  How?  For whom?  This should include code
//#! fragments as appropriate to support text.  Code fragments shall be
//#! delimited by <srcblock> </srcblock> tags.  The synopsis section will
//#! usually be dozens of lines long.
// </synopsis>
//
// <example>
//#! One or two concise (~10-20 lines) examples, with a modest amount of
//#! text to support code fragments.  Use <srcblock> and </srcblock> to
//#! delimit example code.
// </example>
//
// <motivation>
//#! Insight into a class is often provided by a description of
//#! the circumstances that led to its conception and design.  Describe
//#! them here.
// </motivation>
//
// <templating arg=T>
//#! A list of member functions that must be provided by classes that
//#! appear as actual template arguments.  For example:  imagine that you
//#! are writing a templated sort class, which does a quicksort on a
//#! list of arbitrary objects.  Anybody who uses your templated class
//#! must make sure that the actual argument class (say, casacore::Int or
//#! casacore::String or casacore::Matrix) has comparison operators defined.
//#! This tag must be repeated for each template formal argument in the
//#! template class definition -- that's why this tag has the "arg" attribute.
//#! (Most templated classes, however, are templated on only a single
//#! argument.)
//    <li>
//    <li>
// </templating>
//
// <thrown>
//#! A list of exceptions thrown if errors are discovered in the function.
//#! This tag will appear in the body of the header file, preceding the
//#! declaration of each function which throws an exception.
//    <li>
//    <li>
// </thrown>
//
// <todo asof="yyyy/mm/dd">
//#! A casacore::List of bugs, limitations, extensions or planned refinements.
//#! The programmer should fill in a date in the "asof" field, which
//#! will usually be the date at which the class is submitted for review.
//#! If, during the review, new "todo" items come up, then the "asof"
//#! date should be changed to the end of the review period.
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class SingleDishSkyCal : public SolvableVisCal
{
public:

    // Constructor
    SingleDishSkyCal(VisSet& vs);
    SingleDishSkyCal(const MSMetaInfoForCal& msmc);  // preferred ctor
    SingleDishSkyCal(const casacore::Int& nAnt);

    // Destructor
    virtual ~SingleDishSkyCal();

    // Return the type of this calibration matrix (actual type of derived class).
    //    (Must be implemented in specializations!)
    //
    // 2015/03/19 (gmoellen): Use VisCal::M here, so that it is sorted
    //    correctly in the VisEquation relative to "B TSYS"
    virtual Type type() { return VisCal::M; }

    // Return type name as string (ditto)
    virtual casacore::String typeName()     { return "SD SKY"; }
    virtual casacore::String longTypeName() { return "SD SKY (sky spectra)"; }

    // Return casacore::Matrix type
    // single dish calibration is antenna-based
    virtual VisCalEnum::MatrixType matrixType() { return VisCalEnum::JONES; }

    // Mueller matrix type (must be implemented in Mueller specializations!)
    virtual Mueller::MuellerType muellerType() { return Mueller::AddDiag2; }

    // Return the parameter type
    // so far single dish calibration is real
    virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; }

    // Number of pars per ant/bln
    //  virtual casacore::Int nPar() { return nCorr_[currSpw()]; }
    virtual casacore::Int nPar() { return 2; }

    // Total number of (complex) parameters per solve
    //  (specialize to jive with ant- or bln-basedness, etc.)
    virtual casacore::Int nTotalPar() { return nPar(); }

    // Does normalization by MODEL_DATA commute with this VisCal?
    //   (if so, permits pre-solve time-averaging)
    virtual casacore::Bool normalizable() { return false; }

    // Hazard a guess at the parameters (solveCPar) given the data
    virtual void guessPar(VisBuffer& vb);

    // Differentiate VB model w.r.t. Cal  parameters (no 2nd derivative yet)
    virtual void differentiate(CalVisBuffer& cvb);
    virtual void differentiate(VisBuffer& vb,
                     casacore::Cube<casacore::Complex>& V,
                     casacore::Array<casacore::Complex>& dV,
                     casacore::Matrix<casacore::Bool>& Vflg);

    // Differentiate VB model w.r.t. Source parameters
    virtual void diffSrc(VisBuffer& vb,
               casacore::Array<casacore::Complex>& dV);

    // Apply refant (implemented in SVJ)
    virtual void reReference() {}

    // Accumulate another VisCal onto this one
    virtual void accumulate(SolvableVisCal* incr,
              const casacore::Vector<casacore::Int>& fields);

    // Determine and apply flux density scaling
    virtual void fluxscale(const casacore::String& outfile,
        const casacore::Vector<casacore::Int>& refFieldIn,
        const casacore::Vector<casacore::Int>& tranFieldIn,
        const casacore::Vector<casacore::Int>& inRefSpwMap,
        const casacore::Vector<casacore::String>& fldNames,
        const casacore::Float& inGainThres,
        const casacore::String& antSel,
        const casacore::String& timerangeSel,
        const casacore::String& scanSel,
        fluxScaleStruct& oFluxScaleStruct,
        const casacore::String& oListFile,
        const casacore::Bool& incremental,
        const casacore::Int& fitorder,
        const casacore::Bool& display);

    // Use generic data gathering mechanism for solve
    virtual casacore::Bool useGenericGatherForSolve() { return false; }

    // Report state:
    virtual void listCal(const casacore::Vector<casacore::Int> ufldids, const casacore::Vector<casacore::Int> uantids,
               const casacore::Matrix<casacore::Int> uchanids,  //const casacore::Int& spw, const casacore::Int& chan,
               const casacore::String& listfile="",const casacore::Int& pagerows=50);


    // Local setApply
    using SolvableVisCal::setApply;
    virtual void setApply(const casacore::Record& apply);

    // In general, we are freq-dep
    virtual casacore::Bool freqDepPar() { return true; }

    // New CalTable handling
    virtual void keepNCT();

    // Self- gather and/or solve prototypes
    //  (triggered by useGenericGatherForSolve=F or useGenericSolveOne=F)
    virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve);

protected:

    // The number of sets of parameters under consideration
    // This states size of third axis of SolveAllRPar
    virtual casacore::Int& nElem() { return nAnt(); }

    // Number of Calibration matrices on ant/bln axis
    virtual casacore::Int nCalMat() { return nAnt(); }

    // Are the parameters the matrix elements?
    //   (or is a non-trivial calculation required?)
    //    (Must be implemented in specializations!)
    virtual casacore::Bool trivialMuellerElem() { return false; }

    // Initialize solve parameters (shape)
    virtual void initSolvePar();

    // Invalidate diff cal matrices generically
    inline virtual void invalidateDiffCalMat() {}

    // overwride syncMeta2
    virtual void syncMeta2(const vi::VisBuffer2& vb);

    // Sync matrices generically for current meta data
    virtual void syncCalMat(const casacore::Bool& doInv=false);

    // Synchronize the differentiated calibration
    virtual void syncDiffMat();

    // Synchronize weight scale factors
    virtual void syncWtScale();

    // Perform weight scale calculation (specializable)
    template<class ScalingScheme>
    void calcWtScale();

    // Normalize a (complex) solution array (generic)
    virtual casacore::Float calcPowerNorm(casacore::Array<casacore::Float>& amp, const casacore::Array<casacore::Bool>& ok);

    // Invalidate cal matrices generically
    virtual void invalidateCalMat() {}

    // Row-by-row apply to a casacore::Cube<casacore::Complex> (generic)
    virtual void applyCal(VisBuffer& vb, casacore::Cube<casacore::Complex>& Vout,casacore::Bool trial=false);
    virtual void applyCal2(vi::VisBuffer2& vb,
                 casacore::Cube<casacore::Complex>& Vout,casacore::Cube<casacore::Float>& Wout,
                 casacore::Bool trial=false);

    // Fill calibration table by processing reference data found within user selection
    virtual void fillCalibrationTable(casacore::MeasurementSet const &reference_data);

    // Fill calibration table by processing reference data found within user selection,
    // using DataRealComponentAccessor to access real component of data
    // stored either in DATA or FLOAT_DATA column
    template<class DataRealComponentAccessor>
    void fillCalibrationTable(casacore::MeasurementSet const &reference_data);

    // Access to current calibration data
    inline casacore::Cube<casacore::Complex> &currentSky() { return (*currentSky_[currSpw()]); }
    inline casacore::Cube<casacore::Bool> &currentSkyOK() { return (*currentSkyOK_[currSpw()]); }
    inline SkyCal<casacore::Complex, casacore::Complex> &engineC() { return (*engineC_[currSpw()]); }
    inline SkyCal<casacore::Float, casacore::Float> &engineF() { return (*engineF_[currSpw()]); }

    // Select reference data from user-selected data
    // Implementation is specific to each observing-mode
    virtual casacore::MeasurementSet selectReferenceData(casacore::MeasurementSet const &user_selection) = 0;

    // Current antenna
    casacore::Int currAnt_;
    casacore::Vector<casacore::Double> interval_;

    // Single Dish Calibration algebra wrapper (per Spw)
    casacore::PtrBlock<SkyCal<casacore::Complex, casacore::Complex> *> engineC_;
    casacore::PtrBlock<SkyCal<casacore::Float, casacore::Float> *> engineF_;

    // Current Sky spectra
    casacore::PtrBlock<casacore::Cube<casacore::Complex> *> currentSky_; // [nSpw]([1,2],nChanMat,nAnt)
    casacore::PtrBlock<casacore::Cube<casacore::Bool> *> currentSkyOK_;  // [nSpw]([1,2],nChanMat,nAnt)

private:
    void initializeSky();
    void finalizeSky();
    void updateWt2(casacore::Matrix<casacore::Float> &weight, const casacore::Int& antenna1);
    void initializeCorr();

    // number of correlations per spw
    casacore::Vector<casacore::Int> nCorr_;

    // list of timestamps and intervals for each observation, spw, and antenna
    // as a nested map with key obsId (outermost), spwId, antennaId (innermost)
    std::map<std::pair<casacore::Int, casacore::Int>, std::map<casacore::Int, casacore::Matrix<casacore::Double> > > wtScaleData_;
};

class SingleDishPositionSwitchCal : public SingleDishSkyCal 
{
public:

    // Constructor
    SingleDishPositionSwitchCal(VisSet& vs);
    SingleDishPositionSwitchCal(const MSMetaInfoForCal& msmc);  // preferred ctor
    SingleDishPositionSwitchCal(const casacore::Int& nAnt);

    // Destructor
    virtual ~SingleDishPositionSwitchCal();

    // Return type name as string (ditto)
    virtual casacore::String typeName()     { return "SDSKY_PS"; }
    virtual casacore::String longTypeName() { return "SDSKY_PS (position switch sky subtraction)"; }

    // Select reference data on top of user-specified selection,
    // for Single Dish observations consisting of:
    //     * on-the-fly mappings of science targets
    //     * with periodic executions of atmospheric calibrations scans inserted,
    //         * made at the reference position of the science target
    // Reference positions are assumed to be free of spectral-line emission
    virtual casacore::MeasurementSet selectReferenceData(casacore::MeasurementSet const &user_selection);

    // Implement fillCalibrationTable for SingleDishPositionSwitchCal
    virtual void fillCalibrationTable(casacore::MeasurementSet const &reference_data);
private:
    template<class DataRealComponentAccessor>
    void fillCalibrationTable(casacore::MeasurementSet const &reference_data);

};

class SingleDishRasterCal : public SingleDishSkyCal 
{
public:

    // Constructor
    SingleDishRasterCal(VisSet& vs);
    SingleDishRasterCal(const MSMetaInfoForCal& msmc);  // preferred ctor
    SingleDishRasterCal(const casacore::Int& nAnt);

    // Destructor
    virtual ~SingleDishRasterCal();

    // Return type name as string (ditto)
    virtual casacore::String typeName()     { return "SDSKY_RASTER"; }
    virtual casacore::String longTypeName() { return "SDSKY_RASTER (position switch sky subtraction specific to OTF raster observation)"; }

    // local setSolve
    virtual void setSolve(const casacore::Record& solve);

    // Reference data selection, specific to the otf raster observing mode
    virtual casacore::MeasurementSet selectReferenceData(casacore::MeasurementSet const &ms);

private:
    // edge detection parameter for otfraster mode
    casacore::Float fraction_;
    casacore::Int numEdge_;
};

class SingleDishOtfCal : public SingleDishSkyCal 
{
public:

    // Constructor
    SingleDishOtfCal(VisSet& vs);
    //  SingleDishOtfCal(const MSMetaInfoForCal& msmc);  // preferred ctor   ****ctor needs an MS!
    // Renaud: disabledSingleDishOtfCal(const casacore::Int& nAnt);

    // Destructor
    virtual ~SingleDishOtfCal();

    // Return type name as string (ditto)
    virtual casacore::String typeName()     { return "SDSKY_OTF"; }
    virtual casacore::String longTypeName() { return "SDSKY_OTF (position switch sky subtraction specific to OTF fast scan)"; }

    // Reference data selection, specific to the on-the-fly fast scan observing mode
    virtual casacore::MeasurementSet selectReferenceData(casacore::MeasurementSet const &user_selection);
    virtual void setSolve(const casacore::Record& solve);

private:
    // Edge detection parameters for otf mode
    casacore::Float fraction_;
    casacore::Float pixel_scale_;

    // casacore::MeasurementSet filtered with user-specified selection
    const casacore::MeasurementSet & msSel_ ;

};

} //# NAMESPACE CASA - END

#endif /* _SYNTHESIS_SINGLEDISH_SKY_CAL_H_ */


