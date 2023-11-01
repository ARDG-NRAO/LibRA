//# Calibrater.h: High-level calibrator object; controls calibration overall
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
//#
//# $Id: Calibrater.h,v 19.16 2006/02/14 19:46:23 gmoellen Exp $

#ifndef SYNTHESIS_CALIBRATER_H
#define SYNTHESIS_CALIBRATER_H

#include <casacore/casa/aips.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/measures/Measures/MRadialVelocity.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/VisCalGlobals.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/ms/MeasurementSets/MSHistoryHandler.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisibilityProcessing.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <msvis/MSVis/SimpleSimVi2.h>


namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>Controls the solution of calibration components (Jones Matrices)</summary>

// Forward declarations
class MSMetaInfoForCal;

class Calibrater 
{

 public:
  // Default constructor
  Calibrater();


  // Simple MS-only ctor
  Calibrater(casacore::String msname);

  // Simple simulated-data Calibrater
  Calibrater(const vi::SimpleSimVi2Parameters& ssvp);

  // Destructor
  virtual ~Calibrater();

  // Calibrater factories
  static Calibrater* factory(casacore::Bool old=true);
  static Calibrater* factory(casacore::String msname,casacore::Bool old=true);
  
  // Set uv-data selection via MSSelection
  virtual void selectvis(const casacore::String& time="",
			 const casacore::String& spw="",
			 const casacore::String& scan="",
			 const casacore::String& field="",
			 const casacore::String& intent="",
			 const casacore::String& obsIDs="",
			 const casacore::String& baseline="",
			 const casacore::String& uvrange="",
			 const casacore::String& chanmode="none",
			 const casacore::Int& nchan=1,
			 const casacore::Int& start=0, 
			 const casacore::Int& step=1,
			 const casacore::MRadialVelocity& mStart=casacore::MRadialVelocity(),
			 const casacore::MRadialVelocity& mStep=casacore::MRadialVelocity(),
			 const casacore::String& msSelect="");

  // Set up to apply calibration (using casacore::MSSelection syntax)
  casacore::Bool setapply (const casacore::String& type, 
		 const casacore::Double& t,
		 const casacore::String& table,
		 const casacore::String& spw,
		 const casacore::String& field,
		 const casacore::String& interp,
		 const casacore::Bool& calwt,
		 const casacore::Vector<casacore::Int>& spwmap,
		 const casacore::Vector<casacore::Double>& opacity);

  virtual casacore::Bool setapply (const casacore::String& type, 
				   const casacore::Record& applypar);

  // Set up apply-able calibration via a Cal Library
  virtual casacore::Bool setcallib(casacore::Record /*callib*/) { throw(casacore::AipsError("Calibrater::setcallib not implemented")); };
  virtual casacore::Bool setcallib2(casacore::Record callib, const casacore::MeasurementSet* ms=0);
  casacore::Bool validatecallib(casacore::Record callib);

  casacore::Bool setmodel(const casacore::String& modelImage);
  casacore::Bool setModel(const casacore::Vector<casacore::Double>& stokes);

  // Arrange to solve (using casacore::MSSelection syntax)
  casacore::Bool setsolve (const casacore::String& type, 
		 const casacore::String& solint,
		 const casacore::String& table,
		 const casacore::Bool append,
		 const casacore::Double preavg, 
		 const casacore::String& apmode="AP",
		 const casacore::Int minblperant=4,
		 const casacore::String& refant="",
		 const casacore::String& refantmode="flex",
		 const casacore::Bool solnorm=false,
		 const casacore::String& normtype="mean",
		 const casacore::Float minsnr=0.0f,
		 const casacore::String& combine="",
		 const casacore::Int fillgaps=0,
		 const casacore::String& cfcache="",
		 const casacore::Double painc=360.0,
                 const casacore::Int fitorder=0,
                 const casacore::Float fraction=0.1,
                 const casacore::Int numedge=-1,
                 const casacore::String& radius="",
                 const casacore::Bool smooth=true,
                 const casacore::Bool zerorates=false,
       const casacore::Bool globalsolve=true,
       const casacore::Int niter=100,
       const casacore::Vector<casacore::Double>& delaywindow=casacore::Vector<casacore::Double>(), 
       const casacore::Vector<casacore::Double>& ratewindow=casacore::Vector<casacore::Double>(),
       const casacore::Vector<casacore::Bool>& paramactive=casacore::Vector<casacore::Bool>(),
		 const casacore::String& solmode="",
		 const casacore::Vector<casacore::Double>& rmsthresh=casacore::Vector<casacore::Double>()
);

