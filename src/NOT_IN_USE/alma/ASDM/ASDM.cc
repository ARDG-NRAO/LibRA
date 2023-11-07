
/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * Warning!
 *  -------------------------------------------------------------------- 
 * | This is generated code!  Do not modify this file.                  |
 * | If you do, all changes will be lost when the file is re-generated. |
 *  --------------------------------------------------------------------
 *
 * File ASDM.cpp
 */
#include <alma/ASDM/ASDM.h>

#include <alma/ASDM/MainTable.h>

#include <alma/ASDM/AlmaRadiometerTable.h>

#include <alma/ASDM/AnnotationTable.h>

#include <alma/ASDM/AntennaTable.h>

#include <alma/ASDM/CalAmpliTable.h>

#include <alma/ASDM/CalAntennaSolutionsTable.h>

#include <alma/ASDM/CalAppPhaseTable.h>

#include <alma/ASDM/CalAtmosphereTable.h>

#include <alma/ASDM/CalBandpassTable.h>

#include <alma/ASDM/CalCurveTable.h>

#include <alma/ASDM/CalDataTable.h>

#include <alma/ASDM/CalDelayTable.h>

#include <alma/ASDM/CalDeviceTable.h>

#include <alma/ASDM/CalFluxTable.h>

#include <alma/ASDM/CalFocusTable.h>

#include <alma/ASDM/CalFocusModelTable.h>

#include <alma/ASDM/CalGainTable.h>

#include <alma/ASDM/CalHolographyTable.h>

#include <alma/ASDM/CalPhaseTable.h>

#include <alma/ASDM/CalPointingTable.h>

#include <alma/ASDM/CalPointingModelTable.h>

#include <alma/ASDM/CalPositionTable.h>

#include <alma/ASDM/CalPrimaryBeamTable.h>

#include <alma/ASDM/CalReductionTable.h>

#include <alma/ASDM/CalSeeingTable.h>

#include <alma/ASDM/CalWVRTable.h>

#include <alma/ASDM/ConfigDescriptionTable.h>

#include <alma/ASDM/CorrelatorModeTable.h>

#include <alma/ASDM/DataDescriptionTable.h>

#include <alma/ASDM/DelayModelTable.h>

#include <alma/ASDM/DelayModelFixedParametersTable.h>

#include <alma/ASDM/DelayModelVariableParametersTable.h>

#include <alma/ASDM/DopplerTable.h>

#include <alma/ASDM/EphemerisTable.h>

#include <alma/ASDM/ExecBlockTable.h>

#include <alma/ASDM/FeedTable.h>

#include <alma/ASDM/FieldTable.h>

#include <alma/ASDM/FlagTable.h>

#include <alma/ASDM/FlagCmdTable.h>

#include <alma/ASDM/FocusTable.h>

#include <alma/ASDM/FocusModelTable.h>

#include <alma/ASDM/FreqOffsetTable.h>

#include <alma/ASDM/GainTrackingTable.h>

#include <alma/ASDM/HistoryTable.h>

#include <alma/ASDM/HolographyTable.h>

#include <alma/ASDM/ObservationTable.h>

#include <alma/ASDM/PointingTable.h>

#include <alma/ASDM/PointingModelTable.h>

#include <alma/ASDM/PolarizationTable.h>

#include <alma/ASDM/ProcessorTable.h>

#include <alma/ASDM/PulsarTable.h>

#include <alma/ASDM/ReceiverTable.h>

#include <alma/ASDM/SBSummaryTable.h>

#include <alma/ASDM/ScaleTable.h>

#include <alma/ASDM/ScanTable.h>

#include <alma/ASDM/SeeingTable.h>

#include <alma/ASDM/SourceTable.h>

#include <alma/ASDM/SpectralWindowTable.h>

#include <alma/ASDM/SquareLawDetectorTable.h>

#include <alma/ASDM/StateTable.h>

#include <alma/ASDM/StationTable.h>

#include <alma/ASDM/SubscanTable.h>

#include <alma/ASDM/SwitchCycleTable.h>

#include <alma/ASDM/SysCalTable.h>

#include <alma/ASDM/SysPowerTable.h>

#include <alma/ASDM/TotalPowerTable.h>

#include <alma/ASDM/VLAWVRTable.h>

#include <alma/ASDM/WVMCalTable.h>

#include <alma/ASDM/WeatherTable.h>


using asdm::ASDM;

using asdm::MainTable;

using asdm::AlmaRadiometerTable;

using asdm::AnnotationTable;

using asdm::AntennaTable;

using asdm::CalAmpliTable;

using asdm::CalAntennaSolutionsTable;

using asdm::CalAppPhaseTable;

using asdm::CalAtmosphereTable;

using asdm::CalBandpassTable;

using asdm::CalCurveTable;

using asdm::CalDataTable;

using asdm::CalDelayTable;

using asdm::CalDeviceTable;

using asdm::CalFluxTable;

using asdm::CalFocusTable;

using asdm::CalFocusModelTable;

using asdm::CalGainTable;

using asdm::CalHolographyTable;

using asdm::CalPhaseTable;

using asdm::CalPointingTable;

using asdm::CalPointingModelTable;

using asdm::CalPositionTable;

using asdm::CalPrimaryBeamTable;

using asdm::CalReductionTable;

using asdm::CalSeeingTable;

using asdm::CalWVRTable;

using asdm::ConfigDescriptionTable;

using asdm::CorrelatorModeTable;

using asdm::DataDescriptionTable;

using asdm::DelayModelTable;

using asdm::DelayModelFixedParametersTable;

using asdm::DelayModelVariableParametersTable;

using asdm::DopplerTable;

using asdm::EphemerisTable;

using asdm::ExecBlockTable;

using asdm::FeedTable;

using asdm::FieldTable;

using asdm::FlagTable;

using asdm::FlagCmdTable;

using asdm::FocusTable;

using asdm::FocusModelTable;

using asdm::FreqOffsetTable;

using asdm::GainTrackingTable;

using asdm::HistoryTable;

using asdm::HolographyTable;

using asdm::ObservationTable;

using asdm::PointingTable;

using asdm::PointingModelTable;

using asdm::PolarizationTable;

using asdm::ProcessorTable;

using asdm::PulsarTable;

using asdm::ReceiverTable;

using asdm::SBSummaryTable;

using asdm::ScaleTable;

using asdm::ScanTable;

using asdm::SeeingTable;

using asdm::SourceTable;

using asdm::SpectralWindowTable;

using asdm::SquareLawDetectorTable;

using asdm::StateTable;

using asdm::StationTable;

using asdm::SubscanTable;

using asdm::SwitchCycleTable;

using asdm::SysCalTable;

using asdm::SysPowerTable;

using asdm::TotalPowerTable;

using asdm::VLAWVRTable;

using asdm::WVMCalTable;

using asdm::WeatherTable;


#include <alma/ASDM/Parser.h>
#include <alma/ASDM/ConversionException.h>

using asdm::ConversionException;
using asdm::Parser;

#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <time.h>

//#ifdef WITHOUT_ACS
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
//#endif

#include <alma/ASDM/Misc.h>
using namespace asdm;
using namespace std;

namespace asdm { 

	ASDM::Origin ASDM::getOrigin() const { return origin ; }
	
	string ASDM::getDirectory() const { return directory_ ; }
	
	XSLTransformer & ASDM::getXSLTransformer() { return xslTransformer; }
	
