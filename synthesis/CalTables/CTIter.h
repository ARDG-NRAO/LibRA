//# CTIter.h: Iterating interfaces for modern CalTables
//# Copyright (C) 2011
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

#ifndef CALIBRATION_CALTABITER_H
#define CALIBRATION_CALTABITER_H

#include <casa/aips.h>
#include <measures/Measures/MBaseline.h>
#include <measures/Measures/MEpoch.h>
#include <ms/MSOper/MSDerivedValues.h>
#include <tables/Tables/TableIter.h>

#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <synthesis/CalTables/CTColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// ROCTIter: Read-only iterating interface to New Calibration tables
// </summary>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTableDesc">NewCalTable</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration", "table", and "iteration".
// </etymology>
//
// <synopsis>
// The ROCTIter class provide basic iterated access to the contents
// of a NewCalTable.  The iteration is initialized according to sort 
// columns specified in the constructor, and access to the contents
// of each iteration is provided via a NewCalTable object (referencing
// the iterated subset of the original NewCalTable) or methods that return
// Vectors or Arrays of the contents of specific columns.  For the latter,
// the Vectors/Arrays are generated only on-demand.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// This class is used by calibration table clients to access calibration tables
//  in a piecemeal fashion, e.g., for plotting, interpolation, etc.
// </motivation>
//
// <todo asof="11/06/10">
// </todo>

class ROCTIter
{
public:
  // Constructor/Destructor 
  ROCTIter(NewCalTable tab,const casacore::Block<casacore::String>& sortcol);
  virtual ~ROCTIter();
  
  // Iteration operators
  void reset();
  inline casacore::Bool pastEnd() { return ti_->pastEnd(); };
  void next();
  void next0();

  // Return the current table iteration
  NewCalTable table() const { return NewCalTable(ti_->table()); };

  casacore::Int nrow() const { return ti_->table().nrow(); };

  // When not included in ctor table
  void setCTColumns(const NewCalTable& tab);

  // Column accessors
  // Those methods that return scalars for data coordinates 
  //  (e.g., thisTime(), thisField(), etc.) return the first element
  //  of the corresponding vector of values in the current 
  //  iteration.  This makes the most sense when the corresponding
  //  column is a sort column in the iteration, such that the contents
  //  of the vector is in fact a unique value.  (TBD: return -1 from
  //  scalar methods when column is not unique?)

  casacore::Double thisTime() const;
  casacore::Vector<casacore::Double> time() const;
  void time(casacore::Vector<casacore::Double>& v) const;

  casacore::Int thisField() const;
  casacore::Vector<casacore::Int> field() const;
  void field(casacore::Vector<casacore::Int>& v) const;

  casacore::Int thisSpw() const;
  casacore::Vector<casacore::Int> spw() const;
  void spw(casacore::Vector<casacore::Int>& v) const;

  casacore::Int thisScan() const;
  casacore::Vector<casacore::Int> scan() const;
  void scan(casacore::Vector<casacore::Int>& v) const;

  casacore::Int thisObs() const;
  casacore::Vector<casacore::Int> obs() const;
  void obs(casacore::Vector<casacore::Int>& v) const;

  casacore::Int thisAntenna1() const;
  casacore::Vector<casacore::Int> antenna1() const;
  void antenna1(casacore::Vector<casacore::Int>& v) const;
  casacore::Int thisAntenna2() const;
  casacore::Vector<casacore::Int> antenna2() const;
  void antenna2(casacore::Vector<casacore::Int>& v) const;

  casacore::Double thisInterval() const;
  casacore::Vector<casacore::Double> interval() const;
  void interval(casacore::Vector<casacore::Double>& v) const;

  casacore::Cube<casacore::Complex> cparam() const;
  void cparam(casacore::Cube<casacore::Complex>& c) const;

  casacore::Cube<casacore::Float> fparam() const;
  void fparam(casacore::Cube<casacore::Float>& c) const;

