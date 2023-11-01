//# ImageMomentsProgress.h: Progress meter for ImageMoments class
//# Copyright (C) 1996,1997,1999
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
//#        Internet email: aips2-requesemat@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id: ImageMomentsProgress.h 20299 2008-04-03 05:56:44Z gervandiepen $

#ifndef IMAGES_IMAGEMOMENTSPROGRESS_H
#define IMAGES_IMAGEMOMENTSPROGRESS_H


#include <casacore/casa/aips.h>
#include <casacore/lattices/LatticeMath/LatticeProgress.h>
#include <casacore/casa/System/ProgressMeter.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> Provides a progress meter for the ImageMoments class </summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto module=Lattices>casacore::LatticeProgress</linkto> 
// </prerequisite>
//
// <etymology>
// Display a progress meter for the ImageMoments class
// </etymology>
//
// <synopsis>
//   Progress meters can be displayed by the casacore::LatticeApply class 
//   which is used by ImageMoments in order to optimally iterate
//   through the image by lines.  To do this,  one must derive a 
//   class from LatticeProgress. casacore::LatticeApply calls methods declared 
//   in casacore::LatticeProgress and  implemented in the derived class.
// </synopsis>
//
// <motivation>
//  I like progress meters !
// </motivation>
//
// <todo asof="1998/01/10">
// </todo>
class ImageMomentsProgressMonitor;

class ImageMomentsProgress : public casacore::LatticeProgress
{
public:

// Constructor makes a null object
    ImageMomentsProgress() : itsMeter(0), progressMonitor(0) {};

// Destructor deletes the casacore::ProgressMeter pointer
    virtual ~ImageMomentsProgress();

// Initialize this object.  Here we create the ProgressMeter
// This function is called by the <src>init</src> in LatticeProgress
    virtual void initDerived();

// Tell the number of steps done so far.
    virtual void nstepsDone (casacore::uInt nsteps);

// The process has ended so clean things up.
    virtual void done();

    //Set an ImageMomentsProgressMonitor that wants to receive
    //progress updates
    void setProgressMonitor( ImageMomentsProgressMonitor* monitor );

private:
    casacore::ProgressMeter* itsMeter;
    ImageMomentsProgressMonitor* progressMonitor;
    bool maxReported;
};


} //# NAMESPACE CASA - END

#endif
