//# FringeJones.cc: Implementation of FringeJones
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003,2011
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

#include <synthesis/MeasurementComponents/FringeJones.h>

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <synthesis/CalTables/CTIter.h>
#include <synthesis/MeasurementEquations/VisEquation.h>  // *
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <scimath/Mathematics/FFTServer.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/Aipsrc.h>

#include <casa/sstream.h>

#include <measures/Measures/MCBaseline.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MeasTable.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>

#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/MaskArrMath.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_spblas.h>
#include <gsl/gsl_multilarge_nlinear.h>
#include <gsl/gsl_linalg.h>
#include <iomanip>                // needed for setprecision

// DEVDEBUG gates the development debugging information to standard
// error; it should be set to 0 for production.
#define DEVDEBUG false
#define KDISPSCALE 1e6

using namespace casa::vi;
using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN
    
static void unitize(Array<Complex>& vC) 
{
    Array<Float> vCa(amplitude(vC));
    // Divide by non-zero amps
    vCa(vCa<FLT_EPSILON)=1.0;
    vC /= vCa;
}

SDBListGridManager::SDBListGridManager(SDBList& sdbs_) :
    sdbs(sdbs_)
{
    std::set<Double> fmaxes;
    std::set<Double> fmins;
    Float dfn(0.0);
    Int totalChans0(0) ;
    Int nchan(0);

    if (sdbs.nSDB()==0) {
        // The for loop is fine with an empty list, but code below it
        // isn't and there's nothing to lose by bailing early!
        return;
    }
        
    
    for (Int i=0; i != sdbs.nSDB(); i++) {
        SolveDataBuffer& sdb = sdbs(i);
        Int spw = sdb.spectralWindow()(0);
        Double t = sdbs(i).time()(0);
        times.insert(t); 
        if (spwins.find(spw) == spwins.end()) {
            spwins.insert(spw);
            const Vector<Double>& fs = sdb.freqs();
            spwIdToFreqMap[spw] = &(sdb.freqs());
            nchan = sdb.nChannels();
            fmaxes.insert(fs(nchan-1));
            fmins.insert(fs(0));
            // We assume they're all at the same time.

            totalChans0 += nchan;
            Float df0 = fs(1) - fs(0);
            dfn = (fs(nchan-1) - fs(0))/(nchan-1);
            if (DEVDEBUG) {
                cerr << "Spectral window " << spw << " has " << nchan << " channels" << endl;
                cerr << "df0 "<< df0 << "; " << "dfn " << dfn << endl;
            }
        } else {
            continue;
        }
    }
    if (nchan == 1) {
      nt = sdbs.nSDB()/spwins.size();
      tmin = *(times.begin());
      tmax = *(times.rbegin());
      dt = (tmax - tmin)/(nt - 1);
      nSPWChan = nchan;
      fmin = *(fmins.begin());
      fmax = fmin;
      totalChans = 1;
      df = 1;
      return;
    }
      
    nt = sdbs.nSDB()/spwins.size();
    tmin = *(times.begin());
    tmax = *(times.rbegin());
    dt = (tmax - tmin)/(nt - 1);
    nSPWChan = nchan;
    fmin = *(fmins.begin());
    fmax = *(fmaxes.rbegin());
    totalChans = round((fmax - fmin)/dfn + 1);
    df = (fmax - fmin)/(totalChans-1);
    if (DEVDEBUG) {
        cerr << "Global fmin " << fmin << " global max " << fmax << endl;
        cerr << "nt " << nt << " dt " << dt << endl;
        cerr << "tmin " << tmin << " tmax " << tmax << endl;
        cerr << "Global dt " << tmax - tmin << endl;
        cerr << "Global df " << df << endl;
        cerr << "I guess we'll need " << totalChans << " freq points in total." << endl;
        cerr << "Compared to " << totalChans0 << " with simple-minded concatenation." << endl;
    }
}

// checkAllGridPoints is a diagnostic funtion that should not be called
// in production releases, but it doesn't do any harm to have it
// latent.
void
SDBListGridManager::checkAllGridpoints() {
    map<Int, Vector<Double> const *>::iterator it;
    for (it = spwIdToFreqMap.begin(); it != spwIdToFreqMap.end(); it++) {
        Int spwid = it->first;
        Vector<Double> const* fs = it->second;
        Int length;
        fs->shape(length);
        for (Int i=0; i!=length; i++) {
            Double f = (*fs)(i);
            Int j = bigFreqGridIndex(f);
            cerr << "spwid, i = (" << spwid << ", " << i << ") => " << j << " (" << f << ")" << endl;
        }
    }
    cerr << "[1] spwins.size() " << nSPW() << endl;
    cerr << "[2] spwins.size() " << spwins.size() << endl;
}

Int
SDBListGridManager::swStartIndex(Int spw) {
    Vector<Double> const* fs = spwIdToFreqMap[spw];
    Double f0 = (*fs)(0);
    return bigFreqGridIndex(f0);
}
   
// DelayRateFFT is modeled on DelayFFT in KJones.{cc|h}
DelayRateFFT::DelayRateFFT(SDBList& sdbs, Int refant, Array<Double>& delayWindow, Array<Double>& rateWindow) :
    refant_(refant),
    gm_(sdbs),
    nPadFactor_(max(2, 8  / gm_.nSPW())), 
    nt_(gm_.nt),
    nPadT_(nPadFactor_ * nt_),
    nChan_(gm_.nChannels()),
    nPadChan_(nPadFactor_*nChan_),
    dt_(gm_.dt),
    //f0_(gm_.fmin / 1.e9),      // GHz
    f0_(sdbs.centroidFreq() / 1.e9),      // GHz, for delayrate calc
    df_(gm_.df / 1.e9),
    df_all_(gm_.fmax - gm_.fmin),
    Vpad_(),
    xcount_(),
    sumw_(),
    sumww_(),
    activeAntennas_(),
    allActiveAntennas_(),
    delayWindow_(delayWindow),
    rateWindow_(rateWindow) {
    // This check should be commented out in production:
    // gm_.checkAllGridpoints();

    if (nt_ < 2) {
        throw(AipsError("Can't do a 2-dimensional FFT on a single timestep! Please consider changing solint to avoid orphan timesteps."));
    }
    IPosition ds = delayWindow_.shape();
    if (ds.size()!=1 || ds.nelements() != 1) {
        throw AipsError("delaywindow must be a list of length 2.");
    }
    IPosition rs = rateWindow_.shape();
    if (rs.size()!=1 || rs.nelements() != 1) {
        throw AipsError("ratewindow must be a list of length 2.");
    }
    Int nCorrOrig(sdbs(0).nCorrelations());
    nCorr_ = (nCorrOrig> 1 ? 2 : 1); // number of p-hands

    for (Int i=0; i<nCorr_; i++) {
        activeAntennas_[i].insert(refant_);
    }
    
    // when we get the visCubecorrected it is already
    // reduced to parallel hands, but there isn't a
    // corresponding method for flags.
    Int corrStep = (nCorrOrig > 2 ? 3 : 1); // step for p-hands

    for (Int ibuf=0; ibuf != sdbs.nSDB(); ibuf++) {
        SolveDataBuffer& s(sdbs(ibuf));
        for (Int irow=0; irow!=s.nRows(); irow++) {
            Int a1(s.antenna1()(irow)), a2(s.antenna2()(irow));
            allActiveAntennas_.insert(a1);
            allActiveAntennas_.insert(a2);
        }
    }

    nElem_ =  1 + *(allActiveAntennas_.rbegin()) ;
    IPosition aggregateDim(2, nCorr_, nElem_);
    xcount_.resize(aggregateDim);
    sumw_.resize(aggregateDim);
    sumww_.resize(aggregateDim);

    xcount_ = 0;
    sumw_ = 0.0;
    sumww_ = 0.0;
    
    if (DEVDEBUG) {
        cerr << "Filling FFT grid with " << sdbs.nSDB() << " data buffers." << endl;
    }

    // Don't try to check there are multiple times here; let DelayRateFFT check that.
    IPosition paddedDataSize(4, nCorr_, nElem_, nPadT_, nPadChan_);
    Vpad_.resize(paddedDataSize);
    Int totalRows = 0;
    Int goodRows = 0;

    
    for (Int ibuf=0; ibuf != sdbs.nSDB(); ibuf++) {
        SolveDataBuffer& s(sdbs(ibuf));
        totalRows += s.nRows();
        if (!s.Ok())
            continue;

        Int nr = 0;
        for (Int irow=0; irow!=s.nRows(); irow++) {
            if (s.flagRow()(irow))
                continue;
            Int iant;
            Int a1(s.antenna1()(irow)), a2(s.antenna2()(irow));
            if (a1 == a2) {
                continue;
            }
            else if (a1 == refant_) {
                iant = a2;
            }
            else if (a2 == refant_) {
                iant = a1;
            }
            else {
                continue;
            }
            // OK, we're not skipping this one so we have to do something.

            // v has shape (nelems, ?, nrows, nchannels)
            Cube<Complex> v = s.visCubeCorrected();
            Cube<Float> w = s.weightSpectrum();
            Cube<Bool> fl = s.flagCube();
            Int spw = s.spectralWindow()(0);
            Int f_index = gm_.swStartIndex(spw);    // ditto!
            Int t_index = gm_.getTimeIndex(s.time()(0));
            Int spwchans = gm_.nSPWChan;
            IPosition start(4,      0,      iant, t_index, f_index);
            IPosition stop(4,      nCorr_,    1,       1, spwchans);
            IPosition stride(4,      1,         1,       1, 1);
            Slicer sl1(start,     stop, stride, Slicer::endIsLength);
            Slicer sl2(IPosition(3, 0,         0, irow),
                       IPosition(3, nCorr_, spwchans, 1),
                       IPosition(3, corrStep,        1,  1), Slicer::endIsLength);
                
            Slicer flagSlice(IPosition(3, 0,         0, irow),
                             IPosition(3, nCorr_, spwchans, 1),
                             IPosition(3, corrStep,        1, 1), Slicer::endIsLength);
            nr++;
            if (DEVDEBUG && 0) {
                cerr << "nr " << nr
                     << " irow " << endl
                     << "Vpad shape " << Vpad_.shape() << endl
                     << "v shape " << v.shape() << endl
                     << "sl2 " << sl2 << endl
                     << "sl1 " << sl1 << endl
                     << "flagSlice " << flagSlice << endl;
            }
            Array<Complex> rhs = v(sl2).nonDegenerate(1);
            Array<Float> weights = w(sl2).nonDegenerate(1);
                
            unitize(rhs);
            Vpad_(sl1).nonDegenerate(1) = rhs * weights;

            Array<Bool> flagged(fl(flagSlice).nonDegenerate(1));
            // Zero flagged entries.
            Vpad_(sl1).nonDegenerate(1)(flagged) = Complex(0.0);

            if (!allTrue(flagged)) {
                for (Int icorr=0; icorr<nCorr_; ++icorr) {
                    IPosition p(2, icorr, iant);
                    Bool actually = false;
                    activeAntennas_[icorr].insert(iant);
                    for (Int ichan=0; ichan != (Int) spwchans; ichan++) {
                        IPosition pchan(2, icorr, ichan);
                        if (!flagged(pchan)) {
                            Float wv = weights(pchan);
                            if (wv < 0) {
                                cerr << "spwchans " << spwchans << endl;
                                cerr << "Negative weight << (" << wv << ") on row "
                                     << irow << " baseline (" << a1 << ", " << a2 << ") "
                                     << " channel " << ichan << endl;
                                cerr << "pchan " << pchan << endl;
                                cerr << "Weights " << weights << endl;
                            }
                            xcount_(p)++;
                            sumw_(p) += wv;
                            sumww_(p) += wv*wv;
                            actually = true;
                        }
                    }
                    if (actually) {
                        activeAntennas_[icorr].insert(iant);
                        goodRows++;
                    }
                }
            }                
            if (DEVDEBUG && 0) {
                cerr << "flagged " << flagged << endl;
                cerr << "flagSlice " << flagSlice << endl
                     << "fl.shape() " << fl.shape() << endl
                     << "Vpad_.shape() " << Vpad_.shape() << endl
                     << "flagged.shape() " << flagged.shape() << endl
                     << "sl1 " << sl1 << endl;
            }
        }
    }
    if (DEVDEBUG) {
        cerr << "In DelayRateFFT::DelayRateFFT " << endl;
        printActive();
        cerr << "sumw_ " << sumw_ << endl;
        cerr << "Constructed a DelayRateFFT object." << endl;
        cerr << "totalRows " << totalRows << endl;
        cerr << "goodRows " << goodRows << endl;
    }
    
}

DelayRateFFT::DelayRateFFT(Array<Complex>& data, Int nPadFactor, Float f0, Float df, Float dt, SDBList& s,
                           Array<Double>& delayWindow, Array<Double>& rateWindow) :
    refant_(0),
    gm_(s),
    nPadFactor_(nPadFactor),
    dt_(dt),
    f0_(f0),
    df_(df),
    Vpad_(),
    sumw_(),
    sumww_(),
    param_(),
    flag_(),
    delayWindow_(delayWindow),
    rateWindow_(rateWindow) 
{
    IPosition shape = data.shape();
    nCorr_ = shape(0);
    nElem_ = shape(1);
    nt_ = shape(2);
    nChan_ = shape(3);
    nPadT_ = nPadFactor_*nt_;
    nPadChan_ = nPadFactor_*nChan_;
    IPosition paddedDataSize(4, nCorr_, nElem_, nPadT_, nPadChan_);
    Vpad_.resize(paddedDataSize);
    
    IPosition start(4, 0, 0, 0, 0);
    IPosition stop(4, nCorr_,  nElem_, nt_, nChan_);
    IPosition stride(4, 1, 1, 1, 1);
    Slicer sl1(start, stop, stride, Slicer::endIsLength);
    Vpad_(sl1) = data;

    unitize(Vpad_);

}

