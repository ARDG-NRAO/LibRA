
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
 * File PolarizationRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/PolarizationRow.h>
#include <alma/ASDM/PolarizationTable.h>
	

using asdm::ASDM;
using asdm::PolarizationRow;
using asdm::PolarizationTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	PolarizationRow::~PolarizationRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	PolarizationTable &PolarizationRow::getTable() const {
		return table;
	}

	bool PolarizationRow::isAdded() const {
		return hasBeenAdded;
	}	

	void PolarizationRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::PolarizationRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a PolarizationRowIDL struct.
	 */
	PolarizationRowIDL *PolarizationRow::toIDL() const {
		PolarizationRowIDL *x = new PolarizationRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->polarizationId = polarizationId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->numCorr = numCorr;
 				
 			
		
	

	
  		
		
		
			
		x->corrType.length(corrType.size());
		for (unsigned int i = 0; i < corrType.size(); ++i) {
			
				
			x->corrType[i] = corrType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->corrProduct.length(corrProduct.size());
		for (unsigned int i = 0; i < corrProduct.size(); i++) {
			x->corrProduct[i].length(corrProduct.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < corrProduct.size() ; i++)
			for (unsigned int j = 0; j < corrProduct.at(i).size(); j++)
					
						
				x->corrProduct[i][j] = corrProduct.at(i).at(j);
		 				
			 						
		
			
		
	

	
	
		
		
		return x;
	
	}
	
	void PolarizationRow::toIDL(asdmIDL::PolarizationRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.polarizationId = polarizationId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.numCorr = numCorr;
 				
 			
		
	

	
  		
		
		
			
		x.corrType.length(corrType.size());
		for (unsigned int i = 0; i < corrType.size(); ++i) {
			
				
			x.corrType[i] = corrType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.corrProduct.length(corrProduct.size());
		for (unsigned int i = 0; i < corrProduct.size(); i++) {
			x.corrProduct[i].length(corrProduct.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < corrProduct.size() ; i++)
			for (unsigned int j = 0; j < corrProduct.at(i).size(); j++)
					
						
				x.corrProduct[i][j] = corrProduct.at(i).at(j);
		 				
			 						
		
			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct PolarizationRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void PolarizationRow::setFromIDL (PolarizationRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setPolarizationId(Tag (x.polarizationId));
			
 		
		
	

	
		
		
			
		setNumCorr(x.numCorr);
  			
 		
		
	

	
		
		
			
		corrType .clear();
		for (unsigned int i = 0; i <x.corrType.length(); ++i) {
			
			corrType.push_back(x.corrType[i]);
  			
		}
			
  		
		
	

	
		
		
			
		corrProduct .clear();
        
		vector<PolarizationTypeMod::PolarizationType> v_aux_corrProduct;
        
		for (unsigned int i = 0; i < x.corrProduct.length(); ++i) {
			v_aux_corrProduct.clear();
			for (unsigned int j = 0; j < x.corrProduct[0].length(); ++j) {
				
				v_aux_corrProduct.push_back(x.corrProduct[i][j]);
	  			
  			}
  			corrProduct.push_back(v_aux_corrProduct);			
		}
			
  		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Polarization");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string PolarizationRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(polarizationId, "polarizationId", buf);
		
		
	

  	
 		
		
		Parser::toXML(numCorr, "numCorr", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("corrType", corrType));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("corrProduct", corrProduct));
		
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void PolarizationRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setPolarizationId(Parser::getTag("polarizationId","Polarization",rowDoc));
			
		
	

	
  		
			
	  	setNumCorr(Parser::getInteger("numCorr","Polarization",rowDoc));
			
		
	

	
		
		
		
		corrType = EnumerationParser::getStokesParameter1D("corrType","Polarization",rowDoc);			
		
		
		
	

	
		
		
		
		corrProduct = EnumerationParser::getPolarizationType2D("corrProduct","Polarization",rowDoc);			
		
		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Polarization");
		}
	}
	
	void PolarizationRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	polarizationId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numCorr);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) corrType.size());
		for (unsigned int i = 0; i < corrType.size(); i++)
				
			eoss.writeString(CStokesParameter::name(corrType.at(i)));
			/* eoss.writeInt(corrType.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) corrProduct.size());
		eoss.writeInt((int) corrProduct.at(0).size());
		for (unsigned int i = 0; i < corrProduct.size(); i++) 
			for (unsigned int j = 0;  j < corrProduct.at(0).size(); j++) 
				
				eoss.writeString(CPolarizationType::name(corrProduct.at(i).at(j)));				
				/* eoss.writeInt(corrProduct.at(i).at(j)); */
				
	
						
		
	


	
	
	}
	
