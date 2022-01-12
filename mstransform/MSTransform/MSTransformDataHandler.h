//# MSTransformDataHandler.h: This file contains the interface definition of the MSTransformDataHandler class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef MSTransformDataHandler_H_
#define MSTransformDataHandler_H_

// Measurement Set
#include <ms/MeasurementSets.h>

// Measurement Set Selection
#include <ms/MSSel/MSSelection.h>
#include <ms/MSSel/MSSelectionTools.h>

// Needed by setupMS
#include <tables/Tables.h>
#include <ms/MeasurementSets/MSTileLayout.h>

// Needed by copyTable
#include <tables/Tables/PlainTable.h>

// OS methods needed by fillSubTables
#include <casa/OS/Timer.h>
#include <casa/OS/Path.h>
#include <casa/OS/Directory.h>


namespace casa { //# NAMESPACE CASA - BEGIN

//  MSTransformDataHandler definition
class MSTransformDataHandler
{

public:

	enum asdmStManUseAlternatives
	{
		DONT,
		USE_FOR_DATA,
		USE_FOR_DATA_WEIGHT_SIGMA_FLAG
	};

	MSTransformDataHandler(const casacore::String& theMS, casacore::Table::TableOption option,
                               bool virtualModelCol=false, bool virtualCorrectedCol=false,
                               bool reindex=true);
	MSTransformDataHandler(const casacore::MeasurementSet& ms,
                               bool virtualModelCol=false, bool virtualCorrectedCol=false,
                               bool reindex=true);
	~MSTransformDataHandler();

	// Declared static because it's used in setupMS().
	// colNameList is internally up-cased, so it is not const or passed by reference.
	static const casacore::Vector<casacore::MS::PredefinedColumns>& parseColumnNames(casacore::String colNameList);

	// This version uses the casacore::MeasurementSet to check what columns are present,
	// i.e. it makes col=="all" smarter, and it is not necessary to call
	// verifyColumns() after calling this.  Unlike the other version, it knows
	// about FLOAT_DATA and LAG_DATA.  It throws an exception if a
	// _specifically_ requested column is absent.
	static const casacore::Vector<casacore::MS::PredefinedColumns>& parseColumnNames(
                     casacore::String colNameList, bool produceModel,
                     const casacore::MeasurementSet& ms, bool virtualModelCol=false,
                     bool virtualCorrectedCol=false);

	// Helper function for parseColumnNames().  Converts col to a list of
	// casacore::MS::PredefinedColumnss, and returns the # of recognized data columns.
	// static because parseColumnNames() is static.
	static casacore::uInt dataColStrToEnums(const casacore::String& col,casacore::Vector<casacore::MS::PredefinedColumns>& colvec);

	// Selection method using msselection syntax. casacore::Time is not handled by this method.
	bool setmsselect(	const casacore::String& spw = "", const casacore::String& field = "",
						const casacore::String& baseline = "", const casacore::String& scan = "",
						const casacore::String& uvrange = "", const casacore::String& taql = "",
						const casacore::Vector<casacore::Int>& step = casacore::Vector<casacore::Int> (1, 1),
						const casacore::String& subarray = "", const casacore::String& correlation = "",
						const casacore::String& intent = "", const casacore::String& obs = "",
                        const casacore::String& feed = "");

	// Select source or field
	bool selectSource(const casacore::Vector<casacore::Int>& fieldid);

	// Select spw and channels for each spw.
	bool selectSpw(const casacore::String& spwstr, const casacore::Vector<casacore::Int>& steps);

	// Returns the set (possibly empty) of spectral windows that are
	// in SPW but not listed in ms's DATA_DESCRIPTION sub-table.
	// (This happens with certain calibration/hardware setups.)
	static std::set<casacore::Int> findSpwsNotInDD(casacore::MeasurementSet& ms, casacore::Vector<casacore::Int> spwv);

	// Select Antennas to split out
	void selectAntenna(const casacore::Vector<casacore::Int>& antennaids,const casacore::Vector<casacore::String>& antennaSel);

	// Helper function for selectAntenna()
	static bool pickAntennas(	casacore::Vector<casacore::Int>& selected_antennaids,
								casacore::Vector<casacore::String>& selected_antenna_strs,
								const casacore::Vector<casacore::Int>& antennaids,
								const casacore::Vector<casacore::String>& antennaSel);

	// Select array IDs to use.
	void selectArray(const casacore::String& subarray);

	// Setup polarization selection (for now, only from available correlations - no casacore::Stokes transformations.)
	bool selectCorrelations(const casacore::String& corrstr);

