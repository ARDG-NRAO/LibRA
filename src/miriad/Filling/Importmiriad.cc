//# Importmiriad: miriad dataset to MeasurementSet conversion
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

//#Includes
#include <miriad/Filling/Importmiriad.h>

#include <casacore/casa/Inputs/Input.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Utilities/GenSort.h>

#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayUtil.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/MatrixMath.h>


#include <casacore/measures/Measures.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/measures/Measures/MeasData.h>
#include <casacore/measures/Measures/Stokes.h>

#include <casacore/tables/Tables.h>
#include <casacore/tables/Tables/TableInfo.h>

#include <casacore/ms/MeasurementSets.h> 

#include <casacore/mirlib/maxdimc.h>
#include <casacore/mirlib/miriad.h>
#include <sstream>

using namespace casa;

// helper functions

String show_version_info()
{
  return "============================================================\n"
   "Importmiriad - last few updates:\n"
   " Mar 2013 - make it process ATCA/CABB data \n"
   " Jul 2015 - deal with multiple zoom setups \n"
   "           ...\n"
   "============================================================\n";
}


// Convert fits date string of form dd/mm/yy to mjd seconds

Double date2mjd(const String& date)
{
  Int day,month,year;
  
  if (date[2] == '/') {     // old FITS style (dd/mm/yy)
    sscanf(date.chars(),"%2d/%2d/%2d",&day,&month,&year);
    year+=1900;
    if (year<1950) year+=100;   // yuck !
  } else {      // YEAR-2000 (ISO) convention (ccyy-mm-ddThh:mm:ss.sss)
    // cerr<< "Parsing new Year-2000 notation" << endl;
    sscanf(date.chars(),"%4d-%2d-%2d",&year,&month,&day);
    //sscanf(date,"%4d-%2d-%2dT%2d:%2d:%f",&year,&month,&day,&hour,&min,&sec);
  }
  MVTime mjd_date(year,month,(Double)day);
  return mjd_date.second();
}

// apply CARMA line calibration, the 'linecal' method
// check MIRIADs 'uvcal options=linecal' for the other approach

void linecal(int ndata, float *data, float phi1, float phi2)
{
  float x,y,c,s;
  if (ndata <= 0) return;

  c = cos(phi1-phi2);
  s = sin(phi1-phi2);

  for (int i=0; i<ndata*2; i+=2) {
    x = data[i];
    y = data[i+1];
    data[i]   =  c*x - s*y;
    data[i+1] =  s*x + c*y;
  }
}

// ==============================================================================================
Importmiriad::Importmiriad(String& infile, Int debug, 
                         Bool Qtsys, Bool Qarrays, Bool Qlinecal)
{
  infile_p = infile;
  debug_p = debug;
  msc_p = 0;
  nArray_p = 0;
  nFreqSet_p = 0;
  nfield = 0;           //  # mosaiced fields (using offsets?)
  npoint = 0;           //  # pointings (using independant RA/DEC?)
  Qtsys_p = Qtsys;
  Qarrays_p = Qarrays;
  Qlinecal_p = Qlinecal;
  os_p = LogOrigin("Importmiriad");
  zero_tsys = 0;
  for (int i=0; i<MAXFIELD; i++) fcount[i] = 0;
  for (int i=0; i<MAXANT;   i++) phasem1[i] = 0.0;

  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::Importmiriad debug_level=" << debug << LogIO::POST;
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Opening miriad dataset " << infile_p << LogIO::POST;
  if (Debug(1)) os_p << LogIO::DEBUG1 << (Qtsys_p ?  "tsys weights" : "weights=1") << LogIO::POST;
  if (Debug(1)) os_p << LogIO::DEBUG1 << (Qarrays_p ? "split arrays" : "single array forced") << LogIO::POST;
  if (Debug(1)) os_p << LogIO::DEBUG1 << (Qlinecal_p ? "linecal applied" : "linecal not applied") << LogIO::POST;

  if (sizeof(double) != sizeof(Double))
    os_p<< LogIO::SEVERE<<"Double != double; importmiriad will probably fail" << LogIO::POST;
  if (sizeof(int) != sizeof(Int))
    os_p << LogIO::SEVERE<<"int != Int; importmiriad will probably fail" << LogIO::POST;

  // open miriad dataset
  uvopen_c(&uv_handle_p, infile_p.chars(), "old");

  // preamble data must be UVW (default miriad is UV)
  uvset_c(uv_handle_p,"preamble","uvw/time/baseline",0,0.0,0.0,0.0);

  // initialize those UV variables that need to be tracked
  Tracking(-1);      
}

// ============================================================================================== 
Importmiriad::~Importmiriad() 
{
  if (msc_p) { delete msc_p; msc_p=0;}
  if (Debug(1)) os_p <<LogIO::DEBUG1<< "Importmiriad::~Importmiriad" << LogIO::POST;
  if (zero_tsys)
    os_p << "There were " << zero_tsys << " record with no WEIGHT due to zero TSYS" << LogIO::POST;

  if (Debug(1))
    for (int i=0; i<nfield; i++)
      os_p << LogIO::DEBUG1 << "Field " << i << " = " << fcount[i] << " records" << LogIO::POST;

  // most single MIRIAD files are time ordered, so could check for 
  // that, and if so, add SORT_ORDER = 'ASCENDING' and COLUMNS = 'TIME'

  if (Debug(1)) os_p << LogIO::DEBUG1 << "*** Closing " << infile_p << " ***"<<LogIO::POST;
  //os_p << "Importmiriad::END" << LogIO::POST;
  if (Debug(1)) os_p<<LogIO::DEBUG1 << show_version_info()<<LogIO::POST;
}

// ==============================================================================================
void Importmiriad::Error(char *msg)
{
  throw(AipsError(msg));
}

// ==============================================================================================
void Importmiriad::Warning(char *msg)
{
  os_p << LogIO::WARN<< "### Warning: " << msg <<  LogIO::POST;
}

// ==============================================================================================
Bool Importmiriad::Debug(int level)
{
  Bool ok=false;
  if (level <= debug_p) ok=true;
  return ok;
}

