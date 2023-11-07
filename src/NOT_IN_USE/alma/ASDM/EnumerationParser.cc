
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
 * /////////////////////////////////////////////////////////////////
 * // WARNING!  DO NOT MODIFY THIS FILE!                          //
 * //  ---------------------------------------------------------  //
 * // | This is generated code!  Do not modify this file.       | //
 * // | Any changes will be lost when the file is re-generated. | //
 * //  ---------------------------------------------------------  //
 * /////////////////////////////////////////////////////////////////
 *
 * File EnumerationParser.cpp
 */
 
 #include <alma/ASDM/EnumerationParser.h>
 #include <sstream>
 #include <stdlib.h> // for atoi()
 #include <errno.h>  // to detect exception raised by atoi.
 using namespace std;
 
 using namespace asdm;


 using namespace ReceiverBandMod;

 using namespace SBTypeMod;

 using namespace DirectionReferenceCodeMod;

 using namespace CorrelationModeMod;

 using namespace AtmPhaseCorrectionMod;

 using namespace ProcessorTypeMod;

 using namespace SpectralResolutionTypeMod;

 using namespace CalibrationDeviceMod;

 using namespace AntennaMakeMod;

 using namespace AntennaTypeMod;

 using namespace SourceModelMod;

 using namespace FrequencyReferenceCodeMod;

 using namespace StokesParameterMod;

 using namespace RadialVelocityReferenceCodeMod;

 using namespace DopplerReferenceCodeMod;

 using namespace PolarizationTypeMod;

 using namespace BasebandNameMod;

 using namespace NetSidebandMod;

 using namespace SidebandProcessingModeMod;

 using namespace WindowFunctionMod;

 using namespace CorrelationBitMod;

 using namespace ReceiverSidebandMod;

 using namespace ProcessorSubTypeMod;

 using namespace AccumModeMod;

 using namespace AxisNameMod;

 using namespace FilterModeMod;

 using namespace CorrelatorNameMod;

 using namespace WVRMethodMod;

 using namespace ScanIntentMod;

 using namespace CalDataOriginMod;

 using namespace CalibrationFunctionMod;

 using namespace CalibrationSetMod;

 using namespace AntennaMotionPatternMod;

 using namespace SubscanIntentMod;

 using namespace SwitchingModeMod;

 using namespace CorrelatorCalibrationMod;

 using namespace TimeSamplingMod;

 using namespace CalTypeMod;

 using namespace AssociatedCalNatureMod;

 using namespace InvalidatingConditionMod;

 using namespace PositionMethodMod;

 using namespace PointingModelModeMod;

 using namespace PointingMethodMod;

 using namespace SyscalMethodMod;

 using namespace CalCurveTypeMod;

 using namespace StationTypeMod;

 using namespace DetectorBandTypeMod;

 using namespace FocusMethodMod;

 using namespace HolographyChannelTypeMod;

 using namespace FluxCalibrationMethodMod;

 using namespace PrimaryBeamDescriptionMod;

 using namespace TimeScaleMod;

 using namespace DataScaleMod;

 using namespace WeightTypeMod;

 using namespace DifferenceTypeMod;

 using namespace CalibrationModeMod;

 using namespace AssociatedFieldNatureMod;

 using namespace DataContentMod;

 using namespace PrimitiveDataTypeMod;

 using namespace SchedulerModeMod;

 using namespace FieldCodeMod;

 using namespace ACAPolarizationMod;

 using namespace PositionReferenceCodeMod;

 using namespace BaselineReferenceCodeMod;

 using namespace CorrelatorTypeMod;

 using namespace DopplerTrackingModeMod;

 using namespace SynthProfMod;


 namespace asdm {
 
 string EnumerationParser::getField(const string &xml, const string &field) {
		string::size_type b = xml.find("<" + field + ">");
		if (b == string::npos)
			return "";
		b += field.length() + 2;
		string::size_type e = xml.find("</" + field + ">",b);
		if (e == string::npos)
			return "";
		string s = substring(xml,b,e);
		return trim(s);
}

string EnumerationParser::substring(const string &s, int a, int b) {
		return s.substr(a,(b - a));
}

string EnumerationParser::trim(const string &s) {
		string::size_type i = 0;
		while (s.at(i) == ' ' && i < s.length())
			++i;
		if (i == s.length())
			return "";
		string::size_type j = s.length() - 1;
		while (s.at(j) == ' ' && j > i)
			--j;
		return substring(s,i,j + 1);
}
 
 	


		
string EnumerationParser::toXML(const string& elementName, ReceiverBandMod::ReceiverBand e) {
	return "<"+elementName+">"+CReceiverBand::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<ReceiverBandMod::ReceiverBand>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CReceiverBand::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<ReceiverBandMod::ReceiverBand> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CReceiverBand::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<ReceiverBandMod::ReceiverBand> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CReceiverBand::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

ReceiverBandMod::ReceiverBand EnumerationParser::getReceiverBand(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	ReceiverBand result;
	try {
		result = CReceiverBand::newReceiverBand(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a ReceiverBand.", tableName);
	}
	return result;
}

vector<ReceiverBandMod::ReceiverBand> EnumerationParser::getReceiverBand1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<ReceiverBandMod::ReceiverBand>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CReceiverBand::newReceiverBand(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ReceiverBand.", tableName);
	}

	return result;
}

vector<vector<ReceiverBandMod::ReceiverBand> > EnumerationParser::getReceiverBand2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<ReceiverBandMod::ReceiverBand> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<ReceiverBandMod::ReceiverBand> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CReceiverBand::newReceiverBand(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ReceiverBand.", tableName);
	}	
	return result;	
}


vector<vector<vector<ReceiverBandMod::ReceiverBand> > > EnumerationParser::getReceiverBand3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<ReceiverBandMod::ReceiverBand> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<ReceiverBandMod::ReceiverBand> v_aux;
		vector<vector<ReceiverBandMod::ReceiverBand> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CReceiverBand::newReceiverBand(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a ReceiverBand.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SBTypeMod::SBType e) {
	return "<"+elementName+">"+CSBType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SBTypeMod::SBType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSBType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SBTypeMod::SBType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSBType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SBTypeMod::SBType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSBType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SBTypeMod::SBType EnumerationParser::getSBType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SBType result;
	try {
		result = CSBType::newSBType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SBType.", tableName);
	}
	return result;
}

vector<SBTypeMod::SBType> EnumerationParser::getSBType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SBTypeMod::SBType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSBType::newSBType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SBType.", tableName);
	}

	return result;
}

vector<vector<SBTypeMod::SBType> > EnumerationParser::getSBType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SBTypeMod::SBType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SBTypeMod::SBType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSBType::newSBType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SBType.", tableName);
	}	
	return result;	
}


vector<vector<vector<SBTypeMod::SBType> > > EnumerationParser::getSBType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SBTypeMod::SBType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SBTypeMod::SBType> v_aux;
		vector<vector<SBTypeMod::SBType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSBType::newSBType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SBType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, DirectionReferenceCodeMod::DirectionReferenceCode e) {
	return "<"+elementName+">"+CDirectionReferenceCode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<DirectionReferenceCodeMod::DirectionReferenceCode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CDirectionReferenceCode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CDirectionReferenceCode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CDirectionReferenceCode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

DirectionReferenceCodeMod::DirectionReferenceCode EnumerationParser::getDirectionReferenceCode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	DirectionReferenceCode result;
	try {
		result = CDirectionReferenceCode::newDirectionReferenceCode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a DirectionReferenceCode.", tableName);
	}
	return result;
}

vector<DirectionReferenceCodeMod::DirectionReferenceCode> EnumerationParser::getDirectionReferenceCode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<DirectionReferenceCodeMod::DirectionReferenceCode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CDirectionReferenceCode::newDirectionReferenceCode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DirectionReferenceCode.", tableName);
	}

	return result;
}

vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> > EnumerationParser::getDirectionReferenceCode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<DirectionReferenceCodeMod::DirectionReferenceCode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CDirectionReferenceCode::newDirectionReferenceCode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DirectionReferenceCode.", tableName);
	}	
	return result;	
}


vector<vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> > > EnumerationParser::getDirectionReferenceCode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<DirectionReferenceCodeMod::DirectionReferenceCode> v_aux;
		vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CDirectionReferenceCode::newDirectionReferenceCode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a DirectionReferenceCode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CorrelationModeMod::CorrelationMode e) {
	return "<"+elementName+">"+CCorrelationMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CorrelationModeMod::CorrelationMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCorrelationMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CorrelationModeMod::CorrelationMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCorrelationMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CorrelationModeMod::CorrelationMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCorrelationMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CorrelationModeMod::CorrelationMode EnumerationParser::getCorrelationMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CorrelationMode result;
	try {
		result = CCorrelationMode::newCorrelationMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CorrelationMode.", tableName);
	}
	return result;
}

vector<CorrelationModeMod::CorrelationMode> EnumerationParser::getCorrelationMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CorrelationModeMod::CorrelationMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCorrelationMode::newCorrelationMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelationMode.", tableName);
	}

	return result;
}

vector<vector<CorrelationModeMod::CorrelationMode> > EnumerationParser::getCorrelationMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CorrelationModeMod::CorrelationMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CorrelationModeMod::CorrelationMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCorrelationMode::newCorrelationMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelationMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<CorrelationModeMod::CorrelationMode> > > EnumerationParser::getCorrelationMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CorrelationModeMod::CorrelationMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CorrelationModeMod::CorrelationMode> v_aux;
		vector<vector<CorrelationModeMod::CorrelationMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCorrelationMode::newCorrelationMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelationMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, AtmPhaseCorrectionMod::AtmPhaseCorrection e) {
	return "<"+elementName+">"+CAtmPhaseCorrection::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CAtmPhaseCorrection::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CAtmPhaseCorrection::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CAtmPhaseCorrection::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

AtmPhaseCorrectionMod::AtmPhaseCorrection EnumerationParser::getAtmPhaseCorrection(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	AtmPhaseCorrection result;
	try {
		result = CAtmPhaseCorrection::newAtmPhaseCorrection(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a AtmPhaseCorrection.", tableName);
	}
	return result;
}

vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> EnumerationParser::getAtmPhaseCorrection1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CAtmPhaseCorrection::newAtmPhaseCorrection(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AtmPhaseCorrection.", tableName);
	}

	return result;
}

vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > EnumerationParser::getAtmPhaseCorrection2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CAtmPhaseCorrection::newAtmPhaseCorrection(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AtmPhaseCorrection.", tableName);
	}	
	return result;	
}


vector<vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > > EnumerationParser::getAtmPhaseCorrection3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> v_aux;
		vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CAtmPhaseCorrection::newAtmPhaseCorrection(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a AtmPhaseCorrection.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, ProcessorTypeMod::ProcessorType e) {
	return "<"+elementName+">"+CProcessorType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<ProcessorTypeMod::ProcessorType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CProcessorType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<ProcessorTypeMod::ProcessorType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CProcessorType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<ProcessorTypeMod::ProcessorType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CProcessorType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

ProcessorTypeMod::ProcessorType EnumerationParser::getProcessorType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	ProcessorType result;
	try {
		result = CProcessorType::newProcessorType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a ProcessorType.", tableName);
	}
	return result;
}

vector<ProcessorTypeMod::ProcessorType> EnumerationParser::getProcessorType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<ProcessorTypeMod::ProcessorType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CProcessorType::newProcessorType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ProcessorType.", tableName);
	}

	return result;
}

vector<vector<ProcessorTypeMod::ProcessorType> > EnumerationParser::getProcessorType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<ProcessorTypeMod::ProcessorType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<ProcessorTypeMod::ProcessorType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CProcessorType::newProcessorType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ProcessorType.", tableName);
	}	
	return result;	
}


vector<vector<vector<ProcessorTypeMod::ProcessorType> > > EnumerationParser::getProcessorType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<ProcessorTypeMod::ProcessorType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<ProcessorTypeMod::ProcessorType> v_aux;
		vector<vector<ProcessorTypeMod::ProcessorType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CProcessorType::newProcessorType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a ProcessorType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SpectralResolutionTypeMod::SpectralResolutionType e) {
	return "<"+elementName+">"+CSpectralResolutionType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SpectralResolutionTypeMod::SpectralResolutionType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSpectralResolutionType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSpectralResolutionType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSpectralResolutionType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SpectralResolutionTypeMod::SpectralResolutionType EnumerationParser::getSpectralResolutionType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SpectralResolutionType result;
	try {
		result = CSpectralResolutionType::newSpectralResolutionType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SpectralResolutionType.", tableName);
	}
	return result;
}

