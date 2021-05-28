//# Importmiriad: miriad dataset to casacore::MeasurementSet conversion
//# Copyright (C) 1997,2000,2001,2002,2013,2015
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify
//# it under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or
//# (at your option) any later version.
//#
//# This program is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//# 
//# You should have received a copy of the GNU General Public License
//# along with this program; if not, write to the Free Software
//# Foundation, Inc., 675 Masve, Cambridge, MA 02139, USA.
//#

#ifndef MIRIAD_IMPORTMIRIAD_H
#define MIRIAD_IMPORTMIRIAD_H

//#Includes

#include <casa/Inputs/Input.h>
#include <casa/OS/File.h>
#include <casa/Utilities/GenSort.h>

#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayUtil.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/MatrixMath.h>

#include <measures/Measures.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasData.h>
#include <measures/Measures/Stokes.h>

#include <tables/Tables.h>
#include <tables/Tables/TableInfo.h>

#include <ms/MeasurementSets.h> 

#include <mirlib/maxdimc.h>
#include <mirlib/miriad.h>

#include <casa/namespace.h>
namespace casa { //# NAMESPACE CASA - BEGIN


// MIRIAD dataset casacore::MeasurementSet filler. Derived from carmafiller.
//
// Despite that this program is written in C++ and uses classes, it is
// really a set of routines manipulating a huge common block.....
//
// Acknowledgement: this program was originally cloned off uvfitsfiller.cc
//                  whose code is now in MSFitsInput.cc
//                  Ray Plante wrote the other filler, with which we had
//                  some cross talk
// 
//
// Limitations, Caveats and Remaining ToDo's
//    - does not apply the various (miriad) complex gain corrections
//      (uvcat and friends should be used for that in miriad, it would be
//      silly to duplicate that code here)
//    - this code won't run if casacore::Double!=double or casacore::Float!=float or casacore::Int!=int
//      On some future 64bit machines this may cause problems?
//    - CARMA type arrays when dishes are not the same size
//    - do not mix arrays from different telescopes (e.g. BIMA and ATCA)
//      but it should handle different configurations (ARRAYs) of the same
//      instrument
//    - handle multiple array configuration datasets that were UVCAT'ed
//      (needs to count antennae up at each array configuration)
//    - true miriad storage manager ?
//
//    - check UV override, is that being handled transparently?
//
//    - spectral windows layout  (code present, fix table access descriptors) 
//      CHECK: what happens if e.g. uvcat does some line= preprocessing
//    - correct restfreq's in source tables that relate to the spectral windows
//      (this code currently coredumps)
//    - make win=,narrow=,wide=0 cause non of them to be written in that selection
//    - add weather table
//    - although no more wides are written, there is a way to make a small MS
//      from the wides only by preprocessing in miriad, but conceivably
//      could be done here as well??  -- except the narrow= keyword seems to write
//      files that suffer from the casacore::Table array conformance error
//
//      Todo: to deal with multiple zoom setups (freq changes), we need
//      to track which spectral windows each source appears to make a proper
//      source table indexed by source id and spw id


//  History:
//   Spring 1997:   written (cloned off uvfitsfiller)          Peter Teuben
//   July 1997:     Y2K, fixed table-interface                          PJT
//   Dec 1997:      fixed wideband only data (e.g. uvgen)               PJT
//   ???            somebody fixed up this code for some new release    ???
//   May 2000:      fixed up for various new AIPS++ conventions         PJT
//                  and added multi-source & field                      PJT
//   Sep 2000:      development now on linux, converted to OldMS        PJT
//   Dec 2000:      casacore::Conversion to casacore::MS (MS2)                              PJT
//                  typical compile time: (P600/256M/15MBps HD: 36")
//                  typical 3c273 conversion time:  2.6" (5.3->11.3 MB)
//                  Cf. that to "uvio" processing time, which runs at disk I/O
//                  speed (15 MB/s on the beforementioned laptop)
//   Jan-Feb 2001:  window layout, restfreq's and various things to get 
//                  msmultiscale to work; added some syscal support     PJT
//   April 2001:    final cleanup for the 1.5 AIPS++ release -          PJT
//                  [note that from this moment on bimafiller is for
//                   experimental use and deprecated, the new version
//                   will be called 'mirfiller'
//   Oct 2001:      mirlib changed location
//   Sep 2009:      revived in CASA, added various tables for CARMA
//                  also renamed from bimafiller to carmafiller
//   Sep 2011:      initial release
//   Mar 2013:      Rename to importmiriad and make it work for ATCA 
//                  CABB data (more channels, windows and 4 pols)       MHW
//   May 2014:      Fix some compiler warnings, add TOPO option         MHW
//   Jul 2015:      Cope with data with changing freq setups,e.g.,
//                  CABB zoom data with multiple sources                MHW
//   Jan 2016       Turn from standalone app into tool+task             MHW

// a placeholder for the MIRIAD spectral window configuration
// see also the MIRIAD programmers guide, and its appendix on UV Variables 
// for CARMA we have MAXWIN = MAXWIDE (16 normally, 6 in 2006-2009, more later)
// narrow band (plus MAXWIDE space to point into the wide band data too)

typedef struct window {     // CASA defines everything mid-band mid-interval
  int    nspect;                   // number of valid windows (<=MAXWIN, typically 16)
  int    nschan[MAXWIN+MAXWIDE];   // number of channels in a window
  int    ischan[MAXWIN+MAXWIDE];   // starting channel of a window (1-based)
  double sdf[MAXWIN+MAXWIDE];      // channel separation 
  double sfreq[MAXWIN+MAXWIDE];    // frequency of first channel in window (doppler changes)
  double restfreq[MAXWIN+MAXWIDE]; // rest freq, if appropriate
  char   code[MAXWIN+MAXWIDE];     // code to CASA identification (N, W or S; S not used anymore)

  // wide band (for CARMA these are the spectral window averages - i.e. nspect=nwide)
  int    nwide;                    // number of wide band channels
  float  wfreq[MAXWIDE];           // freq
  float  wwidth[MAXWIDE];          // width
  // ifchain for ATCA
  int    chain[MAXWIN+MAXWIDE];    // if conversion chain


} WINDOW;

//  the maximum number of fields in mosaicing observations, careful, miriad
//  often has a smaller value, like 64 or 128.
#ifndef MAXFIELD
# define MAXFIELD  10000
#endif

#ifndef MAXMSG
# define MAXMSG 256
#endif

#ifndef MAXFSET
# define MAXFSET 100
#endif

// a helper class 

class Importmiriad
{
  // This is an implementation helper class used to store 'local' data
  // during the filling process.
public:
  // Create from a miriad dataset (a directory)
  Importmiriad(casacore::String& infile, casacore::Int debug=0, 
              casacore::Bool Qtsys=false,
              casacore::Bool Qarrays=false,
              casacore::Bool Qlinecal=false);

