
//# RFASelector.cc: this defines RFASelector
//# Copyright (C) 2000,2001,2002
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
//# $Id$
#include <casa/Exceptions/Error.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/MaskArrMath.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Logging/LogIO.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <flagging/Flagging/RFASelector.h>
#include <casa/stdio.h>

#include <iomanip>

namespace casa { //# NAMESPACE CASA - BEGIN
        
// -----------------------------------------------------------------------
// reformRange
// Reforms an array of 2N elements into a [2,N] matrix
// -----------------------------------------------------------------------
template<class T> casacore::Bool RFASelector::reformRange( casacore::Matrix<T> &rng,const casacore::Array<T> &arr )
{
  if( arr.ndim()>2 || (arr.nelements()%2) !=0 )
    return false;
  rng = arr.reform(casacore::IPosition(2,2,arr.nelements()/2));
  return true;
}

template<class T> casacore::Array<T> fieldToArray( const casacore::RecordInterface &parm,const casacore::String &id );

/*
template<> casacore::Array<casacore::Int> fieldToArray<casacore::Int>( const casacore::RecordInterface &parm,const casacore::String &id )
{ return parm.toArrayInt(id); }
template<> casacore::Array<casacore::Double> fieldToArray<casacore::Double>( const casacore::RecordInterface &parm,const casacore::String &id )
{ return parm.toArrayDouble(id); }
template<> casacore::Array<casacore::String> fieldToArray<casacore::String>( const casacore::RecordInterface &parm,const casacore::String &id )
{ return parm.toArrayString(id); }
*/

// -----------------------------------------------------------------------
// RFASelector::parseRange
// Returns a record field of 2N elements as a [2,N] matrix
// -----------------------------------------------------------------------
template<class T> casacore::Bool RFASelector::parseRange( casacore::Matrix<T> &rng,const casacore::RecordInterface &parm,const casacore::String &id )
{
  if( isFieldSet(parm,id) )
  {
    try 
    {
      casacore::Array<T> arr( fieldToArray<T>(parm,id) );
      if( !reformRange(rng,arr) )
        throw( casacore::AipsError("") );
      return true;
    } 
    catch( casacore::AipsError x ) 
    {
      os<<"Illegal \""<<id<<"\" array\n"<<casacore::LogIO::EXCEPTION;
    }
  }
  return false;
}

/*
template casacore::Bool RFASelector::reformRange<casacore::Int>( casacore::Matrix<casacore::Int>&,const casacore::Array<casacore::Int>& );
template casacore::Bool RFASelector::reformRange<casacore::Double>( casacore::Matrix<casacore::Double>&,const casacore::Array<casacore::Double>& );
template casacore::Bool RFASelector::reformRange<casacore::String>( casacore::Matrix<casacore::String>&,const casacore::Array<casacore::String>& );
template casacore::Bool RFASelector::parseRange<casacore::Int>( casacore::Matrix<casacore::Int>&,const casacore::RecordInterface&,const casacore::String&);
template casacore::Bool RFASelector::parseRange<casacore::Double>( casacore::Matrix<casacore::Double>&,const casacore::RecordInterface&,const casacore::String&);
template casacore::Bool RFASelector::parseRange<casacore::String>( casacore::Matrix<casacore::String>&,const casacore::RecordInterface&,const casacore::String&);
*/

// -----------------------------------------------------------------------
// RFA_selector::find
// Helper templated method to find an object in an array
// -----------------------------------------------------------------------
template<class T> casacore::Bool RFASelector::find( casacore::uInt &index,const T &obj,const casacore::Vector<T> &arr )
{
  for( casacore::uInt i=0; i<arr.nelements(); i++ )
    if( obj == arr(i) )
    {
      index = i;
      return true;
    }
  return false;
}
/*
template casacore::Bool RFASelector::find<casacore::uInt>(casacore::uInt&,const casacore::uInt&,const casacore::Vector<casacore::uInt>&);
template casacore::Bool RFASelector::find<casacore::Int>(casacore::uInt&,const casacore::Int&,const casacore::Vector<casacore::Int>&);
template casacore::Bool RFASelector::find<casacore::String>(casacore::uInt&,const casacore::String&,const casacore::Vector<casacore::String>&);
*/
} //# NAMESPACE CASA - END