vector<SpectralResolutionTypeMod::SpectralResolutionType> EnumerationParser::getSpectralResolutionType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SpectralResolutionTypeMod::SpectralResolutionType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSpectralResolutionType::newSpectralResolutionType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SpectralResolutionType.", tableName);
	}

	return result;
}

vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> > EnumerationParser::getSpectralResolutionType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SpectralResolutionTypeMod::SpectralResolutionType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSpectralResolutionType::newSpectralResolutionType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SpectralResolutionType.", tableName);
	}	
	return result;	
}


vector<vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> > > EnumerationParser::getSpectralResolutionType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SpectralResolutionTypeMod::SpectralResolutionType> v_aux;
		vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSpectralResolutionType::newSpectralResolutionType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SpectralResolutionType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CalibrationDeviceMod::CalibrationDevice e) {
	return "<"+elementName+">"+CCalibrationDevice::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CalibrationDeviceMod::CalibrationDevice>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCalibrationDevice::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CalibrationDeviceMod::CalibrationDevice> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCalibrationDevice::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CalibrationDeviceMod::CalibrationDevice> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCalibrationDevice::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CalibrationDeviceMod::CalibrationDevice EnumerationParser::getCalibrationDevice(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CalibrationDevice result;
	try {
		result = CCalibrationDevice::newCalibrationDevice(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CalibrationDevice.", tableName);
	}
	return result;
}

vector<CalibrationDeviceMod::CalibrationDevice> EnumerationParser::getCalibrationDevice1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CalibrationDeviceMod::CalibrationDevice>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCalibrationDevice::newCalibrationDevice(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationDevice.", tableName);
	}

	return result;
}

vector<vector<CalibrationDeviceMod::CalibrationDevice> > EnumerationParser::getCalibrationDevice2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CalibrationDeviceMod::CalibrationDevice> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CalibrationDeviceMod::CalibrationDevice> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCalibrationDevice::newCalibrationDevice(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationDevice.", tableName);
	}	
	return result;	
}


vector<vector<vector<CalibrationDeviceMod::CalibrationDevice> > > EnumerationParser::getCalibrationDevice3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CalibrationDeviceMod::CalibrationDevice> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CalibrationDeviceMod::CalibrationDevice> v_aux;
		vector<vector<CalibrationDeviceMod::CalibrationDevice> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCalibrationDevice::newCalibrationDevice(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationDevice.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, AntennaMakeMod::AntennaMake e) {
	return "<"+elementName+">"+CAntennaMake::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<AntennaMakeMod::AntennaMake>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CAntennaMake::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<AntennaMakeMod::AntennaMake> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CAntennaMake::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<AntennaMakeMod::AntennaMake> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CAntennaMake::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

AntennaMakeMod::AntennaMake EnumerationParser::getAntennaMake(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	AntennaMake result;
	try {
		result = CAntennaMake::newAntennaMake(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a AntennaMake.", tableName);
	}
	return result;
}

vector<AntennaMakeMod::AntennaMake> EnumerationParser::getAntennaMake1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<AntennaMakeMod::AntennaMake>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CAntennaMake::newAntennaMake(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaMake.", tableName);
	}

	return result;
}

vector<vector<AntennaMakeMod::AntennaMake> > EnumerationParser::getAntennaMake2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<AntennaMakeMod::AntennaMake> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<AntennaMakeMod::AntennaMake> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CAntennaMake::newAntennaMake(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaMake.", tableName);
	}	
	return result;	
}


vector<vector<vector<AntennaMakeMod::AntennaMake> > > EnumerationParser::getAntennaMake3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<AntennaMakeMod::AntennaMake> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<AntennaMakeMod::AntennaMake> v_aux;
		vector<vector<AntennaMakeMod::AntennaMake> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CAntennaMake::newAntennaMake(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaMake.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, AntennaTypeMod::AntennaType e) {
	return "<"+elementName+">"+CAntennaType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<AntennaTypeMod::AntennaType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CAntennaType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<AntennaTypeMod::AntennaType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CAntennaType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<AntennaTypeMod::AntennaType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CAntennaType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

AntennaTypeMod::AntennaType EnumerationParser::getAntennaType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	AntennaType result;
	try {
		result = CAntennaType::newAntennaType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a AntennaType.", tableName);
	}
	return result;
}

vector<AntennaTypeMod::AntennaType> EnumerationParser::getAntennaType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<AntennaTypeMod::AntennaType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CAntennaType::newAntennaType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaType.", tableName);
	}

	return result;
}

vector<vector<AntennaTypeMod::AntennaType> > EnumerationParser::getAntennaType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<AntennaTypeMod::AntennaType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<AntennaTypeMod::AntennaType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CAntennaType::newAntennaType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaType.", tableName);
	}	
	return result;	
}


vector<vector<vector<AntennaTypeMod::AntennaType> > > EnumerationParser::getAntennaType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<AntennaTypeMod::AntennaType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<AntennaTypeMod::AntennaType> v_aux;
		vector<vector<AntennaTypeMod::AntennaType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CAntennaType::newAntennaType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SourceModelMod::SourceModel e) {
	return "<"+elementName+">"+CSourceModel::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SourceModelMod::SourceModel>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSourceModel::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SourceModelMod::SourceModel> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSourceModel::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SourceModelMod::SourceModel> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSourceModel::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SourceModelMod::SourceModel EnumerationParser::getSourceModel(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SourceModel result;
	try {
		result = CSourceModel::newSourceModel(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SourceModel.", tableName);
	}
	return result;
}

vector<SourceModelMod::SourceModel> EnumerationParser::getSourceModel1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SourceModelMod::SourceModel>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSourceModel::newSourceModel(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SourceModel.", tableName);
	}

	return result;
}

vector<vector<SourceModelMod::SourceModel> > EnumerationParser::getSourceModel2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SourceModelMod::SourceModel> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SourceModelMod::SourceModel> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSourceModel::newSourceModel(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SourceModel.", tableName);
	}	
	return result;	
}


vector<vector<vector<SourceModelMod::SourceModel> > > EnumerationParser::getSourceModel3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SourceModelMod::SourceModel> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SourceModelMod::SourceModel> v_aux;
		vector<vector<SourceModelMod::SourceModel> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSourceModel::newSourceModel(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SourceModel.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, FrequencyReferenceCodeMod::FrequencyReferenceCode e) {
	return "<"+elementName+">"+CFrequencyReferenceCode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<FrequencyReferenceCodeMod::FrequencyReferenceCode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CFrequencyReferenceCode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CFrequencyReferenceCode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CFrequencyReferenceCode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

FrequencyReferenceCodeMod::FrequencyReferenceCode EnumerationParser::getFrequencyReferenceCode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	FrequencyReferenceCode result;
	try {
		result = CFrequencyReferenceCode::newFrequencyReferenceCode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a FrequencyReferenceCode.", tableName);
	}
	return result;
}

vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> EnumerationParser::getFrequencyReferenceCode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<FrequencyReferenceCodeMod::FrequencyReferenceCode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CFrequencyReferenceCode::newFrequencyReferenceCode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FrequencyReferenceCode.", tableName);
	}

	return result;
}

vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > EnumerationParser::getFrequencyReferenceCode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CFrequencyReferenceCode::newFrequencyReferenceCode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FrequencyReferenceCode.", tableName);
	}	
	return result;	
}


vector<vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > > EnumerationParser::getFrequencyReferenceCode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> v_aux;
		vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CFrequencyReferenceCode::newFrequencyReferenceCode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a FrequencyReferenceCode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, StokesParameterMod::StokesParameter e) {
	return "<"+elementName+">"+CStokesParameter::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<StokesParameterMod::StokesParameter>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CStokesParameter::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<StokesParameterMod::StokesParameter> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CStokesParameter::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<StokesParameterMod::StokesParameter> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CStokesParameter::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

StokesParameterMod::StokesParameter EnumerationParser::getStokesParameter(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	StokesParameter result;
	try {
		result = CStokesParameter::newStokesParameter(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a StokesParameter.", tableName);
	}
	return result;
}

vector<StokesParameterMod::StokesParameter> EnumerationParser::getStokesParameter1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<StokesParameterMod::StokesParameter>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CStokesParameter::newStokesParameter(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a StokesParameter.", tableName);
	}

	return result;
}

vector<vector<StokesParameterMod::StokesParameter> > EnumerationParser::getStokesParameter2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<StokesParameterMod::StokesParameter> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<StokesParameterMod::StokesParameter> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CStokesParameter::newStokesParameter(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a StokesParameter.", tableName);
	}	
	return result;	
}


vector<vector<vector<StokesParameterMod::StokesParameter> > > EnumerationParser::getStokesParameter3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<StokesParameterMod::StokesParameter> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<StokesParameterMod::StokesParameter> v_aux;
		vector<vector<StokesParameterMod::StokesParameter> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CStokesParameter::newStokesParameter(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a StokesParameter.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode e) {
	return "<"+elementName+">"+CRadialVelocityReferenceCode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CRadialVelocityReferenceCode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CRadialVelocityReferenceCode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CRadialVelocityReferenceCode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode EnumerationParser::getRadialVelocityReferenceCode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	RadialVelocityReferenceCode result;
	try {
		result = CRadialVelocityReferenceCode::newRadialVelocityReferenceCode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a RadialVelocityReferenceCode.", tableName);
	}
	return result;
}

vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> EnumerationParser::getRadialVelocityReferenceCode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CRadialVelocityReferenceCode::newRadialVelocityReferenceCode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a RadialVelocityReferenceCode.", tableName);
	}

	return result;
}

vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > EnumerationParser::getRadialVelocityReferenceCode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CRadialVelocityReferenceCode::newRadialVelocityReferenceCode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a RadialVelocityReferenceCode.", tableName);
	}	
	return result;	
}


vector<vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > > EnumerationParser::getRadialVelocityReferenceCode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> v_aux;
		vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CRadialVelocityReferenceCode::newRadialVelocityReferenceCode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a RadialVelocityReferenceCode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, DopplerReferenceCodeMod::DopplerReferenceCode e) {
	return "<"+elementName+">"+CDopplerReferenceCode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<DopplerReferenceCodeMod::DopplerReferenceCode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CDopplerReferenceCode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CDopplerReferenceCode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CDopplerReferenceCode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

DopplerReferenceCodeMod::DopplerReferenceCode EnumerationParser::getDopplerReferenceCode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	DopplerReferenceCode result;
	try {
		result = CDopplerReferenceCode::newDopplerReferenceCode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a DopplerReferenceCode.", tableName);
	}
	return result;
}

vector<DopplerReferenceCodeMod::DopplerReferenceCode> EnumerationParser::getDopplerReferenceCode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<DopplerReferenceCodeMod::DopplerReferenceCode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CDopplerReferenceCode::newDopplerReferenceCode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DopplerReferenceCode.", tableName);
	}

	return result;
}

vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> > EnumerationParser::getDopplerReferenceCode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<DopplerReferenceCodeMod::DopplerReferenceCode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CDopplerReferenceCode::newDopplerReferenceCode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DopplerReferenceCode.", tableName);
	}	
	return result;	
}


vector<vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> > > EnumerationParser::getDopplerReferenceCode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<DopplerReferenceCodeMod::DopplerReferenceCode> v_aux;
		vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CDopplerReferenceCode::newDopplerReferenceCode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a DopplerReferenceCode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, PolarizationTypeMod::PolarizationType e) {
	return "<"+elementName+">"+CPolarizationType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<PolarizationTypeMod::PolarizationType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CPolarizationType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<PolarizationTypeMod::PolarizationType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CPolarizationType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<PolarizationTypeMod::PolarizationType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CPolarizationType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

PolarizationTypeMod::PolarizationType EnumerationParser::getPolarizationType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	PolarizationType result;
	try {
		result = CPolarizationType::newPolarizationType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a PolarizationType.", tableName);
	}
	return result;
}

vector<PolarizationTypeMod::PolarizationType> EnumerationParser::getPolarizationType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<PolarizationTypeMod::PolarizationType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CPolarizationType::newPolarizationType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PolarizationType.", tableName);
	}

	return result;
}

vector<vector<PolarizationTypeMod::PolarizationType> > EnumerationParser::getPolarizationType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<PolarizationTypeMod::PolarizationType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<PolarizationTypeMod::PolarizationType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CPolarizationType::newPolarizationType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PolarizationType.", tableName);
	}	
	return result;	
}


