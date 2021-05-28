//# DOnewsimulator: defines classes for simulator DO.
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002
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

#ifndef SYNTHESIS_SIMULATOR_H
#define SYNTHESIS_SIMULATOR_H

#include <casa/aips.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MPosition.h>
#include <synthesis/TransformMachines/BeamSquint.h>
#include <synthesis/TransformMachines/VPSkyJones.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
//#include <synthesis/MeasurementComponents/EPJones.h>

#include <casa/namespace.h>
namespace casacore{

class MeasurementSet;
class MEpoch;
class NewMSSimulator;
template<class T> class PagedImage;
}

namespace casa { //# NAMESPACE CASA - BEGIN
class VisSet;
class VisCal;
class ACoh;
class SkyEquation;
class ComponentList;
class CleanImageSkyModel;
class FTMachine;
class ComponentFTMachine;





// <summary>Simulates MeasurementSets from SkyModel and SkyEquation</summary>


// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="casacore::MeasurementSet">casacore::MeasurementSet</linkto>
//   <li> <linkto class="SkyEquation">SkyEquation</linkto>
//   <li> <linkto class="SkyModel">SkyModel</linkto>
// </prerequisite>
//
// <etymology>
// The name MUST have the 'DO' prefix as this class is derived from
// ApplicationObject, and hence is classified as a distributed object. For the
// same reason the rest of its name must be in lower case. This class 
// simulates visibility data, so it is called DOnewsimulator.
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <motivation> 
// This class was written to make the simulation capability useful from glish
// </motivation>
//
// <thrown>
// <li> casacore::AipsError - if an error occurs
// </thrown>
//
// <todo asof="1999/07/22">
//   <li> everything
// </todo>

class Simulator 
{

public:
 
  Simulator();
  // Construct from string
  Simulator(casacore::String& msname);

  // "newsimulator" ctor
  Simulator(casacore::MeasurementSet &thems);
  
  // Return the name of the MeasurementSet
  casacore::String name() const;
  
  Simulator(const Simulator &other);

  Simulator &operator=(const Simulator &other);
 
  ~Simulator();
  
  casacore::Bool close();


  // Select the data to be predicted or corrupted
  casacore::Bool setdata(const casacore::Vector<casacore::Int>& spectralwindowids,
	       const casacore::Vector<casacore::Int>& fieldids,
	       const casacore::String& msSelect);
  
  casacore::Bool settimes(const casacore::Quantity& integrationTime, 
		const casacore::Bool      useHourAngle,
		const casacore::MEpoch&   refTime);

  // set the configuration; NOTE: the telname used
  // here will determine the Voltage Pattern to be used in the
  // simulation
  casacore::Bool setconfig(const casacore::String& telname,
		 const casacore::Vector<casacore::Double>& x, 
		 const casacore::Vector<casacore::Double>& y, 
		 const casacore::Vector<casacore::Double>& z,
		 const casacore::Vector<casacore::Double>& dishDiameter,
		 const casacore::Vector<casacore::Double>& offset,
		 const casacore::Vector<casacore::String>& mount,
		 const casacore::Vector<casacore::String>& antName,
		 const casacore::Vector<casacore::String>& padName,
		 const casacore::String& coordsystem,
		 const casacore::MPosition& referenceLocation);
  // get info back from e.g. loaded ms in newmssimulator
  casacore::Bool getconfig();

  // set the observed fields for the simulation
  casacore::Bool setfield(const casacore::String& sourceName,           
		const casacore::MDirection& sourceDirection,  
		const casacore::String& calCode,
		const casacore::Quantity& distance);


  // set automatic fields for a mosaic
  casacore::Bool setmosaicfield(const casacore::String& sourcename, const casacore::String& calcode, 
		      const casacore::MDirection& fieldcenter, 
		      const casacore::Int xmosp, const casacore::Int ymosp, 
		      const casacore::Quantity& mosspacing, const casacore::Quantity& distance);


  // set one or more spectral windows and their characteristics
  casacore::Bool setspwindow(const casacore::String& spwName,
		   const casacore::Quantity& freq,
		   const casacore::Quantity& deltafreq,
		   const casacore::Quantity& freqresolution,
		   const casacore::MFrequency::Types& freqType,
		   const casacore::Int nchannels,
		   const casacore::String& stokes);