  // Arrange to solve for BPOLY (using casacore::MSSelection syntax)
  casacore::Bool setsolvebandpoly(const casacore::String& table,
			const casacore::Bool& append,
			const casacore::String& solint,
			const casacore::String& combine,
			const casacore::Vector<casacore::Int>& degree,
			const casacore::Bool& visnorm,
			const casacore::Bool& solnorm,
			const casacore::Int& maskcenter,
			const casacore::Float& maskedge,
			const casacore::String& refant);

  // Arrange to solve for GSPLINE (using casacore::MSSelection syntax)
  casacore::Bool setsolvegainspline(const casacore::String& table,
			  const casacore::Bool& append,
			  const casacore::String& mode,
			  const casacore::Double& splinetime,
			  const casacore::Double& preavg,
			  const casacore::Int& numpoint,
			  const casacore::Double& phasewrap,
			  const casacore::String& refant);

  virtual casacore::Bool setsolve (const casacore::String& type, 
				   const casacore::Record& solvepar);

  // Unset all (default) or one apply calibration component
  casacore::Bool unsetapply(const casacore::Int& which=-1);
  // TBD:   casacore::Bool unsetapply(const casacore::String& type);  // by type?

  // Unset the solved-for calibration component
  casacore::Bool unsetsolve();

  // Set corrDepFlags toggle
  casacore::Bool setCorrDepFlags(const casacore::Bool& corrDepFlags = false);

  // Reset the VisCal lists in the Calibrator object
  casacore::Bool reset(const casacore::Bool& apply=true, 
		       const casacore::Bool& solve=true);

  // Apply all setapply'd calibration components to DATA and
  //  deposit in the CORRECTED_DATA column
  virtual casacore::Bool correct(casacore::String /* mode="calflag"*/)  { throw(casacore::AipsError("Calibrater::correct not implemented")); };
  casacore::Bool correct2(casacore::String mode="calflag");

  // Apply all setapply'd calibration components to MODEL_DATA and
  //  deposit in the MODEL_DATA column
  virtual casacore::Bool corrupt() { return this->corrupt2(); };
  casacore::Bool corrupt2();

  // Initialize sigma/weight, and possibly weight_spectrum
  casacore::Bool initWeights(casacore::String wtmode, casacore::Bool dowtsp=false);
  casacore::Bool initWeights(casacore::Bool doBT=true, casacore::Bool dowtsp=false);
  virtual casacore::Bool initWeightsWithTsys(casacore::String wtmode, casacore::Bool dowtsp=false, casacore::String tsystable="",
					     casacore::String gainfield="", casacore::String interp="linear", 
					     casacore::Vector<casacore::Int> spwmap=casacore::Vector<casacore::Int>());

  // Solve for a given calibration component
  virtual casacore::Bool solve();

  // Modelfit
  virtual casacore::Vector<casacore::Double> modelfit(const casacore::Int& /*iter*/,
						      const casacore::String& /*stype*/,
						      const casacore::Vector<casacore::Double>& /*par*/,
						      const casacore::Vector<casacore::Bool>& /*vary*/,
						      const casacore::String& /*file*/)  { throw(casacore::AipsError("Calibrater::modelfit not implemented")); };

