//# ATAtmosphere.cc: Model of atmospheric opacity
//# Copyright (C) 2004
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
//#---------------------------------------------------------------------------

// own includes
#include "ATAtmosphere.h"

// casa includes
#include <casa/Utilities/Assert.h>
#include <casa/Quanta.h>

// std includes
//#include <cmath>

using namespace casacore;
namespace casa{

/**
 * Default Constructor (apart from optional parameters).
 * The class set up this way will assume International Standard Atmosphere (ISA) conditions,
 * except for humidity. The latter is assumed to be 50%, which seems more realistic for 
 * Australian telescopes than 0%. 
 * @param[in] wvScale water vapour scale height (m), default is 1540m to match MIRIAD's model
 * @param[in] maxAlt maximum altitude of the model atmosphere (m), plane parallel layers are spread linearly up to
 *            this height, default is 10000m to match MIRIAD.
 * @param[in] nLayers number of plane parallel layers in the model (essentially for a numberical integration),
 *            default is 50 to match MIRIAD.
 **/
ATAtmosphere::ATAtmosphere(Double wvScale, Double maxAlt, Int nLayers) :
   itsHeights(nLayers), itsTemperatures(nLayers), 
   itsDryPressures(nLayers), itsVapourPressures(nLayers),
   itsGndTemperature(288.), itsPressure(101325.), itsGndHumidity(0.5), 
   itsLapseRate(0.0065), itsWVScale(wvScale), itsMaxAlt(maxAlt), itsObsHeight(200.)
{
  recomputeAtmosphereModel();
}

/**
 * Constructor with explicitly given parameters of the atmosphere 
 * @param[in] temperature air temperature at the observatory (K)
 * @param[in] pressure air pressure at the sea level if the observatory elevation is set 
 *            (default is set to 200m) or at the observatory ground level if the elevation 
 *            is set to 0 (Pascals)
 * @param[in] humidity air humidity at the observatory (fraction)
 * @param[in] lapseRate temperature lapse rate (K/m), default is 0.0065 K/m to match MIRIAD and ISA 
 * @param[in] wvScale water vapour scale height (m), default is 1540m to match MIRIAD's model
 * @param[in] maxAlt maximum altitude of the model atmosphere (m), plane parallel layers are spread linearly up to
 *            this height, default is 10000m to match MIRIAD.
 * @param[in] nLayers number of plane parallel layers in the model (essentially for a numberical integration),
 *            default is 50 to match MIRIAD.
 **/
ATAtmosphere::ATAtmosphere(Double temperature, Double pressure, Double humidity, Double lapseRate, 
               Double wvScale, Double maxAlt, Int nLayers) :
   itsHeights(nLayers), itsTemperatures(nLayers), 
   itsDryPressures(nLayers), itsVapourPressures(nLayers),
   itsGndTemperature(temperature), itsPressure(pressure), itsGndHumidity(humidity), 
   itsLapseRate(lapseRate), itsWVScale(wvScale), itsMaxAlt(maxAlt), itsObsHeight(200.)
{
  recomputeAtmosphereModel();
}
               
/**
 * Set the new weather station data, recompute the model 
 * @param[in] temperature air temperature at the observatory (K)
 * @param[in] pressure air pressure at the sea level if the observatory elevation is set to non-zero value
 *            (default is set to 200m) or at the observatory ground level if the elevation  
 *            is set to 0 (Pascals)
 * @param[in] humidity air humidity at the observatory (fraction)
 **/
void ATAtmosphere::setWeather(Double temperature, Double pressure, Double humidity)
{
  itsGndTemperature = temperature;
  itsPressure = pressure;
  itsGndHumidity = humidity;
  recomputeAtmosphereModel();
}

/**
 * Set the elevation of the observatory (height above mean sea level)
 * It affects only interpretation of the pressure supplied as part of the weather data, if this value 
 * is non-zero, the pressure (e.g. in setWeather or constructor) is that at mean sea level. If the
 * observatory elevation is set to zero, regardless on real elevation, the pressure is that at the 
 * observatory ground level.
 * 
 * By default, 200m is assumed and the pressure should be a mean sea level pressure..
 * @param[in] elev elevation in metres
 **/
void ATAtmosphere::setObservatoryElevation(Double elev)
{
  itsObsHeight = elev;
  recomputeAtmosphereModel();  
}


/**
 * Build the atmosphere model based on exponential fall-off, ideal gas and hydrostatic
 * equilibrium. The model parameters are taken from the data members of this class.
 **/
void ATAtmosphere::recomputeAtmosphereModel()
{
  AlwaysAssert(itsGndTemperature > 0, AipsError);
  AlwaysAssert(itsPressure > 0., AipsError);
  AlwaysAssert((itsGndHumidity >= 0.) && (itsGndHumidity<=1.), AipsError);
  AlwaysAssert(itsMaxAlt > 0., AipsError);
  AlwaysAssert(itsWVScale > 0., AipsError);
  
  const Double heightStep = itsMaxAlt/Double(nLayers());
  // molar mass of the air
  const Double M = 28.96e-3;
  // free-fall acceleration
  const Double g = 9.81;
  
  const Double wvGndSaturationPressure = wvSaturationPressure(itsGndTemperature);
  const Double gndPressure = itsPressure*exp(-M*g/(QC::R( ).get().getValue()*itsGndTemperature)*
                   (itsObsHeight+0.5*itsLapseRate*itsObsHeight*itsObsHeight/itsGndTemperature));
  for (Int layer = 0; layer < nLayers(); ++layer) {
       const Double height = Double(layer)*heightStep;
       itsHeights[layer] = height;
       itsTemperatures[layer] = itsGndTemperature/(1.+itsLapseRate*height/itsGndTemperature);
       const Double pressure = gndPressure * exp(-M*g/(QC::R( ).get().getValue()*itsGndTemperature)*
                   (height+0.5*itsLapseRate*height*height/itsGndTemperature));
       itsVapourPressures[layer] = casacore::min(itsGndHumidity*exp(-height/itsWVScale)*wvGndSaturationPressure,
                                             wvSaturationPressure(itsTemperatures[layer]));
       itsDryPressures[layer] = pressure - itsVapourPressures[layer];                                      
       //std::cout<<"layer="<<layer<<": H="<<itsHeights[layer]<<" T="<<itsTemperatures[layer]<<
       //    " Pvap="<<itsVapourPressures[layer]<<" Pdry="<<itsDryPressures[layer]<<endl;
  }
}
  
/**
 * Obtain the number of model layers, do consistency check that everything is
 * resized accordingly
 * @retrun number of model layers
 **/
Int ATAtmosphere::nLayers() const
{
  const Int result = itsHeights.size();
  DebugAssert(result > 2, AipsError);
  DebugAssert(itsTemperatures.size() == result, AipsError);
  DebugAssert(itsDryPressures.size() == result, AipsError);
  DebugAssert(itsVapourPressures.size() == result, AipsError);  
  return result;
}

/**
 * Determine the saturation pressure of water vapour for the given temperature.
 *
 * Reference:
 * Waters, Refraction effects in the neutral atmosphere. Methods of
 * Experimental Physics, vol 12B, p 186-200 (1976).
 *   
 * @param[in] temperature temperature in K
 * @return vapour saturation pressure (Pascals) 
 **/
Double ATAtmosphere::wvSaturationPressure(Double temperature)
{
  if (temperature <= 215.) {
      return 0.;
  }
  const Double theta = 300.0/temperature;
  return 1e5/(41.51/std::pow(theta,5)*std::pow(10.,9.834*theta-10.0));
}

/**
 * Compute the complex refractivity of the dry components of the atmosphere
 * (oxygen lines) at the given frequency.
 * @param[in] freq frequency (Hz)
 * @param[in] temperature air temperature (K)
 * @param[in] pDry partial pressure of dry components (Pascals)
 * @param[in] pVapour partial pressure of water vapour (Pascals)
 * @return complex refractivity
 * 
 * Reference:
 * Liebe, An updated model for millimeter wave propogation in moist air,
 * Radio Science, 20, 1069-1089 (1985).
 **/
DComplex ATAtmosphere::dryRefractivity(Double freq, Double temperature, 
                     Double pDry, Double pVapour)
{
  // the number of parameters per atmospheric line and the number of lines taken into account
  const Int nLineParams = 7;
  const Int nLines = 48;
  // actual tabulated values
  const Double lines[nLines][nLineParams] = 
    {{49.452379,    0.12E-6, 11.830,  8.40E-3, 0.0,  5.60E-3,  1.7},
     {49.962257,    0.34E-6, 10.720,  8.50E-3, 0.0,  5.60E-3,  1.7},
     {50.474238,    0.94E-6,  9.690,  8.60E-3, 0.0,  5.60E-3,  1.7},
     {50.987748,    2.46E-6,  8.690,  8.70E-3, 0.0,  5.50E-3,  1.7},
     {51.503350,    6.08E-6,  7.740,  8.90E-3, 0.0,  5.60E-3,  1.8},
     {52.021409,   14.14E-6,  6.840,  9.20E-3, 0.0,  5.50E-3,  1.8},
     {52.542393,   31.02E-6,  6.000,  9.40E-3, 0.0,  5.70E-3,  1.8},
     {53.066906,   64.10E-6,  5.220,  9.70E-3, 0.0,  5.30E-3,  1.9},
     {53.595748,  124.70E-6,  4.480, 10.00E-3, 0.0,  5.40E-3,  1.8},
     {54.129999,  228.00E-6,  3.810, 10.20E-3, 0.0,  4.80E-3,  2.0},
     {54.671157,  391.80E-6,  3.190, 10.50E-3, 0.0,  4.80E-3,  1.9},
     {55.221365,  631.60E-6,  2.620, 10.79E-3, 0.0,  4.17E-3,  2.1},
     {55.783800,  953.50E-6,  2.115, 11.10E-3, 0.0,  3.75E-3,  2.1},
     {56.264777,  548.90E-6,  0.010, 16.46E-3, 0.0,  7.74E-3,  0.9},
     {56.363387, 1344.00E-6,  1.655, 11.44E-3, 0.0,  2.97E-3,  2.3},
     {56.968180, 1763.00E-6,  1.255, 11.81E-3, 0.0,  2.12E-3,  2.5},
     {57.612481, 2141.00E-6,  0.910, 12.21E-3, 0.0,  0.94E-3,  3.7},
     {58.323874, 2386.00E-6,  0.621, 12.66E-3, 0.0, -0.55E-3, -3.1},
     {58.446589, 1457.00E-6,  0.079, 14.49E-3, 0.0,  5.97E-3,  0.8},
     {59.164204, 2404.00E-6,  0.386, 13.19E-3, 0.0, -2.44E-3,  0.1},
     {59.590982, 2112.00E-6,  0.207, 13.60E-3, 0.0,  3.44E-3,  0.5},
     {60.306057, 2124.00E-6,  0.207, 13.82E-3, 0.0, -4.13E-3,  0.7},
     {60.434775, 2461.00E-6,  0.386, 12.97E-3, 0.0,  1.32E-3, -1.0},
     {61.150558, 2504.00E-6,  0.621, 12.48E-3, 0.0, -0.36E-3,  5.8},
     {61.800152, 2298.00E-6,  0.910, 12.07E-3, 0.0, -1.59E-3,  2.9},
     {62.411212, 1933.00E-6,  1.255, 11.71E-3, 0.0, -2.66E-3,  2.3},
     {62.486253, 1517.00E-6,  0.078, 14.68E-3, 0.0, -4.77E-3,  0.9},
     {62.997974, 1503.00E-6,  1.660, 11.39E-3, 0.0, -3.34E-3,  2.2},
     {63.568515, 1087.00E-6,  2.110, 11.08E-3, 0.0, -4.17E-3,  2.0},
     {64.127764,  733.50E-6,  2.620, 10.78E-3, 0.0, -4.48E-3,  2.0},
     {64.678900,  463.50E-6,  3.190, 10.50E-3, 0.0, -5.10E-3,  1.8},
     {65.224067,  274.80E-6,  3.810, 10.20E-3, 0.0, -5.10E-3,  1.9},
     {65.764769,  153.00E-6,  4.480, 10.00E-3, 0.0, -5.70E-3,  1.8},
     {66.302088,   80.09E-6,  5.220,  9.70E-3, 0.0, -5.50E-3,  1.8},
     {66.836827,   39.46E-6,  6.000,  9.40E-3, 0.0, -5.90E-3,  1.7},
     {67.369595,   18.32E-6,  6.840,  9.20E-3, 0.0, -5.60E-3,  1.8},
     {67.900862,    8.01E-6,  7.740,  8.90E-3, 0.0, -5.80E-3,  1.7},
     {68.431001,    3.30E-6,  8.690,  8.70E-3, 0.0, -5.70E-3,  1.7},
     {68.960306,    1.28E-6,  9.690,  8.60E-3, 0.0, -5.60E-3,  1.7},
     {69.489021,    0.47E-6, 10.720,  8.50E-3, 0.0, -5.60E-3,  1.7},
     {70.017342,    0.16E-6, 11.830,  8.40E-3, 0.0, -5.60E-3,  1.7},
     {118.750341,  945.00E-6,  0.000, 15.92E-3, 0.0, -0.44E-3,  0.9},
     {368.498350,   67.90E-6,  0.020, 19.20E-3, 0.6,  0.00E00,  1.0},
     {424.763120,  638.00E-6,  0.011, 19.16E-3, 0.6,  0.00E00,  1.0},
     {487.249370,  235.00E-6,  0.011, 19.20E-3, 0.6,  0.00E00,  1.0},
     {715.393150,   99.60E-6,  0.089, 18.10E-3, 0.6,  0.00E00,  1.0},
     {773.838730,  671.00E-6,  0.079, 18.10E-3, 0.6,  0.00E00,  1.0},
     {834.145330,  180.00E-6,  0.079, 18.10E-3, 0.6,  0.00E00,  1.0}};
     
  // convert to the units of Liebe
  const Double theta = 300./temperature;
  const Double kPaPVap = 0.001*pVapour;
  const Double kPaPDry = 0.001*pDry;
  const Double fGHz = freq * 1e-9;
  
  // some coefficients
  const Double ap = 1.4e-10*(1-1.2e-5*std::pow(fGHz,1.5));
  const Double gamma0 = 5.6e-3*(kPaPDry + 1.1*kPaPVap)*std::pow(theta,0.8);
  // initial refractivity
  DComplex result(2.588*kPaPDry*theta +
         3.07e-4*(1.0/(1.0+std::pow(fGHz/gamma0,2))-1)*kPaPDry*theta*theta,
         (2*3.07e-4/(gamma0*(1+std::pow(fGHz/gamma0,2))*(1+std::pow(fGHz/60,2))) + 
          ap*kPaPDry*std::pow(theta,2.5))*fGHz*kPaPDry*theta*theta);
          
  // sum the contributions of all the lines
  for (Int l = 0; l < nLines; ++l) {
       const Double S = lines[l][1]*kPaPDry*std::pow(theta,3)*exp(lines[l][2]*(1.-theta));
       const Double gamma = lines[l][3]*(kPaPDry*std::pow(theta,0.8-lines[l][4]) + 1.1*kPaPVap*theta);
       const Double delta = lines[l][5]*kPaPDry*std::pow(theta,lines[l][6]);
       const Double x = (lines[l][0]-fGHz)*(lines[l][0]-fGHz) + gamma*gamma;
       const Double y = (lines[l][0]+fGHz)*(lines[l][0]+fGHz) + gamma*gamma;
       const Double z = (lines[l][0]+gamma*gamma/lines[l][0]);
       result += DComplex (S*( (z-fGHz)/x + (z+fGHz)/y - 2./lines[l][0] + 
                                  delta*(1/x-1/y)*gamma*fGHz/lines[l][0]),
               S*( (1/x+1/y)*gamma*fGHz/lines[l][0] -
               delta*((lines[l][0]-fGHz)/x + (lines[l][0]+fGHz)/y)*fGHz/lines[l][0]));       
  }
  
  return result;
}

/**
 * Compute the complex refractivity of the water vapour monomers
 * at the given frequency.
 * @param[in] freq frequency (Hz)
 * @param[in] temperature air temperature (K)
 * @param[in] pDry partial pressure of dry components (Pascals)
 * @param[in] pVapour partial pressure of water vapour (Pascals)
 * @return complex refractivity
 * 
 * Reference:
 * Liebe, An updated model for millimeter wave propogation in moist air,
 * Radio Science, 20, 1069-1089 (1985).
 **/
DComplex ATAtmosphere::vapourRefractivity(Double freq, Double temperature, 
                     Double pDry, Double pVapour)
{
  // the number of parameters per atmospheric line and the number of lines taken into account
  const Int nLineParams = 4;
  const Int nLines = 30;
  // actual tabulated values
  const Double lines[nLines][nLineParams] = 
    {{22.235080,  0.1090, 2.143, 27.84E-3},
     {67.813960,  0.0011, 8.730, 27.60E-3},
     {119.995940,  0.0007, 8.347, 27.00E-3},
     {183.310117,  2.3000, 0.653, 28.35E-3},
     {321.225644,  0.0464, 6.156, 21.40E-3},
     {325.152919,  1.5400, 1.515, 27.00E-3},
     {336.187000,  0.0010, 9.802, 26.50E-3},
     {380.197372, 11.9000, 1.018, 27.60E-3},
     {390.134508,  0.0044, 7.318, 19.00E-3},
     {437.346667,  0.0637, 5.015, 13.70E-3},
     {439.150812,  0.9210, 3.561, 16.40E-3},
     {443.018295,  0.1940, 5.015, 14.40E-3},
     {448.001075, 10.6000, 1.370, 23.80E-3},
     {470.888947,  0.3300, 3.561, 18.20E-3},
     {474.689127,  1.2800, 2.342, 19.80E-3},
     {488.491133,  0.2530, 2.814, 24.90E-3},
     {503.568532,  0.0374, 6.693, 11.50E-3},
     {504.482692,  0.0125, 6.693, 11.90E-3},
     {556.936002, 510.000, 0.114, 30.00E-3},
     {620.700807,  5.0900, 2.150, 22.30E-3},
     {658.006500,  0.2740, 7.767, 30.00E-3},
     {752.033227, 250.000, 0.336, 28.60E-3},
     {841.073593,  0.0130, 8.113, 14.10E-3},
     {859.865000,  0.1330, 7.989, 28.60E-3},
     {899.407000,  0.0550, 7.845, 28.60E-3},
     {902.555000,  0.0380, 8.360, 26.40E-3},
     {906.205524,  0.1830, 5.039, 23.40E-3},
     {916.171582,  8.5600, 1.369, 25.30E-3},
     {970.315022,  9.1600, 1.842, 24.00E-3},
     {987.926764, 138.000, 0.178, 28.60E-3}};

  // convert to the units of Liebe
  const Double theta = 300./temperature;
  const Double kPaPVap = 0.001*pVapour;
  const Double kPaPDry = 0.001*pDry;
  const Double fGHz = freq * 1e-9;
 
  // initial refractivity
  DComplex result(2.39*kPaPVap*theta + 41.6*kPaPVap*theta*theta +
            6.47e-6*std::pow(fGHz,2.05)*kPaPVap*std::pow(theta,2.4),
            (0.915*1.40e-6*kPaPDry + 5.41e-5*kPaPVap*theta*theta*theta)*
             fGHz*kPaPVap*std::pow(theta,2.5));
             
  // sum contributions of all the lines 
  for (Int l = 0; l < nLines; ++l) {
       const Double S = lines[l][1]*kPaPVap*std::pow(theta,3.5)*exp(lines[l][2]*(1.-theta));
       const Double gamma = lines[l][3]*(kPaPDry*std::pow(theta,0.8) + 4.80*kPaPVap*theta);
       const Double x = (lines[l][0]-fGHz)*(lines[l][0]-fGHz) + gamma*gamma;
       const Double y = (lines[l][0]+fGHz)*(lines[l][0]+fGHz) + gamma*gamma;
       const Double z = (lines[l][0]+gamma*gamma/lines[l][0]);
       result += DComplex(S*((z-fGHz)/x + (z+fGHz)/y - 2./lines[l][0]),
                           S*((1./x+1./y)*gamma*fGHz/lines[l][0]));
  }
  
  return result; 
}

/**
 * Calculate zenith opacity at the given frequency. This is a simplified version
 * of the routine implemented in MIRIAD, which calculates just zenith opacity and
 * nothing else. Note, that if the opacity is high, 1/sin(el) law is not correct 
 * even in the plane parallel case due to refraction. 
 * @param[in] freq frequency of interest in Hz
 * @return zenith opacity (nepers, i.e. dimensionless)
 **/
Double ATAtmosphere::zenithOpacity(Double freq) const
{
  // essentially a numerical integration with the Trapezium method
  Double tau = 0.;
  for (Int layer = Int(nLayers()) - 1; layer>=0; --layer) {
       Double dH = 0.;
       if (layer == 0) {
           dH = 0.5*(itsHeights[1]-itsHeights[0]);
       } else if (layer + 1 == int(nLayers())) {
           dH = 0.5*(itsHeights[nLayers()-1]-itsHeights[nLayers()-2]);
       } else {
           dH = 0.5*(itsHeights[layer+1]-itsHeights[layer-1]);
       }
       // imaginary part of the total complex refractivity
       const Double nImag = 1e-6*std::imag(dryRefractivity(freq,itsTemperatures[layer],itsDryPressures[layer],
             itsVapourPressures[layer])+vapourRefractivity(freq,itsTemperatures[layer],itsDryPressures[layer],
             itsVapourPressures[layer]));
       tau += dH*4.*casacore::C::pi/QC::c( ).get().getValue()*freq*nImag;
  }
  return tau;
}

/**
 * Calculate zenith opacity for the range of frequencies. Same as zenithOpacity, but
 * for a vector of frequencies.
 * @param[in] freqs vector of frequencies in Hz
 * @return vector of zenith opacities, one value per frequency (nepers, i.e. dimensionless)
 **/
Vector<Double> ATAtmosphere::zenithOpacities(const Vector<Double> &freqs) const
{
  Vector<Double> result(freqs.size());
  for (uInt ch = 0; ch<freqs.size(); ++ch) {
       result[ch] = zenithOpacity(freqs[ch]);
  }
  return result;
}

/**
 * Calculate opacity at the given frequency and elevation. This is a simplified 
 * version of the routine implemented in MIRIAD, which calculates just the opacity and
 * nothing else. In contract to zenithOpacity, this method takes into account refraction
 * and is more accurate than if one assumes 1/sin(el) factor.
 * @param[in] freq frequency of interest in Hz
 * @param[in] el elevation in radians
 * @return zenith opacity (nepers, i.e. dimensionless)
 **/ 
Double ATAtmosphere::opacity(Double freq, Double el) const
{
  // essentially a numerical integration with the Trapezium method
  Double tau = 0.;
  const Double sineEl = sin(el);
  for (Int layer = Int(nLayers()) - 1; layer>=0; --layer) {
       Double dH = 0.;
       if (layer == 0) {
           dH = 0.5*(itsHeights[1]-itsHeights[0]);
       } else if (layer + 1 == int(nLayers())) {
           dH = 0.5*(itsHeights[nLayers()-1]-itsHeights[nLayers()-2]);
       } else {
           dH = 0.5*(itsHeights[layer+1]-itsHeights[layer-1]);
       }
       // total complex refractivity
       const DComplex n = dryRefractivity(freq,itsTemperatures[layer],itsDryPressures[layer], 
                                                      itsVapourPressures[layer]) + 
                                      vapourRefractivity(freq,itsTemperatures[layer],itsDryPressures[layer],
                                                      itsVapourPressures[layer]);
       // real and imaginary part of the total complex refractivity scaled appropriately
       const Double nImag = 1e-6*std::imag(n);
       const Double nReal = 1. + 1e-6*std::real(n);
       // length increment
       const Double dL = dH*nReal/sqrt(nReal*nReal+sineEl*sineEl-1.);
       tau += dL*4.*casacore::C::pi/QC::c( ).get().getValue()*freq*nImag;
  }
  return tau;  
}

/**
 * Calculate opacities for the range of frequencies at the given elevation. Same as
 * opacity, but for a vector of frequencies.
 * @param[in] freqs vector of frequencies in Hz
 * @param[in] el elevation in radians
 * @return vector of opacities, one value per frequency (nepers, i.e. dimensionless)
 **/
Vector<Double> ATAtmosphere::opacities(const Vector<Double> &freqs, Double el) const
{
  Vector<Double> result(freqs.size());
  for (uInt ch = 0; ch<freqs.size(); ++ch) {
       result[ch] = opacity(freqs[ch],el);
  }
  return result;
}

} //#end casa namespace

