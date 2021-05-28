//# CalCorruptor.cc: Corruptions for simulated data
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
//#                        Charlottesville, VA 22903-2475 USA
//#

#include <synthesis/MeasurementComponents/StandardVisCal.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>

#define PRTLEV 3

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


CalCorruptor::CalCorruptor(const Int nSim) :
  nSim_(nSim),
  curr_slot_(-1),
  times_initialized_(false),
  freqdep_(false),
  nPar_(0),
  curr_time_(0),starttime_(0),stoptime_(0),curr_freq_(1),
  amp_(0),mode_(""),
  prtlev_(PRTLEV),
  nAnt_(0), curr_ant_(-1), nSpw_(0), curr_spw_(-1), curr_ant2_(0)
{}

CalCorruptor::~CalCorruptor() {}


  Complex CalCorruptor::simPar(const VisIter& /*vi*/,VisCal::Type type, Int /*ipar*/){
  // per par (e.g. for D,G, nPar=2, could have diff gains for diff polns)
  if (prtlev()>2) cout << "   Corruptor::simPar("<<VisCal::nameOfType(type)<<")" << endl;

  if (type==VisCal::D) {
    return amp();

 } else {
    //Int iipar=ipar;
    // Every CalCorruptor needs to have its own checks - maybe put D here?
    throw(AipsError("This Corruptor doesn't yet support simulation of this VisCal type"));
  }
}


void CalCorruptor::setEvenSlots(const Double& dt) {
  // set slots to constant intervals dt
  Int nslots = Int( (stopTime()-startTime())/dt + 1 );

  if ((stopTime()-startTime())/dt<1) throw(AipsError("logic problem Corruptor::setEvenSlots called before start/stopTime set"));
  if (dt<=0) throw(AipsError("logic problem Corruptor::setEvenSlots called with bad time interval"));

  nSim()=nslots;
  slot_times().resize(nSim(),false);
  for (uInt i=0;i<nSim();i++)
    slot_time(i) = startTime() + (Double(i)+0.5) * dt;
  times_initialized()=true;

  curr_slot()=0;
  curr_time()=slot_time();
}



void CalCorruptor::setCurrTime(const Double& time) {
  if (prtlev()>4) cout<<"   CalCorruptor::setCurrTime()"<<endl;

  // if we need to invalidate aux parms e.g. airmass in atmcorr, override
  // this and do it here.
  curr_time()=time;
  // find new slot if required
  Double dt(1e10),dt0(-1);
  dt0 = abs(slot_time() - time);

  for (uInt newslot=0;newslot<nSim();newslot++) {
    dt=abs(slot_time(newslot) - time);
    // is this newslot closer to the current time?
    if (dt<dt0) {
      curr_slot()=newslot;
      dt0 = dt;
    }
  }
}




  Complex ANoiseCorruptor::simPar(const VisIter& /*vi*/, VisCal::Type type,Int /*ipar*/) {
  if (prtlev()>5) cout << "AN::simPar(vi,type,ipar) ";
  if (type==VisCal::ANoise) {
    if (currAnt() == currAnt2()){
      return (*nDist_p)()*amp(); // auto-correlation
    }else{
      // work-around to assure consistent random number sequence on different architectures.
      Double randR = (*nDist_p)();
      Double randI = (*nDist_p)();
      return Complex(randR*amp(),randI*amp()); // cross-correlation
    }
  } else throw(AipsError("unknown VC type "+VisCal::nameOfType(type)+" in AnoiseCorruptor::simPar(vi,type,ipar)"));
}


Complex ANoiseCorruptor::simPar() {
  if (prtlev()>5) cout << "AN::simPar() ";
  if (currAnt() == currAnt2()){
    return (*nDist_p)()*amp(); // auto-correlation
  }else{
    // work-around to assure consistent random number sequence on different architectures.
    Double randR = (*nDist_p)();
    Double randI = (*nDist_p)();
    return Complex(randR*amp(),randI*amp()); // cross-correlation
  }
}


ANoiseCorruptor::~ANoiseCorruptor() {
  if (nDist_p) {
    delete nDist_p;
  }
  if (rndGen_p) {
    delete rndGen_p;
  }
};





  Complex DJonesCorruptor::simPar(const VisIter& /*vi*/, VisCal::Type type,Int ipar) {
  if (type==VisCal::D) {
    // work-around to assure consistent random number sequence on different architectures.
    Double randR = (*nDist_p)();
    Double randI = (*nDist_p)();
    Complex g(randR*camp().real(),randI*camp().imag());
    if (prtlev()>5) cout << "D::simPar ";
    if (ipar>0) {
      g += Complex(-offset().real(),offset().imag());
    } else {
      g += offset();
    }
    return g;
  } else throw(AipsError("unknown VC type "+VisCal::nameOfType(type)+" in DCorruptor::simPar"));
}


DJonesCorruptor::~DJonesCorruptor() {};





// and also produce MfM and Topac corruptions


AtmosCorruptor::AtmosCorruptor() :
  CalCorruptor(1),  // parent
  mean_pwv_(-1.),
  screen_p(0),
  itsatm(NULL),
  itsRIP(NULL),
  itsSkyStatus(NULL),
  itsSpecGrid(NULL),
  airMassValid_(false),
  airMassTime_(-1000)
{}

AtmosCorruptor::AtmosCorruptor(const Int nSim) :
  CalCorruptor(nSim),  // parent
  mean_pwv_(-1.),
  screen_p(NULL),itsatm(NULL),itsRIP(NULL),itsSkyStatus(NULL),itsSpecGrid(NULL),
  airMassValid_(false),airMassTime_(-1000){}

AtmosCorruptor::~AtmosCorruptor() {
  if (itsSkyStatus) {
    delete itsSkyStatus;
    itsSkyStatus=0;
  }
  if (itsRIP) {
    delete itsRIP;
    itsRIP=0;
  }
  if (itsSpecGrid) {
    delete itsSpecGrid;
    itsSpecGrid=0;
  }
  if (itsatm) {
    delete itsatm;
    itsatm=0;
  }
  if (screen_p) {
    delete screen_p;
    screen_p=0;
  }
  if (prtlev()>2) cout << "AtmCorruptor::~AtmCorruptor()" << endl;
}



Vector<Float>* AtmosCorruptor::pwv() {
  if (currAnt()<=pwv_p.nelements())
    return pwv_p[currAnt()];
  else
    return NULL;
};

Float& AtmosCorruptor::pwv(const Int islot) {
  if (currAnt()<=pwv_p.nelements())
    return (*pwv_p[currAnt()])(islot);
  else
    throw(AipsError("AtmCorruptor internal error accessing delay()"));;
};





