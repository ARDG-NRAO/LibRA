//# VisBufferUtil.cc: VisBuffer Utilities
//# Copyright (C) 1996,1997,2001
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

#include <casacore/casa/aips.h>

#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MatrixMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Utilities/Sort.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/measures/Measures/UVWMachine.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/ms/MSSel/MSSelectionTools.h>
#include <casacore/ms/MeasurementSets/MSPointingColumns.h>
#include <msvis/MSVis/VisBufferUtil.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// </summary>

// <reviewed reviewer="" date="" tests="tMEGI" demos="">

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


  VisBufferUtil::VisBufferUtil(): oldMSId_p(-1), oldPCMSId_p(-1), timeAntIndex_p(0), cachedPointingDir_p(0), cachedPhaseCenter_p(0){};


// Construct from a VisBuffer (sets frame info)
  VisBufferUtil::VisBufferUtil(const VisBuffer& vb): oldMSId_p(-1), oldPCMSId_p(-1), timeAntIndex_p(0), cachedPointingDir_p(0), cachedPhaseCenter_p(0) {

  // The nominal epoch 
  MEpoch ep=vb.msColumns().timeMeas()(0);

  // The nominal position
  String observatory;
  MPosition pos;
  if (vb.msColumns().observation().nrow() > 0) {
    observatory = vb.msColumns().observation().telescopeName()
      (vb.msColumns().observationId()(0));
  }
  if (observatory.length() == 0 || 
      !MeasTable::Observatory(pos,observatory)) {
    // unknown observatory, use first antenna
    pos=vb.msColumns().antenna().positionMeas()(0);
  }
 
  // The nominal direction
  MDirection dir=vb.phaseCenter();

  // The nominal MeasFrame
  mframe_=MeasFrame(ep, pos, dir);

}

// Construct from a VisBuffer (sets frame info)
  VisBufferUtil::VisBufferUtil(const vi::VisBuffer2& vb): oldMSId_p(-1), oldPCMSId_p(-1),  timeAntIndex_p(0), cachedPointingDir_p(0), cachedPhaseCenter_p(0) {
	if(!vb.isAttached())
		ThrowCc("Programmer Error: used a detached Visbuffer when it should not have been so");
	MSColumns msc(vb.ms());
  // The nominal epoch
  MEpoch ep=msc.timeMeas()(0);

  // The nominal position
  String observatory;
  MPosition pos;
  if (msc.observation().nrow() > 0) {
    observatory = msc.observation().telescopeName()
      (msc.observationId()(0));
  }
  if (observatory.length() == 0 ||
      !MeasTable::Observatory(pos,observatory)) {
    // unknown observatory, use first antenna
    pos=msc.antenna().positionMeas()(0);
  }

  // The nominal direction
  MDirection dir=vb.phaseCenter();

  // The nominal MeasFrame
  mframe_=MeasFrame(ep, pos, dir);

}
VisBufferUtil::VisBufferUtil(const vi::VisibilityIterator2& iter): oldMSId_p(-1) {

	MSColumns msc(iter.ms());
  // The nominal epoch
  MEpoch ep=msc.timeMeas()(0);

  // The nominal position
  String observatory;
  MPosition pos;
  if (msc.observation().nrow() > 0) {
    observatory = msc.observation().telescopeName()
      (msc.observationId()(0));
  }
  if (observatory.length() == 0 ||
      !MeasTable::Observatory(pos,observatory)) {
    // unknown observatory, use first antenna
    pos=msc.antenna().positionMeas()(0);
  }

  // The nominal direction
  //MDirection dir=iter.phaseCenter();
  MDirection dir=msc.field().phaseDirMeasCol()(0)(IPosition(1,0));
  // The nominal MeasFrame
  mframe_=MeasFrame(ep, pos, dir);

}
VisBufferUtil::VisBufferUtil(const MeasFrame& mframe): oldMSId_p(-1) {
	mframe_=mframe;

}
Bool VisBufferUtil::rotateUVW(const vi::VisBuffer2&vb, const MDirection& desiredDir,
				Matrix<Double>& uvw, Vector<Double>& dphase){

    Bool retval=true;
    mframe_.resetEpoch(vb.time()(0));
    UVWMachine uvwMachine(desiredDir, vb.phaseCenter(), mframe_,
			false, false);
    retval = !uvwMachine.isNOP();
    dphase.resize(vb.nRows());
    dphase.set(0.0);
    if(uvw.nelements() ==0)
      uvw=vb.uvw();
    for (rownr_t row=0; row< vb.nRows(); ++row){
      Vector<Double> eluvw(uvw.column(row));
      uvwMachine.convertUVW(dphase(row), eluvw);
    }
    
    return retval;
  }

// Set the visibility buffer for a PSF
void VisBufferUtil::makePSFVisBuffer(VisBuffer& vb) {
  CStokesVector coh(Complex(1.0), Complex(0.0), Complex(0.0), Complex(1.0));
  vb.correctedVisibility()=coh;
}


