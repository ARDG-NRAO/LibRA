/*
 * PointingDirectionCache.cc
 *
 *  Created on: Dec 1, 2016
 *      Author: jjacobs
 */

#include <msvis/MSVis/PointingDirectionCache.h>

#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <measures/Measures/MDirection.h>
#include <ms/MeasurementSets/MSPointingColumns.h>

#include <memory>

using namespace casacore;

namespace casa {
namespace vi {

PointingColumns::PointingColumns (const MSPointingColumns & pointingColumns)
: pointingColumns_p (pointingColumns)
{}

Pointing
PointingColumns::getPointingRow (int row, double /*targetTime*/, bool asMeasure) const
{
    Pointing pointing;

    // See if the row's time is no earlier than 5 minutes before target
    // An optimization to avoid unnecessary direction measure conversions
    // for rows unlikely to be used (the conversions are fairly expensive)

    pointing.time = pointingColumns_p.time () (row);
    pointing.interval = pointingColumns_p.interval () (row);
    pointing.antennaId = pointingColumns_p.antennaId() (row);
    pointing.row = row;
    pointing.source = this;

    if (asMeasure){

        // Pointing is likely to be useful so get all the data.

        pointing.direction.reset (new MDirection (pointingColumns_p.directionMeas (row)));
    }

    return pointing;
}

int
PointingColumns::nRows () const
{
    return pointingColumns_p.nrow();
}


PointingDirectionCache::PointingDirectionCache (int nAntennas,
                                                const PointingSource & pointingSource)
: antennaLevelCache_p (MinTimeEntries, MaxTimeEntries, nAntennas, this),
  antennaEarliestTime_p (nAntennas, -1.0),
  lastRowRead_p (-1),
  nFallbacks_p (0),
  nHits_p (0),
  pointingEofReached_p (false),
  pointingSource_p (pointingSource)
{}

PointingDirectionCache::~PointingDirectionCache ()
{
//    printf ("--> nHits=%d, nFallbacks_p=%d, %%fallback=%f, unused=0%3.2f\n", nHits_p, nFallbacks_p,
//            100.0 * nFallbacks_p / (nHits_p + nFallbacks_p), (nAdded_p - nHits_p) * 1.0f / nAdded_p);
}

void
PointingDirectionCache::fillCache (int antenna, double time, bool flushAndRewind) const
{
    if (flushAndRewind){

        // A time earlier than what was cached is needed: reload the cache from the
        // beginning.  If done with any frequency on large files it could produce a
        // performance problem.

        antennaLevelCache_p.flushTimes ();
        lastRowRead_p = -1;
    }

    int nRows = pointingSource_p.nRows();

    while (true) {

        // Begin reading rows from the pointing table.  Each row will be an entry for
        // an antenna at a particular time.  The implementation will fail if the
        // timestamps for each antenna are not monotonically increasing.

        lastRowRead_p ++;

        if (lastRowRead_p >= nRows){
            pointingEofReached_p = true;
            return; // can't fill anymore
        }

        // Pull out the information from the current row of the pointing table and
        // add it to the cache.

        Pointing pointing = pointingSource_p.getPointingRow (lastRowRead_p, time, false);
        antennaLevelCache_p.addEntry (pointing);

        if (antennaEarliestTime_p [pointing.antennaId] < 0) {
            antennaEarliestTime_p [pointing.antennaId] = pointing.time;
        }

        // See if the current row satisfies the underlying cache read request.
        // If so, return.

        if (pointing.antennaId == antenna){

            if (pd_cache::timeMatch (time, pointing.time, pointing.interval)){
                return;
            }

            if (time < pointing.time){
                // Target time before first pointing time, so give up.
                return;
            }
        }
    }
}

std::pair <bool, casacore::MDirection>
PointingDirectionCache::getPointingDirection (int antenna, double time, const MDirection & phaseCenter) const
{
    using namespace pd_cache;

    // Try to fill the direction request from the cache.

    if (noDataForAntenna (antenna, time)){
        nFallbacks_p ++;
        return std::make_pair (true, phaseCenter); // antenna not in pointing subtable --> return default.
    }

    CacheAccessStatus status;
    const MDirection * direction = nullptr;
    MDirection result;

    std::tie (status, direction) = antennaLevelCache_p.getPointingDirection (antenna, time);

    if (status == CacheAccessStatus::Hit){

        result = * direction;
        nHits_p ++;

    } else if (status == CacheAccessStatus::MissInternal){

        // The cache contained values surrounding the requested value but not that
        // matched it.  Use the phaseCenter as a reasonable fallback.

        result = phaseCenter;
        nFallbacks_p ++;

    } else {

        // The request failed so add more data to the cache.

        bool flushAndRewind = status == CacheAccessStatus::MissPrior;

        fillCache (antenna, time, flushAndRewind);

        // Try to fill the direction request again.

        std::tie (status, direction) = antennaLevelCache_p.getPointingDirection (antenna, time);

        if (status != CacheAccessStatus::Hit){

            // Miss after cache fill so use the phaseCenter as a fallback.

            result = phaseCenter;
            nFallbacks_p ++;
        } else {
            result = * direction;
            nHits_p ++;
        }
    }

bool northPole = result.toString() == "00:00:00.000000 90.00.00.00000 J2000";
if (northPole){
    printf ("Returning north pole: ant=%d, t=%f\n", antenna, time);
}

    return std::make_pair (true, result);
}

const PointingSource *
PointingDirectionCache::getPointingSource () const
{
    return & pointingSource_p;
}

bool
PointingDirectionCache::noDataForAntenna (int antenna, double time) const
{
    // If the entire pointing subtable has been scanned once and the antenna
    // was not seen then return true.  This is intended to prevent excessive
    // reading of the pointing subtable when an antenna is missing.

    bool noData = false;

    if (antennaEarliestTime_p [antenna] >= 0){

        // If there is an earliest time for the antenna, then see if the requested
        // time is before it.

        noData = time < antennaEarliestTime_p [antenna];

    } else {

        // No data has been seen (yet?) for the current antenna.
        // If the entire subtable has been scanned once already, then
        // there is no data for this antenna.

        noData = pointingEofReached_p;
    }

    return noData;
}

namespace pd_cache {

bool
timeMatch (double time, double rowsTime, double rowsInterval)
{
    // The time matches if it falls within an interval around the pointing
    // table time.

    bool match = time >= rowsTime - rowsInterval &&
                 time <= rowsTime + rowsInterval;

    return match;
}

AntennaLevelCache::AntennaLevelCache (int minTimes, int maxTimes, int nAntennas,
                                      const PointingDirectionCache * pdCache)
: pointingDirectionCache_p (pdCache),
  timeLevelCache_p (nAntennas, TimeLevelCache (minTimes, maxTimes, this))
{}


void
AntennaLevelCache::addEntry (Pointing & pointing)
{
    timeLevelCache_p [pointing.antennaId].addEntry (pointing);
}

void
AntennaLevelCache::flushTimes ()
{
    for (auto & timeCache : timeLevelCache_p){

        // Flush the time cache for each antenna

        timeCache.flush ();
    }
}

std::pair<CacheAccessStatus,const casacore::MDirection *>
AntennaLevelCache::getPointingDirection (int antenna, double time)
{
    return timeLevelCache_p [antenna].getPointingDirection(time);
}

const PointingSource *
AntennaLevelCache::getPointingSource () const
{
    return pointingDirectionCache_p->getPointingSource ();
}

TimeLevelEntry::TimeLevelEntry (const Pointing & pointing,
                                const TimeLevelCache * tlCache)
: direction_p (std::move (pointing.direction)),
  row_p (pointing.row),
  timeCenter_p (pointing.time),
  timeLevelCache_p (tlCache),
  interval_p (pointing.interval)
{
bool northPole = direction_p && direction_p->toString() == "00:00:00.000000 90.00.00.00000 J2000";
if (northPole){
    printf ("Returning north pole: ant=%d, t=%f\n", pointing.antennaId, pointing.time);
}
}

TimeLevelEntry::TimeLevelEntry (const TimeLevelEntry & other)
 : direction_p (std::move (other.direction_p)),
   row_p (other.row_p),
   timeCenter_p (other.timeCenter_p),
   timeLevelCache_p (other.timeLevelCache_p),
   interval_p (other.interval_p)
{}

TimeLevelEntry::~TimeLevelEntry ()
{
}

TimeLevelEntry &
TimeLevelEntry::operator= (const TimeLevelEntry & other)
{
    if (& other != this){

        direction_p = std::move (other.direction_p);
        other.direction_p = nullptr;

        row_p = other.row_p;
        timeCenter_p = other.timeCenter_p;
        interval_p = other.interval_p;
    }

    return * this;
}

const casacore::MDirection *
TimeLevelEntry::getDirection () const{

    // If the direction measure is actually cached, then simply return it.

    if (! direction_p){

        // Refetch the row and actually get the direction this time.  Put it in
        // the cache and return a pointer to it.

        Pointing p = timeLevelCache_p->getPointingSource()->getPointingRow (row_p, timeCenter_p, true);
        direction_p.reset (new MDirection (* p.direction.get()));

    }

    return direction_p.get();
}

double
TimeLevelEntry::getInterval () const{
    return interval_p;
}

double
TimeLevelEntry::getTime () const
{
    return timeCenter_p;
}


bool
operator== (double t, const TimeLevelEntry & tle)
{
    return timeMatch (t, tle.getTime(), tle.getInterval());
}

bool
operator== (const TimeLevelEntry & tle, double t)
{
    return t == tle;
}

bool
operator< (double t, const TimeLevelEntry & tle)
{
    return t < tle.getTime() - tle.getInterval();
}

bool
operator< (const TimeLevelEntry & tle, double t)
{
    return t > tle.getTime() + tle.getInterval();
}


TimeLevelCache::TimeLevelCache (int minTimes, int maxTimes, const AntennaLevelCache * alCache)
: antennaLevelCache_p (alCache),
  cache_p (),
  maxTimes_p (maxTimes),
  minTimes_p (minTimes)
{}


void
TimeLevelCache::addEntry (Pointing & pointing)
{
    if (cache_p.size() + 1 > (uint) maxTimes_p){

        // Compact the cache so that it contains minTimes_p elements.

        int nToErase = cache_p.size() - minTimes_p;
        cache_p.erase (cache_p.begin(), cache_p.begin() + nToErase);
    }

    // Add in the new entry on the end.

    if (! cache_p.empty() && pointing.time < cache_p.back().getTime()){

        // The pointing table is not increasing monotonically for this antenna.
        // As a kluge, flush the cache and continue.  This could behave badly
        // in some situations.

        cache_p.clear();
    }

    cache_p.push_back (TimeLevelEntry (pointing, this));
}

void
TimeLevelCache::flush ()
{
    // Get rid of everything.

    cache_p.clear();
}

std::pair<CacheAccessStatus, const casacore::MDirection *>
TimeLevelCache::getPointingDirection (double time)
{
    if (cache_p.empty ()){
        return std::make_pair (CacheAccessStatus::MissPost, nullptr);
    }

    // Find the first element that is >= the requested time.

    Cache::iterator lowerBound = std::lower_bound (cache_p.begin(), cache_p.end(), time);

    if (lowerBound == cache_p.end()){

        // Handle the case where there is no lower bound

        TimeLevelEntry & tle = cache_p.back();
        double upperTime = tle.getTime() + tle.getInterval();

        if (time <= upperTime){

            // Last element in cache's interval contains the target
            // time: we found it

            return std::make_pair (CacheAccessStatus::Hit, tle.getDirection());
        }

        // Off the top end of existing data in the cache

        return std::make_pair (CacheAccessStatus::MissPost, nullptr);
    }

    if (time == * lowerBound){

        // We've found it

        return std::make_pair (CacheAccessStatus::Hit, lowerBound->getDirection());

    }

    if (lowerBound == cache_p.begin()){

        // Nothing below it so it's a miss

        return std::make_pair (CacheAccessStatus::MissPrior, nullptr);
    }

    if (time == * (lowerBound - 1)){

        return std::make_pair (CacheAccessStatus::Hit,
                               (lowerBound - 1)->getDirection());
    }

    // Cache miss: must be between two cache entries (this could be handled
    // by interpolating between the two entries, but might indicate a flaw in
    // the algorithm or the data; for now just let the caller sort it out).

    return std::make_pair (CacheAccessStatus::MissInternal, nullptr);
}

const PointingSource *
TimeLevelCache::getPointingSource () const
{
    return antennaLevelCache_p->getPointingSource ();
}


} // end namespace pd_cache

} // end namespace vi
} // end namepsace casa