void AtmosCorruptor::setCurrTime(const Double& time) {
  if (prtlev()>4) cout<<"   AtmosCorruptor::setCurrTime()"<<endl;

  // if we need to invalidate aux parms e.g. airmass in atmcorr, override
  // this and do it here:

  Double dt(1e10),dt0(-1);

  //dt=abs(curr_time() - time);
  dt=abs(airMassTime_ - time);
  //cout <<" setCurrTime t="<<time<<" prev curr_t="<<curr_time()<<" dt="<<dt<<endl;
  // for airmass, we want to recalculate if it changes by more than 1%
  // if elevation>5 deg, airmass<10, d(airmass)/d(theta)<100,
  // recalc if dtheta > .1/100 -> 0.06deg.  1deg=4 min so make tolerance 100s
  if (dt>100.) airMassValid_=false;

  curr_time()=time;
  // find new slot if required
  dt0 = abs(slot_time() - time);

  for (uInt newslot=0;newslot<nSim();newslot++) {
    dt=abs(slot_time(newslot) - time);
    // is this newslot closer to the current time?
    if (dt<dt0) {
      curr_slot()=newslot;
      dt0 = dt;
    }
  }

  if (prtlev()>4) cout<<"   ~AtmosCorruptor::setCurrTime()"<<endl;
}




  Complex AtmosCorruptor::simPar(const VisIter& vi, VisCal::Type type,Int /*ipar*/){

  //  LogIO os(LogOrigin("AtmCorr", "simPar("+VisCal::nameOfType(type)+")", WHERE));
  //  if (prtlev()>5) cout << "  Atm::simPar("<<VisCal::nameOfType(type)<<") ipar=" <<ipar<< endl;

  // WARNING: removed error trapping and exception reporting from this
  // subroutine for speed 20100831 RI

    if (mode() == "tsys-manual" or mode()=="tsys-atm") {

      double factor(1.0),airmass(1.0);
      double tint = vi.msColumns().exposure()(0);
      int iSpW = vi.spectralWindow();
      double deltaNu =
	abs(vi.msColumns().spectralWindow().totalBandwidth()(iSpW)) /
	Float(vi.msColumns().spectralWindow().numChan()(iSpW));

      // 20100824 getAngle() is very slow: cache airmass
      if (!airMassValid_) {
	if (prtlev()>3) cout <<"     simPar recalc airMass_ .. ";

	Vector<MDirection> antazel(vi.azel(curr_time()));
	float el(0.);

	if (airMass_.nelements() != nAnt()) airMass_.resize(nAnt());
	for (uInt iAnt=0;iAnt<nAnt();iAnt++) {
	  el = antazel(iAnt).getAngle("rad").getValue()(1);
	  if (el>0)
	    airMass_(iAnt)= 1./sin(el);
	  else
	    airMass_(iAnt)= 1000.;
	}
	airMassValid_=true;
	airMassTime_=curr_time();

	if (prtlev()>3) cout <<"done"<<endl;
      }

      //cout << "Delta nu = " << deltaNu << "; amp=" << amp() << "; tint=" << tint << endl;


      if (type==VisCal::M) {
	factor = senscoeff() * amp() / sqrt( deltaNu * tint ) ;
	airmass= 0.5*(airMass_(currAnt()) + airMass_(currAnt2()));

	// this is tsys above atmosphere
	// tsys = tsys(airmass) * exp( tau*airmass )
	return Complex( antDiams(currAnt())*antDiams(currAnt2()) /factor
			/tsys(airmass) );

      } else if (type==VisCal::T) {

	factor = sqrt( senscoeff() * amp() / sqrt( deltaNu * tint ) ) ;
	airmass = airMass_(currAnt());

//	cout << "gain per baseline [mJy]=" <<
//	  tsys(airmass) * (factor/antDiams(currAnt())) * (factor/antDiams(currAnt()))*1000 <<
//	  " " << amp() << endl;

	// this is tsys above atmosphere
	// tsys = tsys(airmass) * exp( tau*airmass )
	return Complex( antDiams(currAnt()) /factor
			/sqrt(tsys(airmass)) );

      } else {
	throw(AipsError("AtmCorruptor: unknown VisCal type "+VisCal::nameOfType(type)));
      }
    } else {
      // not tsys - so either flat noise or phase noise T

      if (type==VisCal::T) {

	if (mode()=="test" or mode()=="1d")
	  return cphase(focusChan());

	else if (mode()=="2d") {
	  // RI TODO Atmcorr:simPar  modify x,y by tan(zenith angle)*(layer altitude)
	  Int ix(Int( antx()(currAnt()) ));
	  Int iy(Int( anty()(currAnt()) ));
	  if (prtlev()>5)
	    cout << " getting gain for antenna ix,iy = " << ix << "," << iy << endl;
	  return cphase(ix,iy,focusChan()); // init all elts of gain vector with scalar
	} else
	  throw(AipsError("AtmCorruptor: unknown corruptor mode "+mode()));
      } else if (type==VisCal::M)
	return Complex( 1./amp() ); // for constant amp MfM
      else
	throw(AipsError("AtmCorruptor: unknown VisCal type "+VisCal::nameOfType(type)));
    }
}