Bool VisBufferUtil::interpolateFrequency(Cube<Complex>& data, 
					 Cube<Bool>& flag, 
					 const VisBuffer& vb,
					 const Vector<Float>& outFreqGrid, 	
					 const MS::PredefinedColumns whichCol, 
					 const MFrequency::Types freqFrame,
					 const InterpolateArray1D<Float,Complex>::InterpolationMethod interpMethod){

  Cube<Complex> origdata;
  // Convert the visibility frequency to the frame requested
  Vector<Double> visFreqD;
  convertFrequency(visFreqD, vb, freqFrame);
  //convert it to Float
  Vector<Float> visFreq(visFreqD.nelements());
  convertArray(visFreq, visFreqD);
  
  //Assign which column is to be regridded to origdata
  if(whichCol==MS::MODEL_DATA){
    origdata.reference(vb.modelVisCube());
  }
  else if(whichCol==MS::CORRECTED_DATA){
      origdata.reference(vb.correctedVisCube());
    }
  else if(whichCol==MS::DATA){
      origdata.reference(vb.visCube());
  }
  else{
    throw(AipsError("Don't know which column is being regridded"));
  }
  Cube<Complex> flipdata;
  Cube<Bool> flipflag;
  //The interpolator interpolates on the 3rd axis only...so need to flip the axes (y,z)
  swapyz(flipflag,vb.flagCube());
  swapyz(flipdata,origdata);

  //interpolate the data and the flag to the output frequency grid
  InterpolateArray1D<Float,Complex>::
    interpolate(data,flag, outFreqGrid,visFreq,flipdata,flipflag,interpMethod);
  flipdata.resize();
  //reflip the data and flag to be in the same order as in Visbuffer output
  swapyz(flipdata,data);
  data.resize();
  data.reference(flipdata);
  flipflag.resize();
  swapyz(flipflag,flag);
  flag.resize();     
  flag.reference(flipflag);

  return true;

}
  void VisBufferUtil::getFreqRange(Double& freqMin, Double& freqMax, vi::VisibilityIterator2& vi, MFrequency::Types freqFrame){
    vi.originChunks();
    vi.origin();

    Double freqEnd=0.0;
    Double freqStart=C::dbl_max;
    vi::VisBuffer2* vb=vi.getVisBuffer();
    for (vi.originChunks(); vi.moreChunks();vi.nextChunk())
    	{
	  for (vi.origin(); vi.more();vi.next())
    		{
		  Double localmax, localmin;
                  IPosition localmaxpos(1,0); 
                  IPosition localminpos(1,0);
		  Vector<Double> freqs=vb->getFrequencies(0, freqFrame);
		  if(freqs.nelements() ==0){
		    throw(AipsError("Frequency selection error" ));
		  }
                  minMax(localmin,localmax,localminpos, localmaxpos, freqs);
		  //localmax=max(freqs);
		  //localmin=min(freqs);
		  //freqEnd=max(freqEnd, localmax);
		  //freqStart=min(freqStart, localmin);

                  Int nfreq = freqs.nelements(); 
                  Vector<Int> curspws = vb->spectralWindows();
                  // as the vb row 0 is used for getFrequencies, the same row 0 is used here
                  Vector<Double> chanWidths = vi.subtableColumns().spectralWindow().chanWidth()(curspws[0]);  
                  // freqs are in channel center freq so add the half the width to the values to return the edge frequencies 
                  if (nfreq==1) {
		    freqEnd=max(freqEnd, localmax+fabs(chanWidths[0]/2.0));
		    freqStart=min(freqStart, localmin-fabs(chanWidths[0]/2.0));
                  }
                  else {
		    freqEnd=max(freqEnd, localmax+fabs(chanWidths[localmaxpos[0]]/2.0));
		    freqStart=min(freqStart, localmin-fabs(chanWidths[localminpos[0]]/2.0));
                  }
		   
		}
	}
    freqMin=freqStart;
    freqMax=freqEnd;
  }

  Bool VisBufferUtil::getFreqRangeFromRange(casacore::Double& outfreqMin, casacore::Double& outfreqMax,  const casacore::MFrequency::Types inFreqFrame, const casacore::Double infreqMin, const casacore::Double infreqMax, vi::VisibilityIterator2& vi, casacore::MFrequency::Types outFreqFrame){


    if(inFreqFrame==outFreqFrame){
      outfreqMin=infreqMin;
      outfreqMax=infreqMax;
      return True;
    }

    
    vi.originChunks();
    vi.origin();
    try{
      outfreqMin=C::dbl_max;
      outfreqMax=0;
      vi::VisBuffer2* vb=vi.getVisBuffer();
      MSColumns msc(vi.ms());
      // The nominal epoch
      MEpoch ep=msc.timeMeas()(0);
      
      // The nominal position
      String observatory;
      MPosition pos;
      if (msc.observation().nrow() > 0) {
	observatory = msc.observation().telescopeName()
	  (msc.observationId()(0));
      }
      if (observatory.length() == 0 ||
	  !MeasTable::Observatory(pos,observatory)) {
	// unknown observatory, use first antenna
	pos=msc.antenna().positionMeas()(0);
      }
      
      // The nominal direction
      MDirection dir=vb->phaseCenter();
      MeasFrame mFrame(ep, pos, dir);
      // The conversion engine:
      MFrequency::Convert toNewFrame(inFreqFrame, 
				     MFrequency::Ref(outFreqFrame, mFrame));
      
      for (vi.originChunks(); vi.moreChunks();vi.nextChunk())
    	{
	  for (vi.origin(); vi.more();vi.next()){
	    //assuming time is fixed in visbuffer
	    mFrame.resetEpoch(vb->time()(0)/86400.0);
	    
	    // Reset the direction (ASSUMES phaseCenter is constant in the VisBuffer)
	    mFrame.resetDirection(vb->phaseCenter());
	    Double temp=toNewFrame(infreqMin).getValue().getValue();
	    if(temp < outfreqMin)
	      outfreqMin = temp;
	    
	    temp=toNewFrame(infreqMax).getValue().getValue();
	    if(temp > outfreqMax)
	      outfreqMax = temp;	      
	  }
	}
    }
    catch(...){
      //Could not do a conversion
      return False;
      
    }
      //cerr << "min " << outfreqMin << " max " << outfreqMax << endl;
      return True;
      
  }

