//# VBContinuumSubtractor.h: Fit a continuum model to a VisBuffer and provide
//# the continuum and/or line estimates as VisBuffers.
//# Copyright (C) 2011
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
//# $Id$
//#
#ifndef MSVIS_VBCONTINUUMSUBTRACTOR_H
#define MSVIS_VBCONTINUUMSUBTRACTOR_H

#include <casa/aips.h>
#include <casa/Arrays/Cube.h>
#include <ms/MeasurementSets/MeasurementSet.h>
//#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffer2.h>


namespace casacore{

class LogIO;
}

namespace casa { //# NAMESPACE CASA - BEGIN

class VisBuffer;
class VisBuffGroupAcc;

// <summary>Fits and optionally subtracts the continuum in visibility spectra</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class=casacore::MeasurementSet>MeasurementSet</linkto>
// </prerequisite>
//
// <etymology>
// This class's main aim is to subtract the continuum from VisBuffers.
// </etymology>
//
// <synopsis>
// Spectral line observations often contain continuum emission which is
// present in all channels (often with a small slope and curvature across the band). This
// class fits a polynomial to this continuum, and can return the continuum
// and/or line (continuum-subtracted) estimates as VisBuffers.
// </synopsis>
//
// <example>
// <srcBlock>
//   const casacore::Int fitorder = 4;                 // Careful!  High orders might
//                                           // absorb power from the lines.
//   casacore::MS inMS(fileName);
//   casacore::Block<casacore::Int> sortcolumns;
//   ROVisIter vi(inMS, sortcolumns, 0.0);
//   VisBuffer cvb(vi);                         // Continuum estimate
//   VisBuffer lvb(vi);                         // Line estimate
//   VisBuffer vb(vi);
//   for(vi.originChunks(); vi.moreChunks(); vi.nextChunk()){
//     for(vi.origin(); vi.more(); ++vi){
//       VBContinuumSubtractor contestor(vb, fitorder); // Continuum Estimator
//
//       contestor.cont_est(cvb);             // Put continuum estimate into cvb.
//       contestor.cont_subtracted(lvb);      // Put line estimate into lvb.
//
//       // Do something with cvb and lvb...
//     }
//   }
// </srcBlock>
// </example>
//
// <motivation>
// This class provides continuum fitting by polynomials with order >= 0, with
// a convenient interface for use by other classes in synthesis and msvis
// (i.e. calibration, imaging, and split).
// </motivation>
//
// <todo asof="">
// </todo>
class VBContinuumSubtractor
{
public:
  VBContinuumSubtractor();

  // Construct it with the low and high frequencies used for scaling the
  // frequencies in the polynomial.
  VBContinuumSubtractor(const casacore::Double lofreq, const casacore::Double hifreq);

  ~VBContinuumSubtractor();

  // Set the # of correlations and fitorder from shp, the total number of input
  // channels to look at (including masked ones!), and the low and high scaling
  // frequencies.
  void init(const casacore::IPosition& shp, const casacore::uInt maxAnt, const casacore::uInt totnumchan,
            const casacore::Double lof, const casacore::Double hif);

  // Set the low and high frequencies, and #s of correlations, antennas, and
  // channels from vbga.  Returns false if vbga is empty.
  casacore::Bool initFromVBGA(VisBuffGroupAcc& vbga);

  // Makes the continuum estimate by fitting a frequency polynomial of order
  // fitorder to the data in vbga.  It sets the low and high frequencies used
  // for scaling the frequencies in the polynomial to the min and max
  // frequencies in vbga.
  // casacore::Input: vbga,         The data
  //        fitorder,     e.g. 2 for a + bf + cf**2
  //        doInit,       if true call initFromVBGA(vbga)
  //        doResize      if true set coeffs and coeffsOK to the right shape.
  // Output (these will be resized):
  //   coeffs:   casacore::Polynomial coefficients for the continuum, indexed by (corr,
  //             order, hash(ant1, ant2).
  //   coeffsOK: and whether or not they're usable.
  void fit(VisBuffGroupAcc& vbga, const casacore::Int fitorder,
           casacore::MS::PredefinedColumns whichcol,
           casacore::Cube<casacore::Complex>& coeffs, casacore::Cube<casacore::Bool>& coeffsOK,
           const casacore::Bool doInit=false, const casacore::Bool doResize=false,
           const casacore::Bool squawk=true);

