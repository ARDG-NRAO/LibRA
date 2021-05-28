//# SIMapper.h: Imager functionality sits here; 
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

#ifndef SYNTHESIS_SIMAPPER_H
#define SYNTHESIS_SIMAPPER_H

#include <casa/aips.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines2/FTMachine.h>

namespace casacore{

template<class T> class ImageInterface;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
  class ComponentFTMachine;
  namespace refim{class ComponentFTMachine;}
  class SkyJones;

// <summary> Class that contains functions needed for imager </summary>

  class SIMapper// : public SIMapperBase
{
 public:
  // Default constructor

  SIMapper( casacore::CountedPtr<SIImageStore>& imagestore,
            casacore::CountedPtr<FTMachine>& ftm, 
	    casacore::CountedPtr<FTMachine>& iftm);

  ///Vi2/VisBuffer2 constructor
   SIMapper( casacore::CountedPtr<SIImageStore>& imagestore,
	     casacore::CountedPtr<refim::FTMachine>& ftm, 
	     casacore::CountedPtr<refim::FTMachine>& iftm);

  SIMapper(const ComponentList& cl, 
	   casacore::String& whichMachine);
  virtual ~SIMapper();

  ///// Major Cycle Functions
  virtual void initializeGrid(vi::VisBuffer2& vb, casacore::Bool dopsf, casacore::Bool firstaccess=false);
  virtual void grid(vi::VisBuffer2& vb, casacore::Bool dopsf, refim::FTMachine::Type col, const casacore::Int whichFTM=-1);
  virtual void finalizeGrid(vi::VisBuffer2& vb, casacore::Bool dopsf);
  virtual void initializeDegrid(vi::VisBuffer2& vb, casacore::Int row=-1);
  virtual void degrid(vi::VisBuffer2& vb);
  virtual void addPB(vi::VisBuffer2& vb, PBMath& pbMath, const casacore::MDirection& altDir=MDirection(), const casacore::Bool useAltDir=False);
  /////////////////////// OLD VI/VB versions
  virtual void initializeGrid(VisBuffer& vb, casacore::Bool dopsf, casacore::Bool firstaccess=false);
  virtual void grid(VisBuffer& vb, casacore::Bool dopsf, FTMachine::Type col, const casacore::Int whichFTM=-1);
  virtual void finalizeGrid(VisBuffer& vb, casacore::Bool dopsf);
  virtual void initializeDegrid(VisBuffer& vb, casacore::Int row=-1);
  virtual void degrid(VisBuffer& vb);
  virtual void addPB(VisBuffer& vb, PBMath& pbMath);

  virtual void finalizeDegrid();

  //////////////the return value is false if no valid record is being returned
  casacore::Bool getCLRecord(casacore::Record& rec);
  casacore::Bool getFTMRecord(casacore::Record& rec, const casacore::String diskimage="");

  virtual casacore::String getImageName(){return itsImages->getName();};
  virtual casacore::CountedPtr<SIImageStore> imageStore(){return itsImages;};
  virtual casacore::Bool releaseImageLocks(){return itsImages->releaseLocks();};

  const casacore::CountedPtr<FTMachine>& getFTM(const casacore::Bool ift=true) {if (ift) return ift_p; else return ft_p;};
  const casacore::CountedPtr<refim::FTMachine>& getFTM2(const casacore::Bool ift=true) {if (ift) return ift2_p; else return ft2_p;};

  
  virtual void initPB();
  

protected:

  casacore::CountedPtr<FTMachine> ft_p, ift_p; 
  casacore::CountedPtr<refim::FTMachine> ft2_p, ift2_p; 
  casacore::CountedPtr<ComponentFTMachine> cft_p;
  casacore::CountedPtr<refim::ComponentFTMachine> cft2_p;
  ComponentList cl_p;
  casacore::Bool useViVb2_p;
  casacore::CountedPtr<SIImageStore> itsImages;

};


} //# NAMESPACE CASA - END

#endif
