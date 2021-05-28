//# VisCalSolver.h: Default solver for calibration using visibilities
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

#ifndef SYNTHESIS_VISCALSOL_H
#define SYNTHESIS_VISCALSOL_H

#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/Constants.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <msvis/MSVis/VisBuffGroupAcc.h>
#include <msvis/MSVis/CalVisBuffer.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// VisCalSolver: Default solver for calibration using visibility data
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="MeasurementComponents">MeasurementComponents</linkto> module
//   <li> <linkto class="VisEquation">VisEquation</linkto> module
// </prerequisite>
//
// <etymology>
// VisCal for visibility calibration (meaning solved from visibilities), Solver for
// solving. 
// </etymology>
//
// <synopsis> 
//
// VisCalSolver describes an interface for solving for calibration from visibility
// data in a VisBuffer.  It hosts the communication of visibility data with otherwise
// generic solving mechanims.  A Levenberg-Marquardt solver is supplied here by
// default, but it is intended that this class be the template for interfaces to
// third-party solving mechanisms.
//
// </synopsis> 
//
// <example>
// <srcblock>

// </srcblock>
// </example>
//
// <motivation>
// It is desirable to establish the distinct communicative boundary between generic
// solving mechanims and the particulars of visibility data and calibration
// component descriptions.  This class is intended to serve this purpose, by providing
// access to visibility data and calibration in terms of quantities necessary for
// least-squares (and other) style solvers.  
// </motivation>
//
// <todo asof="97/10/01">
// </todo>

// **********************************************************
//  VisCalSolver
//

class VisCalSolver {
public:

  // Constructor currently generic
  VisCalSolver();
  
  // Destructor
  ~VisCalSolver();

  // Do the solve
  casacore::Bool solve(VisEquation& viseq, SolvableVisCal& svc, VisBuffer& svb);
  casacore::Bool solve(VisEquation& viseq, SolvableVisCal& svc, VisBuffGroupAcc& vbga);

protected:

  // Access to fundamental external objects:
  inline VisBuffer&       svb()  { return *svb_; };
  inline VisBuffGroupAcc& vbga() { return *vbga_; };
  inline VisEquation&     ve()   { return *ve_; };
  inline SolvableVisCal&  svc()  { return *svc_; };

  // Accessors to current svb's (differentiated) Residuals
  inline casacore::Cube<casacore::Complex>&    R()    { return R_; };
  inline casacore::Array<casacore::Complex>&   dR()   { return dR_; };
  inline casacore::Matrix<casacore::Bool>&     Rflg() { return Rflg_; };

  inline casacore::Array<casacore::Complex>&   dSrc()   { return dSrc_; };

  // Access to maxIter_
  inline casacore::Int&    maxIter() { return maxIter_; };

  // Access to chi2
  inline casacore::Double& chiSq()     { return chiSq_; };
  inline casacore::Vector<casacore::Double>& chiSqV()     { return chiSqV_; };
  inline casacore::Double& lastChiSq() { return lastChiSq_; };
  inline casacore::Double& dChiSq()    { return dChiSq_; };
  inline casacore::Double& sumWt()     { return sumWt_; };
  inline casacore::Int&    nWt()       { return nWt_; };

  // Access to parameters, & grad,hess,dp
  inline casacore::Int&              nTotalPar() { return nTotalPar_; };
  inline casacore::Int&              nCalPar()   { return nCalPar_; };
  inline casacore::Int&              nSrcPar()   { return nSrcPar_; };
  inline casacore::Vector<casacore::Complex>&  par()       { return par_; };
  inline casacore::Vector<casacore::Bool>&     parOK()     { return parOK_; };
  inline casacore::Vector<casacore::Float>&    parErr()    { return parErr_; };
  inline casacore::Vector<casacore::Complex>&  srcPar()    { return srcPar_; };
  inline casacore::Vector<casacore::DComplex>& grad()      { return grad_; };
  inline casacore::Vector<casacore::Double>&   hess()      { return hess_; };
  inline casacore::Vector<casacore::Complex>&  dpar()      { return dpar_; };
  inline casacore::Vector<casacore::Complex>&  dCalPar()   { return dsrcpar_; };
  inline casacore::Vector<casacore::Complex>&  dSrcPar()   { return dcalpar_; };
  inline casacore::Vector<casacore::Complex>&  lastCalPar()   { return lastCalPar_; };
  inline casacore::Vector<casacore::Complex>&  lastSrcPar()   { return lastSrcPar_; };

  inline casacore::Double&           lambda()    { return lambda_; };

  // Initialize solving data
  void initSolve();

  // Obtain trial residuals w.r.t svc's current pars
  void residualate();
  void residualate2();

  // Differentiate the svb w.r.t svc's pars
  void differentiate();
  void differentiate2();

  // Calculate residuals (incl. diff'd) and chi2 
  void chiSquare();
  void chiSquare2();

  // Check for convergence
  casacore::Bool converged();

  // Internal solving methods
  void accGradHess();
  void accGradHess2();
  void revert();
  void solveGradHess();
  void updatePar();

  // Optimize the step parabolically
  void optStepSize();
  void optStepSize2();

  // Get and print par errors
  void getErrors();

  void printPar(const casacore::Int& iter);

private:
  
  // Diagnostic print level
  inline casacore::Int& prtlev() { return prtlev_; };

  // VisBuffer (from outside)
  VisBuffer* svb_;
  VisBuffGroupAcc* vbga_;

  // VisEquation (from outside)
  VisEquation* ve_;

  // SVC (from outside)
  SolvableVisCal* svc_;

  // Total Number of parameters
  casacore::Int nTotalPar_;
  casacore::Int nCalPar_;
  casacore::Int nSrcPar_;

  // Residual/Differentiation caches
  casacore::Cube<casacore::Complex>  R_;
  casacore::Array<casacore::Complex> dR_;
  casacore::Matrix<casacore::Bool>    Rflg_;

  // Derivative wrt Q and U
  casacore::Array<casacore::Complex> dSrc_;

  // Maximum number of solve iterations to attempt
  casacore::Int maxIter_;

  // Chi2, sum wts
  casacore::Double chiSq_;
  casacore::Vector<casacore::Double> chiSqV_;
  casacore::Double lastChiSq_;
  casacore::Double dChiSq_;
  casacore::Double sumWt_;
  casacore::Int    nWt_;
  casacore::Int    cvrgcount_;

  // Parameter storage
  // (these are casacore::Complex to match the VisCal solvePar)
  casacore::Vector<casacore::Complex> par_;
  casacore::Vector<casacore::Bool>    parOK_;
  casacore::Vector<casacore::Float>   parErr_;
  casacore::Vector<casacore::Complex> srcPar_;  
  casacore::Vector<casacore::Complex> lastCalPar_,lastSrcPar_;

  // Parameter update
  casacore::Vector<casacore::Complex> dpar_;
  casacore::Vector<casacore::Complex> dcalpar_,dsrcpar_;

  // Gradient, Hessian
  //  (these are casacore::Double for precision in accumulation
  casacore::Vector<casacore::DComplex> grad_;
  casacore::Vector<casacore::Double> hess_;

  // LM factor
  casacore::Double lambda_;

  // Step optimization toggle
  casacore::Bool optstep_;

  // Diagnostic print level
  casacore::Int prtlev_;

};

}
#endif
