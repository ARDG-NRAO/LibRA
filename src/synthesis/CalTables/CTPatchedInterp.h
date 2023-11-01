//# CTPatchedInterp.h: Definition for Calibration patch panel
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

#ifndef CALTABLES_CTPATCHEDINTERP_H
#define CALTABLES_CTPATCHEDINTERP_H

#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTTimeInterp1.h>
#include <synthesis/CalTables/RIorAParray.h>
#include <synthesis/CalTables/VisCalEnum.h>
#include <casacore/scimath/Mathematics/InterpolateArray1D.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSField.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/aips.h>

//#include <casa/BasicSL/Constants.h>
//#include <casa/OS/File.h>
//#include <casa/Logging/LogMessage.h>
//#include <casa/Logging/LogSink.h>

namespace casa { //# NAMESPACE CASA - BEGIN
    
class MSMetaInfoForCal;

class CTPatchedInterp
{
public:
    
  //msmetadata(const casacore::MeasurementSet * const & s, const float maxcache);
  // From NewCalTable only 
  CTPatchedInterp(NewCalTable& ct,
		  VisCalEnum::MatrixType mtype,
		  casacore::Int nPar,
		  const casacore::String& timetype,
		  const casacore::String& freqtype,
		  const casacore::String& fieldtype,
		  casacore::Vector<casacore::Int> spwmap=casacore::Vector<casacore::Int>(),
		  casacore::Vector<casacore::Int> fldmap=casacore::Vector<casacore::Int>(),
		  const CTTIFactoryPtr cttifactoryptr=&CTTimeInterp1::factory);

  // From NewCalTable and casacore::MS 
  CTPatchedInterp(NewCalTable& ct,
		  VisCalEnum::MatrixType mtype,
		  casacore::Int nPar,
		  const casacore::String& timetype,
		  const casacore::String& freqtype,
		  const casacore::String& fieldtype,
		  const casacore::MeasurementSet& ms,
          const MSMetaInfoForCal& msmc,
		  casacore::Vector<casacore::Int> spwmap=casacore::Vector<casacore::Int>(),
		  const CTTIFactoryPtr cttifactoryptr=&CTTimeInterp1::factory);

  // From NewCalTable and casacore::MSColumns 
  CTPatchedInterp(NewCalTable& ct,
		  VisCalEnum::MatrixType mtype,
		  casacore::Int nPar,
		  const casacore::String& timetype,
		  const casacore::String& freqtype,
		  const casacore::String& fieldtype,
		  const casacore::MSColumns& mscol,
		  casacore::Vector<casacore::Int> spwmap=casacore::Vector<casacore::Int>(),
		  const CTTIFactoryPtr cttifactoryptr=&CTTimeInterp1::factory);

  // From NewCalTable and casacore::MS without  metadata
  CTPatchedInterp(NewCalTable& ct,
          VisCalEnum::MatrixType mtype,
          casacore::Int nPar,
          const casacore::String& timetype,
          const casacore::String& freqtype,
          const casacore::String& fieldtype,
          const casacore::MeasurementSet& ms,
          casacore::Vector<casacore::Int> spwmap=casacore::Vector<casacore::Int>(),
          const CTTIFactoryPtr cttifactoryptr=&CTTimeInterp1::factory);


  // Destructor
  virtual ~CTPatchedInterp();

  // Interpolate, given input field, spw, timestamp, & (optionally) freq list
  //    returns T if new result (anywhere)
  casacore::Bool interpolate(casacore::Int obs, casacore::Int fld, casacore::Int spw, casacore::Double time, casacore::Double freq=-1.0);
  casacore::Bool interpolate(casacore::Int obs, casacore::Int fld, casacore::Int spw, casacore::Double time, const casacore::Vector<casacore::Double>& freq);

  // Access to the result
  casacore::Array<casacore::Float>& resultF(casacore::Int obs, casacore::Int fld, casacore::Int spw) { return result_(spw,fld,thisobs(obs)); };
  casacore::Array<casacore::Complex> resultC(casacore::Int obs, casacore::Int fld, casacore::Int spw) { return RIorAPArray(result_(spw,fld,thisobs(obs))).c(); };
  casacore::Array<casacore::Bool>& rflag(casacore::Int obs, casacore::Int fld, casacore::Int spw) { return resFlag_(spw,fld,thisobs(obs)); };

