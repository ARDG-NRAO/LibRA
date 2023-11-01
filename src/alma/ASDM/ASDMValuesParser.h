#ifndef ASDMVALUESPARSER_H
#define ASDMVALUESPARSER_H
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
 * File ASDMValuesParser.h
 */
#include <iostream>
#include <vector>
#include <sstream>

#ifndef WITHOUT_BOOST
// regex not currently used here, in commented out code
//#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>
#else
//#include <regex>
#include <alma/ASDM/Misc.h>
#endif


namespace asdm {
  /**
   * A class to represent an exception thrown during the parsing of the representation 
   * of a basic type value , scalar or array, in the XML representation of an ASDM table.
   */ 
  class ASDMValuesParserException {
    
  public:
    /**
     * An empty contructor.
     */
    ASDMValuesParserException();

    /**
     * A constructor with a message associated with the exception.
     * @param m a string containing the message.
     */
    ASDMValuesParserException(const std::string& m);
    
    /**
     * The destructor.
     */
    virtual ~ASDMValuesParserException();
    
    /**
     * Returns the message associated to this exception.
     * @return a string.
     */
    std::string getMessage() const;
    
  protected:
    std::string message;
    
  };
  
  inline ASDMValuesParserException::ASDMValuesParserException() : message ("ASDMValuesParserException") {}
  inline ASDMValuesParserException::ASDMValuesParserException(const std::string& m) : message(m) {}
  inline ASDMValuesParserException::~ASDMValuesParserException() {}
  inline std::string ASDMValuesParserException::getMessage() const {
    return "ASDMValuesParserException : " + message;
  }
  
  class ASDMValuesParser {
  private:
    static std::istringstream iss;
    static std::ostringstream oss;

