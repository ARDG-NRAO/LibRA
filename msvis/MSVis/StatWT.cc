//# StatWT.cc: Subtract the continuum from VisBuffGroups and
//# write them to a different MS.
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

#include <msvis/MSVis/StatWT.h>
//#include <msvis/MSVis/SubMS.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <msvis/MSVis/VisBuffGroup.h>
#include <msvis/MSVis/VisBuffGroupAcc.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogIO.h>
#include <ms/MSSel/MSSelection.h>
#include <casa/Arrays/ArrayMath.h>

using namespace casacore;
namespace casa {

StatWT::StatWT(const ROVisibilityIterator& vi,
               const MS::PredefinedColumns datacol,
               const String& fitspw,
               const String& outspw,
               const Bool dorms,
               const uInt minsamp,
               const vector<uInt> selcorrs) :
  GroupWorker(vi),
  datacol_p(datacol),
  fitspw_p(fitspw),
  outspw_p(outspw),
  dorms_p(dorms),
  rowsdone_p(0)
{
  LogIO os(LogOrigin("StatWT", "StatWT()"));

  if(dorms && minsamp < 1){
    os << LogIO::WARN
       << "It takes at least one to measure an rms - using minsamp = 1."
       << LogIO::POST;
    minsamp_p = 1;
  }
  else if(!dorms && minsamp < 2){
    os << LogIO::WARN
       << "It takes at least two to measure a variance - using minsamp = 2."
       << LogIO::POST;
    minsamp_p = 2;
  }
  else
    minsamp_p = minsamp;

  prefetchColumns_p = asyncio::PrefetchColumns::prefetchColumns(
                                  VisBufferComponents::Ant1,
                                  VisBufferComponents::Ant2,
                                  VisBufferComponents::ArrayId,
                                  VisBufferComponents::CorrType,
                                  VisBufferComponents::DataDescriptionId,
                                  //VisBufferComponents::Feed1,
                                  //VisBufferComponents::Feed2,
                                  VisBufferComponents::FieldId,
                                  VisBufferComponents::FlagCube,
                                  VisBufferComponents::Flag,
                                  VisBufferComponents::FlagRow,
                                  VisBufferComponents::ObservationId,
                                  //VisBufferComponents::NChannel,
                                  VisBufferComponents::NCorr,
                                  VisBufferComponents::NRow,
                                  //VisBufferComponents::ProcessorId,
                                  VisBufferComponents::Scan,
                                  VisBufferComponents::SpW,
                                  VisBufferComponents::SigmaMat,
                                  VisBufferComponents::StateId,
                                  //VisBufferComponents::Time,
                                  //VisBufferComponents::TimeCentroid,
                                  //VisBufferComponents::TimeInterval,
                                  VisBufferComponents::WeightMat,
                                  -1);
  if(datacol == MS::DATA)
    prefetchColumns_p.insert(VisBufferComponents::ObservedCube);
  else if(datacol == MS::MODEL_DATA)
    prefetchColumns_p.insert(VisBufferComponents::ModelCube);
  else if(datacol == MS::CORRECTED_DATA)
    prefetchColumns_p.insert(VisBufferComponents::CorrectedCube);
  //  else if(datacol == MS::FLOAT_DATA)    // Not in VisBufferComponents yet.
  //  prefetchColumns_p.insert(VisBufferComponents::FloatCube);

  VisBuffGroupAcc::fillChanMask(fitmask_p, fitspw, invi_p.ms());

  MSSelection mssel;
  mssel.setSpwExpr(outspw);
  Matrix<Int> chansel = mssel.getChanList(&(invi_p.ms()), 1);
  Vector<Int> spws(chansel.column(0));
  uInt nselspws = spws.nelements();
  selcorrs_p = selcorrs;

  for(uInt i = 0; i < nselspws; ++i)
    outspws_p.insert(spws[i]);
}

StatWT::~StatWT()
{
  VisBuffGroupAcc::clearChanMask(fitmask_p);
}

// StatWT& StatWT::operator=(const StatWT &other)
// {
//   // trivial so far.
//   vi_p = other.vi_p;
//   return *this;
// }

Bool StatWT::process(VisBuffGroup& vbg)
{
  LogIO os(LogOrigin("StatWT", "process()"));
  Bool worked = true;
  uInt nvbs = vbg.nBuf();
  Int maxAnt = 0;
  Int maxNCorr = 0;
  Int maxSpw = 0;   // VisBuffGroupAcc is 1 of those things that uses SpW when
                    // it should use DDID.
  
  for(uInt bufnum = 0; bufnum < nvbs; ++bufnum){
    if(vbg(bufnum).numberAnt() > maxAnt)        // Record maxAnt even for buffers
      maxAnt = vbg(bufnum).numberAnt();         // that won't be used in the fit.
    if(vbg(bufnum).nCorr() > maxNCorr)
      maxNCorr = vbg(bufnum).nCorr();

    if(bufnum > 0 && anyTrue(vbg(bufnum).corrType() != vbg(0).corrType())){
      os << LogIO::SEVERE
         << "statwt does not yet support combining data description IDs with different correlation setups."
         << LogIO::POST;
      return false;
    }

    Int spw = vbg(bufnum).spectralWindow();
    if(fitmask_p.count(spw) > 0){               // This requires fitspw to
                                                // follow the '' = nothing,
                                                // '*' = everything convention.
      if(spw > maxSpw)
        maxSpw = vbg(bufnum).spectralWindow();
    }
  }

  Cube<Bool> chanmaskedflags;

  // Map from hashFunction(ant1, ant2) to running number of visibilities[corr]
  std::map<uInt, Vector<uInt> > ns;

  // Map from hashFunction(ant1, ant2) to running mean[corr]
  std::map<uInt, Vector<Complex> > means;

  // Map from hashFunction(ant1, ant2) to variance[corr], initially stored as
  // running sums of squared differences.
  std::map<uInt, Vector<Double> > variances;

  // The accumulation of sums for the variances could be parallelized.
  // See Chan, Tony F.; Golub, Gene H.; LeVeque, Randall J. (1979), "Updating
  // Formulae and a Pairwise Algorithm for Computing Sample Variances.",
  // Technical Report STAN-CS-79-773, Department of Computer Science, Stanford
  // University.

  for(uInt bufnum = 0; bufnum < nvbs; ++bufnum){
    Int spw = vbg(bufnum).spectralWindow();

    if(fitmask_p.count(spw) > 0){
      VisBuffGroup::applyChanMask(chanmaskedflags, fitmask_p[spw], vbg(bufnum));

      if(!update_variances(ns, means, variances, vbg(bufnum), chanmaskedflags,
                           maxAnt))
        return false;
    }
  }
  for(std::map<uInt, Vector<Double> >::iterator it = variances.begin();
      it != variances.end(); ++it)
    for(Int corr = 0; corr < maxNCorr; ++corr)
      it->second[corr] /= (2.*ns[it->first][corr] - 1);

  // TODO
  // if(byantenna_p){
  // // The formula for the variance of antenna k is
  // // \sigma_k^2 = \frac{1}{n - 1} \sum_{i \notequal k} \left(
  // // \sigma_{ik}^2 \frac{\sum_{j \notequal i,k}^{k - 1} \sigma_{jk}^2}
  // // {\sum_{j \notequal i,k} \sigma_{ij}^2}\right)
  // // where \sigma_{ij}^2 is the already calculated variance of baseline ij.
  //
  // // So, get the antenna based variances, take their sqrts \sigma_k, and
  // // update variances to \sigma_i \sigma_j, taking sepacs_p into account all
  // // along.
  // }

  //uInt oldrowsdone = rowsdone_p;
  for(uInt bufnum = 0; bufnum < nvbs; ++bufnum){
    uInt spw = vbg(bufnum).spectralWindow();

    rowsdone_p += vbg(bufnum).nRow();
    if(outspws_p.find(spw) != outspws_p.end()){
      worked &= apply_variances(vbg(bufnum), ns, variances, maxAnt);      
      //cerr << "Wrote out row IDs " << oldrowsdone << " - " << rowsdone_p - 1 << ",";
    }
    //else
    //  cerr << "No output for";
    //cerr << " spw " << spw << endl;
    //oldrowsdone = rowsdone_p;

    // Catch outvi_p up with invi_p.
    if(vbg.chunkEnd(bufnum) && outvi_p.moreChunks()){
      outvi_p.nextChunk();
      outvi_p.origin();
    }
    else if(outvi_p.more())
      ++outvi_p;
  }
  
  return worked;
}

Bool StatWT::update_variances(std::map<uInt, Vector<uInt> >& ns,
                              std::map<uInt, Vector<Complex> >& means,
                              std::map<uInt, Vector<Double> >& variances,
                              const VisBuffer& vb,
                              const Cube<Bool>& chanmaskedflags, const uInt maxAnt)
{
  Cube<Complex> data(vb.dataCube(datacol_p));

  if(data.shape() != chanmaskedflags.shape())
    return false;

  Bool retval = true;
  uInt nCorr = data.shape()[0];
  uInt nChan = data.shape()[1];
  rownr_t nRows = data.shape()[2];
  Vector<uInt> unflagged(nChan);
  Vector<Int> a1(vb.antenna1());
  Vector<Int> a2(vb.antenna2());

  for(rownr_t r = 0; r < nRows; ++r){
    if(!vb.flagRow()[r]){
      uInt hr = hashFunction(a1[r], a2[r], maxAnt);
      // setup defaults, clear on all-flagged not needed as variances == 0 is
      // skipped in apply_variances
      if(!ns.count(hr)){
        ns[hr] = Vector<uInt>(nCorr, 0);
        means[hr] = Vector<Complex>(nCorr, 0);
        variances[hr] = Vector<Double>(nCorr, 0);
      }
      Vector<uInt> & vns = ns[hr];
      Vector<Complex> & vmeans = means[hr];
      Vector<Double> & vvariances = variances[hr];

      for(uInt corr = 0; corr < nCorr; ++corr){
        for(uInt ch = 0; ch < nChan; ++ch){
          if(!chanmaskedflags(corr, ch, r) && !vb.flagCube()(corr,ch,r)){
            Complex vis, vmoldmean, vmmean;
            ++vns[corr];
            vis = data(corr, ch, r);
            vmoldmean = vis - vmeans[corr];

            if(!dorms_p)  // It's not that Complex / Int isn't defined, it's
                          // that it is, along with Complex / Double, creating
                          // an ambiguity.
              vmeans[corr] += vmoldmean / static_cast<Double>(vns[corr]);

            // This term is guaranteed to have its parts be nonnegative.
            vmmean = vis - vmeans[corr];
            vvariances[corr] += vmmean.real() * vmoldmean.real() +
                                vmmean.imag() * vmoldmean.imag();
          }
        }
      }
    }
  }
  return retval;
}

Bool StatWT::apply_variances(VisBuffer& vb,
                             std::map<uInt, Vector<uInt> >& ns,
                             std::map<uInt, Vector<Double> >& variances,
                             const uInt maxAnt)
{
  Bool retval = true;
  IPosition shp(vb.flagCube().shape());
  uInt nCorr = shp[0];
  uInt nChan = shp[1];
  rownr_t nRows = shp[2];
  Vector<Int> a1(vb.antenna1());
  Vector<Int> a2(vb.antenna2());

  for(rownr_t r = 0; r < nRows; ++r){
    uInt hr = hashFunction(a1[r], a2[r], maxAnt);
    Bool unflagged = false;
    Bool havevar = ns.count(hr) > 0;

    for(uInt corr = 0; corr < nCorr; ++corr){
	if(havevar &&
	   (ns[hr][corr] >= minsamp_p) &&
	   (0.0 < variances[hr][corr])){ // For some reason emacs likes 0 < v,
					 // but not v > 0.
	  Double var = variances[hr][corr];

	  unflagged = true;
	  vb.sigmaMat()(corr, r) = sqrt(var);
	  vb.weightMat()(corr, r) = 1.0 / var;
	}
	else{
	  vb.sigmaMat()(corr, r) = -1.0;
	  vb.weightMat()(corr, r) = 0.0;
	  for(uInt ch = 0; ch < nChan; ++ch){
	    vb.flagCube()(corr, ch, r) = true;
	  }
	}
        if(!unflagged)
          vb.flagRow()[r] = true;
    }
  }
  
  // argh
  // outvi_p.setFlagCube(vb.flagCube());
  outvi_p.setFlag(vb.flag());

  outvi_p.setSigmaMat(vb.sigmaMat());
  outvi_p.setWeightMat(vb.weightMat());
  return retval;
}

using namespace casacore;
} // end namespace casa