	// Fills outToIn[pol] with a map from output correlation index to input
	// correlation index, for each input polID pol.
	// It does not yet check the appropriateness of the correlation selection
	// string, so ignore the return value for now.  outToIn[pol] defaults to
	// an empty casacore::Vector if no correlations are selected for pol.
	// That is not the same as the default "select everything in ms".
	static bool getCorrMaps(	casacore::MSSelection& mssel,
								const casacore::MeasurementSet& ms,
								casacore::Vector<casacore::Vector<casacore::Int> >& outToIn,
								const bool areSelecting = false);

	// Select time parameters
	void selectTime(casacore::Double timeBin=-1.0, casacore::String timerng="");

	//Method to make the basic structure of the MS
	//
	//TileShape of size 1 can have 2 values [0], and [1] ...these are used in to
	//determine the tileshape by using MSTileLayout. Otherwise it has to be a
	//vector size 3 e.g [4, 15, 351] => a tile shape of 4 stokes, 15 channels 351
	//rows.
	//
	// combine sets combine_p.  (Columns to ignore while time averaging.)
	//
        // createWeightSpectrumCols: add WEIGHT/SIGMA_SPECTRUM columns
	bool makeMSBasicStructure(casacore::String& msname,
                                            casacore::String& whichDataCol,
                                            bool produceModel,
                                            bool createWeightSpectrumCols,
                                            const casacore::Vector<casacore::Int>& tileShape = casacore::Vector<casacore::Int> (1, 0),
                                            const casacore::String& combine = "",
                                            casacore::Table::TableOption option=casacore::Table::New);

