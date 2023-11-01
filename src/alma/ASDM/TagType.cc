
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
 * File TagType.cpp
 */
 #include <alma/ASDM/TagType.h>

using namespace std;

namespace asdm {

const TagType* TagType::NoType = new TagType("NoType") ;

	

	
const TagType* TagType::AlmaRadiometer = new TagType("AlmaRadiometer") ;
	

	
const TagType* TagType::Annotation = new TagType("Annotation") ;
	

	
const TagType* TagType::Antenna = new TagType("Antenna") ;
	

	

	

	

	

	

	

	
const TagType* TagType::CalData = new TagType("CalData") ;
	

	

	

	

	

	

	

	

	

	

	

	

	

	
const TagType* TagType::CalReduction = new TagType("CalReduction") ;
	

	

	

	
const TagType* TagType::ConfigDescription = new TagType("ConfigDescription") ;
	

	
const TagType* TagType::CorrelatorMode = new TagType("CorrelatorMode") ;
	

	
const TagType* TagType::DataDescription = new TagType("DataDescription") ;
	

	

	
const TagType* TagType::DelayModelFixedParameters = new TagType("DelayModelFixedParameters") ;
	

	
const TagType* TagType::DelayModelVariableParameters = new TagType("DelayModelVariableParameters") ;
	

	
const TagType* TagType::Doppler = new TagType("Doppler") ;
	

	

	
const TagType* TagType::ExecBlock = new TagType("ExecBlock") ;
	

	
const TagType* TagType::Feed = new TagType("Feed") ;
	

	
const TagType* TagType::Field = new TagType("Field") ;
	

	
const TagType* TagType::Flag = new TagType("Flag") ;
	

	

	

	
const TagType* TagType::FocusModel = new TagType("FocusModel") ;
	

	

	

	

	
const TagType* TagType::Holography = new TagType("Holography") ;
	

	
const TagType* TagType::Observation = new TagType("Observation") ;
	

	

	
const TagType* TagType::PointingModel = new TagType("PointingModel") ;
	

	
const TagType* TagType::Polarization = new TagType("Polarization") ;
	

	
const TagType* TagType::Processor = new TagType("Processor") ;
	

	
const TagType* TagType::Pulsar = new TagType("Pulsar") ;
	

	
const TagType* TagType::Receiver = new TagType("Receiver") ;
	

	
const TagType* TagType::SBSummary = new TagType("SBSummary") ;
	

	
const TagType* TagType::Scale = new TagType("Scale") ;
	

	

	

	
const TagType* TagType::Source = new TagType("Source") ;
	

	
const TagType* TagType::SpectralWindow = new TagType("SpectralWindow") ;
	

	
const TagType* TagType::SquareLawDetector = new TagType("SquareLawDetector") ;
	

	
const TagType* TagType::State = new TagType("State") ;
	

	
const TagType* TagType::Station = new TagType("Station") ;
	

	

	
const TagType* TagType::SwitchCycle = new TagType("SwitchCycle") ;
	

	

	

	

	

	

	


map<string, const TagType*> fillName2TagType_m() {
	map<string, const TagType*> result;
	result["NoType"] = TagType::NoType;

	

	
	result["AlmaRadiometer"] = TagType::AlmaRadiometer ;
	

	
	result["Annotation"] = TagType::Annotation ;
	

	
	result["Antenna"] = TagType::Antenna ;
	

	

	

	

	

	

	

	
	result["CalData"] = TagType::CalData ;
	

	

	

	

	

	

	

	

	

	

	

	

	

	
	result["CalReduction"] = TagType::CalReduction ;
	

	

	

	
	result["ConfigDescription"] = TagType::ConfigDescription ;
	

	
	result["CorrelatorMode"] = TagType::CorrelatorMode ;
	

	
	result["DataDescription"] = TagType::DataDescription ;
	

	

	
	result["DelayModelFixedParameters"] = TagType::DelayModelFixedParameters ;
	

	
	result["DelayModelVariableParameters"] = TagType::DelayModelVariableParameters ;
	

	
	result["Doppler"] = TagType::Doppler ;
	

	

	
	result["ExecBlock"] = TagType::ExecBlock ;
	

	
	result["Feed"] = TagType::Feed ;
	

	
	result["Field"] = TagType::Field ;
	

	
	result["Flag"] = TagType::Flag ;
	

	

	

	
	result["FocusModel"] = TagType::FocusModel ;
	

	

	

	

	
	result["Holography"] = TagType::Holography ;
	

	
	result["Observation"] = TagType::Observation ;
	

	

	
	result["PointingModel"] = TagType::PointingModel ;
	

	
	result["Polarization"] = TagType::Polarization ;
	

	
	result["Processor"] = TagType::Processor ;
	

	
	result["Pulsar"] = TagType::Pulsar ;
	

	
	result["Receiver"] = TagType::Receiver ;
	

	
	result["SBSummary"] = TagType::SBSummary ;
	

	
	result["Scale"] = TagType::Scale ;
	

	

	

	
	result["Source"] = TagType::Source ;
	

	
	result["SpectralWindow"] = TagType::SpectralWindow ;
	

	
	result["SquareLawDetector"] = TagType::SquareLawDetector ;
	

	
	result["State"] = TagType::State ;
	

	
	result["Station"] = TagType::Station ;
	

	

	
	result["SwitchCycle"] = TagType::SwitchCycle ;
	

	

	

	

	

	

	

	return result;	
}

static map<string, const TagType*> name2TagType_m = fillName2TagType_m();

TagType::TagTypeMgr TagType::tagTypeMgr;

TagType::TagType(string name) {
  this->name = name;
}

string TagType::toString() const {
  return this->name;
}

const TagType* TagType::getTagType(string name) {
  return name2TagType_m[name];
} 
} // end namespace asdm