	/**
	 * Create an instance of the tables that belong to this model.
	 */
	ASDM::ASDM () {
		Entity emptyEntity;

		main = new MainTable (*this);
		table.push_back(main);
		tableEntity["Main"] = emptyEntity;

		almaRadiometer = new AlmaRadiometerTable (*this);
		table.push_back(almaRadiometer);
		tableEntity["AlmaRadiometer"] = emptyEntity;

		annotation = new AnnotationTable (*this);
		table.push_back(annotation);
		tableEntity["Annotation"] = emptyEntity;

		antenna = new AntennaTable (*this);
		table.push_back(antenna);
		tableEntity["Antenna"] = emptyEntity;

		calAmpli = new CalAmpliTable (*this);
		table.push_back(calAmpli);
		tableEntity["CalAmpli"] = emptyEntity;

		calAntennaSolutions = new CalAntennaSolutionsTable (*this);
		table.push_back(calAntennaSolutions);
		tableEntity["CalAntennaSolutions"] = emptyEntity;

		calAppPhase = new CalAppPhaseTable (*this);
		table.push_back(calAppPhase);
		tableEntity["CalAppPhase"] = emptyEntity;

		calAtmosphere = new CalAtmosphereTable (*this);
		table.push_back(calAtmosphere);
		tableEntity["CalAtmosphere"] = emptyEntity;

		calBandpass = new CalBandpassTable (*this);
		table.push_back(calBandpass);
		tableEntity["CalBandpass"] = emptyEntity;

		calCurve = new CalCurveTable (*this);
		table.push_back(calCurve);
		tableEntity["CalCurve"] = emptyEntity;

		calData = new CalDataTable (*this);
		table.push_back(calData);
		tableEntity["CalData"] = emptyEntity;

		calDelay = new CalDelayTable (*this);
		table.push_back(calDelay);
		tableEntity["CalDelay"] = emptyEntity;

		calDevice = new CalDeviceTable (*this);
		table.push_back(calDevice);
		tableEntity["CalDevice"] = emptyEntity;

		calFlux = new CalFluxTable (*this);
		table.push_back(calFlux);
		tableEntity["CalFlux"] = emptyEntity;

		calFocus = new CalFocusTable (*this);
		table.push_back(calFocus);
		tableEntity["CalFocus"] = emptyEntity;

		calFocusModel = new CalFocusModelTable (*this);
		table.push_back(calFocusModel);
		tableEntity["CalFocusModel"] = emptyEntity;

		calGain = new CalGainTable (*this);
		table.push_back(calGain);
		tableEntity["CalGain"] = emptyEntity;

		calHolography = new CalHolographyTable (*this);
		table.push_back(calHolography);
		tableEntity["CalHolography"] = emptyEntity;

		calPhase = new CalPhaseTable (*this);
		table.push_back(calPhase);
		tableEntity["CalPhase"] = emptyEntity;

		calPointing = new CalPointingTable (*this);
		table.push_back(calPointing);
		tableEntity["CalPointing"] = emptyEntity;

		calPointingModel = new CalPointingModelTable (*this);
		table.push_back(calPointingModel);
		tableEntity["CalPointingModel"] = emptyEntity;

		calPosition = new CalPositionTable (*this);
		table.push_back(calPosition);
		tableEntity["CalPosition"] = emptyEntity;

		calPrimaryBeam = new CalPrimaryBeamTable (*this);
		table.push_back(calPrimaryBeam);
		tableEntity["CalPrimaryBeam"] = emptyEntity;

		calReduction = new CalReductionTable (*this);
		table.push_back(calReduction);
		tableEntity["CalReduction"] = emptyEntity;

		calSeeing = new CalSeeingTable (*this);
		table.push_back(calSeeing);
		tableEntity["CalSeeing"] = emptyEntity;

		calWVR = new CalWVRTable (*this);
		table.push_back(calWVR);
		tableEntity["CalWVR"] = emptyEntity;

		configDescription = new ConfigDescriptionTable (*this);
		table.push_back(configDescription);
		tableEntity["ConfigDescription"] = emptyEntity;

		correlatorMode = new CorrelatorModeTable (*this);
		table.push_back(correlatorMode);
		tableEntity["CorrelatorMode"] = emptyEntity;

		dataDescription = new DataDescriptionTable (*this);
		table.push_back(dataDescription);
		tableEntity["DataDescription"] = emptyEntity;

		delayModel = new DelayModelTable (*this);
		table.push_back(delayModel);
		tableEntity["DelayModel"] = emptyEntity;

		delayModelFixedParameters = new DelayModelFixedParametersTable (*this);
		table.push_back(delayModelFixedParameters);
		tableEntity["DelayModelFixedParameters"] = emptyEntity;

		delayModelVariableParameters = new DelayModelVariableParametersTable (*this);
		table.push_back(delayModelVariableParameters);
		tableEntity["DelayModelVariableParameters"] = emptyEntity;

		doppler = new DopplerTable (*this);
		table.push_back(doppler);
		tableEntity["Doppler"] = emptyEntity;

		ephemeris = new EphemerisTable (*this);
		table.push_back(ephemeris);
		tableEntity["Ephemeris"] = emptyEntity;

		execBlock = new ExecBlockTable (*this);
		table.push_back(execBlock);
		tableEntity["ExecBlock"] = emptyEntity;

		feed = new FeedTable (*this);
		table.push_back(feed);
		tableEntity["Feed"] = emptyEntity;

		field = new FieldTable (*this);
		table.push_back(field);
		tableEntity["Field"] = emptyEntity;

		flag = new FlagTable (*this);
		table.push_back(flag);
		tableEntity["Flag"] = emptyEntity;

		flagCmd = new FlagCmdTable (*this);
		table.push_back(flagCmd);
		tableEntity["FlagCmd"] = emptyEntity;

		focus = new FocusTable (*this);
		table.push_back(focus);
		tableEntity["Focus"] = emptyEntity;

		focusModel = new FocusModelTable (*this);
		table.push_back(focusModel);
		tableEntity["FocusModel"] = emptyEntity;

		freqOffset = new FreqOffsetTable (*this);
		table.push_back(freqOffset);
		tableEntity["FreqOffset"] = emptyEntity;

		gainTracking = new GainTrackingTable (*this);
		table.push_back(gainTracking);
		tableEntity["GainTracking"] = emptyEntity;

		history = new HistoryTable (*this);
		table.push_back(history);
		tableEntity["History"] = emptyEntity;

		holography = new HolographyTable (*this);
		table.push_back(holography);
		tableEntity["Holography"] = emptyEntity;

		observation = new ObservationTable (*this);
		table.push_back(observation);
		tableEntity["Observation"] = emptyEntity;

		pointing = new PointingTable (*this);
		table.push_back(pointing);
		tableEntity["Pointing"] = emptyEntity;

		pointingModel = new PointingModelTable (*this);
		table.push_back(pointingModel);
		tableEntity["PointingModel"] = emptyEntity;

		polarization = new PolarizationTable (*this);
		table.push_back(polarization);
		tableEntity["Polarization"] = emptyEntity;

		processor = new ProcessorTable (*this);
		table.push_back(processor);
		tableEntity["Processor"] = emptyEntity;

		pulsar = new PulsarTable (*this);
		table.push_back(pulsar);
		tableEntity["Pulsar"] = emptyEntity;

		receiver = new ReceiverTable (*this);
		table.push_back(receiver);
		tableEntity["Receiver"] = emptyEntity;

		sBSummary = new SBSummaryTable (*this);
		table.push_back(sBSummary);
		tableEntity["SBSummary"] = emptyEntity;

		scale = new ScaleTable (*this);
		table.push_back(scale);
		tableEntity["Scale"] = emptyEntity;

		scan = new ScanTable (*this);
		table.push_back(scan);
		tableEntity["Scan"] = emptyEntity;

		seeing = new SeeingTable (*this);
		table.push_back(seeing);
		tableEntity["Seeing"] = emptyEntity;

		source = new SourceTable (*this);
		table.push_back(source);
		tableEntity["Source"] = emptyEntity;

		spectralWindow = new SpectralWindowTable (*this);
		table.push_back(spectralWindow);
		tableEntity["SpectralWindow"] = emptyEntity;

		squareLawDetector = new SquareLawDetectorTable (*this);
		table.push_back(squareLawDetector);
		tableEntity["SquareLawDetector"] = emptyEntity;

		state = new StateTable (*this);
		table.push_back(state);
		tableEntity["State"] = emptyEntity;

		station = new StationTable (*this);
		table.push_back(station);
		tableEntity["Station"] = emptyEntity;

		subscan = new SubscanTable (*this);
		table.push_back(subscan);
		tableEntity["Subscan"] = emptyEntity;

		switchCycle = new SwitchCycleTable (*this);
		table.push_back(switchCycle);
		tableEntity["SwitchCycle"] = emptyEntity;

		sysCal = new SysCalTable (*this);
		table.push_back(sysCal);
		tableEntity["SysCal"] = emptyEntity;

		sysPower = new SysPowerTable (*this);
		table.push_back(sysPower);
		tableEntity["SysPower"] = emptyEntity;

		totalPower = new TotalPowerTable (*this);
		table.push_back(totalPower);
		tableEntity["TotalPower"] = emptyEntity;

		vLAWVR = new VLAWVRTable (*this);
		table.push_back(vLAWVR);
		tableEntity["VLAWVR"] = emptyEntity;

		wVMCal = new WVMCalTable (*this);
		table.push_back(wVMCal);
		tableEntity["WVMCal"] = emptyEntity;

		weather = new WeatherTable (*this);
		table.push_back(weather);
		tableEntity["Weather"] = emptyEntity;

		//for (unsigned int i = 0; i < table.size(); ++i)
		//	tableEntity.push_back(new Entity ());
			
		// Define a default entity.		
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("ASDM");
		entity.setEntityVersion("1");
		entity.setInstanceVersion("1");	
		
		
		// Define a default creation time : now.
		time_t rawtime;
		time(&rawtime);
		struct tm* timeInfo = localtime(&rawtime);		
		//setTimeOfCreation(ArrayTime((1900+timeInfo->tm_year), (timeInfo->tm_mon+1), timeInfo->tm_mday, timeInfo->tm_hour, timeInfo->tm_min, (double) timeInfo->tm_sec));
		timeOfCreation = ArrayTime((1900+timeInfo->tm_year), (timeInfo->tm_mon+1), timeInfo->tm_mday, timeInfo->tm_hour, timeInfo->tm_min, (double) timeInfo->tm_sec);
		// Archive binary or XML
		archiveAsBin = false;
		
		// File binary or XML
		fileAsBin = false;			
		
		// Set imperatively to false
		hasBeenAdded = false;
		
		// So far it's created ex nihilo.
		origin = EX_NIHILO;
		loadTablesOnDemand_ = false;
		checkRowUniqueness_ = true;
	}
	
	ASDM::~ASDM () {
		for (unsigned int i = 0; i < table.size(); ++i) {
			delete table[i];
			//delete tableEntity[i];
		}
	}
	

	/**
	 * Get the table Main.
	 * @return The table Main as a MainTable.
	 */
	MainTable & ASDM::getMain () const {
		return *main;
	}

	/**
	 * Get the table AlmaRadiometer.
	 * @return The table AlmaRadiometer as a AlmaRadiometerTable.
	 */
	AlmaRadiometerTable & ASDM::getAlmaRadiometer () const {
		return *almaRadiometer;
	}

	/**
	 * Get the table Annotation.
	 * @return The table Annotation as a AnnotationTable.
	 */
	AnnotationTable & ASDM::getAnnotation () const {
		return *annotation;
	}

	/**
	 * Get the table Antenna.
	 * @return The table Antenna as a AntennaTable.
	 */
	AntennaTable & ASDM::getAntenna () const {
		return *antenna;
	}

	/**
	 * Get the table CalAmpli.
	 * @return The table CalAmpli as a CalAmpliTable.
	 */
	CalAmpliTable & ASDM::getCalAmpli () const {
		return *calAmpli;
	}

	/**
	 * Get the table CalAntennaSolutions.
	 * @return The table CalAntennaSolutions as a CalAntennaSolutionsTable.
	 */
	CalAntennaSolutionsTable & ASDM::getCalAntennaSolutions () const {
		return *calAntennaSolutions;
	}

	/**
	 * Get the table CalAppPhase.
	 * @return The table CalAppPhase as a CalAppPhaseTable.
	 */
	CalAppPhaseTable & ASDM::getCalAppPhase () const {
		return *calAppPhase;
	}

	/**
	 * Get the table CalAtmosphere.
	 * @return The table CalAtmosphere as a CalAtmosphereTable.
	 */
	CalAtmosphereTable & ASDM::getCalAtmosphere () const {
		return *calAtmosphere;
	}

	/**
	 * Get the table CalBandpass.
	 * @return The table CalBandpass as a CalBandpassTable.
	 */
	CalBandpassTable & ASDM::getCalBandpass () const {
		return *calBandpass;
	}

	/**
	 * Get the table CalCurve.
	 * @return The table CalCurve as a CalCurveTable.
	 */
	CalCurveTable & ASDM::getCalCurve () const {
		return *calCurve;
	}

	/**
	 * Get the table CalData.
	 * @return The table CalData as a CalDataTable.
	 */
	CalDataTable & ASDM::getCalData () const {
		return *calData;
	}

	/**
	 * Get the table CalDelay.
	 * @return The table CalDelay as a CalDelayTable.
	 */
	CalDelayTable & ASDM::getCalDelay () const {
		return *calDelay;
	}

	/**
	 * Get the table CalDevice.
	 * @return The table CalDevice as a CalDeviceTable.
	 */
	CalDeviceTable & ASDM::getCalDevice () const {
		return *calDevice;
	}

	/**
	 * Get the table CalFlux.
	 * @return The table CalFlux as a CalFluxTable.
	 */
	CalFluxTable & ASDM::getCalFlux () const {
		return *calFlux;
	}

	/**
	 * Get the table CalFocus.
	 * @return The table CalFocus as a CalFocusTable.
	 */
	CalFocusTable & ASDM::getCalFocus () const {
		return *calFocus;
	}

	/**
	 * Get the table CalFocusModel.
	 * @return The table CalFocusModel as a CalFocusModelTable.
	 */
	CalFocusModelTable & ASDM::getCalFocusModel () const {
		return *calFocusModel;
	}

	/**
	 * Get the table CalGain.
	 * @return The table CalGain as a CalGainTable.
	 */
	CalGainTable & ASDM::getCalGain () const {
		return *calGain;
	}

	/**
	 * Get the table CalHolography.
	 * @return The table CalHolography as a CalHolographyTable.
	 */
	CalHolographyTable & ASDM::getCalHolography () const {
		return *calHolography;
	}

	/**
	 * Get the table CalPhase.
	 * @return The table CalPhase as a CalPhaseTable.
	 */
	CalPhaseTable & ASDM::getCalPhase () const {
		return *calPhase;
	}

	/**
	 * Get the table CalPointing.
	 * @return The table CalPointing as a CalPointingTable.
	 */
	CalPointingTable & ASDM::getCalPointing () const {
		return *calPointing;
	}

	/**
	 * Get the table CalPointingModel.
	 * @return The table CalPointingModel as a CalPointingModelTable.
	 */
	CalPointingModelTable & ASDM::getCalPointingModel () const {
		return *calPointingModel;
	}

	/**
	 * Get the table CalPosition.
	 * @return The table CalPosition as a CalPositionTable.
	 */
	CalPositionTable & ASDM::getCalPosition () const {
		return *calPosition;
	}

	/**
	 * Get the table CalPrimaryBeam.
	 * @return The table CalPrimaryBeam as a CalPrimaryBeamTable.
	 */
	CalPrimaryBeamTable & ASDM::getCalPrimaryBeam () const {
		return *calPrimaryBeam;
	}

	/**
	 * Get the table CalReduction.
	 * @return The table CalReduction as a CalReductionTable.
	 */
	CalReductionTable & ASDM::getCalReduction () const {
		return *calReduction;
	}