  public:
    template<class T> 
      static void READ(T& v) {
      char c;
      iss >> v; if (iss.fail() || (iss.get(c) && c != ' ')) {  
	oss.str("");						
	oss << "Error while reading the string to be parsed : '" << iss.str() << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      iss.putback(c);
    }

    template<class T>
      static T parse(const std::string& s) {
      T result;
      iss.clear();
      iss.str(s);
      READ(result);
      return result;
    }

    template<class T>
      static std::vector<T> parse1D(const std::string& s) {
      int ndim;
      int nvalue;

      iss.clear();
      iss.str(s);
      READ(ndim);
      if (ndim != 1) {
	oss.str("");
	oss << "The first field of a 1D array representation should be '1', I found '" << ndim << "' in '" << s << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      
      READ(nvalue);
      if (nvalue <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue << "'.";
	throw ASDMValuesParserException(oss.str());
      }

      std::vector<T> result(nvalue);
      T value;
      for ( int i = 0; i < nvalue; i++) {
	READ(value);
	result[i]=value;
      }
      
      return result;
    }

    template<class T>  
      static std::vector<std::vector<T> > parse2D(const std::string& s) {
      int ndim;
      int nvalue1;
      int nvalue2;

      iss.clear();
      iss.str(s);
      READ(ndim);
      if (ndim != 2) {
	oss.str("");
	oss << "The first field of a 2D array representation should be '2', I found '" << ndim << "' in '" << s << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      
      READ(nvalue1);
      if (nvalue1 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue1 << "'.";
	throw ASDMValuesParserException(oss.str());
      }

      READ(nvalue2);
      if (nvalue2 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue2 << "'.";
	throw ASDMValuesParserException(oss.str());
      }

      std::vector<std::vector<T> > result(nvalue1);
      T value;
      for ( int i = 0; i < nvalue1; i++) {
	std::vector<T> v(nvalue2);
	for ( int j = 0; j < nvalue2; j++) {
	  READ(value);
	  v[j] = value;
	}
	result[i] = v;
      }
      return result;
    }
 
    template<class T>
      static std::vector<std::vector<std::vector<T> > > parse3D(const std::string& s) {
      int ndim;
      int nvalue1;
      int nvalue2;
      int nvalue3;

      iss.clear();
      iss.str(s);

      READ(ndim);
      if (ndim != 3) {
	oss.str("");
	oss << "The first field of a 3D array representation should be '3', I found '" << ndim << "' in '" << s << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      
      READ(nvalue1);
      if (nvalue1 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue1 << "'.";
	throw ASDMValuesParserException(oss.str());
      }

      READ(nvalue2);
      if (nvalue2 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue2 << "'.";
	throw ASDMValuesParserException(oss.str());
      }

      READ(nvalue3);
      if (nvalue3 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue3 << "'.";
	throw ASDMValuesParserException(oss.str());
      }

      std::vector<std::vector<std::vector<T> > > result(nvalue1);
      T value;
      for ( int i = 0; i < nvalue1; i++) {
	std::vector<std::vector<T> >vv(nvalue2);
	for ( int j = 0; j < nvalue2; j++) {
	  std::vector<T> v(nvalue3);
	  for ( int k = 0; k < nvalue3; k++) {
	    READ(value);
	    v[k] = value;
	  }
	  vv[j] = v;
	}
	result[i] = vv;
      }
      return result;
    }  

    template<class T>
      static std::vector<std::vector<std::vector<std::vector<T> > > > parse4D(const std::string& s) {
      int ndim;
      int nvalue1;
      int nvalue2;
      int nvalue3;
      int nvalue4;
      
      iss.clear();
      iss.str(s);
      READ(ndim);
      if (ndim != 4) {
	oss.str("");
	oss << "The first field of a 3D array representation should be '4', I found '" << ndim << "' in '" << s << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      
      READ(nvalue1);
      if (nvalue1 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue1 << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      
      READ(nvalue2);
      if (nvalue2 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue2 << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      
      READ(nvalue3);
      if (nvalue3 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue3 << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      
      READ(nvalue4);
      if (nvalue4 <= 0) {
	oss.str("");
	oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue4 << "'.";
	throw ASDMValuesParserException(oss.str());
      }
      
      std::vector<std::vector<std::vector<std::vector<T> > > > result(nvalue1);
      T value;
      for ( int i = 0; i < nvalue1; i++) {
	std::vector<std::vector<std::vector<T> > > vvv(nvalue2);
	for ( int j = 0; j < nvalue2; j++) {
	  std::vector<std::vector<T> > vv(nvalue3);
	  for ( int k = 0; k < nvalue3; k++) {
	    std::vector<T> v(nvalue4);
	    for ( int l = 0; l < nvalue4; l++) {
	      READ(value);
	      v[l] = value;
	    }
	    vv[k] = v;
	  }
	  vvv[j] = vv;
	}
	result[i] = vvv;
      }  
      return result;
    }

    static std::string parse(const std::string& s);
    static std::vector<std::string> parse1D(const std::string& s);
    static std::vector<std::vector<std::string > > parse2D(const std::string& s);
    static std::vector<std::vector<std::vector<std::string > > > parse3D(const std::string& s);

    static std::vector<std::string> parseQuoted(const std::string& s);

  private:
    // this value is not currently used anywhere
    //#ifndef WITHOUT_BOOST
    //    static boost::regex quotedStringRegex;
    //#else
    //    static std::regex quotedStringRegex;
    //#endif

  };

  inline std::string ASDMValuesParser::parse(const  std::string& s) { return s; }
  inline std::vector<std::string> ASDMValuesParser::parse1D( const std::string& s) {
    int ndim;
    int nvalue;

    iss.clear();
    iss.str(s);
    READ(ndim);
    if (ndim != 1) {
      oss.str("");
      oss << "The first field of a 1D array representation should be '1', I found '" << ndim << "' in '" << s << "'.";
      throw ASDMValuesParserException(oss.str());
    }

    READ(nvalue);
    if (nvalue <= 0) {
      oss.str("");
      oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue << "'.";
      throw ASDMValuesParserException(oss.str());
    }
     
    std::string remains; getline(iss,remains); 
#ifndef WITHOUT_BOOST
    std::vector<std::string> result = parseQuoted(boost::trim_left_copy(remains));
#else
    std::vector<std::string> result = parseQuoted(asdm::ltrim_copy(remains));
#endif
    if (nvalue > (int) result.size()) {
      oss.str("");
      oss << "Error while reading the string to be parsed : '" << iss.str() << "'.";
      throw ASDMValuesParserException(oss.str());
    }
    return result;
  }

  inline std::vector<std::vector<std::string > > ASDMValuesParser::parse2D(const std::string& s) {
    int ndim;
    int nvalue1;
    int nvalue2;

    iss.clear();
    iss.str(s);
    READ(ndim);
    if (ndim != 2) {
      oss.str("");
      oss << "The first field of a 2D array representation should be '2', I found '" << ndim << "' in '" << s << "'.";
      throw ASDMValuesParserException(oss.str());
    }
      
    READ(nvalue1);
    if (nvalue1 <= 0) {
      oss.str("");
      oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue1 << "'.";
      throw ASDMValuesParserException(oss.str());
    }

    READ(nvalue2);
    if (nvalue2 <= 0) {
      oss.str("");
      oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue2 << "'.";
      throw ASDMValuesParserException(oss.str());
    }

    std::string remains; getline(iss,remains); 
#ifndef WITHOUT_BOOST
    std::vector<std::string> v_s = parseQuoted(boost::trim_left_copy(remains));
#else
    std::vector<std::string> v_s = parseQuoted(asdm::ltrim_copy(remains));
#endif
    if (nvalue1 * nvalue2 > (int) v_s.size()) {
      oss.str("");
      oss << "Error while reading the string to be parsed : '" << iss.str() << "'.";
      throw ASDMValuesParserException(oss.str());
    }
      
    std::vector<std::vector<std::string> > result(nvalue1);
    int start = 0;
    for (unsigned int i = 0; i < result.size(); i++) {
      start = i*nvalue2;
      result[i].assign(v_s.begin()+start, v_s.begin()+start+nvalue2);
    }
    return result;
  }

  inline std::vector<std::vector<std::vector<std::string > > > ASDMValuesParser::parse3D(const std::string& s) {
    int ndim;
    int nvalue1;
    int nvalue2;
    int nvalue3;

    iss.clear();
    iss.str(s);
    READ(ndim);
    if (ndim != 3) {
      oss.str("");
      oss << "The first field of a 2D array representation should be '2', I found '" << ndim << "' in '" << s << "'.";
      throw ASDMValuesParserException(oss.str());
    }
      
    READ(nvalue1);
    if (nvalue1 <= 0) {
      oss.str("");
      oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue1 << "'.";
      throw ASDMValuesParserException(oss.str());
    }

    READ(nvalue2);
    if (nvalue2 <= 0) {
      oss.str("");
      oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue2 << "'.";
      throw ASDMValuesParserException(oss.str());
    }

    READ(nvalue3);
    if (nvalue3 <= 0) {
      oss.str("");
      oss << "The number of values along one dimension of an array must be expressed by a strictly positive integer.I found '" << nvalue3 << "'.";
      throw ASDMValuesParserException(oss.str());
    }

    std::string remains; getline(iss,remains); 
#ifndef WITHOUT_BOOST
    std::vector<std::string> v_s = parseQuoted(boost::trim_left_copy(remains));
#else
    std::vector<std::string> v_s = parseQuoted(asdm::ltrim_copy(remains));
#endif
    if (nvalue1 * nvalue2 * nvalue3 > (int) v_s.size()) {
      oss.str("");
      oss << "Error while reading the string to be parsed : '" << iss.str() << "'.";
      throw ASDMValuesParserException(oss.str());
    }
      
    std::vector<std::vector<std::string> > plane(nvalue2);
    std::vector<std::vector<std::vector<std::string> > > result(nvalue1, plane);
    int start = 0;
    for (unsigned int i = 0; i < (unsigned int) nvalue1; i++) {
      for (unsigned int j = 0; j < (unsigned int) nvalue2; j++) {
	result[i][j].assign(v_s.begin()+start, v_s.begin()+start+nvalue3);
	start += nvalue3; 
      }
    }
    return result;
  }
  
  inline std::vector<std::string> ASDMValuesParser::parseQuoted(const std::string& s) {
#ifndef WITHOUT_BOOST
    std::string separator1("\\");// let quoted arguments escape themselves
    std::string separator2(" "); // split on spaces
    std::string separator3("\"");// let it have quoted arguments
    
    boost::escaped_list_separator<char> els(separator1,separator2,separator3);
    boost::tokenizer<boost::escaped_list_separator<char> > tok(s, els);
    std::vector<std::string> result(tok.begin(), tok.end());
#else
    // there is no c++ equivalent to the boost tokenizer, parse the start by character
    std::vector<std::string> result;

    // an empty string has no tokens
    if (s.empty()) {
      return result;
    }

    std::string token;
    std::string nullString;

    bool quoted, escaped;
    quoted = escaped = false;

    for (std::string::const_iterator it=s.begin(); it!=s.end(); ++it) {
      if (escaped) {
	token += *it;
	escaped = false;
      } else {
	if (*it=='\\') {
	  escaped = true;
	} else {
	  if (quoted) {
	    if (*it=='\"') {
	      quoted = false;
	    } else {
	      token += *it;
	    }
	  } else {
	    if (*it == ' ') {
	      result.push_back(token);
	      token = nullString;
	    } else {
	      if (*it=='\"') {
		quoted = true;
	      } else {
		token += *it;
	      }
	    }
	  }
	}
      }
    }
    result.push_back(token);
#endif
    return result;
  }  
} // End namespace asdm.
    
#endif