  // Fluxscale (using casacore::MSSelection syntax for fields)
  void fluxscale(const casacore::String& infile, 
		 const casacore::String& outfile,
		 const casacore::String& refFields, 
		 const casacore::Vector<casacore::Int>& refSpwMap,
		 const casacore::String& tranFields,
		 const casacore::Bool& append,
                 const casacore::Float& inGainThres,
                 const casacore::String& antSel,
                 const casacore::String& timerangeSel,
                 const casacore::String& scanSel,
		 SolvableVisCal::fluxScaleStruct& oFluxScaleFactor,
		 casacore::Vector<casacore::Int>& tranidx,
		 const casacore::String& oListFile,
                 const casacore::Bool& incremental,
                 const casacore::Int& fitorder,
                 const casacore::Bool& display);

  // Fluxscale (via field indices)
  virtual void fluxscale(const casacore::String& infile, 
			 const casacore::String& outfile,
			 const casacore::Vector<casacore::Int>& refField, 
			 const casacore::Vector<casacore::Int>& refSpwMap,
			 const casacore::Vector<casacore::Int>& tranField,
			 const casacore::Bool& append,
			 const casacore::Float& inGainThres,
			 const casacore::String& antSel,
			 const casacore::String& timerangeSel,
			 const casacore::String& scanSel,
			 SolvableVisCal::fluxScaleStruct& oFluxScaleFactor,
			 const casacore::String& oListFile,
			 const casacore::Bool& incremental,
			 const casacore::Int& fitorder,
			 const casacore::Bool& display);

  // Accumulate (using casacore::MSSelection syntax)
  virtual void accumulate(const casacore::String& /*intab*/,
			  const casacore::String& /*incrtab*/,
			  const casacore::String& /*outtab*/,
			  const casacore::String& /*fields*/,
			  const casacore::String& /*calFields*/,
			  const casacore::String& /*interp="linear"*/,
			  const casacore::Double& /*t=-1.0*/,
			  const casacore::Vector<casacore::Int>& /*spwmap=casacore::Vector<casacore::Int>(1,-1)*/)  { throw(casacore::AipsError("Calibrater::accumulate not implemented")); };

  // Generate cal table from specified values
  virtual void specifycal(const casacore::String& type,
			  const casacore::String& caltable,
			  const casacore::String& time,
			  const casacore::String& spw,
			  const casacore::String& antenna,
			  const casacore::String& pol,
			  const casacore::Vector<casacore::Double>& parameter,
			  const casacore::String& infile,
			  const casacore::Bool& uniform);

  // casacore::Smooth  calibration (using casacore::MSSelection syntax
  virtual casacore::Bool smooth(const casacore::String& infile,
				casacore::String& outfile, 
				const casacore::String& smoothtype, 
				const casacore::Double& smoothtime,
				const casacore::String& fields);


  // Apply new reference antenna to calibration
  virtual casacore::Bool reRefant(const casacore::String& infile,
				  casacore::String& outfile, 
				  const casacore::String& refantmode, 
				  const casacore::String& refant);


  // casacore::List a calibration table
  virtual casacore::Bool listCal(const casacore::String& infile,
				 const casacore::String& field,
				 const casacore::String& antenna,
				 const casacore::String& spw,
				 const casacore::String& listfile="",
				 const casacore::Int& pagerows=50);

  // Initialize the calibrator object from an input MeasurementSet.
  // Optional compression of the calibration columns (MODEL_DATA,
  // and CORRECTED_DATA) is supported.
  //if addScratch=true, optionally one can just add CORRECTED_DATA by
  //setting addModel to false
  virtual casacore::Bool initialize(casacore::MeasurementSet& inputMS, 
				    casacore::Bool compress=true,
				    casacore::Bool addScratch=true, casacore::Bool addModel=true);

  // Re-initialize the calibration scratch columns
  virtual casacore::Bool initCalSet(const casacore::Int& /*calSet*/)  { throw(casacore::AipsError("Calibrater::initCalSet not implemented")); };;

