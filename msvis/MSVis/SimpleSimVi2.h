//# SimpleSimVi2.h: Rudimentary data simulator
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
//# $Id: VisibilityIterator2.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#if ! defined (MSVIS_SimplSimVi2_H)
#define MSVIS_SimplSimVi2_H

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>
#include <casacore/ms/MeasurementSets/MSDataDescColumns.h>
#include <casacore/ms/MeasurementSets/MSPolColumns.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/ViImplementation2.h>
#include <msvis/MSVis/ViiLayerFactory.h>
#include <msvis/MSVis/VisBufferImpl2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/Stokes.h>
#include <casacore/casa/Arrays.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

#include <map>
#include <vector>

#define SSVi2NotPossible() ThrowCc("Not Possible in SimpleSimVi2"); 
#define SSVi2NotYetImplemented() ThrowCc("Not yet implemented in SimpleSimVi2");

namespace casacore{

class AipsError;
class RecordInterface;
class String;
template <typename T, Int n> class SquareMatrix;

}

namespace casa { //# NAMESPACE CASA - BEGIN

class VisImagingWeight;

namespace vi {


//# forward decl

class Subchunk;
class VisBuffer2;


class SimpleSimVi2Parameters {

public:

  // Default ctor (sets default values, useful for self-testing)
  SimpleSimVi2Parameters();

  // Simple, shape-oriented ctor
  SimpleSimVi2Parameters(casacore::Int nField,casacore::Int nScan,
                         casacore::Int nSpw, casacore::Int nAnt, casacore::Int nCorr,
                         const casacore::Vector<casacore::Int>& nTimePerField,
                         const casacore::Vector<casacore::Int>& nChan,
                         casacore::Complex c0=casacore::Complex(0.0f),
                         casacore::String polBasis="circ",
                         casacore::Bool autoPol=false,casacore::Bool doParang=false,
                         casacore::Bool doAC=false);

  // Full control
  SimpleSimVi2Parameters(casacore::Int nField,casacore::Int nScan,casacore::Int nSpw,
                         casacore::Int nAnt,casacore::Int nCorr,
                         const casacore::Vector<casacore::Int>& nTimePerField,
                         const casacore::Vector<casacore::Int>& nChan,
                         casacore::String date0, casacore::Double dt,
                         const casacore::Vector<casacore::Double>& refFreq,
                         const casacore::Vector<casacore::Double>& df,
                         const casacore::Matrix<casacore::Float>& stokes,
                         casacore::Bool doNoise,
                         const casacore::Matrix<casacore::Float>& gain,
                         const casacore::Matrix<casacore::Float>& tsys,
                         casacore::Bool doNorm=true,
                         casacore::String polBasis="circ", casacore::Bool doAC=false,
                         casacore::Complex c0 = casacore::Complex(0.0f),
                         casacore::Bool doParang=false,
                         MetadataScope spwScope = ChunkScope,
                         MetadataScope antennaScope = RowScope);
  
  SimpleSimVi2Parameters(const SimpleSimVi2Parameters& other);
  SimpleSimVi2Parameters& operator=(const SimpleSimVi2Parameters& other);

  ~SimpleSimVi2Parameters();


  void summary() const;

  casacore::Int nField_, nScan_, nSpw_, nAnt_, nCorr_;
  casacore::Vector<casacore::Int> nTimePerField_, nChan_;
  casacore::String date0_; 
  casacore::Double dt_;
  casacore::Vector<casacore::Double> refFreq_, df_;
  casacore::Bool doNoise_;
  casacore::Matrix<casacore::Float> stokes_, gain_, tsys_;
  casacore::Bool doNorm_;
  casacore::String polBasis_;
  casacore::Bool doAC_;
  casacore::Complex c0_;
  casacore::Bool autoPol_;   // set non-trivial linear polarization 
  casacore::Bool doParang_;  // Simple linear-in-time, for now
  MetadataScope spwScope_; // is SPW constant on each chunk, subchunk or row?
  MetadataScope antennaScope_; // are ANTENNA1, ANTENNA2 constant on each subchunk or row? (chunk scope not supported for the time being)

