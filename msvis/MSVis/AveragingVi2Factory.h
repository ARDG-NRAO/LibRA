/*
 * AveragingViFactory.h
 *
 *  Created on: Feb 25, 2013
 *      Author: jjacobs
 */

#ifndef MSVIS_AVERAGINGVIFACTORY_H_
#define MSVIS_AVERAGINGVIFACTORY_H_

#include <msvis/MSVis/VisibilityIterator2.h>

namespace casa {

class MeasurementSet2;

namespace vi {

// AveragingOptions
//
// A container for the options that can be specified when creating
// an AveragingTvi2 based VI.  The user can specify which of the
// data cubes (observed, model and corrected) are to be averaged.
// The source of the weights applied to each cube can also be specified.
// See enum Options below for the symbols to use; the symbols are usually
// combined using bitwise-and ("|") in the constructor.

class AveragingOptions {

public:

    enum Options {Nothing = 0,
                  AverageObserved = 1 << 0,  // Average the observed data
                  AverageModel = 1 << 1,     // Average the model data
                  AverageCorrected = 1 << 2, // Average the corrected data
                  AverageFloat = 1 << 3, 	 // Average float (single dish) data
                  ObservedPlainAvg = 1 << 4,
                  ObservedFlagAvg = 1 << 5,
                  ObservedWeightAvgFromSIGMA = 1 << 6,
                  ObservedFlagWeightAvgFromSIGMA = 1 << 7,
                  ModelPlainAvg = 1 << 8,
                  ModelFlagAvg = 1 << 9,
                  ModelWeightAvgFromWEIGHT = 1 << 10,
                  ModelWeightAvgFromSIGMA = 1 << 11,
                  ModelFlagWeightAvgFromWEIGHT = 1 << 12,
                  ModelFlagWeightAvgFromSIGMA = 1 << 13,
                  CorrectedPlainAvg = 1 << 14,
                  CorrectedFlagAvg = 1 << 15,
                  CorrectedWeightAvgFromWEIGHT = 1 << 16,
                  CorrectedFlagWeightAvgFromWEIGHT = 1 << 17,
                  BaselineDependentAveraging = 1 << 18, // Do averaging with lengths dependent on baselines
                                                        // Requires specifying a max uvw distance parameter
                  phaseShifting = 1 << 19,
                  MarksLast
    };

    AveragingOptions () : options_p (AverageObserved) {}
    AveragingOptions (casacore::Int options) : options_p ((Options) options) {}
    explicit AveragingOptions (Options o) : options_p (o) {}

    AveragingOptions operator& (const AveragingOptions & other) const
    {
        return AveragingOptions (other.options_p & options_p);
    }

    AveragingOptions operator| (const AveragingOptions & other) const
    {
        return AveragingOptions (other.options_p | options_p);
    }

    AveragingOptions & operator|= (const AveragingOptions & other)
    {
        * this = AveragingOptions (options_p | other.options_p);

        return * this;
    }

    AveragingOptions & operator|= (Options options)
    {
        * this = AveragingOptions (options_p | options);

        return * this;
    }

    AveragingOptions operator~ () const
    {
        return AveragingOptions (~ options_p);
    }

    casacore::Bool contains (Options o) const { return (o & options_p) != 0; }

    casacore::Int
    nSet (casacore::Int o) const
    {
        casacore::Int result = o & options_p;
        casacore::Int nSet = 0;

        for (casacore::Int mask = 1; mask < MarksLast; mask = mask << 1){

            nSet += (result & mask) != 0 ? 1 : 0;

        }

        return nSet;
    }

private:


    Options options_p;
};

class AveragingParameters {

public:

    AveragingParameters ();

    AveragingParameters (casacore::Double averagingInterval,
                         casacore::Double chunkInterval,
                         const SortColumns & sortColumns = SortColumns (),
                         const AveragingOptions & options = AveragingOptions (),
                         casacore::Double maxUvwDistance = 0,
                         WeightScaling * weightScalingForAveraging = 0,
                         casacore::Bool isWriteable = false,
                         casacore::Double dx = 0,
                         casacore::Double dy = 0);

    AveragingParameters (const AveragingParameters & other);

    AveragingParameters & operator= (const AveragingParameters & other);

    casacore::Double getAveragingInterval () const;
    casacore::Double getChunkInterval () const;
    casacore::Double getMaxUvwDistance () const;
    const AveragingOptions & getOptions() const;
    const SortColumns & getSortColumns () const;
    WeightScaling * getWeightScaling () const;
    casacore::Double getXpcOffset () const;
    casacore::Double getYpcOffset () const;
    casacore::Bool isWriteable () const;

    void setAveragingInterval (casacore::Double);
    void setChunkInterval (casacore::Double);
    void setSortColumns (const SortColumns &);
    void setOptions (const AveragingOptions &);
    void setMaxUvwDistance (casacore::Double);
    void setWeightScaling (WeightScaling *);
    void setWritable (casacore::Bool isWritable);
    void setPhaseShift (casacore::Double dx, casacore::Double dy);

private:

    void validate ();
    void validateOptions ();

    casacore::Double averagingInterval_p;
    AveragingOptions averagingOptions_p;
    casacore::Double chunkInterval_p;
    casacore::Double maxUvwDistance_p;
    SortColumns sortColumns_p;
    WeightScaling * weightScaling_p;
    casacore::Bool isWritable_p;
    casacore::Double XpcOffset_p;
    casacore::Double YpcOffset_p;

    VisBufferComponents2 allDataColumns () const;

};

// The AveragingVi2Factory is used to initialize a VisibilityIterator2 so that
// it performs time averaging.  The parameters are fairly straightforward:
//
// averagingInterval - the time period to average over
// chunkInterval - the time interval of an averaged chunk
// sortColumns - the usual set of sort columns.  The sort columns also determine
//               the boundaries of a chunk.  If a column is present in the sort
//               columns, then there will be no averaging across values with different
//               values of that column (e.g., scan_number, array_id, etc.).  This is
//               because a chunk is defined to be the set of records having the same
//               values for all of the sort columns except for time.  The time column
//               is used to select rows having the same sort-column values but within
//               a specified interval of time.

class AveragingVi2Factory : public ViFactory {

public:

    AveragingVi2Factory (const AveragingParameters & parameters,
                         casacore::MeasurementSet * ms);

    AveragingVi2Factory (const AveragingParameters & parameters,
                         casacore::MeasurementSet * ms1,
                         casacore::MeasurementSet * ms2,
                         ...);
    // use 0 to mark end of MSs

    AveragingVi2Factory (const AveragingParameters & parameters,
                         const casacore::Block<const casacore::MeasurementSet *> & mss);

    ~AveragingVi2Factory ();

protected:

    virtual ViImplementation2 * createVi () const;

private:

    void initialize (const AveragingParameters & parameters,
                     const casacore::Block<const casacore::MeasurementSet *> & mss);

    casacore::Block<const casacore::MeasurementSet *> mss_p;
    AveragingParameters parameters_p;
};


class AveragingVi2LayerFactory : public ViiLayerFactory {

public:

  AveragingVi2LayerFactory(const AveragingParameters& avepars);

  virtual ~AveragingVi2LayerFactory() {};

 protected:

  
  virtual ViImplementation2 * createInstance(ViImplementation2* vii0) const;

  const AveragingParameters avepars_p;

};


} // end namesapce vi
} // end namespace casa


#endif /* MSVIS_AVERAGINGVIFACTORY_H_ */