void VisBufferUtil::convertFrequency(Vector<Double>& outFreq, 
				     const VisBuffer& vb, 
				     const MFrequency::Types freqFrame){
   Int spw=vb.spectralWindow();
   MFrequency::Types obsMFreqType=(MFrequency::Types)(vb.msColumns().spectralWindow().measFreqRef()(spw));

   // The input frequencies (a reference)
   Vector<Double> inFreq(vb.frequency());

   // The output frequencies
   outFreq.resize(inFreq.nelements());

   MFrequency::Types newMFreqType=freqFrame;
   if (freqFrame==MFrequency::N_Types)
     // Opt out of conversion
     newMFreqType=obsMFreqType;


   // Only convert if the requested frame differs from observed frame
   if(obsMFreqType != newMFreqType){

     // Setting epoch to the first in this iteration
     //     MEpoch ep=vb.msColumns().timeMeas()(0);
     //     MEpoch ep(MVEpoch(vb.time()(0)/86400.0),MEpoch::UTC);
     //     cout << "Time = " << ep.getValue()  << endl;

     // Reset the timestamp (ASSUMES TIME is constant in the VisBuffer)
     mframe_.resetEpoch(vb.time()(0)/86400.0);

     // Reset the direction (ASSUMES phaseCenter is constant in the VisBuffer)
     mframe_.resetDirection(vb.msColumns().field().phaseDirMeasCol()(vb.fieldId())(IPosition(1,0)));

     //     cout << "Frame = " << mframe_ << endl;

     // The conversion engine:
     MFrequency::Convert toNewFrame(obsMFreqType, 
				    MFrequency::Ref(newMFreqType, mframe_));

     // Do the conversion
     for (uInt k=0; k< inFreq.nelements(); ++k)
       outFreq(k)=toNewFrame(inFreq(k)).getValue().getValue();
     
   }
   else{
     // The requested frame is the same as the observed frame
     outFreq=inFreq;
   }

 }

