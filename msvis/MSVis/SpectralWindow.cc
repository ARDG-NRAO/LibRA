/*
 * SpectralWindow.cc
 *
 *  Created on: Feb 15, 2013
 *      Author: jjacobs
 */


#include <ms/MeasurementSets.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <tables/Tables.h>
#include <msvis/MSVis/SpectralWindow.h>
#include <memory>
#include <msvis/MSVis/UtilJ.h>

using namespace std;
using namespace casacore;

namespace casa {
namespace ms {

SpectralWindow::SpectralWindow (const MSSpWindowColumns & columns, Int spectralWindowId)
: id_p (spectralWindowId)
{
    fillScalars (columns);

    fillArrays (columns);
}

void
SpectralWindow::fillArrays(const MSSpWindowColumns & columns)
{
    // Read in the data as parallel arrays

    Vector<Double> effectiveBandwidth = getArray (columns.effectiveBW());
    Vector<Double> frequency = getArray (columns.chanFreq());
    Vector<Double> resolution = getArray (columns.resolution());
    Vector<Double> width = getArray (columns.chanWidth ());

    // Use the arrays to create an array of channel objects

    for (Int i = 0; i <  (int) effectiveBandwidth.nelements(); i ++){

        channels_p.push_back(SpectralChannel (frequency (i), width(i),
                                              effectiveBandwidth (i), resolution(i)));
    }
}

Vector<Double>
SpectralWindow::getArray (const ArrayColumn<Double> & arrayColumn)
{
    Vector<Double> array;
    arrayColumn.get (id_p, array, true);

    return array;
}

template<typename T>
T
SpectralWindow::getScalar (const ScalarColumn<T> & column)
{
    return column.get (id_p);
}

void
SpectralWindow::fillScalars (const MSSpWindowColumns & columns)
{
    flagged_p = getScalar (columns.flagRow());
    frequencyGroup_p = getScalar (columns.freqGroup());
    frequencyGroupName_p = getScalar (columns.freqGroupName());
    frequencyMeasureReference_p = getScalar (columns.measFreqRef());
    ifConversionChain_p = getScalar (columns.ifConvChain());
    name_p = getScalar (columns.name());
    nChannels_p = getScalar (columns.numChan());
    netSideband_p = getScalar (columns.netSideband());
    totalBandwidth_p = getScalar (columns.totalBandwidth());
    referenceFrequency_p = getScalar (columns.refFrequency());
}

SpectralWindows::SpectralWindows (const MeasurementSet * ms)
{
    // Get the Spectral Columns Object

    unique_ptr <MSColumns> msColumns (new MSColumns (* ms));
    const MSSpWindowColumns & spectralWindowColumns = msColumns->spectralWindow();

    // Create on spectral window object per row in the table.

    Int nRows = spectralWindowColumns.nrow();

    for (Int row = 0; row < nRows; row ++){

        windows_p.push_back (SpectralWindow (spectralWindowColumns, row));
    }
}

SpectralWindows::const_iterator
SpectralWindows::begin () const
{
    return windows_p.begin();
}

bool
SpectralWindows::empty () const
{
    return windows_p.empty();
}

SpectralWindows::const_iterator
SpectralWindows::end () const
{
    return windows_p.end();
}

const SpectralWindow &
SpectralWindows::get (Int id) const
{
    Assert (id >= 0 && id < (int) size());

    return windows_p [id];
}

size_t
SpectralWindows::size () const
{
    return windows_p.size();
}

const SpectralChannel &
SpectralWindow::get (Int i) const
{
    Assert (i >= 0 && i < (int) nChannels());
    return channels_p [i];
}


Bool
SpectralWindow::isFlagged () const
{
    return flagged_p;
}

Int
SpectralWindow::frequencyGroup () const
{
    return frequencyGroup_p;
}

String
SpectralWindow::frequencyGroupName () const
{
    return frequencyGroupName_p;
}

Int
SpectralWindow::frequencyMeasureReference () const
{
    return frequencyMeasureReference_p;
}

Int
SpectralWindow::id () const
{
    return id_p;
}


Int
SpectralWindow::ifConversionChain () const
{
    return ifConversionChain_p;
}

String
SpectralWindow::name () const
{
    return name_p;
}

Int
SpectralWindow::nChannels () const
{
    return nChannels_p;
}

Int
SpectralWindow::netSideband () const
{
    return netSideband_p;
}

Double
SpectralWindow::totalBandwidth () const
{
    return totalBandwidth_p;
}

Double
SpectralWindow::referenceFrequency () const
{
    return referenceFrequency_p;
}



} // end namespace vi
} // end namespace casa


