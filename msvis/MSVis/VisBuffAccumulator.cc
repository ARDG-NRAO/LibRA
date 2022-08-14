//# VisBuffAccumulator.cc: Implementation of VisBuffAccumulator.h
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003
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
//# $Id: VisBuffAccumulator.cc,v 19.7 2006/01/17 08:22:27 gvandiep Exp $
//----------------------------------------------------------------------------

#include <msvis/MSVis/VisBuffAccumulator.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <iomanip>

#define PRTLEV_VBA 0

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------
  
VisBuffAccumulator::VisBuffAccumulator(const Int& nAnt, const Double& interval,
				       const Bool& prenorm, const Bool fillModel) 
  : avBuf_p(),
    nAnt_p(nAnt), 
    interval_p(interval), 
    prenorm_p(prenorm),
    prtlev_(PRTLEV_VBA),
    nBuf_p(0),
    fillModel_p(fillModel),
    tvi_debug(False)
{
// Construct from the number of antennas and the averaging interval
// Input:
//    nAnt                 const Int&       No. of antennas
//    interval             const Double&    Time interval (in seconds).
//    prenorm              const Bool&      Pre-normalization flag
//    fillModel            const Bool       Whether or not to accumulate MODEL_DATA
// Output to private data:
//    nAnt_p               Int              No. of antennas
//    interval_p           Double           Time interval (in seconds).
//    prenorm_p            Bool             Pre-normalization flag
//

  if (prtlev()>2) cout << "VBA::VBA()" << endl;

  // Interval MUST be strictly greater than zero
  if (interval_p < DBL_EPSILON)
    interval_p=0.1;   // TBD: is this reasonable? 

  // Reset the averager
  reset();

}

//----------------------------------------------------------------------------

VisBuffAccumulator::~VisBuffAccumulator()
{
// Null default destructor
//
  if (prtlev()>2) cout << "VBA::~VBA()" << endl;
}

//----------------------------------------------------------------------------

void VisBuffAccumulator::reset()
{
// Reset the averager
// Output to private data:
//    tStart_p         Double        Start time of current accumulation
//    firstInterval_p  Bool          Is this the first interval ?
//    nChan_p          Int           No. of channels in the averaging buffer
//    avrow_p          Int           Starting row of current accumulation
//    avBuf_p          VisBuffer     Averaging buffer
//
  tStart_p = 0.0;
  firstInterval_p = true;
  nCorr_p = 0;
  nChan_p = 0;
  avrow_p = 0;
  aveTime_p = 0.0;
  aveTimeWt_p = 0.0;
  globalTime_p = 0.0;
  globalTimeWt_p = 0.0;
  nBuf_p = 0;

  if (prtlev()>2) cout << " VBA::reset()" << endl;

}

//----------------------------------------------------------------------------