void AtmosCorruptor::initAtm() {

  LogIO os(LogOrigin("AtmCorr", "initAtm", WHERE));

  os << LogIO::DEBUG1 << " defining observatory ground characteristics.."
     << LogIO::POST;

  atm::Temperature  T( 270.0,atm::Temperature::UnitKelvin );   // Ground temperature
  atm::Pressure     P( 560.0,atm::Pressure::UnitMilliBar);   // Ground Pressure
  atm::Humidity     H(  20,atm::Percent::UnitPercent );     // Ground Relative Humidity (ind)
  atm::Length       Alt(  5000,atm::Length::UnitMeter ); // Altitude of the site
  atm::Length       WVL(   2.0,atm::Length::UnitKiloMeter); // Water vapor scale height

  // user defined observatory info
  if (simpar().isDefined("tground"))
    T=atm::Temperature(simpar().asFloat("tground"),atm::Temperature::UnitKelvin);
  if (simpar().isDefined("pground"))
    P=atm::Pressure(simpar().asFloat("pground"),atm::Pressure::UnitMilliBar);
  if (simpar().isDefined("relhum"))
    H=atm::Humidity(simpar().asFloat("relhum"),atm::Percent::UnitPercent);
  if (simpar().isDefined("altitude"))
    Alt=atm::Length(simpar().asFloat("altitude"),atm::Length::UnitMeter);
  if (simpar().isDefined("waterheight"))
    WVL=atm::Length(simpar().asFloat("waterheight"),atm::Length::UnitKiloMeter);

  double TLR = -5.6;     // Tropospheric lapse rate (must be in K/km)
  atm::Length  topAtm(  48.0,atm::Length::UnitKiloMeter);   // Upper atm. boundary for calculations
  atm::Pressure Pstep(  10.0,atm::Pressure::UnitMilliBar);   // Primary pressure step
  double PstepFact = 1.2; // Pressure step ratio between two consecutive layers
  unsigned int atmType = 1;//atm::tropical;

  os << "Initializing ATM" << LogIO::POST;
  os << "altitude="<<Alt.get(atm::Length::UnitMeter)<<"m, Pground="<<P.get(atm::Pressure::UnitMilliBar)<<"mb, " <<
    "Tground="<<T.get(atm::Temperature::UnitKelvin)<<"K, humidity= "<<H.get(atm::Percent::UnitPercent)<<"%, " <<
    "water scale height="<<WVL.get(atm::Length::UnitMeter)<<"m"<<LogIO::POST;

  itsatm = new atm::AtmProfile(Alt, P, T, TLR,
			       H, WVL, Pstep, PstepFact,
			       topAtm, atmType);

  if (nSpw()<=0)
    throw(AipsError("AtmCorruptor::initAtm called before spw setup."));

  ATMnChan().resize(nSpw());
  ATMchanMap_.resize(nSpw());
  //  cout<<"nspw="<<nSpw()<<endl;

  // make more channels than one because ATM doesn't get the right answer
  // but then need chanMap[focusChan() from VisCal] = atm channel
  // fnChan() is vector, # chans in the MS/VisCal as a fn of spw:
  // ATMnChan() is vector, # chans in ATM as a fn of spw:
  uInt nChan=fnChan()[0];
  if (nChan==1) {
    ATMnChan()[0]=static_cast<unsigned int>(fWidth()[0]/100.e6);
    if (ATMnChan()[0]>1e4) {
      ATMnChan()[0]=static_cast<unsigned int>(1e4);
    }
    if (ATMnChan()[0]<10) {
      ATMnChan()[0]=10;
    }
    //    cout<<"ATMnChan0="<<ATMnChan()[0]<<endl;
    // for the continuum case we want the single element chanMap to point to
    // the middle channel
    ATMchanMap(0).resize(1);
    ATMchanMap(0)[0] = Int(floor(ATMnChan()[0]/2));
    //    cout<<"ATMnChanmap0="<<ATMchanMap(0)[0]<<endl;

  } else {
    ATMnChan()[0]=nChan; //if MS is cube, just use those chans in ATM for now - could subsample in the future with this machinery
    // ATMchanMap(spw) is a vector, Nelements= fnChan()[spw] - here spw=0
    // and nChan is set above
    ATMchanMap(0).resize(nChan);
    //for (uInt ichan=1;ichan<nChan;ichan++) {
    for (uInt ichan=0;ichan<nChan;ichan++) {
      ATMchanMap(0)[ichan]=ichan;
    }
  }

  // focus chan is middle of MS grid - if continuum, fChan=0
  int fChan=Int(floor(fnChan()[0]/2));
  setFocusChan(fChan);

  // first ATMSpW:
  double ATMfRes(fWidth()[0]/ATMnChan()[0]);

  os << "Spectral window " << fRefFreq()[0]/1e9 << "(ch " << ATMchanMap(0)[focusChan()] << "/"<<ATMnChan()[0] << ")" << LogIO::POST;

  // !!! refChan is 0-based !!!
  itsSpecGrid = new atm::SpectralGrid(ATMnChan()[0],
				      ATMchanMap(0)[focusChan()],
				      atm::Frequency(fRefFreq()[0],atm::Frequency::UnitHertz),
				      atm::Frequency(ATMfRes,atm::Frequency::UnitHertz));

  // any more spw?  TODO: multiple spw hasn't been well tested.
  for (uInt ispw=1;ispw<nSpw();ispw++) {
    ATMfRes = fWidth()[ispw]/ATMnChan()[ispw];

    nChan=fnChan()[ispw];
    if (nChan==1) {
      ATMnChan()[ispw]=static_cast<unsigned int>(fWidth()[ispw]/100.e6);
      if (ATMnChan()[ispw]>1e4) {
	ATMnChan()[ispw]=static_cast<unsigned int>(1e4);
      }
      if (ATMnChan()[ispw]<10) {
	ATMnChan()[ispw]=10;
      }
      // for the continuum case we want the single element chanMap to point to
      // the middle channel
      ATMchanMap(ispw).resize(1);
      ATMchanMap(ispw)[0] = Int(floor(fnChan()[ispw]/2));

    } else {
      ATMnChan()[ispw]=nChan; //if MS is cube, just use those chans in ATM for now - could subsample in the future with this machinery
      // ATMchanMap(spw) is a vector, Nelements= fnChan()[spw] - here spw=0
      // and nChan is set above
      ATMchanMap(ispw).resize(nChan);
      //for (uInt ichan=1;ichan<nChan;ichan++) {
      for (uInt ichan=0;ichan<nChan;ichan++) {
	ATMchanMap(ispw)[ichan]=ichan;
      }
    }

    setFocusChan(Int(floor(ATMnChan()[ispw]/2)));
    ATMfRes=fWidth()[ispw]/ATMnChan()[ispw];

    os << "Spectral window " << fRefFreq()[ispw]/1e9 << "(ch " << ATMchanMap(ispw)[focusChan()] << "/"<<ATMnChan()[ispw] << ")" << LogIO::POST;

    itsSpecGrid->add(ATMnChan()[ispw],
		     ATMchanMap(ispw)[focusChan()]+1,
		     atm::Frequency(fRefFreq()[ispw],atm::Frequency::UnitHertz),
		     atm::Frequency(ATMfRes,atm::Frequency::UnitHertz));
  }

  if (itsRIP != 0) delete itsRIP;
  itsRIP = new atm::RefractiveIndexProfile(*itsSpecGrid,*itsatm);

  // used for Tebb(spw,chan)
  if (itsSkyStatus != 0) delete itsSkyStatus;
  itsSkyStatus = new atm::SkyStatus(*itsRIP);


  // DispersiveH20PhaseDelay is different frmo RIP as from Status

  //  /** Accesor to get the integrated zenith H2O Atmospheric Phase Delay (Dispersive part)
  //   for the current conditions, for a single frequency RefractiveIndexProfile object or
  //   for the point 0 of spectral window 0 of a multi-window RefractiveIndexProfile object.
  //   There is overloading. The same accessor exists in RefractiveIndexProfile but in that
  //   case the returned value corresponds to the zenith water vapor column of the AtmProfile object.*/
  //  Angle getDispersiveH2OPhaseDelay()

//  os << "DispersiveWetPathLength = "
//     << itsRIP->getDispersiveH2OPathLength().get(atm::Length::UnitMicron)
//     << " microns at "
//     << fRefFreq()[0]/1e9 << " GHz; dryOpacity = "
//     << itsRIP->getDryOpacity(currSpw(),focusChan()).get()
//     << ", DispersiveH2ODelay (RefIPfl) = "
//     << itsRIP->getDispersiveH2OPhaseDelay(currSpw(),focusChan()).get(atm::Angle::UnitDegree) << " deg"
//     << "DispersiveH2ODelay (SkyStat) = "
//     << itsSkyStatus->getDispersiveH2OPhaseDelay(currSpw(),focusChan()).get(atm::Angle::UnitDegree) << " deg"
//     << LogIO::POST;

  itsSkyStatus->setUserWH2O(atm::Length(mean_pwv(),atm::Length::UnitMilliMeter));  // set WH2O

  os << "After setting WH2O to " << itsSkyStatus->getUserWH2O().get(atm::Length::UnitMilliMeter) << LogIO::POST;

//   os << "after setting WH20 to " << itsSkyStatus->getUserWH2O().get(atm::Length::UnitMilliMeter)
//      << ", DispersiveH2ODelay (RefIPfl) = "
//     //<< itsRIP->getDispersiveH2OPhaseDelay(currSpw(),focusChan()).get(atm::Angle::UnitDegree) << " deg, "
//      << itsRIP->getDispersiveH2OPhaseDelay(itsRIP->getGroundWH2O(),currSpw(),focusChan()).get(atm::Angle::UnitDegree) << " deg, "
//      << "DispersiveH2ODelay (SkyStat) = "
//      << itsSkyStatus->getDispersiveH2OPhaseDelay(currSpw(),focusChan()).get(atm::Angle::UnitDegree) << "deg = "
//      << itsSkyStatus->getDispersiveH2OPhaseDelay(currSpw(),focusChan()).get(atm::Angle::UnitRadian) << "rad"
//      << LogIO::POST;

  // set focuschan back to spw0 value
  fChan=Int(floor(fnChan()[0]/2));
  setFocusChan(fChan);

  os << "Dry and Wet Opacity from RefractiveIndexProfile = " <<
    itsRIP->getDryOpacity(0,ATMchanMap(0)[fChan]).get(atm::Opacity::UnitNeper)
     << ", " <<
    itsRIP->getWetOpacity(atm::Length(mean_pwv(),atm::Length::UnitMilliMeter),0,
			  ATMchanMap(0)[fChan]).get(atm::Opacity::UnitNeper) <<
    " at " << fRefFreq()[0]/1e9 << " GHz (ch" << fChan  <<")" << LogIO::POST;

  os << "Dry and Wet Opacity from SkyStatus              = " <<
    itsSkyStatus->getDryOpacity(0,ATMchanMap(0)[fChan]).get(atm::Opacity::UnitNeper) << ", " <<
    itsSkyStatus->getWetOpacity(0,ATMchanMap(0)[fChan]).get(atm::Opacity::UnitNeper) << LogIO::POST;



  // go ahead and get Tebb from ends of ATM spw
  atm::Temperature t0 =
    itsSkyStatus->getTebbSky(0,0,atm::Length(mean_pwv(),atm::Length::UnitMilliMeter),1.0,spilleff(),tground());
  atm::Temperature t1 =
    itsSkyStatus->getTebbSky(0,ATMnChan()[0]-1,atm::Length(mean_pwv(),atm::Length::UnitMilliMeter),1.0,spilleff(),tground());
  atm::Temperature tm =
    itsSkyStatus->getTebbSky(0,ATMchanMap(0)[fChan],atm::Length(mean_pwv(),atm::Length::UnitMilliMeter),1.0,spilleff(),tground());
  // CHANINDEX
  atm::Frequency f0=itsSpecGrid->getChanFreq(0,0);
  atm::Frequency f1=itsSpecGrid->getChanFreq(0,ATMnChan()[0]-1);
  atm::Frequency fm=itsSpecGrid->getChanFreq(0,ATMchanMap(0)[fChan]);

  os << "Sky plus ground and CMB Brightness Temp across Spw 0, for spill="<<spilleff()
     << " Tground="<<tground()
     << " pwv="<<mean_pwv()
     << LogIO::POST;
  os << " Zenith Tebb["
     << f0.get(atm::Frequency::UnitGigaHertz) << "," << fm.get(atm::Frequency::UnitGigaHertz) << "," << f1.get(atm::Frequency::UnitGigaHertz) << "]=["
     << t0.get(atm::Temperature::UnitKelvin)   << "," << tm.get(atm::Temperature::UnitKelvin)   << "," << t1.get(atm::Temperature::UnitKelvin) << "]" << LogIO::POST;

}