  // Display the state in a dictionary
  casacore::Record returndict();
  // Get all selected spws
  casacore::Vector<casacore::Int> getSelectedSpws();
  // Get all selected intents
  casacore::Vector<casacore::String> getSelectedIntents();
  // Get all apply tables
  casacore::Vector<casacore::String> getApplyTables();
  // Get solve table
  casacore::Vector<casacore::String> getSolveTable();
  // Get parameters from visibility iterator
  casacore::Bool getIteratorSelection(casacore::Vector<casacore::Int>*,
    casacore::Vector<casacore::Int>*, casacore::Vector<casacore::Int>*,
    casacore::Vector<casacore::Int>*);
  // Convert set to casa vector
  casacore::Vector<casacore::Int> convertSetToVector(const set<casacore::Int>);
  // Report apply/solve state
  casacore::Bool state();
  casacore::Bool applystate();
  casacore::Bool solvestate();

  casacore::Record& getActRec() {return actRec_;};


  // Handle caltable backward compatibility
  static casacore::Bool updateCalTable(const casacore::String& caltable);

  // Return access to the VisEquation
  VisEquation* ve() { return ve_p; };
 
  // Returns true if calibrator object is in a valid state
  virtual casacore::Bool ok();
 
 protected:

  casacore::Bool cleanup();

  // Log functions and variables
  casacore::LogIO sink_p;
  casacore::LogIO& logSink();

  // Method to update casacore::MS HISTORY Table
  void writeHistory(casacore::LogIO& os, 
		    casacore::Bool cliCommand=false);

  // casacore::Time functions and variables
  casacore::String timerString();
  casacore::Timer timer_p;

  // Select on channel using MSSelection
  virtual void selectChannel(const casacore::String& spw);

  // Interpret refant index
  casacore::Vector<casacore::Int> getRefantIdxList(const casacore::String& refant);
  
  casacore::Vector<casacore::Int> getAntIdx(const casacore::String& antenna);

  // Interpret field indices (casacore::MSSelection)
  casacore::Vector<casacore::Int> getFieldIdx(const casacore::String& fields);

  // Interpret spw indices (casacore::MSSelection)
  casacore::Vector<casacore::Int> getSpwIdx(const casacore::String& spws);

  // Interpret spw indices (casacore::MSSelection)
  casacore::Matrix<casacore::Int> getChanIdx(const casacore::String& spws);
  
  // Query apply types to see if we need to calibrate the weights
  casacore::Bool calWt();

  // Given a (supplied) list of uncalibrated spws, determines and returns if there were
  // any, and if so sends them as a warning message to the logger.
  casacore::Bool summarize_uncalspws(const casacore::Vector<casacore::Bool>& uncalspw, const casacore::String& origin,
			   casacore::Bool strictflag=false);

  // Create a VisSet for raw phase transfer if needed
  void getRawPhaseVisSet(casacore::Vector<casacore::Int>& spwid); 

  // The standard solving mechanism (VI2/SDB version)
  virtual casacore::Bool genericGatherAndSolve();

  // casacore::Input casacore::MeasurementSet and derived selected MeasurementSet
  casacore::String msname_p;
  casacore::MeasurementSet* ms_p;
  casacore::MeasurementSet* mssel_p;
  casacore::MeasurementSet* msselRaw_p;

  // casacore::MSSelection object
  casacore::MSSelection* mss_p;
  std::shared_ptr<vi::FrequencySelections> frequencySelections_p;

  // Meta-info server for calibration
  MSMetaInfoForCal* msmc_p;

  // VisEquation derived from the input MeasurementSet
  VisEquation* ve_p;

  // VisCals for applying and solving:
  casacore::PtrBlock<VisCal*> vc_p;
  SolvableVisCal* svc_p;

  // casacore::MeasurementSet selection parameters
  casacore::String dataMode_p;
  casacore::Int dataNchan_p, dataStart_p, dataStep_p;
  casacore::MRadialVelocity mDataStart_p, mDataStep_p;

  //Spline phase wrapping helper params
  casacore::Double phasewrap_p;
  casacore::Int splinepoint_p;

