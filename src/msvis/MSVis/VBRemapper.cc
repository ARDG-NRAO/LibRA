//# VBRemapper.cc: implementation for VisBuffer remapping objects.
//# Copyright (C) 2011
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

#include <msvis/MSVis/VBRemapper.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogIO.h>
#include <map>

using namespace casacore;
namespace casa {

VBRemapper::VBRemapper()// :
  //  inToOutMaps_p()
{
}

VBRemapper::VBRemapper(const std::map<VisBufferComponents::EnumType, std::map<Int, Int> >& inToOutMaps) :
  inToOutMaps_p(inToOutMaps)
{
}

// VBRemapper& VBRemapper::operator=(const VBRemapper &other)
// {
//   // trivial so far.
//   vi_p = other.vi_p;
//   return *this;
// }

Bool VBRemapper::remap(VisBuffer& vb, const Bool squawk) const
{
  LogIO os(LogOrigin("VBRemapper", "remap()"));
  Bool retval = true;

  for(std::map<VisBufferComponents::EnumType,
        std::map<Int, Int> >::const_iterator c = inToOutMaps_p.begin();
      c != inToOutMaps_p.end(); ++c){
    Bool colOK = true;

    switch(c->first){
    case VisBufferComponents::Ant1:
      colOK = remapVector(vb.antenna1(), c->second);
      break;
    case VisBufferComponents::Ant2:
      colOK = remapVector(vb.antenna2(), c->second);
      break;
    case VisBufferComponents::ArrayId:
      colOK = remapScalar(vb.arrayIdRef(), c->second);
      break;
    case VisBufferComponents::DataDescriptionId:
      colOK = remapScalar(vb.dataDescriptionIdRef(), c->second);
      break;
    case VisBufferComponents::Feed1:
      colOK = remapVector(vb.feed1(), c->second);
      break;
    case VisBufferComponents::Feed2:
      colOK = remapVector(vb.feed2(), c->second);
      break;
    case VisBufferComponents::FieldId:
      colOK = remapScalar(vb.fieldIdRef(), c->second);
      break;
    case VisBufferComponents::ObservationId:
      colOK = remapVector(vb.observationId(), c->second);
      break;
    case VisBufferComponents::ProcessorId:
      colOK = remapVector(vb.processorId(), c->second);
      break;
    case VisBufferComponents::Scan:
      colOK = remapVector(vb.scan(), c->second);
      break;
    case VisBufferComponents::SpW:
      colOK = remapScalar(vb.spectralWindow(), c->second);
      break;
    case VisBufferComponents::StateId:
      colOK = remapVector(vb.stateId(), c->second);
      break;
    default:
      if(squawk){
        os << LogIO::WARN
           << "Remapping " << asyncio::PrefetchColumns::columnName(c->first)
           << " is not supported."
           << LogIO::POST;
      }
      // Do NOT set colOK; it would repeat the message.
      retval = false;
    }
    if(!colOK){
      retval = false;
      if(squawk){
        os << LogIO::WARN
           << "Unrecognized value remapping " << asyncio::PrefetchColumns::columnName(c->first)
           << LogIO::POST;
      }
    }
  }
  return retval;
}

Bool VBRemapper::remapScalar(Int& colref, const std::map<Int, Int>& mapper) const
{
  Bool retval = true;
  const std::map<Int, Int>::const_iterator it(mapper.find(colref));

  if(it != mapper.end()){
    const Int middleman(it->second);

    colref = middleman;
  }
  else
    retval = false;
  return retval;
}

Bool VBRemapper::remapVector(Vector<Int>& col, const std::map<Int, Int>& mapper) const
{
  Bool retval = true;

  if(mapper.size() > 0){
    for(Int row = col.nelements(); row--;){
      const std::map<Int, Int>::const_iterator it(mapper.find(col[row]));

      if(it != mapper.end()){
        const Int middleman = it->second;

        col[row] = middleman;
      }
      else
        retval = false;
    }
  }
  else
    retval = false;

  return retval;
}

using namespace casacore;
} // end namespace casa