void AtmosCorruptor::initialize(const Int rxtype) {
  // for testing only

  mode()="test";
  if (!times_initialized())
    throw(AipsError("logic error in AtmCorr::init() - slot times not initialized."));

  mean_pwv() = simpar().asFloat("mean_pwv");
  spilleff() = simpar().asFloat("spillefficiency");
  tground() = simpar().asFloat("tground");
  tatmos() = simpar().asFloat("tatmos");  // only used in manual mode
  tcmb() = simpar().asFloat("tcmb");
  trx() = simpar().asFloat("trx");
  rxType() = rxtype;
  // RI todo AtmCor:init() test is mean_pwv() ever set?
  if (freqDepPar()) initAtm();
  pwv_p.resize(nAnt(),false,true);
  for (uInt ia=0;ia<nAnt();++ia) {
    pwv_p[ia] = new Vector<Float>(nSim());
    // not really pwv, but this is a test mode
    for (uInt i=0;i<nSim();++i)
      (*(pwv_p[ia]))(i) = (Float(i)/Float(nSim()) + Float(ia)/Float(nAnt()))*mean_pwv()*10;
  }

  if (prtlev()>2) cout << "AtmCorruptor::init [test]" << endl;
}


// this one is for the M /T
// pass the VisCal::Type to it - the concept of the corruptor taking a VC
// instead of being a member of it.

  void AtmosCorruptor::initialize(const VisIter& vi, const Record& simpar, VisCal::Type /*type*/, const Int rxtype) {

  LogIO os(LogOrigin("AtmCorr", "init(vi,par,type)", WHERE));

  if (prtlev()>3) cout<<" AtmCorr::init(vi,par,type)"<<endl;

  // start with amp defined as something
  amp()=1.0;
  if (simpar.isDefined("amplitude")) amp()=simpar.asFloat("amplitude");

  // simple means just multiply by amp()
  mode()="simple";
  if (simpar.isDefined("mode")) mode()=simpar.asString("mode");

  if (mode()=="simple") {
    freqDepPar()=false;
    if (prtlev()>2) cout << "AtmCorruptor::init [simple scale by " << amp() << "]" << endl;

  } else {
    antDiams = vi.msColumns().antenna().dishDiameter().getColumn();

    if (simpar.isDefined("mean_pwv")) mean_pwv() = simpar.asFloat("mean_pwv");
    if (simpar.isDefined("spillefficiency")) spilleff() = simpar.asFloat("spillefficiency");
    if (simpar.isDefined("tground")) tground() = simpar.asFloat("tground");
    if (simpar.isDefined("tatmos")) tatmos() = simpar.asFloat("tatmos");  // only used in manual mode
    if (simpar.isDefined("tcmb")) tcmb() = simpar.asFloat("tcmb");
    if (simpar.isDefined("trx")) trx() = simpar.asFloat("trx");
    rxType() = rxtype;
    if (simpar.isDefined("senscoeff")) senscoeff() = simpar.asFloat("senscoeff");

    // use ATM but no time fluctuation of atm - e.g. Tf [Tsys scaling, also Mf]
    if (freqDepPar()) initAtm();
    // even if not freqdeppar, we need to set this so that tsys and focusFreq work
    currSpw()=0;
    setFocusChan(Int(floor(nChan()/2)));


    if (mode()=="tsys-manual") {
      // user is specifying Tatmos and tau.
      // we can still use ATM to provide the tau _scaling_ with frequency,
      // but its only approximate since it doesn't integrate properly through
      // the atmosphere to recover T_ebb
      tauscale()=simpar.asFloat("tau0");
      // find tau in center of band for current ATM parameters
      os << "manual tau set to " << tauscale();

      if (freqDepPar()) {
	tauscale()/=opac(focusChan());
	os << "Using frequency scaling from ATM, but not absolute tau - not self-consistent" << endl; }
      // if freqDepPar() then opac will be called in simPar and multiplied by tauscale

      // there is no check that Tatmos is consistent with the ATM model.
      // that's why using this in freqDepPar() mode, although it should work,
      // may not be the best idea.
      // modified from mm::simPar;  Tsys = Tsys0 + Tsys1*exp(+tau)
      // full calculation moved to tsys()

    } else {
      // tsys-atm
      tauscale()=1.;
      // RI todo AtmCorr::init throw exception if not freqDepPar() here?
    }

    // even if not freqdeppar, we need to set this so that tsys and focusFreq work
    currSpw()=0;
    setFocusChan(Int(floor(nChan()/2)));

    os << LogIO::NORMAL
       << "Zenith Tsys at center of first Spectral Window = " << tsys(1.0)
       << " tground=" << tground() << " spillover=" << spilleff()
       << LogIO::POST;
    if (tsys(1.0)>1e6 or tsys(1.0)<=0) {
      ostringstream o;
      o << "error in ATM setup - Tsys " << tsys(1.0) << " poorly defined - check inputs";
      throw(AipsError(o));
      //      throw(AipsError("error in ATM setup - Tsys poorly defined - check inputs"));
    }
    if (freqDepPar())
      // note - atm init was done assuming refchan= nchan/2
      os << " pwv="<< mean_pwv() << " tau=" << opac(focusChan()) << "(ch" << focusChan() << ")" << LogIO::POST;
    else
      os << " tauscale=" << tauscale() << LogIO::POST;

    // conversion to Jy when divided by D1D2 for an M,
    // extra sqrt(2) for application to real & imag is applied as
    // senscoeff()=1/sqrt(2) for interferometer, and 1 for single dish
    //    amp() = 4 * C::sqrt2 * 1.38062e-16 * 1e23 * 1e-4 /
    amp() = 8 * 1.38062e-16 * 1e23 * 1e-4 /
      ( simpar.asFloat("antefficiency") *
	simpar.asFloat("correfficiency") * C::pi );
    // or divided by D for a T
    os << LogIO::DEBUG1 << " noise ~ " << amp()*1000 << "*Tsys/D^2/sqrt(dnu dt)/Nant" << LogIO::POST;
  }

  if (prtlev()>3) cout<<" ~AtmCorr::init(vi,par,type)"<<endl;

}