// ==============================================================================================
void Importmiriad::checkInput(Block<Int>& spw, Block<Int>& wide)
{
  Int i, nread, nwread, vlen, vupd;
  char vtype[10], vdata[256];
  Float epoch;

  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::checkInput" << LogIO::POST;

  // Let's read one scan and try and derive some basics. If important
  // variables not present, bail out (or else scan on)

  nvis = 0;
  for (;;) {   // loop forever until happy or EOF

    uvread_c(uv_handle_p, preamble, data, flags, MAXCHAN, &nread);    
    if (nread <= 0) break;
    nvis++;
    uvwread_c(uv_handle_p, wdata, wflags, MAXCHAN, &nwread);

    if (nvis == 1) {
    
      // get the initial correllator setup
      check_window();
      // setup the 'keep' array to specify which data we want to keep
      keep_p.resize(MAXWIN+MAXWIDE); keep_p.set(False);
      if (spw.nelements()>0) {
        for (Int i=0; i<MAXWIN+MAXWIDE; i++) keep_p[i]=(spw[0]==-1);
        for (uInt i=0; i<spw.nelements(); i++) {
          if (spw[i]>=0 && spw[i]<win[0].nspect) keep_p[spw[i]]=true;
        }
      }
      Int n=win[0].nspect;
      if (wide.nelements()>0) {
        for (Int i=0; i<win[0].nwide; i++) keep_p[n+i]=(wide[0]==-1);
        for (uInt i=0; i<wide.nelements(); i++) {
          if (wide[i]>0 && wide[i]<win[0].nwide) keep_p[n+wide[i]]=true;
        }
      }
      //  should store nread + nwread, or handle it as option
      if (win[freqSet_p].nspect > 0) {    // narrow band, with possibly wide band also
        nchan_p = nread;
        nwide_p = nwread;
      } else {                            // wide band data only: nread=nwread
        nchan_p = nread;
        nwide_p = 0;
      }

      // get the initial array configuration

      nants_offset_p = 0;
      uvgetvr_c(uv_handle_p,H_INT, "nants", (char *)&nants_p,1);
      if (nants_p > MAXANT) {
	ostringstream  msg;
	msg << "Importmiriad: Too many antennas, "<< nants_p << " > "<< MAXANT<<endl;
    	throw(AipsError(msg.str()));
	return;
      }

      uvgetvr_c(uv_handle_p,H_DBLE,"antpos",(char *)antpos,3*nants_p);
      if (Debug(1)) {
        os_p << LogIO::DEBUG1 << "Found " << nants_p << " antennas (first scan)" << LogIO::POST;
        for (int i=0; i<nants_p; i++) {
          os_p << LogIO::DEBUG1 << antpos[i] << " " << 
            antpos[i+nants_p] << " " << 
            antpos[i+nants_p*2] << LogIO::POST;
        }
      }
    
      // remember systemp is stored systemp[nants][nwin] in C notation
      if (win[freqSet_p].nspect > 0) {
        uvgetvr_c(uv_handle_p,H_REAL,"systemp",(char *)systemp,nants_p*win[freqSet_p].nspect);
        if (Debug(1)) {
          os_p << LogIO::DEBUG1 << "Found systemps (first scan)" ;
          for (Int i=0; i<nants_p; i++)  os_p << systemp[i] << " ";
          os_p << LogIO::POST;
        }
      } else {
        uvgetvr_c(uv_handle_p,H_REAL,"wsystemp",(char *)systemp,nants_p);
        if (Debug(1)) {
          os_p << LogIO::DEBUG1 << "Found wsystemps (first scan)" ;
          for (Int i=0; i<nants_p; i++)  os_p << systemp[i] << " ";
          os_p << LogIO::POST;
        }
      }

      if (win[freqSet_p].nspect > 0) {
        uvgetvr_c(uv_handle_p,H_DBLE,"restfreq",(char *)win[freqSet_p].restfreq,win[freqSet_p].nspect);
        if (Debug(1)) {
          os_p << LogIO::DEBUG1 << "Found restfreq (first scan)" ;
          for (Int i=0; i<win[freqSet_p].nspect; i++)  os_p << win[freqSet_p].restfreq[i] << " ";
          os_p << LogIO::POST;
        }
      }

      // Note that MIRIAD coordinates are in nanosec, but actual unused
      // antennas are filled with -999 values (or sometimes 0!)

      uvprobvr_c(uv_handle_p,"project",vtype,&vlen,&vupd);
      if (vupd) {
        uvgetvr_c(uv_handle_p,H_BYTE,"project",vdata,32);
        project_p = vdata;
      } else
        project_p = "unknown";
      if (Debug(1)) os_p << LogIO::DEBUG1 << "Project=>" << project_p << "<=" << LogIO::POST;

      uvprobvr_c(uv_handle_p,"version",vtype,&vlen,&vupd);
      if (vupd) {
        uvgetvr_c(uv_handle_p,H_BYTE,"version",vdata,80);
        version_p = vdata;
        if (Debug(1)) os_p << LogIO::DEBUG1 << "Version=>" << version_p << "<=" << LogIO::POST;
      }
      uvgetvr_c(uv_handle_p,H_BYTE,"source",vdata,16);
      object_p = vdata;
      
      // TODO: telescope will now change, so this is not a good idea
      uvgetvr_c(uv_handle_p,H_BYTE,"telescop",vdata,16);
      array_p = vdata;

      // array_p = "CARMA";
      if (Debug(1)) os_p << LogIO::DEBUG1 << "First baseline=>" << array_p << "<=" << LogIO::POST;
      
      // All CARMA (OVRO,BIMA,SZA) & ATCA have this 
      mount_p = 0;
#if 0
        if (array_p == "VLA")
          mount_p = 1;
        uvrdvr_c(uv_handle_p,H_INT,"mount",(char *)&mount_p, (char *)&mount_p, 1);
        os_p << "Warning: " << array_p 
             << " Cannot handle all of this telescope yet" << LogIO::POST;
        os_p << "Assumed mount=" << mount_p << LogIO::POST;
#endif
      
      uvprobvr_c(uv_handle_p,"observer",vtype,&vlen,&vupd);
      if (vupd) {
        uvgetvr_c(uv_handle_p,H_BYTE,"observer",vdata,16);
        observer_p = vdata;
      } else              
        observer_p = "unknown";    
      
      uvgetvr_c(uv_handle_p,H_REAL,"epoch",(char *)&epoch,1);
      epoch_p = epoch;
      // do this globally, we used to do this in the Field table alone
      epochRef_p=MDirection::J2000;      
      if (nearAbs(epoch_p,1950.0,0.01)) epochRef_p=MDirection::B1950;   

      uvgetvr_c(uv_handle_p,H_INT,"npol", (char *)&npol_p,1);
      uvgetvr_c(uv_handle_p,H_INT,"pol",(char *)&pol_p,1);
      uvgetvr_c(uv_handle_p,H_REAL,"inttime",(char *)&inttime_p,1);
      uvgetvr_c(uv_handle_p,H_REAL,"jyperk",(char *)&jyperk_p,1);
      
      uvprobvr_c(uv_handle_p,"freq",vtype,&vlen,&vupd);
      freq_p = 1e9;
      if (vupd) {
        uvgetvr_c(uv_handle_p,H_DBLE,"freq",(char *)&freq_p,1);
        freq_p *= 1e9;
      }

      uvprobvr_c(uv_handle_p,"ifchain",vtype,&vlen,&vupd);
      if(!vupd) {
        for (i=0; i<MAXWIN+MAXWIDE;i++) win[freqSet_p].chain[i]=0;
      }
     // and initial source position

      uvgetvr_c(uv_handle_p,H_DBLE,"ra", (char *)&ra_p, 1);
      uvgetvr_c(uv_handle_p,H_DBLE,"dec",(char *)&dec_p,1);
      
      // check if certain calibration tables are present and warn if so,
      // since we can't (don't want to) deal with them here; miriad
      // programs like uvcat should be used to apply them!

      if (hexists_c(uv_handle_p,"gains")) 
        os_p << LogIO::WARN << "gains table present, but cannot apply it" << LogIO::POST;
      if (hexists_c(uv_handle_p,"bandpass")) 
        os_p << LogIO::WARN << "bandpass table present, but cannot apply it" << LogIO::POST;
      if (hexists_c(uv_handle_p,"leakage")) 
        os_p << LogIO::WARN << "leakage table present, but cannot apply it" << LogIO::POST;
      
      
      if (npol_p > 1) {     // read the next npol-1 scans to find the other pols
        for (i=1; i<npol_p; i++) {
          uvread_c(uv_handle_p, preamble, data, flags, MAXCHAN, &nread);
          if (nread <= 0) {
            break;
          }
          if (Debug(1)) { if (i==1) os_p << LogIO::DEBUG1 << "POL(" << i << ") = " << pol_p[0] << LogIO::POST;}
          uvgetvr_c(uv_handle_p,H_INT,"pol",(char *)&pol_p[i],1);        // FIX
          if (Debug(1)) os_p << LogIO::DEBUG1 << "POL(" << i+1 << ") = " << pol_p[i] << LogIO::POST;
        }
      }
      // only do one scan
      break;
    }
  }
  if (nvis == 0) {
    throw(AipsError("Importmiriad: Bad first uvread: no narrow or wide band data present"));
    return;
  }
  //os_p << "Importmiriad::checkInput: " << nvis << " records found" << LogIO::POST;
  //os_p << "Found " << nvis << " records" << LogIO::POST;
  uvrewind_c(uv_handle_p);

  Int numCorr = npol_p;
  corrType_p.resize(numCorr); 
  for (i=0; i < numCorr; i++) {
    // note: 1-based ref pix
    corrType_p(i)=pol_p[i];
    // convert AIPS-convention Stokes description to CASA enum
    // CHECK if these are really the right conversions for CASA
    if (corrType_p(i)<0) {
      if (corrType_p(i)==-8) corrType_p(i)=Stokes::YX;
      if (corrType_p(i)==-7) corrType_p(i)=Stokes::XY;
      if (corrType_p(i)==-6) corrType_p(i)=Stokes::YY;
      if (corrType_p(i)==-5) corrType_p(i)=Stokes::XX;
      if (corrType_p(i)==-4) corrType_p(i)=Stokes::LR;
      if (corrType_p(i)==-3) corrType_p(i)=Stokes::RL;
      if (corrType_p(i)==-2) corrType_p(i)=Stokes::LL;
      if (corrType_p(i)==-1) corrType_p(i)=Stokes::RR;
    }
  }

  Vector<Int> tmp(numCorr); tmp=corrType_p;
  // Sort the polarizations to standard order
  GenSort<Int>::sort(corrType_p);
  corrIndex_p.resize(numCorr);
  // Get the sort indices to rearrange the data to standard order
  for (i=0;i<numCorr;i++) {
    for (Int j=0;j<numCorr;j++) {
      if (corrType_p(j)==tmp(i)) corrIndex_p(i)=j;
    }
  }

  // Figure out the correlation products from the polarizations
  corrProduct_p.resize(2,numCorr); corrProduct_p=0;
  for (i=0; i<numCorr; i++) {
    Fallible<Int> receptor=Stokes::receptor1(Stokes::type(corrType_p(i)));
    if (receptor.isValid()) corrProduct_p(0,i)=receptor;
    receptor=Stokes::receptor2(Stokes::type(corrType_p(i)));
    if (receptor.isValid()) corrProduct_p(1,i)=receptor;
  }
}

