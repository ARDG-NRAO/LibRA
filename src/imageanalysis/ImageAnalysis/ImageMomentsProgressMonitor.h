//# ImageMomentsProgressMonitor.h: Interface for classes wanting to monitor
//# the progress of ImageMoments
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

#ifndef IMAGES_IMAGEMOMENTSPROGRESSMONITOR_H
#define IMAGES_IMAGEMOMENTSPROGRESSMONITOR_H

namespace casa { //# NAMESPACE CASA - BEGIN

//This is just an interface class for monitoring the progress of collapsing
//and image through calculating a moment.

class ImageMomentsProgressMonitor {
	public:
		virtual void setStepCount( int count ) = 0;
		virtual void setStepsCompleted( int count ) = 0;
		virtual void done() = 0;
		virtual ~ImageMomentsProgressMonitor(){}
};


} //# NAMESPACE CASA - END

#endif
