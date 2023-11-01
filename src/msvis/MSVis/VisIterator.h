//# VisIterator.h: Step through the MeasurementEquation by visibility
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: VisIterator.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#ifndef MSVIS_VISITERATOR_H
#define MSVIS_VISITERATOR_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/measures/Measures/MeasConvert.h>
#include <casacore/casa/Quanta/MVDoppler.h>
#include <casacore/measures/Measures/MCDoppler.h>
#include <casacore/measures/Measures/MDoppler.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/scimath/Mathematics/SquareMatrix.h>
#include <casacore/scimath/Mathematics/RigidVector.h>

#include <casacore/ms/MSOper/MSDerivedValues.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <casacore/ms/MeasurementSets/MSIter.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# forward decl
class VisBuffer;

// <summary>
// ROVisIterator iterates through one or more readonly MeasurementSets
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="MSIter">MSIter</linkto>
//   <li> <linkto class="casacore::MeasurementSet">casacore::MeasurementSet</linkto>
//   <li> <linkto class="VisSet">VisSet</linkto>
// </prerequisite>
//
// <etymology>
// The ROVisIterator is a readonly iterator returning visibilities
// </etymology>
//
// <synopsis>
// ROVisIterator provides iteration with various sort orders
// for one or more MSs. It has member functions to retrieve the fields
// commonly needed in synthesis calibration and imaging.
//
// One should use <linkto class="VisBuffer">VisBuffer</linkto>
// to access chunks of data.
// </synopsis>
//
// <example>
// <code>
// //
// </code>
// </example>
//
// <motivation>
// For imaging and calibration you need to access an casacore::MS in some consistent
// order (by field, spectralwindow, time interval etc.). This class provides
// that access.
//
// Unlike ROVisibilityIterator, this supports non-strided in-row selection
// (e.g., of channels and correlations).
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="1997/05/30">
//   <li> cleanup the currently dual interface for visibilities and flags
//   <li> sort out what to do with weights when interpolating
//   <li> Handle the multi-casacore::MS case like ROVisibilityIterator does.
// </todo>

class ROVisIteratorImpl;

class ROVisIterator : public ROVisibilityIterator
{
  public:

  // Default constructor - useful only to assign another iterator later
  ROVisIterator();

  // Construct from casacore::MS and a casacore::Block of casacore::MS column enums specifying the iteration
  // order, if none are specified, time iteration is implicit.  An optional
  // timeInterval can be given to iterate through chunks of time.  The default
  // interval of 0 groups all times together.  Every 'chunk' of data contains
  // all data within a certain time interval (in seconds) and with identical
  // values of the other iteration columns (e.g.  DATA_DESC_ID and FIELD_ID).
  // Using selectChannel(), a number of groups of channels can be requested.
  // At present the channel group iteration will always occur before the
  // interval iteration.
  ROVisIterator(const casacore::MeasurementSet& ms, 
		const casacore::Block<casacore::Int>& sortColumns, 
		casacore::Double timeInterval=0);

  ROVisIterator(const casacore::MeasurementSet & ms, const casacore::Block<casacore::Int>& sortColumns,
                casacore::Double timeInterval, const ROVisibilityIterator::Factory & factory);


  // Copy construct. This calls the assignment operator.
  ROVisIterator(const ROVisIterator & other);

  // Destructor
  ~ROVisIterator();

  // Assignment. Any attached VisBuffers are lost in the assign.
  ROVisIterator & operator=(const ROVisIterator &other);

  // Members
  
  // Advance iterator through data
  ROVisIterator & operator++(int);
  ROVisIterator & operator++();

  // Return channel numbers in selected VisSet spectrum
  // (i.e. disregarding possible selection on the iterator, but
  //  including the selection set when creating the VisSet)

  casacore::Vector<casacore::Int>& chanIds(casacore::Vector<casacore::Int>& chanids) const;
  casacore::Vector<casacore::Int>& chanIds(casacore::Vector<casacore::Int>& chanids,casacore::Int spw) const;

