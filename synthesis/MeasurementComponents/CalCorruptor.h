//# CalCorruptor.h
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

#ifndef SYNTHESIS_CALCORRUPTOR_H
#define SYNTHESIS_CALCORRUPTOR_H

// for simulation 
#include <casacore/casa/BasicMath/Random.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <synthesis/MeasurementComponents/VisCal.h>

using namespace std;

//#ifndef CASA_STANDALONE //libatmosphere should always be available now
#include <atmosphere/ATM/ATMRefractiveIndexProfile.h>
#include <atmosphere/ATM/ATMPercent.h>
#include <atmosphere/ATM/ATMPressure.h>
#include <atmosphere/ATM/ATMNumberDensity.h>
#include <atmosphere/ATM/ATMMassDensity.h>
#include <atmosphere/ATM/ATMTemperature.h>
#include <atmosphere/ATM/ATMLength.h>
#include <atmosphere/ATM/ATMInverseLength.h>
#include <atmosphere/ATM/ATMOpacity.h>
#include <atmosphere/ATM/ATMHumidity.h>
#include <atmosphere/ATM/ATMFrequency.h>
#include <atmosphere/ATM/ATMWaterVaporRadiometer.h>
#include <atmosphere/ATM/ATMWVRMeasurement.h>
#include <atmosphere/ATM/ATMProfile.h>
#include <atmosphere/ATM/ATMSpectralGrid.h>
#include <atmosphere/ATM/ATMRefractiveIndex.h>
#include <atmosphere/ATM/ATMSkyStatus.h>
#include <atmosphere/ATM/ATMAngle.h>
/* #else */
/* //#ATM Not available; mimic the classes and functions used */
/* namespace atm{ */
/* class Angle */
/* { */
/* public: */
/*   double get(string) const {return 0.0;} */
/* }; */
/* class RefractiveIndexProfile */
/* { */
/* public: */
/*   Angle getDispersiveWetPhaseDelay(int,int) const {return Angle();} */
/* }; */
/* class AtmProfile; */
/* class SkyStatus; */
/* class SpectralGrid; */

/* } */
/* #endif */


namespace casa { //# NAMESPACE CASA - BEGIN


// for simulating corruptions
class CalCorruptor {
  
 public:
  
  CalCorruptor(const casacore::Int nSim);
  virtual ~CalCorruptor();
  inline casacore::uInt& nSim() { return nSim_; };
  inline casacore::Bool& times_initialized() { return times_initialized_; };
  inline casacore::Int& curr_slot() { return curr_slot_; };
  inline casacore::Double& curr_time() { return curr_time_; };
  inline casacore::Double& startTime() { return starttime_; };
  inline casacore::Double& stopTime() { return stoptime_; };
  inline casacore::Double& slot_time(const casacore::Int i) { return slot_times_(i); };
  inline casacore::Double& slot_time() { return slot_times_(curr_slot()); };
  inline casacore::Vector<casacore::Double>& slot_times() { return slot_times_; };
  inline casacore::Float& amp() { return amp_;};
  virtual void initialize() {};

  // a generic initializer that just takes amplitude and simpar
  void initialize(const casacore::Float amp, const casacore::Record& simpar) {
    amp_=amp;
    simpar_=simpar;   
  };
  inline casacore::Record& simpar() {return simpar_;}
  inline casacore::String& mode() { return mode_; };

  void setEvenSlots(const casacore::Double& dt);
  virtual casacore::Complex simPar(const VisIter& vi, VisCal::Type type,casacore::Int ipar);

  inline casacore::uInt& nPar() { return nPar_; };
  inline casacore::uInt& nChan() { return fnChan_[currSpw()]; };  
  inline const casacore::uInt& focusChan() {return curr_chan_[currSpw()];};
  inline const casacore::Double& focusFreq() {return curr_freq_;};
  virtual void setFocusChan(casacore::Int chan) {
    curr_chan_[currSpw()]=chan;
    // WARN:  this assumes constant channel width - more detailed 
    // channel freq may be inaccurate
    casacore::Double fRes(fWidth()[currSpw()]/casacore::Double(fnChan()[currSpw()]));
    curr_freq_=fRefFreq()[currSpw()]+chan*fRes;
  };

  virtual void setCurrTime(const casacore::Double& time);
  
  // inherited from VC
  inline casacore::uInt& prtlev() { return prtlev_; };
  inline casacore::uInt& nAnt() { return nAnt_; };
  inline casacore::uInt& nSpw() { return nSpw_; };  
  inline casacore::uInt& currAnt() { return curr_ant_; };
  inline casacore::uInt& currAnt2() { return curr_ant2_; };
  inline casacore::uInt& currSpw() { return curr_spw_; };
  inline casacore::Vector<casacore::Float>& fRefFreq() { return fRefFreq_; };
  inline casacore::Vector<casacore::Float>& fWidth() { return fWidth_; };
  inline casacore::Vector<casacore::uInt>& fnChan() { return fnChan_; };
  inline casacore::Vector<casacore::uInt>& currChans() { return curr_chan_; };

  inline casacore::Bool& freqDepPar() { return freqdep_; };