  // Apply the continuum estimate in coeffs (from fit) to vb.  The affected
  // column of vb is chosen by whichcol, which must be exactly one of casacore::MS::DATA,
  // casacore::MS::MODEL_DATA, or casacore::MS::CORRECTED_DATA lest an exception will be thrown.
  // If doSubtraction is true, vb becomes the line estimate.  Otherwise it is
  // replaced with the continuum estimate.  Returns false if it detects an
  // error, true otherwise.  squawk=true increases the potential number of
  // warnings sent to the logger.
  casacore::Bool apply(VisBuffer& vb,
             const casacore::MS::PredefinedColumns whichcol,
             const casacore::Cube<casacore::Complex>& coeffs,
             const casacore::Cube<casacore::Bool>& coeffsOK, const casacore::Bool doSubtraction=true,
             const casacore::Bool squawk=true);

  // Returns whether or not vb's frequencies are within the bounds used for the
  // continuum fit.  If not, and squawk is true, a warning will be sent to the
  // logger.  (Extrapolation is allowed, just not recommended.)
  casacore::Bool areFreqsInBounds(VisBuffer& vb, const casacore::Bool squawk) const;
  casacore::Bool areFreqsInBounds(vi::VisBuffer2& vb, const casacore::Bool squawk) const;

  // Fills minfreq and maxfreq with the minimum and maximum frequencies (in Hz,
  // acc. to the casacore::MS def'n v.2) of vb (not the continuum fit!).  If you want to
  // get the extreme frequencies over a set of VisBuffers, set initialize to
  // false and initialize minfreq and maxfreq yourself to DBL_MAX and -1.0,
  // respectively.
  static void getMinMaxFreq(VisBuffer& vb, casacore::Double& minfreq, casacore::Double& maxfreq,
                            const casacore::Bool initialize=true);
  static void getMinMaxFreq(vi::VisBuffer2& vb, casacore::Double& minfreq, casacore::Double& maxfreq,
                            const casacore::Bool initialize=true);

  // These are provided for the calibration framework so that a
  // VBContinuumSubtractor can be c'ted from a VisBuffGroupAcc, make a
  // continuum fit, write its results to a caltable, destroyed, and then later
  // another one can be c'ted from the caltable and apply the fit to a
  // VisBuffer.  Obviously it'd be safer and faster to use the same
  // VBContinuumSubtractor for fitting and application, but the rest of CASA
  // isn't ready for that yet (3/7/2011).
  casacore::Int getOrder() const {return fitorder_p;}

  casacore::Double getLowFreq() const {return lofreq_p;}  // Lowest frequency used in the fit,
  casacore::Double getHighFreq() const {return hifreq_p;} // and highest, in Hz, acc. to
                                                // the casacore::MS def'n v.2.
  casacore::Int getMaxAntNum() const {return maxAnt_p;}   // -1 if unready.

  // The total number of input channels that will be looked at (including
  // masked ones!)
  casacore::uInt getTotNumChan() const {return totnumchan_p;}

  // Low (lof) and high (hif) frequencies, in Hz, used for renormalizing
  // frequencies in the polynomials.
  void setScalingFreqs(casacore::Double lof, casacore::Double hif){
    lofreq_p = lof;
    hifreq_p = hif;
    midfreq_p = 0.5 * (lof + hif);
    freqscale_p = calcFreqScale();
  }

