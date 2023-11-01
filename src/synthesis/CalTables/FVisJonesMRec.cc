//# SolvableVJMRec.cc: Implementation of SolvableVJMRec.h
//# Copyright (C) 1996,1997,1998,2000,2003
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
//----------------------------------------------------------------------------

#include <synthesis/CalTables/FVisJonesMRec.h>
#include <msvis/MSVis/MSCalEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

#define AddField(Class,Type,Field,Name) \
  void Class::define##Field (const Type& Field) { \
    Record newRec; \
    newRec.define (MSC::fieldName (MSC::Name), Field); \
    addRec (newRec); \
  } \
  void Class::get##Field (Type& Field) { \
    record().get (MSC::fieldName (MSC::Name), Field); \
  }
  
FVisJonesMRec::FVisJonesMRec() : TimeVarVisJonesMRec()
{
};

FVisJonesMRec::FVisJonesMRec (const Record& inpRec) : 
  TimeVarVisJonesMRec (inpRec)
{
};

AddField(FVisJonesMRec,Float,RotMeas,ROT_MEASURE);
AddField(FVisJonesMRec,Float,RotMeasErr,ROT_MEASURE_ERROR);

FVisJonesIonoMRec::FVisJonesIonoMRec() : FVisJonesMRec()
{
};

FVisJonesIonoMRec::FVisJonesIonoMRec (const Record& inpRec) : 
  FVisJonesMRec (inpRec)
{
};

AddField(FVisJonesIonoMRec,Float,TEC,IONOSPH_TEC);
AddField(FVisJonesIonoMRec,Float,TECErr,IONOSPH_TEC_ERROR);

} //# NAMESPACE CASA - END

