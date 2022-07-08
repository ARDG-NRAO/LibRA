//# PBMath1DEVLA.cc: Implementation for PBMath1DEVLA
//# Copyright (C) 2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  General Public
//# License for more details.
//#
//# You should have received a copy of the GNU General Public License
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
 
#include <synthesis/TransformMachines/PBMath1DEVLA.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <casacore/measures/Measures.h>
#include <casacore/measures/Measures/MeasConvert.h>
#include <casacore/measures/Measures/MDirection.h>

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

  PBMath1DEVLA::PBMath1DEVLA(): pbMathPoly_p(0)  {
    wideFit_p = true;
    init();
    maxRad_p=casacore::Quantity(1.0, "deg");
  }

  PBMath1DEVLA::PBMath1DEVLA(Quantity maxRad, Bool useSymmetricBeam, Double freqToUse): pbMathPoly_p(0), maxRad_p(maxRad), useSymmetric_p(useSymmetricBeam){
    wideFit_p=True;
    init();
    nearestVPArray(freqToUse, False);
  }

  PBMathInterface::PBClass PBMath1DEVLA::whichPBClass(){
    if(pbMathPoly_p)
      return pbMathPoly_p->whichPBClass();

    return PBMathInterface::COMMONPB;
  }

  void PBMath1DEVLA::fillPBArray(){
    ///This is a dummy to make compiler happy
    ///as in this class we recalculate vp_p when needed
    nearestVPArray(1.425e9, False);
    
  }
 
  void PBMath1DEVLA::nearestVPArray(Double freq, bool printINFO){
    LogIO os(LogOrigin("PBMATH1DEVLA", "nearestVPArray"));
	String band="";
	
	if(refim::SynthesisUtils::parseBandName(bandOrFeedName_p).nelements() > 0){
		band=refim::SynthesisUtils::parseBandName(bandOrFeedName_p)[0];
		band=band.after("EVLA_");
	}
	if(band==""){
       band=feed(freq);
	}
	Double origFreq=freq;
	limitFreqForBand(band, freq);
	
    auto confiter=feedConf_p.find(band);
    Float mag = 1.21;
    if(confiter != feedConf_p.end()){
      squint_p=BeamSquint(MDirection(Quantity((mag*cos(confiter->second)), "'"),
				     Quantity((mag*sin(confiter->second)), "'"),
				     MDirection::Ref(MDirection::AZEL)),
			  Quantity(1.0, "GHz"));
    }
    else{
      //No squint 
      squint_p=BeamSquint(MDirection(Quantity(0.0, "'"),
				     Quantity(0.0, "'"),
					      MDirection::Ref(MDirection::AZEL)),
			  Quantity(1.0, "GHz"));
    }
    Vector<Double> coeff;
    Double freqMHz=freq*1e-6;
    Double freqUsed=freq*1e-6;
    std::map<Double, std::vector<Double> >::iterator low, prev;
    low = coeffmap_p.lower_bound(freqMHz);
    if (low == coeffmap_p.end()) {
      --low;
      coeff=Vector<Double>(low->second);
      freqUsed=low->first;
    } 
    else if (low == coeffmap_p.begin()) {
      coeff=Vector<Double>(low->second);
      freqUsed=low->first;
    }
    else{
      prev = low;
      --prev;
      //cerr << "freqMHz " << freqMHz <<  " prev " << prev->first << " low " <<low->first  << endl; 
      if (fabs(freqMHz - prev->first) < fabs(low->first - freqMHz)){
        coeff=Vector<Double>(prev->second);
	freqUsed=prev->first;
      }
      else{
	coeff=Vector<Double>(low->second);
	freqUsed=low->first;
      }
      
    }
    if(printINFO)
        os << LogIO::NORMAL1 << "Using EVLA "+band+" band beam model of frequency  " << freqUsed << " MHz scaled to original frequency "<< origFreq*1e-6 << LogIO::POST;
    pbMathPoly_p= new PBMath1DPoly(coeff, maxRad_p, Quantity(1.0, "GHz"), False,  squint_p, useSymmetric_p);
    (this->vp_p).resize();
    (this->vp_p)=pbMathPoly_p->vp_p;
    //cerr << "vp_p " << vp_p << endl;
    maximumRadius_p=pbMathPoly_p->maximumRadius_p;
    refFreq_p=pbMathPoly_p->refFreq_p;
    fScale_p=pbMathPoly_p->fScale_p;
    inverseIncrementRadius_p=pbMathPoly_p->inverseIncrementRadius_p;
    scale_p=pbMathPoly_p->scale_p;
    composite_p=pbMathPoly_p->composite_p;
    //cerr << "maximumRad " << maximumRadius_p << " inverse " << inverseIncrementRadius_p << endl;
  }
  String PBMath1DEVLA::feed(const Double freq, const Double freqCenter){
    if((freqCenter > 224e6 && freqCenter<480e6) ||  (freq > 224e6 && freq<480e6))
      return "P";
     if((freqCenter > 900e6 && freqCenter<2003.0e6) ||  (freq > 900e6 && freq<2003e6))
       return "L";
     if((freqCenter > 1990e6 && freqCenter<4001.0e6) ||  (freq > 1990e6 && freq<4001e6))
       return "S";
     if((freqCenter > 3990e6 && freqCenter<8001.0e6) ||  (freq > 3990e6 && freq<8001e6))
       return "C";
      if((freqCenter > 7990e6 && freqCenter<12001.0e6) ||  (freq > 7990e6 && freq<12001e6))
       return "X";
      if((freqCenter > 12000e6 && freqCenter<18000.0e6) ||  (freq > 12000e6 && freq<18000e6))
       return "U";
      if((freqCenter > 19000e6 && freqCenter<26000.0e6) ||  (freq > 19000e6 && freq<26000e6))
       return "K";
      if((freqCenter > 28000e6 && freqCenter<38000.0e6) ||  (freq > 28000e6 && freq<38000e6))
       return "A";
      if((freqCenter > 41000e6 && freqCenter<50000.0e6) ||  (freq > 41000e6 && freq<50000e6))
       return "Q";

    return "";
  }
  void PBMath1DEVLA::limitFreqForBand(const casacore::String& band, casacore::Double& freq){
	  
	if(band=="P"){
		if(freq <= 224e6) freq=232e6;
		if(freq >=480e6) freq=470e6;
	}
	else if(band=="L"){
		if(freq <= 900e6) freq=1040e6;
		if(freq >=2000e6) freq=2000e6;
	}
	else if(band=="S"){
		if(freq < 2052e6) freq=2052e6;
		if(freq >=3948e6) freq=3948e6;
	}
	else if(band=="C"){
		if(freq < 4052e6) freq=4052e6;
		if(freq >=7948e6) freq=7948e6;
	}
	else if(band=="X"){
		if(freq < 8052e6) freq=8052e6;
		if(freq >=11948e6) freq=11948e6;
	}  
	else if(band=="U"){
		if(freq < 12052e6) freq=12052e6;
		if(freq >=17948e6) freq=17948e6;
	}  
	else if(band=="K"){
		if(freq < 19052e6) freq=19052e6;
		if(freq >=25948e6) freq=25948e6;
	}  
	else if(band=="A"){
		if(freq < 28052e6) freq=28052e6;
		if(freq >=38048e6) freq=38048e6;
	}  
	else if(band=="Q"){
		if(freq < 41052e6) freq=41052e6;
		if(freq >=43948e6) freq=43948e6;
	}  
  }

  void PBMath1DEVLA::init(){
    wFreqs_p=Vector<Double>(
        std::vector<Double>({232., 246., 281., 296., 312., 328., 344., 357., 382., 392., 403., 421., 458., 470., 1040, 1104, 1168, 1232, 1296, 1360, 1424, 1488, 1552, 1680, 1744, 1808, 1872, 1936, 2000})
    );
    wFreqs_p *=1e6;
    //Float mag = 1.21;  // half-squint magnitude in arcmin at 1 GHz)
    feedConf_p["L"] = (-185.9)*C::pi/180.0;    // squint orientation, rads, North of +AZ axis
    feedConf_p["S"] = (-11.61)*C::pi/180.0;
    feedConf_p["C"]=(-104.8)*C::pi/180.0;
    feedConf_p["X"]=(-113.7)*C::pi/180.0;
    feedConf_p["U"]=(42.4)*C::pi/180.0;
    feedConf_p["K"]=(64.4)*C::pi/180.0;
    feedConf_p["A"]=(106.9)*C::pi/180.0;
    feedConf_p["Q"]=(85.5)*C::pi/180.0;

    squint_p=BeamSquint( MDirection( Quantity(1.25, "arcmin"), Quantity(90.0, "deg"),
				       MDirection::Ref(MDirection::AZEL)), Quantity(1.0, "GHz"));
    ////P 
      coeffmap_p[232]={1.0, -1.137e-3, 5.19e-7, -1.04e-10, 0.71e-14};
      coeffmap_p[246]={1.0, -1.130e-3, 5.04e-7, -1.02e-10, 0.77e-14};
      coeffmap_p[281]={1.0, -1.106e-3, 5.11e-7, -1.10e-10, 0.91e-14};
      coeffmap_p[296]={1.0, -1.125e-3, 5.27e-7, -1.14e-10, 0.96e-14};
      coeffmap_p[312]={1.0, -1.030e-3, 4.44e-7, -0.89e-10, 0.68e-14};
      coeffmap_p[328]={1.0, -0.980e-3, 4.25e-7, -0.87e-10, 0.69e-14}; 
      coeffmap_p[344]={1.0, -0.974e-3, 4.09e-7, -0.76e-10, 0.53e-14};
      coeffmap_p[357]={1.0, -0.996e-3, 4.23e-7, -0.79e-10, 0.51e-14};
      coeffmap_p[382]={1.0, -1.002e-3, 4.39e-7, -0.88e-10, 0.64e-14};
      coeffmap_p[392]={1.0, -1.067e-3, 5.13e-7, -1.12e-10, 0.90e-14};
      coeffmap_p[403]={1.0, -1.057e-3, 4.90e-7, -1.06e-10, 0.87e-14};
      coeffmap_p[421]={1.0, -1.154e-3, 5.85e-7, -1.33e-10, 1.08e-14};
      coeffmap_p[458]={1.0, -0.993e-3, 4.67e-7, -1.04e-10, 0.88e-14};
      coeffmap_p[470]={1.0, -1.010e-3, 4.85e-7, -1.07e-10, 0.86e-14};
      /////////L
      coeffmap_p[1040]={ 1.000, -1.529e-3, 8.69e-7, -1.88e-10};
      coeffmap_p[1104]={ 1.000, -1.486e-3, 8.15e-7, -1.68e-10};  
      coeffmap_p[1168]={ 1.000, -1.439e-3, 7.53e-7, -1.45e-10}; 
      coeffmap_p[1232]={ 1.000, -1.450e-3, 7.87e-7, -1.63e-10}; 
      coeffmap_p[1296]={ 1.000, -1.428e-3, 7.62e-7, -1.54e-10};  
      coeffmap_p[1360]={ 1.000, -1.449e-3, 8.02e-7, -1.74e-10};  
      coeffmap_p[1424]={ 1.000, -1.462e-3, 8.23e-7, -1.83e-10}; 
      coeffmap_p[1488]={ 1.000, -1.455e-3, 7.92e-7, -1.63e-10};  
      coeffmap_p[1552]={ 1.000, -1.435e-3, 7.54e-7, -1.49e-10};  
      coeffmap_p[1680]={ 1.000, -1.443e-3, 7.74e-7, -1.57e-10};  
      coeffmap_p[1744]={ 1.000, -1.462e-3, 8.02e-7, -1.69e-10};  
      coeffmap_p[1808]={ 1.000, -1.488e-3, 8.38e-7, -1.83e-10};  
      coeffmap_p[1872]={ 1.000, -1.486e-3, 8.26e-7, -1.75e-10}; 
      coeffmap_p[1936]={ 1.000, -1.459e-3, 7.93e-7, -1.62e-10};  
      coeffmap_p[2000]={ 1.000, -1.508e-3, 8.31e-7, -1.68e-10};  
      ////////S
      coeffmap_p[2052]={1.000,-1.429e-3,7.52e-7,-1.47e-10};
      coeffmap_p[2180]={1.000,-1.389e-3,7.06e-7,-1.33e-10};
      coeffmap_p[2436]={1.000,-1.377e-3,6.90e-7,-1.27e-10};
      coeffmap_p[2564]={1.000,-1.381e-3,6.92e-7,-1.26e-10};
      coeffmap_p[2692]={1.000,-1.402e-3,7.23e-7,-1.40e-10};
      coeffmap_p[2820]={1.000,-1.433e-3,7.62e-7,-1.54e-10};
      coeffmap_p[2948]={1.000,-1.433e-3,7.46e-7,-1.42e-10};
      coeffmap_p[3052]={1.000,-1.467e-3,8.05e-7,-1.70e-10};
      coeffmap_p[3180]={1.000,-1.497e-3,8.38e-7,-1.80e-10};
      coeffmap_p[3308]={1.000,-1.504e-3,8.37e-7,-1.77e-10};
      coeffmap_p[3436]={1.000,-1.521e-3,8.63e-7,-1.88e-10};
      coeffmap_p[3564]={1.000,-1.505e-3,8.37e-7,-1.75e-10};
      coeffmap_p[3692]={1.000,-1.521e-3,8.51e-7,-1.79e-10};
      coeffmap_p[3820]={1.000,-1.534e-3,8.57e-7,-1.77e-10};
      coeffmap_p[3948]={1.000,-1.516e-3,8.30e-7,-1.66e-10};
      ///C
      coeffmap_p[4052]={1.000,-1.406e-3,7.41e-7,-1.48e-10};
      coeffmap_p[4180]={1.000,-1.385e-3,7.09e-7,-1.36e-10};
      coeffmap_p[4308]={1.000,-1.380e-3,7.08e-7,-1.37e-10};
      coeffmap_p[4436]={1.000,-1.362e-3,6.95e-7,-1.35e-10};
      coeffmap_p[4564]={1.000,-1.365e-3,6.92e-7,-1.31e-10};
      coeffmap_p[4692]={1.000,-1.339e-3,6.56e-7,-1.17e-10};
      coeffmap_p[4820]={1.000,-1.371e-3,7.06e-7,-1.40e-10};
      coeffmap_p[4948]={1.000,-1.358e-3,6.91e-7,-1.34e-10};
      coeffmap_p[5052]={1.000,-1.360e-3,6.91e-7,-1.33e-10};
      coeffmap_p[5180]={1.000,-1.353e-3,6.74e-7,-1.25e-10};
      coeffmap_p[5308]={1.000,-1.359e-3,6.82e-7,-1.27e-10};
      coeffmap_p[5436]={1.000,-1.380e-3,7.05e-7,-1.37e-10};
      coeffmap_p[5564]={1.000,-1.376e-3,6.99e-7,-1.31e-10};
      coeffmap_p[5692]={1.000,-1.405e-3,7.39e-7,-1.47e-10};
      coeffmap_p[5820]={1.000,-1.394e-3,7.29e-7,-1.45e-10};
      coeffmap_p[5948]={1.000,-1.428e-3,7.57e-7,-1.57e-10};
      coeffmap_p[6052]={1.000,-1.445e-3,7.68e-7,-1.50e-10};
      coeffmap_p[6148]={1.000,-1.422e-3,7.38e-7,-1.38e-10};
      coeffmap_p[6308]={1.000,-1.463e-3,7.94e-7,-1.62e-10};
      coeffmap_p[6436]={1.000,-1.478e-3,8.22e-7,-1.74e-10};
      coeffmap_p[6564]={1.000,-1.473e-3,8.00e-7,-1.62e-10};
      coeffmap_p[6692]={1.000,-1.455e-3,7.76e-7,-1.53e-10};
      coeffmap_p[6820]={1.000,-1.487e-3,8.22e-7,-1.72e-10};
      coeffmap_p[6948]={1.000,-1.472e-3,8.05e-7,-1.67e-10};
      coeffmap_p[7052]={1.000,-1.470e-3,8.01e-7,-1.64e-10};
      coeffmap_p[7180]={1.000,-1.503e-3,8.50e-7,-1.84e-10};
      coeffmap_p[7308]={1.000,-1.482e-3,8.19e-7,-1.72e-10};
      coeffmap_p[7436]={1.000,-1.498e-3,8.22e-7,-1.66e-10};
      coeffmap_p[7564]={1.000,-1.490e-3,8.18e-7,-1.66e-10};
      coeffmap_p[7692]={1.000,-1.481e-3,7.98e-7,-1.56e-10};
      coeffmap_p[7820]={1.000,-1.474e-3,7.94e-7,-1.57e-10};
      coeffmap_p[7948]={1.000,-1.448e-3,7.69e-7,-1.51e-10};
      //////X
      coeffmap_p[8052]={1.000,-1.403e-3,7.21e-7,-1.37e-10};
      coeffmap_p[8180]={1.000,-1.398e-3,7.10e-7,-1.32e-10};
      coeffmap_p[8308]={1.000,-1.402e-3,7.16e-7,-1.35e-10};
      coeffmap_p[8436]={1.000,-1.400e-3,7.12e-7,-1.32e-10};
      coeffmap_p[8564]={1.000,-1.391e-3,6.95e-7,-1.25e-10};
      coeffmap_p[8692]={1.000,-1.409e-3,7.34e-7,-1.49e-10};
      coeffmap_p[8820]={1.000,-1.410e-3,7.36e-7,-1.45e-10};
      coeffmap_p[8948]={1.000,-1.410e-3,7.34e-7,-1.43e-10};
      coeffmap_p[9052]={1.000,-1.403e-3,7.20e-7,-1.36e-10};
      coeffmap_p[9180]={1.000,-1.396e-3,7.09e-7,-1.31e-10};
      coeffmap_p[9308]={1.000,-1.432e-3,7.68e-7,-1.55e-10};
      coeffmap_p[9436]={1.000,-1.414e-3,7.43e-7,-1.47e-10};
      coeffmap_p[9564]={1.000,-1.416e-3,7.45e-7,-1.47e-10};
      coeffmap_p[9692]={1.000,-1.406e-3,7.26e-7,-1.39e-10};
      coeffmap_p[9820]={1.000,-1.412e-3,7.36e-7,-1.43e-10};
      coeffmap_p[9948]={1.000,-1.409e-3,7.29e-7,-1.39e-10};
      coeffmap_p[10052]={1.000,-1.421e-3,7.46e-7,-1.45e-10};
      coeffmap_p[10180]={1.000,-1.409e-3,7.25e-7,-1.36e-10};
      coeffmap_p[10308]={1.000,-1.402e-3,7.13e-7,-1.31e-10};
      coeffmap_p[10436]={1.000,-1.399e-3,7.09e-7,-1.29e-10};
      coeffmap_p[10564]={1.000,-1.413e-3,7.37e-7,-1.43e-10};
      coeffmap_p[10692]={1.000,-1.412e-3,7.34e-7,-1.41e-10};
      coeffmap_p[10820]={1.000,-1.401e-3,7.12e-7,-1.31e-10};
      coeffmap_p[10948]={1.000,-1.401e-3,7.12e-7,-1.31e-10};
      coeffmap_p[10052]={1.000,-1.401e-3,7.12e-7,-1.31e-10};
      coeffmap_p[11180]={1.000,-1.394e-3,6.99e-7,-1.24e-10};
      coeffmap_p[11308]={1.000,-1.394e-3,7.01e-7,-1.26e-10};
      coeffmap_p[11436]={1.000,-1.391e-3,6.94e-7,-1.22e-10};
      coeffmap_p[11564]={1.000,-1.389e-3,6.92e-7,-1.22e-10};
      coeffmap_p[11692]={1.000,-1.386e-3,6.80e-7,-1.15e-10};
      coeffmap_p[11820]={1.000,-1.391e-3,6.88e-7,-1.19e-10};
      coeffmap_p[11948]={1.000,-1.399e-3,6.97e-7,-1.22e-10};
      ///U
      coeffmap_p[12052]={1.000,-1.399e-3,7.17e-7,-1.34e-10};
      coeffmap_p[12180]={1.000,-1.392e-3,7.07e-7,-1.31e-10};
      coeffmap_p[12308]={1.000,-1.393e-3,7.19e-7,-1.38e-10};
      coeffmap_p[12436]={1.000,-1.393e-3,7.20e-7,-1.40e-10};
      coeffmap_p[12564]={1.000,-1.395e-3,7.19e-7,-1.38e-10};
      coeffmap_p[12692]={1.000,-1.397e-3,7.20e-7,-1.37e-10};
      coeffmap_p[12820]={1.000,-1.388e-3,7.06e-7,-1.32e-10};
      coeffmap_p[12948]={1.000,-1.397e-3,7.18e-7,-1.36e-10};
      coeffmap_p[13052]={1.000,-1.400e-3,7.27e-7,-1.40e-10};
      coeffmap_p[13180]={1.000,-1.406e-3,7.44e-7,-1.50e-10};
      coeffmap_p[13308]={1.000,-1.403e-3,7.37e-7,-1.47e-10};
      coeffmap_p[13436]={1.000,-1.392e-3,7.08e-7,-1.31e-10};
      coeffmap_p[13564]={1.000,-1.384e-3,6.94e-7,-1.24e-10};
      coeffmap_p[13692]={1.000,-1.382e-3,6.95e-7,-1.25e-10};
      coeffmap_p[13820]={1.000,-1.376e-3,6.88e-7,-1.24e-10};
      coeffmap_p[13948]={1.000,-1.384e-3,6.98e-7,-1.28e-10};
      coeffmap_p[14052]={1.000,-1.400e-3,7.36e-7,-1.48e-10};
      coeffmap_p[14180]={1.000,-1.397e-3,7.29e-7,-1.45e-10};
      coeffmap_p[14308]={1.000,-1.399e-3,7.32e-7,-1.45e-10};
      coeffmap_p[14436]={1.000,-1.396e-3,7.25e-7,-1.42e-10};
      coeffmap_p[14564]={1.000,-1.393e-3,7.20e-7,-1.39e-10};
      coeffmap_p[14692]={1.000,-1.384e-3,7.03e-7,-1.31e-10};
      coeffmap_p[14820]={1.000,-1.388e-3,7.06e-7,-1.32e-10};
      coeffmap_p[14948]={1.000,-1.393e-3,7.16e-7,-1.37e-10};
      coeffmap_p[15052]={1.000,-1.402e-3,7.38e-7,-1.48e-10};
      coeffmap_p[15180]={1.000,-1.407e-3,7.47e-7,-1.53e-10};
      coeffmap_p[15308]={1.000,-1.406e-3,7.41e-7,-1.48e-10};
      coeffmap_p[15436]={1.000,-1.399e-3,7.31e-7,-1.44e-10};
      coeffmap_p[15564]={1.000,-1.397e-3,7.28e-7,-1.43e-10};
      coeffmap_p[15692]={1.000,-1.401e-3,7.35e-7,-1.46e-10};
      coeffmap_p[15820]={1.000,-1.402e-3,7.34e-7,-1.45e-10};
      coeffmap_p[15948]={1.000,-1.399e-3,7.30e-7,-1.44e-10};
      coeffmap_p[16052]={1.000,-1.419e-3,7.59e-7,-1.54e-10};
      coeffmap_p[16180]={1.000,-1.419e-3,7.59e-7,-1.52e-10};
      coeffmap_p[16308]={1.000,-1.412e-3,7.40e-7,-1.44e-10};
      coeffmap_p[16436]={1.000,-1.407e-3,7.32e-7,-1.40e-10};
      coeffmap_p[16564]={1.000,-1.408e-3,7.32e-7,-1.41e-10};
      coeffmap_p[16692]={1.000,-1.410e-3,7.34e-7,-1.40e-10};
      coeffmap_p[16820]={1.000,-1.407e-3,7.27e-7,-1.38e-10};
      coeffmap_p[16948]={1.000,-1.423e-3,7.63e-7,-1.55e-10};
      coeffmap_p[17052]={1.000,-1.437e-3,7.87e-7,-1.66e-10};
      coeffmap_p[17180]={1.000,-1.438e-3,7.84e-7,-1.64e-10};
      coeffmap_p[17308]={1.000,-1.445e-3,7.98e-7,-1.71e-10};
      coeffmap_p[17436]={1.000,-1.452e-3,8.10e-7,-1.77e-10};
      coeffmap_p[17564]={1.000,-1.458e-3,8.13e-7,-1.70e-10};
      coeffmap_p[17692]={1.000,-1.456e-3,8.06e-7,-1.72e-10};
      coeffmap_p[17820]={1.000,-1.453e-3,8.00e-7,-1.68e-10};
      coeffmap_p[17948]={1.000,-1.452e-3,7.99e-7,-1.69e-10};
      /////K
      coeffmap_p[19052]={1.000,-1.419e-3,7.56e-7,-1.53e-10};
      coeffmap_p[19180]={1.000,-1.426e-3,7.70e-7,-1.59e-10};
      coeffmap_p[19308]={1.000,-1.433e-3,7.82e-7,-1.64e-10};
      coeffmap_p[19436]={1.000,-1.429e-3,7.73e-7,-1.60e-10};
      coeffmap_p[19564]={1.000,-1.427e-3,7.70e-7,-1.59e-10};
      coeffmap_p[19692]={1.000,-1.425e-3,7.65e-7,-1.56e-10};
      coeffmap_p[19820]={1.000,-1.430e-3,7.76e-7,-1.62e-10};
      coeffmap_p[19948]={1.000,-1.434e-3,7.81e-7,-1.63e-10};
      coeffmap_p[21052]={1.000,-1.448e-3,8.05e-7,-1.73e-10};
      coeffmap_p[21180]={1.000,-1.436e-3,7.84e-7,-1.63e-10};
      coeffmap_p[21308]={1.000,-1.441e-3,7.94e-7,-1.68e-10};
      coeffmap_p[21436]={1.000,-1.439e-3,7.89e-7,-1.66e-10};
      coeffmap_p[21564]={1.000,-1.442e-3,7.96e-7,-1.69e-10};
      coeffmap_p[21692]={1.000,-1.435e-3,7.81e-7,-1.61e-10};
      coeffmap_p[21820]={1.000,-1.442e-3,7.92e-7,-1.66e-10};
      coeffmap_p[21948]={1.000,-1.439e-3,7.82e-7,-1.61e-10};
      coeffmap_p[23052]={1.000,-1.401e-3,7.21e-7,-1.37e-10};
      coeffmap_p[23180]={1.000,-1.408e-3,7.31e-7,-1.41e-10};
      coeffmap_p[23308]={1.000,-1.407e-3,7.28e-7,-1.39e-10};
      coeffmap_p[23436]={1.000,-1.407e-3,7.31e-7,-1.41e-10};
      coeffmap_p[23564]={1.000,-1.419e-3,7.47e-7,-1.47e-10};
      coeffmap_p[23692]={1.000,-1.395e-3,7.10e-7,-1.33e-10};
      coeffmap_p[23820]={1.000,-1.413e-3,7.36e-7,-1.42e-10};
      coeffmap_p[23948]={1.000,-1.402e-3,7.21e-7,-1.36e-10};
      coeffmap_p[25052]={1.000,-1.402e-3,7.17e-7,-1.31e-10};
      coeffmap_p[25180]={1.000,-1.432e-3,7.73e-7,-1.58e-10};
      coeffmap_p[25308]={1.000,-1.407e-3,7.22e-7,-1.33e-10};
      coeffmap_p[25436]={1.000,-1.417e-3,7.43e-7,-1.45e-10};
      coeffmap_p[25564]={1.000,-1.422e-3,7.52e-7,-1.48e-10};
      coeffmap_p[25692]={1.000,-1.427e-3,7.59e-7,-1.52e-10};
      coeffmap_p[25820]={1.000,-1.416e-3,7.42e-7,-1.44e-10};
      coeffmap_p[25948]={1.000,-1.422e-3,7.46e-7,-1.45e-10};
      ///A
      coeffmap_p[28052]={1.000,-1.444e-3,7.61e-7,-1.44e-10};
      coeffmap_p[28180]={1.000,-1.439e-3,7.54e-7,-1.42e-10};
      coeffmap_p[28308]={1.000,-1.457e-3,7.87e-7,-1.58e-10};
      coeffmap_p[28436]={1.000,-1.457e-3,7.90e-7,-1.60e-10};
      coeffmap_p[28564]={1.000,-1.455e-3,7.87e-7,-1.59e-10};
      coeffmap_p[28692]={1.000,-1.458e-3,7.88e-7,-1.58e-10};
      coeffmap_p[28820]={1.000,-1.453e-3,7.81e-7,-1.56e-10};
      coeffmap_p[28948]={1.000,-1.460e-3,7.98e-7,-1.64e-10};
      coeffmap_p[31052]={1.000,-1.415e-3,7.44e-7,-1.44e-10};
      coeffmap_p[31180]={1.000,-1.408e-3,7.26e-7,-1.37e-10};
      coeffmap_p[31308]={1.000,-1.413e-3,7.28e-7,-1.36e-10};
      coeffmap_p[31436]={1.000,-1.394e-3,7.07e-7,-1.30e-10};
      coeffmap_p[31564]={1.000,-1.404e-3,7.23e-7,-1.37e-10};
      coeffmap_p[31692]={1.000,-1.427e-3,7.48e-7,-1.44e-10};
      coeffmap_p[31820]={1.000,-1.418e-3,7.48e-7,-1.48e-10};
      coeffmap_p[31948]={1.000,-1.413e-3,7.37e-7,-1.42e-10};
      coeffmap_p[34052]={1.000,-1.42e-3,7.28e-7,-1.34e-10};
      coeffmap_p[34180]={1.000,-1.46e-3,7.77e-7,-1.53e-10};
      coeffmap_p[34308]={1.000,-1.42e-3,7.41e-7,-1.42e-10};
      coeffmap_p[34436]={1.000,-1.42e-3,7.36e-7,-1.39e-10};
      coeffmap_p[34564]={1.000,-1.46e-3,7.76e-7,-1.52e-10};
      coeffmap_p[34692]={1.000,-1.42e-3,7.34e-7,-1.38e-10};
      coeffmap_p[34820]={1.000,-1.42e-3,7.34e-7,-1.39e-10};
      coeffmap_p[34948]={1.000,-1.45e-3,7.68e-7,-1.49e-10};
      coeffmap_p[37152]={1.000,-1.42e-3,7.47e-7,-1.44e-10};
      coeffmap_p[37280]={1.000,-1.41e-3,7.35e-7,-1.40e-10};
      coeffmap_p[37408]={1.000,-1.45e-3,7.65e-7,-1.46e-10};
      coeffmap_p[37536]={1.000,-1.41e-3,7.13e-7,-1.29e-10};
      coeffmap_p[37664]={1.000,-1.41e-3,7.30e-7,-1.38e-10};
      coeffmap_p[37792]={1.000,-1.45e-3,7.75e-7,-1.50e-10};
      coeffmap_p[37820]={1.000,-1.45e-3,7.68e-7,-1.49e-10};
      coeffmap_p[38048]={1.000,-1.41e-3,7.38e-7,-1.43e-10};
      //Q
      coeffmap_p[41052]={1.000,-1.453e-3,7.69e-7,-1.47e-10};
      coeffmap_p[41180]={1.000,-1.479e-3,8.03e-7,-1.61e-10};
      coeffmap_p[41308]={1.000,-1.475e-3,7.97e-7,-1.58e-10};
      coeffmap_p[41436]={1.000,-1.451e-3,7.73e-7,-1.51e-10};
      coeffmap_p[41564]={1.000,-1.450e-3,7.71e-7,-1.51e-10};
      coeffmap_p[41692]={1.000,-1.465e-3,7.79e-7,-1.49e-10};
      coeffmap_p[41820]={1.000,-1.460e-3,7.73e-7,-1.47e-10};
      coeffmap_p[41948]={1.000,-1.434e-3,7.47e-7,-1.40e-10};
      coeffmap_p[43052]={1.000,-1.428e-3,7.40e-7,-1.38e-10};
      coeffmap_p[43180]={1.000,-1.418e-3,7.29e-7,-1.34e-10};
      coeffmap_p[43308]={1.000,-1.433e-3,7.49e-7,-1.43e-10};
      coeffmap_p[43436]={1.000,-1.438e-3,7.55e-7,-1.45e-10};
      coeffmap_p[43564]={1.000,-1.419e-3,7.36e-7,-1.40e-10};
      coeffmap_p[43692]={1.000,-1.397e-3,7.13e-7,-1.33e-10};
      coeffmap_p[43820]={1.000,-1.423e-3,7.39e-7,-1.40e-10};
      coeffmap_p[43948]={1.000,-1.452e-3,7.68e-7,-1.47e-10};







    }






} //# NAMESPACE CASA - END
