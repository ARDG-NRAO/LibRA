//# ATCAFiller.h: Definition for ATCA filler 
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
//# $Id: ATCAFiller.h,v 1.6 2004/12/13 04:44:35 mwiering Exp $

#ifndef ATNF_ATCAFILLER_H
#define ATNF_ATCAFILLER_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/tables/DataMan/TiledDataStManAccessor.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>

#include <casacore/casa/namespace.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class ATCAFiller {
public:

  // Construct the ATCAFiller
  ATCAFiller();

  ~ATCAFiller();

  // return the ATCA required tabledesc, this is the standard MeasurementSet
  // plus ATCA specific additions.
  static casacore::TableDesc atcaTableDesc(casacore::Bool compress);

  // make the ATCA specific MeasurementSet
  static casacore::MeasurementSet makeTable(const casacore::String& tableName, casacore::Bool compress,
    casacore::Bool cabb);

  // make the subtables with ATCA specific additions
  static void makeSubTables(casacore::MS& ms, casacore::Table::TableOption option, casacore::Bool cabb);

  // Open the measurement set and one or more RPFits files.
  // Opacity correction is not yet implemented - could go
  // in gencal or here, if gencal scheme doesn't improve
  casacore::Bool open(const casacore::String& msName, const casacore::Vector<casacore::String>& rpfitsFiles,
	          const casacore::Vector<casacore::String> & options, casacore::Int opcor=2);

 // Fill the measurement set
  casacore::Bool fill();
 
  // Select a number of fields by name.
  ATCAFiller & fields(const casacore::Vector<casacore::String> & fieldList);

  // Select a range of frequencies, lowFreq=0 => everything below higFreq,
  //  highFreq=0 => everything above lowFreq.
  ATCAFiller & freqRange(casacore::Double lowFreq, casacore::Double highFreq=0);
 
  // Select frequencies within windowWidth of specified ones.
  // (This selects on center-frequencies only, not channelfrequencies)
  ATCAFiller & frequencies(casacore::Vector<casacore::Double> freqs, casacore::Double windowWidth=1e6);

  // Select a range of scans to read. first=0 or 1 => start at first one,
  //   last=0 => read to end of file.
  ATCAFiller & scanRange(casacore::Int firstScan, casacore::Int lastScan=0);

  // Select range of channels, with optional increment.
  // We may want multiple channel ranges -> use matrix(3,n) for selection?
  ATCAFiller & chanRange(casacore::Int firstChan, casacore::Int lastChan, casacore::Int ChanInc=1);

  // casacore::Time range selection.
  ATCAFiller & timeRange(casacore::Double firstTime, casacore::Double lastTime=0);

  // Select the Freq (which one of the simult. freqs), 0=> no selection.
  // 
  ATCAFiller & freqSel(const casacore::Vector<casacore::Int>& spws); 

  // Select on bandwidth of IF 1.
  ATCAFiller & bandwidth1(casacore::Int bandwidth1);

  // Select on number of channels of IF 1
  ATCAFiller & numChan1(casacore::Int numchan1);

  // Set shadowing limit
  ATCAFiller & shadow(casacore::Float diam);

  // Deselect antennas.
  ATCAFiller & deselectAntenna(casacore::Vector<casacore::Int> antennas);

  // casacore::Smooth xy-phases with running median and 
  // flag data with discrepant xy-phase.
  ATCAFiller & xyPhaseSmooth(casacore::Int window=9, casacore::Double tolerance=10.0);

  // casacore::Smooth Tsys values with running median and recalibrate the data.
  ATCAFiller & tsysSmooth(casacore::Int window=9);
  
  // Set percentage of channels to flag at band edges
  ATCAFiller & edge(casacore::Float edge);

  // Calculate zenith opacities using the ASAP/Miriad model given surface weather conditions and
  // observatory height. Units are: fGHz [GHz], tempK [Kelvin], humi [%], press [hPa], height [m]
  casacore::Vector<casacore::Double> opacities(
    casacore::Vector<casacore::Double> fGHz, casacore::Float tempK,casacore::Float humi,
    casacore::Float press,casacore::Float height);