	/**
	 * Get the table CalSeeing.
	 * @return The table CalSeeing as a CalSeeingTable.
	 */
	CalSeeingTable & ASDM::getCalSeeing () const {
		return *calSeeing;
	}

	/**
	 * Get the table CalWVR.
	 * @return The table CalWVR as a CalWVRTable.
	 */
	CalWVRTable & ASDM::getCalWVR () const {
		return *calWVR;
	}

	/**
	 * Get the table ConfigDescription.
	 * @return The table ConfigDescription as a ConfigDescriptionTable.
	 */
	ConfigDescriptionTable & ASDM::getConfigDescription () const {
		return *configDescription;
	}

	/**
	 * Get the table CorrelatorMode.
	 * @return The table CorrelatorMode as a CorrelatorModeTable.
	 */
	CorrelatorModeTable & ASDM::getCorrelatorMode () const {
		return *correlatorMode;
	}

	/**
	 * Get the table DataDescription.
	 * @return The table DataDescription as a DataDescriptionTable.
	 */
	DataDescriptionTable & ASDM::getDataDescription () const {
		return *dataDescription;
	}

	/**
	 * Get the table DelayModel.
	 * @return The table DelayModel as a DelayModelTable.
	 */
	DelayModelTable & ASDM::getDelayModel () const {
		return *delayModel;
	}

	/**
	 * Get the table DelayModelFixedParameters.
	 * @return The table DelayModelFixedParameters as a DelayModelFixedParametersTable.
	 */
	DelayModelFixedParametersTable & ASDM::getDelayModelFixedParameters () const {
		return *delayModelFixedParameters;
	}

	/**
	 * Get the table DelayModelVariableParameters.
	 * @return The table DelayModelVariableParameters as a DelayModelVariableParametersTable.
	 */
	DelayModelVariableParametersTable & ASDM::getDelayModelVariableParameters () const {
		return *delayModelVariableParameters;
	}

	/**
	 * Get the table Doppler.
	 * @return The table Doppler as a DopplerTable.
	 */
	DopplerTable & ASDM::getDoppler () const {
		return *doppler;
	}

	/**
	 * Get the table Ephemeris.
	 * @return The table Ephemeris as a EphemerisTable.
	 */
	EphemerisTable & ASDM::getEphemeris () const {
		return *ephemeris;
	}

	/**
	 * Get the table ExecBlock.
	 * @return The table ExecBlock as a ExecBlockTable.
	 */
	ExecBlockTable & ASDM::getExecBlock () const {
		return *execBlock;
	}

	/**
	 * Get the table Feed.
	 * @return The table Feed as a FeedTable.
	 */
	FeedTable & ASDM::getFeed () const {
		return *feed;
	}

	/**
	 * Get the table Field.
	 * @return The table Field as a FieldTable.
	 */
	FieldTable & ASDM::getField () const {
		return *field;
	}

	/**
	 * Get the table Flag.
	 * @return The table Flag as a FlagTable.
	 */
	FlagTable & ASDM::getFlag () const {
		return *flag;
	}

	/**
	 * Get the table FlagCmd.
	 * @return The table FlagCmd as a FlagCmdTable.
	 */
	FlagCmdTable & ASDM::getFlagCmd () const {
		return *flagCmd;
	}

	/**
	 * Get the table Focus.
	 * @return The table Focus as a FocusTable.
	 */
	FocusTable & ASDM::getFocus () const {
		return *focus;
	}

	/**
	 * Get the table FocusModel.
	 * @return The table FocusModel as a FocusModelTable.
	 */
	FocusModelTable & ASDM::getFocusModel () const {
		return *focusModel;
	}

	/**
	 * Get the table FreqOffset.
	 * @return The table FreqOffset as a FreqOffsetTable.
	 */
	FreqOffsetTable & ASDM::getFreqOffset () const {
		return *freqOffset;
	}

	/**
	 * Get the table GainTracking.
	 * @return The table GainTracking as a GainTrackingTable.
	 */
	GainTrackingTable & ASDM::getGainTracking () const {
		return *gainTracking;
	}

	/**
	 * Get the table History.
	 * @return The table History as a HistoryTable.
	 */
	HistoryTable & ASDM::getHistory () const {
		return *history;
	}

	/**
	 * Get the table Holography.
	 * @return The table Holography as a HolographyTable.
	 */
	HolographyTable & ASDM::getHolography () const {
		return *holography;
	}

	/**
	 * Get the table Observation.
	 * @return The table Observation as a ObservationTable.
	 */
	ObservationTable & ASDM::getObservation () const {
		return *observation;
	}

	/**
	 * Get the table Pointing.
	 * @return The table Pointing as a PointingTable.
	 */
	PointingTable & ASDM::getPointing () const {
		return *pointing;
	}

	/**
	 * Get the table PointingModel.
	 * @return The table PointingModel as a PointingModelTable.
	 */
	PointingModelTable & ASDM::getPointingModel () const {
		return *pointingModel;
	}

	/**
	 * Get the table Polarization.
	 * @return The table Polarization as a PolarizationTable.
	 */
	PolarizationTable & ASDM::getPolarization () const {
		return *polarization;
	}

	/**
	 * Get the table Processor.
	 * @return The table Processor as a ProcessorTable.
	 */
	ProcessorTable & ASDM::getProcessor () const {
		return *processor;
	}

	/**
	 * Get the table Pulsar.
	 * @return The table Pulsar as a PulsarTable.
	 */
	PulsarTable & ASDM::getPulsar () const {
		return *pulsar;
	}

	/**
	 * Get the table Receiver.
	 * @return The table Receiver as a ReceiverTable.
	 */
	ReceiverTable & ASDM::getReceiver () const {
		return *receiver;
	}

	/**
	 * Get the table SBSummary.
	 * @return The table SBSummary as a SBSummaryTable.
	 */
	SBSummaryTable & ASDM::getSBSummary () const {
		return *sBSummary;
	}

	/**
	 * Get the table Scale.
	 * @return The table Scale as a ScaleTable.
	 */
	ScaleTable & ASDM::getScale () const {
		return *scale;
	}

	/**
	 * Get the table Scan.
	 * @return The table Scan as a ScanTable.
	 */
	ScanTable & ASDM::getScan () const {
		return *scan;
	}

	/**
	 * Get the table Seeing.
	 * @return The table Seeing as a SeeingTable.
	 */
	SeeingTable & ASDM::getSeeing () const {
		return *seeing;
	}

	/**
	 * Get the table Source.
	 * @return The table Source as a SourceTable.
	 */
	SourceTable & ASDM::getSource () const {
		return *source;
	}

	/**
	 * Get the table SpectralWindow.
	 * @return The table SpectralWindow as a SpectralWindowTable.
	 */
	SpectralWindowTable & ASDM::getSpectralWindow () const {
		return *spectralWindow;
	}

	/**
	 * Get the table SquareLawDetector.
	 * @return The table SquareLawDetector as a SquareLawDetectorTable.
	 */
	SquareLawDetectorTable & ASDM::getSquareLawDetector () const {
		return *squareLawDetector;
	}

	/**
	 * Get the table State.
	 * @return The table State as a StateTable.
	 */
	StateTable & ASDM::getState () const {
		return *state;
	}

	/**
	 * Get the table Station.
	 * @return The table Station as a StationTable.
	 */
	StationTable & ASDM::getStation () const {
		return *station;
	}

	/**
	 * Get the table Subscan.
	 * @return The table Subscan as a SubscanTable.
	 */
	SubscanTable & ASDM::getSubscan () const {
		return *subscan;
	}

	/**
	 * Get the table SwitchCycle.
	 * @return The table SwitchCycle as a SwitchCycleTable.
	 */
	SwitchCycleTable & ASDM::getSwitchCycle () const {
		return *switchCycle;
	}

	/**
	 * Get the table SysCal.
	 * @return The table SysCal as a SysCalTable.
	 */
	SysCalTable & ASDM::getSysCal () const {
		return *sysCal;
	}

	/**
	 * Get the table SysPower.
	 * @return The table SysPower as a SysPowerTable.
	 */
	SysPowerTable & ASDM::getSysPower () const {
		return *sysPower;
	}

	/**
	 * Get the table TotalPower.
	 * @return The table TotalPower as a TotalPowerTable.
	 */
	TotalPowerTable & ASDM::getTotalPower () const {
		return *totalPower;
	}

	/**
	 * Get the table VLAWVR.
	 * @return The table VLAWVR as a VLAWVRTable.
	 */
	VLAWVRTable & ASDM::getVLAWVR () const {
		return *vLAWVR;
	}

	/**
	 * Get the table WVMCal.
	 * @return The table WVMCal as a WVMCalTable.
	 */
	WVMCalTable & ASDM::getWVMCal () const {
		return *wVMCal;
	}

	/**
	 * Get the table Weather.
	 * @return The table Weather as a WeatherTable.
	 */
	WeatherTable & ASDM::getWeather () const {
		return *weather;
	}



	string ASDM::toXML() const {
		string out;
		out.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		out.append("<ASDM xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:cntnr=\"http://Alma/XASDM/ASDM\" xsi:schemaLocation=\"http://Alma/XASDM/ASDM http://almaobservatory.org/XML/XASDM/4/ASDM.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\"> ");

		if (entity.isNull())
			throw ConversionException("Container entity cannot be null.","Container");
		out.append(entity.toXML());
		out.append(" ");
		out.append("<TimeOfCreation> ");
		out.append(timeOfCreation.toFITS());
		out.append(" ");
		out.append("</TimeOfCreation>");
		for (unsigned int i = 0; i < table.size(); ++i) {
			out.append("<Table> ");
			out.append("<Name> ");
			out.append(table[i]->getName());
			out.append(" ");
			out.append("</Name> ");
			out.append("<NumberRows> ");
			out.append(Integer::toString(table[i]->size()));
			out.append(" ");
			out.append("</NumberRows> ");
			if (table[i]->size() > 0) {
				if (table[i]->getEntity().isNull())
					throw ConversionException("Table entity is null.",table[i]->getName());
				out.append(table[i]->getEntity().toXML());
			}
			out.append(" ");
			out.append("</Table> ");
		}
		out.append("</ASDM>");
		return out;
	}



	void ASDM::fromXML(string xmlDoc)  {
		Parser xml(xmlDoc);
		if (!xml.isStr("<ASDM")) 
			error();
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "ASDM")
			error();
		setEntity(e);

		s = xml.getElementContent("<TimeOfCreation>","</TimeOfCreation>");
		if (s.length() == 0) 
			error();
		ArrayTime t(s);
		//setTimeOfCreation(t);
		timeOfCreation = t;
		
		// Do we have an element startTimeDurationInXML
		s = xml.getElement("<startTimeDurationInXML","/>");
		if (s.length() != 0) 
			ArrayTimeInterval::readStartTimeDurationInXML(true);
		else 
			ArrayTimeInterval::readStartTimeDurationInXML(false);
			
		// Do we have an element startTimeDurationInBin
		s = xml.getElement("<startTimeDurationInBin","/>");
		if (s.length() != 0) 
			ArrayTimeInterval::readStartTimeDurationInBin(true);
		else 
			ArrayTimeInterval::readStartTimeDurationInBin(false);			 		 