void VisBufferUtil::convertFrequency(Vector<Double>& outFreq, 
				     const vi::VisBuffer2& vb, 
				     const MFrequency::Types freqFrame){
  Int spw=vb.spectralWindows()(0);
  MFrequency::Types obsMFreqType=(MFrequency::Types)(MSColumns(vb.ms()).spectralWindow().measFreqRef()(spw));

  

   // The input frequencies 
  Vector<Int> chanNums=vb.getChannelNumbers(0);
  
  Vector<Double> inFreq(chanNums.nelements());
  Vector<Double> spwfreqs=MSColumns(vb.ms()).spectralWindow().chanFreq().get(spw);
  for (uInt k=0; k < chanNums.nelements(); ++k){

    inFreq[k]=spwfreqs[chanNums[k]];
  }

   // The output frequencies
   outFreq.resize(inFreq.nelements());

   MFrequency::Types newMFreqType=freqFrame;
   if (freqFrame==MFrequency::N_Types)
     // Opt out of conversion
     newMFreqType=obsMFreqType;


   // Only convert if the requested frame differs from observed frame
   if(obsMFreqType != newMFreqType){

     // Setting epoch to the first in this iteration
     //     MEpoch ep=vb.msColumns().timeMeas()(0);
     //     MEpoch ep(MVEpoch(vb.time()(0)/86400.0),MEpoch::UTC);
     //     cout << "Time = " << ep.getValue()  << endl;

     // Reset the timestamp (ASSUMES TIME is constant in the VisBuffer)
     mframe_.resetEpoch(vb.time()(0)/86400.0);

     // Reset the direction (ASSUMES phaseCenter is constant in the VisBuffer)
     mframe_.resetDirection(vb.phaseCenter());

     //     cout << "Frame = " << mframe_ << endl;

     // The conversion engine:
     MFrequency::Convert toNewFrame(obsMFreqType, 
				    MFrequency::Ref(newMFreqType, mframe_));

     // Do the conversion
     for (uInt k=0; k< inFreq.nelements(); ++k)
       outFreq(k)=toNewFrame(inFreq(k)).getValue().getValue();
     
   }
   else{
     // The requested frame is the same as the observed frame
     outFreq=inFreq;
   }

   //cerr << std::setprecision(9) << " infreq " << inFreq[152] << "   " << outFreq[152] << " vb freq " << vb.getFrequencies(0, freqFrame)[152] << endl;

 }

 void VisBufferUtil::toVelocity(Vector<Double>& outVel, 
				const VisBuffer& vb, 
				const MFrequency::Types freqFrame,
				const MVFrequency restFreq,
				const MDoppler::Types veldef){

   // The input frequencies (a reference)
   Vector<Double> inFreq(vb.frequency());

   // The output velocities
   outVel.resize(inFreq.nelements());

   // Reset the timestamp (ASSUMES TIME is constant in the VisBuffer)
   mframe_.resetEpoch(vb.time()(0)/86400.0);
   
   // Reset the direction (ASSUMES phaseCenter is constant in the VisBuffer)
   //mframe_.resetDirection(vb.phaseCenter());
   mframe_.resetDirection(vb.msColumns().field().phaseDirMeasCol()(vb.fieldId())(IPosition(1,0)));
 
   // The frequency conversion engine:
   Int spw=vb.spectralWindow();
   MFrequency::Types obsMFreqType=(MFrequency::Types)(vb.msColumns().spectralWindow().measFreqRef()(spw));

   MFrequency::Types newMFreqType=freqFrame;
   if (freqFrame==MFrequency::N_Types)
     // Don't convert frame
     newMFreqType=obsMFreqType;

   MFrequency::Convert toNewFrame(obsMFreqType, 
				  MFrequency::Ref(newMFreqType, mframe_));

   // The velocity conversion engine:
   MDoppler::Ref dum1(MDoppler::RELATIVISTIC);
   MDoppler::Ref dum2(veldef);
   MDoppler::Convert dopConv(dum1, dum2);

   // Cope with unspecified rest freq
   MVFrequency rf=restFreq;
   if (restFreq.getValue()<=0.0)
     rf=toNewFrame(inFreq(vb.nChannel()/2)).getValue();

   // Do the conversions
   for (uInt k=0; k< inFreq.nelements(); ++k){
	 //cerr <<"old freq " << toNewFrame(inFreq(k)).getValue().get().getValue() << endl;
     MDoppler eh = toNewFrame(inFreq(k)).toDoppler(rf);
     MDoppler eh2 = dopConv(eh);
     outVel(k)=eh2.getValue().get().getValue();
   }

 }

 void VisBufferUtil::toVelocity(Vector<Double>& outVel,
 				const vi::VisBuffer2& vb,
 				const MFrequency::Types freqFrame,
 				const MVFrequency restFreq,
 				const MDoppler::Types veldef, const Int row){

	 	 Vector<Double> inFreq;
	 	 inFreq=vb.getFrequencies(row, freqFrame);
	 	// cerr << "Freqs " << inFreq << endl;
	 	// The output velocities
	 	 outVel.resize(inFreq.nelements());
	 	 // The velocity conversion engine:
	 	 MDoppler::Ref dum1(MDoppler::RELATIVISTIC);
	 	 MDoppler::Ref dum2(veldef);
	 	 MDoppler::Convert dopConv(dum1, dum2);

	 	 // Cope with unspecified rest freq
	 	 MVFrequency rf=restFreq;
	 	 if (restFreq.getValue()<=0.0)
	 	      rf=inFreq(inFreq.nelements()/2);

	 	 // Do the conversions
	 	 for (uInt k=0; k< inFreq.nelements(); ++k){
	 		 MDoppler eh = MFrequency(Quantity(inFreq(k), "Hz"), freqFrame).toDoppler(rf);
	 		 MDoppler eh2 = dopConv(eh);
	 		 outVel(k)=eh2.getValue().get().getValue();
	 	 	}


 }
 void VisBufferUtil::toVelocity(Vector<Double>& outVel,
  				const vi::VisBuffer2& vb,
  				const vi::VisibilityIterator2& iter,
  				const MFrequency::Types freqFrame,
  				const MVFrequency restFreq,
  				const MDoppler::Types veldef, const Int row){

 	 // The input frequencies (a reference)
 	 Vector<Double> inFreq(vb.getFrequencies(row));
 	 MSColumns msc(iter.ms());

     MEpoch ep(Quantity(vb.time()(row)/86400.0, "d"), msc.timeMeas()(0).getRef());
 	 MDirection dir(msc.field().phaseDirMeasCol()(vb.fieldId()(row))(IPosition(1,0)));
 	 Int spw=vb.spectralWindows()(row);
 	 MFrequency::Types obsMFreqType=(MFrequency::Types)(msc.spectralWindow().measFreqRef()(spw));
 	 toVelocity(outVel, freqFrame, inFreq, obsMFreqType, ep, dir, restFreq, veldef);
  }
 void VisBufferUtil::toVelocity(Vector<Double>& outVel,
   		  const MFrequency::Types outFreqFrame,
   		  const Vector<Double>& inFreq,
   		  const MFrequency::Types inFreqFrame,
   		  const MEpoch& ep,
   		  const MDirection& dir,
   		  const MVFrequency restFreq,
   		  const MDoppler::Types veldef){



	 // The output velocities
	 outVel.resize(inFreq.nelements());

	 // Reset the timestamp
	 mframe_.resetEpoch(ep);

	 // Reset the direction
	 mframe_.resetDirection(dir);

	 // The frequency conversion engine:

	 MFrequency::Types newMFreqType=outFreqFrame;
	 if (outFreqFrame==MFrequency::N_Types)
		 // Don't convert frame
		 newMFreqType=inFreqFrame;

	 MFrequency::Convert toNewFrame(inFreqFrame,
	 				  MFrequency::Ref(newMFreqType, mframe_));

	 // The velocity conversion engine:
	 MDoppler::Ref dum1(MDoppler::RELATIVISTIC);
	 MDoppler::Ref dum2(veldef);
	 MDoppler::Convert dopConv(dum1, dum2);

	 // Cope with unspecified rest freq
	 MVFrequency rf=restFreq;
	 if (restFreq.getValue()<=0.0)
	      rf=toNewFrame(inFreq(inFreq.nelements()/2)).getValue();

	 // Do the conversions
	 for (uInt k=0; k< inFreq.nelements(); ++k){
		 MDoppler eh = toNewFrame(inFreq(k)).toDoppler(rf);
		 MDoppler eh2 = dopConv(eh);
		 outVel(k)=eh2.getValue().get().getValue();
	 	}




 }

 MDirection VisBufferUtil::getPointingDir(const VisBuffer& vb, const Int antid, const Int vbrow){
	
   Timer tim;
   tim.mark();
   const MSColumns& msc=vb.msColumns();
   //cerr << "oldMSId_p " << oldMSId_p << " vb " <<  vb.msId() << endl;
   if(vb.msId() < 0)
     throw(AipsError("VisBuffer is not attached to an ms so cannot get pointing "));
   if(oldMSId_p != vb.msId()){
     oldMSId_p=vb.msId();
     if(timeAntIndex_p.shape()(0) < (oldMSId_p+1)){
       timeAntIndex_p.resize(oldMSId_p+1, true);
       cachedPointingDir_p.resize(oldMSId_p+1, true);
     }
     if(  timeAntIndex_p[oldMSId_p].empty()){
       Vector<Double> tOrig;
       msc.time().getColumn(tOrig);
       Vector<Double> t;
       rejectConsecutive(tOrig, t);
       Vector<uInt>  uniqIndx;
       uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, t, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
       uInt nAnt=msc.antenna().nrow();
       const MSPointingColumns& mspc=msc.pointing();
       Vector<Double> tUniq(nTimes);
       for (uInt k=0; k <nTimes; ++k){
	 tUniq[k]= t[uniqIndx[k]];
       }
       Bool tstor, timcolstor, intcolstor, antcolstor;
       Double * tuniqptr=tUniq.getStorage(tstor);
       Int cshape=cachedPointingDir_p[oldMSId_p].shape()[0];
       cachedPointingDir_p[oldMSId_p].resize(cshape+nTimes*nAnt, True);
       Vector<Double> timecol;
       Vector<Double> intervalcol;
       Vector<Int> antcol;
       mspc.time().getColumn(timecol, True);
       mspc.interval().getColumn(intervalcol, True);
       mspc.antennaId().getColumn(antcol, True);
       Double *tcolptr=timecol.getStorage(timcolstor);
       Double *intcolptr=intervalcol.getStorage(intcolstor);
       Int * antcolptr=antcol.getStorage(antcolstor);
       Int npointrow=msc.pointing().nrow();
#pragma omp parallel for firstprivate(nTimes, tuniqptr, tcolptr, antcolptr, intcolptr, npointrow), shared(mspc)
       for (uInt a=0; a < nAnt; ++a){
	 
	 //Double wtime1=omp_get_wtime();
	 Vector<ssize_t> indices;
	 Vector<MDirection> theDirs(nTimes);
	 pointingIndex(tcolptr, antcolptr, intcolptr, npointrow, a, nTimes, tuniqptr, indices);
	 
#pragma omp critical
	 {
	   for (uInt k=0; k <nTimes; ++k){
	     
	     
	     // std::ostringstream oss;
	     // oss.precision(13);
	     // oss << tuniqptr[k] << "_" << a;
	     // String key=oss.str();
	     std::pair<double, int> key=make_pair(t[uniqIndx[k]],a);
	     timeAntIndex_p[oldMSId_p][key]=indices[k] > -1 ? cshape : -1;
	     
	     if(indices[k] >-1){
	       
	       cachedPointingDir_p[oldMSId_p][cshape]=mspc.directionMeas(indices[k]);
	       cshape+=1;
	     }
	     
	     
	   }
	 }//end critical
	 
	 
       }
       
       cachedPointingDir_p[oldMSId_p].resize(cshape, True);
     }
     
   }

   /////
   //	 String index=String::toString(vb.time()(vbrow))+String("_")+String::toString(antid);
   // std::ostringstream oss;
   // oss.precision(13);
   // oss << vb.time()(vbrow) << "_" << antid  ;
   // String index=oss.str();
   //cerr << "index "<< index << endl;
   ////////////
   /*
  for (auto it = timeAntIndex_p[oldMSId_p].begin(); it != timeAntIndex_p[oldMSId_p].end();       ++it)
    {
      cerr << (*it).first << " --> " << (*it).second << endl;
    }
   */
   /////////////
   std::pair<double, int> index=make_pair(vb.time()(vbrow), antid);
   Int rowincache=timeAntIndex_p[oldMSId_p][index];
	 ///////TESTOO
	 /* if(rowincache>=0){
	   cerr << "msid " << oldMSId_p << " key "<< index << " index " << rowincache<<  "   "  << cachedPointingDir_p[oldMSId_p][rowincache] << endl;
	   }*/
	 /////////////
	 //tim.show("retrieved cache");
   if(rowincache <0)
     return vb.phaseCenter();
   return cachedPointingDir_p[oldMSId_p][rowincache];

 }
  MDirection VisBufferUtil::getPointingDir(const vi::VisBuffer2& vb, const Int antid, const Int vbrow, const MDirection::Types dirframe, const Bool usePointing){

    //Double wtime0=omp_get_wtime();
	 Int rowincache=-1;
	 if(usePointing){
	   if(oldMSId_p != vb.msId()){
	     MSColumns msc(vb.ms());
	     oldMSId_p=vb.msId();
	     if(timeAntIndex_p.shape()(0) < (oldMSId_p+1)){
	       timeAntIndex_p.resize(oldMSId_p+1, true);
	       cachedPointingDir_p.resize(oldMSId_p+1, true);
	     }
	     MEpoch::Types timeType=MEpoch::castType(msc.timeMeas()(0).getRef().getType());
	     Unit timeUnit(msc.timeMeas().measDesc().getUnits()(0).getName());
	     MPosition pos;
	     String tel;
	     if (vb.subtableColumns().observation().nrow() > 0) {
	       tel =vb.subtableColumns().observation().telescopeName()(msc.observationId()(0));
	     }
	     if (tel.length() == 0 || !tel.contains("VLA") ||
		 !MeasTable::Observatory(pos,tel)) {
	       // unknown observatory, use first antenna
	       Int ant1=vb.antenna1()(0);
	       pos=vb.subtableColumns().antenna().positionMeas()(ant1);
	     }
	     if(  timeAntIndex_p[oldMSId_p].empty()){
	       Vector<Double> tOrig;
	       msc.time().getColumn(tOrig);
	       Vector<Double> t;
	       rejectConsecutive(tOrig, t);
	       Vector<uInt>  uniqIndx;
	       uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, t, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
	       uInt nAnt=msc.antenna().nrow();
			 const MSPointingColumns& mspc=msc.pointing();
			 Vector<Double> tUniq(nTimes);
			 for (uInt k=0; k <nTimes; ++k){
			   tUniq[k]= t[uniqIndx[k]];
			 }
			 Bool tstor, timcolstor, intcolstor, antcolstor;
			 Double * tuniqptr=tUniq.getStorage(tstor);
			 Int cshape=cachedPointingDir_p[oldMSId_p].shape()[0];
			 cachedPointingDir_p[oldMSId_p].resize(cshape+nTimes*nAnt, True);
			 Vector<Double> timecol;
			 Vector<Double> intervalcol;
			 Vector<Int> antcol;
			 mspc.time().getColumn(timecol, True);
			 mspc.interval().getColumn(intervalcol, True);
			 mspc.antennaId().getColumn(antcol, True);
			 Double *tcolptr=timecol.getStorage(timcolstor);
			 Double *intcolptr=intervalcol.getStorage(intcolstor);
			 Int * antcolptr=antcol.getStorage(antcolstor);
			 Int npointrow=vb.ms().pointing().nrow();
			 //ofstream myfile;
			 //myfile.open ("POINTING.txt", ios::trunc);
#pragma omp parallel for firstprivate(nTimes, tuniqptr, tcolptr, antcolptr, intcolptr, npointrow, timeType, timeUnit, pos, dirframe), shared(mspc)
			 for (uInt a=0; a < nAnt; ++a){
			   
			   //Double wtime1=omp_get_wtime();
			   Vector<ssize_t> indices;
			   //Vector<MDirection> theDirs(nTimes);
			   pointingIndex(tcolptr, antcolptr, intcolptr, npointrow, a, nTimes, tuniqptr, indices);
			   
#pragma omp critical
			   {
			     MEpoch timenow(Quantity(tuniqptr[0], timeUnit),timeType);
			     MeasFrame mframe(timenow, pos);
			     MDirection::Convert cvt(MDirection(), MDirection::Ref(dirframe, mframe));
			     for (uInt k=0; k <nTimes; ++k){
			       
			       
			       /*std::ostringstream oss;
			       oss.precision(13);
			       oss << tuniqptr[k] << "_" << a;
			       String key=oss.str();
			       */
			       //			       myfile <<std::setprecision(13) <<  tuniqptr[k] << "_" << a << " index "<<  indices[k] << "\n";
			       pair<double, int> key=make_pair(tuniqptr[k],a);
			       timeAntIndex_p[oldMSId_p][key]=indices[k] > -1 ? cshape : -1;
			       
			       if(indices[k] >-1){
				 timenow=MEpoch(Quantity(tuniqptr[k], timeUnit),timeType);
				 mframe.resetEpoch(timenow);
				 cachedPointingDir_p[oldMSId_p][cshape]=cvt(mspc.directionMeas(indices[k]));
				 
				 cshape+=1;
			       }
			       
			       
			     }
			   }//end critical
			   
			   
			 }
			 cachedPointingDir_p[oldMSId_p].resize(cshape, True);
	     }
	     
	   }

	   /////
	   //	 String index=String::toString(vb.time()(vbrow))+String("_")+String::toString(antid);
	   /* std::ostringstream oss;
	   oss.precision(13);
	   oss << vb.time()(vbrow) << "_" << antid  ;
	   String index=oss.str();
	   */
	   pair<double, int> index=make_pair(vb.time()(vbrow),antid);
	   rowincache=timeAntIndex_p[oldMSId_p].at(index);

	 //tim.show("retrieved cache");
	 }///if usepointing
	 if(rowincache <0)
	   return getPhaseCenter(vb);
	 return cachedPointingDir_p[oldMSId_p][rowincache];



 }
 
  void VisBufferUtil::pointingIndex(Double*& timecol, Int*& antcol, Double*& intervalcol, const rownr_t nrow,  const Int ant, const Int ntimes, Double*& ptime, Vector<ssize_t>& indices){
   
    indices.resize(ntimes);
    
    indices.set(-1);
    
    for(Int pt=0; pt < ntimes; ++pt){
      //cerr << "  " << guessRow ;
     
      /*for (Int k=0; k< 2; ++k){
	Int start=guessRow;
	Int end=nrow;
	if(k==1){
	  start=0;
	  end=guessRow;
	}
      */
      Double nearval=1e99;
      ssize_t nearestIndx=-1;
      ssize_t start=0;
      ssize_t end=nrow;
      for (ssize_t i=start; i<end; i++) {
	if(intervalcol[i]<=0.0 && ant==antcol[i]){
	  if(abs(timecol[i]-ptime[pt]) < nearval){
	    nearestIndx=i;
	    nearval=abs(timecol[i]-ptime[pt]);
	  }
	}
      }
      indices[pt]=nearestIndx;

      
      for (ssize_t i=start; i<end; i++) {
	  if(ant == antcol[i]){
	    Double halfInt=0.0;
	    Bool done=False;
	    if(intervalcol[i]<=0.0){
	    //  if(abs(timecol[inx[i]]-ptime[pt]) < nearval){
	    //	nearestIndx=inx[i];
	    //  }
	      ssize_t counter=0;
	      ssize_t adder=1;
	      done=False;
		//	      while(!( (timecol[i+counter]!=timecol[i]))){
	      while(!done){
		counter=counter+adder;
		if((ssize_t)nrow <= i+counter){
		  adder=-1; 
		  counter=0;
		}
		////Could not find another point (interval is infinite)  hence only 1 valid point
		if( (i+counter) < 0){
		  cerr << "HIT BREAK " << endl;
		  indices[pt]=i;
		  break;
		}
		if( (antcol[i+counter]==ant && timecol[i+counter] != timecol[i]) ){
		  done=True;
		}
	      }
	    
	      //if(ant==12 && abs(timecol[i+counter]-timecol[i]) > 10.0){
	      //cerr << "i " << i << "  counter " << counter << " done " << done << " adder " << adder << "ant count "<< antcol[i+counter] << "  diff " <<   abs(timecol[i+counter]-timecol[i]) << endl;
	      // }
	      halfInt = abs(timecol[i+counter]-timecol[i])/2.0;
	    }
	    else{
	      halfInt = intervalcol[i]/2.0;
	    }
	    if (halfInt>0.0) {
	      
	      if ((timecol[i] >= (ptime[pt] - halfInt)) && (timecol[i] <= (ptime[pt] + halfInt))) {
		////TESTOO
		//if(ant==12){
		//  cerr << "timecol " << timecol[i] << " halfInt " << halfInt << " TEST " << timecol[nearestIndx] << " inx " << i << "   " << nearestIndx << endl;
		//}
		indices[pt]=abs(timecol[i]-ptime[pt]) <  nearval ? i : nearestIndx;
		////////TESTOO
		if(indices[pt] > 4688000){
		  cerr <<  indices[pt] << " timecol " << timecol[i] << " halfInt " << halfInt << " TEST " << timecol[nearestIndx] << "  nearval " << nearval << " inx " << i << "   " << nearestIndx << endl;
		  
		}
		///////////////////
	      break;
	      }
	    } else {
	      // valid for all times (we should also handle interval<0 -> timestamps)
	      cerr << "JUMPY " << i << " ant " << ant << " halfint " << halfInt << " done "<< done <<  endl;
	      indices[pt]=i;
	      break;
	    }

	  }//if ant
	}//start end
	
	//}//k
     
    }//pt
 
    //cerr << "ant " << ant << " indices " << indices << endl;
  }

   MDirection VisBufferUtil::getPhaseCenter(const vi::VisBuffer2& vb, const Double timeo){
     //Timer tim;
	 
     Double timeph = timeo > 0.0 ? timeo : vb.time()(0); 
	 //MDirection outdir;
	 if(oldPCMSId_p != vb.msId()){
	   MSColumns msc(vb.ms());
	   //tim.mark();
	   //cerr << "MSID: "<< oldPCMSId_p <<  "    " << vb.msId() << endl;
	   oldPCMSId_p=vb.msId();
	   if(cachedPhaseCenter_p.shape()(0) < (oldPCMSId_p+1)){
	     cachedPhaseCenter_p.resize(oldPCMSId_p+1, true);
	     cachedPhaseCenter_p[oldPCMSId_p]=map<Double, MDirection>();
	   }
	   if( cachedPhaseCenter_p[oldPCMSId_p].empty()){
		   Vector<Double> tOrig;
		   msc.time().getColumn(tOrig);
		   Vector<Int> fieldId;
		   msc.fieldId().getColumn(fieldId);
		   Vector<Double> t;
		   Vector<Int> origindx;
		   rejectConsecutive(tOrig, t, origindx);
		   Vector<uInt>  uniqIndx;
		   
		   uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, t, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
		   //cerr << "ntimes " << nTimes << "  " << uniqIndx  << "\n  origInx " << origindx << endl;
		   const MSFieldColumns& msfc=msc.field();
		   for (uInt k=0; k <nTimes; ++k){
		     //cerr << t[uniqIndx[k]] << "   " <<  fieldId[origindx[uniqIndx[k]]] << endl;
		     //cerr << msfc.phaseDirMeas(fieldId[origindx[uniqIndx[k]]], t[uniqIndx[k]]) << endl;
		     //cerr << "size " <<  cachedPhaseCenter_p[oldPCMSId_p].size() << endl;
                     String ephemIfAny=msfc.ephemPath(fieldId[origindx[uniqIndx[k]]]);
                     if(ephemIfAny=="" || !Table::isReadable(ephemIfAny, False)){
		       (cachedPhaseCenter_p[oldPCMSId_p])[t[uniqIndx[k]]]=msfc.phaseDirMeas(fieldId[origindx[uniqIndx[k]]], t[uniqIndx[k]]);
                     }
                     else{
                       Vector<MDirection> refDir(msfc.referenceDirMeasCol()(fieldId[origindx[uniqIndx[k]]]));
                       (cachedPhaseCenter_p[oldPCMSId_p])[t[uniqIndx[k]]]=getEphemBasedPhaseDir(vb, ephemIfAny, refDir(0),   t[uniqIndx[k]]);
                     }
		     
		   }
			

	   }
	   //tim.show("After caching all phasecenters");
	 }
	 //tim.mark();
	 MDirection retval;
	 auto it=cachedPhaseCenter_p[oldPCMSId_p].find(timeph);
	 if(it != cachedPhaseCenter_p[oldPCMSId_p].end()){
	   retval=it->second;
	 }
	 else{
	   auto upp= cachedPhaseCenter_p[oldPCMSId_p].upper_bound(timeph);
	   auto low= cachedPhaseCenter_p[oldPCMSId_p].lower_bound(timeph);
	   if (upp==cachedPhaseCenter_p[oldPCMSId_p].begin())
	     retval=(cachedPhaseCenter_p[oldPCMSId_p].begin())->second;
	   else if(low==cachedPhaseCenter_p[oldPCMSId_p].end()){
	     --low;
	     retval=low->second;
	   }
	   else{
	     if(fabs(timeph - (low->first)) < fabs(timeph - (upp->first))){
	       retval=low->second;
	     }
	     else{
	       retval=upp->second;
	     }

	   }
	 }
	 //tim.show("retrieved cache");
	 //cerr << std::setprecision(12) <<"msid " << oldPCMSId_p << " time "<< timeph << " val "  << retval.toString() << endl;
	 
	 return retval;



 }


  MDirection VisBufferUtil::getEphemBasedPhaseDir(const vi::VisBuffer2& vb, const String& ephemPath, const MDirection&refDir,  const Double t){
    MEpoch ep(Quantity(t, "s"), ROMSColumns(vb.ms()).timeMeas()(0).getRef());
    mframe_.resetEpoch(ep);
    if(!Table::isReadable(ephemPath, False))
      return refDir;
    MeasComet mcomet(Path(ephemPath).absoluteName());
    mframe_.set(mcomet);
    MDirection::Ref outref1(MDirection::AZEL, mframe_);
    MDirection tmpazel=MDirection::Convert(MDirection(MDirection::COMET), outref1)();
    MDirection::Types outtype=(MDirection::Types) refDir.getRef().getType();
    MDirection::Ref outref(outtype, mframe_);
    MDirection outdir=MDirection::Convert(tmpazel, outref)();
    MVDirection mvoutdir(outdir.getAngle());
    MVDirection mvrefdir(refDir.getAngle());
    //copying what ROMSFieldColumns::extractDirMeas  does
    mvoutdir.shift(mvrefdir.getAngle(), True);
    return MDirection(mvoutdir, outtype);
  }
  
   MDirection VisBufferUtil::getEphemDir(const vi::VisBuffer2& vb, 
					 const Double timeo){

     Double timeEphem = timeo > 0.0 ? timeo : vb.time()(0); 
     MSColumns msc(vb.ms());
     const MSFieldColumns& msfc=msc.field();
     Int fieldId=vb.fieldId()(0);
     MDirection refDir(Quantity(0, "deg"), Quantity(0, "deg"),(MDirection::Types)msfc.ephemerisDirMeas(fieldId, timeEphem).getRef().getType());
     //Now do the parallax correction
     return getEphemBasedPhaseDir(vb, msfc.ephemPath(fieldId), refDir, timeEphem);


   }
   
 //utility to reject consecutive similar value for sorting
 void VisBufferUtil::rejectConsecutive(const Vector<Double>& t, Vector<Double>& retval){
     uInt n=t.nelements();
     if(n >0){
       retval.resize(n);
       retval[0]=t[0];
     }
     else
       return;
     Int prev=0;
     for (uInt k=1; k < n; ++k){
       if(t[k] != retval(prev)){
    	   ++prev;

    	   retval[prev]=t[k];
       }
     }
     retval.resize(prev+1, true);

   }