vector<vector<vector<PolarizationTypeMod::PolarizationType> > > EnumerationParser::getPolarizationType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<PolarizationTypeMod::PolarizationType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<PolarizationTypeMod::PolarizationType> v_aux;
		vector<vector<PolarizationTypeMod::PolarizationType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CPolarizationType::newPolarizationType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a PolarizationType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, BasebandNameMod::BasebandName e) {
	return "<"+elementName+">"+CBasebandName::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<BasebandNameMod::BasebandName>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CBasebandName::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<BasebandNameMod::BasebandName> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CBasebandName::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<BasebandNameMod::BasebandName> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CBasebandName::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

BasebandNameMod::BasebandName EnumerationParser::getBasebandName(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	BasebandName result;
	try {
		result = CBasebandName::newBasebandName(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a BasebandName.", tableName);
	}
	return result;
}

vector<BasebandNameMod::BasebandName> EnumerationParser::getBasebandName1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<BasebandNameMod::BasebandName>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CBasebandName::newBasebandName(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a BasebandName.", tableName);
	}

	return result;
}

vector<vector<BasebandNameMod::BasebandName> > EnumerationParser::getBasebandName2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<BasebandNameMod::BasebandName> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<BasebandNameMod::BasebandName> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CBasebandName::newBasebandName(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a BasebandName.", tableName);
	}	
	return result;	
}


vector<vector<vector<BasebandNameMod::BasebandName> > > EnumerationParser::getBasebandName3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<BasebandNameMod::BasebandName> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<BasebandNameMod::BasebandName> v_aux;
		vector<vector<BasebandNameMod::BasebandName> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CBasebandName::newBasebandName(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a BasebandName.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, NetSidebandMod::NetSideband e) {
	return "<"+elementName+">"+CNetSideband::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<NetSidebandMod::NetSideband>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CNetSideband::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<NetSidebandMod::NetSideband> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CNetSideband::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<NetSidebandMod::NetSideband> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CNetSideband::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

NetSidebandMod::NetSideband EnumerationParser::getNetSideband(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	NetSideband result;
	try {
		result = CNetSideband::newNetSideband(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a NetSideband.", tableName);
	}
	return result;
}

vector<NetSidebandMod::NetSideband> EnumerationParser::getNetSideband1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<NetSidebandMod::NetSideband>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CNetSideband::newNetSideband(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a NetSideband.", tableName);
	}

	return result;
}

vector<vector<NetSidebandMod::NetSideband> > EnumerationParser::getNetSideband2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<NetSidebandMod::NetSideband> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<NetSidebandMod::NetSideband> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CNetSideband::newNetSideband(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a NetSideband.", tableName);
	}	
	return result;	
}


vector<vector<vector<NetSidebandMod::NetSideband> > > EnumerationParser::getNetSideband3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<NetSidebandMod::NetSideband> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<NetSidebandMod::NetSideband> v_aux;
		vector<vector<NetSidebandMod::NetSideband> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CNetSideband::newNetSideband(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a NetSideband.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SidebandProcessingModeMod::SidebandProcessingMode e) {
	return "<"+elementName+">"+CSidebandProcessingMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SidebandProcessingModeMod::SidebandProcessingMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSidebandProcessingMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSidebandProcessingMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSidebandProcessingMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SidebandProcessingModeMod::SidebandProcessingMode EnumerationParser::getSidebandProcessingMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SidebandProcessingMode result;
	try {
		result = CSidebandProcessingMode::newSidebandProcessingMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SidebandProcessingMode.", tableName);
	}
	return result;
}

vector<SidebandProcessingModeMod::SidebandProcessingMode> EnumerationParser::getSidebandProcessingMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SidebandProcessingModeMod::SidebandProcessingMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSidebandProcessingMode::newSidebandProcessingMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SidebandProcessingMode.", tableName);
	}

	return result;
}

vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> > EnumerationParser::getSidebandProcessingMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SidebandProcessingModeMod::SidebandProcessingMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSidebandProcessingMode::newSidebandProcessingMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SidebandProcessingMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> > > EnumerationParser::getSidebandProcessingMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SidebandProcessingModeMod::SidebandProcessingMode> v_aux;
		vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSidebandProcessingMode::newSidebandProcessingMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SidebandProcessingMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, WindowFunctionMod::WindowFunction e) {
	return "<"+elementName+">"+CWindowFunction::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<WindowFunctionMod::WindowFunction>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CWindowFunction::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<WindowFunctionMod::WindowFunction> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CWindowFunction::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<WindowFunctionMod::WindowFunction> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CWindowFunction::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

WindowFunctionMod::WindowFunction EnumerationParser::getWindowFunction(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	WindowFunction result;
	try {
		result = CWindowFunction::newWindowFunction(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a WindowFunction.", tableName);
	}
	return result;
}

vector<WindowFunctionMod::WindowFunction> EnumerationParser::getWindowFunction1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<WindowFunctionMod::WindowFunction>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CWindowFunction::newWindowFunction(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a WindowFunction.", tableName);
	}

	return result;
}

vector<vector<WindowFunctionMod::WindowFunction> > EnumerationParser::getWindowFunction2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<WindowFunctionMod::WindowFunction> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<WindowFunctionMod::WindowFunction> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CWindowFunction::newWindowFunction(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a WindowFunction.", tableName);
	}	
	return result;	
}


vector<vector<vector<WindowFunctionMod::WindowFunction> > > EnumerationParser::getWindowFunction3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<WindowFunctionMod::WindowFunction> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<WindowFunctionMod::WindowFunction> v_aux;
		vector<vector<WindowFunctionMod::WindowFunction> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CWindowFunction::newWindowFunction(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a WindowFunction.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CorrelationBitMod::CorrelationBit e) {
	return "<"+elementName+">"+CCorrelationBit::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CorrelationBitMod::CorrelationBit>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCorrelationBit::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CorrelationBitMod::CorrelationBit> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCorrelationBit::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CorrelationBitMod::CorrelationBit> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCorrelationBit::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CorrelationBitMod::CorrelationBit EnumerationParser::getCorrelationBit(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CorrelationBit result;
	try {
		result = CCorrelationBit::newCorrelationBit(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CorrelationBit.", tableName);
	}
	return result;
}

vector<CorrelationBitMod::CorrelationBit> EnumerationParser::getCorrelationBit1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CorrelationBitMod::CorrelationBit>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCorrelationBit::newCorrelationBit(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelationBit.", tableName);
	}

	return result;
}

vector<vector<CorrelationBitMod::CorrelationBit> > EnumerationParser::getCorrelationBit2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CorrelationBitMod::CorrelationBit> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CorrelationBitMod::CorrelationBit> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCorrelationBit::newCorrelationBit(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelationBit.", tableName);
	}	
	return result;	
}


vector<vector<vector<CorrelationBitMod::CorrelationBit> > > EnumerationParser::getCorrelationBit3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CorrelationBitMod::CorrelationBit> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CorrelationBitMod::CorrelationBit> v_aux;
		vector<vector<CorrelationBitMod::CorrelationBit> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCorrelationBit::newCorrelationBit(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelationBit.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, ReceiverSidebandMod::ReceiverSideband e) {
	return "<"+elementName+">"+CReceiverSideband::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<ReceiverSidebandMod::ReceiverSideband>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CReceiverSideband::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<ReceiverSidebandMod::ReceiverSideband> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CReceiverSideband::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<ReceiverSidebandMod::ReceiverSideband> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CReceiverSideband::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

ReceiverSidebandMod::ReceiverSideband EnumerationParser::getReceiverSideband(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	ReceiverSideband result;
	try {
		result = CReceiverSideband::newReceiverSideband(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a ReceiverSideband.", tableName);
	}
	return result;
}

vector<ReceiverSidebandMod::ReceiverSideband> EnumerationParser::getReceiverSideband1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<ReceiverSidebandMod::ReceiverSideband>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CReceiverSideband::newReceiverSideband(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ReceiverSideband.", tableName);
	}

	return result;
}

vector<vector<ReceiverSidebandMod::ReceiverSideband> > EnumerationParser::getReceiverSideband2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<ReceiverSidebandMod::ReceiverSideband> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<ReceiverSidebandMod::ReceiverSideband> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CReceiverSideband::newReceiverSideband(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ReceiverSideband.", tableName);
	}	
	return result;	
}


vector<vector<vector<ReceiverSidebandMod::ReceiverSideband> > > EnumerationParser::getReceiverSideband3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<ReceiverSidebandMod::ReceiverSideband> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<ReceiverSidebandMod::ReceiverSideband> v_aux;
		vector<vector<ReceiverSidebandMod::ReceiverSideband> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CReceiverSideband::newReceiverSideband(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a ReceiverSideband.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, ProcessorSubTypeMod::ProcessorSubType e) {
	return "<"+elementName+">"+CProcessorSubType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<ProcessorSubTypeMod::ProcessorSubType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CProcessorSubType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<ProcessorSubTypeMod::ProcessorSubType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CProcessorSubType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<ProcessorSubTypeMod::ProcessorSubType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CProcessorSubType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

ProcessorSubTypeMod::ProcessorSubType EnumerationParser::getProcessorSubType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	ProcessorSubType result;
	try {
		result = CProcessorSubType::newProcessorSubType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a ProcessorSubType.", tableName);
	}
	return result;
}

vector<ProcessorSubTypeMod::ProcessorSubType> EnumerationParser::getProcessorSubType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<ProcessorSubTypeMod::ProcessorSubType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CProcessorSubType::newProcessorSubType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ProcessorSubType.", tableName);
	}

	return result;
}

vector<vector<ProcessorSubTypeMod::ProcessorSubType> > EnumerationParser::getProcessorSubType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<ProcessorSubTypeMod::ProcessorSubType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<ProcessorSubTypeMod::ProcessorSubType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CProcessorSubType::newProcessorSubType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ProcessorSubType.", tableName);
	}	
	return result;	
}


vector<vector<vector<ProcessorSubTypeMod::ProcessorSubType> > > EnumerationParser::getProcessorSubType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<ProcessorSubTypeMod::ProcessorSubType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<ProcessorSubTypeMod::ProcessorSubType> v_aux;
		vector<vector<ProcessorSubTypeMod::ProcessorSubType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CProcessorSubType::newProcessorSubType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a ProcessorSubType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, AccumModeMod::AccumMode e) {
	return "<"+elementName+">"+CAccumMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<AccumModeMod::AccumMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CAccumMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<AccumModeMod::AccumMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CAccumMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<AccumModeMod::AccumMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CAccumMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

AccumModeMod::AccumMode EnumerationParser::getAccumMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	AccumMode result;
	try {
		result = CAccumMode::newAccumMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a AccumMode.", tableName);
	}
	return result;
}

vector<AccumModeMod::AccumMode> EnumerationParser::getAccumMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<AccumModeMod::AccumMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CAccumMode::newAccumMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AccumMode.", tableName);
	}

	return result;
}

vector<vector<AccumModeMod::AccumMode> > EnumerationParser::getAccumMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<AccumModeMod::AccumMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<AccumModeMod::AccumMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CAccumMode::newAccumMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AccumMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<AccumModeMod::AccumMode> > > EnumerationParser::getAccumMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<AccumModeMod::AccumMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<AccumModeMod::AccumMode> v_aux;
		vector<vector<AccumModeMod::AccumMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CAccumMode::newAccumMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a AccumMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, AxisNameMod::AxisName e) {
	return "<"+elementName+">"+CAxisName::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<AxisNameMod::AxisName>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CAxisName::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<AxisNameMod::AxisName> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CAxisName::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<AxisNameMod::AxisName> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CAxisName::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

AxisNameMod::AxisName EnumerationParser::getAxisName(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	AxisName result;
	try {
		result = CAxisName::newAxisName(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a AxisName.", tableName);
	}
	return result;
}

vector<AxisNameMod::AxisName> EnumerationParser::getAxisName1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<AxisNameMod::AxisName>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CAxisName::newAxisName(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AxisName.", tableName);
	}

	return result;
}

vector<vector<AxisNameMod::AxisName> > EnumerationParser::getAxisName2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<AxisNameMod::AxisName> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<AxisNameMod::AxisName> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CAxisName::newAxisName(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AxisName.", tableName);
	}	
	return result;	
}


vector<vector<vector<AxisNameMod::AxisName> > > EnumerationParser::getAxisName3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<AxisNameMod::AxisName> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<AxisNameMod::AxisName> v_aux;
		vector<vector<AxisNameMod::AxisName> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CAxisName::newAxisName(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a AxisName.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, FilterModeMod::FilterMode e) {
	return "<"+elementName+">"+CFilterMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<FilterModeMod::FilterMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CFilterMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<FilterModeMod::FilterMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CFilterMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<FilterModeMod::FilterMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CFilterMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

FilterModeMod::FilterMode EnumerationParser::getFilterMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	FilterMode result;
	try {
		result = CFilterMode::newFilterMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a FilterMode.", tableName);
	}
	return result;
}

