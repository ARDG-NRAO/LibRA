#if ! defined (ViColumns_H_121126_1506)
#define ViColumns_H_121126_1506

#include <casacore/casa/aips.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>


namespace casa {

namespace vi {

class ViColumns2 {

public:

    ViColumns2 ();

    void attachColumns (const casacore::Table & t, bool attachSpecialColumns = true);


    casacore::ScalarColumn<casacore::Int>    antenna1_p;
    casacore::ScalarColumn<casacore::Int>    antenna2_p;
    casacore::ScalarColumn<casacore::Int>    array_p;
    casacore::ArrayColumn<casacore::Complex> corrVis_p;
    casacore::ScalarColumn<casacore::Int>    dataDescription_p;
    casacore::ScalarColumn<casacore::Double> exposure_p;
    casacore::ScalarColumn<casacore::Int>    feed1_p;
    casacore::ScalarColumn<casacore::Int>    feed2_p;
    casacore::ScalarColumn<casacore::Int>    field_p;
    casacore::ArrayColumn<casacore::Bool>    flagCategory_p;
    casacore::ScalarColumn<casacore::Bool>   flagRow_p;
    casacore::ArrayColumn<casacore::Bool>    flag_p;
    casacore::ArrayColumn<casacore::Float>   floatVis_p;
    casacore::ArrayColumn<casacore::Complex> modelVis_p;
    casacore::ScalarColumn<casacore::Int>    observation_p;
    casacore::ScalarColumn<casacore::Int>    processor_p;
    casacore::ScalarColumn<casacore::Int>    scan_p;
    casacore::ArrayColumn<casacore::Float>   sigma_p;
    casacore::ScalarColumn<casacore::Int>    state_p;
    casacore::ScalarColumn<casacore::Double> timeCentroid_p;
    casacore::ScalarColumn<casacore::Double> timeInterval_p;
    casacore::ScalarColumn<casacore::Double> time_p;
    casacore::ArrayColumn<casacore::Double>  uvw_p;
    casacore::ArrayColumn<casacore::Complex> vis_p;
    casacore::ArrayColumn<casacore::Float>   weightSpectrum_p;
    casacore::ArrayColumn<casacore::Float>   sigmaSpectrum_p;
    casacore::ArrayColumn<casacore::Float>   weight_p;

    casacore::Bool isFloatDataPresent () const;

private:

    casacore::Bool floatDataFound_p;

};

} // namespace vi

} // end namespace casa

#endif // ! defined (ViColumns_H_121126_1506)
