//# SubMS.h: this defines SubMS which creates a subset of an casacore::MS with some
//# transformation
//# Copyright (C) 1997,1998,1999,2000,2001,2003
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
//#
//# $Id$
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MSMainEnums.h>
//#include <msvis/MSVis/VisIterator.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/tables/Tables/Table.h>
//#include <casa/Utilities/CountedPtr.h>
#include <map>
#include <set>
#include <vector>
#include <casacore/scimath/Mathematics/InterpolateArray1D.h>

#include <mstransform/MSTransform/MSTransformManager.h>


#ifndef MSVIS_SUBMS_H

namespace casacore{

class MSSelection; // #include <ms/MSSel/MSSelection.h>
template<class T> class ArrayColumn;
class Table;
}

namespace casa { //# NAMESPACE CASA - BEGIN

#define MSVIS_SUBMS_H

namespace subms {
// Returns wt**-0.5 or -1, depending on whether wt is positive.
// NOT a member function, so it can be easily passed to other functions
// (i.e. arrayTransformInPlace).
double wtToSigma(casacore::Double wt);
double sigToWeight(casacore::Double sig);
}

// <summary>
// SubMS provides functionalities to make a subset of an existing MS
// </summary>

// <visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MeasurementSet
// </prerequisite>
//
// <etymology>
// SubMS ...from the SUBset of an MS
// </etymology>
//
// <synopsis>
// The order of operations (as in ms::split()) is:
//      ctor
//      setmsselect
//      selectTime
//      makeSubMS
// </synopsis>

// These forward declarations are so the corresponding .h files don't have to
// be included in this .h file, but it's only worth it if a lot of other files
// include this file.
class VBRemapper;

  // // These typedefs are necessary because a<b::c> doesn't work.
  // typedef std::vector<casacore::uInt> uivector;
  // struct uIntCmp 
  // {
  //   bool operator()(const casacore::uInt i1, const casacore::uInt i2) const 
  //   {
  //     return i1 < i2;
  //   }
  // };
  // typedef std::map<const casacore::uInt, uivector, uIntCmp> ui2vmap;

