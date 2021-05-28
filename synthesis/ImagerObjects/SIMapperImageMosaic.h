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

#ifndef SYNTHESIS_SIMAPPERMOSAIC_H
#define SYNTHESIS_SIMAPPERMOSAIC_H

#include <casa/aips.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>
#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/ImagerObjects/SIMapper.h>

namespace casacore{

template<class T> class ImageInterface;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
  class ComponentFTMachine;
  namespace refim{class ComponentFTMachine;}
  class SkyJones;

// <summary> Class that contains functions needed for imager </summary>

  class SIMapperImageMosaic : public SIMapper
{
 public:
  // Default constructor

  SIMapperImageMosaic( casacore::CountedPtr<SIImageStore>& imagestore,
            casacore::CountedPtr<FTMachine>& ftm, 
		       casacore::CountedPtr<FTMachine>& iftm);
  //	    casacore::CountedPtr<VPSkyJones>& vp);
   SIMapperImageMosaic( casacore::CountedPtr<SIImageStore>& imagestore,
			casacore::CountedPtr<refim::FTMachine>& ftm, 
			casacore::CountedPtr<refim::FTMachine>& iftm);
  SIMapperImageMosaic(const ComponentList& cl, 
		      casacore::String& whichMachine);
  //	   casacore::CountedPtr<VPSkyJones>& vp);
  virtual ~SIMapperImageMosaic();

  ///// Major Cycle Functions
  void initializeGrid(vi::VisBuffer2& vb, casacore::Bool dopsf, casacore::Bool firstaccess=false);
  void grid(vi::VisBuffer2& vb, casacore::Bool dopsf, refim::FTMachine::Type col);
  void finalizeGrid(vi::VisBuffer2& vb, casacore::Bool dopsf);
  void initializeDegrid(vi::VisBuffer2& vb, casacore::Int row=-1);
  void degrid(vi::VisBuffer2& vb);
  /////////////////////// OLD VI/VB versions
  void initializeGrid(VisBuffer& vb, casacore::Bool dopsf, casacore::Bool firstaccess=false);
  void grid(VisBuffer& vb, casacore::Bool dopsf, FTMachine::Type col);
  void finalizeGrid(VisBuffer& vb, casacore::Bool dopsf);
  void initializeDegrid(VisBuffer& vb, casacore::Int row=-1);
  void degrid(VisBuffer& vb);

  //////////////the return value is false if no valid record is being returned
  //  casacore::Bool getCLRecord(casacore::Record& rec);
  //  casacore::Bool getFTMRecord(casacore::Record& rec);

  virtual casacore::String getImageName(){return itsImages->getName();};
  virtual casacore::CountedPtr<SIImageStore> imageStore(){return itsImages;};
  virtual casacore::Bool releaseImageLocks(){return itsImages->releaseLocks();};

protected:
  //  casacore::Bool changedSkyJonesLogic(const vi::VisBuffer2& vb, casacore::Bool& firstRow, casacore::Bool& internalRow, const casacore::Bool grid=true);
  //////////////OLD vb version
  //  casacore::Bool changedSkyJonesLogic(const VisBuffer& vb, casacore::Bool& firstRow, casacore::Bool& internalRow, const casacore::Bool grid=true);
  ////////////////////////////////////////////

  ComponentList clCorrupted_p;
  //  casacore::CountedPtr<VPSkyJones>  ejgrid_p, ejdegrid_p;
  VisBuffer ovb_p;
  vi::VisBuffer2* vb_p;

  casacore::Bool firstaccess_p;

};


} //# NAMESPACE CASA - END

#endif