 protected:
   
   casacore::uInt nSim_;
   casacore::Int curr_slot_;
   casacore::Bool times_initialized_,freqdep_;
   casacore::uInt nPar_;
   casacore::Double curr_time_,starttime_,stoptime_,curr_freq_;
   casacore::Float amp_;
   casacore::Vector<casacore::Double> slot_times_;   
   casacore::Record simpar_;
   casacore::String mode_; // general parameter for different kinds of corruptions

   casacore::uInt prtlev_;   
   casacore::uInt nAnt_,curr_ant_,nSpw_,curr_spw_,curr_ant2_;
   casacore::Vector<casacore::Float> fRefFreq_,fWidth_; // for each spw
   casacore::Vector<casacore::uInt> fnChan_,curr_chan_;

 private:

};







class ANoiseCorruptor : public CalCorruptor {

  public:
    ANoiseCorruptor(): CalCorruptor(1) {};
    virtual ~ANoiseCorruptor();
    virtual void initialize() {
      initialize(1234,1.0);
    }
    void initialize(const casacore::Int seed, const casacore::Float amp) {
      rndGen_p = new casacore::MLCG(seed);
      nDist_p = new casacore::Normal(rndGen_p, 0.0, 1.0); // sigma=1.
      amp_=amp;
    };
    virtual casacore::Complex simPar(const VisIter& vi,VisCal::Type type,casacore::Int ipar);
    virtual casacore::Complex simPar();

  private:
    casacore::MLCG *rndGen_p;
    casacore::Normal *nDist_p;
  };





// D is like ANoise but has a complex amplitude (different sigma in real/imag), and 
// a systematic offset
class DJonesCorruptor : public CalCorruptor {

  public:
    DJonesCorruptor(): CalCorruptor(1) {};
    virtual ~DJonesCorruptor();
    virtual void initialize() {
      initialize(1234,casacore::Complex(1.0,1.0),casacore::Complex(0.0));
    }
    void initialize(const casacore::Int seed, const casacore::Complex camp, const casacore::Complex offset) {
      rndGen_p = new casacore::MLCG(seed);
      nDist_p = new casacore::Normal(rndGen_p, 0.0, 1.0); // sigma=1.
      camp_=camp;
      offset_=offset;
    };
    virtual casacore::Complex simPar(const VisIter& vi,VisCal::Type type,casacore::Int ipar);
    inline casacore::Complex& camp() { return camp_; };
    inline casacore::Complex& offset() { return offset_; };

  private:
    casacore::MLCG *rndGen_p;
    casacore::Normal *nDist_p;
    casacore::Complex camp_,offset_;
  };







// this generates fractional brownian motion aka generalized 1/f noise
// class fBM : public casacore::Array<casacore::Double> {
class fBM {

 public:

  fBM(casacore::uInt i1);
  fBM(casacore::uInt i1, casacore::uInt i2);
  fBM(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3);
  // virtual ~fBM(); // not ness if we don't derive from this
  inline casacore::Bool& initialized() { return initialized_; };
  void initialize(const casacore::Int seed, const casacore::Float beta);

  inline casacore::Array<casacore::Float> data() { return *data_; };
  inline casacore::Float data(casacore::uInt i1) { return data_->operator()(casacore::IPosition(1,i1)); };
  inline casacore::Float data(casacore::uInt i1, casacore::uInt i2) { return data_->operator()(casacore::IPosition(2,i1,i2)); };
  inline casacore::Float data(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3) { return data_->operator()(casacore::IPosition(3,i1,i2,i3)); };


 private:
  casacore::Bool initialized_;
  casacore::Array<casacore::Float>* data_;

};






class AtmosCorruptor : public CalCorruptor {

 public:
   AtmosCorruptor();
   AtmosCorruptor(const casacore::Int nSim);
   virtual ~AtmosCorruptor();

   casacore::Float& pwv(const casacore::Int i); 
   casacore::Vector<casacore::Float>* pwv();
   void initAtm();
   inline casacore::Float& mean_pwv() { return mean_pwv_; };
   // pwv screen e.g. for a T
   inline casacore::Matrix<casacore::Float>& screen() { return *screen_p; };
   inline casacore::Float screen(const casacore::Int i, const casacore::Int j) { 
     return screen_p->operator()(i,j); };
   using CalCorruptor::initialize;
   virtual void initialize(const casacore::Int rxType);

   // use ATM but no time dependence - e.g. for B[Tsys]
   void initialize(const VisIter& vi, const casacore::Record& simpar, VisCal::Type type, const casacore::Int rxType);
   casacore::Vector<casacore::Double> antDiams;

   void initialize(const casacore::Int Seed, const casacore::Float Beta, const casacore::Float scale, const casacore::Int rxType);
   void initialize(const casacore::Int Seed, const casacore::Float Beta, const casacore::Float scale, const casacore::Int rxType, 
		   const casacore::MSAntennaColumns& antcols);
   // phase corruption gain for a T
   casacore::Complex cphase(const casacore::Int islot);
   casacore::Complex cphase(const casacore::Int ix, const casacore::Int iy, const casacore::Int islot);
   inline casacore::Vector<casacore::Float>& antx() { return antx_; };
   inline casacore::Vector<casacore::Float>& anty() { return anty_; };
   inline casacore::Float& windspeed() { return windspeed_; };
   inline casacore::Float& pixsize() { return pixsize_; };

