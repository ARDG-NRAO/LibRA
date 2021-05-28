//# VisEquation.h: Interface definition for Vis Equation
//# Copyright (C) 1996,1997,1999,2000,2001,2002,2003
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

#ifndef SYNTHESIS_VISEQUATION_H
#define SYNTHESIS_VISEQUATION_H

#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Cube.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// Visibility Measurement Equation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto module="MeasurementComponents">MeasurementComponents</linkto> module
// </prerequisite>
//
// <etymology>
// Visibility Equation describes a model for measurements from a
// generic synthesis telescope
// </etymology>
//
// <synopsis> 
// This is responsible for the Measurement Equation of the Generic
// Interferometer due to Hamaker, Bregman and Sault and later extended
// by Noordam, and Cornwell.
//
// See <linkto module="MeasurementEquations">MeasurementEquations</linkto>
// for more details of the form of the VisEquation.
//
// VisEquation provides the following:
// <ul>
// <li> Sorting of calibration terms (VisCals) in the Visibility Equation
// <li> Application of calibration terms to data (via delegation to VisCals)
// <li> Delivery of other evaluations of the VisEquation, e.g., (differentiated) residuals
// </ul>
// </synopsis> 
//
// <example>
// <srcblock>
//
// </srcblock>
// </example>
//
// <motivation>
// VisEquation is part of a framework of classes that are
// designed for synthesis calibration and imaging. The others are the 
// <linkto module=MeasurementComponents>MeasurementComponents</linkto>.
// </motivation>
//
// <todo asof="">
// </todo>

  class SolveDataBuffer;  // VI2

class VisEquation {
public:

  // Contructor
  VisEquation();
  
  // Copy ctor
  VisEquation(const VisEquation& other);

  // Destructor
  virtual ~VisEquation();
  
  // Assignment
  VisEquation& operator=(const VisEquation& other);

  // Arrange for apply of a VisCal list (i.e., sort them into the correct order)
  void setapply(casacore::PtrBlock<VisCal*>& vcin);

  // Arrange for solve of a single SolvableVisCal
  void setsolve(SolvableVisCal& svc);

  // Arrange a pivot point for evaluating the equation in collapseForSim
  void setPivot(VisCal::Type pivot);

  // Where is the pivot?
  VisCal::Type pivot() const { return pivot_; };

  // Arrange the model to use for calibration
  void setModel(const casacore::Vector<casacore::Float>& stokes);
  inline void unSetModel() { useInternalModel_=false; };

  // How many terms are arranged for apply?
  inline casacore::Int nTerms() const { return napp_; };

  // casacore::List the terms
  casacore::Vector<VisCal::Type> listTypes() const;
  
  // Report if spw has solutions available from all applied tables
  //  To be deprecated in favor of VBOKforCalApply
  casacore::Bool spwOK(const casacore::Int& spw);

  // Report if VB can be calibrated by all supplied
  //  calibration.   If not, we can't ask it to!
  // Modern replacement for spwOK(spw) which is sensitive
  //  to obs, fld, and intent, and which supports
  //  per-caltable agnosticism (see SVC::VBOKforCalApply)
  casacore::Bool VBOKforCalApply(vi::VisBuffer2& vb);

  // Correct in place the OBSERVED visibilities in a VisBuffer
  //  with the apply-able VisCals
  void correct(VisBuffer& vb, casacore::Bool trial=false);
  void correct2(vi::VisBuffer2& vb, casacore::Bool trial=false, casacore::Bool doWtSp=false);

  // Report flag-by-cal statistics
  casacore::Record actionRec();

  // Corrupt in place the MODEL visibilities in a VisBuffer
  //  with the apply-able VisCals
  void corrupt(VisBuffer& vb);
  void corrupt2(vi::VisBuffer2& vb);

  // Correct/Corrupt in place the OBSERVED/MODEL visibilities in a VisBuffer
  //  with the apply-able VisCals on either side of the SolvableVisCal
  void collapse(VisBuffer& vb);
  void collapse2(vi::VisBuffer2& vb); // VB2 version


  // This collapse avoids I/O (assumes the vb data/model are ready),
  //  and uses a generic pivot (see setPivot) rather than the svc
  //  void collapseForSim(VisBuffer& vb);
  void collapseForSim(VisBuffer& vb);

  // Calculate residuals 
  //   (optionally for specific chan)
  void residuals(VisBuffer& vb, 
		 casacore::Cube<casacore::Complex>& R,
		 const casacore::Int chan=-1);
  
  // Calculate residuals and differentiated residuals
  //   (optionally for specific chan)
  void diffResiduals(CalVisBuffer& cvb);
  void diffResiduals(VisBuffer& vb, 
		     casacore::Cube<casacore::Complex>& R, 
		     casacore::Array<casacore::Complex>& dR,
		     casacore::Matrix<casacore::Bool>& Rflg); 
  void diffResiduals(VisBuffer& R,
                     VisBuffer& dR0,
                     VisBuffer& dR1,
                     casacore::Matrix<casacore::Bool>& Rflg);

  // SolveDataBuffer version
  void differentiate(SolveDataBuffer& sdb);  // VI2

  // Differentiate w.r.t. MODEL Stokes Params
  //   I.e., _corrupt_ dM/d(I,Q,U,V)
  //   NB: results stored in the sdb
  void diffModelStokes(vi::VisBuffer2& vb, std::map<casacore::String,casacore::Cube<casacore::Complex> > dMdS);
  
  // Report the VisEq's state
  void state();

  // Set the print level
  inline void setPrtlev(const casacore::Int& prtlev) { prtlev_=prtlev; };
  
protected:

  casacore::Bool ok();

  // Access to the PB of apply terms 
  inline casacore::PtrBlock<VisCal*>& vc()  { return (*vcpb_); };

  // Access to SVC
  inline SolvableVisCal&    svc() { return *svc_; };

  // Detect freq dependence along the Vis Equation
  void setFreqDep();

  // Divide corr data by model (per correlation)
  void divideCorrByModel(vi::VisBuffer2& vb);

  // Divide corr data by Stokes I model 
  void divideByStokesIModel(vi::VisBuffer2& vb);


private:

  // Diagnostic print level access
  inline casacore::Int& prtlev() { return prtlev_; };

  // A local copy of the list of VisCal (pointers) for applying
  //  (No ownership responsibilities)
  casacore::PtrBlock<VisCal*>* vcpb_;

  // Number of apply VisCals in vc_
  casacore::Int napp_;

  // Frequency dependence indices
  casacore::Int lfd_;     // Right-most freq-dep term on LEFT  side
  casacore::Int rfd_;     // Left-most  freq-dep term on RIGHT side

  // Trap for frequency averaging in collapse
  casacore::Bool freqAveOK_;

  // VisCal with solving interface
  //  (No ownership responsibilities)
  SolvableVisCal* svc_;

  // The pivot point used by collapse2
  VisCal::Type pivot_;

  // An internal (global) point source model
  casacore::Bool useInternalModel_;
  casacore::Vector<casacore::Float> stokesModel_;

  // Keep count of total visibilities processed
  //  NB: this is Sum-over-spws(ncorr*nchan*nrow)
  casacore::Int nVisTotal_;

  // Diagnostic print level
  casacore::Int prtlev_;

};

} //# NAMESPACE CASA - END

#endif
