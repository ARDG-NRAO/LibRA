//# PolAverageTVI.h: Transforming VI for polarization averaging
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the Implied warranty of MERCHANTABILITY or
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
//# $Id: $

#ifndef _MSVIS_POLAVERAGINGTVI_H_
#define _MSVIS_POLAVERAGINGTVI_H_

#include <casacore/casa/aips.h>
#include <msvis/MSVis/ViImplementation2.h>
#include <msvis/MSVis/TransformingVi2.h>
#include <msvis/MSVis/VisibilityIterator2.h>

#include <map>
#include <vector>

#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/Containers/Record.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { // # NAMESPACE VI - BEGIN

//# forward decl

class VisBuffer2;

class ChannelSelector;
class ChannelSelectorCache;
typedef casacore::Vector<casacore::Vector<casacore::Slice> > ChannelSlicer;
class SpectralWindowChannelsCache;
class SpectralWindowChannels;
class SubtableColumns;

class PolAverageVi2Factory;

// <summary>
// PolAverageTVI
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
// PolAverageTVI
// </etymology>
//
// <synopsis>
//
// </synopsis>
//
// <example>
// <code>
// //
// </code>
// </example>
//
// <motivation>
//
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
// </todo>

class PolAverageTVI: public TransformingVi2 {

public:

  // Destructor

  virtual ~PolAverageTVI();

  // Report the the ViImplementation type
  //  (should be specialized in child classes)
  virtual casacore::String ViiType() const {
    return casacore::String("PolAverage( ") + getVii()->ViiType() + " )";
  }

  // Methods to control and monitor subchunk iteration

  virtual void origin();
  virtual void next();

  // Override the following methods to always return Stokes::I
  virtual void corrType(casacore::Vector<casacore::Int> & corrTypes) const;
  virtual casacore::Vector<casacore::Int> getCorrelations() const;
  virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesDefined() const;
  virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesSelected() const;

  // Override the number of correlations per shape. This TVI just generates one output shape
  const casacore::Vector<casacore::Int>& nCorrelationsPerShape() const;

  // POLARIZATION table will have additional entry nPolarizationIds() should
  // return original number plus one
  // NB: nDataDescriptionIds() will not be affected
  virtual casacore::Int nPolarizationIds() const {
    return TransformingVi2::nPolarizationIds() + 1;
  }

  // Polarization Id will point to the new row to be added (to the end)
  virtual casacore::Int polarizationId() const {
    return nPolarizationIds() - 1;
  }

  // Return row flag

  virtual void flagRow (casacore::Vector<casacore::Bool> & rowflags) const;

  // Return flag for each polarization, channel and row

  virtual void flag(casacore::Cube<casacore::Bool> & flags) const;

  // Return flag for each channel & row

  virtual void flag(casacore::Matrix<casacore::Bool> & flags) const;

  // Return feed configuration matrix for specified antenna
  // Must fail
  virtual void jonesC(
      casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones) const;

  // Return sigma

  virtual void sigma(casacore::Matrix<casacore::Float> & sigmat) const;

  // Return the visibilities as found in the casacore::MS, casacore::Cube (npol,nchan,nrow).

  virtual void visibilityCorrected(
  casacore::Cube<casacore::Complex> & vis) const;
  virtual void visibilityModel(casacore::Cube<casacore::Complex> & vis) const;
  virtual void visibilityObserved(
  casacore::Cube<casacore::Complex> & vis) const;

  // Return FLOAT_DATA as a casacore::Cube (npol, nchan, nrow) if found in the MS.

  virtual void floatData(casacore::Cube<casacore::Float> & fcube) const;

  // Return the shape of the visibility Cube

  virtual casacore::IPosition visibilityShape() const;

  // Return weight
  // TODO

  virtual void weight(casacore::Matrix<casacore::Float> & wtmat) const;

  // Return weightspectrum (a weight for each channel)
  // TODO

  virtual void weightSpectrum(casacore::Cube<casacore::Float> & wtsp) const;
  virtual void sigmaSpectrum(casacore::Cube<casacore::Float> & wtsp) const;

  // Return imaging weight (a weight for each channel)
  // virtual casacore::Matrix<casacore::Float> & imagingWeight (casacore::Matrix<casacore::Float> & wt) const;

  virtual const VisImagingWeight & getImagingWeightGenerator() const;

protected:

  // Constructor

  PolAverageTVI(ViImplementation2 * inputVi);

  // transform data (DATA, CORRECTED_DATA, MODEL_DATA, FLOAT_DATA)
  virtual void transformComplexData(
  casacore::Cube<casacore::Complex> const &dataIn,
  casacore::Cube<casacore::Bool> const &flagIn,
  casacore::Cube<casacore::Complex> &dataOut) const = 0;
  virtual void transformFloatData(casacore::Cube<casacore::Float> const &dataIn,
  casacore::Cube<casacore::Bool> const &flagIn,
  casacore::Cube<casacore::Float> &dataOut) const = 0;

  // transform weight (WEIGHT, WEIGHT_SPECTRUM)
  // TODO
  virtual void transformWeight(casacore::Array<casacore::Float> const &weightIn,
  casacore::Array<casacore::Float> &weightOut) const = 0;

  void configureShapes();

  // Flags (per ms, per data description) whether transformation must be executed or not
  // condition:
  //
  //   1. if polarization type is neither Linear nor Circular, do not transform
  //   2. if visibility only contain one polarization component, do not transform
  //   3. if visibility contains cross-polarization (XY, YX, RL, LR), do not
  //      take into account it
  //
//  casacore::Vector<Vector<Bool> > doTransform_;
  casacore::Vector<casacore::Bool> doTransform_;

