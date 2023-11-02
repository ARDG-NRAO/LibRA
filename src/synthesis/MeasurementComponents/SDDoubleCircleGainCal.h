/*
 * SDDoubleCircleGainCal.h
 *
 *  Created on: Jun 3, 2016
 *      Author: nakazato
 */

#ifndef SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCAL_H_
#define SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCAL_H_

#include <synthesis/MeasurementComponents/StandardVisCal.h>
#include <casacore/casa/BasicSL/String.h>

#include <synthesis/MeasurementComponents/SDDoubleCircleGainCalImpl.h>

// set false if you want to use new Calibrater
#define USEOLDVI false

namespace casa {

class VisSet;
class VisEquation;

class SDDoubleCircleGainCal final : public GJones {
public:
  SDDoubleCircleGainCal(VisSet& vs);
  SDDoubleCircleGainCal(const MSMetaInfoForCal& msmc);
  virtual ~SDDoubleCircleGainCal();

  // Return type name as string (ditto)
  virtual casacore::String typeName() override {
    return "SDGAIN_OTFD";
  }
  virtual casacore::String longTypeName() override {
    return "SDGAIN_OTFD (Single Dish gain calibration for double circle fast scan";
  }

// Return the parameter type
// so far single dish calibration is real
//  virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; }

  // useGenericGatherForSolve must return true to migrate VI/VB2 based implementation
  virtual casacore::Bool useGenericGatherForSolve() override {
    // it should finally return true, but to pass the unit test, it should return false
    // so, at the moment, return value is configurable depending on whether it is called
    // from old Calibrater or not
    bool ret = !(USEOLDVI);
    return ret;
  }
  // Do not use generic data gathering mechanism for solve
  virtual casacore::Bool useGenericSolveOne() override {
    return false;
  }

  // Set the solving parameters
  virtual void setSolve() override;
  virtual void setSolve(const casacore::Record& solve) override;

  // This is the freq-dep version of G
  //   (this is the ONLY fundamental difference from G)
  virtual casacore::Bool freqDepPar() override { return true; };

  // Freq-dep Weight scaling
  virtual casacore::Bool freqDepCalWt() override { return true; };

  // Report solve info/params, e.g., for logging
  virtual casacore::String solveinfo() override;

  // Post solve tinkering
  virtual void globalPostSolveTinker() override;

  // Self- gather and/or solve prototypes
  //  (triggered by useGenericGatherForSolve=F or useGenericSolveOne=F)
  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve) override;
  virtual void selfSolveOne(SDBList &sdbs) override;

  // specific keepNCT
  virtual void keepNCT() override;

protected:
  virtual void syncWtScale() override;

private:
  template<class Accessor>
  void executeDoubleCircleGainCal(casacore::MeasurementSet const &ms);

  casacore::Double central_disk_size_;
  casacore::Bool smooth_;
  casacore::Vector<casacore::Int> currAnt_;
  SDDoubleCircleGainCalImpl worker_;
};

} // namespace casa END

#endif /* SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCAL_H_ */