	bool isAllColumns(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

	// Method that returns the selected ms (?! - but it's boolean - RR)
	bool makeSelection();

	// This sets up a default new ms
	// Declared static as it can be (and is) called directly from outside
	// Therefore it is not dependent on any member variable.
        static casacore::MeasurementSet* setupMS(const casacore::String& msname, const casacore::Int nchan,
                                                 const casacore::Int npol, const casacore::String& telescop,
                                                 const casacore::Vector<casacore::MS::PredefinedColumns>& colNamesTok,
                                                 bool createWeightSpectrumCols,
                                                 const casacore::Int obstype = 0, const bool compress = false,
                                                 const asdmStManUseAlternatives asdmStManUse = DONT,
                                                 casacore::Table::TableOption option=casacore::Table::New);

	// Same as above except allowing manual tileshapes
        static casacore::MeasurementSet* setupMS(const casacore::String& msname, const casacore::Int nchan,
                                                 const casacore::Int npol, const casacore::Vector<casacore::MS::PredefinedColumns>& colNamesTok,
                                                 bool createWeightSpectrumCols,
                                                 const casacore::Vector<casacore::Int>& tileShape = casacore::Vector<casacore::Int> (1, 0),
                                                 const bool compress = false,
                                                 const asdmStManUseAlternatives asdmStManUse = DONT,
                                                 casacore::Table::TableOption option=casacore::Table::New);


	// The output casacore::MS must have (at least?) 1 of DATA, FLOAT_DATA, or LAG_DATA.
	// MODEL_DATA or CORRECTED_DATA will be converted to DATA if necessary.
	// jagonzal (CAS-5327): The implementation has to go here because a member function cannot have static linkage
	static bool mustConvertToData(	const casacore::uInt nTok,const casacore::Vector<casacore::MS::PredefinedColumns>& datacols)
	{
		return (nTok == 1) && (datacols[0] != casacore::MS::FLOAT_DATA) && (datacols[0] != casacore::MS::LAG_DATA);
	}

	// A customized version of casacore::MS::createDefaultSubtables().
	static void createSubtables(casacore::MeasurementSet& ms, casacore::Table::TableOption option);

	// Sub-table fillers.
	bool fillSubTables(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);
	bool fillFieldTable();
	bool fillDDTables(); // Includes spw and pol

	bool fillPolTable();
	bool fillDDITable();
	bool fillSPWTable();

	// Add optional columns to outTab if present in inTab and possColNames.
	// beLazy should only be true if outTab is in its default state.
	// Returns the number of added columns.
	static casacore::uInt addOptionalColumns(const casacore::Table& inTab, casacore::Table& outTab,const bool beLazy=false);

	// Sets up sourceRelabel_p for mapping input SourceIDs (if any) to output
	// ones.  Must be called after fieldid_p is set and before calling
	// fillFieldTable() or copySource().
	void relabelSources();

	// Adds and copies inTab to msOut_p without any filtering.
	// tabName is the table "type", i.e. POINTING or SYSPOWER
	// without the preceding path.
	//
	// If noRows is true, the structure will be setup but no
	// rows will be copied  (useful for filtering).
	void copySubtable(const casacore::String& tabName, const casacore::Table& inTab,const bool noRows = false);

	// Sets mapper to to a map from the distinct values of inv,
	// in increasing order, to 0, 1, 2, ..., mapper.size() - 1.
	static void make_map(std::map<casacore::Int, casacore::Int>& mapper, const casacore::Vector<casacore::Int>& inv);

	bool copyPointing();
	// Sets up the stub of a POINTING, enough to create an MSColumns.
	void setupNewPointing();

	bool copySource();
	bool copyAntenna();
	bool copyFeed();
	bool copyFlag_Cmd();
	bool copyHistory();
	bool copyObservation();
	bool copyProcessor();
	bool copyState();
	bool copySyscal();
	bool copyWeather();
	void copyMainTableKeywords (casacore::TableRecord& outKeys,
			const casacore::TableRecord& inKeys);

	casacore::Int getProcessorId(casacore::Int dataDescriptionId, casacore::String msname);

	// This falls between copyGenericSubtables() and the copiers for standard
	// sub-tables like copyFeed().  It is for optional sub-tables like CALDEVICE
	// and SYSPOWER which can be watched for by name and may need their
	// ANTENNA_ID and SPECTRAL_WINDOW_ID columns re-mapped.
	// (Technically FEED_ID, too, if split ever starts re-mapping feeds.)
	//
	// It must be called BEFORE copyGenericSubtables()!
	//
	bool filterOptSubtable(const casacore::String& subtabname);

	bool copyGenericSubtables();

	// To consolidate several sub-tables when dealing with MMS
	static bool mergeSpwSubTables(casacore::Vector<casacore::String> filenames);
	static bool mergeDDISubTables(casacore::Vector<casacore::String> filenames);
	static bool mergeFeedSubTables(casacore::Vector<casacore::String> filenames, casacore::Vector<casacore::uInt> mapSubmsSpwid);
	static bool mergeSourceSubTables(casacore::Vector<casacore::String> filenames, casacore::Vector<casacore::uInt> mapSubmsSpwid);
	static bool mergeSyscalSubTables(casacore::Vector<casacore::String> filenames, casacore::Vector<casacore::uInt> mapSubmsSpwid);
	static bool mergeFreqOffsetTables(casacore::Vector<casacore::String> filenames, casacore::Vector<casacore::uInt> mapSubmsSpwid);
	static bool mergeCalDeviceSubtables(casacore::Vector<casacore::String> filenames, casacore::Vector<casacore::uInt> mapSubmsSpwid);
	static bool mergeSysPowerSubtables(casacore::Vector<casacore::String> filenames, casacore::Vector<casacore::uInt> mapSubmsSpwid);
// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
	//template <class T>  bool MSTransformDataHandler::columnOk (casacore::ArrayColumn<T> column)
	template <class T>  static bool columnOk(casacore::ArrayColumn<T> column)
        {
	     bool ret;
	     // jagonzal (CAS-6206): ndimColumn only returns >0 is there is the array column has fixed size
	     if (column.isNull()==false and column.hasContent()==true and column.ndim(0) > 0)
	       {
		 ret = true;
	       }
	     else
	       {
		 ret = false;
	       }

	     return ret;
	}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
	//  template <class T>  bool MSTransformDataHandler::columnOk (casacore::ScalarColumn<T> column)
	template <class T>  static bool columnOk(casacore::ScalarColumn<T> column)
        {
	      bool ret;
	      if (column.isNull()==false and column.hasContent()==true)
		{
		  ret = true;
		}
	      else
		{
		  ret = false;
		}
	      
	      return ret;
	}



	// Accesors for the casacore::MS objects
	casacore::MeasurementSet * getInputMS() {return &ms_p;};
	casacore::MeasurementSet * getSelectedInputMS() {return &mssel_p;};
	casacore::MeasurementSet * getOutputMS() {return &msOut_p;};
	casacore::MSColumns * getSelectedInputMSColumns() {return mscIn_p;};
	casacore::MSColumns * getOutputMSColumns() {return msc_p;};

	// Accesors for the Re-mapper objects
	std::map<casacore::Int, casacore::Int> & getStateRemapper() {return stateRemapper_p;};
	casacore::Vector<casacore::Int> & getAntennaRemapper() {return antNewIndex_p;};
	std::map<casacore::Int, std::vector<casacore::Int>> & getDroppedChannelsMap() {return spwDropChannelMap_p;};
	std::map<casacore::Int,std::map < casacore::Int, std::vector<casacore::Int> > > & getSelectedChannelsMap() {return spwSelectedChannelMap_p;};

	// Accesors for additional parameters
	void setVirtualModelCol(bool virtualModelCol) {virtualModelCol_p = virtualModelCol;};
	void setVirtualCorrectedCol(bool virtualCorrectedCol) {virtualCorrectedCol_p = virtualCorrectedCol;};
	void setReindex(bool reindex) {reindex_p = reindex;};

protected:
	// copy ephemeris table and reindex field table if requested
	bool copyEphemerisTable(casacore::MSFieldColumns & msField);

	// Initialized* by ctors.  (Maintain order both here and in ctors.)
	//  * not necessarily to anything useful.
	casacore::MeasurementSet ms_p, mssel_p;
	casacore::MSColumns * msc_p; // columns of msOut_p
	casacore::MSColumns * mscIn_p;
	bool keepShape_p, // Iff true, each output array has the
			// same shape as the corresponding input one.
			// sameShape_p,             // Iff true, the shapes of the arrays do not
			//  			// vary with row number.
			antennaSel_p; // Selecting by antenna?
	casacore::Double timeBin_p;
	casacore::String scanString_p, // Selects scans by #number#.  Historically named.
			intentString_p, // Selects scans by string.  scanString_p was taken.
			obsString_p, // casacore::String for observationID selection.
			uvrangeString_p, taqlString_p, feedString_p;
	casacore::String timeRange_p, arrayExpr_p, corrString_p, spwString_p;
	casacore::String combine_p; // Should time averaging not split bins by
	// scan #, observation, and/or state ID?
	// Must be lowercase at all times.
	casacore::Int fitorder_p; // The polynomial order for continuum fitting.
	// If < 0 (default), continuum subtraction is
	// not done.
	casacore::String fitspw_p; // Selection string for line-free channels.
	casacore::String fitoutspw_p; // Selection string for output channels if doing
	// continuum subtraction.

	// Uninitialized by ctors.
	casacore::MeasurementSet msOut_p;
	casacore::Vector<casacore::Int> spw_p, // The input spw corresponding to each output spw.
			spw_uniq_p, // Uniquified version of spw_p.
			 nchan_p, // The # of output channels for each range.
			totnchan_p, // The # of output channels for each output spw.
			chanStart_p, // 1st input channel index in a selection.
			chanEnd_p, // last input channel index in a selection.
			chanStep_p, // Increment between input chans, i.e. if 3, only every third
			// input channel will be used.
			widths_p, // # of input chans per output chan for each range.
			ncorr_p, // The # of output correlations for each DDID.
			inNumChan_p, // The # of input channels for each spw.
			inNumCorr_p; // The # of input correlations for each DDID.

	std::map<casacore::Int, std::vector<casacore::Int> > spwDropChannelMap_p;
	std::map<casacore::Int, std::map < casacore::Int, std::vector<casacore::Int> > > spwSelectedChannelMap_p;

	casacore::Vector<casacore::Int> fieldid_p;
	casacore::Vector<casacore::Int> spwRelabel_p, fieldRelabel_p, sourceRelabel_p;
	casacore::Vector<casacore::Int> oldDDSpwMatch_p;
	casacore::Vector<casacore::String> antennaSelStr_p;
	casacore::Vector<casacore::Int> antennaId_p;
	casacore::Vector<casacore::Int> antIndexer_p;
	casacore::Vector<casacore::Int> antNewIndex_p;

	casacore::Vector<casacore::Int> selObsId_p; // casacore::List of selected OBSERVATION_IDs.
	casacore::Vector<casacore::Int> polID_p; // casacore::Map from input DDID to input polID, filled in fillDDTables().
	casacore::Vector<casacore::Int> spw2ddid_p;

	// inCorrInd = outPolCorrToInCorrMap_p[polID_p[ddID]][outCorrInd]
	casacore::Vector<casacore::Vector<casacore::Int> > inPolOutCorrToInCorrMap_p;

	std::map<casacore::Int, casacore::Int> stateRemapper_p;

	casacore::Vector<casacore::Vector<casacore::Slice> > chanSlices_p; // Used by VisIterator::selectChannel()
	casacore::Vector<casacore::Slice> corrSlice_p;
	casacore::Vector<casacore::Vector<casacore::Slice> > corrSlices_p; // Used by VisIterator::selectCorrelation()
	casacore::Matrix<casacore::Double> selTimeRanges_p;

	bool virtualModelCol_p; // CAS-5348 (jagonzal): Make virtual MODEL data column real
	bool virtualCorrectedCol_p; //CAS-7286 (jagonzal): Make virtual CORRECTED data column real
	bool reindex_p; // jagonzal: In order not to re-index asub-tables

};

} //# NAMESPACE CASA - END

#endif /* MSTransformDataHandler_H_ */