  // List of polarization ids that points either (XX,YY) or (RR,LL)
//  Vector<Vector<uInt> > polId0;
//  Vector<Vector<uInt> > polId1;
  casacore::Vector<casacore::Int> polId0_;casacore::Vector<casacore::Int> polId1_;

  casacore::Vector<casacore::Int> nCorrelationsPerShape_;

private:

  // Properly fill doTransform_, polId0_, and polId1_ based on current MS
  void configurePolAverage();

  // Reconfigure if necessary
  void reconfigurePolAverageIfNecessary() {
    if (isNewMs()) {
      configurePolAverage();
    }
  }

  // warn if current dd is inappropriate for polarization averaging
  void warnIfNoTransform();

  friend PolAverageVi2Factory;
};

class GeometricPolAverageTVI: public PolAverageTVI {
public:
  GeometricPolAverageTVI(ViImplementation2 * inputVi);
  ~GeometricPolAverageTVI();

  // Report the the ViImplementation type
  //  (should be specialized in child classes)
  virtual casacore::String ViiType() const {
    return casacore::String("GeometricPolAverage( ") + getVii()->ViiType()
        + " )";
  }

  // transform data (DATA, CORRECTED_DATA, MODEL_DATA, FLOAT_DATA)
  virtual void transformComplexData(
  casacore::Cube<casacore::Complex> const &dataIn,
  casacore::Cube<casacore::Bool> const &flagIn,
  casacore::Cube<casacore::Complex> &dataOut) const;
  virtual void transformFloatData(casacore::Cube<casacore::Float> const &dataIn,
  casacore::Cube<casacore::Bool> const &flagIn,
  casacore::Cube<casacore::Float> &dataOut) const;

  // transform weight (WEIGHT, WEIGHT_SPECTRUM)
  // TODO
  virtual void transformWeight(casacore::Array<casacore::Float> const &weightIn,
  casacore::Array<casacore::Float> &weightOut) const;

protected:

  template<class T>
  void transformData(casacore::Cube<T> const &dataIn,
  casacore::Cube<casacore::Bool> const &flagIn,
  casacore::Int pid0,
  casacore::Int pid1,
  casacore::Cube<T> &dataOut) const;
};

class StokesPolAverageTVI: public PolAverageTVI {
public:
  StokesPolAverageTVI(ViImplementation2 * inputVi);
  ~StokesPolAverageTVI();

  // Report the the ViImplementation type
  //  (should be specialized in child classes)
  virtual casacore::String ViiType() const {
    return casacore::String("StokesPolAverage( ") + getVii()->ViiType() + " )";
  }

  // transform data (DATA, CORRECTED_DATA, MODEL_DATA, FLOAT_DATA)
  virtual void transformComplexData(
  casacore::Cube<casacore::Complex> const &dataIn,
  casacore::Cube<casacore::Bool> const &flagIn,
  casacore::Cube<casacore::Complex> &dataOut) const;
  virtual void transformFloatData(casacore::Cube<casacore::Float> const &dataIn,
  casacore::Cube<casacore::Bool> const &flagIn,
  casacore::Cube<casacore::Float> &dataOut) const;

  // transform weight (WEIGHT, WEIGHT_SPECTRUM)
  // TODO
  virtual void transformWeight(casacore::Array<casacore::Float> const &weightIn,
  casacore::Array<casacore::Float> &weightOut) const;

protected:

  template<class T>
  void transformData(casacore::Cube<T> const &dataIn,
  casacore::Cube<casacore::Bool> const &flagIn,
  casacore::Int pid0,
  casacore::Int pid1,
  casacore::Cube<T> &dataOut) const;
};

// <summary>
// A factory for generating ViImplementation2 for polarization averaging.
// </summary>
//
// <use visibility=export>
//
// <prerequisite>
//   <li> <linkto class="VisibilityIterator2:description">VisibilityIterator2</linkto>
// </prerequisite>
//
// <etymology>
// Factory for layered ViImplementation2 construction
// </etymology>
//
// <synopsis>
// PolAverageVi2Factory
// </synopsis>
//
// <motivation>
//
// </motivation>
//
// <example>
//
// </example>

class PolAverageVi2Factory: public ViFactory {

public:
  // Constructor
  PolAverageVi2Factory(casacore::Record const &configuration,
      ViImplementation2 *inputVII);
  PolAverageVi2Factory(casacore::Record const &configuration,
  casacore::MeasurementSet const *ms, SortColumns const sortColumns,
  casacore::Double timeInterval, casacore::Bool isWritable);

  // Destructor
  ~PolAverageVi2Factory();

  ViImplementation2 * createVi() const;

private:
  enum AveragingMode {
    GEOMETRIC, STOKES, NUM_MODES, DEFAULT = STOKES
  };

  static AveragingMode GetAverageModeFromConfig(
  casacore::Record const &configuration) {
    if (configuration.isDefined("mode")) {
      casacore::String mode = configuration.asString("mode");
      mode.downcase();
      if (mode == "geometric") {
        return AveragingMode::GEOMETRIC;
      } else if (mode == "stokes") {
        return AveragingMode::STOKES;
      } else if (mode == "default") {
        return AveragingMode::DEFAULT;
      } else {
        return AveragingMode::NUM_MODES;
      }
    }
    return AveragingMode::DEFAULT;
  }

  ViImplementation2 *inputVII_p;

  AveragingMode mode_;
};

class PolAverageTVILayerFactory: public ViiLayerFactory {

public:
  PolAverageTVILayerFactory(casacore::Record const &configuration);
  virtual ~PolAverageTVILayerFactory() {
  }
  ;

protected:

  ViImplementation2 * createInstance(ViImplementation2* vii0) const;

  casacore::Record configuration_p;

};

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

#endif // _MSVIS_POLAVERAGINGTVI_H_

