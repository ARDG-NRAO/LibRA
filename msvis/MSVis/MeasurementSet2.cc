#include <msvis/MSVis/MeasurementSet2.h>
#include <ms/MSSel/MSSelection.h>
#include <tables/Tables.h>

using namespace casacore;

namespace casa {


MeasurementSet2::MeasurementSet2 (const Table & table, MSSelection * msSelection)
: MeasurementSet (table),
  msSelection_p (msSelection) // ours now
{
}

MeasurementSet2::MeasurementSet2 (const MeasurementSet2 & other)
: MeasurementSet (other),
  msSelection_p (0)
{
    msSelection_p = new MSSelection (* other.msSelection_p);
    wasCreatedWithSelection_p = other.wasCreatedWithSelection();
}

MeasurementSet2::~MeasurementSet2 ()
{
    delete msSelection_p;
}

MeasurementSet2 &
MeasurementSet2::operator= (const MeasurementSet2 & other)
{
    MeasurementSet::operator= (other);

    msSelection_p = new MSSelection (* other.msSelection_p);
    wasCreatedWithSelection_p = other.wasCreatedWithSelection();

    return * this;
}

MeasurementSet2
MeasurementSet2::create (const String & filename, const MSSelection * msSelection)
{
    MeasurementSet msOriginal (filename);

    MSSelection * msSelectionCopy = new MSSelection (* msSelection); // make a private copy

    TableExprNode node = msSelectionCopy->toTableExprNode (& msOriginal);

    Table tablesel(msOriginal.tableName(), Table::Update);

    MeasurementSet2 ms2 (tablesel(node, node.nrow()), msSelectionCopy);
        // The MS takes ownership of the MSSelection

    return ms2;
}


const MSSelection *
MeasurementSet2::getMSSelection () const
{
    return msSelection_p;
}

Bool
MeasurementSet2::wasCreatedWithSelection () const
{
    return wasCreatedWithSelection_p;
}

} // end namespace casa

