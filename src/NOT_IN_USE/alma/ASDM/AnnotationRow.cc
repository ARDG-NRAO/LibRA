
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
 * File AnnotationRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/AnnotationRow.h>
#include <alma/ASDM/AnnotationTable.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>
	

using asdm::ASDM;
using asdm::AnnotationRow;
using asdm::AnnotationTable;

using asdm::AntennaTable;
using asdm::AntennaRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	AnnotationRow::~AnnotationRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	AnnotationTable &AnnotationRow::getTable() const {
		return table;
	}

	bool AnnotationRow::isAdded() const {
		return hasBeenAdded;
	}	

	void AnnotationRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::AnnotationRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a AnnotationRowIDL struct.
	 */
	AnnotationRowIDL *AnnotationRow::toIDL() const {
		AnnotationRowIDL *x = new AnnotationRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->annotationId = annotationId.toIDLTag();
			
		
	

	
  		
		
		
			
		x->time = time.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->issue = CORBA::string_dup(issue.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->details = CORBA::string_dup(details.c_str());
				
 			
		
	

	
  		
		
		x->numAntennaExists = numAntennaExists;
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		x->basebandNameExists = basebandNameExists;
		
		
			
		x->basebandName.length(basebandName.size());
		for (unsigned int i = 0; i < basebandName.size(); ++i) {
			
				
			x->basebandName[i] = basebandName.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->numBasebandExists = numBasebandExists;
		
		
			
				
		x->numBaseband = numBaseband;
 				
 			
		
	

	
  		
		
		x->intervalExists = intervalExists;
		
		
			
		x->interval = interval.toIDLInterval();
			
		
	

	
  		
		
		x->dValueExists = dValueExists;
		
		
			
				
		x->dValue = dValue;
 				
 			
		
	

	
  		
		
		x->vdValueExists = vdValueExists;
		
		
			
		x->vdValue.length(vdValue.size());
		for (unsigned int i = 0; i < vdValue.size(); ++i) {
			
				
			x->vdValue[i] = vdValue.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->vvdValuesExists = vvdValuesExists;
		
		
			
		x->vvdValues.length(vvdValues.size());
		for (unsigned int i = 0; i < vvdValues.size(); i++) {
			x->vvdValues[i].length(vvdValues.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < vvdValues.size() ; i++)
			for (unsigned int j = 0; j < vvdValues.at(i).size(); j++)
					
						
				x->vvdValues[i][j] = vvdValues.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x->llValueExists = llValueExists;
		
		
			
				
		x->llValue = llValue;
 				
 			
		
	

	
  		
		
		x->vllValueExists = vllValueExists;
		
		
			
		x->vllValue.length(vllValue.size());
		for (unsigned int i = 0; i < vllValue.size(); ++i) {
			
				
			x->vllValue[i] = vllValue.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->vvllValueExists = vvllValueExists;
		
		
			
		x->vvllValue.length(vvllValue.size());
		for (unsigned int i = 0; i < vvllValue.size(); i++) {
			x->vvllValue[i].length(vvllValue.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < vvllValue.size() ; i++)
			for (unsigned int j = 0; j < vvllValue.at(i).size(); j++)
					
						
				x->vvllValue[i][j] = vvllValue.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x->sValueExists = sValueExists;
		
		
			
				
		x->sValue = CORBA::string_dup(sValue.c_str());
				
 			
		
	

	
	
		
	
  	
 		
 		
		x->antennaIdExists = antennaIdExists;
		
		
		
		x->antennaId.length(antennaId.size());
		for (unsigned int i = 0; i < antennaId.size(); ++i) {
			
			x->antennaId[i] = antennaId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void AnnotationRow::toIDL(asdmIDL::AnnotationRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.annotationId = annotationId.toIDLTag();
			
		
	

	
  		
		
		
			
		x.time = time.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.issue = CORBA::string_dup(issue.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.details = CORBA::string_dup(details.c_str());
				
 			
		
	

	
  		
		
		x.numAntennaExists = numAntennaExists;
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		x.basebandNameExists = basebandNameExists;
		
		
			
		x.basebandName.length(basebandName.size());
		for (unsigned int i = 0; i < basebandName.size(); ++i) {
			
				
			x.basebandName[i] = basebandName.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.numBasebandExists = numBasebandExists;
		
		
			
				
		x.numBaseband = numBaseband;
 				
 			
		
	

	
  		
		
		x.intervalExists = intervalExists;
		
		
			
		x.interval = interval.toIDLInterval();
			
		
	

	
  		
		
		x.dValueExists = dValueExists;
		
		
			
				
		x.dValue = dValue;
 				
 			
		
	

	
  		
		
		x.vdValueExists = vdValueExists;
		
		
			
		x.vdValue.length(vdValue.size());
		for (unsigned int i = 0; i < vdValue.size(); ++i) {
			
				
			x.vdValue[i] = vdValue.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.vvdValuesExists = vvdValuesExists;
		
		
			
		x.vvdValues.length(vvdValues.size());
		for (unsigned int i = 0; i < vvdValues.size(); i++) {
			x.vvdValues[i].length(vvdValues.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < vvdValues.size() ; i++)
			for (unsigned int j = 0; j < vvdValues.at(i).size(); j++)
					
						
				x.vvdValues[i][j] = vvdValues.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x.llValueExists = llValueExists;
		
		
			
				
		x.llValue = llValue;
 				
 			
		
	

	
  		
		
		x.vllValueExists = vllValueExists;
		
		
			
		x.vllValue.length(vllValue.size());
		for (unsigned int i = 0; i < vllValue.size(); ++i) {
			
				
			x.vllValue[i] = vllValue.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.vvllValueExists = vvllValueExists;
		
		
			
		x.vvllValue.length(vvllValue.size());
		for (unsigned int i = 0; i < vvllValue.size(); i++) {
			x.vvllValue[i].length(vvllValue.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < vvllValue.size() ; i++)
			for (unsigned int j = 0; j < vvllValue.at(i).size(); j++)
					
						
				x.vvllValue[i][j] = vvllValue.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x.sValueExists = sValueExists;
		
		
			
				
		x.sValue = CORBA::string_dup(sValue.c_str());
				
 			
		
	

	
	
		
	
  	
 		
 		
		x.antennaIdExists = antennaIdExists;
		
		
		
		x.antennaId.length(antennaId.size());
		for (unsigned int i = 0; i < antennaId.size(); ++i) {
			
			x.antennaId[i] = antennaId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct AnnotationRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void AnnotationRow::setFromIDL (AnnotationRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setAnnotationId(Tag (x.annotationId));
			
 		
		
	

	
		
		
			
		setTime(ArrayTime (x.time));
			
 		
		
	

	
		
		
			
		setIssue(string (x.issue));
			
 		
		
	

	
		
		
			
		setDetails(string (x.details));
			
 		
		
	

	
		
		numAntennaExists = x.numAntennaExists;
		if (x.numAntennaExists) {
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
		}
		
	

	
		
		basebandNameExists = x.basebandNameExists;
		if (x.basebandNameExists) {
		
		
			
		basebandName .clear();
		for (unsigned int i = 0; i <x.basebandName.length(); ++i) {
			
			basebandName.push_back(x.basebandName[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		numBasebandExists = x.numBasebandExists;
		if (x.numBasebandExists) {
		
		
			
		setNumBaseband(x.numBaseband);
  			
 		
		
		}
		
	

	
		
		intervalExists = x.intervalExists;
		if (x.intervalExists) {
		
		
			
		setInterval(Interval (x.interval));
			
 		
		
		}
		
	

	
		
		dValueExists = x.dValueExists;
		if (x.dValueExists) {
		
		
			
		setDValue(x.dValue);
  			
 		
		
		}
		
	

	
		
		vdValueExists = x.vdValueExists;
		if (x.vdValueExists) {
		
		
			
		vdValue .clear();
		for (unsigned int i = 0; i <x.vdValue.length(); ++i) {
			
			vdValue.push_back(x.vdValue[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		vvdValuesExists = x.vvdValuesExists;
		if (x.vvdValuesExists) {
		
		
			
		vvdValues .clear();
        
        vector<double> v_aux_vvdValues;
        
		for (unsigned int i = 0; i < x.vvdValues.length(); ++i) {
			v_aux_vvdValues.clear();
			for (unsigned int j = 0; j < x.vvdValues[0].length(); ++j) {
				
				v_aux_vvdValues.push_back(x.vvdValues[i][j]);
	  			
  			}
  			vvdValues.push_back(v_aux_vvdValues);			
		}
			
  		
		
		}
		
	

	
		
		llValueExists = x.llValueExists;
		if (x.llValueExists) {
		
		
			
		setLlValue(x.llValue);
  			
 		
		
		}
		
	

	
		
		vllValueExists = x.vllValueExists;
		if (x.vllValueExists) {
		
		
			
		vllValue .clear();
		for (unsigned int i = 0; i <x.vllValue.length(); ++i) {
			
			vllValue.push_back(x.vllValue[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		vvllValueExists = x.vvllValueExists;
		if (x.vvllValueExists) {
		
		
			
		vvllValue .clear();
        
        vector<int64_t> v_aux_vvllValue;
        
		for (unsigned int i = 0; i < x.vvllValue.length(); ++i) {
			v_aux_vvllValue.clear();
			for (unsigned int j = 0; j < x.vvllValue[0].length(); ++j) {
				
				v_aux_vvllValue.push_back(x.vvllValue[i][j]);
	  			
  			}
  			vvllValue.push_back(v_aux_vvllValue);			
		}
			
  		
		
		}
		
	

	
		
		sValueExists = x.sValueExists;
		if (x.sValueExists) {
		
		
			
		setSValue(string (x.sValue));
			
 		
		
		}
		
	

	
	
		
	
		
		antennaIdExists = x.antennaIdExists;
		if (x.antennaIdExists) {
		
		antennaId .clear();
		for (unsigned int i = 0; i <x.antennaId.length(); ++i) {
			
			antennaId.push_back(Tag (x.antennaId[i]));
			
		}
		
		}
		
  	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Annotation");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string AnnotationRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(annotationId, "annotationId", buf);
		
		
	

  	
 		
		
		Parser::toXML(time, "time", buf);
		
		
	

  	
 		
		
		Parser::toXML(issue, "issue", buf);
		
		
	

  	
 		
		
		Parser::toXML(details, "details", buf);
		
		
	

  	
 		
		if (numAntennaExists) {
		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
		}
		
	

  	
 		
		if (basebandNameExists) {
		
		
			buf.append(EnumerationParser::toXML("basebandName", basebandName));
		
		
		}
		
	

  	
 		
		if (numBasebandExists) {
		
		
		Parser::toXML(numBaseband, "numBaseband", buf);
		
		
		}
		
	

  	
 		
		if (intervalExists) {
		
		
		Parser::toXML(interval, "interval", buf);
		
		
		}
		
	

  	
 		
		if (dValueExists) {
		
		
		Parser::toXML(dValue, "dValue", buf);
		
		
		}
		
	

  	
 		
		if (vdValueExists) {
		
		
		Parser::toXML(vdValue, "vdValue", buf);
		
		
		}
		
	

  	
 		
		if (vvdValuesExists) {
		
		
		Parser::toXML(vvdValues, "vvdValues", buf);
		
		
		}
		
	

  	
 		
		if (llValueExists) {
		
		
		Parser::toXML(llValue, "llValue", buf);
		
		
		}
		
	

  	
 		
		if (vllValueExists) {
		
		
		Parser::toXML(vllValue, "vllValue", buf);
		
		
		}
		
	

  	
 		
		if (vvllValueExists) {
		
		
		Parser::toXML(vvllValue, "vvllValue", buf);
		
		
		}
		
	

  	
 		
		if (sValueExists) {
		
		
		Parser::toXML(sValue, "sValue", buf);
		
		
		}
		
	

	
	
		
  	
 		
		if (antennaIdExists) {
		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
		}
		
	

	
		
	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void AnnotationRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setAnnotationId(Parser::getTag("annotationId","Annotation",rowDoc));
			
		
	

	
  		
			
	  	setTime(Parser::getArrayTime("time","Annotation",rowDoc));
			
		
	

	
  		
			
	  	setIssue(Parser::getString("issue","Annotation",rowDoc));
			
		
	

	
  		
			
	  	setDetails(Parser::getString("details","Annotation",rowDoc));
			
		
	

	
  		
        if (row.isStr("<numAntenna>")) {
			
	  		setNumAntenna(Parser::getInteger("numAntenna","Annotation",rowDoc));
			
		}
 		
	

	
		
	if (row.isStr("<basebandName>")) {
		
		
		
		basebandName = EnumerationParser::getBasebandName1D("basebandName","Annotation",rowDoc);			
		
		
		
		basebandNameExists = true;
	}
		
	

	
  		
        if (row.isStr("<numBaseband>")) {
			
	  		setNumBaseband(Parser::getInteger("numBaseband","Annotation",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<interval>")) {
			
	  		setInterval(Parser::getInterval("interval","Annotation",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<dValue>")) {
			
	  		setDValue(Parser::getDouble("dValue","Annotation",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<vdValue>")) {
			
								
	  		setVdValue(Parser::get1DDouble("vdValue","Annotation",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<vvdValues>")) {
			
								
	  		setVvdValues(Parser::get2DDouble("vvdValues","Annotation",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<llValue>")) {
			
	  		setLlValue(Parser::getLong("llValue","Annotation",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<vllValue>")) {
			
								
	  		setVllValue(Parser::get1DLong("vllValue","Annotation",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<vvllValue>")) {
			
								
	  		setVvllValue(Parser::get2DLong("vvllValue","Annotation",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<sValue>")) {
			
	  		setSValue(Parser::getString("sValue","Annotation",rowDoc));
			
		}
 		
	

	
	
		
	
  		
  		if (row.isStr("<antennaId>")) {
  			setAntennaId(Parser::get1DTag("antennaId","Annotation",rowDoc));  		
  		}
  		
  	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Annotation");
		}
	}
	
	void AnnotationRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	annotationId.toBin(eoss);
		
	

	
	
		
	time.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(issue);
				
		
	

	
	
		
						
			eoss.writeString(details);
				
		
	


	
	
	eoss.writeBoolean(numAntennaExists);
	if (numAntennaExists) {
	
	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	}

	eoss.writeBoolean(basebandNameExists);
	if (basebandNameExists) {
	
	
	
		
		
			
		eoss.writeInt((int) basebandName.size());
		for (unsigned int i = 0; i < basebandName.size(); i++)
				
			eoss.writeString(CBasebandName::name(basebandName.at(i)));
			/* eoss.writeInt(basebandName.at(i)); */
				
				
						
		
	

	}

	eoss.writeBoolean(numBasebandExists);
	if (numBasebandExists) {
	
	
	
		
						
			eoss.writeInt(numBaseband);
				
		
	

	}

	eoss.writeBoolean(intervalExists);
	if (intervalExists) {
	
	
	
		
	interval.toBin(eoss);
		
	

	}

	eoss.writeBoolean(dValueExists);
	if (dValueExists) {
	
	
	
		
						
			eoss.writeDouble(dValue);
				
		
	

	}

	eoss.writeBoolean(vdValueExists);
	if (vdValueExists) {
	
	
	
		
		
			
		eoss.writeInt((int) vdValue.size());
		for (unsigned int i = 0; i < vdValue.size(); i++)
				
			eoss.writeDouble(vdValue.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(vvdValuesExists);
	if (vvdValuesExists) {
	
	
	
		
		
			
		eoss.writeInt((int) vvdValues.size());
		eoss.writeInt((int) vvdValues.at(0).size());
		for (unsigned int i = 0; i < vvdValues.size(); i++) 
			for (unsigned int j = 0;  j < vvdValues.at(0).size(); j++) 
							 
				eoss.writeDouble(vvdValues.at(i).at(j));
				
	
						
		
	

	}

	eoss.writeBoolean(llValueExists);
	if (llValueExists) {
	
	
	
		
						
			eoss.writeLong(llValue);
				
		
	

	}

	eoss.writeBoolean(vllValueExists);
	if (vllValueExists) {
	
	
	
		
		
			
		eoss.writeInt((int) vllValue.size());
		for (unsigned int i = 0; i < vllValue.size(); i++)
				
			eoss.writeLong(vllValue.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(vvllValueExists);
	if (vvllValueExists) {
	
	
	
		
		
			
		eoss.writeInt((int) vvllValue.size());
		eoss.writeInt((int) vvllValue.at(0).size());
		for (unsigned int i = 0; i < vvllValue.size(); i++) 
			for (unsigned int j = 0;  j < vvllValue.at(0).size(); j++) 
							 
				eoss.writeLong(vvllValue.at(i).at(j));
				
	
						
		
	

	}

	eoss.writeBoolean(sValueExists);
	if (sValueExists) {
	
	
	
		
						
			eoss.writeString(sValue);
				
		
	

	}

	eoss.writeBoolean(antennaIdExists);
	if (antennaIdExists) {
	
	
	
		
	Tag::toBin(antennaId, eoss);
		
	

	}

	}
	
void AnnotationRow::annotationIdFromBin(EndianIStream& eis) {
		
	
		
		
		annotationId =  Tag::fromBin(eis);
		
	
	
}
void AnnotationRow::timeFromBin(EndianIStream& eis) {
		
	
		
		
		time =  ArrayTime::fromBin(eis);
		
	
	
}
void AnnotationRow::issueFromBin(EndianIStream& eis) {
		
	
	
		
			
		issue =  eis.readString();
			
		
	
	
}
void AnnotationRow::detailsFromBin(EndianIStream& eis) {
		
	
	
		
			
		details =  eis.readString();
			
		
	
	
}

void AnnotationRow::numAntennaFromBin(EndianIStream& eis) {
		
	numAntennaExists = eis.readBoolean();
	if (numAntennaExists) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	

	}
	
}
void AnnotationRow::basebandNameFromBin(EndianIStream& eis) {
		
	basebandNameExists = eis.readBoolean();
	if (basebandNameExists) {
		
	
	
		
			
	
		basebandName.clear();
		
		unsigned int basebandNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < basebandNameDim1; i++)
			
			basebandName.push_back(CBasebandName::literal(eis.readString()));
			
	

		
	

	}
	
}
void AnnotationRow::numBasebandFromBin(EndianIStream& eis) {
		
	numBasebandExists = eis.readBoolean();
	if (numBasebandExists) {
		
	
	
		
			
		numBaseband =  eis.readInt();
			
		
	

	}
	
}
void AnnotationRow::intervalFromBin(EndianIStream& eis) {
		
	intervalExists = eis.readBoolean();
	if (intervalExists) {
		
	
		
		
		interval =  Interval::fromBin(eis);
		
	

	}
	
}
void AnnotationRow::dValueFromBin(EndianIStream& eis) {
		
	dValueExists = eis.readBoolean();
	if (dValueExists) {
		
	
	
		
			
		dValue =  eis.readDouble();
			
		
	

	}
	
}
void AnnotationRow::vdValueFromBin(EndianIStream& eis) {
		
	vdValueExists = eis.readBoolean();
	if (vdValueExists) {
		
	
	
		
			
	
		vdValue.clear();
		
		unsigned int vdValueDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < vdValueDim1; i++)
			
			vdValue.push_back(eis.readDouble());
			
	

		
	

	}
	
}
void AnnotationRow::vvdValuesFromBin(EndianIStream& eis) {
		
	vvdValuesExists = eis.readBoolean();
	if (vvdValuesExists) {
		
	
	
		
			
	
		vvdValues.clear();
		
		unsigned int vvdValuesDim1 = eis.readInt();
		unsigned int vvdValuesDim2 = eis.readInt();
        
		vector <double> vvdValuesAux1;
        
		for (unsigned int i = 0; i < vvdValuesDim1; i++) {
			vvdValuesAux1.clear();
			for (unsigned int j = 0; j < vvdValuesDim2 ; j++)			
			
			vvdValuesAux1.push_back(eis.readDouble());
			
			vvdValues.push_back(vvdValuesAux1);
		}
	
	

		
	

	}
	
}
void AnnotationRow::llValueFromBin(EndianIStream& eis) {
		
	llValueExists = eis.readBoolean();
	if (llValueExists) {
		
	
	
		
			
		llValue =  eis.readLong();
			
		
	

	}
	
}
void AnnotationRow::vllValueFromBin(EndianIStream& eis) {
		
	vllValueExists = eis.readBoolean();
	if (vllValueExists) {
		
	
	
		
			
	
		vllValue.clear();
		
		unsigned int vllValueDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < vllValueDim1; i++)
			
			vllValue.push_back(eis.readLong());
			
	

		
	

	}
	
}
void AnnotationRow::vvllValueFromBin(EndianIStream& eis) {
		
	vvllValueExists = eis.readBoolean();
	if (vvllValueExists) {
		
	
	
		
			
	
		vvllValue.clear();
		
		unsigned int vvllValueDim1 = eis.readInt();
		unsigned int vvllValueDim2 = eis.readInt();
        
		vector <int64_t> vvllValueAux1;
        
		for (unsigned int i = 0; i < vvllValueDim1; i++) {
			vvllValueAux1.clear();
			for (unsigned int j = 0; j < vvllValueDim2 ; j++)			
			
			vvllValueAux1.push_back(eis.readLong());
			
			vvllValue.push_back(vvllValueAux1);
		}
	
	

		
	

	}
	
}
void AnnotationRow::sValueFromBin(EndianIStream& eis) {
		
	sValueExists = eis.readBoolean();
	if (sValueExists) {
		
	
	
		
			
		sValue =  eis.readString();
			
		
	

	}
	
}
void AnnotationRow::antennaIdFromBin(EndianIStream& eis) {
		
	antennaIdExists = eis.readBoolean();
	if (antennaIdExists) {
		
	
		
		
			
	
	antennaId = Tag::from1DBin(eis);	
	

		
	

	}
	
}
	
	
	AnnotationRow* AnnotationRow::fromBin(EndianIStream& eis, AnnotationTable& table, const vector<string>& attributesSeq) {
		AnnotationRow* row = new  AnnotationRow(table);
		
		map<string, AnnotationAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "AnnotationTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void AnnotationRow::annotationIdFromText(const string & s) {
		 
          
		annotationId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void AnnotationRow::timeFromText(const string & s) {
		 
          
		time = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void AnnotationRow::issueFromText(const string & s) {
		 
          
		issue = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void AnnotationRow::detailsFromText(const string & s) {
		 
          
		details = ASDMValuesParser::parse<string>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void AnnotationRow::numAntennaFromText(const string & s) {
		numAntennaExists = true;
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an BasebandName 
	void AnnotationRow::basebandNameFromText(const string & s) {
		basebandNameExists = true;
		 
          
		basebandName = ASDMValuesParser::parse1D<BasebandNameMod::BasebandName>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void AnnotationRow::numBasebandFromText(const string & s) {
		numBasebandExists = true;
		 
          
		numBaseband = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Interval 
	void AnnotationRow::intervalFromText(const string & s) {
		intervalExists = true;
		 
          
		interval = ASDMValuesParser::parse<Interval>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void AnnotationRow::dValueFromText(const string & s) {
		dValueExists = true;
		 
          
		dValue = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void AnnotationRow::vdValueFromText(const string & s) {
		vdValueExists = true;
		 
          
		vdValue = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void AnnotationRow::vvdValuesFromText(const string & s) {
		vvdValuesExists = true;
		 
          
		vvdValues = ASDMValuesParser::parse2D<double>(s);
          
		
	}
	
	
	// Convert a string into an long 
	void AnnotationRow::llValueFromText(const string & s) {
		llValueExists = true;
		 
          
		llValue = ASDMValuesParser::parse<int64_t>(s);
          
		
	}
	
	
	// Convert a string into an long 
	void AnnotationRow::vllValueFromText(const string & s) {
		vllValueExists = true;
		 
          
		vllValue = ASDMValuesParser::parse1D<int64_t>(s);
          
		
	}
	
	
	// Convert a string into an long 
	void AnnotationRow::vvllValueFromText(const string & s) {
		vvllValueExists = true;
		 
          
		vvllValue = ASDMValuesParser::parse2D<int64_t>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void AnnotationRow::sValueFromText(const string & s) {
		sValueExists = true;
		 
          
		sValue = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void AnnotationRow::antennaIdFromText(const string & s) {
		antennaIdExists = true;
		 
          
		antennaId = ASDMValuesParser::parse1D<Tag>(s);
          
		
	}
	
	
	
	void AnnotationRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, AnnotationAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "AnnotationTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get annotationId.
 	 * @return annotationId as Tag
 	 */
 	Tag AnnotationRow::getAnnotationId() const {
	
  		return annotationId;
 	}

 	/**
 	 * Set annotationId with the specified Tag.
 	 * @param annotationId The Tag value to which annotationId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void AnnotationRow::setAnnotationId (Tag annotationId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("annotationId", "Annotation");
		
  		}
  	
 		this->annotationId = annotationId;
	
 	}
	
	

	

	
 	/**
 	 * Get time.
 	 * @return time as ArrayTime
 	 */
 	ArrayTime AnnotationRow::getTime() const {
	
  		return time;
 	}

 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 	
 		
 	 */
 	void AnnotationRow::setTime (ArrayTime time)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->time = time;
	
 	}
	
	

	

	
 	/**
 	 * Get issue.
 	 * @return issue as std::string
 	 */
 	std::string AnnotationRow::getIssue() const {
	
  		return issue;
 	}

 	/**
 	 * Set issue with the specified std::string.
 	 * @param issue The std::string value to which issue is to be set.
 	 
 	
 		
 	 */
 	void AnnotationRow::setIssue (std::string issue)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->issue = issue;
	
 	}
	
	

	

	
 	/**
 	 * Get details.
 	 * @return details as std::string
 	 */
 	std::string AnnotationRow::getDetails() const {
	
  		return details;
 	}

 	/**
 	 * Set details with the specified std::string.
 	 * @param details The std::string value to which details is to be set.
 	 
 	
 		
 	 */
 	void AnnotationRow::setDetails (std::string details)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->details = details;
	
 	}
	
	

	
	/**
	 * The attribute numAntenna is optional. Return true if this attribute exists.
	 * @return true if and only if the numAntenna attribute exists. 
	 */
	bool AnnotationRow::isNumAntennaExists() const {
		return numAntennaExists;
	}
	

	
 	/**
 	 * Get numAntenna, which is optional.
 	 * @return numAntenna as int
 	 * @throw IllegalAccessException If numAntenna does not exist.
 	 */
 	int AnnotationRow::getNumAntenna() const  {
		if (!numAntennaExists) {
			throw IllegalAccessException("numAntenna", "Annotation");
		}
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setNumAntenna (int numAntenna) {
	
 		this->numAntenna = numAntenna;
	
		numAntennaExists = true;
	
 	}
	
	
	/**
	 * Mark numAntenna, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearNumAntenna () {
		numAntennaExists = false;
	}
	

	
	/**
	 * The attribute basebandName is optional. Return true if this attribute exists.
	 * @return true if and only if the basebandName attribute exists. 
	 */
	bool AnnotationRow::isBasebandNameExists() const {
		return basebandNameExists;
	}
	

	
 	/**
 	 * Get basebandName, which is optional.
 	 * @return basebandName as std::vector<BasebandNameMod::BasebandName >
 	 * @throw IllegalAccessException If basebandName does not exist.
 	 */
 	std::vector<BasebandNameMod::BasebandName > AnnotationRow::getBasebandName() const  {
		if (!basebandNameExists) {
			throw IllegalAccessException("basebandName", "Annotation");
		}
	
  		return basebandName;
 	}

 	/**
 	 * Set basebandName with the specified std::vector<BasebandNameMod::BasebandName >.
 	 * @param basebandName The std::vector<BasebandNameMod::BasebandName > value to which basebandName is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setBasebandName (std::vector<BasebandNameMod::BasebandName > basebandName) {
	
 		this->basebandName = basebandName;
	
		basebandNameExists = true;
	
 	}
	
	
	/**
	 * Mark basebandName, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearBasebandName () {
		basebandNameExists = false;
	}
	

	
	/**
	 * The attribute numBaseband is optional. Return true if this attribute exists.
	 * @return true if and only if the numBaseband attribute exists. 
	 */
	bool AnnotationRow::isNumBasebandExists() const {
		return numBasebandExists;
	}
	

	
 	/**
 	 * Get numBaseband, which is optional.
 	 * @return numBaseband as int
 	 * @throw IllegalAccessException If numBaseband does not exist.
 	 */
 	int AnnotationRow::getNumBaseband() const  {
		if (!numBasebandExists) {
			throw IllegalAccessException("numBaseband", "Annotation");
		}
	
  		return numBaseband;
 	}

 	/**
 	 * Set numBaseband with the specified int.
 	 * @param numBaseband The int value to which numBaseband is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setNumBaseband (int numBaseband) {
	
 		this->numBaseband = numBaseband;
	
		numBasebandExists = true;
	
 	}
	
	
	/**
	 * Mark numBaseband, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearNumBaseband () {
		numBasebandExists = false;
	}
	

	
	/**
	 * The attribute interval is optional. Return true if this attribute exists.
	 * @return true if and only if the interval attribute exists. 
	 */
	bool AnnotationRow::isIntervalExists() const {
		return intervalExists;
	}
	

	
 	/**
 	 * Get interval, which is optional.
 	 * @return interval as Interval
 	 * @throw IllegalAccessException If interval does not exist.
 	 */
 	Interval AnnotationRow::getInterval() const  {
		if (!intervalExists) {
			throw IllegalAccessException("interval", "Annotation");
		}
	
  		return interval;
 	}

 	/**
 	 * Set interval with the specified Interval.
 	 * @param interval The Interval value to which interval is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setInterval (Interval interval) {
	
 		this->interval = interval;
	
		intervalExists = true;
	
 	}
	
	
	/**
	 * Mark interval, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearInterval () {
		intervalExists = false;
	}
	

	
	/**
	 * The attribute dValue is optional. Return true if this attribute exists.
	 * @return true if and only if the dValue attribute exists. 
	 */
	bool AnnotationRow::isDValueExists() const {
		return dValueExists;
	}
	

	
 	/**
 	 * Get dValue, which is optional.
 	 * @return dValue as double
 	 * @throw IllegalAccessException If dValue does not exist.
 	 */
 	double AnnotationRow::getDValue() const  {
		if (!dValueExists) {
			throw IllegalAccessException("dValue", "Annotation");
		}
	
  		return dValue;
 	}

 	/**
 	 * Set dValue with the specified double.
 	 * @param dValue The double value to which dValue is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setDValue (double dValue) {
	
 		this->dValue = dValue;
	
		dValueExists = true;
	
 	}
	
	
	/**
	 * Mark dValue, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearDValue () {
		dValueExists = false;
	}
	

	
	/**
	 * The attribute vdValue is optional. Return true if this attribute exists.
	 * @return true if and only if the vdValue attribute exists. 
	 */
	bool AnnotationRow::isVdValueExists() const {
		return vdValueExists;
	}
	

	
 	/**
 	 * Get vdValue, which is optional.
 	 * @return vdValue as std::vector<double >
 	 * @throw IllegalAccessException If vdValue does not exist.
 	 */
 	std::vector<double > AnnotationRow::getVdValue() const  {
		if (!vdValueExists) {
			throw IllegalAccessException("vdValue", "Annotation");
		}
	
  		return vdValue;
 	}

 	/**
 	 * Set vdValue with the specified std::vector<double >.
 	 * @param vdValue The std::vector<double > value to which vdValue is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setVdValue (std::vector<double > vdValue) {
	
 		this->vdValue = vdValue;
	
		vdValueExists = true;
	
 	}
	
	
	/**
	 * Mark vdValue, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearVdValue () {
		vdValueExists = false;
	}
	

	
	/**
	 * The attribute vvdValues is optional. Return true if this attribute exists.
	 * @return true if and only if the vvdValues attribute exists. 
	 */
	bool AnnotationRow::isVvdValuesExists() const {
		return vvdValuesExists;
	}
	

	
 	/**
 	 * Get vvdValues, which is optional.
 	 * @return vvdValues as std::vector<std::vector<double > >
 	 * @throw IllegalAccessException If vvdValues does not exist.
 	 */
 	std::vector<std::vector<double > > AnnotationRow::getVvdValues() const  {
		if (!vvdValuesExists) {
			throw IllegalAccessException("vvdValues", "Annotation");
		}
	
  		return vvdValues;
 	}

 	/**
 	 * Set vvdValues with the specified std::vector<std::vector<double > >.
 	 * @param vvdValues The std::vector<std::vector<double > > value to which vvdValues is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setVvdValues (std::vector<std::vector<double > > vvdValues) {
	
 		this->vvdValues = vvdValues;
	
		vvdValuesExists = true;
	
 	}
	
	
	/**
	 * Mark vvdValues, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearVvdValues () {
		vvdValuesExists = false;
	}
	

	
	/**
	 * The attribute llValue is optional. Return true if this attribute exists.
	 * @return true if and only if the llValue attribute exists. 
	 */
	bool AnnotationRow::isLlValueExists() const {
		return llValueExists;
	}
	

	
 	/**
 	 * Get llValue, which is optional.
 	 * @return llValue as int64_t
 	 * @throw IllegalAccessException If llValue does not exist.
 	 */
 	int64_t AnnotationRow::getLlValue() const  {
		if (!llValueExists) {
			throw IllegalAccessException("llValue", "Annotation");
		}
	
  		return llValue;
 	}

 	/**
 	 * Set llValue with the specified int64_t.
 	 * @param llValue The int64_t value to which llValue is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setLlValue (int64_t llValue) {
	
 		this->llValue = llValue;
	
		llValueExists = true;
	
 	}
	
	
	/**
	 * Mark llValue, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearLlValue () {
		llValueExists = false;
	}
	

	
	/**
	 * The attribute vllValue is optional. Return true if this attribute exists.
	 * @return true if and only if the vllValue attribute exists. 
	 */
	bool AnnotationRow::isVllValueExists() const {
		return vllValueExists;
	}
	

	
 	/**
 	 * Get vllValue, which is optional.
 	 * @return vllValue as std::vector<int64_t >
 	 * @throw IllegalAccessException If vllValue does not exist.
 	 */
 	std::vector<int64_t > AnnotationRow::getVllValue() const  {
		if (!vllValueExists) {
			throw IllegalAccessException("vllValue", "Annotation");
		}
	
  		return vllValue;
 	}

 	/**
 	 * Set vllValue with the specified std::vector<int64_t >.
 	 * @param vllValue The std::vector<int64_t > value to which vllValue is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setVllValue (std::vector<int64_t > vllValue) {
	
 		this->vllValue = vllValue;
	
		vllValueExists = true;
	
 	}
	
	
	/**
	 * Mark vllValue, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearVllValue () {
		vllValueExists = false;
	}
	

	
	/**
	 * The attribute vvllValue is optional. Return true if this attribute exists.
	 * @return true if and only if the vvllValue attribute exists. 
	 */
	bool AnnotationRow::isVvllValueExists() const {
		return vvllValueExists;
	}
	

	
 	/**
 	 * Get vvllValue, which is optional.
 	 * @return vvllValue as std::vector<std::vector<int64_t > >
 	 * @throw IllegalAccessException If vvllValue does not exist.
 	 */
 	std::vector<std::vector<int64_t > > AnnotationRow::getVvllValue() const  {
		if (!vvllValueExists) {
			throw IllegalAccessException("vvllValue", "Annotation");
		}
	
  		return vvllValue;
 	}

 	/**
 	 * Set vvllValue with the specified std::vector<std::vector<int64_t > >.
 	 * @param vvllValue The std::vector<std::vector<int64_t > > value to which vvllValue is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setVvllValue (std::vector<std::vector<int64_t > > vvllValue) {
	
 		this->vvllValue = vvllValue;
	
		vvllValueExists = true;
	
 	}
	
	
	/**
	 * Mark vvllValue, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearVvllValue () {
		vvllValueExists = false;
	}
	

	
	/**
	 * The attribute sValue is optional. Return true if this attribute exists.
	 * @return true if and only if the sValue attribute exists. 
	 */
	bool AnnotationRow::isSValueExists() const {
		return sValueExists;
	}
	

	
 	/**
 	 * Get sValue, which is optional.
 	 * @return sValue as std::string
 	 * @throw IllegalAccessException If sValue does not exist.
 	 */
 	std::string AnnotationRow::getSValue() const  {
		if (!sValueExists) {
			throw IllegalAccessException("sValue", "Annotation");
		}
	
  		return sValue;
 	}

 	/**
 	 * Set sValue with the specified std::string.
 	 * @param sValue The std::string value to which sValue is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setSValue (std::string sValue) {
	
 		this->sValue = sValue;
	
		sValueExists = true;
	
 	}
	
	
	/**
	 * Mark sValue, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearSValue () {
		sValueExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	
	/**
	 * The attribute antennaId is optional. Return true if this attribute exists.
	 * @return true if and only if the antennaId attribute exists. 
	 */
	bool AnnotationRow::isAntennaIdExists() const {
		return antennaIdExists;
	}
	

	
 	/**
 	 * Get antennaId, which is optional.
 	 * @return antennaId as std::vector<Tag> 
 	 * @throw IllegalAccessException If antennaId does not exist.
 	 */
 	std::vector<Tag>  AnnotationRow::getAntennaId() const  {
		if (!antennaIdExists) {
			throw IllegalAccessException("antennaId", "Annotation");
		}
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified std::vector<Tag> .
 	 * @param antennaId The std::vector<Tag>  value to which antennaId is to be set.
 	 
 	
 	 */
 	void AnnotationRow::setAntennaId (std::vector<Tag>  antennaId) {
	
 		this->antennaId = antennaId;
	
		antennaIdExists = true;
	
 	}
	
	
	/**
	 * Mark antennaId, which is an optional field, as non-existent.
	 */
	void AnnotationRow::clearAntennaId () {
		antennaIdExists = false;
	}
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
 		
 	/**
 	 * Set antennaId[i] with the specified Tag.
 	 * @param i The index in antennaId where to set the Tag value.
 	 * @param antennaId The Tag value to which antennaId[i] is to be set. 
 	 * @throws OutOfBoundsException
  	 */
  	void AnnotationRow::setAntennaId (int i, Tag antennaId) {
  		if ((i < 0) || (i > ((int) this->antennaId.size())))
  			throw OutOfBoundsException("Index out of bounds during a set operation on attribute antennaId in table AnnotationTable");
  		std::vector<Tag> ::iterator iter = this->antennaId.begin();
  		int j = 0;
  		while (j < i) {
  			j++; iter++;
  		}
  		this->antennaId.insert(this->antennaId.erase(iter), antennaId); 	
  	}
 			
	
	
	
		
/**
 * Append a Tag to antennaId.
 * @param id the Tag to be appended to antennaId
 */
 void AnnotationRow::addAntennaId(Tag id){
 	antennaId.push_back(id);
}

/**
 * Append an array of Tag to antennaId.
 * @param id an array of Tag to be appended to antennaId
 */
 void AnnotationRow::addAntennaId(const std::vector<Tag> & id) {
 	for (unsigned int i=0; i < id.size(); i++)
 		antennaId.push_back(id.at(i));
 }
 

 /**
  * Returns the Tag stored in antennaId at position i.
  *
  */
 const Tag AnnotationRow::getAntennaId(int i) {
 	return antennaId.at(i);
 }
 
 /**
  * Returns the AntennaRow linked to this row via the Tag stored in antennaId
  * at position i.
  */
 AntennaRow* AnnotationRow::getAntennaUsingAntennaId(int i) {
 	return table.getContainer().getAntenna().getRowByKey(antennaId.at(i));
 } 
 
 /**
  * Returns the vector of AntennaRow* linked to this row via the Tags stored in antennaId
  *
  */
 vector<AntennaRow *> AnnotationRow::getAntennasUsingAntennaId() {
 	vector<AntennaRow *> result;
 	for (unsigned int i = 0; i < antennaId.size(); i++)
 		result.push_back(table.getContainer().getAntenna().getRowByKey(antennaId.at(i)));
 		
 	return result;
 }
  

	

	
	/**
	 * Create a AnnotationRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	AnnotationRow::AnnotationRow (AnnotationTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	
		numAntennaExists = false;
	

	
		basebandNameExists = false;
	

	
		numBasebandExists = false;
	

	
		intervalExists = false;
	

	
		dValueExists = false;
	

	
		vdValueExists = false;
	

	
		vvdValuesExists = false;
	

	
		llValueExists = false;
	

	
		vllValueExists = false;
	

	
		vvllValueExists = false;
	

	
		sValueExists = false;
	

	
	
		antennaIdExists = false;
	

	
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["annotationId"] = &AnnotationRow::annotationIdFromBin; 
	 fromBinMethods["time"] = &AnnotationRow::timeFromBin; 
	 fromBinMethods["issue"] = &AnnotationRow::issueFromBin; 
	 fromBinMethods["details"] = &AnnotationRow::detailsFromBin; 
		
	
	 fromBinMethods["numAntenna"] = &AnnotationRow::numAntennaFromBin; 
	 fromBinMethods["basebandName"] = &AnnotationRow::basebandNameFromBin; 
	 fromBinMethods["numBaseband"] = &AnnotationRow::numBasebandFromBin; 
	 fromBinMethods["interval"] = &AnnotationRow::intervalFromBin; 
	 fromBinMethods["dValue"] = &AnnotationRow::dValueFromBin; 
	 fromBinMethods["vdValue"] = &AnnotationRow::vdValueFromBin; 
	 fromBinMethods["vvdValues"] = &AnnotationRow::vvdValuesFromBin; 
	 fromBinMethods["llValue"] = &AnnotationRow::llValueFromBin; 
	 fromBinMethods["vllValue"] = &AnnotationRow::vllValueFromBin; 
	 fromBinMethods["vvllValue"] = &AnnotationRow::vvllValueFromBin; 
	 fromBinMethods["sValue"] = &AnnotationRow::sValueFromBin; 
	 fromBinMethods["antennaId"] = &AnnotationRow::antennaIdFromBin; 
	
	
	
	
				 
	fromTextMethods["annotationId"] = &AnnotationRow::annotationIdFromText;
		 
	
				 
	fromTextMethods["time"] = &AnnotationRow::timeFromText;
		 
	
				 
	fromTextMethods["issue"] = &AnnotationRow::issueFromText;
		 
	
				 
	fromTextMethods["details"] = &AnnotationRow::detailsFromText;
		 
	

	 
				
	fromTextMethods["numAntenna"] = &AnnotationRow::numAntennaFromText;
		 	
	 
				
	fromTextMethods["basebandName"] = &AnnotationRow::basebandNameFromText;
		 	
	 
				
	fromTextMethods["numBaseband"] = &AnnotationRow::numBasebandFromText;
		 	
	 
				
	fromTextMethods["interval"] = &AnnotationRow::intervalFromText;
		 	
	 
				
	fromTextMethods["dValue"] = &AnnotationRow::dValueFromText;
		 	
	 
				
	fromTextMethods["vdValue"] = &AnnotationRow::vdValueFromText;
		 	
	 
				
	fromTextMethods["vvdValues"] = &AnnotationRow::vvdValuesFromText;
		 	
	 
				
	fromTextMethods["llValue"] = &AnnotationRow::llValueFromText;
		 	
	 
				
	fromTextMethods["vllValue"] = &AnnotationRow::vllValueFromText;
		 	
	 
				
	fromTextMethods["vvllValue"] = &AnnotationRow::vvllValueFromText;
		 	
	 
				
	fromTextMethods["sValue"] = &AnnotationRow::sValueFromText;
		 	
	 
				
	fromTextMethods["antennaId"] = &AnnotationRow::antennaIdFromText;
		 	
		
	}
	
	AnnotationRow::AnnotationRow (AnnotationTable &t, AnnotationRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	
		numAntennaExists = false;
	

	
		basebandNameExists = false;
	

	
		numBasebandExists = false;
	

	
		intervalExists = false;
	

	
		dValueExists = false;
	

	
		vdValueExists = false;
	

	
		vvdValuesExists = false;
	

	
		llValueExists = false;
	

	
		vllValueExists = false;
	

	
		vvllValueExists = false;
	

	
		sValueExists = false;
	

	
	
		antennaIdExists = false;
	
		
		}
		else {
	
		
			annotationId = row->annotationId;
		
		
		
		
			time = row->time;
		
			issue = row->issue;
		
			details = row->details;
		
		
		
		
		if (row->numAntennaExists) {
			numAntenna = row->numAntenna;		
			numAntennaExists = true;
		}
		else
			numAntennaExists = false;
		
		if (row->basebandNameExists) {
			basebandName = row->basebandName;		
			basebandNameExists = true;
		}
		else
			basebandNameExists = false;
		
		if (row->numBasebandExists) {
			numBaseband = row->numBaseband;		
			numBasebandExists = true;
		}
		else
			numBasebandExists = false;
		
		if (row->intervalExists) {
			interval = row->interval;		
			intervalExists = true;
		}
		else
			intervalExists = false;
		
		if (row->dValueExists) {
			dValue = row->dValue;		
			dValueExists = true;
		}
		else
			dValueExists = false;
		
		if (row->vdValueExists) {
			vdValue = row->vdValue;		
			vdValueExists = true;
		}
		else
			vdValueExists = false;
		
		if (row->vvdValuesExists) {
			vvdValues = row->vvdValues;		
			vvdValuesExists = true;
		}
		else
			vvdValuesExists = false;
		
		if (row->llValueExists) {
			llValue = row->llValue;		
			llValueExists = true;
		}
		else
			llValueExists = false;
		
		if (row->vllValueExists) {
			vllValue = row->vllValue;		
			vllValueExists = true;
		}
		else
			vllValueExists = false;
		
		if (row->vvllValueExists) {
			vvllValue = row->vvllValue;		
			vvllValueExists = true;
		}
		else
			vvllValueExists = false;
		
		if (row->sValueExists) {
			sValue = row->sValue;		
			sValueExists = true;
		}
		else
			sValueExists = false;
		
		if (row->antennaIdExists) {
			antennaId = row->antennaId;		
			antennaIdExists = true;
		}
		else
			antennaIdExists = false;
		
		}
		
		 fromBinMethods["annotationId"] = &AnnotationRow::annotationIdFromBin; 
		 fromBinMethods["time"] = &AnnotationRow::timeFromBin; 
		 fromBinMethods["issue"] = &AnnotationRow::issueFromBin; 
		 fromBinMethods["details"] = &AnnotationRow::detailsFromBin; 
			
	
		 fromBinMethods["numAntenna"] = &AnnotationRow::numAntennaFromBin; 
		 fromBinMethods["basebandName"] = &AnnotationRow::basebandNameFromBin; 
		 fromBinMethods["numBaseband"] = &AnnotationRow::numBasebandFromBin; 
		 fromBinMethods["interval"] = &AnnotationRow::intervalFromBin; 
		 fromBinMethods["dValue"] = &AnnotationRow::dValueFromBin; 
		 fromBinMethods["vdValue"] = &AnnotationRow::vdValueFromBin; 
		 fromBinMethods["vvdValues"] = &AnnotationRow::vvdValuesFromBin; 
		 fromBinMethods["llValue"] = &AnnotationRow::llValueFromBin; 
		 fromBinMethods["vllValue"] = &AnnotationRow::vllValueFromBin; 
		 fromBinMethods["vvllValue"] = &AnnotationRow::vvllValueFromBin; 
		 fromBinMethods["sValue"] = &AnnotationRow::sValueFromBin; 
		 fromBinMethods["antennaId"] = &AnnotationRow::antennaIdFromBin; 
			
	}

	
	bool AnnotationRow::compareNoAutoInc(ArrayTime time, std::string issue, std::string details) {
		bool result;
		result = true;
		
	
		
		result = result && (this->time == time);
		
		if (!result) return false;
	

	
		
		result = result && (this->issue == issue);
		
		if (!result) return false;
	

	
		
		result = result && (this->details == details);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool AnnotationRow::compareRequiredValue(ArrayTime time, std::string issue, std::string details) {
		bool result;
		result = true;
		
	
		if (!(this->time == time)) return false;
	

	
		if (!(this->issue == issue)) return false;
	

	
		if (!(this->details == details)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the AnnotationRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool AnnotationRow::equalByRequiredValue(AnnotationRow*  x ) {
		
			
		if (this->time != x->time) return false;
			
		if (this->issue != x->issue) return false;
			
		if (this->details != x->details) return false;
			
		
		return true;
	}	
	
/*
	 map<string, AnnotationAttributeFromBin> AnnotationRow::initFromBinMethods() {
		map<string, AnnotationAttributeFromBin> result;
		
		result["annotationId"] = &AnnotationRow::annotationIdFromBin;
		result["time"] = &AnnotationRow::timeFromBin;
		result["issue"] = &AnnotationRow::issueFromBin;
		result["details"] = &AnnotationRow::detailsFromBin;
		
		
		result["numAntenna"] = &AnnotationRow::numAntennaFromBin;
		result["basebandName"] = &AnnotationRow::basebandNameFromBin;
		result["numBaseband"] = &AnnotationRow::numBasebandFromBin;
		result["interval"] = &AnnotationRow::intervalFromBin;
		result["dValue"] = &AnnotationRow::dValueFromBin;
		result["vdValue"] = &AnnotationRow::vdValueFromBin;
		result["vvdValues"] = &AnnotationRow::vvdValuesFromBin;
		result["llValue"] = &AnnotationRow::llValueFromBin;
		result["vllValue"] = &AnnotationRow::vllValueFromBin;
		result["vvllValue"] = &AnnotationRow::vvllValueFromBin;
		result["sValue"] = &AnnotationRow::sValueFromBin;
		result["antennaId"] = &AnnotationRow::antennaIdFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