Float AtmosCorruptor::tsys(const Float& airmass) {
  double tau=0.;
  /* plank=6.6262e-34,boltz=1.3806E-23 */
  double hn_k = 4.80145e-11*focusFreq();  /* OT value - less accurate */
  double R=0.;
  double g=rxType();

  if (freqDepPar()) {  // for AtmosCorruptor, freqDep means use ATM
    // reference Tsys to top of atmosphere
    // opac does its own map to ATMchan
    tau = opac(focusChan()) * airmass;

    if (mode()=="tsys-atm") {
      // most general accessor:
      atm::Temperature tatmosatm =
	itsSkyStatus->getTebbSky(currSpw(),ATMchanMap(currSpw())[focusChan()],
				 atm::Length(mean_pwv(),atm::Length::UnitMilliMeter),airmass,
				 spilleff(),tground());
      // 1/e(hn/kt)-1 recalculated for us by every setFocusChan

      // cmb is already in ATM Tebb (but not in manual one below)
      // and Trx don't need the plank correction
      // does Tebb need the plank?  2012/06/27 yes its a real temp
      R = exp(tau) * ( 1./(exp(hn_k/tatmosatm.get(atm::Temperature::UnitKelvin))-1.) + trx()/hn_k );
      //R = exp(tau) * ( tatmosatm.get(atm::Temperature::UnitKelvin)/hn_k + trx()/hn_k );

    } else {
      tau = tau*tauscale();
      // manual: tauscale = tau0/opac(band center)
      R = spilleff() * Rtcmb() +
	exp(tau) *
	( spilleff() * (1.-exp(-tau)) * Rtatmos() +
	  (1.-spilleff()) * Rtground() + trx()/hn_k );
	  //	  Rtrx() );
    }
  } else {
    // not freqDep - best for manual - just specify tau and no freq dependence.
    if (mode()=="tsys-atm")
      throw(AipsError("non-freqDep AtmCorr::tsys called in unsupported ATM mode"));
    else {
      tau=tauscale()*airmass; // no reference to ATM here - it's not initialized!
      R = spilleff() * Rtcmb() +
	exp(tau) *
	( spilleff() * (1.-exp(-tau)) * Rtatmos() +
	  (1.-spilleff()) * Rtground() + trx()/hn_k);
	  // Rtrx() );
    }
  }
  //  return hn_k/log(1.+1./R);
  // we want to return a noise temp, i.e. with the plank correction
  return hn_k*R*(1+g);
}




// opacity - for screens, we'll need other versions of this like
// the different cphase calls that multiply wetopacity by fluctuation in pwv
Float AtmosCorruptor::opac(const Int ichan) {
  // CHANINDEX
  Float opac = itsRIP->getDryOpacity(currSpw(),ATMchanMap(currSpw())[ichan]).get() +
    //mean_pwv()*(itsRIP->getWetOpacity(currSpw(),ichan).get())  ;
    itsRIP->getWetOpacity(atm::Length(mean_pwv(),atm::Length::UnitMilliMeter),currSpw(),ATMchanMap(currSpw())[ichan]).get();
  return opac;
}






