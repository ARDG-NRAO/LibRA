/*
 * VisBufferComponents.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: jjacobs
 */

#include <msvis/MSVis/UtilJ.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <map>

using namespace std;

using namespace casacore;
namespace casa {

namespace vi {

const VisBufferComponents2 VisBufferComponents2::all_p = initializeAll ();

VisBufferComponents2
VisBufferComponents2::operator+ (const VisBufferComponents2 & other) const
{
    VisBufferComponents2 result = * this;

    result.set_p.insert (other.begin(), other.end());

    return result;
}

VisBufferComponents2 &
VisBufferComponents2::operator+= (const VisBufferComponents2 & other)
{
    if (& other != this){

        set_p.insert (other.set_p.begin(), other.set_p.end());
    }

    return * this;
}

VisBufferComponents2 &
VisBufferComponents2::operator+= (VisBufferComponent2 componentToAdd)
{
    set_p.insert (componentToAdd);

    return * this;
}


VisBufferComponents2
VisBufferComponents2::all ()
{
    return all_p;
}

VisBufferComponents2::const_iterator
VisBufferComponents2::begin () const
{
    return set_p.begin();
}

Bool
VisBufferComponents2::contains (VisBufferComponent2 component) const
{
    return utilj::containsKey (component, set_p);
}

Bool
VisBufferComponents2::empty () const
{
    return set_p.empty();
}

VisBufferComponents2::const_iterator
VisBufferComponents2::end () const
{
    return set_p.end();
}

VisBufferComponents2
VisBufferComponents2::exceptThese (std::initializer_list<VisBufferComponent2> components)
{
   VisBufferComponents2 result = all();

   for (auto component : components){
       result.set_p.erase (component);
   }

   return result;
}

VisBufferComponents2
VisBufferComponents2::initializeAll ()
{

    VisBufferComponents2 all;

//    VisBufferComponent2
//    writableComponents [] = {Corrected,
//                             CorrectedCube,
//                             Flag,
//                             FlagCube,
//                             FlagRow,
//                             Model,
//                             ModelCube,
//                             Observed,
//                             ObservedCube,
//                             Sigma,
//                             SigmaMat,
//                             Weight,
//                             WeightMat,
//                             Unknown};

    for (Int i = 0; i < static_cast<int> (VisBufferComponent2::N_VisBufferComponents2); i++){

        all.set_p.insert (static_cast<VisBufferComponent2> (i));
    }

    return all;
}


String
VisBufferComponents2::name (VisBufferComponent2 id)
{
   // This method is called only occasionally for debugging so at this time a
   // brute-force implmentation is acceptable.

    map<VisBufferComponent2,String> names;

    names [VisBufferComponent2::AllBeamOffsetsZero] = "AllBeamOffsetsZero";
    names [VisBufferComponent2::AntennaMounts] = "AntennaMounts";
    names [VisBufferComponent2::Antenna1] = "Antenna1";
    names [VisBufferComponent2::Antenna2] = "Antenna2";
    names [VisBufferComponent2::ArrayId] = "ArrayId";
    names [VisBufferComponent2::BeamOffsets] = "BeamOffsets";
    names [VisBufferComponent2::CorrType] = "CorrType";
    names [VisBufferComponent2::DataDescriptionIds] = "DataDescriptionIds";
    names [VisBufferComponent2::Direction1] = "Direction1";
    names [VisBufferComponent2::Direction2] = "Direction2";
    names [VisBufferComponent2::Exposure] = "Exposure";
    names [VisBufferComponent2::Feed1] = "Feed1";
    names [VisBufferComponent2::FeedPa1] = "FeedPa1";
    names [VisBufferComponent2::Feed2] = "Feed2";
    names [VisBufferComponent2::FeedPa2] = "FeedPa2";
    names [VisBufferComponent2::FieldId] = "FieldId";
    //names [VisBufferComponent2::Flag] = "Flag";
    names [VisBufferComponent2::FlagCategory] = "FlagCategory";
    names [VisBufferComponent2::FlagCube] = "FlagCube";
    names [VisBufferComponent2::FlagCubes] = "FlagCubes";
    names [VisBufferComponent2::FlagRow] = "FlagRow";
    names [VisBufferComponent2::FloatData] = "FloatData";
    names [VisBufferComponent2::Frequencies] = "Frequencies";
    names [VisBufferComponent2::ImagingWeight] = "ImagingWeight";
    names [VisBufferComponent2::JonesC] = "JonesC";
    names [VisBufferComponent2::NAntennas] = "NAntennas";
    names [VisBufferComponent2::NChannels] = "NChannels";
    names [VisBufferComponent2::NCorrelations] = "NCorrelations";
    names [VisBufferComponent2::NRows] = "NRows";
    names [VisBufferComponent2::NShapes] = "NShapes";
    names [VisBufferComponent2::NRowsPerShape] = "NRowsPerShape";
    names [VisBufferComponent2::NChannelsPerShape] = "NChannelsPerShape";
    names [VisBufferComponent2::NCorrelationsPerShape] = "NCorrelationsPerShape";
    names [VisBufferComponent2::ObservationId] = "ObservationId";
    names [VisBufferComponent2::PhaseCenter] = "PhaseCenter";
    names [VisBufferComponent2::PolFrame] = "PolFrame";
    names [VisBufferComponent2::PolarizationId] = "PolarizationId";
    names [VisBufferComponent2::ProcessorId] = "ProcessorId";
    names [VisBufferComponent2::ReceptorAngles] = "ReceptorAngles";
    names [VisBufferComponent2::RowIds] = "RowIds";
    names [VisBufferComponent2::Scan] = "Scan";
    names [VisBufferComponent2::Sigma] = "Sigma";
    names [VisBufferComponent2::Sigmas] = "Sigmas";
    names [VisBufferComponent2::SpectralWindows] = "SpectralWindows";
    names [VisBufferComponent2::StateId] = "StateId";
    names [VisBufferComponent2::Time] = "Time";
    names [VisBufferComponent2::TimeCentroid] = "TimeCentroid";
    names [VisBufferComponent2::TimeInterval] = "TimeInterval";
    names [VisBufferComponent2::Weight] = "Weight";
    names [VisBufferComponent2::Weights] = "Weights";
    names [VisBufferComponent2::WeightScaled] = "WeightScaled";
    names [VisBufferComponent2::WeightSpectrum] = "WeightSpectrum";
    names [VisBufferComponent2::WeightSpectra] = "WeightSpectra";
    names [VisBufferComponent2::SigmaSpectrum] = "SigmaSpectrum";
    names [VisBufferComponent2::SigmaSpectra] = "SigmaSpectra";
    names [VisBufferComponent2::WeightSpectrumScaled] = "WeightSpectrumScaled";
    names [VisBufferComponent2::Uvw] = "Uvw";
    names [VisBufferComponent2::VisibilityCorrected] = "VisibilityCorrected";
    names [VisBufferComponent2::VisibilityModel] = "VisibilityModel";
    names [VisBufferComponent2::VisibilityObserved] = "VisibilityObserved";
    names [VisBufferComponent2::VisibilityCubeCorrected] = "VisibilityCubeCorrected";
    names [VisBufferComponent2::VisibilityCubeModel] = "VisibilityCubeModel";
    names [VisBufferComponent2::VisibilityCubeObserved] = "VisibilityCubeObserved";
    names [VisBufferComponent2::VisibilityCubeFloat] = "VisibilityCubeFloat";
    names [VisBufferComponent2::VisibilityCubesCorrected] = "VisibilityCubesCorrected";
    names [VisBufferComponent2::VisibilityCubesModel] = "VisibilityCubesModel";
    names [VisBufferComponent2::VisibilityCubesObserved] = "VisibilityCubesObserved";
    names [VisBufferComponent2::VisibilityCubesFloat] = "VisibilityCubesFloat";

    assert (names.size() == static_cast<int> (VisBufferComponent2::N_VisBufferComponents2));

    return names [id];
}

VisBufferComponents2
VisBufferComponents2::none ()
{
    return VisBufferComponents2 ();
}

VisBufferComponents2
VisBufferComponents2::singleton (VisBufferComponent2 component)
{
    VisBufferComponents2 result;
    result.set_p.insert (component);

    return result;
}

VisBufferComponents2
VisBufferComponents2::these (std::initializer_list<VisBufferComponent2> components)
{
   VisBufferComponents2 result;

   for (auto component : components){
       result.set_p.insert (component);
   }

   return result;
}


String
VisBufferComponents2::toString () const
{
    String result = "{";

    for (const_iterator i = begin(); i != end(); i++){
        result += name (*i) + ",";
    }

    * result.rbegin() = '}'; // replace last "," with "}"

    return result;
}


} // end namespace vi

using namespace casacore;
} // end namespace casa
