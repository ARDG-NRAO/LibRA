//# CalSet.h: Definition for Calibration parameter cache
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#

#ifndef CALTABLES_CALSET_H
#define CALTABLES_CALSET_H
#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Arrays.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <synthesis/CalTables/CalTableDesc2.h>
#include <synthesis/CalTables/CalTable2.h>
#include <synthesis/CalTables/SolvableCalSetMCol.h>
#include <synthesis/CalTables/VisCalEnum.h>
//#include <synthesis/CalTables/BaseCalSet.h>

// #include <synthesis/MeasurementComponents/VisJones.h>
// #include <synthesis/MeasurementEquations/VisEquation.h>

namespace casa { //# NAMESPACE CASA - BEGIN


template<class T>
class CalSet 
{
public:

  // Null ctor does nothing
  CalSet() {};

  // Minimal, knows only how many potential spws
  //   (solve context)
  CalSet(const casacore::Int& nSpw);

  // From full shape
  CalSet(const casacore::Int& nSpw,
	 const casacore::Int& nPar,
	 const casacore::Vector<casacore::Int>& nChan,
	 const casacore::Int& nElem,
	 const casacore::Vector<casacore::Int>& nTime);


  // From existing CalTable
  //   (apply context)
  //   (for now, we will verify that filled solutions match nPar,nElem,nSpw)
  CalSet(const casacore::String& calTableName,
	 const casacore::String& select,
	 const casacore::Int& nSpw,
	 const casacore::Int& nPar,
	 const casacore::Int& nElem);

  // Copy
  CalSet(const CalSet& other);

  // Assignment
  //  virtual CalSet& operator=(const CalSet& other);

  // Destructor
  virtual ~CalSet();

  // Resize caches
  void resize(const casacore::Int& nPar,
	      const casacore::Vector<casacore::Int>& nChan,
	      const casacore::Int& nElem,
	      const casacore::Vector<casacore::Int>& nTime);

  // Access to shape info:
  casacore::Int& nSpw() { return nSpw_; };
  casacore::Int& nPar() { return nPar_; };
  casacore::Vector<casacore::Int>& nChan()  { return nChan_; };
  casacore::Int& nChan(const casacore::Int& spw)  { return nChan_(spw); };
  casacore::Int& nElem()  { return nElem_; };
  casacore::Vector<casacore::Int>& nTime()  { return nTime_; };
  casacore::Int& nTime(const casacore::Int& spw) { return nTime_(spw); };

  //  casacore::IPosition& shape(const casacore::Int& spw) { return casacore::IPosition(4,nPar(),nChan(spw),nElem(),nTime(spw)); };
  casacore::IPosition shape(const casacore::Int& spw) { return casacore::IPosition(4,nPar_,nChan_(spw),nElem_,nTime_(spw)); };

  // Set up spwOK according to solution availability
  inline void setSpwOK() { spwOK_ = (nTime()!=0); };

  // Report if solutions available for specified spw
  casacore::Vector<casacore::Bool> spwOK() { return spwOK_; };

  // Freq list per spw
  casacore::Vector<casacore::Double>& frequencies(const casacore::Int& spw) { return *freq_[spw]; };

  // Temporary method to support current conventions in SVJ
  void setStartChan(const casacore::Vector<casacore::Int>& startChan) {startChan_ = startChan;};
  casacore::Vector<casacore::Int>& startChan() {return startChan_;};

  // Access to whole solution caches 
  casacore::Vector<casacore::Double>& startTime(const casacore::Int& spw)    { return *MJDStart_[spw]; };
  casacore::Vector<casacore::Double>& stopTime(const casacore::Int& spw)     { return *MJDStop_[spw]; };
  casacore::Vector<casacore::Double>& time(const casacore::Int& spw)         { return *MJDTimeStamp_[spw]; };
  casacore::Vector<casacore::Int>&    fieldId(const casacore::Int& spw)      { return *fieldId_[spw]; };
  casacore::Vector<casacore::String>& fieldName(const casacore::Int& spw)    { return *fieldName_[spw]; };
  casacore::Vector<casacore::String>& sourceName(const casacore::Int& spw)   { return *sourceName_[spw]; };
  casacore::Array<T>&       par(const casacore::Int& spw)          { return *par_[spw]; };
  casacore::Array<casacore::Bool>&    parOK(const casacore::Int& spw)        { return *parOK_[spw]; };
  casacore::Array<casacore::Float>&   parErr(const casacore::Int& spw)       { return *parErr_[spw]; };
  casacore::Array<casacore::Float>&   parSNR(const casacore::Int& spw)       { return *parSNR_[spw]; };

