//# ImagerMultiMS.h: Imager functionality sits here; 
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//#
//# $Id$


#ifndef SYNTHESIS_IMAGERMULTIMS_H
#define SYNTHESIS_IMAGERMULTIMS_H

#include <synthesis/MeasurementEquations/Imager.h>

namespace casa {

  class ImagerMultiMS : public Imager
    {

    public: 
      // Default constructor with no ms                                        

      ImagerMultiMS();
      //Copy the sub ms to memory useful when imaging only 
      //a few channels 
      virtual casacore::Bool setDataToMemory(const casacore::String& msname, const casacore::String& mode, 
				   const casacore::Vector<casacore::Int>& nchan, 
				   const casacore::Vector<casacore::Int>& start,
				   const casacore::Vector<casacore::Int>& step,
				   const casacore::Vector<casacore::Int>& spectralwindowids,
				   const casacore::Vector<casacore::Int>& fieldids,
				   const casacore::String& msSelect,
				   const casacore::String& timerng,
				   const casacore::String& fieldnames,
				   const casacore::Vector<casacore::Int>& antIndex,
				   const casacore::String& antnames,
				   const casacore::String& spwstring,
				   const casacore::String& uvdist,
				   const casacore::String& scan,
                                   const casacore::String& intent="", 
                                   const casacore::String& obs="");  // select by obs ID
      // Set the data selection on for each ms seperately
      virtual  casacore::Bool setDataPerMS(const casacore::String& msname, const casacore::String& mode, 
				 const casacore::Vector<casacore::Int>& nchan, 
				 const casacore::Vector<casacore::Int>& start,
				 const casacore::Vector<casacore::Int>& step,
				 const casacore::Vector<casacore::Int>& spectralwindowids,
				 const casacore::Vector<casacore::Int>& fieldids,
				 const casacore::String& msSelect="",
				 const casacore::String& timerng="",
				 const casacore::String& fieldnames="",
				 const casacore::Vector<casacore::Int>& antIndex=casacore::Vector<casacore::Int>(),
				 const casacore::String& antnames="",
				 const casacore::String& spwstring="",
				 const casacore::String& uvdist="",
                                 const casacore::String& scan="",
                                 const casacore::String& intent="",
                                 const casacore::String& obs="",
                                 const casacore::Bool useModel=false,
				 const casacore::Bool msreadonly=false);


       // Set image construction parameters
      virtual casacore::Bool setimage(const casacore::Int nx, const casacore::Int ny,
		const casacore::Quantity& cellx, const casacore::Quantity& celly,
		const casacore::String& stokes,
                casacore::Bool doShift,
		const casacore::MDirection& phaseCenter, 
                const casacore::Quantity& shiftx, const casacore::Quantity& shifty,
		const casacore::String& mode, const casacore::Int nchan,
                const casacore::Int start, const casacore::Int step,
		const casacore::MRadialVelocity& mStart, const casacore::MRadialVelocity& mStep,
		const casacore::Vector<casacore::Int>& spectralwindowids, const casacore::Int fieldid,
		const casacore::Int facets, const casacore::Quantity& distance);
  

      casacore::Bool selectDataChannel();
      
      // Lock the ms and its subtables
      virtual casacore::Bool lock();
      
      // Unlock the ms and its subtables
      virtual casacore::Bool unlock();

      //open sub tables
      virtual casacore::Bool openSubTables();
      
      // @copydoc Imager::mapExtent()
      virtual casacore::Bool mapExtent(const casacore::String &referenceFrame, const casacore::String &movingSource,
              const casacore::String &pointingColumn, casacore::Vector<casacore::Double> &center, casacore::Vector<casacore::Double> &blc,
              casacore::Vector<casacore::Double> &trc, casacore::Vector<casacore::Double> &extent);

    protected:
      
      casacore::Block<casacore::Vector<casacore::Int> > blockNChan_p;
      casacore::Block<casacore::Vector<casacore::Int> > blockStart_p;
      casacore::Block<casacore::Vector<casacore::Int> > blockStep_p;
      casacore::Block<casacore::Vector<casacore::Int> > blockSpw_p;
      casacore::Block<casacore::MeasurementSet> blockMSSel_p;
      casacore::Bool setDataOnThisMS(casacore::MeasurementSet& ms, const casacore::String& mode="none", 
			   const casacore::Vector<casacore::Int>& nchan=casacore::Vector<casacore::Int>(0), 
			   const casacore::Vector<casacore::Int>& start=casacore::Vector<casacore::Int>(1,0),
			   const casacore::Vector<casacore::Int>& step=casacore::Vector<casacore::Int>(1,1),
			   const casacore::Vector<casacore::Int>& spectralwindowids=casacore::Vector<casacore::Int>(0),
			   const casacore::Vector<casacore::Int>& fieldids=casacore::Vector<casacore::Int>(0),
			   const casacore::String& msSelect="",
			   const casacore::String& timerng="",
			   const casacore::String& fieldnames="",
			   const casacore::Vector<casacore::Int>& antIndex=casacore::Vector<casacore::Int>(),
			   const casacore::String& antnames="",
			   const casacore::String& spwstring="",
			   const casacore::String& uvdist="",
			   const casacore::String& scan="",
                           const casacore::String& intent="",
                           const casacore::String& obs="");
      casacore::Bool dataSet_p;

      
    };
  
} //# NAMESPACE CASA - END


#endif


