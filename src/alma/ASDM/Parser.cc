
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
 * File Parser.cpp
 */
#include <iostream>
#include <sstream>
#include <alma/ASDM/Base64.h>

#include <alma/ASDM/Parser.h>
#ifndef WITHOUT_BOOST
#include <boost/property_tree/detail/xml_parser_utils.hpp>
#endif
#include <alma/ASDM/OutOfBoundsException.h>
#include <alma/ASDM/NumberFormatException.h>
#include <alma/ASDM/BooleanWrapper.h>
#include <alma/ASDM/ByteWrapper.h>
#include <alma/ASDM/CharacterWrapper.h>
#include <alma/ASDM/DoubleWrapper.h>
#include <alma/ASDM/FloatWrapper.h>
#include <alma/ASDM/IntegerWrapper.h>
#include <alma/ASDM/LongWrapper.h>
#include <alma/ASDM/ShortWrapper.h>
using asdm::OutOfBoundsException;
using asdm::NumberFormatException;
using asdm::Boolean;
using asdm::Byte;
using asdm::Character;
using asdm::Double;
using asdm::Float;
using asdm::Integer;
using asdm::Long;
using asdm::Short;

using namespace std;

namespace asdm { 
 
	string Parser::substring(const string &s, int a, int b) {
		return s.substr(a,(b - a));
	}
	
  	string Parser::trim(const string &s) {
		unsigned int i = 0;
		//while (s.at(i) == ' ' && i < s.length())
		while (i < s.length() && s.at(i) == ' ')
			++i;
		if (i == s.length())
			return "";
		unsigned int j = s.length() - 1;
		//while (s.at(j) == ' ' && j > i)
		while (j > i && s.at(j) == ' ')
			--j;
		return substring(s,i,j + 1);		
	}

	string Parser::encode(const string &s) {
#ifndef WITHOUT_BOOST
	    return boost::property_tree::xml_parser::encode_char_entities(s);
#else
            string result;		    
            for (string::const_iterator it=s.begin(); it!=s.end();++it) {
                switch(*it) {
                case '&':  result += "&amp;";   break;
                case '<':  result += "&gt;";    break;
                case '>':  result += "&lt;";    break;
                case '\"': result += "&quot;";  break;
                case '\'': result += "&apos;";  break;
                default:   result += *it;
                }
            }
            return result;
#endif
        }

#ifndef WITHOUT_BOOST
        string Parser::decode(const string &s, const string &/* tableName */) {
            return boost::property_tree::xml_parser::decode_char_entities(s);
        }
#else
        string Parser::decode(const string &s, const string &tableName) {
            string result;
            for (string::const_iterator it=s.begin(); it!=s.end();++it) {
                // is this the start of a special set of characters to be decoded
                if (*it == '&') {
                    // is it correctly terminated
                    string::const_iterator term = find(it+1,s.end(),';');
                    if (term == s.end())
		        throw ConversionException("Error: Missing semi-colon after start of special characters in \"" + 
                                 s + "\" : Invalid XML syntax", tableName);

                    string thing(it+1,term);
                    if (thing == "amp") result += '&';
                    else if (thing == "gt") result += '>';
                    else if (thing == "lt") result += '<';
                    else if (thing == "quot") result += '\"';
                    else if (thing == "apos") result += '\'';
                    else 
		        throw ConversionException("Error: Unrecognized special characters in \"" + 
                                 s + "\" : Invalid XML syntax", tableName);

                    it = term;
                } else {
                    result += *it;
                }
            }
            return result;
        }
#endif

 
 	/**
	 * Get the portion of the string bounded by s1 and s2, inclusive.
	 * @param s1
	 * @param s2
	 * @return
	 */
	string Parser::getElement(const string &s1, const string &s2) {
		beg = str.find(s1,pos);
		if (beg == string::npos)
			return "";
		end = str.find(s2,beg + s1.length());
		if (end == string::npos)
			return "";
		pos = end + s2.length();
		return substring(str,beg,end + s2.length());
	}

	/**
	 * Get the portion of the string bounded by s1 and s2, exclusive.
	 * @param s1
	 * @param s2
	 * @return
	 */
	string Parser::getElementContent(const string &s1, const string &s2) {
		string s = getElement(s1,s2);
		if (s.length() == 0)
			return "";
		s = substring(str,beg + s1.length(),end);
		return trim(s);
	}

	string Parser::getField(const string &field) {
		beg = str.find("<" + field + ">");
		if (beg == string::npos)
			return "";
		beg += field.length() + 2;
		end = str.find("</" + field + ">",beg);
		if (end == string::npos)
			return "";
		string s = substring(str,beg,end);
		return trim(s);
	}

	string Parser::getField(const string &xml, const string &field) {
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

	//////////////////////////////////////////////////////
	// The follwing is a special case.
	//////////////////////////////////////////////////////

	string Parser::getString(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		/*
		if (xmlField == "")
			throw  ConversionException("Error: Missing field \"" + 
					name + "\" or invalid syntax",tableName);
		*/
		return Parser::decode(xmlField,tableName);
	}

	vector<string> Parser::get1DString(const string &name, const string &tableName, const string &xmlDoc)
	{
		vector<string> x(2);
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField == "") {
			throw  ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
						name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<string> value(dim0);
			if (dim0 == 0)
				return value;
			t.nextToken("\""); // the space
			value[0] = t.nextToken();
			for (int i = 1; i < dim0; ++i) {
				t.nextToken(); // the space
				string thisToken = t.nextToken();		
				value[i] = Parser::decode(thisToken,tableName);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
						name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
					name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Unexpected end of string", tableName);
		}
	}
 
	vector <vector<string> > Parser::get2DString(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField == "") {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
						name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector <vector<string> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;
			t.nextToken("\""); // the space
			
			vector<string>v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.push_back(Parser::decode(t.nextToken(),tableName));
					if (i != dim0 - 1 || j != dim1 - 1)
						t.nextToken(); // the space
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
						name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
					name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Unexpected end of string", tableName);
		}
	}
 