void VisBuffAccumulator::accumulate (const VisBuffer& vb)
{
// Accumulate a VisBuffer
// Input:
//    vb               const VisBuffer&     VisBuffer to accumulate
// Output to private data:
//    tStart_p         Double               Start time of current accumulation
//    nChan_p          Int                  No. of channels in the avg. buffer
//    avrow_p          Int                  Start row of current accumulation
//    avBuf_p          CalVisBuffer         Averaging buffer
//

  if (prtlev()>2) cout << " VBA::accumulate() " << endl;

  // Check if avBuf_p initialization required; if so,
  // assign to vb to establish a two-way connection to
  // the underlying VisIter. 
  if (firstInterval_p) {
    // Initialize the averaging buffer
    //  THIS IS OLD INEFFICIENT WAY (PRE-CALVISBUFFER)
    //    avBuf_p = vb;

    // Assign main meta info only
    //    avBuf_p.assign(vb,true);  
    avBuf_p.assign(vb,false);  
    //avBuf_p.updateCoordInfo();  // This is (simplified) CalVisBuffer version!
    avBuf_p.copyCoordInfo(vb, true);    // This only goes to VisIter if vb is
                                        // missing something needed.

    // Zero row count
    avBuf_p.nRow();
    avBuf_p.nRow() = 0;

    // Immutables:
    nChan_p = vb.nChannel();
    nCorr_p = vb.corrType().nelements();

    // Initialize the first accumulation interval
    //   without copy (nothing accumulated yet)
    initialize(false);

  }

  // Iterate through the current VisBuffer
  Int row = 0;
  while (row < vb.nRow()) {
    // Find the next unflagged time
    while (row < vb.nRow() && vb.flagRow()(row)) {
      row++;
    }
    if (row < vb.nRow()) {
      Double thisTime = vb.time()(row);

      // Check for the first accumulation interval
      if (firstInterval_p) {
	tStart_p = thisTime;
	firstInterval_p = false;
      }

      // Check for end of the current accumulation interval

      if ((vb.time()(row) - tStart_p) > (interval_p-DBL_EPSILON)) {
	// Normalize
	normalize();
	// Advance indices to the next accumulation interval
	tStart_p = thisTime;
	avrow_p += hashFunction(nAnt_p-1, nAnt_p-1) + 1;
	// Initialize the next accumulation interval
	//  (copy prior preavg'd intervals)
	initialize(true);
      }

      // Add the VisBuffer row to the current accumulation
      // 
      // Only accumulate VisBuffers with the same number of channels
      Int nCorr=vb.corrType().nelements();
      if (vb.nChannel() != nChan_p || nCorr != nCorr_p) {
	throw(AipsError("VisBuffAccumulator: data shape does not conform"));
      }

      Int ant1 = vb.antenna1()(row);
      Int ant2 = vb.antenna2()(row);

      // Calculate row from antenna numbers with the hash function.
      Int outrow = avrow_p + hashFunction (ant1, ant2);

      // Record the row in vb that corresponds to outrow in avBuf_p.
      outToInRow_p[outrow] = row;

      Vector<Float> wtM(vb.weightMat().column(row));
      // Row weight for timestamp ave:
      //  (used to use vb.weight(), which read WEIGHT col 
      //   directly, which is wrong because it might be calibrated!)
      //  The following is what vb.weight() does, but here with
      //   the weigthMat info, which has been reset from SIGMA
      Float wt = (wtM(0)+wtM(nCorr-1))/2;  

      // (Prenormalization removed!)

      // Accumulate the visCube itself
      //  TBD: Handle weights of channel-dep accumulation
      //       better (i.e., if some channels are sometimes
      //       flagged, they should accumulate with different
      //       total weights, even if we will maintain only 
      //       a channel-indep weightMat.  So, either use
      //       weightSpectrum, or at least accumulate per
      //       channel correctly!)

      Int goodChan(0);
      for (Int chn=0; chn<vb.nChannel(); chn++) {
	if (!vb.flag()(chn,row) or tvi_debug) {// jagonzal: Always copy inputVis to outputVis
	  goodChan++;
	  avBuf_p.flag()(chn,outrow) = false;
	  for (Int cor=0;cor<nCorr;cor++) {
	    avBuf_p.visCube()(cor,chn,outrow) += 
	      (wtM(cor)*vb.visCube()(cor,chn,row));
            if(fillModel_p)
              avBuf_p.modelVisCube()(cor,chn,outrow) += 
                (wtM(cor)*vb.modelVisCube()(cor,chn,row));
	  }
	}
      }

      // Only if there is any good channels this row
      if (goodChan > 0) {
	avBuf_p.flagRow()(outrow) = false;
	avBuf_p.weight()(outrow) += wt;
	for (Int cor=0;cor<nCorr;cor++) 
	  avBuf_p.weightMat()(cor,outrow) += wtM(cor);

	// UVW (vector average, is this right?)
	// gcc-3.2 needs the multiplication on a separate line; gcc-3.4 can do
	// it as: avBuf_p.uvw()(outrow) += vb.uvw()(row) * Double(wt);
	RigidVector<Double,3> wtuvw = vb.uvw()(row) * Double(wt);
	avBuf_p.uvw()(outrow) += wtuvw;
	
	// Accumulate global timestamp average for this interval:
	//   (subtract offset from time here to avoid roundoff problems)
	aveTime_p += (vb.time()(row)-tStart_p) * Double(wt);
	aveTimeWt_p += Double(wt);
      }

      // Increment the row number
      row++;
    } // if (row < vb.nRow())
  } // while (row < vb.nRow())
  ++nBuf_p;

  // jagonzal: Fill rowId and flagCube (flag only provides
  // an OR of the flags corresponding to all corrs per chan)
  if (nBuf_p==1 and tvi_debug)
  {
	  for (uInt outrow_idx=0;outrow_idx<outToInRow_p.size();outrow_idx++)
	  {
		  if (outToInRow_p(outrow_idx) > -1)
		  {
			  avBuf_p.rowIds()(outrow_idx) = vb.rowIds()(outToInRow_p(outrow_idx));
			  avBuf_p.flagCube().xyPlane(outrow_idx) = vb.flagCube().xyPlane(outToInRow_p(outrow_idx));
		  }
		  else
		  {
			  avBuf_p.rowIds()(outrow_idx) = -1;
			  avBuf_p.flagCube().xyPlane(outrow_idx) = True;
		  }
	  }
  }
}

