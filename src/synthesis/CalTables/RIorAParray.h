//# RIorAParray.h: Definition for RI/AP on-demand converter
//# Copyright (C) 2012                                     
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

#ifndef CALTABLES_RIORAPARRAY_H
#define CALTABLES_RIORAPARRAY_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/BasicSL/Complex.h>

namespace casa { //# NAMESPACE CASA - BEGIN                                                   


class RIorAPArray
{
public:
  // Construct empty
  RIorAPArray();

  // Construct from external casacore::Complex Array
  RIorAPArray(const casacore::Array<casacore::Complex>& c);

  // Construct from external casacore::Float Array
  RIorAPArray(const casacore::Array<casacore::Float>& f);

  // Destructor
  ~RIorAPArray();

  // Set data
  void setData(const casacore::Array<casacore::Complex>& c);
  void setData(const casacore::Array<casacore::Float>& f);

  // State
  void state(casacore::Bool verbose=false);

  // Render casacore::Complex version (calc from casacore::Float, if necessary)
  casacore::Array<casacore::Complex> c();

  // Render casacore::Float version (calc from casacore::Complex, if necessary)
  casacore::Array<casacore::Float> f(casacore::Bool trackphase);

private:

  // Resize internal target arrays
  void resizec_();
  void resizef_();

  // Perform the actual calculation
  void calc_c();
  void calc_f(casacore::Bool trackphase);

  // Unwind phase
  void trackPhase(casacore::Array<casacore::Float>& ph);

  // State of casacore::Complex and casacore::Float versions
  casacore::Bool c_ok_, f_ok_;

  casacore::Bool phaseTracked_;

  // Internal Arrays for casacore::Complex and casacore::Float versions
  casacore::Array<casacore::Complex> c_;
  casacore::Array<casacore::Float> f_;

};

} //# NAMESPACE CASA - END

#endif