  // Return frequencies for specified spw
  casacore::Vector<casacore::Double> freqs(casacore::Int spw) const;

private:
  void initialize(const casacore::Vector<casacore::Int>& nTimePerField, const casacore::Vector<casacore::Int>& nChan,
		  const casacore::Vector<casacore::Double>& refFreq, const casacore::Vector<casacore::Double>& df,
		  const casacore::Matrix<casacore::Float>& stokes, 
		  const casacore::Matrix<casacore::Float>& gain, const casacore::Matrix<casacore::Float>& tsys);

};

class SimpleSimVi2 : public ViImplementation2 {

public:

  SimpleSimVi2 (const SimpleSimVi2Parameters& pars);

  // make noncopyable...
  SimpleSimVi2( const SimpleSimVi2& ) = delete;
  SimpleSimVi2& operator=( const SimpleSimVi2& ) = delete;

  // Destructor
  virtual ~SimpleSimVi2 ();

  // Report the the ViImplementation type
  virtual casacore::String ViiType() const override {return casacore::String("Simulated(*)"); };

  //   +==================================+
  //   |                                  |
  //   | Iteration Control and Monitoring |
  //   |                                  |
  //   +==================================+
  

  // Methods to control and monitor subchunk iteration

  virtual void origin () override;
  virtual casacore::Bool more () const override;
  virtual void next () override;
  virtual Subchunk getSubchunkId () const override;
  
  // Methods to control chunk iterator

  virtual void originChunks (casacore::Bool forceRewind = false) override;
  virtual casacore::Bool moreChunks () const override;
  virtual void nextChunk () override;

  virtual void result(casacore::Record& res) const override;

  // Detecting the key change isn't possible (yet?)  
  virtual casacore::String keyChange() const override { SSVi2NotPossible() };

  virtual casacore::Bool isWritable () const override { return false; };

  // Return the time interval (in seconds) used for iteration.
  // This is not the same as the INTERVAL column.  Setting the
  // the interval requires calling origin chunks before performing
  // further iterator.
  
  virtual casacore::Double getInterval() const override {return 1.0e9; };
  virtual void setInterval (casacore::Double) override { SSVi2NotPossible() };

  // Select the channels to be returned.  Requires calling originChunks before
  // performing additional iteration.
  
  virtual void setFrequencySelections (const FrequencySelections & selection) override;

  // Set the 'blocking' size for returning data.
  virtual void setRowBlocking (casacore::rownr_t) override { SSVi2NotPossible() };
  
  virtual casacore::Bool existsColumn (VisBufferComponent2 id) const override;
  
  virtual const SortColumns & getSortColumns() const override { SSVi2NotPossible() };

  virtual casacore::Bool isNewArrayId () const override { return false; };
  virtual casacore::Bool isNewFieldId () const override { return thisField_!=lastField_; };
  virtual casacore::Bool isNewMs () const override  { return false; };
  virtual casacore::Bool isNewSpectralWindow () const override { return thisSpw_!=lastSpw_; };

  // Return the number of rows in the current iteration
  virtual casacore::rownr_t nRows () const override;
  
  // Return the number of distinct cube/array shapes in the current iteration
  virtual casacore::rownr_t nShapes () const override;
  
  // Return the number of rows for each distinct array/cube shapes in the current iteration
  virtual const casacore::Vector<casacore::rownr_t>& nRowsPerShape () const override;

  // Return the number of channels for each distinct array/cube shapes in the current iteration
  virtual const casacore::Vector<casacore::Int>& nChannelsPerShape () const override;

  // Return the number of correlations for each distinct array/cube shapes in the current iteration
  virtual const casacore::Vector<casacore::Int>& nCorrelationsPerShape () const override;

  // Return the row ids as from the original root table. This is useful
  // to find correspondance between a given row in this iteration to the
  // original ms row

  virtual void getRowIds (casacore::Vector<casacore::rownr_t> & rowids) const override;

  virtual VisBuffer2 * getVisBuffer () const override;


  //   +=========================+
  //   |                         |
  //   | Subchunk casacore::Data Accessors |
  //   |                         |
  //   +=========================+
  