// ==============================================================================================
void Importmiriad::setupMeasurementSet(const String& MSFileName, Bool useTSM)
{
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::setupMeasurementSet" << LogIO::POST;

  Int nCorr =  (array_p=="CARMA" ? 1 : npol_p); // # stokes (1 for CARMA for now)
  Int nChan = nchan_p;  // we are only exporting the narrow channels to the MS

  // Make the MS table
  TableDesc td = MS::requiredTableDesc();

  MS::addColumnToDesc(td, MS::DATA,2);
  td.removeColumn(MS::columnName(MS::FLAG));
  MS::addColumnToDesc(td, MS::FLAG,2);

#if 0
  // why does the FITS code do this? We don't need it....
  td.removeColumn(MS::columnName(MS::SIGMA));
  MS::addColumnToDesc(td, MS::SIGMA, IPosition(1,nCorr), 
                      ColumnDesc::Direct);
#endif


  // #define OLD_CODE     // define this if you want to try the old method again

#ifdef OLD_CODE
  // OLD
  if (useTSM) {
    td.defineHypercolumn("TiledData",3,
                         stringToVector(MS::columnName(MS::DATA)+","+
                                        MS::columnName(MS::FLAG)));
  }
#else
  // NEW
  if (useTSM) {    
    td.defineHypercolumn("TiledData",3,
                         stringToVector(MS::columnName(MS::DATA)));
    td.defineHypercolumn("TiledFlag",3,
                         stringToVector(MS::columnName(MS::FLAG)));
    td.defineHypercolumn("TiledUVW",2,
                         stringToVector(MS::columnName(MS::UVW)));
  }
#endif

  if (Debug(1))  os_p << LogIO::DEBUG1 << "Creating MS=" << MSFileName  << LogIO::POST;
  SetupNewTable newtab(MSFileName, td, Table::New);
  
  // Set the default Storage Manager to be the Incr one
  IncrementalStMan incrStMan ("ISMData");
  newtab.bindAll(incrStMan, true);
  StandardStMan aipsStMan; 


#ifdef OLD_CODE
  // ORIGINAL CODE
  newtab.bindColumn(MS::columnName(MS::ANTENNA2), aipsStMan);
  if (useTSM) {
    // choose a tile size in the channel direction that is <=10
    Int tileSize=(nChan+nChan/10)/(nChan/10+1);
    // make the tile about 32k big
    TiledColumnStMan tiledStMan1("TiledData",
                                 IPosition(3,nCorr,tileSize,
                                           2000/nCorr/tileSize));
    TiledColumnStMan tiledStMan2("TiledWeight",
                                 IPosition(2,tileSize,
                                           8000/tileSize));
    // Bind the DATA and FLAG columns to the tiled stman
    newtab.bindColumn(MS::columnName(MS::DATA),tiledStMan1);
    newtab.bindColumn(MS::columnName(MS::FLAG),tiledStMan1);
  }
  // Change some to aipsStMan as they change every row
  newtab.bindColumn(MS::columnName(MS::ANTENNA2),aipsStMan);
  newtab.bindColumn(MS::columnName(MS::UVW),aipsStMan);
  if (!useTSM) {
    newtab.bindColumn(MS::columnName(MS::DATA),aipsStMan);
    newtab.bindColumn(MS::columnName(MS::FLAG),aipsStMan);
  }    
  MeasurementSet ms(newtab);
#else
  //  NEW CODE TO ACCOMODATE VARYING SHAPED COLUMNS 
  if (useTSM) {
    Int tileSize=nChan/10+1;

    TiledShapeStMan tiledStMan1("TiledData",
                                 IPosition(3,nCorr,tileSize,
                                           16384/nCorr/tileSize));
    TiledShapeStMan tiledStMan1f("TiledFlag",
                                 IPosition(3,nCorr,tileSize,
                                           16384/nCorr/tileSize));
    //TiledShapeStMan tiledStMan2("TiledWeight",
    //                             IPosition(3,nCorr,tileSize,
    //                                       16384/nCorr/tileSize));
    TiledColumnStMan tiledStMan3("TiledUVW",
                                 IPosition(2,3,1024));

    // Bind the DATA and FLAG columns to the tiled stman
    newtab.bindColumn(MS::columnName(MS::DATA),tiledStMan1);
    newtab.bindColumn(MS::columnName(MS::FLAG),tiledStMan1f);
    newtab.bindColumn(MS::columnName(MS::UVW),tiledStMan3);
  } else {
    newtab.bindColumn(MS::columnName(MS::DATA),aipsStMan);
    newtab.bindColumn(MS::columnName(MS::FLAG),aipsStMan);
    newtab.bindColumn(MS::columnName(MS::UVW),aipsStMan);
  }   
  TableLock lock(TableLock::AutoLocking);
  MeasurementSet ms(newtab,lock);
#endif

  // create all subtables
  // we make new tables with 0 rows
  Table::TableOption option=Table::New;

  // Set up the default subtables for the MS
  ms.createDefaultSubtables(option);

  // Add some optional columns to the required tables
  //ms.spectralWindow().addColumn(ArrayColumnDesc<Int>(
  //  MSSpectralWindow::columnName(MSSpectralWindow::ASSOC_SPW_ID),
  //  MSSpectralWindow::columnStandardComment(MSSpectralWindow::ASSOC_SPW_ID)));

  //ms.spectralWindow().addColumn(ArrayColumnDesc<String>(
  //  MSSpectralWindow::columnName(MSSpectralWindow::ASSOC_NATURE),
  //  MSSpectralWindow::columnStandardComment(MSSpectralWindow::ASSOC_NATURE)));

  ms.spectralWindow().addColumn(ScalarColumnDesc<Int>(
    MSSpectralWindow::columnName(MSSpectralWindow::DOPPLER_ID),
    MSSpectralWindow::columnStandardComment(MSSpectralWindow::DOPPLER_ID)));

  // Now setup some optional columns::

  // the SOURCE table, 2 extra optional columns needed
  TableDesc sourceDesc = MSSource::requiredTableDesc();
  MSSource::addColumnToDesc(sourceDesc,MSSourceEnums::REST_FREQUENCY,1);
  MSSource::addColumnToDesc(sourceDesc,MSSourceEnums::SYSVEL,1);
  MSSource::addColumnToDesc(sourceDesc,MSSourceEnums::TRANSITION,1);
  SetupNewTable sourceSetup(ms.sourceTableName(),sourceDesc,option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::SOURCE),
                                     Table(sourceSetup));

  // the DOPPLER table, no optional columns needed
  TableDesc dopplerDesc = MSDoppler::requiredTableDesc();
  SetupNewTable dopplerSetup(ms.dopplerTableName(),dopplerDesc,option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::DOPPLER),
                                     Table(dopplerSetup));

  // the SYSCAL table, 1 optional column needed
  TableDesc syscalDesc = MSSysCal::requiredTableDesc();
  MSSysCal::addColumnToDesc(syscalDesc,MSSysCalEnums::TSYS,1);
  SetupNewTable syscalSetup(ms.sysCalTableName(),syscalDesc,option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::SYSCAL),
                                     Table(syscalSetup));

  // update the references to the subtable keywords
  ms.initRefs();

  { // Set the TableInfo
    TableInfo& info(ms.tableInfo());
    info.setType(TableInfo::type(TableInfo::MEASUREMENTSET));
    info.setSubType(String("MIRIAD"));
    info.readmeAddLine("Made with Importmiriad");
  }                                       

  ms_p=ms;
  msc_p = new MSColumns(ms_p);
} // setupMeasurementSet()

// ==============================================================================================
void Importmiriad::fillObsTables()
{
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillObsTables" << LogIO::POST;

  char hline[256];
  Int heof;

  ms_p.observation().addRow();
  MSObservationColumns msObsCol(ms_p.observation());

  msObsCol.telescopeName().put(0,array_p);
  msObsCol.observer().put(0,observer_p);
  msObsCol.project().put(0,project_p);
  if (array_p == "HATCREEK") {
    Vector<String> blog(1);
    blog(0) = "See HISTORY for CARMA observing log";
    msObsCol.log().put(0,blog);
  }
  Vector<Double> range(2);
  MSColumns msCol(ms_p);
  range(0) = msCol.time()(0)-1;
  range(1) = msCol.time()(ms_p.nrow()-1)+1;
  msObsCol.timeRange().put(0,range);

  // should double buffer history, and search for  (e.g.)
  // GPAVER: Executed on: 96SEP12:15:40:48.0
 
  // String date("");
  // if (date=="") date="01/01/00";
  // Double time=date2mjd(date);

  MSHistoryColumns msHisCol(ms_p.history());

  String history;
  Int row=-1;
  hisopen_c(uv_handle_p,"read");
  for (;;) {
    hisread_c(uv_handle_p,hline,256,&heof);
    if (heof) break;
    ms_p.history().addRow(); 
    row++;
    msHisCol.observationId().put(row,0);
    //    msHisCol.time().put(row,time);    // fix the "2000/01/01/24:00:00" bug
    //  nono, better file a report, it appears to be an aips++ problem
    msHisCol.priority().put(row,"NORMAL");
    msHisCol.origin().put(row,"Importmiriad::fillObsTables");
    msHisCol.application().put(row,"importmiriad");
    msHisCol.message().put(row,hline);
  }
  hisclose_c(uv_handle_p);
} // fillObsTables()