Matrix<Float>
DelayRateFFT::delay() const {
    IPosition start(2, 1, 0);
    IPosition stop(2, 3*nCorr_-1, nElem_-1);
    IPosition stride(2, 3, 1);
    Slicer sl1(start,  stop, stride, Slicer::endIsLast);
    return param_(sl1);
}

Matrix<Float>
DelayRateFFT::rate() const {
    IPosition start(2, 2, 0);
    IPosition stop(2, 3*nCorr_-1, nElem_-1);
    IPosition stride(2, 3, 1);
    Slicer sl1(start,  stop, stride, Slicer::endIsLast);
    return param_(sl1);
}

void
DelayRateFFT::printActive() {
    for (Int icorr=0; icorr != nCorr_; icorr++) {
        cerr << "Antennas found for correlation " << icorr << ": ";
        for (std::set<Int>::iterator it = activeAntennas_[icorr].begin(); it != activeAntennas_[icorr].end(); it++) {
            cerr << *it << ", ";
        }
        cerr << endl;
    }
    cerr << endl;
}    

void
DelayRateFFT::FFT() {
    // Axes are 0: correlation (i.e., hand of polarization), 1: antenna, 2: time, 3: channel
    Vector<Bool> ax(4, false);
    ax(2) = true;
    ax(3) = true;
    // Also copied from DelayFFT in KJones.
    // we make a copy to FFT in place.
    if (DEVDEBUG) {
        cerr << "Vpad_.shape() " << Vpad_.shape() << endl;
    }
    ArrayLattice<Complex> c(Vpad_);
    LatticeFFT::cfft0(c, ax, true);
    if (DEVDEBUG) {
        cerr << "FFT transformed" << endl;
    }
}

std::pair<Bool, Float>
DelayRateFFT::xinterp(Float alo, Float amax, Float ahi) {
    if (amax > 0.0 && alo == amax && amax == ahi)
        return std::make_pair(true, 0.5);

    Float denom(alo-2.0*amax+ahi);
    Bool cond = amax>0.0 && abs(denom)>0.0 ;
    Float fpk = cond ? 0.5-(ahi-amax)/denom : 0.0;
    return std::make_pair(cond, fpk);
}
    
void
DelayRateFFT::searchPeak() {
    // Recall param_ -> [phase, delay, rate] for each correlation
    param_.resize(3*nCorr_, nElem_); // This might be better done elsewhere.
    param_.set(0.0);
    flag_.resize(3*nCorr_, nElem_);
    flag_.set(true);  // all flagged initially
    if (DEVDEBUG) {
        cerr << "nt_ " << nt_ << " nPadChan_ " << nPadChan_ << endl;
        cerr << "Vpad_.shape() " << Vpad_.shape() << endl;
        cerr << "delayWindow_ " << delayWindow_ << endl;

    }
    
    for (Int icorr=0; icorr<nCorr_; ++icorr) {
        flag_(icorr*3 + 0, refant()) = false; 
        flag_(icorr*3 + 1, refant()) = false;
        flag_(icorr*3 + 2, refant()) = false;
        for (Int ielem=0; ielem<nElem_; ++ielem) {
            if (ielem==refant()) {
                continue;
            }
            // NB: Time, Channel
            // And once again we fail at slicing
            IPosition start(4, icorr, ielem,      0,         0);
            IPosition stop(4,     1,     1, nPadT_, nPadChan_);
            IPosition step(4,     1,     1,       1,        1);
            Slicer sl(start, stop, step, Slicer::endIsLength);
            Matrix<Complex> aS = Vpad_(sl).nonDegenerate();
            Int sgn = (ielem < refant()) ? 1 : -1;

            // Below is the gory details for turning delay window into index range
            Double bw = Float(nPadChan_)*df_;
            Double d0 = sgn*delayWindow_(IPosition(1, 0));
            Double d1 = sgn*delayWindow_(IPosition(1, 1));
            if (d0 > d1) std::swap(d0, d1);
            d0 = max(d0, -0.5/df_);
            d1 = min(d1, (0.5-1/nPadChan_)/df_);

            // It's simpler to keep the ranges as signed integers and
            // handle the wrapping of the FFT in the loop over
            // indices. Recall that the FFT result returned has indices
            // that run from 0 to nPadChan_/2 -1 and then from
            // -nPadChan/2 to -1, so far as our delay is concerned.
            Int i0 = bw*d0;
            Int i1 = bw*d1;
            if (i1==i0) i1++;
            // Now for the gory details for turning rate window into index range
            Double width = nPadT_*dt_*1e9*f0_;
            Double r0 = sgn*rateWindow_(IPosition(1,0));
            Double r1 = sgn*rateWindow_(IPosition(1,1));
            if (r0 > r1) std::swap(r0, r1);
            r0 = max(r0, -0.5/(dt_*1e9*f0_));
            r1 = min(r1, (0.5 - 1/nPadT_)/(dt_*1e9*f0_));
            
            Int j0 = width*r0;
            Int j1 = width*r1;
            if (j1==j0) j1++;
            if (DEVDEBUG) {
                cerr << "Checking the windows for delay and rate search." << endl;
                cerr << "bw " << bw << endl;
                cerr << "d0 " << d0 << " d1 " << d1 << endl;
                cerr << "i0 " << i0 << " i1 " << i1 << endl; 
                cerr << "r0 " << r0 << " r1 " << r1 << endl;
                cerr << "j0 " << j0 << " j1 " << j1 << endl; 
            }
            Matrix<Float> amp(amplitude(aS));
            Int ipkch(0);
            Int ipkt(0);
            Float amax(-1.0);
            // Unlike KJones we have to iterate in time too
            for (Int itime0=j0; itime0 != j1; itime0++) {
                Int itime = (itime0 < 0) ? itime0 + nPadT_ : itime0;
                for (Int ich0=i0; ich0 != i1; ich0++) {
                    Int ich = (ich0 < 0) ? ich0 + nPadChan_ : ich0;
                    // cerr << "Gridpoint " << itime << ", " << ich << "->" << amp(itime, ich) << endl;
                    if (amp(itime, ich) > amax) {
                        ipkch = ich;
                        ipkt  = itime;
                        amax=amp(itime, ich);
                    }
                }
            }
            // Finished grovelling. Now we have the location of the
            // maximum amplitude.
            Float alo_ch = amp(ipkt, (ipkch > 0) ? ipkch-1 : nPadChan_-1);
            Float ahi_ch = amp(ipkt, ipkch<(nPadChan_-1) ? ipkch+1 : 0);
            std::pair<Bool, Float> maybeFpkch = xinterp(alo_ch, amax, ahi_ch);
            // We handle wrapping while looking for neighbours
            Float alo_t = amp(ipkt > 0 ? ipkt-1 : nPadT_ -1,     ipkch);
            Float ahi_t = amp(ipkt < (nPadT_ -1) ? ipkt+1 : 0,   ipkch);
            if (DEVDEBUG) {
                cerr << "For element " << ielem << endl;
                cerr << "In channel dimension ipkch " << ipkch << " alo " << alo_ch
                     << " amax " << amax << " ahi " << ahi_ch << endl;
                cerr << "In time dimension ipkt " << ipkt << " alo " << alo_t
                     << " amax " << amax << " ahi " << ahi_t << endl;
            }
            std::pair<Bool, Float> maybeFpkt = xinterp(alo_t, amax, ahi_t);

            if (maybeFpkch.first and maybeFpkt.first) {
                // Phase
                Complex c = aS(ipkt, ipkch);
                Float phase = arg(c);
                param_(icorr*3 + 0, ielem) = sgn*phase;
                Float delay = (ipkch)/Float(nPadChan_);
                if (delay > 0.5) delay -= 1.0;           // fold
                delay /= df_;                           // nsec
                param_(icorr*3 + 1, ielem) = sgn*delay; //
                Double rate = (ipkt)/Float(nPadT_);
                if (rate > 0.5) rate -= 1.0;
                Double rate0 = rate/dt_;
                Double rate1 = rate0/(1e9 * f0_); 

                param_(icorr*3 + 2, ielem) = Float(sgn*rate1); 
                if (DEVDEBUG) {
                    cerr << "maybeFpkch.second=" << maybeFpkch.second
                         << ", df_= " << df_ 
                         << " fpkch=" << (ipkch + maybeFpkch.second) << endl;
                    cerr << " maybeFpkt.second=" << maybeFpkt.second
                         << " rate0=" << rate
                         << " 1e9 * f0_=" << 1e9 * f0_ 
                         << ", dt_=" << dt_
                         << " fpkt=" << (ipkt + maybeFpkt.second) << endl;
                        
                }
                if (DEVDEBUG) {
                    cerr << "Found peak for element " << ielem << " correlation " << icorr
                         << " ipkt=" << ipkt << "/" << nPadT_ << ", ipkch=" << ipkch << "/" << nPadChan_
                         << " peak=" << amax 
                         << "; delay " << delay << ", rate " << rate
                         << ", phase " << arg(c) << " sign= " << sgn << endl;
                }
                // Set 3 flags.
                flag_(icorr*3 + 0, ielem)=false; 
                flag_(icorr*3 + 1, ielem)=false;
                flag_(icorr*3 + 2, ielem)=false;
            }
            else {
                if (DEVDEBUG) {
                    cerr << "No peak in 2D FFT for element " << ielem << " correlation " << icorr << endl;
                }
            }
        }
    }
}


Float
DelayRateFFT::snr(Int icorr, Int ielem, Float delay, Float rate) {
    // We calculate a signal-to-noise ration for the 2D FFT fringefit
    // using a formula transcribed from AIPS FRING.
    //
    // Have to convert delay and rate back into indices on the padded 2D grid.
    Int sgn = (ielem < refant()) ? 1 : -1;
    delay *= sgn*df_;
    if (delay < 0.0) delay += 1;
    Int ichan = Int(delay*nPadChan_ + 0.5); 
    if (ichan == nPadChan_) ichan = 0;
        
    rate *= sgn*1e9 * f0_;
    rate *= dt_;
    if (rate < 0.0) rate += 1;
    Int itime = Int(rate*nPadT_ + 0.5);
    if (itime == nPadT_) itime = 0;
    // What about flags? If the datapoint closest to the computed
    // delay and rate values is flagged we probably shouldn't use
    // it, but what *should* we use?
    IPosition ipos(4, icorr, ielem, itime, ichan);
    IPosition p(2, icorr, ielem);
    Complex v = Vpad_(ipos);
    Float peak = abs(v);
    if (peak > 0.999*sumw_(p)) peak=0.999*sumw_(p);
    // xcount is number of data points for baseline to ielem
    // sumw is sum of weights,
    // sumww is sum of squares of weights

    Float cwt;
    if (fabs(sumw_(p))<FLT_EPSILON) {
        cwt = 0;
        if (DEVDEBUG) {
            cerr << "Correlation " << icorr << " antenna " << ielem << ": sum of weights is zero." << endl;
        }
    } else {
        Float x = C::pi/2*peak/sumw_(p);
        // The magic numbers in the following formula are from AIPS FRING
        cwt = (pow(tan(x), 1.163) * sqrt(sumw_(p)/sqrt(sumww_(p)/xcount_(p))));
        if (DEVDEBUG) {
            cerr << "Correlation " << icorr << " antenna " << ielem << " ipos " << ipos
                 << " peak=" << peak << "; xang=" << x << "; xcount=" << xcount_(p) << "; sumw=" << sumw_(p) << "; sumww=" << sumww_(p)
                 << " snr " << cwt << endl;
        }
    }
    return cwt;
}
    

void DelayRateFFT::removeAntennasCorrelation(Int icor, std::set< Int > s) {
    std::set< Int > & as = activeAntennas_.find(icor)->second;
    for (std::set< Int >::iterator it=s.begin(); it!=s.end(); it++) {
        as.erase(*it);
    }
}


// Start of GSL compliant solver

class AuxParamBundle {
public:
    SDBList &sdbs;    
    size_t nCalls;
private:
    // We make sure there are no copy or default constructors to
    // preserve the integrity of our reference member.
    AuxParamBundle();
    AuxParamBundle(AuxParamBundle const&);
    AuxParamBundle const& operator=(AuxParamBundle const&);

    size_t refant;
    size_t nCorrelations;
    size_t corrStep;
    Double t0;
    Double reftime;
    std::map< Int, std::set< Int > > activeAntennas;
    std::map< Int, Int > antennaIndexMap;
    // Can't I just have a vector, which maps indices to values anyway?
    std::vector<bool> parameterFlags;
    Int nParams;
    std::map< Int, Int > parameterMap;
    Int activeCorr;
public:
    AuxParamBundle(SDBList& sdbs_, size_t refant, const std::map< Int, std::set<Int> >& activeAntennas_, Vector<Bool> paramActive) :
        sdbs(sdbs_),
        nCalls(0),
        refant(refant),
        nCorrelations(sdbs.nCorrelations() > 1 ? 2 : 1),
        corrStep(sdbs.nCorrelations() > 2 ? 3 : 1),
        activeAntennas(activeAntennas_),
        parameterFlags(),
        parameterMap(),
        activeCorr(-1)
        // corrStep(3)
        {
            Int last_index = sdbs.nSDB() - 1 ;
            t0 = sdbs(0).time()(0);
            Double tlast = sdbs(last_index).time()(0);
            reftime = 0.5*(t0 + tlast);

            parameterFlags = paramActive.tovector();
            Int j = 0; // The CASA parameter index (0=peculiar phase, 1=delay, 2=rate, 3=dispersive)
            Int i = 0; // the Least Squares parameter vector index, depending on what's being solved for
            for (auto p=parameterFlags.begin(); p!=parameterFlags.end(); p++) {
                if (*p) {
                    parameterMap.insert(std::pair<Int, Int>(j, i));
                    i++;
                }
                j++;
            }
            if (i==0) {
                throw(AipsError("No parameters specified!"));
            }
            nParams = i; // There's always at least one parameter!
            // cerr << "AuxParamBundle reftime " << reftime << " t0 " << t0 <<" dt " << tlast - t0 << endl;
        }

