
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
 * File CorrelatorModeRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CorrelatorModeRow.h>
#include <alma/ASDM/CorrelatorModeTable.h>
	

using asdm::ASDM;
using asdm::CorrelatorModeRow;
using asdm::CorrelatorModeTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	CorrelatorModeRow::~CorrelatorModeRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CorrelatorModeTable &CorrelatorModeRow::getTable() const {
		return table;
	}

	bool CorrelatorModeRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CorrelatorModeRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CorrelatorModeRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CorrelatorModeRowIDL struct.
	 */
	CorrelatorModeRowIDL *CorrelatorModeRow::toIDL() const {
		CorrelatorModeRowIDL *x = new CorrelatorModeRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->correlatorModeId = correlatorModeId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->numBaseband = numBaseband;
 				
 			
		
	

	
  		
		
		
			
		x->basebandNames.length(basebandNames.size());
		for (unsigned int i = 0; i < basebandNames.size(); ++i) {
			
				
			x->basebandNames[i] = basebandNames.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->basebandConfig.length(basebandConfig.size());
		for (unsigned int i = 0; i < basebandConfig.size(); ++i) {
			
				
			x->basebandConfig[i] = basebandConfig.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->accumMode = accumMode;
 				
 			
		
	

	
  		
		
		
			
				
		x->binMode = binMode;
 				
 			
		
	

	
  		
		
		
			
				
		x->numAxes = numAxes;
 				
 			
		
	

	
  		
		
		
			
		x->axesOrderArray.length(axesOrderArray.size());
		for (unsigned int i = 0; i < axesOrderArray.size(); ++i) {
			
				
			x->axesOrderArray[i] = axesOrderArray.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->filterMode.length(filterMode.size());
		for (unsigned int i = 0; i < filterMode.size(); ++i) {
			
				
			x->filterMode[i] = filterMode.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->correlatorName = correlatorName;
 				
 			
		
	

	
	
		
		
		return x;
	
	}
	
	void CorrelatorModeRow::toIDL(asdmIDL::CorrelatorModeRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.correlatorModeId = correlatorModeId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.numBaseband = numBaseband;
 				
 			
		
	

	
  		
		
		
			
		x.basebandNames.length(basebandNames.size());
		for (unsigned int i = 0; i < basebandNames.size(); ++i) {
			
				
			x.basebandNames[i] = basebandNames.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.basebandConfig.length(basebandConfig.size());
		for (unsigned int i = 0; i < basebandConfig.size(); ++i) {
			
				
			x.basebandConfig[i] = basebandConfig.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.accumMode = accumMode;
 				
 			
		
	

	
  		
		
		
			
				
		x.binMode = binMode;
 				
 			
		
	

	
  		
		
		
			
				
		x.numAxes = numAxes;
 				
 			
		
	

	
  		
		
		
			
		x.axesOrderArray.length(axesOrderArray.size());
		for (unsigned int i = 0; i < axesOrderArray.size(); ++i) {
			
				
			x.axesOrderArray[i] = axesOrderArray.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.filterMode.length(filterMode.size());
		for (unsigned int i = 0; i < filterMode.size(); ++i) {
			
				
			x.filterMode[i] = filterMode.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.correlatorName = correlatorName;
 				
 			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CorrelatorModeRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CorrelatorModeRow::setFromIDL (CorrelatorModeRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setCorrelatorModeId(Tag (x.correlatorModeId));
			
 		
		
	

	
		
		
			
		setNumBaseband(x.numBaseband);
  			
 		
		
	

	
		
		
			
		basebandNames .clear();
		for (unsigned int i = 0; i <x.basebandNames.length(); ++i) {
			
			basebandNames.push_back(x.basebandNames[i]);
  			
		}
			
  		
		
	

	
		
		
			
		basebandConfig .clear();
		for (unsigned int i = 0; i <x.basebandConfig.length(); ++i) {
			
			basebandConfig.push_back(x.basebandConfig[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setAccumMode(x.accumMode);
  			
 		
		
	

	
		
		
			
		setBinMode(x.binMode);
  			
 		
		
	

	
		
		
			
		setNumAxes(x.numAxes);
  			
 		
		
	

	
		
		
			
		axesOrderArray .clear();
		for (unsigned int i = 0; i <x.axesOrderArray.length(); ++i) {
			
			axesOrderArray.push_back(x.axesOrderArray[i]);
  			
		}
			
  		
		
	

	
		
		
			
		filterMode .clear();
		for (unsigned int i = 0; i <x.filterMode.length(); ++i) {
			
			filterMode.push_back(x.filterMode[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setCorrelatorName(x.correlatorName);
  			
 		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CorrelatorMode");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CorrelatorModeRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(correlatorModeId, "correlatorModeId", buf);
		
		
	

  	
 		
		
		Parser::toXML(numBaseband, "numBaseband", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("basebandNames", basebandNames));
		
		
	

  	
 		
		
		Parser::toXML(basebandConfig, "basebandConfig", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("accumMode", accumMode));
		
		
	

  	
 		
		
		Parser::toXML(binMode, "binMode", buf);
		
		
	

  	
 		
		
		Parser::toXML(numAxes, "numAxes", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("axesOrderArray", axesOrderArray));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("filterMode", filterMode));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("correlatorName", correlatorName));
		
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void CorrelatorModeRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setCorrelatorModeId(Parser::getTag("correlatorModeId","CorrelatorMode",rowDoc));
			
		
	

	
  		
			
	  	setNumBaseband(Parser::getInteger("numBaseband","CorrelatorMode",rowDoc));
			
		
	

	
		
		
		
		basebandNames = EnumerationParser::getBasebandName1D("basebandNames","CorrelatorMode",rowDoc);			
		
		
		
	

	
  		
			
					
	  	setBasebandConfig(Parser::get1DInteger("basebandConfig","CorrelatorMode",rowDoc));
	  			
	  		
		
	

	
		
		
		
		accumMode = EnumerationParser::getAccumMode("accumMode","CorrelatorMode",rowDoc);
		
		
		
	

	
  		
			
	  	setBinMode(Parser::getInteger("binMode","CorrelatorMode",rowDoc));
			
		
	

	
  		
			
	  	setNumAxes(Parser::getInteger("numAxes","CorrelatorMode",rowDoc));
			
		
	

	
		
		
		
		axesOrderArray = EnumerationParser::getAxisName1D("axesOrderArray","CorrelatorMode",rowDoc);			
		
		
		
	

	
		
		
		
		filterMode = EnumerationParser::getFilterMode1D("filterMode","CorrelatorMode",rowDoc);			
		
		
		
	

	
		
		
		
		correlatorName = EnumerationParser::getCorrelatorName("correlatorName","CorrelatorMode",rowDoc);
		
		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CorrelatorMode");
		}
	}
	
	void CorrelatorModeRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	correlatorModeId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numBaseband);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) basebandNames.size());
		for (unsigned int i = 0; i < basebandNames.size(); i++)
				
			eoss.writeString(CBasebandName::name(basebandNames.at(i)));
			/* eoss.writeInt(basebandNames.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) basebandConfig.size());
		for (unsigned int i = 0; i < basebandConfig.size(); i++)
				
			eoss.writeInt(basebandConfig.at(i));
				
				
						
		
	

	
	
		
					
		eoss.writeString(CAccumMode::name(accumMode));
			/* eoss.writeInt(accumMode); */
				
		
	

	
	
		
						
			eoss.writeInt(binMode);
				
		
	

	
	
		
						
			eoss.writeInt(numAxes);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) axesOrderArray.size());
		for (unsigned int i = 0; i < axesOrderArray.size(); i++)
				
			eoss.writeString(CAxisName::name(axesOrderArray.at(i)));
			/* eoss.writeInt(axesOrderArray.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) filterMode.size());
		for (unsigned int i = 0; i < filterMode.size(); i++)
				
			eoss.writeString(CFilterMode::name(filterMode.at(i)));
			/* eoss.writeInt(filterMode.at(i)); */
				
				
						
		
	

	
	
		
					
		eoss.writeString(CCorrelatorName::name(correlatorName));
			/* eoss.writeInt(correlatorName); */
				
		
	


	
	
	}
	
void CorrelatorModeRow::correlatorModeIdFromBin(EndianIStream& eis) {
		
	
		
		
		correlatorModeId =  Tag::fromBin(eis);
		
	
	
}
void CorrelatorModeRow::numBasebandFromBin(EndianIStream& eis) {
		
	
	
		
			
		numBaseband =  eis.readInt();
			
		
	
	
}
void CorrelatorModeRow::basebandNamesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		basebandNames.clear();
		
		unsigned int basebandNamesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < basebandNamesDim1; i++)
			
			basebandNames.push_back(CBasebandName::literal(eis.readString()));
			
	

		
	
	
}
void CorrelatorModeRow::basebandConfigFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		basebandConfig.clear();
		
		unsigned int basebandConfigDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < basebandConfigDim1; i++)
			
			basebandConfig.push_back(eis.readInt());
			
	

		
	
	
}
void CorrelatorModeRow::accumModeFromBin(EndianIStream& eis) {
		
	
	
		
			
		accumMode = CAccumMode::literal(eis.readString());
			
		
	
	
}
void CorrelatorModeRow::binModeFromBin(EndianIStream& eis) {
		
	
	
		
			
		binMode =  eis.readInt();
			
		
	
	
}
void CorrelatorModeRow::numAxesFromBin(EndianIStream& eis) {
		
	
	
		
			
		numAxes =  eis.readInt();
			
		
	
	
}
void CorrelatorModeRow::axesOrderArrayFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		axesOrderArray.clear();
		
		unsigned int axesOrderArrayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < axesOrderArrayDim1; i++)
			
			axesOrderArray.push_back(CAxisName::literal(eis.readString()));
			
	

		
	
	
}
void CorrelatorModeRow::filterModeFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		filterMode.clear();
		
		unsigned int filterModeDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < filterModeDim1; i++)
			
			filterMode.push_back(CFilterMode::literal(eis.readString()));
			
	

		
	
	
}
void CorrelatorModeRow::correlatorNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		correlatorName = CCorrelatorName::literal(eis.readString());
			
		
	
	
}

		
	
	CorrelatorModeRow* CorrelatorModeRow::fromBin(EndianIStream& eis, CorrelatorModeTable& table, const vector<string>& attributesSeq) {
		CorrelatorModeRow* row = new  CorrelatorModeRow(table);
		
		map<string, CorrelatorModeAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CorrelatorModeTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void CorrelatorModeRow::correlatorModeIdFromText(const string & s) {
		 
          
		correlatorModeId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CorrelatorModeRow::numBasebandFromText(const string & s) {
		 
          
		numBaseband = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an BasebandName 
	void CorrelatorModeRow::basebandNamesFromText(const string & s) {
		 
          
		basebandNames = ASDMValuesParser::parse1D<BasebandNameMod::BasebandName>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CorrelatorModeRow::basebandConfigFromText(const string & s) {
		 
          
		basebandConfig = ASDMValuesParser::parse1D<int>(s);
          
		
	}
	
	
	// Convert a string into an AccumMode 
	void CorrelatorModeRow::accumModeFromText(const string & s) {
		 
          
		accumMode = ASDMValuesParser::parse<AccumModeMod::AccumMode>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CorrelatorModeRow::binModeFromText(const string & s) {
		 
          
		binMode = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CorrelatorModeRow::numAxesFromText(const string & s) {
		 
          
		numAxes = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an AxisName 
	void CorrelatorModeRow::axesOrderArrayFromText(const string & s) {
		 
          
		axesOrderArray = ASDMValuesParser::parse1D<AxisNameMod::AxisName>(s);
          
		
	}
	
	
	// Convert a string into an FilterMode 
	void CorrelatorModeRow::filterModeFromText(const string & s) {
		 
          
		filterMode = ASDMValuesParser::parse1D<FilterModeMod::FilterMode>(s);
          
		
	}
	
	
	// Convert a string into an CorrelatorName 
	void CorrelatorModeRow::correlatorNameFromText(const string & s) {
		 
          
		correlatorName = ASDMValuesParser::parse<CorrelatorNameMod::CorrelatorName>(s);
          
		
	}
	

		
	
	void CorrelatorModeRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CorrelatorModeAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CorrelatorModeTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get correlatorModeId.
 	 * @return correlatorModeId as Tag
 	 */
 	Tag CorrelatorModeRow::getCorrelatorModeId() const {
	
  		return correlatorModeId;
 	}

 	/**
 	 * Set correlatorModeId with the specified Tag.
 	 * @param correlatorModeId The Tag value to which correlatorModeId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CorrelatorModeRow::setCorrelatorModeId (Tag correlatorModeId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("correlatorModeId", "CorrelatorMode");
		
  		}
  	
 		this->correlatorModeId = correlatorModeId;
	
 	}
	
	

	

	
 	/**
 	 * Get numBaseband.
 	 * @return numBaseband as int
 	 */
 	int CorrelatorModeRow::getNumBaseband() const {
	
  		return numBaseband;
 	}

 	/**
 	 * Set numBaseband with the specified int.
 	 * @param numBaseband The int value to which numBaseband is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setNumBaseband (int numBaseband)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numBaseband = numBaseband;
	
 	}
	
	

	

	
 	/**
 	 * Get basebandNames.
 	 * @return basebandNames as std::vector<BasebandNameMod::BasebandName >
 	 */
 	std::vector<BasebandNameMod::BasebandName > CorrelatorModeRow::getBasebandNames() const {
	
  		return basebandNames;
 	}

 	/**
 	 * Set basebandNames with the specified std::vector<BasebandNameMod::BasebandName >.
 	 * @param basebandNames The std::vector<BasebandNameMod::BasebandName > value to which basebandNames is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setBasebandNames (std::vector<BasebandNameMod::BasebandName > basebandNames)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->basebandNames = basebandNames;
	
 	}
	
	

	

	
 	/**
 	 * Get basebandConfig.
 	 * @return basebandConfig as std::vector<int >
 	 */
 	std::vector<int > CorrelatorModeRow::getBasebandConfig() const {
	
  		return basebandConfig;
 	}

 	/**
 	 * Set basebandConfig with the specified std::vector<int >.
 	 * @param basebandConfig The std::vector<int > value to which basebandConfig is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setBasebandConfig (std::vector<int > basebandConfig)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->basebandConfig = basebandConfig;
	
 	}
	
	

	

	
 	/**
 	 * Get accumMode.
 	 * @return accumMode as AccumModeMod::AccumMode
 	 */
 	AccumModeMod::AccumMode CorrelatorModeRow::getAccumMode() const {
	
  		return accumMode;
 	}

 	/**
 	 * Set accumMode with the specified AccumModeMod::AccumMode.
 	 * @param accumMode The AccumModeMod::AccumMode value to which accumMode is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setAccumMode (AccumModeMod::AccumMode accumMode)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->accumMode = accumMode;
	
 	}
	
	

	

	
 	/**
 	 * Get binMode.
 	 * @return binMode as int
 	 */
 	int CorrelatorModeRow::getBinMode() const {
	
  		return binMode;
 	}

 	/**
 	 * Set binMode with the specified int.
 	 * @param binMode The int value to which binMode is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setBinMode (int binMode)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->binMode = binMode;
	
 	}
	
	

	

	
 	/**
 	 * Get numAxes.
 	 * @return numAxes as int
 	 */
 	int CorrelatorModeRow::getNumAxes() const {
	
  		return numAxes;
 	}

 	/**
 	 * Set numAxes with the specified int.
 	 * @param numAxes The int value to which numAxes is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setNumAxes (int numAxes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numAxes = numAxes;
	
 	}
	
	

	

	
 	/**
 	 * Get axesOrderArray.
 	 * @return axesOrderArray as std::vector<AxisNameMod::AxisName >
 	 */
 	std::vector<AxisNameMod::AxisName > CorrelatorModeRow::getAxesOrderArray() const {
	
  		return axesOrderArray;
 	}

 	/**
 	 * Set axesOrderArray with the specified std::vector<AxisNameMod::AxisName >.
 	 * @param axesOrderArray The std::vector<AxisNameMod::AxisName > value to which axesOrderArray is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setAxesOrderArray (std::vector<AxisNameMod::AxisName > axesOrderArray)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->axesOrderArray = axesOrderArray;
	
 	}
	
	

	

	
 	/**
 	 * Get filterMode.
 	 * @return filterMode as std::vector<FilterModeMod::FilterMode >
 	 */
 	std::vector<FilterModeMod::FilterMode > CorrelatorModeRow::getFilterMode() const {
	
  		return filterMode;
 	}

 	/**
 	 * Set filterMode with the specified std::vector<FilterModeMod::FilterMode >.
 	 * @param filterMode The std::vector<FilterModeMod::FilterMode > value to which filterMode is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setFilterMode (std::vector<FilterModeMod::FilterMode > filterMode)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->filterMode = filterMode;
	
 	}
	
	

	

	
 	/**
 	 * Get correlatorName.
 	 * @return correlatorName as CorrelatorNameMod::CorrelatorName
 	 */
 	CorrelatorNameMod::CorrelatorName CorrelatorModeRow::getCorrelatorName() const {
	
  		return correlatorName;
 	}

 	/**
 	 * Set correlatorName with the specified CorrelatorNameMod::CorrelatorName.
 	 * @param correlatorName The CorrelatorNameMod::CorrelatorName value to which correlatorName is to be set.
 	 
 	
 		
 	 */
 	void CorrelatorModeRow::setCorrelatorName (CorrelatorNameMod::CorrelatorName correlatorName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->correlatorName = correlatorName;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a CorrelatorModeRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CorrelatorModeRow::CorrelatorModeRow (CorrelatorModeTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	
	
	
	
	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
accumMode = CAccumMode::from_int(0);
	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
correlatorName = CCorrelatorName::from_int(0);
	

	
	
	 fromBinMethods["correlatorModeId"] = &CorrelatorModeRow::correlatorModeIdFromBin; 
	 fromBinMethods["numBaseband"] = &CorrelatorModeRow::numBasebandFromBin; 
	 fromBinMethods["basebandNames"] = &CorrelatorModeRow::basebandNamesFromBin; 
	 fromBinMethods["basebandConfig"] = &CorrelatorModeRow::basebandConfigFromBin; 
	 fromBinMethods["accumMode"] = &CorrelatorModeRow::accumModeFromBin; 
	 fromBinMethods["binMode"] = &CorrelatorModeRow::binModeFromBin; 
	 fromBinMethods["numAxes"] = &CorrelatorModeRow::numAxesFromBin; 
	 fromBinMethods["axesOrderArray"] = &CorrelatorModeRow::axesOrderArrayFromBin; 
	 fromBinMethods["filterMode"] = &CorrelatorModeRow::filterModeFromBin; 
	 fromBinMethods["correlatorName"] = &CorrelatorModeRow::correlatorNameFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["correlatorModeId"] = &CorrelatorModeRow::correlatorModeIdFromText;
		 
	
				 
	fromTextMethods["numBaseband"] = &CorrelatorModeRow::numBasebandFromText;
		 
	
				 
	fromTextMethods["basebandNames"] = &CorrelatorModeRow::basebandNamesFromText;
		 
	
				 
	fromTextMethods["basebandConfig"] = &CorrelatorModeRow::basebandConfigFromText;
		 
	
				 
	fromTextMethods["accumMode"] = &CorrelatorModeRow::accumModeFromText;
		 
	
				 
	fromTextMethods["binMode"] = &CorrelatorModeRow::binModeFromText;
		 
	
				 
	fromTextMethods["numAxes"] = &CorrelatorModeRow::numAxesFromText;
		 
	
				 
	fromTextMethods["axesOrderArray"] = &CorrelatorModeRow::axesOrderArrayFromText;
		 
	
				 
	fromTextMethods["filterMode"] = &CorrelatorModeRow::filterModeFromText;
		 
	
				 
	fromTextMethods["correlatorName"] = &CorrelatorModeRow::correlatorNameFromText;
		 
	

		
	}
	
	CorrelatorModeRow::CorrelatorModeRow (CorrelatorModeTable &t, CorrelatorModeRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

			
		}
		else {
	
		
			correlatorModeId = row->correlatorModeId;
		
		
		
		
			numBaseband = row->numBaseband;
		
			basebandNames = row->basebandNames;
		
			basebandConfig = row->basebandConfig;
		
			accumMode = row->accumMode;
		
			binMode = row->binMode;
		
			numAxes = row->numAxes;
		
			axesOrderArray = row->axesOrderArray;
		
			filterMode = row->filterMode;
		
			correlatorName = row->correlatorName;
		
		
		
		
		}
		
		 fromBinMethods["correlatorModeId"] = &CorrelatorModeRow::correlatorModeIdFromBin; 
		 fromBinMethods["numBaseband"] = &CorrelatorModeRow::numBasebandFromBin; 
		 fromBinMethods["basebandNames"] = &CorrelatorModeRow::basebandNamesFromBin; 
		 fromBinMethods["basebandConfig"] = &CorrelatorModeRow::basebandConfigFromBin; 
		 fromBinMethods["accumMode"] = &CorrelatorModeRow::accumModeFromBin; 
		 fromBinMethods["binMode"] = &CorrelatorModeRow::binModeFromBin; 
		 fromBinMethods["numAxes"] = &CorrelatorModeRow::numAxesFromBin; 
		 fromBinMethods["axesOrderArray"] = &CorrelatorModeRow::axesOrderArrayFromBin; 
		 fromBinMethods["filterMode"] = &CorrelatorModeRow::filterModeFromBin; 
		 fromBinMethods["correlatorName"] = &CorrelatorModeRow::correlatorNameFromBin; 
			
	
			
	}

	
	bool CorrelatorModeRow::compareNoAutoInc(int numBaseband, std::vector<BasebandNameMod::BasebandName > basebandNames, std::vector<int > basebandConfig, AccumModeMod::AccumMode accumMode, int binMode, int numAxes, std::vector<AxisNameMod::AxisName > axesOrderArray, std::vector<FilterModeMod::FilterMode > filterMode, CorrelatorNameMod::CorrelatorName correlatorName) {
		bool result;
		result = true;
		
	
		
		result = result && (this->numBaseband == numBaseband);
		
		if (!result) return false;
	

	
		
		result = result && (this->basebandNames == basebandNames);
		
		if (!result) return false;
	

	
		
		result = result && (this->basebandConfig == basebandConfig);
		
		if (!result) return false;
	

	
		
		result = result && (this->accumMode == accumMode);
		
		if (!result) return false;
	

	
		
		result = result && (this->binMode == binMode);
		
		if (!result) return false;
	

	
		
		result = result && (this->numAxes == numAxes);
		
		if (!result) return false;
	

	
		
		result = result && (this->axesOrderArray == axesOrderArray);
		
		if (!result) return false;
	

	
		
		result = result && (this->filterMode == filterMode);
		
		if (!result) return false;
	

	
		
		result = result && (this->correlatorName == correlatorName);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CorrelatorModeRow::compareRequiredValue(int numBaseband, std::vector<BasebandNameMod::BasebandName > basebandNames, std::vector<int > basebandConfig, AccumModeMod::AccumMode accumMode, int binMode, int numAxes, std::vector<AxisNameMod::AxisName > axesOrderArray, std::vector<FilterModeMod::FilterMode > filterMode, CorrelatorNameMod::CorrelatorName correlatorName) {
		bool result;
		result = true;
		
	
		if (!(this->numBaseband == numBaseband)) return false;
	

	
		if (!(this->basebandNames == basebandNames)) return false;
	

	
		if (!(this->basebandConfig == basebandConfig)) return false;
	

	
		if (!(this->accumMode == accumMode)) return false;
	

	
		if (!(this->binMode == binMode)) return false;
	

	
		if (!(this->numAxes == numAxes)) return false;
	

	
		if (!(this->axesOrderArray == axesOrderArray)) return false;
	

	
		if (!(this->filterMode == filterMode)) return false;
	

	
		if (!(this->correlatorName == correlatorName)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CorrelatorModeRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CorrelatorModeRow::equalByRequiredValue(CorrelatorModeRow*  x ) {
		
			
		if (this->numBaseband != x->numBaseband) return false;
			
		if (this->basebandNames != x->basebandNames) return false;
			
		if (this->basebandConfig != x->basebandConfig) return false;
			
		if (this->accumMode != x->accumMode) return false;
			
		if (this->binMode != x->binMode) return false;
			
		if (this->numAxes != x->numAxes) return false;
			
		if (this->axesOrderArray != x->axesOrderArray) return false;
			
		if (this->filterMode != x->filterMode) return false;
			
		if (this->correlatorName != x->correlatorName) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CorrelatorModeAttributeFromBin> CorrelatorModeRow::initFromBinMethods() {
		map<string, CorrelatorModeAttributeFromBin> result;
		
		result["correlatorModeId"] = &CorrelatorModeRow::correlatorModeIdFromBin;
		result["numBaseband"] = &CorrelatorModeRow::numBasebandFromBin;
		result["basebandNames"] = &CorrelatorModeRow::basebandNamesFromBin;
		result["basebandConfig"] = &CorrelatorModeRow::basebandConfigFromBin;
		result["accumMode"] = &CorrelatorModeRow::accumModeFromBin;
		result["binMode"] = &CorrelatorModeRow::binModeFromBin;
		result["numAxes"] = &CorrelatorModeRow::numAxesFromBin;
		result["axesOrderArray"] = &CorrelatorModeRow::axesOrderArrayFromBin;
		result["filterMode"] = &CorrelatorModeRow::filterModeFromBin;
		result["correlatorName"] = &CorrelatorModeRow::correlatorNameFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