  // Statistics
  //  casacore::Matrix<casacore::Bool>&   iSolutionOK(const casacore::Int& spw)  { return *iSolutionOK_[spw]; };
  casacore::Matrix<casacore::Float>&  iFit(const casacore::Int& spw)         { return *iFit_[spw]; };
  casacore::Matrix<casacore::Float>&  iFitwt(const casacore::Int& spw)       { return *iFitwt_[spw]; };
  casacore::Vector<casacore::Bool>&   solutionOK(const casacore::Int& spw)   { return *solutionOK_[spw]; };
  casacore::Vector<casacore::Float>&  fit(const casacore::Int& spw)          { return *fit_[spw]; };
  casacore::Vector<casacore::Float>&  fitwt(const casacore::Int& spw)        { return *fitwt_[spw]; };

  // Store/Retrieve solutions to/from a table.
  //  (will evolve to use table iteration)
  virtual void initCalTableDesc(const casacore::String& type, const casacore::Int& parType);
  virtual void attach();
  virtual void store(const casacore::String& file, const casacore::String& type, const casacore::Bool& append,
		     const casacore::String& msname="");
//   virtual void store(const casacore::String& file, const casacore::String& type, 
// 		     const casacore::String& msname, const casacore::Bool& append);
  virtual void load(const casacore::String& file, const casacore::String& select);

protected:


private:

  // new/delete of cache
  void inflate();
  void deflate();


  // casacore::Table name
  casacore::String calTableName_;

  // Number of Spectral windows
  casacore::Int nSpw_;

  // Number of parameters describing this calibration component
  casacore::Int nPar_;

  // Number of channels
  casacore::Vector<casacore::Int> nChan_;          // (nSpw_) number of _soln_ channels per spw

  // Number of elements (ants or baselines)
  casacore::Int nElem_;

  // Number of time slots
  casacore::Vector<casacore::Int> nTime_;         // (nSpw_) number of slots per spw

  // Spw OK?
  casacore::Vector<casacore::Bool> spwOK_;

  // Channel frequencies, etc.
  casacore::Vector<casacore::Int> startChan_;            // (nSpw_) start data channel per spw
  casacore::PtrBlock<casacore::Vector<casacore::Double>*> freq_;   // [nSpw_](nChan_)

  // Per spw, per slot Meta information
  casacore::PtrBlock<casacore::Vector<casacore::Double>*> MJDStart_;       // (nSpw_)(numberSlots_)
  casacore::PtrBlock<casacore::Vector<casacore::Double>*> MJDStop_;        // (nSpw_)(numberSlots_)
  casacore::PtrBlock<casacore::Vector<casacore::Double>*> MJDTimeStamp_;   // (nSpw_)(numberSlots_)
  casacore::PtrBlock<casacore::Vector<casacore::Int>*>    fieldId_;        // (nSpw_)(numberSlots_)
  casacore::PtrBlock<casacore::Vector<casacore::String>*> fieldName_;      // (nSpw_)(numberSlots_)
  casacore::PtrBlock<casacore::Vector<casacore::String>*> sourceName_;     // (nSpw_)(numberSlots_)

  // Per spw, per channel, per element, per slot solution generic PARAMETER storage  
  casacore::PtrBlock<casacore::Array<T>*>      par_;    // (nSpw_)(nPar_,nSolnChan_,nElem_,numberSlots_)
  casacore::PtrBlock<casacore::Array<casacore::Bool>*>   parOK_;  // (nSpw_)(nPar_,nSolnChan_,nElem_,numberSlots_)
  casacore::PtrBlock<casacore::Array<casacore::Float>*>  parErr_; // (nSpw_)(nPar_,nSolnChan_,nElem_,numberSlots_)
  casacore::PtrBlock<casacore::Array<casacore::Float>*>  parSNR_; // (nSpw_)(nPar_,nSolnChan_,nElem_,numberSlots_)

  // Statistics
  //  casacore::PtrBlock<casacore::Matrix<casacore::Bool>*> iSolutionOK_;  // [nSpw_](nElem_,nSlots_)
  casacore::PtrBlock<casacore::Matrix<casacore::Float>*> iFit_;        // [nSpw_](nElem_,nSlots_)
  casacore::PtrBlock<casacore::Matrix<casacore::Float>*> iFitwt_;      // [nSpw_](nElem_,nSlots_)

  casacore::PtrBlock<casacore::Vector<casacore::Bool>*> solutionOK_;   // [nSpw_](numberSlots_)
  casacore::PtrBlock<casacore::Vector<casacore::Float>*> fit_;         // [nSpw_](numberSlots_)
  casacore::PtrBlock<casacore::Vector<casacore::Float>*> fitwt_;       // [nSpw_](numberSlots_)

  casacore::LogSink logSink_p;
  casacore::LogSink& logSink() {return logSink_p;};

  CalTableDesc2 *calTabDesc_;
  CalTable2 *calTab_;
  SolvableCalSetMCol<T> *svjmcol_;

};

// Globals

// casacore::Smooth the solutions in a CalSet
  void smooth(CalSet<casacore::Complex>& cs,
	      const casacore::String& smtype,
	      const casacore::Double& smtime,
	      casacore::Vector<casacore::Int> selfields);

} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <synthesis/CalTables/CalSet.tcc>
#endif

#endif