private:
  //# disallow all these
  ATCAFiller(const ATCAFiller &);
  ATCAFiller & operator=(const ATCAFiller &);

  //for constructors
  void init();

  // When storing header items skip irrelevant items for the last (empty) scan,
  // it is just there to capture caobs commands and comments
  void storeHeader(Bool last=False);
  void storeATCAHeader(Bool last=False);
  void storeSysCal();

  // fill a single input file
  casacore::Bool fill1(const casacore::String & rpfitsname);

  // casacore::List the file on cout
  void list();

   // Flag data if samplerstats are bad.
  casacore::Bool samplerFlag(casacore::Int row, casacore::Double posNegTolerance=3.0, 
                   casacore::Double zeroTolerance=0.5);
  
  casacore::Int birdChan(casacore::Double refFreq, casacore::Int refChan, casacore::Double chanSpac);
  void reweight();
  void storeData();
  casacore::Int checkSpW(casacore::Int ifNumber,casacore::Bool log=true);
  void checkField();
  void checkObservation();
  // Fill the feed table (with dummy values)
  void fillFeedTable();
  void fillObservationTable();
  void fillMeasureReferences();
  casacore::Bool selected(casacore::Int ifNum);
  void listScan(casacore::Double & mjd, casacore::Int scan, casacore::Double ut);
  casacore::Bool checkCABB(const casacore::String & rpfitsname);
  void rfiFlag(casacore::Matrix<casacore::Bool> & flags);
  

  casacore::String atcaPosToStation(casacore::Vector<casacore::Double>& xyz);
  void flush();
  void unlock();

  void shadow(casacore::Int row, casacore::Bool last=false);

  // Constants
  // MaxNPol needs to agree with if_cstok in RPFITS.h
  enum{MaxNChan=16385, MaxNPol=4};
  // Data
  casacore::MeasurementSet atms_p;
  casacore::MSColumns *msc_p;

  // Filenames
  casacore::Vector<casacore::String> rpfitsFiles_p;
  casacore::Int opcor_p;
  casacore::String currentFile_p;

  // The following should be constant throughout the rpfits file
  casacore::Int nAnt_p; 

  // Number of scans seen; #SpWs, #fields stored sofar
  casacore::Int nScan_p, nSpW_p, nField_p;
  // #scanheaders stored, index into casacore::MS SpW and Field Tables for current data
  casacore::Int scanNo_p, spWId_p, fieldId_p, prev_fieldId_p, obsId_p;

  // Bools
  casacore::Bool gotAN_p; //have we got an antenna casacore::Table yet?
  casacore::Bool appendMode_p;
  casacore::Bool storedHeader_p;
  casacore::Bool skipScan_p;
  casacore::Bool skipData_p;
  casacore::Bool firstHeader_p;
  casacore::Bool listHeader_p;
  casacore::uInt fileSize_p;
  casacore::Bool eof_p;
  casacore::Bool birdie_p;   // flag birdie channels
  casacore::Bool reweight_p; // gibbs reweighting
  casacore::Bool noxycorr_p; // do not apply xy phase correction
  casacore::Bool noac_p;    // Don't load the autocorrelation data
  casacore::Int obsType_p; // the type of observation: 0= standard, 1= fastmosaic
  casacore::Bool hires_p; // transform binned data into high time res data
  casacore::Bool init_p; // Are we initialized yet?
  casacore::Bool cabb_p; // Are we reading CABB data?

  // rpfits data
  casacore::Float vis[2*MaxNPol*MaxNChan];
  casacore::Float weight[MaxNPol*MaxNChan];
  casacore::Int baseline, flg, bin, if_no, sourceno; //index into rpfits casacore::Table(not casacore::MS) 
  casacore::Float ut, u, v, w;

  // storage manager accessor
  casacore::TiledDataStManAccessor dataAccessor_p,sigmaAccessor_p,flagAccessor_p,
    flagCatAccessor_p;
  casacore::TiledDataStManAccessor modelDataAccessor_p,corrDataAccessor_p,imWtAccessor_p;

  // Column objects to access Tables

  // colXXX objects are ATCA specific columns in the casacore::MeasurementSet 
  casacore::ScalarColumn<casacore::Int> colSysCalIdAnt1, colSysCalIdAnt2;
  casacore::ScalarColumn<casacore::Float> colXYAmplitude,colTrackErrMax,colTrackErrRMS,
      colWeatherSeeMonPhase,colWeatherSeeMonRMS,colWeatherRainGauge;
  casacore::ScalarColumn<casacore::Int> colSamplerBits;
  casacore::ArrayColumn<casacore::Float> colSamplerStatsNeg, colSamplerStatsZero,
      colSamplerStatsPos;
  casacore::ArrayColumn<casacore::Float> colGTP, colSDO, colCalJy;
  casacore::ScalarColumn<casacore::Bool> colWeatherSeeMonFlag;
  // ATCA_SCAN_INFO columns & table
  casacore::ScalarColumn<casacore::Int> colScanInfoAntId, colScanInfoScanId, colScanInfoSpWId,
       colScanInfoCacal;
  casacore::ArrayColumn<casacore::Int> colScanInfoFine, colScanInfoCoarse, colScanInfommAtt;
  casacore::ArrayColumn<casacore::Float> colScanInfoSubreflector;
  casacore::ScalarColumn<casacore::String> colScanInfoCorrConfig, colScanInfoScanType,
      colScanInfoCoordType,colScanInfoPointInfo;
  casacore::ScalarColumn<casacore::Bool> colScanInfoLineMode;
  casacore::Table msScanInfo_p;
  casacore::Matrix<casacore::Float> pointingCorr_p;
  casacore::Bool newPointingCorr_p;

  // reference date
  casacore::Double mjd0_p;

  // variables to keep the state of the sysCal search & binning state
  casacore::Vector<casacore::Int> sysCalId_p;
  casacore::Float lastUT_p;
  casacore::Int lastSpWId_p;
  casacore::Bool gotSysCalId_p;
  casacore::Float lastWeatherUT_p;
  casacore::Int errCount_p;

  // Selection parameters
  casacore::Vector<casacore::String> fieldSelection_p;
  casacore::Double lowFreq_p, highFreq_p;
  casacore::Vector<casacore::Double> freqs_p;
  casacore::Double windowWidth_p;
  casacore::Int firstScan_p, lastScan_p;
  casacore::Int firstChan_p, lastChan_p;
  casacore::Double firstTime_p, lastTime_p;
  casacore::Vector<casacore::Int> spws_p;
  casacore::Int bandWidth1_p, numChan1_p;
  casacore::Vector<casacore::Int> baselines_p, antennas_p;

  // Track sources
  casacore::String sources_p;
  casacore::Int nsources_p;
  
  // Polarization order
  casacore::Matrix<casacore::Int> corrIndex_p;

  // Check for shadowing
  casacore::Float shadow_p;
  casacore::Float edge_p;
  casacore::Block<casacore::Int> rowCache_p;
  casacore::Int nRowCache_p;
  casacore::Double prevTime_p;
  
  // Flagging
  casacore::Bool autoFlag_p,flagScanType_p;
  enum {COUNT=0, FLAG, ONLINE, SCANTYPE, SYSCAL, SHADOW, NFLAG};
  casacore::Vector<casacore::Int> flagCount_p;

  // Logger
  casacore::LogIO os_p;
      
};

} //# NAMESPACE CASA - END


#endif
