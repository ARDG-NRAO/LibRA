//# VisSetUtil.h: Definitions for casacore::Stokes Image utilities
//# Copyright (C) 1996,1997,1998
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
//# $Id$

#ifndef MSVIS_VISSETUTIL_H
#define MSVIS_VISSETUTIL_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <msvis/MSVis/VisibilityIterator.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  class VisSet;

// <summary> 
// Utilities for operating on VisSets.
// </summary>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis> 
// </synopsis> 
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="">
// </todo>


class VisSetUtil {
  
public:
  // Calculate sensitivity
  static void Sensitivity(VisSet &vs, casacore::Matrix<casacore::Double>& mssFreqSel,casacore::Matrix<casacore::Int>& mssChanSel,
			  casacore::Quantity& pointsourcesens, casacore::Double& relativesens,
			  casacore::Double& sumwt,
			  casacore::Double& effectiveBandwidth,
			  casacore::Double& effectiveIntegration,
			  casacore::Int& nBaselines,
			  casacore::Vector<casacore::Vector<casacore::Int> >& nData,
			  casacore::Vector<casacore::Vector<casacore::Double> >& sumwtChan,
			  casacore::Vector<casacore::Vector<casacore::Double> >& sumwtsqChan,
			  casacore::Vector<casacore::Vector<casacore::Double> >& sumInverseVarianceChan);
  static void Sensitivity(ROVisibilityIterator &vi, 
			  casacore::Matrix<casacore::Double>& mssFreqSel,
			  casacore::Matrix<casacore::Int>& mssChanSel,
			  casacore::Quantity& pointsourcesens, 
			  casacore::Double& relativesens,
			  casacore::Double& sumwt,
			  casacore::Double& effectiveBandwidth,
			  casacore::Double& effectiveIntegration,
			  casacore::Int& nBaselines,
			  casacore::Vector<casacore::Vector<casacore::Int> >& nData,
			  casacore::Vector<casacore::Vector<casacore::Double> >& sumwtChan,
			  casacore::Vector<casacore::Vector<casacore::Double> >& sumwtsqChan,
			  casacore::Vector<casacore::Vector<casacore::Double> >& sumInverseVarianceChan);
  // Hanning smoothing of spectral channels
  static void HanningSmooth(VisSet &vs, const casacore::String& dataCol="corrected", 
			    const casacore::Bool& doFlagAndWeight=true);
  static void HanningSmooth(VisibilityIterator &vi, const casacore::String& dataCol="corrected",
			    const casacore::Bool& doFlagAndWeight=true);
  // Subtract/add model from/to corrected visibility data
  static void UVSub(VisSet &vs, casacore::Bool reverse=false);
  static void UVSub(VisibilityIterator &vs, casacore::Bool reverse=false);


  // Add and (optionally) initialize scratch columns
  static void addScrCols(casacore::MeasurementSet& ms, casacore::Bool addModel, casacore::Bool addCorr, 
			 casacore::Bool init, casacore::Bool compress);

  // Initialize scr cols
  static void initScrCols(casacore::MeasurementSet& ms, casacore::Bool initModel, casacore::Bool initCorr);
  static void initScrCols(VisibilityIterator& ms, casacore::Bool initModel, casacore::Bool initCorr);

  // Remove OTF model keywords
  static void remOTFModel(casacore::MeasurementSet& ms);

  // Remove physical scrcols
  static void remScrCols(casacore::MeasurementSet& ms, casacore::Bool remModel, casacore::Bool remCorr);

   // Remove an existing cal set (a CORRECTED_DATA and MODEL_DATA 
  // column set and, optionally, any associated compression columns)
  //if removeModel=true...any model for OTF model vis saved in the header is removed
  static void removeCalSet(casacore::MeasurementSet& ms, casacore::Bool removeModel=false);

};

} //# NAMESPACE CASA - END

#endif


