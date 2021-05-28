#include <msvis/MSVis/ViColumns2.h>
#include <ms/MeasurementSets.h>

using namespace casacore;
namespace casa {

namespace vi {

ViColumns2::ViColumns2 ()
: antenna1_p (),
  antenna2_p (),
  array_p (),
  corrVis_p (),
  dataDescription_p (),
  exposure_p (),
  feed1_p (),
  feed2_p (),
  field_p (),
  flagCategory_p (),
  flagRow_p (),
  flag_p (),
  floatVis_p (),
  modelVis_p (),
  observation_p (),
  processor_p (),
  scan_p (),
  sigma_p (),
  state_p (),
  timeCentroid_p (),
  timeInterval_p (),
  time_p (),
  uvw_p (),
  vis_p (),
  weightSpectrum_p (),
  sigmaSpectrum_p (),
  weight_p (),
  floatDataFound_p (false)
{}


void
ViColumns2::attachColumns (const Table & t, bool attachSpecialColumns)
{
    const ColumnDescSet & cds = t.tableDesc ().columnDescSet ();

    antenna1_p.attach (t, MS::columnName (MS::ANTENNA1));
    antenna2_p.attach (t, MS::columnName (MS::ANTENNA2));

    if (cds.isDefined ("CORRECTED_DATA")) {
        corrVis_p.attach (t, "CORRECTED_DATA");
    }

    exposure_p.attach (t, MS::columnName (MS::EXPOSURE));
    feed1_p.attach (t, MS::columnName (MS::FEED1));
    feed2_p.attach (t, MS::columnName (MS::FEED2));
    flag_p.attach (t, MS::columnName (MS::FLAG));
    flagCategory_p.attach (t, MS::columnName (MS::FLAG_CATEGORY));
    flagRow_p.attach (t, MS::columnName (MS::FLAG_ROW));

    if (cds.isDefined (MS::columnName (MS::FLOAT_DATA))) {
        floatVis_p.attach (t, MS::columnName (MS::FLOAT_DATA));
        floatDataFound_p = true;
    } else {
        floatDataFound_p = false;
    }

    if (cds.isDefined ("MODEL_DATA")) {
        modelVis_p.attach (t, "MODEL_DATA");
    }

    observation_p.attach (t, MS::columnName (MS::OBSERVATION_ID));
    processor_p.attach (t, MS::columnName (MS::PROCESSOR_ID));
    scan_p.attach (t, MS::columnName (MS::SCAN_NUMBER));
    sigma_p.attach (t, MS::columnName (MS::SIGMA));
    state_p.attach (t, MS::columnName (MS::STATE_ID));
    time_p.attach (t, MS::columnName (MS::TIME));
    timeCentroid_p.attach (t, MS::columnName (MS::TIME_CENTROID));
    timeInterval_p.attach (t, MS::columnName (MS::INTERVAL));
    uvw_p.attach (t, MS::columnName (MS::UVW));

    if (cds.isDefined (MS::columnName (MS::DATA))) {
        vis_p.attach (t, MS::columnName (MS::DATA));
    }

    weight_p.attach (t, MS::columnName (MS::WEIGHT));

    if (cds.isDefined (MS::columnName (MS::WEIGHT_SPECTRUM))) {
        weightSpectrum_p.attach (t, MS::columnName (MS::WEIGHT_SPECTRUM));
    }

    if (cds.isDefined (MS::columnName (MS::SIGMA_SPECTRUM))) {
    	sigmaSpectrum_p.attach (t, MS::columnName (MS::SIGMA_SPECTRUM));
    }

    if (attachSpecialColumns){

        array_p.attach (t, MS::columnName (MS::ARRAY_ID));

        dataDescription_p.attach (t, MS::columnName (MS::DATA_DESC_ID));

        field_p.attach (t, MS::columnName (MS::FIELD_ID));
    }
}

Bool
ViColumns2::isFloatDataPresent () const
{
    return floatDataFound_p;
}

} // end namespace vi

using namespace casacore;
} // end namespace casa