  // Temporary public function for testing
  casacore::Array<casacore::Float>& tresultF(casacore::Int obs, casacore::Int fld, casacore::Int spw) { return timeResult_(spw,fld,thisobs(obs)); };
  casacore::Array<casacore::Bool>& tresultFlag(casacore::Int obs, casacore::Int fld, casacore::Int spw) { return timeResFlag_(spw,fld,thisobs(obs)); };

  // spwOK info for users
  casacore::Bool spwOK(casacore::Int spw) const;
  casacore::Bool spwInOK(casacore::Int spw) const;

  // Const access to various state
  // TBD

  // Report state
  void state();
    
    // Access to the MSMetaInfoForCal (throws if none)
  const MSMetaInfoForCal& msmc() const
  {
      if (msmc_) return *msmc_;
      else throw(casacore::AipsError("VisCal::msmc(): No MSMetaInfoForCal object!"));
  };

private:

  // Null ctor does nothing
  CTPatchedInterp() :
    mtype_(VisCalEnum::GLOBAL),
    freqInterpMethod0_(casacore::InterpolateArray1D<casacore::Double,casacore::Float>::nearestNeighbour) {};
  
  // Setup methods
  void sliceTable();
  void makeInterpolators();

  casacore::Int thisobs(casacore::Int obs) { return (byObs_?obs:0); };
    
  // The MSMetaInfoForCal pointer
  const MSMetaInfoForCal* msmc_;

  // Methods to set up 1:1 patch-panel maps
  //  Private for now as not yet ready to control from outside
  // Field
  // default: all 0 (no field-dep yet)
  void setDefFldMap() {fldMap_.resize(nMSFld_); fldMap_.set(0);};
  void setFldMap(const casacore::MSField& msfld);           // via nearest on-sky
  void setFldMap(const casacore::MSFieldColumns& fcol);  // via nearest on-sky
  void setFldMap(casacore::Vector<casacore::Int>& fldmap);        // via ordered index list
  //void setFldMap(casacore::Vector<casacore::String>& field);     // via name matching
  //void setFldMap(casacore::uInt to, casacore::uInt from);        // via single to/from 

  // Calculate fldmap redundancy, enabling reuse
  void calcAltFld();

  // Spw
  // default: indgen (index identity)
  void setDefSpwMap() {spwMap_.resize(nMSSpw_); indgen(spwMap_);};
  void setSpwMap(casacore::Vector<casacore::Int>& spwmap);
  //void setSpwMap(casacore::Vector<casacore::Double>& refFreqs);  // via refFreq matching
  //void setSpwMap(casacore::uInt to, casacore::uInt from);        // via single to/from

  // Antenna
  // default: indgen (index identity) 
  void setDefAntMap() {antMap_.resize(nMSAnt_); indgen(antMap_);};
  //void setAntMap(casacore::Vector<casacore::Int>& ant);          // via ordered index list
  //void setAntMap(casacore::Vector<casacore::String>& ant);       // via name/station matching
  //void setAntMap(casacore::uInt to, casacore::uInt from);        // via single to/from

  // Set generic antenna/baseline map
  void setElemMap();

  // Resample in frequency
  void resampleInFreq(casacore::Matrix<casacore::Float>& fres,casacore::Matrix<casacore::Bool>& fflg,const casacore::Vector<casacore::Double>& fout,
		      casacore::Matrix<casacore::Float>& tres,casacore::Matrix<casacore::Bool>& tflg,const casacore::Vector<casacore::Double>& fin);
  void resampleFlagsInFreq(casacore::Vector<casacore::Bool>& flgout,const casacore::Vector<casacore::Double>& fout,
			   casacore::Vector<casacore::Bool>& flgin,const casacore::Vector<casacore::Double>& fin);

  // Baseline index from antenna indices: (assumes a1<=a2 !!)
  inline casacore::Int blnidx(const casacore::Int& a1, const casacore::Int& a2, const casacore::Int& nAnt) { return  a1*nAnt-a1*(a1+1)/2+a2; };