vector<FilterModeMod::FilterMode> EnumerationParser::getFilterMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<FilterModeMod::FilterMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CFilterMode::newFilterMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FilterMode.", tableName);
	}

	return result;
}

vector<vector<FilterModeMod::FilterMode> > EnumerationParser::getFilterMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<FilterModeMod::FilterMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<FilterModeMod::FilterMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CFilterMode::newFilterMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FilterMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<FilterModeMod::FilterMode> > > EnumerationParser::getFilterMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<FilterModeMod::FilterMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<FilterModeMod::FilterMode> v_aux;
		vector<vector<FilterModeMod::FilterMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CFilterMode::newFilterMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a FilterMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CorrelatorNameMod::CorrelatorName e) {
	return "<"+elementName+">"+CCorrelatorName::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CorrelatorNameMod::CorrelatorName>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCorrelatorName::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CorrelatorNameMod::CorrelatorName> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCorrelatorName::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CorrelatorNameMod::CorrelatorName> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCorrelatorName::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CorrelatorNameMod::CorrelatorName EnumerationParser::getCorrelatorName(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CorrelatorName result;
	try {
		result = CCorrelatorName::newCorrelatorName(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CorrelatorName.", tableName);
	}
	return result;
}

vector<CorrelatorNameMod::CorrelatorName> EnumerationParser::getCorrelatorName1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CorrelatorNameMod::CorrelatorName>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCorrelatorName::newCorrelatorName(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorName.", tableName);
	}

	return result;
}

vector<vector<CorrelatorNameMod::CorrelatorName> > EnumerationParser::getCorrelatorName2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CorrelatorNameMod::CorrelatorName> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CorrelatorNameMod::CorrelatorName> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCorrelatorName::newCorrelatorName(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorName.", tableName);
	}	
	return result;	
}


vector<vector<vector<CorrelatorNameMod::CorrelatorName> > > EnumerationParser::getCorrelatorName3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CorrelatorNameMod::CorrelatorName> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CorrelatorNameMod::CorrelatorName> v_aux;
		vector<vector<CorrelatorNameMod::CorrelatorName> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCorrelatorName::newCorrelatorName(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorName.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, WVRMethodMod::WVRMethod e) {
	return "<"+elementName+">"+CWVRMethod::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<WVRMethodMod::WVRMethod>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CWVRMethod::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<WVRMethodMod::WVRMethod> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CWVRMethod::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<WVRMethodMod::WVRMethod> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CWVRMethod::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

WVRMethodMod::WVRMethod EnumerationParser::getWVRMethod(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	WVRMethod result;
	try {
		result = CWVRMethod::newWVRMethod(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a WVRMethod.", tableName);
	}
	return result;
}

vector<WVRMethodMod::WVRMethod> EnumerationParser::getWVRMethod1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<WVRMethodMod::WVRMethod>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CWVRMethod::newWVRMethod(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a WVRMethod.", tableName);
	}

	return result;
}

vector<vector<WVRMethodMod::WVRMethod> > EnumerationParser::getWVRMethod2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<WVRMethodMod::WVRMethod> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<WVRMethodMod::WVRMethod> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CWVRMethod::newWVRMethod(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a WVRMethod.", tableName);
	}	
	return result;	
}


vector<vector<vector<WVRMethodMod::WVRMethod> > > EnumerationParser::getWVRMethod3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<WVRMethodMod::WVRMethod> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<WVRMethodMod::WVRMethod> v_aux;
		vector<vector<WVRMethodMod::WVRMethod> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CWVRMethod::newWVRMethod(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a WVRMethod.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, ScanIntentMod::ScanIntent e) {
	return "<"+elementName+">"+CScanIntent::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<ScanIntentMod::ScanIntent>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CScanIntent::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<ScanIntentMod::ScanIntent> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CScanIntent::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<ScanIntentMod::ScanIntent> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CScanIntent::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

ScanIntentMod::ScanIntent EnumerationParser::getScanIntent(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	ScanIntent result;
	try {
		result = CScanIntent::newScanIntent(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a ScanIntent.", tableName);
	}
	return result;
}

vector<ScanIntentMod::ScanIntent> EnumerationParser::getScanIntent1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<ScanIntentMod::ScanIntent>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CScanIntent::newScanIntent(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ScanIntent.", tableName);
	}

	return result;
}

vector<vector<ScanIntentMod::ScanIntent> > EnumerationParser::getScanIntent2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<ScanIntentMod::ScanIntent> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<ScanIntentMod::ScanIntent> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CScanIntent::newScanIntent(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ScanIntent.", tableName);
	}	
	return result;	
}


vector<vector<vector<ScanIntentMod::ScanIntent> > > EnumerationParser::getScanIntent3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<ScanIntentMod::ScanIntent> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<ScanIntentMod::ScanIntent> v_aux;
		vector<vector<ScanIntentMod::ScanIntent> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CScanIntent::newScanIntent(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a ScanIntent.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CalDataOriginMod::CalDataOrigin e) {
	return "<"+elementName+">"+CCalDataOrigin::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CalDataOriginMod::CalDataOrigin>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCalDataOrigin::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CalDataOriginMod::CalDataOrigin> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCalDataOrigin::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CalDataOriginMod::CalDataOrigin> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCalDataOrigin::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CalDataOriginMod::CalDataOrigin EnumerationParser::getCalDataOrigin(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CalDataOrigin result;
	try {
		result = CCalDataOrigin::newCalDataOrigin(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CalDataOrigin.", tableName);
	}
	return result;
}

vector<CalDataOriginMod::CalDataOrigin> EnumerationParser::getCalDataOrigin1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CalDataOriginMod::CalDataOrigin>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCalDataOrigin::newCalDataOrigin(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalDataOrigin.", tableName);
	}

	return result;
}

vector<vector<CalDataOriginMod::CalDataOrigin> > EnumerationParser::getCalDataOrigin2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CalDataOriginMod::CalDataOrigin> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CalDataOriginMod::CalDataOrigin> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCalDataOrigin::newCalDataOrigin(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalDataOrigin.", tableName);
	}	
	return result;	
}


vector<vector<vector<CalDataOriginMod::CalDataOrigin> > > EnumerationParser::getCalDataOrigin3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CalDataOriginMod::CalDataOrigin> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CalDataOriginMod::CalDataOrigin> v_aux;
		vector<vector<CalDataOriginMod::CalDataOrigin> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCalDataOrigin::newCalDataOrigin(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalDataOrigin.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CalibrationFunctionMod::CalibrationFunction e) {
	return "<"+elementName+">"+CCalibrationFunction::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CalibrationFunctionMod::CalibrationFunction>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCalibrationFunction::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CalibrationFunctionMod::CalibrationFunction> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCalibrationFunction::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CalibrationFunctionMod::CalibrationFunction> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCalibrationFunction::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CalibrationFunctionMod::CalibrationFunction EnumerationParser::getCalibrationFunction(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CalibrationFunction result;
	try {
		result = CCalibrationFunction::newCalibrationFunction(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CalibrationFunction.", tableName);
	}
	return result;
}

vector<CalibrationFunctionMod::CalibrationFunction> EnumerationParser::getCalibrationFunction1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CalibrationFunctionMod::CalibrationFunction>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCalibrationFunction::newCalibrationFunction(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationFunction.", tableName);
	}

	return result;
}

vector<vector<CalibrationFunctionMod::CalibrationFunction> > EnumerationParser::getCalibrationFunction2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CalibrationFunctionMod::CalibrationFunction> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CalibrationFunctionMod::CalibrationFunction> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCalibrationFunction::newCalibrationFunction(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationFunction.", tableName);
	}	
	return result;	
}


vector<vector<vector<CalibrationFunctionMod::CalibrationFunction> > > EnumerationParser::getCalibrationFunction3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CalibrationFunctionMod::CalibrationFunction> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CalibrationFunctionMod::CalibrationFunction> v_aux;
		vector<vector<CalibrationFunctionMod::CalibrationFunction> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCalibrationFunction::newCalibrationFunction(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationFunction.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CalibrationSetMod::CalibrationSet e) {
	return "<"+elementName+">"+CCalibrationSet::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CalibrationSetMod::CalibrationSet>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCalibrationSet::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CalibrationSetMod::CalibrationSet> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCalibrationSet::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CalibrationSetMod::CalibrationSet> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCalibrationSet::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CalibrationSetMod::CalibrationSet EnumerationParser::getCalibrationSet(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CalibrationSet result;
	try {
		result = CCalibrationSet::newCalibrationSet(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CalibrationSet.", tableName);
	}
	return result;
}

vector<CalibrationSetMod::CalibrationSet> EnumerationParser::getCalibrationSet1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CalibrationSetMod::CalibrationSet>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCalibrationSet::newCalibrationSet(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationSet.", tableName);
	}

	return result;
}

vector<vector<CalibrationSetMod::CalibrationSet> > EnumerationParser::getCalibrationSet2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CalibrationSetMod::CalibrationSet> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CalibrationSetMod::CalibrationSet> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCalibrationSet::newCalibrationSet(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationSet.", tableName);
	}	
	return result;	
}


vector<vector<vector<CalibrationSetMod::CalibrationSet> > > EnumerationParser::getCalibrationSet3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CalibrationSetMod::CalibrationSet> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CalibrationSetMod::CalibrationSet> v_aux;
		vector<vector<CalibrationSetMod::CalibrationSet> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCalibrationSet::newCalibrationSet(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationSet.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, AntennaMotionPatternMod::AntennaMotionPattern e) {
	return "<"+elementName+">"+CAntennaMotionPattern::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<AntennaMotionPatternMod::AntennaMotionPattern>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CAntennaMotionPattern::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CAntennaMotionPattern::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CAntennaMotionPattern::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

AntennaMotionPatternMod::AntennaMotionPattern EnumerationParser::getAntennaMotionPattern(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	AntennaMotionPattern result;
	try {
		result = CAntennaMotionPattern::newAntennaMotionPattern(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a AntennaMotionPattern.", tableName);
	}
	return result;
}

vector<AntennaMotionPatternMod::AntennaMotionPattern> EnumerationParser::getAntennaMotionPattern1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<AntennaMotionPatternMod::AntennaMotionPattern>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CAntennaMotionPattern::newAntennaMotionPattern(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaMotionPattern.", tableName);
	}

	return result;
}

vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> > EnumerationParser::getAntennaMotionPattern2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<AntennaMotionPatternMod::AntennaMotionPattern> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CAntennaMotionPattern::newAntennaMotionPattern(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaMotionPattern.", tableName);
	}	
	return result;	
}


vector<vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> > > EnumerationParser::getAntennaMotionPattern3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<AntennaMotionPatternMod::AntennaMotionPattern> v_aux;
		vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CAntennaMotionPattern::newAntennaMotionPattern(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a AntennaMotionPattern.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SubscanIntentMod::SubscanIntent e) {
	return "<"+elementName+">"+CSubscanIntent::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SubscanIntentMod::SubscanIntent>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSubscanIntent::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SubscanIntentMod::SubscanIntent> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSubscanIntent::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SubscanIntentMod::SubscanIntent> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSubscanIntent::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SubscanIntentMod::SubscanIntent EnumerationParser::getSubscanIntent(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SubscanIntent result;
	try {
		result = CSubscanIntent::newSubscanIntent(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SubscanIntent.", tableName);
	}
	return result;
}

vector<SubscanIntentMod::SubscanIntent> EnumerationParser::getSubscanIntent1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SubscanIntentMod::SubscanIntent>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSubscanIntent::newSubscanIntent(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SubscanIntent.", tableName);
	}

	return result;
}

vector<vector<SubscanIntentMod::SubscanIntent> > EnumerationParser::getSubscanIntent2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SubscanIntentMod::SubscanIntent> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SubscanIntentMod::SubscanIntent> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSubscanIntent::newSubscanIntent(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SubscanIntent.", tableName);
	}	
	return result;	
}


