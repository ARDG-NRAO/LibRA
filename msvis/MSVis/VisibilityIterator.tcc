//# VisibilityIterator.tcc 
//# Copyright (C) 2009
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify
//# it under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or
//# (at your option) any later version.
//#
//# This library is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//# 
//# You should have received a copy of the GNU General Public License
//# along with this library; if not, write to the Free Software
//# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
#include <msvis/MSVis/VisibilityIterator.h>
#include <casa/Arrays/Cube.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  // helper function to swap the y and z axes of a Cube
  template<class T>
  void ROVisibilityIterator::swapyz(casacore::Cube<T>& out, const casacore::Cube<T>& in) const
  {
    casacore::IPosition inShape = in.shape();
    casacore::uInt nx = inShape(0), ny = inShape(2), nz = inShape(1);

    out.resize(nx, ny, nz);
    casacore::Bool deleteIn, deleteOut;
    const T* pin = in.getStorage(deleteIn);
    T* pout = out.getStorage(deleteOut);
    casacore::uInt i = 0, zOffset = 0;

    for(casacore::uInt iz = 0; iz < nz; iz++, zOffset += nx){
      casacore::Int yOffset = zOffset;

      for(casacore::uInt iy = 0; iy < ny; iy++, yOffset += nx * nz){
        for(casacore::uInt ix = 0; ix < nx; ix++)
          pout[i++] = pin[ix + yOffset];
      }
    }
    out.putStorage(pout,deleteOut);
    in.freeStorage(pin,deleteIn);
  }
} //# NAMESPACE CASA - END

  