    Int nParameters() {
        return nParams;
    }
    Double get_t0() {
        return t0;
    }
    Double
    get_ref_time() {
        return reftime;
    }
    size_t
    get_num_corrs() {
        //return sdbs.nCorrelations() > 1 ? 2 : 1;
        return nCorrelations;
    }
    size_t
    get_num_antennas() {
        if (activeCorr < 0) {
            throw(AipsError("Correlation out of range."));
        }
        std::set< Int > ants = activeAntennas.find(activeCorr)->second;
        return (size_t) ants.size();
    }
    size_t
    get_max_antenna_index() {
        if (activeCorr < 0) {
            throw(AipsError("Correlation out of range."));
        }
        return *(activeAntennas.find(activeCorr)->second.rbegin());
    }
    // Sometimes there is Int, sometimes size_t; the following ones are casacore::Int.
    Int
    get_num_data_points() {
        Int nTotalRows = 0;
        for (Int i = 0; i != sdbs.nSDB(); i++) {
            nTotalRows += sdbs(i).nRows();
        }
        return nTotalRows * sdbs.nChannels();
    }
    Int
    get_actual_num_data_points() {
        Int nTotalRows = 0;
        for (Int i = 0; i != sdbs.nSDB(); i++) {
            SolveDataBuffer& s (sdbs(i));
            for (Int irow=0; irow!=s.nRows(); irow++) {
                if (s.flagRow()(irow)) continue;
                nTotalRows++;
            }
        }
        return nTotalRows * sdbs.nChannels();
    }
    size_t 
    get_data_corr_index(size_t icorr) {
        if (icorr > nCorrelations) {
            throw(AipsError("Correlation out of range."));
        }
        size_t dcorr = icorr * corrStep;
        return dcorr;
    }
    bool
    isActive(size_t iant) {
        std::set<Int> ants = activeAntennas.find(activeCorr)->second;
        if (iant == refant) return true;
        else return (ants.find(iant) != ants.end());
    }
    Int
    get_param_corr_param_index(size_t iant0, size_t ipar) {
        if (iant0 == refant) return -1;
        int iant1 = antennaIndexMap[iant0];
        if (iant1 > antennaIndexMap[refant]) {
            iant1 -= 1;
        }
        int ipar1;
        auto p = parameterMap.find(ipar);
        if (p==parameterMap.end()) {
            ipar1 = -1;
        }
        else {
            ipar1 = (iant1 * nParameters()) + p->second;
        }
        return ipar1;
    }
    size_t
    get_active_corr() {
        return activeCorr;
    }
    void
    set_active_corr(size_t icorr) {
        activeCorr = icorr;
        antennaIndexMap.clear();
        Int i = 0;
        std::set<Int>::iterator it;
        std::set<Int> ants = activeAntennas.find(activeCorr)->second;
        for (it = ants.begin(); it != ants.end(); it++) {
            antennaIndexMap[*it] = i++;
        }
    }
};



void
print_baselines(std::set<std::pair< Int, Int > > baselines) {
    cerr << "Baselines encountered ";
    std::set<std::pair< Int, Int > >::iterator it;
    for (it=baselines.begin(); it != baselines.end(); ++it) {
        cerr << "(" << it->first << ", " << it->second << ") ";
    }
    cerr << endl;
}


int
expb_f(const gsl_vector *param, void *d, gsl_vector *f)
{
    AuxParamBundle *bundle = (AuxParamBundle *)d;
    SDBList& sdbs = bundle->sdbs;
    Double refTime = bundle->get_t0();
    
    gsl_vector_set_zero(f);
    //    Vector<Double> freqs = sdbs.freqs();

    const Double reffreq0=sdbs(0).freqs()(0);  // First freq in first SDB
    
    size_t count = 0; // This is the master index.

    Double sumwt = 0.0;
    Double xi_squared = 0.0;

    for (Int ibuf=0; ibuf < sdbs.nSDB(); ibuf++) {
        SolveDataBuffer& s (sdbs(ibuf));
        if (!s.Ok()) continue;

	const Vector<Double> freqs(s.freqs()); // This ibuf's freqs
        Float fmin = min(freqs);
        Float fmax = max(freqs);

        Cube<Complex> v = s.visCubeCorrected();
        Cube<Bool> fl = s.flagCube();
        Cube<Float> weights = s.weightSpectrum();
           
        for (Int irow=0; irow!=s.nRows(); irow++) {
            if (s.flagRow()(irow)) continue;

            Int ant1(s.antenna1()(irow));
            Int ant2(s.antenna2()(irow));
            if (!bundle->isActive(ant1) || !bundle->isActive(ant2))
                continue;            
            if (ant1==ant2) continue;

            // VisBuffer.h seems to suggest that a vb.visCube may have shape
            // (nCorr(), nChannel(), nRow())
            size_t icorr0 = bundle->get_active_corr();
            size_t dcorr = bundle->get_data_corr_index(icorr0);
            // We also need to get the right parameters for this,
            // polarization (icorr is an encoding of the
            // polarization of the correlation products).
            
            Double phi0, tau, r, disp;
            {
                Int i;
                Double phi0_1, tau1, r1, disp1;
                Double phi0_2, tau2, r2, disp2;
                
                phi0_1 = ((i = bundle->get_param_corr_param_index(ant1, 0))>=0) ? gsl_vector_get(param, i) : 0.0;
                tau1   = ((i = bundle->get_param_corr_param_index(ant1, 1))>=0) ? gsl_vector_get(param, i) : 0.0;
                r1     = ((i = bundle->get_param_corr_param_index(ant1, 2))>=0) ? gsl_vector_get(param, i) : 0.0;
                disp1  = ((i = bundle->get_param_corr_param_index(ant1, 3))>=0) ? gsl_vector_get(param, i) : 0.0;
                
                phi0_2 = ((i = bundle->get_param_corr_param_index(ant2, 0))>=0) ? gsl_vector_get(param, i) : 0.0;
                tau2   = ((i = bundle->get_param_corr_param_index(ant2, 1))>=0) ? gsl_vector_get(param, i) : 0.0;
                r2     = ((i = bundle->get_param_corr_param_index(ant2, 2))>=0) ? gsl_vector_get(param, i) : 0.0;
                disp2  = ((i = bundle->get_param_corr_param_index(ant2, 3))>=0) ? gsl_vector_get(param, i) : 0.0;
                
                phi0 = phi0_2 - phi0_1;
                tau = tau2 - tau1;
                r = r2-r1;
                disp = disp2-disp1;
            }
            
            for (size_t ichan = 0; ichan != v.ncolumn(); ichan++) {
                if (fl(dcorr, ichan, irow)) continue;

                Float freq = freqs(ichan);
                Float k_disp = KDISPSCALE*C::_2pi*(1.0/freq + (freq-fmin-fmax)/(fmin*fmax));

                Complex vis = v(dcorr, ichan, irow);
                Double w0 = weights(dcorr, ichan, irow);
                // FIXME: what should we use to scale the weights?
                // Double weightScale = norm(vis);
                // Double weightScale = abs(vis);
                // Double weightScale = 1;
                // Double weightScale = 1/sqrt(w0); // Actually AIPS 0, not AIPS 1!
                // Double weightScale = pow(w0, -0.75); // AIPS 2
                //  Double weightScale = 1/w0; // AIPS 3
                // Double weightScale = norm(vis); // Casa 1, I guess
                Double w = sqrt(w0);
                sumwt += w*w;
                if (fabs(w) < FLT_EPSILON) continue;
                // We have to turn the delay back into seconds from nanoseconds.
                // Freq difference is in Hz, which comes out typically as 1e6 bands
                //Double wDf = C::_2pi*(freqs(ichan) - freqs(0))*1e-9;
                Double wDf = C::_2pi*(freqs(ichan) - reffreq0)*1e-9;
                //
                Double t1 = s.time()(0);
                // FIXME: Remind me why we *do* scale wDf with 1e-9
                // but do *not* do that with ref_freq?
                // I have a theory which is mine:
                // this is because tau is in nanoseconds.
                //Double ref_freq = freqs(0);
                //Double wDt = C::_2pi*(t1 - refTime) * ref_freq; 
                Double wDt = C::_2pi*(t1 - refTime) * reffreq0; 

                Double mtheta = -(phi0 + tau*wDf + r*wDt + disp*k_disp); 
                Double vtheta = arg(vis);

                Double c_r = w*(cos(mtheta) - cos(vtheta));
                Double c_i = w*(sin(mtheta)  - sin(vtheta));
                gsl_vector_set(f, count, c_r);
                gsl_vector_set(f, count+1, c_i);

                count += 2;
                xi_squared += c_r*c_r + c_i*c_i;
            }
        }
    }
    // cerr << "Residual xi-squared = " << xi_squared << endl;
    return GSL_SUCCESS;
}

    
int
expb_df(CBLAS_TRANSPOSE_t TransJ, const gsl_vector* param, const gsl_vector *u, void *bundle_, gsl_vector *v, gsl_matrix *JTJ)
{

    // param is the current vector for which we're finding the jacobian.
    // if TransJ is true, evaluate J^T u and store in v.
    // Also store J^T . J in lower half of JTJ.
    std::set <std::pair < Int, Int> > baselines;
    AuxParamBundle *bundle = (AuxParamBundle *)bundle_;

    SDBList& sdbs = bundle->sdbs;
    //Vector<Double> freqs = sdbs.freqs();

    const Double reffreq0=sdbs(0).freqs()(0);  // First freq in first SDB

    size_t count = 0; // This is the master index.

    gsl_vector_set_zero(v);
    gsl_matrix_set_zero(JTJ);
    
    Double refTime = bundle->get_t0();

    for (Int ibuf=0; ibuf < sdbs.nSDB(); ibuf++) {
        SolveDataBuffer& s (sdbs(ibuf));
        if (!s.Ok()) continue;

	const Vector<Double> freqs(s.freqs()); // This ibuf's freq
        Float fmin = min(freqs);
        Float fmax = max(freqs);
        
        Cube<Complex> vis = s.visCubeCorrected();
        Cube<Bool> fl = s.flagCube();
        Cube<Float> weights = s.weightSpectrum();

        Double t1 = s.time()(0);
        // cerr << "ibuf " << ibuf << " t1 - t0 = " << t1 - t0 << endl;
        for (Int irow=0; irow!=s.nRows(); irow++) {
            if (s.flagRow()(irow)) continue;

            Int ant1(s.antenna1()(irow));
            Int ant2(s.antenna2()(irow));
            
            if (ant1==ant2) continue;
            if (!bundle->isActive(ant1) || !bundle->isActive(ant2)) {
                // cerr << "Skipping " << ant1 << ", " << ant2 << endl;                   
                continue;
            }

            // VisBuffer.h seems to suggest that a vb.visCube may have shape
            // (nCorr(), nChannel(), nRow()) 

            size_t icorr0 = bundle->get_active_corr();
            size_t dcorr = bundle->get_data_corr_index(icorr0);
            // We also need to get the right parameters for this
            // polarization (icorr is an encoding of the
            // polarization of the correlation products).
            
            Double phi0, tau, r, disp;
            {
                Int i;
                Double phi0_1, tau1, r1, disp1;
                Double phi0_2, tau2, r2, disp2;
                
                phi0_1 = ((i = bundle->get_param_corr_param_index(ant1, 0))>=0) ? gsl_vector_get(param, i) : 0.0;
                tau1   = ((i = bundle->get_param_corr_param_index(ant1, 1))>=0) ? gsl_vector_get(param, i) : 0.0;
                r1     = ((i = bundle->get_param_corr_param_index(ant1, 2))>=0) ? gsl_vector_get(param, i) : 0.0;
                disp1  = ((i = bundle->get_param_corr_param_index(ant1, 3))>=0) ? gsl_vector_get(param, i) : 0.0;
                
                phi0_2 = ((i = bundle->get_param_corr_param_index(ant2, 0))>=0) ? gsl_vector_get(param, i) : 0.0;
                tau2   = ((i = bundle->get_param_corr_param_index(ant2, 1))>=0) ? gsl_vector_get(param, i) : 0.0;
                r2     = ((i = bundle->get_param_corr_param_index(ant2, 2))>=0) ? gsl_vector_get(param, i) : 0.0;
                disp2  = ((i = bundle->get_param_corr_param_index(ant2, 3))>=0) ? gsl_vector_get(param, i) : 0.0;
                
                phi0 = phi0_2 - phi0_1;
                tau = tau2 - tau1;
                r = r2-r1;
                disp = disp2-disp1;
            }

                
            //Double ref_freq = freqs(0); 
            //Double wDt = C::_2pi*(t1 - refTime) * ref_freq; 
            Double wDt = C::_2pi*(t1 - refTime) * reffreq0; 
            bool found_data = false;

            for (size_t ichan = 0; ichan != vis.ncolumn(); ichan++) {
                if (fl(dcorr, ichan, irow)) continue;
                Double w0 = weights(dcorr, ichan, irow);
                Double w = sqrt(w0);
                if (fabs(w) < FLT_EPSILON) continue;
                found_data = true;

                Float freq = freqs(ichan);
                Float k_disp = KDISPSCALE*C::_2pi*(1.0/freq + (freq-fmin-fmax)/(fmin*fmax));
                    
                // Add a 1e-9 factor because tau parameter is in nanoseconds.
                //Double wDf = C::_2pi*(freqs(ichan) - freqs(0))*1e-9;
                Double wDf = C::_2pi*(freqs(ichan) - reffreq0)*1e-9;
                //
                Double mtheta = -(phi0 + tau*wDf + r*wDt + disp*k_disp);
                Double ws = sin(mtheta);
                Double wc = cos(mtheta);

                Double p0 = 1.0;
                Double p1 = wDf;
                Double p2 = wDt;
                Double p3 = k_disp;
                
                Vector<Double> dterm2(4);
                dterm2(0) = -p0;
                dterm2(1) = -p1;
                dterm2(2) = -p2;
                dterm2(3) = -p3;

                Vector<Double> dterm1(4);
                dterm1(0) = p0;
                dterm1(1) = p1;
                dterm1(2) = p2;
                dterm1(3) = p3;
                
                /* 
                   What we want to express is just:
                   J[count + 0, iparam2 + 0] = w*-ws*-1.0; 
                   J[count + 1, iparam2 + 0] = w*+wc*-1.0;
                   J[count + 0, iparam2 + 1] = w*-ws*-wDf;
                   J[count + 1, iparam2 + 1] = w*+wc*-wDf;
                   J[count + 0, iparam2 + 2] = w*-ws*-wDt;
                   J[count + 1, iparam2 + 2] = w*+wc*-wDt;
                   
                   But in the GSL multilarge framework we have to
                   be ready to calculate either J*u for a given u
                   or J^T*u, depending on the flag TransJ, and we also have to fill in the 
                   
                   v[iparam + ...] = J[count + ..., iparam + ...] * u[iparam + ...]
                   
                   or
                   
                   v[iparam + ...] = J^T[iparam + ..., count + ...] * u[count + ...]
                   
                   <https://www.gnu.org/software/gsl/doc/html/nls.html#c.gsl_multifit_nlinear_default_parameters>
                   
                   "Additionally, the normal equations matrix J^T J should be stored in the lower half of JTJ."
                   
                   So we should also use
                   JTJ[iparam + ..., iparam + ...] += J^T[iparam + ..., count + ...] J[count + ..., iparam + ...] 
                   
                */
                if (TransJ==CblasNoTrans) {
                    for (Int di=0; di<4; di++) {
                        Int i;
                        if ((i = bundle->get_param_corr_param_index(ant2, di))>=0) {
                            (*gsl_vector_ptr(v, count + 0)) += (w*-ws*dterm2(di)) * gsl_vector_get(u, i);
                            (*gsl_vector_ptr(v, count + 1)) += (w*+wc*dterm2(di)) * gsl_vector_get(u, i);
                        }
                        if ((i = bundle->get_param_corr_param_index(ant1, di))>=0) {
                            (*gsl_vector_ptr(v, count + 0)) += gsl_vector_get(u, i) * (w*-ws*dterm1(di));
                            (*gsl_vector_ptr(v, count + 1)) += gsl_vector_get(u, i) * (w*+wc*dterm1(di));
                        }
                    }
                } else {
                    for (Int di=0; di<4; di++) {
                        Int i;
                        if ((i = bundle->get_param_corr_param_index(ant2, di))>=0) {
                            (*gsl_vector_ptr(v, i)) += (w*-ws*dterm2(di)) * gsl_vector_get(u, count + 0);
                            (*gsl_vector_ptr(v, i)) += (w*+wc*dterm2(di)) * gsl_vector_get(u, count + 1);
                        }
                        if ((i = bundle->get_param_corr_param_index(ant1, di))>=0) {
                            (*gsl_vector_ptr(v, i)) += gsl_vector_get(u, count + 0) * (w*-ws*dterm1(di));
                            (*gsl_vector_ptr(v, i)) += gsl_vector_get(u, count + 1) * (w*+wc*dterm1(di));
                        }
                    }
                }
                if (JTJ) {
                    Int i, j;
                    Double wterm = (-ws) * (-ws) + (+wc) * (+wc);
                    if (fabs(1-wterm) > 1e-15)
                        throw AipsError("Insufficiently at one");
                    for (Int di=0; di<4; di++) {
                        for (Int dj=0; dj<=di; dj++) {
                            if (((i = bundle->get_param_corr_param_index(ant2, di))>=0) &&
                                ((j = bundle->get_param_corr_param_index(ant2, dj))>=0)) {
                                (*gsl_matrix_ptr(JTJ, i, j)) += w0*dterm2(di)*dterm2(dj);
                            }
                            if (((i = bundle->get_param_corr_param_index(ant1, di))>=0) &&
                                ((j = bundle->get_param_corr_param_index(ant1, dj))>=0)) {
                                (*gsl_matrix_ptr(JTJ, i, j)) += w0*dterm1(di)*dterm1(dj);
                            }
                        }
                    }
                    // iant1 != iant2, so we don't have to worry about collisions
                    for (Int di=0; di<4; di++) {
                        for (Int dj=0; dj<4; dj++) {
                            Int i0, j0;
                            if (((i0 = bundle->get_param_corr_param_index(ant1, di))>=0) &&
                                ((j0 = bundle->get_param_corr_param_index(ant2, dj))>=0)) {
                                Int i1 = max(i0, j0);
                                Int j1 = min(i0, j0);
                                (*gsl_matrix_ptr(JTJ, i1, j1)) += w0*dterm2(di)*dterm1(dj);
                            }
                        }
                    }
                    count += 2;
                } // if JTJ
            } // loop over channels
            if (found_data) {
                std::pair<Int, Int> antpair = std::make_pair(ant1, ant2);
                bool newBaseline = (baselines.find(antpair) == baselines.end());
                if (newBaseline) {
                    baselines.insert(antpair);
                    // cerr << "paramFlagging for antenna "<< ant1 << ": ";
                    // for (size_t di=0; di<4; di++) {
                    //     cerr << (bundle->get_param_corr_param_index(ant1, di)>=0) << " ";
                    // }
                    // cerr << endl;
                    // cerr << "indices for antenna "<< ant1 << ": ";
                    // if (bundle->get_param_corr_param_index(ant1, 0) >= 0) { 
                    //     for (size_t di=0; di<4; di++) {
                    //         cerr << bundle->get_param_corr_param_index(ant1, di) << " ";
                    //     }                    
                    //     cerr << endl;
                    // }
                    // cerr << "phi0 " << phi0 << " tau " << tau << " r " << r << endl;
                }
            }
        }
    }
    if (DEVDEBUG && 0) {
        print_baselines(baselines);
        cerr << "count " << count << endl;
        cerr << "v = ";
        for (size_t i=0; i!=v->size; i++) {
            cerr << gsl_vector_get(v, i) << " ";
        }
        cerr << endl;
        // if (JTJ) {
        //     cerr <<"JTJ " << std::scientific << endl;
        //     for (size_t i=0; i!=JTJ->size1; i++) {
        //         for (size_t j=0; j!=JTJ->size2; j++) {
        //             cerr << gsl_matrix_get(JTJ, i, j) << " ";
        //         }
        //         cerr << endl;
        //     }
        //     cerr << endl;
        // }
    }
    return GSL_SUCCESS;
}
    


