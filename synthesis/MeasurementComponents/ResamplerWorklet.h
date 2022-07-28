// -*- C++ -*-
//# ResamplerWorklet.h: Definition of the ResamplerWorklet class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
//# $Id$

#ifndef SYNTHESIS_RESAMPLERWORKLET_H
#define SYNTHESIS_RESAMPLERWORKLET_H

#include <synthesis/TransformMachines/VisibilityResamplerBase.h>
#include <synthesis/TransformMachines/VBStore.h>
#include <synthesis/Utilities/ThreadCoordinator.h>
#include <msvis/MSVis/AsynchronousTools.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Array.h>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogIO.h>
#include <sys/syscall.h>

using namespace casa::async;
using namespace std;
namespace casa { //# NAMESPACE CASA - BEGIN
  class ResamplerWorklet : public Thread 
  {
  public: 
    enum PGridderMode {DataToGrid=0, GridToData};
    ResamplerWorklet() {myGriddedDataDouble_p=NULL;myGriddedDataSingle_p=NULL;};
    virtual ~ResamplerWorklet(){terminate();}
    ResamplerWorklet& operator=(const ResamplerWorklet& other);

    void setID(const casacore::Int& id) {myID_p=id;}
    void initThread(casacore::Int& id, casacore::CountedPtr<ThreadCoordinator<casacore::Int> >& threadClerk,
		    VisibilityResamplerBase* resampler);
    void initToVis(VBStore* vbs, const casacore::Array<casacore::Complex>* skyFTGrid) ;
    void initToSky(VBStore* vbs,casacore::Array<casacore::DComplex>* griddedData, casacore::Matrix<casacore::Double>* sumwt) ;
    void initToSky(VBStore* vbs,casacore::Array<casacore::Complex>* griddedData, casacore::Matrix<casacore::Double>* sumwt) ;

    void init(casacore::Int& id, 
	      casacore::CountedPtr<ThreadCoordinator<casacore::Int> >& threadClerk,
	      VisibilityResamplerBase* resampler, VBStore* vbs,
	      casacore::Array<casacore::DComplex>* griddedData, casacore::Matrix<casacore::Double>* sumwt,
	      casacore::Array<casacore::Complex>* skyFTGrid=NULL);
    void init(casacore::Int& id, 
	      casacore::CountedPtr<ThreadCoordinator<casacore::Int> >& threadClerk,
	      VisibilityResamplerBase* resampler, VBStore* vbs,
	      casacore::Array<casacore::Complex>* griddedData, casacore::Matrix<casacore::Double>* sumwt,
	      casacore::Array<casacore::Complex>* skyFTGrid=NULL);
    void* run();
    void setPID();
    pid_t getPID() {return myPID_p;}
    pid_t getTID() {return myTID_p;}
    casacore::Int getID () {return myID_p;}

    //
    //------------------------------------------------------------------------------
    //----------------------------Private parts-------------------------------------
    //------------------------------------------------------------------------------
    //
  private:
    casacore::Int myID_p;
    casacore::CountedPtr<ThreadCoordinator<casacore::Int> > myThreadClerk_p;
    VisibilityResamplerBase *myResampler_p;
    VBStore* myVBStore_p;
    casacore::Array<casacore::DComplex>* myGriddedDataDouble_p;
    casacore::Array<casacore::Complex>* myGriddedDataSingle_p;
    casacore::Matrix<casacore::Double>* mySumWt_p;
    pid_t myPID_p, myTID_p;
    const casacore::Array<casacore::Complex>* mySkyFTGrid_p;
    pid_t gettid_p () {return syscall (SYS_gettid);};
  };
}; //# NAMESPACE CASA - END

#endif // 
