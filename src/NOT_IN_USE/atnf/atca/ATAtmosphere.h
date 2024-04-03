//# ATAtmosphere.h: Model of atmospheric opacity
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

#ifndef ATNF_ATATMOSPHERE_H
#define ATNF_ATATMOSPHERE_H

//  includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/namespace.h>
namespace casa {

/**
  * This class implements opacity/atmospheric brightness temperature model
  * equivalent to the model available in MIRIAD. The actual math is a 
  * conversion of the Fortran code written by Bob Sault for MIRIAD.
  * It implements a simple model of the atmosphere and Liebe's model (1985) 
  * of the complex refractive index of air.
  *
  * The model of the atmosphere is one with an exponential fall-off in
  * the water vapour content (scale height of 1540 m) and a temperature lapse
  * rate of 6.5 mK/m. Otherwise the atmosphere obeys the ideal gas equation
  * and hydrostatic equilibrium.
  *
  * Note, the model includes atmospheric lines up to 800 GHz, but was not 
  * rigorously tested above 100 GHz and for instruments located at 
  * a significant elevation. For high-elevation sites it may be necessary to
  * adjust scale height and lapse rate.
  * 
  * @brief The ASAP atmosphere opacity model
  * @author Max Voronkov
  * @date $Date: 2010-03-17 14:55:17 +1000 (Thu, 26 Apr 2007) $
  * @version
  */
class ATAtmosphere {
public:
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
  explicit ATAtmosphere(Double wvScale = 1540., Double maxAlt = 10000.0, Int nLayers = 50);
   
  /**
   * Constructor with explicitly given parameters of the atmosphere 
   * @param[in] temperature air temperature at the observatory (K)
   * @param[in] pressure air pressure at the sea level if the observatory elevation 
   *            is set to non-zero value (note, by default is set to 200m) or at the 
   *            observatory ground level if the elevation is set to 0. (The value is in Pascals)
   * @param[in] pressure air pressure at the observatory (Pascals)
   * @param[in] humidity air humidity at the observatory (fraction)
   * @param[in] lapseRate temperature lapse rate (K/m), default is 0.0065 K/m to match MIRIAD and ISA 
   * @param[in] wvScale water vapour scale height (m), default is 1540m to match MIRIAD's model
   * @param[in] maxAlt maximum altitude of the model atmosphere (m), plane parallel layers are spread linearly up to
   *            this height, default is 10000m to match MIRIAD.
   * @param[in] nLayers number of plane parallel layers in the model (essentially for a numberical integration),
   *            default is 50 to match MIRIAD.
   **/
  ATAtmosphere(Double temperature, Double pressure, Double humidity, Double lapseRate = 0.0065, 
               Double wvScale = 1540., Double maxAlt = 10000.0, Int nLayers = 50);
   
  /**
   * Set the new weather station data, recompute the model 
   * @param[in] temperature air temperature at the observatory (K)
   * @param[in] pressure air pressure at the sea level if the observatory elevation 
   *            is set to non-zero value (note, by default is set to 200m) or at the 
   *            observatory ground level if the elevation is set to 0. (The value is in Pascals)
   * @param[in] humidity air humidity at the observatory (fraction)
   **/
  void setWeather(Double temperature, Double pressure, Double humidity);

  /**
   * Set the elevation of the observatory (height above mean sea level)
   *
   * The observatory elevation affects only interpretation of the pressure supplied as part 
   * of the weather data, if this value is non-zero, the pressure (e.g. in setWeather or 
   * constructor) is that at mean sea level. If the observatory elevation is set to zero, 
   * regardless on real elevation, the pressure is that at the observatory ground level.
   *
   * By default, 200m is assumed.
   * @param[in] elev elevation in metres
   **/
  void setObservatoryElevation(Double elev);

  /**
   * Calculate zenith opacity at the given frequency. This is a simplified version
   * of the routine implemented in MIRIAD, which calculates just zenith opacity and
   * nothing else. Note, that if the opacity is high, 1/sin(el) law is not correct 
   * even in the plane parallel case due to refraction. 
   * @param[in] freq frequency of interest in Hz
   * @return zenith opacity (nepers, i.e. dimensionless)
   **/
  Double zenithOpacity(Double freq) const;

  /**
   * Calculate zenith opacity for the range of frequencies. Same as zenithOpacity, but
   * for a Vector of frequencies.
   * @param[in] freqs Vector of frequencies in Hz
   * @return Vector of zenith opacities, one value per frequency (nepers, i.e. dimensionless)
   **/
  Vector<Double> zenithOpacities(const Vector<Double> &freqs) const;
  
  /**
   * Calculate opacity at the given frequency and elevation. This is a simplified 
   * version of the routine implemented in MIRIAD, which calculates just the opacity and
   * nothing else. In contract to zenithOpacity, this method takes into account refraction
   * and is more accurate than if one assumes 1/sin(el) factor.
   * @param[in] freq frequency of interest in Hz
   * @param[in] el elevation in radians
   * @return zenith opacity (nepers, i.e. dimensionless)
   **/ 
  Double opacity(Double freq, Double el) const;

  /**
   * Calculate opacities for the range of frequencies at the given elevation. Same as
   * opacity, but for a Vector of frequencies.
   * @param[in] freqs Vector of frequencies in Hz
   * @param[in] el elevation in radians
   * @return Vector of opacities, one value per frequency (nepers, i.e. dimensionless)
   **/
  Vector<Double> opacities(const Vector<Double> &freqs, Double el) const;
          
protected:
  /**
   * Build the atmosphere model based on exponential fall-off, ideal gas and hydrostatic
   * equilibrium. The model parameters are taken from the data members of this class.
   **/
  void recomputeAtmosphereModel();
  
  /**
   * Obtain the number of model layers, do consistency check that everything is
   * resized accordingly
   * @retrun number of model layers
   **/
  Int nLayers() const;
  
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
  static Double wvSaturationPressure(Double temperature);
   
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
  static DComplex dryRefractivity(Double freq, Double temperature, 
                     Double pDry, Double pVapour);
  
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
  static DComplex vapourRefractivity(Double freq, Double temperature, 
                     Double pDry, Double pVapour);
      
private:
  
  // heights of all model layers
  Vector<Double> itsHeights;
  
  // temperatures of all model layers
  Vector<Double> itsTemperatures;
  
  // partial pressures of dry component for all model layers
  Vector<Double> itsDryPressures;
  
  // partial pressure of water vapour for all model layers
  Vector<Double> itsVapourPressures;
  
  /**
   *  Atmosphere parameters
   **/
  
  // ground level temperature (K)
  Double itsGndTemperature;
  
  // sea level pressure (Pascals)
  Double itsPressure;
  
  // ground level humidity (fraction)
  Double itsGndHumidity;
  
  // lapse rate (K/m)
  Double itsLapseRate;
  
  // water vapour scale height (m)
  Double itsWVScale;
  
  // altitude of the highest layer of the model (m)
  Double itsMaxAlt;
  
  // observatory elevation (m)
  Double itsObsHeight;
};

} // namespace casa

#endif // #ifndef ATNF_ATATMOSPHERE_H