vector<vector<vector<SubscanIntentMod::SubscanIntent> > > EnumerationParser::getSubscanIntent3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SubscanIntentMod::SubscanIntent> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SubscanIntentMod::SubscanIntent> v_aux;
		vector<vector<SubscanIntentMod::SubscanIntent> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSubscanIntent::newSubscanIntent(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SubscanIntent.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SwitchingModeMod::SwitchingMode e) {
	return "<"+elementName+">"+CSwitchingMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SwitchingModeMod::SwitchingMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSwitchingMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SwitchingModeMod::SwitchingMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSwitchingMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SwitchingModeMod::SwitchingMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSwitchingMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SwitchingModeMod::SwitchingMode EnumerationParser::getSwitchingMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SwitchingMode result;
	try {
		result = CSwitchingMode::newSwitchingMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SwitchingMode.", tableName);
	}
	return result;
}

vector<SwitchingModeMod::SwitchingMode> EnumerationParser::getSwitchingMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SwitchingModeMod::SwitchingMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSwitchingMode::newSwitchingMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SwitchingMode.", tableName);
	}

	return result;
}

vector<vector<SwitchingModeMod::SwitchingMode> > EnumerationParser::getSwitchingMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SwitchingModeMod::SwitchingMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SwitchingModeMod::SwitchingMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSwitchingMode::newSwitchingMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SwitchingMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<SwitchingModeMod::SwitchingMode> > > EnumerationParser::getSwitchingMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SwitchingModeMod::SwitchingMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SwitchingModeMod::SwitchingMode> v_aux;
		vector<vector<SwitchingModeMod::SwitchingMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSwitchingMode::newSwitchingMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SwitchingMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CorrelatorCalibrationMod::CorrelatorCalibration e) {
	return "<"+elementName+">"+CCorrelatorCalibration::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CorrelatorCalibrationMod::CorrelatorCalibration>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCorrelatorCalibration::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCorrelatorCalibration::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCorrelatorCalibration::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CorrelatorCalibrationMod::CorrelatorCalibration EnumerationParser::getCorrelatorCalibration(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CorrelatorCalibration result;
	try {
		result = CCorrelatorCalibration::newCorrelatorCalibration(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CorrelatorCalibration.", tableName);
	}
	return result;
}

vector<CorrelatorCalibrationMod::CorrelatorCalibration> EnumerationParser::getCorrelatorCalibration1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CorrelatorCalibrationMod::CorrelatorCalibration>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCorrelatorCalibration::newCorrelatorCalibration(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorCalibration.", tableName);
	}

	return result;
}

vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> > EnumerationParser::getCorrelatorCalibration2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CorrelatorCalibrationMod::CorrelatorCalibration> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCorrelatorCalibration::newCorrelatorCalibration(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorCalibration.", tableName);
	}	
	return result;	
}


vector<vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> > > EnumerationParser::getCorrelatorCalibration3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CorrelatorCalibrationMod::CorrelatorCalibration> v_aux;
		vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCorrelatorCalibration::newCorrelatorCalibration(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorCalibration.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, TimeSamplingMod::TimeSampling e) {
	return "<"+elementName+">"+CTimeSampling::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<TimeSamplingMod::TimeSampling>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CTimeSampling::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<TimeSamplingMod::TimeSampling> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CTimeSampling::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<TimeSamplingMod::TimeSampling> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CTimeSampling::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

TimeSamplingMod::TimeSampling EnumerationParser::getTimeSampling(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	TimeSampling result;
	try {
		result = CTimeSampling::newTimeSampling(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a TimeSampling.", tableName);
	}
	return result;
}

vector<TimeSamplingMod::TimeSampling> EnumerationParser::getTimeSampling1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<TimeSamplingMod::TimeSampling>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CTimeSampling::newTimeSampling(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a TimeSampling.", tableName);
	}

	return result;
}

vector<vector<TimeSamplingMod::TimeSampling> > EnumerationParser::getTimeSampling2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<TimeSamplingMod::TimeSampling> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<TimeSamplingMod::TimeSampling> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CTimeSampling::newTimeSampling(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a TimeSampling.", tableName);
	}	
	return result;	
}


vector<vector<vector<TimeSamplingMod::TimeSampling> > > EnumerationParser::getTimeSampling3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<TimeSamplingMod::TimeSampling> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<TimeSamplingMod::TimeSampling> v_aux;
		vector<vector<TimeSamplingMod::TimeSampling> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CTimeSampling::newTimeSampling(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a TimeSampling.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CalTypeMod::CalType e) {
	return "<"+elementName+">"+CCalType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CalTypeMod::CalType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCalType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CalTypeMod::CalType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCalType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CalTypeMod::CalType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCalType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CalTypeMod::CalType EnumerationParser::getCalType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CalType result;
	try {
		result = CCalType::newCalType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CalType.", tableName);
	}
	return result;
}

vector<CalTypeMod::CalType> EnumerationParser::getCalType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CalTypeMod::CalType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCalType::newCalType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalType.", tableName);
	}

	return result;
}

vector<vector<CalTypeMod::CalType> > EnumerationParser::getCalType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CalTypeMod::CalType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CalTypeMod::CalType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCalType::newCalType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalType.", tableName);
	}	
	return result;	
}


vector<vector<vector<CalTypeMod::CalType> > > EnumerationParser::getCalType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CalTypeMod::CalType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CalTypeMod::CalType> v_aux;
		vector<vector<CalTypeMod::CalType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCalType::newCalType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, AssociatedCalNatureMod::AssociatedCalNature e) {
	return "<"+elementName+">"+CAssociatedCalNature::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<AssociatedCalNatureMod::AssociatedCalNature>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CAssociatedCalNature::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<AssociatedCalNatureMod::AssociatedCalNature> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CAssociatedCalNature::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<AssociatedCalNatureMod::AssociatedCalNature> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CAssociatedCalNature::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

AssociatedCalNatureMod::AssociatedCalNature EnumerationParser::getAssociatedCalNature(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	AssociatedCalNature result;
	try {
		result = CAssociatedCalNature::newAssociatedCalNature(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a AssociatedCalNature.", tableName);
	}
	return result;
}

vector<AssociatedCalNatureMod::AssociatedCalNature> EnumerationParser::getAssociatedCalNature1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<AssociatedCalNatureMod::AssociatedCalNature>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CAssociatedCalNature::newAssociatedCalNature(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AssociatedCalNature.", tableName);
	}

	return result;
}

vector<vector<AssociatedCalNatureMod::AssociatedCalNature> > EnumerationParser::getAssociatedCalNature2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<AssociatedCalNatureMod::AssociatedCalNature> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<AssociatedCalNatureMod::AssociatedCalNature> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CAssociatedCalNature::newAssociatedCalNature(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AssociatedCalNature.", tableName);
	}	
	return result;	
}


vector<vector<vector<AssociatedCalNatureMod::AssociatedCalNature> > > EnumerationParser::getAssociatedCalNature3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<AssociatedCalNatureMod::AssociatedCalNature> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<AssociatedCalNatureMod::AssociatedCalNature> v_aux;
		vector<vector<AssociatedCalNatureMod::AssociatedCalNature> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CAssociatedCalNature::newAssociatedCalNature(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a AssociatedCalNature.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, InvalidatingConditionMod::InvalidatingCondition e) {
	return "<"+elementName+">"+CInvalidatingCondition::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<InvalidatingConditionMod::InvalidatingCondition>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CInvalidatingCondition::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<InvalidatingConditionMod::InvalidatingCondition> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CInvalidatingCondition::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<InvalidatingConditionMod::InvalidatingCondition> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CInvalidatingCondition::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

InvalidatingConditionMod::InvalidatingCondition EnumerationParser::getInvalidatingCondition(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	InvalidatingCondition result;
	try {
		result = CInvalidatingCondition::newInvalidatingCondition(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a InvalidatingCondition.", tableName);
	}
	return result;
}

vector<InvalidatingConditionMod::InvalidatingCondition> EnumerationParser::getInvalidatingCondition1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<InvalidatingConditionMod::InvalidatingCondition>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CInvalidatingCondition::newInvalidatingCondition(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a InvalidatingCondition.", tableName);
	}

	return result;
}

vector<vector<InvalidatingConditionMod::InvalidatingCondition> > EnumerationParser::getInvalidatingCondition2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<InvalidatingConditionMod::InvalidatingCondition> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<InvalidatingConditionMod::InvalidatingCondition> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CInvalidatingCondition::newInvalidatingCondition(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a InvalidatingCondition.", tableName);
	}	
	return result;	
}


vector<vector<vector<InvalidatingConditionMod::InvalidatingCondition> > > EnumerationParser::getInvalidatingCondition3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<InvalidatingConditionMod::InvalidatingCondition> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<InvalidatingConditionMod::InvalidatingCondition> v_aux;
		vector<vector<InvalidatingConditionMod::InvalidatingCondition> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CInvalidatingCondition::newInvalidatingCondition(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a InvalidatingCondition.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, PositionMethodMod::PositionMethod e) {
	return "<"+elementName+">"+CPositionMethod::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<PositionMethodMod::PositionMethod>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CPositionMethod::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<PositionMethodMod::PositionMethod> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CPositionMethod::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<PositionMethodMod::PositionMethod> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CPositionMethod::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

PositionMethodMod::PositionMethod EnumerationParser::getPositionMethod(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	PositionMethod result;
	try {
		result = CPositionMethod::newPositionMethod(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a PositionMethod.", tableName);
	}
	return result;
}

vector<PositionMethodMod::PositionMethod> EnumerationParser::getPositionMethod1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<PositionMethodMod::PositionMethod>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CPositionMethod::newPositionMethod(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PositionMethod.", tableName);
	}

	return result;
}

vector<vector<PositionMethodMod::PositionMethod> > EnumerationParser::getPositionMethod2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<PositionMethodMod::PositionMethod> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<PositionMethodMod::PositionMethod> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CPositionMethod::newPositionMethod(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PositionMethod.", tableName);
	}	
	return result;	
}


vector<vector<vector<PositionMethodMod::PositionMethod> > > EnumerationParser::getPositionMethod3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<PositionMethodMod::PositionMethod> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<PositionMethodMod::PositionMethod> v_aux;
		vector<vector<PositionMethodMod::PositionMethod> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CPositionMethod::newPositionMethod(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a PositionMethod.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, PointingModelModeMod::PointingModelMode e) {
	return "<"+elementName+">"+CPointingModelMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<PointingModelModeMod::PointingModelMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CPointingModelMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<PointingModelModeMod::PointingModelMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CPointingModelMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<PointingModelModeMod::PointingModelMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CPointingModelMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

PointingModelModeMod::PointingModelMode EnumerationParser::getPointingModelMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	PointingModelMode result;
	try {
		result = CPointingModelMode::newPointingModelMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a PointingModelMode.", tableName);
	}
	return result;
}

vector<PointingModelModeMod::PointingModelMode> EnumerationParser::getPointingModelMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<PointingModelModeMod::PointingModelMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CPointingModelMode::newPointingModelMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PointingModelMode.", tableName);
	}

	return result;
}

vector<vector<PointingModelModeMod::PointingModelMode> > EnumerationParser::getPointingModelMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<PointingModelModeMod::PointingModelMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<PointingModelModeMod::PointingModelMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CPointingModelMode::newPointingModelMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PointingModelMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<PointingModelModeMod::PointingModelMode> > > EnumerationParser::getPointingModelMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<PointingModelModeMod::PointingModelMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<PointingModelModeMod::PointingModelMode> v_aux;
		vector<vector<PointingModelModeMod::PointingModelMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CPointingModelMode::newPointingModelMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a PointingModelMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, PointingMethodMod::PointingMethod e) {
	return "<"+elementName+">"+CPointingMethod::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<PointingMethodMod::PointingMethod>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CPointingMethod::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<PointingMethodMod::PointingMethod> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CPointingMethod::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<PointingMethodMod::PointingMethod> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CPointingMethod::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

PointingMethodMod::PointingMethod EnumerationParser::getPointingMethod(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	PointingMethod result;
	try {
		result = CPointingMethod::newPointingMethod(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a PointingMethod.", tableName);
	}
	return result;
}

vector<PointingMethodMod::PointingMethod> EnumerationParser::getPointingMethod1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<PointingMethodMod::PointingMethod>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CPointingMethod::newPointingMethod(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PointingMethod.", tableName);
	}

	return result;
}

vector<vector<PointingMethodMod::PointingMethod> > EnumerationParser::getPointingMethod2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<PointingMethodMod::PointingMethod> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<PointingMethodMod::PointingMethod> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CPointingMethod::newPointingMethod(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PointingMethod.", tableName);
	}	
	return result;	
}