int
expb_hess(gsl_vector *param, AuxParamBundle *bundle, gsl_matrix *hess, Double xi_squared, gsl_vector *snr_vector, LogIO& logSink)
{
    // We calculate the diagonal for the hessian as used by AIPS for
    // the signal to noise. The AIPS formulation, by Fred Schwab, is a
    // hand-rolled routine that solves a different problem to ours: by
    // using a triangular matrix for the Jacobian (requiring antenna i<
    // antenna j) the J^T * J term vanishes throughout and the Hessian
    // of the Xi^2 functional *only* includes the second-order
    // derivative terms, which are usually neglected.
    //
    // This is very clever, but it also means different covariance and
    // information matrices, and therefore a different SNR.  Here we
    // use a generic least squares solver but we cheat slightly and use
    // the AIPS form for the Hessian and SNR calculations.
    //
    // FIXME: Is there any compelling reason to use gsl_vectors for
    // this, given that we're not really hooked in to the gsl
    // least squares framework by the time we do this?
    
    SDBList& sdbs = bundle->sdbs;
    Double refTime = bundle->get_t0();

    // Dimensions of (num_antennas); is the same dimension as
    // param vector here.
    gsl_matrix_set_zero(hess);

    const Double reffreq0=sdbs(0).freqs()(0);  // First freq in first SDB

    size_t nobs = 0;
    Double sumwt = 0;
    size_t numpar = param->size;

    for (Int ibuf=0; ibuf < sdbs.nSDB(); ibuf++)
    {
        SolveDataBuffer& s (sdbs(ibuf));
        if (!s.Ok()) continue;

	const Vector<Double> freqs(s.freqs()); // This ibuf's freqs
        Float fmin = min(freqs);
        Float fmax = max(freqs);
            
        Cube<Complex> v = s.visCubeCorrected();
        Cube<Bool> fl = s.flagCube();
        Cube<Float> weights = s.weightSpectrum();
           
            
        for (Int irow=0; irow!=s.nRows(); irow++) {
            if (s.flagRow()(irow)) continue;

            Int ant1(s.antenna1()(irow));
            Int ant2(s.antenna2()(irow));
            if (!bundle->isActive(ant1) || !bundle->isActive(ant2))
                continue;            
            if (ant1==ant2) continue;

            // VisBuffer.h seems to suggest that a vb.visCube may have shape
            // (nCorr(), nChannel(), nRow())
            size_t icorr0 = bundle->get_active_corr();
            size_t dcorr = bundle->get_data_corr_index(icorr0);
            // We also need to get the right parameters for this,
            // polarization (icorr is an encoding of the
            // polarization of the correlation products).
            
            Double phi0, tau, r, disp;
            {
                Int i;
                Double phi0_1, tau1, r1, disp1;
                Double phi0_2, tau2, r2, disp2;
                
                phi0_1 = ((i = bundle->get_param_corr_param_index(ant1, 0))>=0) ? gsl_vector_get(param, i) : 0.0;
                tau1   = ((i = bundle->get_param_corr_param_index(ant1, 1))>=0) ? gsl_vector_get(param, i) : 0.0;
                r1     = ((i = bundle->get_param_corr_param_index(ant1, 2))>=0) ? gsl_vector_get(param, i) : 0.0;
                disp1  = ((i = bundle->get_param_corr_param_index(ant1, 3))>=0) ? gsl_vector_get(param, i) : 0.0;

                
                phi0_2 = ((i = bundle->get_param_corr_param_index(ant2, 0))>=0) ? gsl_vector_get(param, i) : 0.0;
                tau2   = ((i = bundle->get_param_corr_param_index(ant2, 1))>=0) ? gsl_vector_get(param, i) : 0.0;
                r2     = ((i = bundle->get_param_corr_param_index(ant2, 2))>=0) ? gsl_vector_get(param, i) : 0.0;  
                disp2  = ((i = bundle->get_param_corr_param_index(ant2, 3))>=0) ? gsl_vector_get(param, i) : 0.0;
              
                phi0 = phi0_2 - phi0_1;
                tau = tau2 - tau1;
                r = r2-r1;
                disp = disp2-disp1;
            }

            for (size_t ichan = 0; ichan != v.ncolumn(); ichan++) {
                if (fl(dcorr, ichan, irow)) continue;
                Complex vis = v(dcorr, ichan, irow);
                // Fixme: this isn't a square root.
                Double w0 = weights(dcorr, ichan, irow);
                sumwt += w0;
                Double w = w0;
                nobs++;
                if (fabs(w) < FLT_EPSILON) continue;
                
                // We have to turn the delay back into seconds from nanoseconds.
                // Freq difference is in Hz, which comes out typically as 1e6 bands
                //Double wDf = C::_2pi*(freqs(ichan) - freqs(0))*1e-9;
                Double wDf = C::_2pi*(freqs(ichan) - reffreq0)*1e-9;
                //
                Double t1 = s.time()(0);

                //Double ref_freq = freqs(0);
                //Double wDt = C::_2pi*(t1 - refTime) * ref_freq;
                Double wDt = C::_2pi*(t1 - refTime) * reffreq0; 

                Float freq = freqs(ichan);
                Float k_disp = KDISPSCALE*C::_2pi*(1.0/freq + (freq-fmin-fmax)/(fmin*fmax));

                Double mtheta = -(phi0 + tau*wDf + r*wDt + disp*k_disp); 
                Double vtheta = arg(vis);

                // Hold on a minute though! 
                Double cx = w*cos(vtheta - mtheta);

                Matrix<Double> dterm(4,4);
                dterm(0, 0) = cx;
                dterm(0, 1) = wDf*cx;
                dterm(0, 2) = wDt*cx;
                dterm(0, 3) = k_disp*dterm(0, 1);
                dterm(1, 1) = wDf*dterm(0, 1);
                dterm(1, 2) = wDt*dterm(0, 1);
                dterm(1, 3) = wDf*dterm(0, 3);
                dterm(2, 2) = wDt*dterm(1, 2);
                dterm(2, 3) = wDt*dterm(1, 3);
                dterm(3, 3) = k_disp*dterm(2, 3);

                // Symmetry terms:
                dterm(1, 0) = dterm(0, 1);
                dterm(2, 0) = dterm(0, 2);
                dterm(3, 0) = dterm(0, 3);
                dterm(2, 1) = dterm(1, 2);
                dterm(3, 1) = dterm(1, 3);
                dterm(3, 2) = dterm(2, 3);
                


                for (Int di=0; di<4; di++) {
                    for (Int dj=0; dj<4; dj++) {
                        Int i, j;
                        if (((i = bundle->get_param_corr_param_index(ant1, di))>=0) &&
                            ((j = bundle->get_param_corr_param_index(ant1, dj))>=0)) {
                            *gsl_matrix_ptr(hess, i, j) += dterm(di, dj);
                        }
                        // Exactly the same logic, but with antenna2
                        if (((i = bundle->get_param_corr_param_index(ant2, di))>=0) &&
                            ((j = bundle->get_param_corr_param_index(ant2, dj))>=0)) {
                            *gsl_matrix_ptr(hess, i, j) += dterm(di, dj);
                        }
                    }
                }
                // FIXME: Not just diagonal terms any more!
                // Note that some of these are not in the lower
                // triangular part, even though they are copied
                // faithfully from AIPS which thinks it is filling
                // a triangular matrix and handles symmetry
                // later. Unless I've missed something (again).
                for (Int di=0;  di<4; di++) {
                    for (Int dj=0; dj<4; dj++) {
                        Int i, j;
                        if (((i = bundle->get_param_corr_param_index(ant1, di))>=0) &&
                            ((j = bundle->get_param_corr_param_index(ant2, dj))>=0)) {
                            *gsl_matrix_ptr(hess, i, j) -= dterm(di, dj);
                            *gsl_matrix_ptr(hess, j, i) -= dterm(dj, di);
                        } // if
                    } // dj
                } // di
            } // ichan
        } // irow
    } // ibuff
    // s is more tricky: it is the xi^2 term from exp_f
    
    xi_squared = max(xi_squared, 1e-25);

    if (DEVDEBUG && 0) {
        cerr << "The matrix is" << endl;
        cerr << setprecision(3) << scientific;
        for (size_t i = 0; i != hess->size1; i++) {
            for (size_t j = 0; j < hess->size2; j++) {
                cerr << gsl_matrix_get(hess,i,j) << " ";
            }
            cerr << endl;
        }
        cerr << endl;
        // str.unsetf(cerr:floatfield);
        cerr << std::defaultfloat;
    }
    //
    size_t hsize = hess->size1;
    int signum;
    gsl_permutation *perm = gsl_permutation_alloc (hsize);
    gsl_matrix *lu = gsl_matrix_alloc(hsize, hsize);
    gsl_matrix *inv_hess = gsl_matrix_alloc(hsize, hsize);

    gsl_linalg_LU_decomp(hess, perm, &signum);
    Double det = gsl_linalg_LU_det(hess, signum);
    if ((fabs(det) < GSL_DBL_EPSILON) || std::isnan(det)) {
        logSink << "Hessian matrix singular (determinant=" << det << "); setting signal-to-noise ratio to zero." << LogIO::POST;
       // Singular matrix; fill snrs with zero.
        for (size_t i=0; i < hess->size1; i+=bundle->nParameters()) {
            Double snr = 0;
            gsl_vector_set(snr_vector, i, snr);
        }
    }
    else {
        gsl_linalg_LU_invert(hess, perm, inv_hess);
    
        Double sigma2 = xi_squared / (nobs - numpar) * nobs / sumwt;
        // cerr << "xi_squared " << xi_squared << " Nobs " << nobs << " sumwt " << sumwt << " sigma2 " << sigma2 << endl;
        for (size_t i=0; i < hess->size1; i+=bundle->nParameters()) {
            Double h = gsl_matrix_get(inv_hess, i, i);
            Double snr0 = sqrt(sigma2*h*0.5);
            snr0 = min(snr0, 9999.999);
            Double snr = (snr0 > 1e-20) ? 1.0/snr0 : snr0;
            gsl_vector_set(snr_vector, i, snr);
        }
    }
    gsl_matrix_free(lu);
    gsl_matrix_free(inv_hess);
    gsl_permutation_free(perm);
    // SNR[i], according to aips, is 1/sqrt(sigma2*hess(i1,i1)*0.5);
    // Note that in AIPS i1 ranges over 1..NANT
    // We use 1 as a success code.
    return 1;
}


