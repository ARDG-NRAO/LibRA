//# ComponentImager.h: this defines ComponentImager, which ...
//# Copyright (C) 1999,2000,2003
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
//#
//# $Id: ComponentImager.h 20299 2008-04-03 05:56:44Z gervandiepen $
//#! ========================================================================
//#!                Attention!  Programmers read this!
//#!
//#! This file is a template to guide you in creating a header file
//#! for your new class.   By following this template, you will create
//#! a permanent reference document for your class, suitable for both
//#! the novice client programmer, the seasoned veteran, and anyone in 
//#! between.  It is essential that you write the documentation portions 
//#! of this file with as much care as you do the source code.
//#!
//#! If you are unfamilar with the AIPS++ header style please refer to
//#! template-class-h.
//#!
//#!                         Replacement Tokens
//#!                         ------------------
//#!
//#! These are character strings enclosed in angle brackets, on a commented
//#! line.  Two are found on the first line of this file:
//#!
//#!   <ClassFileName.h> <ClassName>
//#!
//#! You should remove the angle brackets, and replace the characters within
//#! the brackets with names specific to your class.  Mimic the capitalization
//#! and punctuation of the original.  For example, you would change
//#!
//#!   <ClassFileName.h>  to   LatticeIter.h
//#!   <ClassName>        to   LatticeIterator
//#!
//#! Another replacement token will be found in the "include guard" just
//#! a few lines below.
//#!
//#!  #define <AIPS_CLASSFILENAME_H>  to  #define AIPS_LATTICEITER_H
//#!

#ifndef IMAGES_COMPONENTIMAGER_H
#define IMAGES_COMPONENTIMAGER_H

#include <casacore/casa/aips.h>

#include <imageanalysis/ImageAnalysis/ImageTask.h>
#include <components/ComponentModels/ComponentList.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

#include <memory>

namespace casacore{

template <class T> class ImageInterface;
class Unit;
class LogIO;
template <class T> class MeasRef;
class MDirection;
class MVAngle;
class MVFrequency;
class MFrequency;

}

namespace casa {


// <summary>
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> ImageInterface
//   <li> ComponentList
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// Project the componentlist onto the image.  If any of the coordinate
// transformations for a particular pixel fail (e.g. coordinate system
// is not defined for that pixel) if the image has a writable mask, then those 
// pixels will be masked, otherwise they are just zeroed.  Any pixels
// that are already masked mask=F) will not have their values changed
// (perhaps this behaviour should be changed).
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>

class ComponentImager: public ImageTask<casacore::Float> {
public:

	ComponentImager() = delete;

	ComponentImager(
		const SPIIF image, const casacore::Record *const &regionPtr,
	    const casacore::String& maskInp
	);

	~ComponentImager();

	void setComponentList(const ComponentList& list) {
		_list = list;
	}

	void setSubtract(casacore::Bool b) { _subtract = b; }

	casacore::String getClass() const { return "ComponentImager"; }

	void modify(casacore::Bool verbose);

	// Project the componentlist onto the image.  If any of the coordinate
	// transformations for a particular pixel fail (e.g. coordinate system
	// is not defined for that pixel) if the image has a writable mask, then those
	// pixels will be masked, otherwise they are just zeroed.  Any pixels
	// that are already masked mask=F) will not have their values changed
	// (perhaps this behaviour should be changed).
	static void project(
		casacore::ImageInterface<casacore::Float>& image,
		const ComponentList& list
	);

private:
	// we cannot use pointer stored in base class because that's const
	SPIIF _image;

	ComponentList _list;

	casacore::Bool _subtract = false;

   	CasacRegionManager::StokesControl _getStokesControl() const {
   		return CasacRegionManager::USE_ALL_STOKES;
   	}

    std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
    	return std::vector<casacore::Coordinate::Type>(1, casacore::Coordinate::DIRECTION);
    }

	static std::unique_ptr<ComponentList> _doPoints(
		casacore::ImageInterface<casacore::Float>& image, const ComponentList& list,
		int longAxis, int latAxis, const casacore::Unit& fluxUnits,
		const casacore::MeasRef<casacore::MDirection>& dirRef, const casacore::MVAngle& pixelLatSize,
		const casacore::MVAngle& pixelLongSize, const casacore::Vector<casacore::MVFrequency>& freqValues,
		const casacore::MeasRef<casacore::MFrequency>& freqRef, casacore::Int freqAxis, casacore::Int polAxis, casacore::uInt nStokes
	);

};

}

#endif