// ==============================================================================================
//
// Loop over the visibility data and fill the main table of the MeasurementSet 
// as you find corr/wcorr's
//
void Importmiriad::fillMSMainTable()
{
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillMSMainTable" << LogIO::POST;

  MSColumns& msc(*msc_p);           // Get access to the MS columns, new way
  Int nCorr = (array_p=="CARMA" ? 1 : npol_p); // # stokes (1 for CARMA for now)
  Int nChan = MAXCHAN;              // max # channels to be written
  if (Debug(1)) os_p << LogIO::DEBUG1 << "nCorr = "<<nCorr<<", nChan = "<< nChan <<LogIO::POST;
  Int nCat  = 3;                    // # initial flagging categories (fixed at 3)
  Int iscan = 0;
  Int ifield_old = 0;

  Matrix<Complex> vis(nCorr,nChan);
  Vector<Float>   sigma(nCorr);
  Vector<String>  cat(nCat);
  cat(0)="FLAG_CMD";
  cat(1)="ORIGINAL";
  cat(2)="USER";
  msc.flagCategory().rwKeywordSet().define("CATEGORY",cat);
  Cube<Bool> flagCat(nCorr,nChan,nCat,false);  
  Matrix<Bool> flag = flagCat.xyPlane(0); // references flagCat's storage
  Vector<Float> w1(nCorr), w2(nCorr);

  uvrewind_c(uv_handle_p);
  
  nAnt_p.resize(1);
  nAnt_p[0]=0;

  receptorAngle_p.resize(1);
  Int group, row=-1;
  Double interval;
  Bool lastRowFlag = false;

  //os_p << "Found  " << win[0].nspect << " spectral window" << (win[0].nspect>1 ? "s":"") << LogIO::POST;

  time_p=0;
  for (group=0; ; group++) {        // loop forever until end-of-file
    int nread, nwread;
    uvread_c(uv_handle_p, preamble, data, flags, MAXCHAN, &nread);
    // os_p << "UVREAD: " << data[0] << " " << data[1] << LogIO::POST;

    Float baseline = preamble[4];
    Int ant1 = Int(baseline)/256;              // baseline = 256*A1 + A2
    Int ant2 = Int(baseline) - ant1*256;       // mostly A1 <= A2


    if (Debug(9)) os_p << LogIO::DEBUG2 << "UVREAD: " << nread << LogIO::POST;
    if (nread <= 0) break;          // done with reading miriad data
    if (win[freqSet_p].nspect > 0)
        uvwread_c(uv_handle_p, wdata, wflags, MAXCHAN, &nwread);
    else
        nwread=0;

    // get time in MJD seconds ; input was in JD
    Double time = (preamble[3] - 2400000.5) * C::day;
    if (time>time_p) {
      if (time_p==0) timeFirst_p=time;
      // new time slot (assuming time sorted data)
      // update tsys data - TODO
    }
    time_p = time;

    // for MIRIAD, this would always cause a single array dataset,
    // but we need to count the antpos occurences to find out
    // which array configuration we're in.

    if (uvupdate_c(uv_handle_p)) {       // aha, something important changed
        if (Debug(4)) {
            os_p << LogIO::DEBUG2 << "Record " << group+1 << " uvupdate" << LogIO::POST;
        }
        Tracking(group);
    } else {
        if (Debug(5)) os_p << LogIO::DEBUG2 << "Record " << group << LogIO::POST;
    }

    // now that phasem1 has been loaded, apply linelength, if needed
    if (Qlinecal_p) {
      linecal(nread,data,phasem1[ant1-1],phasem1[ant2-1]);
      // linecal(nwread,wdata,phasem1[ant1-1],phasem1[ant2-1]);
    }

    nAnt_p[nArray_p-1] = max(nAnt_p[nArray_p-1],ant1);   // for MIRIAD, and also 
    nAnt_p[nArray_p-1] = max(nAnt_p[nArray_p-1],ant2);
    ant1--; ant2--;                                      // make them 0-based for CASA

    ant1 += nants_offset_p;     // correct for different array offsets
    ant2 += nants_offset_p;


    // should ant1 and ant2 be offset with (nArray_p-1)*nant_p ???
    // in case there are multiple arrays???
    // TODO: code should just assuming single array
    
    Vector<Double> uvw(3);
    uvw(0) = -preamble[0] * 1e-9; // convert to seconds
    uvw(1) = -preamble[1] * 1e-9; // MIRIAD uses nanosec
    uvw(2) = -preamble[2] * 1e-9; // note - sign (CASA vs. MIRIAD convention)
    uvw   *= C::c;                // Finally convert to meters for CASA

    if (group==0 && Debug(1)) {
        os_p << LogIO::DEBUG1 << "### First record: " << LogIO::POST;
        os_p << LogIO::DEBUG1 << "### Preamble: " << preamble[0] << " " <<
                                preamble[1] << " " <<
                                preamble[2] << " nanosec.(MIRIAD convention)" << LogIO::POST;
        os_p << LogIO::DEBUG1 << "### uvw: " << uvw(0) << " " <<
                               uvw(1) << " " <<
                               uvw(2) << " meter. (CASA convention)" << LogIO::POST;
    }


    // first construct the data (vis & flag) in a single long array
    // containing all spectral windows
    // In the (optional) loop over all spectral windows, subsets of
    // these arrays will be written out

    for (Int i=0; i<nCorr; i++) {
      Int count = 0;                // index into data[] array
      if (i>0) uvread_c(uv_handle_p, preamble, data, flags, MAXCHAN, &nread);
      //if (group==0) {
      //        os_p << "pol="<< pol<<", nread="<<nread<<LogIO::POST;
      //  os_p << "data(500)="<< data[1000] <<", "<< data[1001] <<LogIO::POST;
      //}
      for (Int chan=0; chan<nChan; chan++) {

        // miriad uses bl=ant1-ant2, FITS/AIPS/CASA use bl=ant2-ant1
        // apart from using -(UVW)'s, the visib need to be conjugated as well
        Bool  visFlag =  (flags[count/2] == 0) ? false : true;
        Float visReal = +data[count]; count++; 
        Float visImag = -data[count]; count++;
        Float wt = 1.0;
        if (!visFlag) wt = -wt;
        
        // check flags array !! need separate counter (count/2)
        Int pol = corrIndex_p(i);
        flag(pol,chan) = (wt<=0); 
        vis(pol,chan) = Complex(visReal,visImag);
      } // chan
      //if (group==0) os_p << "vis = "<<vis(pol,500)<<LogIO::POST;
    } // pol

    Int ispw=-1;
    for (Int sno=0; sno < win[freqSet_p].nspect; sno++) {
      if (not keep_p(sno)) continue;    
      // IFs go to separate rows in the MS, pol's do not!
      ms_p.addRow(); 
      row++; ispw++;

      // first fill in values for all the unused columns
      if (row==0) {
        ifield_old = ifield;
        msc.feed1().put(row,0);
        msc.feed2().put(row,0);
        msc.flagRow().put(row,false);
        lastRowFlag = false;
        msc.scanNumber().put(row,iscan);
        msc.processorId().put(row,-1);
        msc.observationId().put(row,0);
        msc.stateId().put(row,-1);
      }
      
      Matrix<Complex> tvis(nCorr,win[freqSet_p].nschan[sno]);
      Cube<Bool> tflagCat(nCorr,win[freqSet_p].nschan[sno],nCat,false);  
      Matrix<Bool> tflag = tflagCat.xyPlane(0); // references flagCat's storage
      
      Int woffset = win[freqSet_p].ischan[sno]-1;
      Int wsize   = win[freqSet_p].nschan[sno];
      for (Int j=0; j<nCorr; j++) {
        for (Int i=0; i< wsize; i++) {
          tvis(j,i) = vis(j,i+woffset);
          tflag(j,i) = flag(j,i+woffset);
        }
      }

      //if (group==0) os_p<<"tvis="<<tvis(0,500)<<", "<<tvis(1,500)<<LogIO::POST;
      msc.data().put(row,tvis);
      msc.flag().put(row,tflag);
      msc.flagCategory().put(row,tflagCat);

      Bool rowFlag = allEQ(flag,true);
      if (rowFlag != lastRowFlag) {
        msc.flagRow().put(row,rowFlag);
        lastRowFlag = rowFlag;
      }

      msc.antenna1().put(row,ant1);
      msc.antenna2().put(row,ant2);
      msc.time().put(row,time);           // CARMA did begin of scan.., now middle (2009)
      msc.timeCentroid().put(row,time);   // do we really need this ? flagging/blanking ?

      interval = inttime_p;
      msc.exposure().put(row,interval);
      msc.interval().put(row,interval);
      Float chnbw = win[freqSet_p].sdf[sno]*1e9;
      Float factor = interval * abs(chnbw)/jyperk_p/jyperk_p;
      // sigma=sqrt(Tx1*Tx2)/sqrt(chnbw*intTime)*JyPerK;
      if (Qtsys_p) {    
        w2 = 1.0; 
        if( systemp[ant1] == 0 || systemp[ant2] == 0) {
          zero_tsys++;
          w1 = 0.0;
        } else {
          w1 = factor/(systemp[ant1]*systemp[ant2]);  // see uvio::uvinfo_variance()
          w2 = sqrt(systemp[ant1]*systemp[ant2])/sqrt(factor);
        }
        // os_p << w1 << " " << w2 << " " << jyperk_p << " "<< chnbw<< " "<< interval<< LogIO::POST;
        msc.weight().put(row,w1);
        msc.sigma().put(row,w2);        
      } else {
          w1=factor/50/50;  // Use nominal 50K systemp to keep values similar
          w2=50/sqrt(factor);
          msc.weight().put(row,w1);
          msc.sigma().put(row,w2);
      }
      msc.uvw().put(row,uvw);
      msc.arrayId().put(row,nArray_p-1);
      // calc index into table
      msc.dataDescId().put(row,ddid_p+ispw);
      msc.fieldId().put(row,ifield);


      if (ifield_old != ifield) 
        iscan++;
      ifield_old = ifield;
      msc.scanNumber().put(row,iscan);

    }  // sno
    fcount[ifield]++;
  } // for(grou) : loop over all visibilities
  show();
  if (ms_p.nrow()==0) {
    os_p<<LogIO::SEVERE<<"No data selected, table is empty!" <<LogIO::POST;
  }
  else {
    os_p << "Processed " << group << " visibilities from " << infile_p  
       << (Qlinecal_p ? " (applying linecal)." : " (raw)." )
       << LogIO::POST;
    os_p << "Found " << npoint << " pointings with "
       <<  nfield << " unique source/fields, "
       <<  source_p.nelements() << " sources and "
       <<  nArray_p << " array"<< (nArray_p>1 ? "s":"")<<"." 
       << LogIO::POST;
  }
  if (Debug(1))
    os_p << LogIO::DEBUG1 << "nAnt_p contains: " << nAnt_p.nelements() << LogIO::POST;


} // fillMSMainTable()

