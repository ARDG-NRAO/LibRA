//# PBMath1DEVLA.h: Definitions of 1-D Polynomial forEVLA
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
#ifndef SYNTHESIS_PBMATH1DEVLA_H
#define SYNTHESIS_PBMATH1DEVLA_H
#include <casacore/casa/aips.h>
#include <synthesis/TransformMachines/PBMath1DPoly.h>
#include <synthesis/TransformMachines/PBMath1D.h>
namespace casa { //# NAMESPACE CASA - BEGIN
  class PBMath1DEVLA : public PBMath1D {

// <summary> 
// PBMath1DEVLA  is an expansion  1-D Polynomial  for a Primary Beam
// to hold beams a per EVLA memo 195
// https://library.nrao.edu/public/memos/evla/EVLAM_195.pdf
// </summary>


  public:
    PBMath1DEVLA();
    //freqToUse is not necessary ..at apply time 
    // the right polynomial will be used for the frequency applicable
    // This value can be used for example to run summary
    PBMath1DEVLA(casacore::Quantity maxRad, bool useSymmetricBeam=false, double freqToUse=1.0e9);

    virtual PBMathInterface::PBClass whichPBClass();

  protected:
    virtual void nearestVPArray(double freq, bool printINFO=true);
    virtual void fillPBArray();
  private:
	  void limitFreqForBand(const casacore::String& band, casacore::Double& freq);
    std::map<double, std::vector<double> > coeffmap_p;
    std::map<casacore::String, double> feedConf_p;
    void init();
    casacore::CountedPtr<PBMath1DPoly> pbMathPoly_p;
    BeamSquint squint_p;
    casacore::Quantity maxRad_p;
    bool useSymmetric_p;
    casacore::String feed(const double freq, const double freqCenter=0.0);

  };
} //End namespace casa
#endif