void PolarizationRow::polarizationIdFromBin(EndianIStream& eis) {
		
	
		
		
		polarizationId =  Tag::fromBin(eis);
		
	
	
}
void PolarizationRow::numCorrFromBin(EndianIStream& eis) {
		
	
	
		
			
		numCorr =  eis.readInt();
			
		
	
	
}
void PolarizationRow::corrTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		corrType.clear();
		
		unsigned int corrTypeDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < corrTypeDim1; i++)
			
			corrType.push_back(CStokesParameter::literal(eis.readString()));
			
	

		
	
	
}
void PolarizationRow::corrProductFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		corrProduct.clear();
		
		unsigned int corrProductDim1 = eis.readInt();
		unsigned int corrProductDim2 = eis.readInt();
        
		vector <PolarizationTypeMod::PolarizationType> corrProductAux1;
        
		for (unsigned int i = 0; i < corrProductDim1; i++) {
			corrProductAux1.clear();
			for (unsigned int j = 0; j < corrProductDim2 ; j++)			
			
			corrProductAux1.push_back(CPolarizationType::literal(eis.readString()));
			
			corrProduct.push_back(corrProductAux1);
		}
	
	

		
	
	
}

		
	
	PolarizationRow* PolarizationRow::fromBin(EndianIStream& eis, PolarizationTable& table, const vector<string>& attributesSeq) {
		PolarizationRow* row = new  PolarizationRow(table);
		
		map<string, PolarizationAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "PolarizationTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void PolarizationRow::polarizationIdFromText(const string & s) {
		 
          
		polarizationId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void PolarizationRow::numCorrFromText(const string & s) {
		 
          
		numCorr = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an StokesParameter 
	void PolarizationRow::corrTypeFromText(const string & s) {
		 
          
		corrType = ASDMValuesParser::parse1D<StokesParameterMod::StokesParameter>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void PolarizationRow::corrProductFromText(const string & s) {
		 
          
		corrProduct = ASDMValuesParser::parse2D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	

		
	
	void PolarizationRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, PolarizationAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "PolarizationTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get polarizationId.
 	 * @return polarizationId as Tag
 	 */
 	Tag PolarizationRow::getPolarizationId() const {
	
  		return polarizationId;
 	}

 	/**
 	 * Set polarizationId with the specified Tag.
 	 * @param polarizationId The Tag value to which polarizationId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void PolarizationRow::setPolarizationId (Tag polarizationId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("polarizationId", "Polarization");
		
  		}
  	
 		this->polarizationId = polarizationId;
	
 	}
	
	

	

	
 	/**
 	 * Get numCorr.
 	 * @return numCorr as int
 	 */
 	int PolarizationRow::getNumCorr() const {
	
  		return numCorr;
 	}

 	/**
 	 * Set numCorr with the specified int.
 	 * @param numCorr The int value to which numCorr is to be set.
 	 
 	
 		
 	 */
 	void PolarizationRow::setNumCorr (int numCorr)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numCorr = numCorr;
	
 	}
	
	

	

	
 	/**
 	 * Get corrType.
 	 * @return corrType as std::vector<StokesParameterMod::StokesParameter >
 	 */
 	std::vector<StokesParameterMod::StokesParameter > PolarizationRow::getCorrType() const {
	
  		return corrType;
 	}

 	/**
 	 * Set corrType with the specified std::vector<StokesParameterMod::StokesParameter >.
 	 * @param corrType The std::vector<StokesParameterMod::StokesParameter > value to which corrType is to be set.
 	 
 	
 		
 	 */
 	void PolarizationRow::setCorrType (std::vector<StokesParameterMod::StokesParameter > corrType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->corrType = corrType;
	
 	}
	
	

	

	
 	/**
 	 * Get corrProduct.
 	 * @return corrProduct as std::vector<std::vector<PolarizationTypeMod::PolarizationType > >
 	 */
 	std::vector<std::vector<PolarizationTypeMod::PolarizationType > > PolarizationRow::getCorrProduct() const {
	
  		return corrProduct;
 	}

 	/**
 	 * Set corrProduct with the specified std::vector<std::vector<PolarizationTypeMod::PolarizationType > >.
 	 * @param corrProduct The std::vector<std::vector<PolarizationTypeMod::PolarizationType > > value to which corrProduct is to be set.
 	 
 	
 		
 	 */
 	void PolarizationRow::setCorrProduct (std::vector<std::vector<PolarizationTypeMod::PolarizationType > > corrProduct)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->corrProduct = corrProduct;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a PolarizationRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	PolarizationRow::PolarizationRow (PolarizationTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	
	
	
	
	

	

	

	

	
	
	 fromBinMethods["polarizationId"] = &PolarizationRow::polarizationIdFromBin; 
	 fromBinMethods["numCorr"] = &PolarizationRow::numCorrFromBin; 
	 fromBinMethods["corrType"] = &PolarizationRow::corrTypeFromBin; 
	 fromBinMethods["corrProduct"] = &PolarizationRow::corrProductFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["polarizationId"] = &PolarizationRow::polarizationIdFromText;
		 
	
				 
	fromTextMethods["numCorr"] = &PolarizationRow::numCorrFromText;
		 
	
				 
	fromTextMethods["corrType"] = &PolarizationRow::corrTypeFromText;
		 
	
				 
	fromTextMethods["corrProduct"] = &PolarizationRow::corrProductFromText;
		 
	

		
	}
	
	PolarizationRow::PolarizationRow (PolarizationTable &t, PolarizationRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

			
		}
		else {
	
		
			polarizationId = row->polarizationId;
		
		
		
		
			numCorr = row->numCorr;
		
			corrType = row->corrType;
		
			corrProduct = row->corrProduct;
		
		
		
		
		}
		
		 fromBinMethods["polarizationId"] = &PolarizationRow::polarizationIdFromBin; 
		 fromBinMethods["numCorr"] = &PolarizationRow::numCorrFromBin; 
		 fromBinMethods["corrType"] = &PolarizationRow::corrTypeFromBin; 
		 fromBinMethods["corrProduct"] = &PolarizationRow::corrProductFromBin; 
			
	
			
	}

	
	bool PolarizationRow::compareNoAutoInc(int numCorr, std::vector<StokesParameterMod::StokesParameter > corrType, std::vector<std::vector<PolarizationTypeMod::PolarizationType > > corrProduct) {
		bool result;
		result = true;
		
	
		
		result = result && (this->numCorr == numCorr);
		
		if (!result) return false;
	

	
		
		result = result && (this->corrType == corrType);
		
		if (!result) return false;
	

	
		
		result = result && (this->corrProduct == corrProduct);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool PolarizationRow::compareRequiredValue(int numCorr, std::vector<StokesParameterMod::StokesParameter > corrType, std::vector<std::vector<PolarizationTypeMod::PolarizationType > > corrProduct) {
		bool result;
		result = true;
		
	
		if (!(this->numCorr == numCorr)) return false;
	

	
		if (!(this->corrType == corrType)) return false;
	

	
		if (!(this->corrProduct == corrProduct)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the PolarizationRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool PolarizationRow::equalByRequiredValue(PolarizationRow*  x ) {
		
			
		if (this->numCorr != x->numCorr) return false;
			
		if (this->corrType != x->corrType) return false;
			
		if (this->corrProduct != x->corrProduct) return false;
			
		
		return true;
	}	
	
/*
	 map<string, PolarizationAttributeFromBin> PolarizationRow::initFromBinMethods() {
		map<string, PolarizationAttributeFromBin> result;
		
		result["polarizationId"] = &PolarizationRow::polarizationIdFromBin;
		result["numCorr"] = &PolarizationRow::numCorrFromBin;
		result["corrType"] = &PolarizationRow::corrTypeFromBin;
		result["corrProduct"] = &PolarizationRow::corrProductFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
