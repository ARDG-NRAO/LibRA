//# RFATimeFreqCrop.h : This defines RFATimeFreqCrop
//# Copyright (C) 2000,2001
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

#ifndef FLAGGING_RFATIMEFREQCROP_H
#define FLAGGING_RFATIMEFREQCROP_H

#include <flagging/Flagging/RFAFlagCubeBase.h> 
#include <flagging/Flagging/RFDataMapper.h> 
#include <flagging/Flagging/RFFlagCube.h> 
#include <flagging/Flagging/RFRowClipper.h> 
#include <flagging/Flagging/RFABase.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Functionals/Polynomial.h>
#include <scimath/Fitting.h>
#include <scimath/Fitting/LinearFit.h>
#include <scimath/Fitting/GenericL2Fit.h>
#include <scimath/Mathematics/MedianSlider.h> 
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>
// RFATimeFreqCrop : 2-Dimensional casacore::Time Frequency Autoflag agent 
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> RFDataMapper
//   <li> RFFlagCubeBase
//   <li> RFCubeLattice
// </prerequisite>
//
// <synopsis>
// </synopsis>
//
// <todo asof="2002/08/06">
//   <li> 
//   <li> 
//   <li> 
//   // </todo>



//class RFATimeFreqCrop : public RFADiffMapBase
class RFATimeFreqCrop : public RFAFlagCubeBase, public RFDataMapper
{
public:
  RFATimeFreqCrop  ( RFChunkStats &ch,const casacore::RecordInterface &parm );
  virtual ~RFATimeFreqCrop ();

  virtual casacore::Bool newChunk (casacore::Int &i);
  virtual void endChunk ();
  virtual void startData (bool verbose);
  virtual IterMode iterTime (casacore::uInt itime);
  virtual IterMode iterRow  (casacore::uInt irow);
  virtual IterMode endData  ();
  virtual void startFlag (bool verbose);
  virtual void iterFlag(casacore::uInt itime);
  static const casacore::RecordInterface & getDefaults ();
  
private:
  
  casacore::Float UMean(casacore::Vector<casacore::Float> vect, casacore::Vector<casacore::Bool> flag);
  casacore::Float UStd(casacore::Vector<casacore::Float> vect, casacore::Vector<casacore::Bool> flag, casacore::Vector<casacore::Float> fit);
  casacore::Float UStd(casacore::Vector<casacore::Float> vect, casacore::Vector<casacore::Bool> flag, casacore::Float mean);
  void CleanBand(casacore::Vector<casacore::Float> data,casacore::Vector<casacore::Float> fit);
  void PolyFit(casacore::Vector<casacore::Float> data,casacore::Vector<casacore::Bool> flag, casacore::Vector<casacore::Float> fit, casacore::uInt lim1, casacore::uInt lim2,casacore::uInt deg);
  void LineFit(casacore::Vector<casacore::Float> data,casacore::Vector<casacore::Bool> flag, casacore::Vector<casacore::Float> fit, casacore::uInt lim1, casacore::uInt lim2);
  void Ants(casacore::uInt bs, casacore::uInt *a1, casacore::uInt *a2);
  casacore::uInt BaselineIndex(casacore::uInt row, casacore::uInt a1, casacore::uInt a2);

  void Display_ds9(casacore::Int xdim, casacore::Int ydim, casacore::Matrix<casacore::Float> &data, casacore::Int frame);
  void Plot_ds9(casacore::Int dim, casacore::Vector<casacore::Float> data1, casacore::Vector<casacore::Float> data2);

  void AllocateMemory();  
  void FlagZeros();
  void RunTFCrop();
  void FlagTimeSeries(casacore::uInt pl, casacore::uInt bs);
  void FitCleanBandPass(casacore::uInt pl, casacore::uInt bs);
  void FlagBandPass(casacore::uInt pl, casacore::uInt bs);
  void GrowFlags(casacore::uInt pl, casacore::uInt bs);
  RFA::IterMode ShowFlagPlots();
  void ExtendFlags();
  void FillChunkFlags();

  // casacore::uInt itime;  
  VisibilityIterator &vi; 
  VisBuffer &vb;
  // casacore::uInt TimeStamps;
  casacore::String msname;
  casacore::Vector<casacore::Int> ant1,ant2;
  
  // casacore::Cube <casacore::Complex> *vv; 	// visCube ptr - for one timestamp
  casacore::Cube <casacore::Complex> vc; 	// visCube ptr - for one timestamp
  casacore::Cube <casacore::Bool> ff; 	// flagCube ptr - for one timestamp
  casacore::Vector <casacore::Bool> fr;     // rowflag ptr - for one timestamp
  casacore::Cube<casacore::Float> visc; 	// visCube
  casacore::Cube<casacore::Bool> flagc; 	// flagCube
  casacore::Cube<casacore::Float> meanBP; 	// mean bandpass - npol x nbaselines x nchannels
  casacore::Cube<casacore::Float> cleanBP; 	// cleaned bandpasses - npol x nbaselines x nchannels
  casacore::Vector<casacore::Bool> rowflags; // rowflags from the casacore::MS - ntime x nbaselines
  casacore::Vector<casacore::Bool> baselineflags; // flags for baselines - NumB (for internal use)

  casacore::Cube<casacore::Bool> chunkflags; // Flags for the entire chunk.
  
  casacore::Vector<casacore::Float> tempBP; // temporary workspace - nchannels
  casacore::Vector<casacore::Float> tempTS; // temporary workspace - ntimes
  casacore::Vector<casacore::Bool> flagBP; 	// temporary list of flags - nchannels
  casacore::Vector<casacore::Bool> flagTS; 	// temporary list of flags - ntimes
  casacore::Vector<casacore::Float> fitBP; 	// temporary fit array - nchannels
  casacore::Vector<casacore::Float> fitTS; 	// temporary fit array - ntimes
 
  casacore::Float T_TOL,F_TOL,ANT_TOL,BASELN_TOL; // casacore::Input parameters
  casacore::uInt FlagLevel,CorrChoice,NumTime,MaxNPieces;//casacore::Input parameters
  casacore::Bool ShowPlots,DryRun,IgnorePreflags;
  // casacore::uInt nPol;
  casacore::Bool FreqLineFit;
  casacore::Bool StopAndExit;
  casacore::String Column;
  casacore::Vector<casacore::String> Expr;
 
  casacore::Matrix<casacore::Bool> RowFlags; // Row Flags
  
  casacore::IPosition cubepos,matpos; 

  // casacore::uInt passcnt;
  // casacore::uInt iterRowcnt;
  casacore::uInt iterTimecnt;
  casacore::uInt timecnt;

  casacore::uInt NumT; // Number of timestamps in one block
  casacore::uInt NumB; // Number of baselines.
  casacore::uInt NumAnt; // Number of antennas.
  casacore::uInt NumC; // Number of channels
  casacore::uInt NumP; // Number of polarizations
 
  FlagCubeIterator flag_iter; // Local Flag Iterator
};

    

} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <flagging/Flagging/RFATimeFreqCrop.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