  // Set the simulated feed characteristics
  casacore::Bool setfeed(const casacore::String& mode,
	       const casacore::Vector<casacore::Double>& x,
	       const casacore::Vector<casacore::Double>& y,
	       const casacore::Vector<casacore::String>& pol);		

  // Set the voltage pattern
  casacore::Bool setvp(const casacore::Bool dovp,
             const casacore::Bool defaultVP,
             const casacore::String& vpTable,
             const casacore::Bool doSquint,
             const casacore::Quantity &parAngleInc,
	     const casacore::Quantity &skyPosThreshold,
	     const casacore::Float &pbLimit);

  // Set the random number generator seed for the addition of errors
  casacore::Bool setseed(const casacore::Int seed);

  // Arrange to corrupt with existing calibration
  //   (cf Calibrater setapply)
  casacore::Bool setapply (const casacore::String& type,
                 const casacore::Double& t,
                 const casacore::String& table,
                 const casacore::String& spw,
                 const casacore::String& field,
                 const casacore::String& interp,
                 const casacore::Bool& calwt,
                 const casacore::Vector<casacore::Int>& spwmap,
                 const casacore::Float& opacity);

  // Apply antenna-based gain errors
  casacore::Bool setgain(const casacore::String& mode, 
	       const casacore::String& table,
	       const casacore::Quantity& interval,
	       const casacore::Vector<casacore::Double>& amplitude);

  casacore::Bool settrop(const casacore::String& mode, 
	       const casacore::String& table,
	       const casacore::Float pwv,
	       const casacore::Float deltapwv,
	       const casacore::Float beta,
	       const casacore::Float windspeed);

  // Apply antenna pointing and squint errors
  casacore::Bool setpointingerror(const casacore::String& epJTableName,
			const casacore::Bool applyPointingOffsets,
			const casacore::Bool doPBCorrection);

  // Apply polarization leakage errors
  casacore::Bool setleakage(const casacore::String& mode, const casacore::String& table,
		  //const casacore::Quantity& interval, 
		  const casacore::Vector<casacore::Double>& amplitude,
		  const casacore::Vector<casacore::Double>& offset);

  // Apply bandpass errors
  casacore::Bool setbandpass(const casacore::String& mode, const casacore::String& table,
		   const casacore::Quantity& interval, const casacore::Vector<casacore::Double>& amplitude);

  // Simulate the parallactic angle phase effect
  casacore::Bool setpa(const casacore::String& mode, const casacore::String& table,
	     const casacore::Quantity& interval);

  // Simulate quasi-realistic thermal noise, which can depend upon
  // elevation, bandwidth, antenna diameter, as expected
  casacore::Bool oldsetnoise(const casacore::String& mode, 
		   const casacore::String& table,
		   const casacore::Quantity& simplenoise,
		   const casacore::Float antefficiency,
		   const casacore::Float correfficiency,
		   const casacore::Float spillefficiency,
		   const casacore::Float tau,
		   const casacore::Float trx,
		   const casacore::Float tatmos, 
		   const casacore::Float tcmb);

  casacore::Bool setnoise(const casacore::String& mode,
		const casacore::String& caltable,
		const casacore::Quantity& simplenoise,
		// if blank, not stored
		// or ATM calculation
		const casacore::Quantity& pground,
		const casacore::Float relhum,
		const casacore::Quantity& altitude,
		const casacore::Quantity& waterheight,
		const casacore::Quantity& pwv,
		// user-specified tau and tatmos 
		const casacore::Float tatmos,
		const casacore::Float tau,
		//
		const casacore::Float antefficiency,
		const casacore::Float spillefficiency,
		const casacore::Float correfficiency,
		const casacore::Float trx,
		const casacore::Float tground,
		const casacore::Float tcmb,
		const casacore::Bool OTF,
		const casacore::Float senscoeff,
		const casacore::Int rxtype);

  // apply errors to the data in our MS
  //  casacore::Bool corrupt();
  casacore::Bool corrupt();

  // Set limits
  casacore::Bool setlimits(const casacore::Double shadowFraction,
		 const casacore::Quantity& elevationLimit);

  // Set autocorrelation weight
  casacore::Bool setauto(const casacore::Double autocorrwt);