//----------------------------------------------------------------------------

void VisBuffAccumulator::finalizeAverage ()
{
// Finalize the average, and return the result
// Output:
//    avBuf         VisBuffer&       Averaged buffer
//

  if (prtlev()>2) cout << " VBA::finalizeAverage()" << endl;

  // Normalize the current (final) accumulation interval
  normalize();

}

//----------------------------------------------------------------------------

void VisBuffAccumulator::initialize(const Bool& copydata)
{
// Initialize the next accumulation interval
// Output to private data:
//    avBuf_p          VisBuffer       Averaging buffer
//    

  if (prtlev()>2) cout << "  VBA::initialize()" << endl;



  
  ///KG notes
  ////Really dangerous and unmaintainable code changing private variables of vb without 
  ////really changing the internal connnections ...so if say some internal or vb or like here
  //// vb.modelVisCube decides to use its private variables ...its nrow is totally wrong ...
  /// after the next few lines !
  ////function decide to use nRow_p or nChannel_p it is bound to be wrong
  //// that is why i am calling the damn visCube and modelVisCube above because
  /// coders belive that vb.nRow should give the visbuffer number of rows when accessing /// a given array

  Int nRowAdd = hashFunction (nAnt_p-1, nAnt_p-1) + 1;
  avBuf_p.nRow() += nRowAdd;
  avBuf_p.nChannel() = nChan_p;

  // Resize and initialize the VisBuffer columns used here
  Int nRow = avBuf_p.nRow();

  avBuf_p.antenna1().resize(nRow, copydata);
  avBuf_p.antenna2().resize(nRow, copydata);

  avBuf_p.time().resize(nRow, copydata); 
  avBuf_p.uvw().resize(nRow, copydata); 

  avBuf_p.visCube().resize(nCorr_p,nChan_p, nRow,copydata);
  if(fillModel_p)
    copydata ?
      avBuf_p.modelVisCube().resize(nCorr_p,nChan_p, nRow,copydata):
      avBuf_p.setModelVisCube(Cube<Complex>(nCorr_p,nChan_p, nRow));

  avBuf_p.weight().resize(nRow, copydata); 
  avBuf_p.weightMat().resize(nCorr_p,nRow, copydata); 

  avBuf_p.flagRow().resize(nRow, copydata); 

  avBuf_p.flag().resize(nChan_p, nRow,copydata);

  // jagonzal: Fill rowId and flagCube (flag only provides
  // an OR of the flags corresponding to all corrs per chan)
  if (tvi_debug)
  {
	  avBuf_p.rowIds().resize(nRow, copydata);
	  avBuf_p.flagCube().resize(nCorr_p,nChan_p, nRow,copydata);
  }

  // Setup the map from avBuf_p's row numbers to input row numbers.
  outToInRow_p.resize(nRow, copydata);
  if(!copydata)
    outToInRow_p = -1;                   // Unfilled rows point to -1.

  // Fill in the antenna numbers for all rows
  Int row = avrow_p;
  for (Int ant1=0; ant1 < nAnt_p; ant1++) {
    for (Int ant2 = ant1; ant2 < nAnt_p; ant2++) {
      avBuf_p.antenna1()(row) = ant1;
      avBuf_p.antenna2()(row) = ant2;
      row++;
    }
  }

  // Initialize everything else
  for (row = avrow_p; row < nRow; row++) {
    avBuf_p.time()(row) = 0.0;
    avBuf_p.uvw()(row) = 0.0;
    for (Int chn = 0; chn < nChan_p; chn++) {
      avBuf_p.flag()(chn,row) = true;
      for (Int cor=0; cor < nCorr_p; cor++) {
	avBuf_p.visCube()(cor,chn,row) = Complex(0.0);
        if(fillModel_p)
          avBuf_p.modelVisCube()(cor,chn,row) = Complex(0.0);
      }
    }
    avBuf_p.weight()(row) = 0.0f;
    avBuf_p.weightMat().column(row) = 0.0f;
    avBuf_p.flagRow()(row) = true;
  }

  // Init global timestamp
  aveTime_p = 0.0;
  aveTimeWt_p = 0.0;

}

//----------------------------------------------------------------------------