  // Return info
  virtual void antenna1 (casacore::Vector<casacore::Int> & ant1) const override;
  virtual void antenna2 (casacore::Vector<casacore::Int> & ant2) const override;
  virtual void corrType (casacore::Vector<casacore::Int> & corrTypes) const override;
  virtual casacore::Int  dataDescriptionId () const override;
  virtual void dataDescriptionIds (casacore::Vector<casacore::Int> & ddis) const override;
  virtual void exposure (casacore::Vector<casacore::Double> & expo) const override;
  virtual void feed1 (casacore::Vector<casacore::Int> & fd1) const override;
  virtual void feed2 (casacore::Vector<casacore::Int> & fd2) const override;
  virtual void fieldIds (casacore::Vector<casacore::Int>&) const override;
  virtual void arrayIds (casacore::Vector<casacore::Int>&) const override;
  virtual casacore::String fieldName () const override;

  virtual void flag (casacore::Cube<casacore::Bool> & flags) const override;
  virtual void flag (casacore::Vector<casacore::Cube<casacore::Bool>> & flags) const override;
  virtual void flag (casacore::Matrix<casacore::Bool> &) const override { SSVi2NotPossible() };
  virtual casacore::Bool flagCategoryExists () const override { return false; };
  virtual void flagCategory (casacore::Array<casacore::Bool> &) const override { SSVi2NotPossible() };
  virtual void flagRow (casacore::Vector<casacore::Bool> & rowflags) const override;
  virtual void observationId (casacore::Vector<casacore::Int> & obsids) const override;
  virtual casacore::Int polarizationId () const override;
  virtual void processorId (casacore::Vector<casacore::Int> & procids) const override;
  virtual void scan (casacore::Vector<casacore::Int> & scans) const override;
  virtual casacore::String sourceName () const override;
  virtual void stateId (casacore::Vector<casacore::Int> & stateids) const override;
  virtual void jonesC (casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > &) const override { SSVi2NotPossible() };
  virtual casacore::Int polFrame () const override;
  virtual void sigma (casacore::Matrix<casacore::Float> & sigmat) const override;
  virtual void sigma (casacore::Vector<casacore::Matrix<casacore::Float>> & sigmat) const override;
  virtual void spectralWindows (casacore::Vector<casacore::Int> & spws) const override;
  virtual void polarizationIds (casacore::Vector<casacore::Int> & polIds) const override;
  virtual void time (casacore::Vector<casacore::Double> & t) const override;
  virtual void timeCentroid (casacore::Vector<casacore::Double> & t) const override;
  virtual void timeInterval (casacore::Vector<casacore::Double> & ti) const override;
  virtual void uvw (casacore::Matrix<casacore::Double> & uvwmat) const override;