  // add new visibilities as described by the set methods to an existing
  // or just created measurement set
  casacore::Bool observe(const casacore::String& sourcename, const casacore::String& spwname,
	       const casacore::Quantity& startTime, 
	       const casacore::Quantity& stopTime,
	       const casacore::Bool add_observationn=true,
	       const casacore::Bool state_sig=true,
	       const casacore::Bool state_ref=true,
	       const double& state_cal=0.,
	       const double& state_load=0.,
	       const unsigned int state_sub_scan=1,
	       const casacore::String& state_obs_mode="OBSERVE_TARGET#ON_SOURCE",
	       const casacore::String& observername="CASA simulator",
	       const casacore::String& projectname="CASA simulation");


  casacore::Bool observemany(const casacore::Vector<casacore::String>& sourcenames, const casacore::String& spwname,
		   const casacore::Vector<casacore::Quantity>& startTimes, 
		   const casacore::Vector<casacore::Quantity>& stopTimes,
		   const casacore::Vector<casacore::MDirection>& directions,
		   const casacore::Bool add_observation,
		   const casacore::Bool state_sig,
		   const casacore::Bool state_ref,
		   const double& state_cal,
		   const double& state_load,
		   const unsigned int state_sub_scan,
		   const casacore::String& state_obs_mode,
		   const casacore::String& observername,
		   const casacore::String& projectname);
    

  // Given a model image, predict the visibilities onto the (u,v) coordinates
  // of our MS
  casacore::Bool predict(const casacore::Vector<casacore::String>& modelImage, 
	       const casacore::String& compList, 
	       const casacore::Bool incremental);

  casacore::String state();

  casacore::Bool summary();

  casacore::Bool resetviscal();
  casacore::Bool resetimcal();
  casacore::Bool reset();

  // Set the processing options
  casacore::Bool setoptions(const casacore::String& ftmachine, 
			    const casacore::Int cache, 
			    const casacore::Int tile,
			    const casacore::String& gridfunction, 
			    const casacore::MPosition& mLocation,
			    const casacore::Float padding, 
			    const casacore::Int facets,
			    const casacore::Double maxData,
			    const casacore::Int wprojPlanes);

 
  // Set the print level
  inline void setPrtlev(const casacore::Int& prtlev) { prtlev_=prtlev; };
  // Return print (cout) level
  inline casacore::Int& prtlev() { return prtlev_; };

  
private:

  // Get VP record - copied from SynthesisImager
  void getVPRecord(casacore::Record &rec, PBMath::CommonPB &kpb, casacore::String telescop);
  
  // Arrange to corrupt with simulated calibration
  //   (cf Calibrater setapply)
  casacore::Bool create_corrupt(casacore::Record& simpar);

  // Prints an error message if the newsimulator DO is detached and returns true.
  casacore::Bool detached() const;

  // set up some defaults
  void defaults();

  // Make a VisSet if needed
  void makeVisSet();

  // print out some help about create()
  // casacore::Format direction nicely
  casacore::String formatDirection(const casacore::MDirection& direction);

  // casacore::Format time nicely
  casacore::String formatTime(const casacore::Double time);
  
  // individual summary() functions
  // <group>
  casacore::Bool createSummary(casacore::LogIO& os);
  casacore::Bool configSummary(casacore::LogIO& os);
  casacore::Bool fieldSummary(casacore::LogIO& os);
  casacore::Bool spWindowSummary(casacore::LogIO& os);
  casacore::Bool feedSummary(casacore::LogIO& os);
  casacore::Bool timeSummary(casacore::LogIO& os);

  casacore::Bool predictSummary(casacore::LogIO& os);
  casacore::Bool vpSummary(casacore::LogIO& os);
  casacore::Bool optionsSummary(casacore::LogIO& os);

  casacore::Bool corruptSummary(casacore::LogIO& os);
  casacore::Bool noiseSummary(casacore::LogIO& os);
  // </group>


  // SkyEquation management
  // <group>
  casacore::Bool createSkyEquation( const casacore::Vector<casacore::String>& image, const casacore::String complist);
  void destroySkyEquation();
  // </group>

  casacore::String msname_p;
  casacore::MeasurementSet* ms_p;
  casacore::MeasurementSet* mssel_p;
  VisSet* vs_p;

  casacore::Int seed_p;

  ACoh     *ac_p;

  SkyEquation* se_p;
  CleanImageSkyModel* sm_p;
  FTMachine *ft_p;
  ComponentFTMachine *cft_p;

  casacore::Int nmodels_p;
  casacore::PtrBlock<casacore::PagedImage<casacore::Float>* > images_p;
  ComponentList *componentList_p;

