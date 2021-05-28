/*
 * SpectralWindow.h
 *
 *  Created on: Feb 15, 2013
 *      Author: jjacobs
 */

#ifndef MSVIS_SPECTRALWINDOW_H_
#define MSVIS_SPECTRALWINDOW_H_

#include <casa/aipstype.h>
#include <casa/BasicSL.h>
#include <vector>

namespace casacore{

template<typename T> class ArrayColumn;
class MeasurementSet;
}

namespace casa {

namespace ms {

  //#warning "Needs battery of low-level tests; also needs write functionality"

class SpectralChannel {

public:

    SpectralChannel (casacore::Double frequency, casacore::Double width,
                     casacore::Double effectiveBandwidth, casacore::Double resolution)
    : effectiveBandwidth_p (effectiveBandwidth),
      frequency_p (frequency),
      resolution_p (resolution),
      width_p (width)
    {}

    casacore::Double effectiveBandwidth () const;
    casacore::Double frequency () const;
    casacore::Double resolution () const;
    casacore::Double width () const;

private:

    casacore::Double effectiveBandwidth_p;
    casacore::Double frequency_p;
    casacore::Double resolution_p;
    casacore::Double width_p;

};

class SpectralWindow {

public:

    typedef std::vector<SpectralChannel> Channels;
    typedef Channels::const_iterator const_iterator;

    SpectralWindow (const casacore::MSSpWindowColumns & columns, casacore::Int spectralWindowId);

    const_iterator begin () const;
    bool empty () const;
    const_iterator end () const;
    const SpectralChannel & get (casacore::Int i) const;

    casacore::Int id () const;
    casacore::Bool isFlagged () const;
    casacore::Int frequencyGroup () const;
    casacore::String frequencyGroupName () const;
    casacore::Int frequencyMeasureReference () const;
    casacore::Int ifConversionChain () const;
    casacore::String name () const;
    int nChannels () const;
    int netSideband () const;
    casacore::Double totalBandwidth () const;
    casacore::Double referenceFrequency () const;

protected:

    void fillArrays(const casacore::MSSpWindowColumns & columns);
    casacore::Vector<casacore::Double> getArray (const casacore::ArrayColumn<casacore::Double> & arrayColumn);
    void fillScalars (const casacore::MSSpWindowColumns & columns);
    template<typename T>
    T getScalar (const casacore::ScalarColumn<T> & column);

private:

    Channels channels_p;
    casacore::Bool flagged_p;
    casacore::Int frequencyGroup_p;
    casacore::String frequencyGroupName_p;
    casacore::Int frequencyMeasureReference_p;
    casacore::Int id_p;
    casacore::Int ifConversionChain_p;
    casacore::String name_p;
    int nChannels_p;
    int netSideband_p;
    casacore::Double totalBandwidth_p;
    casacore::Double referenceFrequency_p;
};

class SpectralWindows {

public:

    typedef std::vector<SpectralWindow> Windows;
    typedef Windows::const_iterator const_iterator;

    SpectralWindows (const casacore::MeasurementSet * ms);

    const_iterator begin () const;
    bool empty () const;
    const_iterator end () const;
    const SpectralWindow & get (casacore::Int id) const;
    size_t size () const;

protected:

private:

    Windows windows_p;
};

} // end namespace vi

} // end namespace casa

#endif /* SPECTRALWINDOW_H_ */