void VisBufferUtil::rejectConsecutive(const Vector<Double>& t, Vector<Double>& retval, Vector<Int>& indx){
    uInt n=t.nelements();
    if(n >0){
      retval.resize(n);
      indx.resize(n);
      retval[0]=t[0];
      indx[0]=0;
    }
    else
      return;
    Int prev=0;
    for (uInt k=1; k < n; ++k){ 
      if(t[k] != retval(prev)){
	++prev;
	//retval.resize(prev+1, true);
	retval[prev]=t[k];
	//indx.resize(prev+1, true);
	indx[prev]=k;
      }
    }
    retval.resize(prev+1, true);
    indx.resize(prev+1, true);
    
  }

// helper function to swap the y and z axes of a Cube
 void   VisBufferUtil::swapyz(Cube<Complex>& out, const Cube<Complex>& in)
{
  IPosition inShape=in.shape();
  uInt nxx=inShape(0),nyy=inShape(2),nzz=inShape(1);
  //resize breaks  references...so out better have the right shape 
  //if references is not to be broken
  if(out.nelements()==0)
    out.resize(nxx,nyy,nzz);
  Bool deleteIn,deleteOut;
  const Complex* pin = in.getStorage(deleteIn);
  Complex* pout = out.getStorage(deleteOut);
  uInt i=0, zOffset=0;
  for (uInt iz=0; iz<nzz; ++iz, zOffset+=nxx) {
    Int yOffset=zOffset;
    for (uInt iy=0; iy<nyy; ++iy, yOffset+=nxx*nzz) {
      for (uInt ix=0; ix<nxx; ++ix){ 
	pout[i++] = pin[ix+yOffset];
      }
    }
  }
  out.putStorage(pout,deleteOut);
  in.freeStorage(pin,deleteIn);
}

// helper function to swap the y and z axes of a Cube
void VisBufferUtil::swapyz(Cube<Bool>& out, const Cube<Bool>& in)
{
  IPosition inShape=in.shape();
  uInt nxx=inShape(0),nyy=inShape(2),nzz=inShape(1);
  if(out.nelements()==0)
    out.resize(nxx,nyy,nzz);
  Bool deleteIn,deleteOut;
  const Bool* pin = in.getStorage(deleteIn);
  Bool* pout = out.getStorage(deleteOut);
  uInt i=0, zOffset=0;
  for (uInt iz=0; iz<nzz; iz++, zOffset+=nxx) {
    Int yOffset=zOffset;
    for (uInt iy=0; iy<nyy; iy++, yOffset+=nxx*nzz) {
      for (uInt ix=0; ix<nxx; ix++) pout[i++] = pin[ix+yOffset];
    }
  }
  out.putStorage(pout,deleteOut);
  in.freeStorage(pin,deleteIn);
}

 


} //# NAMESPACE CASA - END