  // Return selected correlation indices
  casacore::Vector<casacore::Int>& corrIds(casacore::Vector<casacore::Int>& corrids) const;

  // Return the correlation type (returns casacore::Stokes enums)
  casacore::Vector<casacore::Int>& corrType(casacore::Vector<casacore::Int>& corrTypes) const;

  // Set up new chan/corr selection via casacore::Vector<casacore::Slice>
  void selectChannel(const casacore::Vector<casacore::Vector<casacore::Slice> >& chansel);
  void selectCorrelation(const casacore::Vector<casacore::Vector<casacore::Slice> >& corrsel);

  // Set up/return channel averaging bounds 
  casacore::Vector<casacore::Matrix<casacore::Int> >& setChanAveBounds(casacore::Float factor, casacore::Vector<casacore::Matrix<casacore::Int> >& bounds);

  // Get selected spws and channel counts
  void allSelectedSpectralWindows (casacore::Vector<casacore::Int> & spws, casacore::Vector<casacore::Int> & nvischan);
  void lsrFrequency(const casacore::Int& spw, casacore::Vector<casacore::Double>& freq, casacore::Bool& convert, const  casacore::Bool ignoreconv=false);

  // The following throws an exception, because this isn't the
  // language of channel selection in VisIterator
  void getChannelSelection(casacore::Block< casacore::Vector<casacore::Int> >&,
                           casacore::Block< casacore::Vector<casacore::Int> >&,
                           casacore::Block< casacore::Vector<casacore::Int> >&,
                           casacore::Block< casacore::Vector<casacore::Int> >&,
                           casacore::Block< casacore::Vector<casacore::Int> >&) 
  { throw(casacore::AipsError("VisIterator::getChannelSelection: you can't do that!")); };


  // Return number of chans/corrs per spw/pol
  casacore::Int numberChan(casacore::Int spw) const;
  casacore::Int numberCorr(casacore::Int pol) const;

protected:

  class Factory : public ROVisibilityIterator::Factory {
  public:

      Factory (ROVisIterator * vi) : vi_p (vi) {}
      VisibilityIteratorReadImpl *
      operator() (const asyncio::PrefetchColumns * prefetchColumns,
                  const casacore::Block<casacore::MeasurementSet>& mss,
                  const casacore::Block<casacore::Int>& sortColumns,
                  const casacore::Bool addDefaultSortCols,
                  casacore::Double timeInterval) const;
  private:

      ROVisIterator * vi_p;

  };

  void getDataColumn(DataColumn whichOne, const casacore::Vector<casacore::Vector<casacore::Slice> >& slices, casacore::Cube<casacore::Complex>& data) const;
  virtual ROVisIteratorImpl * getReadImpl () const;
};

// <summary>
// VisIterator iterates through one or more writable MeasurementSets
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="ROVisIterator">ROVisIterator</linkto>
// </prerequisite>
//
// <etymology>
// The VisIterator is a read/write iterator returning visibilities
// </etymology>
//
// <synopsis>
// VisIterator provides iteration with various sort orders
// for one or more MSs. It has member functions to retrieve the fields
// commonly needed in synthesis calibration and imaging. It is 
// derived from the read-only iterator
// <linkto class="ROVisIterator">ROVisIterator</linkto>.
//
// One should use <linkto class="VisBuffer">VisBuffer</linkto>
// to access chunks of data.
// </synopsis>
//
// <example>
// <code>
// //
// </code>
// </example>
//
// <motivation>
// For imaging and calibration you need to access an casacore::MS in some consistent
// order (by field, spectralwindow, time interval etc.). This class provides
// that access.
//
// Unlike ROVisibilityIterator, this supports non-strided in-row selection
// (e.g., of channels).
// </motivation>
//
// #<thrown>
//
// #</thrown>
//
// <todo asof="1997/05/30">
//   <li> cleanup the currently dual interface for visibilities and flags
//   <li> sort out what to do with weights when interpolating
//   <li> Handle the multi-casacore::MS case like VisibilityIterator does.
// </todo>