vector<vector<vector<PointingMethodMod::PointingMethod> > > EnumerationParser::getPointingMethod3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<PointingMethodMod::PointingMethod> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<PointingMethodMod::PointingMethod> v_aux;
		vector<vector<PointingMethodMod::PointingMethod> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CPointingMethod::newPointingMethod(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a PointingMethod.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SyscalMethodMod::SyscalMethod e) {
	return "<"+elementName+">"+CSyscalMethod::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SyscalMethodMod::SyscalMethod>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSyscalMethod::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SyscalMethodMod::SyscalMethod> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSyscalMethod::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SyscalMethodMod::SyscalMethod> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSyscalMethod::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SyscalMethodMod::SyscalMethod EnumerationParser::getSyscalMethod(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SyscalMethod result;
	try {
		result = CSyscalMethod::newSyscalMethod(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SyscalMethod.", tableName);
	}
	return result;
}

vector<SyscalMethodMod::SyscalMethod> EnumerationParser::getSyscalMethod1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SyscalMethodMod::SyscalMethod>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSyscalMethod::newSyscalMethod(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SyscalMethod.", tableName);
	}

	return result;
}

vector<vector<SyscalMethodMod::SyscalMethod> > EnumerationParser::getSyscalMethod2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SyscalMethodMod::SyscalMethod> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SyscalMethodMod::SyscalMethod> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSyscalMethod::newSyscalMethod(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SyscalMethod.", tableName);
	}	
	return result;	
}


vector<vector<vector<SyscalMethodMod::SyscalMethod> > > EnumerationParser::getSyscalMethod3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SyscalMethodMod::SyscalMethod> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SyscalMethodMod::SyscalMethod> v_aux;
		vector<vector<SyscalMethodMod::SyscalMethod> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSyscalMethod::newSyscalMethod(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SyscalMethod.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CalCurveTypeMod::CalCurveType e) {
	return "<"+elementName+">"+CCalCurveType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CalCurveTypeMod::CalCurveType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCalCurveType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CalCurveTypeMod::CalCurveType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCalCurveType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CalCurveTypeMod::CalCurveType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCalCurveType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CalCurveTypeMod::CalCurveType EnumerationParser::getCalCurveType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CalCurveType result;
	try {
		result = CCalCurveType::newCalCurveType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CalCurveType.", tableName);
	}
	return result;
}

vector<CalCurveTypeMod::CalCurveType> EnumerationParser::getCalCurveType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CalCurveTypeMod::CalCurveType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCalCurveType::newCalCurveType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalCurveType.", tableName);
	}

	return result;
}

vector<vector<CalCurveTypeMod::CalCurveType> > EnumerationParser::getCalCurveType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CalCurveTypeMod::CalCurveType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CalCurveTypeMod::CalCurveType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCalCurveType::newCalCurveType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalCurveType.", tableName);
	}	
	return result;	
}


vector<vector<vector<CalCurveTypeMod::CalCurveType> > > EnumerationParser::getCalCurveType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CalCurveTypeMod::CalCurveType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CalCurveTypeMod::CalCurveType> v_aux;
		vector<vector<CalCurveTypeMod::CalCurveType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCalCurveType::newCalCurveType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalCurveType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, StationTypeMod::StationType e) {
	return "<"+elementName+">"+CStationType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<StationTypeMod::StationType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CStationType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<StationTypeMod::StationType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CStationType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<StationTypeMod::StationType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CStationType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

StationTypeMod::StationType EnumerationParser::getStationType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	StationType result;
	try {
		result = CStationType::newStationType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a StationType.", tableName);
	}
	return result;
}

vector<StationTypeMod::StationType> EnumerationParser::getStationType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<StationTypeMod::StationType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CStationType::newStationType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a StationType.", tableName);
	}

	return result;
}

vector<vector<StationTypeMod::StationType> > EnumerationParser::getStationType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<StationTypeMod::StationType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<StationTypeMod::StationType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CStationType::newStationType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a StationType.", tableName);
	}	
	return result;	
}


vector<vector<vector<StationTypeMod::StationType> > > EnumerationParser::getStationType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<StationTypeMod::StationType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<StationTypeMod::StationType> v_aux;
		vector<vector<StationTypeMod::StationType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CStationType::newStationType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a StationType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, DetectorBandTypeMod::DetectorBandType e) {
	return "<"+elementName+">"+CDetectorBandType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<DetectorBandTypeMod::DetectorBandType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CDetectorBandType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<DetectorBandTypeMod::DetectorBandType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CDetectorBandType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<DetectorBandTypeMod::DetectorBandType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CDetectorBandType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

DetectorBandTypeMod::DetectorBandType EnumerationParser::getDetectorBandType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	DetectorBandType result;
	try {
		result = CDetectorBandType::newDetectorBandType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a DetectorBandType.", tableName);
	}
	return result;
}

vector<DetectorBandTypeMod::DetectorBandType> EnumerationParser::getDetectorBandType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<DetectorBandTypeMod::DetectorBandType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CDetectorBandType::newDetectorBandType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DetectorBandType.", tableName);
	}

	return result;
}

vector<vector<DetectorBandTypeMod::DetectorBandType> > EnumerationParser::getDetectorBandType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<DetectorBandTypeMod::DetectorBandType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<DetectorBandTypeMod::DetectorBandType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CDetectorBandType::newDetectorBandType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DetectorBandType.", tableName);
	}	
	return result;	
}


vector<vector<vector<DetectorBandTypeMod::DetectorBandType> > > EnumerationParser::getDetectorBandType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<DetectorBandTypeMod::DetectorBandType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<DetectorBandTypeMod::DetectorBandType> v_aux;
		vector<vector<DetectorBandTypeMod::DetectorBandType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CDetectorBandType::newDetectorBandType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a DetectorBandType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, FocusMethodMod::FocusMethod e) {
	return "<"+elementName+">"+CFocusMethod::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<FocusMethodMod::FocusMethod>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CFocusMethod::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<FocusMethodMod::FocusMethod> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CFocusMethod::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<FocusMethodMod::FocusMethod> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CFocusMethod::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

FocusMethodMod::FocusMethod EnumerationParser::getFocusMethod(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	FocusMethod result;
	try {
		result = CFocusMethod::newFocusMethod(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a FocusMethod.", tableName);
	}
	return result;
}

vector<FocusMethodMod::FocusMethod> EnumerationParser::getFocusMethod1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<FocusMethodMod::FocusMethod>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CFocusMethod::newFocusMethod(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FocusMethod.", tableName);
	}

	return result;
}

vector<vector<FocusMethodMod::FocusMethod> > EnumerationParser::getFocusMethod2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<FocusMethodMod::FocusMethod> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<FocusMethodMod::FocusMethod> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CFocusMethod::newFocusMethod(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FocusMethod.", tableName);
	}	
	return result;	
}


vector<vector<vector<FocusMethodMod::FocusMethod> > > EnumerationParser::getFocusMethod3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<FocusMethodMod::FocusMethod> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<FocusMethodMod::FocusMethod> v_aux;
		vector<vector<FocusMethodMod::FocusMethod> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CFocusMethod::newFocusMethod(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a FocusMethod.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, HolographyChannelTypeMod::HolographyChannelType e) {
	return "<"+elementName+">"+CHolographyChannelType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<HolographyChannelTypeMod::HolographyChannelType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CHolographyChannelType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<HolographyChannelTypeMod::HolographyChannelType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CHolographyChannelType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<HolographyChannelTypeMod::HolographyChannelType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CHolographyChannelType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

HolographyChannelTypeMod::HolographyChannelType EnumerationParser::getHolographyChannelType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	HolographyChannelType result;
	try {
		result = CHolographyChannelType::newHolographyChannelType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a HolographyChannelType.", tableName);
	}
	return result;
}

vector<HolographyChannelTypeMod::HolographyChannelType> EnumerationParser::getHolographyChannelType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<HolographyChannelTypeMod::HolographyChannelType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CHolographyChannelType::newHolographyChannelType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a HolographyChannelType.", tableName);
	}

	return result;
}

vector<vector<HolographyChannelTypeMod::HolographyChannelType> > EnumerationParser::getHolographyChannelType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<HolographyChannelTypeMod::HolographyChannelType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<HolographyChannelTypeMod::HolographyChannelType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CHolographyChannelType::newHolographyChannelType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a HolographyChannelType.", tableName);
	}	
	return result;	
}


vector<vector<vector<HolographyChannelTypeMod::HolographyChannelType> > > EnumerationParser::getHolographyChannelType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<HolographyChannelTypeMod::HolographyChannelType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<HolographyChannelTypeMod::HolographyChannelType> v_aux;
		vector<vector<HolographyChannelTypeMod::HolographyChannelType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CHolographyChannelType::newHolographyChannelType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a HolographyChannelType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, FluxCalibrationMethodMod::FluxCalibrationMethod e) {
	return "<"+elementName+">"+CFluxCalibrationMethod::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<FluxCalibrationMethodMod::FluxCalibrationMethod>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CFluxCalibrationMethod::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CFluxCalibrationMethod::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CFluxCalibrationMethod::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

FluxCalibrationMethodMod::FluxCalibrationMethod EnumerationParser::getFluxCalibrationMethod(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	FluxCalibrationMethod result;
	try {
		result = CFluxCalibrationMethod::newFluxCalibrationMethod(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a FluxCalibrationMethod.", tableName);
	}
	return result;
}

vector<FluxCalibrationMethodMod::FluxCalibrationMethod> EnumerationParser::getFluxCalibrationMethod1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<FluxCalibrationMethodMod::FluxCalibrationMethod>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CFluxCalibrationMethod::newFluxCalibrationMethod(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FluxCalibrationMethod.", tableName);
	}

	return result;
}

vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > EnumerationParser::getFluxCalibrationMethod2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<FluxCalibrationMethodMod::FluxCalibrationMethod> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CFluxCalibrationMethod::newFluxCalibrationMethod(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FluxCalibrationMethod.", tableName);
	}	
	return result;	
}


vector<vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > > EnumerationParser::getFluxCalibrationMethod3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<FluxCalibrationMethodMod::FluxCalibrationMethod> v_aux;
		vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CFluxCalibrationMethod::newFluxCalibrationMethod(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a FluxCalibrationMethod.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, PrimaryBeamDescriptionMod::PrimaryBeamDescription e) {
	return "<"+elementName+">"+CPrimaryBeamDescription::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CPrimaryBeamDescription::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CPrimaryBeamDescription::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CPrimaryBeamDescription::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

PrimaryBeamDescriptionMod::PrimaryBeamDescription EnumerationParser::getPrimaryBeamDescription(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	PrimaryBeamDescription result;
	try {
		result = CPrimaryBeamDescription::newPrimaryBeamDescription(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a PrimaryBeamDescription.", tableName);
	}
	return result;
}

vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> EnumerationParser::getPrimaryBeamDescription1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CPrimaryBeamDescription::newPrimaryBeamDescription(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PrimaryBeamDescription.", tableName);
	}

	return result;
}

vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > EnumerationParser::getPrimaryBeamDescription2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CPrimaryBeamDescription::newPrimaryBeamDescription(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PrimaryBeamDescription.", tableName);
	}	
	return result;	
}


vector<vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > > EnumerationParser::getPrimaryBeamDescription3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> v_aux;
		vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CPrimaryBeamDescription::newPrimaryBeamDescription(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a PrimaryBeamDescription.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, TimeScaleMod::TimeScale e) {
	return "<"+elementName+">"+CTimeScale::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<TimeScaleMod::TimeScale>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CTimeScale::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<TimeScaleMod::TimeScale> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CTimeScale::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<TimeScaleMod::TimeScale> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CTimeScale::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

TimeScaleMod::TimeScale EnumerationParser::getTimeScale(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	TimeScale result;
	try {
		result = CTimeScale::newTimeScale(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a TimeScale.", tableName);
	}
	return result;
}

vector<TimeScaleMod::TimeScale> EnumerationParser::getTimeScale1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<TimeScaleMod::TimeScale>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CTimeScale::newTimeScale(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a TimeScale.", tableName);
	}

	return result;
}

vector<vector<TimeScaleMod::TimeScale> > EnumerationParser::getTimeScale2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<TimeScaleMod::TimeScale> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<TimeScaleMod::TimeScale> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CTimeScale::newTimeScale(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a TimeScale.", tableName);
	}	
	return result;	
}