void AtmosCorruptor::initialize(const Int Seed, const Float Beta, const Float scale, const Int rxtype) {
  // individual delays for each antenna

  mean_pwv() = simpar().asFloat("mean_pwv");
  spilleff() = simpar().asFloat("spillefficiency");
  tground() = simpar().asFloat("tground");
  tatmos() = simpar().asFloat("tatmos");  // only used in manual mode
  tcmb() = simpar().asFloat("tcmb");
  trx() = simpar().asFloat("trx");
  rxType() = rxtype;
  initAtm();

  mode()="1d";
  // assumes nSim already changed to be even intervals in time by the caller -
  // i.e. if the solTimes are not even, this has been dealt with.
  if (!times_initialized())
    throw(AipsError("logic error in AtmCorr::init(Seed,Beta,scale) - slot times not initialized."));

  fBM* myfbm = new fBM(nSim());
  pwv_p.resize(nAnt(),false,true);
  for (uInt iant=0;iant<nAnt();++iant){
    pwv_p[iant] = new Vector<Float>(nSim());
    float pmean(0.),rms(0.);
    do {
      myfbm->initialize(Seed+iant,Beta); // (re)initialize
      *(pwv_p[iant]) = myfbm->data(); // iAnt()=iant; delay() = myfbm->data();
      pmean = mean(*(pwv_p[iant]));
      rms = sqrt(mean( (*(pwv_p[iant])-pmean)*(*(pwv_p[iant])-pmean) ));
    } while (!isnormal(rms));
    if (prtlev()>3 and currAnt()<2) {
      cout << "RMS fBM fluctuation for antenna " << iant
	   << " = " << rms << " ( " << pmean << " ; beta = " << Beta << " ) " << endl;
    }
    // scale is RELATIVE ie. pwv_p has rms=scale
    for (uInt islot=0;islot<nSim();++islot)
      (*(pwv_p[iant]))[islot] = (*(pwv_p[iant]))[islot]*scale/rms;
    if (prtlev()>2 and currAnt()<5) {
      float pmean = mean(*(pwv_p[iant]));
      float rms = sqrt(mean( (*(pwv_p[iant])-pmean)*(*(pwv_p[iant])-pmean) ));
      cout << "RMS fractional fluctuation for antenna " << iant
	   << " = " << rms << " ( " << pmean << " ) "
	// << " lscale = " << lscale
	   << endl;
    }
    currAnt()=iant;
  }

  if (prtlev()>2) cout << "AtmCorruptor::init [1d]" << endl;

}




  void AtmosCorruptor::initialize(const Int Seed, const Float Beta, const Float scale, const Int rxtype, const MSAntennaColumns& antcols) {
  // 2d delay screen
  LogIO os(LogOrigin("AtmCorr", "init(Seed,Beta,Scale,AntCols)", WHERE));
  mean_pwv() = simpar().asFloat("mean_pwv");
  spilleff() = simpar().asFloat("spillefficiency");
  tground() = simpar().asFloat("tground");
  tatmos() = simpar().asFloat("tatmos");  // only used in manual mode
  tcmb() = simpar().asFloat("tcmb");
  trx() = simpar().asFloat("trx");
  rxType() = rxtype;
  initAtm();

  mode()="2d";

  if (!times_initialized())
    throw(AipsError("logic error in AtmCorr::init(Seed,Beta,scale) - slot times not initialized."));

   // figure out where the antennas are, for blowing a phase screen over them
  // and how big they are, to set the pixel scale of the screen
  Float mindiam = min(antcols.dishDiameter().getColumn()); // units? dDQuant()?
  pixsize() = 0.5*mindiam; // RI_TODO temp compensate for lack of screen interpolation
  nAnt()=antcols.nrow();
  antx().resize(nAnt());
  anty().resize(nAnt());
  MVPosition ant;
  for (uInt i=0;i<nAnt();i++) {
    ant = antcols.positionMeas()(i).getValue();
    // have to convert to ENU or WGS84
    // ant = MPosition::Convert(ant,MPosition::WGS84)().getValue();
    // RI_TODO do this projection properly
    antx()[i] = ant.getLong()*6371000.;
    anty()[i] = ant.getLat()*6371000.; // m
  }
  // from SDTableIterator
  //// but this expects ITRF XYZ, so make a Position and convert
  //obsPos = MPosition(Quantity(siteElev_.asdouble(thisRow_), "m"),
  //			 Quantity(siteLong_.asdouble(thisRow_), "deg"),
  //			 Quantity(siteLat_.asdouble(thisRow_), "deg"),
  //			 MPosition::WGS84);
  //obsPos = MPosition::Convert(obsPos, MPosition::ITRF)();
  Float meanlat=mean(anty())/6371000.;
  antx()=antx()*cos(meanlat);
  if (prtlev()>5)
    cout << antx() << endl << anty() << endl;
  Int buffer(2); // # pix border
  //antx()=antx()-mean(antx());
  //anty()=anty()-mean(anty());
  antx()=antx()-min(antx());
  anty()=anty()-min(anty());
  antx()=antx()/pixsize();
  anty()=anty()/pixsize();
  if (prtlev()>3) {
    os << antx() << LogIO::POST;
    os << anty() << LogIO::POST;
  }

  Int ysize(Int(ceil(max(anty())+buffer)));

  const Float tracklength = stopTime()-startTime();
  const Float blowlength = windspeed()*tracklength*1.05; // 5% margin
  if (prtlev()>3)
    cout << "blowlength: " << blowlength << " track time = " << tracklength << endl;

  Int xsize(Int(ceil(max(antx())+buffer+blowlength/pixsize())));

  if (prtlev()>3)
    cout << "xy screen size = " << xsize << "," << ysize <<
      " pixels (" << pixsize() << "m)" << endl;
  // if the array is too elongated, FFT sometimes gets upset;
  if (Float(xsize)/Float(ysize)>5) ysize=xsize/5;

  if (prtlev()>2)
    cout << "creating new fBM of size " << xsize << "," << ysize << " (may take a few minutes) ... " << endl;
  os << LogIO::POST << "creating new fBM of size " << xsize << "," << ysize << " (may take a few minutes) ... " << LogIO::POST;

  fBM* myfbm = new fBM(xsize,ysize);
  screen_p = new Matrix<Float>(xsize,ysize);
  Float pmean(0.),rms(0.);
  Int ct(0);
  do {
    myfbm->initialize(Seed,Beta);
    *screen_p=myfbm->data();
    if (prtlev()>3) cout << " fBM created" << endl;
    pmean = mean(*screen_p);
    rms = sqrt(mean( ((*screen_p)-pmean)*((*screen_p)-pmean) ));
    ct++;
  } while (!(isnormal(rms) and pmean<1 and pmean>0) and ct<5);
  if (ct>=5) os << "possible issue with fBM init:" << LogIO::DEBUG1;
  // if (prtlev()>4) cout << (*screen_p)[10] << endl;
  if (currAnt()<2) {
    if (prtlev()>3)
      cout << "RMS screen fluctuation "
	   << " = " << rms << " ( mean = " << pmean << " ; beta = " << Beta << " ) " << endl;
    os << "RMS screen fluctuation "
       << " = " << rms << " ( mean = " << pmean << " ; beta = " << Beta << " ; scale = " << scale << "  ) " << LogIO::DEBUG1;
  }
  // scale is RELATIVE i,e, screen has rms=scale
  *screen_p = myfbm->data() * scale/rms;

  if (prtlev()>2) cout << "AtmCorruptor::init [2d] scale= " << scale << endl;

  // carefully delete this - screen_p should have a copy, not just a ref.
  delete myfbm;
  myfbm=0;

}