  // Standard destructor
  ~Importmiriad();
  
  // Check some of the contents of the data and header read
  void checkInput(casacore::Block<casacore::Int>& spw, casacore::Block<casacore::Int>& wide);

  // Debug output level
  casacore::Bool Debug(int level);

  // Set up the casacore::MeasurementSet, including StorageManagers and fixed columns.
  // If useTSM is true, the Tiled Storage Manager will be used to store
  // DATA, FLAG and WEIGHT_SPECTRUM
  void setupMeasurementSet(const casacore::String& MSFileName, casacore::Bool useTSM=true);

  // Fill the main table by reading in all the visibilities
  void fillMSMainTable();

  // Make an Antenna casacore::Table (can be called incrementally now)
  void fillAntennaTable();

  // Make a Syscal casacore::Table (can be called incrementally)
  void fillSyscalTable();

  // fill Spectralwindow table 
  void fillSpectralWindowTable(casacore::String vel);

  // fill Field table 
  void fillFieldTable();

  // fill Source table 
  void fillSourceTable();

  // fill the Feed table with minimal info needed for synthesis processing
  void fillFeedTable();

  // Fill the Observation and History (formerly ObsLog) tables
  void fillObsTables();

  // fix up the EPOCH MEASURE_REFERENCE keywords using the value found
  // in the (last) AN table - check if miriad really needs it
  void fixEpochReferences();

