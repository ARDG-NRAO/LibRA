//# ImageMomentsProgress.cc:  Class to display progress meter for ImageMoments
//# Copyright (C) 1995,1996,1997,1999
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
//# $Id: ImageMomentsProgress.cc 18093 2004-11-30 17:51:10Z ddebonis $
//


#include <imageanalysis/ImageAnalysis/ImageMomentsProgress.h>
#include <imageanalysis/ImageAnalysis/ImageMomentsProgressMonitor.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/BasicSL/String.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

ImageMomentsProgress::~ImageMomentsProgress()
{
    delete itsMeter;
}

void ImageMomentsProgress::initDerived()
{
    delete itsMeter;
    itsMeter = new ProgressMeter(0.0, Double(expectedNsteps()), String("Compute Moments"),
                                 String("Vectors extracted"), String(""), String(""),
                                 true, max(1,Int(expectedNsteps()/20)));
    maxReported = false;
}

void ImageMomentsProgress::nstepsDone (uInt nsteps)
{
    itsMeter->update (nsteps);
    if ( progressMonitor != NULL ){
    	if ( !maxReported ){
    		int max = static_cast<int>(itsMeter->max());
    		progressMonitor->setStepCount( max );
    		maxReported = true;
    	}
    	progressMonitor->setStepsCompleted( nsteps );
    }
}

void ImageMomentsProgress::setProgressMonitor( ImageMomentsProgressMonitor* monitor ){
	progressMonitor = monitor;

}

void ImageMomentsProgress::done()
{   
	if ( progressMonitor != NULL ){
		progressMonitor->done();
	}
    delete itsMeter;
    itsMeter = 0;
}
 
 

} //# NAMESPACE CASA - END

