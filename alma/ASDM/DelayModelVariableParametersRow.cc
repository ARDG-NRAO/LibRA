
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
 * File DelayModelVariableParametersRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/DelayModelVariableParametersRow.h>
#include <alma/ASDM/DelayModelVariableParametersTable.h>

#include <alma/ASDM/DelayModelFixedParametersTable.h>
#include <alma/ASDM/DelayModelFixedParametersRow.h>
	

using asdm::ASDM;
using asdm::DelayModelVariableParametersRow;
using asdm::DelayModelVariableParametersTable;

using asdm::DelayModelFixedParametersTable;
using asdm::DelayModelFixedParametersRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	DelayModelVariableParametersRow::~DelayModelVariableParametersRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	DelayModelVariableParametersTable &DelayModelVariableParametersRow::getTable() const {
		return table;
	}

	bool DelayModelVariableParametersRow::isAdded() const {
		return hasBeenAdded;
	}	

	void DelayModelVariableParametersRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::DelayModelVariableParametersRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a DelayModelVariableParametersRowIDL struct.
	 */
	DelayModelVariableParametersRowIDL *DelayModelVariableParametersRow::toIDL() const {
		DelayModelVariableParametersRowIDL *x = new DelayModelVariableParametersRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->delayModelVariableParametersId = delayModelVariableParametersId.toIDLTag();
			
		
	

	
  		
		
		
			
		x->time = time.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->ut1_utc = ut1_utc;
 				
 			
		
	

	
  		
		
		
			
				
		x->iat_utc = iat_utc;
 				
 			
		
	

	
  		
		
		
			
				
		x->timeType = timeType;
 				
 			
		
	

	
  		
		
		
			
		x->gstAtUt0 = gstAtUt0.toIDLAngle();
			
		
	

	
  		
		
		
			
		x->earthRotationRate = earthRotationRate.toIDLAngularRate();
			
		
	

	
  		
		
		
			
		x->polarOffsets.length(polarOffsets.size());
		for (unsigned int i = 0; i < polarOffsets.size(); ++i) {
			
				
			x->polarOffsets[i] = polarOffsets.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->polarOffsetsType = polarOffsetsType;
 				
 			
		
	

	
  		
		
		x->nutationInLongitudeExists = nutationInLongitudeExists;
		
		
			
		x->nutationInLongitude = nutationInLongitude.toIDLAngle();
			
		
	

	
  		
		
		x->nutationInLongitudeRateExists = nutationInLongitudeRateExists;
		
		
			
		x->nutationInLongitudeRate = nutationInLongitudeRate.toIDLAngularRate();
			
		
	

	
  		
		
		x->nutationInObliquityExists = nutationInObliquityExists;
		
		
			
		x->nutationInObliquity = nutationInObliquity.toIDLAngle();
			
		
	

	
  		
		
		x->nutationInObliquityRateExists = nutationInObliquityRateExists;
		
		
			
		x->nutationInObliquityRate = nutationInObliquityRate.toIDLAngularRate();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->delayModelFixedParametersId = delayModelFixedParametersId.toIDLTag();
			
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void DelayModelVariableParametersRow::toIDL(asdmIDL::DelayModelVariableParametersRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.delayModelVariableParametersId = delayModelVariableParametersId.toIDLTag();
			
		
	

	
  		
		
		
			
		x.time = time.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.ut1_utc = ut1_utc;
 				
 			
		
	

	
  		
		
		
			
				
		x.iat_utc = iat_utc;
 				
 			
		
	

	
  		
		
		
			
				
		x.timeType = timeType;
 				
 			
		
	

	
  		
		
		
			
		x.gstAtUt0 = gstAtUt0.toIDLAngle();
			
		
	

	
  		
		
		
			
		x.earthRotationRate = earthRotationRate.toIDLAngularRate();
			
		
	

	
  		
		
		
			
		x.polarOffsets.length(polarOffsets.size());
		for (unsigned int i = 0; i < polarOffsets.size(); ++i) {
			
				
			x.polarOffsets[i] = polarOffsets.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.polarOffsetsType = polarOffsetsType;
 				
 			
		
	

	
  		
		
		x.nutationInLongitudeExists = nutationInLongitudeExists;
		
		
			
		x.nutationInLongitude = nutationInLongitude.toIDLAngle();
			
		
	

	
  		
		
		x.nutationInLongitudeRateExists = nutationInLongitudeRateExists;
		
		
			
		x.nutationInLongitudeRate = nutationInLongitudeRate.toIDLAngularRate();
			
		
	

	
  		
		
		x.nutationInObliquityExists = nutationInObliquityExists;
		
		
			
		x.nutationInObliquity = nutationInObliquity.toIDLAngle();
			
		
	

	
  		
		
		x.nutationInObliquityRateExists = nutationInObliquityRateExists;
		
		
			
		x.nutationInObliquityRate = nutationInObliquityRate.toIDLAngularRate();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.delayModelFixedParametersId = delayModelFixedParametersId.toIDLTag();
			
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct DelayModelVariableParametersRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void DelayModelVariableParametersRow::setFromIDL (DelayModelVariableParametersRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setDelayModelVariableParametersId(Tag (x.delayModelVariableParametersId));
			
 		
		
	

	
		
		
			
		setTime(ArrayTime (x.time));
			
 		
		
	

	
		
		
			
		setUt1_utc(x.ut1_utc);
  			
 		
		
	

	
		
		
			
		setIat_utc(x.iat_utc);
  			
 		
		
	

	
		
		
			
		setTimeType(x.timeType);
  			
 		
		
	

	
		
		
			
		setGstAtUt0(Angle (x.gstAtUt0));
			
 		
		
	

	
		
		
			
		setEarthRotationRate(AngularRate (x.earthRotationRate));
			
 		
		
	

	
		
		
			
		polarOffsets .clear();
		for (unsigned int i = 0; i <x.polarOffsets.length(); ++i) {
			
			polarOffsets.push_back(x.polarOffsets[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setPolarOffsetsType(x.polarOffsetsType);
  			
 		
		
	

	
		
		nutationInLongitudeExists = x.nutationInLongitudeExists;
		if (x.nutationInLongitudeExists) {
		
		
			
		setNutationInLongitude(Angle (x.nutationInLongitude));
			
 		
		
		}
		
	

	
		
		nutationInLongitudeRateExists = x.nutationInLongitudeRateExists;
		if (x.nutationInLongitudeRateExists) {
		
		
			
		setNutationInLongitudeRate(AngularRate (x.nutationInLongitudeRate));
			
 		
		
		}
		
	

	
		
		nutationInObliquityExists = x.nutationInObliquityExists;
		if (x.nutationInObliquityExists) {
		
		
			
		setNutationInObliquity(Angle (x.nutationInObliquity));
			
 		
		
		}
		
	

	
		
		nutationInObliquityRateExists = x.nutationInObliquityRateExists;
		if (x.nutationInObliquityRateExists) {
		
		
			
		setNutationInObliquityRate(AngularRate (x.nutationInObliquityRate));
			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setDelayModelFixedParametersId(Tag (x.delayModelFixedParametersId));
			
 		
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"DelayModelVariableParameters");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string DelayModelVariableParametersRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(delayModelVariableParametersId, "delayModelVariableParametersId", buf);
		
		
	

  	
 		
		
		Parser::toXML(time, "time", buf);
		
		
	

  	
 		
		
		Parser::toXML(ut1_utc, "ut1_utc", buf);
		
		
	

  	
 		
		
		Parser::toXML(iat_utc, "iat_utc", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("timeType", timeType));
		
		
	

  	
 		
		
		Parser::toXML(gstAtUt0, "gstAtUt0", buf);
		
		
	

  	
 		
		
		Parser::toXML(earthRotationRate, "earthRotationRate", buf);
		
		
	

  	
 		
		
		Parser::toXML(polarOffsets, "polarOffsets", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarOffsetsType", polarOffsetsType));
		
		
	

  	
 		
		if (nutationInLongitudeExists) {
		
		
		Parser::toXML(nutationInLongitude, "nutationInLongitude", buf);
		
		
		}
		
	

  	
 		
		if (nutationInLongitudeRateExists) {
		
		
		Parser::toXML(nutationInLongitudeRate, "nutationInLongitudeRate", buf);
		
		
		}
		
	

  	
 		
		if (nutationInObliquityExists) {
		
		
		Parser::toXML(nutationInObliquity, "nutationInObliquity", buf);
		
		
		}
		
	

  	
 		
		if (nutationInObliquityRateExists) {
		
		
		Parser::toXML(nutationInObliquityRate, "nutationInObliquityRate", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(delayModelFixedParametersId, "delayModelFixedParametersId", buf);
		
		
	

	
		
	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void DelayModelVariableParametersRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setDelayModelVariableParametersId(Parser::getTag("delayModelVariableParametersId","DelayModelVariableParameters",rowDoc));
			
		
	

	
  		
			
	  	setTime(Parser::getArrayTime("time","DelayModelVariableParameters",rowDoc));
			
		
	

	
  		
			
	  	setUt1_utc(Parser::getDouble("ut1_utc","DelayModelVariableParameters",rowDoc));
			
		
	

	
  		
			
	  	setIat_utc(Parser::getDouble("iat_utc","DelayModelVariableParameters",rowDoc));
			
		
	

	
		
		
		
		timeType = EnumerationParser::getDifferenceType("timeType","DelayModelVariableParameters",rowDoc);
		
		
		
	

	
  		
			
	  	setGstAtUt0(Parser::getAngle("gstAtUt0","DelayModelVariableParameters",rowDoc));
			
		
	

	
  		
			
	  	setEarthRotationRate(Parser::getAngularRate("earthRotationRate","DelayModelVariableParameters",rowDoc));
			
		
	

	
  		
			
					
	  	setPolarOffsets(Parser::get1DDouble("polarOffsets","DelayModelVariableParameters",rowDoc));
	  			
	  		
		
	

	
		
		
		
		polarOffsetsType = EnumerationParser::getDifferenceType("polarOffsetsType","DelayModelVariableParameters",rowDoc);
		
		
		
	

	
  		
        if (row.isStr("<nutationInLongitude>")) {
			
	  		setNutationInLongitude(Parser::getAngle("nutationInLongitude","DelayModelVariableParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<nutationInLongitudeRate>")) {
			
	  		setNutationInLongitudeRate(Parser::getAngularRate("nutationInLongitudeRate","DelayModelVariableParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<nutationInObliquity>")) {
			
	  		setNutationInObliquity(Parser::getAngle("nutationInObliquity","DelayModelVariableParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<nutationInObliquityRate>")) {
			
	  		setNutationInObliquityRate(Parser::getAngularRate("nutationInObliquityRate","DelayModelVariableParameters",rowDoc));
			
		}
 		
	

	
	
		
	
  		
			
	  	setDelayModelFixedParametersId(Parser::getTag("delayModelFixedParametersId","DelayModelVariableParameters",rowDoc));
			
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"DelayModelVariableParameters");
		}
	}
	
	void DelayModelVariableParametersRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	delayModelVariableParametersId.toBin(eoss);
		
	

	
	
		
	time.toBin(eoss);
		
	

	
	
		
						
			eoss.writeDouble(ut1_utc);
				
		
	

	
	
		
						
			eoss.writeDouble(iat_utc);
				
		
	

	
	
		
					
		eoss.writeString(CDifferenceType::name(timeType));
			/* eoss.writeInt(timeType); */
				
		
	

	
	
		
	gstAtUt0.toBin(eoss);
		
	

	
	
		
	earthRotationRate.toBin(eoss);
		
	

	
	
		
		
			
		eoss.writeInt((int) polarOffsets.size());
		for (unsigned int i = 0; i < polarOffsets.size(); i++)
				
			eoss.writeDouble(polarOffsets.at(i));
				
				
						
		
	

	
	
		
					
		eoss.writeString(CDifferenceType::name(polarOffsetsType));
			/* eoss.writeInt(polarOffsetsType); */
				
		
	

	
	
		
	delayModelFixedParametersId.toBin(eoss);
		
	


	
	
	eoss.writeBoolean(nutationInLongitudeExists);
	if (nutationInLongitudeExists) {
	
	
	
		
	nutationInLongitude.toBin(eoss);
		
	

	}

	eoss.writeBoolean(nutationInLongitudeRateExists);
	if (nutationInLongitudeRateExists) {
	
	
	
		
	nutationInLongitudeRate.toBin(eoss);
		
	

	}

	eoss.writeBoolean(nutationInObliquityExists);
	if (nutationInObliquityExists) {
	
	
	
		
	nutationInObliquity.toBin(eoss);
		
	

	}

	eoss.writeBoolean(nutationInObliquityRateExists);
	if (nutationInObliquityRateExists) {
	
	
	
		
	nutationInObliquityRate.toBin(eoss);
		
	

	}

	}
	
void DelayModelVariableParametersRow::delayModelVariableParametersIdFromBin(EndianIStream& eis) {
		
	
		
		
		delayModelVariableParametersId =  Tag::fromBin(eis);
		
	
	
}
void DelayModelVariableParametersRow::timeFromBin(EndianIStream& eis) {
		
	
		
		
		time =  ArrayTime::fromBin(eis);
		
	
	
}
void DelayModelVariableParametersRow::ut1_utcFromBin(EndianIStream& eis) {
		
	
	
		
			
		ut1_utc =  eis.readDouble();
			
		
	
	
}
void DelayModelVariableParametersRow::iat_utcFromBin(EndianIStream& eis) {
		
	
	
		
			
		iat_utc =  eis.readDouble();
			
		
	
	
}
void DelayModelVariableParametersRow::timeTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		timeType = CDifferenceType::literal(eis.readString());
			
		
	
	
}
void DelayModelVariableParametersRow::gstAtUt0FromBin(EndianIStream& eis) {
		
	
		
		
		gstAtUt0 =  Angle::fromBin(eis);
		
	
	
}
void DelayModelVariableParametersRow::earthRotationRateFromBin(EndianIStream& eis) {
		
	
		
		
		earthRotationRate =  AngularRate::fromBin(eis);
		
	
	
}
void DelayModelVariableParametersRow::polarOffsetsFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		polarOffsets.clear();
		
		unsigned int polarOffsetsDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarOffsetsDim1; i++)
			
			polarOffsets.push_back(eis.readDouble());
			
	

		
	
	
}
void DelayModelVariableParametersRow::polarOffsetsTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		polarOffsetsType = CDifferenceType::literal(eis.readString());
			
		
	
	
}
void DelayModelVariableParametersRow::delayModelFixedParametersIdFromBin(EndianIStream& eis) {
		
	
		
		
		delayModelFixedParametersId =  Tag::fromBin(eis);
		
	
	
}

void DelayModelVariableParametersRow::nutationInLongitudeFromBin(EndianIStream& eis) {
		
	nutationInLongitudeExists = eis.readBoolean();
	if (nutationInLongitudeExists) {
		
	
		
		
		nutationInLongitude =  Angle::fromBin(eis);
		
	

	}
	
}
void DelayModelVariableParametersRow::nutationInLongitudeRateFromBin(EndianIStream& eis) {
		
	nutationInLongitudeRateExists = eis.readBoolean();
	if (nutationInLongitudeRateExists) {
		
	
		
		
		nutationInLongitudeRate =  AngularRate::fromBin(eis);
		
	

	}
	
}
void DelayModelVariableParametersRow::nutationInObliquityFromBin(EndianIStream& eis) {
		
	nutationInObliquityExists = eis.readBoolean();
	if (nutationInObliquityExists) {
		
	
		
		
		nutationInObliquity =  Angle::fromBin(eis);
		
	

	}
	
}
void DelayModelVariableParametersRow::nutationInObliquityRateFromBin(EndianIStream& eis) {
		
	nutationInObliquityRateExists = eis.readBoolean();
	if (nutationInObliquityRateExists) {
		
	
		
		
		nutationInObliquityRate =  AngularRate::fromBin(eis);
		
	

	}
	
}
	
	
	DelayModelVariableParametersRow* DelayModelVariableParametersRow::fromBin(EndianIStream& eis, DelayModelVariableParametersTable& table, const vector<string>& attributesSeq) {
		DelayModelVariableParametersRow* row = new  DelayModelVariableParametersRow(table);
		
		map<string, DelayModelVariableParametersAttributeFromBin>::iterator iter ;
		for (unsigned int i = 0; i < attributesSeq.size(); i++) {
			iter = row->fromBinMethods.find(attributesSeq.at(i));
			if (iter != row->fromBinMethods.end()) {
				(row->*(row->fromBinMethods[ attributesSeq.at(i) ] ))(eis);			
			}
			else {
				BinaryAttributeReaderFunctor* functorP = table.getUnknownAttributeBinaryReader(attributesSeq.at(i));
				if (functorP)
					(*functorP)(eis);
				else
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "DelayModelVariableParametersTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void DelayModelVariableParametersRow::delayModelVariableParametersIdFromText(const string & s) {
		 
          
		delayModelVariableParametersId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void DelayModelVariableParametersRow::timeFromText(const string & s) {
		 
          
		time = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelVariableParametersRow::ut1_utcFromText(const string & s) {
		 
          
		ut1_utc = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelVariableParametersRow::iat_utcFromText(const string & s) {
		 
          
		iat_utc = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an DifferenceType 
	void DelayModelVariableParametersRow::timeTypeFromText(const string & s) {
		 
          
		timeType = ASDMValuesParser::parse<DifferenceTypeMod::DifferenceType>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void DelayModelVariableParametersRow::gstAtUt0FromText(const string & s) {
		 
          
		gstAtUt0 = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	// Convert a string into an AngularRate 
	void DelayModelVariableParametersRow::earthRotationRateFromText(const string & s) {
		 
          
		earthRotationRate = ASDMValuesParser::parse<AngularRate>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelVariableParametersRow::polarOffsetsFromText(const string & s) {
		 
          
		polarOffsets = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an DifferenceType 
	void DelayModelVariableParametersRow::polarOffsetsTypeFromText(const string & s) {
		 
          
		polarOffsetsType = ASDMValuesParser::parse<DifferenceTypeMod::DifferenceType>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void DelayModelVariableParametersRow::delayModelFixedParametersIdFromText(const string & s) {
		 
          
		delayModelFixedParametersId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	

	
	// Convert a string into an Angle 
	void DelayModelVariableParametersRow::nutationInLongitudeFromText(const string & s) {
		nutationInLongitudeExists = true;
		 
          
		nutationInLongitude = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	// Convert a string into an AngularRate 
	void DelayModelVariableParametersRow::nutationInLongitudeRateFromText(const string & s) {
		nutationInLongitudeRateExists = true;
		 
          
		nutationInLongitudeRate = ASDMValuesParser::parse<AngularRate>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void DelayModelVariableParametersRow::nutationInObliquityFromText(const string & s) {
		nutationInObliquityExists = true;
		 
          
		nutationInObliquity = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	// Convert a string into an AngularRate 
	void DelayModelVariableParametersRow::nutationInObliquityRateFromText(const string & s) {
		nutationInObliquityRateExists = true;
		 
          
		nutationInObliquityRate = ASDMValuesParser::parse<AngularRate>(s);
          
		
	}
	
	
	
	void DelayModelVariableParametersRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, DelayModelVariableParametersAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "DelayModelVariableParametersTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get delayModelVariableParametersId.
 	 * @return delayModelVariableParametersId as Tag
 	 */
 	Tag DelayModelVariableParametersRow::getDelayModelVariableParametersId() const {
	
  		return delayModelVariableParametersId;
 	}

 	/**
 	 * Set delayModelVariableParametersId with the specified Tag.
 	 * @param delayModelVariableParametersId The Tag value to which delayModelVariableParametersId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void DelayModelVariableParametersRow::setDelayModelVariableParametersId (Tag delayModelVariableParametersId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("delayModelVariableParametersId", "DelayModelVariableParameters");
		
  		}
  	
 		this->delayModelVariableParametersId = delayModelVariableParametersId;
	
 	}
	
	

	

	
 	/**
 	 * Get time.
 	 * @return time as ArrayTime
 	 */
 	ArrayTime DelayModelVariableParametersRow::getTime() const {
	
  		return time;
 	}

 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setTime (ArrayTime time)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->time = time;
	
 	}
	
	

	

	
 	/**
 	 * Get ut1_utc.
 	 * @return ut1_utc as double
 	 */
 	double DelayModelVariableParametersRow::getUt1_utc() const {
	
  		return ut1_utc;
 	}

 	/**
 	 * Set ut1_utc with the specified double.
 	 * @param ut1_utc The double value to which ut1_utc is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setUt1_utc (double ut1_utc)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->ut1_utc = ut1_utc;
	
 	}
	
	

	

	
 	/**
 	 * Get iat_utc.
 	 * @return iat_utc as double
 	 */
 	double DelayModelVariableParametersRow::getIat_utc() const {
	
  		return iat_utc;
 	}

 	/**
 	 * Set iat_utc with the specified double.
 	 * @param iat_utc The double value to which iat_utc is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setIat_utc (double iat_utc)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->iat_utc = iat_utc;
	
 	}
	
	

	

	
 	/**
 	 * Get timeType.
 	 * @return timeType as DifferenceTypeMod::DifferenceType
 	 */
 	DifferenceTypeMod::DifferenceType DelayModelVariableParametersRow::getTimeType() const {
	
  		return timeType;
 	}

 	/**
 	 * Set timeType with the specified DifferenceTypeMod::DifferenceType.
 	 * @param timeType The DifferenceTypeMod::DifferenceType value to which timeType is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setTimeType (DifferenceTypeMod::DifferenceType timeType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->timeType = timeType;
	
 	}
	
	

	

	
 	/**
 	 * Get gstAtUt0.
 	 * @return gstAtUt0 as Angle
 	 */
 	Angle DelayModelVariableParametersRow::getGstAtUt0() const {
	
  		return gstAtUt0;
 	}

 	/**
 	 * Set gstAtUt0 with the specified Angle.
 	 * @param gstAtUt0 The Angle value to which gstAtUt0 is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setGstAtUt0 (Angle gstAtUt0)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->gstAtUt0 = gstAtUt0;
	
 	}
	
	

	

	
 	/**
 	 * Get earthRotationRate.
 	 * @return earthRotationRate as AngularRate
 	 */
 	AngularRate DelayModelVariableParametersRow::getEarthRotationRate() const {
	
  		return earthRotationRate;
 	}

 	/**
 	 * Set earthRotationRate with the specified AngularRate.
 	 * @param earthRotationRate The AngularRate value to which earthRotationRate is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setEarthRotationRate (AngularRate earthRotationRate)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->earthRotationRate = earthRotationRate;
	
 	}
	
	

	

	
 	/**
 	 * Get polarOffsets.
 	 * @return polarOffsets as std::vector<double >
 	 */
 	std::vector<double > DelayModelVariableParametersRow::getPolarOffsets() const {
	
  		return polarOffsets;
 	}

 	/**
 	 * Set polarOffsets with the specified std::vector<double >.
 	 * @param polarOffsets The std::vector<double > value to which polarOffsets is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setPolarOffsets (std::vector<double > polarOffsets)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarOffsets = polarOffsets;
	
 	}
	
	

	

	
 	/**
 	 * Get polarOffsetsType.
 	 * @return polarOffsetsType as DifferenceTypeMod::DifferenceType
 	 */
 	DifferenceTypeMod::DifferenceType DelayModelVariableParametersRow::getPolarOffsetsType() const {
	
  		return polarOffsetsType;
 	}

 	/**
 	 * Set polarOffsetsType with the specified DifferenceTypeMod::DifferenceType.
 	 * @param polarOffsetsType The DifferenceTypeMod::DifferenceType value to which polarOffsetsType is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setPolarOffsetsType (DifferenceTypeMod::DifferenceType polarOffsetsType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarOffsetsType = polarOffsetsType;
	
 	}
	
	

	
	/**
	 * The attribute nutationInLongitude is optional. Return true if this attribute exists.
	 * @return true if and only if the nutationInLongitude attribute exists. 
	 */
	bool DelayModelVariableParametersRow::isNutationInLongitudeExists() const {
		return nutationInLongitudeExists;
	}
	

	
 	/**
 	 * Get nutationInLongitude, which is optional.
 	 * @return nutationInLongitude as Angle
 	 * @throw IllegalAccessException If nutationInLongitude does not exist.
 	 */
 	Angle DelayModelVariableParametersRow::getNutationInLongitude() const  {
		if (!nutationInLongitudeExists) {
			throw IllegalAccessException("nutationInLongitude", "DelayModelVariableParameters");
		}
	
  		return nutationInLongitude;
 	}

 	/**
 	 * Set nutationInLongitude with the specified Angle.
 	 * @param nutationInLongitude The Angle value to which nutationInLongitude is to be set.
 	 
 	
 	 */
 	void DelayModelVariableParametersRow::setNutationInLongitude (Angle nutationInLongitude) {
	
 		this->nutationInLongitude = nutationInLongitude;
	
		nutationInLongitudeExists = true;
	
 	}
	
	
	/**
	 * Mark nutationInLongitude, which is an optional field, as non-existent.
	 */
	void DelayModelVariableParametersRow::clearNutationInLongitude () {
		nutationInLongitudeExists = false;
	}
	

	
	/**
	 * The attribute nutationInLongitudeRate is optional. Return true if this attribute exists.
	 * @return true if and only if the nutationInLongitudeRate attribute exists. 
	 */
	bool DelayModelVariableParametersRow::isNutationInLongitudeRateExists() const {
		return nutationInLongitudeRateExists;
	}
	

	
 	/**
 	 * Get nutationInLongitudeRate, which is optional.
 	 * @return nutationInLongitudeRate as AngularRate
 	 * @throw IllegalAccessException If nutationInLongitudeRate does not exist.
 	 */
 	AngularRate DelayModelVariableParametersRow::getNutationInLongitudeRate() const  {
		if (!nutationInLongitudeRateExists) {
			throw IllegalAccessException("nutationInLongitudeRate", "DelayModelVariableParameters");
		}
	
  		return nutationInLongitudeRate;
 	}

 	/**
 	 * Set nutationInLongitudeRate with the specified AngularRate.
 	 * @param nutationInLongitudeRate The AngularRate value to which nutationInLongitudeRate is to be set.
 	 
 	
 	 */
 	void DelayModelVariableParametersRow::setNutationInLongitudeRate (AngularRate nutationInLongitudeRate) {
	
 		this->nutationInLongitudeRate = nutationInLongitudeRate;
	
		nutationInLongitudeRateExists = true;
	
 	}
	
	
	/**
	 * Mark nutationInLongitudeRate, which is an optional field, as non-existent.
	 */
	void DelayModelVariableParametersRow::clearNutationInLongitudeRate () {
		nutationInLongitudeRateExists = false;
	}
	

	
	/**
	 * The attribute nutationInObliquity is optional. Return true if this attribute exists.
	 * @return true if and only if the nutationInObliquity attribute exists. 
	 */
	bool DelayModelVariableParametersRow::isNutationInObliquityExists() const {
		return nutationInObliquityExists;
	}
	

	
 	/**
 	 * Get nutationInObliquity, which is optional.
 	 * @return nutationInObliquity as Angle
 	 * @throw IllegalAccessException If nutationInObliquity does not exist.
 	 */
 	Angle DelayModelVariableParametersRow::getNutationInObliquity() const  {
		if (!nutationInObliquityExists) {
			throw IllegalAccessException("nutationInObliquity", "DelayModelVariableParameters");
		}
	
  		return nutationInObliquity;
 	}

 	/**
 	 * Set nutationInObliquity with the specified Angle.
 	 * @param nutationInObliquity The Angle value to which nutationInObliquity is to be set.
 	 
 	
 	 */
 	void DelayModelVariableParametersRow::setNutationInObliquity (Angle nutationInObliquity) {
	
 		this->nutationInObliquity = nutationInObliquity;
	
		nutationInObliquityExists = true;
	
 	}
	
	
	/**
	 * Mark nutationInObliquity, which is an optional field, as non-existent.
	 */
	void DelayModelVariableParametersRow::clearNutationInObliquity () {
		nutationInObliquityExists = false;
	}
	

	
	/**
	 * The attribute nutationInObliquityRate is optional. Return true if this attribute exists.
	 * @return true if and only if the nutationInObliquityRate attribute exists. 
	 */
	bool DelayModelVariableParametersRow::isNutationInObliquityRateExists() const {
		return nutationInObliquityRateExists;
	}
	

	
 	/**
 	 * Get nutationInObliquityRate, which is optional.
 	 * @return nutationInObliquityRate as AngularRate
 	 * @throw IllegalAccessException If nutationInObliquityRate does not exist.
 	 */
 	AngularRate DelayModelVariableParametersRow::getNutationInObliquityRate() const  {
		if (!nutationInObliquityRateExists) {
			throw IllegalAccessException("nutationInObliquityRate", "DelayModelVariableParameters");
		}
	
  		return nutationInObliquityRate;
 	}

 	/**
 	 * Set nutationInObliquityRate with the specified AngularRate.
 	 * @param nutationInObliquityRate The AngularRate value to which nutationInObliquityRate is to be set.
 	 
 	
 	 */
 	void DelayModelVariableParametersRow::setNutationInObliquityRate (AngularRate nutationInObliquityRate) {
	
 		this->nutationInObliquityRate = nutationInObliquityRate;
	
		nutationInObliquityRateExists = true;
	
 	}
	
	
	/**
	 * Mark nutationInObliquityRate, which is an optional field, as non-existent.
	 */
	void DelayModelVariableParametersRow::clearNutationInObliquityRate () {
		nutationInObliquityRateExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get delayModelFixedParametersId.
 	 * @return delayModelFixedParametersId as Tag
 	 */
 	Tag DelayModelVariableParametersRow::getDelayModelFixedParametersId() const {
	
  		return delayModelFixedParametersId;
 	}

 	/**
 	 * Set delayModelFixedParametersId with the specified Tag.
 	 * @param delayModelFixedParametersId The Tag value to which delayModelFixedParametersId is to be set.
 	 
 	
 		
 	 */
 	void DelayModelVariableParametersRow::setDelayModelFixedParametersId (Tag delayModelFixedParametersId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->delayModelFixedParametersId = delayModelFixedParametersId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the DelayModelFixedParameters table having DelayModelFixedParameters.delayModelFixedParametersId == delayModelFixedParametersId
	 * @return a DelayModelFixedParametersRow*
	 * 
	 
	 */
	 DelayModelFixedParametersRow* DelayModelVariableParametersRow::getDelayModelFixedParametersUsingDelayModelFixedParametersId() {
	 
	 	return table.getContainer().getDelayModelFixedParameters().getRowByKey(delayModelFixedParametersId);
	 }
	 

	

	
	/**
	 * Create a DelayModelVariableParametersRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	DelayModelVariableParametersRow::DelayModelVariableParametersRow (DelayModelVariableParametersTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	
		nutationInLongitudeExists = false;
	

	
		nutationInLongitudeRateExists = false;
	

	
		nutationInObliquityExists = false;
	

	
		nutationInObliquityRateExists = false;
	

	
	

	
	
	
	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
timeType = CDifferenceType::from_int(0);
	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
polarOffsetsType = CDifferenceType::from_int(0);
	

	

	

	

	

	
	
	 fromBinMethods["delayModelVariableParametersId"] = &DelayModelVariableParametersRow::delayModelVariableParametersIdFromBin; 
	 fromBinMethods["time"] = &DelayModelVariableParametersRow::timeFromBin; 
	 fromBinMethods["ut1_utc"] = &DelayModelVariableParametersRow::ut1_utcFromBin; 
	 fromBinMethods["iat_utc"] = &DelayModelVariableParametersRow::iat_utcFromBin; 
	 fromBinMethods["timeType"] = &DelayModelVariableParametersRow::timeTypeFromBin; 
	 fromBinMethods["gstAtUt0"] = &DelayModelVariableParametersRow::gstAtUt0FromBin; 
	 fromBinMethods["earthRotationRate"] = &DelayModelVariableParametersRow::earthRotationRateFromBin; 
	 fromBinMethods["polarOffsets"] = &DelayModelVariableParametersRow::polarOffsetsFromBin; 
	 fromBinMethods["polarOffsetsType"] = &DelayModelVariableParametersRow::polarOffsetsTypeFromBin; 
	 fromBinMethods["delayModelFixedParametersId"] = &DelayModelVariableParametersRow::delayModelFixedParametersIdFromBin; 
		
	
	 fromBinMethods["nutationInLongitude"] = &DelayModelVariableParametersRow::nutationInLongitudeFromBin; 
	 fromBinMethods["nutationInLongitudeRate"] = &DelayModelVariableParametersRow::nutationInLongitudeRateFromBin; 
	 fromBinMethods["nutationInObliquity"] = &DelayModelVariableParametersRow::nutationInObliquityFromBin; 
	 fromBinMethods["nutationInObliquityRate"] = &DelayModelVariableParametersRow::nutationInObliquityRateFromBin; 
	
	
	
	
				 
	fromTextMethods["delayModelVariableParametersId"] = &DelayModelVariableParametersRow::delayModelVariableParametersIdFromText;
		 
	
				 
	fromTextMethods["time"] = &DelayModelVariableParametersRow::timeFromText;
		 
	
				 
	fromTextMethods["ut1_utc"] = &DelayModelVariableParametersRow::ut1_utcFromText;
		 
	
				 
	fromTextMethods["iat_utc"] = &DelayModelVariableParametersRow::iat_utcFromText;
		 
	
				 
	fromTextMethods["timeType"] = &DelayModelVariableParametersRow::timeTypeFromText;
		 
	
				 
	fromTextMethods["gstAtUt0"] = &DelayModelVariableParametersRow::gstAtUt0FromText;
		 
	
				 
	fromTextMethods["earthRotationRate"] = &DelayModelVariableParametersRow::earthRotationRateFromText;
		 
	
				 
	fromTextMethods["polarOffsets"] = &DelayModelVariableParametersRow::polarOffsetsFromText;
		 
	
				 
	fromTextMethods["polarOffsetsType"] = &DelayModelVariableParametersRow::polarOffsetsTypeFromText;
		 
	
				 
	fromTextMethods["delayModelFixedParametersId"] = &DelayModelVariableParametersRow::delayModelFixedParametersIdFromText;
		 
	

	 
				
	fromTextMethods["nutationInLongitude"] = &DelayModelVariableParametersRow::nutationInLongitudeFromText;
		 	
	 
				
	fromTextMethods["nutationInLongitudeRate"] = &DelayModelVariableParametersRow::nutationInLongitudeRateFromText;
		 	
	 
				
	fromTextMethods["nutationInObliquity"] = &DelayModelVariableParametersRow::nutationInObliquityFromText;
		 	
	 
				
	fromTextMethods["nutationInObliquityRate"] = &DelayModelVariableParametersRow::nutationInObliquityRateFromText;
		 	
		
	}
	
	DelayModelVariableParametersRow::DelayModelVariableParametersRow (DelayModelVariableParametersTable &t, DelayModelVariableParametersRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	
		nutationInLongitudeExists = false;
	

	
		nutationInLongitudeRateExists = false;
	

	
		nutationInObliquityExists = false;
	

	
		nutationInObliquityRateExists = false;
	

	
	
		
		}
		else {
	
		
			delayModelVariableParametersId = row->delayModelVariableParametersId;
		
		
		
		
			time = row->time;
		
			ut1_utc = row->ut1_utc;
		
			iat_utc = row->iat_utc;
		
			timeType = row->timeType;
		
			gstAtUt0 = row->gstAtUt0;
		
			earthRotationRate = row->earthRotationRate;
		
			polarOffsets = row->polarOffsets;
		
			polarOffsetsType = row->polarOffsetsType;
		
			delayModelFixedParametersId = row->delayModelFixedParametersId;
		
		
		
		
		if (row->nutationInLongitudeExists) {
			nutationInLongitude = row->nutationInLongitude;		
			nutationInLongitudeExists = true;
		}
		else
			nutationInLongitudeExists = false;
		
		if (row->nutationInLongitudeRateExists) {
			nutationInLongitudeRate = row->nutationInLongitudeRate;		
			nutationInLongitudeRateExists = true;
		}
		else
			nutationInLongitudeRateExists = false;
		
		if (row->nutationInObliquityExists) {
			nutationInObliquity = row->nutationInObliquity;		
			nutationInObliquityExists = true;
		}
		else
			nutationInObliquityExists = false;
		
		if (row->nutationInObliquityRateExists) {
			nutationInObliquityRate = row->nutationInObliquityRate;		
			nutationInObliquityRateExists = true;
		}
		else
			nutationInObliquityRateExists = false;
		
		}
		
		 fromBinMethods["delayModelVariableParametersId"] = &DelayModelVariableParametersRow::delayModelVariableParametersIdFromBin; 
		 fromBinMethods["time"] = &DelayModelVariableParametersRow::timeFromBin; 
		 fromBinMethods["ut1_utc"] = &DelayModelVariableParametersRow::ut1_utcFromBin; 
		 fromBinMethods["iat_utc"] = &DelayModelVariableParametersRow::iat_utcFromBin; 
		 fromBinMethods["timeType"] = &DelayModelVariableParametersRow::timeTypeFromBin; 
		 fromBinMethods["gstAtUt0"] = &DelayModelVariableParametersRow::gstAtUt0FromBin; 
		 fromBinMethods["earthRotationRate"] = &DelayModelVariableParametersRow::earthRotationRateFromBin; 
		 fromBinMethods["polarOffsets"] = &DelayModelVariableParametersRow::polarOffsetsFromBin; 
		 fromBinMethods["polarOffsetsType"] = &DelayModelVariableParametersRow::polarOffsetsTypeFromBin; 
		 fromBinMethods["delayModelFixedParametersId"] = &DelayModelVariableParametersRow::delayModelFixedParametersIdFromBin; 
			
	
		 fromBinMethods["nutationInLongitude"] = &DelayModelVariableParametersRow::nutationInLongitudeFromBin; 
		 fromBinMethods["nutationInLongitudeRate"] = &DelayModelVariableParametersRow::nutationInLongitudeRateFromBin; 
		 fromBinMethods["nutationInObliquity"] = &DelayModelVariableParametersRow::nutationInObliquityFromBin; 
		 fromBinMethods["nutationInObliquityRate"] = &DelayModelVariableParametersRow::nutationInObliquityRateFromBin; 
			
	}

	
	bool DelayModelVariableParametersRow::compareNoAutoInc(ArrayTime time, double ut1_utc, double iat_utc, DifferenceTypeMod::DifferenceType timeType, Angle gstAtUt0, AngularRate earthRotationRate, std::vector<double > polarOffsets, DifferenceTypeMod::DifferenceType polarOffsetsType, Tag delayModelFixedParametersId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->time == time);
		
		if (!result) return false;
	

	
		
		result = result && (this->ut1_utc == ut1_utc);
		
		if (!result) return false;
	

	
		
		result = result && (this->iat_utc == iat_utc);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeType == timeType);
		
		if (!result) return false;
	

	
		
		result = result && (this->gstAtUt0 == gstAtUt0);
		
		if (!result) return false;
	

	
		
		result = result && (this->earthRotationRate == earthRotationRate);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarOffsets == polarOffsets);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarOffsetsType == polarOffsetsType);
		
		if (!result) return false;
	

	
		
		result = result && (this->delayModelFixedParametersId == delayModelFixedParametersId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool DelayModelVariableParametersRow::compareRequiredValue(ArrayTime time, double ut1_utc, double iat_utc, DifferenceTypeMod::DifferenceType timeType, Angle gstAtUt0, AngularRate earthRotationRate, std::vector<double > polarOffsets, DifferenceTypeMod::DifferenceType polarOffsetsType, Tag delayModelFixedParametersId) {
		bool result;
		result = true;
		
	
		if (!(this->time == time)) return false;
	

	
		if (!(this->ut1_utc == ut1_utc)) return false;
	

	
		if (!(this->iat_utc == iat_utc)) return false;
	

	
		if (!(this->timeType == timeType)) return false;
	

	
		if (!(this->gstAtUt0 == gstAtUt0)) return false;
	

	
		if (!(this->earthRotationRate == earthRotationRate)) return false;
	

	
		if (!(this->polarOffsets == polarOffsets)) return false;
	

	
		if (!(this->polarOffsetsType == polarOffsetsType)) return false;
	

	
		if (!(this->delayModelFixedParametersId == delayModelFixedParametersId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the DelayModelVariableParametersRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool DelayModelVariableParametersRow::equalByRequiredValue(DelayModelVariableParametersRow*  x ) {
		
			
		if (this->time != x->time) return false;
			
		if (this->ut1_utc != x->ut1_utc) return false;
			
		if (this->iat_utc != x->iat_utc) return false;
			
		if (this->timeType != x->timeType) return false;
			
		if (this->gstAtUt0 != x->gstAtUt0) return false;
			
		if (this->earthRotationRate != x->earthRotationRate) return false;
			
		if (this->polarOffsets != x->polarOffsets) return false;
			
		if (this->polarOffsetsType != x->polarOffsetsType) return false;
			
		if (this->delayModelFixedParametersId != x->delayModelFixedParametersId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, DelayModelVariableParametersAttributeFromBin> DelayModelVariableParametersRow::initFromBinMethods() {
		map<string, DelayModelVariableParametersAttributeFromBin> result;
		
		result["delayModelVariableParametersId"] = &DelayModelVariableParametersRow::delayModelVariableParametersIdFromBin;
		result["time"] = &DelayModelVariableParametersRow::timeFromBin;
		result["ut1_utc"] = &DelayModelVariableParametersRow::ut1_utcFromBin;
		result["iat_utc"] = &DelayModelVariableParametersRow::iat_utcFromBin;
		result["timeType"] = &DelayModelVariableParametersRow::timeTypeFromBin;
		result["gstAtUt0"] = &DelayModelVariableParametersRow::gstAtUt0FromBin;
		result["earthRotationRate"] = &DelayModelVariableParametersRow::earthRotationRateFromBin;
		result["polarOffsets"] = &DelayModelVariableParametersRow::polarOffsetsFromBin;
		result["polarOffsetsType"] = &DelayModelVariableParametersRow::polarOffsetsTypeFromBin;
		result["delayModelFixedParametersId"] = &DelayModelVariableParametersRow::delayModelFixedParametersIdFromBin;
		
		
		result["nutationInLongitude"] = &DelayModelVariableParametersRow::nutationInLongitudeFromBin;
		result["nutationInLongitudeRate"] = &DelayModelVariableParametersRow::nutationInLongitudeRateFromBin;
		result["nutationInObliquity"] = &DelayModelVariableParametersRow::nutationInObliquityFromBin;
		result["nutationInObliquityRate"] = &DelayModelVariableParametersRow::nutationInObliquityRateFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