  // Translate freq axis interpolation string
  casacore::InterpolateArray1D<casacore::Double,casacore::Float>::InterpolationMethod ftype(casacore::String& strtype);


  // PRIVATE DATA:
  
  // The Caltable
  NewCalTable ct_;

  // casacore::Matrix type
  VisCalEnum::MatrixType mtype_;

  // Are parameters fundamentally complex?
  casacore::Bool isCmplx_;

  // The number of (casacore::Float) parameters (per-chan, per-element)
  casacore::Int nPar_, nFPar_;

  // Interpolation modes
  casacore::String timeType_, freqTypeStr_;

  // Relative-to-center interpolation
  casacore::Bool relativeFreq_;

  // Freq-dep interpolation method (from InterpolateArray1D enum)
#define INTERPMETHOD casacore::InterpolateArray1D<casacore::Double,casacore::Float>::InterpolationMethod
  const INTERPMETHOD freqInterpMethod0_;  // user-specified
  INTERPMETHOD freqInterpMethod_;         // current
  casacore::Vector<INTERPMETHOD> freqInterpMethodVec_;  // per ms spw
  
  // Are we slicing caltable by field?
  casacore::Bool byObs_,byField_;

  // CalTable freq axis info
  casacore::Vector<casacore::Int> nChanIn_;
  casacore::Vector<casacore::Vector<casacore::Double> > freqIn_;


  // Obs, Field, Spw, Ant _output_ (casacore::MS) sizes 
  //   calibration required for up to this many
  casacore::Int nMSObs_, nMSFld_, nMSSpw_, nMSAnt_, nMSElem_;

  // Alternate field indices
  casacore::Vector<casacore::Int> altFld_;

  // Obs, Field, Spw, Ant _input_ (CalTable) sizes
  //  patch panels should not violate these (point to larger indices)
  casacore::Int nCTObs_, nCTFld_, nCTSpw_, nCTAnt_, nCTElem_;

  // OK flag
  casacore::Vector<casacore::Bool> spwInOK_;

  // The patch panels
  //   Each has length from casacore::MS, values refer to CT
  casacore::Vector<casacore::Int> fldMap_, spwMap_, antMap_, elemMap_;

  // Control conjugation of baseline-based solutions when mapping requires
  casacore::Vector<casacore::Bool> conjTab_;

  // Internal result Arrays
  casacore::Cube<casacore::Cube<casacore::Float> > timeResult_,freqResult_;   // [nMSSpw_,nMSFld_,nMSObs_][nFpar,nChan,nAnt]
  casacore::Cube<casacore::Cube<casacore::Bool> >  timeResFlag_,freqResFlag_; // [nMSSpw_,nMSFld_,nMSObs_][nFpar,nChan,nAnt]

  // Current interpolation result Arrays
  //  These will reference time or freq result, depending on context,
  //  and may be referenced by external code
  casacore::Cube<casacore::Cube<casacore::Float> > result_;     // [nMSSpw_,nMSFld_,nMSObs_][nFpar,nChan,nAnt]
  casacore::Cube<casacore::Cube<casacore::Bool> >  resFlag_;    // [nMSSpw_,nMSFld_,nMSObs_][nFpar,nChan,nAnt]

  // The CalTable slices
  casacore::Array<NewCalTable*> ctSlices_;  // [nCTElem_,nCTSpw_,nCTFld_,nCTObs_]

  // The pre-patched casacore::Time interpolation engines
  //   These are populated by the available caltables slices
  casacore::Array<CTTimeInterp1*> tI_;  // [nMSElem_,nMSSpw_,nMSFld_,nMSObs_]
  casacore::Array<casacore::Bool> tIdel_;         // [nMSElem_,nMSSpw_,nMSFld_,mMSObs_]

  casacore::Vector<casacore::Int> lastFld_,lastObs_;

  // Pointer to the factory to use to create CTTimeInterp1s (or specializations)
  CTTIFactoryPtr cttifactoryptr_;

};


} //# NAMESPACE CASA - END

#endif
