
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
 * File CalDataRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalDataRow.h>
#include <alma/ASDM/CalDataTable.h>
	

using asdm::ASDM;
using asdm::CalDataRow;
using asdm::CalDataTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	CalDataRow::~CalDataRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalDataTable &CalDataRow::getTable() const {
		return table;
	}

	bool CalDataRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalDataRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalDataRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalDataRowIDL struct.
	 */
	CalDataRowIDL *CalDataRow::toIDL() const {
		CalDataRowIDL *x = new CalDataRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->calDataId = calDataId.toIDLTag();
			
		
	

	
  		
		
		
			
		x->startTimeObserved = startTimeObserved.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endTimeObserved = endTimeObserved.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->execBlockUID = execBlockUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
				
		x->calDataType = calDataType;
 				
 			
		
	

	
  		
		
		
			
				
		x->calType = calType;
 				
 			
		
	

	
  		
		
		
			
				
		x->numScan = numScan;
 				
 			
		
	

	
  		
		
		
			
		x->scanSet.length(scanSet.size());
		for (unsigned int i = 0; i < scanSet.size(); ++i) {
			
				
			x->scanSet[i] = scanSet.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->assocCalDataIdExists = assocCalDataIdExists;
		
		
			
		x->assocCalDataId = assocCalDataId.toIDLTag();
			
		
	

	
  		
		
		x->assocCalNatureExists = assocCalNatureExists;
		
		
			
				
		x->assocCalNature = assocCalNature;
 				
 			
		
	

	
  		
		
		x->fieldNameExists = fieldNameExists;
		
		
			
		x->fieldName.length(fieldName.size());
		for (unsigned int i = 0; i < fieldName.size(); ++i) {
			
				
			x->fieldName[i] = CORBA::string_dup(fieldName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x->sourceNameExists = sourceNameExists;
		
		
			
		x->sourceName.length(sourceName.size());
		for (unsigned int i = 0; i < sourceName.size(); ++i) {
			
				
			x->sourceName[i] = CORBA::string_dup(sourceName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x->sourceCodeExists = sourceCodeExists;
		
		
			
		x->sourceCode.length(sourceCode.size());
		for (unsigned int i = 0; i < sourceCode.size(); ++i) {
			
				
			x->sourceCode[i] = CORBA::string_dup(sourceCode.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x->scanIntentExists = scanIntentExists;
		
		
			
		x->scanIntent.length(scanIntent.size());
		for (unsigned int i = 0; i < scanIntent.size(); ++i) {
			
				
			x->scanIntent[i] = scanIntent.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
		
		return x;
	
	}
	
	void CalDataRow::toIDL(asdmIDL::CalDataRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.calDataId = calDataId.toIDLTag();
			
		
	

	
  		
		
		
			
		x.startTimeObserved = startTimeObserved.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endTimeObserved = endTimeObserved.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.execBlockUID = execBlockUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
				
		x.calDataType = calDataType;
 				
 			
		
	

	
  		
		
		
			
				
		x.calType = calType;
 				
 			
		
	

	
  		
		
		
			
				
		x.numScan = numScan;
 				
 			
		
	

	
  		
		
		
			
		x.scanSet.length(scanSet.size());
		for (unsigned int i = 0; i < scanSet.size(); ++i) {
			
				
			x.scanSet[i] = scanSet.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.assocCalDataIdExists = assocCalDataIdExists;
		
		
			
		x.assocCalDataId = assocCalDataId.toIDLTag();
			
		
	

	
  		
		
		x.assocCalNatureExists = assocCalNatureExists;
		
		
			
				
		x.assocCalNature = assocCalNature;
 				
 			
		
	

	
  		
		
		x.fieldNameExists = fieldNameExists;
		
		
			
		x.fieldName.length(fieldName.size());
		for (unsigned int i = 0; i < fieldName.size(); ++i) {
			
				
			x.fieldName[i] = CORBA::string_dup(fieldName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x.sourceNameExists = sourceNameExists;
		
		
			
		x.sourceName.length(sourceName.size());
		for (unsigned int i = 0; i < sourceName.size(); ++i) {
			
				
			x.sourceName[i] = CORBA::string_dup(sourceName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x.sourceCodeExists = sourceCodeExists;
		
		
			
		x.sourceCode.length(sourceCode.size());
		for (unsigned int i = 0; i < sourceCode.size(); ++i) {
			
				
			x.sourceCode[i] = CORBA::string_dup(sourceCode.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x.scanIntentExists = scanIntentExists;
		
		
			
		x.scanIntent.length(scanIntent.size());
		for (unsigned int i = 0; i < scanIntent.size(); ++i) {
			
				
			x.scanIntent[i] = scanIntent.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalDataRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalDataRow::setFromIDL (CalDataRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setStartTimeObserved(ArrayTime (x.startTimeObserved));
			
 		
		
	

	
		
		
			
		setEndTimeObserved(ArrayTime (x.endTimeObserved));
			
 		
		
	

	
		
		
			
		setExecBlockUID(EntityRef (x.execBlockUID));
			
 		
		
	

	
		
		
			
		setCalDataType(x.calDataType);
  			
 		
		
	

	
		
		
			
		setCalType(x.calType);
  			
 		
		
	

	
		
		
			
		setNumScan(x.numScan);
  			
 		
		
	

	
		
		
			
		scanSet .clear();
		for (unsigned int i = 0; i <x.scanSet.length(); ++i) {
			
			scanSet.push_back(x.scanSet[i]);
  			
		}
			
  		
		
	

	
		
		assocCalDataIdExists = x.assocCalDataIdExists;
		if (x.assocCalDataIdExists) {
		
		
			
		setAssocCalDataId(Tag (x.assocCalDataId));
			
 		
		
		}
		
	

	
		
		assocCalNatureExists = x.assocCalNatureExists;
		if (x.assocCalNatureExists) {
		
		
			
		setAssocCalNature(x.assocCalNature);
  			
 		
		
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
		
		
			
		sourceName .clear();
		for (unsigned int i = 0; i <x.sourceName.length(); ++i) {
			
			sourceName.push_back(string (x.sourceName[i]));
			
		}
			
  		
		
		}
		
	

	
		
		sourceCodeExists = x.sourceCodeExists;
		if (x.sourceCodeExists) {
		
		
			
		sourceCode .clear();
		for (unsigned int i = 0; i <x.sourceCode.length(); ++i) {
			
			sourceCode.push_back(string (x.sourceCode[i]));
			
		}
			
  		
		
		}
		
	

	
		
		scanIntentExists = x.scanIntentExists;
		if (x.scanIntentExists) {
		
		
			
		scanIntent .clear();
		for (unsigned int i = 0; i <x.scanIntent.length(); ++i) {
			
			scanIntent.push_back(x.scanIntent[i]);
  			
		}
			
  		
		
		}
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalData");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalDataRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(calDataId, "calDataId", buf);
		
		
	

  	
 		
		
		Parser::toXML(startTimeObserved, "startTimeObserved", buf);
		
		
	

  	
 		
		
		Parser::toXML(endTimeObserved, "endTimeObserved", buf);
		
		
	

  	
 		
		
		Parser::toXML(execBlockUID, "execBlockUID", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("calDataType", calDataType));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("calType", calType));
		
		
	

  	
 		
		
		Parser::toXML(numScan, "numScan", buf);
		
		
	

  	
 		
		
		Parser::toXML(scanSet, "scanSet", buf);
		
		
	

  	
 		
		if (assocCalDataIdExists) {
		
		
		Parser::toXML(assocCalDataId, "assocCalDataId", buf);
		
		
		}
		
	

  	
 		
		if (assocCalNatureExists) {
		
		
			buf.append(EnumerationParser::toXML("assocCalNature", assocCalNature));
		
		
		}
		
	

  	
 		
		if (fieldNameExists) {
		
		
		Parser::toXML(fieldName, "fieldName", buf);
		
		
		}
		
	

  	
 		
		if (sourceNameExists) {
		
		
		Parser::toXML(sourceName, "sourceName", buf);
		
		
		}
		
	

  	
 		
		if (sourceCodeExists) {
		
		
		Parser::toXML(sourceCode, "sourceCode", buf);
		
		
		}
		
	

  	
 		
		if (scanIntentExists) {
		
		
			buf.append(EnumerationParser::toXML("scanIntent", scanIntent));
		
		
		}
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void CalDataRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setStartTimeObserved(Parser::getArrayTime("startTimeObserved","CalData",rowDoc));
			
		
	

	
  		
			
	  	setEndTimeObserved(Parser::getArrayTime("endTimeObserved","CalData",rowDoc));
			
		
	

	
  		
			
	  	setExecBlockUID(Parser::getEntityRef("execBlockUID","CalData",rowDoc));
			
		
	

	
		
		
		
		calDataType = EnumerationParser::getCalDataOrigin("calDataType","CalData",rowDoc);
		
		
		
	

	
		
		
		
		calType = EnumerationParser::getCalType("calType","CalData",rowDoc);
		
		
		
	

	
  		
			
	  	setNumScan(Parser::getInteger("numScan","CalData",rowDoc));
			
		
	

	
  		
			
					
	  	setScanSet(Parser::get1DInteger("scanSet","CalData",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<assocCalDataId>")) {
			
	  		setAssocCalDataId(Parser::getTag("assocCalDataId","CalData",rowDoc));
			
		}
 		
	

	
		
	if (row.isStr("<assocCalNature>")) {
		
		
		
		assocCalNature = EnumerationParser::getAssociatedCalNature("assocCalNature","CalData",rowDoc);
		
		
		
		assocCalNatureExists = true;
	}
		
	

	
  		
        if (row.isStr("<fieldName>")) {
			
								
	  		setFieldName(Parser::get1DString("fieldName","CalData",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<sourceName>")) {
			
								
	  		setSourceName(Parser::get1DString("sourceName","CalData",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<sourceCode>")) {
			
								
	  		setSourceCode(Parser::get1DString("sourceCode","CalData",rowDoc));
	  			
	  		
		}
 		
	

	
		
	if (row.isStr("<scanIntent>")) {
		
		
		
		scanIntent = EnumerationParser::getScanIntent1D("scanIntent","CalData",rowDoc);			
		
		
		
		scanIntentExists = true;
	}
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalData");
		}
	}
	
	void CalDataRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	startTimeObserved.toBin(eoss);
		
	

	
	
		
	endTimeObserved.toBin(eoss);
		
	

	
	
		
	execBlockUID.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CCalDataOrigin::name(calDataType));
			/* eoss.writeInt(calDataType); */
				
		
	

	
	
		
					
		eoss.writeString(CCalType::name(calType));
			/* eoss.writeInt(calType); */
				
		
	

	
	
		
						
			eoss.writeInt(numScan);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) scanSet.size());
		for (unsigned int i = 0; i < scanSet.size(); i++)
				
			eoss.writeInt(scanSet.at(i));
				
				
						
		
	


	
	
	eoss.writeBoolean(assocCalDataIdExists);
	if (assocCalDataIdExists) {
	
	
	
		
	assocCalDataId.toBin(eoss);
		
	

	}

	eoss.writeBoolean(assocCalNatureExists);
	if (assocCalNatureExists) {
	
	
	
		
					
		eoss.writeString(CAssociatedCalNature::name(assocCalNature));
			/* eoss.writeInt(assocCalNature); */
				
		
	

	}

	eoss.writeBoolean(fieldNameExists);
	if (fieldNameExists) {
	
	
	
		
		
			
		eoss.writeInt((int) fieldName.size());
		for (unsigned int i = 0; i < fieldName.size(); i++)
				
			eoss.writeString(fieldName.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(sourceNameExists);
	if (sourceNameExists) {
	
	
	
		
		
			
		eoss.writeInt((int) sourceName.size());
		for (unsigned int i = 0; i < sourceName.size(); i++)
				
			eoss.writeString(sourceName.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(sourceCodeExists);
	if (sourceCodeExists) {
	
	
	
		
		
			
		eoss.writeInt((int) sourceCode.size());
		for (unsigned int i = 0; i < sourceCode.size(); i++)
				
			eoss.writeString(sourceCode.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(scanIntentExists);
	if (scanIntentExists) {
	
	
	
		
		
			
		eoss.writeInt((int) scanIntent.size());
		for (unsigned int i = 0; i < scanIntent.size(); i++)
				
			eoss.writeString(CScanIntent::name(scanIntent.at(i)));
			/* eoss.writeInt(scanIntent.at(i)); */
				
				
						
		
	

	}

	}
	
void CalDataRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalDataRow::startTimeObservedFromBin(EndianIStream& eis) {
		
	
		
		
		startTimeObserved =  ArrayTime::fromBin(eis);
		
	
	
}
void CalDataRow::endTimeObservedFromBin(EndianIStream& eis) {
		
	
		
		
		endTimeObserved =  ArrayTime::fromBin(eis);
		
	
	
}
void CalDataRow::execBlockUIDFromBin(EndianIStream& eis) {
		
	
		
		
		execBlockUID =  EntityRef::fromBin(eis);
		
	
	
}
void CalDataRow::calDataTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		calDataType = CCalDataOrigin::literal(eis.readString());
			
		
	
	
}
void CalDataRow::calTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		calType = CCalType::literal(eis.readString());
			
		
	
	
}
void CalDataRow::numScanFromBin(EndianIStream& eis) {
		
	
	
		
			
		numScan =  eis.readInt();
			
		
	
	
}
void CalDataRow::scanSetFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		scanSet.clear();
		
		unsigned int scanSetDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < scanSetDim1; i++)
			
			scanSet.push_back(eis.readInt());
			
	

		
	
	
}

void CalDataRow::assocCalDataIdFromBin(EndianIStream& eis) {
		
	assocCalDataIdExists = eis.readBoolean();
	if (assocCalDataIdExists) {
		
	
		
		
		assocCalDataId =  Tag::fromBin(eis);
		
	

	}
	
}
void CalDataRow::assocCalNatureFromBin(EndianIStream& eis) {
		
	assocCalNatureExists = eis.readBoolean();
	if (assocCalNatureExists) {
		
	
	
		
			
		assocCalNature = CAssociatedCalNature::literal(eis.readString());
			
		
	

	}
	
}
void CalDataRow::fieldNameFromBin(EndianIStream& eis) {
		
	fieldNameExists = eis.readBoolean();
	if (fieldNameExists) {
		
	
	
		
			
	
		fieldName.clear();
		
		unsigned int fieldNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < fieldNameDim1; i++)
			
			fieldName.push_back(eis.readString());
			
	

		
	

	}
	
}
void CalDataRow::sourceNameFromBin(EndianIStream& eis) {
		
	sourceNameExists = eis.readBoolean();
	if (sourceNameExists) {
		
	
	
		
			
	
		sourceName.clear();
		
		unsigned int sourceNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < sourceNameDim1; i++)
			
			sourceName.push_back(eis.readString());
			
	

		
	

	}
	
}
void CalDataRow::sourceCodeFromBin(EndianIStream& eis) {
		
	sourceCodeExists = eis.readBoolean();
	if (sourceCodeExists) {
		
	
	
		
			
	
		sourceCode.clear();
		
		unsigned int sourceCodeDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < sourceCodeDim1; i++)
			
			sourceCode.push_back(eis.readString());
			
	

		
	

	}
	
}
void CalDataRow::scanIntentFromBin(EndianIStream& eis) {
		
	scanIntentExists = eis.readBoolean();
	if (scanIntentExists) {
		
	
	
		
			
	
		scanIntent.clear();
		
		unsigned int scanIntentDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < scanIntentDim1; i++)
			
			scanIntent.push_back(CScanIntent::literal(eis.readString()));
			
	

		
	

	}
	
}
	
	
	CalDataRow* CalDataRow::fromBin(EndianIStream& eis, CalDataTable& table, const vector<string>& attributesSeq) {
		CalDataRow* row = new  CalDataRow(table);
		
		map<string, CalDataAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalDataTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void CalDataRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalDataRow::startTimeObservedFromText(const string & s) {
		 
          
		startTimeObserved = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalDataRow::endTimeObservedFromText(const string & s) {
		 
          
		endTimeObserved = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	
	// Convert a string into an CalDataOrigin 
	void CalDataRow::calDataTypeFromText(const string & s) {
		 
          
		calDataType = ASDMValuesParser::parse<CalDataOriginMod::CalDataOrigin>(s);
          
		
	}
	
	
	// Convert a string into an CalType 
	void CalDataRow::calTypeFromText(const string & s) {
		 
          
		calType = ASDMValuesParser::parse<CalTypeMod::CalType>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalDataRow::numScanFromText(const string & s) {
		 
          
		numScan = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalDataRow::scanSetFromText(const string & s) {
		 
          
		scanSet = ASDMValuesParser::parse1D<int>(s);
          
		
	}
	

	
	// Convert a string into an Tag 
	void CalDataRow::assocCalDataIdFromText(const string & s) {
		assocCalDataIdExists = true;
		 
          
		assocCalDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an AssociatedCalNature 
	void CalDataRow::assocCalNatureFromText(const string & s) {
		assocCalNatureExists = true;
		 
          
		assocCalNature = ASDMValuesParser::parse<AssociatedCalNatureMod::AssociatedCalNature>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalDataRow::fieldNameFromText(const string & s) {
		fieldNameExists = true;
		 
          
		fieldName = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalDataRow::sourceNameFromText(const string & s) {
		sourceNameExists = true;
		 
          
		sourceName = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalDataRow::sourceCodeFromText(const string & s) {
		sourceCodeExists = true;
		 
          
		sourceCode = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an ScanIntent 
	void CalDataRow::scanIntentFromText(const string & s) {
		scanIntentExists = true;
		 
          
		scanIntent = ASDMValuesParser::parse1D<ScanIntentMod::ScanIntent>(s);
          
		
	}
	
	
	
	void CalDataRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalDataAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalDataTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalDataRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalDataRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalData");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get startTimeObserved.
 	 * @return startTimeObserved as ArrayTime
 	 */
 	ArrayTime CalDataRow::getStartTimeObserved() const {
	
  		return startTimeObserved;
 	}

 	/**
 	 * Set startTimeObserved with the specified ArrayTime.
 	 * @param startTimeObserved The ArrayTime value to which startTimeObserved is to be set.
 	 
 	
 		
 	 */
 	void CalDataRow::setStartTimeObserved (ArrayTime startTimeObserved)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startTimeObserved = startTimeObserved;
	
 	}
	
	

	

	
 	/**
 	 * Get endTimeObserved.
 	 * @return endTimeObserved as ArrayTime
 	 */
 	ArrayTime CalDataRow::getEndTimeObserved() const {
	
  		return endTimeObserved;
 	}

 	/**
 	 * Set endTimeObserved with the specified ArrayTime.
 	 * @param endTimeObserved The ArrayTime value to which endTimeObserved is to be set.
 	 
 	
 		
 	 */
 	void CalDataRow::setEndTimeObserved (ArrayTime endTimeObserved)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endTimeObserved = endTimeObserved;
	
 	}
	
	

	

	
 	/**
 	 * Get execBlockUID.
 	 * @return execBlockUID as EntityRef
 	 */
 	EntityRef CalDataRow::getExecBlockUID() const {
	
  		return execBlockUID;
 	}

 	/**
 	 * Set execBlockUID with the specified EntityRef.
 	 * @param execBlockUID The EntityRef value to which execBlockUID is to be set.
 	 
 	
 		
 	 */
 	void CalDataRow::setExecBlockUID (EntityRef execBlockUID)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->execBlockUID = execBlockUID;
	
 	}
	
	

	

	
 	/**
 	 * Get calDataType.
 	 * @return calDataType as CalDataOriginMod::CalDataOrigin
 	 */
 	CalDataOriginMod::CalDataOrigin CalDataRow::getCalDataType() const {
	
  		return calDataType;
 	}

 	/**
 	 * Set calDataType with the specified CalDataOriginMod::CalDataOrigin.
 	 * @param calDataType The CalDataOriginMod::CalDataOrigin value to which calDataType is to be set.
 	 
 	
 		
 	 */
 	void CalDataRow::setCalDataType (CalDataOriginMod::CalDataOrigin calDataType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->calDataType = calDataType;
	
 	}
	
	

	

	
 	/**
 	 * Get calType.
 	 * @return calType as CalTypeMod::CalType
 	 */
 	CalTypeMod::CalType CalDataRow::getCalType() const {
	
  		return calType;
 	}

 	/**
 	 * Set calType with the specified CalTypeMod::CalType.
 	 * @param calType The CalTypeMod::CalType value to which calType is to be set.
 	 
 	
 		
 	 */
 	void CalDataRow::setCalType (CalTypeMod::CalType calType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->calType = calType;
	
 	}
	
	

	

	
 	/**
 	 * Get numScan.
 	 * @return numScan as int
 	 */
 	int CalDataRow::getNumScan() const {
	
  		return numScan;
 	}

 	/**
 	 * Set numScan with the specified int.
 	 * @param numScan The int value to which numScan is to be set.
 	 
 	
 		
 	 */
 	void CalDataRow::setNumScan (int numScan)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numScan = numScan;
	
 	}
	
	

	

	
 	/**
 	 * Get scanSet.
 	 * @return scanSet as std::vector<int >
 	 */
 	std::vector<int > CalDataRow::getScanSet() const {
	
  		return scanSet;
 	}

 	/**
 	 * Set scanSet with the specified std::vector<int >.
 	 * @param scanSet The std::vector<int > value to which scanSet is to be set.
 	 
 	
 		
 	 */
 	void CalDataRow::setScanSet (std::vector<int > scanSet)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->scanSet = scanSet;
	
 	}
	
	

	
	/**
	 * The attribute assocCalDataId is optional. Return true if this attribute exists.
	 * @return true if and only if the assocCalDataId attribute exists. 
	 */
	bool CalDataRow::isAssocCalDataIdExists() const {
		return assocCalDataIdExists;
	}
	

	
 	/**
 	 * Get assocCalDataId, which is optional.
 	 * @return assocCalDataId as Tag
 	 * @throw IllegalAccessException If assocCalDataId does not exist.
 	 */
 	Tag CalDataRow::getAssocCalDataId() const  {
		if (!assocCalDataIdExists) {
			throw IllegalAccessException("assocCalDataId", "CalData");
		}
	
  		return assocCalDataId;
 	}

 	/**
 	 * Set assocCalDataId with the specified Tag.
 	 * @param assocCalDataId The Tag value to which assocCalDataId is to be set.
 	 
 	
 	 */
 	void CalDataRow::setAssocCalDataId (Tag assocCalDataId) {
	
 		this->assocCalDataId = assocCalDataId;
	
		assocCalDataIdExists = true;
	
 	}
	
	
	/**
	 * Mark assocCalDataId, which is an optional field, as non-existent.
	 */
	void CalDataRow::clearAssocCalDataId () {
		assocCalDataIdExists = false;
	}
	

	
	/**
	 * The attribute assocCalNature is optional. Return true if this attribute exists.
	 * @return true if and only if the assocCalNature attribute exists. 
	 */
	bool CalDataRow::isAssocCalNatureExists() const {
		return assocCalNatureExists;
	}
	

	
 	/**
 	 * Get assocCalNature, which is optional.
 	 * @return assocCalNature as AssociatedCalNatureMod::AssociatedCalNature
 	 * @throw IllegalAccessException If assocCalNature does not exist.
 	 */
 	AssociatedCalNatureMod::AssociatedCalNature CalDataRow::getAssocCalNature() const  {
		if (!assocCalNatureExists) {
			throw IllegalAccessException("assocCalNature", "CalData");
		}
	
  		return assocCalNature;
 	}

 	/**
 	 * Set assocCalNature with the specified AssociatedCalNatureMod::AssociatedCalNature.
 	 * @param assocCalNature The AssociatedCalNatureMod::AssociatedCalNature value to which assocCalNature is to be set.
 	 
 	
 	 */
 	void CalDataRow::setAssocCalNature (AssociatedCalNatureMod::AssociatedCalNature assocCalNature) {
	
 		this->assocCalNature = assocCalNature;
	
		assocCalNatureExists = true;
	
 	}
	
	
	/**
	 * Mark assocCalNature, which is an optional field, as non-existent.
	 */
	void CalDataRow::clearAssocCalNature () {
		assocCalNatureExists = false;
	}
	

	
	/**
	 * The attribute fieldName is optional. Return true if this attribute exists.
	 * @return true if and only if the fieldName attribute exists. 
	 */
	bool CalDataRow::isFieldNameExists() const {
		return fieldNameExists;
	}
	

	
 	/**
 	 * Get fieldName, which is optional.
 	 * @return fieldName as std::vector<std::string >
 	 * @throw IllegalAccessException If fieldName does not exist.
 	 */
 	std::vector<std::string > CalDataRow::getFieldName() const  {
		if (!fieldNameExists) {
			throw IllegalAccessException("fieldName", "CalData");
		}
	
  		return fieldName;
 	}

 	/**
 	 * Set fieldName with the specified std::vector<std::string >.
 	 * @param fieldName The std::vector<std::string > value to which fieldName is to be set.
 	 
 	
 	 */
 	void CalDataRow::setFieldName (std::vector<std::string > fieldName) {
	
 		this->fieldName = fieldName;
	
		fieldNameExists = true;
	
 	}
	
	
	/**
	 * Mark fieldName, which is an optional field, as non-existent.
	 */
	void CalDataRow::clearFieldName () {
		fieldNameExists = false;
	}
	

	
	/**
	 * The attribute sourceName is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceName attribute exists. 
	 */
	bool CalDataRow::isSourceNameExists() const {
		return sourceNameExists;
	}
	

	
 	/**
 	 * Get sourceName, which is optional.
 	 * @return sourceName as std::vector<std::string >
 	 * @throw IllegalAccessException If sourceName does not exist.
 	 */
 	std::vector<std::string > CalDataRow::getSourceName() const  {
		if (!sourceNameExists) {
			throw IllegalAccessException("sourceName", "CalData");
		}
	
  		return sourceName;
 	}

 	/**
 	 * Set sourceName with the specified std::vector<std::string >.
 	 * @param sourceName The std::vector<std::string > value to which sourceName is to be set.
 	 
 	
 	 */
 	void CalDataRow::setSourceName (std::vector<std::string > sourceName) {
	
 		this->sourceName = sourceName;
	
		sourceNameExists = true;
	
 	}
	
	
	/**
	 * Mark sourceName, which is an optional field, as non-existent.
	 */
	void CalDataRow::clearSourceName () {
		sourceNameExists = false;
	}
	

	
	/**
	 * The attribute sourceCode is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceCode attribute exists. 
	 */
	bool CalDataRow::isSourceCodeExists() const {
		return sourceCodeExists;
	}
	

	
 	/**
 	 * Get sourceCode, which is optional.
 	 * @return sourceCode as std::vector<std::string >
 	 * @throw IllegalAccessException If sourceCode does not exist.
 	 */
 	std::vector<std::string > CalDataRow::getSourceCode() const  {
		if (!sourceCodeExists) {
			throw IllegalAccessException("sourceCode", "CalData");
		}
	
  		return sourceCode;
 	}

 	/**
 	 * Set sourceCode with the specified std::vector<std::string >.
 	 * @param sourceCode The std::vector<std::string > value to which sourceCode is to be set.
 	 
 	
 	 */
 	void CalDataRow::setSourceCode (std::vector<std::string > sourceCode) {
	
 		this->sourceCode = sourceCode;
	
		sourceCodeExists = true;
	
 	}
	
	
	/**
	 * Mark sourceCode, which is an optional field, as non-existent.
	 */
	void CalDataRow::clearSourceCode () {
		sourceCodeExists = false;
	}
	

	
	/**
	 * The attribute scanIntent is optional. Return true if this attribute exists.
	 * @return true if and only if the scanIntent attribute exists. 
	 */
	bool CalDataRow::isScanIntentExists() const {
		return scanIntentExists;
	}
	

	
 	/**
 	 * Get scanIntent, which is optional.
 	 * @return scanIntent as std::vector<ScanIntentMod::ScanIntent >
 	 * @throw IllegalAccessException If scanIntent does not exist.
 	 */
 	std::vector<ScanIntentMod::ScanIntent > CalDataRow::getScanIntent() const  {
		if (!scanIntentExists) {
			throw IllegalAccessException("scanIntent", "CalData");
		}
	
  		return scanIntent;
 	}

 	/**
 	 * Set scanIntent with the specified std::vector<ScanIntentMod::ScanIntent >.
 	 * @param scanIntent The std::vector<ScanIntentMod::ScanIntent > value to which scanIntent is to be set.
 	 
 	
 	 */
 	void CalDataRow::setScanIntent (std::vector<ScanIntentMod::ScanIntent > scanIntent) {
	
 		this->scanIntent = scanIntent;
	
		scanIntentExists = true;
	
 	}
	
	
	/**
	 * Mark scanIntent, which is an optional field, as non-existent.
	 */
	void CalDataRow::clearScanIntent () {
		scanIntentExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a CalDataRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalDataRow::CalDataRow (CalDataTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	
		assocCalDataIdExists = false;
	

	
		assocCalNatureExists = false;
	

	
		fieldNameExists = false;
	

	
		sourceNameExists = false;
	

	
		sourceCodeExists = false;
	

	
		scanIntentExists = false;
	

	
	
	
	
	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
calDataType = CCalDataOrigin::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
calType = CCalType::from_int(0);
	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
assocCalNature = CAssociatedCalNature::from_int(0);
	

	

	

	

	

	
	
	 fromBinMethods["calDataId"] = &CalDataRow::calDataIdFromBin; 
	 fromBinMethods["startTimeObserved"] = &CalDataRow::startTimeObservedFromBin; 
	 fromBinMethods["endTimeObserved"] = &CalDataRow::endTimeObservedFromBin; 
	 fromBinMethods["execBlockUID"] = &CalDataRow::execBlockUIDFromBin; 
	 fromBinMethods["calDataType"] = &CalDataRow::calDataTypeFromBin; 
	 fromBinMethods["calType"] = &CalDataRow::calTypeFromBin; 
	 fromBinMethods["numScan"] = &CalDataRow::numScanFromBin; 
	 fromBinMethods["scanSet"] = &CalDataRow::scanSetFromBin; 
		
	
	 fromBinMethods["assocCalDataId"] = &CalDataRow::assocCalDataIdFromBin; 
	 fromBinMethods["assocCalNature"] = &CalDataRow::assocCalNatureFromBin; 
	 fromBinMethods["fieldName"] = &CalDataRow::fieldNameFromBin; 
	 fromBinMethods["sourceName"] = &CalDataRow::sourceNameFromBin; 
	 fromBinMethods["sourceCode"] = &CalDataRow::sourceCodeFromBin; 
	 fromBinMethods["scanIntent"] = &CalDataRow::scanIntentFromBin; 
	
	
	
	
				 
	fromTextMethods["calDataId"] = &CalDataRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["startTimeObserved"] = &CalDataRow::startTimeObservedFromText;
		 
	
				 
	fromTextMethods["endTimeObserved"] = &CalDataRow::endTimeObservedFromText;
		 
	
		 
	
				 
	fromTextMethods["calDataType"] = &CalDataRow::calDataTypeFromText;
		 
	
				 
	fromTextMethods["calType"] = &CalDataRow::calTypeFromText;
		 
	
				 
	fromTextMethods["numScan"] = &CalDataRow::numScanFromText;
		 
	
				 
	fromTextMethods["scanSet"] = &CalDataRow::scanSetFromText;
		 
	

	 
				
	fromTextMethods["assocCalDataId"] = &CalDataRow::assocCalDataIdFromText;
		 	
	 
				
	fromTextMethods["assocCalNature"] = &CalDataRow::assocCalNatureFromText;
		 	
	 
				
	fromTextMethods["fieldName"] = &CalDataRow::fieldNameFromText;
		 	
	 
				
	fromTextMethods["sourceName"] = &CalDataRow::sourceNameFromText;
		 	
	 
				
	fromTextMethods["sourceCode"] = &CalDataRow::sourceCodeFromText;
		 	
	 
				
	fromTextMethods["scanIntent"] = &CalDataRow::scanIntentFromText;
		 	
		
	}
	
	CalDataRow::CalDataRow (CalDataTable &t, CalDataRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	
		assocCalDataIdExists = false;
	

	
		assocCalNatureExists = false;
	

	
		fieldNameExists = false;
	

	
		sourceNameExists = false;
	

	
		sourceCodeExists = false;
	

	
		scanIntentExists = false;
	

			
		}
		else {
	
		
			calDataId = row->calDataId;
		
		
		
		
			startTimeObserved = row->startTimeObserved;
		
			endTimeObserved = row->endTimeObserved;
		
			execBlockUID = row->execBlockUID;
		
			calDataType = row->calDataType;
		
			calType = row->calType;
		
			numScan = row->numScan;
		
			scanSet = row->scanSet;
		
		
		
		
		if (row->assocCalDataIdExists) {
			assocCalDataId = row->assocCalDataId;		
			assocCalDataIdExists = true;
		}
		else
			assocCalDataIdExists = false;
		
		if (row->assocCalNatureExists) {
			assocCalNature = row->assocCalNature;		
			assocCalNatureExists = true;
		}
		else
			assocCalNatureExists = false;
		
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
		
		if (row->sourceCodeExists) {
			sourceCode = row->sourceCode;		
			sourceCodeExists = true;
		}
		else
			sourceCodeExists = false;
		
		if (row->scanIntentExists) {
			scanIntent = row->scanIntent;		
			scanIntentExists = true;
		}
		else
			scanIntentExists = false;
		
		}
		
		 fromBinMethods["calDataId"] = &CalDataRow::calDataIdFromBin; 
		 fromBinMethods["startTimeObserved"] = &CalDataRow::startTimeObservedFromBin; 
		 fromBinMethods["endTimeObserved"] = &CalDataRow::endTimeObservedFromBin; 
		 fromBinMethods["execBlockUID"] = &CalDataRow::execBlockUIDFromBin; 
		 fromBinMethods["calDataType"] = &CalDataRow::calDataTypeFromBin; 
		 fromBinMethods["calType"] = &CalDataRow::calTypeFromBin; 
		 fromBinMethods["numScan"] = &CalDataRow::numScanFromBin; 
		 fromBinMethods["scanSet"] = &CalDataRow::scanSetFromBin; 
			
	
		 fromBinMethods["assocCalDataId"] = &CalDataRow::assocCalDataIdFromBin; 
		 fromBinMethods["assocCalNature"] = &CalDataRow::assocCalNatureFromBin; 
		 fromBinMethods["fieldName"] = &CalDataRow::fieldNameFromBin; 
		 fromBinMethods["sourceName"] = &CalDataRow::sourceNameFromBin; 
		 fromBinMethods["sourceCode"] = &CalDataRow::sourceCodeFromBin; 
		 fromBinMethods["scanIntent"] = &CalDataRow::scanIntentFromBin; 
			
	}

	
	bool CalDataRow::compareNoAutoInc(ArrayTime startTimeObserved, ArrayTime endTimeObserved, EntityRef execBlockUID, CalDataOriginMod::CalDataOrigin calDataType, CalTypeMod::CalType calType, int numScan, std::vector<int > scanSet) {
		bool result;
		result = true;
		
	
		
		result = result && (this->startTimeObserved == startTimeObserved);
		
		if (!result) return false;
	

	
		
		result = result && (this->endTimeObserved == endTimeObserved);
		
		if (!result) return false;
	

	
		
		result = result && (this->execBlockUID == execBlockUID);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataType == calDataType);
		
		if (!result) return false;
	

	
		
		result = result && (this->calType == calType);
		
		if (!result) return false;
	

	
		
		result = result && (this->numScan == numScan);
		
		if (!result) return false;
	

	
		
		result = result && (this->scanSet == scanSet);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalDataRow::compareRequiredValue(ArrayTime startTimeObserved, ArrayTime endTimeObserved, EntityRef execBlockUID, CalDataOriginMod::CalDataOrigin calDataType, CalTypeMod::CalType calType, int numScan, std::vector<int > scanSet) {
		bool result;
		result = true;
		
	
		if (!(this->startTimeObserved == startTimeObserved)) return false;
	

	
		if (!(this->endTimeObserved == endTimeObserved)) return false;
	

	
		if (!(this->execBlockUID == execBlockUID)) return false;
	

	
		if (!(this->calDataType == calDataType)) return false;
	

	
		if (!(this->calType == calType)) return false;
	

	
		if (!(this->numScan == numScan)) return false;
	

	
		if (!(this->scanSet == scanSet)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalDataRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalDataRow::equalByRequiredValue(CalDataRow*  x ) {
		
			
		if (this->startTimeObserved != x->startTimeObserved) return false;
			
		if (this->endTimeObserved != x->endTimeObserved) return false;
			
		if (this->execBlockUID != x->execBlockUID) return false;
			
		if (this->calDataType != x->calDataType) return false;
			
		if (this->calType != x->calType) return false;
			
		if (this->numScan != x->numScan) return false;
			
		if (this->scanSet != x->scanSet) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalDataAttributeFromBin> CalDataRow::initFromBinMethods() {
		map<string, CalDataAttributeFromBin> result;
		
		result["calDataId"] = &CalDataRow::calDataIdFromBin;
		result["startTimeObserved"] = &CalDataRow::startTimeObservedFromBin;
		result["endTimeObserved"] = &CalDataRow::endTimeObservedFromBin;
		result["execBlockUID"] = &CalDataRow::execBlockUIDFromBin;
		result["calDataType"] = &CalDataRow::calDataTypeFromBin;
		result["calType"] = &CalDataRow::calTypeFromBin;
		result["numScan"] = &CalDataRow::numScanFromBin;
		result["scanSet"] = &CalDataRow::scanSetFromBin;
		
		
		result["assocCalDataId"] = &CalDataRow::assocCalDataIdFromBin;
		result["assocCalNature"] = &CalDataRow::assocCalNatureFromBin;
		result["fieldName"] = &CalDataRow::fieldNameFromBin;
		result["sourceName"] = &CalDataRow::sourceNameFromBin;
		result["sourceCode"] = &CalDataRow::sourceCodeFromBin;
		result["scanIntent"] = &CalDataRow::scanIntentFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