Int
findRefAntWithData(SDBList& sdbs, Vector<Int>& refAntList, Int prtlev) {
    std::set<Int> activeAntennas;
    for (Int ibuf=0; ibuf != sdbs.nSDB(); ibuf++) {
        SolveDataBuffer& s(sdbs(ibuf));
        if (!s.Ok())
            continue;
        Cube<Bool> fl = s.flagCube();
        for (Int irow=0; irow!=s.nRows(); irow++) {
            if (s.flagRow()(irow))
                continue;
            Int a1(s.antenna1()(irow));
            Int a2(s.antenna2()(irow));
            // Not using irow
            Matrix<Bool> flr = fl.xyPlane(irow);
            if (!allTrue(flr)) {
                activeAntennas.insert(a1);
                activeAntennas.insert(a2);
            }
        }
    }
    if (prtlev > 2) {
        cout << "[FringeJones.cc::findRefAntWithData] refantlist " << refAntList << endl;
        cout << "[FringeJones.cc::findRefAntWithData] activeAntennas: ";
        std::copy(
            activeAntennas.begin(),
            activeAntennas.end(),
            std::ostream_iterator<Int>(std::cout, " ")
            );
        cout << endl;
    }
    Int refAnt = -1;
    for (Vector<Int>::ConstIteratorSTL a = refAntList.begin(); a != refAntList.end(); a++) {
        if (activeAntennas.find(*a) != activeAntennas.end()) {
            if (prtlev > 2)
                cout << "[FringeJones.cc::findRefAntWithData] We are choosing refant " << *a << endl;
            refAnt = *a;
            break;
        } else {
            if (prtlev > 2)
                cout << "[FringeJones.cc::findRefAntWithData] No data for refant " << *a << endl;
        }
    }
    return refAnt;
}


// Stolen from SolveDataBuffer
void
aggregateTimeCentroid(SDBList& sdbs, Int refAnt, std::map<Int, Double>& aggregateTime) {
    // Weighted average of SDBs' timeCentroids
    std::map<Int, Double> aggregateWeight;
    for (Int i=0; i < sdbs.nSDB(); ++i) {
        SolveDataBuffer& s = sdbs(i);
        Vector<Double> wtvD;
        // Sum over correlations and channels to get a vector of weights for each row
        Vector<Float> wtv(partialSums(s.weightSpectrum(), IPosition(2, 0, 1)));
        wtvD.resize(wtv.nelements());
        convertArray(wtvD, wtv);
        for (Int j=0; j<s.nRows(); j++) {
            Int a1 = s.antenna1()(j);
            Int a2 = s.antenna2()(j);
            Int ant;
            if (a1 == refAnt) { ant = a2; }
            else if (a2 == refAnt) { ant = a1; }
            else continue;
            Double w = wtv(j);
            aggregateTime[ant] += w*s.timeCentroid()(j);
            aggregateWeight[ant] += w;
        }
    }
    for (auto it=aggregateTime.begin(); it!=aggregateTime.end(); ++it) {
        Int a = it->first;
        it->second /= aggregateWeight[a];
    }

}


void
print_gsl_vector(gsl_vector *v)
{
    const size_t n = v->size;
    for (size_t i=0; i!=n; i++) {
        cerr << gsl_vector_get(v, i) << " ";
        if (i>0 && (i % 4)==3) cerr << endl;
    }
    cerr << endl;
}

void
print_max_gsl3(gsl_vector *v)
{
    double phi_max = 0.0;
    double del_max = 0.0;
    double rat_max = 0.0;
        
    const size_t n = v->size;
    for (size_t i=0; i!=n/3; i++) {
        if (fabs(gsl_vector_get(v, 3*i+0)) > fabs(phi_max)) phi_max = gsl_vector_get(v, 3*i+0);
        if (fabs(gsl_vector_get(v, 3*i+1)) > fabs(del_max)) del_max = gsl_vector_get(v, 3*i+1);
        if (fabs(gsl_vector_get(v, 3*i+2)) > fabs(rat_max)) rat_max = gsl_vector_get(v, 3*i+2);
    }
    cerr << "phi_max " << phi_max << " del_max " << del_max << " rat_max " << rat_max << endl;
}



/*
gsl-2.4/multilarge_nlinear/fdf.c defines gsl_multilarge_nlinear_driver,
which I have butchered for my purposes here into
not_gsl_multilarge_nlinear_driver(). We still iterate the nonlinear
least squares solver until completion, but we adopt a convergence
criterion copied from AIPS.

Inputs: maxiter  - maximum iterations to allow
        w        - workspace

Additionally I've removed the info parameter, and I may yet regret it.

Originally:
        info     - (output) info flag on why iteration terminated
                   1 = stopped due to small step size ||dx|
                   2 = stopped due to small gradient
                   3 = stopped due to small change in f
                   GSL_ETOLX = ||dx|| has converged to within machine
                               precision (and xtol is too small)
                   GSL_ETOLG = ||g||_inf is smaller than machine
                               precision (gtol is too small)
                   GSL_ETOLF = change in ||f|| is smaller than machine
                               precision (ftol is too small)

Return:
GSL_SUCCESS if converged
GSL_MAXITER if maxiter exceeded without converging
GSL_ENOPROG if no accepted step found on first iteration
*/

int
least_squares_inner_driver (const size_t maxiter,
                            gsl_multilarge_nlinear_workspace * w,
                            AuxParamBundle *bundle)
{
  int status;
  size_t iter = 0;
  /* call user callback function prior to any iterations
   * with initial system state */
  Double s;
  Double last_s = 1.0e30;
  Bool converged = false;
  do  {
      status = gsl_multilarge_nlinear_iterate (w);
      /*
       * If the solver reports no progress on the first iteration,
       * then it didn't find a single step to reduce the
       * cost function and more iterations won't help so return.
       *
       * If we get a no progress flag on subsequent iterations,
       * it means we did find a good step in a previous iteration,
       * so continue iterating since the solver has now reset
       * mu to its initial value.
       */
      if (status == GSL_ENOPROG && iter == 0) {
          return GSL_EMAXITER;
      }

      Double fnorm = gsl_blas_dnrm2(w->f);      
      s = 0.5 * fnorm * fnorm;
      if (DEVDEBUG) {
          cerr << "Iter: " << iter << " ";
          cerr << "Parameters: " << endl;
          print_gsl_vector(w->x);
          print_max_gsl3(w->dx);
          cerr << "1 - s/last_s=" << 1 - s/last_s << endl;
      }
      ++iter;
      if ((1 - s/last_s < 5e-6) && (iter > 1)) converged = true;
      last_s = s;
      /* old test for convergence:
         status = not_gsl_multilarge_nlinear_test(xtol, gtol, ftol, info, w); */
  } while (!converged && iter < maxiter);
  /*
   * the following error codes mean that the solution has converged
   * to within machine precision, so record the error code in info
   * and return success
   */
  if (status == GSL_ETOLF || status == GSL_ETOLX || status == GSL_ETOLG)
  {
      status = GSL_SUCCESS;
  }
  /* check if max iterations reached */
  if (iter >= maxiter && status != GSL_SUCCESS)
      status = GSL_EMAXITER;  return status;
} /* gsl_multilarge_nlinear_driver() */




