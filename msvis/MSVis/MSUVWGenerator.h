//# MSUVWGenerator.h: Generate and insert the (u, v, w)s of a MS.
//# Copyright (C) 2008
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
//#
#ifndef MS_MSUVWGENERATOR_H
#define MS_MSUVWGENERATOR_H

#include <casa/aips.h>
#include <casa/BasicSL/String.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <measures/Measures/MBaseline.h>

// FTMachine::rotateUVW(casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Double>& dphase,
//                      const VisBuffer& vb)

namespace casacore{

class MeasurementSet;
class LogIO;
}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>Generates and inserts the (u, v, w)s of a casacore::MS that may or may not
// already have them.  Includes antenna offsets when known.</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class=casacore::MeasurementSet>MeasurementSet</linkto>
//   <li> <linkto class=casacore::Measure>Measure</linkto>
// </prerequisite>
//
// <etymology>
// It generates a UVW column for the casacore::MS, whether or not it already has one.
// It is an adaptation of alma/apps/asdm2MS/UvwCoords to work with MSes instead
// of ASDMs.
// </etymology>
//
// <synopsis>
// (u, v, w)s are needed for imaging, but some data sets may not come with them
// included, or the (u, v, w)s may need correction.
// </synopsis>
//
// <motivation>
// Currently (10/30/2008), ASDMs from either the ALMA Test Facility or the
// EVLA do not come with (u, v, w)s, and need to be processed with the UVFIX
// task in AIPS.  It would be preferable to process them entirely inside CASA.
// </motivation>
//
// <todo asof="11/20/2008">
// It requires as input the casacore::MS timeCentroids item which is an output in the
// ASDM DAMs.
//     - the use-case when the timeCentroid is baseline-based and or spectral
//       window based.
//     - correlationMode filter (hopefully handled by SDM -> casacore::MS import routines).
//       direction item in the field table.
// @note The current limitations are set by the status of the ASDM
//     - OTF not supported due to a limitation in the model of the phase
// </todo>
class MSUVWGenerator
{
public:
  // Constructor 
  //      @param ms_ref Reference to the casacore::MS's columns.
  //      @post - The relative positions for all the antennas in the Antenna
  //              table are in bl_an_p.
  //            - timeRes_p has been conservatively calculated using rough
  //              estimates of the maximum baseline length and field of view.
  MSUVWGenerator(casacore::MSColumns& ms_ref, const casacore::MBaseline::Types bltype,
		 const casacore::Muvw::Types uvwtype);

  // Destructor
  ~MSUVWGenerator();

  // Determine the uvw for a single phaseDir and timeCentroid, and pair of
  // feeds (which may be the same).
  //
  //	   @param ant1   Row number in the ANTENNA table of the 1st antenna.
  //	   @param feed1  Row number in the FEED    table of the 1st feed.
  //	   @param ant1   Row number in the ANTENNA table of the 2nd antenna.
  //	   @param feed2  Row number in the FEED    table of the 2nd feed.
  //       @param uvw    The returned UVW coordinates.
  void uvw_bl(const casacore::uInt ant1, const casacore::uInt feed1,
              const casacore::uInt ant2, const casacore::uInt feed2, casacore::Array<casacore::Double>& uvw);

  // Calculate the uvws for the field IDs in flds that are not -1, and set
  // those phase directions according to phaseDirs.
  //       @param flds       A map from row numbers in the FIELD table to
  //       		     indices in phaseDirs.  For example, if the casacore::MS has
  //       		     5 fields, and the user wants to (re)calculate the
  //       		     UVWs of only 0, 2, and 4, phaseDirs will have 3
  //       		     entries and flds will be [0, -1, 1, -1, 2].  
  casacore::Bool make_uvws(const casacore::Vector<casacore::Int> flds);
private:
  // Sets up the antenna positions as baselines (bl_an_p), the number of
  // antennas (nant_p), and timeRes_p.
  void fill_bl_an(casacore::Vector<casacore::MVBaseline>& bl_an_p);
  
  // Determine antUVW_p for every member of the sequence of antennas
  // defining a (sub)array.
  //     @param  timeCentroid    An epoch, the 'when' characterization.
  //     @param  fldID           The row number in the FIELD table which gives
  //                             the phase tracking center.
  //     @param  WSRTConvention  If true (WSRT only?), the l in the ul + vm
  //                             phase calculation decreases toward increasing
  //                             RA.  If false (VLA), it increases with
  //                             increasing RA.  See the code for more info.
  //     @note This function only calculates UVWs for a single time and a
  //     single phase center.  Fields can potentially have multiple phase
  //     directions, so be prepared to call this function from within a loop
  //     that also takes care of setting timeCentroid and phaseDir.
  //     @warning timeCentroid can be initialized like
  //       casacore::MEpoch timeCentroid(casacore::Quantity(<double>, "s"), casacore::MEpoch::TAI);
  //      but the accuracy is limited since there is no extra precision
  //      attribute (see Main table of casacore::MS v2).
  void uvw_an(const casacore::MEpoch& timeCentroid, const casacore::Int fldID,
              const casacore::Bool WSRTConvention=false);

  // (Sub-)array parameters constraining order and size of the output vector 
  // of UVW coords triplets.
  // struct ArrayParam{
//   public:
//     casacore::Int                   subarrayId;        // (sub)array identifier
//     vector<Tag>           v_ant;             //<! sequence of antennas
//     unsigned int          nrepeat;           //<! number of casacore::MS main table rows
// 					     //   per antenna baseline
//     Enum<CorrelationMode> e_correlationMode; //<! correlation mode (original
// 					     //   mode passed through the user
// 					     //   filter) (FOLLOWUP: is this needed?)
//     string show(){
//       ostringstream os;
//       os << " nrepeat " << nrepeat;
//       return os.str();
//     }
  //};

