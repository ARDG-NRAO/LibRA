//# SkyCal.h: this defines <ClassName>, which ...
//# Copyright (C) 2003
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
//#
//# $Id$

#ifndef _SYNTHESIS_SKY_CAL_H_
#define _SYNTHESIS_SKY_CAL_H_

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Exceptions/Error.h>

namespace casa { //# NAMESPACE CASA - BEGIN

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

template<class DataType, class CalDataType>
class SkyCal
{
  // all the member functions are inline
public:
  SkyCal()
  : npol_(2),
    nchan_(1),
    m0_(NULL), 
    ok0_(NULL),
    m_(NULL), 
    mi_(NULL), 
    ok_(NULL),
    oki_(NULL),
    cOne_(1.0), 
    cZero_(0.0), 
    scalardata_(false)
  {}

  virtual ~SkyCal() {}

  // stride
  // In Mueller series, typesize is defined as "number of polarizations"
  // while SkyCal definition is "number of elements in the DATA cell",
  // which is npol * nchan in practice.
  casacore::uInt typesize() const { return npol_ * nchan_; }
  void setNumChannel(casacore::uInt n) { nchan_ = n; }
  void setNumPolarization(casacore::uInt n) { npol_ = n; }

  // Set scalardata_ 
  //  TBD: Handle this better; for now, we need to set this from
  //       an external call so we handle single-corr data properly
  //       when setting non-corr-dep flags
  void setScalarData(casacore::Bool scalardata) const { scalardata_=scalardata; }
  
  // Synchronize with leading element in external array
  void sync(CalDataType& mat) { m0_=&mat; origin(); }
  void sync(CalDataType& mat, casacore::Bool& ok) { m0_=&mat; ok0_=&ok; origin(); }
  
  // Reset to origin
  void origin() {m_=m0_;ok_=ok0_;}
  
  // Increment to next vector (according to len)
  // In practice, this operator increments row index
  void operator++()    { m_+=typesize(); if (ok_) ok_+=typesize();}
  void operator++(int) { m_+=typesize(); if (ok_) ok_+=typesize();}

  // Advance step matrices forward (according to len)
  void advance(const casacore::Int& step) { m_+=(step*typesize()); if (ok_) ok_+=(step*typesize());}

  // In-place invert
  void invert() {}

  // Set matrix elements according to ok flag
  //  (so we don't have to check ok flags atomically in apply)
  void setMatByOk()
  {
    throw(casacore::AipsError("Illegal use of SkyCal::setMatByOk"));
  }

  // In-place multiply onto a data with flag information
  // apply implements position switch calibration: (ON - OFF)/OFF
  //
  // This processes the data corresponding to each DATA cell
  // (npol * nchan) together in contrast to Mueller series, which
  // processes one casacore::Stokes vector, i.e., process each channel
  // individually.
  void apply(casacore::Matrix<DataType>& v, casacore::Matrix<casacore::Bool> &f)
  {
    if (f.shape() == v.shape()) {
      flag(f);
    }
    
    constexpr size_t nPolCal = 2;

    casacore::Bool deleteIt;
    DataType *data = v.getStorage(deleteIt);
    for (size_t i = 0; i < nchan_; ++i) {
      auto const iData = i * npol_;
      auto const iMat = i * nPolCal;
      for (size_t j = 0; j < npol_; ++j) {
        // (ON - OFF) / OFF
        data[iData+j] = (data[iData+j] - m_[iMat+j]) / m_[iMat+j];
      }
    }
    v.putStorage(data, deleteIt);
  }

  void apply(casacore::Matrix<DataType>& v, casacore::Matrix<casacore::Bool> &f, casacore::Vector<casacore::Bool>& vflag)
  {
    if (!ok_) throw(casacore::AipsError("Illegal use of SkyCal::applyFlag."));
    
    applyFlag(vflag);
    apply(v, f);
  }

  // Apply only flags according to cal flags
  //
  // Similar to apply, flagging also processes each DATA cell together.
  void applyFlag(casacore::Vector<casacore::Bool>& vflag)
  {
    if (!ok_) throw(casacore::AipsError("Illegal use of SkyCal::applyFlag(vflag)."));

    constexpr size_t nPolCal = 2;

    if (scalardata_) {
      for (size_t i = 0; i < nchan_; ++i) {
        vflag[i] |= (!ok_[i*nPolCal]);
      }
    }
    else {
      for (size_t i = 0; i < nchan_; ++i) {
        for (size_t j = 0; j < npol_; ++j) {
          vflag[i] |= !(ok_[i*nPolCal + j]);
        }
      }
    }
  }
  
  void flag(casacore::Matrix<casacore::Bool>& v)
  {
    constexpr size_t nPolCal = 2;

    casacore::Bool deleteIt;
    casacore::Bool *data = v.getStorage(deleteIt);
    for (size_t i = 0; i < nchan_; ++i) {
      auto const iData = i * npol_;
      auto const iMat = i * nPolCal;
      for (size_t j = 0; j < npol_; ++j) {
        data[iData+j] |= (!ok_[iMat+j]); // data: false is valid, ok_: true is valid
      }
    }
    v.putStorage(data, deleteIt);
  }

  // Multiply onto a vis VisVector, preserving input (copy then in-place apply)
  void apply(casacore::Matrix<DataType>& out, casacore::Matrix<casacore::Bool> &outFlag,
                     const casacore::Matrix<DataType>& in, const casacore::Matrix<casacore::Bool> &inFlag)
  {
    out = in;
    outFlag = inFlag;
    apply(out, outFlag);
  }

  // print it out
    // unused
  /*
  friend std::ostream& operator<<(std::ostream& os, const SkyCal<DataType, CalDataType>& mat)
  {
    return os;
  }
  */

protected:

private:
  casacore::uInt npol_;
  casacore::uInt nchan_;
  
  // Pointer to origin
  CalDataType *m0_;
  casacore::Bool *ok0_;

  // Moving pointer
  CalDataType *m_, *mi_;
  casacore::Bool *ok_, *oki_;

  // casacore::Complex unity, zero (for use in invert and similar methods)
  const CalDataType cOne_,cZero_;

  mutable casacore::Bool scalardata_;

  // Copy ctor protected 
  SkyCal(const SkyCal<DataType, CalDataType>& mat);
};

} //# NAMESPACE CASA - END

//#include <synthesis/MeasurementComponents/SkyCal.tcc>

#endif /* _SYNTHESIS_SKY_CAL_H_ */