		// Get each table in the dataset.
		s = xml.getElementContent("<Table>","</Table>");
		while (s.length() != 0) {
			Parser tab(s);
			s = tab.getElementContent("<Name>","</Name>");
			if (s.length() == 0) 
				error();
			string tableName = s;
			s = tab.getElementContent("<NumberRows>","</NumberRows>");
			if (s.length() == 0) 
				error();
			int numberRows = 0;
			try {
				numberRows = Integer::parseInt(s);
				getTable(tableName).declaredSize = numberRows;
			    if (numberRows > 0 ) {
				   s = tab.getElementContent("<Entity","/>");
				   if (s.length() == 0) 
					 error();
				   Entity tabE;
				   tabE.setFromXML(s);
				   if (tabE.getEntityTypeName() != (tableName + "Table"))
					 error();
				   tableEntity[tableName] = tabE;
				}
			} 
			catch (const NumberFormatException &err) {
				error(); // Expected to happen while parsing the number of rows.
			}
			catch (const InvalidArgumentException &err) {
		      ; // This happens when the name of a table which is not recognized by this
			    // version of the software. Then we simply ignore it !
			}
			s = xml.getElementContent("<Table>","</Table>");
		}
		if (!xml.isStr("</ASDM>")) 
			error();	
	}

	
#ifndef WITHOUT_ACS
	using namespace asdmIDL;
#endif

#ifndef WITHOUT_ACS	
	ASDMDataSetIDL* ASDM::toIDL() {
		ASDMDataSetIDL* result = new ASDMDataSetIDL();
		
		result->main = *(this->main->toIDL());
		
		result->almaRadiometer = *(this->almaRadiometer->toIDL());
		
		result->annotation = *(this->annotation->toIDL());
		
		result->antenna = *(this->antenna->toIDL());
		
		result->calAmpli = *(this->calAmpli->toIDL());
		
		result->calAntennaSolutions = *(this->calAntennaSolutions->toIDL());
		
		result->calAppPhase = *(this->calAppPhase->toIDL());
		
		result->calAtmosphere = *(this->calAtmosphere->toIDL());
		
		result->calBandpass = *(this->calBandpass->toIDL());
		
		result->calCurve = *(this->calCurve->toIDL());
		
		result->calData = *(this->calData->toIDL());
		
		result->calDelay = *(this->calDelay->toIDL());
		
		result->calDevice = *(this->calDevice->toIDL());
		
		result->calFlux = *(this->calFlux->toIDL());
		
		result->calFocus = *(this->calFocus->toIDL());
		
		result->calFocusModel = *(this->calFocusModel->toIDL());
		
		result->calGain = *(this->calGain->toIDL());
		
		result->calHolography = *(this->calHolography->toIDL());
		
		result->calPhase = *(this->calPhase->toIDL());
		
		result->calPointing = *(this->calPointing->toIDL());
		
		result->calPointingModel = *(this->calPointingModel->toIDL());
		
		result->calPosition = *(this->calPosition->toIDL());
		
		result->calPrimaryBeam = *(this->calPrimaryBeam->toIDL());
		
		result->calReduction = *(this->calReduction->toIDL());
		
		result->calSeeing = *(this->calSeeing->toIDL());
		
		result->calWVR = *(this->calWVR->toIDL());
		
		result->configDescription = *(this->configDescription->toIDL());
		
		result->correlatorMode = *(this->correlatorMode->toIDL());
		
		result->dataDescription = *(this->dataDescription->toIDL());
		
		result->delayModel = *(this->delayModel->toIDL());
		
		result->delayModelFixedParameters = *(this->delayModelFixedParameters->toIDL());
		
		result->delayModelVariableParameters = *(this->delayModelVariableParameters->toIDL());
		
		result->doppler = *(this->doppler->toIDL());
		
		result->ephemeris = *(this->ephemeris->toIDL());
		
		result->execBlock = *(this->execBlock->toIDL());
		
		result->feed = *(this->feed->toIDL());
		
		result->field = *(this->field->toIDL());
		
		result->flag = *(this->flag->toIDL());
		
		result->flagCmd = *(this->flagCmd->toIDL());
		
		result->focus = *(this->focus->toIDL());
		
		result->focusModel = *(this->focusModel->toIDL());
		
		result->freqOffset = *(this->freqOffset->toIDL());
		
		result->gainTracking = *(this->gainTracking->toIDL());
		
		result->history = *(this->history->toIDL());
		
		result->holography = *(this->holography->toIDL());
		
		result->observation = *(this->observation->toIDL());
		
		result->pointing = *(this->pointing->toIDL());
		
		result->pointingModel = *(this->pointingModel->toIDL());
		
		result->polarization = *(this->polarization->toIDL());
		
		result->processor = *(this->processor->toIDL());
		
		result->pulsar = *(this->pulsar->toIDL());
		
		result->receiver = *(this->receiver->toIDL());
		
		result->sBSummary = *(this->sBSummary->toIDL());
		
		result->scale = *(this->scale->toIDL());
		
		result->scan = *(this->scan->toIDL());
		
		result->seeing = *(this->seeing->toIDL());
		
		result->source = *(this->source->toIDL());
		
		result->spectralWindow = *(this->spectralWindow->toIDL());
		
		result->squareLawDetector = *(this->squareLawDetector->toIDL());
		
		result->state = *(this->state->toIDL());
		
		result->station = *(this->station->toIDL());
		
		result->subscan = *(this->subscan->toIDL());
		
		result->switchCycle = *(this->switchCycle->toIDL());
		
		result->sysCal = *(this->sysCal->toIDL());
		
		result->sysPower = *(this->sysPower->toIDL());
		
		result->totalPower = *(this->totalPower->toIDL());
		
		result->vLAWVR = *(this->vLAWVR->toIDL());
		
		result->wVMCal = *(this->wVMCal->toIDL());
		
		result->weather = *(this->weather->toIDL());
		
		return result;	
	}
	
	void ASDM::fromIDL(ASDMDataSetIDL* x) {
		
		this->main->fromIDL(x->main);
		
		this->almaRadiometer->fromIDL(x->almaRadiometer);
		
		this->annotation->fromIDL(x->annotation);
		
		this->antenna->fromIDL(x->antenna);
		
		this->calAmpli->fromIDL(x->calAmpli);
		
		this->calAntennaSolutions->fromIDL(x->calAntennaSolutions);
		
		this->calAppPhase->fromIDL(x->calAppPhase);
		
		this->calAtmosphere->fromIDL(x->calAtmosphere);
		
		this->calBandpass->fromIDL(x->calBandpass);
		
		this->calCurve->fromIDL(x->calCurve);
		
		this->calData->fromIDL(x->calData);
		
		this->calDelay->fromIDL(x->calDelay);
		
		this->calDevice->fromIDL(x->calDevice);
		
		this->calFlux->fromIDL(x->calFlux);
		
		this->calFocus->fromIDL(x->calFocus);
		
		this->calFocusModel->fromIDL(x->calFocusModel);
		
		this->calGain->fromIDL(x->calGain);
		
		this->calHolography->fromIDL(x->calHolography);
		
		this->calPhase->fromIDL(x->calPhase);
		
		this->calPointing->fromIDL(x->calPointing);
		
		this->calPointingModel->fromIDL(x->calPointingModel);
		
		this->calPosition->fromIDL(x->calPosition);
		
		this->calPrimaryBeam->fromIDL(x->calPrimaryBeam);
		
		this->calReduction->fromIDL(x->calReduction);
		
		this->calSeeing->fromIDL(x->calSeeing);
		
		this->calWVR->fromIDL(x->calWVR);
		
		this->configDescription->fromIDL(x->configDescription);
		
		this->correlatorMode->fromIDL(x->correlatorMode);
		
		this->dataDescription->fromIDL(x->dataDescription);
		
		this->delayModel->fromIDL(x->delayModel);
		
		this->delayModelFixedParameters->fromIDL(x->delayModelFixedParameters);
		
		this->delayModelVariableParameters->fromIDL(x->delayModelVariableParameters);
		
		this->doppler->fromIDL(x->doppler);
		
		this->ephemeris->fromIDL(x->ephemeris);
		
		this->execBlock->fromIDL(x->execBlock);
		
		this->feed->fromIDL(x->feed);
		
		this->field->fromIDL(x->field);
		
		this->flag->fromIDL(x->flag);
		
		this->flagCmd->fromIDL(x->flagCmd);
		
		this->focus->fromIDL(x->focus);
		
		this->focusModel->fromIDL(x->focusModel);
		
		this->freqOffset->fromIDL(x->freqOffset);
		
		this->gainTracking->fromIDL(x->gainTracking);
		
		this->history->fromIDL(x->history);
		
		this->holography->fromIDL(x->holography);
		
		this->observation->fromIDL(x->observation);
		
		this->pointing->fromIDL(x->pointing);
		
		this->pointingModel->fromIDL(x->pointingModel);
		
		this->polarization->fromIDL(x->polarization);
		
		this->processor->fromIDL(x->processor);
		
		this->pulsar->fromIDL(x->pulsar);
		
		this->receiver->fromIDL(x->receiver);
		
		this->sBSummary->fromIDL(x->sBSummary);
		
		this->scale->fromIDL(x->scale);
		
		this->scan->fromIDL(x->scan);
		
		this->seeing->fromIDL(x->seeing);
		
		this->source->fromIDL(x->source);
		
		this->spectralWindow->fromIDL(x->spectralWindow);
		
		this->squareLawDetector->fromIDL(x->squareLawDetector);
		
		this->state->fromIDL(x->state);
		
		this->station->fromIDL(x->station);
		
		this->subscan->fromIDL(x->subscan);
		
		this->switchCycle->fromIDL(x->switchCycle);
		
		this->sysCal->fromIDL(x->sysCal);
		
		this->sysPower->fromIDL(x->sysPower);
		
		this->totalPower->fromIDL(x->totalPower);
		
		this->vLAWVR->fromIDL(x->vLAWVR);
		
		this->wVMCal->fromIDL(x->wVMCal);
		
		this->weather->fromIDL(x->weather);
		
	}
