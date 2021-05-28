//# ImageTypedefs.h
//# Copyright (C) 1998,1999,2000,2001
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

#ifndef IMAGETYPEDEFS_H_
#define IMAGETYPEDEFS_H_

#include <casa/aipstype.h>
#include <casa/BasicSL/Complexfwd.h>
#include <casa/Utilities/CountedPtr.h>

#define SPIIT std::shared_ptr<casacore::ImageInterface<T>>
#define SPCIIT std::shared_ptr<const casacore::ImageInterface<T>>

#define SPIIU std::shared_ptr<casacore::ImageInterface<U>>
#define SPCIIU std::shared_ptr<const casacore::ImageInterface<U>>

#define SPIICT std::shared_ptr<casacore::ImageInterface<ComplexType>>
#define SPIIRT std::shared_ptr<casacore::ImageInterface<RealType>>

namespace casacore{

	template<class T> class ImageInterface;
}

namespace casa {

    using SPCIIF = std::shared_ptr<const casacore::ImageInterface<casacore::Float> >;
	using SPIIF = std::shared_ptr<casacore::ImageInterface<casacore::Float> >;
	using SPCIIC = std::shared_ptr<const casacore::ImageInterface<casacore::Complex> >;
	using SPIIC = std::shared_ptr<casacore::ImageInterface<casacore::Complex> >;
	using SPCIID = std::shared_ptr<const casacore::ImageInterface<casacore::Double> >;
	using SPIID = std::shared_ptr<casacore::ImageInterface<casacore::Double> >;
	using SPCIIDC = std::shared_ptr<const casacore::ImageInterface<casacore::DComplex> >;
	using SPIIDC = std::shared_ptr<casacore::ImageInterface<casacore::DComplex> >;
	using ITUPLE = std::tuple<SPIIF, SPIIC, SPIID, SPIIDC>;
}

#endif