Complex AtmosCorruptor::cphase(const Int ix, const Int iy, const Int ichan) {
  // this is the complex phase gain for a T
  // expects pixel positions in screen - already converted using the pixscale
  // of the screen, and modified for off-zenith pointing

  AlwaysAssert(mode()=="2d",AipsError);
  float delay;
  ostringstream o;

  if (curr_slot()>=0 and static_cast<uInt>(curr_slot())<nSim()) {
    // blow
    Int blown(Int(floor( (slot_time(curr_slot())-slot_time(0)) *
			 windspeed()/pixsize() )));
    if (prtlev()>4 and currAnt()<2) cout << "blown " << blown << endl;

    if ((ix+blown)>(screen_p->shape())[0]) {
      o << "Delay screen blown out of range (" << ix << "+"
	<< blown << "," << iy << ") (" << screen_p->shape() << ")" << endl;
      throw(AipsError(o));
    }
    // RI TODO Atmcorr::cphase  interpolate screen!
    //
    // the screen, and this deltapwv, is a _fractional_ variation delta/mean
    // so a fractional pwv fluctuation turns into a fractional wet phase
    // fluctuation
    float deltapwv = (*screen_p)(ix+blown,iy);
    //    delay = itsRIP->getDispersiveH2OPhaseDelay(currSpw(),ichan).get(atm::Angle::UnitRadian)
    //      * deltapwv / 57.2958; // convert from deg to rad
    // Skystatus delay scales with userWH0 which was set in initialize to mean_pwv
    delay = itsSkyStatus->getDispersiveH2OPhaseDelay(currSpw(),ATMchanMap(currSpw())[ichan]).get(atm::Angle::UnitRadian) * deltapwv;
    //cout << "delay=" << delay << " " << Complex(cos(delay),sin(delay))<<endl;

    return Complex(cos(delay),sin(delay));
  } else {
    o << "atmosCorruptor::cphase: slot " << curr_slot() << "out of range!" <<endl;
    throw(AipsError(o));
    return Complex(1.);
  }
}


Complex AtmosCorruptor::cphase(const Int ichan) {
  AlwaysAssert(mode()=="1d" or mode()=="test",AipsError);
  float delay;

  if (curr_slot()>=0 and static_cast<uInt>(curr_slot())<nSim()) {
    // if this gets used in the future,
    // be careful about using freq if not freqDepPar()
    // Float freq = fRefFreq()[currSpw()] +
    //   Float(ichan) * (fWidth()[currSpw()]/Float(fnChan()[currSpw()]));

    if (currAnt()<=pwv_p.nelements()) {
    // pwv_p is a _fractional_ variation delta/mean
    // so a fractional pwv fluctuation turns into a fractional wet phase
    // fluctuation
      float deltapwv = (*pwv_p[currAnt()])(curr_slot());
      // CHANINDEX
      //      delay = itsRIP->getDispersiveH2OPhaseDelay(currSpw(),ichan).get(atm::Angle::UnitRadian)
      //	* deltapwv / 57.2958; // convert from deg to rad
      // the acessor from RIP doesn't scale with WH20, but skystatus' accessor does
      delay = itsSkyStatus->getDispersiveH2OPhaseDelay(currSpw(),ATMchanMap(currSpw())[ichan]).get(atm::Angle::UnitRadian) * deltapwv;
    } else
      throw(AipsError("AtmCorruptor internal error accessing pwv()"));
    return Complex(cos(delay),sin(delay));
  } else {
    cout << "AtmCorruptor::cphase: slot " << curr_slot() << "out of range!" <<endl;
    return Complex(1.);
  }
}











//###################################  fractional brownian motion


fBM::fBM(uInt i1) :
  initialized_(false)
{ data_ = new Vector<Float>(i1); };

fBM::fBM(uInt i1, uInt i2) :
  initialized_(false)
{ data_ = new Matrix<Float>(i1,i2); };

fBM::fBM(uInt i1, uInt i2, uInt i3) :
  initialized_(false)
{ data_ = new Cube<Float>(i1,i2,i3); };