void
least_squares_driver(SDBList& sdbs, Matrix<Float>& casa_param, Matrix<Bool>& casa_flags, Matrix<Float>& casa_snr, Int refant,
                     const std::map< Int, std::set<Int> >& activeAntennas, Int maxits, Vector<Bool> paramActive, LogIO& logSink) {
    // The variable casa_param is the Casa calibration framework's RParam matrix; we transcribe our results into it only at the end.
    // n below is number of variables,
    // p is number of parameters

    // We could pass in an AuxParamBundle instead I guess?
    AuxParamBundle bundle(sdbs, refant, activeAntennas, paramActive);
    for (size_t icor=0; icor != bundle.get_num_corrs(); icor++) {
        bundle.set_active_corr(icor);
        if (bundle.get_num_antennas() == 0) {
            logSink << "No antennas for correlation " << icor << "; not running least-squares solver." << LogIO::POST;
            continue;
        }
        if (bundle.get_num_antennas() == 1) {
            logSink << "No baselines for correlation " << icor << "; not running least-squares solver." << LogIO::POST;
            continue;
        }
        // Four parameters for every antenna, with dispersion
        size_t p = bundle.nParameters() * (bundle.get_num_antennas() - 1);
        // We need to store complex visibilities in a real matrix so we
        // just store real and imaginary components separately.
        size_t n = 2 * bundle.get_actual_num_data_points();

        if (DEVDEBUG) {
            cerr << "bundle.nParameters() " << bundle.nParameters()
                 << " bundle.get_num_antennas() " <<bundle.get_num_antennas() << endl;
            cerr << "p " << p << " n " << n << endl;
        }
        // Parameters for the least-squares solver.
        // param_tol sets roughly the number of decimal places accuracy you want in the answer;
        // I feel that 3 is probably plenty for fringe fitting.
        // param_tol is not used
        //const double param_tol = 1.0e-3;
        // gtol is not used
        // const double gtol = pow(GSL_DBL_EPSILON, 1.0/3.0);
        // ftol is not used
        // const double ftol = 1.0e-20;   
        const size_t max_iter = maxits ;

        const gsl_multilarge_nlinear_type *T = gsl_multilarge_nlinear_trust;
        gsl_multilarge_nlinear_parameters params = gsl_multilarge_nlinear_default_parameters();
        // the Moré scaling is the best equipped to handle very different scales; it should be the best choice to accommodate dispersive terms of O(f)
        params.scale = gsl_multilarge_nlinear_scale_more;
        params.trs = gsl_multilarge_nlinear_trs_lm;
        params.solver = gsl_multilarge_nlinear_solver_cholesky;
        gsl_multilarge_nlinear_workspace *w = gsl_multilarge_nlinear_alloc(T, &params, n, p);
        gsl_multilarge_nlinear_fdf f;

        f.f = &expb_f;
        /* Can't set to NULL for finite-difference Jacobian in multilarge case. */
        f.df =  &expb_df;   
        f.n = n;    /* number of data points */
        f.p = p;    /* number of parameters */
        f.params = &bundle;

        
        // Our original param is a matrix of (3*nCorr, nElem).
        // We have to transcribe it to a vector.

        gsl_vector *gp = gsl_vector_alloc(p);
        gsl_vector_set_zero(gp);

        // We transcribe Casa parameters into gsl vector format, as required by the solver.
        for (size_t iant=0; iant != bundle.get_max_antenna_index()+1; iant++) {
            if (!bundle.isActive(iant)) {
                // logSink << "Skipping antenna " << iant << " for correlation " << icor << "." << LogIO::POST;
                continue;
            }
            for (int di=0; di<4; di++) {
                Int param_ind = bundle.get_param_corr_param_index(iant, di);
                if (param_ind < 0) continue;
                gsl_vector_set(gp, param_ind, casa_param(4*icor + di, iant));
            }
        }
        gsl_vector *gp_orig = gsl_vector_alloc(p);
        // Keep a copy of original parameters
        gsl_vector_memcpy (gp_orig, gp);
        // initialise workspace
        gsl_multilarge_nlinear_init(gp, &f, w);
    
        // compute initial residual norm */
        gsl_vector *res_f = gsl_multilarge_nlinear_residual(w);

        int info;
        int status = least_squares_inner_driver(max_iter, w, &bundle);
        double chi1 = gsl_blas_dnrm2(res_f);
        
        gsl_vector_sub(gp_orig, w->x);
        // diff is not used
        //gsl_vector *diff = gp_orig;
        // diffsize is not used
        //double diffsize = gsl_blas_dnrm2(diff);
    
        gsl_vector *res = gsl_multilarge_nlinear_position(w);
        
        // We transcribe values back from gsl_vector to the param matrix
        
        gsl_matrix *hess = gsl_matrix_alloc(p,p);
        gsl_vector *snr_vector = gsl_vector_alloc(p);
        gsl_matrix_set_zero(hess);
        gsl_vector_set_zero(snr_vector);
        expb_hess(gp, &bundle, hess, chi1*chi1, snr_vector, logSink);
        
        // Transcribe parameters back into CASA arrays
        for (size_t iant=0; iant != bundle.get_max_antenna_index()+1; iant++) {
            if (!bundle.isActive(iant)) continue;
            Int iparam = bundle.get_param_corr_param_index(iant, 0);
            if (iparam<0) continue;
            if (1) {
                // flag unused
                // bool flag = false;
                // if (fabs(gsl_vector_get(diff, iparam + 0) > FLT_EPSILON)) {
                //     flag = true;
                // }
                // if (fabs(gsl_vector_get(diff, iparam + 1) > FLT_EPSILON)) {
                //     flag = true;
                // }
                // if (fabs(gsl_vector_get(diff, iparam + 2) > 1e-30)) {
                //     flag = true;
                // }
                
                if (DEVDEBUG) {
                    int i;
                    logSink << "Old values for ant " << iant << " correlation " << icor 
                            << ": Angle " << casa_param(4*icor + 0, iant)
                            << " delay " << casa_param(4*icor + 1, iant) << " ns "
                            << " rate " << casa_param(4*icor + 2, iant) << "."
                            << endl;
                    logSink << "New values for ant " << iant << " correlation " << icor << ":";
                    if ((i=bundle.get_param_corr_param_index(iant, 0))>=0) {
                        logSink << " Angle " << gsl_vector_get(res, i);
                    }
                    if ((i=bundle.get_param_corr_param_index(iant, 1))>=0) {
                        logSink << " delay " << gsl_vector_get(res, i) << " ns ";
                    }
                    if ((i=bundle.get_param_corr_param_index(iant, 2))>=0) {
                        logSink << " rate " << gsl_vector_get(res, i);
                    }
                    logSink << "." << LogIO::POST;
                }
            }
            if (status==GSL_SUCCESS || status==GSL_EMAXITER) {
                // Current policy is to assume that exceeding max
                // number of iterations is not a deal-breaker, leave it
                // to SNR calculation to decide if the results are
                // useful.
                for (size_t di=0; di<4; di++) {
                    int i=bundle.get_param_corr_param_index(iant, di);
                    int i0 = bundle.get_param_corr_param_index(iant, 0);
                    if (i>=0) {
                        casa_param(4*icor + di, iant) = gsl_vector_get(res, i);
                        casa_snr(4*icor + di, iant) = gsl_vector_get(snr_vector, i0);
                    } else {
                        casa_param(4*icor + di, iant) = 0.0;
                        casa_snr(4*icor + di, iant) = 0.0;
                    }
                }
            } else { // gsl solver failed; flag data
                logSink << "Least-squares solver failed to converge; flagging" << endl;
                for (size_t di=0; di<4; di++) {
                    casa_flags(4*icor + di, iant) = false;
                }
            }
        }

        logSink <<  "Least squares complete for correlation " << icor
                << " after " <<  gsl_multilarge_nlinear_niter(w) << " iterations." << LogIO::POST;

            // << "reason for stopping: " << ((info == 1) ? "small step size" : "small gradient") << endl
            // << "initial |f(x)| = " << chi0 << endl
            // << "final   |f(x)| = " << chi1 << endl
            // << "final step taken = " << diffsize 

        if (DEVDEBUG) {
            cerr << "casa_param " << casa_param << endl;

            switch (info) {
            case 1:
                logSink << "Small step size." << endl;
                break;
            case 2:
                logSink << "Flatness." << endl;
            }
            logSink << LogIO::POST;
        }
        gsl_vector_free(gp);
        gsl_vector_free(gp_orig);
        gsl_vector_free(snr_vector);
        gsl_matrix_free(hess);
        gsl_multilarge_nlinear_free(w);
    }
}

    


// **********************************************************
//  CTRateAwareTimeInterp1 Implementations
//

CTRateAwareTimeInterp1::CTRateAwareTimeInterp1(NewCalTable& ct,
					       const casacore::String& timetype,
					       casacore::Array<Float>& result,
					       casacore::Array<Bool>& rflag) :
  CTTimeInterp1(ct,timetype,result,rflag)
{}

// Destructor (nothing to do locally)
CTRateAwareTimeInterp1::~CTRateAwareTimeInterp1() {}

Bool CTRateAwareTimeInterp1::interpolate(Double newtime) {
  // Call generic first
  if (CTTimeInterp1::interpolate(newtime)) {
    // Only if generic yields new calibration
    // NB: lastWasExact_=exact in generic
    applyPhaseRate(timeType().contains("nearest") || lastWasExact_);
    return true;
  }
  else
    // No change
    return false;

}

// Do the phase rate math
void CTRateAwareTimeInterp1::applyPhaseRate(Bool single)
{

  Int ispw=mcols_p->spwId()(0);  // should only be one (sliced ct_)!
  MSSpectralWindow msSpw(ct_.spectralWindow());
  MSSpWindowColumns msCol(msSpw);
  //Vector<Double> refFreqs;
  //msCol.refFrequency().getColumn(refFreqs,True);

  Vector<Double> freqs;
  msCol.chanFreq().get(ispw,freqs,True);  // should only be 1
  Double centroidFreq=freqs(0);

  // cout << "time = " << (currTime_ - timeRef_) << endl;

  if (single) {
    for (Int ipol=0;ipol<2;ipol++) {
      Double dtime=(currTime_-timeRef_)-timelist_(currIdx_);
      Double phase=result_(IPosition(2,ipol*4,0));
      Double rate=result_(IPosition(2,ipol*4+2,0));
      phase+=2.0*C::pi*rate*centroidFreq*dtime;
      result_(IPosition(2,ipol*4,0))=phase;
    }
  } else {
    Vector<uInt> rows(2); indgen(rows); rows+=uInt(currIdx_);
    Cube<Float> r(mcols_p->fparamArray("",rows));

    Vector<Double> dtime(2);
    dtime(0)=(currTime_-timeRef_)-timelist_(currIdx_);
    dtime(1)=(currTime_-timeRef_)-timelist_(currIdx_+1);
    Double wt=dtime(1) / (dtime(1)-dtime(0));


    for (Int ipol=0;ipol<2;ipol++) {
      Vector<Double> phase(2), rate(2);
      phase(0)=r.xyPlane(0)(IPosition(2,ipol*4,0));
      phase(1)=r.xyPlane(1)(IPosition(2,ipol*4,0));
      rate(0)=r.xyPlane(0)(IPosition(2,ipol*4+2,0));
      rate(1)=r.xyPlane(1)(IPosition(2,ipol*4+2,0));

      phase(0)+=2.0*C::pi*rate(0)*centroidFreq*dtime(0);
      phase(1)+=2.0*C::pi*rate(1)*centroidFreq*dtime(1);

      Vector<Complex> ph(2);
      ph(0)=Complex(cos(phase(0)),sin(phase(0)));
      ph(1)=Complex(cos(phase(1)),sin(phase(1)));
      ph(0)=Float(wt)*ph(0) + Float(1.0-wt)*ph(1);
      result_(IPosition(2,ipol*4,0))=arg(ph(0));
    }
  }
}




// **********************************************************
//  FringeJones Implementations
//
FringeJones::FringeJones(VisSet& vs) :
    VisCal(vs),             // virtual base
    VisMueller(vs),         // virtual base
    GJones(vs)       // immediate parent
{
    if (prtlev()>2) cout << "FringeJones::FringeJones(vs)" << endl;
}

FringeJones::FringeJones(String msname,Int MSnAnt,Int MSnSpw) :
    VisCal(msname,MSnAnt,MSnSpw),             // virtual base
    VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
    GJones(msname,MSnAnt,MSnSpw)    // immediate parent
{
    if (prtlev()>2) cout << "FringeJones::FringeJones(msname,MSnAnt,MSnSpw)" << endl;
}

FringeJones::FringeJones(const MSMetaInfoForCal& msmc) :
    VisCal(msmc),             // virtual base
    VisMueller(msmc),         // virtual base
    GJones(msmc)    // immediate parent
{
    if (prtlev()>2) cout << "FringeJones::FringeJones(msmc)" << endl;
}

FringeJones::FringeJones(Int nAnt) :
    VisCal(nAnt), 
    VisMueller(nAnt),
    GJones(nAnt)
{
    if (prtlev()>2) cout << "FringeJones::FringeJones(nAnt)" << endl;
}

FringeJones::~FringeJones() {
    if (prtlev()>2) cout << "FringeJones::~FringeJones()" << endl;
}

void FringeJones::setApply(const Record& apply) {
    // Call parent to do conventional things
    GJones::setApply(apply);

    if (calWt()) 
        logSink() << " (" << this->typeName() << ": Enforcing calWt()=false for phase/delay-like terms)" << LogIO::POST;

   // Enforce calWt() = false for delays
    calWt()=false;

    /*
    // Extract per-spw ref Freq for phase(delay) calculation
    //  from the CalTable
    // TBD:  revise as per refFreq decisions
    MSSpectralWindow msSpw(ct_->spectralWindow());
    MSSpWindowColumns msCol(msSpw);
    msCol.refFrequency().getColumn(KrefFreqs_,true);
    KrefFreqs_/=1.0e9;  // in GHz

    /// Re-assign KrefFreq_ according spwmap (if any)
    if (spwMap().nelements()>0) {
        Vector<Double> tmpfreqs;
        tmpfreqs.assign(KrefFreqs_);
        for (uInt ispw=0;ispw<spwMap().nelements();++ispw)
            if (spwMap()(ispw)>-1)
                KrefFreqs_(ispw)=tmpfreqs(spwMap()(ispw));
    }
    */

    // Use the "physical" (centroid) frequency, per spw 
    MSSpectralWindow msSpw(ct_->spectralWindow());
    MSSpWindowColumns msCol(msSpw);
    Vector<Double> tmpfreqs;
    Vector<Double> chanfreq;
    tmpfreqs.resize(msSpw.nrow());
    for (rownr_t ispw=0;ispw<msSpw.nrow();++ispw) {
      if (ispw < msSpw.nrow()) {
	msCol.chanFreq().get(ispw,chanfreq,true);  // reshape, if nec.
	Int nch=chanfreq.nelements();
	tmpfreqs(ispw)=chanfreq(nch/2);
      }
    }

    KrefFreqs_.resize(nSpw()); KrefFreqs_.set(0.0);
    for (rownr_t ispw=0;ispw<(rownr_t)nSpw();++ispw) {
        if (ispw < tmpfreqs.nelements())
            KrefFreqs_(ispw)=tmpfreqs(ispw);
    }

    /// Re-assign KrefFreq_ according spwmap (if any)
    if (spwMap().nelements()>0) {
      for (uInt ispw=0;ispw<spwMap().nelements();++ispw)
	if (spwMap()(ispw)>-1 && ispw < tmpfreqs.nelements())
	  KrefFreqs_(ispw)=tmpfreqs(spwMap()(ispw));
    }

    KrefFreqs_/=1.0e9;  // in GHz
}