vector<vector<vector<TimeScaleMod::TimeScale> > > EnumerationParser::getTimeScale3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<TimeScaleMod::TimeScale> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<TimeScaleMod::TimeScale> v_aux;
		vector<vector<TimeScaleMod::TimeScale> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CTimeScale::newTimeScale(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a TimeScale.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, DataScaleMod::DataScale e) {
	return "<"+elementName+">"+CDataScale::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<DataScaleMod::DataScale>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CDataScale::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<DataScaleMod::DataScale> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CDataScale::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<DataScaleMod::DataScale> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CDataScale::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

DataScaleMod::DataScale EnumerationParser::getDataScale(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	DataScale result;
	try {
		result = CDataScale::newDataScale(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a DataScale.", tableName);
	}
	return result;
}

vector<DataScaleMod::DataScale> EnumerationParser::getDataScale1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<DataScaleMod::DataScale>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CDataScale::newDataScale(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DataScale.", tableName);
	}

	return result;
}

vector<vector<DataScaleMod::DataScale> > EnumerationParser::getDataScale2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<DataScaleMod::DataScale> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<DataScaleMod::DataScale> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CDataScale::newDataScale(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DataScale.", tableName);
	}	
	return result;	
}


vector<vector<vector<DataScaleMod::DataScale> > > EnumerationParser::getDataScale3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<DataScaleMod::DataScale> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<DataScaleMod::DataScale> v_aux;
		vector<vector<DataScaleMod::DataScale> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CDataScale::newDataScale(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a DataScale.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, WeightTypeMod::WeightType e) {
	return "<"+elementName+">"+CWeightType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<WeightTypeMod::WeightType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CWeightType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<WeightTypeMod::WeightType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CWeightType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<WeightTypeMod::WeightType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CWeightType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

WeightTypeMod::WeightType EnumerationParser::getWeightType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	WeightType result;
	try {
		result = CWeightType::newWeightType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a WeightType.", tableName);
	}
	return result;
}

vector<WeightTypeMod::WeightType> EnumerationParser::getWeightType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<WeightTypeMod::WeightType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CWeightType::newWeightType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a WeightType.", tableName);
	}

	return result;
}

vector<vector<WeightTypeMod::WeightType> > EnumerationParser::getWeightType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<WeightTypeMod::WeightType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<WeightTypeMod::WeightType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CWeightType::newWeightType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a WeightType.", tableName);
	}	
	return result;	
}


vector<vector<vector<WeightTypeMod::WeightType> > > EnumerationParser::getWeightType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<WeightTypeMod::WeightType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<WeightTypeMod::WeightType> v_aux;
		vector<vector<WeightTypeMod::WeightType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CWeightType::newWeightType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a WeightType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, DifferenceTypeMod::DifferenceType e) {
	return "<"+elementName+">"+CDifferenceType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<DifferenceTypeMod::DifferenceType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CDifferenceType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<DifferenceTypeMod::DifferenceType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CDifferenceType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<DifferenceTypeMod::DifferenceType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CDifferenceType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

DifferenceTypeMod::DifferenceType EnumerationParser::getDifferenceType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	DifferenceType result;
	try {
		result = CDifferenceType::newDifferenceType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a DifferenceType.", tableName);
	}
	return result;
}

vector<DifferenceTypeMod::DifferenceType> EnumerationParser::getDifferenceType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<DifferenceTypeMod::DifferenceType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CDifferenceType::newDifferenceType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DifferenceType.", tableName);
	}

	return result;
}

vector<vector<DifferenceTypeMod::DifferenceType> > EnumerationParser::getDifferenceType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<DifferenceTypeMod::DifferenceType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<DifferenceTypeMod::DifferenceType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CDifferenceType::newDifferenceType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DifferenceType.", tableName);
	}	
	return result;	
}


vector<vector<vector<DifferenceTypeMod::DifferenceType> > > EnumerationParser::getDifferenceType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<DifferenceTypeMod::DifferenceType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<DifferenceTypeMod::DifferenceType> v_aux;
		vector<vector<DifferenceTypeMod::DifferenceType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CDifferenceType::newDifferenceType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a DifferenceType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CalibrationModeMod::CalibrationMode e) {
	return "<"+elementName+">"+CCalibrationMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CalibrationModeMod::CalibrationMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCalibrationMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CalibrationModeMod::CalibrationMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCalibrationMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CalibrationModeMod::CalibrationMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCalibrationMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CalibrationModeMod::CalibrationMode EnumerationParser::getCalibrationMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CalibrationMode result;
	try {
		result = CCalibrationMode::newCalibrationMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CalibrationMode.", tableName);
	}
	return result;
}

vector<CalibrationModeMod::CalibrationMode> EnumerationParser::getCalibrationMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CalibrationModeMod::CalibrationMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCalibrationMode::newCalibrationMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationMode.", tableName);
	}

	return result;
}

vector<vector<CalibrationModeMod::CalibrationMode> > EnumerationParser::getCalibrationMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CalibrationModeMod::CalibrationMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CalibrationModeMod::CalibrationMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCalibrationMode::newCalibrationMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<CalibrationModeMod::CalibrationMode> > > EnumerationParser::getCalibrationMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CalibrationModeMod::CalibrationMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CalibrationModeMod::CalibrationMode> v_aux;
		vector<vector<CalibrationModeMod::CalibrationMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCalibrationMode::newCalibrationMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CalibrationMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, AssociatedFieldNatureMod::AssociatedFieldNature e) {
	return "<"+elementName+">"+CAssociatedFieldNature::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<AssociatedFieldNatureMod::AssociatedFieldNature>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CAssociatedFieldNature::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CAssociatedFieldNature::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CAssociatedFieldNature::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

AssociatedFieldNatureMod::AssociatedFieldNature EnumerationParser::getAssociatedFieldNature(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	AssociatedFieldNature result;
	try {
		result = CAssociatedFieldNature::newAssociatedFieldNature(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a AssociatedFieldNature.", tableName);
	}
	return result;
}

vector<AssociatedFieldNatureMod::AssociatedFieldNature> EnumerationParser::getAssociatedFieldNature1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<AssociatedFieldNatureMod::AssociatedFieldNature>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CAssociatedFieldNature::newAssociatedFieldNature(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AssociatedFieldNature.", tableName);
	}

	return result;
}

vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> > EnumerationParser::getAssociatedFieldNature2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<AssociatedFieldNatureMod::AssociatedFieldNature> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CAssociatedFieldNature::newAssociatedFieldNature(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a AssociatedFieldNature.", tableName);
	}	
	return result;	
}


vector<vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> > > EnumerationParser::getAssociatedFieldNature3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<AssociatedFieldNatureMod::AssociatedFieldNature> v_aux;
		vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CAssociatedFieldNature::newAssociatedFieldNature(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a AssociatedFieldNature.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, DataContentMod::DataContent e) {
	return "<"+elementName+">"+CDataContent::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<DataContentMod::DataContent>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CDataContent::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<DataContentMod::DataContent> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CDataContent::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<DataContentMod::DataContent> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CDataContent::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

DataContentMod::DataContent EnumerationParser::getDataContent(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	DataContent result;
	try {
		result = CDataContent::newDataContent(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a DataContent.", tableName);
	}
	return result;
}

vector<DataContentMod::DataContent> EnumerationParser::getDataContent1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<DataContentMod::DataContent>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CDataContent::newDataContent(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DataContent.", tableName);
	}

	return result;
}

vector<vector<DataContentMod::DataContent> > EnumerationParser::getDataContent2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<DataContentMod::DataContent> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<DataContentMod::DataContent> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CDataContent::newDataContent(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DataContent.", tableName);
	}	
	return result;	
}


vector<vector<vector<DataContentMod::DataContent> > > EnumerationParser::getDataContent3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<DataContentMod::DataContent> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<DataContentMod::DataContent> v_aux;
		vector<vector<DataContentMod::DataContent> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CDataContent::newDataContent(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a DataContent.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, PrimitiveDataTypeMod::PrimitiveDataType e) {
	return "<"+elementName+">"+CPrimitiveDataType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<PrimitiveDataTypeMod::PrimitiveDataType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CPrimitiveDataType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CPrimitiveDataType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CPrimitiveDataType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

PrimitiveDataTypeMod::PrimitiveDataType EnumerationParser::getPrimitiveDataType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	PrimitiveDataType result;
	try {
		result = CPrimitiveDataType::newPrimitiveDataType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a PrimitiveDataType.", tableName);
	}
	return result;
}

vector<PrimitiveDataTypeMod::PrimitiveDataType> EnumerationParser::getPrimitiveDataType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<PrimitiveDataTypeMod::PrimitiveDataType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CPrimitiveDataType::newPrimitiveDataType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PrimitiveDataType.", tableName);
	}

	return result;
}

vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> > EnumerationParser::getPrimitiveDataType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<PrimitiveDataTypeMod::PrimitiveDataType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CPrimitiveDataType::newPrimitiveDataType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PrimitiveDataType.", tableName);
	}	
	return result;	
}


vector<vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> > > EnumerationParser::getPrimitiveDataType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<PrimitiveDataTypeMod::PrimitiveDataType> v_aux;
		vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CPrimitiveDataType::newPrimitiveDataType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a PrimitiveDataType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SchedulerModeMod::SchedulerMode e) {
	return "<"+elementName+">"+CSchedulerMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SchedulerModeMod::SchedulerMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSchedulerMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SchedulerModeMod::SchedulerMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSchedulerMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SchedulerModeMod::SchedulerMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSchedulerMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SchedulerModeMod::SchedulerMode EnumerationParser::getSchedulerMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SchedulerMode result;
	try {
		result = CSchedulerMode::newSchedulerMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SchedulerMode.", tableName);
	}
	return result;
}

vector<SchedulerModeMod::SchedulerMode> EnumerationParser::getSchedulerMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SchedulerModeMod::SchedulerMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSchedulerMode::newSchedulerMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SchedulerMode.", tableName);
	}

	return result;
}

vector<vector<SchedulerModeMod::SchedulerMode> > EnumerationParser::getSchedulerMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SchedulerModeMod::SchedulerMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SchedulerModeMod::SchedulerMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSchedulerMode::newSchedulerMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SchedulerMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<SchedulerModeMod::SchedulerMode> > > EnumerationParser::getSchedulerMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SchedulerModeMod::SchedulerMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SchedulerModeMod::SchedulerMode> v_aux;
		vector<vector<SchedulerModeMod::SchedulerMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSchedulerMode::newSchedulerMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SchedulerMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, FieldCodeMod::FieldCode e) {
	return "<"+elementName+">"+CFieldCode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<FieldCodeMod::FieldCode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CFieldCode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<FieldCodeMod::FieldCode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CFieldCode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<FieldCodeMod::FieldCode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CFieldCode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

FieldCodeMod::FieldCode EnumerationParser::getFieldCode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	FieldCode result;
	try {
		result = CFieldCode::newFieldCode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a FieldCode.", tableName);
	}
	return result;
}

vector<FieldCodeMod::FieldCode> EnumerationParser::getFieldCode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<FieldCodeMod::FieldCode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CFieldCode::newFieldCode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FieldCode.", tableName);
	}

	return result;
}

vector<vector<FieldCodeMod::FieldCode> > EnumerationParser::getFieldCode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<FieldCodeMod::FieldCode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<FieldCodeMod::FieldCode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CFieldCode::newFieldCode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a FieldCode.", tableName);
	}	
	return result;	
}


vector<vector<vector<FieldCodeMod::FieldCode> > > EnumerationParser::getFieldCode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<FieldCodeMod::FieldCode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<FieldCodeMod::FieldCode> v_aux;
		vector<vector<FieldCodeMod::FieldCode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CFieldCode::newFieldCode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a FieldCode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, ACAPolarizationMod::ACAPolarization e) {
	return "<"+elementName+">"+CACAPolarization::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<ACAPolarizationMod::ACAPolarization>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CACAPolarization::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<ACAPolarizationMod::ACAPolarization> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CACAPolarization::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<ACAPolarizationMod::ACAPolarization> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CACAPolarization::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

ACAPolarizationMod::ACAPolarization EnumerationParser::getACAPolarization(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	ACAPolarization result;
	try {
		result = CACAPolarization::newACAPolarization(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a ACAPolarization.", tableName);
	}
	return result;
}

vector<ACAPolarizationMod::ACAPolarization> EnumerationParser::getACAPolarization1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<ACAPolarizationMod::ACAPolarization>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CACAPolarization::newACAPolarization(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ACAPolarization.", tableName);
	}

	return result;
}

vector<vector<ACAPolarizationMod::ACAPolarization> > EnumerationParser::getACAPolarization2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<ACAPolarizationMod::ACAPolarization> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<ACAPolarizationMod::ACAPolarization> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CACAPolarization::newACAPolarization(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a ACAPolarization.", tableName);
	}	
	return result;	
}