void fBM::initialize(const Int seed, const Float beta) {

  MLCG rndGen_p(seed,seed);
  Normal nDist_p(&rndGen_p, 0.0, 1.0); // sigma=1.
  Uniform uDist_p(&rndGen_p, 0.0, 1.0);
  IPosition s = data_->shape();
  uInt ndim = s.nelements();
  Float amp,phase,pi=3.14159265358979;
  //UNUSED: uInt i0,j0;

  // FFTServer<Float,Complex> server;
  // This class assumes that a Complex array is stored as
  // pairs of floating point numbers, with no intervening gaps,
  // and with the real component first ie.,
  // <src>[re0,im0,re1,im1, ...]</src>. This means that the
  // following type casts work,
  // <srcblock>
  // S * complexPtr;
  // T * realPtr = (T * ) complexPtr;
  // </srcblock>

  Int stemp(s(0));
  if (ndim>1)
    stemp=s(1);

  IPosition size(1,s(0));
  IPosition size2(2,s(0),stemp);
  // takes a lot of thread thrashing to resize the server but I can't
  // figure a great way around the scope issues to just define a 2d one
  // right off the bat
  FFTServer<Float,Complex> server(size);

  Vector<Complex> F(s(0)/2);
  Vector<Float> G; // size zero to let FFTServer calc right size

  Matrix<Complex> F2(s(0)/2,stemp/2);
  Matrix<Float> G2;

  // FFTServer C,R assumes that the input is hermitian and only has
  // half of the elements in each direction

  switch(ndim) {
  case 1:
    // beta = 1+2H = 5-2D
    for (uInt i=0; i<s(0)/2-1; i++) {
      // data_->operator()(IPosition(1,i))=5.;
      phase = 2.*pi*uDist_p();
      // RI TODO is this assuming the origin is at 0,0 in which case
      // we should be using FFTServer::fft0 ?
      amp = pow(Float(i+1), -0.5*beta) * nDist_p();
      F(i)=Complex(amp*cos(phase),amp*sin(phase));
      // F(s(0)-i)=Complex(amp*cos(phase),-amp*sin(phase));
    }
    server.fft(G,F,false);  // complex to real Xform
    // G comes out twice length of F
    for (uInt i=0; i<s(0); i++)
      data_->operator()(IPosition(1,i)) = G(i); // there has to be a better way with strides or something.
    //    cout << endl << F(2) << " -> " << G(2) << " -> "
    //	 << data_->operator()(IPosition(1,2)) << endl;
    break;
  case 2:
    // beta = 1+2H = 7-2D
    server.resize(size2);
    // RI_TODO fBM::init make sure hermitian calc is correct - fftw only doubles first axis.
    // F2.resize(s(0)/2,s(1)/2);
    F2.resize(s(0)/2,s(1));
    for (uInt i=0; i<s(0)/2; i++)
      // for (uInt j=0; j<s(1)/2; j++) {
      for (uInt j=0; j<s(1); j++) {
	phase = 2.*pi*uDist_p();
	// RI TODO is this assuming the origin is at 0,0 in which case
	// we should be using FFTServer::fft0 ?
	if (i!=0 or j!=0) {
	  Float ij2 = sqrt(Float(i)*Float(i) + Float(j)*Float(j));
	  // RI_TODO still something not quite right with exponent
	  // amp = pow(ij2, -0.25*(beta+1)) * nDist_p();
	  amp = pow(ij2, -0.5*(beta+0.5) ) * nDist_p();
	} else {
	  amp = 0.;
	}
	F2(i,j)=Complex(amp*cos(phase),amp*sin(phase));
	// if (i==0) {

	//   i0=0;
	// } else {
	//   i0=s(0)-i;
	// }
	// do we need this ourselves in the second dimension?
	// if (j==0) {
	//   j0=0;
	// } else {
	//   j0=s(1)-j;
	// }
	// F2(i0,j0)=Complex(amp*cos(phase),-amp*sin(phase));
      }
    // The complex to real transform does not check that the
    // imaginary component of the values where u=0 are zero
    F2(s(0)/2,0)=Complex(F2(s(0)/2,0).real( ),0.0);
    F2(0,s(1)/2)=Complex(F2(0,s(1)/2).real( ),0.0);
    // cout << endl;
    F2(s(0)/2,s(1)/2)=Complex(F2(s(0)/2,s(1)/2).real( ),0.0);
    // for (uInt i=0; i<s(0)/2; i++)
    // 	for (uInt j=0; j<s(1)/2; j++) {
    // 	  phase = 2.*pi*uDist_p();
    // 	  amp = pow(Double(i)*Double(i) + Double(j)*Double(j),
    // 		    -0.25*(beta+1)) * nDist_p();
    // 	  F2(i,s(1)-j) = Complex(amp*cos(phase),amp*sin(phase));
    // 	  F2(s(0)-i,j) = Complex(amp*cos(phase),-amp*sin(phase));
    // 	}
    server.fft(G2,F2,false);  // complex to real Xform
    // G2 comes out sized s(0),s(1)/2 i.e. only doubles the first dim.
    // cout << G2.shape() << endl;
    // there has to be a better way
    for (uInt i=0; i<s(0); i++)
      for (uInt j=0; j<s(1); j++)
	data_->operator()(IPosition(2,i,j)) = G2(i,j);
    break;
  case 3:
    // beta = 1+2H = 9-2D
    throw(AipsError("no 3d fractional Brownian motion yet."));
    for (uInt i=0; i<s(0); i++)
      for (uInt j=0; j<s(1); j++)
	for (uInt k=0; j<s(3); k++)
	  data_->operator()(IPosition(3,i,j,k))=5.;
  }
};










//###################################

  Complex GJonesCorruptor::simPar(const VisIter& /*vi*/,VisCal::Type type,Int ipar) {
  if (type==VisCal::G || type==VisCal::B) {
    if (mode()=="fbm") {
      return gain(ipar,focusChan());
    } else if (mode()=="random") {
      // work-around to assure consistent random number sequence on different architectures.
      Double randR = (*nDist_p)();
      Double randI = (*nDist_p)();
      return Complex(randR*camp().real(),randI*camp().imag());
    } else throw(AipsError("unknown corruptor mode "+mode()));
  } else  throw(AipsError("GCorruptor: incompatible VisCal type "+VisCal::nameOfType(type)));
}


GJonesCorruptor::GJonesCorruptor(const Int nSim) :
  CalCorruptor(nSim),  // parent
  tsys_(0.0)
{}

GJonesCorruptor::~GJonesCorruptor() {
  if (prtlev()>2) cout << "GCorruptor::~GCorruptor()" << endl;
}


Matrix<Complex>* GJonesCorruptor::drift() {
  if (currAnt()<=drift_p.nelements())
    return drift_p[currAnt()];
  else
    return NULL;
};


void GJonesCorruptor::initialize() {
  // for testing only
  if (prtlev()>2) cout << "GCorruptor::init [test]" << endl;
}

void GJonesCorruptor::initialize(const Int Seed, const Float Beta, const Float scale) {
  // individual delays for each antenna
  if (mode()!="fbm") throw(AipsError("Attempt to use fBM initializer but mode is "+mode()));

  fBM* myfbm = new fBM(nSim());
  drift_p.resize(nAnt(),false,true);
  for (uInt iant=0;iant<nAnt();++iant) {
    drift_p[iant] = new Matrix<Complex>(nPar(),nSim());
    for (uInt icorr=0;icorr<nPar();++icorr){
      myfbm->initialize(Seed+iant+icorr,Beta); // (re)initialize
      float pmean = mean(myfbm->data());
      float rms = sqrt(mean( ((myfbm->data())-pmean)*((myfbm->data())-pmean) ));
      if (prtlev()>3 and currAnt()<2) {
	cout << "RMS fBM fluctuation for antenna " << iant
	     << " = " << rms << " ( " << pmean << " ; beta = " << Beta << " ) " << endl;
      }
      // amp
      Vector<Float> amp = (myfbm->data()) * scale/rms;
      for (uInt i=0;i<nSim();++i)  {
	(*(drift_p[iant]))(icorr,i) = Complex(1.+amp[i],0);
      }

      // phase
      myfbm->initialize((Seed+iant+icorr)*100,Beta); // (re)initialize
      pmean = mean(myfbm->data());
      rms = sqrt(mean( ((myfbm->data())-pmean)*((myfbm->data())-pmean) ));
      Vector<Float> angle = (myfbm->data()) * scale/rms * 3.141592; // *2 ?
      for (uInt i=0;i<nSim();++i)  {
	(*(drift_p[iant]))(icorr,i) *= exp(Complex(0,angle[i]));
      }

      currAnt()=iant;
    }
  }

  if (prtlev()>2) cout << "GCorruptor::init" << endl;

}


Complex GJonesCorruptor::gain(const Int icorr,const Int /*ichan*/) {
  if (curr_slot()>=0 and static_cast<uInt>(curr_slot())<nSim()
      and icorr>=0 and static_cast<uInt>(icorr)<nPar()) {
    if (currAnt()>drift_p.nelements())
      throw(AipsError("GJonesCorruptor internal error accessing drift()"));
    Complex delta = (*drift_p[currAnt()])(icorr,curr_slot());
    return delta;
  } else {
    cout << "GCorruptor::gain: slot " << curr_slot() << "out of range!" <<endl;
    return Complex(1.);
  }
}

} //casa namespace
