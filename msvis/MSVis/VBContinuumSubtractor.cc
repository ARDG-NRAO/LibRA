//# VBContinuumSubtractor.cc:  Subtract continuum from spectral line data
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
//# $Id$
//#
//#include <casa/Arrays/ArrayLogical.h>
//#include <casa/Arrays/ArrayMath.h>
//#include <casa/Arrays/ArrayUtil.h>
#include <casacore/casa/Arrays/Cube.h>
//#include <casa/Arrays/MaskedArray.h>
//#include <casa/Arrays/MaskArrMath.h>
//#include <casa/Containers/Record.h>
//#include <casa/Containers/RecordFieldId.h>
//#include <casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogIO.h>
//#include <casa/Quanta/MVTime.h>
//#include <casa/Quanta/QuantumHolder.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <msvis/MSVis/VBContinuumSubtractor.h>
#include <msvis/MSVis/VisBuffGroupAcc.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/scimath/Fitting/LinearFitSVD.h>
#include <casacore/scimath/Functionals/Polynomial.h>

#include <msvis/MSVis/VisBuffer2.h>

//#include <algorithm>

using namespace casacore;
using namespace casa::vi;

namespace casa { //# NAMESPACE CASA - BEGIN

VBContinuumSubtractor::VBContinuumSubtractor():
  fitorder_p(-1),
  lofreq_p(-1.0),
  hifreq_p(-1.0),
  midfreq_p(-1.0),
  freqscale_p(1.0),
  maxAnt_p(-1),
  nHashes_p(0),
  ncorr_p(0),
  totnumchan_p(0),
  tvi_debug(False)
{
}

VBContinuumSubtractor::VBContinuumSubtractor(const Double lofreq,
                                             const Double hifreq):
  fitorder_p(-1),
  lofreq_p(lofreq),
  hifreq_p(hifreq),
  midfreq_p(-1.0),
  freqscale_p(1.0),
  maxAnt_p(-1),
  nHashes_p(0),
  ncorr_p(0),
  totnumchan_p(0),
  tvi_debug(False)
{
  midfreq_p = 0.5 * (lofreq + hifreq);
  freqscale_p = calcFreqScale();
}

void VBContinuumSubtractor::resize(Cube<Complex>& coeffs,
                                   Cube<Bool>& coeffsOK) const
{
  if(maxAnt_p < 0 || fitorder_p < 0 || ncorr_p < 1)
    throw(AipsError("The fit order, # of corrs, and/or max antenna # must be set."));

  // An nth order polynomial has n + 1 coefficients.
  coeffs.resize(ncorr_p, fitorder_p + 1, nHashes_p);
  // Calibrater wants coeffsOK to be a Cube, even though a Matrix would do for
  // VBContinuumSubtractor.  Unfortunately problems arise (worse, quietly) in
  // SolvableVisCal::keep() and store() if one tries to get away with
  //coeffsOK.resize(ncorr_p, 1, nHashes_p);
  coeffsOK.resize(ncorr_p, fitorder_p + 1, nHashes_p);
}

void VBContinuumSubtractor::init(const IPosition& shp, const uInt maxAnt,
                                 const uInt totnumchan,
                                 const Double lof, const Double hif)
{
  ncorr_p    = shp[0];
  fitorder_p = shp[1] - 1;

  //// Going from the number of baselines to the number of antennas is a little
  //// backwards, but so is this function.
  // uInt nAnt = round((-1 + sqrt(1 + 8 * shp[2])) / 2);
  setNAnt(maxAnt + 1);
  
  totnumchan_p = totnumchan;
  setScalingFreqs(lof, hif);
}

Bool VBContinuumSubtractor::initFromVBGA(VisBuffGroupAcc& vbga)
{
  Bool retval = true;

  if(vbga.nBuf() > 0){
    ncorr_p = vbga(0).nCorr();
    setNAnt(vbga.nAnt());
  
    // Count the total number of channels, and get the minimum and maximum
    // frequencies for scaling.
    totnumchan_p = 0;
    hifreq_p = -1.0;
    lofreq_p = DBL_MAX;
    for(Int ibuf = 0; ibuf < vbga.nBuf(); ++ibuf){
      VisBuffer& vb(vbga(ibuf));

      totnumchan_p += vb.nChannel();
      getMinMaxFreq(vb, lofreq_p, hifreq_p, false);
    }
    midfreq_p = 0.5 * (lofreq_p + hifreq_p);
    freqscale_p = calcFreqScale();
  }
  else
    retval = false;
  return retval;
}

VBContinuumSubtractor::~VBContinuumSubtractor()
{}

void VBContinuumSubtractor::fit(VisBuffGroupAcc& vbga, const Int fitorder,
                                MS::PredefinedColumns whichcol,
                                Cube<Complex>& coeffs,
                                Cube<Bool>& coeffsOK, const Bool doInit,
                                const Bool doResize,
                                const Bool squawk)
{
  LogIO os(LogOrigin("VBContinuumSubtractor", "fit()", WHERE));

  // jagonzal: Debug code
  /*
  uInt row_debug = 0;
  uInt corr_debug = 0;
  */

  fitorder_p = fitorder;

  if(!(whichcol == MS::DATA || whichcol == MS::MODEL_DATA ||
       whichcol == MS::CORRECTED_DATA)){
    if(squawk)
      os << LogIO::SEVERE
         << MS::columnName(whichcol) << " is not supported.\n"
         << MS::columnName(MS::DATA) << " will be used instead."
         << LogIO::POST;
    whichcol = MS::DATA;
  }

  if(doInit)
    initFromVBGA(vbga);

  if(maxAnt_p < 0 || fitorder_p < 0 || ncorr_p < 1 || totnumchan_p < 1
     || lofreq_p < 0.0 || hifreq_p < 0.0)
    throw(AipsError("The continuum fitter must first be initialized."));

  if(doResize)
    resize(coeffs, coeffsOK);

  if(!checkSize(coeffs, coeffsOK))
    throw(AipsError("Shape mismatch in the coefficient storage cubes."));

  // Make the estimate
  LinearFitSVD<Float> fitter;
  fitter.asWeight(true);        // Makes the "sigma" arg = w = 1/sig**2

  coeffsOK.set(false);

  // Translate vbga to arrays for use by LinearFitSVD.

  // The fitorder will actually be clamped on a baseline-by-baseline basis
  // because of flagging, but a summary note is in order here.
  if(static_cast<Int>(totnumchan_p) < fitorder_p)
    os << LogIO::WARN
       << "fitorder = " << fitorder_p
       << ", but only " << totnumchan_p << " channels were selected.\n"
       << "The polynomial order will be lowered accordingly."
       << LogIO::POST;
  // Scale frequencies to [-1, 1].
  midfreq_p = 0.5 * (lofreq_p + hifreq_p);
  freqscale_p = calcFreqScale();
  Vector<Float> freqs(totnumchan_p);
  uInt totchan = 0;
  for(Int ibuf = 0; ibuf < vbga.nBuf(); ++ibuf){
    VisBuffer& vb(vbga(ibuf));
    Vector<Double> freq(vb.frequency());
    uInt nchan = vb.nChannel();

    for(uInt c = 0; c < nchan; ++c){
      freqs[totchan] = freqscale_p * (freq[c] - midfreq_p);
      ++totchan;
    }
  }

  Vector<Float> wt(totnumchan_p);
  Vector<Float> unflaggedfreqs(totnumchan_p);
  Vector<Complex> vizzes(totnumchan_p);
  Vector<Float> floatvs(totnumchan_p);
  Vector<Float> realsolution(fitorder_p + 1);
  Vector<Float> imagsolution(fitorder_p + 1);

  for(uInt corrind = 0; corrind < ncorr_p; ++corrind){
    for(uInt blind = 0; blind < nHashes_p; ++blind){
      uInt totchan = 0;
      uInt totunflaggedchan = 0;

      // Fill wt, unflaggedfreqs, and vizzes with the baseline's values for
      // all channels being used in the fit.
      wt.resize(totnumchan_p);
      vizzes.resize(totnumchan_p);
      unflaggedfreqs.resize(totnumchan_p);

      for(Int ibuf = 0; ibuf < vbga.nBuf(); ++ibuf){
        VisBuffer& vb(vbga(ibuf));
        uInt nchan = vb.nChannel();
        //Int vbrow = vbga.outToInRow(ibuf, false)[blind];

        if(!vb.flagRow()[blind]){
          Cube<Complex>& viscube(vb.dataCube(whichcol));
          Float w;

          // 2/24/2011: VisBuffer doesn't (yet) have sigmaSpectrum, and I have
          // never seen it in an MS anyway.  Settle for 1/sqrt(weightSpectrum)
          // if it is available or sigmaMat otherwise.
          //const Bool haveWS = vb.existsWeightSpectrum();
          // 5/13/2011: Sigh.  VisBuffAccumulator doesn't even handle
          // WeightSpectrum, let alone sigma.
          //const Bool haveWS = false;

          //if(!haveWS) // w is needed either way, in case ws == 0.0.
          w = vb.weightMat()(corrind, blind);

          // w needs a sanity check, because a VisBuffer from vbga is not
          // necessarily still attached to the MS and sigmaMat() is not one
          // of the accumulated quantities.  This caused problems for
          // the last integration in CAS-3135.  checkVisIter() didn't do the
          // trick in that case.  Fortunately w isn't all that important; if
          // all the channels have the same weight the only consequence of
          // setting w to 1 is that the estimated errors (which we don't yet
          // use) will be wrong.
          //
          // 5e-45 ended up getting squared in the fitter and producing a NaN.
          if(isnan(w) || w < 1.0e-20 || w > 1.0e20)
            w = 1.0;  // Emit a warning?

          Bool chanFlag;
          for(uInt c = 0; c < nchan; ++c){
            // AAARRGGGHHH!!  With Calibrater you have to use vb.flag(), not
            // flagCube(), to get the channel selection!
            //if(!vb.flagCube()(corrind, c, vbrow)){0
        	  if (!tvi_debug)
        	  {
        		  chanFlag = vb.flag()(c, blind);
        	  }
        	  else
        	  {
        		  // jagonzal: Use flagCube, meaning that each correlation is fit
        		  // independently even if the other correlations are fully flagged
        		  chanFlag = vb.flagCube()(corrind, c, blind);
        		  // jagonzal: Apparently line-free chan mask is contained in vb.flag()
        		  chanFlag = vb.flagCube()(corrind, c, blind) |  vb.flag()(c, blind);
        	  }
            //if(!vb.flag()(c, blind)){
        	// if(!vb.flagCube()(corrind, c, blind)){
        	  if (!chanFlag) {
              unflaggedfreqs[totunflaggedchan] = freqs[totchan];
              // if(haveWS){
              //   Double ws = vb.weightSpectrum()(corrind, c, vbrow);

              //   wt[totunflaggedchan] = ws;
              // }
              // else
                wt[totunflaggedchan] = w / nchan;
              vizzes[totunflaggedchan] = viscube(corrind, c, blind);
              ++totunflaggedchan;
            }
            ++totchan;
          }
        }
        else
          totchan += nchan;
      }

      // jagonzal: Debug code
      /*
      if (tvi_debug)
      {
          VisBuffer& vb(vbga(0));
          uInt rowId = vb.rowIds()[blind];
          if (rowId == row_debug and corrind == corr_debug and totunflaggedchan <= 0)
          {
          	os << LogIO::WARN << "All channels flagged" << LogIO::POST;
          }
      }
      */

      if(totunflaggedchan > 0){                 // OK, try a fit.
        // Truncate the Vectors.
        wt.resize(totunflaggedchan, true);
        //vizzes.resize(totunflaggedchan, true);
        floatvs.resize(totunflaggedchan);
        unflaggedfreqs.resize(totunflaggedchan, true);

        // perform least-squares fit of a polynomial.
        // Don't try to solve for more coefficients than valid channels.
        Int locFitOrd = min(fitorder_p, static_cast<Int>(totunflaggedchan) - 1);

        // if(locFitOrd < 1)
        //   os << LogIO::DEBUG1
        //      << "locFitOrd = " << locFitOrd
        //      << LogIO::POST;

        Polynomial<AutoDiff<Float> > pnom(locFitOrd);

        // The way LinearFit is templated, "y" can be Complex, but at the cost
        // of "x" being Complex as well, and worse, wt too.  It is better to
        // separately fit the reals and imags.
        // Do reals.
        for(Int ordind = 0; ordind <= locFitOrd; ++ordind)       // Note <=.
          pnom.setCoefficient(ordind, 1.0);

        for(uInt c = 0; c < totunflaggedchan; ++c)
          floatvs[c] = vizzes[c].real();

        fitter.setFunction(pnom);
        realsolution(Slice(0,locFitOrd+1,1)) = fitter.fit(unflaggedfreqs, floatvs, wt);

        // jagonzal: Debug code
        /*
        if (tvi_debug)
        {
            VisBuffer& vb(vbga(0));
            uInt rowId = vb.rowIds()[blind];
            if (rowId == row_debug and corrind == corr_debug)
            {
            	os << "locFitOrd=" << locFitOrd << LogIO::POST;
            	os << "flag=" << vb.flag().column(blind) << LogIO::POST;
            	os << "flagCube=" << vb.flagCube().xyPlane(blind).row(corrind) << LogIO::POST;
        		os << "unflaggedfreqs=" << unflaggedfreqs << LogIO::POST;
        		os << "wt=" << wt<< LogIO::POST;
        		os << "real floatvs=" << floatvs << LogIO::POST;
        		os << "real realsolution=" << realsolution << LogIO::POST;
            }
        }
        */

        // if(isnan(realsolution[0])){
        //   os << LogIO::DEBUG1 << "NaN found." << LogIO::POST;
        //   for(uInt c = 0; c < totunflaggedchan; ++c){
        //     if(isnan(unflaggedfreqs[c]))
        //       os << LogIO::DEBUG1
        //          << "unflaggedfreqs[" << c << "] is a NaN."
        //          << LogIO::POST;
        //     if(isnan(floatvs[c]))
        //       os << LogIO::DEBUG1
        //          << "floatvs[" << c << "] is a NaN."
        //          << LogIO::POST;
        //     if(isnan(wt[c]))
        //       os << LogIO::DEBUG1
        //          << "wt[" << c << "] is a NaN."
        //          << LogIO::POST;
        //     else if(wt[c] <= 0.0)
        //       os << LogIO::DEBUG1
        //          << "wt[" << c << "] = " << wt[c]
        //          << LogIO::POST;
        //   }
        // }

        // Do imags.
        for(Int ordind = 0; ordind <= locFitOrd; ++ordind)       // Note <=.
          pnom.setCoefficient(ordind, 1.0);

        for(uInt c = 0; c < totunflaggedchan; ++c)
          floatvs[c] = vizzes[c].imag();

        fitter.setFunction(pnom);
        imagsolution(Slice(0,locFitOrd+1,1)) = fitter.fit(unflaggedfreqs, floatvs, wt);

        // jagonzal: Debug code
        /*
        if (tvi_debug)
        {
            VisBuffer& vb(vbga(0));
            uInt rowId = vb.rowIds()[blind];
            if (rowId == row_debug and corrind == corr_debug)
            {
            	os << "imag floatvs=" << floatvs << LogIO::POST;
        		os << "imag solution=" << imagsolution << LogIO::POST;
            }
        }
        */
          

        for(Int ordind = 0; ordind <= locFitOrd; ++ordind){      // Note <=.
          coeffs(corrind, ordind, blind) = Complex(realsolution[ordind],
                                                   imagsolution[ordind]);
          coeffsOK(corrind, ordind, blind) = true;
        }

        // Pad remaining orders (if any) with 0.0.  Note <=.
        for(Int ordind = locFitOrd + 1; ordind <= fitorder_p; ++ordind){
          coeffs(corrind, ordind, blind) = 0.0;

          // Since coeffs(corrind, ordind, blind) == 0, it isn't necessary to
          // pay attention to coeffsOK(corrind, ordind, blind) (especially?) if
          // ordind > 0.  But Calibrater's SolvableVisCal::keep() and store()
          // quietly go awry if you try coeffsOK.resize(ncorr_p, 1, nHashes_p);
          coeffsOK(corrind, ordind, blind) = false;
        }

        // TODO: store uncertainties
      }
    }
  }
}

void VBContinuumSubtractor::getMinMaxFreq(VisBuffer& vb,
                                          Double& minfreq,
                                          Double& maxfreq,
                                          const Bool initialize)
{
  const Vector<Double>& freq(vb.frequency());
  Int hichan = vb.nChannel() - 1;
  Int lochan = 0;

  if(initialize){
    maxfreq = -1.0;
    minfreq = DBL_MAX;
  }
  
  if(freq[hichan] < freq[lochan]){
    lochan = hichan;
    hichan = 0;
  }
  if(freq[hichan] > maxfreq)
    maxfreq = freq[hichan];
  if(freq[lochan] < minfreq)
    minfreq = freq[lochan];
}

Bool VBContinuumSubtractor::areFreqsInBounds(VisBuffer& vb,
                                             const Bool squawk) const
{
  Double maxfreq, minfreq;
  
  getMinMaxFreq(vb, minfreq, maxfreq);
  Bool result = minfreq >= lofreq_p && maxfreq <= hifreq_p;

  if(squawk && !result){
    // The truth was considered too alarming (CAS-1968).
    // LogIO os(LogOrigin("VBContinuumSubtractor", "areFreqsInBounds"));
    LogIO os(LogOrigin("VBContinuumSubtractor", "apply"));

    os << LogIO::WARN
       << "Extrapolating to cover [" << 1.0e-9 * minfreq << ", "
       << 1.0e-9 * maxfreq << "] (GHz).\n"
       << "The frequency range used for the continuum fit was ["
       << 1.0e-9 * lofreq_p << ", "
       << 1.0e-9 * hifreq_p << "] (GHz)."
       << LogIO::POST;
  }
  return result;
}

Bool VBContinuumSubtractor::doShapesMatch(VisBuffer& vb,
                                          LogIO& os, const Bool squawk) const
{
  Bool theydo = true;

  if(vb.nCorr() != static_cast<Int>(ncorr_p)){
    theydo = false;
    if(squawk)
      os << LogIO::SEVERE
         << "The supplied number of correlations, " << vb.nCorr()
         << ", does not match the expected " << ncorr_p
         << LogIO::POST;
  }
  // It's no longer the # of rows that matter but the maximum antenna #.
  // if(vb.nRow() != nrow_p){
  if(max(vb.antenna2()) > maxAnt_p){
    theydo = false;             // Should it just flag unknown baselines?
    if(squawk)
      os << LogIO::SEVERE
         << "The fit is only valid for antennas with indices <= " << maxAnt_p
         << LogIO::POST;
  }
  return theydo;
}

// Do the subtraction
Bool VBContinuumSubtractor::apply(VisBuffer& vb,
                                  const MS::PredefinedColumns whichcol,
                                  const Cube<Complex>& coeffs,
                                  const Cube<Bool>& coeffsOK,
                                  const Bool doSubtraction,
                                  const Bool squawk)
{
    using casacore::operator*;

  LogIO os(LogOrigin("VBContinuumSubtractor", "apply"));

  if(!doShapesMatch(vb, os, squawk))
    return false;
    
  Bool ok = areFreqsInBounds(vb, squawk); // A Bool might be too Boolean here.
  ok = true;                              // Yep, returning false for a slight
                                          // extrapolation is too harsh.

  if(!(whichcol == MS::DATA || whichcol == MS::MODEL_DATA ||
       whichcol == MS::CORRECTED_DATA)){
    if(squawk)
      os << LogIO::SEVERE
         << MS::columnName(whichcol) << " is not supported."
         << LogIO::POST;
    return false;
  }
  Cube<Complex>& viscube(vb.dataCube(whichcol));
  
  uInt nchan = vb.nChannel();
  uInt nvbrow = vb.nRow();

  // DEBUGGING
  // os << LogIO::DEBUG1
  //    << "nvbrow: " << nvbrow << ", nchan: " << nchan
  //    << LogIO::POST;
  // // Check coeffs.
  // for(uInt vbrow = 0; vbrow < nvbrow; ++vbrow){
  //   uInt blind = hashFunction(vb.antenna1()[vbrow],
  //                             vb.antenna2()[vbrow]);

  //   for(uInt corrind = 0; corrind < ncorr_p; ++corrind){
  //     if(coeffsOK(corrind, 0, blind)){
  //       Complex cont = coeffs(corrind, 0, blind);
    
  //       if(fabs(cont) < 0.001)
  //         os << LogIO::WARN
  //            << "cont(" << corrind << ", 0, " << blind << ") = "
  //            << cont
  //            << LogIO::POST;
  //     }
  //   }
  // }
  // END DEBUGGING

  Vector<Double> freqpow(fitorder_p + 1);           // sf**ordind
  freqpow[0] = 1.0;
  Vector<Double>& freq(vb.frequency());

  for(uInt c = 0; c < nchan; ++c){
    Double sf = freqscale_p * (freq[c] - midfreq_p);      // scaled frequency
  
    for(Int ordind = 1; ordind <= fitorder_p; ++ordind)
      freqpow[ordind] = sf * freqpow[ordind - 1];

    for(uInt vbrow = 0; vbrow < nvbrow; ++vbrow){
      uInt blind = hashFunction(vb.antenna1()[vbrow],
                                vb.antenna2()[vbrow]);

      for(uInt corrind = 0; corrind < ncorr_p; ++corrind){
        if(coeffsOK(corrind, 0, blind)){
          Complex cont = coeffs(corrind, 0, blind);

          for(Int ordind = 1; ordind <= fitorder_p; ++ordind)
            cont += coeffs(corrind, ordind, blind) * freqpow[ordind];
          if(doSubtraction)
            viscube(corrind, c, vbrow) -= cont;
          else
            viscube(corrind, c, vbrow) = cont;            

          // TODO: Adjust WEIGHT_SPECTRUM (create if necessary?), WEIGHT, and
          // SIGMA.
        }
        else
        {
            // jagonzal: Do not flag data in case of not successful
            // sfit because it might be cause by the line-free mask
        	// and we may end up flagging 'good' line channels
            if (!tvi_debug) vb.flagCube()(corrind, c, vbrow) = true;
            // jagonzal: When returning the continuum, in case of not
            // successful fit we should return 0, not the input data
            if (tvi_debug and !doSubtraction) viscube(corrind, c, vbrow) = 0;
        }
        //vb.flag()(c, vbrow) = true;
      }
    }
  }
  return ok;
}


// Do the subtraction  VB2 version
Bool VBContinuumSubtractor::apply2(VisBuffer2& vb,
				   Cube<Complex>& Vout,
				   //const MS::PredefinedColumns whichcol,
				   const Cube<Complex>& coeffs,
				   const Cube<Bool>& coeffsOK,
				   const Bool doSubtraction,
				   const Bool squawk)
{
    using casacore::operator*;

  LogIO os(LogOrigin("VBContinuumSubtractor", "apply2"));

  if(!doShapesMatch(vb, os, squawk))
    return false;
    
  Bool ok = areFreqsInBounds(vb, squawk); // A Bool might be too Boolean here.
  ok = true;                              // Yep, returning false for a slight
                                          // extrapolation is too harsh.

  Cube<Complex>& viscube(Vout);
  
  Cube<Bool> fC;
  fC.reference(vb.flagCube());

  uInt nchan = vb.nChannels();
  uInt nvbrow = vb.nRows();

  Vector<Double> freqpow(fitorder_p + 1);           // sf**ordind
  freqpow[0] = 1.0;
  const Vector<Double>& freq(vb.getFrequencies(0));

  for(uInt c = 0; c < nchan; ++c){
    Double sf = freqscale_p * (freq[c] - midfreq_p);      // scaled frequency
  
    for(Int ordind = 1; ordind <= fitorder_p; ++ordind)
      freqpow[ordind] = sf * freqpow[ordind - 1];

    for(uInt vbrow = 0; vbrow < nvbrow; ++vbrow){
      uInt blind = hashFunction(vb.antenna1()(vbrow),
                                vb.antenna2()(vbrow));

      for(uInt corrind = 0; corrind < ncorr_p; ++corrind){
        if(coeffsOK(corrind, 0, blind)){
          Complex cont = coeffs(corrind, 0, blind);

          for(Int ordind = 1; ordind <= fitorder_p; ++ordind)
            cont += coeffs(corrind, ordind, blind) * freqpow[ordind];
          if(doSubtraction)
            viscube(corrind, c, vbrow) -= cont;
          else
            viscube(corrind, c, vbrow) = cont;            

          // TODO: Adjust WEIGHT_SPECTRUM (create if necessary?), WEIGHT, and
          // SIGMA.
        }
        else
        {
            // jagonzal: Do not flag data in case of not successful
            // sfit because it might be cause by the line-free mask
        	// and we may end up flagging 'good' line channels
            if (!tvi_debug) fC(corrind, c, vbrow) = true;
            // jagonzal: When returning the continuum, in case of not
            // successful fit we should return 0, not the input data
            if (tvi_debug and !doSubtraction) viscube(corrind, c, vbrow) = 0;
        }
        //vb.flag()(c, vbrow) = true;
      }
    }
  }
  return ok;
}

Bool VBContinuumSubtractor::doShapesMatch(VisBuffer2& vb,
                                          LogIO& os, const Bool squawk) const
{
  Bool theydo = true;

  if(vb.nCorrelations() != static_cast<Int>(ncorr_p)){
    theydo = false;
    if(squawk)
      os << LogIO::SEVERE
         << "The supplied number of correlations, " << vb.nCorrelations()
         << ", does not match the expected " << ncorr_p
         << LogIO::POST;
  }
  if(max(vb.antenna2()) > maxAnt_p){
    theydo = false;             // Should it just flag unknown baselines?
    if(squawk)
      os << LogIO::SEVERE
         << "The fit is only valid for antennas with indices <= " << maxAnt_p
         << LogIO::POST;
  }
  return theydo;
}

void VBContinuumSubtractor::getMinMaxFreq(VisBuffer2& vb,
                                          Double& minfreq,
                                          Double& maxfreq,
                                          const Bool initialize)
{
  const Vector<Double>& freq(vb.getFrequencies(0));  // row=0, assumed uniform in VB2
  Int hichan = vb.nChannels() - 1;
  Int lochan = 0;

  if(initialize){
    maxfreq = -1.0;
    minfreq = DBL_MAX;
  }
  
  if(freq[hichan] < freq[lochan]){
    lochan = hichan;
    hichan = 0;
  }
  if(freq[hichan] > maxfreq)
    maxfreq = freq[hichan];
  if(freq[lochan] < minfreq)
    minfreq = freq[lochan];
}


Bool VBContinuumSubtractor::areFreqsInBounds(VisBuffer2& vb,
                                             const Bool squawk) const
{
  Double maxfreq, minfreq;
  
  getMinMaxFreq(vb, minfreq, maxfreq);
  Bool result = minfreq >= lofreq_p && maxfreq <= hifreq_p;

  if(squawk && !result){
    // The truth was considered too alarming (CAS-1968).
    // LogIO os(LogOrigin("VBContinuumSubtractor", "areFreqsInBounds"));
    LogIO os(LogOrigin("VBContinuumSubtractor", "apply"));

    os << LogIO::WARN
       << "Extrapolating to cover [" << 1.0e-9 * minfreq << ", "
       << 1.0e-9 * maxfreq << "] (GHz).\n"
       << "The frequency range used for the continuum fit was ["
       << 1.0e-9 * lofreq_p << ", "
       << 1.0e-9 * hifreq_p << "] (GHz)."
       << LogIO::POST;
  }
  return result;
}



} //# NAMESPACE CASA - END