void Importmiriad::fillAntennaTable()
{
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillAntennaTable" << LogIO::POST;
  Int nAnt=nants_p;

#if 0
  Int array = nArray_p;
  // we don't have 'array' yet, and nAnt_p isnt' big enough....
  if (nAnt_p[array]>MAXANT)
    throw(AipsError("Too many antennas -- should never occur"));
  if (nAnt_p[array]>nants_p)
    throw(AipsError("Not all antennas found in antenna table:"));


  receptorAngle_p[array].resize(2*nAnt);
#endif

  // === Here's a recipe to get them in ITRF format in casapy ===
  //     But there appears to be some GEODETIC vs. GEOCENTRIC issue 
  // WGS84 
  // b=me.measure(me.observatory('carma'),'itrf')    
  // b2=me.measure(me.observatory('carma'),'wgs84')    
  // lon=me.getvalue(b)['m0']['value'] 
  // lat=me.getvalue(b2)['m1']['value'] 
  // r=me.getvalue(b)['m2']['value'] 
  // x=r*cos(lat)*cos(lon)
  // y=r*cos(lat)*sin(lon)
  // z=r*sin(lat)
  // print 'arrayXYZ_p(0) =',x,';'
  // print 'arrayXYZ_p(1) =',y,';'
  // print 'arrayXYZ_p(2) =',z,';'
  arrayXYZ_p.resize(3);
  if (array_p == "HATCREEK" || array_p == "BIMA") {     // Array center:
    arrayXYZ_p(0) = -2523862.04;
    arrayXYZ_p(1) = -4123592.80;
    arrayXYZ_p(2) =  4147750.37;
  } else if (array_p == "ATCA") {
    arrayXYZ_p(0) = -4750915.837;
    arrayXYZ_p(1) =  2792906.182;
    arrayXYZ_p(2) = -3200483.747;
  } else if (array_p == "OVRO" || array_p == "CARMA") {
    arrayXYZ_p(0) = -2397389.65197;
    arrayXYZ_p(1) = -4482068.56252;
    arrayXYZ_p(2) =  3843528.41479;
  } else {
    os_p << LogIO::WARN<< "unknown array position for "<<array_p<<LogIO::POST;
    arrayXYZ_p = 0.0;
  }
  if(Debug(3)) os_p << LogIO::DEBUG2 << "number of antennas ="<<nAnt<<LogIO::POST;
  if(Debug(3)) os_p << LogIO::DEBUG2 << "array ref pos:"<<arrayXYZ_p<<LogIO::POST;

  String timsys = "TAI";  // assume, for now .... 

  // store the time keywords ; again, miriad doesn't have this (yet)
  // check w/ uvfitsfiller again

  //save value to set time reference frame elsewhere
  timsys_p=timsys;

  // Antenna diamater:
  // Should check the 'antdiam' UV variable, but it doesn't appear to 
  // exist in our CARMA datasets.
  // So, fill in some likely values
  Float diameter=25;                        //# most common size (:-)
  if (array_p=="ATCA")     diameter=22;     //# only at 'low' freq !!
  if (array_p=="HATCREEK") diameter=6;
  if (array_p=="BIMA")     diameter=6;
  if (array_p=="CARMA")    diameter=8;
  if (array_p=="OVRO")     diameter=10;

  if (nAnt == 15 && array_p=="OVRO") {
    os_p << "CARMA array (6 OVRO, 9 BIMA) assumed" << LogIO::POST;
    array_p = "CARMA";
  } else  if (nAnt == 23 && array_p=="OVRO") {
    os_p << "CARMA array (6 OVRO, 9 BIMA, 8 SZA) assumed" << LogIO::POST;
    array_p = "CARMA";
  } else  if (array_p=="CARMA") {
    os_p << "Hurray, CARMA data; version " << version_p << " with " << nAnt
         << " antennas" << LogIO::POST;
  } else if (array_p=="ATCA") {
    os_p <<"Found ATCA data with " << nAnt << " antennas" << LogIO::POST;
  } else
    os_p << "Ant configuration not supported yet" << LogIO::POST;

  MSAntennaColumns& ant(msc_p->antenna());
  Vector<Double> antXYZ(3);

  // add antenna info to table
  if (nArray_p == 0) {                   // check if needed
    ant.setPositionRef(MPosition::ITRF);
    //ant.setPositionRef(MPosition::WGS84);
  }
  Int row=ms_p.antenna().nrow()-1;

  if (Debug(2)) os_p << LogIO::DEBUG2 << "Importmiriad::fillAntennaTable row=" << row+1 
       << " array " << nArray_p+1 << LogIO::POST;

  for (Int i=0; i<nAnt; i++) {

    ms_p.antenna().addRow(); 
    row++;
    if (array_p=="OVRO" || array_p=="BIMA" || array_p=="HATCREEK" || array_p=="CARMA") {
      if (i<6)
        ant.dishDiameter().put(row,10.4);  // OVRO
      else if (i<15)
        ant.dishDiameter().put(row,6.1);   // BIMA or HATCREEK
      else
        ant.dishDiameter().put(row,3.5);   // SZA
    } else {
      ant.dishDiameter().put(row,diameter); // others
    }
    antXYZ(0) = antpos[i];              //# these are now in nano-sec
    antXYZ(1) = antpos[i+nAnt];
    antXYZ(2) = antpos[i+nAnt*2];
    antXYZ *= 1e-9 * C::c;             //# and now in meters
    if (Debug(2)) os_p << LogIO::DEBUG2 << "Ant " << i+1 << ":" << antXYZ << " (m)." << LogIO::POST;

    String mount;                           // really should consult
    switch (mount_p) {                      // the "mount" uv-variable
      case  0: mount="ALT-AZ";      break;
      case  1: mount="EQUATORIAL";  break;
      case  2: mount="X-Y";         break;
      case  3: mount="ORBITING";    break;
      case  4: mount="BIZARRE";     break;
      // case  5: mount="SPACE-HALCA"; break;
      default: mount="UNKNOWN";     break;
    }
    ant.mount().put(row,mount);
    ant.flagRow().put(row,false);
    String antName = "C";
    if (array_p=="ATCA") antName="CA0";
    antName += String::toString(i+1);
    ant.name().put(row,antName);
    ant.station().put(row,"ANT" + String::toString(i+1));  // unknown PADs, so for now ANT#
    ant.type().put(row,"GROUND-BASED");

    Vector<Double> offsets(3);
    offsets=0.0;
    // store absolute positions, with all offsets 0

#if 1
    // from MirFiller; but why we're rotating this? To reverse miriad's rotation
    // of y-axis to local East
    Double arrayLong = atan2(arrayXYZ_p(1),arrayXYZ_p(0));
    Matrix<Double> posRot = Rot3D(2,arrayLong);
    antXYZ = product(posRot,antXYZ);
#endif

#if 1
// This doesn't work because miriad calculated the relative positions with
// respect to the first antenna with non zero coordinates, 
// not the array reference position. This makes it impossible to invert exactly
    ant.position().put(row,arrayXYZ_p+antXYZ);
#else
    //test
    ant.position().put(row,arrayXYZ_p);
#endif
    ant.offset().put(row,offsets);

    // store the angle for use in the feed table
//    receptorAngle_p[array](2*i+0)=polangleA(i)*C::degree;
//    receptorAngle_p[array](2*i+1)=polangleB(i)*C::degree;
  }
  // ant.position().rwKeywordSet().define("MEASURE_REFERENCE","ITRF");

  nArray_p++;
  nAnt_p.resize(nArray_p);
  nAnt_p[nArray_p-1] = 0;
  if (Debug(3) && nArray_p > 1)
    os_p << LogIO::DEBUG2  << nAnt_p[nArray_p-2] << LogIO::POST;
  
  if (nArray_p > 1) return;

  // now do some things which only need to happen the first time around

  // store these items in non-standard keywords for now
  // 
  String arrnam = array_p;
  ant.name().rwKeywordSet().define("ARRAY_NAME",arrnam);
  ant.position().rwKeywordSet().define("ARRAY_POSITION",arrayXYZ_p);


  // fill the array table entry
  // this assumes there is one AN table for each (sub)array index encountered.

  //PJT ms_p.array().addRow();
  // array is now gone, there is an array_id in the main MS table for 
  // id purposes.  We store the ARRAY_POSITION as a non-standard keyword
  // with the POSITION collumn in the ANTENNA table (see above)
#if 0
  MSArrayColumns arr(ms_p.array());
  arr.name().put(array,array_p);
  arr.position().put(array,arrayXYZ);
  arr.position().rwKeywordSet().define("MEASURE_REFERENCE","ITRF");
#endif
} // fillAntennaTable

// ==============================================================================================
void Importmiriad::fillSyscalTable()
{
  //if (Debug(1)) os_p << "Importmiriad::fillSyscalTable" << LogIO::POST;

  MSSysCalColumns&     msSys(msc_p->sysCal());
  Vector<Float> Systemp(1);    // should we set both receptors same?
  Int row = ms_p.sysCal().nrow();

  //  if (Debug(1)) 
  //   for (Int i=0; i<nants_p; i++)
  //     os_p  << "SYSTEMP: " << i << ": " << systemp[i] << LogIO::POST;

  for (Int j=0; j<win[freqSet_p].nspect; j++) {
    for (Int i=0; i<nants_p; i++) {
      ms_p.sysCal().addRow(); 
      msSys.antennaId().put(row,i);   //  i, or i+nants_offset_p ????
      msSys.feedId().put(row,0);
      msSys.spectralWindowId().put(row,j);    // all of them for now .....
      msSys.time().put(row,time_p);
      msSys.interval().put(row,-1.0);
    
      Systemp(0) = systemp[i+j*nants_p];
      msSys.tsys().put(row,Systemp);
      row++; 
    }
  }
 


  // this may actually be a nasty problem for MIRIAD datasets that are not
  // timesorted. A temporary table needs to be written with all records,
  // which then needs to be sorted and 'recomputed'
}