#endif
		
	/**
	 * Write this ASDM dataset to the specified directory
	 * as a collection of XML documents. 
	 * @param directory The directory to which this dataset is written.
	 * @throws ConversionException If any error occurs in converting the
	 * table to XML and writing it to the directory.  This method will
	 * not overwrite any existing file; a ConversionException is also
	 * thrown in this case.
	 */
	void ASDM::toXML(string directory) const {
	
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}
		
		/*
		if (mkdir(directory.c_str(), S_IRWXU) == -1) 
			throw ConversionException("Could not create directory ", directory);
		*/
			
		string containername = directory + "/ASDM.xml";
		ofstream out(containername.c_str(),ios::out|ios::trunc);
		if (out.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + containername + " for writing","ASDM");
		out << toXML() << endl;
		out.close();
		if (out.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + containername,"ASDM");
		for (unsigned int i = 0; i < table.size(); ++i) {
			if (table[i]->size() == 0)
				continue; // Skip any empty table.
			string tablename = directory + "/" + table[i]->getName() + ".xml";
			ofstream tableout(tablename.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + tablename + " for writing",table[i]->getName());
			tableout << table[i]->toXML() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + tablename,table[i]->getName());
		}	
	}

	/**
	 * Get an ASDM dataset, given the full path name of the 
	 * directory containing the XML version of the dataset.
	 * @param xmlDirectory The full path name of the directory
	 * containing this dataset.
	 * @return The complete dataset that belongs to the container
	 * in this directory.
	 * @throws ConversionException If any error occurs reading the 
	 * files in the directory or in converting the tables from XML.
	 */
	ASDM *ASDM::getFromXML(string xmlDirectory)  {
		string containername = xmlDirectory + "/ASDM.xml";
		ifstream in(containername.c_str(),ios::in);
		if (in.rdstate() == istream::failbit)
			throw ConversionException("Could not open file " + containername + " for reading","ASDM");
		string xmlDoc;
		const int SIZE = 4096*1024;
		char line[SIZE];
		while (in.getline(line,SIZE)) {
			if (in.rdstate() == istream::failbit || in.rdstate() == istream::badbit) {
				throw ConversionException("Error reading file " + containername,"ASDM");
			};
			xmlDoc.append(line);
		}
		in.close();
		if (in.rdstate() == istream::failbit)
			throw ConversionException("Could not close file " + containername,"ASDM");
		ASDM *dataset = new ASDM ();
		dataset->fromXML(xmlDoc);
		// Now, do each table.
		int BLOCKSIZE = 1000000;
		char c[BLOCKSIZE];
		Entity entity;

		entity = dataset->tableEntity["Main"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Main.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Main");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getMain().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["AlmaRadiometer"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/AlmaRadiometer.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "AlmaRadiometer");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getAlmaRadiometer().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Annotation"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Annotation.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Annotation");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getAnnotation().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Antenna"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Antenna.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Antenna");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getAntenna().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalAmpli"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalAmpli.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalAmpli");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalAmpli().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalAntennaSolutions"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalAntennaSolutions.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalAntennaSolutions");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalAntennaSolutions().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalAppPhase"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalAppPhase.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalAppPhase");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalAppPhase().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalAtmosphere"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalAtmosphere.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalAtmosphere");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalAtmosphere().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalBandpass"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalBandpass.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalBandpass");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalBandpass().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalCurve"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalCurve.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalCurve");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalCurve().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalData"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalData.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalData");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalData().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalDelay"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalDelay.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalDelay");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalDelay().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalDevice"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalDevice.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalDevice");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalDevice().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalFlux"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalFlux.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalFlux");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalFlux().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalFocus"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalFocus.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalFocus");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalFocus().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalFocusModel"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalFocusModel.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalFocusModel");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalFocusModel().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalGain"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalGain.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalGain");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalGain().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalHolography"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalHolography.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalHolography");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalHolography().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalPhase"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalPhase.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalPhase");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalPhase().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalPointing"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalPointing.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalPointing");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalPointing().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalPointingModel"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalPointingModel.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalPointingModel");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalPointingModel().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalPosition"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalPosition.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalPosition");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalPosition().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalPrimaryBeam"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalPrimaryBeam.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalPrimaryBeam");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalPrimaryBeam().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalReduction"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalReduction.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalReduction");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalReduction().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalSeeing"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalSeeing.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalSeeing");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalSeeing().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CalWVR"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CalWVR.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CalWVR");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCalWVR().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["ConfigDescription"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/ConfigDescription.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "ConfigDescription");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getConfigDescription().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["CorrelatorMode"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/CorrelatorMode.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "CorrelatorMode");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getCorrelatorMode().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["DataDescription"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/DataDescription.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "DataDescription");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getDataDescription().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["DelayModel"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/DelayModel.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "DelayModel");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getDelayModel().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["DelayModelFixedParameters"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/DelayModelFixedParameters.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "DelayModelFixedParameters");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getDelayModelFixedParameters().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["DelayModelVariableParameters"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/DelayModelVariableParameters.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "DelayModelVariableParameters");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getDelayModelVariableParameters().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Doppler"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Doppler.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Doppler");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getDoppler().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Ephemeris"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Ephemeris.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Ephemeris");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getEphemeris().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["ExecBlock"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/ExecBlock.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "ExecBlock");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getExecBlock().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Feed"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Feed.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Feed");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getFeed().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Field"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Field.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Field");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getField().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Flag"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Flag.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Flag");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getFlag().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["FlagCmd"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/FlagCmd.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "FlagCmd");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getFlagCmd().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Focus"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Focus.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Focus");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getFocus().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["FocusModel"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/FocusModel.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "FocusModel");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getFocusModel().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["FreqOffset"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/FreqOffset.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "FreqOffset");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getFreqOffset().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["GainTracking"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/GainTracking.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "GainTracking");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getGainTracking().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["History"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/History.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "History");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getHistory().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Holography"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Holography.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Holography");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getHolography().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Observation"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Observation.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Observation");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getObservation().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Pointing"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Pointing.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Pointing");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getPointing().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["PointingModel"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/PointingModel.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "PointingModel");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getPointingModel().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Polarization"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Polarization.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Polarization");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getPolarization().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Processor"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Processor.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Processor");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getProcessor().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Pulsar"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Pulsar.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Pulsar");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getPulsar().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Receiver"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Receiver.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Receiver");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getReceiver().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["SBSummary"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/SBSummary.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "SBSummary");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSBSummary().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Scale"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Scale.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Scale");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getScale().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Scan"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Scan.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Scan");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getScan().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Seeing"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Seeing.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Seeing");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSeeing().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Source"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Source.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Source");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSource().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["SpectralWindow"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/SpectralWindow.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "SpectralWindow");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSpectralWindow().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["SquareLawDetector"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/SquareLawDetector.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "SquareLawDetector");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSquareLawDetector().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["State"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/State.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "State");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getState().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Station"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Station.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Station");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getStation().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Subscan"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Subscan.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Subscan");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSubscan().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["SwitchCycle"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/SwitchCycle.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "SwitchCycle");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSwitchCycle().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["SysCal"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/SysCal.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "SysCal");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSysCal().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["SysPower"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/SysPower.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "SysPower");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getSysPower().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["TotalPower"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/TotalPower.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "TotalPower");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getTotalPower().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["VLAWVR"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/VLAWVR.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "VLAWVR");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getVLAWVR().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["WVMCal"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/WVMCal.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "WVMCal");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getWVMCal().fromXML(tableDoc);						
		}

		entity = dataset->tableEntity["Weather"];
		if (entity.getEntityId().getId().length()  != 0) {
			// Which file must we read ?
			string tablename = xmlDirectory + "/Weather.xml";

			// Determine the file size
			ifstream::pos_type size;	
			ifstream tablein (tablename.c_str() , ios::in|ios::binary|ios::ate);
  			if (tablein.is_open()) { 
  				size = tablein.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + tablename, "Weather");
			}
			
			// Read the file in a string
			string tableDoc;

			tableDoc.reserve(size);
			tablein.seekg (0);	
			int nread = BLOCKSIZE;	
			while (nread == BLOCKSIZE) {
				tablein.read(c, BLOCKSIZE);
				if (tablein.rdstate() == istream::failbit || tablein.rdstate() == istream::badbit) {
					throw ConversionException("Error reading file " + tablename,"ASDM");
				}
				nread = tablein.gcount();
				tableDoc.append(c, nread);
			}
			tablein.close();
			if (tablein.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + tablename,"ASDM");
			
			// And finally parse the XML document to populate the table.	
			dataset->getWeather().fromXML(tableDoc);						
		}

		return dataset;
	}
	
	string ASDM::toMIME() {
		// To be implemented
		return "";
		;
	}
	
	void ASDM::setFromMIME(const string & /* mimeMsg */) {
		// To be implemented
		;
	}
	
	
	void ASDM::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		// Firstly send the container to its file.		
		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/ASDM.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName, "ASDM");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "ASDM");		
		}			
		else {
			// write the XML
			string fileName = directory + "/ASDM.xml";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName , "ASDM");
			tableout << toXML() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "ASDM");		
		}
		
		// Then send each of its table to its own file.
	
		if (getMain().size() > 0) {
			getMain().toFile(directory);
		}
	
		if (getAlmaRadiometer().size() > 0) {
			getAlmaRadiometer().toFile(directory);
		}
	
		if (getAnnotation().size() > 0) {
			getAnnotation().toFile(directory);
		}
	
		if (getAntenna().size() > 0) {
			getAntenna().toFile(directory);
		}
	
		if (getCalAmpli().size() > 0) {
			getCalAmpli().toFile(directory);
		}
	
		if (getCalAntennaSolutions().size() > 0) {
			getCalAntennaSolutions().toFile(directory);
		}
	
		if (getCalAppPhase().size() > 0) {
			getCalAppPhase().toFile(directory);
		}
	
		if (getCalAtmosphere().size() > 0) {
			getCalAtmosphere().toFile(directory);
		}
	
		if (getCalBandpass().size() > 0) {
			getCalBandpass().toFile(directory);
		}
	
		if (getCalCurve().size() > 0) {
			getCalCurve().toFile(directory);
		}
	
		if (getCalData().size() > 0) {
			getCalData().toFile(directory);
		}
	
		if (getCalDelay().size() > 0) {
			getCalDelay().toFile(directory);
		}
	
		if (getCalDevice().size() > 0) {
			getCalDevice().toFile(directory);
		}
	
		if (getCalFlux().size() > 0) {
			getCalFlux().toFile(directory);
		}
	
		if (getCalFocus().size() > 0) {
			getCalFocus().toFile(directory);
		}
	
		if (getCalFocusModel().size() > 0) {
			getCalFocusModel().toFile(directory);
		}
	
		if (getCalGain().size() > 0) {
			getCalGain().toFile(directory);
		}
	
		if (getCalHolography().size() > 0) {
			getCalHolography().toFile(directory);
		}
	
		if (getCalPhase().size() > 0) {
			getCalPhase().toFile(directory);
		}
	
		if (getCalPointing().size() > 0) {
			getCalPointing().toFile(directory);
		}
	
		if (getCalPointingModel().size() > 0) {
			getCalPointingModel().toFile(directory);
		}
	
		if (getCalPosition().size() > 0) {
			getCalPosition().toFile(directory);
		}
	
		if (getCalPrimaryBeam().size() > 0) {
			getCalPrimaryBeam().toFile(directory);
		}
	
		if (getCalReduction().size() > 0) {
			getCalReduction().toFile(directory);
		}
	
		if (getCalSeeing().size() > 0) {
			getCalSeeing().toFile(directory);
		}
	
		if (getCalWVR().size() > 0) {
			getCalWVR().toFile(directory);
		}
	
		if (getConfigDescription().size() > 0) {
			getConfigDescription().toFile(directory);
		}
	
		if (getCorrelatorMode().size() > 0) {
			getCorrelatorMode().toFile(directory);
		}
	
		if (getDataDescription().size() > 0) {
			getDataDescription().toFile(directory);
		}
	
		if (getDelayModel().size() > 0) {
			getDelayModel().toFile(directory);
		}
	
		if (getDelayModelFixedParameters().size() > 0) {
			getDelayModelFixedParameters().toFile(directory);
		}
	
		if (getDelayModelVariableParameters().size() > 0) {
			getDelayModelVariableParameters().toFile(directory);
		}
	
		if (getDoppler().size() > 0) {
			getDoppler().toFile(directory);
		}
	
		if (getEphemeris().size() > 0) {
			getEphemeris().toFile(directory);
		}
	
		if (getExecBlock().size() > 0) {
			getExecBlock().toFile(directory);
		}
	
		if (getFeed().size() > 0) {
			getFeed().toFile(directory);
		}
	
		if (getField().size() > 0) {
			getField().toFile(directory);
		}
	
		if (getFlag().size() > 0) {
			getFlag().toFile(directory);
		}
	
		if (getFlagCmd().size() > 0) {
			getFlagCmd().toFile(directory);
		}
	
		if (getFocus().size() > 0) {
			getFocus().toFile(directory);
		}
	
		if (getFocusModel().size() > 0) {
			getFocusModel().toFile(directory);
		}
	
		if (getFreqOffset().size() > 0) {
			getFreqOffset().toFile(directory);
		}
	
		if (getGainTracking().size() > 0) {
			getGainTracking().toFile(directory);
		}
	
		if (getHistory().size() > 0) {
			getHistory().toFile(directory);
		}
	
		if (getHolography().size() > 0) {
			getHolography().toFile(directory);
		}
	
		if (getObservation().size() > 0) {
			getObservation().toFile(directory);
		}
	
		if (getPointing().size() > 0) {
			getPointing().toFile(directory);
		}
	
		if (getPointingModel().size() > 0) {
			getPointingModel().toFile(directory);
		}
	
		if (getPolarization().size() > 0) {
			getPolarization().toFile(directory);
		}
	
		if (getProcessor().size() > 0) {
			getProcessor().toFile(directory);
		}
	
		if (getPulsar().size() > 0) {
			getPulsar().toFile(directory);
		}
	
		if (getReceiver().size() > 0) {
			getReceiver().toFile(directory);
		}
	
		if (getSBSummary().size() > 0) {
			getSBSummary().toFile(directory);
		}
	
		if (getScale().size() > 0) {
			getScale().toFile(directory);
		}
	
		if (getScan().size() > 0) {
			getScan().toFile(directory);
		}
	
		if (getSeeing().size() > 0) {
			getSeeing().toFile(directory);
		}
	
		if (getSource().size() > 0) {
			getSource().toFile(directory);
		}
	
		if (getSpectralWindow().size() > 0) {
			getSpectralWindow().toFile(directory);
		}
	
		if (getSquareLawDetector().size() > 0) {
			getSquareLawDetector().toFile(directory);
		}
	
		if (getState().size() > 0) {
			getState().toFile(directory);
		}
	
		if (getStation().size() > 0) {
			getStation().toFile(directory);
		}
	
		if (getSubscan().size() > 0) {
			getSubscan().toFile(directory);
		}
	
		if (getSwitchCycle().size() > 0) {
			getSwitchCycle().toFile(directory);
		}
	
		if (getSysCal().size() > 0) {
			getSysCal().toFile(directory);
		}
	
		if (getSysPower().size() > 0) {
			getSysPower().toFile(directory);
		}
	
		if (getTotalPower().size() > 0) {
			getTotalPower().toFile(directory);
		}
	
		if (getVLAWVR().size() > 0) {
			getVLAWVR().toFile(directory);
		}
	
		if (getWVMCal().size() > 0) {
			getWVMCal().toFile(directory);
		}
	
		if (getWeather().size() > 0) {
			getWeather().toFile(directory);
		}
			
	}

	
	

	void ASDM::setFromFile(string directory, bool loadTablesOnDemand) {
		this->setFromFile(directory, ASDMParseOptions().loadTablesOnDemand(loadTablesOnDemand));
	}
	
	void ASDM::setFromFile(string directory, const ASDMParseOptions& parse) {

		this->loadTablesOnDemand_ = parse.loadTablesOnDemand_;
		this->checkRowUniqueness_ = parse.checkRowUniqueness_;
		this->directory_ = directory;					
		string fileName;
		if (fileAsBin) {
			fileName = directory + "/ASDM.bin";
			// Determine the file size.
			ifstream::pos_type size;
			ifstream theFile(fileName.c_str(), ios::in|ios::binary|ios::ate);

 			if (theFile.is_open()) { 
  				size = theFile.tellg(); 
  			}
			else {
				throw ConversionException("Could not open file " + fileName, "ASDM");
			}
		
			// Re position to the beginning.
			theFile.seekg(0);
		
			// Read in a stringstream.
			stringstream ss;
			ss << theFile.rdbuf();

			if (theFile.rdstate() == istream::failbit || theFile.rdstate() == istream::badbit) {
				throw ConversionException("Error reading file " + fileName,"ASDM");
			}

			// And close
			theFile.close();
			if (theFile.rdstate() == istream::failbit)
				throw ConversionException("Could not close file " + fileName,"ASDM");
				
			setFromMIME(ss.str());
		}
		else {
			fileName = directory + "/ASDM.xml";
			string version ;
			ASDMUtils::Origin origin;
			
			try { 
				version = parse.detectVersion_ ? ASDMUtils::version(directory_) : parse.version_;
				origin = parse.detectOrigin_ ? ASDMUtils::origin(ASDMUtils::telescopeNames(directory_)) : parse.origin_;
			}
			catch (const ASDMUtilsException &e) {
				throw ConversionException ("Caught an exception whose message is '" + e.getMessage() + "'.", "ASDM");
			}
			
    		        if ((version == "UNKNOWN") && (origin == ASDMUtils::UNKNOWN))
    			        throw ConversionException("I cannot read this dataset with version='UNKNOWN' and origin='UNKNOWN'", "ASDM");
 		
                        // convert leading part of version to an integer (this will work for things like "4.2.1" if necessary)
                        std::locale loc;
                        // require that the first character is a digit
                        int iVersion = -1;

                        try {
                            iVersion = std::isdigit(version[0],loc) ? std::stoi(version) : -1;
                        } catch (const std::invalid_argument &ia) {
                            throw ConversionException("Invalid argument converting this version into an integer: '" + version + ". Can not continue.", "ASDM");
                        } catch (const std::out_of_range &oor) {
                            throw ConversionException("Integer in this version is out of range: '" + version + ". Can not continue.", "ASDM");
                        }

                        if (iVersion < 2) {
                                 throw ConversionException("I cannot read this dataset with version='" + version + "'.", "ASDM");
                        }
                        // this is the case that we know how to transform, ignore everything else and hope for the best
                        bool doTransform = (iVersion == 2) && (origin == ASDMUtils::EVLA || origin == ASDMUtils::ALMA);
    		        string xsltPath;
			string xmlDoc;
 			try {
 				if (doTransform) {
    				if (origin == ASDMUtils::EVLA)
						xsltPath = ASDMUtils::pathToV2V3EVLAxslTransform();
					else 
						xsltPath = ASDMUtils::pathToV2V3ALMAxslTransform();
				
					if (xsltPath.size() == 0)
						throw ConversionException("An XSL transformation must be done on this '" + ((origin==ASDMUtils::EVLA) ? string("EVLA") : string("ALMA")) + "' dataset, but its file , '" + ASDMUtils::nameOfV2V3xslTransform(origin) + "' cannot be found.", "ASDM");
					 
					xslTransformer.setTransformation(xsltPath);
					//cout << "An XSL transformation will be done on this dataset using '" << xsltPath << "'." << endl;
				}
				else {
					//cout << "No transformation will be applied on this dataset." << endl;
				}
				 
				xmlDoc = getXSLTransformer()(fileName);
			}
			catch (const XSLTransformerException &e) {
				throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "ASDM");
			}
			fromXML(xmlDoc);
		}
		
		if (!loadTablesOnDemand_) {
			// Now read and parse all files for the tables whose number of rows appear as
			// non null in the container just built.
			Entity entity;
	
			entity = tableEntity["Main"];
			if (entity.getEntityId().getId().length()  != 0) {
				getMain().setFromFile(directory_);
			}
	
			entity = tableEntity["AlmaRadiometer"];
			if (entity.getEntityId().getId().length()  != 0) {
				getAlmaRadiometer().setFromFile(directory_);
			}
	
			entity = tableEntity["Annotation"];
			if (entity.getEntityId().getId().length()  != 0) {
				getAnnotation().setFromFile(directory_);
			}
	
			entity = tableEntity["Antenna"];
			if (entity.getEntityId().getId().length()  != 0) {
				getAntenna().setFromFile(directory_);
			}
	
			entity = tableEntity["CalAmpli"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalAmpli().setFromFile(directory_);
			}
	
			entity = tableEntity["CalAntennaSolutions"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalAntennaSolutions().setFromFile(directory_);
			}
	
			entity = tableEntity["CalAppPhase"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalAppPhase().setFromFile(directory_);
			}
	
			entity = tableEntity["CalAtmosphere"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalAtmosphere().setFromFile(directory_);
			}
	
			entity = tableEntity["CalBandpass"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalBandpass().setFromFile(directory_);
			}
	
			entity = tableEntity["CalCurve"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalCurve().setFromFile(directory_);
			}
	
			entity = tableEntity["CalData"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalData().setFromFile(directory_);
			}
	
			entity = tableEntity["CalDelay"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalDelay().setFromFile(directory_);
			}
	
			entity = tableEntity["CalDevice"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalDevice().setFromFile(directory_);
			}
	
			entity = tableEntity["CalFlux"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalFlux().setFromFile(directory_);
			}
	
			entity = tableEntity["CalFocus"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalFocus().setFromFile(directory_);
			}
	
			entity = tableEntity["CalFocusModel"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalFocusModel().setFromFile(directory_);
			}
	
			entity = tableEntity["CalGain"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalGain().setFromFile(directory_);
			}
	
			entity = tableEntity["CalHolography"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalHolography().setFromFile(directory_);
			}
	
			entity = tableEntity["CalPhase"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalPhase().setFromFile(directory_);
			}
	
			entity = tableEntity["CalPointing"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalPointing().setFromFile(directory_);
			}
	
			entity = tableEntity["CalPointingModel"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalPointingModel().setFromFile(directory_);
			}
	
			entity = tableEntity["CalPosition"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalPosition().setFromFile(directory_);
			}
	
			entity = tableEntity["CalPrimaryBeam"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalPrimaryBeam().setFromFile(directory_);
			}
	
			entity = tableEntity["CalReduction"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalReduction().setFromFile(directory_);
			}
	
			entity = tableEntity["CalSeeing"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalSeeing().setFromFile(directory_);
			}
	
			entity = tableEntity["CalWVR"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCalWVR().setFromFile(directory_);
			}
	
			entity = tableEntity["ConfigDescription"];
			if (entity.getEntityId().getId().length()  != 0) {
				getConfigDescription().setFromFile(directory_);
			}
	
			entity = tableEntity["CorrelatorMode"];
			if (entity.getEntityId().getId().length()  != 0) {
				getCorrelatorMode().setFromFile(directory_);
			}
	
			entity = tableEntity["DataDescription"];
			if (entity.getEntityId().getId().length()  != 0) {
				getDataDescription().setFromFile(directory_);
			}
	
			entity = tableEntity["DelayModel"];
			if (entity.getEntityId().getId().length()  != 0) {
				getDelayModel().setFromFile(directory_);
			}
	
			entity = tableEntity["DelayModelFixedParameters"];
			if (entity.getEntityId().getId().length()  != 0) {
				getDelayModelFixedParameters().setFromFile(directory_);
			}
	
			entity = tableEntity["DelayModelVariableParameters"];
			if (entity.getEntityId().getId().length()  != 0) {
				getDelayModelVariableParameters().setFromFile(directory_);
			}
	
			entity = tableEntity["Doppler"];
			if (entity.getEntityId().getId().length()  != 0) {
				getDoppler().setFromFile(directory_);
			}
	
			entity = tableEntity["Ephemeris"];
			if (entity.getEntityId().getId().length()  != 0) {
				getEphemeris().setFromFile(directory_);
			}
	
			entity = tableEntity["ExecBlock"];
			if (entity.getEntityId().getId().length()  != 0) {
				getExecBlock().setFromFile(directory_);
			}
	
			entity = tableEntity["Feed"];
			if (entity.getEntityId().getId().length()  != 0) {
				getFeed().setFromFile(directory_);
			}
	
			entity = tableEntity["Field"];
			if (entity.getEntityId().getId().length()  != 0) {
				getField().setFromFile(directory_);
			}
	
			entity = tableEntity["Flag"];
			if (entity.getEntityId().getId().length()  != 0) {
				getFlag().setFromFile(directory_);
			}
	
			entity = tableEntity["FlagCmd"];
			if (entity.getEntityId().getId().length()  != 0) {
				getFlagCmd().setFromFile(directory_);
			}
	
			entity = tableEntity["Focus"];
			if (entity.getEntityId().getId().length()  != 0) {
				getFocus().setFromFile(directory_);
			}
	
			entity = tableEntity["FocusModel"];
			if (entity.getEntityId().getId().length()  != 0) {
				getFocusModel().setFromFile(directory_);
			}
	
			entity = tableEntity["FreqOffset"];
			if (entity.getEntityId().getId().length()  != 0) {
				getFreqOffset().setFromFile(directory_);
			}
	
			entity = tableEntity["GainTracking"];
			if (entity.getEntityId().getId().length()  != 0) {
				getGainTracking().setFromFile(directory_);
			}
	
			entity = tableEntity["History"];
			if (entity.getEntityId().getId().length()  != 0) {
				getHistory().setFromFile(directory_);
			}
	
			entity = tableEntity["Holography"];
			if (entity.getEntityId().getId().length()  != 0) {
				getHolography().setFromFile(directory_);
			}
	
			entity = tableEntity["Observation"];
			if (entity.getEntityId().getId().length()  != 0) {
				getObservation().setFromFile(directory_);
			}
	
			entity = tableEntity["Pointing"];
			if (entity.getEntityId().getId().length()  != 0) {
				getPointing().setFromFile(directory_);
			}
	
			entity = tableEntity["PointingModel"];
			if (entity.getEntityId().getId().length()  != 0) {
				getPointingModel().setFromFile(directory_);
			}
	
			entity = tableEntity["Polarization"];
			if (entity.getEntityId().getId().length()  != 0) {
				getPolarization().setFromFile(directory_);
			}
	
			entity = tableEntity["Processor"];
			if (entity.getEntityId().getId().length()  != 0) {
				getProcessor().setFromFile(directory_);
			}
	
			entity = tableEntity["Pulsar"];
			if (entity.getEntityId().getId().length()  != 0) {
				getPulsar().setFromFile(directory_);
			}
	
			entity = tableEntity["Receiver"];
			if (entity.getEntityId().getId().length()  != 0) {
				getReceiver().setFromFile(directory_);
			}
	
			entity = tableEntity["SBSummary"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSBSummary().setFromFile(directory_);
			}
	
			entity = tableEntity["Scale"];
			if (entity.getEntityId().getId().length()  != 0) {
				getScale().setFromFile(directory_);
			}
	
			entity = tableEntity["Scan"];
			if (entity.getEntityId().getId().length()  != 0) {
				getScan().setFromFile(directory_);
			}
	
			entity = tableEntity["Seeing"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSeeing().setFromFile(directory_);
			}
	
			entity = tableEntity["Source"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSource().setFromFile(directory_);
			}
	
			entity = tableEntity["SpectralWindow"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSpectralWindow().setFromFile(directory_);
			}
	
			entity = tableEntity["SquareLawDetector"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSquareLawDetector().setFromFile(directory_);
			}
	
			entity = tableEntity["State"];
			if (entity.getEntityId().getId().length()  != 0) {
				getState().setFromFile(directory_);
			}
	
			entity = tableEntity["Station"];
			if (entity.getEntityId().getId().length()  != 0) {
				getStation().setFromFile(directory_);
			}
	
			entity = tableEntity["Subscan"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSubscan().setFromFile(directory_);
			}
	
			entity = tableEntity["SwitchCycle"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSwitchCycle().setFromFile(directory_);
			}
	
			entity = tableEntity["SysCal"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSysCal().setFromFile(directory_);
			}
	
			entity = tableEntity["SysPower"];
			if (entity.getEntityId().getId().length()  != 0) {
				getSysPower().setFromFile(directory_);
			}
	
			entity = tableEntity["TotalPower"];
			if (entity.getEntityId().getId().length()  != 0) {
				getTotalPower().setFromFile(directory_);
			}
	
			entity = tableEntity["VLAWVR"];
			if (entity.getEntityId().getId().length()  != 0) {
				getVLAWVR().setFromFile(directory_);
			}
	
			entity = tableEntity["WVMCal"];
			if (entity.getEntityId().getId().length()  != 0) {
				getWVMCal().setFromFile(directory_);
			}
	
			entity = tableEntity["Weather"];
			if (entity.getEntityId().getId().length()  != 0) {
				getWeather().setFromFile(directory_);
			}
	
		}
		else {
	
			getMain().presentInMemory = tableEntity["Main"].getEntityId().getId().length() == 0;	
	
			getAlmaRadiometer().presentInMemory = tableEntity["AlmaRadiometer"].getEntityId().getId().length() == 0;	
	
			getAnnotation().presentInMemory = tableEntity["Annotation"].getEntityId().getId().length() == 0;	
	
			getAntenna().presentInMemory = tableEntity["Antenna"].getEntityId().getId().length() == 0;	
	
			getCalAmpli().presentInMemory = tableEntity["CalAmpli"].getEntityId().getId().length() == 0;	
	
			getCalAntennaSolutions().presentInMemory = tableEntity["CalAntennaSolutions"].getEntityId().getId().length() == 0;	
	
			getCalAppPhase().presentInMemory = tableEntity["CalAppPhase"].getEntityId().getId().length() == 0;	
	
			getCalAtmosphere().presentInMemory = tableEntity["CalAtmosphere"].getEntityId().getId().length() == 0;	
	
			getCalBandpass().presentInMemory = tableEntity["CalBandpass"].getEntityId().getId().length() == 0;	
	
			getCalCurve().presentInMemory = tableEntity["CalCurve"].getEntityId().getId().length() == 0;	
	
			getCalData().presentInMemory = tableEntity["CalData"].getEntityId().getId().length() == 0;	
	
			getCalDelay().presentInMemory = tableEntity["CalDelay"].getEntityId().getId().length() == 0;	
	
			getCalDevice().presentInMemory = tableEntity["CalDevice"].getEntityId().getId().length() == 0;	
	
			getCalFlux().presentInMemory = tableEntity["CalFlux"].getEntityId().getId().length() == 0;	
	
			getCalFocus().presentInMemory = tableEntity["CalFocus"].getEntityId().getId().length() == 0;	
	
			getCalFocusModel().presentInMemory = tableEntity["CalFocusModel"].getEntityId().getId().length() == 0;	
	
			getCalGain().presentInMemory = tableEntity["CalGain"].getEntityId().getId().length() == 0;	
	
			getCalHolography().presentInMemory = tableEntity["CalHolography"].getEntityId().getId().length() == 0;	
	
			getCalPhase().presentInMemory = tableEntity["CalPhase"].getEntityId().getId().length() == 0;	
	
			getCalPointing().presentInMemory = tableEntity["CalPointing"].getEntityId().getId().length() == 0;	
	
			getCalPointingModel().presentInMemory = tableEntity["CalPointingModel"].getEntityId().getId().length() == 0;	
	
			getCalPosition().presentInMemory = tableEntity["CalPosition"].getEntityId().getId().length() == 0;	
	
			getCalPrimaryBeam().presentInMemory = tableEntity["CalPrimaryBeam"].getEntityId().getId().length() == 0;	
	
			getCalReduction().presentInMemory = tableEntity["CalReduction"].getEntityId().getId().length() == 0;	
	
			getCalSeeing().presentInMemory = tableEntity["CalSeeing"].getEntityId().getId().length() == 0;	
	
			getCalWVR().presentInMemory = tableEntity["CalWVR"].getEntityId().getId().length() == 0;	
	
			getConfigDescription().presentInMemory = tableEntity["ConfigDescription"].getEntityId().getId().length() == 0;	
	
			getCorrelatorMode().presentInMemory = tableEntity["CorrelatorMode"].getEntityId().getId().length() == 0;	
	
			getDataDescription().presentInMemory = tableEntity["DataDescription"].getEntityId().getId().length() == 0;	
	
			getDelayModel().presentInMemory = tableEntity["DelayModel"].getEntityId().getId().length() == 0;	
	
			getDelayModelFixedParameters().presentInMemory = tableEntity["DelayModelFixedParameters"].getEntityId().getId().length() == 0;	
	
			getDelayModelVariableParameters().presentInMemory = tableEntity["DelayModelVariableParameters"].getEntityId().getId().length() == 0;	
	
			getDoppler().presentInMemory = tableEntity["Doppler"].getEntityId().getId().length() == 0;	
	
			getEphemeris().presentInMemory = tableEntity["Ephemeris"].getEntityId().getId().length() == 0;	
	
			getExecBlock().presentInMemory = tableEntity["ExecBlock"].getEntityId().getId().length() == 0;	
	
			getFeed().presentInMemory = tableEntity["Feed"].getEntityId().getId().length() == 0;	
	
			getField().presentInMemory = tableEntity["Field"].getEntityId().getId().length() == 0;	
	
			getFlag().presentInMemory = tableEntity["Flag"].getEntityId().getId().length() == 0;	
	
			getFlagCmd().presentInMemory = tableEntity["FlagCmd"].getEntityId().getId().length() == 0;	
	
			getFocus().presentInMemory = tableEntity["Focus"].getEntityId().getId().length() == 0;	
	
			getFocusModel().presentInMemory = tableEntity["FocusModel"].getEntityId().getId().length() == 0;	
	
			getFreqOffset().presentInMemory = tableEntity["FreqOffset"].getEntityId().getId().length() == 0;	
	
			getGainTracking().presentInMemory = tableEntity["GainTracking"].getEntityId().getId().length() == 0;	
	
			getHistory().presentInMemory = tableEntity["History"].getEntityId().getId().length() == 0;	
	
			getHolography().presentInMemory = tableEntity["Holography"].getEntityId().getId().length() == 0;	
	
			getObservation().presentInMemory = tableEntity["Observation"].getEntityId().getId().length() == 0;	
	
			getPointing().presentInMemory = tableEntity["Pointing"].getEntityId().getId().length() == 0;	
	
			getPointingModel().presentInMemory = tableEntity["PointingModel"].getEntityId().getId().length() == 0;	
	
			getPolarization().presentInMemory = tableEntity["Polarization"].getEntityId().getId().length() == 0;	
	
			getProcessor().presentInMemory = tableEntity["Processor"].getEntityId().getId().length() == 0;	
	
			getPulsar().presentInMemory = tableEntity["Pulsar"].getEntityId().getId().length() == 0;	
	
			getReceiver().presentInMemory = tableEntity["Receiver"].getEntityId().getId().length() == 0;	
	
			getSBSummary().presentInMemory = tableEntity["SBSummary"].getEntityId().getId().length() == 0;	
	
			getScale().presentInMemory = tableEntity["Scale"].getEntityId().getId().length() == 0;	
	
			getScan().presentInMemory = tableEntity["Scan"].getEntityId().getId().length() == 0;	
	
			getSeeing().presentInMemory = tableEntity["Seeing"].getEntityId().getId().length() == 0;	
	
			getSource().presentInMemory = tableEntity["Source"].getEntityId().getId().length() == 0;	
	
			getSpectralWindow().presentInMemory = tableEntity["SpectralWindow"].getEntityId().getId().length() == 0;	
	
			getSquareLawDetector().presentInMemory = tableEntity["SquareLawDetector"].getEntityId().getId().length() == 0;	
	
			getState().presentInMemory = tableEntity["State"].getEntityId().getId().length() == 0;	
	
			getStation().presentInMemory = tableEntity["Station"].getEntityId().getId().length() == 0;	
	
			getSubscan().presentInMemory = tableEntity["Subscan"].getEntityId().getId().length() == 0;	
	
			getSwitchCycle().presentInMemory = tableEntity["SwitchCycle"].getEntityId().getId().length() == 0;	
	
			getSysCal().presentInMemory = tableEntity["SysCal"].getEntityId().getId().length() == 0;	
	
			getSysPower().presentInMemory = tableEntity["SysPower"].getEntityId().getId().length() == 0;	
	
			getTotalPower().presentInMemory = tableEntity["TotalPower"].getEntityId().getId().length() == 0;	
	
			getVLAWVR().presentInMemory = tableEntity["VLAWVR"].getEntityId().getId().length() == 0;	
	
			getWVMCal().presentInMemory = tableEntity["WVMCal"].getEntityId().getId().length() == 0;	
	
			getWeather().presentInMemory = tableEntity["Weather"].getEntityId().getId().length() == 0;	
				
		}
			
		origin = FILE;
	}
	
	bool ASDM::checkRowUniqueness() const { return checkRowUniqueness_; } 
	

	


	Entity ASDM::getEntity() const {
		return entity;
	}

	void ASDM::setEntity(Entity e) {
		this->entity = e; 
	}
	
	/**
	 * Meaningless, but required for the Representable interface.
	 */
	string ASDM::getName() const {
		return "";
	}
	
	/**
	 * Meaningless, but required for the Representable interface.
	 */
	unsigned int ASDM::size()  {
		return 0;
	}
	