  casacore::Bool isAllColumns(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

class SubMS
{

 // jagonzal: Allow TransformVisDataHandler to access protected methods and members of this class
 friend class MSTransformManager;

 public:

  enum RegriddingAlternatives {
    useFFTShift = -100,   // needs input and output grid to have the same number of channels and be equidistant in freq.
    useLinIntThenFFTShift // for the case the input grid is not equidistant in frequency but the output grid is
  };

  enum asdmStManUseAlternatives {
    DONT,
    USE_FOR_DATA,
    USE_FOR_DATA_WEIGHT_SIGMA_FLAG
  };

  SubMS(casacore::String& theMS, casacore::Table::TableOption option = casacore::Table::Old);
  
  // construct from an MS
  SubMS(casacore::MeasurementSet& ms);

  virtual ~SubMS();
  
  // Change or Set the casacore::MS this casacore::MSSelector refers to.
  void setMS(casacore::MeasurementSet& ms);

  // Returns the set (possibly empty) of spectral windows that are in spwv but
  // not listed in ms's DATA_DESCRIPTION subtable.  (This happens with certain
  // calibration/hardware setups.)
  static std::set<casacore::Int> findBadSpws(casacore::MeasurementSet& ms, casacore::Vector<casacore::Int> spwv);

  // Select spw and channels for each spw.
  // This is the version used by split.  It returns true on success and false
  // on failure.
  casacore::Bool selectSpw(const casacore::String& spwstr, const casacore::Vector<casacore::Int>& steps);

  // This older version is used by the older version of setmsselect().
  void selectSpw(casacore::Vector<casacore::Int> spw, casacore::Vector<casacore::Int> nchan, casacore::Vector<casacore::Int> start, 
                 casacore::Vector<casacore::Int> step);
  
  // Setup polarization selection (for now, only from available correlations -
  // no casacore::Stokes transformations.)
  casacore::Bool selectCorrelations(const casacore::String& corrstr);

  //select casacore::Time and time averaging or regridding
  //void selectTime();

  //select stuff using msselection syntax ...time is left out
  // call it separately with timebin
  // This version returns a success value, and does not need nchan, start, and
  // step.  It is used by split.
  casacore::Bool setmsselect(const casacore::String& spw="", const casacore::String& field="", 
		   const casacore::String& baseline="", const casacore::String& scan="",
                   const casacore::String& uvrange="", const casacore::String& taql="", 
		   const casacore::Vector<casacore::Int>& step=casacore::Vector<casacore::Int> (1,1),
		   const casacore::String& subarray="", const casacore::String& correlation="",
                   const casacore::String& intent="", const casacore::String& obs="");

  // This older version does not return a success value, and does need nchan,
  // start, and step.  It is used elsewhere (i.e. ImagerMultiMS).
  void setmsselect(const casacore::String& spw,        const casacore::String& field, 
                   const casacore::String& baseline,   const casacore::String& scan,
                   const casacore::String& obs,        const casacore::String& uvrange,
                   const casacore::String& taql,       const casacore::Vector<casacore::Int>& nchan,
                   const casacore::Vector<casacore::Int>& start, const casacore::Vector<casacore::Int>& step,
                   const casacore::String& subarray, const casacore::String& intent="");

  // Select source or field
  casacore::Bool selectSource(const casacore::Vector<casacore::Int>& fieldid);
  
  // Select Antennas to split out  
  void selectAntenna(const casacore::Vector<casacore::Int>& antennaids,
		     const casacore::Vector<casacore::String>& antennaSel)
  {
    antennaSel_p = pickAntennas(antennaId_p, antennaSelStr_p,
				antennaids, antennaSel);
  }
  static casacore::Bool pickAntennas(casacore::Vector<casacore::Int>& selected_antennaids,
			   casacore::Vector<casacore::String>& selected_antenna_strs,
			   const casacore::Vector<casacore::Int>& antennaids,
			   const casacore::Vector<casacore::String>& antennaSel);
  
  // Select array IDs to use.
  void selectArray(const casacore::String& subarray) {arrayExpr_p = subarray;}

  //select time parameters
  void selectTime(casacore::Double timeBin=-1.0, casacore::String timerng="");

  //void selectSource(casacore::Vector<casacore::String> sourceid);

  //Method to set if a phase Center rotation is needed
  //void setPhaseCenter(casacore::Int fieldid, casacore::MDirection& newPhaseCenter);

  // Sets the polynomial order for continuum fitting to fitorder.
  // If < 0, continuum subtraction is not done.
  void setFitOrder(casacore::Int fitorder, casacore::Bool advise=true);
  // Set the selection string for line-free channels.
  void setFitSpw(const casacore::String& fitspw) {fitspw_p = fitspw;}
  // Selection string for output channels if doing continuum subtraction.
  void setFitOutSpw(const casacore::String& fitoutspw) {fitoutspw_p = fitoutspw;}

  //Method to make the subMS
  //
  //TileShape of size 1 can have 2 values [0], and [1] ...these are used in to
  //determine the tileshape by using MSTileLayout. Otherwise it has to be a
  //vector size 3 e.g [4, 15, 351] => a tile shape of 4 stokes, 15 channels 351
  //rows.
  //
  // combine sets combine_p.  (Columns to ignore while time averaging.)
  //
  casacore::Bool makeSubMS(casacore::String& submsname, casacore::String& whichDataCol,
                 const casacore::Vector<casacore::Int>& tileShape=casacore::Vector<casacore::Int>(1, 0),
                 const casacore::String& combine="");

  //Method to make a scratch subMS and even in memory if posssible
  //Useful if temporary subselection/averaging is necessary
  // It'll be in memory if the basic output ms is less than half of 
  // memory reported by casacore::HostInfo unless forced to by user...
  virtual casacore::MeasurementSet* makeScratchSubMS(const casacore::Vector<casacore::MS::PredefinedColumns>& whichDataCols, 
				   const casacore::Bool forceInMemory=false);
  // In this form whichDataCol gets passed to parseColumnNames().
  virtual casacore::MeasurementSet* makeScratchSubMS(const casacore::String& whichDataCol, 
				   const casacore::Bool forceInMemory=false);

  // This sets up a default new ms
  // Declared static as it can be (and is) called directly outside of SubMS.
  // Therefore it is not dependent on any member variable.
  static casacore::MeasurementSet* setupMS(const casacore::String& msname, const casacore::Int nchan,
                                 const casacore::Int npol, const casacore::String& telescop,
                                 const casacore::Vector<casacore::MS::PredefinedColumns>& colNamesTok,
				 const casacore::Int obstype=0,
                                 const casacore::Bool compress=false,
				 const asdmStManUseAlternatives asdmStManUse=DONT);

  // Same as above except allowing manual tileshapes
  static casacore::MeasurementSet* setupMS(const casacore::String& msname, const casacore::Int nchan,
                                 const casacore::Int npol,
                                 const casacore::Vector<casacore::MS::PredefinedColumns>& colNamesTok,
				 const casacore::Vector<casacore::Int>& tileShape=casacore::Vector<casacore::Int>(1,0),
                                 const casacore::Bool compress=false,
				 const asdmStManUseAlternatives asdmStManUse=DONT);

  
  // Add optional columns to outTab if present in inTab and possColNames.
  // beLazy should only be true if outTab is in its default state.
  // Returns the number of added columns.
  static casacore::uInt addOptionalColumns(const casacore::Table& inTab, casacore::Table& outTab,
                                 const casacore::Bool beLazy=false);

  // Like casacore::TableCopy::copyRows, but by column.
  static casacore::Bool copyCols(casacore::Table& out, const casacore::Table& in, const casacore::Bool flush=true);

  // A customized version of casacore::MS::createDefaultSubtables().
  static void createSubtables(casacore::MeasurementSet& ms, casacore::Table::TableOption option);

  // Declared static because it's used in setupMS().  Therefore it can't use
  // any member variables.  It is also used in MSFixvis.cc.
  // colNameList is internally upcased, so it is not const or passed by reference.
  static const casacore::Vector<casacore::MS::PredefinedColumns>& parseColumnNames(casacore::String colNameList);
  // This version uses the casacore::MeasurementSet to check what columns are present,
  // i.e. it makes col=="all" smarter, and it is not necessary to call
  // verifyColumns() after calling this.  Unlike the other version, it knows
  // about FLOAT_DATA and LAG_DATA.  It throws an exception if a
  // _specifically_ requested column is absent.
  static const casacore::Vector<casacore::MS::PredefinedColumns>& parseColumnNames(casacore::String colNameList,
                                                    const casacore::MeasurementSet& ms);

  void verifyColumns(const casacore::MeasurementSet& ms, const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

  // The output casacore::MS must have (at least?) 1 of DATA, FLOAT_DATA, or LAG_DATA.
  // MODEL_DATA or CORRECTED_DATA will be converted to DATA if necessary.
  static casacore::Bool mustConvertToData(const casacore::uInt nTok,
                                const casacore::Vector<casacore::MS::PredefinedColumns>& datacols)
  {
    return (nTok == 1) && (datacols[0] != casacore::MS::FLOAT_DATA) &&
      (datacols[0] != casacore::MS::LAG_DATA);
  }

  static casacore::Bool sepFloat(const casacore::Vector<casacore::MS::PredefinedColumns>& anyDataCols,
                       casacore::Vector<casacore::MS::PredefinedColumns>& complexDataCols);

  // Fills outToIn[pol] with a map from output correlation index to input
  // correlation index, for each input polID pol.
  // It does not yet check the appropriateness of the correlation selection
  // string, so ignore the return value for now.  outToIn[pol] defaults to
  // an empty casacore::Vector if no correlations are selected for pol.
  // That is not the same as the default "select everything in ms".
  static casacore::Bool getCorrMaps(casacore::MSSelection& mssel, const casacore::MeasurementSet& ms,
			  casacore::Vector<casacore::Vector<casacore::Int> >& outToIn,
			  const casacore::Bool areSelecting=false);
  
  // Replaces col[i] with mapper[col[i]] for each element of col.
  // Does NOT check whether mapper[col[i]] is defined, but it does return
  // right away (i.e. a no-op) if mapper is empty.
  static void remap(casacore::Vector<casacore::Int>& col, const casacore::Vector<casacore::Int>& mapper);
  static void remap(casacore::Vector<casacore::Int>& col, const std::map<casacore::Int, casacore::Int>& mapper);

  // Transform spectral data to different reference frame,
  // optionally regrid the frequency channels 
  // return values: -1 = casacore::MS not modified, 1 = casacore::MS modified and OK, 
  // 0 = casacore::MS modified but not OK (i.e. casacore::MS is probably damaged) 
  casacore::Int regridSpw(casacore::String& message, // returns the casacore::MS history entry 
		const casacore::String& outframe="", // default = "keep the same"
		const casacore::String& regridQuant="chan",
		const casacore::Double regridVeloRestfrq=-3E30, // default = "not set" 
		const casacore::String& regridInterpMeth="LINEAR",
		const casacore::Double regridCenter=-3E30, // default = "not set" 
		const casacore::Double regridBandwidth=-1., // default = "not set" 
		const casacore::Double regridChanWidth=-1., // default = "not set" 
		const casacore::Bool doHanningSmooth=false,
		const casacore::Int phaseCenterFieldId=-2, // -2 = use pahse center from field table
		casacore::MDirection phaseCenter=casacore::MDirection(), // this direction is used if phaseCenterFieldId==-1
		const casacore::Bool centerIsStart=false, // if true, the parameter regridCenter specifies the start
		const casacore::Bool startIsEnd=false, // if true, and centerIsStart is true, regridCenter specifies the upper end in frequency
		const casacore::Int nchan=0, // if >0: used instead of regridBandwidth, ==
		const casacore::Int width=0, // if >0 and regridQuant=="freq": used instead of regridChanWidth
		const casacore::Int start=-1 // if >=0 and regridQuant=="freq": used instead of regridCenter
		);

  // the following inline convenience methods for regridSpw bypass the whole CASA measure system
  // because when they are used, they can assume that the frame stays the same and the units are OK
  static casacore::lDouble vrad(const casacore::lDouble freq, const casacore::lDouble rest){ return (casacore::C::c * (1. - freq/rest)); };
  static casacore::lDouble vopt(const casacore::lDouble freq, const casacore::lDouble rest){ return (casacore::C::c *(rest/freq - 1.)); };
  static casacore::lDouble lambda(const casacore::lDouble freq){ return (casacore::C::c/freq); };
  static casacore::lDouble freq_from_vrad(const casacore::lDouble vrad, const casacore::lDouble rest){ return (rest * (1. - vrad/casacore::C::c)); };
  static casacore::lDouble freq_from_vopt(const casacore::lDouble vopt, const casacore::lDouble rest){ return (rest / (1. + vopt/casacore::C::c)); };
  static casacore::lDouble freq_from_lambda(const casacore::lDouble lambda){ return (casacore::C::c/lambda); };
  
  // Support method for regridSpw():
  // results in the column oldName being renamed to newName, and a new column
  // which is an empty copy of oldName being created together with a
  // TileShapeStMan data manager and hypercolumn (name copied from the old
  // hypercolumn) with given dimension, the old hypercolumn of name
  // hypercolumnName is renamed to name+"B"
  casacore::Bool createPartnerColumn(casacore::TableDesc& modMSTD, const casacore::String& oldName,
                           const casacore::String& newName, const casacore::Int& hypercolumnDim,
                           const casacore::IPosition& tileShape);

  // Support method for regridSpw():
  // calculate the final new channel boundaries from the regridding parameters
  // and the old channel boundaries (already transformed to the desired
  // reference frame); returns false if input paramters were invalid and no
  // useful boundaries could be created
  static casacore::Bool regridChanBounds(casacore::Vector<casacore::Double>& newChanLoBound, 
			       casacore::Vector<casacore::Double>& newChanHiBound,
			       const casacore::Double regridCenter, 
			       const casacore::Double regridBandwidth,
			       const casacore::Double regridChanWidth,
			       const casacore::Double regridVeloRestfrq, 
			       const casacore::String regridQuant,
			       const casacore::Vector<casacore::Double>& transNewXin, 
			       const casacore::Vector<casacore::Double>& transCHAN_WIDTH,
			       casacore::String& message, // message to the user, epsecially in case of error 
			       const casacore::Bool centerIsStart=false, // if true, the parameter regridCenter specifies the start
			       const casacore::Bool startIsEnd=false, // if true, and centerIsStart is true, regridCenter specifies the upper end in frequency
			       const casacore::Int nchan=0, // if != 0 : used instead of regridBandwidth, -1 means use all channels
			       const casacore::Int width=0, // if >0 and regridQuant=="freq": used instead of regridChanWidth
			       const casacore::Int start=-1 // if >=0 and regridQuant=="freq": used instead of regridCenter
			       );

  // a helper function for handling the gridding parameter user input
  static casacore::Bool convertGridPars(casacore::LogIO& os,
			      const casacore::String& mode, 
			      const int nchan, 
			      const casacore::String& start, 
			      const casacore::String& width,
			      const casacore::String& interp, 
			      const casacore::String& restfreq, 
			      const casacore::String& outframe,
			      const casacore::String& veltype,
			      casacore::String& t_mode,
			      casacore::String& t_outframe,
			      casacore::String& t_regridQuantity,
			      casacore::Double& t_restfreq,
			      casacore::String& t_regridInterpMeth,
			      casacore::Double& t_cstart, 
			      casacore::Double& t_bandwidth,
			      casacore::Double& t_cwidth,
			      casacore::Bool& t_centerIsStart, 
			      casacore::Bool& t_startIsEnd,			      
			      casacore::Int& t_nchan,
			      casacore::Int& t_width,
			      casacore::Int& t_start);

  // A wrapper for SubMS::regridChanBounds() which takes the user interface type gridding parameters
  // The ready-made grid is returned in newCHAN_FREQ and newCHAN_WIDTH
  static casacore::Bool calcChanFreqs(casacore::LogIO& os,
			    // output
			    casacore::Vector<casacore::Double>& newCHAN_FREQ,
			    casacore::Vector<casacore::Double>& newCHAN_WIDTH,
			    // input
			    const casacore::Vector<casacore::Double>& oldCHAN_FREQ, // the original grid
			    const casacore::Vector<casacore::Double>& oldCHAN_WIDTH, 
			    // the gridding parameters
			    const casacore::MDirection  phaseCenter,
			    const casacore::MFrequency::Types theOldRefFrame,
			    const casacore::MEpoch theObsTime,
			    const casacore::MPosition mObsPos,
			    const casacore::String& mode, 
			    const int nchan, 
			    const casacore::String& start, 
			    const casacore::String& width,
			    const casacore::String& restfreq, 
			    const casacore::String& outframe,
			    const casacore::String& veltype,
			    const casacore::Bool verbose=false,
			    const casacore::MRadialVelocity mRV=casacore::MRadialVelocity() // additional radial velo shift to apply, 
                                                                        // used e.g. when outframe=="SOURCE"
			    );

  // Overloaded version of the above method returning the additional value weightScale
  // which is the factor by which WEIGHT needs to be scaled when transforming from
  // the old grid to the new grid.
  static casacore::Bool calcChanFreqs(casacore::LogIO& os,
			    // output
			    casacore::Vector<casacore::Double>& newCHAN_FREQ,
			    casacore::Vector<casacore::Double>& newCHAN_WIDTH,
			    casacore::Double& weightScale,
			    // input
			    const casacore::Vector<casacore::Double>& oldCHAN_FREQ, // the original grid
			    const casacore::Vector<casacore::Double>& oldCHAN_WIDTH, 
			    // the gridding parameters
			    const casacore::MDirection  phaseCenter,
			    const casacore::MFrequency::Types theOldRefFrame,
			    const casacore::MEpoch theObsTime,
			    const casacore::MPosition mObsPos,
			    const casacore::String& mode, 
			    const int nchan, 
			    const casacore::String& start, 
			    const casacore::String& width,
			    const casacore::String& restfreq, 
			    const casacore::String& outframe,
			    const casacore::String& veltype,
			    const casacore::Bool verbose=false,
			    const casacore::MRadialVelocity mRV=casacore::MRadialVelocity() // additional radial velo shift to apply, 
                                                                        // used e.g. when outframe=="SOURCE"
			    );

  // Support method for regridSpw():
  // if writeTables is false, the (const) input parameters are only verified, nothing is written;
  // return value is true if the parameters are OK.
  // if writeTables is true, the vectors are filled and the SPW, DD, and SOURCE tables are modified;
  // the return value in this case is true only if a successful modification (or none) took place
  casacore::Bool setRegridParameters(std::vector<casacore::Int>& oldSpwId,
			   std::vector<casacore::Int>& oldFieldId,
			   std::vector<casacore::Int>& newDataDescId,
			   std::vector<casacore::Bool>& regrid,
			   std::vector<casacore::Bool>& transform,
			   std::vector<casacore::MDirection>& theFieldDirV,
			   std::vector<casacore::MPosition>& mObsPosV,
			   std::vector<casacore::MFrequency::Types>& fromFrameTypeV,
			   std::vector<casacore::MFrequency::Ref>& outFrameV,
			   std::vector<casacore::MRadialVelocity>& outRadVelV,
			   std::vector<casacore::Double>& weightScaleV,
			   std::vector< casacore::Vector<casacore::Double> >& xold, 
			   std::vector< casacore::Vector<casacore::Double> >& xout, 
			   std::vector< casacore::Vector<casacore::Double> >& xin, 
			   vector< casacore::Int >& method, // interpolation method cast to Int
			   casacore::Bool& msMod,
			   const casacore::String& outframe,
			   const casacore::String& regridQuant,
			   const casacore::Double regridVeloRestfrq,
			   const casacore::String& regridInterpMeth,
			   const casacore::Double regridCenter, 
			   const casacore::Double regridBandwidth, 
			   const casacore::Double regridChanWidth,
			   const casacore::Int regridPhaseCenterFieldId, // -2 = take from field table, -1 = use 
			   const casacore::MDirection regridPhaseCenter, //    <- this value, >-1 = take from this field
			   const casacore::Bool writeTables,
			   casacore::LogIO& os,
			   casacore::String& regridMessage,
			   const casacore::Bool centerIsStart=false, // if true, the parameter regridCenter specifies the start
			   const casacore::Bool startIsEnd=false, // if true, and centerIsStart is true, regridCenter specifies the upper end in frequency
			   const casacore::Int nchan=0, // if >0: used instead of regridBandwidth
			   const casacore::Int width=0, // if >0 and regridQuant=="freq": used instead of regridChanWidth
			   const casacore::Int start=-1 // if >=0 and regridQuant=="freq": used instead of regridCenter
			   );

  // combineSpws():
  // make one spectral window from all spws given by the spwids vector
  casacore::Bool combineSpws(const casacore::Vector<casacore::Int>& spwids,  // casacore::Vector<casacore::Int>(1,-1) means: use all SPWs
		   const casacore::Bool noModify,   // if true, the casacore::MS will not be modified
		   casacore::Vector<casacore::Double>& newCHAN_FREQ, // will return the grid of the resulting SPW
		   casacore::Vector<casacore::Double>& newCHAN_WIDTH,
		   casacore::Bool verbose=false
		   );

  casacore::Bool combineSpws(const casacore::Vector<casacore::Int>& spwids = casacore::Vector<casacore::Int>(1,-1)){  // casacore::Vector<casacore::Int>(1,-1) means: use all SPWs
    casacore::Vector<casacore::Double> temp1; 
    casacore::Vector<casacore::Double> temp2;
    return combineSpws(spwids, false, temp1, temp2, true);
  }

  // Fills mapper[ntok] with a map from dataColumn indices to ArrayColumns in
  // the output.  mapper must have ntok slots!
  static void getDataColMap(casacore::MSColumns* msc, casacore::ArrayColumn<casacore::Complex>* mapper,
			    casacore::uInt ntok,
			    const casacore::Vector<casacore::MS::PredefinedColumns>& colEnums); 

  void setTVIDebug(bool debug) {tvi_debug = debug;}
  void setWantCont(bool want_cont) {want_cont_p = want_cont;}


 protected:

  //method that returns the selected ms (?! - but it's Boolean - RR)
  casacore::Bool makeSelection();

  // (Sub)table fillers.
  casacore::Bool fillAllTables(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);
  casacore::Bool fillDDTables();		// Includes spw and pol.
  casacore::Bool fillFieldTable();
  
  // Used to be fillMainTable(colnames), but what distinguishes it from
  // writeSomeMainRows(colnames) is that it is for cases where there is
  // a 1-1 match between rows in mssel_p and msOut_p (including order).
  casacore::Bool writeAllMainRows(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

  // Used to be fillAverMainTable(colnames), but what distinguishes it from
  // writeAllMainRows(colnames) is that it is for cases where there is not
  // necessarily a 1-1 match between rows in mssel_p and msOut_p.
  casacore::Bool writeSomeMainRows(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

  casacore::Bool copyAntenna();
  casacore::Bool copyFeed();
  casacore::Bool copyFlag_Cmd();
  casacore::Bool copyHistory();
  casacore::Bool copyObservation();
  casacore::Bool copyPointing();
  casacore::Bool copyProcessor();
  casacore::Bool copySource();
  casacore::Bool copyState();
  casacore::Bool copySyscal();
  casacore::Bool copyWeather();
  casacore::Bool copyGenericSubtables();
  void copyMainTableKeywords(casacore::TableRecord& outKeys,const casacore::TableRecord& inKeys);

  // This falls between copyGenericSubtables() and the copiers for standard
  // subtables like copyFeed().  It is for optional subtables like CALDEVICE
  // and SYSPOWER which can be watched for by name and may need their
  // ANTENNA_ID and SPECTRAL_WINDOW_ID columns remapped.  (Technically FEED_ID,
  // too, if split ever starts remapping feeds.)
  //
  // It must be called BEFORE copyGenericSubtables()!
  //
  casacore::Bool filterOptSubtable(const casacore::String& subtabname);

  //  casacore::Bool writeDiffSpwShape(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);
  casacore::Bool fillAccessoryMainCols();

  // *** Private member functions ***

  // Adds and copies inTab to msOut_p without any filtering.
  // tabName is the table "type", i.e. POINTING or SYSPOWER without the
  // preceding path.
  //
  // If noRows is true, the structure will be setup but no rows will be
  // copied.  (Useful for filtering)
  void copySubtable(const casacore::String& tabName, const casacore::Table& inTab,
                    const casacore::Bool noRows=false);

  casacore::Bool getDataColumn(casacore::ArrayColumn<casacore::Complex>& data,
                     const casacore::MS::PredefinedColumns colName);
  casacore::Bool getDataColumn(casacore::ArrayColumn<casacore::Float>& data,
                     const casacore::MS::PredefinedColumns colName);
  casacore::Bool putDataColumn(casacore::MSColumns& msc, casacore::ArrayColumn<casacore::Complex>& data,
                     const casacore::MS::PredefinedColumns datacol,
                     const casacore::Bool writeToDataCol=false);
  casacore::Bool putDataColumn(casacore::MSColumns& msc, casacore::ArrayColumn<casacore::Float>& data,
                     const casacore::MS::PredefinedColumns datacol,
                     const casacore::Bool writeToDataCol=false);

  // This method uses VisIter for efficient copy mode data transfer
  casacore::Bool copyDataFlagsWtSp(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames,
                         const casacore::Bool writeToDataCol);

  // Like doTimeAver(), but it subtracts the continuum instead of time
  // averaging.  Unlike doTimeAver(), it infers writeToDataCol from
  // colNames.nelements() (subtractContinuum is intentionally not as general as
  // copyDataFlagsWtSp), and writes according to fitoutspw_p instead of spw_p.
  casacore::Bool subtractContinuum(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames,
                         const VBRemapper& remapper);
  
  // Helper function for parseColumnNames().  Converts col to a list of
  // casacore::MS::PredefinedColumnss, and returns the # of recognized data columns.
  // static because parseColumnNames() is static.
  static casacore::uInt dataColStrToEnums(const casacore::String& col,
                                casacore::Vector<casacore::MS::PredefinedColumns>& colvec);
    
  casacore::Bool doChannelMods(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

  void fill_vbmaps(std::map<VisBufferComponents::EnumType,
                            std::map<casacore::Int, casacore::Int> >& vbmaps);

  // return the number of unique antennas selected
  //casacore::Int numOfBaselines(casacore::Vector<casacore::Int>& ant1, casacore::Vector<casacore::Int>& ant2,
  //                   casacore::Bool includeAutoCorr=false);

  // Used in a couple of places to estimate how much memory to grab.
  casacore::Double n_bytes() {return mssel_p.nrow() * nchan_p[0] * ncorr_p[0] *
                           sizeof(casacore::Complex);}

  // Picks a reference to DATA, MODEL_DATA, CORRECTED_DATA, or LAG_DATA out
  // of ms_p.  FLOAT_DATA is not included because it is not natively complex. 
  const casacore::ArrayColumn<casacore::Complex>& right_column(const casacore::MSColumns *ms_p,
                                             const casacore::MS::PredefinedColumns datacol);

  // The writable version of the above.
  casacore::ArrayColumn<casacore::Complex>& right_column(casacore::MSColumns *msclala,
				     const casacore::MS::PredefinedColumns col,
				     const casacore::Bool writeToDataCol);

  // Figures out the number, maximum, and index of the selected antennas.
  casacore::uInt fillAntIndexer(std::map<casacore::Int, casacore::Int>& antIndexer, const casacore::MSColumns *msc);

  // Read the input, time average it to timeBin_p, and write the output.
  // The first version uses VisibilityIterator (much faster), but the second
  // supports correlation selection using VisIterator.  VisIterator should be
  // sped up soon!
  casacore::Bool doTimeAver(const casacore::Vector<casacore::MS::PredefinedColumns>& dataColNames,
                  const VBRemapper& remapper);
  casacore::Bool doTimeAverVisIterator(const casacore::Vector<casacore::MS::PredefinedColumns>& dataColNames,
                  const VBRemapper& remapper);

  void getDataColMap(casacore::ArrayColumn<casacore::Complex>* mapper, casacore::uInt ntok,
                     const casacore::Vector<casacore::MS::PredefinedColumns>& colEnums)
  {
    getDataColMap(msc_p, mapper, ntok, colEnums);
  }

  // Returns whether or not the numbers of correlations and channels
  // are independent of DATA_DESCRIPTION_ID, for both the input and output
  // MSes.
  casacore::Bool areDataShapesConstant();

  // Returns whether or not the input casacore::MS has a valid FLAG_CATEGORY, and its
  // first row has the right number of correlations and channels.
  casacore::Bool existsFlagCategory() const;

  // Sets up sourceRelabel_p for mapping input SourceIDs (if any) to output
  // ones.  Must be called after fieldid_p is set and before calling
  // fillFieldTable() or copySource().
  void relabelSources();

  void relabelIDs();

  // Sets outcol to the indices of selvals that correspond to incol, i.e.
  // outcol(i) = j s.t. selvals[j] == incol(i).
  //
  // Ideally selvals is set up so that outcol's values will be row numbers in
  // the corresponding subtable of its ms.
  //
  // Throws an exception if incol and outcol do not have the same # of rows, or
  // incol has a value that is not in selvals.
  void remapColumn(casacore::ScalarColumn<casacore::Int>& outcol, const casacore::ScalarColumn<casacore::Int>& incol,
                   const casacore::Vector<casacore::Int>& selvals);

  // Equivalent to but slightly more efficient than
  // remapColumn(outcol, incol, incol.getColumn()).
  void remapColumn(casacore::ScalarColumn<casacore::Int>& outcol, const casacore::ScalarColumn<casacore::Int>& incol);

  //static void make_map(const casacore::Vector<casacore::Int>& mscol, casacore::Vector<casacore::Int>& mapper);

  // Sets mapper to to a map from the distinct values of inv, in increasing
  // order, to 0, 1, 2, ..., mapper.size() - 1.
  static void make_map(std::map<casacore::Int, casacore::Int>& mapper, const casacore::Vector<casacore::Int>& inv);
  // Sets mapper to form a map from inv whose elements are mapped values. It skips
  // to store in mapper if the value is -1
  static void make_map2(std::map<casacore::Int, casacore::Int>& mapper, const casacore::Vector<casacore::Int>& inv);

  casacore::uInt remapped(const casacore::Int ov, const casacore::Vector<casacore::Int>& mapper, casacore::uInt i);

  // Sets up the stub of a POINTING, enough to create an MSColumns.
  void setupNewPointing();

  // Sets sort to a casacore::Block of columns that a VisibilityIterator should sort by,
  // according to combine_p.  Columns that should never be combined in the
  // calling function, i.e. spw for time averaging, should be listed in
  // uncombinable.
  //
  // verbose: log a message on error.
  //
  // Returns whether or not there were any conflicts between combine_p and
  // uncombinable.
  casacore::Bool setSortOrder(casacore::Block<casacore::Int>& sort, const casacore::String& uncombinable="",
                    const casacore::Bool verbose=true) const;

  // Returns whether col is (not in combine_p) || in uncombinable.
  // Columns that should never be combined in the
  // calling function, i.e. spw for time averaging, should be listed in
  // uncombinable.
  //
  // verbose: log a message on error.
  //
  // conflict is set to true if there is a conflict between combine_p and
  // uncombinable.
  casacore::Bool shouldWatch(casacore::Bool& conflict, const casacore::String& col,
                   const casacore::String& uncombinable="",
                   const casacore::Bool verbose=true) const;

  // *** Member variables ***

  // Initialized* by ctors.  (Maintain order both here and in ctors.)
  //  * not necessarily to anything useful.
  casacore::MeasurementSet ms_p, mssel_p;
  casacore::MSColumns * msc_p;		// columns of msOut_p
  casacore::MSColumns * mscIn_p;
  casacore::Bool keepShape_p,      	// Iff true, each output array has the
				// same shape as the corresponding input one.
       // sameShape_p,             // Iff true, the shapes of the arrays do not
       //  			// vary with row number.
       antennaSel_p;		// Selecting by antenna?
  casacore::Double timeBin_p;
  casacore::String scanString_p,          // Selects scans by #number#.  Historically named.
         intentString_p,        // Selects scans by string.  scanString_p was taken.
         obsString_p,           // casacore::String for observationID selection.
         uvrangeString_p,
         taqlString_p;
  casacore::String timeRange_p, arrayExpr_p, corrString_p;
  casacore::String combine_p;          // Should time averaging not split bins by
                             // scan #, observation, and/or state ID?
                             // Must be lowercase at all times.
  casacore::Int fitorder_p;               // The polynomial order for continuum fitting.
                                // If < 0 (default), continuum subtraction is
                                // not done.
  casacore::String fitspw_p;           // Selection string for line-free channels.
  casacore::String fitoutspw_p;        // Selection string for output channels if doing
                             // continuum subtraction.

  // jagonzal: Allow main table to be left empty, so that it can be filled by another layer.
  casacore::Bool fillMainTable_p;

  // Uninitialized by ctors.
  casacore::MeasurementSet msOut_p;
  casacore::Vector<casacore::Int> spw_p,      // The input spw corresponding to each output spw.
              spw_uniq_p, // Uniquified version of spw_p.
              spwind_to_min_spwind_p,
              nchan_p,    // The # of output channels for each range.
              totnchan_p, // The # of output channels for each output spw.
              chanStart_p, // 1st input channel index in a selection.
              chanEnd_p,   // last input channel index in a selection.
              chanStep_p, // Increment between input chans, i.e. if 3, only every third
                          // input channel will be used. 
              widths_p,   // # of input chans per output chan for each range.
              ncorr_p,    // The # of output correlations for each DDID.
              inNumChan_p,    // The # of input channels for each spw.
              inNumCorr_p;    // The # of input correlations for each DDID.
  casacore::Vector<casacore::Int> fieldid_p;
  casacore::Vector<casacore::Int> spwRelabel_p, fieldRelabel_p, sourceRelabel_p;
  casacore::Vector<casacore::Int> oldDDSpwMatch_p;
  casacore::Vector<casacore::String> antennaSelStr_p;
  casacore::Vector<casacore::Int> antennaId_p;
  casacore::Vector<casacore::Int> antIndexer_p;
  casacore::Vector<casacore::Int> antNewIndex_p;

  casacore::Vector<casacore::Int> selObsId_p;  // casacore::List of selected OBSERVATION_IDs.
  casacore::Vector<casacore::Int> polID_p;	       // casacore::Map from input DDID to input polID, filled in fillDDTables(). 
  casacore::Vector<casacore::uInt> spw2ddid_p;

  // inCorrInd = outPolCorrToInCorrMap_p[polID_p[ddID]][outCorrInd]
  casacore::Vector<casacore::Vector<casacore::Int> > inPolOutCorrToInCorrMap_p;

  std::map<casacore::Int, casacore::Int> stateRemapper_p; 

  casacore::Vector<casacore::Vector<casacore::Slice> > chanSlices_p;  // Used by VisIterator::selectChannel()
  casacore::Vector<casacore::Slice> corrSlice_p;
  casacore::Vector<casacore::Vector<casacore::Slice> > corrSlices_p;  // Used by VisIterator::selectCorrelation()
  casacore::Matrix<casacore::Double> selTimeRanges_p;

  bool tvi_debug;
  bool want_cont_p;
};

} //# NAMESPACE CASA - END

#endif

