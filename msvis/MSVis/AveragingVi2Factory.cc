/*
 * AveragingVi2Factory.cc
 *
 *  Created on: Feb 25, 2013
 *      Author: jjacobs
 */


#include <msvis/MSVis/AveragingVi2Factory.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>
#include <msvis/MSVis/AveragingTvi2.h>
#include <cstdarg>
#include <msvis/MSVis/UtilJ.h>

using namespace std;

using namespace casacore;
namespace casa {
namespace vi {

AveragingParameters::AveragingParameters ()
: averagingInterval_p (0),
  averagingOptions_p (AveragingOptions ()),
  chunkInterval_p (0),
  sortColumns_p (SortColumns ()),
  weightScaling_p (0),
  isWritable_p(false)
{}

AveragingParameters::AveragingParameters (Double averagingInterval,
                                          Double chunkInterval,
                                          const SortColumns & sortColumns,
                                          const AveragingOptions & options,
                                          Double maxUvwDistance,
                                          WeightScaling * weightScalingForAveraging,
                                          Bool isWriteable,
                                          Double dx,
                                          Double dy)
: averagingInterval_p (averagingInterval),
  averagingOptions_p (options),
  chunkInterval_p (chunkInterval),
  maxUvwDistance_p (maxUvwDistance),
  sortColumns_p (sortColumns),
  weightScaling_p (weightScalingForAveraging),
  isWritable_p(isWriteable),
  XpcOffset_p(dx),
  YpcOffset_p(dy)
{
    Assert (averagingInterval > 0);
    Assert (chunkInterval >= 0);
    Assert (chunkInterval == 0 || chunkInterval >= averagingInterval);
    Assert (! options.contains (AveragingOptions::BaselineDependentAveraging) ||
            maxUvwDistance_p > 0.001);

    validateOptions (); // Throws if error
}

AveragingParameters::AveragingParameters (const AveragingParameters & other)
{
    * this = other;
}

AveragingParameters &
AveragingParameters::operator= (const AveragingParameters & other)
{
    if (this != & other){

        averagingInterval_p = other.averagingInterval_p;
        averagingOptions_p = other.averagingOptions_p;
        chunkInterval_p = other.chunkInterval_p;
        maxUvwDistance_p = other.maxUvwDistance_p;
        sortColumns_p = other.sortColumns_p;
        weightScaling_p = other.weightScaling_p;
        isWritable_p = other.isWritable_p;
        XpcOffset_p = other.XpcOffset_p;
        YpcOffset_p = other.YpcOffset_p;

        validate ();
    }

    return *this;
}

VisBufferComponents2
AveragingParameters::allDataColumns () const
{
    return VisBufferComponents2::these ({VisBufferComponent2::VisibilityModel,
                                         VisBufferComponent2::VisibilityObserved,
                                         VisBufferComponent2::VisibilityCorrected});
}

Double
AveragingParameters::getChunkInterval () const
{
    Assert (chunkInterval_p >= 0);

    return chunkInterval_p;
}

Double
AveragingParameters::getAveragingInterval () const
{
    Assert (averagingInterval_p > 0);

    return averagingInterval_p;
}

Double
AveragingParameters::getMaxUvwDistance () const
{
    return maxUvwDistance_p;
}

const AveragingOptions &
AveragingParameters::getOptions () const
{
    return averagingOptions_p;
}

const SortColumns &
AveragingParameters::getSortColumns () const
{
    return sortColumns_p;
}

WeightScaling *
AveragingParameters::getWeightScaling () const
{
    return weightScaling_p;
}

Double
AveragingParameters::getXpcOffset () const
{
    return XpcOffset_p;
}

Double
AveragingParameters::getYpcOffset () const
{
    return YpcOffset_p;
}

Bool
AveragingParameters::isWriteable () const
{
    return isWritable_p;
}


void
AveragingParameters::setChunkInterval (Double value)
{
    ThrowIf (value >= 0, "ChunkInterval must be >= 0.");

    chunkInterval_p = value;
}

void
AveragingParameters::setAveragingInterval (Double value)
{
    ThrowIf (value > 0, "AveragingInterval must be > 0.");

    averagingInterval_p = value;
}

void
AveragingParameters::setMaxUvwDistance (Double value)
{
    ThrowIf (value < 0, "MaxUvwDistance must be >= 0.");

    maxUvwDistance_p = value;
}

void
AveragingParameters::setOptions (const AveragingOptions & value)
{
    averagingOptions_p = value;

    validateOptions ();
}

void
AveragingParameters::setSortColumns (const SortColumns & value)
{
    sortColumns_p = value;
}

void
AveragingParameters::setWeightScaling (WeightScaling * value)
{
    weightScaling_p = value;
}

void
AveragingParameters::setWritable (Bool isWritable)
{
    isWritable_p = isWritable;
}

void
AveragingParameters::setPhaseShift (Double dx, Double dy)
{
	XpcOffset_p = dx;
	YpcOffset_p = dy;
}

void
AveragingParameters::validate()
{
    Assert (averagingInterval_p > 0);
    Assert (chunkInterval_p >= 0);
    Assert (chunkInterval_p == 0 || chunkInterval_p >= averagingInterval_p);
    Assert (! averagingOptions_p.contains (AveragingOptions::BaselineDependentAveraging) ||
            maxUvwDistance_p > 0.001);

    validateOptions (); // Throws if error
}


void
AveragingParameters::validateOptions ()
{

    if (averagingOptions_p.contains(AveragingOptions::AverageObserved))
    {

        Int bits  = AveragingOptions::ObservedPlainAvg |
                	AveragingOptions::ObservedFlagAvg |
                	AveragingOptions::ObservedWeightAvgFromSIGMA |
                	AveragingOptions::ObservedFlagWeightAvgFromSIGMA;

        Int nSet = averagingOptions_p.nSet (bits);

        ThrowIf (nSet > 1, "Inconsistent DATA weights options provided");

        ThrowIf (nSet == 0, "Need to specify DATA weighting option");
    }


    if (averagingOptions_p.contains(AveragingOptions::AverageCorrected))
    {

        Int bits  = AveragingOptions::CorrectedPlainAvg |
                	AveragingOptions::CorrectedFlagAvg |
                	AveragingOptions::CorrectedWeightAvgFromWEIGHT |
                	AveragingOptions::CorrectedFlagWeightAvgFromWEIGHT;

        Int nSet = averagingOptions_p.nSet (bits);

        ThrowIf (nSet > 1, "Inconsistent CORRECTED_DATA weights options provided");

        ThrowIf (nSet == 0, "Need to specify CORRECTED_DATA weighting option");
    }

    if (averagingOptions_p.contains(AveragingOptions::AverageModel))
    {

        Int bits  = AveragingOptions::ModelPlainAvg |
                	AveragingOptions::ModelFlagAvg |
                	AveragingOptions::ModelWeightAvgFromWEIGHT |
                	AveragingOptions::ModelWeightAvgFromSIGMA |
                	AveragingOptions::ModelFlagWeightAvgFromWEIGHT |
                	AveragingOptions::ModelFlagWeightAvgFromSIGMA;

        Int nSet = averagingOptions_p.nSet (bits);

        ThrowIf (nSet > 1, "Inconsistent MODEL_DATA weights options provided");

        ThrowIf (nSet == 0, "Need to specify MODEL_DATA weighting option");
    }
}

AveragingVi2Factory::AveragingVi2Factory (const AveragingParameters & parameters,
                                          MeasurementSet * ms)
{
    Block <const MeasurementSet *> mss (1, ms);

    initialize (parameters, mss);
}

AveragingVi2Factory::AveragingVi2Factory  (const AveragingParameters & parameters,
                                           MeasurementSet * ms1,
                                           MeasurementSet * ms2,
                                           ...)
{
    // Capture the first argument directly into the stl vector

    vector<MeasurementSet *> mssV;
    mssV.push_back (ms1);

    va_list args;

    va_start (args, ms2);

    MeasurementSet * ms = va_arg (args, MeasurementSet *);

    while (ms != 0){

        mssV.push_back (ms);

        ms = va_arg (args, MeasurementSet *);
    }

    // Convert the stl vector to the casa Vector.

    Block <const MeasurementSet *> mss (mssV.size());
    for (uInt i = 0; i < mssV.size(); i++){
        mss [i] = mssV [i];
    }

    // Now have the other overload do the actual work.

    initialize (parameters, mss);
}


AveragingVi2Factory::AveragingVi2Factory (const AveragingParameters & parameters,
                                          const Block<const MeasurementSet *> & mss)
{
    initialize (parameters, mss);
}

AveragingVi2Factory::~AveragingVi2Factory ()
{
}


void
AveragingVi2Factory::initialize (const AveragingParameters & parameters,
                                 const Block<const MeasurementSet *> & mss)
{
    parameters_p = parameters;
    mss_p = mss;
}


ViImplementation2 *
AveragingVi2Factory::createVi () const
{

    // Make the chunk interval compatible with the averaging interval up rounding it up to
    // the nearest multiple of the averaging interval (e.g., chunkInterval 12 with averaging
    // interval of 5 is rounded to 15.

    Double chunkInterval = parameters_p.getChunkInterval ();
    Double chunkRatio = ceil (chunkInterval / parameters_p.getAveragingInterval ());
    chunkInterval = parameters_p.getAveragingInterval () * chunkRatio;
    Bool isWriteable = parameters_p.isWriteable();

    // Create a simple VI implementation to perform the reading.

    VisibilityIteratorImpl2 * vii2 = new VisibilityIteratorImpl2 (mss_p,
                                                                  parameters_p.getSortColumns (),
                                                                  chunkInterval,
                                                                  isWriteable);

    vii2->setWeightScaling (parameters_p.getWeightScaling());

    AveragingTvi2 * averagingTvi2 = new AveragingTvi2 (vii2, parameters_p);

    return averagingTvi2;
}

// AveragingVi2LayerFactory ctor
AveragingVi2LayerFactory::AveragingVi2LayerFactory(const AveragingParameters& avepars)
  : ViiLayerFactory(),
    avepars_p(avepars)
{}

// AveragingVi2-specific layer-creator
ViImplementation2*
AveragingVi2LayerFactory::createInstance (ViImplementation2* vii0) const 
{
    // Make the AveragintTvi2, using supplied ViImplementation2, and return it
    ViImplementation2 *vii = new AveragingTvi2(vii0,avepars_p);
    return vii;
}





} // end namesapce vi
using namespace casacore;
} // end namespace casa