#ifndef WITHOUT_ACS
	/**
	 * Create an ASDM dataset from the ALMA archive, given the
	 * entityId of its container.
	 * @param datasetId The entityId of the container of the dataset.
	 */
	ASDM *ASDM::fromArchive(EntityId datasetId) {
		// Get the xml representation of the container.
		string xml = getXMLEntity(datasetId);
		// Create the container ...
		ASDM *container = new ASDM ();
		// ... and populate it with data.
		container->fromXML(xml);
		
		// Now build the tables.
		Entity entity;
			
		entity = container->tableEntity["Main"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getMain().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getMain().fromXML(xml);
		}
			
		entity = container->tableEntity["AlmaRadiometer"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getAlmaRadiometer().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getAlmaRadiometer().fromXML(xml);
		}
			
		entity = container->tableEntity["Annotation"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getAnnotation().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getAnnotation().fromXML(xml);
		}
			
		entity = container->tableEntity["Antenna"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getAntenna().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getAntenna().fromXML(xml);
		}
			
		entity = container->tableEntity["CalAmpli"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalAmpli().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalAmpli().fromXML(xml);
		}
			
		entity = container->tableEntity["CalAntennaSolutions"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalAntennaSolutions().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalAntennaSolutions().fromXML(xml);
		}
			
		entity = container->tableEntity["CalAppPhase"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalAppPhase().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalAppPhase().fromXML(xml);
		}
			
		entity = container->tableEntity["CalAtmosphere"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalAtmosphere().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalAtmosphere().fromXML(xml);
		}
			
		entity = container->tableEntity["CalBandpass"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalBandpass().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalBandpass().fromXML(xml);
		}
			
		entity = container->tableEntity["CalCurve"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalCurve().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalCurve().fromXML(xml);
		}
			
		entity = container->tableEntity["CalData"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalData().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalData().fromXML(xml);
		}
			
		entity = container->tableEntity["CalDelay"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalDelay().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalDelay().fromXML(xml);
		}
			
		entity = container->tableEntity["CalDevice"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalDevice().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalDevice().fromXML(xml);
		}
			
		entity = container->tableEntity["CalFlux"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalFlux().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalFlux().fromXML(xml);
		}
			
		entity = container->tableEntity["CalFocus"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalFocus().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalFocus().fromXML(xml);
		}
			
		entity = container->tableEntity["CalFocusModel"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalFocusModel().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalFocusModel().fromXML(xml);
		}
			
		entity = container->tableEntity["CalGain"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalGain().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalGain().fromXML(xml);
		}
			
		entity = container->tableEntity["CalHolography"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalHolography().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalHolography().fromXML(xml);
		}
			
		entity = container->tableEntity["CalPhase"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalPhase().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalPhase().fromXML(xml);
		}
			
		entity = container->tableEntity["CalPointing"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalPointing().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalPointing().fromXML(xml);
		}
			
		entity = container->tableEntity["CalPointingModel"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalPointingModel().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalPointingModel().fromXML(xml);
		}
			
		entity = container->tableEntity["CalPosition"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalPosition().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalPosition().fromXML(xml);
		}
			
		entity = container->tableEntity["CalPrimaryBeam"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalPrimaryBeam().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalPrimaryBeam().fromXML(xml);
		}
			
		entity = container->tableEntity["CalReduction"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalReduction().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalReduction().fromXML(xml);
		}
			
		entity = container->tableEntity["CalSeeing"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalSeeing().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalSeeing().fromXML(xml);
		}
			
		entity = container->tableEntity["CalWVR"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCalWVR().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCalWVR().fromXML(xml);
		}
			
		entity = container->tableEntity["ConfigDescription"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getConfigDescription().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getConfigDescription().fromXML(xml);
		}
			
		entity = container->tableEntity["CorrelatorMode"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getCorrelatorMode().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getCorrelatorMode().fromXML(xml);
		}
			
		entity = container->tableEntity["DataDescription"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getDataDescription().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getDataDescription().fromXML(xml);
		}
			
		entity = container->tableEntity["DelayModel"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getDelayModel().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getDelayModel().fromXML(xml);
		}
			
		entity = container->tableEntity["DelayModelFixedParameters"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getDelayModelFixedParameters().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getDelayModelFixedParameters().fromXML(xml);
		}
			
		entity = container->tableEntity["DelayModelVariableParameters"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getDelayModelVariableParameters().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getDelayModelVariableParameters().fromXML(xml);
		}
			
		entity = container->tableEntity["Doppler"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getDoppler().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getDoppler().fromXML(xml);
		}
			
		entity = container->tableEntity["Ephemeris"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getEphemeris().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getEphemeris().fromXML(xml);
		}
			
		entity = container->tableEntity["ExecBlock"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getExecBlock().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getExecBlock().fromXML(xml);
		}
			
		entity = container->tableEntity["Feed"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getFeed().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getFeed().fromXML(xml);
		}
			
		entity = container->tableEntity["Field"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getField().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getField().fromXML(xml);
		}
			
		entity = container->tableEntity["Flag"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getFlag().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getFlag().fromXML(xml);
		}
			
		entity = container->tableEntity["FlagCmd"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getFlagCmd().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getFlagCmd().fromXML(xml);
		}
			
		entity = container->tableEntity["Focus"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getFocus().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getFocus().fromXML(xml);
		}
			
		entity = container->tableEntity["FocusModel"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getFocusModel().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getFocusModel().fromXML(xml);
		}
			
		entity = container->tableEntity["FreqOffset"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getFreqOffset().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getFreqOffset().fromXML(xml);
		}
			
		entity = container->tableEntity["GainTracking"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getGainTracking().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getGainTracking().fromXML(xml);
		}
			
		entity = container->tableEntity["History"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getHistory().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getHistory().fromXML(xml);
		}
			
		entity = container->tableEntity["Holography"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getHolography().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getHolography().fromXML(xml);
		}
			
		entity = container->tableEntity["Observation"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getObservation().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getObservation().fromXML(xml);
		}
			
		entity = container->tableEntity["Pointing"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getPointing().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getPointing().fromXML(xml);
		}
			
		entity = container->tableEntity["PointingModel"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getPointingModel().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getPointingModel().fromXML(xml);
		}
			
		entity = container->tableEntity["Polarization"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getPolarization().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getPolarization().fromXML(xml);
		}
			
		entity = container->tableEntity["Processor"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getProcessor().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getProcessor().fromXML(xml);
		}
			
		entity = container->tableEntity["Pulsar"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getPulsar().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getPulsar().fromXML(xml);
		}
			
		entity = container->tableEntity["Receiver"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getReceiver().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getReceiver().fromXML(xml);
		}
			
		entity = container->tableEntity["SBSummary"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSBSummary().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSBSummary().fromXML(xml);
		}
			
		entity = container->tableEntity["Scale"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getScale().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getScale().fromXML(xml);
		}
			
		entity = container->tableEntity["Scan"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getScan().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getScan().fromXML(xml);
		}
			
		entity = container->tableEntity["Seeing"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSeeing().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSeeing().fromXML(xml);
		}
			
		entity = container->tableEntity["Source"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSource().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSource().fromXML(xml);
		}
			
		entity = container->tableEntity["SpectralWindow"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSpectralWindow().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSpectralWindow().fromXML(xml);
		}
			
		entity = container->tableEntity["SquareLawDetector"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSquareLawDetector().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSquareLawDetector().fromXML(xml);
		}
			
		entity = container->tableEntity["State"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getState().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getState().fromXML(xml);
		}
			
		entity = container->tableEntity["Station"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getStation().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getStation().fromXML(xml);
		}
			
		entity = container->tableEntity["Subscan"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSubscan().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSubscan().fromXML(xml);
		}
			
		entity = container->tableEntity["SwitchCycle"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSwitchCycle().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSwitchCycle().fromXML(xml);
		}
			
		entity = container->tableEntity["SysCal"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSysCal().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSysCal().fromXML(xml);
		}
			
		entity = container->tableEntity["SysPower"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getSysPower().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getSysPower().fromXML(xml);
		}
			
		entity = container->tableEntity["TotalPower"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getTotalPower().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getTotalPower().fromXML(xml);
		}
			
		entity = container->tableEntity["VLAWVR"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getVLAWVR().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getVLAWVR().fromXML(xml);
		}
			
		entity = container->tableEntity["WVMCal"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getWVMCal().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getWVMCal().fromXML(xml);
		}
			
		entity = container->tableEntity["Weather"];
		if (entity.getEntityId().getId().size() != 0) {
			container->getWeather().setEntity(entity);
			xml = getXMLEntity(entity.getEntityId());
			container->getWeather().fromXML(xml);
		}
		
		return container;
	}