// ==============================================================================================
void Importmiriad::fillSpectralWindowTable(String vel)
{
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillSpectralWindowTable" << LogIO::POST;

  MSSpWindowColumns&      msSpW(msc_p->spectralWindow());
  MSDataDescColumns&      msDD(msc_p->dataDescription());
  MSPolarizationColumns&  msPol(msc_p->polarization());
  MSDopplerColumns&       msDop(msc_p->doppler());

  Int nCorr = ( array_p=="CARMA" ? 1 : npol_p);            // CARMA wants 1 polarization 
  Int i, side;
  Double BW = 0.0;

  MDirection::Types dirtype = epochRef_p;    // MDirection::B1950 or MDirection::J2000;
  MEpoch ep(Quantity(time_p, "s"), MEpoch::UTC);
  // ERROR::   type specifier omitted for parameter  in older AIPS++, now works in CASA
  MPosition obspos(MVPosition(arrayXYZ_p), MPosition::ITRF);
  //MPosition obspos(MVPosition(arrayXYZ_p), MPosition::WGS84);
  MDirection dir(Quantity(ra_p, "rad"), Quantity(dec_p, "rad"), dirtype);
  MeasFrame frame(ep, obspos, dir);
  
  String velsys = vel;
  // Keep previous default, for ATCA leave at TOPO (multi-source data)
  if (array_p!="ATCA" && velsys=="") velsys="LSRK";
  
  Bool convert=true;
  MFrequency::Types freqsys_p;
  if (velsys=="LSRK") {
    freqsys_p = MFrequency::LSRK;        // LSRD vs. LSRK
    if (Debug(1)) os_p << LogIO::DEBUG1 << "USE_LSRK" << LogIO::POST;
  } else if (velsys=="LSRD"){
    freqsys_p = MFrequency::LSRD;        // LSRD vs. LSRK
    if (Debug(1)) os_p << LogIO::DEBUG1 << "USE_LSRD" << LogIO::POST;
  } else {
    freqsys_p = MFrequency::TOPO;        // use TOPO if unspecified
    convert=false;
  }

  MFrequency::Convert tolsr(MFrequency::TOPO, 
                            MFrequency::Ref(freqsys_p, frame));     // LSRD vs. LSRK
  // fill out the polarization info (only 1 entry allowed for now)
  ms_p.polarization().addRow();
  msPol.numCorr().put(0,nCorr);
  msPol.corrType().put(0,corrType_p);
  msPol.corrProduct().put(0,corrProduct_p);
  msPol.flagRow().put(0,false);

  // fill out doppler table (only 1 entry needed, CARMA data only identify 1 line :-(
  if (array_p=="CARMA") {
    if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad:: now writing Doppler table " << LogIO::POST;
    for (i=0; i<win[0].nspect; i++) {
      ms_p.doppler().addRow();
      msDop.dopplerId().put(i,i);
      msDop.sourceId().put(i,-1);     // how the heck..... for all i guess
      msDop.transitionId().put(i,-1);
      msDop.velDefMeas().put(i,MDoppler(Quantity(0),MDoppler::RADIO));
    }
  }
  Int ddid=-1;
  for (Int k=0; k < nFreqSet_p; k++) {
    for (Int i=0; i < win[k].nspect; i++) {
      if(not keep_p(i)) continue;
      ddid++;
      Int n = win[k].nschan[i];
      Vector<Double> f(n), w(n),cs(n);
      
      ms_p.spectralWindow().addRow();
      ms_p.dataDescription().addRow();
      
      msDD.spectralWindowId().put(ddid,ddid);
      msDD.polarizationId().put(ddid,0);
      msDD.flagRow().put(ddid,false);

      msSpW.numChan().put(ddid,win[k].nschan[i]);
      BW = 0.0;
      Double fwin = win[k].sfreq[i]*1e9;
      if (convert) {
        if (Debug(1)) os_p << LogIO::DEBUG1 << "Fwin: OBS=" << fwin/1e9;
        fwin = tolsr(fwin).getValue().getValue();
        if (Debug(1)) os_p << LogIO::DEBUG1 << " LSR=" << fwin/1e9 << LogIO::POST;
      }
      for (Int j=0; j < win[k].nschan[i]; j++) {
        f(j) = fwin + j * win[k].sdf[i] * 1e9;
        w(j) = abs(win[k].sdf[i]*1e9);
        cs(j) = win[k].sdf[i]*1e9;
        BW += w(j);
      }

      msSpW.chanFreq().put(ddid,f);
      if (i<win[k].nspect) {
        // I think restfreq should just be in source table,
        // but leave for now if it makes sense
        if (win[k].restfreq[i]>0 && freqsys_p!=MFrequency::TOPO) {
          msSpW.refFrequency().put(ddid,win[k].restfreq[i]*1e9);
        } else {
          msSpW.refFrequency().put(ddid,win[k].sfreq[i]*1e9);
        } 
      } else
        msSpW.refFrequency().put(ddid,freq_p);            // no reference for wide band???
      
      msSpW.resolution().put(ddid,w);
      msSpW.chanWidth().put(ddid,cs); // MSSelection wants width<0 if spectrum inverted
      msSpW.effectiveBW().put(ddid,w);
      msSpW.totalBandwidth().put(ddid,BW);
      Int ifchain = win[k].chain[i];
      msSpW.ifConvChain().put(ddid,ifchain);
      // can also do it implicitly via Measures you give to the freq's
      msSpW.measFreqRef().put(ddid,freqsys_p);
      if (i<win[k].nspect && array_p=="CARMA")
        msSpW.dopplerId().put(ddid,i);    // CARMA has only 1 ref freq line
      else
        msSpW.dopplerId().put(ddid,-1);    // no ref

      if (win[k].sdf[i] > 0)      side = 1;
      else if (win[k].sdf[i] < 0) side = -1;
      else                     side = 0;
      
      switch (win[k].code[i]) {
      case 'N':
        msSpW.netSideband().put(ddid,side);
        msSpW.freqGroup().put(ddid,1);
        msSpW.freqGroupName().put(ddid,"MULTI-CHANNEL-DATA");
        break;
      case 'W':
        msSpW.netSideband().put(ddid,side);
        msSpW.freqGroup().put(ddid,3);
        msSpW.freqGroupName().put(ddid,"SIDE-BAND-AVERAGE");
        break;
      case 'S':
        msSpW.netSideband().put(ddid,side);
        msSpW.freqGroup().put(ddid,2);
        msSpW.freqGroupName().put(ddid,"MULTI-CHANNEL-AVG");
        break;
      default:
        throw(AipsError("Bad code for a spectral window"));
        break;
      }
    }
  }
}

// ==============================================================================================
void Importmiriad::fillFieldTable()
{
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillFieldTable" << LogIO::POST;

  // set the DIRECTION MEASURE REFERENCE for appropriate columns
  // but note we're not varying them accross rows
  /// MDirection::Types epochRef=MDirection::J2000;
  /// if (nearAbs(epoch_p,1950.0,0.01)) epochRef=MDirection::B1950;   
  msc_p->setDirectionRef(epochRef_p);

  MSFieldColumns& msField(msc_p->field());

  Vector<Double> radec(2), pm(2);
  Vector<MDirection> radecMeas(1);
  Int fld;
  Double cosdec;

  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillFieldTable() adding " << nfield 
                     << " fields" << LogIO::POST;

  pm = 0;                       // Proper motion is zero

  if (nfield == 0) {            // if no pointings found, say there is 1
    os_p << "Warning: no dra/ddec pointings found, creating 1." << LogIO::POST;
    nfield = npoint = 1;
    dra[0] = ddec[0] = 0.0;
  } 

  for (fld = 0; fld < nfield; fld++) {

    ms_p.field().addRow();

    if (Debug(1))
      os_p << LogIO::DEBUG1 << "FLD: " << fld << " " << field[fld] << " " << source_p[field[fld]] << LogIO::POST;

    msField.sourceId().put(fld,field[fld]); 
    msField.name().put(fld,source_p[field[fld]]);        // this is the source name
    msField.code().put(fld,purpose_p[field[fld]]);
    msField.numPoly().put(fld,0);
    
    cosdec = cos(dec[fld]);
    radec(0) = ra[fld]  + dra[fld]/cosdec;           // RA, in radians
    radec(1) = dec[fld] + ddec[fld];                 // DEC, in radians
    radecMeas(0).set(MVDirection(radec(0), radec(1)), MDirection::Ref(epochRef_p));

    msField.delayDirMeasCol().put(fld,radecMeas);
    msField.phaseDirMeasCol().put(fld,radecMeas);
    msField.referenceDirMeasCol().put(fld,radecMeas);
    // put in best guess for time of position, this is not the coord epoch,
    // rather it is meant to cope with moving objects which have a rate.
    msField.time().put(fld,timeFirst_p);
  }
}

// ==============================================================================================
void Importmiriad::fillSourceTable()
{
  // According to MS2 specs we should have a source table with
  // an entry for every source/spectral window combination that occurs.
  // For the moment we ignore spectral window (and TIME) as an index and 
  // just use -1. This means there is only a single rest frequency.
  
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillSourceTable" << LogIO::POST;
  Int ns = 0;
  Int skip;

  MSSourceColumns& msSource(msc_p->source());

  Vector<Double> radec(2);
  Vector<Double> restFreq(1),sysvel(1);
  sysvel(0)=0;
  Int m=win[0].nspect;
  restFreq(0)=0;
  // pick first non zero restFreq
  for (Int i=0; i<m; i++) {
    if (win[0].restfreq[i]>0) {
      restFreq(0) = win[0].restfreq[i] * 1e9;    // convert from GHz to Hz
      break;
    }
  }

  //String key("MEASURE_REFERENCE");
  //msSource.restFrequency().rwKeywordSet().define(key,"REST");

  if (Debug(1)) {
    os_p << LogIO::DEBUG1 << "Importmiriad::fillSourceTable() querying " << source_p.nelements()
         << " sources" << LogIO::POST;
    os_p << LogIO::DEBUG1 << source_p << LogIO::POST;
  }
  

  // 
  for (uInt src=0; src < source_p.nelements(); src++) {

    skip = 0;
    for (uInt i=0; i<src; i++) {               // loop over sources to avoid duplicates
      if (source_p[i] == source_p[src]) {
        skip=1; cerr<<"Found duplicate source name! Fix code!"<<LogIO::POST;
        break;
      }
    }

    if (Debug(1)) os_p << LogIO::DEBUG1 << "source : " << source_p[src] << " " << skip << LogIO::POST;

    if (skip) continue;    // if seen before, don't add it again
    ms_p.source().addRow();

    radec(0) = ras_p[src];
    radec(1) = decs_p[src];

    msSource.sourceId().put(ns,src);
    msSource.name().put(ns,source_p[src]);
    msSource.spectralWindowId().put(src,-1);     // set valid for all
    msSource.direction().put(ns,radec);
    msSource.numLines().put(ns,1);
    msSource.restFrequency().put(ns,restFreq);
    msSource.time().put(ns,0.0);               // valid for all times
    msSource.interval().put(ns,0);             // valid forever
    msSource.sysvel().put(ns,sysvel);
    // TODO?
    // missing position/sysvel/transition in the produced MS/SOURCE sub-table ??

    // listobs complains:
    // No systemic velocity information found in SOURCE table.

    ns++;
  }

  // TODO:  #sources wrong if you take raw miriad before noise taken out
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillSourceTable() added " << ns << " sources" << LogIO::POST;
}