class VisIteratorImpl;

class VisIterator : public ROVisIterator
{
public:

  // Constructors.
  // Note: The VisIterator is not initialized correctly by default, you
  // need to call origin() before using it to iterate.
  VisIterator();
  VisIterator(casacore::MeasurementSet & ms, const casacore::Block<casacore::Int>& sortColumns, 
	      casacore::Double timeInterval=0);

  VisIterator(const VisIterator & MSI);

  // Destructor
  virtual ~VisIterator();

  VisIterator & operator=(const VisIterator &MSI);

  // Members
  
  // Advance iterator through data
  VisIterator & operator++(int);
  VisIterator & operator++();

  // Set/modify the flag row column; dimension casacore::Vector(nrow)
  void setFlagRow(const casacore::Vector<casacore::Bool>& rowflags);

  // Set/modify the flags in the data.
  // This sets the flags as found in the casacore::MS, casacore::Cube(npol,nchan,nrow),
  // where nrow is the number of rows in the current iteration (given by
  // nRow()).
  void setFlag(const casacore::Cube<casacore::Bool>& flag);

  // Set/modify the visibilities
  // This sets the data as found in the casacore::MS, casacore::Cube(npol,nchan,nrow).
  void setVis(const casacore::Cube<casacore::Complex>& vis, DataColumn whichOne);

  // Set the visibility and flags, and interpolate from velocities if needed
  void setVisAndFlag(const casacore::Cube<casacore::Complex>& vis, const casacore::Cube<casacore::Bool>& flag,
		     DataColumn whichOne);

  // Set/modify the weightMat
  void setWeightMat(const casacore::Matrix<casacore::Float>& wtmat);

  // Set/modify the weightSpectrum
  void setWeightSpectrum(const casacore::Cube<casacore::Float>& wtsp);

protected:

  class Factory : public ROVisibilityIterator::Factory {
  public:

      Factory (VisIterator * vi) : vi_p (vi) {}
      VisibilityIteratorReadImpl *
      operator() (const asyncio::PrefetchColumns * prefetchColumns,
                  const casacore::Block<casacore::MeasurementSet>& mss,
                  const casacore::Block<casacore::Int>& sortColumns,
                  const casacore::Bool addDefaultSortCols,
                  casacore::Double timeInterval) const;
  private:

      VisIterator * vi_p;

  };

  virtual void attachColumns(const casacore::Table &t);

  // deals with casacore::Float or casacore::Complex observed data (DATA and FLOAT_DATA).
  void putDataColumn(DataColumn whichOne, const casacore::Vector<casacore::Vector<casacore::Slice> >& slices,
		     const casacore::Cube<casacore::Complex>& data);
  void putDataColumn(DataColumn whichOne, const casacore::Cube<casacore::Complex>& data);

  // column access functions
  virtual void putCol(casacore::ScalarColumn<casacore::Bool> &column, const casacore::Vector<casacore::Bool> &array);

  virtual void putCol(casacore::ArrayColumn<casacore::Bool> &column, const casacore::Array<casacore::Bool> &array);
  virtual void putCol(casacore::ArrayColumn<casacore::Float> &column, const casacore::Array<casacore::Float> &array);
  virtual void putCol(casacore::ArrayColumn<casacore::Complex> &column, const casacore::Array<casacore::Complex> &array);

  virtual void putCol(casacore::ArrayColumn<casacore::Bool> &column, const casacore::Slicer &slicer, const casacore::Array<casacore::Bool> &array);
  virtual void putCol(casacore::ArrayColumn<casacore::Float> &column, const casacore::Slicer &slicer, const casacore::Array<casacore::Float> &array);
  virtual void putCol(casacore::ArrayColumn<casacore::Complex> &column, const casacore::Slicer &slicer, const casacore::Array<casacore::Complex> &array);

  VisIteratorImpl * getImpl () const;
};




} //# NAMESPACE CASA - END

#endif