  virtual void visibilityCorrected (casacore::Cube<casacore::Complex> & vis) const override;
  virtual void visibilityCorrected (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;
  virtual void visibilityModel (casacore::Cube<casacore::Complex> & vis) const override;
  virtual void visibilityModel (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;
  virtual void visibilityObserved (casacore::Cube<casacore::Complex> & vis) const override;
  virtual void visibilityObserved (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;
  virtual void floatData (casacore::Cube<casacore::Float> & fcube) const override;
  virtual void floatData (casacore::Vector<casacore::Cube<casacore::Float>> & fcubes) const override;

  virtual casacore::IPosition visibilityShape () const override;

  virtual void weight (casacore::Matrix<casacore::Float> & wtmat) const override;
  virtual void weight (casacore::Vector<casacore::Matrix<casacore::Float>> & wtmat) const override;
  virtual casacore::Bool weightSpectrumExists () const override;
  virtual casacore::Bool sigmaSpectrumExists () const override;
  virtual void weightSpectrum (casacore::Cube<casacore::Float> & wtsp) const override;
  virtual void weightSpectrum (casacore::Vector<casacore::Cube<casacore::Float>> & wtsp) const override;
  virtual void sigmaSpectrum (casacore::Cube<casacore::Float> & wtsp) const override;
  virtual void sigmaSpectrum (casacore::Vector<casacore::Cube<casacore::Float>> & wtsp) const override;

  virtual void setWeightScaling (casacore::CountedPtr<WeightScaling>) override { SSVi2NotPossible() };
  virtual casacore::Bool hasWeightScaling () const override { return false; };
  virtual casacore::CountedPtr<WeightScaling> getWeightScaling () const override; //  { SSVi2NotPossible() };

  //   +------------------------+
  //   |                        |
  //   | Angular casacore::Data Providers |
  //   |                        |
  //   +------------------------+

  // No underlying geometry is available for these!
  virtual casacore::Bool allBeamOffsetsZero () const override { SSVi2NotPossible() };
  virtual casacore::MDirection azel0 (casacore::Double) const override { SSVi2NotPossible() };
  virtual const casacore::Vector<casacore::MDirection> & azel (casacore::Double) const override { SSVi2NotPossible() };
  virtual const casacore::Vector<casacore::Float> & feed_pa (casacore::Double t) const override; 
  virtual std::pair<bool, casacore::MDirection> getPointingAngle (int /*antenna*/, double /*time*/) const override
  { return std::make_pair (true, phaseCenter()); }
  virtual const casacore::Cube<casacore::RigidVector<casacore::Double, 2> > & getBeamOffsets () const override { SSVi2NotPossible() };
  virtual casacore::Double hourang (casacore::Double) const override { SSVi2NotPossible() };
  virtual const casacore::Float & parang0 (casacore::Double) const override { SSVi2NotPossible() };
  virtual const casacore::Vector<casacore::Float> & parang (casacore::Double) const override { SSVi2NotPossible() };
  virtual const casacore::MDirection & phaseCenter () const override { return phaseCenter_; }; // trivial value (for now)
  virtual const casacore::Cube<casacore::Double> & receptorAngles () const override { SSVi2NotPossible() };

  //   +=========================+
  //   |                         |
  //   | Chunk and casacore::MS Level casacore::Data |
  //   |                         |
  //   +=========================+

  virtual const casacore::Vector<casacore::String> & antennaMounts () const override { SSVi2NotPossible() };
  virtual const VisImagingWeight & getImagingWeightGenerator () const override { SSVi2NotPossible() };
  
  virtual casacore::MFrequency::Types getObservatoryFrequencyType () const override { SSVi2NotPossible() };
  virtual casacore::MPosition getObservatoryPosition () const override { SSVi2NotPossible() };
  virtual casacore::Vector<casacore::Float> getReceptor0Angle () override { SSVi2NotPossible() };
 
  virtual casacore::Int getReportingFrameOfReference () const override { return -2;  /*SSVi2NotPossible()*/ };
  virtual void setReportingFrameOfReference (casacore::Int) override { SSVi2NotPossible() };
 
  virtual casacore::MEpoch getEpoch () const override;

  virtual casacore::Vector<casacore::Int> getCorrelations () const override;

  virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesDefined () const override { return corrdef_; /*SSVi2NotPossible()*/ };
  virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesSelected () const override { return corrdef_; /*SSVi2NotPossible()*/ };

  virtual casacore::Vector<casacore::Int> getChannels (casacore::Double time, casacore::Int frameOfReference, casacore::Int spectralWndow = -1,
				   casacore::Int msId = -1) const override;
  virtual casacore::Vector<casacore::Double> getFrequencies (casacore::Double time, casacore::Int frameOfReference, casacore::Int spectralWndow = -1,
					 casacore::Int msId = -1) const override;
  virtual casacore::Vector<casacore::Double> getChanWidths (casacore::Double time, casacore::Int frameOfReference, casacore::Int spectralWndow = -1,
					 casacore::Int msId = -1) const override;

  //reference to actual ms in interator  (TRIVIAL returns
  virtual casacore::Int msId () const override { return -1; }; // zero-based index of current casacore::MS in set of MSs
  virtual const casacore::MeasurementSet & ms () const override { SSVi2NotPossible() };
  virtual casacore::Int getNMs () const override { return 0; };

  // Name of the MS in the interator
  virtual casacore::String msName() const override { return casacore::String("<noms>"); };

  // Call to use the slurp i/o method for all scalar columns. 
  //  Not meaningful for non-I/O
  virtual void slurp () const override { SSVi2NotPossible() };

  // Access the current casacore::MSColumns object in MSIter -- NOT POSSIBLE
  virtual const vi::SubtableColumns & subtableColumns () const override { SSVi2NotPossible() };


  // get back the selected spectral windows and spectral channels for
  // current ms

  virtual const SpectralWindowChannels & getSpectralWindowChannels (casacore::Int msId, casacore::Int spectralWindowId) const override;

  //assign a VisImagingWeight object to this iterator
  virtual void useImagingWeight (const VisImagingWeight & ) override { SSVi2NotPossible() };
  
  // Return number of antennasm spws, polids, ddids
  
  virtual casacore::Int nAntennas () const override;
  virtual casacore::Int nDataDescriptionIds () const override;
  virtual casacore::Int nPolarizationIds () const override;
  virtual casacore::rownr_t nRowsInChunk () const override; // number rows in current chunk
  virtual casacore::rownr_t nRowsViWillSweep () const override; // number of rows in all selected ms's
  virtual casacore::Int nSpectralWindows () const override;
  virtual casacore::Int nTimes() const override;

  // Writeback methods are Irrelevant for non-disk-reading VI2
  virtual void writeBackChanges (VisBuffer2* ) override { SSVi2NotPossible() };
  virtual void writeFlag (const casacore::Cube<casacore::Bool> &) override { SSVi2NotPossible() };
  virtual void writeFlagRow (const casacore::Vector<casacore::Bool> &) override { SSVi2NotPossible() };
  virtual void writeFlagCategory(const casacore::Array<casacore::Bool>&) override { SSVi2NotPossible() };
  virtual void writeVisCorrected (const casacore::Cube<casacore::Complex> &) override { SSVi2NotPossible() };
  virtual void writeVisModel (const casacore::Cube<casacore::Complex> &) override { SSVi2NotPossible() };
  virtual void writeVisObserved (const casacore::Cube<casacore::Complex> &) override { SSVi2NotPossible() };
  virtual void writeWeight (const casacore::Matrix<casacore::Float> &) override { SSVi2NotPossible() };
  virtual void writeWeightSpectrum (const casacore::Cube<casacore::Float> &) override { SSVi2NotPossible() };
  virtual void initWeightSpectrum (const casacore::Cube<casacore::Float> &) override { SSVi2NotPossible() };
  virtual void writeSigmaSpectrum (const casacore::Cube<casacore::Float> &) override { SSVi2NotPossible() };
  virtual void writeSigma (const casacore::Matrix<casacore::Float> &) override { SSVi2NotPossible() };
  virtual void writeModel(const casacore::RecordInterface&,casacore::Bool,casacore::Bool) override { SSVi2NotPossible() };

  //**********************************************************************
  // Methods to access the subtables.
  //**********************************************************************

  // Access to antenna subtable
  const casacore::MSAntennaColumns& antennaSubtablecols() const override;

  // Access to dataDescription subtable
  const casacore::MSDataDescColumns& dataDescriptionSubtablecols() const override;

  // Access to feed subtable
  const casacore::MSFeedColumns& feedSubtablecols() const override;

  // Access to field subtable
  const casacore::MSFieldColumns& fieldSubtablecols() const override;

  // Access to flagCmd subtable
  const casacore::MSFlagCmdColumns& flagCmdSubtablecols() const override;

  // Access to history subtable
  const casacore::MSHistoryColumns& historySubtablecols() const override;

  // Access to observation subtable
  const casacore::MSObservationColumns& observationSubtablecols() const override;

  // Access to pointing subtable
  const casacore::MSPointingColumns& pointingSubtablecols() const override;

  // Access to polarization subtable
  const casacore::MSPolarizationColumns& polarizationSubtablecols() const override;

  // Access to processor subtable
  const casacore::MSProcessorColumns& processorSubtablecols() const override;

  // Access to spectralWindow subtable
  const casacore::MSSpWindowColumns& spectralWindowSubtablecols() const override;

  // Access to state subtable
  const casacore::MSStateColumns& stateSubtablecols() const override;

  // Access to doppler subtable
  const casacore::MSDopplerColumns& dopplerSubtablecols() const override;

  // Access to freqOffset subtable
  const casacore::MSFreqOffsetColumns& freqOffsetSubtablecols() const override;

  // Access to source subtable
  const casacore::MSSourceColumns& sourceSubtablecols() const override;

  // Access to sysCal subtable
  const casacore::MSSysCalColumns& sysCalSubtablecols() const override;

  // Access to weather subtable
  const casacore::MSWeatherColumns& weatherSubtablecols() const override;

private:

  // Make default ctor invisible
  SimpleSimVi2 ();

  // Keep VB2 sync'd
  void configureNewSubchunk();

  // Generate noise on data
  void addNoise(casacore::Cube<casacore::Complex>& vis, casacore::rownr_t vbRowOffset) const;

  // Corrupt by (ad hoc) parang factors
  void corruptByParang(casacore::Cube<casacore::Complex>& vis, casacore::rownr_t vbRowOffset) const;

  // Generate the antenna, spw and DD subtables
  void generateSubtables();

  // casacore::Input parameters
  const SimpleSimVi2Parameters pars_;
  /*
  casacore::Int nField_,nScan_,nSpw_,nAnt_,nCorr_;
  casacore::Vector<casacore::Int> nTimePerField_, nChan_;
  casacore::String date0_;
  casacore::Double dt_;
  casacore::Vector<casacore::Double> refFreq_, df_;
  casacore::Matrix<casacore::Float> stokes_, sefd_;
  casacore::String polBasis_;
  casacore::Bool doAC_;
  */

  // Derived parameters
  casacore::Int nChunk_, nBsln_;
  casacore::rownr_t  nSubchunk_;
  casacore::Complex c0_;
  casacore::Double t0_;
  casacore::Vector<casacore::Float> wt0_;
  casacore::Matrix<casacore::Complex> vis0_;


  // Counters
  casacore::Int iChunk_;
  casacore::rownr_t iSubChunk_;
  casacore::Int iRow0_;
  casacore::Int iScan_;
  casacore::Double iChunkTime0_;

  // Meta-info for current iteration
  casacore::Int thisScan_, thisField_, thisSpw_, thisAntenna1_, thisAntenna2_;
  casacore::Int lastScan_, lastField_, lastSpw_;
  casacore::Double thisTime_;
  casacore::rownr_t nRows_;
  casacore::rownr_t nShapes_;
  casacore::Vector<casacore::rownr_t> nRowsPerShape_;
  casacore::Vector<casacore::Int> nChannPerShape_;
  casacore::Vector<casacore::Int> nCorrsPerShape_;

  // Correlation stuff
  casacore::Vector<casacore::Stokes::StokesTypes> corrdef_;

  // The associated VB
  std::unique_ptr<VisBuffer2> vb_;

  // Subtables
  casacore::MSAntenna antennaSubTable_p;
  std::unique_ptr<casacore::MSAntennaColumns> antennaSubTablecols_p;
  casacore::MSSpectralWindow spwSubTable_p;
  std::unique_ptr<casacore::MSSpWindowColumns> spwSubTablecols_p;
  casacore::MSDataDescription ddSubTable_p;
  std::unique_ptr<casacore::MSDataDescColumns> ddSubTablecols_p;
  casacore::MSPolarization polSubTable_p;
  std::unique_ptr<casacore::MSPolarizationColumns> polSubTablecols_p;

  // Trivial (for now) MDirection, so phaseCenter() has something to return
  casacore::MDirection phaseCenter_;

  // Trivial (for now) parang
  mutable casacore::Vector<casacore::Float> feedpa_;

};



class SimpleSimVi2Factory : public ViFactory {

public:

  SimpleSimVi2Factory(const SimpleSimVi2Parameters& pars);
  
  ~SimpleSimVi2Factory ();

protected:

  virtual ViImplementation2 * createVi () const;

private:

  const SimpleSimVi2Parameters& pars_;

};


class SimpleSimVi2LayerFactory : public ViiLayerFactory {

public:

  SimpleSimVi2LayerFactory(const SimpleSimVi2Parameters& pars);

  virtual ~SimpleSimVi2LayerFactory () {}

protected:

  // SimpleSimVi2-specific layer-creater
  //   
  virtual ViImplementation2 * createInstance (ViImplementation2* vii0) const;

private:

  // Store a copy of the parameters
  const SimpleSimVi2Parameters pars_;


};






} // end namespace vi

} //# NAMESPACE CASA - END

#endif // ! defined (MSVIS_SimpleSimVi2_H)