// ==============================================================================================
void Importmiriad::fillFeedTable() 
{
  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fillFeedTable" << LogIO::POST;

  MSFeedColumns msfc(ms_p.feed());

  // find out the POLARIZATION_TYPE
  // In the fits files we handle there can be only a single, uniform type
  // of polarization so the following should work.
  MSPolarizationColumns& msPolC(msc_p->polarization());

  Int numCorr=msPolC.numCorr()(0);
  Vector<String> rec_type(2); rec_type="";
  if (corrType_p(0)>=Stokes::RR && corrType_p(numCorr-1)<=Stokes::LL) {
      rec_type(0)="R"; rec_type(1)="L";
  }
  if (corrType_p(0)>=Stokes::XX && corrType_p(numCorr-1)<=Stokes::YY) {
      rec_type(0)="X"; rec_type(1)="Y";
  }

  Matrix<Complex> polResponse(2,2); 
  polResponse=0.; polResponse(0,0)=polResponse(1,1)=1.;
  Matrix<Double> offset(2,2); offset=0.;
  Vector<Double> position(3); position=0.;
  Vector<Double> ra(2);
  ra = 0.0;
  if (array_p == "ATCA") {
    ra(0)=C::pi/4;
    // 7mm feed is different; assumes all spectral windows are in same band for now...
    if (win[freqSet_p].sfreq[0]>30. && win[freqSet_p].sfreq[0]<50.) ra(0)+=C::pi/2;
    ra(1)=ra(0)+C::pi/2;
  }

  // fill the feed table
  // will only do UP TO the largest antenna referenced in the dataset
  Int row=-1;
  if (Debug(3)) os_p << LogIO::DEBUG2 << nAnt_p.nelements() << LogIO::POST;
  for (Int arr=0; arr< (Int)nAnt_p.nelements(); arr++) {   
    if (Debug(3)) os_p << LogIO::DEBUG2  << nAnt_p[arr] << LogIO::POST; 
    for (Int ant=0; ant<nAnt_p[arr]; ant++) {
      ms_p.feed().addRow(); row++;

      msfc.antennaId().put(row,ant);
      msfc.beamId().put(row,-1);
      msfc.feedId().put(row,0);
      msfc.interval().put(row,DBL_MAX);

      // msfc.phasedFeedId().put(row,-1);    // now optional
      msfc.spectralWindowId().put(row,-1); 
      msfc.time().put(row,0.);
      msfc.numReceptors().put(row,2);
      msfc.beamOffset().put(row,offset);
      msfc.polarizationType().put(row,rec_type);
      msfc.polResponse().put(row,polResponse);
      msfc.position().put(row,position);
      // fix these when incremental array building is ok.
      // although for CARMA this would never change ....
      msfc.receptorAngle().put(row,ra);
      // msfc.receptorAngle().put(row,receptorAngle_p[arr](Slice(2*ant,2)));
    }
  }      
}

// ==============================================================================================
void Importmiriad::fixEpochReferences() {

  if (Debug(1)) os_p << LogIO::DEBUG1 << "Importmiriad::fixEpochReferences" << LogIO::POST;

  if (timsys_p=="IAT") timsys_p="TAI";
  if (timsys_p=="UTC" || timsys_p=="TAI") {
    String key("MEASURE_REFERENCE");
    MSColumns msc(ms_p);
    msc.time().rwKeywordSet().define(key,timsys_p);
    msc.feed().time().rwKeywordSet().define(key,timsys_p);
    msc.field().time().rwKeywordSet().define(key,timsys_p);
    // Fits obslog time is probably local time instead of TAI or UTC
    //PJT msc.obsLog().time().rwKeywordSet().define(key,timsys_p);
  } else {
    if (timsys_p!="")
      cerr << "Unhandled time reference frame: "<<timsys_p<<LogIO::POST;
  }
}

//
// track some important uv variables to get notified when they change
// 
// ==============================================================================================
void Importmiriad::Tracking(int record)
{
  if (Debug(3)) os_p << LogIO::DEBUG2 << "Importmiriad::Tracking" << LogIO::POST;

  char vtype[10], vdata[10];
  int vlen, vupd, idat, vupd1, vupd2, vupd3, j, k;
  //  float dx, dy;
  //  Float rdat;
  //  Double ddat;

  if (record < 0) {                 // first time around: set variables to track
    uvtrack_c(uv_handle_p,"nschan","u");   // narrow lines
    uvtrack_c(uv_handle_p,"nspect","u");   // window averages
    uvtrack_c(uv_handle_p,"ischan","u");
    uvtrack_c(uv_handle_p,"sdf","u");
    uvtrack_c(uv_handle_p,"sfreq","u");    // changes a lot (doppler)

    uvtrack_c(uv_handle_p,"restfreq","u"); // never really changes....
    uvtrack_c(uv_handle_p,"freq","u");     // never really changes....
    uvtrack_c(uv_handle_p,"ifchain","u");  // optional


    uvtrack_c(uv_handle_p,"nwide","u");
    uvtrack_c(uv_handle_p,"wfreq","u");
    uvtrack_c(uv_handle_p,"wwidth","u");

    uvtrack_c(uv_handle_p,"antpos","u");   // array's
    uvtrack_c(uv_handle_p,"pol","u");      // pol's
    uvtrack_c(uv_handle_p,"dra","u");      // fields
    uvtrack_c(uv_handle_p,"ddec","u");     // fields

    uvtrack_c(uv_handle_p,"ra","u");       // source position
    uvtrack_c(uv_handle_p,"dec","u");      // source position

    uvtrack_c(uv_handle_p,"inttime","u");

    if (Qlinecal_p)
      uvtrack_c(uv_handle_p,"phasem1","u");  // linelength meaurements

    // weather:
    // uvtrack_c(uv_handle_p,"airtemp","u");
    // uvtrack_c(uv_handle_p,"dewpoint","u");
    // uvtrack_c(uv_handle_p,"relhumid","u");
    // uvtrack_c(uv_handle_p,"winddir","u");
    // uvtrack_c(uv_handle_p,"windmph","u");

    return;
  }

  // here is all the special tracking code...
  
  check_window(); // check if the freq setup has changed

  uvprobvr_c(uv_handle_p,"pol",vtype,&vlen,&vupd);
  if (vupd && npol_p==1) {
    uvrdvr_c(uv_handle_p,H_INT,"pol",(char *)&idat, NULL, 1);
    if (idat != pol_p[0])
        os_p << LogIO::WARN<<"polarization changed to " << *pol_p << LogIO::POST;
    pol_p[0] = idat;
  }

  uvprobvr_c(uv_handle_p,"npol",vtype,&vlen,&vupd);
  if (vupd) {
    uvrdvr_c(uv_handle_p,H_INT,"npol",(char *)&idat, NULL, 1);
    if (idat != npol_p)
      throw(AipsError("Cannot handle a changing npol yet"));
  }

  uvprobvr_c(uv_handle_p,"inttime",vtype,&vlen,&vupd);
  if (vupd) {
    uvgetvr_c(uv_handle_p,H_REAL,"inttime",(char *)&inttime_p,1);
  }
  uvprobvr_c(uv_handle_p,"antpos",vtype,&vlen,&vupd);
  if (vupd && record) {
    if (Qarrays_p) 
      nants_offset_p += nants_p;      // increment from size of previous array
    uvgetvr_c(uv_handle_p,H_INT, "nants", (char *)&nants_p,1);
    uvgetvr_c(uv_handle_p,H_DBLE,"antpos",(char *)antpos,3*nants_p);
    if (Debug(2)) {
      os_p << LogIO::DEBUG2 << "Found " << nants_p << " antennas for array " 
           << nArray_p << LogIO::POST;
      for (int i=0; i<nants_p; i++) {
        os_p << antpos[i] << " " << 
                antpos[i+nants_p] << " " << 
                antpos[i+nants_p*2] << LogIO::POST;
      }
    }
    if (Debug(2)) os_p << LogIO::DEBUG2 << "Warning: antpos changed at record " << record << LogIO::POST;
    if (Qarrays_p)
      fillAntennaTable();
  }

  if (Qlinecal_p) {
    uvprobvr_c(uv_handle_p,"phasem1",vtype,&vlen,&vupd);
    if (vupd) {
      // lets assume (hope?) that nants_p didn't change, it better not.
      uvgetvr_c(uv_handle_p,H_REAL,"phasem1",(char *)phasem1,nants_p);
      // os_p << "PHASEM1: " << phasem1[0] << " " << phasem1[1] << " ...\n";
    }
  }

  if (win[freqSet_p].nspect > 0) {
    uvprobvr_c(uv_handle_p,"systemp",vtype,&vlen,&vupd);  
    if (vupd) {
      uvgetvr_c(uv_handle_p,H_REAL,"systemp",(char *)systemp,nants_p*win[freqSet_p].nspect);
      if (Debug(3)) {
        os_p << LogIO::DEBUG2 << "Found systemps (new scan)" ;
        for (Int i=0; i<nants_p; i++)  os_p << systemp[i] << " ";
        os_p << LogIO::POST;
      }
      fillSyscalTable();
    }
  } else {
    uvprobvr_c(uv_handle_p,"wsystemp",vtype,&vlen,&vupd);  
    if (vupd) {
      uvgetvr_c(uv_handle_p,H_REAL,"wsystemp",(char *)systemp,nants_p);
      if (Debug(3)) {
        os_p << LogIO::DEBUG2 << "Found wsystemps (new scan)" ;
        for (Int i=0; i<nants_p; i++)  os_p << systemp[i] << " ";
        os_p << LogIO::POST;
      }
    }
  }

  // Go after a new pointing (where {source,ra,dec} was changed)
  // SOURCE and DRA/DDEC are mixed together they define a row in the FIELD table
  // so we need to build a field index here as well

  uvprobvr_c(uv_handle_p,"source",vtype,&vlen,&vupd);
  if (vupd) {
    uvgetvr_c(uv_handle_p,H_BYTE,"source",vdata,16);
    object_p = vdata;  

    j=-1;
    for (uInt i=0; i<source_p.nelements(); i++) {    // find first matching source name
      if (source_p[i] == object_p) {
              j = i ;
              os_p << "Found old source: " << object_p << LogIO::POST;

              break;
      }
    }
    if (j==-1) {
      os_p << "Found new source: " << object_p << LogIO::POST;
      source_p.resize(source_p.nelements()+1, true);     // need to copy the old values
      source_p[source_p.nelements()-1] = object_p;
    
      ras_p.resize(ras_p.nelements()+1, true);     
      decs_p.resize(decs_p.nelements()+1, true);   
      ras_p[ras_p.nelements()-1] = 0.0;                  // if no source at (0,0) offset
      decs_p[decs_p.nelements()-1] = 0.0;                // these would never be initialized  
    }
    uvprobvr_c(uv_handle_p,"purpose",vtype,&vlen,&vupd3);
    purpose_p.resize(purpose_p.nelements()+1, true);   // need to copy the old values
    purpose_p[purpose_p.nelements()-1] = " ";
    if (vupd3) {
      uvgetvr_c(uv_handle_p,H_BYTE,"purpose",vdata,16);
      purpose_p[purpose_p.nelements()-1] = vdata;
    }
  }

  uvprobvr_c(uv_handle_p,"dra", vtype,&vlen,&vupd1);
  uvprobvr_c(uv_handle_p,"ddec",vtype,&vlen,&vupd2);

  uvgetvr_c(uv_handle_p,H_DBLE,"ra", (char *)&ra_p, 1);
  uvgetvr_c(uv_handle_p,H_DBLE,"dec",(char *)&dec_p,1);

  if (vupd || vupd1 || vupd2) {    // if either source or offset changed, find FIELD_ID
    npoint++;
    if (vupd1) uvgetvr_c(uv_handle_p,H_REAL,"dra", (char *)&dra_p,  1);
    if (vupd2) uvgetvr_c(uv_handle_p,H_REAL,"ddec",(char *)&ddec_p, 1);

    j=-1;
    for (uInt i=0; i<source_p.nelements(); i++) {    // find first matching source name
      if (source_p[i] == object_p) {
        j = i ;
        break;
      }
    }
    // j should always be >= 0 now, and is the source index
    k=-1;
    for (Int i=0; i<nfield; i++) { // check if we had this pointing/source before 
      if (dra[i] == dra_p && ddec[i] == ddec_p && field[i] == j) {
        k = i;
        break;
      }
    }
    // k could be -1, when a new field/source is found
    // else it is >=0 and the index into the field array

    if (Debug(1)) {
      os_p << LogIO::DEBUG1 << "POINTING: " << npoint 
           << " source: " << object_p << " [" << j << "," << k << "] "
           << " dra/ddec: "   << dra_p << " " << ddec_p << LogIO::POST;
    }
    
    if (k<0) {                             // we have a new source/field combination
      ifield = nfield;
      nfield++;
      if (Debug(2)) os_p << LogIO::DEBUG2 << "Adding new field " << ifield 
                         << " for " << object_p << " " << source_p[j] 
                         << " at " 
                         << dra_p *206264.8062 << " " 
                         << ddec_p*206264.8062 << " arcsec." << LogIO::POST;
      if (Debug(1)) show();

      if (nfield >= MAXFIELD) {
        os_p << "Cannot handle more than " << MAXFIELD << " fields." << LogIO::POST;
        exit(1);
      }
      ra[ifield]  = ra_p;
      dec[ifield] = dec_p;
      dra[ifield]  = dra_p;
      ddec[ifield] = ddec_p;
      field[ifield] = j;
      if (dra_p == 0.0 && ddec_p==0.0) {   // store ra/dec for SOURCE table as well 
        ras_p[j]  = ra_p;
        decs_p[j] = dec_p;
      }
    } else {
      ifield = k;
    }

    if (Debug(3)) os_p << LogIO::DEBUG2 << "Warning: pointing " << j 
        << " (dra/ddec) changed at record " << record << " : " 
        << dra_p *206264.8062 << " " 
        << ddec_p*206264.8062 << LogIO::POST;
  }
} // Tracking()