  void Tracking(int record);
  void check_window();
  casacore::Bool compareWindows(WINDOW& w1, WINDOW& w2);
  void Error(char *msg);
  void Warning(char *msg);
  void show();
  void close();

private:
  casacore::String                 infile_p;     // filename
  casacore::Int                    uv_handle_p;  // miriad handle 
  casacore::MeasurementSet         ms_p;         // the casacore::MS itself
  casacore::MSColumns             *msc_p;        // handy pointer to the columns in an casacore::MS  
  casacore::Int                    debug_p;      // debug level
  casacore::String                 array_p, 
                         project_p, 
                         object_p, 
                         telescope_p, 
                         observer_p, 
                         version_p,
                         timsys_p;
  casacore::Vector<casacore::Int>            nPixel_p, corrType_p, corrIndex_p;
  casacore::Matrix<casacore::Int>            corrProduct_p;
  casacore::Double                 epoch_p;
  casacore::MDirection::Types      epochRef_p;
  casacore::Int                    nArray_p;      // number of arrays (nAnt_p.nelements())
  casacore::Block<casacore::Int>             nAnt_p;        // number of antennas per array
  casacore::Block<casacore::Vector<casacore::Double> > receptorAngle_p;
  casacore::Vector<casacore::Double>         arrayXYZ_p;    // needs to be made with 3 elements
  casacore::Vector<casacore::Double>         ras_p, decs_p; // ra/dec for source list (source_p)
  casacore::Vector<casacore::String>         source_p,      // list of source names (?? object_p ??)
                         purpose_p;     // purpose of this source 
  casacore::LogIO                  os_p;          // logger
  casacore::Vector<casacore::Bool>   keep_p; // keep this window for output to MS


  // the following variables are for miriad, hence not casacore::Double/casacore::Int/Float
  // thus the code may have to be fixed on machines where these do not
  // agree ... may need special access code to get those into CASA
  // types on 64 bit machines??

  double preamble[5] /*, first_time*/;
  int    ifield, nfield, npoint /*, nsource*/;     // both dra/ddec should become casacore::Vector's
  float  dra[MAXFIELD], ddec[MAXFIELD];       // offset in radians
  double ra[MAXFIELD], dec[MAXFIELD];
  int    field[MAXFIELD];                     // source index
  int    fcount[MAXFIELD];
  float  dra_p=0, ddec_p=0;
  int    pol_p[4];
  // char   message[MAXMSG];


  // The following items more or less follow the uv variables in a dataset
  casacore::Int    nants_p, nants_offset_p, nchan_p, nwide_p, npol_p;
  casacore::Double antpos[3*MAXANT];
  casacore::Float  phasem1[MAXANT];
  casacore::Double ra_p, dec_p;       // current pointing center RA,DEC at EPOCH 
  casacore::Float  inttime_p, jyperk_p;
  casacore::Double freq_p;            // rest frequency of the primary line
  casacore::Int    mount_p;
  casacore::Double time_p;            // current MJD time
  casacore::Double timeFirst_p;       // First MJD time encountered

  // MIRIAD spectral window definition
  casacore::Int    freqSet_p,nFreqSet_p,ddid_p;
  WINDOW win[MAXFSET];  // allow for 16 different frequency setups
  
  casacore::Bool   Qtsys_p;    /* tsys weight's */
  casacore::Bool   Qarrays_p;  /* write separate arrays */
  casacore::Bool   Qlinecal_p; /* do linecal */

  // casacore::Data buffers.... again in MIRIAD format
  
  float  data[2*MAXCHAN], wdata[2*MAXCHAN];     // 2*MAXCHAN since (Re,Im) pairs complex numbers
  int    flags[MAXCHAN], wflags[MAXCHAN];
  float  systemp[MAXANT*MAXWIDE];
  int    zero_tsys;

  // Counters
  int    nvis;
};
} // Namespace CASA - END

#endif