void VisBuffAccumulator::normalize()
{
// Normalize the current accumulation interval
// Output to private data:
//    avBuf_p         VisBuffer&       Averaged buffer
//  

  if (prtlev()>2) cout  << "  VBA::normalize()" << endl;

  // Only if there will be a valid timestamp
  if (aveTimeWt_p > 0.0 ) {

    // Contribute to global timestamp average
    globalTime_p-=tStart_p;
    globalTime_p*=globalTimeWt_p;
    globalTime_p+=aveTime_p;
    globalTimeWt_p+=aveTimeWt_p;
    globalTime_p/=globalTimeWt_p;
    globalTime_p+=tStart_p;

    // Mean timestamp for this interval
    aveTime_p/=aveTimeWt_p;
    aveTime_p+=tStart_p;

    // Divide by the weights
    for (Int row=avrow_p; row<avBuf_p.nRow(); row++) {
      Float wt=avBuf_p.weight()(row);
      Vector<Float> wtM(avBuf_p.weightMat().column(row));
      if (wt==0.0f) avBuf_p.flagRow()(row)=true;
      if (!avBuf_p.flagRow()(row)) {
	avBuf_p.time()(row)=aveTime_p;
	avBuf_p.uvw()(row)*=(1.0f/wt);
	for (Int chn=0; chn<avBuf_p.nChannel(); chn++) {
	  if (!avBuf_p.flag()(chn,row)) {
	    for (Int cor=0;cor<nCorr_p;cor++) {
	      if (wtM(cor)>0.0f) {
		avBuf_p.visCube()(cor,chn,row)*=1.0f/wtM(cor);
                if(fillModel_p)
                  avBuf_p.modelVisCube()(cor,chn,row)*=1.0f/wtM(cor);
	      }
	      else {
		avBuf_p.visCube()(cor,chn,row)=0.0;
                if(fillModel_p)
                  avBuf_p.modelVisCube()(cor,chn,row)=0.0;
	      }
	    }
	  }
	}
      }
    }
  }
  else {
    // Ensure this interval is entirely flagged
    for (Int row=avrow_p; row<avBuf_p.nRow(); row++) {
      avBuf_p.flagRow()(row)=true;
      avBuf_p.weight()(row)=0.0f;
      avBuf_p.weightMat().column(row)=0.0f;
    }
  }
}

//----------------------------------------------------------------------------

Int VisBuffAccumulator::hashFunction (const Int& ant1, const Int& ant2)
{
// Compute row index in an accumulation interval for an
// interferometer index (ant1, ant2).
// Input:
//    ant1            const Int&      Antenna 1
//    ant2            const Int&      Antenna 2
// Output:
//    hashFunction    Int             Row offset in current accumulation
//
  Int index;
  index = nAnt_p * ant1 - (ant1 * (ant1 - 1)) / 2 + ant2 - ant1;
  return index;
}

//----------------------------------------------------------------------------

void VisBuffAccumulator::throw_err(const String& origin, const String &msg)
{
  LogOrigin("VisBuffAccumulator", origin);
  throw(AipsError(msg));
}

void VisBuffAccumulator::reportData() {

  CalVisBuffer& cvb(aveCalVisBuff());
  Slice corrs(0,2,3), sl;

  Vector<Int> a1(cvb.antenna1()), a2(cvb.antenna2());
  Cube<Complex> vCC(cvb.visCube()(corrs,sl,sl));
  Cube<Complex> vCM(cvb.modelVisCube()(corrs,sl,sl));
  Matrix<Float> wtS(cvb.weightMat()(corrs,sl));
  Matrix<Bool> flg(cvb.flag()(sl,sl));

  cout << "Time=" << MVTime(timeStamp()/C::day).string(MVTime::YMD,7) << endl;
  cout.precision(8);
  for (Int irow=0;irow<cvb.nRow();++irow) {
    for (Int ich=0;ich<cvb.nChannel();++ich) {
      if (flg(ich,irow)) continue;
      cout << std::setw(2) << a1(irow) << "-" << std::setw(2) << a2(irow) << " ";
      if (cvb.nChannel()>1) cout << "ich=" << ich << " ";
      cout << "fl=" << flg(Slice(ich),Slice(irow)).nonDegenerate() << " ";
      cout << "wt=" << wtS(Slice(),Slice(irow)).nonDegenerate() << " ";
      cout << "cM=" << vCM(Slice(),Slice(),Slice(irow)).nonDegenerate() << " ";
      cout << "cC=" << vCC(Slice(),Slice(),Slice(irow)).nonDegenerate() << " ";
      cout << endl;
    }
  }



}



} //# NAMESPACE CASA - END