// ==============================================================================================
// Check for changes of the frequency setup and store them all
void Importmiriad::check_window()
{
  int vlen,vupd;
  char vtype[10];
  Int next = nFreqSet_p;
  if (Debug(2)) os_p << LogIO::DEBUG2 << "Importmiriad::check_window" << LogIO::POST;
  Int idx, nchan=0, nspect=0, nwide=0;

  uvrdvr_c(uv_handle_p,H_INT,"nchan",(char *)&nchan, (char *)&nchan, 1);
  uvrdvr_c(uv_handle_p,H_INT,"nspect",(char *)&nspect,(char *)&nspect, 1);
  win[next].nspect = nspect;
  uvrdvr_c(uv_handle_p,H_INT,"nwide",(char *)&nwide, (char *)&nwide, 1);
  win[next].nwide = nwide;

  if (nspect > 0 && nspect <= MAXWIN) {

    uvgetvr_c(uv_handle_p,H_INT,"ischan",(char *)win[next].ischan, nspect);
    uvgetvr_c(uv_handle_p,H_INT,"nschan",(char *)win[next].nschan, nspect);
    uvgetvr_c(uv_handle_p,H_DBLE,"restfreq",(char *)win[next].restfreq, nspect);
    uvgetvr_c(uv_handle_p,H_DBLE,"sdf",(char *)win[next].sdf, nspect);
    uvgetvr_c(uv_handle_p,H_DBLE,"sfreq",(char *)win[next].sfreq, nspect);
    uvprobvr_c(uv_handle_p,"ifchain", vtype,&vlen,&vupd);
    if (vtype[0]=='i')
      uvgetvr_c(uv_handle_p,H_INT,"ifchain",(char *)win[next].chain, nspect);
  }
  if (nwide>0) {
       uvgetvr_c(uv_handle_p,H_REAL,"wfreq",(char *)win[next].wfreq, nwide);
       uvgetvr_c(uv_handle_p,H_REAL,"wwidth",(char *)win[next].wwidth, nwide); 
  }

  if (nwide >0 && nspect != nwide) {
    // don't know how to handle this
    // we could assume the smaller one is the one we should deal with
    // but there's no way to check how select=win() was used...
    // also, if you've used uvcat options=nowide, nwide=0 and nspect non-zero.
    // we really don't care about the wide bands anymore
    if (nwide < nspect)
      throw(AipsError("nspect != nwide"));
    else {
      nwide = nspect;
    }
  }

  for (Int i=0; i<nspect; i++) {
    win[next].code[i] = 'N';
  }

  idx = (nspect > 0 ? nspect : 0);           // idx points into the combined win.xxx[] elements
  for (Int i=0; i<nwide; i++) {
    Int side = (win[next].sdf[i] < 0 ? -1 : 1);
    win[next].code[idx]     = 'S';
    win[next].ischan[idx]   = nchan + i + 1;
    win[next].nschan[idx]   = 1;
    win[next].sfreq[idx]    = win[next].wfreq[i];
    win[next].sdf[idx]      = side * win[next].wwidth[i];
    win[next].restfreq[idx] = -1.0;  // no meaning
    idx++;
  }
  
  if (nspect>0) {
    // Got all the window details, now check if we already have this one
    Bool found=false;
    for (Int i=0; i<nFreqSet_p; i++) {
      // compare win[i] and win[next]
      found=compareWindows(win[i],win[next]);
      if (found) {
        freqSet_p=i;
        break;
      }
    }

    if (not found && Debug(1)) {
      os_p << LogIO::DEBUG1 << "Layout of spectral windows (check_window): nspect=" << nspect 
           << " nwide=" << nwide << LogIO::POST;
      os_p << LogIO::DEBUG1 << "(N=narrow    W=wide,   S=spectral window averages)" << LogIO::POST;

      for (Int i=0; i<nspect+nwide; i++)
        os_p << LogIO::DEBUG1 << win[next].code[i] << ": " << i+1  << " " << keep_p(i) << " "
             << win[next].nschan[i] << " " << win[next].ischan[i] << " " 
             << win[next].sfreq[i] <<  " " << win[next].sdf[i] <<  " " << win[next].restfreq[i]
             << LogIO::POST;
    }


    if (not found) {
      os_p << "New frequency setting with "<<nspect<<" spectral windows"<<LogIO::POST;
      nFreqSet_p=nFreqSet_p+1;
      if (nFreqSet_p>=MAXFSET) throw(AipsError("Too many frequency settings"));
      freqSet_p=next;
    }
    // Calculate datadesc_id offset
    ddid_p=0;
    for (Int i=0; i<freqSet_p; i++){
      for (Int j=0; j<win[i].nspect+win[i].nwide; j++) {
        if (keep_p(j)) ddid_p+=1;
      }
    }
  }
}

// ==============================================================================================
Bool Importmiriad::compareWindows(WINDOW& win1,WINDOW& win2)
{
  // Check if two freq/corr windows are the same (within tolerance)
  if (win1.nspect!= win2.nspect || win1.nwide!=win2.nwide) return false;
  for (Int i=0; i<win1.nspect; i++){
    if (win1.nschan[i]!=win2.nschan[i]) return false;
    Double w = abs(win1.sdf[i]);
    if (abs(win1.sdf[i]-win2.sdf[i])>0.01*w) return false;
    if (abs(win1.sfreq[i]-win2.sfreq[i])>0.5*w) return false;
    if (abs(win1.restfreq[i]-win2.restfreq[i])>0.5*w) return false;
    if (win1.chain[i]!=win2.chain[i]) return false;
  }
  // could check wides, but since they are not written..
  return true;
}

// ==============================================================================================
void Importmiriad::show()
{
#if 0
  os_p << "Importmiriad::show()" << LogIO::POST;
  for (int i=0; i<source_p.nelements(); i++)  os_p << "SOURCE_P_1: " << source_p[i] << LogIO::POST;
#endif
}

// ==============================================================================================
void Importmiriad::close()
{
  // close the file
  uvclose_c(uv_handle_p);
}