vector<vector<vector<ACAPolarizationMod::ACAPolarization> > > EnumerationParser::getACAPolarization3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<ACAPolarizationMod::ACAPolarization> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<ACAPolarizationMod::ACAPolarization> v_aux;
		vector<vector<ACAPolarizationMod::ACAPolarization> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CACAPolarization::newACAPolarization(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a ACAPolarization.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, PositionReferenceCodeMod::PositionReferenceCode e) {
	return "<"+elementName+">"+CPositionReferenceCode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<PositionReferenceCodeMod::PositionReferenceCode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CPositionReferenceCode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<PositionReferenceCodeMod::PositionReferenceCode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CPositionReferenceCode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<PositionReferenceCodeMod::PositionReferenceCode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CPositionReferenceCode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

PositionReferenceCodeMod::PositionReferenceCode EnumerationParser::getPositionReferenceCode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	PositionReferenceCode result;
	try {
		result = CPositionReferenceCode::newPositionReferenceCode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a PositionReferenceCode.", tableName);
	}
	return result;
}

vector<PositionReferenceCodeMod::PositionReferenceCode> EnumerationParser::getPositionReferenceCode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<PositionReferenceCodeMod::PositionReferenceCode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CPositionReferenceCode::newPositionReferenceCode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PositionReferenceCode.", tableName);
	}

	return result;
}

vector<vector<PositionReferenceCodeMod::PositionReferenceCode> > EnumerationParser::getPositionReferenceCode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<PositionReferenceCodeMod::PositionReferenceCode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<PositionReferenceCodeMod::PositionReferenceCode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CPositionReferenceCode::newPositionReferenceCode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a PositionReferenceCode.", tableName);
	}	
	return result;	
}


vector<vector<vector<PositionReferenceCodeMod::PositionReferenceCode> > > EnumerationParser::getPositionReferenceCode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<PositionReferenceCodeMod::PositionReferenceCode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<PositionReferenceCodeMod::PositionReferenceCode> v_aux;
		vector<vector<PositionReferenceCodeMod::PositionReferenceCode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CPositionReferenceCode::newPositionReferenceCode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a PositionReferenceCode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, BaselineReferenceCodeMod::BaselineReferenceCode e) {
	return "<"+elementName+">"+CBaselineReferenceCode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<BaselineReferenceCodeMod::BaselineReferenceCode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CBaselineReferenceCode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CBaselineReferenceCode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CBaselineReferenceCode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

BaselineReferenceCodeMod::BaselineReferenceCode EnumerationParser::getBaselineReferenceCode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	BaselineReferenceCode result;
	try {
		result = CBaselineReferenceCode::newBaselineReferenceCode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a BaselineReferenceCode.", tableName);
	}
	return result;
}

vector<BaselineReferenceCodeMod::BaselineReferenceCode> EnumerationParser::getBaselineReferenceCode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<BaselineReferenceCodeMod::BaselineReferenceCode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CBaselineReferenceCode::newBaselineReferenceCode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a BaselineReferenceCode.", tableName);
	}

	return result;
}

vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> > EnumerationParser::getBaselineReferenceCode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<BaselineReferenceCodeMod::BaselineReferenceCode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CBaselineReferenceCode::newBaselineReferenceCode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a BaselineReferenceCode.", tableName);
	}	
	return result;	
}


vector<vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> > > EnumerationParser::getBaselineReferenceCode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<BaselineReferenceCodeMod::BaselineReferenceCode> v_aux;
		vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CBaselineReferenceCode::newBaselineReferenceCode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a BaselineReferenceCode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, CorrelatorTypeMod::CorrelatorType e) {
	return "<"+elementName+">"+CCorrelatorType::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<CorrelatorTypeMod::CorrelatorType>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CCorrelatorType::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<CorrelatorTypeMod::CorrelatorType> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CCorrelatorType::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<CorrelatorTypeMod::CorrelatorType> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CCorrelatorType::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

CorrelatorTypeMod::CorrelatorType EnumerationParser::getCorrelatorType(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	CorrelatorType result;
	try {
		result = CCorrelatorType::newCorrelatorType(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a CorrelatorType.", tableName);
	}
	return result;
}

vector<CorrelatorTypeMod::CorrelatorType> EnumerationParser::getCorrelatorType1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<CorrelatorTypeMod::CorrelatorType>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CCorrelatorType::newCorrelatorType(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorType.", tableName);
	}

	return result;
}

vector<vector<CorrelatorTypeMod::CorrelatorType> > EnumerationParser::getCorrelatorType2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<CorrelatorTypeMod::CorrelatorType> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<CorrelatorTypeMod::CorrelatorType> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CCorrelatorType::newCorrelatorType(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorType.", tableName);
	}	
	return result;	
}


vector<vector<vector<CorrelatorTypeMod::CorrelatorType> > > EnumerationParser::getCorrelatorType3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<CorrelatorTypeMod::CorrelatorType> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<CorrelatorTypeMod::CorrelatorType> v_aux;
		vector<vector<CorrelatorTypeMod::CorrelatorType> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CCorrelatorType::newCorrelatorType(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a CorrelatorType.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, DopplerTrackingModeMod::DopplerTrackingMode e) {
	return "<"+elementName+">"+CDopplerTrackingMode::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<DopplerTrackingModeMod::DopplerTrackingMode>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CDopplerTrackingMode::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CDopplerTrackingMode::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CDopplerTrackingMode::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

DopplerTrackingModeMod::DopplerTrackingMode EnumerationParser::getDopplerTrackingMode(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	DopplerTrackingMode result;
	try {
		result = CDopplerTrackingMode::newDopplerTrackingMode(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a DopplerTrackingMode.", tableName);
	}
	return result;
}

vector<DopplerTrackingModeMod::DopplerTrackingMode> EnumerationParser::getDopplerTrackingMode1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<DopplerTrackingModeMod::DopplerTrackingMode>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CDopplerTrackingMode::newDopplerTrackingMode(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DopplerTrackingMode.", tableName);
	}

	return result;
}

vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> > EnumerationParser::getDopplerTrackingMode2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<DopplerTrackingModeMod::DopplerTrackingMode> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CDopplerTrackingMode::newDopplerTrackingMode(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a DopplerTrackingMode.", tableName);
	}	
	return result;	
}


vector<vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> > > EnumerationParser::getDopplerTrackingMode3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<DopplerTrackingModeMod::DopplerTrackingMode> v_aux;
		vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CDopplerTrackingMode::newDopplerTrackingMode(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a DopplerTrackingMode.", tableName);
	}
	
	return result;	
}					




		
string EnumerationParser::toXML(const string& elementName, SynthProfMod::SynthProf e) {
	return "<"+elementName+">"+CSynthProf::name(e)+"</"+elementName+">";
}

string EnumerationParser::toXML(const string& elementName, const vector<SynthProfMod::SynthProf>& v_e) {
	ostringstream oss;
	oss << "<" << elementName << ">" 
		<< " 1" 
		<< " " << v_e.size();

	for (unsigned int i = 0; i < v_e.size(); i++) 
		oss << " " << CSynthProf::name(v_e.at(i));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<SynthProfMod::SynthProf> >& vv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 2"
		<< " " <<vv_e.size()
		<< " " <<vv_e.at(0).size();
		
	for (unsigned int i = 0; i < vv_e.size(); i++)
		for (unsigned int j = 0; j < vv_e.at(i).size(); j++) 
			oss << " " << CSynthProf::name(vv_e.at(i).at(j));
	oss << "</" << elementName << ">";
	return oss.str();
}

string EnumerationParser::toXML(const string& elementName, const vector<vector<vector<SynthProfMod::SynthProf> > >& vvv_e) {
	ostringstream oss;
	oss << "<" << elementName << ">"  
		<< " 3"
		<< " " <<vvv_e.size()
		<< " " <<vvv_e.at(0).size()
		<< " " <<vvv_e.at(0).at(0).size();
		
	for (unsigned int i = 0; i < vvv_e.size(); i++)
		for (unsigned int j = 0; j < vvv_e.at(i).size(); j++)
			for (unsigned int k = 0; k < vvv_e.at(i).at(j).size(); k++)
				oss << " " << CSynthProf::name(vvv_e.at(i).at(j).at(k));
	oss << "</" << elementName << ">";
	return oss.str();
}

SynthProfMod::SynthProf EnumerationParser::getSynthProf(const string &name, const string &tableName, const string &xmlDoc) {
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
				
	SynthProf result;
	try {
		result = CSynthProf::newSynthProf(s);
	}
	catch (...) {
			throw ConversionException("Error: could not convert '"+s+"' into a SynthProf.", tableName);
	}
	return result;
}

vector<SynthProfMod::SynthProf> EnumerationParser::getSynthProf1D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<SynthProfMod::SynthProf>	result;
	
	string s = getField(xmlDoc,name);
		if (s.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;
	
	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 2 at the minimum (there may be an empty array)
	if (tokens.size() < 2) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);

	
	
	// The number of dimension should be 1.
	if (tokens.at(0) != "1")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	// Then parse the size of the unique dimension
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 < 0)
		throw ConversionException("Error: wrong size for the unique dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	if (tokens.size() != (unsigned int) (size1 + 2))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 2;
	try {
		for (unsigned int i = 0 ; i < (unsigned int) size1; i++) {
			 result.push_back(CSynthProf::newSynthProf(tokens.at(k).c_str()));
			 k++;
		}
	} 
	catch (...) {
			throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SynthProf.", tableName);
	}

	return result;
}

vector<vector<SynthProfMod::SynthProf> > EnumerationParser::getSynthProf2D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<SynthProfMod::SynthProf> >	result;
	
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 3 at the minimum (there may be an empty array)
	if (tokens.size() < 3) 
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	
		
		
	// The number of dimension should be 2.
	if (tokens.at(0) != "2")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
	
	// Then parse the size of the two dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
	errno = 0;
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2 + 3))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
		
	int k = 3;
	try {
		vector<SynthProfMod::SynthProf> v_aux;
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			v_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.push_back(CSynthProf::newSynthProf(tokens.at(k).c_str()));
				k++;
			}
			result.push_back(v_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error: in '" + s + "' could not convert '"+tokens.at(k)+"' into a SynthProf.", tableName);
	}	
	return result;	
}


vector<vector<vector<SynthProfMod::SynthProf> > > EnumerationParser::getSynthProf3D(const string &name, const string &tableName, const string &xmlDoc) {
	vector<vector<vector<SynthProfMod::SynthProf> >	>result;
		
	string s = getField(xmlDoc,name);
	if (s.length() == 0)
		throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
	istringstream iss;
	iss.str(s);
	vector<string> tokens;

	// Tokenize.
	string buf;
	while (iss >> buf) {
		tokens.push_back(buf);
	}
	
	// The length must be 4 at the minimum (there may be an empty array)
	if (tokens.size() < 4)
		throw ConversionException("Error: missing values in field \"" + 
				name + "\" or invalid syntax(" + s +"')",tableName);	 

		
	// The number of dimension should be 3.
	if (tokens.at(0) != "3")
		throw ConversionException("Error: wrong dimensionality in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);	
	
	// Then parse the size of the three dimensions
	errno = 0;
	int size1 = atoi(tokens.at(1).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size1 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;		
	int size2 = atoi(tokens.at(2).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	if (size2 <= 0)
		throw ConversionException("Error: wrong size for the first dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 

	errno = 0;
	int size3 = atoi(tokens.at(3).c_str());
	if (errno != 0) throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax ('" + s +"')", tableName);	
	
	
	if (size3 < 0)
		throw ConversionException("Error: wrong size for the second dimension \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName); 
		
	if (tokens.size() != (unsigned int) (size1*size2*size3 + 4))
		throw ConversionException("Error: incorrect number of values in field \"" + 
				name + "\" or invalid syntax('" + s +"')",tableName);
				
	int k = 4;
	try {
		vector<SynthProfMod::SynthProf> v_aux;
		vector<vector<SynthProfMod::SynthProf> > vv_aux;	
		for (unsigned int i = 0; i < (unsigned int) size1; i++) {
			vv_aux.clear();
			for (unsigned int j = 0; j < (unsigned int) size2; j++) {
				v_aux.clear();
				for (unsigned int l = 0; l < (unsigned int) size3; l++) {
					v_aux.push_back(CSynthProf::newSynthProf(tokens.at(k).c_str()));
					k++;
				}
				vv_aux.push_back(v_aux);
			}
			result.push_back(vv_aux);
		}
	}
	catch (...) {
		throw ConversionException("Error:in '" + s + "' could not convert '"+tokens.at(k)+"' into a SynthProf.", tableName);
	}
	
	return result;	
}					


 
 } // namespace asdm.
 
