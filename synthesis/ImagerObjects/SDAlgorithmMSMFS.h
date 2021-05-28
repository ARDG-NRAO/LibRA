//# SDAlgorithmMSMFS.h: Definition for SDAlgorithmMSMFS
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_SDALGORITHMMSMFS_H
#define SYNTHESIS_SDALGORITHMMSMFS_H

#include <ms/MeasurementSets/MeasurementSet.h>
#include <synthesis/MeasurementComponents/SkyModel.h>
#include <casa/Arrays/Matrix.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/System/PGPlotter.h>

#include<synthesis/ImagerObjects/SDAlgorithmBase.h>
#include <synthesis/MeasurementEquations/MultiTermMatrixCleaner.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  /* Forware Declaration */
  class SIMinorCycleController;


  class SDAlgorithmMSMFS : public SDAlgorithmBase 
  {
  public:
    
    // Empty constructor
    SDAlgorithmMSMFS(casacore::uInt nTaylorTerms, casacore::Vector<casacore::Float> scalesizes, casacore::Float smallscalebias);
    virtual  ~SDAlgorithmMSMFS();
    
    void restore( std::shared_ptr<SIImageStore> imagestore );
     ///returns the estimate of memory used in kilobytes (kB);
    virtual casacore::Long estimateRAM(const std::vector<int>& imsize);
  protected:
    
    // Local functions to be overloaded by various algorithm deconvolvers.
    void takeOneStep( casacore::Float loopgain, casacore::Int cycleNiter, casacore::Float cycleThreshold, casacore::Float &peakresidual, casacore::Float &modelflux, casacore::Int &iterdone );
    //    void initializeDeconvolver( casacore::Float &peakresidual, casacore::Float &modelflux );
    void initializeDeconvolver();
    void finalizeDeconvolver();
    //    void queryDesiredShape(Bool &onechan, Bool &onepol, IPosition imshape); // , nImageFacets.

    casacore::uInt getNTaylorTerms(){ return itsNTerms; };
    
    //    void initializeSubImages( std::shared_ptr<SIImageStore> &imagestore, casacore::uInt subim);

    casacore::Bool createMask(casacore::LatticeExpr<casacore::Bool> &lemask, casacore::ImageInterface<casacore::Float> &outimage);

    //    std::shared_ptr<SIImageStore> itsImages;

    casacore::Vector< casacore::Array<casacore::Float> > itsMatPsfs, itsMatResiduals, itsMatModels;
    casacore::Array<casacore::Float> itsMatMask;  // Make an array if we eventually use multi-term masks...

    /*    
    casacore::IPosition itsMaxPos;
    casacore::Float itsPeakResidual;
    casacore::Float itsModelFlux;

    casacore::Matrix<casacore::Float> itsMatMask;
    */

    casacore::uInt itsNTerms;
    casacore::Vector<casacore::Float> itsScaleSizes;
    casacore::Float itsSmallScaleBias;

    MultiTermMatrixCleaner itsMTCleaner;

  private:
    casacore::Bool itsMTCsetup; 

  };

} //# NAMESPACE CASA - END

#endif
