#if ! defined (MeasurementSet2_H_121204_1015)
#define MeasurementSet2_H_121204_1015

#include <casacore/ms/MeasurementSets.h>

namespace casacore{

class String;
class MSSelection;
}

namespace casa {


class MeasurementSet2 : public casacore::MeasurementSet {

public:

    ~MeasurementSet2 ();

    MeasurementSet2 (const MeasurementSet2 & other);
    MeasurementSet2 & operator= (const MeasurementSet2 & other);

    const casacore::MSSelection * getMSSelection () const;
    casacore::Bool wasCreatedWithSelection () const;

    static MeasurementSet2 create (const casacore::String & filename, const casacore::MSSelection * msSelection);

protected:

    MeasurementSet2 (const casacore::Table & table, casacore::MSSelection * msSelection);

private:

    casacore::MSSelection * msSelection_p;     // [own]
    casacore::Bool wasCreatedWithSelection_p;

};

}


#endif // ! defined (MeasurementSet2_H_121204_1015)