  // Set the maximum number of antennas (actually, 1 + the maximum antenna
  // number).
  void setNAnt(const casacore::uInt nAnt){
    maxAnt_p = nAnt - 1;
    nHashes_p = (nAnt * (nAnt + 1)) / 2;  // Allows for autocorrs.  
  }

  // Set the total number of input channels that will be looked at (including
  // masked ones!)
  void setTotNumChan(const casacore::uInt tnc) {totnumchan_p = tnc;}

  // A convenience function for prepping coeffs and coeffsOK to hold the
  // polynomial coefficients and their validities.
  void resize(casacore::Cube<casacore::Complex>& coeffs, casacore::Cube<casacore::Bool>& coeffsOK) const;

  casacore::Bool checkSize(casacore::Cube<casacore::Complex>& coeffs, casacore::Cube<casacore::Bool>& coeffsOK) const
  {
    return coeffs.shape()[0] == static_cast<casacore::Int>(ncorr_p) &&
      coeffs.shape()[1] == fitorder_p + 1 &&
      coeffs.shape()[2] == static_cast<casacore::Int>(nHashes_p) &&
      coeffsOK.shape()[0] == static_cast<casacore::Int>(ncorr_p) &&
      coeffsOK.shape()[1] == fitorder_p + 1 &&
      coeffsOK.shape()[2] == static_cast<casacore::Int>(nHashes_p);
  }

  casacore::Double calcFreqScale() const {
    return hifreq_p > midfreq_p ? 1.0 / (hifreq_p - midfreq_p) : 1.0;
  }

  void setTVIDebug(bool debug) {tvi_debug = debug;}


  casacore::Bool apply2(vi::VisBuffer2& vb,
			casacore::Cube<casacore::Complex>& Vout,
			//const casacore::MS::PredefinedColumns whichcol,
			const casacore::Cube<casacore::Complex>& coeffs,
			const casacore::Cube<casacore::Bool>& coeffsOK, const casacore::Bool doSubtraction=true,
			const casacore::Bool squawk=true);



private:
  // Disable default copying, and assignment.
  VBContinuumSubtractor& operator=(VBContinuumSubtractor& other);

  // Can the fit be _applied_ to vb?
  // If not and squawk is true, send a severe message to os.
  casacore::Bool doShapesMatch(VisBuffer& vb, casacore::LogIO& os, const casacore::Bool squawk=true) const;
  casacore::Bool doShapesMatch(vi::VisBuffer2& vb, casacore::LogIO& os, const casacore::Bool squawk=true) const;

  // Compute baseline (row) index in coeffs_p for (ant1, ant2).
  // It ASSUMES that ant1 and ant2 are both <= maxAnt_p.
  casacore::uInt hashFunction(const casacore::Int ant1, const casacore::Int ant2)
  {
    return (maxAnt_p + 1) * ant1 - (ant1 * (ant1 - 1)) / 2 + ant2 - ant1;
  }

  //VisBuffGroupAcc& vbga_p;      // Holds the VisBuffers
  casacore::Int       fitorder_p;  // Order of the fitting polynomial.

  casacore::Double    lofreq_p;    // Lowest frequency used in the continuum fit,
  casacore::Double    hifreq_p;    // and highest, in Hz, acc. to the casacore::MS def'n v.2.
  casacore::Double    midfreq_p;   // 0.5 * (lofreq_p + hifreq_p)
  casacore::Double    freqscale_p;
  casacore::Int       maxAnt_p;         // Highest 0 based antenna number that can be
                              // used with the fits.  -1 if not ready.
  casacore::uInt      nHashes_p;        // Calculated and cached from maxAnt_p.
  casacore::uInt      ncorr_p;
  casacore::uInt      totnumchan_p;

  casacore::PtrBlock<casacore::Vector<casacore::Bool> * > chanmask_p;

  bool tvi_debug;
};

} //# NAMESPACE CASA - END

#endif
