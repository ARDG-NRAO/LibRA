
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
 * File FieldRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/FieldRow.h>
#include <alma/ASDM/FieldTable.h>

#include <alma/ASDM/EphemerisTable.h>
#include <alma/ASDM/EphemerisRow.h>

#include <alma/ASDM/SourceTable.h>
#include <alma/ASDM/SourceRow.h>

#include <alma/ASDM/FieldTable.h>
#include <alma/ASDM/FieldRow.h>
	

using asdm::ASDM;
using asdm::FieldRow;
using asdm::FieldTable;

using asdm::EphemerisTable;
using asdm::EphemerisRow;

using asdm::SourceTable;
using asdm::SourceRow;

using asdm::FieldTable;
using asdm::FieldRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	FieldRow::~FieldRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	FieldTable &FieldRow::getTable() const {
		return table;
	}

	bool FieldRow::isAdded() const {
		return hasBeenAdded;
	}	

	void FieldRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::FieldRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FieldRowIDL struct.
	 */
	FieldRowIDL *FieldRow::toIDL() const {
		FieldRowIDL *x = new FieldRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->fieldId = fieldId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->fieldName = CORBA::string_dup(fieldName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->numPoly = numPoly;
 				
 			
		
	

	
  		
		
		
			
		x->delayDir.length(delayDir.size());
		for (unsigned int i = 0; i < delayDir.size(); i++) {
			x->delayDir[i].length(delayDir.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < delayDir.size() ; i++)
			for (unsigned int j = 0; j < delayDir.at(i).size(); j++)
					
				x->delayDir[i][j]= delayDir.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		
			
		x->phaseDir.length(phaseDir.size());
		for (unsigned int i = 0; i < phaseDir.size(); i++) {
			x->phaseDir[i].length(phaseDir.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phaseDir.size() ; i++)
			for (unsigned int j = 0; j < phaseDir.at(i).size(); j++)
					
				x->phaseDir[i][j]= phaseDir.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		
			
		x->referenceDir.length(referenceDir.size());
		for (unsigned int i = 0; i < referenceDir.size(); i++) {
			x->referenceDir[i].length(referenceDir.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < referenceDir.size() ; i++)
			for (unsigned int j = 0; j < referenceDir.at(i).size(); j++)
					
				x->referenceDir[i][j]= referenceDir.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		x->timeExists = timeExists;
		
		
			
		x->time = time.toIDLArrayTime();
			
		
	

	
  		
		
		x->codeExists = codeExists;
		
		
			
				
		x->code = CORBA::string_dup(code.c_str());
				
 			
		
	

	
  		
		
		x->directionCodeExists = directionCodeExists;
		
		
			
				
		x->directionCode = directionCode;
 				
 			
		
	

	
  		
		
		x->directionEquinoxExists = directionEquinoxExists;
		
		
			
		x->directionEquinox = directionEquinox.toIDLArrayTime();
			
		
	

	
  		
		
		x->assocNatureExists = assocNatureExists;
		
		
			
				
		x->assocNature = CORBA::string_dup(assocNature.c_str());
				
 			
		
	

	
	
		
	
  	
 		
 		
		x->assocFieldIdExists = assocFieldIdExists;
		
		
	 	
			
		x->assocFieldId = assocFieldId.toIDLTag();
			
	 	 		
  	

	
  	
 		
 		
		x->ephemerisIdExists = ephemerisIdExists;
		
		
	 	
			
				
		x->ephemerisId = ephemerisId;
 				
 			
	 	 		
  	

	
  	
 		
 		
		x->sourceIdExists = sourceIdExists;
		
		
	 	
			
				
		x->sourceId = sourceId;
 				
 			
	 	 		
  	

	
		
	

	

	

		
		return x;
	
	}
	
	void FieldRow::toIDL(asdmIDL::FieldRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.fieldId = fieldId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.fieldName = CORBA::string_dup(fieldName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.numPoly = numPoly;
 				
 			
		
	

	
  		
		
		
			
		x.delayDir.length(delayDir.size());
		for (unsigned int i = 0; i < delayDir.size(); i++) {
			x.delayDir[i].length(delayDir.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < delayDir.size() ; i++)
			for (unsigned int j = 0; j < delayDir.at(i).size(); j++)
					
				x.delayDir[i][j]= delayDir.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		
			
		x.phaseDir.length(phaseDir.size());
		for (unsigned int i = 0; i < phaseDir.size(); i++) {
			x.phaseDir[i].length(phaseDir.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phaseDir.size() ; i++)
			for (unsigned int j = 0; j < phaseDir.at(i).size(); j++)
					
				x.phaseDir[i][j]= phaseDir.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		
			
		x.referenceDir.length(referenceDir.size());
		for (unsigned int i = 0; i < referenceDir.size(); i++) {
			x.referenceDir[i].length(referenceDir.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < referenceDir.size() ; i++)
			for (unsigned int j = 0; j < referenceDir.at(i).size(); j++)
					
				x.referenceDir[i][j]= referenceDir.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		x.timeExists = timeExists;
		
		
			
		x.time = time.toIDLArrayTime();
			
		
	

	
  		
		
		x.codeExists = codeExists;
		
		
			
				
		x.code = CORBA::string_dup(code.c_str());
				
 			
		
	

	
  		
		
		x.directionCodeExists = directionCodeExists;
		
		
			
				
		x.directionCode = directionCode;
 				
 			
		
	

	
  		
		
		x.directionEquinoxExists = directionEquinoxExists;
		
		
			
		x.directionEquinox = directionEquinox.toIDLArrayTime();
			
		
	

	
  		
		
		x.assocNatureExists = assocNatureExists;
		
		
			
				
		x.assocNature = CORBA::string_dup(assocNature.c_str());
				
 			
		
	

	
	
		
	
  	
 		
 		
		x.assocFieldIdExists = assocFieldIdExists;
		
		
	 	
			
		x.assocFieldId = assocFieldId.toIDLTag();
			
	 	 		
  	

	
  	
 		
 		
		x.ephemerisIdExists = ephemerisIdExists;
		
		
	 	
			
				
		x.ephemerisId = ephemerisId;
 				
 			
	 	 		
  	

	
  	
 		
 		
		x.sourceIdExists = sourceIdExists;
		
		
	 	
			
				
		x.sourceId = sourceId;
 				
 			
	 	 		
  	

	
		
	

	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FieldRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void FieldRow::setFromIDL (FieldRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setFieldId(Tag (x.fieldId));
			
 		
		
	

	
		
		
			
		setFieldName(string (x.fieldName));
			
 		
		
	

	
		
		
			
		setNumPoly(x.numPoly);
  			
 		
		
	

	
		
		
			
		delayDir .clear();
        
        vector<Angle> v_aux_delayDir;
        
		for (unsigned int i = 0; i < x.delayDir.length(); ++i) {
			v_aux_delayDir.clear();
			for (unsigned int j = 0; j < x.delayDir[0].length(); ++j) {
				
				v_aux_delayDir.push_back(Angle (x.delayDir[i][j]));
				
  			}
  			delayDir.push_back(v_aux_delayDir);			
		}
			
  		
		
	

	
		
		
			
		phaseDir .clear();
        
        vector<Angle> v_aux_phaseDir;
        
		for (unsigned int i = 0; i < x.phaseDir.length(); ++i) {
			v_aux_phaseDir.clear();
			for (unsigned int j = 0; j < x.phaseDir[0].length(); ++j) {
				
				v_aux_phaseDir.push_back(Angle (x.phaseDir[i][j]));
				
  			}
  			phaseDir.push_back(v_aux_phaseDir);			
		}
			
  		
		
	

	
		
		
			
		referenceDir .clear();
        
        vector<Angle> v_aux_referenceDir;
        
		for (unsigned int i = 0; i < x.referenceDir.length(); ++i) {
			v_aux_referenceDir.clear();
			for (unsigned int j = 0; j < x.referenceDir[0].length(); ++j) {
				
				v_aux_referenceDir.push_back(Angle (x.referenceDir[i][j]));
				
  			}
  			referenceDir.push_back(v_aux_referenceDir);			
		}
			
  		
		
	

	
		
		timeExists = x.timeExists;
		if (x.timeExists) {
		
		
			
		setTime(ArrayTime (x.time));
			
 		
		
		}
		
	

	
		
		codeExists = x.codeExists;
		if (x.codeExists) {
		
		
			
		setCode(string (x.code));
			
 		
		
		}
		
	

	
		
		directionCodeExists = x.directionCodeExists;
		if (x.directionCodeExists) {
		
		
			
		setDirectionCode(x.directionCode);
  			
 		
		
		}
		
	

	
		
		directionEquinoxExists = x.directionEquinoxExists;
		if (x.directionEquinoxExists) {
		
		
			
		setDirectionEquinox(ArrayTime (x.directionEquinox));
			
 		
		
		}
		
	

	
		
		assocNatureExists = x.assocNatureExists;
		if (x.assocNatureExists) {
		
		
			
		setAssocNature(string (x.assocNature));
			
 		
		
		}
		
	

	
	
		
	
		
		assocFieldIdExists = x.assocFieldIdExists;
		if (x.assocFieldIdExists) {
		
		
			
		setAssocFieldId(Tag (x.assocFieldId));
			
 		
		
		}
		
	

	
		
		ephemerisIdExists = x.ephemerisIdExists;
		if (x.ephemerisIdExists) {
		
		
			
		setEphemerisId(x.ephemerisId);
  			
 		
		
		}
		
	

	
		
		sourceIdExists = x.sourceIdExists;
		if (x.sourceIdExists) {
		
		
			
		setSourceId(x.sourceId);
  			
 		
		
		}
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Field");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string FieldRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(fieldId, "fieldId", buf);
		
		
	

  	
 		
		
		Parser::toXML(fieldName, "fieldName", buf);
		
		
	

  	
 		
		
		Parser::toXML(numPoly, "numPoly", buf);
		
		
	

  	
 		
		
		Parser::toXML(delayDir, "delayDir", buf);
		
		
	

  	
 		
		
		Parser::toXML(phaseDir, "phaseDir", buf);
		
		
	

  	
 		
		
		Parser::toXML(referenceDir, "referenceDir", buf);
		
		
	

  	
 		
		if (timeExists) {
		
		
		Parser::toXML(time, "time", buf);
		
		
		}
		
	

  	
 		
		if (codeExists) {
		
		
		Parser::toXML(code, "code", buf);
		
		
		}
		
	

  	
 		
		if (directionCodeExists) {
		
		
			buf.append(EnumerationParser::toXML("directionCode", directionCode));
		
		
		}
		
	

  	
 		
		if (directionEquinoxExists) {
		
		
		Parser::toXML(directionEquinox, "directionEquinox", buf);
		
		
		}
		
	

  	
 		
		if (assocNatureExists) {
		
		
		Parser::toXML(assocNature, "assocNature", buf);
		
		
		}
		
	

	
	
		
  	
 		
		if (assocFieldIdExists) {
		
		
		Parser::toXML(assocFieldId, "assocFieldId", buf);
		
		
		}
		
	

  	
 		
		if (ephemerisIdExists) {
		
		
		Parser::toXML(ephemerisId, "ephemerisId", buf);
		
		
		}
		
	

  	
 		
		if (sourceIdExists) {
		
		
		Parser::toXML(sourceId, "sourceId", buf);
		
		
		}
		
	

	
		
	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void FieldRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setFieldId(Parser::getTag("fieldId","Field",rowDoc));
			
		
	

	
  		
			
	  	setFieldName(Parser::getString("fieldName","Field",rowDoc));
			
		
	

	
  		
			
	  	setNumPoly(Parser::getInteger("numPoly","Field",rowDoc));
			
		
	

	
  		
			
					
	  	setDelayDir(Parser::get2DAngle("delayDir","Field",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setPhaseDir(Parser::get2DAngle("phaseDir","Field",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setReferenceDir(Parser::get2DAngle("referenceDir","Field",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<time>")) {
			
	  		setTime(Parser::getArrayTime("time","Field",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<code>")) {
			
	  		setCode(Parser::getString("code","Field",rowDoc));
			
		}
 		
	

	
		
	if (row.isStr("<directionCode>")) {
		
		
		
		directionCode = EnumerationParser::getDirectionReferenceCode("directionCode","Field",rowDoc);
		
		
		
		directionCodeExists = true;
	}
		
	

	
  		
        if (row.isStr("<directionEquinox>")) {
			
	  		setDirectionEquinox(Parser::getArrayTime("directionEquinox","Field",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<assocNature>")) {
			
	  		setAssocNature(Parser::getString("assocNature","Field",rowDoc));
			
		}
 		
	

	
	
		
	
  		
        if (row.isStr("<assocFieldId>")) {
			
	  		setAssocFieldId(Parser::getTag("assocFieldId","Field",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<ephemerisId>")) {
			
	  		setEphemerisId(Parser::getInteger("ephemerisId","Field",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<sourceId>")) {
			
	  		setSourceId(Parser::getInteger("sourceId","Field",rowDoc));
			
		}
 		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Field");
		}
	}
	
	void FieldRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	fieldId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(fieldName);
				
		
	

	
	
		
						
			eoss.writeInt(numPoly);
				
		
	

	
	
		
	Angle::toBin(delayDir, eoss);
		
	

	
	
		
	Angle::toBin(phaseDir, eoss);
		
	

	
	
		
	Angle::toBin(referenceDir, eoss);
		
	


	
	
	eoss.writeBoolean(timeExists);
	if (timeExists) {
	
	
	
		
	time.toBin(eoss);
		
	

	}

	eoss.writeBoolean(codeExists);
	if (codeExists) {
	
	
	
		
						
			eoss.writeString(code);
				
		
	

	}

	eoss.writeBoolean(directionCodeExists);
	if (directionCodeExists) {
	
	
	
		
					
		eoss.writeString(CDirectionReferenceCode::name(directionCode));
			/* eoss.writeInt(directionCode); */
				
		
	

	}

	eoss.writeBoolean(directionEquinoxExists);
	if (directionEquinoxExists) {
	
	
	
		
	directionEquinox.toBin(eoss);
		
	

	}

	eoss.writeBoolean(assocNatureExists);
	if (assocNatureExists) {
	
	
	
		
						
			eoss.writeString(assocNature);
				
		
	

	}

	eoss.writeBoolean(ephemerisIdExists);
	if (ephemerisIdExists) {
	
	
	
		
						
			eoss.writeInt(ephemerisId);
				
		
	

	}

	eoss.writeBoolean(sourceIdExists);
	if (sourceIdExists) {
	
	
	
		
						
			eoss.writeInt(sourceId);
				
		
	

	}

	eoss.writeBoolean(assocFieldIdExists);
	if (assocFieldIdExists) {
	
	
	
		
	assocFieldId.toBin(eoss);
		
	

	}

	}
	
void FieldRow::fieldIdFromBin(EndianIStream& eis) {
		
	
		
		
		fieldId =  Tag::fromBin(eis);
		
	
	
}
void FieldRow::fieldNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		fieldName =  eis.readString();
			
		
	
	
}
void FieldRow::numPolyFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPoly =  eis.readInt();
			
		
	
	
}
void FieldRow::delayDirFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	delayDir = Angle::from2DBin(eis);		
	

		
	
	
}
void FieldRow::phaseDirFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	phaseDir = Angle::from2DBin(eis);		
	

		
	
	
}
void FieldRow::referenceDirFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	referenceDir = Angle::from2DBin(eis);		
	

		
	
	
}

void FieldRow::timeFromBin(EndianIStream& eis) {
		
	timeExists = eis.readBoolean();
	if (timeExists) {
		
	
		
		
		time =  ArrayTime::fromBin(eis);
		
	

	}
	
}
void FieldRow::codeFromBin(EndianIStream& eis) {
		
	codeExists = eis.readBoolean();
	if (codeExists) {
		
	
	
		
			
		code =  eis.readString();
			
		
	

	}
	
}
void FieldRow::directionCodeFromBin(EndianIStream& eis) {
		
	directionCodeExists = eis.readBoolean();
	if (directionCodeExists) {
		
	
	
		
			
		directionCode = CDirectionReferenceCode::literal(eis.readString());
			
		
	

	}
	
}
void FieldRow::directionEquinoxFromBin(EndianIStream& eis) {
		
	directionEquinoxExists = eis.readBoolean();
	if (directionEquinoxExists) {
		
	
		
		
		directionEquinox =  ArrayTime::fromBin(eis);
		
	

	}
	
}
void FieldRow::assocNatureFromBin(EndianIStream& eis) {
		
	assocNatureExists = eis.readBoolean();
	if (assocNatureExists) {
		
	
	
		
			
		assocNature =  eis.readString();
			
		
	

	}
	
}
void FieldRow::ephemerisIdFromBin(EndianIStream& eis) {
		
	ephemerisIdExists = eis.readBoolean();
	if (ephemerisIdExists) {
		
	
	
		
			
		ephemerisId =  eis.readInt();
			
		
	

	}
	
}
void FieldRow::sourceIdFromBin(EndianIStream& eis) {
		
	sourceIdExists = eis.readBoolean();
	if (sourceIdExists) {
		
	
	
		
			
		sourceId =  eis.readInt();
			
		
	

	}
	
}
void FieldRow::assocFieldIdFromBin(EndianIStream& eis) {
		
	assocFieldIdExists = eis.readBoolean();
	if (assocFieldIdExists) {
		
	
		
		
		assocFieldId =  Tag::fromBin(eis);
		
	

	}
	
}
	
	
	FieldRow* FieldRow::fromBin(EndianIStream& eis, FieldTable& table, const vector<string>& attributesSeq) {
		FieldRow* row = new  FieldRow(table);
		
		map<string, FieldAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "FieldTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void FieldRow::fieldIdFromText(const string & s) {
		 
          
		fieldId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FieldRow::fieldNameFromText(const string & s) {
		 
          
		fieldName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FieldRow::numPolyFromText(const string & s) {
		 
          
		numPoly = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void FieldRow::delayDirFromText(const string & s) {
		 
          
		delayDir = ASDMValuesParser::parse2D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void FieldRow::phaseDirFromText(const string & s) {
		 
          
		phaseDir = ASDMValuesParser::parse2D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void FieldRow::referenceDirFromText(const string & s) {
		 
          
		referenceDir = ASDMValuesParser::parse2D<Angle>(s);
          
		
	}
	

	
	// Convert a string into an ArrayTime 
	void FieldRow::timeFromText(const string & s) {
		timeExists = true;
		 
          
		time = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FieldRow::codeFromText(const string & s) {
		codeExists = true;
		 
          
		code = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an DirectionReferenceCode 
	void FieldRow::directionCodeFromText(const string & s) {
		directionCodeExists = true;
		 
          
		directionCode = ASDMValuesParser::parse<DirectionReferenceCodeMod::DirectionReferenceCode>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void FieldRow::directionEquinoxFromText(const string & s) {
		directionEquinoxExists = true;
		 
          
		directionEquinox = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FieldRow::assocNatureFromText(const string & s) {
		assocNatureExists = true;
		 
          
		assocNature = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FieldRow::ephemerisIdFromText(const string & s) {
		ephemerisIdExists = true;
		 
          
		ephemerisId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FieldRow::sourceIdFromText(const string & s) {
		sourceIdExists = true;
		 
          
		sourceId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void FieldRow::assocFieldIdFromText(const string & s) {
		assocFieldIdExists = true;
		 
          
		assocFieldId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	
	void FieldRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, FieldAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "FieldTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get fieldId.
 	 * @return fieldId as Tag
 	 */
 	Tag FieldRow::getFieldId() const {
	
  		return fieldId;
 	}

 	/**
 	 * Set fieldId with the specified Tag.
 	 * @param fieldId The Tag value to which fieldId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FieldRow::setFieldId (Tag fieldId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("fieldId", "Field");
		
  		}
  	
 		this->fieldId = fieldId;
	
 	}
	
	

	

	
 	/**
 	 * Get fieldName.
 	 * @return fieldName as std::string
 	 */
 	std::string FieldRow::getFieldName() const {
	
  		return fieldName;
 	}

 	/**
 	 * Set fieldName with the specified std::string.
 	 * @param fieldName The std::string value to which fieldName is to be set.
 	 
 	
 		
 	 */
 	void FieldRow::setFieldName (std::string fieldName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->fieldName = fieldName;
	
 	}
	
	

	

	
 	/**
 	 * Get numPoly.
 	 * @return numPoly as int
 	 */
 	int FieldRow::getNumPoly() const {
	
  		return numPoly;
 	}

 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 	
 		
 	 */
 	void FieldRow::setNumPoly (int numPoly)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPoly = numPoly;
	
 	}
	
	

	

	
 	/**
 	 * Get delayDir.
 	 * @return delayDir as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > FieldRow::getDelayDir() const {
	
  		return delayDir;
 	}

 	/**
 	 * Set delayDir with the specified std::vector<std::vector<Angle > >.
 	 * @param delayDir The std::vector<std::vector<Angle > > value to which delayDir is to be set.
 	 
 	
 		
 	 */
 	void FieldRow::setDelayDir (std::vector<std::vector<Angle > > delayDir)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->delayDir = delayDir;
	
 	}
	
	

	

	
 	/**
 	 * Get phaseDir.
 	 * @return phaseDir as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > FieldRow::getPhaseDir() const {
	
  		return phaseDir;
 	}

 	/**
 	 * Set phaseDir with the specified std::vector<std::vector<Angle > >.
 	 * @param phaseDir The std::vector<std::vector<Angle > > value to which phaseDir is to be set.
 	 
 	
 		
 	 */
 	void FieldRow::setPhaseDir (std::vector<std::vector<Angle > > phaseDir)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phaseDir = phaseDir;
	
 	}
	
	

	

	
 	/**
 	 * Get referenceDir.
 	 * @return referenceDir as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > FieldRow::getReferenceDir() const {
	
  		return referenceDir;
 	}

 	/**
 	 * Set referenceDir with the specified std::vector<std::vector<Angle > >.
 	 * @param referenceDir The std::vector<std::vector<Angle > > value to which referenceDir is to be set.
 	 
 	
 		
 	 */
 	void FieldRow::setReferenceDir (std::vector<std::vector<Angle > > referenceDir)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->referenceDir = referenceDir;
	
 	}
	
	

	
	/**
	 * The attribute time is optional. Return true if this attribute exists.
	 * @return true if and only if the time attribute exists. 
	 */
	bool FieldRow::isTimeExists() const {
		return timeExists;
	}
	

	
 	/**
 	 * Get time, which is optional.
 	 * @return time as ArrayTime
 	 * @throw IllegalAccessException If time does not exist.
 	 */
 	ArrayTime FieldRow::getTime() const  {
		if (!timeExists) {
			throw IllegalAccessException("time", "Field");
		}
	
  		return time;
 	}

 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 	
 	 */
 	void FieldRow::setTime (ArrayTime time) {
	
 		this->time = time;
	
		timeExists = true;
	
 	}
	
	
	/**
	 * Mark time, which is an optional field, as non-existent.
	 */
	void FieldRow::clearTime () {
		timeExists = false;
	}
	

	
	/**
	 * The attribute code is optional. Return true if this attribute exists.
	 * @return true if and only if the code attribute exists. 
	 */
	bool FieldRow::isCodeExists() const {
		return codeExists;
	}
	

	
 	/**
 	 * Get code, which is optional.
 	 * @return code as std::string
 	 * @throw IllegalAccessException If code does not exist.
 	 */
 	std::string FieldRow::getCode() const  {
		if (!codeExists) {
			throw IllegalAccessException("code", "Field");
		}
	
  		return code;
 	}

 	/**
 	 * Set code with the specified std::string.
 	 * @param code The std::string value to which code is to be set.
 	 
 	
 	 */
 	void FieldRow::setCode (std::string code) {
	
 		this->code = code;
	
		codeExists = true;
	
 	}
	
	
	/**
	 * Mark code, which is an optional field, as non-existent.
	 */
	void FieldRow::clearCode () {
		codeExists = false;
	}
	

	
	/**
	 * The attribute directionCode is optional. Return true if this attribute exists.
	 * @return true if and only if the directionCode attribute exists. 
	 */
	bool FieldRow::isDirectionCodeExists() const {
		return directionCodeExists;
	}
	

	
 	/**
 	 * Get directionCode, which is optional.
 	 * @return directionCode as DirectionReferenceCodeMod::DirectionReferenceCode
 	 * @throw IllegalAccessException If directionCode does not exist.
 	 */
 	DirectionReferenceCodeMod::DirectionReferenceCode FieldRow::getDirectionCode() const  {
		if (!directionCodeExists) {
			throw IllegalAccessException("directionCode", "Field");
		}
	
  		return directionCode;
 	}

 	/**
 	 * Set directionCode with the specified DirectionReferenceCodeMod::DirectionReferenceCode.
 	 * @param directionCode The DirectionReferenceCodeMod::DirectionReferenceCode value to which directionCode is to be set.
 	 
 	
 	 */
 	void FieldRow::setDirectionCode (DirectionReferenceCodeMod::DirectionReferenceCode directionCode) {
	
 		this->directionCode = directionCode;
	
		directionCodeExists = true;
	
 	}
	
	
	/**
	 * Mark directionCode, which is an optional field, as non-existent.
	 */
	void FieldRow::clearDirectionCode () {
		directionCodeExists = false;
	}
	

	
	/**
	 * The attribute directionEquinox is optional. Return true if this attribute exists.
	 * @return true if and only if the directionEquinox attribute exists. 
	 */
	bool FieldRow::isDirectionEquinoxExists() const {
		return directionEquinoxExists;
	}
	

	
 	/**
 	 * Get directionEquinox, which is optional.
 	 * @return directionEquinox as ArrayTime
 	 * @throw IllegalAccessException If directionEquinox does not exist.
 	 */
 	ArrayTime FieldRow::getDirectionEquinox() const  {
		if (!directionEquinoxExists) {
			throw IllegalAccessException("directionEquinox", "Field");
		}
	
  		return directionEquinox;
 	}

 	/**
 	 * Set directionEquinox with the specified ArrayTime.
 	 * @param directionEquinox The ArrayTime value to which directionEquinox is to be set.
 	 
 	
 	 */
 	void FieldRow::setDirectionEquinox (ArrayTime directionEquinox) {
	
 		this->directionEquinox = directionEquinox;
	
		directionEquinoxExists = true;
	
 	}
	
	
	/**
	 * Mark directionEquinox, which is an optional field, as non-existent.
	 */
	void FieldRow::clearDirectionEquinox () {
		directionEquinoxExists = false;
	}
	

	
	/**
	 * The attribute assocNature is optional. Return true if this attribute exists.
	 * @return true if and only if the assocNature attribute exists. 
	 */
	bool FieldRow::isAssocNatureExists() const {
		return assocNatureExists;
	}
	

	
 	/**
 	 * Get assocNature, which is optional.
 	 * @return assocNature as std::string
 	 * @throw IllegalAccessException If assocNature does not exist.
 	 */
 	std::string FieldRow::getAssocNature() const  {
		if (!assocNatureExists) {
			throw IllegalAccessException("assocNature", "Field");
		}
	
  		return assocNature;
 	}

 	/**
 	 * Set assocNature with the specified std::string.
 	 * @param assocNature The std::string value to which assocNature is to be set.
 	 
 	
 	 */
 	void FieldRow::setAssocNature (std::string assocNature) {
	
 		this->assocNature = assocNature;
	
		assocNatureExists = true;
	
 	}
	
	
	/**
	 * Mark assocNature, which is an optional field, as non-existent.
	 */
	void FieldRow::clearAssocNature () {
		assocNatureExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	
	/**
	 * The attribute assocFieldId is optional. Return true if this attribute exists.
	 * @return true if and only if the assocFieldId attribute exists. 
	 */
	bool FieldRow::isAssocFieldIdExists() const {
		return assocFieldIdExists;
	}
	

	
 	/**
 	 * Get assocFieldId, which is optional.
 	 * @return assocFieldId as Tag
 	 * @throw IllegalAccessException If assocFieldId does not exist.
 	 */
 	Tag FieldRow::getAssocFieldId() const  {
		if (!assocFieldIdExists) {
			throw IllegalAccessException("assocFieldId", "Field");
		}
	
  		return assocFieldId;
 	}

 	/**
 	 * Set assocFieldId with the specified Tag.
 	 * @param assocFieldId The Tag value to which assocFieldId is to be set.
 	 
 	
 	 */
 	void FieldRow::setAssocFieldId (Tag assocFieldId) {
	
 		this->assocFieldId = assocFieldId;
	
		assocFieldIdExists = true;
	
 	}
	
	
	/**
	 * Mark assocFieldId, which is an optional field, as non-existent.
	 */
	void FieldRow::clearAssocFieldId () {
		assocFieldIdExists = false;
	}
	

	
	/**
	 * The attribute ephemerisId is optional. Return true if this attribute exists.
	 * @return true if and only if the ephemerisId attribute exists. 
	 */
	bool FieldRow::isEphemerisIdExists() const {
		return ephemerisIdExists;
	}
	

	
 	/**
 	 * Get ephemerisId, which is optional.
 	 * @return ephemerisId as int
 	 * @throw IllegalAccessException If ephemerisId does not exist.
 	 */
 	int FieldRow::getEphemerisId() const  {
		if (!ephemerisIdExists) {
			throw IllegalAccessException("ephemerisId", "Field");
		}
	
  		return ephemerisId;
 	}

 	/**
 	 * Set ephemerisId with the specified int.
 	 * @param ephemerisId The int value to which ephemerisId is to be set.
 	 
 	
 	 */
 	void FieldRow::setEphemerisId (int ephemerisId) {
	
 		this->ephemerisId = ephemerisId;
	
		ephemerisIdExists = true;
	
 	}
	
	
	/**
	 * Mark ephemerisId, which is an optional field, as non-existent.
	 */
	void FieldRow::clearEphemerisId () {
		ephemerisIdExists = false;
	}
	

	
	/**
	 * The attribute sourceId is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceId attribute exists. 
	 */
	bool FieldRow::isSourceIdExists() const {
		return sourceIdExists;
	}
	

	
 	/**
 	 * Get sourceId, which is optional.
 	 * @return sourceId as int
 	 * @throw IllegalAccessException If sourceId does not exist.
 	 */
 	int FieldRow::getSourceId() const  {
		if (!sourceIdExists) {
			throw IllegalAccessException("sourceId", "Field");
		}
	
  		return sourceId;
 	}

 	/**
 	 * Set sourceId with the specified int.
 	 * @param sourceId The int value to which sourceId is to be set.
 	 
 	
 	 */
 	void FieldRow::setSourceId (int sourceId) {
	
 		this->sourceId = sourceId;
	
		sourceIdExists = true;
	
 	}
	
	
	/**
	 * Mark sourceId, which is an optional field, as non-existent.
	 */
	void FieldRow::clearSourceId () {
		sourceIdExists = false;
	}
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		
	

	

	
	
	
		

	// ===> Slice link from a row of Field table to a collection of row of Source table.
	
	/**
	 * Get the collection of row in the Source table having their attribut sourceId == this->sourceId
	 */
	vector <SourceRow *> FieldRow::getSources() {
		
			if (sourceIdExists) {
				return table.getContainer().getSource().getRowBySourceId(sourceId);
			}
			else 
				throw IllegalAccessException();
		
	}
	

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the Field table having Field.assocFieldId == assocFieldId
	 * @return a FieldRow*
	 * 
	 
	 * throws IllegalAccessException
	 
	 */
	 FieldRow* FieldRow::getFieldUsingAssocFieldId() {
	 
	 	if (!assocFieldIdExists)
	 		throw IllegalAccessException();	 		 
	 
	 	return table.getContainer().getField().getRowByKey(assocFieldId);
	 }
	 

	

	
	/**
	 * Create a FieldRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FieldRow::FieldRow (FieldTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	
		timeExists = false;
	

	
		codeExists = false;
	

	
		directionCodeExists = false;
	

	
		directionEquinoxExists = false;
	

	
		assocNatureExists = false;
	

	
	
		assocFieldIdExists = false;
	

	
		ephemerisIdExists = false;
	

	
		sourceIdExists = false;
	

	
	
	
	

	

	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
directionCode = CDirectionReferenceCode::from_int(0);
	

	

	

	
	
	 fromBinMethods["fieldId"] = &FieldRow::fieldIdFromBin; 
	 fromBinMethods["fieldName"] = &FieldRow::fieldNameFromBin; 
	 fromBinMethods["numPoly"] = &FieldRow::numPolyFromBin; 
	 fromBinMethods["delayDir"] = &FieldRow::delayDirFromBin; 
	 fromBinMethods["phaseDir"] = &FieldRow::phaseDirFromBin; 
	 fromBinMethods["referenceDir"] = &FieldRow::referenceDirFromBin; 
		
	
	 fromBinMethods["time"] = &FieldRow::timeFromBin; 
	 fromBinMethods["code"] = &FieldRow::codeFromBin; 
	 fromBinMethods["directionCode"] = &FieldRow::directionCodeFromBin; 
	 fromBinMethods["directionEquinox"] = &FieldRow::directionEquinoxFromBin; 
	 fromBinMethods["assocNature"] = &FieldRow::assocNatureFromBin; 
	 fromBinMethods["ephemerisId"] = &FieldRow::ephemerisIdFromBin; 
	 fromBinMethods["sourceId"] = &FieldRow::sourceIdFromBin; 
	 fromBinMethods["assocFieldId"] = &FieldRow::assocFieldIdFromBin; 
	
	
	
	
				 
	fromTextMethods["fieldId"] = &FieldRow::fieldIdFromText;
		 
	
				 
	fromTextMethods["fieldName"] = &FieldRow::fieldNameFromText;
		 
	
				 
	fromTextMethods["numPoly"] = &FieldRow::numPolyFromText;
		 
	
				 
	fromTextMethods["delayDir"] = &FieldRow::delayDirFromText;
		 
	
				 
	fromTextMethods["phaseDir"] = &FieldRow::phaseDirFromText;
		 
	
				 
	fromTextMethods["referenceDir"] = &FieldRow::referenceDirFromText;
		 
	

	 
				
	fromTextMethods["time"] = &FieldRow::timeFromText;
		 	
	 
				
	fromTextMethods["code"] = &FieldRow::codeFromText;
		 	
	 
				
	fromTextMethods["directionCode"] = &FieldRow::directionCodeFromText;
		 	
	 
				
	fromTextMethods["directionEquinox"] = &FieldRow::directionEquinoxFromText;
		 	
	 
				
	fromTextMethods["assocNature"] = &FieldRow::assocNatureFromText;
		 	
	 
				
	fromTextMethods["ephemerisId"] = &FieldRow::ephemerisIdFromText;
		 	
	 
				
	fromTextMethods["sourceId"] = &FieldRow::sourceIdFromText;
		 	
	 
				
	fromTextMethods["assocFieldId"] = &FieldRow::assocFieldIdFromText;
		 	
		
	}
	
	FieldRow::FieldRow (FieldTable &t, FieldRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	
		timeExists = false;
	

	
		codeExists = false;
	

	
		directionCodeExists = false;
	

	
		directionEquinoxExists = false;
	

	
		assocNatureExists = false;
	

	
	
		assocFieldIdExists = false;
	

	
		ephemerisIdExists = false;
	

	
		sourceIdExists = false;
	
		
		}
		else {
	
		
			fieldId = row->fieldId;
		
		
		
		
			fieldName = row->fieldName;
		
			numPoly = row->numPoly;
		
			delayDir = row->delayDir;
		
			phaseDir = row->phaseDir;
		
			referenceDir = row->referenceDir;
		
		
		
		
		if (row->timeExists) {
			time = row->time;		
			timeExists = true;
		}
		else
			timeExists = false;
		
		if (row->codeExists) {
			code = row->code;		
			codeExists = true;
		}
		else
			codeExists = false;
		
		if (row->directionCodeExists) {
			directionCode = row->directionCode;		
			directionCodeExists = true;
		}
		else
			directionCodeExists = false;
		
		if (row->directionEquinoxExists) {
			directionEquinox = row->directionEquinox;		
			directionEquinoxExists = true;
		}
		else
			directionEquinoxExists = false;
		
		if (row->assocNatureExists) {
			assocNature = row->assocNature;		
			assocNatureExists = true;
		}
		else
			assocNatureExists = false;
		
		if (row->ephemerisIdExists) {
			ephemerisId = row->ephemerisId;		
			ephemerisIdExists = true;
		}
		else
			ephemerisIdExists = false;
		
		if (row->sourceIdExists) {
			sourceId = row->sourceId;		
			sourceIdExists = true;
		}
		else
			sourceIdExists = false;
		
		if (row->assocFieldIdExists) {
			assocFieldId = row->assocFieldId;		
			assocFieldIdExists = true;
		}
		else
			assocFieldIdExists = false;
		
		}
		
		 fromBinMethods["fieldId"] = &FieldRow::fieldIdFromBin; 
		 fromBinMethods["fieldName"] = &FieldRow::fieldNameFromBin; 
		 fromBinMethods["numPoly"] = &FieldRow::numPolyFromBin; 
		 fromBinMethods["delayDir"] = &FieldRow::delayDirFromBin; 
		 fromBinMethods["phaseDir"] = &FieldRow::phaseDirFromBin; 
		 fromBinMethods["referenceDir"] = &FieldRow::referenceDirFromBin; 
			
	
		 fromBinMethods["time"] = &FieldRow::timeFromBin; 
		 fromBinMethods["code"] = &FieldRow::codeFromBin; 
		 fromBinMethods["directionCode"] = &FieldRow::directionCodeFromBin; 
		 fromBinMethods["directionEquinox"] = &FieldRow::directionEquinoxFromBin; 
		 fromBinMethods["assocNature"] = &FieldRow::assocNatureFromBin; 
		 fromBinMethods["ephemerisId"] = &FieldRow::ephemerisIdFromBin; 
		 fromBinMethods["sourceId"] = &FieldRow::sourceIdFromBin; 
		 fromBinMethods["assocFieldId"] = &FieldRow::assocFieldIdFromBin; 
			
	}

	
	bool FieldRow::compareNoAutoInc(std::string fieldName, int numPoly, std::vector<std::vector<Angle > > delayDir, std::vector<std::vector<Angle > > phaseDir, std::vector<std::vector<Angle > > referenceDir) {
		bool result;
		result = true;
		
	
		
		result = result && (this->fieldName == fieldName);
		
		if (!result) return false;
	

	
		
		result = result && (this->numPoly == numPoly);
		
		if (!result) return false;
	

	
		
		result = result && (this->delayDir == delayDir);
		
		if (!result) return false;
	

	
		
		result = result && (this->phaseDir == phaseDir);
		
		if (!result) return false;
	

	
		
		result = result && (this->referenceDir == referenceDir);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool FieldRow::compareRequiredValue(std::string fieldName, int numPoly, std::vector<std::vector<Angle > > delayDir, std::vector<std::vector<Angle > > phaseDir, std::vector<std::vector<Angle > > referenceDir) {
		bool result;
		result = true;
		
	
		if (!(this->fieldName == fieldName)) return false;
	

	
		if (!(this->numPoly == numPoly)) return false;
	

	
		if (!(this->delayDir == delayDir)) return false;
	

	
		if (!(this->phaseDir == phaseDir)) return false;
	

	
		if (!(this->referenceDir == referenceDir)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the FieldRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool FieldRow::equalByRequiredValue(FieldRow*  x ) {
		
			
		if (this->fieldName != x->fieldName) return false;
			
		if (this->numPoly != x->numPoly) return false;
			
		if (this->delayDir != x->delayDir) return false;
			
		if (this->phaseDir != x->phaseDir) return false;
			
		if (this->referenceDir != x->referenceDir) return false;
			
		
		return true;
	}	
	
/*
	 map<string, FieldAttributeFromBin> FieldRow::initFromBinMethods() {
		map<string, FieldAttributeFromBin> result;
		
		result["fieldId"] = &FieldRow::fieldIdFromBin;
		result["fieldName"] = &FieldRow::fieldNameFromBin;
		result["numPoly"] = &FieldRow::numPolyFromBin;
		result["delayDir"] = &FieldRow::delayDirFromBin;
		result["phaseDir"] = &FieldRow::phaseDirFromBin;
		result["referenceDir"] = &FieldRow::referenceDirFromBin;
		
		
		result["time"] = &FieldRow::timeFromBin;
		result["code"] = &FieldRow::codeFromBin;
		result["directionCode"] = &FieldRow::directionCodeFromBin;
		result["directionEquinox"] = &FieldRow::directionEquinoxFromBin;
		result["assocNature"] = &FieldRow::assocNatureFromBin;
		result["ephemerisId"] = &FieldRow::ephemerisIdFromBin;
		result["sourceId"] = &FieldRow::sourceIdFromBin;
		result["assocFieldId"] = &FieldRow::assocFieldIdFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