#endif

#ifndef WITHOUT_ACS
	/**
	 * Update an ASDM dataset that already exists in the ALMA archive.
	 */
	void ASDM::updateArchive() const{
		// Assumption: Entity objects have already been assigned.
		// Convert each table to an XML document and write it to the archive.
		for (unsigned int i = 0; i < table.size(); ++i) {
			string xml = table[i]->toXML();
			putXMLEntity(xml);
		}
		// Now, convert the container and write it to the archive.
		string xml = toXML();
		putXMLEntity(xml);
	}
#endif
	
	/**
	 * Return the table, as a Representable object, with the
	 * specified name.
	 */
	Representable &ASDM::getTable(string tableName)  {
		for (unsigned int i = 0; i < table.size(); ++i)
			if (table[i]->getName() == tableName)
				return *table[i];
		throw InvalidArgumentException("No such table as " + tableName);
	}

	
	

	
 	/**
 	 * Get timeOfCreation.
 	 * @return timeOfCreation as ArrayTime
 	 */
 	ArrayTime ASDM::getTimeOfCreation() const {
	
  		return timeOfCreation;
 	}

 	/**
 	 * Set timeOfCreation with the specified ArrayTime.
 	 * @param timeOfCreation The ArrayTime value to which timeOfCreation is to be set.
 	 
 	
 		
 	 */
 	void ASDM::setTimeOfCreation (ArrayTime timeOfCreation)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->timeOfCreation = timeOfCreation;
	
 	}
	
	

	

	
 	/**
 	 * Get version.
 	 * @return version as int
 	 */
 	int ASDM::getVersion() const {
	
  		return version;
 	}

 	/**
 	 * Set version with the specified int.
 	 * @param version The int value to which version is to be set.
 	 
 	
 		
 	 */
 	void ASDM::setVersion (int version)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->version = version;
	
 	}
	
	

	

	
 	/**
 	 * Get xmlnsPrefix.
 	 * @return xmlnsPrefix as std::string
 	 */
 	std::string ASDM::getXmlnsPrefix() const {
	
  		return xmlnsPrefix;
 	}

 	/**
 	 * Set xmlnsPrefix with the specified std::string.
 	 * @param xmlnsPrefix The std::string value to which xmlnsPrefix is to be set.
 	 
 	
 		
 	 */
 	void ASDM::setXmlnsPrefix (std::string xmlnsPrefix)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->xmlnsPrefix = xmlnsPrefix;
	
 	}
	
	


	void ASDM::error() {
		throw ConversionException("Invalid xml document","ASDM");
	}
	
	string ASDM::getXMLEntity(EntityId /* id */) {
		throw ConversionException("Not implemented","ASDM");
	}
	
	void ASDM::putXMLEntity(string /* xml */) {
		throw ConversionException("Not implemented","ASDM");
	}
	
} // End namespace asdm
 