  //Used to update the casacore::MS HISTORY Table
  casacore::Int histLockCounter_p;
  casacore::MSHistoryHandler *hist_p;
  casacore::Table historytab_p;

  casacore::Bool usingCalLibrary_;

  // Toggle for correlation-dependent flags
  //  (if false, when one correlation is flagged, all will be treated as flagged, per-antenna, per-channel)
  //  (CASA tradition is false; use cases where some antennas may only have one polarization, e.g., VLBI need true)
  casacore::Bool corrDepFlags_;

  // Activity record
  casacore::Record actRec_;

 private:
  // Copy constructor and assignment operator are forbidden
  Calibrater(const Calibrater&);
  Calibrater& operator=(const Calibrater&);

  // Simulated-data/testing context info
  const bool simdata_p;
  const vi::SimpleSimVi2Parameters ssvp_p;

  // configuration for calibration dependent data filter
  casacore::Record calFilterConfig_p;
  void setCalFilterConfiguration(casacore::String const &type,
      casacore::Record const &config);

};

// Preserve old-fashioned Calibrater here:

class OldCalibrater : public Calibrater 
{

 public:
  // Default constructor
  OldCalibrater();


  // Simple casacore::MS-only ctor
  OldCalibrater(casacore::String msname);

  // Destructor
  virtual ~OldCalibrater();


  // Set uv-data selection via MSSelection
  virtual void selectvis(const casacore::String& time="",
			 const casacore::String& spw="",
			 const casacore::String& scan="",
			 const casacore::String& field="",
			 const casacore::String& intent="",
			 const casacore::String& obsIDs="",
			 const casacore::String& baseline="",
			 const casacore::String& uvrange="",
			 const casacore::String& chanmode="none",
			 const casacore::Int& nchan=1,
			 const casacore::Int& start=0, 
			 const casacore::Int& step=1,
			 const casacore::MRadialVelocity& mStart=casacore::MRadialVelocity(),
			 const casacore::MRadialVelocity& mStep=casacore::MRadialVelocity(),
			 const casacore::String& msSelect="");

  // Uses *vs_p to ctor VisCals
  using Calibrater::setapply;
  virtual casacore::Bool setapply (const casacore::String& type, 
				   const casacore::Record& applypar);


  // Set up apply-able calibration via a Cal Library
  virtual casacore::Bool setcallib(casacore::Record callib);
  virtual casacore::Bool setcallib2(casacore::Record callib, const casacore::MeasurementSet* ms=0);

  // Uses *vs_p to ctor the SolvableVisCal
  virtual casacore::Bool setsolve (const casacore::String& type, 
				   const casacore::Record& solvepar);
  
  // Apply all setapply'd calibration components to DATA and
  //  deposit in the CORRECTED_DATA column
  virtual casacore::Bool correct(casacore::String mode="calflag");

  // Apply all setapply'd calibration components to MODEL_DATA and
  //  deposit in the MODEL_DATA column
  virtual casacore::Bool corrupt();

  // Initialize sigma/weight, and possibly weight_spectrum
  // Tsys-specific version (SD)
  virtual casacore::Bool initWeightsWithTsys(casacore::String wtmode, casacore::Bool dowtsp=false, casacore::String tsystable="",
					     casacore::String gainfield="", casacore::String interp="linear", 
					     casacore::Vector<casacore::Int> spwmap=casacore::Vector<casacore::Int>());


  // Solve for a given calibration component
  virtual casacore::Bool solve();


  // Modelfit
  virtual casacore::Vector<casacore::Double> modelfit(const casacore::Int& iter,
						      const casacore::String& stype,
						      const casacore::Vector<casacore::Double>& par,
						      const casacore::Vector<casacore::Bool>& vary,
						      const casacore::String& file);

