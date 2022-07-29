//# VisSet.h: VisSet definitions
//# Copyright (C) 1996,1997,1998,2001,2002
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef MSVIS_VISSET_H
#define MSVIS_VISSET_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisibilityIterator.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// The interface to the casacore::MeasurementSet for synthesis processing
// </summary>

// <reviewed reviewer="" date="" tests="t" demos="">

// <prerequisite>
//   <li> <linkto module="casacore::MeasurementSet">casacore::MeasurementSet</linkto>
// </prerequisite>
//
// <etymology>
// VisSet is the Set of Visibilities
// </etymology>
//
// <synopsis> 
// The VisSet is a class that simplifies access to the visibility data
// for the synthesis processing code. It holds a reference to an original
// casacore::MeasurementSet with observed data and creates two extra data
// columns for the storage of model visibilities and
// corrected visibilities. All access to the casacore::MeasurementSet is done via
// the <linkto class="VisibilityIterator">VisibilityIterator</linkto>
// and the <linkto class="VisBuffer">VisBuffer</linkto>
//
// The VisSet allows selection and sorting of the casacore::MeasurementSet to be applied.
// A number of columns can be specified to define the iteration order, a
// a time interval can be given to iterate in chunks of time and a channel
// selection can be made for each spectral window present in the data.
// </synopsis> 
//
// <example>
// <srcblock>
//    casacore::MeasurementSet ms("Example.MS",casacore::Table::Update);
//    cout << "Constructing VisSet"<<endl;
//    casacore::Block<casacore::Int> bi(2); 
//    bi[0]=casacore::MS::SPECTRAL_WINDOW_ID;
//    bi[1]=casacore::MS::TIME; 
//    casacore::Matrix<casacore::Int> chanSelection; // no channel selection
//    // iterate in 600s chunks within each SpectralWindow
//    casacore::Double interval=600.; 
//    VisSet vs(ms,bi,chanSelection,interval);
// </srcblock>
// </example>
//
// <motivation>
// This class provides an easy interface to the MS. 
// It keeps the iterator around
// for reuse, thus avoiding repeated sorting of the data.
// </motivation>
//
// <todo asof="">
// </todo>

typedef casacore::uInt Antenna;
typedef casacore::Double Frequency;
typedef casacore::RigidVector<casacore::Float,3> Position;
typedef casacore::RigidVector<casacore::Double,3> Direction;

typedef casacore::Vector<CStokesVector> vvCoh;

typedef ROVisibilityIterator ROVisIter;
typedef VisibilityIterator VisIter;

class VisSet {

public:
  // default constructor, only useful to assign to later.
  VisSet() {}

  // Construct from a casacore::MeasurementSet, with iteration order specified in
  // columns (giving the casacore::MS enum for the column) 
  // Specify channel selection as a casacore::Matrix(3,nSpw) where for each
  // spectral window the three values are start,nChannel and
  // spectral window number. Spectral windows without an entry will have 
  // all channels selected.
  // Specify a time interval for iterating in 'chunks' of time.
  // The default time interval of 0 groups all times together.
  // This constructor creates two new columns:
  // MODEL_DATA and CORRECTED_DATA
  // If they already exist and have the
  // same channel selection applied, they are reused.
  // Note that the contents of these columns are NOT initialized,
  // you should fill them before trying to read the data.
  // The casacore::MS calibration scratch columns can be optionally compressed.
  VisSet(casacore::MeasurementSet & ms, const casacore::Block<casacore::Int>& columns, 
	 const casacore::Matrix<casacore::Int>& chanSelection, casacore::Double timeInterval=0,
	 casacore::Bool compress=false, casacore::Bool doModelData=true);

  // Same as above, but provide scratch column option
  VisSet(casacore::MeasurementSet& ms,const casacore::Block<casacore::Int>& columns, 
	 const casacore::Matrix<casacore::Int>& chanSelection, 
	 casacore::Bool addScratch,
	 casacore::Double timeInterval=0,casacore::Bool compress=false, casacore::Bool doModelData=true);

  // This is a constructor for multiple MS...but everything is same as the one 
  // above


  VisSet(casacore::Block<casacore::MeasurementSet>& mss, const casacore::Block<casacore::Int>& columns, 
         const casacore::Block< casacore::Matrix<casacore::Int> >& chanSelections, casacore::Bool addStratch=false, casacore::Double timeInterval=0,
         casacore::Bool compress=false, casacore::Bool doModelData=true);





