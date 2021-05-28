// Based on code/alma/apps/UVWCoords

#include <casa/Logging/LogIO.h>
#include <msvis/MSVis/MSUVWGenerator.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MeasurementSets/MSAntennaColumns.h>
#include <measures/Measures/MCBaseline.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <map>

using namespace casacore;
namespace casa {

// The UvwCoords ctor has lines for the antennas, antenna offsets, and station
// positions.  This ctor assumes they're present in msc_p if present at all.
  MSUVWGenerator::MSUVWGenerator(MSColumns &msc_ref, const MBaseline::Types bltype,
				 const Muvw::Types) :
  msc_p(msc_ref),				    	
  bl_csys_p(MBaseline::Ref(bltype)), // MBaseline::J2000, ITRF, etc.
  antColumns_p(msc_p.antenna()),
  antPositions_p(antColumns_p.positionMeas()),
  antOffset_p(antColumns_p.offsetMeas()),
  refpos_p(antPositions_p(0)),  // We use the first antenna for the reference
  feedOffset_p(msc_p.feed().positionMeas())
{
  // It seems that using a String is the only safe way to go from say,
  // MPosition::ITRF to MBaseline::ITRF.
  MBaseline::getType(refposref_p,
                     MPosition::showType(refpos_p.getRef().getType()));

  fill_bl_an(bl_an_p);		
}

MSUVWGenerator::~MSUVWGenerator(){
}

void MSUVWGenerator::fill_bl_an(Vector<MVBaseline>& bl_an_p)
{
  nant_p = antPositions_p.table().nrow();
  logSink() << LogIO::DEBUG1 << "nant_p: " << nant_p << LogIO::POST;

  Double max_baseline = -1.0;
  Double bl_len;
 
  const ScalarColumn<Double>& antDiams = antColumns_p.dishDiameter();
  Double smallestDiam = antDiams(0);
  Double secondSmallestDiam = antDiams(0);
  
  bl_an_p.resize(nant_p);
  for(uInt an = 0; an < nant_p; ++an){
    // MVBaselines are basically xyz Vectors, not Measures.
    bl_an_p[an] = MVBaseline(refpos_p.getValue(), antPositions_p(an).getValue());

    // MVBaseline has functions to return the length, but Manhattan distances
    // are good enough for this, and faster than a sqrt.
    Vector<Double> bluvw(bl_an_p[an].getValue());
    bl_len = fabs(bluvw[0]) + fabs(bluvw[1]) + fabs(bluvw[2]);

    if(bl_len > max_baseline)
      max_baseline = bl_len;

    if(antDiams(an) < secondSmallestDiam){
      if(antDiams(an) < smallestDiam){
	secondSmallestDiam = smallestDiam;
	smallestDiam = antDiams(an);
      }
      else
	secondSmallestDiam = antDiams(an);
    }
  }

  // Setting timeRes_p to 0.0025 * the time for a 1 radian phase change on the
  // longest baseline at 2x the primary beamwidth should be sufficiently short
  // for Earth based observations.  Space-based baselines will move faster, but
  // probably don't have the data rate to support full beam imaging.  An
  // alternative limit could come from |d(uvw)/dt|/|uvw|, but that is
  // guaranteed to be at least somewhat larger than this limit (replace the
  // 2.44 with 2, and remove the dish diameters and max_baseline).
  //
  // Do not raise the 0.0025 coefficient by too much, since the times used for
  // UVW calculation could be biased by as much as -0.5 * timeRes_p if more
  // than one integration fits into timeRes_p.  timeRes_p is not intended so
  // much for skipping calculations as it is for preventing antUVW_p from being
  // calculated for each row in the same integration.  The integration interval
  // may change within an MS, but ideally antUVW_p is calculated once per
  // integration (timeRes_p <~ integration) and there is no bias.
  timeRes_p = 0.0025 * 24.0 * 3600.0 / (6.283 * 2.44) *
    sqrt(smallestDiam * secondSmallestDiam) / max_baseline;
}  

void MSUVWGenerator::uvw_an(const MEpoch& timeCentroid, const Int fldID,
                            const Bool WSRTConvention)
{
  const MDirection& phasedir = msc_p.field().phaseDirMeas(fldID);
  MeasFrame  measFrame(refpos_p, timeCentroid, phasedir);
  MVBaseline mvbl;
  MBaseline  basMeas;

  //logSink() << LogIO::DEBUG1
  //   << "timeCentroid: " << timeCentroid
  //   << "\nfldID: " << fldID
  //   << "\nphasedir: " << phasedir
  //   << LogIO::POST;

  MBaseline::Ref basref(refposref_p);
  basMeas.set(mvbl, basref);            // in antenna frame
  basMeas.getRefPtr()->set(measFrame);

  // Setup a machine for converting a baseline vector from the antenna frame to
  // bl_csys_p's frame
  MBaseline::Convert elconv(basMeas, bl_csys_p);

  for(uInt i = 0; i < nant_p; ++i){
    //TODO: (Soon!) Antenna offsets are not handled yet.
    basMeas.set(bl_an_p[i], basref);
    MBaseline basOutFrame = elconv(basMeas);
    //MBaseline::Types botype = MBaseline::castType(basOutFrame.getRef().getType());
    MVuvw uvwOutFrame(basOutFrame.getValue(), phasedir.getValue());
    
    antUVW_p[i] = uvwOutFrame.getValue();
  }

  // Tony Willis supplied this crucial bit of history: the WSRT definition of
  // phase is opposite to that of the VLA - the WSRT definition of the l,m
  // direction cosines is that l increases toward decreasing RA, whereas the
  // VLA definition is that l increases toward increasing RA.  AIPS seems to
  // understand this.  (For a completely useless piece of trivia, the WSRT defn
  // of phase is consistent with that of the Cambridge one-mile telescope as
  // Wim Brouw set things up to be in agreement with the one-mile telescope ca
  // 1968.)
  // RR: It is not just l, v and w are flipped as well.
  if(WSRTConvention)
    antUVW_p = -antUVW_p;
}

// antUVW_p must be set up for the correct timeCentroid and phase direction by
// uvw_an() before calling this.
void MSUVWGenerator::uvw_bl(const uInt ant1, const uInt,// feed1,
			    const uInt ant2, const uInt,// feed2,
			    Array<Double>& uvw)
{
  //uvw.resize(3);      // Probably redundant.  Does it significantly slow things down?
  //TODO: Feed offsets are not handled yet.
  uvw = antUVW_p[ant2] - antUVW_p[ant1];
}

Bool MSUVWGenerator::make_uvws(const Vector<Int> flds)
{
  ArrayColumn<Double>&      UVWcol   = msc_p.uvw();  
  const ScalarMeasColumn<MEpoch>& timeCentMeas = msc_p.timeCentroidMeas();
  const ScalarColumn<Int> fieldID(msc_p.fieldId());
  const ScalarColumn<Int> ant1(msc_p.antenna1());
  const ScalarColumn<Int> ant2(msc_p.antenna2());
  const ScalarColumn<Int> feed1(msc_p.feed1());
  const ScalarColumn<Int> feed2(msc_p.feed2());
  const ScalarColumn<Int> obsID(msc_p.observationId());

  // Use a time ordered index to minimize the number of calls to uvw_an.
  // TODO: use field as a secondary sort key.
  Vector<uInt> tOI;
  GenSortIndirect<Double>::sort(tOI, msc_p.timeCentroid().getColumn());

  // Having uvw_an() calculate positions for each antenna for every field is
  // somewhat inefficient since in a multiconfig MS not all antennas will be
  // used in each time interval, but it's not clear that determining which
  // antennas will be used for a given uvw_an() call would be any more
  // efficient.  It's not horribly inefficient, because uvw_an() is O(nant_p),
  // and uvw_bl() is only called for baselines that are actually used.
  antUVW_p.resize(nant_p);

  logSink() << LogOrigin("MSUVWGenerator", "make_uvws") << LogIO::NORMAL3;
  
  logSink() << LogIO::DEBUG1 << "timeRes_p: " << timeRes_p << LogIO::POST;

  try{
    Bool oldWsrtConvention = (msc_p.observation().telescopeName()(obsID(tOI[0])) ==
                              "WSRT");

    // Ensure a call to uvw_an on the 1st iteration.
    const Unit sec("s");
    Double oldTime = timeCentMeas(tOI[0]).get(sec).getValue() - 2.0 * timeRes_p;
    Int    oldFld  = -2;

    // IO reordering buffer to avoid poor IO patterns on files not sorted by time
    std::map<uInt, Vector<Double> > writebuffer;

    for(uInt row = 0; row < msc_p.nrow(); ++row){
      uInt   toir = tOI[row];
      const MEpoch & timecentmeas = timeCentMeas(toir);
      Double currTime = timecentmeas.get(sec).getValue();
      Int    currFld  = fieldID(toir);
      Bool   newWsrtConvention = (msc_p.observation().telescopeName()(obsID(toir)) ==
                                  "WSRT");
      if ((row % (1<<22)) == 0) {
        logSink() << LogIO::NORMAL << "Rows handled: "
                  << row << "/" << msc_p.nrow() << LogIO::POST;
      }

      if(currTime - oldTime > timeRes_p || currFld != oldFld
         || newWsrtConvention != oldWsrtConvention){
        oldTime = currTime;
        oldFld  = currFld;

        if(newWsrtConvention != oldWsrtConvention){
          logSink() << LogIO::WARN
                    << "There is a switch to or from the WSRT convention at row "
                    << toir << ".\nWatch for an inconsistency in the sign of UVW."
                    << LogIO::POST;
          oldWsrtConvention = newWsrtConvention;
        }

        //logSink() << LogIO::DEBUG1 << "currTime: " << currTime
        //          << "\ncurrFld: " << currFld << LogIO::POST;
        uvw_an(timecentmeas, currFld, newWsrtConvention);
      }
    
      if(flds[fieldID(toir)] > -1){
	//      uvw_bl(ant1(toir), ant2(toir),
	//     feed1(toir), feed2(toir), UVWcol(toir));
        // collect output and write larger chunk in row sequential order
        writebuffer[toir] = antUVW_p[ant2(toir)] - antUVW_p[ant1(toir)];
        if (writebuffer.size() > 100000) {
          for (auto it = writebuffer.begin(); it != writebuffer.end(); ++it) {
            UVWcol.put(it->first, it->second);
          }
          writebuffer.clear();
        }
      }
    }
    // flush rest of buffer
    for (auto it = writebuffer.begin(); it != writebuffer.end(); ++it) {
      UVWcol.put(it->first, it->second);
    }
  }
  catch(AipsError x){
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
              << LogIO::POST;
    throw(AipsError("Error in MSUVWGenerator::make_uvws."));
    return false;
  }
  return true;
}

// void MSUVWGenerator::get_ant_offsets(const MDirection& dir_with_a_frame)
// {
//   // This appears to be a required column of the ANTENNA table in version 2.0
//   // of the MeasurementSet definition
//   // (http://aips2.nrao.edu/docs/notes/229/229.html), so it is assumed to be
//   // present.  However, it is usually a set of zeroes, based on the common
//   // belief that it is only needed for heterogeneous arrays, since the
//   // receivers of homogeneous arrays move in concert.  That is not true when
//   // there are independent pointing errors.

//   // Convert ant_offset_measures to Vectors and check for nonzeroness.
//   ant_offset_vec_p.resize(nant);
//   for(uInt n = 0; n < nant; ++n)
//     ant_offset_vec_p[n] = ant_offset_meas_p.convert(0, pointingdir).getValue();
// }

// Bool MSUVWGenerator::set_receiv_offsets(const MDirection& dir_with_a_frame)
// {
//   // This appears to be a required column of the FEED table in version 2.0
//   // of the MeasurementSet definition
//   // (http://aips2.nrao.edu/docs/notes/229/229.html), so it is assumed to be
//   // present.  However, it is usually a set of zeroes, based on the common
//   // belief that it is only needed for heterogeneous arrays, since the
//   // receivers of homogeneous arrays move in concert.  That is not true when
//   // there are independent pointing errors.

//   // Convert feed_offset_measures to Vectors and check for nonzeroness.
//   Vector<Vector<Double> > offsetvects;
//   offsetvects.resize(nant);
//   for(uInt n = 0; n < nant; ++n){
//     offsetvects[n] = ant_offsets->convert(0, pointingdir).getValue();
//     if(ant_offsets[n] != ant_offsets[0]){
//       varying_offsets = true;
//     }
//   }

//   ignore_offsets = true;
//   return ignore_offsets;
// }

using namespace casacore;
} // Ends namespace casa.