  //****************** Member variables ********************************

  //**************** Initialized by ctor, ******************************
  //**************** so they should appear ******************************
  //**************** here in the same order ******************************
  //**************** as they do in the ctor. ******************************

  casacore::MSColumns& msc_p;  // the columns of the measurement set.

  // casacore::Coordinate system selectors.
  casacore::MBaseline::Ref bl_csys_p;
  
  const casacore::MSAntennaColumns& antColumns_p;

  // The antenna positions of ms_p in ITRF.  It cannot be const because of the
  // need to update satellite positions.
  // antPositions_p and antOffset_p are references and must therefore be
  // initialized in the initialization list.
  const casacore::ROScalarMeasColumn<casacore::MPosition>& antPositions_p;

  // The offsets between the phase reference point (see feed_offsets below for
  // clarification) of each antenna and the closest point which is fixed
  // relative to the antenna position (i.e. an axis).  Since no one else has
  // defined it yet, I am defining it as the offset when the antenna is
  // pointing at zenith, oriented so that if it slewed down, it would move
  // toward the north.  (x, y, z) = (east, north, up).
  //
  // This appears to be a required column of the ANTENNA table in version 2.0
  // of the casacore::MeasurementSet definition
  // (http://aips2.nrao.edu/docs/notes/229/229.html), so it is assumed to be
  // present.  However, it is usually a set of zeroes, based on the common
  // belief that it is only needed for heterogeneous arrays, since the
  // receivers of homogeneous arrays move in concert.  That is not true when
  // there are independent pointing errors.
  const casacore::ROScalarMeasColumn<casacore::MPosition>& antOffset_p;

  // The position of the first antenna.
  casacore::MPosition refpos_p;
  
  // Ditto for feed.
  //const casacore::MSFeedColumns *feedColumns_;

  // The offset between the feed and the phase reference point of each antenna
  // in the same frame as ant_offsets.  Therefore the feed position is
  //  ant_positions_p(ant) + [rotation matrix](pointing) (ant_offsets_p[ant] +
  //							feed_offsets_p[ant])
  const casacore::ROScalarMeasColumn<casacore::MPosition>& feedOffset_p;

  casacore::MBaseline::Types refposref_p;  

  // The antenna positions - refpos_p.getValue().
  casacore::Vector<casacore::MVBaseline> bl_an_p;

  casacore::uInt nant_p;  // # of antennas

  // The minimum time difference for forcing an update of the UVWs.  We're not
  // trying to do time averaging here, so it should be small, but not so small
  // that uvw_an() is called for every baseline, even when the times are
  // practically the same.
  casacore::Double timeRes_p;

  //************* Initialized later, if at all. ********************
  
  // Log functions and variables
  casacore::LogIO sink_p;
  casacore::LogIO& logSink() {return sink_p;}

  //map<Tag, ArrayParam>      m_array_p;     // FIX: Tag

  // The UVW "positions" of each antenna being used during a time bin,
  // i.e. the (u, v, w) of the baseline between antennas m and n is
  //  antUVW_p[n] - antUVW_p[m].
  // <todo asof="02/18/2009">
  // Generalize to multifeed systems.
  // </todo>
  casacore::Vector<casacore::Vector<casacore::Double> > antUVW_p;

  // the 3 fundamental attributes of the state machine
  casacore::ArrayColumn<casacore::Double> phaseDir_p; 
  //casacore::Int                   subarrayId_p;

  // The number of wavelengths by which a feed may move relative to the
  // corresponding feed in another antenna of the array without requiring the
  // offset to be included in uvw calculations.  If < 0 the offset will always
  // be included.
  //
  // Offsets include:
  //	ant_offset:  OFFSET in the ANTENNA table of an MS.
  //		     The separation between an antenna's "phase reference
  //		     point" and its closest point (usu. an axis) fixed rel
  //		     to its position.
  //
  //	feed_offset: POSITION in the FEED table of an MS.
  //		     The separation between a feed and ant_pos + ant_offset.
  //		     Just another offset to add to ant_offset, but unlike
  //		     ant_offset it is likely to change when the band is
  //		     changed.
  //
  //	rec_offset: Not found in MSes, acc. to vers. 2 of of the MeasurementSet
  //		    definition (http://aips2.nrao.edu/docs/notes/229/229.html).
  //		    The separation between a receptor and its feed.
  //		    Only needed for feed arrays.  The above URL defines a feed
  //		    (incl. feed arrays) as a set of receptors that should be
  //		    calibrated as a single entity.  Predicting how observers
  //		    will decide to calibrate feed arrays is difficult,
  //		    especially since there are no interferometers with feed
  //		    arrays yet, but it can be argued that rec_offset can be
  //		    ignored in many cases.  If a feed array's output is
  //		    combined before leaving the feed (i.e. a beamforming
  //		    array), it may as well be treated as a single feed.  If the
  //		    receptor outputs are kept separate longer, they likely need
  //		    to be calibrated separately and get their own FEED tables.
  //
  // They affect how the baselines of heterogeneous arrays vary with phase
  // tracking direction, and make the baselines of any array depend on pointing
  // errors.  The latter effect is often neglected, and the offsets are written
  // as sets of zeroes, even though they really are not.
  //
  
  // 
};
  
} //# NAMESPACE CASA - END

#endif