	vector <vector <vector<string> > > Parser::get3DString(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField == "") {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
						name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector <vector <vector<string> > > value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			t.nextToken("\""); // the space
			
			vector<string> v_aux;
			vector<vector <string> > vv_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.push_back( Parser::decode(t.nextToken(),tableName));
						if (i != dim0 - 1 || j != dim1 - 1 || k != dim2 - 1)
							t.nextToken(); // the space
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
						name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
					name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Unexpected end of string", tableName);
		}
	}

	// Generated methods for conversion to and from XML
	// data representations for all types, both primitive
	// and extended.  Also included are 1, 2, and 3 
	// dimensional arrays of these types.

	
	// Field type: int

	void Parser::toXML(int data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(Integer::toString(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	 void Parser::toXML(set< int > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		set < int >::iterator iter;
		for (iter=data.begin(); iter!=data.end(); iter++) {
			
			buf.append(Integer::toString(*iter));
			buf.append(" ");			
			
		}	
	}
	
	
	void Parser::toXML(vector<int> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(Integer::toString(data[i]));
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<int> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(Integer::toString(data[i][j]));
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<int> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(Integer::toString(data[i][j][k]));
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<int> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(Integer::toString(data[i][j][k][l]));
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
	
	set< int >  Parser::getIntSet(const string &name, const string &tableName, const string &xmlDoc) {
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
		StringTokenizer t(xmlField," ");
		set < int > result;
		
		while (t.hasMoreTokens()) {
		
			
			try {
				int data = Integer::parseInt(xmlField);
				result.insert(data);
			} catch (const NumberFormatException &e) {
				throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
			}
		
		}
		return result;							
	}
		
	
	int Parser::getInteger(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		try {
			int data = Integer::parseInt(xmlField);
			return data;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		}
		
	}

	vector<int> Parser::get1DInteger(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<int> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Integer::parseInt(t.nextToken());
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<int> > Parser::get2DInteger(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<int> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<int> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Integer::parseInt(t.nextToken()));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<int> > > Parser::get3DInteger(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<int> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<int> > vv_aux;
			vector<int> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Integer::parseInt(t.nextToken()));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<int> > > >Parser::get4DInteger(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<int> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<int> > >vvv_aux;
			vector<vector< int> > vv_aux;
			vector<int> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Integer::parseInt(t.nextToken()));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: short

	void Parser::toXML(short data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(Short::toString(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<short> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(Short::toString(data[i]));
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<short> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(Short::toString(data[i][j]));
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<short> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(Short::toString(data[i][j][k]));
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<short> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(Short::toString(data[i][j][k][l]));
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	short Parser::getShort(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		try {
			short data = Short::parseShort(xmlField);
			return data;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		}
		
	}

	vector<short> Parser::get1DShort(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<short> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Short::parseShort(t.nextToken());
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<short> > Parser::get2DShort(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<short> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<short> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Short::parseShort(t.nextToken()));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<short> > > Parser::get3DShort(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<short> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<short> > vv_aux;
			vector<short> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Short::parseShort(t.nextToken()));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<short> > > >Parser::get4DShort(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<short> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<short> > >vvv_aux;
			vector<vector< short> > vv_aux;
			vector<short> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Short::parseShort(t.nextToken()));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: int64_t

	void Parser::toXML(int64_t data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(Long::toString(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<int64_t> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(Long::toString(data[i]));
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<int64_t> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(Long::toString(data[i][j]));
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<int64_t> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(Long::toString(data[i][j][k]));
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<int64_t> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(Long::toString(data[i][j][k][l]));
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	int64_t Parser::getLong(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		try {
			int64_t data = Long::parseLong(xmlField);
			return data;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		}
		
	}

	vector<int64_t> Parser::get1DLong(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<int64_t> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Long::parseLong(t.nextToken());
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<int64_t> > Parser::get2DLong(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<int64_t> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<int64_t> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Long::parseLong(t.nextToken()));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<int64_t> > > Parser::get3DLong(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<int64_t> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<int64_t> > vv_aux;
			vector<int64_t> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Long::parseLong(t.nextToken()));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<int64_t> > > >Parser::get4DLong(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<int64_t> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<int64_t> > >vvv_aux;
			vector<vector< int64_t> > vv_aux;
			vector<int64_t> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Long::parseLong(t.nextToken()));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: char

	void Parser::toXML(char data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(Byte::toString(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<char> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(Byte::toString(data[i]));
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<char> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(Byte::toString(data[i][j]));
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<char> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(Byte::toString(data[i][j][k]));
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<char> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(Byte::toString(data[i][j][k][l]));
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	char Parser::getByte(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		try {
			char data = Byte::parseByte(xmlField);
			return data;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		}
		
	}

	vector<char> Parser::get1DByte(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<char> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Byte::parseByte(t.nextToken());
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<char> > Parser::get2DByte(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<char> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<char> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Byte::parseByte(t.nextToken()));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<char> > > Parser::get3DByte(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<char> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<char> > vv_aux;
			vector<char> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Byte::parseByte(t.nextToken()));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<char> > > >Parser::get4DByte(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<char> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<char> > >vvv_aux;
			vector<vector< char> > vv_aux;
			vector<char> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Byte::parseByte(t.nextToken()));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: float

	void Parser::toXML(float data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(Float::toString(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<float> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(Float::toString(data[i]));
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<float> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(Float::toString(data[i][j]));
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<float> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(Float::toString(data[i][j][k]));
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<float> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(Float::toString(data[i][j][k][l]));
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	float Parser::getFloat(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		try {
			float data = Float::parseFloat(xmlField);
			return data;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		}
		
	}

	vector<float> Parser::get1DFloat(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<float> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Float::parseFloat(t.nextToken());
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<float> > Parser::get2DFloat(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<float> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<float> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Float::parseFloat(t.nextToken()));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<float> > > Parser::get3DFloat(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<float> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<float> > vv_aux;
			vector<float> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Float::parseFloat(t.nextToken()));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<float> > > >Parser::get4DFloat(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<float> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<float> > >vvv_aux;
			vector<vector< float> > vv_aux;
			vector<float> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Float::parseFloat(t.nextToken()));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: double

	void Parser::toXML(double data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(Double::toString(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<double> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(Double::toString(data[i]));
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<double> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(Double::toString(data[i][j]));
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<double> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(Double::toString(data[i][j][k]));
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<double> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(Double::toString(data[i][j][k][l]));
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	double Parser::getDouble(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		try {
			double data = Double::parseDouble(xmlField);
			return data;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		}
		
	}

	vector<double> Parser::get1DDouble(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<double> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Double::parseDouble(t.nextToken());
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<double> > Parser::get2DDouble(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<double> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<double> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Double::parseDouble(t.nextToken()));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<double> > > Parser::get3DDouble(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<double> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<double> > vv_aux;
			vector<double> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Double::parseDouble(t.nextToken()));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<double> > > >Parser::get4DDouble(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<double> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<double> > >vvv_aux;
			vector<vector< double> > vv_aux;
			vector<double> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Double::parseDouble(t.nextToken()));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: unsigned char

	void Parser::toXML(unsigned char data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(Character::toString(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<unsigned char> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(Character::toString(data[i]));
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<unsigned char> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(Character::toString(data[i][j]));
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<unsigned char> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(Character::toString(data[i][j][k]));
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<unsigned char> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(Character::toString(data[i][j][k][l]));
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	unsigned char Parser::getCharacter(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		return xmlField.at(0);
		
	}

	vector<unsigned char> Parser::get1DCharacter(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<unsigned char> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = t.nextToken().at(0);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<unsigned char> > Parser::get2DCharacter(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<unsigned char> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<unsigned char> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(t.nextToken().at(0));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<unsigned char> > > Parser::get3DCharacter(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<unsigned char> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<unsigned char> > vv_aux;
			vector<unsigned char> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back( t.nextToken().at(0));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<unsigned char> > > >Parser::get4DCharacter(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<unsigned char> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<unsigned char> > >vvv_aux;
			vector<vector< unsigned char> > vv_aux;
			vector<unsigned char> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back( t.nextToken().at(0));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: bool

	void Parser::toXML(bool data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(Boolean::toString(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<bool> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(Boolean::toString(data[i]));
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<bool> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(Boolean::toString(data[i][j]));
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<bool> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(Boolean::toString(data[i][j][k]));
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<bool> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(Boolean::toString(data[i][j][k][l]));
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	bool Parser::getBoolean(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		try {
			bool data = Boolean::parseBoolean(xmlField);
			return data;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		}
		
	}

	vector<bool> Parser::get1DBoolean(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<bool> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Boolean::parseBoolean(t.nextToken());
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<bool> > Parser::get2DBoolean(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<bool> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<bool> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Boolean::parseBoolean(t.nextToken()));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<bool> > > Parser::get3DBoolean(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<bool> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<bool> > vv_aux;
			vector<bool> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Boolean::parseBoolean(t.nextToken()));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<bool> > > >Parser::get4DBoolean(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<bool> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<bool> > >vvv_aux;
			vector<vector< bool> > vv_aux;
			vector<bool> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Boolean::parseBoolean(t.nextToken()));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: std::string

	void Parser::toXML(std::string data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		if (data.size()>0)
		    buf.append(Parser::encode(data));
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<std::string> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append("\"");
			if (data[i].size()>0)	
			    buf.append(Parser::encode(data[i]));
			buf.append("\"");
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<std::string> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append("\"");
				if(data[i][j].size()>0)	
				    buf.append(Parser::encode(data[i][j]));

				buf.append("\"");
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<std::string> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append("\"");
					if (data[i][j][k].size() > 0)	
					    buf.append(Parser::encode(data[i][j][k]));

					buf.append("\"");
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<std::string> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append("\"");
						if (data[i][j][k][l].size() > 0)
						    Parser::encode(data[i][j][k][l]);
						buf.append("\"");
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		


	// Field type: Angle

	void Parser::toXML(Angle data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Angle> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Angle> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Angle> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Angle> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Angle Parser::getAngle(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Angle (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Angle> Parser::get1DAngle(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Angle> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Angle::getAngle(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Angle> > Parser::get2DAngle(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Angle> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Angle> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Angle::getAngle(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Angle> > > Parser::get3DAngle(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Angle> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Angle> > vv_aux;
			vector<Angle> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Angle::getAngle(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Angle> > > >Parser::get4DAngle(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Angle> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Angle> > >vvv_aux;
			vector<vector< Angle> > vv_aux;
			vector<Angle> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Angle::getAngle(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: AngularRate

	void Parser::toXML(AngularRate data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<AngularRate> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<AngularRate> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<AngularRate> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<AngularRate> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	AngularRate Parser::getAngularRate(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return AngularRate (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<AngularRate> Parser::get1DAngularRate(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<AngularRate> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = AngularRate::getAngularRate(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<AngularRate> > Parser::get2DAngularRate(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<AngularRate> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<AngularRate> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(AngularRate::getAngularRate(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<AngularRate> > > Parser::get3DAngularRate(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<AngularRate> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<AngularRate> > vv_aux;
			vector<AngularRate> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(AngularRate::getAngularRate(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<AngularRate> > > >Parser::get4DAngularRate(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<AngularRate> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<AngularRate> > >vvv_aux;
			vector<vector< AngularRate> > vv_aux;
			vector<AngularRate> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(AngularRate::getAngularRate(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: ArrayTime

	void Parser::toXML(ArrayTime data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<ArrayTime> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<ArrayTime> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<ArrayTime> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<ArrayTime> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	ArrayTime Parser::getArrayTime(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return ArrayTime (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<ArrayTime> Parser::get1DArrayTime(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<ArrayTime> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = ArrayTime::getArrayTime(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<ArrayTime> > Parser::get2DArrayTime(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<ArrayTime> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<ArrayTime> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(ArrayTime::getArrayTime(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<ArrayTime> > > Parser::get3DArrayTime(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<ArrayTime> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<ArrayTime> > vv_aux;
			vector<ArrayTime> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(ArrayTime::getArrayTime(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<ArrayTime> > > >Parser::get4DArrayTime(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<ArrayTime> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<ArrayTime> > >vvv_aux;
			vector<vector< ArrayTime> > vv_aux;
			vector<ArrayTime> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(ArrayTime::getArrayTime(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: ArrayTimeInterval

	void Parser::toXML(ArrayTimeInterval data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		//buf.append(data.getStart().toString()+" "+data.getDuration().toString());	
		buf.append(data.getMidPoint().toString()+" "+data.getDuration().toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<ArrayTimeInterval> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
		//buf.append(data[i].getStart().toString()+" "+data[i].getDuration().toString()+" ");	
		buf.append(data[i].getMidPoint().toString()+" "+data[i].getDuration().toString()+" ");	
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<ArrayTimeInterval> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				//buf.append(data[i][j].getStart().toString()+" "+data[i][j].getDuration().toString()+" ");	
				buf.append(data[i][j].getMidPoint().toString()+" "+data[i][j].getDuration().toString()+" ");				
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<ArrayTimeInterval> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					//buf.append(data[i][j][k].getStart().toString()+" "+data[i][j][k].getDuration().toString()+" ");	
					buf.append(data[i][j][k].getMidPoint().toString()+" "+data[i][j][k].getDuration().toString()+" ");	
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<ArrayTimeInterval> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						//buf.append(data[i][j][k][l].getStart().toString()+" "+data[i][j][k][l].getDuration().toString()+" ");
						buf.append(data[i][j][k][l].getMidPoint().toString()+" "+data[i][j][k][l].getDuration().toString()+" ");		
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	ArrayTimeInterval Parser::getArrayTimeInterval(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		StringTokenizer t(xmlField," ");
		//int64_t start = Long::parseLong(t.nextToken());
		int64_t l1 = Long::parseLong(t.nextToken());
		int64_t l2 = Long::parseLong(t.nextToken());
		if ( ArrayTimeInterval::readStartTimeDurationInXML() ) 
			return ArrayTimeInterval (l1, l2);
		else 
			return ArrayTimeInterval (l1 - l2 / 2, l2);
		
	}

	vector<ArrayTimeInterval> Parser::get1DArrayTimeInterval(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<ArrayTimeInterval> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				int64_t l1 = Long::parseLong(t.nextToken());
				int64_t l2 = Long::parseLong(t.nextToken());
				if ( ArrayTimeInterval::readStartTimeDurationInXML() ) 
					value[i] = ArrayTimeInterval (l1, l2);
				else 
					value[i] = ArrayTimeInterval (l1 - l2 / 2, l2);

	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<ArrayTimeInterval> > Parser::get2DArrayTimeInterval(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<ArrayTimeInterval> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<ArrayTimeInterval> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					int64_t l1 = Long::parseLong(t.nextToken());
					int64_t l2 = Long::parseLong(t.nextToken());
					if ( ArrayTimeInterval::readStartTimeDurationInXML() ) 
						v_aux.push_back(ArrayTimeInterval (l1, l2));
					else 
						v_aux.push_back(ArrayTimeInterval (l1 - l2 / 2, l2));
											
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<ArrayTimeInterval> > > Parser::get3DArrayTimeInterval(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<ArrayTimeInterval> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<ArrayTimeInterval> > vv_aux;
			vector<ArrayTimeInterval> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						int64_t l1 = Long::parseLong(t.nextToken());
						int64_t l2 = Long::parseLong(t.nextToken());
						if ( ArrayTimeInterval::readStartTimeDurationInXML() ) 
							v_aux.push_back(ArrayTimeInterval (l1, l2));
						else 
							v_aux.push_back(ArrayTimeInterval (l1 - l2 / 2, l2));						
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<ArrayTimeInterval> > > >Parser::get4DArrayTimeInterval(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<ArrayTimeInterval> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<ArrayTimeInterval> > >vvv_aux;
			vector<vector< ArrayTimeInterval> > vv_aux;
			vector<ArrayTimeInterval> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							int64_t l1 = Long::parseLong(t.nextToken());
							int64_t l2 = Long::parseLong(t.nextToken());
							if ( ArrayTimeInterval::readStartTimeDurationInXML() ) 
								v_aux.push_back(ArrayTimeInterval (l1, l2));
							else 
								v_aux.push_back(ArrayTimeInterval (l1 - l2 / 2, l2));	
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Complex

	void Parser::toXML(Complex data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Complex> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Complex> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Complex> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Complex> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Complex Parser::getComplex(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Complex (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Complex> Parser::get1DComplex(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Complex> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Complex::getComplex(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Complex> > Parser::get2DComplex(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Complex> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Complex> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Complex::getComplex(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Complex> > > Parser::get3DComplex(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Complex> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Complex> > vv_aux;
			vector<Complex> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Complex::getComplex(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Complex> > > >Parser::get4DComplex(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Complex> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Complex> > >vvv_aux;
			vector<vector< Complex> > vv_aux;
			vector<Complex> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Complex::getComplex(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Entity

	void Parser::toXML(Entity data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Entity> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Entity> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Entity> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Entity> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Entity Parser::getEntity(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Entity (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Entity> Parser::get1DEntity(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Entity> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Entity::getEntity(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Entity> > Parser::get2DEntity(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Entity> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Entity> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Entity::getEntity(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Entity> > > Parser::get3DEntity(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Entity> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Entity> > vv_aux;
			vector<Entity> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Entity::getEntity(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Entity> > > >Parser::get4DEntity(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Entity> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Entity> > >vvv_aux;
			vector<vector< Entity> > vv_aux;
			vector<Entity> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Entity::getEntity(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: EntityId

	void Parser::toXML(EntityId data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<EntityId> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<EntityId> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<EntityId> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<EntityId> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	EntityId Parser::getEntityId(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return EntityId (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<EntityId> Parser::get1DEntityId(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<EntityId> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = EntityId::getEntityId(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<EntityId> > Parser::get2DEntityId(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<EntityId> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<EntityId> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(EntityId::getEntityId(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<EntityId> > > Parser::get3DEntityId(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<EntityId> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<EntityId> > vv_aux;
			vector<EntityId> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(EntityId::getEntityId(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<EntityId> > > >Parser::get4DEntityId(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<EntityId> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<EntityId> > >vvv_aux;
			vector<vector< EntityId> > vv_aux;
			vector<EntityId> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(EntityId::getEntityId(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: EntityRef

	void Parser::toXML(EntityRef data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<EntityRef> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<EntityRef> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<EntityRef> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<EntityRef> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	EntityRef Parser::getEntityRef(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return EntityRef (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<EntityRef> Parser::get1DEntityRef(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<EntityRef> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = EntityRef::getEntityRef(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<EntityRef> > Parser::get2DEntityRef(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<EntityRef> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<EntityRef> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(EntityRef::getEntityRef(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<EntityRef> > > Parser::get3DEntityRef(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<EntityRef> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<EntityRef> > vv_aux;
			vector<EntityRef> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(EntityRef::getEntityRef(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<EntityRef> > > >Parser::get4DEntityRef(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<EntityRef> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<EntityRef> > >vvv_aux;
			vector<vector< EntityRef> > vv_aux;
			vector<EntityRef> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(EntityRef::getEntityRef(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Flux

	void Parser::toXML(Flux data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Flux> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Flux> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Flux> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Flux> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Flux Parser::getFlux(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Flux (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Flux> Parser::get1DFlux(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Flux> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Flux::getFlux(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Flux> > Parser::get2DFlux(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Flux> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Flux> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Flux::getFlux(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Flux> > > Parser::get3DFlux(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Flux> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Flux> > vv_aux;
			vector<Flux> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Flux::getFlux(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Flux> > > >Parser::get4DFlux(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Flux> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Flux> > >vvv_aux;
			vector<vector< Flux> > vv_aux;
			vector<Flux> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Flux::getFlux(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Frequency

	void Parser::toXML(Frequency data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Frequency> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Frequency> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Frequency> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Frequency> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Frequency Parser::getFrequency(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Frequency (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Frequency> Parser::get1DFrequency(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Frequency> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Frequency::getFrequency(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Frequency> > Parser::get2DFrequency(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Frequency> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Frequency> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Frequency::getFrequency(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Frequency> > > Parser::get3DFrequency(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Frequency> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Frequency> > vv_aux;
			vector<Frequency> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Frequency::getFrequency(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Frequency> > > >Parser::get4DFrequency(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Frequency> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Frequency> > >vvv_aux;
			vector<vector< Frequency> > vv_aux;
			vector<Frequency> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Frequency::getFrequency(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Humidity

	void Parser::toXML(Humidity data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Humidity> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Humidity> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Humidity> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Humidity> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Humidity Parser::getHumidity(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Humidity (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Humidity> Parser::get1DHumidity(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Humidity> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Humidity::getHumidity(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Humidity> > Parser::get2DHumidity(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Humidity> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Humidity> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Humidity::getHumidity(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Humidity> > > Parser::get3DHumidity(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Humidity> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Humidity> > vv_aux;
			vector<Humidity> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Humidity::getHumidity(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Humidity> > > >Parser::get4DHumidity(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Humidity> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Humidity> > >vvv_aux;
			vector<vector< Humidity> > vv_aux;
			vector<Humidity> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Humidity::getHumidity(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Interval

	void Parser::toXML(Interval data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Interval> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Interval> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Interval> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Interval> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Interval Parser::getInterval(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Interval (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Interval> Parser::get1DInterval(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Interval> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Interval::getInterval(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Interval> > Parser::get2DInterval(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Interval> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Interval> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Interval::getInterval(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Interval> > > Parser::get3DInterval(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Interval> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Interval> > vv_aux;
			vector<Interval> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Interval::getInterval(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Interval> > > >Parser::get4DInterval(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Interval> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Interval> > >vvv_aux;
			vector<vector< Interval> > vv_aux;
			vector<Interval> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Interval::getInterval(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Length

	void Parser::toXML(Length data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Length> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Length> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Length> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Length> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Length Parser::getLength(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Length (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Length> Parser::get1DLength(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Length> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Length::getLength(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Length> > Parser::get2DLength(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Length> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Length> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Length::getLength(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Length> > > Parser::get3DLength(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Length> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Length> > vv_aux;
			vector<Length> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Length::getLength(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Length> > > >Parser::get4DLength(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Length> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Length> > >vvv_aux;
			vector<vector< Length> > vv_aux;
			vector<Length> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Length::getLength(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Pressure

	void Parser::toXML(Pressure data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Pressure> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Pressure> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Pressure> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Pressure> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Pressure Parser::getPressure(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Pressure (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Pressure> Parser::get1DPressure(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Pressure> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Pressure::getPressure(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Pressure> > Parser::get2DPressure(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Pressure> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Pressure> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Pressure::getPressure(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Pressure> > > Parser::get3DPressure(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Pressure> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Pressure> > vv_aux;
			vector<Pressure> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Pressure::getPressure(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Pressure> > > >Parser::get4DPressure(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Pressure> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Pressure> > >vvv_aux;
			vector<vector< Pressure> > vv_aux;
			vector<Pressure> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Pressure::getPressure(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Speed

	void Parser::toXML(Speed data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Speed> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Speed> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Speed> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Speed> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Speed Parser::getSpeed(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Speed (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Speed> Parser::get1DSpeed(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Speed> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Speed::getSpeed(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Speed> > Parser::get2DSpeed(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Speed> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Speed> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Speed::getSpeed(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Speed> > > Parser::get3DSpeed(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Speed> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Speed> > vv_aux;
			vector<Speed> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Speed::getSpeed(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Speed> > > >Parser::get4DSpeed(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Speed> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Speed> > >vvv_aux;
			vector<vector< Speed> > vv_aux;
			vector<Speed> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Speed::getSpeed(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Tag

	void Parser::toXML(Tag data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	 void Parser::toXML(set< Tag > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		set < Tag >::iterator iter;
		for (iter=data.begin(); iter!=data.end(); iter++) {
			
			buf.append((*iter).toString()+" ");
			
		}	
	}
	
	
	void Parser::toXML(vector<Tag> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Tag> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Tag> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Tag> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
	
	set< Tag >  Parser::getTagSet(const string &name, const string &tableName, const string &xmlDoc) {
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
		StringTokenizer t(xmlField," ");
		set < Tag > result;
		
		while (t.hasMoreTokens()) {
		
			
			try {
				result.insert(Tag::parseTag(t.nextToken()));
			}
			catch (const TagFormatException &e) {
				throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
			}	
		
		}
		return result;							
	}
		
	
	Tag Parser::getTag(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
		try {
			return Tag::parseTag(xmlField);
		}
		catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
			name + "\": " + e.getMessage(), tableName);				
		}
		
	}

	vector<Tag> Parser::get1DTag(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Tag> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
					value[i] = Tag::parseTag(t.nextToken());
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Tag> > Parser::get2DTag(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Tag> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Tag> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Tag::parseTag(t.nextToken()));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Tag> > > Parser::get3DTag(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Tag> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Tag> > vv_aux;
			vector<Tag> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Tag::parseTag(t.nextToken()));							
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Tag> > > >Parser::get4DTag(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Tag> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Tag> > >vvv_aux;
			vector<vector< Tag> > vv_aux;
			vector<Tag> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Tag::parseTag(t.nextToken()));							
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


	// Field type: Temperature

	void Parser::toXML(Temperature data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
	
		buf.append(data.toString());
		
		buf.append(" </" + name + "> ");
	}

	
	
	
	void Parser::toXML(vector<Temperature> data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("1 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
	
			buf.append(data[i].toString());
		
			buf.append(" ");
		}
		buf.append(" </" + name + "> ");
	}

	void Parser::toXML(vector< vector<Temperature> > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("2 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
	
				buf.append(data[i][j].toString());
		
				buf.append(" ");
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector< vector< vector<Temperature> > > data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("3 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
	
					buf.append(data[i][j][k].toString());
		
					buf.append(" ");
				}
			}
		}
		buf.append(" </" + name + "> ");
	}
	
	void Parser::toXML(vector<vector< vector< vector<Temperature> > > >data, const string &name, string &buf) {
		buf.append("<" + name + "> ");
		buf.append("4 ");
		buf.append(Integer::toString(data.size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0].size()));
		buf.append(" ");
		buf.append(Integer::toString(data[0][0][0].size()));
		buf.append(" ");
		for (unsigned int i = 0; i < data.size(); ++i) {
			for (unsigned int j = 0; j < data[i].size(); ++j) {
				for (unsigned int k = 0; k < data[i][j].size(); ++k) {
					for (unsigned int l = 0; l < data[i][j][k].size(); l++) {
	
						buf.append(data[i][j][k][l].toString());
		
						buf.append(" ");
					}
				}
			}
		}
		buf.append(" </" + name + "> ");
	}	
	
		

	
		
	
	Temperature Parser::getTemperature(const string &name, const string &tableName, const string &xmlDoc) 
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0)
			throw ConversionException("Error: Missing field \"" + 
				name + "\" or invalid syntax",tableName);
	
        try {	
			return Temperature (xmlField);
		}
		catch (const InvalidArgumentException &e) {
			throw ConversionException("Error: message was '"+ 
			                           e.getMessage()+
			                           "'",
			                           tableName);
		}
		
	}

	vector<Temperature> Parser::get1DTemperature(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 1) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			vector<Temperature> value (dim0);
			if (dim0 == 0)
				return value;
			for (int i = 0; i < dim0; ++i) {
	
				value[i] = Temperature::getTemperature(t);
	
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
		
	vector< vector<Temperature> > Parser::get2DTemperature(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 2) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			vector< vector<Temperature> > value;

			if (dim0 == 0 || dim1 == 0)
				return value;

			vector<Temperature> v_aux;
			for (int i = 0; i < dim0; ++i) {
				v_aux.clear();
				for (int j = 0; j < dim1; ++j) {
	
					v_aux.push_back(Temperature::getTemperature(t));
	
				}
				value.push_back(v_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	
	vector< vector< vector<Temperature> > > Parser::get3DTemperature(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 3) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			vector< vector< vector<Temperature> > > value ;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0)
				return value;
			
			vector<vector<Temperature> > vv_aux;
			vector<Temperature> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					v_aux.clear();
					for (int k = 0; k < dim2; ++k) {
	
						v_aux.push_back(Temperature::getTemperature(t));
	
					}
					vv_aux.push_back(v_aux);
				}
				value.push_back(vv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}
	
	vector< vector< vector< vector<Temperature> > > >Parser::get4DTemperature(const string &name, const string &tableName, const string &xmlDoc)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		StringTokenizer t(xmlField," ");
		try {
			int ndim = Integer::parseInt(t.nextToken());
			if (ndim != 4) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Invalid array format", tableName);
			}
			int dim0 = Integer::parseInt(t.nextToken());
			int dim1 = Integer::parseInt(t.nextToken());
			int dim2 = Integer::parseInt(t.nextToken());
			int dim3 = Integer::parseInt(t.nextToken());
			vector<vector< vector< vector<Temperature> > > >value;
			if (dim0 == 0 || dim1 == 0 || dim2 == 0 ||  dim3 == 0)
				return value;
			
			vector<vector<vector<Temperature> > >vvv_aux;
			vector<vector< Temperature> > vv_aux;
			vector<Temperature> v_aux;
			for (int i = 0; i < dim0; ++i) {
				vvv_aux.clear();
				for (int j = 0; j < dim1; ++j) {
					vv_aux.clear();
					for (int k = 0; k < dim2; ++k) {
						v_aux.clear();
						for (int l = 0; l < dim3; l++) {
	
							v_aux.push_back(Temperature::getTemperature(t));
	
						}
						vv_aux.push_back(v_aux);
					}
					vvv_aux.push_back(vv_aux);
				}
				value.push_back(vvv_aux);
			}
			if (t.hasMoreTokens()) {
				throw ConversionException("Error: Field \"" + 
					name + "\": Syntax error.", tableName);
			}
			return value;
		} catch (const NumberFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);
		} catch (const OutOfBoundsException &e) {
			throw  ConversionException("Error: Field \"" + 
				name + "\": Unexpected end of string", tableName);
		} catch (const TagFormatException &e) {
			throw ConversionException("Error: Field \"" + 
				name + "\": " + e.getMessage(), tableName);				
		}		
	}	
	

		


 
    // Generated methods for conversion to and from XML
	// data representations with  a Base64 encoded content.
	// The methods are generated only for 1, 2 and 3 dimensional arrays
	// of data whose BasicType have a non null BaseWrapperName.
	// In practice this represents data whose type is one of the basic numeric types
	// or is built upon a basic numeric type. 

	
	
	
	
	
	void Parser::toXMLBase64(vector<int> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
     		int v = data.at(i);
     		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<int> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
     			int v = data.at(i).at(j);
     		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<int> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
     				int v = data.at(i).at(j).at(k);
     		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<int> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
     					int v = data.at(i).at(j).at(k).at(l);
     		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Int_CPP
		
#define Int_CPP int	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<int>& Parser::get1DIntegerFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<int>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Int_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(v);
			
		}
		return attribute;
	}
	
	 
	vector <vector<int> >& Parser::get2DIntegerFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<int> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Int_CPP v;
		vector<int> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(v);
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<int> > >& Parser::get3DIntegerFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<int> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Int_CPP v;
		vector <vector<int> > aux2;
		vector<int> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(v);
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<int> > > >& Parser::get4DIntegerFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<int> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Int_CPP v;
		vector <vector <vector<int> > > aux2;
		vector <vector<int> > aux3;
		vector<int> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(v);
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<short> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
     		short v = data.at(i);
     		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<short> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
     			short v = data.at(i).at(j);
     		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<short> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
     				short v = data.at(i).at(j).at(k);
     		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<short> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
     					short v = data.at(i).at(j).at(k).at(l);
     		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Short_CPP
		
#define Short_CPP short	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<short>& Parser::get1DShortFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<short>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Short_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(v);
			
		}
		return attribute;
	}
	
	 
	vector <vector<short> >& Parser::get2DShortFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<short> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Short_CPP v;
		vector<short> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(v);
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<short> > >& Parser::get3DShortFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<short> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Short_CPP v;
		vector <vector<short> > aux2;
		vector<short> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(v);
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<short> > > >& Parser::get4DShortFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<short> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Short_CPP v;
		vector <vector <vector<short> > > aux2;
		vector <vector<short> > aux3;
		vector<short> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(v);
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<int64_t> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
     		int64_t v = data.at(i);
     		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<int64_t> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
     			int64_t v = data.at(i).at(j);
     		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<int64_t> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
     				int64_t v = data.at(i).at(j).at(k);
     		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<int64_t> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
     					int64_t v = data.at(i).at(j).at(k).at(l);
     		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Long_CPP
		
#define Long_CPP int64_t
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<int64_t>& Parser::get1DLongFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<int64_t>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(v);
			
		}
		return attribute;
	}
	
	 
	vector <vector<int64_t> >& Parser::get2DLongFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<int64_t> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector<int64_t> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(v);
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<int64_t> > >& Parser::get3DLongFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<int64_t> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector <vector<int64_t> > aux2;
		vector<int64_t> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(v);
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<int64_t> > > >& Parser::get4DLongFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<int64_t> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector <vector <vector<int64_t> > > aux2;
		vector <vector<int64_t> > aux3;
		vector<int64_t> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(v);
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<char> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
     		char v = data.at(i);
     		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<char> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
     			char v = data.at(i).at(j);
     		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<char> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
     				char v = data.at(i).at(j).at(k);
     		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<char> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
     					char v = data.at(i).at(j).at(k).at(l);
     		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Byte_CPP
		
#define Byte_CPP char
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<char>& Parser::get1DByteFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<char>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Byte_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(v);
			
		}
		return attribute;
	}
	
	 
	vector <vector<char> >& Parser::get2DByteFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<char> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Byte_CPP v;
		vector<char> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(v);
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<char> > >& Parser::get3DByteFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<char> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Byte_CPP v;
		vector <vector<char> > aux2;
		vector<char> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(v);
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<char> > > >& Parser::get4DByteFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<char> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Byte_CPP v;
		vector <vector <vector<char> > > aux2;
		vector <vector<char> > aux3;
		vector<char> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(v);
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<float> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
     		float v = data.at(i);
     		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<float> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
     			float v = data.at(i).at(j);
     		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<float> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
     				float v = data.at(i).at(j).at(k);
     		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<float> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
     					float v = data.at(i).at(j).at(k).at(l);
     		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Float_CPP
		
#define Float_CPP float	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<float>& Parser::get1DFloatFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<float>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Float_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(v);
			
		}
		return attribute;
	}
	
	 
	vector <vector<float> >& Parser::get2DFloatFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<float> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Float_CPP v;
		vector<float> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(v);
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<float> > >& Parser::get3DFloatFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<float> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Float_CPP v;
		vector <vector<float> > aux2;
		vector<float> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(v);
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<float> > > >& Parser::get4DFloatFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<float> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Float_CPP v;
		vector <vector <vector<float> > > aux2;
		vector <vector<float> > aux3;
		vector<float> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(v);
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<double> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
     		double v = data.at(i);
     		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<double> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
     			double v = data.at(i).at(j);
     		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<double> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
     				double v = data.at(i).at(j).at(k);
     		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<double> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
     					double v = data.at(i).at(j).at(k).at(l);
     		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<double>& Parser::get1DDoubleFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<double>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(v);
			
		}
		return attribute;
	}
	
	 
	vector <vector<double> >& Parser::get2DDoubleFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<double> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<double> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(v);
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<double> > >& Parser::get3DDoubleFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<double> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<double> > aux2;
		vector<double> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(v);
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<double> > > >& Parser::get4DDoubleFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<double> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<double> > > aux2;
		vector <vector<double> > aux3;
		vector<double> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(v);
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<unsigned char> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
     		unsigned char v = data.at(i);
     		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<unsigned char> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
     			unsigned char v = data.at(i).at(j);
     		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<unsigned char> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
     				unsigned char v = data.at(i).at(j).at(k);
     		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<unsigned char> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
     					unsigned char v = data.at(i).at(j).at(k).at(l);
     		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Char_CPP
		
#define Char_CPP char	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<unsigned char>& Parser::get1DCharacterFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<unsigned char>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Char_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(v);
			
		}
		return attribute;
	}
	
	 
	vector <vector<unsigned char> >& Parser::get2DCharacterFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<unsigned char> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Char_CPP v;
		vector<unsigned char> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(v);
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<unsigned char> > >& Parser::get3DCharacterFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<unsigned char> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Char_CPP v;
		vector <vector<unsigned char> > aux2;
		vector<unsigned char> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(v);
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<unsigned char> > > >& Parser::get4DCharacterFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<unsigned char> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Char_CPP v;
		vector <vector <vector<unsigned char> > > aux2;
		vector <vector<unsigned char> > aux3;
		vector<unsigned char> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(v);
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<bool> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
     		bool v = data.at(i);
     		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<bool> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
     			bool v = data.at(i).at(j);
     		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<bool> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
     				bool v = data.at(i).at(j).at(k);
     		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<bool> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
     					bool v = data.at(i).at(j).at(k).at(l);
     		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Boolean_CPP
		
#define Boolean_CPP bool
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<bool>& Parser::get1DBooleanFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<bool>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Boolean_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(v);
			
		}
		return attribute;
	}
	
	 
	vector <vector<bool> >& Parser::get2DBooleanFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<bool> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Boolean_CPP v;
		vector<bool> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(v);
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<bool> > >& Parser::get3DBooleanFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<bool> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Boolean_CPP v;
		vector <vector<bool> > aux2;
		vector<bool> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(v);
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<bool> > > >& Parser::get4DBooleanFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<bool> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Boolean_CPP v;
		vector <vector <vector<bool> > > aux2;
		vector <vector<bool> > aux3;
		vector<bool> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(v);
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Angle> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Angle> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Angle> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Angle> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Angle>& Parser::get1DAngleFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Angle>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Angle(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Angle> >& Parser::get2DAngleFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Angle> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<Angle> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Angle(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Angle> > >& Parser::get3DAngleFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Angle> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<Angle> > aux2;
		vector<Angle> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Angle(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Angle> > > >& Parser::get4DAngleFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Angle> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<Angle> > > aux2;
		vector <vector<Angle> > aux3;
		vector<Angle> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Angle(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<AngularRate> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<AngularRate> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<AngularRate> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<AngularRate> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<AngularRate>& Parser::get1DAngularRateFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<AngularRate>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(AngularRate(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<AngularRate> >& Parser::get2DAngularRateFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<AngularRate> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<AngularRate> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(AngularRate(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<AngularRate> > >& Parser::get3DAngularRateFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<AngularRate> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<AngularRate> > aux2;
		vector<AngularRate> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(AngularRate(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<AngularRate> > > >& Parser::get4DAngularRateFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<AngularRate> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<AngularRate> > > aux2;
		vector <vector<AngularRate> > aux3;
		vector<AngularRate> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(AngularRate(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<ArrayTime> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 long v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<ArrayTime> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	long v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<ArrayTime> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		long v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<ArrayTime> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			long v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Long_CPP
		
#define Long_CPP int64_t
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<ArrayTime>& Parser::get1DArrayTimeFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<ArrayTime>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(ArrayTime(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<ArrayTime> >& Parser::get2DArrayTimeFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<ArrayTime> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector<ArrayTime> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(ArrayTime(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<ArrayTime> > >& Parser::get3DArrayTimeFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<ArrayTime> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector <vector<ArrayTime> > aux2;
		vector<ArrayTime> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(ArrayTime(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<ArrayTime> > > >& Parser::get4DArrayTimeFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<ArrayTime> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector <vector <vector<ArrayTime> > > aux2;
		vector <vector<ArrayTime> > aux3;
		vector<ArrayTime> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(ArrayTime(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	

	
	

	
	

	
	

	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Flux> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Flux> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Flux> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Flux> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Flux>& Parser::get1DFluxFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Flux>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Flux(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Flux> >& Parser::get2DFluxFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Flux> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<Flux> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Flux(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Flux> > >& Parser::get3DFluxFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Flux> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<Flux> > aux2;
		vector<Flux> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Flux(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Flux> > > >& Parser::get4DFluxFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Flux> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<Flux> > > aux2;
		vector <vector<Flux> > aux3;
		vector<Flux> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Flux(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Frequency> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Frequency> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Frequency> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Frequency> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Frequency>& Parser::get1DFrequencyFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Frequency>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Frequency(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Frequency> >& Parser::get2DFrequencyFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Frequency> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<Frequency> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Frequency(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Frequency> > >& Parser::get3DFrequencyFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Frequency> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<Frequency> > aux2;
		vector<Frequency> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Frequency(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Frequency> > > >& Parser::get4DFrequencyFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Frequency> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<Frequency> > > aux2;
		vector <vector<Frequency> > aux3;
		vector<Frequency> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Frequency(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Humidity> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Humidity> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Humidity> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Humidity> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Humidity>& Parser::get1DHumidityFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Humidity>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Humidity(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Humidity> >& Parser::get2DHumidityFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Humidity> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<Humidity> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Humidity(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Humidity> > >& Parser::get3DHumidityFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Humidity> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<Humidity> > aux2;
		vector<Humidity> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Humidity(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Humidity> > > >& Parser::get4DHumidityFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Humidity> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<Humidity> > > aux2;
		vector <vector<Humidity> > aux3;
		vector<Humidity> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Humidity(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Interval> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 long v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Interval> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	long v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Interval> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		long v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Interval> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			long v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Long_CPP
		
#define Long_CPP int64_t
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Interval>& Parser::get1DIntervalFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Interval>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Interval(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Interval> >& Parser::get2DIntervalFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Interval> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector<Interval> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Interval(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Interval> > >& Parser::get3DIntervalFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Interval> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector <vector<Interval> > aux2;
		vector<Interval> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Interval(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Interval> > > >& Parser::get4DIntervalFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Interval> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Long_CPP v;
		vector <vector <vector<Interval> > > aux2;
		vector <vector<Interval> > aux3;
		vector<Interval> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Interval(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Length> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Length> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Length> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Length> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Length>& Parser::get1DLengthFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Length>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Length(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Length> >& Parser::get2DLengthFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Length> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<Length> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Length(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Length> > >& Parser::get3DLengthFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Length> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<Length> > aux2;
		vector<Length> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Length(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Length> > > >& Parser::get4DLengthFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Length> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<Length> > > aux2;
		vector <vector<Length> > aux3;
		vector<Length> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Length(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Pressure> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Pressure> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Pressure> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Pressure> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Pressure>& Parser::get1DPressureFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Pressure>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Pressure(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Pressure> >& Parser::get2DPressureFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Pressure> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<Pressure> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Pressure(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Pressure> > >& Parser::get3DPressureFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Pressure> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<Pressure> > aux2;
		vector<Pressure> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Pressure(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Pressure> > > >& Parser::get4DPressureFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Pressure> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<Pressure> > > aux2;
		vector <vector<Pressure> > aux3;
		vector<Pressure> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Pressure(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Speed> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Speed> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Speed> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Speed> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Speed>& Parser::get1DSpeedFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Speed>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Speed(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Speed> >& Parser::get2DSpeedFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Speed> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<Speed> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Speed(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Speed> > >& Parser::get3DSpeedFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Speed> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<Speed> > aux2;
		vector<Speed> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Speed(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Speed> > > >& Parser::get4DSpeedFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Speed> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<Speed> > > aux2;
		vector <vector<Speed> > aux3;
		vector<Speed> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Speed(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	

	
	

	
	
	
	
	void Parser::toXMLBase64(vector<Temperature> data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 1;
  		int dim1 = data.size();
 
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));

  		for (unsigned int i = 0; i < data.size(); i++)  {
    		
    		 double v = data.at(i).get();
      		
      		oss.write((char *) &v, sizeof(v));
    	}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ");	
	}
	
	
	void Parser::toXMLBase64(vector< vector<Temperature> > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 2;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) {
    		
    		 	double v = data.at(i).at(j).get();
      		
      			oss.write((char *) &v, sizeof(v));
    		}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector< vector< vector<Temperature> > > data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();

  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++){
    		
    		 		double v = data.at(i).at(j).at(k).get();
      		
      				oss.write((char *) &v, sizeof(v));
    			}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
	void Parser::toXMLBase64(vector<vector< vector< vector<Temperature> > > >data, const string &name, string &buf){
		stringstream oss(stringstream::out | stringstream::binary);
  		string encoded;
  		
		buf.append("<" + name + "> ");
		int ndim = 3;
  		int dim1 = data.size();
  		int dim2 = data.at(0).size();
  		int dim3 = data.at(0).at(0).size();
  		int dim4 = data.at(0).at(0).at(0).size();
  		
  		oss.write((char *)&ndim, sizeof(int));
 		oss.write((char *)&dim1, sizeof(dim1));
  		oss.write((char *)&dim2, sizeof(dim2));
  		oss.write((char *)&dim3, sizeof(dim3));
   		oss.write((char *)&dim4, sizeof(dim4));
  		for (unsigned int i = 0; i < data.size(); i++) 
    		for (unsigned int j = 0; j < data.at(0).size(); j++) 
    			for (unsigned int k = 0; k < data.at(0).at(0).size(); k++)
    				for (unsigned int l = 0; l < data.at(0).at(0).at(0).size(); l++){
    		
    		 			double v = data.at(i).at(j).at(k).at(l).get();
      		
      				oss.write((char *) &v, sizeof(v));
    				}

  		Base64 b64;
 	    b64.encode(oss.str(), encoded, false);
 	    buf.append(encoded);
		buf.append(" </" + name + "> ")
		;
	}
	
	
#ifndef Double_CPP
		
#define Double_CPP double	
		
#endif
	

	#define TRYREAD(_stream_, _value_) _stream_.read((char*) &(_value_), sizeof(_value_));	\
	if (_stream_.bad()) throw ConversionException("I/O error during read of " + name, tableName);
		
    
	vector<Temperature>& Parser::get1DTemperatureFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector<Temperature>& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		if (ndim != 1) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 1.", tableName);
			}
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		for (unsigned int i = 0; i < dim1; i++) {
			//iss.read((char*) &v, sizeof(v));
			TRYREAD(iss,v);
		
			attribute.push_back(Temperature(v));
			
		}
		return attribute;
	}
	
	 
	vector <vector<Temperature> >& Parser::get2DTemperatureFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector<Temperature> >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss,ndim);
		
		if (ndim != 2) {
			throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 2.", tableName);
		}
	
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector<Temperature> aux2;
		aux2.reserve(dim2);
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				//iss.read((char*) &v, sizeof(v));
				TRYREAD(iss,v);
		
				aux2.push_back(Temperature(v));
		
			}
			attribute.push_back(aux2);	
		}
		return attribute;
	}
		
 	
	vector <vector <vector<Temperature> > >& Parser::get3DTemperatureFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector <vector <vector<Temperature> > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 3) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 3.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector<Temperature> > aux2;
		vector<Temperature> aux3;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					//iss.read((char*) &v, sizeof(v));
					TRYREAD(iss,v);
		
					aux3.push_back(Temperature(v));
		
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	
	vector <vector <vector <vector<Temperature> > > >& Parser::get4DTemperatureFromBase64(const string &name, const string &tableName, const string &xmlDoc, vector< vector <vector <vector<Temperature> > > >& attribute)
	{
		string xmlField = Parser::getField(xmlDoc,name);
		if (xmlField.length() == 0) {
			throw ConversionException("Error: Field \"" + 
					name + "\": Invalid XML syntax", tableName);
		}
		
		string decodedString;
		Base64 b64;
		b64.decode(xmlField, decodedString);
		
		stringstream iss(stringstream::in | stringstream::binary);
		iss.str(decodedString);
		
		attribute.clear();
		
		unsigned int ndim = 0;
		//iss.read((char *)&ndim, sizeof(ndim));
		TRYREAD(iss, ndim);	
		if (ndim != 4) {
				throw ConversionException("Error while decoding Base64 representation of \"" + name + "\" : found " + Integer::toString(ndim) + " for the number of dimensions, expecting 4.", tableName);
		}
				
		unsigned int dim1 = 0;
		//iss.read((char *)&dim1, sizeof(dim1));
		TRYREAD(iss,dim1);
		
		unsigned int dim2 = 0;
		//iss.read((char *)&dim2, sizeof(dim2));
		TRYREAD(iss,dim2);
		
		unsigned int dim3 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		unsigned int dim4 = 0;
		//iss.read((char *)&dim2, sizeof(dim3));
		TRYREAD(iss,dim3);
		
		attribute.reserve(dim1);
		
		Double_CPP v;
		vector <vector <vector<Temperature> > > aux2;
		vector <vector<Temperature> > aux3;
		vector<Temperature> aux4;		
		aux2.reserve(dim2);
		aux3.reserve(dim3);
		aux4.reserve(dim4);
		
		for (unsigned int i = 0; i < dim1; i++) {
			aux2.clear();
			for (unsigned int j = 0; j < dim2; j++) {
				aux3.clear();
				for (unsigned int k = 0; k < dim3; k++) {
					aux4.clear();
					for (unsigned int l = 0; l < dim4; l++) {
					//iss.read((char*) &v, sizeof(v));
						TRYREAD(iss,v);
		
						aux4.push_back(Temperature(v));
		
					}
					aux3.push_back(aux4);
				}
				aux2.push_back(aux3);
			}
			attribute.push_back(aux2);	
		}
		return attribute;
		;
	}
	
	


 } // End namespace asdm
 
