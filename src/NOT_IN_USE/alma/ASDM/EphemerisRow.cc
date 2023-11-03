
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
 * File EphemerisRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/EphemerisRow.h>
#include <alma/ASDM/EphemerisTable.h>
	

using asdm::ASDM;
using asdm::EphemerisRow;
using asdm::EphemerisTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	EphemerisRow::~EphemerisRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	EphemerisTable &EphemerisRow::getTable() const {
		return table;
	}

	bool EphemerisRow::isAdded() const {
		return hasBeenAdded;
	}	

	void EphemerisRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::EphemerisRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a EphemerisRowIDL struct.
	 */
	EphemerisRowIDL *EphemerisRow::toIDL() const {
		EphemerisRowIDL *x = new EphemerisRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x->ephemerisId = ephemerisId;
 				
 			
		
	

	
  		
		
		
			
		x->observerLocation.length(observerLocation.size());
		for (unsigned int i = 0; i < observerLocation.size(); ++i) {
			
				
			x->observerLocation[i] = observerLocation.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->equinoxEquator = equinoxEquator;
 				
 			
		
	

	
  		
		
		
			
				
		x->numPolyDir = numPolyDir;
 				
 			
		
	

	
  		
		
		
			
		x->dir.length(dir.size());
		for (unsigned int i = 0; i < dir.size(); i++) {
			x->dir[i].length(dir.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < dir.size() ; i++)
			for (unsigned int j = 0; j < dir.at(i).size(); j++)
					
						
				x->dir[i][j] = dir.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
				
		x->numPolyDist = numPolyDist;
 				
 			
		
	

	
  		
		
		
			
		x->distance.length(distance.size());
		for (unsigned int i = 0; i < distance.size(); ++i) {
			
				
			x->distance[i] = distance.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->timeOrigin = timeOrigin.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->origin = CORBA::string_dup(origin.c_str());
				
 			
		
	

	
  		
		
		x->numPolyRadVelExists = numPolyRadVelExists;
		
		
			
				
		x->numPolyRadVel = numPolyRadVel;
 				
 			
		
	

	
  		
		
		x->radVelExists = radVelExists;
		
		
			
		x->radVel.length(radVel.size());
		for (unsigned int i = 0; i < radVel.size(); ++i) {
			
				
			x->radVel[i] = radVel.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
		
		return x;
	
	}
	
	void EphemerisRow::toIDL(asdmIDL::EphemerisRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x.ephemerisId = ephemerisId;
 				
 			
		
	

	
  		
		
		
			
		x.observerLocation.length(observerLocation.size());
		for (unsigned int i = 0; i < observerLocation.size(); ++i) {
			
				
			x.observerLocation[i] = observerLocation.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.equinoxEquator = equinoxEquator;
 				
 			
		
	

	
  		
		
		
			
				
		x.numPolyDir = numPolyDir;
 				
 			
		
	

	
  		
		
		
			
		x.dir.length(dir.size());
		for (unsigned int i = 0; i < dir.size(); i++) {
			x.dir[i].length(dir.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < dir.size() ; i++)
			for (unsigned int j = 0; j < dir.at(i).size(); j++)
					
						
				x.dir[i][j] = dir.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
				
		x.numPolyDist = numPolyDist;
 				
 			
		
	

	
  		
		
		
			
		x.distance.length(distance.size());
		for (unsigned int i = 0; i < distance.size(); ++i) {
			
				
			x.distance[i] = distance.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.timeOrigin = timeOrigin.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.origin = CORBA::string_dup(origin.c_str());
				
 			
		
	

	
  		
		
		x.numPolyRadVelExists = numPolyRadVelExists;
		
		
			
				
		x.numPolyRadVel = numPolyRadVel;
 				
 			
		
	

	
  		
		
		x.radVelExists = radVelExists;
		
		
			
		x.radVel.length(radVel.size());
		for (unsigned int i = 0; i < radVel.size(); ++i) {
			
				
			x.radVel[i] = radVel.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct EphemerisRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void EphemerisRow::setFromIDL (EphemerisRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setEphemerisId(x.ephemerisId);
  			
 		
		
	

	
		
		
			
		observerLocation .clear();
		for (unsigned int i = 0; i <x.observerLocation.length(); ++i) {
			
			observerLocation.push_back(x.observerLocation[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setEquinoxEquator(x.equinoxEquator);
  			
 		
		
	

	
		
		
			
		setNumPolyDir(x.numPolyDir);
  			
 		
		
	

	
		
		
			
		dir .clear();
        
        vector<double> v_aux_dir;
        
		for (unsigned int i = 0; i < x.dir.length(); ++i) {
			v_aux_dir.clear();
			for (unsigned int j = 0; j < x.dir[0].length(); ++j) {
				
				v_aux_dir.push_back(x.dir[i][j]);
	  			
  			}
  			dir.push_back(v_aux_dir);			
		}
			
  		
		
	

	
		
		
			
		setNumPolyDist(x.numPolyDist);
  			
 		
		
	

	
		
		
			
		distance .clear();
		for (unsigned int i = 0; i <x.distance.length(); ++i) {
			
			distance.push_back(x.distance[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setTimeOrigin(ArrayTime (x.timeOrigin));
			
 		
		
	

	
		
		
			
		setOrigin(string (x.origin));
			
 		
		
	

	
		
		numPolyRadVelExists = x.numPolyRadVelExists;
		if (x.numPolyRadVelExists) {
		
		
			
		setNumPolyRadVel(x.numPolyRadVel);
  			
 		
		
		}
		
	

	
		
		radVelExists = x.radVelExists;
		if (x.radVelExists) {
		
		
			
		radVel .clear();
		for (unsigned int i = 0; i <x.radVel.length(); ++i) {
			
			radVel.push_back(x.radVel[i]);
  			
		}
			
  		
		
		}
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Ephemeris");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string EphemerisRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(ephemerisId, "ephemerisId", buf);
		
		
	

  	
 		
		
		Parser::toXML(observerLocation, "observerLocation", buf);
		
		
	

  	
 		
		
		Parser::toXML(equinoxEquator, "equinoxEquator", buf);
		
		
	

  	
 		
		
		Parser::toXML(numPolyDir, "numPolyDir", buf);
		
		
	

  	
 		
		
		Parser::toXML(dir, "dir", buf);
		
		
	

  	
 		
		
		Parser::toXML(numPolyDist, "numPolyDist", buf);
		
		
	

  	
 		
		
		Parser::toXML(distance, "distance", buf);
		
		
	

  	
 		
		
		Parser::toXML(timeOrigin, "timeOrigin", buf);
		
		
	

  	
 		
		
		Parser::toXML(origin, "origin", buf);
		
		
	

  	
 		
		if (numPolyRadVelExists) {
		
		
		Parser::toXML(numPolyRadVel, "numPolyRadVel", buf);
		
		
		}
		
	

  	
 		
		if (radVelExists) {
		
		
		Parser::toXML(radVel, "radVel", buf);
		
		
		}
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void EphemerisRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","Ephemeris",rowDoc));
			
		
	

	
  		
			
	  	setEphemerisId(Parser::getInteger("ephemerisId","Ephemeris",rowDoc));
			
		
	

	
  		
			
					
	  	setObserverLocation(Parser::get1DDouble("observerLocation","Ephemeris",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setEquinoxEquator(Parser::getDouble("equinoxEquator","Ephemeris",rowDoc));
			
		
	

	
  		
			
	  	setNumPolyDir(Parser::getInteger("numPolyDir","Ephemeris",rowDoc));
			
		
	

	
  		
			
					
	  	setDir(Parser::get2DDouble("dir","Ephemeris",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setNumPolyDist(Parser::getInteger("numPolyDist","Ephemeris",rowDoc));
			
		
	

	
  		
			
					
	  	setDistance(Parser::get1DDouble("distance","Ephemeris",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setTimeOrigin(Parser::getArrayTime("timeOrigin","Ephemeris",rowDoc));
			
		
	

	
  		
			
	  	setOrigin(Parser::getString("origin","Ephemeris",rowDoc));
			
		
	

	
  		
        if (row.isStr("<numPolyRadVel>")) {
			
	  		setNumPolyRadVel(Parser::getInteger("numPolyRadVel","Ephemeris",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<radVel>")) {
			
								
	  		setRadVel(Parser::get1DDouble("radVel","Ephemeris",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Ephemeris");
		}
	}
	
	void EphemerisRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(ephemerisId);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) observerLocation.size());
		for (unsigned int i = 0; i < observerLocation.size(); i++)
				
			eoss.writeDouble(observerLocation.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeDouble(equinoxEquator);
				
		
	

	
	
		
						
			eoss.writeInt(numPolyDir);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) dir.size());
		eoss.writeInt((int) dir.at(0).size());
		for (unsigned int i = 0; i < dir.size(); i++) 
			for (unsigned int j = 0;  j < dir.at(0).size(); j++) 
							 
				eoss.writeDouble(dir.at(i).at(j));
				
	
						
		
	

	
	
		
						
			eoss.writeInt(numPolyDist);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) distance.size());
		for (unsigned int i = 0; i < distance.size(); i++)
				
			eoss.writeDouble(distance.at(i));
				
				
						
		
	

	
	
		
	timeOrigin.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(origin);
				
		
	


	
	
	eoss.writeBoolean(numPolyRadVelExists);
	if (numPolyRadVelExists) {
	
	
	
		
						
			eoss.writeInt(numPolyRadVel);
				
		
	

	}

	eoss.writeBoolean(radVelExists);
	if (radVelExists) {
	
	
	
		
		
			
		eoss.writeInt((int) radVel.size());
		for (unsigned int i = 0; i < radVel.size(); i++)
				
			eoss.writeDouble(radVel.at(i));
				
				
						
		
	

	}

	}
	
void EphemerisRow::timeIntervalFromBin(EndianIStream& eis) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eis);
		
	
	
}
void EphemerisRow::ephemerisIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		ephemerisId =  eis.readInt();
			
		
	
	
}
void EphemerisRow::observerLocationFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		observerLocation.clear();
		
		unsigned int observerLocationDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < observerLocationDim1; i++)
			
			observerLocation.push_back(eis.readDouble());
			
	

		
	
	
}
void EphemerisRow::equinoxEquatorFromBin(EndianIStream& eis) {
		
	
	
		
			
		equinoxEquator =  eis.readDouble();
			
		
	
	
}
void EphemerisRow::numPolyDirFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPolyDir =  eis.readInt();
			
		
	
	
}
void EphemerisRow::dirFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		dir.clear();
		
		unsigned int dirDim1 = eis.readInt();
		unsigned int dirDim2 = eis.readInt();
        
		vector <double> dirAux1;
        
		for (unsigned int i = 0; i < dirDim1; i++) {
			dirAux1.clear();
			for (unsigned int j = 0; j < dirDim2 ; j++)			
			
			dirAux1.push_back(eis.readDouble());
			
			dir.push_back(dirAux1);
		}
	
	

		
	
	
}
void EphemerisRow::numPolyDistFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPolyDist =  eis.readInt();
			
		
	
	
}
void EphemerisRow::distanceFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		distance.clear();
		
		unsigned int distanceDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < distanceDim1; i++)
			
			distance.push_back(eis.readDouble());
			
	

		
	
	
}
void EphemerisRow::timeOriginFromBin(EndianIStream& eis) {
		
	
		
		
		timeOrigin =  ArrayTime::fromBin(eis);
		
	
	
}
void EphemerisRow::originFromBin(EndianIStream& eis) {
		
	
	
		
			
		origin =  eis.readString();
			
		
	
	
}

void EphemerisRow::numPolyRadVelFromBin(EndianIStream& eis) {
		
	numPolyRadVelExists = eis.readBoolean();
	if (numPolyRadVelExists) {
		
	
	
		
			
		numPolyRadVel =  eis.readInt();
			
		
	

	}
	
}
void EphemerisRow::radVelFromBin(EndianIStream& eis) {
		
	radVelExists = eis.readBoolean();
	if (radVelExists) {
		
	
	
		
			
	
		radVel.clear();
		
		unsigned int radVelDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < radVelDim1; i++)
			
			radVel.push_back(eis.readDouble());
			
	

		
	

	}
	
}
	
	
	EphemerisRow* EphemerisRow::fromBin(EndianIStream& eis, EphemerisTable& table, const vector<string>& attributesSeq) {
		EphemerisRow* row = new  EphemerisRow(table);
		
		map<string, EphemerisAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "EphemerisTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an ArrayTimeInterval 
	void EphemerisRow::timeIntervalFromText(const string & s) {
		 
          
		timeInterval = ASDMValuesParser::parse<ArrayTimeInterval>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void EphemerisRow::ephemerisIdFromText(const string & s) {
		 
          
		ephemerisId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void EphemerisRow::observerLocationFromText(const string & s) {
		 
          
		observerLocation = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void EphemerisRow::equinoxEquatorFromText(const string & s) {
		 
          
		equinoxEquator = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void EphemerisRow::numPolyDirFromText(const string & s) {
		 
          
		numPolyDir = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void EphemerisRow::dirFromText(const string & s) {
		 
          
		dir = ASDMValuesParser::parse2D<double>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void EphemerisRow::numPolyDistFromText(const string & s) {
		 
          
		numPolyDist = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void EphemerisRow::distanceFromText(const string & s) {
		 
          
		distance = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void EphemerisRow::timeOriginFromText(const string & s) {
		 
          
		timeOrigin = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void EphemerisRow::originFromText(const string & s) {
		 
          
		origin = ASDMValuesParser::parse<string>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void EphemerisRow::numPolyRadVelFromText(const string & s) {
		numPolyRadVelExists = true;
		 
          
		numPolyRadVel = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void EphemerisRow::radVelFromText(const string & s) {
		radVelExists = true;
		 
          
		radVel = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	
	void EphemerisRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, EphemerisAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "EphemerisTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval EphemerisRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void EphemerisRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "Ephemeris");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get ephemerisId.
 	 * @return ephemerisId as int
 	 */
 	int EphemerisRow::getEphemerisId() const {
	
  		return ephemerisId;
 	}

 	/**
 	 * Set ephemerisId with the specified int.
 	 * @param ephemerisId The int value to which ephemerisId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void EphemerisRow::setEphemerisId (int ephemerisId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("ephemerisId", "Ephemeris");
		
  		}
  	
 		this->ephemerisId = ephemerisId;
	
 	}
	
	

	

	
 	/**
 	 * Get observerLocation.
 	 * @return observerLocation as std::vector<double >
 	 */
 	std::vector<double > EphemerisRow::getObserverLocation() const {
	
  		return observerLocation;
 	}

 	/**
 	 * Set observerLocation with the specified std::vector<double >.
 	 * @param observerLocation The std::vector<double > value to which observerLocation is to be set.
 	 
 	
 		
 	 */
 	void EphemerisRow::setObserverLocation (std::vector<double > observerLocation)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->observerLocation = observerLocation;
	
 	}
	
	

	

	
 	/**
 	 * Get equinoxEquator.
 	 * @return equinoxEquator as double
 	 */
 	double EphemerisRow::getEquinoxEquator() const {
	
  		return equinoxEquator;
 	}

 	/**
 	 * Set equinoxEquator with the specified double.
 	 * @param equinoxEquator The double value to which equinoxEquator is to be set.
 	 
 	
 		
 	 */
 	void EphemerisRow::setEquinoxEquator (double equinoxEquator)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->equinoxEquator = equinoxEquator;
	
 	}
	
	

	

	
 	/**
 	 * Get numPolyDir.
 	 * @return numPolyDir as int
 	 */
 	int EphemerisRow::getNumPolyDir() const {
	
  		return numPolyDir;
 	}

 	/**
 	 * Set numPolyDir with the specified int.
 	 * @param numPolyDir The int value to which numPolyDir is to be set.
 	 
 	
 		
 	 */
 	void EphemerisRow::setNumPolyDir (int numPolyDir)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPolyDir = numPolyDir;
	
 	}
	
	

	

	
 	/**
 	 * Get dir.
 	 * @return dir as std::vector<std::vector<double > >
 	 */
 	std::vector<std::vector<double > > EphemerisRow::getDir() const {
	
  		return dir;
 	}

 	/**
 	 * Set dir with the specified std::vector<std::vector<double > >.
 	 * @param dir The std::vector<std::vector<double > > value to which dir is to be set.
 	 
 	
 		
 	 */
 	void EphemerisRow::setDir (std::vector<std::vector<double > > dir)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->dir = dir;
	
 	}
	
	

	

	
 	/**
 	 * Get numPolyDist.
 	 * @return numPolyDist as int
 	 */
 	int EphemerisRow::getNumPolyDist() const {
	
  		return numPolyDist;
 	}

 	/**
 	 * Set numPolyDist with the specified int.
 	 * @param numPolyDist The int value to which numPolyDist is to be set.
 	 
 	
 		
 	 */
 	void EphemerisRow::setNumPolyDist (int numPolyDist)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPolyDist = numPolyDist;
	
 	}
	
	

	

	
 	/**
 	 * Get distance.
 	 * @return distance as std::vector<double >
 	 */
 	std::vector<double > EphemerisRow::getDistance() const {
	
  		return distance;
 	}

 	/**
 	 * Set distance with the specified std::vector<double >.
 	 * @param distance The std::vector<double > value to which distance is to be set.
 	 
 	
 		
 	 */
 	void EphemerisRow::setDistance (std::vector<double > distance)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->distance = distance;
	
 	}
	
	

	

	
 	/**
 	 * Get timeOrigin.
 	 * @return timeOrigin as ArrayTime
 	 */
 	ArrayTime EphemerisRow::getTimeOrigin() const {
	
  		return timeOrigin;
 	}

 	/**
 	 * Set timeOrigin with the specified ArrayTime.
 	 * @param timeOrigin The ArrayTime value to which timeOrigin is to be set.
 	 
 	
 		
 	 */
 	void EphemerisRow::setTimeOrigin (ArrayTime timeOrigin)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->timeOrigin = timeOrigin;
	
 	}
	
	

	

	
 	/**
 	 * Get origin.
 	 * @return origin as std::string
 	 */
 	std::string EphemerisRow::getOrigin() const {
	
  		return origin;
 	}

 	/**
 	 * Set origin with the specified std::string.
 	 * @param origin The std::string value to which origin is to be set.
 	 
 	
 		
 	 */
 	void EphemerisRow::setOrigin (std::string origin)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->origin = origin;
	
 	}
	
	

	
	/**
	 * The attribute numPolyRadVel is optional. Return true if this attribute exists.
	 * @return true if and only if the numPolyRadVel attribute exists. 
	 */
	bool EphemerisRow::isNumPolyRadVelExists() const {
		return numPolyRadVelExists;
	}
	

	
 	/**
 	 * Get numPolyRadVel, which is optional.
 	 * @return numPolyRadVel as int
 	 * @throw IllegalAccessException If numPolyRadVel does not exist.
 	 */
 	int EphemerisRow::getNumPolyRadVel() const  {
		if (!numPolyRadVelExists) {
			throw IllegalAccessException("numPolyRadVel", "Ephemeris");
		}
	
  		return numPolyRadVel;
 	}

 	/**
 	 * Set numPolyRadVel with the specified int.
 	 * @param numPolyRadVel The int value to which numPolyRadVel is to be set.
 	 
 	
 	 */
 	void EphemerisRow::setNumPolyRadVel (int numPolyRadVel) {
	
 		this->numPolyRadVel = numPolyRadVel;
	
		numPolyRadVelExists = true;
	
 	}
	
	
	/**
	 * Mark numPolyRadVel, which is an optional field, as non-existent.
	 */
	void EphemerisRow::clearNumPolyRadVel () {
		numPolyRadVelExists = false;
	}
	

	
	/**
	 * The attribute radVel is optional. Return true if this attribute exists.
	 * @return true if and only if the radVel attribute exists. 
	 */
	bool EphemerisRow::isRadVelExists() const {
		return radVelExists;
	}
	

	
 	/**
 	 * Get radVel, which is optional.
 	 * @return radVel as std::vector<double >
 	 * @throw IllegalAccessException If radVel does not exist.
 	 */
 	std::vector<double > EphemerisRow::getRadVel() const  {
		if (!radVelExists) {
			throw IllegalAccessException("radVel", "Ephemeris");
		}
	
  		return radVel;
 	}

 	/**
 	 * Set radVel with the specified std::vector<double >.
 	 * @param radVel The std::vector<double > value to which radVel is to be set.
 	 
 	
 	 */
 	void EphemerisRow::setRadVel (std::vector<double > radVel) {
	
 		this->radVel = radVel;
	
		radVelExists = true;
	
 	}
	
	
	/**
	 * Mark radVel, which is an optional field, as non-existent.
	 */
	void EphemerisRow::clearRadVel () {
		radVelExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a EphemerisRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	EphemerisRow::EphemerisRow (EphemerisTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	
		numPolyRadVelExists = false;
	

	
		radVelExists = false;
	

	
	
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["timeInterval"] = &EphemerisRow::timeIntervalFromBin; 
	 fromBinMethods["ephemerisId"] = &EphemerisRow::ephemerisIdFromBin; 
	 fromBinMethods["observerLocation"] = &EphemerisRow::observerLocationFromBin; 
	 fromBinMethods["equinoxEquator"] = &EphemerisRow::equinoxEquatorFromBin; 
	 fromBinMethods["numPolyDir"] = &EphemerisRow::numPolyDirFromBin; 
	 fromBinMethods["dir"] = &EphemerisRow::dirFromBin; 
	 fromBinMethods["numPolyDist"] = &EphemerisRow::numPolyDistFromBin; 
	 fromBinMethods["distance"] = &EphemerisRow::distanceFromBin; 
	 fromBinMethods["timeOrigin"] = &EphemerisRow::timeOriginFromBin; 
	 fromBinMethods["origin"] = &EphemerisRow::originFromBin; 
		
	
	 fromBinMethods["numPolyRadVel"] = &EphemerisRow::numPolyRadVelFromBin; 
	 fromBinMethods["radVel"] = &EphemerisRow::radVelFromBin; 
	
	
	
	
				 
	fromTextMethods["timeInterval"] = &EphemerisRow::timeIntervalFromText;
		 
	
				 
	fromTextMethods["ephemerisId"] = &EphemerisRow::ephemerisIdFromText;
		 
	
				 
	fromTextMethods["observerLocation"] = &EphemerisRow::observerLocationFromText;
		 
	
				 
	fromTextMethods["equinoxEquator"] = &EphemerisRow::equinoxEquatorFromText;
		 
	
				 
	fromTextMethods["numPolyDir"] = &EphemerisRow::numPolyDirFromText;
		 
	
				 
	fromTextMethods["dir"] = &EphemerisRow::dirFromText;
		 
	
				 
	fromTextMethods["numPolyDist"] = &EphemerisRow::numPolyDistFromText;
		 
	
				 
	fromTextMethods["distance"] = &EphemerisRow::distanceFromText;
		 
	
				 
	fromTextMethods["timeOrigin"] = &EphemerisRow::timeOriginFromText;
		 
	
				 
	fromTextMethods["origin"] = &EphemerisRow::originFromText;
		 
	

	 
				
	fromTextMethods["numPolyRadVel"] = &EphemerisRow::numPolyRadVelFromText;
		 	
	 
				
	fromTextMethods["radVel"] = &EphemerisRow::radVelFromText;
		 	
		
	}
	
	EphemerisRow::EphemerisRow (EphemerisTable &t, EphemerisRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	
		numPolyRadVelExists = false;
	

	
		radVelExists = false;
	

			
		}
		else {
	
		
			timeInterval = row->timeInterval;
		
			ephemerisId = row->ephemerisId;
		
		
		
		
			observerLocation = row->observerLocation;
		
			equinoxEquator = row->equinoxEquator;
		
			numPolyDir = row->numPolyDir;
		
			dir = row->dir;
		
			numPolyDist = row->numPolyDist;
		
			distance = row->distance;
		
			timeOrigin = row->timeOrigin;
		
			origin = row->origin;
		
		
		
		
		if (row->numPolyRadVelExists) {
			numPolyRadVel = row->numPolyRadVel;		
			numPolyRadVelExists = true;
		}
		else
			numPolyRadVelExists = false;
		
		if (row->radVelExists) {
			radVel = row->radVel;		
			radVelExists = true;
		}
		else
			radVelExists = false;
		
		}
		
		 fromBinMethods["timeInterval"] = &EphemerisRow::timeIntervalFromBin; 
		 fromBinMethods["ephemerisId"] = &EphemerisRow::ephemerisIdFromBin; 
		 fromBinMethods["observerLocation"] = &EphemerisRow::observerLocationFromBin; 
		 fromBinMethods["equinoxEquator"] = &EphemerisRow::equinoxEquatorFromBin; 
		 fromBinMethods["numPolyDir"] = &EphemerisRow::numPolyDirFromBin; 
		 fromBinMethods["dir"] = &EphemerisRow::dirFromBin; 
		 fromBinMethods["numPolyDist"] = &EphemerisRow::numPolyDistFromBin; 
		 fromBinMethods["distance"] = &EphemerisRow::distanceFromBin; 
		 fromBinMethods["timeOrigin"] = &EphemerisRow::timeOriginFromBin; 
		 fromBinMethods["origin"] = &EphemerisRow::originFromBin; 
			
	
		 fromBinMethods["numPolyRadVel"] = &EphemerisRow::numPolyRadVelFromBin; 
		 fromBinMethods["radVel"] = &EphemerisRow::radVelFromBin; 
			
	}

	
	bool EphemerisRow::compareNoAutoInc(ArrayTimeInterval timeInterval, int ephemerisId, std::vector<double > observerLocation, double equinoxEquator, int numPolyDir, std::vector<std::vector<double > > dir, int numPolyDist, std::vector<double > distance, ArrayTime timeOrigin, std::string origin) {
		bool result;
		result = true;
		
	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->ephemerisId == ephemerisId);
		
		if (!result) return false;
	

	
		
		result = result && (this->observerLocation == observerLocation);
		
		if (!result) return false;
	

	
		
		result = result && (this->equinoxEquator == equinoxEquator);
		
		if (!result) return false;
	

	
		
		result = result && (this->numPolyDir == numPolyDir);
		
		if (!result) return false;
	

	
		
		result = result && (this->dir == dir);
		
		if (!result) return false;
	

	
		
		result = result && (this->numPolyDist == numPolyDist);
		
		if (!result) return false;
	

	
		
		result = result && (this->distance == distance);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeOrigin == timeOrigin);
		
		if (!result) return false;
	

	
		
		result = result && (this->origin == origin);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool EphemerisRow::compareRequiredValue(std::vector<double > observerLocation, double equinoxEquator, int numPolyDir, std::vector<std::vector<double > > dir, int numPolyDist, std::vector<double > distance, ArrayTime timeOrigin, std::string origin) {
		bool result;
		result = true;
		
	
		if (!(this->observerLocation == observerLocation)) return false;
	

	
		if (!(this->equinoxEquator == equinoxEquator)) return false;
	

	
		if (!(this->numPolyDir == numPolyDir)) return false;
	

	
		if (!(this->dir == dir)) return false;
	

	
		if (!(this->numPolyDist == numPolyDist)) return false;
	

	
		if (!(this->distance == distance)) return false;
	

	
		if (!(this->timeOrigin == timeOrigin)) return false;
	

	
		if (!(this->origin == origin)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the EphemerisRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool EphemerisRow::equalByRequiredValue(EphemerisRow*  x ) {
		
			
		if (this->observerLocation != x->observerLocation) return false;
			
		if (this->equinoxEquator != x->equinoxEquator) return false;
			
		if (this->numPolyDir != x->numPolyDir) return false;
			
		if (this->dir != x->dir) return false;
			
		if (this->numPolyDist != x->numPolyDist) return false;
			
		if (this->distance != x->distance) return false;
			
		if (this->timeOrigin != x->timeOrigin) return false;
			
		if (this->origin != x->origin) return false;
			
		
		return true;
	}	
	
/*
	 map<string, EphemerisAttributeFromBin> EphemerisRow::initFromBinMethods() {
		map<string, EphemerisAttributeFromBin> result;
		
		result["timeInterval"] = &EphemerisRow::timeIntervalFromBin;
		result["ephemerisId"] = &EphemerisRow::ephemerisIdFromBin;
		result["observerLocation"] = &EphemerisRow::observerLocationFromBin;
		result["equinoxEquator"] = &EphemerisRow::equinoxEquatorFromBin;
		result["numPolyDir"] = &EphemerisRow::numPolyDirFromBin;
		result["dir"] = &EphemerisRow::dirFromBin;
		result["numPolyDist"] = &EphemerisRow::numPolyDistFromBin;
		result["distance"] = &EphemerisRow::distanceFromBin;
		result["timeOrigin"] = &EphemerisRow::timeOriginFromBin;
		result["origin"] = &EphemerisRow::originFromBin;
		
		
		result["numPolyRadVel"] = &EphemerisRow::numPolyRadVelFromBin;
		result["radVel"] = &EphemerisRow::radVelFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