void FringeJones::setApply() {
  // This was omitted in copying KJones. It shouldn't have been.
    
  // Call parent to do conventional things
  GJones::setApply();

  // Enforce calWt() = false for delays
  calWt()=false;

  // Set the ref freqs to something usable
  KrefFreqs_.resize(nSpw());
  KrefFreqs_.set(5.0);

}


void FringeJones::setCallib(const Record& callib,
                            const MeasurementSet& selms) {

    // Call parent to do conventional things
    SolvableVisCal::setCallib(callib,selms);

    /*
    if (calWt()) 
        logSink() << " (" << this->typeName() << ": Enforcing calWt()=false for phase/delay-like terms)" << LogIO::POST;
    */
    // Enforce calWt() = false for delays
    calWt()=false;

    /*
    // Extract per-spw ref Freq for phase(delay) calculation
    //  from the CalTable 
   KrefFreqs_.assign(cpp_->refFreqIn());
    KrefFreqs_/=1.0e9;  // in GHz

    // Re-assign KrefFreq_ according spwmap (if any)
    if (spwMap().nelements()>0) {
        Vector<Double> tmpfreqs;
        tmpfreqs.assign(KrefFreqs_);
        for (uInt ispw=0;ispw<spwMap().nelements();++ispw)
            if (spwMap()(ispw)>-1)
                KrefFreqs_(ispw)=tmpfreqs(spwMap()(ispw));
    }
    */

    // Use the "physical" (centroid) frequency, per spw 
    KrefFreqs_.resize(nSpw());
    for (Int ispw=0;ispw<nSpw();++ispw) {
      const Vector<Double>& f(cpp_->freqIn(ispw));
      Int nf=f.nelements();
      KrefFreqs_[ispw]=f[nf/2];  // center (usually this will be same as [0])
    }
    KrefFreqs_/=1.0e9;  // In GHz

    // Re-assign KrefFreq_ according spwmap (if any)
    if (spwMap().nelements()>0) {
      Vector<Double> tmpfreqs;
      tmpfreqs.assign(KrefFreqs_);
      for (uInt ispw=0;ispw<spwMap().nelements();++ispw)
	if (spwMap()(ispw)>-1)
	  KrefFreqs_(ispw)=tmpfreqs(spwMap()(ispw));
    }
    
}

void FringeJones::setSolve(const Record& solve) {

    // Call parent to do conventional things
    GJones::setSolve(solve);
    preavg() = -DBL_MAX;

    // refant isn't properly set until selfSolveOne.  We set it to a
    // known value here so that it can be checked in debugging code.
    refant() = -1;
    if (prtlev() > 2) {
        cout << "Before GJones::setSolve" << endl
             << "FringeJones::setSolve()" <<endl
             << "FringeJones::refant() = "<< refant() <<endl
             << "FringeJones::refantlist() = "<< refantlist() <<endl;
    }
    if (solve.isDefined("zerorates")) {
        zeroRates() = solve.asBool("zerorates");
    }
    if (solve.isDefined("globalsolve")) {
        globalSolve() = solve.asBool("globalsolve");
    }
    if (solve.isDefined("delaywindow")) {
        Array<Double> dw = solve.asArrayDouble("delaywindow");
        delayWindow() = dw;
    } else {
        cerr << "No delay window!" << endl;
    }
    if (solve.isDefined("ratewindow")) {
        rateWindow() = solve.asArrayDouble("ratewindow");
    } else {
        cerr << "No rate window!" << endl;
    }
    if (solve.isDefined("niter")) {
        maxits() = solve.asInt("niter");
    }
    if (solve.isDefined("paramactive")) {
        paramActive() = solve.asArrayBool("paramactive");
    }
    
}

// Note: this was previously omitted
void FringeJones::specify(const Record& specify) {

  return SolvableVisCal::specify(specify);

}

void FringeJones::calcAllJones() {

  if (prtlev()>6) cout << "       FringeJones::calcAllJones()" << endl;

  // Should handle OK flags in this method, and only
  //  do Jones calc if OK

  Vector<Complex> oneJones;
  Vector<Bool> oneJOK;
  Vector<Float> onePar;
  Vector<Bool> onePOK;

  ArrayIterator<Complex> Jiter(currJElem(),1);
  ArrayIterator<Bool>    JOKiter(currJElemOK(),1);
  ArrayIterator<Float>   Piter(currRPar(),1);
  ArrayIterator<Bool>    POKiter(currParOK(),1);

  Double phase;

  for (Int iant=0; iant<nAnt(); iant++) {
    onePar.reference(Piter.array());
    onePOK.reference(POKiter.array());

    Float fmin = min(currFreq());
    Float fmax = max(currFreq());
    for (Int ich=0; ich<nChanMat(); ich++) {
      
      oneJones.reference(Jiter.array());
      oneJOK.reference(JOKiter.array());

      for (Int ipar=0;ipar<nPar();ipar+=4) {
	if (onePOK(ipar)) {
          Float freq = currFreq()(ich);
          Float k_disp = 1e-9*KDISPSCALE*C::_2pi*(1.0/freq + (freq-fmin-fmax)/(fmin*fmax));
                          
	  phase=onePar(ipar);
	  phase+=2.0*C::pi*onePar(ipar+1)*
	    (currFreq()(ich)-KrefFreqs_(currSpw()));
	  phase+=2.0*C::pi*onePar(ipar+2)*KrefFreqs_(currSpw())*1e9*
	    (currTime() - refTime());
          Float dph_d = onePar(ipar+3) * k_disp;
          if (DEVDEBUG && 0) {
              cerr << "fmin " << fmin << " fmax " << fmax << " k_disp " << k_disp
                   << " param " << onePar(ipar+3) << " dph_d " << dph_d << endl;
          }
          phase+=dph_d;
	  oneJones(ipar/4)=Complex(cos(phase),sin(phase));
	  oneJOK(ipar/4)=True;
	} else {
	  oneJOK(ipar/4)=False;
	}
      }
      // Advance iterators
      Jiter.next();
      JOKiter.next();
    }
    // Step to next antenns's pars
    Piter.next();
    POKiter.next();
  }
}


void
FringeJones::calculateSNR(Int nCorr, DelayRateFFT drf) {
    Matrix<Float> sRP(solveRPar().nonDegenerate(1));
    Matrix<Bool> sPok(solveParOK().nonDegenerate(1));
    Matrix<Float> sSNR(solveParSNR().nonDegenerate(1));
    
    for (size_t icor=0; icor != (size_t) nCorr; icor++) {
        const set<Int>& activeAntennas = drf.getActiveAntennasCorrelation(icor);
        for (Int iant=0; iant != nAnt(); iant++) {
            if (iant == refant()) {
                Double maxsnr = 999.0;
                sSNR(4*icor + 0, iant) = maxsnr;
                sSNR(4*icor + 1, iant) = maxsnr;
                sSNR(4*icor + 2, iant) = maxsnr;
                sSNR(4*icor + 3, iant) = maxsnr;
            }
            else if (activeAntennas.find(iant) != activeAntennas.end()) {
                Double delay = sRP(4*icor + 1, iant);
                Double rate = sRP(4*icor + 2, iant);
                // Note that DelayRateFFT::snr is also used to calculate SNRs for the least square values!
                Float snrval = drf.snr(icor, iant, delay, rate);
                sSNR(4*icor + 0, iant) = snrval;
                sSNR(4*icor + 1, iant) = snrval;
                sSNR(4*icor + 2, iant) = snrval;
                sSNR(4*icor + 3, iant) = snrval;
            } else {
                sPok(4*icor + 0, iant) = false;
                sPok(4*icor + 1, iant) = false;
                sPok(4*icor + 2, iant) = false;
                sPok(4*icor + 3, iant) = false;
            }
        }
    }
}



// void FringeJones::solveLotsOfSDBs(SDBList& sdbs)

void
FringeJones::selfSolveOne(SDBList& sdbs) {
    solveRPar()=0.0;
    solveParOK()=false; 
    solveParErr()=1.0; // Does nothing?
    // Maybe we put refFreq, refTime stuff in here?
    Vector<Double> myRefFreqs;
    // Cannot assume we have a calibration table (ct_) in this method.
    // MSSpectralWindow msSpw(ct_->spectralWindow());
    /// MSSpWindowColumns spwCol(msSpw);
    // spwCol.refFrequency().getColumn(myRefFreqs, true);
    //Double ref_freq = myRefFreqs(currSpw());
    //Double ref_freq = sdbs.freqs()(0);
    Double centroidFreq = sdbs.centroidFreq();
    Double t0 = sdbs(0).time()(0);
    Double dt0 = refTime() - t0;
    //Double df0 = ref_freq - sdbs.freqs()(0);
    //Double df0 = 0; 
    Double df0 = centroidFreq - sdbs(0).freqs()(0);  // global center to global edge

    logSink() << "Solving for fringes for spw=" << currSpw() << " at t="
              << MVTime(refTime()/C::day).string(MVTime::YMD,7)  << LogIO::POST;

    std::map<Int, Double> aggregateTime;
    // Set the refant to the first choice that has data!
    refant() = findRefAntWithData(sdbs, refantlist(), prtlev());
    if (refant()<0)
        throw(AipsError("No valid reference antenna supplied."));
    else
        logSink() << "Using reference antenna " << refant() << LogIO::POST;

    aggregateTimeCentroid(sdbs, refant(), aggregateTime);

    if (DEVDEBUG) {
        std::cerr << "Weighted time centroids" << endl; 
        for (auto it=aggregateTime.begin(); it!=aggregateTime.end(); ++it)
            std::cerr << it->first << " => " << it->second - t0 << std::endl;
    }

    DelayRateFFT drf(sdbs, refant(), delayWindow(), rateWindow());
    drf.FFT();
    drf.searchPeak();
    Matrix<Float> sRP(solveRPar().nonDegenerate(1));
    Matrix<Bool> sPok(solveParOK().nonDegenerate(1));
    Matrix<Float> sSNR(solveParSNR().nonDegenerate(1));
    logSink() << "sPok " << sPok.shape() << LogIO::POST;
    
    // Map from MS antenna number to index
    // transcribe fft results to sRP
    Int ncol = drf.param().ncolumn();
    Int nrow = drf.param().nrow();
    if (DEVDEBUG) {
        std::cerr << "nrow " << nrow << ", ncol " << ncol << endl; 
        std::cerr << "drf.flag() " << drf.flag() << endl; 
    }

    for (Int i=0; i!=ncol; i++) { // i==iant
        for (Int j=0; j!=nrow; j++) { // j is parameter number
            Int oj = (j>=3) ? j+1 : j;
            sRP(IPosition(2, oj, i)) = drf.param()(IPosition(2, j, i));
            sPok(IPosition(2, oj, i)) = !(drf.flag()(IPosition(2, j, i)));
        }
        // Our estimate for dispersion is zero, unconditionally, and we stand by it.
        sPok(IPosition(2, 3, i)) = true;
        if (nrow > 3)
            sPok(IPosition(2, 7, i)) = true;
    }
    size_t nCorrOrig(sdbs(0).nCorrelations());
    size_t nCorr = (nCorrOrig> 1 ? 2 : 1); // number of p-hands

    calculateSNR(nCorr, drf);

    set<Int> belowThreshold;

    Float threshold = minSNR();
    
    for (size_t icor=0; icor != nCorr; icor++) {
        const set<Int>& activeAntennas = drf.getActiveAntennasCorrelation(icor);
        for (Int iant=0; iant != nAnt(); iant++) {
            if (iant != refant() && (activeAntennas.find(iant) != activeAntennas.end())) {
                Float s = sSNR(4*icor + 0, iant);
		// Start the log message; finished below
		logSink() << "Antenna " << iant << " correlation " << icor << " has (FFT) SNR of " << s;
                if (s < threshold) {
                    belowThreshold.insert(iant);
                    logSink() << " below threshold (" << threshold << ")";
                    // Don't assume these will be flagged later; do it right away.
                    // (The least squares routine will eventually become optional.)
                    sPok(4*icor + 0, iant) = false;
                    sPok(4*icor + 1, iant) = false;
                    sPok(4*icor + 2, iant) = false;
                    sPok(4*icor + 3, iant) = false;
                }
		// Finish the log message
		logSink() << "." << LogIO::POST;
            }
        }
        // We currently remove the antennas below SNR threshold from
        // the object used to handle the FFT fringe search.
        drf.removeAntennasCorrelation(icor, belowThreshold);
        if (DEVDEBUG) {
            drf.printActive();
        }
    }
    if (globalSolve()) {
        logSink() << "Starting least squares optimization." << LogIO::POST;
        // Note that least_squares_driver is *not* a method of
        // FringeJones so we pass everything in, including the logSink
        // reference.  Note also that sRP is passed by reference and
        // altered in place.
        least_squares_driver(sdbs, sRP, sPok, sSNR, refant(), drf.getActiveAntennas(), maxits(),
                             paramActive(), logSink());
    }
    else {
        logSink() << "Skipping least squares optimisation." << LogIO::POST;
    }

    if (DEVDEBUG) {
        cerr << "Ref time " << MVTime(refTime()/C::day).string(MVTime::YMD,7) << endl;
        cerr << "df0 " << df0 << " dt0 " << dt0 << " centroidFreq*dt0 " << centroidFreq*dt0 << endl;
        cerr << "centroidFreq " << centroidFreq << endl;
        cerr << "df0 " << df0 << " dt0 " << dt0 << " centroidFreq*dt0 " << centroidFreq*dt0 << endl;
        cerr << "sRP " << sRP << endl; 
    }

    for (Int iant=0; iant != nAnt(); iant++) {
        for (size_t icor=0; icor != nCorr; icor++) {
            Double phi0 = sRP(4*icor + 0, iant);
            Double delay = sRP(4*icor + 1, iant);
            Double rate = sRP(4*icor + 2, iant);
            Double k_disp = sRP(4*icor + 3, iant);
            aggregateTime.find(iant);
            // We assume the reference frequency for fringe fitting
            // (which is NOT the one stored in the SPECTRAL_WINDOW
            // table) is the left-hand edge of the frequency grid.
            Double delta1 = df0*delay/1e9;
            Double delta2 = centroidFreq*dt0*rate;
            Double delta3 = C::_2pi*(delta1+delta2);
            Double dt;
            auto p = aggregateTime.find(iant);
            if (zeroRates() && p != aggregateTime.end()) {
                dt = p->second - t0;
            } else {
                dt = refTime() - t0;
            }
            if (DEVDEBUG) {
                cerr << "Antenna " << iant << ": phi0 " << phi0 << " delay " << delay << " rate " << rate << " dt " << dt << endl
                     << "k_disp " << k_disp << endl
                     << "centroidFreq "<< centroidFreq << " Adding corrections for frequency (" << 360*delta1 << ")" 
                     << " and time (" << 360*delta2 << ") degrees." << endl;
            }
            sRP(4*icor + 0, iant) += delta3;
         }
    }
    
    // We can zero the rates here (if needed) whether we did least squares or not.
    if (zeroRates()) {
        logSink() << "Zeroing delay rates in calibration table." << LogIO::POST;
        
        for (size_t icor=0; icor != nCorr; icor++) {
            for (Int iant=0; iant != nAnt(); iant++) {
                sRP(4*icor + 2, iant) = 0.0;
            }
        }
    }
    if (DEVDEBUG) {
        std::cerr << "sPok " << sPok << endl;
    }
}