  casacore::String ftmachine_p, gridfunction_p;
  casacore::Int cache_p, tile_p;
  casacore::MPosition mLocation_p;
  casacore::Float padding_p;
  //casacore::Bool MSMayBeOK;
  casacore::Int facets_p;
  casacore::Int wprojPlanes_p;
  //casacore::Long maxData_p;


  // info for coordinates and station locations
  // <group>
  casacore::Bool           areStationCoordsSet_p;
  casacore::String         telescope_p;
  casacore::Vector<casacore::Double> x_p;
  casacore::Vector<casacore::Double> y_p;
  casacore::Vector<casacore::Double> z_p;
  casacore::Vector<casacore::Double>  diam_p;
  casacore::Vector<casacore::Double>  offset_p;
  casacore::Vector<casacore::String> mount_p;
  casacore::Vector<casacore::String> antName_p;
  casacore::Vector<casacore::String> padName_p;
  casacore::String         coordsystem_p;
  casacore::MPosition      mRefLocation_p;
  // </group>

  // info for observed field parameters
  // <group>

  casacore::Int nField;
  casacore::Vector<casacore::String> 	sourceName_p;
  casacore::Vector<casacore::String>        calCode_p;
  casacore::Vector<casacore::MDirection>	sourceDirection_p;
  casacore::Vector<casacore::Quantity>      distance_p;

  // </group>
  // VisEquation handles corruption by visibility calibration effects
  VisEquation ve_p;
  // Generic container for any number of calibration effects to corrupt with
  casacore::PtrBlock<VisCal*> vc_p;

  // info for spectral window parameters
  // <group>

  // spectral windows data
  // <group>
  // RI 20091107 durn. whoever build this didn't enable multiple spw.
  // we'll at least get some functionality here, but there are probably
  // combinations of pols etc that won't properly report here. 
  // better than now, when it doesn't even report more than one spw.
  casacore::Int nSpw;
  casacore::Vector<casacore::String> 	spWindowName_p; 
  casacore::Vector<casacore::Int>		nChan_p;
  casacore::Vector<casacore::Quantity>     	startFreq_p;
  casacore::Vector<casacore::Quantity>     	freqInc_p;
  casacore::Vector<casacore::Quantity>     	freqRes_p;
  casacore::Vector<casacore::String>     	stokesString_p;   
  // </group>
  // </group>


  // Feed information (there will be much more coming,
  // but we are brain dead at this moment).
  casacore::String feedMode_p;
  casacore::Int nFeeds_p;
  casacore::Bool feedsHaveBeenSet;
  casacore::Bool feedsInitialized;

  // Some times which are required for settimes
  // <group>
  casacore::Quantity integrationTime_p;
  casacore::Bool     useHourAngle_p;
  casacore::MEpoch   refTime_p;
  casacore::Bool timesHaveBeenSet_p;
  // </group>

  // Some parameters for voltage pattern (vp):
  // <group>
  casacore::Bool doVP_p;			// Do we apply VP or not?
  casacore::Bool doDefaultVP_p;		// Do we use the default VP for this telescope?
  casacore::String vpTableStr_p;		// Otherwise, use the VP specified in this Table
  casacore::Quantity  parAngleInc_p;	// Parallactic Angle increment
  casacore::Quantity  skyPosThreshold_p;  // a tolerance in the pointing center position
  casacore::Float pbLimit_p;              // The PB level (in percentage) after which the PB is assumed to be zero
  BeamSquint::SquintType  squintType_p;	// Control of squint to use
  VPSkyJones* vp_p;		// pointer to VPSkyJones for the sky equation
  VPSkyJones* gvp_p;		// pointer to VPSkyJones for the sky equation
  // </group>

  // Saving some information about the various corrupting terms
  // <group>
  casacore::String noisemode_p;
  // </group>

  // Cache the newsimulator
  casacore::NewMSSimulator* sim_p;

  // The Jones matrix to hold the antenna pointing offsets and the
  // associated table name.  if applyPointingOffsets is false, only
  // VLA polarization squint will be included in EPJones.  If
  // doPBCorrection is true, the model image will be divided by the
  // primary beam before being used to predict the visibilities.
  // <group>
  //  EPJones *epJ_p;
  casacore::String epJTableName_p;
  casacore::Bool applyPointingOffsets_p;
  casacore::Bool doPBCorrection_p;
  // </group>
  
  casacore::Int prtlev_;

};

} //# NAMESPACE CASA - END

#endif
