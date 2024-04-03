/*
 * PointingDirectionCache.h
 *
 *  Created on: Dec 1, 2016
 *      Author: jjacobs
 */

#ifndef MSVIS_MSVIS_POINTINGDIRECTIONCACHE_H_
#define MSVIS_MSVIS_POINTINGDIRECTIONCACHE_H_

#include <casacore/measures/Measures/MDirection.h>

#include <memory>
#include <vector>

namespace casacore {
    class MDirection;
    class MeasurementSet;
    class MSPointingColumns;
}

namespace casa {
namespace vi {

struct Pointing;
class PointingDirectionCache;
class PointingSource;

namespace pd_cache {

// The namespace pd_cache contains the internal classes to the features provided
// by PointingDirectionCache.

enum class CacheAccessStatus : int {
    Hit,            // Found requested value in cache
    MissPrior,      // Desired value before earliest time in cache
    MissInternal,   // Desired value in between cache values
    MissPost        // Desired value is after last one in cache
};

class AntennaLevelCache;
class TimeLevelCache;

bool timeMatch (double time, double rowsTime, double rowsInterval);

class TimeLevelEntry {

public:

    TimeLevelEntry () = delete;
    TimeLevelEntry (const Pointing &, const TimeLevelCache *);
    TimeLevelEntry (const TimeLevelEntry &);
    ~TimeLevelEntry ();

    TimeLevelEntry& operator= (const TimeLevelEntry & other);

    const casacore::MDirection * getDirection () const;
    double getInterval () const;
    double getTime () const;

private:

    mutable std::unique_ptr<casacore::MDirection> direction_p; // own
    int    row_p;
    double timeCenter_p;
    const TimeLevelCache * timeLevelCache_p;
    double interval_p;

    const PointingSource * getPointingSource () const;
};

bool operator== (double t, const TimeLevelEntry &);
bool operator== (const TimeLevelEntry &, double t);
bool operator< (double t, const TimeLevelEntry &);
bool operator< (const TimeLevelEntry &, double t);

class TimeLevelCache {

public:

    TimeLevelCache (int minTimes, int maxTimes, const AntennaLevelCache * );

    void addEntry (Pointing & pointing);
    void flush ();
    std::pair<CacheAccessStatus, const casacore::MDirection *> getPointingDirection (double time);
    const PointingSource * getPointingSource () const;

private:

    typedef std::vector<TimeLevelEntry> Cache;
    const AntennaLevelCache * antennaLevelCache_p;
    Cache cache_p;
    int maxTimes_p;
    int minTimes_p;

};


class AntennaLevelCache {

public:

    AntennaLevelCache (int minTimes, int maxTimes, int nAntennas, const PointingDirectionCache *);
    AntennaLevelCache (const AntennaLevelCache & other) = delete;

    AntennaLevelCache & operator= (const AntennaLevelCache & other) = delete;

    void addEntry (Pointing & pointing);
    void flushTimes ();
    std::pair<CacheAccessStatus, const casacore::MDirection *> getPointingDirection (int antenna, double time);
    const PointingSource * getPointingSource () const;

private:

    const PointingDirectionCache * pointingDirectionCache_p;
    std::vector<TimeLevelCache> timeLevelCache_p;
};

} // end namepsace pd_cache

class PointingSource;

struct Pointing {

    // The direction and interval components are only usable if
    // "valid" is set to true.

    Pointing () : antennaId (0), direction (nullptr), interval (0), row (0), source (nullptr), time (0) {}
    Pointing (const Pointing & other)
    : antennaId (other.antennaId),
      direction (std::move (other.direction)),
      interval (other.interval),
      row (other.row),
      source (other.source),
      time (other.time)
    {}


    int antennaId;
    mutable std::unique_ptr<casacore::MDirection> direction;
    double interval;
    int row;
    const PointingSource * source;
    double time;
};

class PointingSource {

    // Generic interface to allow passing different pointing information
    // sources into the PointingDirectionCache.  The primary source
    // will be a POINTING subtable; the other likely source will be a unit
    // test source.

public:

    virtual ~PointingSource () {}

    virtual Pointing getPointingRow (int row, double targetTime, bool asMeasure) const = 0;
    virtual int nRows () const = 0;

protected:

private:

};

class PointingColumns : public PointingSource {

    // Wrapper class to provide pointing subtable information into
    // the cache.

public:

    PointingColumns (const casacore::MSPointingColumns &);

    virtual Pointing getPointingRow (int row, double targetTime, bool asMeasure) const override;
    virtual int nRows () const override;

private:

    const casacore::MSPointingColumns & pointingColumns_p;
};



class PointingDirectionCache {

public:

    PointingDirectionCache (int nAntennas, const PointingSource & pointingSource);
    ~PointingDirectionCache ();

    std::pair<bool, casacore::MDirection>
    getPointingDirection (int antennaId, double time, const casacore::MDirection & phaseCenter) const;

protected:

    friend class pd_cache::AntennaLevelCache; // allow access to getPointingSource

    void fillCache (int antenna, double time, bool flushAndRewind) const;
    const PointingSource * getPointingSource () const;
    bool noDataForAntenna (int antenna, double time) const;

private:

    mutable pd_cache::AntennaLevelCache antennaLevelCache_p;
    mutable std::vector<double> antennaEarliestTime_p;
    mutable int lastRowRead_p;
    mutable int nFallbacks_p;
    mutable int nHits_p;
    mutable bool pointingEofReached_p;
    const PointingSource & pointingSource_p;

    // These constants control the cache sizing.  The max value specifies how many entries
    // can be cached.  The min entry controls how many are kept when the cache size exceeds
    // the max value.

    static const int MaxTimeEntries = 3000;
    static const int MinTimeEntries = 1000;

};



} // end namespace vi
} // end namepsace casa

#endif /* MSVIS_MSVIS_POINTINGDIRECTIONCACHE_H_ */