   inline casacore::Float& tauscale() { return tauscale_; };
   casacore::Float tsys(const casacore::Float& airmass);
   casacore::Float opac(const casacore::Int ichan);
   inline casacore::Float& spilleff() { return spilleff_; };

   inline casacore::Float& tground() { return tground_; };
   inline casacore::Float& tatmos() { return tatmos_; };
   inline casacore::Float& trx() { return trx_; };
   inline casacore::Float& tcmb() { return tcmb_; };
   inline casacore::Int& rxType() { return rxtype_; };  // 0=2SB, 1=DSB
   // gets slow to calculate 1/exp(hv/kt)-1 all the time so 
   inline casacore::Double& Rtground() { return Rtground_; };
   inline casacore::Double& Rtatmos() { return Rtatmos_; };
   //inline casacore::Double& Rtrx() { return Rtrx_; };
   inline casacore::Double& Rtcmb() { return Rtcmb_; };
   inline casacore::Float& senscoeff() { return sensitivityCoeff_; };

   virtual casacore::Complex simPar(const VisIter& vi, VisCal::Type type,casacore::Int ipar);
   
   inline casacore::Vector<casacore::uInt>& ATMnChan() { return ATMnChan_; };
   inline casacore::Vector<casacore::uInt>& ATMchanMap(casacore::uInt ispw) { return ATMchanMap_[ispw]; };
  
   virtual void setFocusChan(casacore::Int chan) {
     curr_chan_[currSpw()]=chan;
     // WARN:  this assumes constant channel width - more detailed 
     // channel freq may be inaccurate
     casacore::Double fRes(fWidth()[currSpw()]/casacore::Double(fnChan()[currSpw()]));
     curr_freq_=fRefFreq()[currSpw()]+chan*fRes;
     // for temp calculations, recalculate the radiances 1/exp(hn/kt)-1
     double hn_k = 0.04799274551*1e-9*focusFreq(); 
     Rtcmb() = 1./(exp(hn_k/tcmb())-1.);
     Rtground() = 1./(exp(hn_k/tground())-1.);
     //Rtrx() = 1./(exp(hn_k/trx())-1.);
     Rtatmos() = 1./(exp(hn_k/tatmos())-1.);
  };

  virtual void setCurrTime(const casacore::Double& time);
  
 protected:

 private:   
   casacore::Int rxtype_;
   casacore::Float mean_pwv_,windspeed_,pixsize_,tauscale_,
     tground_,spilleff_,trx_,tatmos_,tcmb_;
   casacore::Double Rtatmos_,Rtcmb_,Rtground_;//,Rtrx_
   casacore::Matrix<casacore::Float>* screen_p; 

   atm::AtmProfile *itsatm;
   atm::RefractiveIndexProfile *itsRIP;
   atm::SkyStatus *itsSkyStatus;
   atm::SpectralGrid *itsSpecGrid;

   casacore::Vector<casacore::uInt> ATMnChan_;
   casacore::Vector<casacore::Vector<casacore::uInt> > ATMchanMap_;

   casacore::PtrBlock<casacore::Vector<casacore::Float>*> pwv_p;
   casacore::Vector<casacore::Float> antx_,anty_;  // antenna positions in units of screen resl

   casacore::Vector<casacore::Float> airMass_; // length= nAnt, recalculated if ness
   casacore::Bool airMassValid_;
   casacore::Double airMassTime_;
   casacore::Float sensitivityCoeff_;
};





class GJonesCorruptor : public CalCorruptor {

 public:
   GJonesCorruptor(const casacore::Int nSim);
   virtual ~GJonesCorruptor();

   //casacore::Complex& drift(const casacore::Int i);  // drift as fBM
   casacore::Matrix<casacore::Complex>* drift();   
   inline casacore::Float& tsys() { return tsys_; };
   virtual void initialize();
   void initialize(const casacore::Int Seed, const casacore::Float Beta, const casacore::Float scale);
   casacore::Complex gain(const casacore::Int icorr, const casacore::Int islot);  // tsys scale and time-dep drift   
   virtual casacore::Complex simPar(const VisIter& vi, VisCal::Type type,casacore::Int ipar);

   // for the residual/gaussian noise
   void initialize(const casacore::Int seed, const casacore::Complex camp) {
     rndGen_p = new casacore::MLCG(seed);
     nDist_p = new casacore::Normal(rndGen_p, 0.0, 1.0); // sigma=1.
     camp_=camp;
    };
    inline casacore::Complex& camp() { return camp_; };

 protected:

 private:   
   casacore::Float tsys_;
   casacore::PtrBlock<casacore::Matrix<casacore::Complex>*> drift_p;
   // RI todo rearrange so there's a Gauss corruptor for AN,D,G, a fBMcorrupt,etc
   casacore::MLCG *rndGen_p;
   casacore::Normal *nDist_p;
   casacore::Complex camp_;
};





}
#endif