void
FringeJones::solveOneVB(const VisBuffer&) {
    throw(AipsError("VisBuffer interface not supported!"));
}


void FringeJones::globalPostSolveTinker() {

  // Re-reference the phase, if requested
  if (refantlist()(0)>-1) applyRefAnt();
}

void FringeJones::applyRefAnt() {

  // TBD:
  // 1. Synchronize refant changes on par axis
  // 2. Implement minimum mean deviation algorithm

  if (refantlist()(0)<0) 
    throw(AipsError("No refant specified."));

  Int nUserRefant=refantlist().nelements();

  // Get the preferred refant names from the MS
  String refantName(msmc().antennaName(refantlist()(0)));
  if (nUserRefant>1) {
    refantName+=" (";
    for (Int i=1;i<nUserRefant;++i) {
      refantName+=msmc().antennaName(refantlist()(i));
      if (i<nUserRefant-1) refantName+=",";
    }
    refantName+=")";
  }

  logSink() << "Applying refant: " << refantName
	    << " refantmode = " << refantmode();
  if (refantmode()=="flex")
    logSink() << " (hold alternate refants' phase constant) when refant flagged";
  if (refantmode()=="strict")
    logSink() << " (flag all antennas when refant flagged)";
  logSink() << LogIO::POST;

  // Generate a prioritized refant choice list
  //  The first entry in this list is the user's primary refant,
  //   the second entry is the refant used on the previous interval,
  //   and the rest is a prioritized list of alternate refants,
  //   starting with the user's secondary (if provided) refants,
  //   followed by the rest of the array, in distance order.   This
  //   makes the priorities correct all the time, and prevents
  //   a semi-stochastic alternation (by preferring the last-used
  //   alternate, even if nominally higher-priority refants become
  //   available)


  // Extract antenna positions
  Matrix<Double> xyz;
  if (msName()!="<noms>") {
    MeasurementSet ms(msName());
    MSAntennaColumns msant(ms.antenna());
    msant.position().getColumn(xyz);
  }
  else {
    // TBD RO*
    CTColumns ctcol(*ct_);
    CTAntennaColumns& antcol(ctcol.antenna());
    antcol.position().getColumn(xyz);
  }

  // Calculate (squared) antenna distances, relative
  //  to last preferred antenna
  Vector<Double> dist2(xyz.ncolumn(),0.0);
  for (Int i=0;i<3;++i) {
    Vector<Double> row=xyz.row(i);
    row-=row(refantlist()(nUserRefant-1));
    dist2+=square(row);
  }
  // Move preferred antennas to a large distance
  for (Int i=0;i<nUserRefant;++i)
    dist2(refantlist()(i))=DBL_MAX;

  // Generated sorted index
  Vector<uInt> ord;
  genSort(ord,dist2);

  // Assemble the whole choices list
  Int nchoices=nUserRefant+1+ord.nelements();
  Vector<Int> refantchoices(nchoices,0);
  Vector<Int> r(refantchoices(IPosition(1,nUserRefant+1),IPosition(1,refantchoices.nelements()-1)));
  convertArray(r,ord);

  // set first two to primary preferred refant
  refantchoices(0)=refantchoices(1)=refantlist()(0);

  // set user's secondary refants (if any)
  if (nUserRefant>1) 
    refantchoices(IPosition(1,2),IPosition(1,nUserRefant))=
      refantlist()(IPosition(1,1),IPosition(1,nUserRefant-1));

  //cout << "refantchoices = " << refantchoices << endl;


  if (refantmode()=="strict") {
    nchoices=1;
    refantchoices.resize(1,True);
  }

  Vector<Int> nPol(nSpw(),2);  // TBD:or 1, if data was single pol

  if (nPar()==8) {
    // Verify that 2nd poln has unflagged solutions, PER SPW
    ROCTMainColumns ctmc(*ct_);

    Block<String> cols(1);
    cols[0]="SPECTRAL_WINDOW_ID";
    CTIter ctiter(*ct_,cols);
    Cube<Bool> fl;

    while (!ctiter.pastEnd()) {

      Int ispw=ctiter.thisSpw();
      fl.assign(ctiter.flag());

      IPosition blc(3,0,0,0), trc(fl.shape());
      trc-=1; blc(0)=nPar()/2;
      
      //      cout << "ispw = " << ispw << " nfalse(fl(1,:,:)) = " << nfalse(fl(blc,trc)) << endl;
      
      // If there are no unflagged solutions in 2nd pol, 
      //   avoid it in refant calculations
      if (nfalse(fl(blc,trc))==0)
	nPol(ispw)=1;

      ctiter.next();      
    }
  }
  //  cout << "nPol = " << nPol << endl;

  Bool usedaltrefant(false);
  Int currrefant(refantchoices(0)), lastrefant(-1);

  Block<String> cols(2);
  cols[0]="SPECTRAL_WINDOW_ID";
  cols[1]="TIME";
  CTIter ctiter(*ct_,cols);

  // Arrays to hold per-timestamp solutions
  Cube<Float> solA, solB;
  Cube<Bool> flA, flB;
  Vector<Int> ant1A, ant1B, ant2B;
  Matrix<Complex> refPhsr;  // the reference phasor [npol,nchan] 
  Int lastspw(-1);
  Bool first(true);
  while (!ctiter.pastEnd()) {
    Int ispw=ctiter.thisSpw();
    if (ispw!=lastspw) first=true;  // spw changed, start over

    // Read in the current sol, fl, ant1:
    solB.assign(ctiter.fparam());
    flB.assign(ctiter.flag());
    ant1B.assign(ctiter.antenna1());
    ant2B.assign(ctiter.antenna2()); 

    // First time thru, 'previous' solution same as 'current'
    if (first) {
      solA.reference(solB);
      flA.reference(flB);
      ant1A.reference(ant1B);
    }
    IPosition shB(solB.shape());
    IPosition shA(solA.shape());

    // Find a good refant at this time
    //  A good refant is one that is unflagged in all polarizations
    //     in the current(B) and previous(A) intervals (so they can be connected)
    Int irefA(0),irefB(0);  // index on 3rd axis of solution arrays
    Int ichoice(0);  // index in refantchoicelist
    Bool found(false);
    IPosition blcA(3,0,0,0),trcA(shA),blcB(3,0,0,0),trcB(shB);
    trcA-=1; trcA(0)=trcA(2)=0;
    trcB-=1; trcB(0)=trcB(2)=0;
    ichoice=0;
    while (!found && ichoice<nchoices) { 
      // Find index of current refant choice
      irefA=irefB=0;
      while (ant1A(irefA)!=refantchoices(ichoice) && irefA<shA(2)) ++irefA;
      while (ant1B(irefB)!=refantchoices(ichoice) && irefB<shB(2)) ++irefB;

      if (irefA<shA(2) && irefB<shB(2)) {

	//	cout << " Trial irefA,irefB: " << irefA << "," << irefB 
	//	     << "   Ants=" << ant1A(irefA) << "," << ant1B(irefB) << endl;

	blcA(2)=trcA(2)=irefA;
	blcB(2)=trcB(2)=irefB;
	found=true;  // maybe
	for (Int ipol=0;ipol<nPol(ispw);++ipol) {
	  blcA(0)=blcB(0)=ipol*nPar()/2;
	  trcA(0)=trcB(0)=(ipol+1)*nPar()/2-1;
	  found &= (nfalse(flA(blcA,trcA))>0);  // previous interval
	  found &= (nfalse(flB(blcB,trcB))>0);  // current interval
	} 
      }
      else
	// irefA or irefB out-of-range
	found=false;  // Just to be sure

      if (!found) ++ichoice;  // try next choice next round

    }

    if (found) {
      // at this point, irefA/irefB point to a good refant
      
      // Keep track
      usedaltrefant|=(ichoice>0);
      currrefant=refantchoices(ichoice);
      refantchoices(1)=currrefant;  // 2nd priorty next time

      //      cout << " currrefant = " << currrefant << " (" << ichoice << ")" << endl;

      //      cout << " Final irefA,irefB: " << irefA << "," << irefB 
      //	   << "   Ants=" << ant1A(irefA) << "," << ant1B(irefB) << endl;


      // Only report if using an alternate refant
      if (currrefant!=lastrefant && ichoice>0) {
	logSink() 
	  << "At " 
	  << MVTime(ctiter.thisTime()/C::day).string(MVTime::YMD,7) 
	  << " ("
	  << "Spw=" << ctiter.thisSpw()
	  << ", Fld=" << ctiter.thisField()
	  << ")"
	  << ", using refant " << msmc().antennaName(currrefant)
	  << " (id=" << currrefant 
	  << ")" << " (alternate)"
	  << LogIO::POST;
      }  

      // Form reference phasor [nPar,nChan]
      Matrix<Float> rA,rB;
      Matrix<Bool> rflA,rflB;
      rB.assign(solB.xyPlane(irefB));
      rflB.assign(flB.xyPlane(irefB));
      
      if (!first) {
	// Get and condition previous phasor for the current refant
	rA.assign(solA.xyPlane(irefA));
	rflA.assign(flA.xyPlane(irefA));
	rB-=rA;

	// Accumulate flags
	rflB&=rflA;
      }
      
      //      cout << " rB = " << rB << endl;
      //      cout << boolalpha << " rflB = " << rflB << endl;
      // TBD: fillChanGaps?
      
      // Now apply reference phasor to all antennas
      Matrix<Float> thissol;
      for (Int iant=0;iant<shB(2);++iant) {
	thissol.reference(solB.xyPlane(iant));
	thissol-=rB;
      }
      
      // Set refant, so we can put it back
      ant2B=currrefant;
      
      // put back referenced solutions
      ctiter.setfparam(solB);
      ctiter.setantenna2(ant2B);

      // Next time thru, solB is previous
      solA.reference(solB);
      flA.reference(flB);
      ant1A.reference(ant1B);
      solB.resize();  // (break references)
      flB.resize();
      ant1B.resize();
	
      lastrefant=currrefant;
      first=false;  // avoid first-pass stuff from now on
      
    } // found
    else {
      logSink() 
	<< "At " 
	<< MVTime(ctiter.thisTime()/C::day).string(MVTime::YMD,7) 
	<< " ("
	<< "Spw=" << ctiter.thisSpw()
	<< ", Fld=" << ctiter.thisField()
	<< ")"
	<< ", refant (id=" << currrefant 
	<< ") was flagged; flagging all antennas strictly."
	<< LogIO::POST;
      // Flag all solutions in this interval
      flB.set(True);
      ctiter.setflag(flB);
      if (ichoice == nchoices){
          logSink() << LogIO::WARN
          << "From time: "
          << MVTime(ctiter.thisTime()/C::day).string(MVTime::YMD,7)
          << "in Spw: "
          << ctiter.thisSpw()
          << " refant list exhausted, flagging all solutions"
          << LogIO::POST; 
      }
    }

    // advance to the next interval
    lastspw=ispw;
    ctiter.next();
  }

  if (usedaltrefant)
    logSink() << LogIO::NORMAL
	      << " NB: An alternate refant was used at least once to maintain" << endl
	      << "  phase continuity where the user's preferred refant drops out." << endl
	      << "  Alternate refants are held constant in phase (_not_ zeroed)" << endl
	      << "  during these periods, and the preferred refant may return at" << endl
	      << "  a non-zero phase.  This is generally harmless."
	      << LogIO::POST;

  return;

}

} //# NAMESPACE CASA - END


/*
Example of use in at Python level:

fringefit(vis="n14c2.ms", caltable="fail.fj", field="",spw="1",intent="",
          selectdata=True, timerange="", antenna="", scan="5", observation="",
          msselect="", solint="inf", refant="EF", minsnr=3.0, append=False,
          gaintable=['n14c2.gcal'], parang=False)
*/