  // Fluxscale (via field indices)
  //  NB: uses *vs_p to make SVCs
  virtual void fluxscale(const casacore::String& infile, 
			 const casacore::String& outfile,
			 const casacore::Vector<casacore::Int>& refField, 
			 const casacore::Vector<casacore::Int>& refSpwMap,
			 const casacore::Vector<casacore::Int>& tranField,
			 const casacore::Bool& append,
			 const casacore::Float& inGainThres,
			 const casacore::String& antSel,
			 const casacore::String& timerangeSel,
			 const casacore::String& scanSel,
			 SolvableVisCal::fluxScaleStruct& oFluxScaleFactor,
			 const casacore::String& oListFile,
			 const casacore::Bool& incremental,
			 const casacore::Int& fitorder,
			 const casacore::Bool& display);

  // Accumulate (using casacore::MSSelection syntax)
  virtual void accumulate(const casacore::String& intab,
			  const casacore::String& incrtab,
			  const casacore::String& outtab,
			  const casacore::String& fields,
			  const casacore::String& calFields,
			  const casacore::String& interp="linear",
			  const casacore::Double& t=-1.0,
			  const casacore::Vector<casacore::Int>& spwmap=casacore::Vector<casacore::Int>(1,-1));

  // Generate cal table from specified values
  //  NB: creates VCs with *vs_p
  virtual void specifycal(const casacore::String& type,
			  const casacore::String& caltable,
			  const casacore::String& time,
			  const casacore::String& spw,
			  const casacore::String& antenna,
			  const casacore::String& pol,
			  const casacore::Vector<casacore::Double>& parameter,
			  const casacore::String& infile,
			  const casacore::Bool& uniform);

  // casacore::Smooth  calibration (using casacore::MSSelection syntax
  // NB: uses *vs_p to create SVC
  virtual casacore::Bool smooth(const casacore::String& infile,
				casacore::String& outfile, 
				const casacore::String& smoothtype, 
				const casacore::Double& smoothtime,
				const casacore::String& fields);

  // casacore::List a calibration table
  virtual casacore::Bool listCal(const casacore::String& infile,
				 const casacore::String& field,
				 const casacore::String& antenna,
				 const casacore::String& spw,
				 const casacore::String& listfile="",
				 const casacore::Int& pagerows=50);


  // Initialize the calibrator object from an input MeasurementSet.
  // Optional compression of the calibration columns (MODEL_DATA,
  // and CORRECTED_DATA) is supported.
  //if addScratch=true, optionally one can just add CORRECTED_DATA by
  //setting addModel to false
  virtual casacore::Bool initialize(casacore::MeasurementSet& inputMS, 
				    casacore::Bool compress=true,
				    casacore::Bool addScratch=true, 
				    casacore::Bool addModel=true);

  // Re-initialize the calibration scratch columns
  virtual casacore::Bool initCalSet(const casacore::Int& calSet);

  casacore::Bool cleanupVisSet();

 protected:

  // Organize sort columns for correction
  virtual VisibilityIterator::DataColumn configureForCorrection ();

  // Select on channel using MSSelection
  virtual void selectChannel(const casacore::String& spw);

  // Channel mask services
  virtual void initChanMask();

  // Select on channel in the VisSet
  virtual void selectChannel(const casacore::String& mode, 
			     const casacore::Int& nchan, const casacore::Int& start, const casacore::Int& step,
			     const casacore::MRadialVelocity& mStart,
			     const casacore::MRadialVelocity& mStep);
  

  // Returns true if calibrator object is in a valid state
  virtual casacore::Bool ok();

  // The standard solving mechanism
  virtual casacore::Bool genericGatherAndSolve();

  // VisSet that hosts an old VI
  VisSet* vs_p;
  VisSet* rawvs_p;

  // channel masking 
  casacore::PtrBlock<casacore::Vector<casacore::Bool>*> chanmask_;

 private:
  // Copy constructor and assignment operator are forbidden
  OldCalibrater(const OldCalibrater&);
  OldCalibrater& operator=(const OldCalibrater&);


};





} //# NAMESPACE CASA - END

#endif