  casacore::Cube<casacore::Float> casfparam(casacore::String what="") const;
  void casfparam(casacore::Cube<casacore::Float>& casf, casacore::String what="") const;

  casacore::Cube<casacore::Float> paramErr() const;
  void paramErr(casacore::Cube<casacore::Float>& c) const;

  casacore::Cube<casacore::Float> snr() const;
  void snr(casacore::Cube<casacore::Float>& c) const;

  casacore::Cube<casacore::Float> wt() const;
  void wt(casacore::Cube<casacore::Float>& c) const;

  casacore::Cube<casacore::Bool> flag() const;
  void flag(casacore::Cube<casacore::Bool>& c) const;

  casacore::Int nchan() const;
  casacore::Vector<casacore::Int> chan() const;
  void chan(casacore::Vector<casacore::Int>& v) const;
  casacore::Vector<casacore::Double> freq() const;
  void freq(casacore::Vector<casacore::Double>& v) const;
  int freqFrame(int spwId) const;

  casacore::MDirection azel0(casacore::Double time);
  casacore::Double hourang(casacore::Double time);
  casacore::Float parang0(casacore::Double time);

  casacore::Matrix<casacore::Double> uvw();
 protected:

  // Attach accessors
  virtual void attach();
  
 private:

  // Prohibit public use of copy, assignment
  ROCTIter (const ROCTIter& other);
  ROCTIter& operator= (const ROCTIter& other);

  // Set epoch_ from first time measure
  void initEpoch();

  // Update phase center in MSDerivedValues
  void updatePhaseCenter();

  // Initialize and calculate uvw for each antenna.
  // Based on msvis MSUVWGenerator
  void initUVW();
  void updateAntennaUVW();

  // Remember the sort columns...
  casacore::Vector<casacore::String> sortCols_;

  // If true, spw is unique per iteration, and it is
  //   safe to access channel axis info
  casacore::Bool singleSpw_;

  // The parent NewCalTable (casacore::Table) object
  //   (stays in scope for the life of the CTIter)
  NewCalTable parentNCT_;

  // Access to subtables (e.g., for frequencies)
  ROCTColumns* iROCTCols_;

  // The underlying TableIterator
  casacore::TableIterator *ti_;

  // Per-iteration table
  NewCalTable *inct_;

  // Per-iteration columns
  ROCTMainColumns *iROCTMainCols_;

  // For calculated axes
  bool init_epoch_, init_uvw_;
  casacore::Int nAnt_;
  casacore::MDirection phaseCenter_;
  casacore::MEpoch epoch_;
  casacore::MPosition refAntPos_;
  casacore::MBaseline::Types baseline_type_;
  casacore::Vector<casacore::MVBaseline> mvbaselines_;              // ant pos rel to ant0
  casacore::Vector<casacore::Vector<casacore::Double>> antennaUVW_; // [u,v,w] for each ant
};

// Writable version (limited to certain 'columns')
class CTIter : public ROCTIter
{
public:
  // Constructor/Destructor 
  CTIter(NewCalTable tab,const casacore::Block<casacore::String>& sortcol);
  virtual ~CTIter();

  // Set fieldid
  void setfield(casacore::Int fieldid);

  // Set scan number
  void setscan(casacore::Int scan);

  // Set obsid
  void setobs(casacore::Int obs);

  // Set antenna2 (e.g., used for setting refant)
  void setantenna2(const casacore::Vector<casacore::Int>& a2);

  // Set the flags
  void setflag(const casacore::Cube<casacore::Bool>& flag);

  // Set the parameters
  void setfparam(const casacore::Cube<casacore::Float>& f);
  void setcparam(const casacore::Cube<casacore::Complex>& c);

protected:

  // Attach writable column access
  virtual void attach();

private:

  // Prohibit public use of copy, assignment
  CTIter (const CTIter& other);
  CTIter& operator= (const CTIter& other);

  // Per-iteration table
  NewCalTable *irwnct_;

  // Writable column access
  CTMainColumns *iRWCTMainCols_;

};


} //# NAMESPACE CASA - END

#endif