  // This is a no frills constructor, no re-sorting, the default order is used,
  // no scratch columns is made even if they don't exist. So if you use
  // this constructor and plan to use the scratch columns make sure 
  // that they exist prior to constructing the VisSet this way.
  VisSet(casacore::MeasurementSet & ms, const casacore::Matrix<casacore::Int>& chanSelection, 
	 casacore::Double timeInterval=0);

  //Constructor from visibility iterator ....a temporary fix 
  //as EPJones as Imager stops using VisSet 
  VisSet(ROVisibilityIterator& vi);
  // Construct from an existing VisSet, this references the underlying
  // casacore::MeasurementSet(s) but allows a new iteration order and time interval
  // to be specified.
  VisSet(const VisSet & vs, const casacore::Block<casacore::Int>& columns, casacore::Double timeInterval=0);

  // Destructor, flushes the data to disk
  ~VisSet();
  // referencing assignment operator
  VisSet& operator=(const VisSet& other);

  // Re-initialize the VisibilityIterator (cf copy ctor)
  void resetVisIter(const casacore::Block<casacore::Int>& columns, casacore::Double timeInterval = 0,
                    asyncio::PrefetchColumns * prefetchColumns = NULL);

  // Initializes scratch columns
  void initCalSet(casacore::Int calSet=0);

  // Flushes the data to disk
  void flush();
  // Iterator access to the  data
  VisIter& iter();

  // Reset the channel selection. Only subsets of the original selection
  // (set in constructor) can be specified.
  // Note: this calls origin on the iterator.
  void selectChannel(casacore::Int nGroup,casacore::Int start, casacore::Int width, casacore::Int increment, 
		     casacore::Int spectralWindow);
  // call to VisIter origin optional:
  void selectChannel(casacore::Int nGroup,casacore::Int start, casacore::Int width, casacore::Int increment, 
		     casacore::Int spectralWindow, casacore::Bool callOrigin);

  // Collective selection via casacore::MSSelection channel selection Matrix
  void selectChannel(const casacore::Matrix<casacore::Int>& chansel);

  // Set nominal selection to ALL channels
  void selectAllChans();

  // number of antennas
  casacore::Int numberAnt();

  // number of fields
  casacore::Int numberFld();

  // number of spectral windows
  casacore::Int numberSpw();

  // number of channels in each spectral window
  casacore::Vector<casacore::Int> numberChan() const;

  // start channel of VisSet selection in each spectral window
  casacore::Vector<casacore::Int> startChan() const;

  // number of coherences
  casacore::Int numberCoh() const;

  // Lock and unlock the associated MS
  void lock() {ms_p.lock();};
  void unlock() {ms_p.unlock();};

  // Return the associated casacore::MS name
  casacore::String msName();

  // SYSCAL table name of the associated MS
  casacore::String sysCalTableName();

  // SPECTRAL_WINDOW table name of the associated MS
  casacore::String spectralWindowTableName();

  // FIELD table name of the associated MS
  casacore::String fieldTableName();

  // SYSPOWER table name of the associated MS
  casacore::String syspowerTableName();

  // CALDEVICE table name of the associated MS
  casacore::String caldeviceTableName();

private:

  //Add the scratch columns
  void addScratchCols(casacore::MeasurementSet& ms, casacore::Bool compress=false, casacore::Bool doModelData=true);

  // Add a calibration set (comprising a set of CORRECTED_DATA and MODEL_DATA)
  // to the casacore::MeasurementSet (casacore::MS). Optionally compress these columns using the 
  // casacore::CompressComplex column engine.
  void addCalSet(casacore::MeasurementSet& ms, casacore::Bool compress=true, casacore::Bool doModelData=true);
  void addCalSet2(casacore::MeasurementSet& ms, casacore::Bool compress=false, casacore::Bool doModelData=true);

 
  casacore::MeasurementSet ms_p;
  VisIter* iter_p;
  casacore::Matrix<casacore::Int> selection_p;
  casacore::Block<casacore::MeasurementSet> *blockOfMS_p;
  casacore::Bool multims_p;

};


} //# NAMESPACE CASA - END

#endif


