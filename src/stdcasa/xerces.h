//# xerces.h: handle differences between Xerces-C++ version 2.0 and 3.0...
//# Copyright (C) 2014
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: $
#ifndef STDCASA_XERCES_
#define STDCASA_XERCES_
#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

namespace casa {
     namespace xerces {
#if XERCES_VERSION_MAJOR > 2
        typedef xercesc::DOMLSSerializer Writer;
        typedef xercesc::DOMLSParser Parser;
#if defined(DEFINE_CASA_XERCES_STATICS)
        static const XMLCh *fgDOMValidate = xercesc::XMLUni::fgDOMValidate;
#endif
        inline Writer *create_writer(void *impl) { return static_cast<xercesc::DOMImplementationLS*>(impl)->createLSSerializer( ); }
        inline Parser *create_parser(void *impl) { return static_cast<xercesc::DOMImplementationLS*>(impl)->createLSParser(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS,0 ); }
        inline XMLCh *write_to_string( Writer *writer, xercesc::DOMNode *n ) { return writer->writeToString(n); }
        inline xercesc::DOMDocument *dom_parse( Parser *parser, const xercesc::Wrapper4InputSource &source ) { return parser->parse(&source); }
        inline bool write_to_file( Writer *writer, xercesc::DOMNode *n, std::string file, const XMLCh * const encoding ) { 
            xercesc::DOMImplementation* impl=xercesc::DOMImplementationRegistry::getDOMImplementation(xercesc::XMLString::transcode("LS"));
            xercesc::DOMLSOutput *sink = static_cast<xercesc::DOMImplementationLS*>(impl)->createLSOutput();
            xercesc::LocalFileFormatTarget fileTarget(file.c_str());
            sink->setEncoding(encoding);
            sink->setByteStream(&fileTarget);
            bool result = writer->write(n,sink);
            sink->release( );
            return result;
        }
        template<typename T>
        inline bool can_set_parameter( Parser *parser, const XMLCh *name, T value ) { return parser->getDomConfig()->canSetParameter(name,value); }
        template<typename T>
        inline void set_parameter( Parser *parser, const XMLCh *name, T value ) { return parser->getDomConfig()->setParameter(name,value); }
#else
        typedef xercesc::DOMWriter Writer;
        typedef xercesc::DOMBuilder Parser;
#if defined(DEFINE_CASA_XERCES_STATICS)
        static const XMLCh *fgDOMValidate = xercesc::XMLUni::fgDOMValidation;
#endif
        inline Writer *create_writer(void *impl) { return static_cast<xercesc::DOMImplementationLS*>(impl)->createDOMWriter( ); }
        inline Parser *create_parser(void *impl) { return static_cast<xercesc::DOMImplementationLS*>(impl)->createDOMBuilder(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0); }
        inline XMLCh *write_to_string( Writer *writer, xercesc::DOMNode *n ) { return writer->writeToString(*n); }
        inline xercesc::DOMDocument *dom_parse( Parser *parser, const xercesc::Wrapper4InputSource &source ) { return parser->parse(source); }
        inline bool write_to_file( Writer *writer, xercesc::DOMNode *n, std::string file, const XMLCh * const encoding ) { 
            xercesc::LocalFileFormatTarget fileTarget(file.c_str());
            writer->setEncoding(encoding);
            return writer->writeNode(&fileTarget,*n);
        }
        template<typename T>
        inline bool can_set_parameter( Parser *parser, const XMLCh *name, T value ) { return parser->canSetFeature(name,value); }
        template<typename T>
        inline void set_parameter( Parser *parser, const XMLCh *name, T value ) { return parser->setFeature(name,value); }
#endif
    }
}
#endif
