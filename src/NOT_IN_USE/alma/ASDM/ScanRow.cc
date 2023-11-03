
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
 * File ScanRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/ScanRow.h>
#include <alma/ASDM/ScanTable.h>

#include <alma/ASDM/ExecBlockTable.h>
#include <alma/ASDM/ExecBlockRow.h>
	

using asdm::ASDM;
using asdm::ScanRow;
using asdm::ScanTable;

using asdm::ExecBlockTable;
using asdm::ExecBlockRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	ScanRow::~ScanRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	ScanTable &ScanRow::getTable() const {
		return table;
	}

	bool ScanRow::isAdded() const {
		return hasBeenAdded;
	}	

	void ScanRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::ScanRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a ScanRowIDL struct.
	 */
	ScanRowIDL *ScanRow::toIDL() const {
		ScanRowIDL *x = new ScanRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->scanNumber = scanNumber;
 				
 			
		
	

	
  		
		
		
			
		x->startTime = startTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endTime = endTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->numIntent = numIntent;
 				
 			
		
	

	
  		
		
		
			
				
		x->numSubscan = numSubscan;
 				
 			
		
	

	
  		
		
		
			
		x->scanIntent.length(scanIntent.size());
		for (unsigned int i = 0; i < scanIntent.size(); ++i) {
			
				
			x->scanIntent[i] = scanIntent.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->calDataType.length(calDataType.size());
		for (unsigned int i = 0; i < calDataType.size(); ++i) {
			
				
			x->calDataType[i] = calDataType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->calibrationOnLine.length(calibrationOnLine.size());
		for (unsigned int i = 0; i < calibrationOnLine.size(); ++i) {
			
				
			x->calibrationOnLine[i] = calibrationOnLine.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->calibrationFunctionExists = calibrationFunctionExists;
		
		
			
		x->calibrationFunction.length(calibrationFunction.size());
		for (unsigned int i = 0; i < calibrationFunction.size(); ++i) {
			
				
			x->calibrationFunction[i] = calibrationFunction.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->calibrationSetExists = calibrationSetExists;
		
		
			
		x->calibrationSet.length(calibrationSet.size());
		for (unsigned int i = 0; i < calibrationSet.size(); ++i) {
			
				
			x->calibrationSet[i] = calibrationSet.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->calPatternExists = calPatternExists;
		
		
			
		x->calPattern.length(calPattern.size());
		for (unsigned int i = 0; i < calPattern.size(); ++i) {
			
				
			x->calPattern[i] = calPattern.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->numFieldExists = numFieldExists;
		
		
			
				
		x->numField = numField;
 				
 			
		
	

	
  		
		
		x->fieldNameExists = fieldNameExists;
		
		
			
		x->fieldName.length(fieldName.size());
		for (unsigned int i = 0; i < fieldName.size(); ++i) {
			
				
			x->fieldName[i] = CORBA::string_dup(fieldName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x->sourceNameExists = sourceNameExists;
		
		
			
				
		x->sourceName = CORBA::string_dup(sourceName.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void ScanRow::toIDL(asdmIDL::ScanRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.scanNumber = scanNumber;
 				
 			
		
	

	
  		
		
		
			
		x.startTime = startTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endTime = endTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.numIntent = numIntent;
 				
 			
		
	

	
  		
		
		
			
				
		x.numSubscan = numSubscan;
 				
 			
		
	

	
  		
		
		
			
		x.scanIntent.length(scanIntent.size());
		for (unsigned int i = 0; i < scanIntent.size(); ++i) {
			
				
			x.scanIntent[i] = scanIntent.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.calDataType.length(calDataType.size());
		for (unsigned int i = 0; i < calDataType.size(); ++i) {
			
				
			x.calDataType[i] = calDataType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.calibrationOnLine.length(calibrationOnLine.size());
		for (unsigned int i = 0; i < calibrationOnLine.size(); ++i) {
			
				
			x.calibrationOnLine[i] = calibrationOnLine.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.calibrationFunctionExists = calibrationFunctionExists;
		
		
			
		x.calibrationFunction.length(calibrationFunction.size());
		for (unsigned int i = 0; i < calibrationFunction.size(); ++i) {
			
				
			x.calibrationFunction[i] = calibrationFunction.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.calibrationSetExists = calibrationSetExists;
		
		
			
		x.calibrationSet.length(calibrationSet.size());
		for (unsigned int i = 0; i < calibrationSet.size(); ++i) {
			
				
			x.calibrationSet[i] = calibrationSet.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.calPatternExists = calPatternExists;
		
		
			
		x.calPattern.length(calPattern.size());
		for (unsigned int i = 0; i < calPattern.size(); ++i) {
			
				
			x.calPattern[i] = calPattern.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.numFieldExists = numFieldExists;
		
		
			
				
		x.numField = numField;
 				
 			
		
	

	
  		
		
		x.fieldNameExists = fieldNameExists;
		
		
			
		x.fieldName.length(fieldName.size());
		for (unsigned int i = 0; i < fieldName.size(); ++i) {
			
				
			x.fieldName[i] = CORBA::string_dup(fieldName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x.sourceNameExists = sourceNameExists;
		
		
			
				
		x.sourceName = CORBA::string_dup(sourceName.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct ScanRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void ScanRow::setFromIDL (ScanRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setScanNumber(x.scanNumber);
  			
 		
		
	

	
		
		
			
		setStartTime(ArrayTime (x.startTime));
			
 		
		
	

	
		
		
			
		setEndTime(ArrayTime (x.endTime));
			
 		
		
	

	
		
		
			
		setNumIntent(x.numIntent);
  			
 		
		
	

	
		
		
			
		setNumSubscan(x.numSubscan);
  			
 		
		
	

	
		
		
			
		scanIntent .clear();
		for (unsigned int i = 0; i <x.scanIntent.length(); ++i) {
			
			scanIntent.push_back(x.scanIntent[i]);
  			
		}
			
  		
		
	

	
		
		
			
		calDataType .clear();
		for (unsigned int i = 0; i <x.calDataType.length(); ++i) {
			
			calDataType.push_back(x.calDataType[i]);
  			
		}
			
  		
		
	

	
		
		
			
		calibrationOnLine .clear();
		for (unsigned int i = 0; i <x.calibrationOnLine.length(); ++i) {
			
			calibrationOnLine.push_back(x.calibrationOnLine[i]);
  			
		}
			
  		
		
	

	
		
		calibrationFunctionExists = x.calibrationFunctionExists;
		if (x.calibrationFunctionExists) {
		
		
			
		calibrationFunction .clear();
		for (unsigned int i = 0; i <x.calibrationFunction.length(); ++i) {
			
			calibrationFunction.push_back(x.calibrationFunction[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		calibrationSetExists = x.calibrationSetExists;
		if (x.calibrationSetExists) {
		
		
			
		calibrationSet .clear();
		for (unsigned int i = 0; i <x.calibrationSet.length(); ++i) {
			
			calibrationSet.push_back(x.calibrationSet[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		calPatternExists = x.calPatternExists;
		if (x.calPatternExists) {
		
		
			
		calPattern .clear();
		for (unsigned int i = 0; i <x.calPattern.length(); ++i) {
			
			calPattern.push_back(x.calPattern[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		numFieldExists = x.numFieldExists;
		if (x.numFieldExists) {
		
		
			
		setNumField(x.numField);
  			
 		
		
		}
		
	

	
		
		fieldNameExists = x.fieldNameExists;
		if (x.fieldNameExists) {
		
		
			
		fieldName .clear();
		for (unsigned int i = 0; i <x.fieldName.length(); ++i) {
			
			fieldName.push_back(string (x.fieldName[i]));
			
		}
			
  		
		
		}
		
	

	
		
		sourceNameExists = x.sourceNameExists;
		if (x.sourceNameExists) {
		
		
			
		setSourceName(string (x.sourceName));
			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setExecBlockId(Tag (x.execBlockId));
			
 		
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Scan");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string ScanRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(scanNumber, "scanNumber", buf);
		
		
	

  	
 		
		
		Parser::toXML(startTime, "startTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endTime, "endTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(numIntent, "numIntent", buf);
		
		
	

  	
 		
		
		Parser::toXML(numSubscan, "numSubscan", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("scanIntent", scanIntent));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("calDataType", calDataType));
		
		
	

  	
 		
		
		Parser::toXML(calibrationOnLine, "calibrationOnLine", buf);
		
		
	

  	
 		
		if (calibrationFunctionExists) {
		
		
			buf.append(EnumerationParser::toXML("calibrationFunction", calibrationFunction));
		
		
		}
		
	

  	
 		
		if (calibrationSetExists) {
		
		
			buf.append(EnumerationParser::toXML("calibrationSet", calibrationSet));
		
		
		}
		
	

  	
 		
		if (calPatternExists) {
		
		
			buf.append(EnumerationParser::toXML("calPattern", calPattern));
		
		
		}
		
	

  	
 		
		if (numFieldExists) {
		
		
		Parser::toXML(numField, "numField", buf);
		
		
		}
		
	

  	
 		
		if (fieldNameExists) {
		
		
		Parser::toXML(fieldName, "fieldName", buf);
		
		
		}
		
	

  	
 		
		if (sourceNameExists) {
		
		
		Parser::toXML(sourceName, "sourceName", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(execBlockId, "execBlockId", buf);
		
		
	

	
		
	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void ScanRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setScanNumber(Parser::getInteger("scanNumber","Scan",rowDoc));
			
		
	

	
  		
			
	  	setStartTime(Parser::getArrayTime("startTime","Scan",rowDoc));
			
		
	

	
  		
			
	  	setEndTime(Parser::getArrayTime("endTime","Scan",rowDoc));
			
		
	

	
  		
			
	  	setNumIntent(Parser::getInteger("numIntent","Scan",rowDoc));
			
		
	

	
  		
			
	  	setNumSubscan(Parser::getInteger("numSubscan","Scan",rowDoc));
			
		
	

	
		
		
		
		scanIntent = EnumerationParser::getScanIntent1D("scanIntent","Scan",rowDoc);			
		
		
		
	

	
		
		
		
		calDataType = EnumerationParser::getCalDataOrigin1D("calDataType","Scan",rowDoc);			
		
		
		
	

	
  		
			
					
	  	setCalibrationOnLine(Parser::get1DBoolean("calibrationOnLine","Scan",rowDoc));
	  			
	  		
		
	

	
		
	if (row.isStr("<calibrationFunction>")) {
		
		
		
		calibrationFunction = EnumerationParser::getCalibrationFunction1D("calibrationFunction","Scan",rowDoc);			
		
		
		
		calibrationFunctionExists = true;
	}
		
	

	
		
	if (row.isStr("<calibrationSet>")) {
		
		
		
		calibrationSet = EnumerationParser::getCalibrationSet1D("calibrationSet","Scan",rowDoc);			
		
		
		
		calibrationSetExists = true;
	}
		
	

	
		
	if (row.isStr("<calPattern>")) {
		
		
		
		calPattern = EnumerationParser::getAntennaMotionPattern1D("calPattern","Scan",rowDoc);			
		
		
		
		calPatternExists = true;
	}
		
	

	
  		
        if (row.isStr("<numField>")) {
			
	  		setNumField(Parser::getInteger("numField","Scan",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<fieldName>")) {
			
								
	  		setFieldName(Parser::get1DString("fieldName","Scan",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<sourceName>")) {
			
	  		setSourceName(Parser::getString("sourceName","Scan",rowDoc));
			
		}
 		
	

	
	
		
	
  		
			
	  	setExecBlockId(Parser::getTag("execBlockId","ExecBlock",rowDoc));
			
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Scan");
		}
	}
	
	void ScanRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	execBlockId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(scanNumber);
				
		
	

	
	
		
	startTime.toBin(eoss);
		
	

	
	
		
	endTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numIntent);
				
		
	

	
	
		
						
			eoss.writeInt(numSubscan);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) scanIntent.size());
		for (unsigned int i = 0; i < scanIntent.size(); i++)
				
			eoss.writeString(CScanIntent::name(scanIntent.at(i)));
			/* eoss.writeInt(scanIntent.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) calDataType.size());
		for (unsigned int i = 0; i < calDataType.size(); i++)
				
			eoss.writeString(CCalDataOrigin::name(calDataType.at(i)));
			/* eoss.writeInt(calDataType.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) calibrationOnLine.size());
		for (unsigned int i = 0; i < calibrationOnLine.size(); i++)
				
			eoss.writeBoolean(calibrationOnLine.at(i));
				
				
						
		
	


	
	
	eoss.writeBoolean(calibrationFunctionExists);
	if (calibrationFunctionExists) {
	
	
	
		
		
			
		eoss.writeInt((int) calibrationFunction.size());
		for (unsigned int i = 0; i < calibrationFunction.size(); i++)
				
			eoss.writeString(CCalibrationFunction::name(calibrationFunction.at(i)));
			/* eoss.writeInt(calibrationFunction.at(i)); */
				
				
						
		
	

	}

	eoss.writeBoolean(calibrationSetExists);
	if (calibrationSetExists) {
	
	
	
		
		
			
		eoss.writeInt((int) calibrationSet.size());
		for (unsigned int i = 0; i < calibrationSet.size(); i++)
				
			eoss.writeString(CCalibrationSet::name(calibrationSet.at(i)));
			/* eoss.writeInt(calibrationSet.at(i)); */
				
				
						
		
	

	}

	eoss.writeBoolean(calPatternExists);
	if (calPatternExists) {
	
	
	
		
		
			
		eoss.writeInt((int) calPattern.size());
		for (unsigned int i = 0; i < calPattern.size(); i++)
				
			eoss.writeString(CAntennaMotionPattern::name(calPattern.at(i)));
			/* eoss.writeInt(calPattern.at(i)); */
				
				
						
		
	

	}

	eoss.writeBoolean(numFieldExists);
	if (numFieldExists) {
	
	
	
		
						
			eoss.writeInt(numField);
				
		
	

	}

	eoss.writeBoolean(fieldNameExists);
	if (fieldNameExists) {
	
	
	
		
		
			
		eoss.writeInt((int) fieldName.size());
		for (unsigned int i = 0; i < fieldName.size(); i++)
				
			eoss.writeString(fieldName.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(sourceNameExists);
	if (sourceNameExists) {
	
	
	
		
						
			eoss.writeString(sourceName);
				
		
	

	}

	}
	
void ScanRow::execBlockIdFromBin(EndianIStream& eis) {
		
	
		
		
		execBlockId =  Tag::fromBin(eis);
		
	
	
}
void ScanRow::scanNumberFromBin(EndianIStream& eis) {
		
	
	
		
			
		scanNumber =  eis.readInt();
			
		
	
	
}
void ScanRow::startTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startTime =  ArrayTime::fromBin(eis);
		
	
	
}
void ScanRow::endTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endTime =  ArrayTime::fromBin(eis);
		
	
	
}
void ScanRow::numIntentFromBin(EndianIStream& eis) {
		
	
	
		
			
		numIntent =  eis.readInt();
			
		
	
	
}
void ScanRow::numSubscanFromBin(EndianIStream& eis) {
		
	
	
		
			
		numSubscan =  eis.readInt();
			
		
	
	
}
void ScanRow::scanIntentFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		scanIntent.clear();
		
		unsigned int scanIntentDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < scanIntentDim1; i++)
			
			scanIntent.push_back(CScanIntent::literal(eis.readString()));
			
	

		
	
	
}
void ScanRow::calDataTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		calDataType.clear();
		
		unsigned int calDataTypeDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < calDataTypeDim1; i++)
			
			calDataType.push_back(CCalDataOrigin::literal(eis.readString()));
			
	

		
	
	
}
void ScanRow::calibrationOnLineFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		calibrationOnLine.clear();
		
		unsigned int calibrationOnLineDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < calibrationOnLineDim1; i++)
			
			calibrationOnLine.push_back(eis.readBoolean());
			
	

		
	
	
}

void ScanRow::calibrationFunctionFromBin(EndianIStream& eis) {
		
	calibrationFunctionExists = eis.readBoolean();
	if (calibrationFunctionExists) {
		
	
	
		
			
	
		calibrationFunction.clear();
		
		unsigned int calibrationFunctionDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < calibrationFunctionDim1; i++)
			
			calibrationFunction.push_back(CCalibrationFunction::literal(eis.readString()));
			
	

		
	

	}
	
}
void ScanRow::calibrationSetFromBin(EndianIStream& eis) {
		
	calibrationSetExists = eis.readBoolean();
	if (calibrationSetExists) {
		
	
	
		
			
	
		calibrationSet.clear();
		
		unsigned int calibrationSetDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < calibrationSetDim1; i++)
			
			calibrationSet.push_back(CCalibrationSet::literal(eis.readString()));
			
	

		
	

	}
	
}
void ScanRow::calPatternFromBin(EndianIStream& eis) {
		
	calPatternExists = eis.readBoolean();
	if (calPatternExists) {
		
	
	
		
			
	
		calPattern.clear();
		
		unsigned int calPatternDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < calPatternDim1; i++)
			
			calPattern.push_back(CAntennaMotionPattern::literal(eis.readString()));
			
	

		
	

	}
	
}
void ScanRow::numFieldFromBin(EndianIStream& eis) {
		
	numFieldExists = eis.readBoolean();
	if (numFieldExists) {
		
	
	
		
			
		numField =  eis.readInt();
			
		
	

	}
	
}
void ScanRow::fieldNameFromBin(EndianIStream& eis) {
		
	fieldNameExists = eis.readBoolean();
	if (fieldNameExists) {
		
	
	
		
			
	
		fieldName.clear();
		
		unsigned int fieldNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < fieldNameDim1; i++)
			
			fieldName.push_back(eis.readString());
			
	

		
	

	}
	
}
void ScanRow::sourceNameFromBin(EndianIStream& eis) {
		
	sourceNameExists = eis.readBoolean();
	if (sourceNameExists) {
		
	
	
		
			
		sourceName =  eis.readString();
			
		
	

	}
	
}
	
	
	ScanRow* ScanRow::fromBin(EndianIStream& eis, ScanTable& table, const vector<string>& attributesSeq) {
		ScanRow* row = new  ScanRow(table);
		
		map<string, ScanAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "ScanTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void ScanRow::execBlockIdFromText(const string & s) {
		 
          
		execBlockId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void ScanRow::scanNumberFromText(const string & s) {
		 
          
		scanNumber = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void ScanRow::startTimeFromText(const string & s) {
		 
          
		startTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void ScanRow::endTimeFromText(const string & s) {
		 
          
		endTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void ScanRow::numIntentFromText(const string & s) {
		 
          
		numIntent = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void ScanRow::numSubscanFromText(const string & s) {
		 
          
		numSubscan = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an ScanIntent 
	void ScanRow::scanIntentFromText(const string & s) {
		 
          
		scanIntent = ASDMValuesParser::parse1D<ScanIntentMod::ScanIntent>(s);
          
		
	}
	
	
	// Convert a string into an CalDataOrigin 
	void ScanRow::calDataTypeFromText(const string & s) {
		 
          
		calDataType = ASDMValuesParser::parse1D<CalDataOriginMod::CalDataOrigin>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void ScanRow::calibrationOnLineFromText(const string & s) {
		 
          
		calibrationOnLine = ASDMValuesParser::parse1D<bool>(s);
          
		
	}
	

	
	// Convert a string into an CalibrationFunction 
	void ScanRow::calibrationFunctionFromText(const string & s) {
		calibrationFunctionExists = true;
		 
          
		calibrationFunction = ASDMValuesParser::parse1D<CalibrationFunctionMod::CalibrationFunction>(s);
          
		
	}
	
	
	// Convert a string into an CalibrationSet 
	void ScanRow::calibrationSetFromText(const string & s) {
		calibrationSetExists = true;
		 
          
		calibrationSet = ASDMValuesParser::parse1D<CalibrationSetMod::CalibrationSet>(s);
          
		
	}
	
	
	// Convert a string into an AntennaMotionPattern 
	void ScanRow::calPatternFromText(const string & s) {
		calPatternExists = true;
		 
          
		calPattern = ASDMValuesParser::parse1D<AntennaMotionPatternMod::AntennaMotionPattern>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void ScanRow::numFieldFromText(const string & s) {
		numFieldExists = true;
		 
          
		numField = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void ScanRow::fieldNameFromText(const string & s) {
		fieldNameExists = true;
		 
          
		fieldName = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void ScanRow::sourceNameFromText(const string & s) {
		sourceNameExists = true;
		 
          
		sourceName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	
	void ScanRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, ScanAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "ScanTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get scanNumber.
 	 * @return scanNumber as int
 	 */
 	int ScanRow::getScanNumber() const {
	
  		return scanNumber;
 	}

 	/**
 	 * Set scanNumber with the specified int.
 	 * @param scanNumber The int value to which scanNumber is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void ScanRow::setScanNumber (int scanNumber)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("scanNumber", "Scan");
		
  		}
  	
 		this->scanNumber = scanNumber;
	
 	}
	
	

	

	
 	/**
 	 * Get startTime.
 	 * @return startTime as ArrayTime
 	 */
 	ArrayTime ScanRow::getStartTime() const {
	
  		return startTime;
 	}

 	/**
 	 * Set startTime with the specified ArrayTime.
 	 * @param startTime The ArrayTime value to which startTime is to be set.
 	 
 	
 		
 	 */
 	void ScanRow::setStartTime (ArrayTime startTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startTime = startTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endTime.
 	 * @return endTime as ArrayTime
 	 */
 	ArrayTime ScanRow::getEndTime() const {
	
  		return endTime;
 	}

 	/**
 	 * Set endTime with the specified ArrayTime.
 	 * @param endTime The ArrayTime value to which endTime is to be set.
 	 
 	
 		
 	 */
 	void ScanRow::setEndTime (ArrayTime endTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endTime = endTime;
	
 	}
	
	

	

	
 	/**
 	 * Get numIntent.
 	 * @return numIntent as int
 	 */
 	int ScanRow::getNumIntent() const {
	
  		return numIntent;
 	}

 	/**
 	 * Set numIntent with the specified int.
 	 * @param numIntent The int value to which numIntent is to be set.
 	 
 	
 		
 	 */
 	void ScanRow::setNumIntent (int numIntent)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numIntent = numIntent;
	
 	}
	
	

	

	
 	/**
 	 * Get numSubscan.
 	 * @return numSubscan as int
 	 */
 	int ScanRow::getNumSubscan() const {
	
  		return numSubscan;
 	}

 	/**
 	 * Set numSubscan with the specified int.
 	 * @param numSubscan The int value to which numSubscan is to be set.
 	 
 	
 		
 	 */
 	void ScanRow::setNumSubscan (int numSubscan)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numSubscan = numSubscan;
	
 	}
	
	

	

	
 	/**
 	 * Get scanIntent.
 	 * @return scanIntent as std::vector<ScanIntentMod::ScanIntent >
 	 */
 	std::vector<ScanIntentMod::ScanIntent > ScanRow::getScanIntent() const {
	
  		return scanIntent;
 	}

 	/**
 	 * Set scanIntent with the specified std::vector<ScanIntentMod::ScanIntent >.
 	 * @param scanIntent The std::vector<ScanIntentMod::ScanIntent > value to which scanIntent is to be set.
 	 
 	
 		
 	 */
 	void ScanRow::setScanIntent (std::vector<ScanIntentMod::ScanIntent > scanIntent)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->scanIntent = scanIntent;
	
 	}
	
	

	

	
 	/**
 	 * Get calDataType.
 	 * @return calDataType as std::vector<CalDataOriginMod::CalDataOrigin >
 	 */
 	std::vector<CalDataOriginMod::CalDataOrigin > ScanRow::getCalDataType() const {
	
  		return calDataType;
 	}

 	/**
 	 * Set calDataType with the specified std::vector<CalDataOriginMod::CalDataOrigin >.
 	 * @param calDataType The std::vector<CalDataOriginMod::CalDataOrigin > value to which calDataType is to be set.
 	 
 	
 		
 	 */
 	void ScanRow::setCalDataType (std::vector<CalDataOriginMod::CalDataOrigin > calDataType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->calDataType = calDataType;
	
 	}
	
	

	

	
 	/**
 	 * Get calibrationOnLine.
 	 * @return calibrationOnLine as std::vector<bool >
 	 */
 	std::vector<bool > ScanRow::getCalibrationOnLine() const {
	
  		return calibrationOnLine;
 	}

 	/**
 	 * Set calibrationOnLine with the specified std::vector<bool >.
 	 * @param calibrationOnLine The std::vector<bool > value to which calibrationOnLine is to be set.
 	 
 	
 		
 	 */
 	void ScanRow::setCalibrationOnLine (std::vector<bool > calibrationOnLine)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->calibrationOnLine = calibrationOnLine;
	
 	}
	
	

	
	/**
	 * The attribute calibrationFunction is optional. Return true if this attribute exists.
	 * @return true if and only if the calibrationFunction attribute exists. 
	 */
	bool ScanRow::isCalibrationFunctionExists() const {
		return calibrationFunctionExists;
	}
	

	
 	/**
 	 * Get calibrationFunction, which is optional.
 	 * @return calibrationFunction as std::vector<CalibrationFunctionMod::CalibrationFunction >
 	 * @throw IllegalAccessException If calibrationFunction does not exist.
 	 */
 	std::vector<CalibrationFunctionMod::CalibrationFunction > ScanRow::getCalibrationFunction() const  {
		if (!calibrationFunctionExists) {
			throw IllegalAccessException("calibrationFunction", "Scan");
		}
	
  		return calibrationFunction;
 	}

 	/**
 	 * Set calibrationFunction with the specified std::vector<CalibrationFunctionMod::CalibrationFunction >.
 	 * @param calibrationFunction The std::vector<CalibrationFunctionMod::CalibrationFunction > value to which calibrationFunction is to be set.
 	 
 	
 	 */
 	void ScanRow::setCalibrationFunction (std::vector<CalibrationFunctionMod::CalibrationFunction > calibrationFunction) {
	
 		this->calibrationFunction = calibrationFunction;
	
		calibrationFunctionExists = true;
	
 	}
	
	
	/**
	 * Mark calibrationFunction, which is an optional field, as non-existent.
	 */
	void ScanRow::clearCalibrationFunction () {
		calibrationFunctionExists = false;
	}
	

	
	/**
	 * The attribute calibrationSet is optional. Return true if this attribute exists.
	 * @return true if and only if the calibrationSet attribute exists. 
	 */
	bool ScanRow::isCalibrationSetExists() const {
		return calibrationSetExists;
	}
	

	
 	/**
 	 * Get calibrationSet, which is optional.
 	 * @return calibrationSet as std::vector<CalibrationSetMod::CalibrationSet >
 	 * @throw IllegalAccessException If calibrationSet does not exist.
 	 */
 	std::vector<CalibrationSetMod::CalibrationSet > ScanRow::getCalibrationSet() const  {
		if (!calibrationSetExists) {
			throw IllegalAccessException("calibrationSet", "Scan");
		}
	
  		return calibrationSet;
 	}

 	/**
 	 * Set calibrationSet with the specified std::vector<CalibrationSetMod::CalibrationSet >.
 	 * @param calibrationSet The std::vector<CalibrationSetMod::CalibrationSet > value to which calibrationSet is to be set.
 	 
 	
 	 */
 	void ScanRow::setCalibrationSet (std::vector<CalibrationSetMod::CalibrationSet > calibrationSet) {
	
 		this->calibrationSet = calibrationSet;
	
		calibrationSetExists = true;
	
 	}
	
	
	/**
	 * Mark calibrationSet, which is an optional field, as non-existent.
	 */
	void ScanRow::clearCalibrationSet () {
		calibrationSetExists = false;
	}
	

	
	/**
	 * The attribute calPattern is optional. Return true if this attribute exists.
	 * @return true if and only if the calPattern attribute exists. 
	 */
	bool ScanRow::isCalPatternExists() const {
		return calPatternExists;
	}
	

	
 	/**
 	 * Get calPattern, which is optional.
 	 * @return calPattern as std::vector<AntennaMotionPatternMod::AntennaMotionPattern >
 	 * @throw IllegalAccessException If calPattern does not exist.
 	 */
 	std::vector<AntennaMotionPatternMod::AntennaMotionPattern > ScanRow::getCalPattern() const  {
		if (!calPatternExists) {
			throw IllegalAccessException("calPattern", "Scan");
		}
	
  		return calPattern;
 	}

 	/**
 	 * Set calPattern with the specified std::vector<AntennaMotionPatternMod::AntennaMotionPattern >.
 	 * @param calPattern The std::vector<AntennaMotionPatternMod::AntennaMotionPattern > value to which calPattern is to be set.
 	 
 	
 	 */
 	void ScanRow::setCalPattern (std::vector<AntennaMotionPatternMod::AntennaMotionPattern > calPattern) {
	
 		this->calPattern = calPattern;
	
		calPatternExists = true;
	
 	}
	
	
	/**
	 * Mark calPattern, which is an optional field, as non-existent.
	 */
	void ScanRow::clearCalPattern () {
		calPatternExists = false;
	}
	

	
	/**
	 * The attribute numField is optional. Return true if this attribute exists.
	 * @return true if and only if the numField attribute exists. 
	 */
	bool ScanRow::isNumFieldExists() const {
		return numFieldExists;
	}
	

	
 	/**
 	 * Get numField, which is optional.
 	 * @return numField as int
 	 * @throw IllegalAccessException If numField does not exist.
 	 */
 	int ScanRow::getNumField() const  {
		if (!numFieldExists) {
			throw IllegalAccessException("numField", "Scan");
		}
	
  		return numField;
 	}

 	/**
 	 * Set numField with the specified int.
 	 * @param numField The int value to which numField is to be set.
 	 
 	
 	 */
 	void ScanRow::setNumField (int numField) {
	
 		this->numField = numField;
	
		numFieldExists = true;
	
 	}
	
	
	/**
	 * Mark numField, which is an optional field, as non-existent.
	 */
	void ScanRow::clearNumField () {
		numFieldExists = false;
	}
	

	
	/**
	 * The attribute fieldName is optional. Return true if this attribute exists.
	 * @return true if and only if the fieldName attribute exists. 
	 */
	bool ScanRow::isFieldNameExists() const {
		return fieldNameExists;
	}
	

	
 	/**
 	 * Get fieldName, which is optional.
 	 * @return fieldName as std::vector<std::string >
 	 * @throw IllegalAccessException If fieldName does not exist.
 	 */
 	std::vector<std::string > ScanRow::getFieldName() const  {
		if (!fieldNameExists) {
			throw IllegalAccessException("fieldName", "Scan");
		}
	
  		return fieldName;
 	}

 	/**
 	 * Set fieldName with the specified std::vector<std::string >.
 	 * @param fieldName The std::vector<std::string > value to which fieldName is to be set.
 	 
 	
 	 */
 	void ScanRow::setFieldName (std::vector<std::string > fieldName) {
	
 		this->fieldName = fieldName;
	
		fieldNameExists = true;
	
 	}
	
	
	/**
	 * Mark fieldName, which is an optional field, as non-existent.
	 */
	void ScanRow::clearFieldName () {
		fieldNameExists = false;
	}
	

	
	/**
	 * The attribute sourceName is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceName attribute exists. 
	 */
	bool ScanRow::isSourceNameExists() const {
		return sourceNameExists;
	}
	

	
 	/**
 	 * Get sourceName, which is optional.
 	 * @return sourceName as std::string
 	 * @throw IllegalAccessException If sourceName does not exist.
 	 */
 	std::string ScanRow::getSourceName() const  {
		if (!sourceNameExists) {
			throw IllegalAccessException("sourceName", "Scan");
		}
	
  		return sourceName;
 	}

 	/**
 	 * Set sourceName with the specified std::string.
 	 * @param sourceName The std::string value to which sourceName is to be set.
 	 
 	
 	 */
 	void ScanRow::setSourceName (std::string sourceName) {
	
 		this->sourceName = sourceName;
	
		sourceNameExists = true;
	
 	}
	
	
	/**
	 * Mark sourceName, which is an optional field, as non-existent.
	 */
	void ScanRow::clearSourceName () {
		sourceNameExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag ScanRow::getExecBlockId() const {
	
  		return execBlockId;
 	}

 	/**
 	 * Set execBlockId with the specified Tag.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void ScanRow::setExecBlockId (Tag execBlockId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("execBlockId", "Scan");
		
  		}
  	
 		this->execBlockId = execBlockId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the ExecBlock table having ExecBlock.execBlockId == execBlockId
	 * @return a ExecBlockRow*
	 * 
	 
	 */
	 ExecBlockRow* ScanRow::getExecBlockUsingExecBlockId() {
	 
	 	return table.getContainer().getExecBlock().getRowByKey(execBlockId);
	 }
	 

	

	
	/**
	 * Create a ScanRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	ScanRow::ScanRow (ScanTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	
		calibrationFunctionExists = false;
	

	
		calibrationSetExists = false;
	

	
		calPatternExists = false;
	

	
		numFieldExists = false;
	

	
		fieldNameExists = false;
	

	
		sourceNameExists = false;
	

	
	

	
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["execBlockId"] = &ScanRow::execBlockIdFromBin; 
	 fromBinMethods["scanNumber"] = &ScanRow::scanNumberFromBin; 
	 fromBinMethods["startTime"] = &ScanRow::startTimeFromBin; 
	 fromBinMethods["endTime"] = &ScanRow::endTimeFromBin; 
	 fromBinMethods["numIntent"] = &ScanRow::numIntentFromBin; 
	 fromBinMethods["numSubscan"] = &ScanRow::numSubscanFromBin; 
	 fromBinMethods["scanIntent"] = &ScanRow::scanIntentFromBin; 
	 fromBinMethods["calDataType"] = &ScanRow::calDataTypeFromBin; 
	 fromBinMethods["calibrationOnLine"] = &ScanRow::calibrationOnLineFromBin; 
		
	
	 fromBinMethods["calibrationFunction"] = &ScanRow::calibrationFunctionFromBin; 
	 fromBinMethods["calibrationSet"] = &ScanRow::calibrationSetFromBin; 
	 fromBinMethods["calPattern"] = &ScanRow::calPatternFromBin; 
	 fromBinMethods["numField"] = &ScanRow::numFieldFromBin; 
	 fromBinMethods["fieldName"] = &ScanRow::fieldNameFromBin; 
	 fromBinMethods["sourceName"] = &ScanRow::sourceNameFromBin; 
	
	
	
	
				 
	fromTextMethods["execBlockId"] = &ScanRow::execBlockIdFromText;
		 
	
				 
	fromTextMethods["scanNumber"] = &ScanRow::scanNumberFromText;
		 
	
				 
	fromTextMethods["startTime"] = &ScanRow::startTimeFromText;
		 
	
				 
	fromTextMethods["endTime"] = &ScanRow::endTimeFromText;
		 
	
				 
	fromTextMethods["numIntent"] = &ScanRow::numIntentFromText;
		 
	
				 
	fromTextMethods["numSubscan"] = &ScanRow::numSubscanFromText;
		 
	
				 
	fromTextMethods["scanIntent"] = &ScanRow::scanIntentFromText;
		 
	
				 
	fromTextMethods["calDataType"] = &ScanRow::calDataTypeFromText;
		 
	
				 
	fromTextMethods["calibrationOnLine"] = &ScanRow::calibrationOnLineFromText;
		 
	

	 
				
	fromTextMethods["calibrationFunction"] = &ScanRow::calibrationFunctionFromText;
		 	
	 
				
	fromTextMethods["calibrationSet"] = &ScanRow::calibrationSetFromText;
		 	
	 
				
	fromTextMethods["calPattern"] = &ScanRow::calPatternFromText;
		 	
	 
				
	fromTextMethods["numField"] = &ScanRow::numFieldFromText;
		 	
	 
				
	fromTextMethods["fieldName"] = &ScanRow::fieldNameFromText;
		 	
	 
				
	fromTextMethods["sourceName"] = &ScanRow::sourceNameFromText;
		 	
		
	}
	
	ScanRow::ScanRow (ScanTable &t, ScanRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	
		calibrationFunctionExists = false;
	

	
		calibrationSetExists = false;
	

	
		calPatternExists = false;
	

	
		numFieldExists = false;
	

	
		fieldNameExists = false;
	

	
		sourceNameExists = false;
	

	
	
		
		}
		else {
	
		
			execBlockId = row->execBlockId;
		
			scanNumber = row->scanNumber;
		
		
		
		
			startTime = row->startTime;
		
			endTime = row->endTime;
		
			numIntent = row->numIntent;
		
			numSubscan = row->numSubscan;
		
			scanIntent = row->scanIntent;
		
			calDataType = row->calDataType;
		
			calibrationOnLine = row->calibrationOnLine;
		
		
		
		
		if (row->calibrationFunctionExists) {
			calibrationFunction = row->calibrationFunction;		
			calibrationFunctionExists = true;
		}
		else
			calibrationFunctionExists = false;
		
		if (row->calibrationSetExists) {
			calibrationSet = row->calibrationSet;		
			calibrationSetExists = true;
		}
		else
			calibrationSetExists = false;
		
		if (row->calPatternExists) {
			calPattern = row->calPattern;		
			calPatternExists = true;
		}
		else
			calPatternExists = false;
		
		if (row->numFieldExists) {
			numField = row->numField;		
			numFieldExists = true;
		}
		else
			numFieldExists = false;
		
		if (row->fieldNameExists) {
			fieldName = row->fieldName;		
			fieldNameExists = true;
		}
		else
			fieldNameExists = false;
		
		if (row->sourceNameExists) {
			sourceName = row->sourceName;		
			sourceNameExists = true;
		}
		else
			sourceNameExists = false;
		
		}
		
		 fromBinMethods["execBlockId"] = &ScanRow::execBlockIdFromBin; 
		 fromBinMethods["scanNumber"] = &ScanRow::scanNumberFromBin; 
		 fromBinMethods["startTime"] = &ScanRow::startTimeFromBin; 
		 fromBinMethods["endTime"] = &ScanRow::endTimeFromBin; 
		 fromBinMethods["numIntent"] = &ScanRow::numIntentFromBin; 
		 fromBinMethods["numSubscan"] = &ScanRow::numSubscanFromBin; 
		 fromBinMethods["scanIntent"] = &ScanRow::scanIntentFromBin; 
		 fromBinMethods["calDataType"] = &ScanRow::calDataTypeFromBin; 
		 fromBinMethods["calibrationOnLine"] = &ScanRow::calibrationOnLineFromBin; 
			
	
		 fromBinMethods["calibrationFunction"] = &ScanRow::calibrationFunctionFromBin; 
		 fromBinMethods["calibrationSet"] = &ScanRow::calibrationSetFromBin; 
		 fromBinMethods["calPattern"] = &ScanRow::calPatternFromBin; 
		 fromBinMethods["numField"] = &ScanRow::numFieldFromBin; 
		 fromBinMethods["fieldName"] = &ScanRow::fieldNameFromBin; 
		 fromBinMethods["sourceName"] = &ScanRow::sourceNameFromBin; 
			
	}

	
	bool ScanRow::compareNoAutoInc(Tag execBlockId, int scanNumber, ArrayTime startTime, ArrayTime endTime, int numIntent, int numSubscan, std::vector<ScanIntentMod::ScanIntent > scanIntent, std::vector<CalDataOriginMod::CalDataOrigin > calDataType, std::vector<bool > calibrationOnLine) {
		bool result;
		result = true;
		
	
		
		result = result && (this->execBlockId == execBlockId);
		
		if (!result) return false;
	

	
		
		result = result && (this->scanNumber == scanNumber);
		
		if (!result) return false;
	

	
		
		result = result && (this->startTime == startTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endTime == endTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->numIntent == numIntent);
		
		if (!result) return false;
	

	
		
		result = result && (this->numSubscan == numSubscan);
		
		if (!result) return false;
	

	
		
		result = result && (this->scanIntent == scanIntent);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataType == calDataType);
		
		if (!result) return false;
	

	
		
		result = result && (this->calibrationOnLine == calibrationOnLine);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool ScanRow::compareRequiredValue(ArrayTime startTime, ArrayTime endTime, int numIntent, int numSubscan, std::vector<ScanIntentMod::ScanIntent > scanIntent, std::vector<CalDataOriginMod::CalDataOrigin > calDataType, std::vector<bool > calibrationOnLine) {
		bool result;
		result = true;
		
	
		if (!(this->startTime == startTime)) return false;
	

	
		if (!(this->endTime == endTime)) return false;
	

	
		if (!(this->numIntent == numIntent)) return false;
	

	
		if (!(this->numSubscan == numSubscan)) return false;
	

	
		if (!(this->scanIntent == scanIntent)) return false;
	

	
		if (!(this->calDataType == calDataType)) return false;
	

	
		if (!(this->calibrationOnLine == calibrationOnLine)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the ScanRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool ScanRow::equalByRequiredValue(ScanRow*  x ) {
		
			
		if (this->startTime != x->startTime) return false;
			
		if (this->endTime != x->endTime) return false;
			
		if (this->numIntent != x->numIntent) return false;
			
		if (this->numSubscan != x->numSubscan) return false;
			
		if (this->scanIntent != x->scanIntent) return false;
			
		if (this->calDataType != x->calDataType) return false;
			
		if (this->calibrationOnLine != x->calibrationOnLine) return false;
			
		
		return true;
	}	
	
/*
	 map<string, ScanAttributeFromBin> ScanRow::initFromBinMethods() {
		map<string, ScanAttributeFromBin> result;
		
		result["execBlockId"] = &ScanRow::execBlockIdFromBin;
		result["scanNumber"] = &ScanRow::scanNumberFromBin;
		result["startTime"] = &ScanRow::startTimeFromBin;
		result["endTime"] = &ScanRow::endTimeFromBin;
		result["numIntent"] = &ScanRow::numIntentFromBin;
		result["numSubscan"] = &ScanRow::numSubscanFromBin;
		result["scanIntent"] = &ScanRow::scanIntentFromBin;
		result["calDataType"] = &ScanRow::calDataTypeFromBin;
		result["calibrationOnLine"] = &ScanRow::calibrationOnLineFromBin;
		
		
		result["calibrationFunction"] = &ScanRow::calibrationFunctionFromBin;
		result["calibrationSet"] = &ScanRow::calibrationSetFromBin;
		result["calPattern"] = &ScanRow::calPatternFromBin;
		result["numField"] = &ScanRow::numFieldFromBin;
		result["fieldName"] = &ScanRow::fieldNameFromBin;
		result["sourceName"] = &ScanRow::sourceNameFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
