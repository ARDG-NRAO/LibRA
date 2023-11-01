
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
 * File CalFluxRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalFluxRow.h>
#include <alma/ASDM/CalFluxTable.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>
	

using asdm::ASDM;
using asdm::CalFluxRow;
using asdm::CalFluxTable;

using asdm::CalDataTable;
using asdm::CalDataRow;

using asdm::CalReductionTable;
using asdm::CalReductionRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	CalFluxRow::~CalFluxRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalFluxTable &CalFluxRow::getTable() const {
		return table;
	}

	bool CalFluxRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalFluxRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalFluxRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalFluxRowIDL struct.
	 */
	CalFluxRowIDL *CalFluxRow::toIDL() const {
		CalFluxRowIDL *x = new CalFluxRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->sourceName = CORBA::string_dup(sourceName.c_str());
				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->numFrequencyRanges = numFrequencyRanges;
 				
 			
		
	

	
  		
		
		
			
				
		x->numStokes = numStokes;
 				
 			
		
	

	
  		
		
		
			
		x->frequencyRanges.length(frequencyRanges.size());
		for (unsigned int i = 0; i < frequencyRanges.size(); i++) {
			x->frequencyRanges[i].length(frequencyRanges.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < frequencyRanges.size() ; i++)
			for (unsigned int j = 0; j < frequencyRanges.at(i).size(); j++)
					
				x->frequencyRanges[i][j]= frequencyRanges.at(i).at(j).toIDLFrequency();
									
		
			
		
	

	
  		
		
		
			
				
		x->fluxMethod = fluxMethod;
 				
 			
		
	

	
  		
		
		
			
		x->flux.length(flux.size());
		for (unsigned int i = 0; i < flux.size(); i++) {
			x->flux[i].length(flux.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < flux.size() ; i++)
			for (unsigned int j = 0; j < flux.at(i).size(); j++)
					
						
				x->flux[i][j] = flux.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->fluxError.length(fluxError.size());
		for (unsigned int i = 0; i < fluxError.size(); i++) {
			x->fluxError[i].length(fluxError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < fluxError.size() ; i++)
			for (unsigned int j = 0; j < fluxError.at(i).size(); j++)
					
						
				x->fluxError[i][j] = fluxError.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->stokes.length(stokes.size());
		for (unsigned int i = 0; i < stokes.size(); ++i) {
			
				
			x->stokes[i] = stokes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->directionExists = directionExists;
		
		
			
		x->direction.length(direction.size());
		for (unsigned int i = 0; i < direction.size(); ++i) {
			
			x->direction[i] = direction.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		x->directionCodeExists = directionCodeExists;
		
		
			
				
		x->directionCode = directionCode;
 				
 			
		
	

	
  		
		
		x->directionEquinoxExists = directionEquinoxExists;
		
		
			
		x->directionEquinox = directionEquinox.toIDLAngle();
			
		
	

	
  		
		
		x->PAExists = PAExists;
		
		
			
		x->PA.length(PA.size());
		for (unsigned int i = 0; i < PA.size(); i++) {
			x->PA[i].length(PA.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < PA.size() ; i++)
			for (unsigned int j = 0; j < PA.at(i).size(); j++)
					
				x->PA[i][j]= PA.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		x->PAErrorExists = PAErrorExists;
		
		
			
		x->PAError.length(PAError.size());
		for (unsigned int i = 0; i < PAError.size(); i++) {
			x->PAError[i].length(PAError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < PAError.size() ; i++)
			for (unsigned int j = 0; j < PAError.at(i).size(); j++)
					
				x->PAError[i][j]= PAError.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		x->sizeExists = sizeExists;
		
		
			
		x->size.length(size.size());
		for (unsigned int i = 0; i < size.size(); i++) {
			x->size[i].length(size.at(i).size());
			for (unsigned int j = 0; j < size.at(i).size(); j++) {
				x->size[i][j].length(size.at(i).at(j).size());
			}					 		
		}
		
		for (unsigned int i = 0; i < size.size() ; i++)
			for (unsigned int j = 0; j < size.at(i).size(); j++)
				for (unsigned int k = 0; k < size.at(i).at(j).size(); k++)
					
					x->size[i][j][k] = size.at(i).at(j).at(k).toIDLAngle();
												
			
		
	

	
  		
		
		x->sizeErrorExists = sizeErrorExists;
		
		
			
		x->sizeError.length(sizeError.size());
		for (unsigned int i = 0; i < sizeError.size(); i++) {
			x->sizeError[i].length(sizeError.at(i).size());
			for (unsigned int j = 0; j < sizeError.at(i).size(); j++) {
				x->sizeError[i][j].length(sizeError.at(i).at(j).size());
			}					 		
		}
		
		for (unsigned int i = 0; i < sizeError.size() ; i++)
			for (unsigned int j = 0; j < sizeError.at(i).size(); j++)
				for (unsigned int k = 0; k < sizeError.at(i).at(j).size(); k++)
					
					x->sizeError[i][j][k] = sizeError.at(i).at(j).at(k).toIDLAngle();
												
			
		
	

	
  		
		
		x->sourceModelExists = sourceModelExists;
		
		
			
				
		x->sourceModel = sourceModel;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalFluxRow::toIDL(asdmIDL::CalFluxRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.sourceName = CORBA::string_dup(sourceName.c_str());
				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.numFrequencyRanges = numFrequencyRanges;
 				
 			
		
	

	
  		
		
		
			
				
		x.numStokes = numStokes;
 				
 			
		
	

	
  		
		
		
			
		x.frequencyRanges.length(frequencyRanges.size());
		for (unsigned int i = 0; i < frequencyRanges.size(); i++) {
			x.frequencyRanges[i].length(frequencyRanges.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < frequencyRanges.size() ; i++)
			for (unsigned int j = 0; j < frequencyRanges.at(i).size(); j++)
					
				x.frequencyRanges[i][j]= frequencyRanges.at(i).at(j).toIDLFrequency();
									
		
			
		
	

	
  		
		
		
			
				
		x.fluxMethod = fluxMethod;
 				
 			
		
	

	
  		
		
		
			
		x.flux.length(flux.size());
		for (unsigned int i = 0; i < flux.size(); i++) {
			x.flux[i].length(flux.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < flux.size() ; i++)
			for (unsigned int j = 0; j < flux.at(i).size(); j++)
					
						
				x.flux[i][j] = flux.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.fluxError.length(fluxError.size());
		for (unsigned int i = 0; i < fluxError.size(); i++) {
			x.fluxError[i].length(fluxError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < fluxError.size() ; i++)
			for (unsigned int j = 0; j < fluxError.at(i).size(); j++)
					
						
				x.fluxError[i][j] = fluxError.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.stokes.length(stokes.size());
		for (unsigned int i = 0; i < stokes.size(); ++i) {
			
				
			x.stokes[i] = stokes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.directionExists = directionExists;
		
		
			
		x.direction.length(direction.size());
		for (unsigned int i = 0; i < direction.size(); ++i) {
			
			x.direction[i] = direction.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		x.directionCodeExists = directionCodeExists;
		
		
			
				
		x.directionCode = directionCode;
 				
 			
		
	

	
  		
		
		x.directionEquinoxExists = directionEquinoxExists;
		
		
			
		x.directionEquinox = directionEquinox.toIDLAngle();
			
		
	

	
  		
		
		x.PAExists = PAExists;
		
		
			
		x.PA.length(PA.size());
		for (unsigned int i = 0; i < PA.size(); i++) {
			x.PA[i].length(PA.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < PA.size() ; i++)
			for (unsigned int j = 0; j < PA.at(i).size(); j++)
					
				x.PA[i][j]= PA.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		x.PAErrorExists = PAErrorExists;
		
		
			
		x.PAError.length(PAError.size());
		for (unsigned int i = 0; i < PAError.size(); i++) {
			x.PAError[i].length(PAError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < PAError.size() ; i++)
			for (unsigned int j = 0; j < PAError.at(i).size(); j++)
					
				x.PAError[i][j]= PAError.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		x.sizeExists = sizeExists;
		
		
			
		x.size.length(size.size());
		for (unsigned int i = 0; i < size.size(); i++) {
			x.size[i].length(size.at(i).size());
			for (unsigned int j = 0; j < size.at(i).size(); j++) {
				x.size[i][j].length(size.at(i).at(j).size());
			}					 		
		}
		
		for (unsigned int i = 0; i < size.size() ; i++)
			for (unsigned int j = 0; j < size.at(i).size(); j++)
				for (unsigned int k = 0; k < size.at(i).at(j).size(); k++)
					
					x.size[i][j][k] = size.at(i).at(j).at(k).toIDLAngle();
												
			
		
	

	
  		
		
		x.sizeErrorExists = sizeErrorExists;
		
		
			
		x.sizeError.length(sizeError.size());
		for (unsigned int i = 0; i < sizeError.size(); i++) {
			x.sizeError[i].length(sizeError.at(i).size());
			for (unsigned int j = 0; j < sizeError.at(i).size(); j++) {
				x.sizeError[i][j].length(sizeError.at(i).at(j).size());
			}					 		
		}
		
		for (unsigned int i = 0; i < sizeError.size() ; i++)
			for (unsigned int j = 0; j < sizeError.at(i).size(); j++)
				for (unsigned int k = 0; k < sizeError.at(i).at(j).size(); k++)
					
					x.sizeError[i][j][k] = sizeError.at(i).at(j).at(k).toIDLAngle();
												
			
		
	

	
  		
		
		x.sourceModelExists = sourceModelExists;
		
		
			
				
		x.sourceModel = sourceModel;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalFluxRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalFluxRow::setFromIDL (CalFluxRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setSourceName(string (x.sourceName));
			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setNumFrequencyRanges(x.numFrequencyRanges);
  			
 		
		
	

	
		
		
			
		setNumStokes(x.numStokes);
  			
 		
		
	

	
		
		
			
		frequencyRanges .clear();
        
        vector<Frequency> v_aux_frequencyRanges;
        
		for (unsigned int i = 0; i < x.frequencyRanges.length(); ++i) {
			v_aux_frequencyRanges.clear();
			for (unsigned int j = 0; j < x.frequencyRanges[0].length(); ++j) {
				
				v_aux_frequencyRanges.push_back(Frequency (x.frequencyRanges[i][j]));
				
  			}
  			frequencyRanges.push_back(v_aux_frequencyRanges);			
		}
			
  		
		
	

	
		
		
			
		setFluxMethod(x.fluxMethod);
  			
 		
		
	

	
		
		
			
		flux .clear();
        
        vector<double> v_aux_flux;
        
		for (unsigned int i = 0; i < x.flux.length(); ++i) {
			v_aux_flux.clear();
			for (unsigned int j = 0; j < x.flux[0].length(); ++j) {
				
				v_aux_flux.push_back(x.flux[i][j]);
	  			
  			}
  			flux.push_back(v_aux_flux);			
		}
			
  		
		
	

	
		
		
			
		fluxError .clear();
        
        vector<double> v_aux_fluxError;
        
		for (unsigned int i = 0; i < x.fluxError.length(); ++i) {
			v_aux_fluxError.clear();
			for (unsigned int j = 0; j < x.fluxError[0].length(); ++j) {
				
				v_aux_fluxError.push_back(x.fluxError[i][j]);
	  			
  			}
  			fluxError.push_back(v_aux_fluxError);			
		}
			
  		
		
	

	
		
		
			
		stokes .clear();
		for (unsigned int i = 0; i <x.stokes.length(); ++i) {
			
			stokes.push_back(x.stokes[i]);
  			
		}
			
  		
		
	

	
		
		directionExists = x.directionExists;
		if (x.directionExists) {
		
		
			
		direction .clear();
		for (unsigned int i = 0; i <x.direction.length(); ++i) {
			
			direction.push_back(Angle (x.direction[i]));
			
		}
			
  		
		
		}
		
	

	
		
		directionCodeExists = x.directionCodeExists;
		if (x.directionCodeExists) {
		
		
			
		setDirectionCode(x.directionCode);
  			
 		
		
		}
		
	

	
		
		directionEquinoxExists = x.directionEquinoxExists;
		if (x.directionEquinoxExists) {
		
		
			
		setDirectionEquinox(Angle (x.directionEquinox));
			
 		
		
		}
		
	

	
		
		PAExists = x.PAExists;
		if (x.PAExists) {
		
		
			
		PA .clear();
        
        vector<Angle> v_aux_PA;
        
		for (unsigned int i = 0; i < x.PA.length(); ++i) {
			v_aux_PA.clear();
			for (unsigned int j = 0; j < x.PA[0].length(); ++j) {
				
				v_aux_PA.push_back(Angle (x.PA[i][j]));
				
  			}
  			PA.push_back(v_aux_PA);			
		}
			
  		
		
		}
		
	

	
		
		PAErrorExists = x.PAErrorExists;
		if (x.PAErrorExists) {
		
		
			
		PAError .clear();
        
        vector<Angle> v_aux_PAError;
        
		for (unsigned int i = 0; i < x.PAError.length(); ++i) {
			v_aux_PAError.clear();
			for (unsigned int j = 0; j < x.PAError[0].length(); ++j) {
				
				v_aux_PAError.push_back(Angle (x.PAError[i][j]));
				
  			}
  			PAError.push_back(v_aux_PAError);			
		}
			
  		
		
		}
		
	

	
		
		sizeExists = x.sizeExists;
		if (x.sizeExists) {
		
		
			
		size .clear();
		vector< vector<Angle> > vv_aux_size;
		vector<Angle> v_aux_size;
		
		for (unsigned int i = 0; i < x.size.length(); ++i) {
			vv_aux_size.clear();
			for (unsigned int j = 0; j < x.size[0].length(); ++j) {
				v_aux_size.clear();
				for (unsigned int k = 0; k < x.size[0][0].length(); ++k) {
					
					v_aux_size.push_back(Angle (x.size[i][j][k]));
					
		  		}
		  		vv_aux_size.push_back(v_aux_size);
  			}
  			size.push_back(vv_aux_size);
		}
			
  		
		
		}
		
	

	
		
		sizeErrorExists = x.sizeErrorExists;
		if (x.sizeErrorExists) {
		
		
			
		sizeError .clear();
		vector< vector<Angle> > vv_aux_sizeError;
		vector<Angle> v_aux_sizeError;
		
		for (unsigned int i = 0; i < x.sizeError.length(); ++i) {
			vv_aux_sizeError.clear();
			for (unsigned int j = 0; j < x.sizeError[0].length(); ++j) {
				v_aux_sizeError.clear();
				for (unsigned int k = 0; k < x.sizeError[0][0].length(); ++k) {
					
					v_aux_sizeError.push_back(Angle (x.sizeError[i][j][k]));
					
		  		}
		  		vv_aux_sizeError.push_back(v_aux_sizeError);
  			}
  			sizeError.push_back(vv_aux_sizeError);
		}
			
  		
		
		}
		
	

	
		
		sourceModelExists = x.sourceModelExists;
		if (x.sourceModelExists) {
		
		
			
		setSourceModel(x.sourceModel);
  			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalFlux");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalFluxRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(sourceName, "sourceName", buf);
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(numFrequencyRanges, "numFrequencyRanges", buf);
		
		
	

  	
 		
		
		Parser::toXML(numStokes, "numStokes", buf);
		
		
	

  	
 		
		
		Parser::toXML(frequencyRanges, "frequencyRanges", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("fluxMethod", fluxMethod));
		
		
	

  	
 		
		
		Parser::toXML(flux, "flux", buf);
		
		
	

  	
 		
		
		Parser::toXML(fluxError, "fluxError", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("stokes", stokes));
		
		
	

  	
 		
		if (directionExists) {
		
		
		Parser::toXML(direction, "direction", buf);
		
		
		}
		
	

  	
 		
		if (directionCodeExists) {
		
		
			buf.append(EnumerationParser::toXML("directionCode", directionCode));
		
		
		}
		
	

  	
 		
		if (directionEquinoxExists) {
		
		
		Parser::toXML(directionEquinox, "directionEquinox", buf);
		
		
		}
		
	

  	
 		
		if (PAExists) {
		
		
		Parser::toXML(PA, "PA", buf);
		
		
		}
		
	

  	
 		
		if (PAErrorExists) {
		
		
		Parser::toXML(PAError, "PAError", buf);
		
		
		}
		
	

  	
 		
		if (sizeExists) {
		
		
		Parser::toXML(size, "size", buf);
		
		
		}
		
	

  	
 		
		if (sizeErrorExists) {
		
		
		Parser::toXML(sizeError, "sizeError", buf);
		
		
		}
		
	

  	
 		
		if (sourceModelExists) {
		
		
			buf.append(EnumerationParser::toXML("sourceModel", sourceModel));
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(calDataId, "calDataId", buf);
		
		
	

  	
 		
		
		Parser::toXML(calReductionId, "calReductionId", buf);
		
		
	

	
		
	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void CalFluxRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setSourceName(Parser::getString("sourceName","CalFlux",rowDoc));
			
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalFlux",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalFlux",rowDoc));
			
		
	

	
  		
			
	  	setNumFrequencyRanges(Parser::getInteger("numFrequencyRanges","CalFlux",rowDoc));
			
		
	

	
  		
			
	  	setNumStokes(Parser::getInteger("numStokes","CalFlux",rowDoc));
			
		
	

	
  		
			
					
	  	setFrequencyRanges(Parser::get2DFrequency("frequencyRanges","CalFlux",rowDoc));
	  			
	  		
		
	

	
		
		
		
		fluxMethod = EnumerationParser::getFluxCalibrationMethod("fluxMethod","CalFlux",rowDoc);
		
		
		
	

	
  		
			
					
	  	setFlux(Parser::get2DDouble("flux","CalFlux",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setFluxError(Parser::get2DDouble("fluxError","CalFlux",rowDoc));
	  			
	  		
		
	

	
		
		
		
		stokes = EnumerationParser::getStokesParameter1D("stokes","CalFlux",rowDoc);			
		
		
		
	

	
  		
        if (row.isStr("<direction>")) {
			
								
	  		setDirection(Parser::get1DAngle("direction","CalFlux",rowDoc));
	  			
	  		
		}
 		
	

	
		
	if (row.isStr("<directionCode>")) {
		
		
		
		directionCode = EnumerationParser::getDirectionReferenceCode("directionCode","CalFlux",rowDoc);
		
		
		
		directionCodeExists = true;
	}
		
	

	
  		
        if (row.isStr("<directionEquinox>")) {
			
	  		setDirectionEquinox(Parser::getAngle("directionEquinox","CalFlux",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<PA>")) {
			
								
	  		setPA(Parser::get2DAngle("PA","CalFlux",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<PAError>")) {
			
								
	  		setPAError(Parser::get2DAngle("PAError","CalFlux",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<size>")) {
			
								
	  		setSize(Parser::get3DAngle("size","CalFlux",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<sizeError>")) {
			
								
	  		setSizeError(Parser::get3DAngle("sizeError","CalFlux",rowDoc));
	  			
	  		
		}
 		
	

	
		
	if (row.isStr("<sourceModel>")) {
		
		
		
		sourceModel = EnumerationParser::getSourceModel("sourceModel","CalFlux",rowDoc);
		
		
		
		sourceModelExists = true;
	}
		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalFlux");
		}
	}
	
	void CalFluxRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
						
			eoss.writeString(sourceName);
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numFrequencyRanges);
				
		
	

	
	
		
						
			eoss.writeInt(numStokes);
				
		
	

	
	
		
	Frequency::toBin(frequencyRanges, eoss);
		
	

	
	
		
					
		eoss.writeString(CFluxCalibrationMethod::name(fluxMethod));
			/* eoss.writeInt(fluxMethod); */
				
		
	

	
	
		
		
			
		eoss.writeInt((int) flux.size());
		eoss.writeInt((int) flux.at(0).size());
		for (unsigned int i = 0; i < flux.size(); i++) 
			for (unsigned int j = 0;  j < flux.at(0).size(); j++) 
							 
				eoss.writeDouble(flux.at(i).at(j));
				
	
						
		
	

	
	
		
		
			
		eoss.writeInt((int) fluxError.size());
		eoss.writeInt((int) fluxError.at(0).size());
		for (unsigned int i = 0; i < fluxError.size(); i++) 
			for (unsigned int j = 0;  j < fluxError.at(0).size(); j++) 
							 
				eoss.writeDouble(fluxError.at(i).at(j));
				
	
						
		
	

	
	
		
		
			
		eoss.writeInt((int) stokes.size());
		for (unsigned int i = 0; i < stokes.size(); i++)
				
			eoss.writeString(CStokesParameter::name(stokes.at(i)));
			/* eoss.writeInt(stokes.at(i)); */
				
				
						
		
	


	
	
	eoss.writeBoolean(directionExists);
	if (directionExists) {
	
	
	
		
	Angle::toBin(direction, eoss);
		
	

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

	eoss.writeBoolean(PAExists);
	if (PAExists) {
	
	
	
		
	Angle::toBin(PA, eoss);
		
	

	}

	eoss.writeBoolean(PAErrorExists);
	if (PAErrorExists) {
	
	
	
		
	Angle::toBin(PAError, eoss);
		
	

	}

	eoss.writeBoolean(sizeExists);
	if (sizeExists) {
	
	
	
		
	Angle::toBin(size, eoss);
		
	

	}

	eoss.writeBoolean(sizeErrorExists);
	if (sizeErrorExists) {
	
	
	
		
	Angle::toBin(sizeError, eoss);
		
	

	}

	eoss.writeBoolean(sourceModelExists);
	if (sourceModelExists) {
	
	
	
		
					
		eoss.writeString(CSourceModel::name(sourceModel));
			/* eoss.writeInt(sourceModel); */
				
		
	

	}

	}
	
void CalFluxRow::sourceNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		sourceName =  eis.readString();
			
		
	
	
}
void CalFluxRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalFluxRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalFluxRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalFluxRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalFluxRow::numFrequencyRangesFromBin(EndianIStream& eis) {
		
	
	
		
			
		numFrequencyRanges =  eis.readInt();
			
		
	
	
}
void CalFluxRow::numStokesFromBin(EndianIStream& eis) {
		
	
	
		
			
		numStokes =  eis.readInt();
			
		
	
	
}
void CalFluxRow::frequencyRangesFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	frequencyRanges = Frequency::from2DBin(eis);		
	

		
	
	
}
void CalFluxRow::fluxMethodFromBin(EndianIStream& eis) {
		
	
	
		
			
		fluxMethod = CFluxCalibrationMethod::literal(eis.readString());
			
		
	
	
}
void CalFluxRow::fluxFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		flux.clear();
		
		unsigned int fluxDim1 = eis.readInt();
		unsigned int fluxDim2 = eis.readInt();
        
		vector <double> fluxAux1;
        
		for (unsigned int i = 0; i < fluxDim1; i++) {
			fluxAux1.clear();
			for (unsigned int j = 0; j < fluxDim2 ; j++)			
			
			fluxAux1.push_back(eis.readDouble());
			
			flux.push_back(fluxAux1);
		}
	
	

		
	
	
}
void CalFluxRow::fluxErrorFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		fluxError.clear();
		
		unsigned int fluxErrorDim1 = eis.readInt();
		unsigned int fluxErrorDim2 = eis.readInt();
        
		vector <double> fluxErrorAux1;
        
		for (unsigned int i = 0; i < fluxErrorDim1; i++) {
			fluxErrorAux1.clear();
			for (unsigned int j = 0; j < fluxErrorDim2 ; j++)			
			
			fluxErrorAux1.push_back(eis.readDouble());
			
			fluxError.push_back(fluxErrorAux1);
		}
	
	

		
	
	
}
void CalFluxRow::stokesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		stokes.clear();
		
		unsigned int stokesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < stokesDim1; i++)
			
			stokes.push_back(CStokesParameter::literal(eis.readString()));
			
	

		
	
	
}

void CalFluxRow::directionFromBin(EndianIStream& eis) {
		
	directionExists = eis.readBoolean();
	if (directionExists) {
		
	
		
		
			
	
	direction = Angle::from1DBin(eis);	
	

		
	

	}
	
}
void CalFluxRow::directionCodeFromBin(EndianIStream& eis) {
		
	directionCodeExists = eis.readBoolean();
	if (directionCodeExists) {
		
	
	
		
			
		directionCode = CDirectionReferenceCode::literal(eis.readString());
			
		
	

	}
	
}
void CalFluxRow::directionEquinoxFromBin(EndianIStream& eis) {
		
	directionEquinoxExists = eis.readBoolean();
	if (directionEquinoxExists) {
		
	
		
		
		directionEquinox =  Angle::fromBin(eis);
		
	

	}
	
}
void CalFluxRow::PAFromBin(EndianIStream& eis) {
		
	PAExists = eis.readBoolean();
	if (PAExists) {
		
	
		
		
			
	
	PA = Angle::from2DBin(eis);		
	

		
	

	}
	
}
void CalFluxRow::PAErrorFromBin(EndianIStream& eis) {
		
	PAErrorExists = eis.readBoolean();
	if (PAErrorExists) {
		
	
		
		
			
	
	PAError = Angle::from2DBin(eis);		
	

		
	

	}
	
}
void CalFluxRow::sizeFromBin(EndianIStream& eis) {
		
	sizeExists = eis.readBoolean();
	if (sizeExists) {
		
	
		
		
			
	
	size = Angle::from3DBin(eis);		
	

		
	

	}
	
}
void CalFluxRow::sizeErrorFromBin(EndianIStream& eis) {
		
	sizeErrorExists = eis.readBoolean();
	if (sizeErrorExists) {
		
	
		
		
			
	
	sizeError = Angle::from3DBin(eis);		
	

		
	

	}
	
}
void CalFluxRow::sourceModelFromBin(EndianIStream& eis) {
		
	sourceModelExists = eis.readBoolean();
	if (sourceModelExists) {
		
	
	
		
			
		sourceModel = CSourceModel::literal(eis.readString());
			
		
	

	}
	
}
	
	
	CalFluxRow* CalFluxRow::fromBin(EndianIStream& eis, CalFluxTable& table, const vector<string>& attributesSeq) {
		CalFluxRow* row = new  CalFluxRow(table);
		
		map<string, CalFluxAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalFluxTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an String 
	void CalFluxRow::sourceNameFromText(const string & s) {
		 
          
		sourceName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalFluxRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalFluxRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalFluxRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalFluxRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalFluxRow::numFrequencyRangesFromText(const string & s) {
		 
          
		numFrequencyRanges = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalFluxRow::numStokesFromText(const string & s) {
		 
          
		numStokes = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalFluxRow::frequencyRangesFromText(const string & s) {
		 
          
		frequencyRanges = ASDMValuesParser::parse2D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an FluxCalibrationMethod 
	void CalFluxRow::fluxMethodFromText(const string & s) {
		 
          
		fluxMethod = ASDMValuesParser::parse<FluxCalibrationMethodMod::FluxCalibrationMethod>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalFluxRow::fluxFromText(const string & s) {
		 
          
		flux = ASDMValuesParser::parse2D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalFluxRow::fluxErrorFromText(const string & s) {
		 
          
		fluxError = ASDMValuesParser::parse2D<double>(s);
          
		
	}
	
	
	// Convert a string into an StokesParameter 
	void CalFluxRow::stokesFromText(const string & s) {
		 
          
		stokes = ASDMValuesParser::parse1D<StokesParameterMod::StokesParameter>(s);
          
		
	}
	

	
	// Convert a string into an Angle 
	void CalFluxRow::directionFromText(const string & s) {
		directionExists = true;
		 
          
		direction = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an DirectionReferenceCode 
	void CalFluxRow::directionCodeFromText(const string & s) {
		directionCodeExists = true;
		 
          
		directionCode = ASDMValuesParser::parse<DirectionReferenceCodeMod::DirectionReferenceCode>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalFluxRow::directionEquinoxFromText(const string & s) {
		directionEquinoxExists = true;
		 
          
		directionEquinox = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalFluxRow::PAFromText(const string & s) {
		PAExists = true;
		 
          
		PA = ASDMValuesParser::parse2D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalFluxRow::PAErrorFromText(const string & s) {
		PAErrorExists = true;
		 
          
		PAError = ASDMValuesParser::parse2D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalFluxRow::sizeFromText(const string & s) {
		sizeExists = true;
		 
          
		size = ASDMValuesParser::parse3D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalFluxRow::sizeErrorFromText(const string & s) {
		sizeErrorExists = true;
		 
          
		sizeError = ASDMValuesParser::parse3D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an SourceModel 
	void CalFluxRow::sourceModelFromText(const string & s) {
		sourceModelExists = true;
		 
          
		sourceModel = ASDMValuesParser::parse<SourceModelMod::SourceModel>(s);
          
		
	}
	
	
	
	void CalFluxRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalFluxAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalFluxTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get sourceName.
 	 * @return sourceName as std::string
 	 */
 	std::string CalFluxRow::getSourceName() const {
	
  		return sourceName;
 	}

 	/**
 	 * Set sourceName with the specified std::string.
 	 * @param sourceName The std::string value to which sourceName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFluxRow::setSourceName (std::string sourceName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("sourceName", "CalFlux");
		
  		}
  	
 		this->sourceName = sourceName;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalFluxRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalFluxRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get numFrequencyRanges.
 	 * @return numFrequencyRanges as int
 	 */
 	int CalFluxRow::getNumFrequencyRanges() const {
	
  		return numFrequencyRanges;
 	}

 	/**
 	 * Set numFrequencyRanges with the specified int.
 	 * @param numFrequencyRanges The int value to which numFrequencyRanges is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setNumFrequencyRanges (int numFrequencyRanges)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numFrequencyRanges = numFrequencyRanges;
	
 	}
	
	

	

	
 	/**
 	 * Get numStokes.
 	 * @return numStokes as int
 	 */
 	int CalFluxRow::getNumStokes() const {
	
  		return numStokes;
 	}

 	/**
 	 * Set numStokes with the specified int.
 	 * @param numStokes The int value to which numStokes is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setNumStokes (int numStokes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numStokes = numStokes;
	
 	}
	
	

	

	
 	/**
 	 * Get frequencyRanges.
 	 * @return frequencyRanges as std::vector<std::vector<Frequency > >
 	 */
 	std::vector<std::vector<Frequency > > CalFluxRow::getFrequencyRanges() const {
	
  		return frequencyRanges;
 	}

 	/**
 	 * Set frequencyRanges with the specified std::vector<std::vector<Frequency > >.
 	 * @param frequencyRanges The std::vector<std::vector<Frequency > > value to which frequencyRanges is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setFrequencyRanges (std::vector<std::vector<Frequency > > frequencyRanges)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->frequencyRanges = frequencyRanges;
	
 	}
	
	

	

	
 	/**
 	 * Get fluxMethod.
 	 * @return fluxMethod as FluxCalibrationMethodMod::FluxCalibrationMethod
 	 */
 	FluxCalibrationMethodMod::FluxCalibrationMethod CalFluxRow::getFluxMethod() const {
	
  		return fluxMethod;
 	}

 	/**
 	 * Set fluxMethod with the specified FluxCalibrationMethodMod::FluxCalibrationMethod.
 	 * @param fluxMethod The FluxCalibrationMethodMod::FluxCalibrationMethod value to which fluxMethod is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setFluxMethod (FluxCalibrationMethodMod::FluxCalibrationMethod fluxMethod)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->fluxMethod = fluxMethod;
	
 	}
	
	

	

	
 	/**
 	 * Get flux.
 	 * @return flux as std::vector<std::vector<double > >
 	 */
 	std::vector<std::vector<double > > CalFluxRow::getFlux() const {
	
  		return flux;
 	}

 	/**
 	 * Set flux with the specified std::vector<std::vector<double > >.
 	 * @param flux The std::vector<std::vector<double > > value to which flux is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setFlux (std::vector<std::vector<double > > flux)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->flux = flux;
	
 	}
	
	

	

	
 	/**
 	 * Get fluxError.
 	 * @return fluxError as std::vector<std::vector<double > >
 	 */
 	std::vector<std::vector<double > > CalFluxRow::getFluxError() const {
	
  		return fluxError;
 	}

 	/**
 	 * Set fluxError with the specified std::vector<std::vector<double > >.
 	 * @param fluxError The std::vector<std::vector<double > > value to which fluxError is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setFluxError (std::vector<std::vector<double > > fluxError)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->fluxError = fluxError;
	
 	}
	
	

	

	
 	/**
 	 * Get stokes.
 	 * @return stokes as std::vector<StokesParameterMod::StokesParameter >
 	 */
 	std::vector<StokesParameterMod::StokesParameter > CalFluxRow::getStokes() const {
	
  		return stokes;
 	}

 	/**
 	 * Set stokes with the specified std::vector<StokesParameterMod::StokesParameter >.
 	 * @param stokes The std::vector<StokesParameterMod::StokesParameter > value to which stokes is to be set.
 	 
 	
 		
 	 */
 	void CalFluxRow::setStokes (std::vector<StokesParameterMod::StokesParameter > stokes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->stokes = stokes;
	
 	}
	
	

	
	/**
	 * The attribute direction is optional. Return true if this attribute exists.
	 * @return true if and only if the direction attribute exists. 
	 */
	bool CalFluxRow::isDirectionExists() const {
		return directionExists;
	}
	

	
 	/**
 	 * Get direction, which is optional.
 	 * @return direction as std::vector<Angle >
 	 * @throw IllegalAccessException If direction does not exist.
 	 */
 	std::vector<Angle > CalFluxRow::getDirection() const  {
		if (!directionExists) {
			throw IllegalAccessException("direction", "CalFlux");
		}
	
  		return direction;
 	}

 	/**
 	 * Set direction with the specified std::vector<Angle >.
 	 * @param direction The std::vector<Angle > value to which direction is to be set.
 	 
 	
 	 */
 	void CalFluxRow::setDirection (std::vector<Angle > direction) {
	
 		this->direction = direction;
	
		directionExists = true;
	
 	}
	
	
	/**
	 * Mark direction, which is an optional field, as non-existent.
	 */
	void CalFluxRow::clearDirection () {
		directionExists = false;
	}
	

	
	/**
	 * The attribute directionCode is optional. Return true if this attribute exists.
	 * @return true if and only if the directionCode attribute exists. 
	 */
	bool CalFluxRow::isDirectionCodeExists() const {
		return directionCodeExists;
	}
	

	
 	/**
 	 * Get directionCode, which is optional.
 	 * @return directionCode as DirectionReferenceCodeMod::DirectionReferenceCode
 	 * @throw IllegalAccessException If directionCode does not exist.
 	 */
 	DirectionReferenceCodeMod::DirectionReferenceCode CalFluxRow::getDirectionCode() const  {
		if (!directionCodeExists) {
			throw IllegalAccessException("directionCode", "CalFlux");
		}
	
  		return directionCode;
 	}

 	/**
 	 * Set directionCode with the specified DirectionReferenceCodeMod::DirectionReferenceCode.
 	 * @param directionCode The DirectionReferenceCodeMod::DirectionReferenceCode value to which directionCode is to be set.
 	 
 	
 	 */
 	void CalFluxRow::setDirectionCode (DirectionReferenceCodeMod::DirectionReferenceCode directionCode) {
	
 		this->directionCode = directionCode;
	
		directionCodeExists = true;
	
 	}
	
	
	/**
	 * Mark directionCode, which is an optional field, as non-existent.
	 */
	void CalFluxRow::clearDirectionCode () {
		directionCodeExists = false;
	}
	

	
	/**
	 * The attribute directionEquinox is optional. Return true if this attribute exists.
	 * @return true if and only if the directionEquinox attribute exists. 
	 */
	bool CalFluxRow::isDirectionEquinoxExists() const {
		return directionEquinoxExists;
	}
	

	
 	/**
 	 * Get directionEquinox, which is optional.
 	 * @return directionEquinox as Angle
 	 * @throw IllegalAccessException If directionEquinox does not exist.
 	 */
 	Angle CalFluxRow::getDirectionEquinox() const  {
		if (!directionEquinoxExists) {
			throw IllegalAccessException("directionEquinox", "CalFlux");
		}
	
  		return directionEquinox;
 	}

 	/**
 	 * Set directionEquinox with the specified Angle.
 	 * @param directionEquinox The Angle value to which directionEquinox is to be set.
 	 
 	
 	 */
 	void CalFluxRow::setDirectionEquinox (Angle directionEquinox) {
	
 		this->directionEquinox = directionEquinox;
	
		directionEquinoxExists = true;
	
 	}
	
	
	/**
	 * Mark directionEquinox, which is an optional field, as non-existent.
	 */
	void CalFluxRow::clearDirectionEquinox () {
		directionEquinoxExists = false;
	}
	

	
	/**
	 * The attribute PA is optional. Return true if this attribute exists.
	 * @return true if and only if the PA attribute exists. 
	 */
	bool CalFluxRow::isPAExists() const {
		return PAExists;
	}
	

	
 	/**
 	 * Get PA, which is optional.
 	 * @return PA as std::vector<std::vector<Angle > >
 	 * @throw IllegalAccessException If PA does not exist.
 	 */
 	std::vector<std::vector<Angle > > CalFluxRow::getPA() const  {
		if (!PAExists) {
			throw IllegalAccessException("PA", "CalFlux");
		}
	
  		return PA;
 	}

 	/**
 	 * Set PA with the specified std::vector<std::vector<Angle > >.
 	 * @param PA The std::vector<std::vector<Angle > > value to which PA is to be set.
 	 
 	
 	 */
 	void CalFluxRow::setPA (std::vector<std::vector<Angle > > PA) {
	
 		this->PA = PA;
	
		PAExists = true;
	
 	}
	
	
	/**
	 * Mark PA, which is an optional field, as non-existent.
	 */
	void CalFluxRow::clearPA () {
		PAExists = false;
	}
	

	
	/**
	 * The attribute PAError is optional. Return true if this attribute exists.
	 * @return true if and only if the PAError attribute exists. 
	 */
	bool CalFluxRow::isPAErrorExists() const {
		return PAErrorExists;
	}
	

	
 	/**
 	 * Get PAError, which is optional.
 	 * @return PAError as std::vector<std::vector<Angle > >
 	 * @throw IllegalAccessException If PAError does not exist.
 	 */
 	std::vector<std::vector<Angle > > CalFluxRow::getPAError() const  {
		if (!PAErrorExists) {
			throw IllegalAccessException("PAError", "CalFlux");
		}
	
  		return PAError;
 	}

 	/**
 	 * Set PAError with the specified std::vector<std::vector<Angle > >.
 	 * @param PAError The std::vector<std::vector<Angle > > value to which PAError is to be set.
 	 
 	
 	 */
 	void CalFluxRow::setPAError (std::vector<std::vector<Angle > > PAError) {
	
 		this->PAError = PAError;
	
		PAErrorExists = true;
	
 	}
	
	
	/**
	 * Mark PAError, which is an optional field, as non-existent.
	 */
	void CalFluxRow::clearPAError () {
		PAErrorExists = false;
	}
	

	
	/**
	 * The attribute size is optional. Return true if this attribute exists.
	 * @return true if and only if the size attribute exists. 
	 */
	bool CalFluxRow::isSizeExists() const {
		return sizeExists;
	}
	

	
 	/**
 	 * Get size, which is optional.
 	 * @return size as std::vector<std::vector<std::vector<Angle > > >
 	 * @throw IllegalAccessException If size does not exist.
 	 */
 	std::vector<std::vector<std::vector<Angle > > > CalFluxRow::getSize() const  {
		if (!sizeExists) {
			throw IllegalAccessException("size", "CalFlux");
		}
	
  		return size;
 	}

 	/**
 	 * Set size with the specified std::vector<std::vector<std::vector<Angle > > >.
 	 * @param size The std::vector<std::vector<std::vector<Angle > > > value to which size is to be set.
 	 
 	
 	 */
 	void CalFluxRow::setSize (std::vector<std::vector<std::vector<Angle > > > size) {
	
 		this->size = size;
	
		sizeExists = true;
	
 	}
	
	
	/**
	 * Mark size, which is an optional field, as non-existent.
	 */
	void CalFluxRow::clearSize () {
		sizeExists = false;
	}
	

	
	/**
	 * The attribute sizeError is optional. Return true if this attribute exists.
	 * @return true if and only if the sizeError attribute exists. 
	 */
	bool CalFluxRow::isSizeErrorExists() const {
		return sizeErrorExists;
	}
	

	
 	/**
 	 * Get sizeError, which is optional.
 	 * @return sizeError as std::vector<std::vector<std::vector<Angle > > >
 	 * @throw IllegalAccessException If sizeError does not exist.
 	 */
 	std::vector<std::vector<std::vector<Angle > > > CalFluxRow::getSizeError() const  {
		if (!sizeErrorExists) {
			throw IllegalAccessException("sizeError", "CalFlux");
		}
	
  		return sizeError;
 	}

 	/**
 	 * Set sizeError with the specified std::vector<std::vector<std::vector<Angle > > >.
 	 * @param sizeError The std::vector<std::vector<std::vector<Angle > > > value to which sizeError is to be set.
 	 
 	
 	 */
 	void CalFluxRow::setSizeError (std::vector<std::vector<std::vector<Angle > > > sizeError) {
	
 		this->sizeError = sizeError;
	
		sizeErrorExists = true;
	
 	}
	
	
	/**
	 * Mark sizeError, which is an optional field, as non-existent.
	 */
	void CalFluxRow::clearSizeError () {
		sizeErrorExists = false;
	}
	

	
	/**
	 * The attribute sourceModel is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceModel attribute exists. 
	 */
	bool CalFluxRow::isSourceModelExists() const {
		return sourceModelExists;
	}
	

	
 	/**
 	 * Get sourceModel, which is optional.
 	 * @return sourceModel as SourceModelMod::SourceModel
 	 * @throw IllegalAccessException If sourceModel does not exist.
 	 */
 	SourceModelMod::SourceModel CalFluxRow::getSourceModel() const  {
		if (!sourceModelExists) {
			throw IllegalAccessException("sourceModel", "CalFlux");
		}
	
  		return sourceModel;
 	}

 	/**
 	 * Set sourceModel with the specified SourceModelMod::SourceModel.
 	 * @param sourceModel The SourceModelMod::SourceModel value to which sourceModel is to be set.
 	 
 	
 	 */
 	void CalFluxRow::setSourceModel (SourceModelMod::SourceModel sourceModel) {
	
 		this->sourceModel = sourceModel;
	
		sourceModelExists = true;
	
 	}
	
	
	/**
	 * Mark sourceModel, which is an optional field, as non-existent.
	 */
	void CalFluxRow::clearSourceModel () {
		sourceModelExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalFluxRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFluxRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalFlux");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalFluxRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFluxRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalFlux");
		
  		}
  	
 		this->calReductionId = calReductionId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* CalFluxRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalFluxRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	/**
	 * Create a CalFluxRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalFluxRow::CalFluxRow (CalFluxTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	
		directionExists = false;
	

	
		directionCodeExists = false;
	

	
		directionEquinoxExists = false;
	

	
		PAExists = false;
	

	
		PAErrorExists = false;
	

	
		sizeExists = false;
	

	
		sizeErrorExists = false;
	

	
		sourceModelExists = false;
	

	
	

	

	
	
	
	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
fluxMethod = CFluxCalibrationMethod::from_int(0);
	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
directionCode = CDirectionReferenceCode::from_int(0);
	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
sourceModel = CSourceModel::from_int(0);
	

	
	
	 fromBinMethods["sourceName"] = &CalFluxRow::sourceNameFromBin; 
	 fromBinMethods["calDataId"] = &CalFluxRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalFluxRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalFluxRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalFluxRow::endValidTimeFromBin; 
	 fromBinMethods["numFrequencyRanges"] = &CalFluxRow::numFrequencyRangesFromBin; 
	 fromBinMethods["numStokes"] = &CalFluxRow::numStokesFromBin; 
	 fromBinMethods["frequencyRanges"] = &CalFluxRow::frequencyRangesFromBin; 
	 fromBinMethods["fluxMethod"] = &CalFluxRow::fluxMethodFromBin; 
	 fromBinMethods["flux"] = &CalFluxRow::fluxFromBin; 
	 fromBinMethods["fluxError"] = &CalFluxRow::fluxErrorFromBin; 
	 fromBinMethods["stokes"] = &CalFluxRow::stokesFromBin; 
		
	
	 fromBinMethods["direction"] = &CalFluxRow::directionFromBin; 
	 fromBinMethods["directionCode"] = &CalFluxRow::directionCodeFromBin; 
	 fromBinMethods["directionEquinox"] = &CalFluxRow::directionEquinoxFromBin; 
	 fromBinMethods["PA"] = &CalFluxRow::PAFromBin; 
	 fromBinMethods["PAError"] = &CalFluxRow::PAErrorFromBin; 
	 fromBinMethods["size"] = &CalFluxRow::sizeFromBin; 
	 fromBinMethods["sizeError"] = &CalFluxRow::sizeErrorFromBin; 
	 fromBinMethods["sourceModel"] = &CalFluxRow::sourceModelFromBin; 
	
	
	
	
				 
	fromTextMethods["sourceName"] = &CalFluxRow::sourceNameFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalFluxRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalFluxRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalFluxRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalFluxRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["numFrequencyRanges"] = &CalFluxRow::numFrequencyRangesFromText;
		 
	
				 
	fromTextMethods["numStokes"] = &CalFluxRow::numStokesFromText;
		 
	
				 
	fromTextMethods["frequencyRanges"] = &CalFluxRow::frequencyRangesFromText;
		 
	
				 
	fromTextMethods["fluxMethod"] = &CalFluxRow::fluxMethodFromText;
		 
	
				 
	fromTextMethods["flux"] = &CalFluxRow::fluxFromText;
		 
	
				 
	fromTextMethods["fluxError"] = &CalFluxRow::fluxErrorFromText;
		 
	
				 
	fromTextMethods["stokes"] = &CalFluxRow::stokesFromText;
		 
	

	 
				
	fromTextMethods["direction"] = &CalFluxRow::directionFromText;
		 	
	 
				
	fromTextMethods["directionCode"] = &CalFluxRow::directionCodeFromText;
		 	
	 
				
	fromTextMethods["directionEquinox"] = &CalFluxRow::directionEquinoxFromText;
		 	
	 
				
	fromTextMethods["PA"] = &CalFluxRow::PAFromText;
		 	
	 
				
	fromTextMethods["PAError"] = &CalFluxRow::PAErrorFromText;
		 	
	 
				
	fromTextMethods["size"] = &CalFluxRow::sizeFromText;
		 	
	 
				
	fromTextMethods["sizeError"] = &CalFluxRow::sizeErrorFromText;
		 	
	 
				
	fromTextMethods["sourceModel"] = &CalFluxRow::sourceModelFromText;
		 	
		
	}
	
	CalFluxRow::CalFluxRow (CalFluxTable &t, CalFluxRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	
		directionExists = false;
	

	
		directionCodeExists = false;
	

	
		directionEquinoxExists = false;
	

	
		PAExists = false;
	

	
		PAErrorExists = false;
	

	
		sizeExists = false;
	

	
		sizeErrorExists = false;
	

	
		sourceModelExists = false;
	

	
	

	
		
		}
		else {
	
		
			sourceName = row->sourceName;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			numFrequencyRanges = row->numFrequencyRanges;
		
			numStokes = row->numStokes;
		
			frequencyRanges = row->frequencyRanges;
		
			fluxMethod = row->fluxMethod;
		
			flux = row->flux;
		
			fluxError = row->fluxError;
		
			stokes = row->stokes;
		
		
		
		
		if (row->directionExists) {
			direction = row->direction;		
			directionExists = true;
		}
		else
			directionExists = false;
		
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
		
		if (row->PAExists) {
			PA = row->PA;		
			PAExists = true;
		}
		else
			PAExists = false;
		
		if (row->PAErrorExists) {
			PAError = row->PAError;		
			PAErrorExists = true;
		}
		else
			PAErrorExists = false;
		
		if (row->sizeExists) {
			size = row->size;		
			sizeExists = true;
		}
		else
			sizeExists = false;
		
		if (row->sizeErrorExists) {
			sizeError = row->sizeError;		
			sizeErrorExists = true;
		}
		else
			sizeErrorExists = false;
		
		if (row->sourceModelExists) {
			sourceModel = row->sourceModel;		
			sourceModelExists = true;
		}
		else
			sourceModelExists = false;
		
		}
		
		 fromBinMethods["sourceName"] = &CalFluxRow::sourceNameFromBin; 
		 fromBinMethods["calDataId"] = &CalFluxRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalFluxRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalFluxRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalFluxRow::endValidTimeFromBin; 
		 fromBinMethods["numFrequencyRanges"] = &CalFluxRow::numFrequencyRangesFromBin; 
		 fromBinMethods["numStokes"] = &CalFluxRow::numStokesFromBin; 
		 fromBinMethods["frequencyRanges"] = &CalFluxRow::frequencyRangesFromBin; 
		 fromBinMethods["fluxMethod"] = &CalFluxRow::fluxMethodFromBin; 
		 fromBinMethods["flux"] = &CalFluxRow::fluxFromBin; 
		 fromBinMethods["fluxError"] = &CalFluxRow::fluxErrorFromBin; 
		 fromBinMethods["stokes"] = &CalFluxRow::stokesFromBin; 
			
	
		 fromBinMethods["direction"] = &CalFluxRow::directionFromBin; 
		 fromBinMethods["directionCode"] = &CalFluxRow::directionCodeFromBin; 
		 fromBinMethods["directionEquinox"] = &CalFluxRow::directionEquinoxFromBin; 
		 fromBinMethods["PA"] = &CalFluxRow::PAFromBin; 
		 fromBinMethods["PAError"] = &CalFluxRow::PAErrorFromBin; 
		 fromBinMethods["size"] = &CalFluxRow::sizeFromBin; 
		 fromBinMethods["sizeError"] = &CalFluxRow::sizeErrorFromBin; 
		 fromBinMethods["sourceModel"] = &CalFluxRow::sourceModelFromBin; 
			
	}

	
	bool CalFluxRow::compareNoAutoInc(std::string sourceName, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numFrequencyRanges, int numStokes, std::vector<std::vector<Frequency > > frequencyRanges, FluxCalibrationMethodMod::FluxCalibrationMethod fluxMethod, std::vector<std::vector<double > > flux, std::vector<std::vector<double > > fluxError, std::vector<StokesParameterMod::StokesParameter > stokes) {
		bool result;
		result = true;
		
	
		
		result = result && (this->sourceName == sourceName);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataId == calDataId);
		
		if (!result) return false;
	

	
		
		result = result && (this->calReductionId == calReductionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->startValidTime == startValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endValidTime == endValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->numFrequencyRanges == numFrequencyRanges);
		
		if (!result) return false;
	

	
		
		result = result && (this->numStokes == numStokes);
		
		if (!result) return false;
	

	
		
		result = result && (this->frequencyRanges == frequencyRanges);
		
		if (!result) return false;
	

	
		
		result = result && (this->fluxMethod == fluxMethod);
		
		if (!result) return false;
	

	
		
		result = result && (this->flux == flux);
		
		if (!result) return false;
	

	
		
		result = result && (this->fluxError == fluxError);
		
		if (!result) return false;
	

	
		
		result = result && (this->stokes == stokes);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalFluxRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, int numFrequencyRanges, int numStokes, std::vector<std::vector<Frequency > > frequencyRanges, FluxCalibrationMethodMod::FluxCalibrationMethod fluxMethod, std::vector<std::vector<double > > flux, std::vector<std::vector<double > > fluxError, std::vector<StokesParameterMod::StokesParameter > stokes) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->numFrequencyRanges == numFrequencyRanges)) return false;
	

	
		if (!(this->numStokes == numStokes)) return false;
	

	
		if (!(this->frequencyRanges == frequencyRanges)) return false;
	

	
		if (!(this->fluxMethod == fluxMethod)) return false;
	

	
		if (!(this->flux == flux)) return false;
	

	
		if (!(this->fluxError == fluxError)) return false;
	

	
		if (!(this->stokes == stokes)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalFluxRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalFluxRow::equalByRequiredValue(CalFluxRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->numFrequencyRanges != x->numFrequencyRanges) return false;
			
		if (this->numStokes != x->numStokes) return false;
			
		if (this->frequencyRanges != x->frequencyRanges) return false;
			
		if (this->fluxMethod != x->fluxMethod) return false;
			
		if (this->flux != x->flux) return false;
			
		if (this->fluxError != x->fluxError) return false;
			
		if (this->stokes != x->stokes) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalFluxAttributeFromBin> CalFluxRow::initFromBinMethods() {
		map<string, CalFluxAttributeFromBin> result;
		
		result["sourceName"] = &CalFluxRow::sourceNameFromBin;
		result["calDataId"] = &CalFluxRow::calDataIdFromBin;
		result["calReductionId"] = &CalFluxRow::calReductionIdFromBin;
		result["startValidTime"] = &CalFluxRow::startValidTimeFromBin;
		result["endValidTime"] = &CalFluxRow::endValidTimeFromBin;
		result["numFrequencyRanges"] = &CalFluxRow::numFrequencyRangesFromBin;
		result["numStokes"] = &CalFluxRow::numStokesFromBin;
		result["frequencyRanges"] = &CalFluxRow::frequencyRangesFromBin;
		result["fluxMethod"] = &CalFluxRow::fluxMethodFromBin;
		result["flux"] = &CalFluxRow::fluxFromBin;
		result["fluxError"] = &CalFluxRow::fluxErrorFromBin;
		result["stokes"] = &CalFluxRow::stokesFromBin;
		
		
		result["direction"] = &CalFluxRow::directionFromBin;
		result["directionCode"] = &CalFluxRow::directionCodeFromBin;
		result["directionEquinox"] = &CalFluxRow::directionEquinoxFromBin;
		result["PA"] = &CalFluxRow::PAFromBin;
		result["PAError"] = &CalFluxRow::PAErrorFromBin;
		result["size"] = &CalFluxRow::sizeFromBin;
		result["sizeError"] = &CalFluxRow::sizeErrorFromBin;
		result["sourceModel"] = &CalFluxRow::sourceModelFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
