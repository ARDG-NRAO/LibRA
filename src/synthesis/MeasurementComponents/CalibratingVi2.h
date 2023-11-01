//# CalibratingVi2.h: Interface definition of the CalibratingVi2 class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef CalibratingVi2_H_
#define CalibratingVi2_H_

// Where TransformingVi2 interface is defined
#include <msvis/MSVis/TransformingVi2.h>
#include <synthesis/MeasurementComponents/Calibrater.h>
#include <synthesis/MeasurementEquations/VisEquation.h>

#include <casacore/casa/Containers/Record.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace vi { //# NAMESPACE VI - BEGIN

class CalibratingParameters {

// TBD:  add solve-context parameters, like apply pivot (solved-for term), normalization

public:

  CalibratingParameters();

  CalibratingParameters(const casacore::Record& calLibRecord);
  CalibratingParameters(const casacore::String& callib);
  CalibratingParameters(casacore::Float corrFactor); // temporary, for initial testing

  CalibratingParameters(const CalibratingParameters& other);
  CalibratingParameters& operator=(const CalibratingParameters & other);

  casacore::Bool byCalLib() const;

  const casacore::Record& getCalLibRecord() const;
  casacore::Float getCorrFactor() const;  

  void setCalLibRecord(const casacore::Record& calLibRecord);
  void setCorrFactor(casacore::Float corrFactor);  

private:

  void validate() const;
  casacore::Bool byCalLib_p;
  casacore::Record calLibRecord_p;
  casacore::Float corrFactor_p;  // temporary, for initial testing
};




class CalibratingVi2 : public vi::TransformingVi2
{

public:

  CalibratingVi2( vi::ViImplementation2 * inputVii,
                  const CalibratingParameters& calpar);

  // From cal pars and an MS (used by OTF calibration outside synthesis)
  CalibratingVi2( vi::ViImplementation2 * inputVii,
                  const CalibratingParameters& calpar,
                  casacore::String msname);

  // From a VE pointer (used within synthesis, e.g., for solving)
  CalibratingVi2( vi::ViImplementation2 * inputVii,
                  VisEquation *ve);

  ~CalibratingVi2();
  
  // Report the the ViImplementation type
  virtual casacore::String ViiType() const { return casacore::String("Cal( ")+getVii()->ViiType()+" )"; };


  // Iterating management
  virtual void origin ();
  virtual void next ();

  // Smart corrected data/weights/flags accessors
  virtual void weight(casacore::Matrix<casacore::Float>& wt) const;
  virtual void weightSpectrum(casacore::Cube<casacore::Float>& wtsp) const;
  virtual void visibilityCorrected(casacore::Cube<casacore::Complex>& vis) const;
  virtual void flag(casacore::Cube<casacore::Bool>& flagC) const;
  //  virtual void flag(casacore::Matrix<casacore::Bool>& flagM) const;  // CORR-INDEP FLAGS DEPRECATED IN VI2/VB2?

  // Reports true in case of *Corrected columns
  //  (because this class provides it, even if it doesn't exist physically!)
  virtual casacore::Bool existsColumn (VisBufferComponent2 id) const;
  
protected:

  // Correct the current VB
  virtual void calibrateCurrentVB() const;

  // Initialize corrected data member to raw data
  //  (this method is sensitive to DATA vs. FLOAT_DATA
  virtual void initCorrected(casa::vi::VisBuffer2* vb) const;

  // Calibrater and VisEquation
  Calibrater *cb_p;
  VisEquation *ve_p;   

  // A simple factor for testing
  casacore::Float corrFactor_p;  

  // signals whether or not correctCurrentVB has been called
  mutable casacore::Bool visCalibrationOK_p;

};

class CalVi2LayerFactory : public ViiLayerFactory {

 public:

  CalVi2LayerFactory(const CalibratingParameters& calpars);

  virtual ~CalVi2LayerFactory () {}

 protected:

  // CalVi2-specific layer-creater
  //   
  virtual ViImplementation2 * createInstance (ViImplementation2* vii0) const;

  // Store a copy of the parameters
  const CalibratingParameters calpars_;


};

class CalSolvingVi2 : public vi::CalibratingVi2
{

public:

  CalSolvingVi2( vi::ViImplementation2 * inputVii,
		 const CalibratingParameters& calpar);

  CalSolvingVi2( vi::ViImplementation2 * inputVii,
		 const CalibratingParameters& calpar,
		 casacore::String msname);

  CalSolvingVi2( vi::ViImplementation2 * inputVii,
		 VisEquation *ve,
		 const casacore::Bool& corrDepFlags=false);

  ~CalSolvingVi2();


  // Iterating management
  virtual void originChunks(casacore::Bool forceRewind = false);
  virtual void origin ();
  virtual void next ();

  // Report the the ViImplementation type
  virtual casacore::String ViiType() const {
      return casacore::String("CalForSolve( ")+getVii()->ViiType()+" )";
  }

  // Weight spectrum exists _by_defn_ in the solving context
  //  (it is generated by verifyWeightSpectrum() below)
  virtual casacore::Bool weightSpectrumExists () const { return true; };

  // Cal solving also involve modified model data
  virtual void visibilityModel(casacore::Cube<casacore::Complex>& vis) const;
  
private:

  // specialized cal application for solving
  virtual void calibrateCurrentVB() const;

  // Flag all corrs if any one is flagged (OLD style)
  virtual void corrIndepFlags(casa::vi::VisBuffer2* vb) const;

  // Ensure weightSpectrum is populated
  virtual void verifyWeightSpectrum(casa::vi::VisBuffer2* vb) const;

  // Sample counters
  mutable casacore::Int64 ntotal_,nflagged_,nskipped_;
  mutable casacore::Int64 nVB_,nVB0_;
  mutable casacore::Double Tio_,Tcalws_,Tcalfl_,Tcal2_;

  casacore::Bool corrDepFlags_;

};


class CalSolvingVi2LayerFactory : public CalVi2LayerFactory {

 public:

  CalSolvingVi2LayerFactory(const CalibratingParameters& calpars);

  virtual ~CalSolvingVi2LayerFactory () {}

 protected:

  // CalSolvingVi2-specific layer-creater
  virtual ViImplementation2 * createInstance (ViImplementation2* vii0) const;

};

class CalSolvingVi2LayerFactoryByVE : public ViiLayerFactory {

 public:

  CalSolvingVi2LayerFactoryByVE(VisEquation* ve,const casacore::Bool& corrDepFlags=false);  // From a VE pointer in calling scope

  virtual ~CalSolvingVi2LayerFactoryByVE () {}

 protected:

  // CalSolvingVi2-specific layer-creater
  virtual ViImplementation2 * createInstance (ViImplementation2* vii0) const;

 private:
  
  VisEquation *ve_p;

  casacore::Bool corrDepFlags_;

};




} //# NAMESPACE VI - END
} //# NAMESPACE CASA - END


#endif /* CalibratingVi2_H_ */

