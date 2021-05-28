//# EPJones.h: Declaration of EPJones (Solvable)VisCal type
//# Copyright (C) 1996,1997,2000,2001,2002,2003
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

#ifndef SYNTHESIS_EPJONES_H
#define SYNTHESIS_EPJONES_H

#include <casa/aips.h>
#include <casa/Containers/Record.h>
#include <casa/BasicSL/Complex.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/nPBWProjectFT.h>
#include <synthesis/MeasurementComponents/PBMosaicFT.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/ImageInfo.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <msvis/MSVis/VisSet.h>
#include <synthesis/CalTables/CalSet.h>
#include <casa/OS/Timer.h>
namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declaration
  class VisEquation;
  class nPBWProjectFT;
  class PBMosaicFT;

// **********************************************************
//  EPJones (pointing errors)
//

class EPJones : public SolvableVisJones {
public:

  // Constructor
  EPJones(VisSet& vs);
	 //  {throw(casacore::AipsError("Use the constructor EPJones(VisSet&, MeasurementSet&) instead"));};
  EPJones(VisSet& vs, casacore::MeasurementSet& ms);
  //   EPJones(const casacore::Int& nAnt);  // NYI

  virtual ~EPJones();

  // Return the type enum
  virtual Type type() { return VisCal::E; };

  // Return type name as string
  virtual casacore::String typeName()     { return "EP Jones"; };
  virtual casacore::String longTypeName() { return "EP Jones (pointing errors)"; };

  // Type of Jones matrix according to nPar()
  Jones::JonesType jonesType() { return Jones::Diagonal; };
  
  virtual VisCalEnum::VCParType parType() { return setParType(VisCalEnum::REAL);};

  // Specialized access to pointing parameters (no chan axis)
  casacore::Cube<casacore::Float>& loadPar();

  virtual void setModel(const casacore::String& modelImage);
  // Set the solving parameters
  virtual void setSolve();
  virtual void setSolve(const casacore::Record& solve);
  virtual void setNiter(const casacore::Int& niter) {niter_p=niter;}
  virtual void setTolerance(const casacore::Float& tol) {tolerance_p = tol;}
  virtual void setGain(const casacore::Float& gain) {gain_p = gain;}

  // Arrange to apply (corrupt only)
  using SolvableVisCal::setApply;
  virtual void setApply(const casacore::Record& applypar);

  // Apply calibration to a VisBuffer 
  virtual void applyCal(VisBuffer& vb, 
			casacore::Cube<casacore::Complex>& Mout);

  // Differentiate a VisBuffer w.r.t. pointng parameters
  //
  // These effectively compute residuals and derivatives for
  // a time-averaged VisBuffer
  //
  virtual void differentiate(VisBuffer& vb,
			     casacore::Cube<casacore::Complex>& Mout,
			     casacore::Array<casacore::Complex>& dMout,
			     casacore::Matrix<casacore::Bool>& Mflg);
  // Differentiate a VisBuffer w.r.t. pointng parameters
  virtual void differentiate(VisBuffer& vb,
			     VisBuffer& dvb0,
			     VisBuffer& dvb1,
			     casacore::Matrix<casacore::Bool>& Mflg);
  //
  // This one averages the residuals and the derivatives in time.
  //
  virtual void diffResiduals(VisIter& vi, VisEquation& ve,
			    VisBuffer& residuals, 
			    VisBuffer& dVr1, 
			    VisBuffer& dVr2, 
			    casacore::Matrix<casacore::Bool>& flags);  
  // Guess (throws error because we don't yet solve for this)
  virtual void guessPar(VisBuffer& vb);
  virtual void guessPar() {pointPar_=0;}
  
  virtual casacore::Cube<casacore::Float>& solveRPar() {return pointPar_;}
  virtual void setRPar(casacore::Cube<casacore::Float>& val) {pointPar_.resize(val.shape());pointPar_=val;}
  virtual void setRPar(casacore::Double val) {pointPar_=val;}
  
  //  virtual void keep(const casacore::Int& slot);

  virtual casacore::Bool normalizable() { return false; };

  //  virtual BaseCalSet& cs() {return *rcs_;};
  virtual void keep(const casacore::Int& slot);
  
  inline virtual CalSet<casacore::Float>& rcs() {return *rcs_;};

  virtual void inflate(const casacore::Vector<casacore::Int>& nChan,
		       const casacore::Vector<casacore::Int>& startChan,
		       const casacore::Vector<casacore::Int>& nSlot);
  void initSolve(VisSet& vs);
  void initSolvePar();
  void store();
  void store(const casacore::String& table,const casacore::Bool& append);
  casacore::Bool verifyForSolve(VisBuffer& vb);
  virtual void postSolveMassage(const VisBuffer&);
  virtual void selfGatherAndSolve(VisSet& vs,VisEquation& ve);
  virtual casacore::Bool useGenericGatherForSolve() { return false; };
  virtual casacore::Float printFraction(const casacore::Int& ) {return 0.1;};
  casacore::Array<casacore::Float> getOffsets(const casacore::Int& spw) {return rcs().par(spw);}
  casacore::Array<casacore::Double> getTime(const casacore::Int& spw) {return rcs().time(spw);}
  void nearest(const casacore::Double time, casacore::Array<casacore::Float>& vals);
  void printRPar();  

protected:

  // EP has a pair of real parameters per feed
  virtual casacore::Int nPar() { return 4; };

  // Jones matrix elements are NOT trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // Fill-in a complex grid with the image values in prepration for 
  // computing it's FT
  virtual void makeComplexGrid(casacore::TempImage<casacore::Complex>& Grid, 
					casacore::PagedImage<casacore::Float>& ModelImage,
					VisBuffer& vb);
  void printActivity(const casacore::Int slotNo, const casacore::Int fieldId, const casacore::Int spw, const casacore::Int nSolutions);
  //  inline virtual CalSet<casacore::Float> rcs() {return *cs_;}

private:

  // Local casacore::Matrix for referencing pointing pars in a convenient way
  casacore::Cube<casacore::Float>  pointPar_;
  nPBWProjectFT *pbwp_p;
  //  PBMosaicFT *pbwp_p;
  //casacore::MeasurementSet *ms_p;
  VisSet *vs_p;
  //  casacore::Array<casacore::Float> azOff, elOff;
  casacore::TempImage<casacore::Complex> targetVisModel_;
  CalSet<casacore::Float> *rcs_;
  casacore::Double maxTimePerSolution, minTimePerSolution, avgTimePerSolution;
  casacore::Timer timer;
  casacore::Vector<casacore::Int> polMap_p;
  casacore::Float tolerance_p, gain_p;
  casacore::Int niter_p;
};





} //# NAMESPACE CASA - END

#endif

