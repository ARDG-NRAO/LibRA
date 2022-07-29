//# VisibilityIterator2.cc: Step through MeasurementEquation by visibility
//# Copyright (C) 1996-2012
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the !GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: VisibilityIterator2.cc,v 19.15 2006/02/01 01:25:14 kgolap Exp $

#include <tuple>
#include <casacore/casa/Arrays.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Exceptions.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/System/AipsrcValue.h>
#include <casacore/casa/Utilities.h>
#include <casacore/ms/MeasurementSets.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/ms/MSSel/MSSpwIndex.h>
#include <casacore/scimath/Mathematics/InterpolateArray1D.h>
//#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/MeasurementSet2.h>
#include <msvis/MSVis/MSUtil.h>
#include <msvis/MSVis/MSIter2.h>
#include <msvis/MSVis/UtilJ.h>
#include <msvis/MSVis/SpectralWindow.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>
#include <msvis/MSVis/PointingDirectionCache.h>
#include <msvis/MSVis/VisModelDataI.h>
#include <casacore/tables/Tables/ColDescSet.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/DataMan/IncrStManAccessor.h>
#include <casacore/tables/DataMan/StandardStManAccessor.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/DataMan/TiledStManAccessor.h>

#include <cassert>
#include <algorithm>
#include <limits>
#include <memory>
#include <map>
#include <vector>

using std::make_pair;
using namespace casacore;
using namespace casa::vi;
using namespace std;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi {

Bool
operator!=(const Slice & a, const Slice & b)
{
	Bool result = a.start() != b.start()||
		a.length() != b.length() ||
		a.inc() != b.inc();

	return result;
}

// ChannelSubslicer - represents a single selection from a DATA matrix
//
// Typically the ChannelSubslicer uses selects elements from the DATA matrix in
// two dimensions: correlation and channel.  Two slice objects are used, the
// first one for correlation and the other for channel.  The slice object is a
// triple of starting channel, increment and the number of channels.

class ChannelSubslicer
{

public:

	enum {Correlation, Channel};

	ChannelSubslicer()
		: subslicer_p()
		{}

	ChannelSubslicer(Int n)
		: subslicer_p(n)
		{}

	ChannelSubslicer(const Vector<Slice> & axis)
		: subslicer_p(axis.nelements())
		{
			for (uInt i = 0; i < axis.nelements(); i++) {
				subslicer_p[i] = axis(i);
			}
		}

	Bool
	operator==(const ChannelSubslicer & other) const
		{
			if (other.nelements() != nelements()) {
				return false;
			}

			for (uInt i = 0; i < nelements(); i++) {

				if (!slicesEqual(subslicer_p[i], other.subslicer_p[i])) {
					return false;
				}
			}

			return true;
		}

	Bool
	operator!=(const ChannelSubslicer & other) const
		{
			return !(* this == other);
		}

	const Slice &
	getSlice(Int i) const
		{
			return subslicer_p[i];
		}

	Vector<Slice>
	getSlices() const
		{
			return Vector<Slice>(subslicer_p);
		}

	size_t nelements() const
		{
			return subslicer_p.size();
		}

	void
	setSlice(Int i, const Slice & slice)
		{
			subslicer_p[i] = slice;
		}

protected:

	static Bool
	slicesEqual(const Slice & a, const Slice & b) {

		return a.start() == b.start() &&
			a.length() == b.length() &&
			a.inc() == b.inc();

	}

private:

	vector<Slice> subslicer_p;
};

// ChannelSlicer - represents the channels selected in a window.
//
// The ChannelSlicer is a collection of channel and correlation selections.
// Each selection, a ChannelSubslicer, specifies a slice for each dimension of
// the data cube: correlation, channel (row selection is not managed by this
// data structure).

class ChannelSlicer
{

public:

	typedef vector<ChannelSubslicer> Rep;
	typedef Vector<Vector <Slice> > CoreRep;

	ChannelSlicer()
		: slicer_p()
		{}

	ChannelSlicer(Int nAxes)
		: slicer_p(nAxes)
		{}

	bool
	operator==(const ChannelSlicer & other) const
		{

			if (nelements() != other.nelements()) {
				return false;
			}


			for (uInt i = 0; i < nelements(); i++) {

				if (slicer_p[i] != other.slicer_p[i]) {
					return false;
				}

			}

			return true;
		}

	void
	appendDimension()
		{
			slicer_p.push_back(ChannelSubslicer());
		}

	CoreRep
	getSlicerInCoreRep() const
		{
			// Convert to Vector<Vector <Slice> > for use by
			// casacore methods

			CoreRep rep(nelements());

			for (uInt i = 0; i < nelements(); i++) {

				const ChannelSubslicer & subslicer = slicer_p[i];

				rep[i] = subslicer.getSlices();

			}

			return rep;
		}

	ColumnSlicer
	getColumnSlicer() const
		{

			// The goal is to create a 2D array of Slicers.  Two types of
			// Slicers are required: one to slice into the data as read and
			// another to put it into the target array.  Both are computed and
			// returned here.  We are effectively computing a cross-product
			// between the correlation and channel slices.

			// might be able to replace this?
			CoreRep slicing = getSlicerInCoreRep();

			Vector<Slice> correlationSlices = slicing(0);
			Vector<Slice> channelSlices = slicing(1);

			IPosition start(2), length(2), increment(2);

			uInt nCorrelationSlices = slicing(0).size();
			uInt nChannelSlices = channelSlices.size();
			uInt nSlices = nCorrelationSlices * nChannelSlices;

			Vector<Slicer *> dataSlices(nSlices, 0);
			Vector<Slicer *> destinationSlices(nSlices, 0);
			IPosition shape(2, 0);

			uInt outputSlice = 0;
			const uInt Channel = 1;
			const uInt Correlation = 0;

			uInt correlationDestination = 0;

			for (uInt correlationSlice = 0;
			     correlationSlice < nCorrelationSlices;
			     correlationSlice++) {

				const Slice & aSlice = correlationSlices(correlationSlice);


				uInt channelDestination = 0;

				for (uInt channelSlice = 0;
				     channelSlice < nChannelSlices; channelSlice++) {

					const Slice & bSlice = channelSlices(channelSlice);

					start(Channel) = bSlice.start();
					length(Channel) = bSlice.length();
					increment(Channel) = bSlice.inc();

					// Can't move outside loop because of of mutation during
					// destination logic below
					start(Correlation) = aSlice.start();
					length(Correlation) = aSlice.length();
					increment(Correlation) = aSlice.inc();

					dataSlices[outputSlice] =
						new Slicer(start, length, increment);

					// The destination slicer will always have increment one and
					// the same length as the data slicer.  However, it's
					// starting location depends on how many slices (both axes)
					// it is away from the origin.

					start(Channel) = channelDestination;
					increment(Channel) = 1;
					channelDestination += length(Channel);

					start(Correlation) = correlationDestination;
					increment(Correlation) = 1;

					destinationSlices[outputSlice] =
						new Slicer(start, length, increment);

					outputSlice++;
					shape(Channel) = max(shape(Channel), channelDestination);

				}

				correlationDestination += length(Correlation);
				shape(Correlation) = correlationDestination;
			}

			return ColumnSlicer(shape, dataSlices, destinationSlices);
		}

	const ChannelSubslicer &
	getSubslicer(Int i) const
		{
			return slicer_p[i];
		}

	size_t nelements() const
		{
			return slicer_p.size();
		}

	void
	setSubslicer(Int i, const ChannelSubslicer & subslice)
		{
			slicer_p[i] = subslice;
		}


	String
	toString() const
		{
			String result = "{";

			for (Rep::const_iterator i = slicer_p.begin();
			     i != slicer_p.end();
			     i++) {

				result += String((i == slicer_p.begin()) ? "" : ", ") + "{ ";

				const ChannelSubslicer & subslicer = * i;

				for (uInt j = 0; j < subslicer.nelements(); j++) {

					const Slice & slice = subslicer.getSlice(j);
					result += String::format(
						"(st=%d, len=%d, inc=%d)",
						slice.start(), slice.length(), slice.inc());
				}

				result += " }";
			}

			result += " }";

			return result;
		}

private:

	Rep slicer_p;
};

// ChannelSelector - class that provides the channels that need to be extracted
//                   from a spectral window at a given time for a given MS.
//

class ChannelSelector
{

public:

	ChannelSelector(
		Double time,
		Int _msId,
		Int _spectralWindowId,
		Int _polarizationId,
		const ChannelSlicer & slicer)
		: msId(_msId)
		, polarizationId(_polarizationId)
		, spectralWindowId(_spectralWindowId)
		, timeStamp(time)
		, slicer_p(slicer)
		{
			// Count up the number of frequencies selected

			const ChannelSubslicer & frequencySlicer = slicer_p.getSubslicer(1);

			nFrequencies_p = 0;

			for (Int i = 0; i < (int) frequencySlicer.nelements(); i++) {

				nFrequencies_p += frequencySlicer.getSlice(i).length();
			}

			// Create the slicer for FlagCategory data which can't use the
			// normal slicer.

			createFlagCategorySlicer();
		}

	Bool
	equals(const ChannelSelector & other) const
		{
			// To be equal the other ChannelSelector must be for the same
			// MS and spectral window

			Bool equal = msId == other.msId &&
				spectralWindowId == other.spectralWindowId;

			// If the timestamps match, then they're equal

			if (timeStamp == other.timeStamp) {
				return true;
			}

			// They differed on timestamps, but if they select the same channels
			// then they're equivalent.

			equal = equal && slicer_p == other.slicer_p;

			return equal;
		}

	void
	createFlagCategorySlicer()
		{
			// The shape of the flag categories column cell is [nC, nF,
			//nCategories] whereas every other sliced column cell is[nC, nF].
			//Use the normal slicer to create the flag category slicer.

			slicerFlagCategories_p = slicer_p;
			slicerFlagCategories_p.appendDimension();

		}

	// Returns a list of channels to be extracted.

	Vector<Int>
	getChannels() const
		{

			// create result of appropriate size
			Vector<Int> frequencies(nFrequencies_p);

			// get channel axis of slicer
			const ChannelSubslicer & channelSlices = slicer_p.getSubslicer(1);

			// Iterator over all of the slices contained in the channel portion
			// of the slicer.  For each slice, use each channel number to fill
			// in the appropriate index of the result.  The result will be that
			// frequencies[x] will contain the actual channel number that
			// resulted from the frequency selection process.

			int k = 0;

			for (int i = 0; i < (int) channelSlices.nelements(); i++) {

				const Slice & slice = channelSlices.getSlice(i);
				Int channel = slice.start();
				Int increment = slice.inc();
				Int nChannels = slice.length();

				assert(k + nChannels - 1 <= nFrequencies_p);

				for (int j = 0; j < (int) nChannels; j++) {

					frequencies[k++] = channel;
					channel += increment;
				}

			}

			return frequencies;
		}

	Vector<Int>
	getCorrelations() const {

		const ChannelSubslicer & correlationAxis = slicer_p.getSubslicer(0);

		vector<Int> correlations;

		for (uInt i = 0; i < correlationAxis.nelements(); i++) {

			const Slice & slice = correlationAxis.getSlice(i);

			for (uInt j = slice.start(), n = 0;
			     n < slice.length();
			     j += slice.inc()) {

				correlations.push_back(j);
				n++;

			}
		}

		Vector<Int> result(correlations.size());

		for (uInt i = 0; i < correlations.size(); i++) {
			result[i] = correlations[i];
		}

		return result;
	}

	Int
	getNFrequencies() const
		{
			return nFrequencies_p;
		}

	// Returns the ChannelSlicer object which contains the actual
	// channelselection for the current time, window and MS.

	const ChannelSlicer &
	getSlicer() const
		{
			return slicer_p;
		}

	const ChannelSlicer &
	getSlicerForFlagCategories() const
		{
			return slicerFlagCategories_p;
		}

	const Int msId;
	const Int polarizationId;
	const Int spectralWindowId;
	const Double timeStamp;

private:

	Int nFrequencies_p;
	ChannelSlicer slicer_p;
	ChannelSlicer slicerFlagCategories_p;
};

class ChannelSelectorCache
{
public:

    ChannelSelectorCache(Int maxEntries = 200)
        : frameOfReference_p(FrequencySelection::Unknown)
        , maxEntries_p(maxEntries)
        , msId_p(-1)
    {}

    ~ChannelSelectorCache() {};

    void add(std::shared_ptr<ChannelSelector> entry, Int frameOfReference)
    {
        if (entry->msId != msId_p
            || frameOfReference != frameOfReference_p) {

            // Cache only holds values for a single MS and a single frame of
            // reference at a time.

            flush();
    
            msId_p = entry->msId;
            frameOfReference_p = frameOfReference;
        }

        if (cache_p.size() >= maxEntries_p) {

            // Boot the first entry out of the cache.
            // In most situations would have the
            // lowest timestamp and lowest spectral window id.

            cache_p.erase(cache_p.begin());
        }

        Double time =
            ((frameOfReference_p == FrequencySelection::ByChannel)
             ? -1 // channel selection not function of time
             : entry->timeStamp);

        // take ownership of entry
        cache_p[Key(time, entry->spectralWindowId)] = entry;
    }

    std::shared_ptr<ChannelSelector> 
    find(Double time, Int msId, Int frameOfReference,
         Int spectralWindowId) const {

        std::shared_ptr<ChannelSelector> result = nullptr;

        if (msId == msId_p && frameOfReference == frameOfReference_p) {

            if (frameOfReference_p == FrequencySelection::ByChannel) {
                time = -1; // channel selection is not function of time
            }

            Cache::const_iterator i = cache_p.find(Key(time, spectralWindowId));

            if (i != cache_p.end()) {
                result = i->second;
            }
        }

        return result;
    }

    void flush() {
        cache_p.clear();
    }

private:

    typedef pair<Double, Int> Key; // (time, spectralWindowId)
    typedef map <Key, std::shared_ptr<ChannelSelector>> Cache;

    Cache cache_p;          // the cache itself
    Int frameOfReference_p; // cache's frame of reference
    const uInt maxEntries_p;   // max # of entries to keep in the cache
    Int msId_p;             // cache's MS ID

};

class SpectralWindowChannel
{

public:

	SpectralWindowChannel() // for use of vector only
		: channel_p(-1)
		, frequency_p(-1)
		, width_p(-1)
		{}

	SpectralWindowChannel(Int channel, Double frequency, Double width)
		: channel_p(channel)
		, frequency_p(frequency)
		, width_p(width)
		{}

	Bool
	operator<(const SpectralWindowChannel other) const
		{
			return frequency_p < other.frequency_p;
		}

	Bool
	operator<(Double other) const
		{
			return frequency_p < other;
		}

	Int
	getChannel() const
		{
			return channel_p;
		}

	Double
	getFrequency() const
		{
			return frequency_p;
		}

	Double
	getWidth() const
		{
			return width_p;
		}

private:

	Int channel_p;
	Double frequency_p;
	Double width_p;
};

Bool
operator<(Double frequency, const SpectralWindowChannel & swc)
{
	return frequency < swc.getFrequency();
}

Bool
operator>(Double frequency, const SpectralWindowChannel & swc)
{
	return frequency > swc.getFrequency();
}

class SpectralWindowChannels : public std::vector <SpectralWindowChannel>
{
public:

	SpectralWindowChannels(const Vector<Double> & frequencies,
	                       const Vector<Double> widths)
		: std::vector <SpectralWindowChannel>()
		{
			reserve(frequencies.nelements());

			// Use the passed frequencies and widths to fill out the values.
			// The first indices of frequency are the channel numbers.

			int channel = 0;
			Array<Double>::const_iterator frequency = frequencies.begin();
			Array<Double>::const_iterator width = widths.begin();
			int nChannels = frequencies.size();

			for (; channel < nChannels; ++channel, ++frequency, ++width) {
				push_back(SpectralWindowChannel(channel, * frequency, * width));
			}

			// Remember the positions of the highest and lowest entries and
			// whether the channels are in order of increasing frequency.

			increasingOrder_p =
				begin()->getFrequency() < rbegin()->getFrequency();

			if (increasingOrder_p) {
				lowest_p =  0;
				highest_p = ((int) size()) - 1;
			} else {
				lowest_p =  ((int) size()) - 1;
				highest_p = 0;
			}
		}

	double
	getFrequency(int channel) const
		{

			ThrowIf(channel < 0 || channel >= (int) size(),
			        String::format("Channel %d not in range [0,%d])", channel,
			                       size()-1));

			double result = at(channel).getFrequency();

			return result;
		}

	double
	getWidth(int channel) const
		{

			ThrowIf(channel < 0 || channel >= (int) size(),
			        String::format("Channel %d not in range [0,%d])", channel,
			                       size()-1));

			double result = at(channel).getWidth();

			return result;
		}

	Slice
	getIntersection(double lowerFrequency, double upperFrequency) const
		{
			bool noIntersection =
				(at(lowest_p).getFrequency() - 0.5 * at(lowest_p).getWidth()
				 > upperFrequency) ||
				(at(highest_p).getFrequency() + 0.5 * at(highest_p).getWidth()
				 < lowerFrequency);

			if (noIntersection) {
				return Slice(0,0); // sentinel value
			}

			// Search for the left and right ends of the intersection which need
			// to run.  The above check for intersection should guarantee that
			// one will be found so there's no need to check the iterator
			// limits.

			int increment = increasingOrder_p ? 1 : -1;

			int left, right;

			for (left = lowest_p;
			     at(left).getFrequency() + 0.5 * at(left).getWidth()
				     < lowerFrequency;
			     left += increment) {
			}

			for (right = highest_p;
			     at(right).getFrequency() - 0.5 * at(right).getWidth()
				     > upperFrequency;
			     right -= increment) {
			}

			// Slices can only go from upward so if the channel frequencies are
			// in reverse order, swap them (i.e., (128, 1) --> (1, 128))

			int a, b;
			if (increasingOrder_p) {
				a = at(left).getChannel();
				b = at(right).getChannel();
			} else {
				a = at(right).getChannel();
				b = at(left).getChannel();
			}

			Slice s(a, b - a + 1); // (start, length)

			return s;
		}

private:

	int highest_p;
	bool increasingOrder_p;
	int lowest_p;

};


class SpectralWindowChannelsCache
{

public:

	SpectralWindowChannelsCache()
		: msId_p(-1)
		, nBytes_p(0)
		{}

	~SpectralWindowChannelsCache()
		{
			flush();
		}

	void
	add(const SpectralWindowChannels * entry, Int msId, Int spectralWindowId)
		{
			if (msId != msId_p) {
				flush();
				nBytes_p = 0;
			}

			// If necessary, insert code here to put an upper limit on the size
			// of the cache.

			// Add the entry to the cache

			map_p[spectralWindowId] = entry;
			msId_p = msId;
			nBytes_p += entry->size() * sizeof(SpectralWindowChannel);
		}

	const SpectralWindowChannels *
	find(Int msId, Int spectralWindowId)
		{
			const SpectralWindowChannels * result = 0;

			if (msId == msId_p) {

				Map::const_iterator i = map_p.find(spectralWindowId);

				if (i != map_p.end()) {
					result = i->second;
				}
			}

			return result;
		}

	void flush()
		{
			// Delete all of the contained SpectralWindowChannels objects

			for (Map::iterator i = map_p.begin(); i != map_p.end(); i++) {
				delete (i->second);
			}

			map_p.clear();
		}


private:

	typedef map<Int, const SpectralWindowChannels *> Map;
	// spectralWindowId->SpectralWindowChannels *

	Map map_p;
	Int msId_p;
	Int nBytes_p;

};

Subchunk
Subchunk::noMoreData()
{
	Int maxInt = std::numeric_limits<Int>::max();
	return Subchunk(maxInt, maxInt);
}

String
Subchunk::toString() const
{
	return String::format("(%d,%d)", first, second);
}

template <typename T>
void
VisibilityIteratorImpl2::getColumnRows(const ArrayColumn<T> & column,
                                       Array<T> & array) const
{
    ColumnSlicer columnSlicer =
        channelSelectors_p[0]->getSlicer().getColumnSlicer();

    column.getColumnCells(rowBounds_p.subchunkRows_p,
                          columnSlicer,
                          array,
                          true);
}

template <typename T>
void
VisibilityIteratorImpl2::getColumnRows(const ArrayColumn<T> & column,
                                       Vector<Cube<T>> & cubeVector) const
{
    cubeVector.resize(channelSelectors_p.size());
    for (size_t iVector=0; iVector < channelSelectors_p.size(); iVector++)
    {
        ColumnSlicer columnSlicer =
                channelSelectors_p[iVector]->getSlicer().getColumnSlicer();

        column.getColumnCells(rowBounds_p.subchunkEqChanSelRows_p[iVector],
                columnSlicer,
                cubeVector[iVector],
                true);
    }
}

template <typename T>
void
VisibilityIteratorImpl2::getColumnRowsMatrix(const ArrayColumn<T> & column,
                                             Matrix<T> & array,
                                             Bool correlationSlicing) const
{
    if (correlationSlicing) {

        // Extract the correlations slices and repackage them for use by
        // getColumnCells

        const ChannelSlicer & slicer = channelSelectors_p[0]->getSlicer();
        // has to be at least one
        const ChannelSubslicer subslicer = slicer.getSubslicer(0);

        Vector<Slice> correlationSlices = subslicer.getSlices();

        Vector<Slicer *> dataSlicers(correlationSlices.size(), 0);
        Vector<Slicer *> destinationSlicers(correlationSlices.size(), 0);

        IPosition start(1, 0), length(1, 0), increment(1, 0);
        uInt sliceStart = 0;

        for (uInt i = 0; i < correlationSlices.size(); i++) {

            start(0) = correlationSlices(i).start();
            length(0) = correlationSlices(i).length();
            increment(0) = correlationSlices(i).inc();
            dataSlicers(i) = new Slicer(start, length, increment);

            start(0) = sliceStart;
            increment(0) = 1;
            destinationSlicers(i) = new Slicer(start, length, increment);

            sliceStart += length(0);
        }

        IPosition shape(1, sliceStart);

        ColumnSlicer columnSlicer(shape, dataSlicers, destinationSlicers);

        column.getColumnCells(rowBounds_p.subchunkRows_p, columnSlicer, array,
                              true);
    }
    else{

        column.getColumnCells(rowBounds_p.subchunkRows_p, array, true);
    }
}

template <typename T>
void
VisibilityIteratorImpl2::getColumnRowsMatrix(const ArrayColumn<T> & column,
                                             Vector<Matrix<T>> & matrixVector) const
{
    matrixVector.resize(channelSelectors_p.size());
    for (size_t iVector=0; iVector < channelSelectors_p.size(); iVector++)
    {

        // Extract the correlations slices and repackage them for use by
        // getColumnCells

        const ChannelSlicer & slicer = channelSelectors_p[iVector]->getSlicer();
        // has to be at least one
        const ChannelSubslicer subslicer = slicer.getSubslicer(0);

        Vector<Slice> correlationSlices = subslicer.getSlices();

        Vector<Slicer *> dataSlicers(correlationSlices.size(), 0);
        Vector<Slicer *> destinationSlicers(correlationSlices.size(), 0);

        IPosition start(1, 0), length(1, 0), increment(1, 0);
        uInt sliceStart = 0;

        for (uInt i = 0; i < correlationSlices.size(); i++) {

            start(0) = correlationSlices(i).start();
            length(0) = correlationSlices(i).length();
            increment(0) = correlationSlices(i).inc();
            dataSlicers(i) = new Slicer(start, length, increment);

            start(0) = sliceStart;
            increment(0) = 1;
            destinationSlicers(i) = new Slicer(start, length, increment);

            sliceStart += length(0);
        }

        IPosition shape(1, sliceStart);

        ColumnSlicer columnSlicer(shape, dataSlicers, destinationSlicers);

        column.getColumnCells(rowBounds_p.subchunkEqChanSelRows_p[iVector],
                              columnSlicer,
                              matrixVector[iVector],
                              true);
    }
}

template <typename T>
void
VisibilityIteratorImpl2::getColumnRows(const ScalarColumn<T> & column,
                                       Vector<T> & array) const
{
	column.getColumnCells(rowBounds_p.subchunkRows_p, array, true);
}

template <typename T>
void
VisibilityIteratorImpl2::putColumnRows(
	ArrayColumn<T> & column,
	const Array<T> & array)
{
	ColumnSlicer columnSlicer =
		channelSelectors_p[0]->getSlicer().getColumnSlicer();

	column.putColumnCells(rowBounds_p.subchunkRows_p,
	                      columnSlicer,
	                      array);
}

template <typename T>
void
VisibilityIteratorImpl2::putColumnRows(
	ArrayColumn<T> & column,
	const Matrix<T> & array)
{
	RefRows & rows = rowBounds_p.subchunkRows_p;

	column.putColumnCells(rows, array);
}

template <typename T>
void
VisibilityIteratorImpl2::putColumnRows(
	ScalarColumn<T> & column,
	const Vector <T> & array)
{
	RefRows & rows = rowBounds_p.subchunkRows_p;

	column.putColumnCells(rows, array);
}

VisibilityIteratorImpl2::VisibilityIteratorImpl2(
        const Block<const MeasurementSet *> &mss,
        const SortColumns & sortColumns,
        Double timeInterval,
        Bool writable,
        Bool useMSIter2)
: ViImplementation2(),
  channelSelectors_p(),
  channelSelectorCache_p(new ChannelSelectorCache()),
  columns_p(),
  floatDataFound_p(false),
  frequencySelections_p(nullptr),
  measurementFrame_p(VisBuffer2::FrameNotSpecified),
  modelDataGenerator_p(VisModelDataI::create2()),
  more_p(false),
  msIndex_p(0),
  msIterAtOrigin_p(false),
  msIter_p(),
  nCorrelations_p(-1),
  nRowBlocking_p(0),
  pendingChanges_p(new PendingChanges()),
  reportingFrame_p(VisBuffer2::FrameNotSpecified),
  spectralWindowChannelsCache_p(new SpectralWindowChannelsCache()),
  subtableColumns_p(nullptr),
  tileCacheModMtx_p(new std::mutex()),
  tileCacheIsSet_p(new std::vector<bool>()),
  timeInterval_p(timeInterval),
  vb_p(nullptr),
  weightScaling_p(),
  writable_p(writable),
  ddIdScope_p(UnknownScope),
  timeScope_p(UnknownScope),
  sortColumns_p(sortColumns),
  subchunkSortColumns_p(false)
{
    // Set the default subchunk iteration sorting scheme, i.e.
    // unique timestamps in each subchunk.
    CountedPtr<BaseCompare> singleTimeCompare(new ObjCompare<Double>());
    subchunkSortColumns_p.addSortingColumn(MS::TIME, singleTimeCompare);
    timeScope_p = SubchunkScope;

    initialize(mss, useMSIter2);

    VisBufferOptions options = writable_p ? VbWritable : VbNoOptions;

    vb_p = createAttachedVisBuffer(options);
}

VisibilityIteratorImpl2::VisibilityIteratorImpl2(
    const casacore::Block<const casacore::MeasurementSet *> & mss,
    const SortColumns & chunkSortColumns,
    const SortColumns & subchunkSortColumns,
    bool isWritable)
: ViImplementation2(),
  channelSelectors_p(),
  channelSelectorCache_p(new ChannelSelectorCache()),
  columns_p(),
  floatDataFound_p(false),
  frequencySelections_p(nullptr),
  measurementFrame_p(VisBuffer2::FrameNotSpecified),
  modelDataGenerator_p(VisModelDataI::create2()),
  more_p(false),
  msIndex_p(0),
  msIterAtOrigin_p(false),
  msIter_p(),
  nCorrelations_p(-1),
  nRowBlocking_p(0),
  pendingChanges_p(new PendingChanges()),
  reportingFrame_p(VisBuffer2::FrameNotSpecified),
  spectralWindowChannelsCache_p(new SpectralWindowChannelsCache()),
  subtableColumns_p(nullptr),
  tileCacheModMtx_p(new std::mutex()),
  tileCacheIsSet_p(new std::vector<bool>()),
  timeInterval_p(0),
  vb_p(nullptr),
  weightScaling_p(),
  writable_p(isWritable),
  ddIdScope_p(UnknownScope),
  timeScope_p(UnknownScope),
  sortColumns_p(chunkSortColumns),
  subchunkSortColumns_p(subchunkSortColumns)
{
    initialize(mss);

    VisBufferOptions options = writable_p ? VbWritable : VbNoOptions;

    vb_p = createAttachedVisBuffer(options);
}

VisibilityIteratorImpl2::VisibilityIteratorImpl2(
	const VisibilityIteratorImpl2& vii)
	: ViImplementation2()
	, channelSelectors_p()
	, channelSelectorCache_p(nullptr)
	, frequencySelections_p(nullptr)
	, modelDataGenerator_p(nullptr)
	, pendingChanges_p(nullptr)
	, spectralWindowChannelsCache_p(nullptr)
	, vb_p(nullptr)
{
	*this = vii;
}

VisibilityIteratorImpl2 &
VisibilityIteratorImpl2::operator=(const VisibilityIteratorImpl2& vii)
{
    // clone msIter_p
    msIter_p = vii.msIter_p->clone();

    // copy cache
    cache_p = vii.cache_p;

    tileCacheModMtx_p = vii.tileCacheModMtx_p;
    tileCacheIsSet_p = vii.tileCacheIsSet_p;

    // clone frequencySelections_p
    if (frequencySelections_p)
        delete frequencySelections_p;
    frequencySelections_p = vii.frequencySelections_p->clone();

    // copy channelSelectors_p. 
    channelSelectors_p.clear();
    for (auto chSel : vii.channelSelectors_p)
    {
        channelSelectors_p.push_back(std::shared_ptr<ChannelSelector>
            (new ChannelSelector(chSel->timeStamp, chSel->msId, 
                                 chSel->spectralWindowId, chSel->polarizationId, 
                                 chSel->getSlicer())));
    }

    // get frame of reference for current MS
    const FrequencySelection &selection =
    vii.frequencySelections_p->get(vii.msId());
    Int frameOfReference = selection.getFrameOfReference();

    // initialize channelSelectorCache_p with current channelSelectors_p
    channelSelectorCache_p->flush();
    for (auto chSel : channelSelectors_p)
        channelSelectorCache_p->add(chSel, frameOfReference);

    // copy assign some values
    columns_p = vii.columns_p;
    floatDataFound_p = vii.floatDataFound_p;
    imwgt_p = vii.imwgt_p;
    measurementFrame_p = vii.measurementFrame_p;
    more_p = vii.more_p;
    msIndex_p = vii.msIndex_p;
    msIterAtOrigin_p = vii.msIterAtOrigin_p;
    nCorrelations_p = vii.nCorrelations_p;
    nRowBlocking_p = vii.nRowBlocking_p;
    reportingFrame_p = vii.reportingFrame_p;
    rowBounds_p = vii.rowBounds_p;
    subchunk_p = vii.subchunk_p;
    timeFrameOfReference_p = vii.timeFrameOfReference_p;
    timeInterval_p = vii.timeInterval_p;
    weightScaling_p = vii.weightScaling_p;
    writable_p = vii.writable_p;
    ddIdScope_p = vii.ddIdScope_p;
    timeScope_p = vii.timeScope_p;

    // clone modelDataGenerator_p
    if (modelDataGenerator_p) delete modelDataGenerator_p;
    modelDataGenerator_p = vii.modelDataGenerator_p->clone();

    // initialize MSDerivedValues as done in configureNewChunk()
    msd_p.setAntennas(msIter_p->msColumns().antenna());
    msd_p.setFieldCenter(msIter_p->phaseCenter());

    // clone pendingChanges_p
    pendingChanges_p.reset(vii.pendingChanges_p->clone());

    // initialize subtableColumns_p
    if (subtableColumns_p) delete subtableColumns_p;
    subtableColumns_p = new SubtableColumns(msIter_p);

    // initialize attached VisBuffer...vii.vb_p does *not* get copied
    // TODO: it would be better to use a shared_ptr to the attached VisBuffer,
    // since we don't know if there are any outstanding references, as they can
    // escape from VisibilityIteratorImpl2...for now, just delete it
    if (vb_p) delete vb_p;
    vb_p = createAttachedVisBuffer(writable_p ? VbWritable : VbNoOptions);

    return *this;
}

VisibilityIteratorImpl2::VisibilityIteratorImpl2(VisibilityIteratorImpl2&& vii)
	: ViImplementation2()
	, channelSelectors_p()
	, channelSelectorCache_p(nullptr)
	, frequencySelections_p(nullptr)
	, modelDataGenerator_p(nullptr)
	, pendingChanges_p(nullptr)
	, spectralWindowChannelsCache_p(nullptr)
	, vb_p(nullptr)
	, writable_p(false)
{
	*this = std::move(vii);
}

VisibilityIteratorImpl2 &
VisibilityIteratorImpl2::operator=(VisibilityIteratorImpl2&& vii)
{
    // copy msIter_p
    msIter_p = vii.msIter_p;

    // copy cache
    cache_p = vii.cache_p;

    tileCacheModMtx_p = std::move(vii.tileCacheModMtx_p);
    tileCacheIsSet_p = std::move(vii.tileCacheIsSet_p);

    // move frequencySelections_p
    if (frequencySelections_p) 
        delete frequencySelections_p;
    frequencySelections_p = vii.frequencySelections_p;
    vii.frequencySelections_p = nullptr;

    // move channelSelectors_p. Owned by channelSelectorCache_p 
    // so don't delete initial destination value or source value
    channelSelectors_p = std::move(vii.channelSelectors_p);

    // move channelSelectorCache_p
    if (channelSelectorCache_p) 
        delete channelSelectorCache_p;
    channelSelectorCache_p = vii.channelSelectorCache_p;
    vii.channelSelectorCache_p = nullptr;

    // move backWriters_p
    backWriters_p = std::move(vii.backWriters_p);

    // copy assign some values
    autoTileCacheSizing_p = vii.autoTileCacheSizing_p;
    columns_p = vii.columns_p;
    floatDataFound_p = vii.floatDataFound_p;
    imwgt_p = vii.imwgt_p;
    measurementFrame_p = vii.measurementFrame_p;
    measurementSets_p = vii.measurementSets_p;
    more_p = vii.more_p;
    msIndex_p = vii.msIndex_p;
    msIterAtOrigin_p = vii.msIterAtOrigin_p;
    nCorrelations_p = vii.nCorrelations_p;
    nRowBlocking_p = vii.nRowBlocking_p;
    reportingFrame_p = vii.reportingFrame_p;
    rowBounds_p = vii.rowBounds_p;
    subchunk_p = vii.subchunk_p;
    timeFrameOfReference_p = vii.timeFrameOfReference_p;
    timeInterval_p = vii.timeInterval_p;
    weightScaling_p = vii.weightScaling_p;
    writable_p = vii.writable_p;
    ddIdScope_p = vii.ddIdScope_p;
    timeScope_p = vii.timeScope_p;
    sortColumns_p = vii.sortColumns_p;
    subchunkSortColumns_p = vii.subchunkSortColumns_p;

    // move modelDataGenerator_p
    if (modelDataGenerator_p) 
        delete modelDataGenerator_p;
    modelDataGenerator_p = vii.modelDataGenerator_p;
    vii.modelDataGenerator_p = nullptr;

    // initialize MSDerivedValues as done in configureNewChunk()...moving or
    // copying the value is not well supported
    msd_p.setAntennas(msIter_p->msColumns().antenna());
    msd_p.setFieldCenter(msIter_p->phaseCenter());

    // move pendingChanges_p
    pendingChanges_p = std::move(vii.pendingChanges_p);

    // initialize subtableColumns_p
    if (subtableColumns_p) 
        delete subtableColumns_p;
    subtableColumns_p = vii.subtableColumns_p;
    vii.subtableColumns_p = nullptr;

    // initialize vb_p...can't steal VisBuffer2 instance since it is attached to
    // vii
    if (vb_p) 
        delete vb_p;
    vb_p = createAttachedVisBuffer(writable_p ? VbWritable : VbNoOptions);
    // TODO: again, it would be better were vb_p a shared_ptr
    delete vii.vb_p;
    vii.vb_p = nullptr;

    return *this;
}

void
VisibilityIteratorImpl2::addDataSelection(const MeasurementSet & ms)
{
	const MeasurementSet2 * ms2 = dynamic_cast <const MeasurementSet2 *>(&ms);

	if (ms2 == 0) {

		// A normal MS was used so there is no frequency selection attached to
		// it.  Simply add in an empty selection which will select everything.

		frequencySelections_p->add(FrequencySelectionUsingChannels());

		return;

	}

	// Get the channel and correlation selectin.
	//
	// Channel slices are indexed by spectral window ID and correlation slices
	// by polarization ID

	MSSelection * msSelection =
		const_cast<MSSelection *>(ms2->getMSSelection());
	// *KLUGE* -- MSSelection is somewhat sloppy about making methods const
	// so simply getting the slices requires a non-const object ;-(

	Vector <Vector <Slice> > channelSlices;
	msSelection->getChanSlices(channelSlices, ms2);
	Vector <Vector <Slice> > correlationSlices;
	msSelection->getCorrSlices(correlationSlices, ms2);

	FrequencySelectionUsingChannels selection;

	for (uInt spectralWindow = 0;
	     spectralWindow < channelSlices.nelements();
	     spectralWindow++) {

		// Get the frequency slices for this spectral window

		Vector<Slice> & slices = channelSlices[spectralWindow];

		for (uInt s = 0; s < slices.nelements(); s++) {

			// Add each frequency slice to the selection for this spectral
			// window

			Slice & slice = slices[s];

			selection.add(spectralWindow, slice.start(), slice.length(),
			              slice.inc());
		}
	}

	selection.addCorrelationSlices(correlationSlices);

	frequencySelections_p->add(selection);
}


void
VisibilityIteratorImpl2::initialize(const Block<const MeasurementSet *> &mss,
                                    Bool useMSIter2)
{

    ThrowIf(!sortColumns_p.usingDefaultSortingFunctions(),
            "Sorting definition for chunks doesn't support generic functions yet");

    cache_p.flush();

    msIndex_p = 0;

    frequencySelections_p = new FrequencySelections();

    Int nMs = mss.nelements();
    measurementSets_p.resize(nMs);
    tileCacheIsSet_p->resize(nMs);

    for (Int k = 0; k < nMs; ++k) {
        measurementSets_p[k] = * mss[k];
        addDataSelection(measurementSets_p[k]);
        (*tileCacheIsSet_p)[k] = false;
    }

    if (useMSIter2)

        // This version uses the MSSmartInterval for time comparisons in the
        // Table sort/iteration
        msIter_p = new MSIter2(measurementSets_p,
                sortColumns_p.getColumnIds(),
                timeInterval_p,
                sortColumns_p.shouldAddDefaultColumns(),
                false);
    else
        // The old-fashioned version
        msIter_p = new MSIter(measurementSets_p,
                sortColumns_p.getColumnIds(),
                timeInterval_p,
                sortColumns_p.shouldAddDefaultColumns(),
                false);

    subtableColumns_p = new SubtableColumns(msIter_p);

    // Check whether DDID is unique within each chunk. Otherwise assume
    // that it can change for every row.
    ddIdScope_p = RowScope;
    freqSelScope_p = RowScope;
    if (sortColumns_p.shouldAddDefaultColumns())
        ddIdScope_p = ChunkScope;
    else
    {
        for (auto sortCol : sortColumns_p.getColumnIds())
        {
            if(sortCol == MS::DATA_DESC_ID)
                ddIdScope_p = ChunkScope;
        }
    }

    freqSelScope_p = ddIdScope_p;
    // If frequency/channel selection also depends on time (selection based on
    // frequencies), then the scope can be further limited by timestamp scope
    if (!(frequencySelections_p->getFrameOfReference() == FrequencySelection::ByChannel))
    {
        if(freqSelScope_p == ChunkScope)
            freqSelScope_p = timeScope_p;
    }


    casacore::AipsrcValue<Bool>::find(
            autoTileCacheSizing_p,
            VisibilityIterator2::getAipsRcBase() + ".AutoTileCacheSizing", false);
}

void
VisibilityIteratorImpl2::initialize(const Block<const MeasurementSet *> &mss)
{
    cache_p.flush();

    msIndex_p = 0;

    frequencySelections_p = new FrequencySelections();

    Int nMs = mss.nelements();
    measurementSets_p.resize(nMs);
    tileCacheIsSet_p->resize(nMs);

    for (Int k = 0; k < nMs; ++k) {
        measurementSets_p[k] = * mss[k];
        addDataSelection(measurementSets_p[k]);
        (*tileCacheIsSet_p)[k] = false;
    }

    msIter_p = new MSIter(measurementSets_p,
                sortColumns_p.sortingDefinition());

    subtableColumns_p = new SubtableColumns(msIter_p);

    // Set the scope of each of the metadata to track
    setMetadataScope();

    casacore::AipsrcValue<Bool>::find(
            autoTileCacheSizing_p,
            VisibilityIterator2::getAipsRcBase() + ".AutoTileCacheSizing", false);
}

VisibilityIteratorImpl2::~VisibilityIteratorImpl2()
{
	if (channelSelectorCache_p) delete channelSelectorCache_p;
	if (frequencySelections_p) delete frequencySelections_p;
	if (modelDataGenerator_p) delete modelDataGenerator_p;
	if (spectralWindowChannelsCache_p) delete spectralWindowChannelsCache_p;
	if (subtableColumns_p) delete subtableColumns_p;
	if (vb_p) delete vb_p;
}

std::unique_ptr<VisibilityIteratorImpl2>
VisibilityIteratorImpl2::clone() const
{
	unsigned nms = measurementSets_p.nelements();
	Block<const MeasurementSet *> msps(nms);
	for (unsigned i = 0; i < nms; ++i)
		msps[i] = &measurementSets_p[i];

	std::unique_ptr<VisibilityIteratorImpl2> result(
		new VisibilityIteratorImpl2(
			msps,
			sortColumns_p,
			timeInterval_p,
			writable_p,
			false));
	*result = *this;
	return result;
}

void VisibilityIteratorImpl2::setMetadataScope()
{
    // Check if each chunk will receive a unique DDId.
    // For that it must be a sorting column and comparison function should
    // be of the type ObjCompare<Int>
    bool uniqueDDIdInChunk = false;
    bool uniqueDDIdInSubchunk = false;
    for(auto& sortDef : sortColumns_p.sortingDefinition())
        if(sortDef.first == MS::columnName(MS::DATA_DESC_ID) &&
           dynamic_cast<ObjCompare<Int>*>(sortDef.second.get()))
            uniqueDDIdInChunk = true;

    for(auto& sortDef : subchunkSortColumns_p.sortingDefinition())
        if(sortDef.first == MS::columnName(MS::DATA_DESC_ID) &&
           dynamic_cast<ObjCompare<Int>*>(sortDef.second.get()))
            uniqueDDIdInSubchunk = true;

    if(uniqueDDIdInChunk)
        ddIdScope_p = ChunkScope;
    else if(uniqueDDIdInSubchunk)
        ddIdScope_p = SubchunkScope;
    else
        ddIdScope_p = RowScope;

    // Similar for time
    bool uniqueTimeInChunk = false, uniqueTimeInSubchunk = false;
    for(auto& sortDef : sortColumns_p.sortingDefinition())
        if(sortDef.first == MS::columnName(MS::TIME) &&
           dynamic_cast<ObjCompare<Int>*>(sortDef.second.get()))
            uniqueTimeInChunk = true;

    for(auto& sortDef : subchunkSortColumns_p.sortingDefinition())
        if(sortDef.first == MS::columnName(MS::TIME) &&
           dynamic_cast<ObjCompare<Int>*>(sortDef.second.get()))
            uniqueTimeInSubchunk = true;

    if(uniqueTimeInChunk)
        timeScope_p = ChunkScope;
    else if(uniqueTimeInSubchunk)
        timeScope_p = SubchunkScope;
    else
        timeScope_p = RowScope;

    // Determine the scope of the frequency/channel selections
    // The scope of the frequency selections is at most the same as the DDID
    freqSelScope_p = ddIdScope_p;
    // If frequency/channel selection also depends on time (selection based on
    // frequencies), then the scope can be further limited by row (timestamp) scope
    if (!(frequencySelections_p->getFrameOfReference() == FrequencySelection::ByChannel))
    {
        if(!(freqSelScope_p == RowScope)) // Only if scope is broader than Row can be further restricted
        {
            if(freqSelScope_p == SubchunkScope && timeScope_p == RowScope)
                freqSelScope_p = RowScope;
            if(freqSelScope_p == ChunkScope)
                freqSelScope_p = timeScope_p;
        }
    }

}

VisibilityIteratorImpl2::Cache::Cache()
	:
	azel0Time_p(-1),
	azelTime_p(-1),
	feedpaTime_p(-1),
	hourang_p(0),
	hourangTime_p(-1),
	msHasFlagCategory_p(false),
	msHasWeightSpectrum_p(false),
	msHasSigmaSpectrum_p(false),
	parang0_p(0),
	parang0Time_p(-1),
	parangTime_p(-1)
{}

void
VisibilityIteratorImpl2::Cache::flush()
{
	azel0Time_p = -1;
	azelTime_p = -1;
	feedpaTime_p = -1;
	hourangTime_p = -1;
	parangTime_p = -1;
	parang0Time_p = -1;
}

const Cube<RigidVector<Double, 2> > &
VisibilityIteratorImpl2::getBeamOffsets() const
{
	return msIter_p->getBeamOffsets();
}

std::pair <bool, casacore::MDirection>
VisibilityIteratorImpl2::getPointingAngle(int antenna, double time) const
{
	if (!pointingDirectionCache_p) {
		pointingSource_p.reset(
			new PointingColumns(subtableColumns_p->pointing()));
		pointingDirectionCache_p.reset(
			new PointingDirectionCache(nAntennas(), *pointingSource_p.get()));
	}

	return pointingDirectionCache_p->getPointingDirection(
		antenna, time, phaseCenter());
}


Vector<Double>
VisibilityIteratorImpl2::getFrequencies(Double time, Int frameOfReference,
                                        Int spectralWindowId, Int msId) const
{
	const SpectralWindowChannels & spectralWindowChannels =
		getSpectralWindowChannels(msId, spectralWindowId);

	// Get the channel numbers selected for this time (the spectral window and
	// MS index are assumed to be the same as those currently pointed to by the
	// MSIter).

	Vector<Int> channels =
		getChannels(time, frameOfReference, spectralWindowId, msId);

	Vector<Double> frequencies(channels.nelements());
//    MFrequency::Types observatoryFrequencyType =
//         getObservatoryFrequencyType();


	MFrequency::Types measurementFrequencyType =
		static_cast<MFrequency::Types>(getMeasurementFrame(spectralWindowId));

	if (frameOfReference == measurementFrequencyType) {

		// Since the observed frequency is being asked for, no conversion is
		// necessary.  Just copy each selected channel's observed frequency over
		// to the result.

		for (Int i = 0; i < (int) channels.nelements(); i++) {

			Int channelNumber = channels[i];

			frequencies[i] = spectralWindowChannels.getFrequency(channelNumber);
		}

		return frequencies;
	}

	// Get the converter from the observed to the requested frame.

	MFrequency::Convert fromObserved =
		makeFrequencyConverter(time, spectralWindowId, frameOfReference,
		                       false, Unit(String("Hz")));

	// For each selected channel, get its observed frequency and convert it into
	// the frequency in the requested frame.  Put the result into the
	// corresponding slot of "frequencies".

	for (Int i = 0; i < (int) channels.nelements(); i++) {

		Int channelNumber = channels[i];

		Double fO = spectralWindowChannels.getFrequency(channelNumber);
		// Observed frequency

		Double fF = fromObserved(fO).getValue();
		// Frame frequency

		frequencies[i] = fF;
	}

	return frequencies;
}

Vector<Double>
VisibilityIteratorImpl2::getChanWidths(Double time, Int frameOfReference,
				       Int spectralWindowId, Int msId) const
{
  // This gets the native frame channel widths (no frame conversions performed, for now)
  
	const SpectralWindowChannels & spectralWindowChannels =
		getSpectralWindowChannels(msId, spectralWindowId);

	// Get the channel numbers selected for this time (the spectral window and
	// MS index are assumed to be the same as those currently pointed to by the
	// MSIter).

	Vector<Int> channels =
		getChannels(time, frameOfReference, spectralWindowId, msId);

	Vector<Double> widths(channels.nelements());

	for (Int i = 0; i < (int) channels.nelements(); i++) {

	  Int channelNumber = channels[i];

	  widths[i] = spectralWindowChannels.getWidth(channelNumber);
	
	}

	return widths;
}

Vector<Int>
VisibilityIteratorImpl2::getChannels(Double time, Int /*frameOfReference*/,
                                     Int spectralWindowId, Int msId) const
{
	std::shared_ptr<ChannelSelector> channelSelector =
		determineChannelSelection(time, spectralWindowId, -1, msId);

	return channelSelector->getChannels();
}

Vector<Int>
VisibilityIteratorImpl2::getCorrelations() const
{
    assert(!channelSelectors_p.empty());

    return channelSelectors_p[0]->getCorrelations();
}

Vector<Stokes::StokesTypes>
VisibilityIteratorImpl2::getCorrelationTypesDefined() const
{
    assert(!channelSelectors_p.empty());

    Vector<Int> typesAsInt;
    Int polarizationId = channelSelectors_p[0]->polarizationId;
    subtableColumns_p->polarization().corrType().get(
    polarizationId, typesAsInt, true);
    Vector<Stokes::StokesTypes> correlationTypesDefined(typesAsInt.size());

    for (uInt i = 0; i < typesAsInt.size(); i++) {
        correlationTypesDefined(i) =
            static_cast<Stokes::StokesTypes>(typesAsInt(i));
    }

    return correlationTypesDefined;
}

Vector<Stokes::StokesTypes>
VisibilityIteratorImpl2::getCorrelationTypesSelected() const
{
    assert(!channelSelectors_p.empty());

    Vector<Int> correlationIndices = getCorrelations();
    Vector<Stokes::StokesTypes> correlationTypesDefined =
    getCorrelationTypesDefined();
    Vector<Stokes::StokesTypes> correlationTypesSelected(
    correlationIndices.size());

    for (uInt i = 0; i < correlationIndices.size(); i++) {
        correlationTypesSelected(i) =
            correlationTypesDefined(correlationIndices(i));
    }

    return correlationTypesSelected;
}

Double
VisibilityIteratorImpl2::getInterval() const
{
	return timeInterval_p;
}

int
VisibilityIteratorImpl2::getMeasurementFrame(int spectralWindowId) const
{
	int frame = subtableColumns_p->spectralWindow().measFreqRef()(
		spectralWindowId);

	return frame;
}


Bool
VisibilityIteratorImpl2::isNewArrayId() const
{
	return msIter_p->newArray();
}

Bool
VisibilityIteratorImpl2::isNewFieldId() const
{
	return msIter_p->newField();
}

Bool
VisibilityIteratorImpl2::isNewMs() const
{
	return msIter_p->newMS();
}

Bool
VisibilityIteratorImpl2::isNewSpectralWindow() const
{
	return msIter_p->newSpectralWindow();
}

Bool
VisibilityIteratorImpl2::allBeamOffsetsZero() const
{
	return msIter_p->allBeamOffsetsZero();
}

rownr_t
VisibilityIteratorImpl2::nRows() const
{
    return rowBounds_p.subchunkNRows_p;
}

rownr_t
VisibilityIteratorImpl2::nShapes() const
{
    return 1;
}

const casacore::Vector<casacore::rownr_t>&
VisibilityIteratorImpl2::nRowsPerShape () const
{
    return nRowsPerShape_p;
}

const casacore::Vector<casacore::Int>&
VisibilityIteratorImpl2::nChannelsPerShape () const
{
    return nChannPerShape_p;
}

const casacore::Vector<casacore::Int>&
VisibilityIteratorImpl2::nCorrelationsPerShape () const
{
    return nCorrsPerShape_p;
}

rownr_t VisibilityIteratorImpl2::nRowsInChunk() const
{
    return msIter_p->table().nrow();
}

Int VisibilityIteratorImpl2::nTimes() const {
    static const auto timeName = MeasurementSet::columnName(MSMainEnums::TIME);
    auto times = ScalarColumn<Double>(msIter_p->table(), timeName).getColumn();
    std::set<Double> uniqueTimes(times.cbegin(), times.cend());
    return uniqueTimes.size();
}

Bool
VisibilityIteratorImpl2::more() const
{
	return more_p;
}

Bool
VisibilityIteratorImpl2::moreChunks() const
{
	return msIter_p->more();
}

void
VisibilityIteratorImpl2::result(casacore::Record& res) const
{
    if (moreChunks()) {
        throw AipsError("TransformingVi2::result(Record&) can only be called at the end of "
                        "the iteration. It has been called while there are still "
                        "moreChunks(). Please check and/or revisit this condition.");
    }
    // For now nothing to add to result record from here
}

const MSColumns *
VisibilityIteratorImpl2::msColumnsKluge() const
{
	return & msIter_p->msColumns();
}

Int
VisibilityIteratorImpl2::msId() const
{
	return msIter_p->msId();
}

const MeasurementSet &
VisibilityIteratorImpl2::ms() const
{
	return msIter_p->ms();
}

String
VisibilityIteratorImpl2::msName() const
{
	// Name of current MS
	return ms().tableName();
}

void
VisibilityIteratorImpl2::fieldIds(Vector<Int> & fieldIds) const
{
	getColumnRows(columns_p.field_p, fieldIds);
}

// Return the current ArrayId

void
VisibilityIteratorImpl2::arrayIds(Vector<Int> & arrayIds) const
{
	getColumnRows(columns_p.array_p, arrayIds);
}

// Return the current Field Name
String
VisibilityIteratorImpl2::fieldName() const
{
	return msIter_p->fieldName();
}

// Return the current Source Name
String
VisibilityIteratorImpl2::sourceName() const
{
	return msIter_p->sourceName();
}

const Vector<String> &
VisibilityIteratorImpl2::antennaMounts() const
{
	return msIter_p->antennaMounts();
}

void
VisibilityIteratorImpl2::setInterval(Double timeInterval)
{
	pendingChanges_p->setInterval(timeInterval);
}

void
VisibilityIteratorImpl2::setRowBlocking(rownr_t nRow)
{
	pendingChanges_p->setNRowBlocking(nRow);
}

rownr_t
VisibilityIteratorImpl2::getRowBlocking() const
{
	return nRowBlocking_p;
}

const MDirection &
VisibilityIteratorImpl2::phaseCenter() const
{
	return msIter_p->phaseCenter();
}

Int
VisibilityIteratorImpl2::polFrame() const
{
	return msIter_p->polFrame();
}

void
VisibilityIteratorImpl2::spectralWindows(Vector<Int> & spws) const
{
    // Get's the list of spectral windows for each row in the VB window

    Vector<Int> ddis;
    dataDescriptionIds(ddis);
    spws.resize(ddis.size());

    for (uInt idx = 0; idx < ddis.size(); idx++) {
        spws(idx) = subtableColumns_p->dataDescription().spectralWindowId()(
            ddis(idx));
    }

    return;
}

void
VisibilityIteratorImpl2::polarizationIds(Vector<Int> & polIds) const
{
    // Get's the list of polarization Ids for each row in the VB window

    Vector<Int> ddis;
    dataDescriptionIds(ddis);
    polIds.resize(ddis.size());

    for (uInt idx = 0; idx < ddis.size(); idx++) {
        polIds(idx) = subtableColumns_p->dataDescription().polarizationId()(
            ddis(idx));
    }

    return;
}

// Return current Polarization Id
Int
VisibilityIteratorImpl2::polarizationId() const
{
	return msIter_p->polarizationId();
}

// Return current DataDescription Id
Int
VisibilityIteratorImpl2::dataDescriptionId() const
{
	return msIter_p->dataDescriptionId();
}

void
VisibilityIteratorImpl2::dataDescriptionIds(Vector<Int> & ddis) const
{
	getColumnRows(columns_p.dataDescription_p, ddis);
}

Bool
VisibilityIteratorImpl2::newFieldId() const
{
	return (rowBounds_p.subchunkBegin_p == 0 && msIter_p->newField());
}

Bool
VisibilityIteratorImpl2::newArrayId() const
{
	return (rowBounds_p.subchunkBegin_p == 0 && msIter_p->newArray());
}

Bool
VisibilityIteratorImpl2::newSpectralWindow() const
{
	return (rowBounds_p.subchunkBegin_p == 0 && msIter_p->newSpectralWindow());
}

Bool
VisibilityIteratorImpl2::existsColumn(VisBufferComponent2 id) const
{
  Bool result;
  switch (id) {

  case VisBufferComponent2::VisibilityCorrected:
  case VisBufferComponent2::VisibilityCubeCorrected:

    result =
        !columns_p.corrVis_p.isNull() && columns_p.corrVis_p.isDefined(0);
    break;

  case VisBufferComponent2::VisibilityModel:
  case VisBufferComponent2::VisibilityCubeModel:

    result = 
        (!columns_p.modelVis_p.isNull() && columns_p.modelVis_p.isDefined(0)) ||
        modelDataGenerator_p != nullptr;
    break;

  case VisBufferComponent2::VisibilityObserved:
  case VisBufferComponent2::VisibilityCubeObserved:

    result = (!columns_p.vis_p.isNull() && columns_p.vis_p.isDefined(0)) ||
    (columns_p.floatVis_p.isNull() && columns_p.floatVis_p.isNull());

    break;

  case VisBufferComponent2::VisibilityCubeFloat:

    result =
        !columns_p.floatVis_p.isNull() && columns_p.floatVis_p.isDefined(0);

    break;

  case VisBufferComponent2::WeightSpectrum:

    result =
        !columns_p.weightSpectrum_p.isNull()
        && columns_p.weightSpectrum_p.isDefined(0);
    break;

  case VisBufferComponent2::SigmaSpectrum:

    result =
        !columns_p.sigmaSpectrum_p.isNull()
        && columns_p.sigmaSpectrum_p.isDefined(0);
    break;

  default:
    result = true; // required columns
    break;
  }

  return result;
}

const SubtableColumns &
VisibilityIteratorImpl2::subtableColumns() const
{
	return *subtableColumns_p;
}

void
VisibilityIteratorImpl2::allSpectralWindowsSelected(
	Vector<Int> & selectedWindows,
	Vector<Int> & nChannels) const
{

	Vector<Int> firstChannels; // ignored
	Vector<Int> channelIncrement; // ignored

	// info generation should not use time as input
	std::tie(selectedWindows, nChannels, firstChannels, channelIncrement) =
		getChannelInformation();
}

void
VisibilityIteratorImpl2::useImagingWeight(const VisImagingWeight & imWgt)
{
	imwgt_p = imWgt;
}

void
VisibilityIteratorImpl2::origin()
{
    ThrowIf(rowBounds_p.chunkNRows_p < 0,
        "Call to origin without first initializing chunk");

    throwIfPendingChanges();

    rowBounds_p.subchunkBegin_p = 0; // begin at the beginning
    more_p = true;
    subchunk_p.resetSubChunk();


    if( ! (nRowBlocking_p > 0) )
    {
        // Create a MeasurementSet which points
        // to the current iteration with msIter
        msSubchunk_p.reset(new casacore::MeasurementSet(msIter_p->table(),
                                                     &(msIter_p->ms())));

        // Create a MSIter for the subchunk loop which iterates the
        // the MS created before.
        msIterSubchunk_p.reset(new casacore::MSIter(*msSubchunk_p,
                            subchunkSortColumns_p.sortingDefinition()));
        msIterSubchunk_p->origin();
    }

    configureNewSubchunk();
}

void
VisibilityIteratorImpl2::originChunks()
{
	originChunks(false);
}

void
VisibilityIteratorImpl2::applyPendingChanges()
{
    if (!pendingChanges_p->empty()) {

        Bool exists;

        // Handle a pending frequency selection if it exists.

        FrequencySelections * newSelection;
        std::tie(exists, newSelection) =
        pendingChanges_p->popFrequencySelections();

        if (exists) {

            delete frequencySelections_p; // out with the old

            frequencySelections_p = newSelection; // in with the new
            setMetadataScope();
        }

        // Handle any pending interval change

        Double newInterval;
        std::tie(exists, newInterval) = pendingChanges_p->popInterval();

        if (exists) {

            msIter_p->setInterval(newInterval);
            timeInterval_p = newInterval;
        }

        // Handle any row-blocking change

        Int newBlocking;
        std::tie(exists, newBlocking) = pendingChanges_p->popNRowBlocking();

        if (exists) {

            nRowBlocking_p = newBlocking;

        }

        // force rewind since window selections may have changed
        msIterAtOrigin_p = false;
    }
}

void
VisibilityIteratorImpl2::originChunks(Bool forceRewind)
{
	subchunk_p.resetToOrigin();

	applyPendingChanges();

	if (!msIterAtOrigin_p || forceRewind) {

		msIter_p->origin();
		msIterAtOrigin_p = true;

		positionMsIterToASelectedSpectralWindow();

		msIndex_p = msId();
	}

	configureNewChunk();
}

void
VisibilityIteratorImpl2::positionMsIterToASelectedSpectralWindow()
{
	while (msIter_p->more() &&
	       !frequencySelections_p->isSpectralWindowSelected(
		       msIter_p->msId(), msIter_p->spectralWindowId())) {

		(* msIter_p)++;
	}
}

void
VisibilityIteratorImpl2::next()
{
    ThrowIf(!more_p, "Attempt to advance subchunk past end of chunk");

    throwIfPendingChanges(); // throw if unapplied changes exist

    measurementFrame_p = VisBuffer2::FrameNotSpecified; // flush cached value

    if(nRowBlocking_p > 0)
    {
        // Attempt to advance to the next subchunk
        rowBounds_p.subchunkBegin_p = rowBounds_p.subchunkEnd_p + 1;
        more_p = rowBounds_p.subchunkBegin_p < rowBounds_p.chunkNRows_p;
    }
    else
    {
        // Increment the subchunk MSIter
        (*msIterSubchunk_p)++;
        more_p = msIterSubchunk_p->more();
    }

    if (more_p) {

        subchunk_p.incrementSubChunk();

        configureNewSubchunk();
    }
    else
    {
        // Leave the columns referencing a valid table. This ensures that some
        // TVIs can still get some valid metadata when they are not at the end
        // of iteration (even if the underlying VI2 is already at the end).
        attachColumns(msIter_p->table());
    }
}

Subchunk
VisibilityIteratorImpl2::getSubchunkId() const
{
	return subchunk_p;
}

const SortColumns &
VisibilityIteratorImpl2::getSortColumns() const
{
	return sortColumns_p;
}

void
VisibilityIteratorImpl2::throwIfPendingChanges()
{
	// Throw an exception if there are any pending changes to the operation of
	// the visibility iterator.  The user must call originChunks to cause the
	// changes to take effect; it is an error to try to advance the VI if there
	// are unapplied changes pending.

	ThrowIf(!pendingChanges_p->empty(),
	        "Call to originChunks required after applying frequencySelection");

}

Bool
VisibilityIteratorImpl2::isInASelectedSpectralWindow() const
{
	return frequencySelections_p->isSpectralWindowSelected(
		msIter_p->msId(), msIter_p->spectralWindowId());
}

Bool
VisibilityIteratorImpl2::isWritable() const
{
	return writable_p;
}

void
VisibilityIteratorImpl2::nextChunk()
{
	ThrowIf(!msIter_p->more(),
	        "Attempt to advance past end of data using nextChunk");

	throwIfPendingChanges(); // error if unapplied changes exist

	// Advance the MS Iterator until either there's no more data or it points to
	// a selected spectral window.

	(* msIter_p)++;

	positionMsIterToASelectedSpectralWindow();

	msIterAtOrigin_p = false;

	// If the MS Iterator was successfully advanced then
	// set up for a new chunk

	if (msIter_p->more()) {

		subchunk_p.incrementChunk();

		configureNewChunk();

		vb_p->invalidate(); // flush the cache ??????????
	}

	more_p = msIter_p->more();
}

void
VisibilityIteratorImpl2::configureNewSubchunk()
{

    // Only for rowBlocking: work out how many rows to return for the moment
    // we return all rows with
    // the same value for time unless row blocking is set, in which case we
    // return more rows at once.

    rowBounds_p.subchunkEqChanSelRows_p.clear();
    if (nRowBlocking_p > 0) {
        rowBounds_p.subchunkEnd_p =
                rowBounds_p.subchunkBegin_p + nRowBlocking_p;

        if (rowBounds_p.subchunkEnd_p >= rowBounds_p.chunkNRows_p) {
            rowBounds_p.subchunkEnd_p = rowBounds_p.chunkNRows_p - 1;
        }
        // This is needed because the call to spectralWindows() needs to
        // have rowBounds_p.subchunkRows_p properly initialized
        rowBounds_p.subchunkRows_p =
                RefRows(rowBounds_p.subchunkBegin_p, rowBounds_p.subchunkEnd_p);


        // Scan the subchunk to see if the same channels are selected in each
        // row.  End the subchunk when a row using different channels is
        // encountered.
        Double previousRowTime =
                rowBounds_p.times_p(rowBounds_p.subchunkBegin_p);
        channelSelectors_p.clear();
        channelSelectorsNrows_p.clear();

        channelSelectors_p.push_back(determineChannelSelection(previousRowTime,
            -1, polarizationId(), msId()));

        for (Int i = rowBounds_p.subchunkBegin_p + 1;
                i <= rowBounds_p.subchunkEnd_p;
                i++) {

            Double rowTime = rowBounds_p.times_p(i);

            if (rowTime == previousRowTime) {
                continue; // Same time means same rows.
            }

            // Compute the channel selector for this row so it can be compared
            // with the previous row's channel selector.

            std::shared_ptr<ChannelSelector> newSelector =
                    determineChannelSelection(rowTime, msIter_p->spectralWindowId(),
                                              msIter_p->polarizationId(), msId());

            if (newSelector.get() != channelSelectors_p[0].get()) {

                // This row uses different channels than the previous row and so
                // it cannot be included in this subchunk.  Make the previous
                // row the end of the subchunk.

                rowBounds_p.subchunkEnd_p = i - 1;
            }
        }
        // Set the number of rows that use this channelSelector
        channelSelectorsNrows_p.push_back(rowBounds_p.subchunkEnd_p - rowBounds_p.subchunkBegin_p + 1);

        rowBounds_p.subchunkNRows_p =
                rowBounds_p.subchunkEnd_p - rowBounds_p.subchunkBegin_p + 1;
        // Reset this in case rowBounds_p.subchunkEnd_p has changed
        rowBounds_p.subchunkRows_p =
                RefRows(rowBounds_p.subchunkBegin_p, rowBounds_p.subchunkEnd_p);
        rowBounds_p.subchunkEqChanSelRows_p.push_back(rowBounds_p.subchunkRows_p);
    }
    else {
        // All the information is in the subchunk MSIter
        rowBounds_p.subchunkNRows_p = msIterSubchunk_p->table().nrow();

        attachColumns(attachTable());

        // Fetch all of the times in this chunk and get the min/max
        // of those times

        rowBounds_p.times_p.resize(rowBounds_p.subchunkNRows_p);
        columns_p.time_p.getColumn(rowBounds_p.times_p);

        // The subchunk rows refer to the subchunk iterator
        // and therefore are consecutive.
        rowBounds_p.subchunkBegin_p = 0;
        rowBounds_p.subchunkEnd_p = msIterSubchunk_p->table().nrow() - 1;
        rowBounds_p.subchunkNRows_p =
                rowBounds_p.subchunkEnd_p - rowBounds_p.subchunkBegin_p + 1;
        rowBounds_p.subchunkRows_p =
                RefRows(rowBounds_p.subchunkBegin_p, rowBounds_p.subchunkEnd_p);


        // Under some circumstances, there is only one channel selector per chunk:
        // 1. The selection doesn't depend on time (it is based only on channel number)
        //    and DDId (and consequently SPW, polID) is the same for the whole subchunk.
        // 2. The selection might depend on time but DDid *and* time are the same for
        //    the whole subchunk.
        if(freqSelScope_p == SubchunkScope)
        {
            channelSelectors_p.clear();
            channelSelectorsNrows_p.clear();
            double timeStamp = -1;
            if(frequencySelections_p->getFrameOfReference() != FrequencySelection::ByChannel)
                timeStamp = columns_p.time_p.asdouble(0);
            channelSelectors_p.push_back(
                    determineChannelSelection(timeStamp,
                                              msIterSubchunk_p->spectralWindowId(),
                                              msIterSubchunk_p->polarizationId(), msId()));
            channelSelectorsNrows_p.push_back(rowBounds_p.subchunkEnd_p - rowBounds_p.subchunkBegin_p + 1);
            rowBounds_p.subchunkEqChanSelRows_p.push_back(rowBounds_p.subchunkRows_p);
        }
        // In all other cases the channel selector needs to be computed
        // for each row. Each channel selector will then apply to a set
        // of n consecutive rows (as defined in channelSelectorsNrows_p).
        else if(freqSelScope_p == RowScope)
        {
            channelSelectors_p.clear();
            channelSelectorsNrows_p.clear();
            Vector<Int> spws, polIds;
            spectralWindows(spws);
            polarizationIds(polIds);
            double timeStamp = -1;
            for(Int irow = 0 ; irow < rowBounds_p.subchunkNRows_p; ++irow)
            {
                if(frequencySelections_p->getFrameOfReference() != FrequencySelection::ByChannel)
                    timeStamp = columns_p.time_p.asdouble(0);
                auto newChannelSelector = determineChannelSelection(
                        timeStamp,
                        spws[irow], polIds[irow], msId());
                if(irow == 0 || newChannelSelector != channelSelectors_p.back())
                {
                    channelSelectors_p.push_back(newChannelSelector);
                    channelSelectorsNrows_p.push_back(1);
                }
                else
                    channelSelectorsNrows_p.back()++;
            }
            size_t beginRefRowIdx = rowBounds_p.subchunkBegin_p;
            for (auto nrows : channelSelectorsNrows_p)
            {
                rowBounds_p.subchunkEqChanSelRows_p.push_back(RefRows(beginRefRowIdx, beginRefRowIdx + nrows - 1));
                beginRefRowIdx += nrows;
            }
        }
        // The remaining case is that scope of frequency selections is chunk.
        // In this case the channelSelector is constant for a chunk 
        // and has already been computed in configureNewChunk. 
        // The number of rows still needs to be updated 
        // to account for the the number of rows in this subchunk
        else
        {
            channelSelectorsNrows_p.clear();
            channelSelectorsNrows_p.push_back(rowBounds_p.subchunkEnd_p - rowBounds_p.subchunkBegin_p + 1);
            rowBounds_p.subchunkEqChanSelRows_p.push_back(rowBounds_p.subchunkRows_p);
        }
    }

    // Set flags for current subchunk

    Vector<Int> correlations = channelSelectors_p[0]->getCorrelations();
    nCorrelations_p = correlations.nelements();

    Vector<Stokes::StokesTypes> correlationsDefined =
            getCorrelationTypesDefined();
    Vector<Stokes::StokesTypes> correlationsSelected =
            getCorrelationTypesSelected();

    String msName = ms().tableName();

    auto nShapes = channelSelectors_p.size();
    nRowsPerShape_p = Vector<uInt64>(channelSelectorsNrows_p);
    nChannPerShape_p.resize(nShapes);
    nCorrsPerShape_p.resize(nShapes);

    rownr_t ishape = 0;
    for (auto channelSelector : channelSelectors_p)
    {
        nChannPerShape_p[ishape] = channelSelector->getNFrequencies();
        nCorrsPerShape_p[ishape] = channelSelector->getCorrelations().nelements();
        ++ishape;
    }

    vb_p->configureNewSubchunk(
            msId(), msName, isNewMs(), isNewArrayId(), isNewFieldId(),
            isNewSpectralWindow(), subchunk_p, 
            nRowsPerShape_p,
            nChannPerShape_p,
            nCorrsPerShape_p,
            correlations, correlationsDefined, correlationsSelected,
            weightScaling_p);
}

std::shared_ptr<ChannelSelector>
VisibilityIteratorImpl2::determineChannelSelection(
	Double time,
	Int spectralWindowId,
	Int polarizationId,
	Int msId) const
{
// --> The channels selected will need to be identical for all members of the
//     subchunk; otherwise the underlying fetch method won't work since it takes
//     a single Vector<Vector <Slice> > to specify the channels.

	assert(frequencySelections_p != 0);

	if (spectralWindowId == -1) {
        Vector<Int> spws;
        this->spectralWindows(spws);
		spectralWindowId = spws[0];
	}

	if (msId < 0) {
		msId = this->msId();
	}

	const FrequencySelection & selection = frequencySelections_p->get(msId);
	Int frameOfReference = selection.getFrameOfReference();

	// See if the appropriate channel selector is in the cache.

	std::shared_ptr<ChannelSelector> cachedSelector =
		channelSelectorCache_p->find(time, msId, frameOfReference,
		                             spectralWindowId);

	if (cachedSelector != nullptr) {
		return cachedSelector;
	}

	// Create the appropriate frequency selection for the current
	// MS and Spectral Window

	selection.filterByWindow(spectralWindowId);

	// Find(or create) the appropriate channel selection.

	std::shared_ptr<ChannelSelector> newSelector;

	if (polarizationId < 0) {
		polarizationId = getPolarizationId(spectralWindowId, msId);
	}

	if (selection.getFrameOfReference() == FrequencySelection::ByChannel) {
		newSelector = makeChannelSelectorC(selection, time, msId,
		                                   spectralWindowId, polarizationId);
	}
	else{
		newSelector = makeChannelSelectorF(selection, time, msId,
		                                   spectralWindowId, polarizationId);
	}

	// Cache it for possible future use.  The cache may hold multiple equivalent
	// selectors, each having a different timestamp.  Since selectors are small
	// and there are not expected to be many equivalent selectors in the cache
	// at a time, this shouldn't be a problem (the special case of selection by
	// channel number is already handled).

	channelSelectorCache_p->add(newSelector, frameOfReference);

	return newSelector;
}

Int
VisibilityIteratorImpl2::getPolarizationId(Int spectralWindowId, Int msId) const
{
    ThrowIf(msId != this->msId(),
        String::format("Requested MS number is %d but current is %d", msId,
                       this->msId()));

    auto & ddCols = subtableColumns_p->dataDescription();
    auto nSpw = subtableColumns_p->spectralWindow().nrow();

    // This will break if the same spectral window is referenced by two
    // different data_descrption IDs.  Ideally, this whole thing should be
    // reworked to used DDIDs with spectral window ID only used internally.
    Int polID = -1;
    for (uInt idd = 0; idd < ddCols.spectralWindowId().nrow(); idd++) {
        if(ddCols.spectralWindowId()(idd) == spectralWindowId)
            polID = ddCols.polarizationId()(idd);
    }

    // If the SPW is not found in the DD it will return -1, rather than failing.
    // This can happen for the so-called phantom SPWs. See CAS-11734
    if(spectralWindowId < (Int)nSpw)
        return polID;

    // spectralWindowId is not present in subtables 
    ThrowIf(true, String::format(
            "Could not find entry for spectral window id"
            "%d in spectral_window in MS #%d", spectralWindowId, msId));

    return -1; // Can't get here so make the compiler happy
}


std::shared_ptr<vi::ChannelSelector>
VisibilityIteratorImpl2::makeChannelSelectorC(
    const FrequencySelection & selectionIn,
    Double time,
    Int msId,
    Int spectralWindowId,
    Int polarizationId) const
{
    const FrequencySelectionUsingChannels & selection =
    dynamic_cast<const FrequencySelectionUsingChannels &>(selectionIn);

    if (selection.refinementNeeded())
    {
        auto spwcFetcher =
            [this, msId]
            (int spwId, double lowerFrequency, double upperFrequency)
            {
                const SpectralWindowChannels & spwChannels =
                getSpectralWindowChannels(msId, spwId);
                return spwChannels.getIntersection(
                    lowerFrequency, upperFrequency);
            };
            selection.applyRefinement(spwcFetcher);
    }

    vector<Slice> frequencySlices;

    // Iterate over all of the frequency selections for the specified spectral
    // window and collect them into a vector of Slice objects.
    for (FrequencySelectionUsingChannels::const_iterator i = selection.begin();
         i != selection.end(); i++)
    {

        frequencySlices.push_back(i->getSlice());
    }

    if (frequencySlices.empty())
    {
        // And empty selection implies all channels
        Int nChannels =
            subtableColumns_p->spectralWindow().numChan()(spectralWindowId);
        frequencySlices.push_back(Slice(0, nChannels, 1));
    }

    ChannelSlicer slices(2);

    // Install the polarization selections
    if(polarizationId != -1)
    {
        Vector<Slice> correlationSlices =
            selection.getCorrelationSlices(polarizationId);

        if (correlationSlices.empty())
        {
            Int nCorrelations =
                subtableColumns_p->polarization().numCorr().get(polarizationId);
            correlationSlices = Vector<Slice>(1, Slice(0, nCorrelations));
        }

        slices.setSubslicer(0, ChannelSubslicer(correlationSlices));

    }

    // Create and install the frequency selections
    ChannelSubslicer frequencyAxis(frequencySlices.size());
    for (Int i = 0; i <(int) frequencySlices.size(); i++)
    {
        frequencyAxis.setSlice(i, frequencySlices[i]);
    }

    slices.setSubslicer(1, frequencyAxis);

    // Package up the result and return it.
    std::shared_ptr<ChannelSelector> result(new ChannelSelector
        (time, msId, spectralWindowId, polarizationId, slices));

    return result;
}

std::shared_ptr<ChannelSelector>
VisibilityIteratorImpl2::makeChannelSelectorF(
	const FrequencySelection & selectionIn,
	Double time, Int msId, Int spectralWindowId,
	Int polarizationId) const
{
	// Make a ChannelSelector from a frame-based frequency selection.

	const FrequencySelectionUsingFrame & selection =
		dynamic_cast<const FrequencySelectionUsingFrame &>(selectionIn);

	vector<Slice> frequencySlices;

	selection.filterByWindow(spectralWindowId);

	// Set up frequency converter so that we can convert to the measured
	// frequency

	MFrequency::Convert convertToObservedFrame =
		makeFrequencyConverter(
			time, spectralWindowId, selection.getFrameOfReference(),
			true, Unit("Hz"));

	// Convert each frequency selection into a Slice(interval) of channels.

	const SpectralWindowChannels & spectralWindowChannels =
		getSpectralWindowChannels(msId, spectralWindowId);

	for (FrequencySelectionUsingFrame::const_iterator i = selection.begin();
	     i != selection.end();
	     i++) {

		Double f = i->getBeginFrequency();
		Double lowerFrequency = convertToObservedFrame(f).getValue();

		f = i->getEndFrequency();
		Double upperFrequency = convertToObservedFrame(f).getValue();

		Slice s =
			findChannelsInRange(lowerFrequency, upperFrequency,
			                    spectralWindowChannels);

		if (s.length() > 0) {
			frequencySlices.push_back(s);
		}
	}

	// Convert the STL vector of slices into the expected Casa Vector<Vector
	// <Slice>> form. Element one of the Vector is empty indicating that the
	// entire correlations axis is desired.  The second element of the outer
	// array specifies different channel intervals along the channel axis.

	ChannelSlicer slices(2);

	// Install the polarization selections

	Vector<Slice> correlationSlices =
		selection.getCorrelationSlices(polarizationId);
	if (correlationSlices.empty()) {

		Int nCorrelations;
		subtableColumns_p->polarization().numCorr().get(
			polarizationId, nCorrelations);

		correlationSlices = Vector<Slice>(1, Slice(0, nCorrelations));
	}

	slices.setSubslicer(0, ChannelSubslicer(correlationSlices));

	ChannelSubslicer frequencyAxis(frequencySlices.size());

	for (Int i = 0; i <(int) frequencySlices.size(); i++) {
		frequencyAxis.setSlice(i, frequencySlices[i]);
	}

	slices.setSubslicer(1, frequencyAxis);

	// Package up result and return it.

    std::shared_ptr<ChannelSelector> result(new ChannelSelector
        (time, msId, spectralWindowId, polarizationId, slices));

	return result;
}

MFrequency::Convert
VisibilityIteratorImpl2::makeFrequencyConverter(
	Double time,
	int spectralWindowId,
	Int otherFrameOfReference,
	Bool toObservedFrame,
	Unit unit) const
{

	// Time is from the "time" field of the data row and is potentially in
	// MFrequency::Types observatoryFrequencyType =
	getObservatoryFrequencyType();

	MFrequency::Types measurementFrequencyType =
		static_cast<MFrequency::Types>(getMeasurementFrame(spectralWindowId));

	// Set up frequency converter so that we can convert to the
	// measured frequency
	// =========================================================

	// Take the provided time in units native to the MS and attach the frame of
	// reference so that the time users won't be confused(most MSs have time in
	// UTC, but there are some that use different time standards).

	MEpoch epoch(MVEpoch(Quantity(time, "s")), timeFrameOfReference_p);

	MPosition position = getObservatoryPosition();
	MDirection direction = phaseCenter();

	MeasFrame measFrame(epoch, position, direction);

	MFrequency::Ref observedFrame(measurementFrequencyType, measFrame);

	MFrequency::Types selectionFrame =
		static_cast<MFrequency::Types>(otherFrameOfReference);

	MFrequency::Convert result;

	if (toObservedFrame) {

		result = MFrequency::Convert(unit, selectionFrame, observedFrame);
	}
	else{

		result = MFrequency::Convert(unit, observedFrame, selectionFrame);
	}

	return result;
}

Slice
VisibilityIteratorImpl2::findChannelsInRange(
    Double lowerFrequency, Double upperFrequency,
    const vi::SpectralWindowChannels & spectralWindowChannels) const
{
    ThrowIf(spectralWindowChannels.empty(),
            String::format(
            "No spectral window channel info for  ms=%d", msId()));

    return spectralWindowChannels.getIntersection(lowerFrequency, upperFrequency);
}

Int
VisibilityIteratorImpl2::getNMs() const
{
	return measurementSets_p.nelements();
}


MFrequency::Types
VisibilityIteratorImpl2::getObservatoryFrequencyType() const
{
	const MFrequency & f0 = msIter_p->frequency0();

	MFrequency::Types t = MFrequency::castType(f0.getRef().getType());

	return t;
}

MPosition
VisibilityIteratorImpl2::getObservatoryPosition() const
{
	return msIter_p->telescopePosition();
}

const SpectralWindowChannels &
VisibilityIteratorImpl2::getSpectralWindowChannels(
	Int msId,
	Int spectralWindowId) const
{
	const SpectralWindowChannels * cached =
		spectralWindowChannelsCache_p->find(msId, spectralWindowId);

	if (cached != 0) {
		return * cached;
	}

	// Get the columns for the spectral window subtable and then select out the
	// frequency and width columns.  Use those columns to extract the frequency
	// and width lists for the specified spectral window.

	const MSSpWindowColumns& spectralWindow =
		subtableColumns_p->spectralWindow();

	const ArrayColumn<Double>& frequenciesColumn = spectralWindow.chanFreq();
	Vector<Double> frequencies;
	frequenciesColumn.get(spectralWindowId, frequencies, true);

	const ArrayColumn<Double>& widthsColumn = spectralWindow.chanWidth();
	Vector<Double> widths;
	widthsColumn.get(spectralWindowId, widths, true);

	Assert(!frequencies.empty());
	Assert(frequencies.size() == widths.size());

	// Use the frequencies and widths to fill out a vector of
	// SpectralWindowChannel objects.(No: This array needs to be in order of
	// increasing frequency.)  N.B.: If frequencies are in random order(i.e.,
	// rather than reverse order) then all sorts of things will break elsewhere.
	// Width also needs to be positive.

	SpectralWindowChannels *result =
		new SpectralWindowChannels(frequencies, widths);
	bool inOrder = true;

	for (Int i = 0; i <(int) frequencies.nelements(); i++) {
		(*result)[i] = SpectralWindowChannel(i, frequencies[i], abs(widths[i]));
		inOrder = inOrder &&(i == 0 || frequencies[i] > frequencies[i - 1]);
	}

	// Sanity check for frequencies that aren't monotonically
	// increasing/decreasing.

	for (Int i = 1; i <(int) frequencies.nelements(); i++) {
		ThrowIf(
			abs((*result)[i].getChannel() -(*result)[i-1].getChannel()) != 1,
			String::format(
				"Spectral window %d in MS #%d has random ordered frequencies",
				spectralWindowId, msId));
	}

	spectralWindowChannelsCache_p->add(result, msId, spectralWindowId);

	return * result;
}

void
VisibilityIteratorImpl2::configureNewChunk()
{
    rowBounds_p.chunkNRows_p = msIter_p->table().nrow();
    rowBounds_p.subchunkBegin_p = -1; // undefined value
    rowBounds_p.subchunkEnd_p = -1;   // will increment to 1st row

    cache_p.chunkRowIds_p.resize(0); // flush cached row number map.

    // If this is a new MeasurementSet then set up the antenna locations, etc.

    if (msIter_p->newMS()) {

        // Flush some cache flag values

        cache_p.flush();

        msd_p.setAntennas(msIter_p->msColumns().antenna());

        // Grab the time frame of reference so that it can be converted to UTC
        // for use in other frame of reference conversions.

        timeFrameOfReference_p = msIter_p->msColumns().timeMeas()(0).getRef();

    }

    if (isNewMs()) { // New ms so flush pointing caches(if they exist).
        pointingDirectionCache_p.reset();
        pointingSource_p.reset();
    }

    if (msIter_p->newField() || msIterAtOrigin_p) {
        msd_p.setFieldCenter(msIter_p->phaseCenter());
    }

    if(nRowBlocking_p >0)
    {
        attachColumns(msIter_p->table());

        // Fetch all of the times in this chunk

        rowBounds_p.times_p.resize(rowBounds_p.chunkNRows_p);
        columns_p.time_p.getColumn(rowBounds_p.times_p);
    }
    else
    {
        // Columns are attached to the msIter chunk iteration.
        // This is needed for the call of setTileCache() below, which
        // performs some tests on the attached columns
        // Later, in configureNewSubchunk the columns are reset to
        // the subchunk msIterSubchunk_p columns.
        attachColumns(msIter_p->table());
    }

    // Reset channel selectors vector in each chunk
    channelSelectors_p.clear();

    // If frequency selections are constant for the whole chunk
    if (freqSelScope_p == ChunkScope)
    {
        // If selection does not depend on time, assign a time value of -1.
        // Since there is only one DDId we can ask for a single SPWId, PolID
        // in msIter.
        double timeStamp = -1;
        if(frequencySelections_p->getFrameOfReference() == FrequencySelection::ByChannel)
            timeStamp = msIter_p->msColumns().time().asdouble(0);
        // Note that channelSelectorNRow is not initialized since it should
        // refer to nunber of subchunk rows. It is properly set in configureNewSubchunk
        channelSelectors_p.push_back(
                determineChannelSelection(timeStamp,
                                          msIter_p->spectralWindowId(),
                                          msIter_p->polarizationId(), msId()));
    }

    setTileCache();
}

const MSDerivedValues &
VisibilityIteratorImpl2::getMsd() const
{
	return msd_p;
}

void
VisibilityIteratorImpl2::setTileCache()
{
	std::lock_guard<std::mutex> lck(*tileCacheModMtx_p);

	if ((*tileCacheIsSet_p)[msId()]
	    || !msIter_p->newMS()
	    || autoTileCacheSizing_p) {
		return; // Only useful when at start of an MS
	}

	// This function sets the tile cache because of a feature in sliced data
	// access that grows memory dramatically in some cases if (useSlicer_p) {

//    if (!(msIter_p->newDataDescriptionId() || msIter_p->newMS()) ) {
//        return;
//    }

	const MeasurementSet & theMs = msIter_p->ms();
	if (theMs.tableType() == Table::Memory) {
		return;
	}

//    if (autoTileCacheSizing_p) {
//        return; // take the default behavior
//    }

	vector<MSMainEnums::PredefinedColumns> columnIds =
		{ MS::CORRECTED_DATA, MS::DATA, MS::FLAG, MS::MODEL_DATA, MS::SIGMA,
		  MS::SIGMA_SPECTRUM, MS::UVW, MS::WEIGHT, MS::WEIGHT_SPECTRUM };

	vector<String> msNames;

	if (theMs.tableInfo().subType() == "CONCATENATED") {

		Block<String> names = theMs.getPartNames(false);
		msNames.assign(names.begin(), names.end());

		for (String msName : msNames) {
			MeasurementSet anMs(msName);

			setMsCacheSizes(anMs, columnIds);
		}

	} else {
		setMsCacheSizes(theMs, columnIds);
	}
	(*tileCacheIsSet_p)[msId()] = true;
}

void VisibilityIteratorImpl2::setMsCacheSizes(
	const MeasurementSet & ms,
	vector<MSMainEnums::PredefinedColumns> columnIds)
{
	const ColumnDescSet & cds = ms.tableDesc().columnDescSet();

	for (MSMainEnums::PredefinedColumns  columnId : columnIds) {

		String column = MS::columnName(columnId);

		try {

			if (!cds.isDefined(column) || !usesTiledDataManager(column, ms)) {
				continue; // skip if column not in MS or not using tiles
			}

			if (columnId == MS::WEIGHT_SPECTRUM
			    || columnId == MS::SIGMA_SPECTRUM) {

				// These two columns are frequently present in an MS but
				// uninitialized.

				TableColumn tableColumn(ms, column);
				if (!tableColumn.hasContent()) {
					continue; // Skip
				}
			}

			setMsColumnCacheSizes(ms, column);

		} catch(AipsError & e) {
			continue; // It failed so leave the caching as is
		}
	}
}

void
VisibilityIteratorImpl2::setMsColumnCacheSizes(const MeasurementSet & partMs,
                                               const string & column)
{
	// For the column in the provided MS, loop over the hypercubes and
	// set the cache size appropriately.

	ROTiledStManAccessor accessor(partMs, column, true);
	uInt nHypercubes = accessor.nhypercubes();

	for (uInt cube = 0; cube != nHypercubes; cube++) {

		// Get hypercube shape(includes row axis) and tile shape(does not
		// include the row axis).

		const IPosition tileShape(accessor.getTileShape(cube));
		const IPosition hypercubeShape(accessor.getHypercubeShape(cube));

		uInt nAxes = hypercubeShape.size();  // how many axes
		if (nAxes < 1) {

			// Empty hypercube so skip it. Can't rely on loop below to handle
			// this case because nAxes is unsigned so nAxes-1 is going to wrap
			// around and become yuuge!

			continue;
		}

		// Compute the appropriate cache size which will hold at least a single
		// row's worth of tiles in the cache.  Use the factor of 2 as the
		// initial value since some large Alma data sets cause the row to span
		// tiles along the baseline axis due to the autocorrelations not
		// occurring near the corresponding cross correlation baselines.

		// Doubling to handle case where baselines span tiles in the row
		// direction
		uInt cacheSize = 2;

		// Compute the number of tiles required to span the non-row axes of the
		// hypercube.

		for (uInt axis = 0; axis < nAxes - 1; ++axis) {
			cacheSize *=
				(uInt)ceil(hypercubeShape[axis] /(Float)(tileShape[axis]));
		}

		// Apply the cache size(in tiles).

		accessor.setHypercubeCacheSize(cube, cacheSize);
	}
}


Bool
VisibilityIteratorImpl2::usesTiledDataManager(
	const String & columnName,
	const MeasurementSet & theMs) const
{
	Bool noData = false;

	// Have to do something special about weight_spectrum as it tend to exist
	// but has no valid data.

	noData = noData ||
		(columnName == MS::columnName(MS::WEIGHT_SPECTRUM)
		 && !weightSpectrumExists());

	noData = noData ||
		(columnName == MS::columnName(MS::SIGMA_SPECTRUM)
		 && !sigmaSpectrumExists());

	// Check to see if the column exist and have valid data

	noData = noData ||
		(columnName == MS::columnName(MS::DATA) &&
		 (columns_p.vis_p.isNull() || !columns_p.vis_p.isDefined(0)));

	noData = noData ||
		(columnName == MS::columnName(MS::MODEL_DATA) &&
		 (columns_p.modelVis_p.isNull() || !columns_p.modelVis_p.isDefined(0)));

	noData = noData ||
		(columnName == MS::columnName(MS::CORRECTED_DATA) &&
		 (columns_p.corrVis_p.isNull() || !columns_p.corrVis_p.isDefined(0)));

	noData = noData ||
		(columnName == MS::columnName(MS::FLAG) &&
		 (columns_p.flag_p.isNull() || !columns_p.flag_p.isDefined(0)));

	noData = noData ||
		(columnName == MS::columnName(MS::WEIGHT) &&
		 (columns_p.weight_p.isNull() || !columns_p.weight_p.isDefined(0)));

	noData = noData ||
		(columnName == MS::columnName(MS::SIGMA) &&
		 (columns_p.sigma_p.isNull() || !columns_p.sigma_p.isDefined(0)));

	noData = noData ||
		(columnName == MS::columnName(MS::UVW) &&
		 (columns_p.uvw_p.isNull() || !columns_p.uvw_p.isDefined(0)));

	Bool usesTiles = false;

	if (!noData) {
		String dataManType =
			RODataManAccessor(theMs, columnName, true).dataManagerType();

		usesTiles = dataManType.contains("Tiled");
	}

	return usesTiles;
}

void
VisibilityIteratorImpl2::attachColumns(const Table & t)
{
	columns_p.attachColumns(t);

	floatDataFound_p = columns_p.isFloatDataPresent();
}

MEpoch
VisibilityIteratorImpl2::getEpoch() const
{
	MEpoch mEpoch = msIter_p->msColumns().timeMeas()(0);

	return mEpoch;
}

Vector<Float>
VisibilityIteratorImpl2::getReceptor0Angle()
{
	Int nAntennas = this->nAntennas();

	Vector<Float> receptor0Angle(nAntennas);

	for (int i = 0; i < nAntennas; i++) {
		receptor0Angle[i] = msIter_p->receptorAngle()(0, i);
	}

	return receptor0Angle;
}

void
VisibilityIteratorImpl2::getRowIds(Vector<rownr_t> & rowIds) const
{
    if(nRowBlocking_p >0)
    {
        // Resize the rowIds vector and fill it with the row numbers contained in
        // the current subchunk. These row numbers are relative to the reference
        // table used by MSIter to define a chunk; thus rowId 0 is the first row in
        // the chunk.
        rowIds.resize(rowBounds_p.subchunkNRows_p);
        rowIds = rowBounds_p.subchunkRows_p.convert();

        if (cache_p.chunkRowIds_p.nelements() == 0) {
            // Create chunkRowIds_p as a "map" from chunk rows to MS rows. This
            // needs to be created once per chunk since a new reference table is
            // created each time the MSIter moves to the next chunk.
            cache_p.chunkRowIds_p = msIter_p->table().rowNumbers(msIter_p->ms());
        }

        // Using chunkRowIds_p as a map from chunk rows to MS rows replace the
        // chunk-relative row numbers with the actual row number from the MS.
        for (uInt i = 0; i < rowIds.nelements(); i++) {
            rowIds(i) = cache_p.chunkRowIds_p(rowIds(i));
        }
    }
    else
    {
        // Resize the rowIds vector and fill it with the row numbers contained in
        // the current subchunk.
        rowIds.resize(rowBounds_p.subchunkNRows_p);

        // Initialize the cache it if not yet done
        // (it is reset each time nextChunk() is called).
        // The cache contains the mapping between chunk rows and MS rows.
        // This needs to be created once per chunk since a new reference table is
        // created each time the MSIter moves to the next chunk.
        if (cache_p.chunkRowIds_p.size() == 0)
            cache_p.chunkRowIds_p = msIter_p->table().rowNumbers(msIter_p->ms());

        // Now create the map from subchunk rows to chunk rows. This
        // needs to be created for each subchunk since a new reference table
        // in the msIterInner_p iterator is created each time the MSIter moves.
        // Note that what we get are row Ids for msIter_p->table(), which is itself
        // a reference table.
        auto subchunkRowIds  = msIterSubchunk_p->table().rowNumbers(msIter_p->table(), true);

        // Now, for each row in the subchunk (i), get the row in the outer loop
        // table (subchunkRowId(i)) and use cache_p.chunkRowIds_p to get the row
        // in the original MS.
        for (uInt i = 0; i < rowIds.size(); i++)
            rowIds(i) = cache_p.chunkRowIds_p(subchunkRowIds(i));
    }
}

void
VisibilityIteratorImpl2::antenna1(Vector<Int> & ant1) const
{
	getColumnRows(columns_p.antenna1_p, ant1);
}

void
VisibilityIteratorImpl2::antenna2(Vector<Int> & ant2) const
{
	getColumnRows(columns_p.antenna2_p, ant2);
}

void
VisibilityIteratorImpl2::feed1(Vector<Int> & fd1) const
{
	getColumnRows(columns_p.feed1_p, fd1);
}

void
VisibilityIteratorImpl2::feed2(Vector<Int> & fd2) const
{
	getColumnRows(columns_p.feed2_p, fd2);
}

void
VisibilityIteratorImpl2::corrType(Vector<Int> & corrTypes) const
{
	Int polId = msIter_p->polarizationId();

	subtableColumns_p->polarization().corrType().get(polId, corrTypes, true);
}

void
VisibilityIteratorImpl2::flag(Cube<Bool> & flags) const
{
    getColumnRows(columns_p.flag_p, flags);
}

void
VisibilityIteratorImpl2::flag(Vector<Cube<Bool>> & flags) const
{
    getColumnRows(columns_p.flag_p, flags);
}

void
VisibilityIteratorImpl2::flag(Matrix<Bool> & flags) const
{
	Cube<Bool> flagCube;

	flag(flagCube);

	getColumnRows(columns_p.flag_p, flagCube);

	uInt nChannels = flagCube.shape()(1);

	flags.resize(nChannels, rowBounds_p.subchunkNRows_p);

	for (Int row = 0; row < rowBounds_p.subchunkNRows_p; row++) {

		for (uInt channel = 0; channel < nChannels; channel++) {

			Bool flagIt = flagCube(0, channel, row);

			for (Int correlation = 1;
			     correlation < nCorrelations_p && not flagIt;
			     correlation++) {

				flagIt = flagCube(correlation, channel, row);
			}

			flags(channel, row) = flagIt;
		}
	}
}

Bool
VisibilityIteratorImpl2::flagCategoryExists() const
{
	if (msIter_p->newMS()) { // Cache to avoid testing unnecessarily.
		cache_p.msHasFlagCategory_p = columns_p.flagCategory_p.hasContent();
	}
	return cache_p.msHasFlagCategory_p;
}

void
VisibilityIteratorImpl2::flagCategory(Array<Bool> & /*flagCategories*/) const
{
	ThrowIf(true, "The flag_category column is not supported.");
//    if (columns_p.flagCategory_p.isNull() ||
//        !columns_p.flagCategory_p.isDefined(0)) { // It often is.
//
//        flagCategories.resize();    // Zap it.
//    }
//    else {
//
//        // Since flag category is shaped [nC, nF, nCategories] it requires a
//        // slightly different slicer and cannot use the usual getColumns
//        method.
//
//        const ChannelSlicer & channelSlicer =
//            channelSelectors_p[0]->getSlicerForFlagCategories();
//
//        columns_p.flagCategory_p.getSliceForRows(
//             rowBounds_p.subchunkRows_p,
//             channelSlicer.getSlicerInCoreRep(),
//             flagCategories);
//    }
}

void
VisibilityIteratorImpl2::flagRow(Vector<Bool> & rowflags) const
{
	getColumnRows(columns_p.flagRow_p, rowflags);
}

void
VisibilityIteratorImpl2::observationId(Vector<Int> & obsIDs) const
{
	getColumnRows(columns_p.observation_p, obsIDs);
}

void
VisibilityIteratorImpl2::processorId(Vector<Int> & procIDs) const
{
	getColumnRows(columns_p.processor_p, procIDs);
}

void
VisibilityIteratorImpl2::scan(Vector<Int> & scans) const
{
	getColumnRows(columns_p.scan_p, scans);
}

void
VisibilityIteratorImpl2::stateId(Vector<Int> & stateIds) const
{
	getColumnRows(columns_p.state_p, stateIds);
}

void
VisibilityIteratorImpl2::time(Vector<Double> & t) const
{
	getColumnRows(columns_p.time_p, t);
}

void
VisibilityIteratorImpl2::timeCentroid(Vector<Double> & t) const
{
	getColumnRows(columns_p.timeCentroid_p, t);
}

void
VisibilityIteratorImpl2::timeInterval(Vector<Double> & t) const
{
	getColumnRows(columns_p.timeInterval_p, t);
}

void
VisibilityIteratorImpl2::exposure(Vector<Double> & expo) const
{
	getColumnRows(columns_p.exposure_p, expo);
}

void
VisibilityIteratorImpl2::visibilityCorrected(Cube<Complex> & vis) const
{
  if(columns_p.corrVis_p.isNull())
    throw AipsError("Requesting visibilityCorrected but column is null");
    getColumnRows(columns_p.corrVis_p, vis);
}

void
VisibilityIteratorImpl2::visibilityCorrected(Vector<Cube<Complex>> & vis) const
{
  if(columns_p.corrVis_p.isNull())
    throw AipsError("Requesting visibilityCorrected but column is null");
    getColumnRows(columns_p.corrVis_p, vis);
}

void
VisibilityIteratorImpl2::visibilityModel(Cube<Complex> & vis) const
{
    // See if the data can be filled from a virtual model column; if not then
    // get it from the model column.

    if (!fillFromVirtualModel(vis)) {
        getColumnRows(columns_p.modelVis_p, vis);
    }
}

void
VisibilityIteratorImpl2::visibilityModel(Vector<Cube<Complex>> & vis) const
{
    if (!fillFromVirtualModel(vis[0])) {
        getColumnRows(columns_p.modelVis_p, vis);
    }
    else
        throw AipsError("VisibilityIteratorImpl2::visibilityModel(Vector<Cube<Complex>> & vis) from model not yet implemented");
}

void
VisibilityIteratorImpl2::visibilityObserved(Cube<Complex> & vis) const
{
    if (floatDataFound_p) {

        // Since there is a floating data column, read that and convert it into
        // the expected Complex form.

        Cube<Float> dataFloat;

        getColumnRows(columns_p.floatVis_p, dataFloat);

        vis.resize(dataFloat.shape());

        convertArray(vis, dataFloat);
    }
    else {
      if(columns_p.vis_p.isNull())
        throw AipsError("Requesting visibilityObserved but column is null");
        getColumnRows(columns_p.vis_p, vis);
    }
}

void
VisibilityIteratorImpl2::visibilityObserved(Vector<Cube<Complex>> & vis) const
{
    if (floatDataFound_p) {

        // Since there is a floating data column, read that and convert it into
        // the expected Complex form.

        Vector<Cube<Float>> dataFloat;

        getColumnRows(columns_p.floatVis_p, dataFloat);

        vis.resize(dataFloat.size());

        size_t iVec = 0;
        for (iVec= 0; iVec < vis.size(); iVec++)
        {
            vis[iVec].resize(dataFloat[iVec].shape());

            convertArray(vis[iVec], dataFloat[iVec]);
        }
    }
    else {
      if(columns_p.vis_p.isNull())
        throw AipsError("Requesting visibilityObserved but column is null");
        getColumnRows(columns_p.vis_p, vis);
    }
}

void
VisibilityIteratorImpl2::floatData(Cube<Float> & fcube) const
{
    if (floatDataFound_p) {
        getColumnRows(columns_p.floatVis_p, fcube);
    }
    else{
        fcube.resize();
    }
}

void
VisibilityIteratorImpl2::floatData(Vector<Cube<Float>> & fcubes) const
{
    if (floatDataFound_p) {
        getColumnRows(columns_p.floatVis_p, fcubes);
    }
    else{
        fcubes.resize(1);
        fcubes[0].resize();
    }
}

void
VisibilityIteratorImpl2::uvw(Matrix<Double> & uvwmat) const
{
	getColumnRowsMatrix(columns_p.uvw_p, uvwmat, false);
}

// Fill in parallactic angle.
const Vector<Float> &
VisibilityIteratorImpl2::feed_pa(Double time) const
{
	// Absolute UT

	Double ut = time;

	if (ut != cache_p.feedpaTime_p) {

		// Set up the Epoch using the absolute MJD in seconds get the Epoch
		// reference from the column

		MEpoch mEpoch = getEpoch();

		const Matrix<Double> & angles = receptorAngles().xyPlane(0);
		Int nAnt = angles.shape()(1);

		Vector<Float> receptor0Angle(nAnt, 0);

		for (int i = 0; i < nAnt; i++) {
			receptor0Angle[i] = angles(0, i);
		}

		cache_p.feedpa_p.assign(
			feed_paCalculate(time, msd_p, nAnt, mEpoch, receptor0Angle));

		cache_p.feedpaTime_p = ut;
	}
	return cache_p.feedpa_p;
}

// Fill in parallactic angle.
const Float &
VisibilityIteratorImpl2::parang0(Double time) const
{
	if (time != cache_p.parang0Time_p) {

		cache_p.parang0Time_p = time;

		// Set up the Epoch using the absolute MJD in seconds get the Epoch
		// reference from the column
		MEpoch mEpoch = getEpoch();
		cache_p.parang0_p = parang0Calculate(time, msd_p, mEpoch);
	}
	return cache_p.parang0_p;
}

// Fill in parallactic angle(NO FEED PA offset!).
const Vector<Float> &
VisibilityIteratorImpl2::parang(Double time) const
{
	if (time != cache_p.parangTime_p) {

		cache_p.parangTime_p = time;

		// Set up the Epoch using the absolute MJD in seconds get the Epoch
		// reference from the column

		MEpoch mEpoch = getEpoch();
		Int nAnt = msIter_p->receptorAngle().shape()(1);

		cache_p.parang_p = parangCalculate(time, msd_p, nAnt, mEpoch);

	}
	return cache_p.parang_p;
}

// Fill in azimuth/elevation of the antennas.  Cloned from feed_pa, we need to
// check that this is all correct!
const Vector<MDirection> &
VisibilityIteratorImpl2::azel(Double ut) const
{
	if (ut != cache_p.azelTime_p) {

		cache_p.azelTime_p = ut;

		Int nAnt = msIter_p->receptorAngle().shape()(1);

		MEpoch mEpoch = getEpoch();

		azelCalculate(ut, msd_p, cache_p.azel_p, nAnt, mEpoch);

	}
	return cache_p.azel_p;
}


// Fill in azimuth/elevation of the antennas.  Cloned from feed_pa, we need to
// check that this is all correct!
MDirection
VisibilityIteratorImpl2::azel0(Double time) const
{
	// Absolute UT

	Double ut = time;

	if (ut != cache_p.azel0Time_p) {

		cache_p.azel0Time_p = ut;

		MEpoch mEpoch = getEpoch();

		azel0Calculate(time, msd_p, cache_p.azel0_p, mEpoch);

	}
	return cache_p.azel0_p;
}


// Hour angle at specified time.
Double
VisibilityIteratorImpl2::hourang(Double time) const
{
	// Absolute UT

	Double ut = time;

	if (ut != cache_p.hourangTime_p) {

		cache_p.hourangTime_p = ut;

		// Set up the Epoch using the absolute MJD in seconds get the Epoch
		// reference from the column keyword

		MEpoch mEpoch = getEpoch();

		cache_p.hourang_p = hourangCalculate(time, msd_p, mEpoch);

	}
	return cache_p.hourang_p;
}


void
VisibilityIteratorImpl2::sigma(Matrix<Float> & sigma) const
{
    getColumnRowsMatrix(columns_p.sigma_p, sigma, true);
}

void
VisibilityIteratorImpl2::sigma(Vector<Matrix<Float>> & sigma) const
{
    getColumnRowsMatrix(columns_p.sigma_p, sigma);
}

void
VisibilityIteratorImpl2::weight(Matrix<Float> & wt) const
{
    getColumnRowsMatrix(columns_p.weight_p, wt, true);
}

void
VisibilityIteratorImpl2::weight(Vector<Matrix<Float>> & wt) const
{
    getColumnRowsMatrix(columns_p.weight_p, wt);
}

Bool
VisibilityIteratorImpl2::weightSpectrumExists() const
{
	if (msIter_p->newSpectralWindow()) {
		// Cache to avoid testing unnecessarily.
		cache_p.msHasWeightSpectrum_p = columns_p.weightSpectrum_p.hasContent();
	}

	return cache_p.msHasWeightSpectrum_p;
}

Bool
VisibilityIteratorImpl2::sigmaSpectrumExists() const
{
	if (msIter_p->newMS()) { // Cache to avoid testing unnecessarily.

		cache_p.msHasSigmaSpectrum_p = columns_p.sigmaSpectrum_p.hasContent();

	}

	return cache_p.msHasSigmaSpectrum_p;
}

void
VisibilityIteratorImpl2::weightSpectrum(Cube<Float> & spectrum) const
{
    if (weightSpectrumExists())
        getColumnRows(columns_p.weightSpectrum_p, spectrum);
    else
        spectrum.resize(0, 0, 0);
}

void
VisibilityIteratorImpl2::weightSpectrum(Vector<Cube<Float>> & spectrum) const
{
    if (weightSpectrumExists())
        getColumnRows(columns_p.weightSpectrum_p, spectrum);
    else
    {
        spectrum.resize(1);
        spectrum[0].resize(0, 0, 0);
    }
}

void
VisibilityIteratorImpl2::sigmaSpectrum(Cube<Float> & spectrum) const
{
    if (sigmaSpectrumExists())
        getColumnRows(columns_p.sigmaSpectrum_p, spectrum);
    else
        spectrum.resize(0, 0, 0);
}

void
VisibilityIteratorImpl2::sigmaSpectrum(Vector<Cube<Float>> & spectrum) const
{
    if (sigmaSpectrumExists())
        getColumnRows(columns_p.sigmaSpectrum_p, spectrum);
    else
    {
        spectrum.resize(1);
        spectrum[0].resize(0, 0, 0);
    }
}

void
VisibilityIteratorImpl2::setWeightScaling(
	CountedPtr<WeightScaling> weightScaling)
{
	weightScaling_p = weightScaling;
}

Bool
VisibilityIteratorImpl2::hasWeightScaling() const
{
	return !weightScaling_p.null();
}

CountedPtr<WeightScaling>
VisibilityIteratorImpl2::getWeightScaling() const
{
	return weightScaling_p;
}


const VisImagingWeight &
VisibilityIteratorImpl2::getImagingWeightGenerator() const
{
	return imwgt_p;
}

Block<MeasurementSet>
VisibilityIteratorImpl2::getMeasurementSets() const
{
	return measurementSets_p;
}

Int
VisibilityIteratorImpl2::getReportingFrameOfReference() const
{
    Int frame;
    if (reportingFrame_p == VisBuffer2::FrameNotSpecified) {

        if (frequencySelections_p != 0) {

            frame = frequencySelections_p->getFrameOfReference();

            if (frame == FrequencySelection::ByChannel) {

                // Since selection was done by channels, the frequencies are
                // native.

                Vector<Int> spws;
                spectralWindows(spws);
                measurementFrame_p = getMeasurementFrame(spws[0]);
                frame = measurementFrame_p;
            }
        }
        else{
            frame = VisBuffer2::FrameNotSpecified;
        }
    }
    else{
        frame = reportingFrame_p;
    }

    return frame;
}

void
VisibilityIteratorImpl2::setReportingFrameOfReference(Int frame)
{
	ThrowIf(frame < 0 || frame >= MFrequency::N_Types,
	        String::format("Unknown frame: id=%d", frame));

	reportingFrame_p = frame;
}

VisBuffer2 *
VisibilityIteratorImpl2::getVisBuffer() const
{
	return vb_p;
}

VisBuffer2 *
VisibilityIteratorImpl2::getVisBuffer(const VisibilityIterator2 * vi) const
{
	ThrowIf(vb_p == nullptr, "VI Implementation has no VisBuffer.");
	vb_p->associateWithVi2(vi);
	return vb_p;
}


Int
VisibilityIteratorImpl2::nAntennas() const
{
	return subtableColumns_p->antenna().nrow(); // for single(sub)array only..
}

Int
VisibilityIteratorImpl2::nSpectralWindows() const
{
	return subtableColumns_p->spectralWindow().nrow();
}

Int
VisibilityIteratorImpl2::nDataDescriptionIds() const
{
	return subtableColumns_p->dataDescription().nrow();
}

Int
VisibilityIteratorImpl2::nPolarizationIds() const
{
	return subtableColumns_p->polarization().nrow();
}

rownr_t
VisibilityIteratorImpl2::nRowsViWillSweep() const
{
	Int numcoh = 0;
	for (uInt k = 0; k < uInt(msIter_p->numMS()) ; ++k) {
		numcoh += msIter_p->ms(k).nrow();
	}
	return numcoh;

}

const Table
VisibilityIteratorImpl2::attachTable() const
{
	return msIterSubchunk_p->table();
}

void
VisibilityIteratorImpl2::slurp() const
{
	// Set the table data manager(ISM and SSM) cache size to the full column
	// size, for the columns ANTENNA1, ANTENNA2, FEED1, FEED2, TIME, INTERVAL,
	// FLAG_ROW, SCAN_NUMBER and UVW

	Record dmInfo(msIter_p->ms().dataManagerInfo());

	RecordDesc desc = dmInfo.description();

	for (unsigned i = 0; i < dmInfo.nfields(); i++) {

		if (desc.isSubRecord(i)) {

			Record sub = dmInfo.subRecord(i);

			if (sub.fieldNumber("NAME") >= 0 &&
			    sub.fieldNumber("TYPE") >= 0 &&
			    sub.fieldNumber("COLUMNS") >= 0 &&
			    sub.type(sub.fieldNumber("NAME")) == TpString &&
			    sub.type(sub.fieldNumber("TYPE")) == TpString &&
			    sub.type(sub.fieldNumber("COLUMNS")) == TpArrayString) {

				Array<String> columns;
				dmInfo.subRecord(i).get("COLUMNS", columns);

				bool match = false;

				for (unsigned j = 0; j < columns.nelements(); j++) {

					String column = columns(IPosition(1, j));

					match |=(column == MS::columnName(MS::ANTENNA1) ||
					         column == MS::columnName(MS::ANTENNA2) ||
					         column == MS::columnName(MS::FEED1) ||
					         column == MS::columnName(MS::FEED2) ||
					         column == MS::columnName(MS::TIME) ||
					         column == MS::columnName(MS::INTERVAL) ||
					         column == MS::columnName(MS::FLAG_ROW) ||
					         column == MS::columnName(MS::SCAN_NUMBER) ||
					         column == MS::columnName(MS::UVW));
				}

				if (match) {

					String dm_name;
					dmInfo.subRecord(i).get("NAME", dm_name);

					String dm_type;
					dmInfo.subRecord(i).get("TYPE", dm_type);

					Bool can_exceed_nr_buckets = false;
					uInt num_buckets = msIter_p->ms().nrow();
					// One bucket is at least one row, so this is enough

					if (dm_type == "IncrementalStMan") {

						ROIncrementalStManAccessor acc(msIter_p->ms(), dm_name);
						acc.setCacheSize(num_buckets, can_exceed_nr_buckets);

					} else if (dm_type == "StandardStMan") {

						ROStandardStManAccessor acc(msIter_p->ms(), dm_name);
						acc.setCacheSize(num_buckets, can_exceed_nr_buckets);
					}
					/* These are the only storage managers which use the
					   BucketCache (and therefore are slow for random access and
					   small cache sizes)
					*/
				}
				else {

					String dm_name;
					dmInfo.subRecord(i).get("NAME", dm_name);

				}
			} else {
				cerr << "Data manager info has unexpected shape! "
				     << sub << endl;
			}
		}
	}
	return;
}

const Cube<Double> &
VisibilityIteratorImpl2::receptorAngles() const
{
	return msIter_p->receptorAngles();
}

IPosition
VisibilityIteratorImpl2::visibilityShape() const
{

	IPosition result(3,
	                 nCorrelations_p,
	                 channelSelectors_p[0]->getNFrequencies(),
	                 rowBounds_p.subchunkNRows_p);

	return result;
}

void
VisibilityIteratorImpl2::setFrequencySelections(
    FrequencySelections const& frequencySelections)
{
    pendingChanges_p->setFrequencySelections(frequencySelections.clone());

    channelSelectorCache_p->flush();
    spectralWindowChannelsCache_p->flush();
    channelSelectors_p.clear();
    channelSelectorsNrows_p.clear();
    setMetadataScope();
}

void
VisibilityIteratorImpl2::jonesC(
	Vector<SquareMatrix<complex<float>, 2> >& cjones) const
{
	cjones.resize(msIter_p->CJones().nelements());
	cjones = msIter_p->CJones();
}

void
VisibilityIteratorImpl2::writeFlag(const Cube<Bool> & flags)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	putColumnRows(columns_p.flag_p, flags);
}

void
VisibilityIteratorImpl2::writeFlagCategory(const Array<Bool>& flagCategory)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	// Flag categories are [nC, nF, nCategories] and therefore must use a
	// different slicer which also prevents use of more usual putColumn method.

	RefRows & rows = rowBounds_p.subchunkRows_p;
	const ChannelSlicer & channelSlicer =
		channelSelectors_p[0]->getSlicerForFlagCategories();

	columns_p.flagCategory_p.putSliceFromRows(
		rows, channelSlicer.getSlicerInCoreRep(), flagCategory);
}

void
VisibilityIteratorImpl2::writeFlagRow(const Vector<Bool> & rowflags)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	putColumnRows(columns_p.flagRow_p, rowflags);
}

void
VisibilityIteratorImpl2::writeVisCorrected(const Cube<Complex> & vis)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	putColumnRows(columns_p.corrVis_p, vis);
}

void
VisibilityIteratorImpl2::writeVisModel(const Cube<Complex> & vis)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	putColumnRows(columns_p.modelVis_p, vis);
}

void
VisibilityIteratorImpl2::writeVisObserved(const Cube<Complex> & vis)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	if (floatDataFound_p) {

		// This MS has float data; convert the cube to float
		// and right it out

		Cube<Float> dataFloat = real(vis);
		putColumnRows(columns_p.floatVis_p, dataFloat);

	}
	else {
		putColumnRows(columns_p.vis_p, vis);
	}

}

void
VisibilityIteratorImpl2::writeWeight(const Matrix<Float> & weight)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	putColumnRows(columns_p.weight_p, weight);
}

void
VisibilityIteratorImpl2::writeWeightSpectrum(const Cube<Float> & weightSpectrum)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	if (!columns_p.weightSpectrum_p.isNull()) {
		putColumnRows(columns_p.weightSpectrum_p, weightSpectrum);
	}
}

void
VisibilityIteratorImpl2::initWeightSpectrum(const Cube<Float> & weightSpectrum)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	if (!columns_p.weightSpectrum_p.isNull()) {
		RefRows & rows = rowBounds_p.subchunkRows_p;
		columns_p.weightSpectrum_p.putColumnCells(rows, weightSpectrum);
	}
}

void
VisibilityIteratorImpl2::writeSigmaSpectrum(const Cube<Float> & sigmaSpectrum)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	if (!columns_p.sigmaSpectrum_p.isNull()) {
		putColumnRows(columns_p.sigmaSpectrum_p, sigmaSpectrum);
	}
}

void
VisibilityIteratorImpl2::initSigmaSpectrum(const Cube<Float> & sigmaSpectrum)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	if (!columns_p.sigmaSpectrum_p.isNull() ) {
		RefRows & rows = rowBounds_p.subchunkRows_p;
		columns_p.sigmaSpectrum_p.putColumnCells(rows, sigmaSpectrum);
	}
}

void
VisibilityIteratorImpl2::writeSigma(const Matrix<Float> & sigma)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	putColumnRows(columns_p.sigma_p, sigma);
}

void
VisibilityIteratorImpl2::writeModel(
	const RecordInterface& rec, Bool iscomponentlist, Bool incremental)
{

	ThrowIf(!isWritable(), "This visibility iterator is not writable");
	/* Version 1 stuff
	Vector<Int> fields = columns_p.field_p.getColumn();

	const Int option = Sort::HeapSort | Sort::NoDuplicates;
	const Sort::Order order = Sort::Ascending;

	Int nFields = GenSort<Int>::sort(fields, order, option);

	// Make sure  we have the right size

	fields.resize(nFields, true);
	*/

	Matrix<Int>  combiIndex;
	MSUtil::getIndexCombination(MSColumns(ms()), combiIndex);
	Vector<Int> selectedWindows;
	Vector<Int> nChannels;
	Vector<Int> firstChannels;
	Vector<Int> channelIncrement;

	std::tie(selectedWindows, nChannels, firstChannels, channelIncrement) =
		getChannelInformation();
	 Matrix<Int> chansel(selectedWindows.nelements(),4);
	 chansel.column(0)=selectedWindows;
	 chansel.column(1)=firstChannels;
	 chansel.column(2)=nChannels;
	 chansel.column(3)=channelIncrement;
	CountedPtr<VisModelDataI> visModelData = VisModelDataI::create();

	/*visModelData->putModelI(
		ms(), rec, fields, selectedWindows, firstChannels, nChannels,
		channelIncrement, iscomponentlist, incremental);*/
	//Version 2 interface to keep state and scan number in track
	 visModelData->putModelI (ms(), rec, combiIndex, chansel, iscomponentlist, incremental);
	
}

VisibilityIteratorImpl2::ChannelInfo
VisibilityIteratorImpl2::getChannelInformationUsingFrequency() const
{
    const FrequencySelectionUsingFrame  *frequencySelection =
    dynamic_cast<const FrequencySelectionUsingFrame*>(
        &frequencySelections_p->get(msId()));
    if (!frequencySelection)
        throw(AipsError(
            "Programmer Error channel info with wrong object called"));
    set<Int> windows = frequencySelection->getSelectedWindows();

    Vector<Int> spectralWindow(windows.size());
    Vector<Int> nChannels(windows.size(), -1);
    Vector<Int> firstChannel(windows.size(), -1);
    Vector<Int> channelIncrement(windows.size(), -1);


    Int i = 0;
    map<int, pair<int, int> > spwRanges=frequencySelection->getChannelRange ( measurementSets_p [msId()]) ;

    for (set<Int>::iterator j = windows.begin(); j != windows.end(); j++){

        //spectralWindow [i] = * j;
        auto sel = spwRanges.find(*j);

        if(sel != spwRanges.end()){
            spectralWindow.resize(i+1, True);
            nChannels.resize(i+1,True);
            firstChannel.resize(i+1, True);
            channelIncrement.resize(i+1,True);
            spectralWindow [i] = * j;
            nChannels [i] = (sel->second).first;
            firstChannel [i] =(sel->second).second;
            channelIncrement[i] = 1;

            ++i;
        }
       
    }

    return std::make_tuple(spectralWindow, nChannels, firstChannel,
                           channelIncrement);
}


VisibilityIteratorImpl2::ChannelInfo
VisibilityIteratorImpl2::getChannelInformation() const
{
    const FrequencySelectionUsingChannels * frequencySelection =
    dynamic_cast<const FrequencySelectionUsingChannels *>(
        &frequencySelections_p->get(msId()));

    if (frequencySelection == 0) {
        return getChannelInformationUsingFrequency();
    }

    Vector<Int> spectralWindow;
    Vector<Int> nChannels;
    Vector<Int> firstChannel;
    Vector<Int> channelIncrement;

    if (frequencySelection->empty()) {

        // No explicit selection, so everything is selected.

        casa::ms::SpectralWindows spectralWindows(& measurementSets_p[msId()]);

        spectralWindow.resize(spectralWindows.size());
        nChannels.resize(spectralWindows.size());
        firstChannel.resize(spectralWindows.size());
        channelIncrement.resize(spectralWindows.size());

        Int i = 0;

        for(casa::ms::SpectralWindows::const_iterator s =
            spectralWindows.begin();
            s != spectralWindows.end();
            s++) {

            spectralWindow(i) = s->id();
            nChannels(i) = s->nChannels();
            firstChannel(i) = 0;
            channelIncrement(i) = 1;

            i++;
        }
    }
    else {

        // Use the explicit channel-based selection to compute the result.

        spectralWindow.resize(frequencySelection->size());
        nChannels.resize(frequencySelection->size());
        firstChannel.resize(frequencySelection->size());
        channelIncrement.resize(frequencySelection->size());

        Int i = 0;
        for (FrequencySelectionUsingChannels::const_iterator j =
             frequencySelection->begin();
             j != frequencySelection->end();
             ++j) {

            spectralWindow(i) = j->spectralWindow_p;
            nChannels(i) = j->nChannels_p;
            firstChannel(i) = j->firstChannel_p;
            channelIncrement(i) = j->increment_p;

            i++;
        }
    }

    return std::make_tuple(spectralWindow, nChannels, firstChannel,
                           channelIncrement);
}

Vector<casacore::Vector<Int> > VisibilityIteratorImpl2::getAllSelectedSpws() const{
	
	Vector<Vector<Int> > retval(	 frequencySelections_p->size());
	for (uInt k=0; k < retval.nelements(); ++k){
		std::set<Int> spw=(frequencySelections_p->get(k)).getSelectedWindows();
		retval[k]=Vector<Int>(std::vector<Int>(spw.begin(), spw.end()));
	}
	return retval;
}

void
VisibilityIteratorImpl2::writeBackChanges(VisBuffer2 * vb)
{
	ThrowIf(!isWritable(), "This visibility iterator is not writable");

	if (backWriters_p.empty()) {
		initializeBackWriters();
	}

	VisBufferComponents2 dirtyComponents = vb->dirtyComponentsGet();

	for (VisBufferComponents2::const_iterator dirtyComponent =
		     dirtyComponents.begin();
	     dirtyComponent != dirtyComponents.end();
	     dirtyComponent++) {

		ThrowIf(backWriters_p.find(* dirtyComponent) == backWriters_p.end(),
		        String::format(
			        "No writer defined for VisBuffer component %d",
			        *dirtyComponent));
		BackWriter * backWriter = backWriters_p[ * dirtyComponent];

		try {
			(* backWriter)(this, vb);
		} catch(AipsError & e) {
			Rethrow(
				e,
				String::format(
					"Error while writing back VisBuffer component %d",
					*dirtyComponent));
		}
	}
}

void
VisibilityIteratorImpl2::initializeBackWriters()
{
	backWriters_p[VisBufferComponent2::FlagCube] =
		makeBackWriter(& VisibilityIteratorImpl2::writeFlag, & VisBuffer2::flagCube);
	backWriters_p[VisBufferComponent2::FlagRow] =
		makeBackWriter(& VisibilityIteratorImpl2::writeFlagRow, & VisBuffer2::flagRow);
	backWriters_p[VisBufferComponent2::FlagCategory] =
		makeBackWriter(& VisibilityIteratorImpl2::writeFlagCategory, & VisBuffer2::flagCategory);
	backWriters_p[VisBufferComponent2::Sigma] =
		makeBackWriter(& VisibilityIteratorImpl2::writeSigma, & VisBuffer2::sigma);
	backWriters_p[VisBufferComponent2::Weight] =
		makeBackWriter(& VisibilityIteratorImpl2::writeWeight, & VisBuffer2::weight);
	backWriters_p[VisBufferComponent2::WeightSpectrum] =
		makeBackWriter(& VisibilityIteratorImpl2::writeWeightSpectrum, & VisBuffer2::weightSpectrum);
	backWriters_p[VisBufferComponent2::SigmaSpectrum] =
		makeBackWriter(& VisibilityIteratorImpl2::writeSigmaSpectrum, & VisBuffer2::sigmaSpectrum);

	// Now do the visibilities.

	backWriters_p[VisBufferComponent2::VisibilityCubeObserved] =
		makeBackWriter(& VisibilityIteratorImpl2::writeVisObserved, & VisBuffer2::visCube);
	backWriters_p[VisBufferComponent2::VisibilityCubeCorrected] =
		makeBackWriter(& VisibilityIteratorImpl2::writeVisCorrected, & VisBuffer2::visCubeCorrected);
	backWriters_p[VisBufferComponent2::VisibilityCubeModel] =
		makeBackWriter(& VisibilityIteratorImpl2::writeVisModel, & VisBuffer2::visCubeModel);

}

VisibilityIteratorImpl2::PendingChanges::PendingChanges()
	: frequencySelections_p(0)
	, frequencySelectionsPending_p(false)
	, interval_p(Empty)
	, nRowBlocking_p(Empty)
{}

VisibilityIteratorImpl2::PendingChanges::~PendingChanges()
{
	delete frequencySelections_p;
}

VisibilityIteratorImpl2::PendingChanges *
VisibilityIteratorImpl2::PendingChanges::clone() const
{
	PendingChanges * theClone = new PendingChanges();

	theClone->frequencySelections_p =
		new FrequencySelections(* frequencySelections_p);
	theClone->frequencySelectionsPending_p = frequencySelectionsPending_p;
	theClone->interval_p = interval_p;
	theClone->nRowBlocking_p = nRowBlocking_p;

	return theClone;
}



Bool
VisibilityIteratorImpl2::PendingChanges::empty() const
{
	Bool result = frequencySelections_p == 0 &&
		interval_p == Empty &&
		nRowBlocking_p == Empty;

	return result;
}

pair<Bool, FrequencySelections *>
VisibilityIteratorImpl2::PendingChanges::popFrequencySelections()
{
	// yields ownership
	FrequencySelections * result = frequencySelections_p;
	Bool wasPending = frequencySelectionsPending_p;

	frequencySelections_p = 0;
	frequencySelectionsPending_p = false;

	return make_pair(wasPending, result);
}

pair<Bool, Double>
VisibilityIteratorImpl2::PendingChanges::popInterval()
{
	pair<Bool,Double> result = make_pair(interval_p != Empty, interval_p);

	interval_p = Empty;

	return result;
}

pair<Bool, Int>
VisibilityIteratorImpl2::PendingChanges::popNRowBlocking()
{
	pair<Bool,Int> result = make_pair(nRowBlocking_p != Empty, nRowBlocking_p);

	nRowBlocking_p = Empty;

	return result;
}

void
VisibilityIteratorImpl2::PendingChanges::setFrequencySelections(
	FrequencySelections * fs)
{
	// takes ownership
	Assert(!frequencySelectionsPending_p);

	frequencySelections_p = fs;
	frequencySelectionsPending_p = true;
}

void
VisibilityIteratorImpl2::PendingChanges::setInterval(Double interval)
{
	Assert(interval_p == Empty);

	interval_p = interval;
}

void
VisibilityIteratorImpl2::PendingChanges::setNRowBlocking(Int nRowBlocking)
{
	Assert(nRowBlocking_p == Empty);

	nRowBlocking_p = nRowBlocking;
}

bool
VisibilityIteratorImpl2::fillFromVirtualModel(Cube <Complex> & value) const
{
	// The model is virtual if there is no model column or there is a virtual
	// model defined in the MS.

	String modelKey = "definedmodel_field_" + String::toString(vb_p->fieldId());
	Int sourceRow;
	Bool hasModelKey= !(modelDataGenerator_p == 0) &&
		modelDataGenerator_p->isModelDefinedI(vb_p->fieldId()(0), ms(),
		                                      modelKey, sourceRow);

	Bool isVirtual = hasModelKey || !(ms().tableDesc().isColumn("MODEL_DATA"));

	if (isVirtual) {
	  modelDataGenerator_p->init(*vb_p);


	  //////////This bit can be removed once version 1 is no longer read
	  if(!(modelDataGenerator_p->isVersion2())){

	    auto field = vb_p->fieldId()(0);
	    auto spw = vb_p->spectralWindows()(0);
		if (modelDataGenerator_p->hasModel(msId(),field , spw) == -1) {

			// If the model generator does not have a model for this(ms,field,
			// spectralWindow) then try to add it.

			if (hasModelKey) {

				// Read the record of model information and if found add it to
				// the model generator.

				TableRecord modelRecord;
				if (modelDataGenerator_p->getModelRecordI(
					    modelKey, modelRecord, ms())) {

					modelDataGenerator_p->addModel(
						modelRecord, Vector<Int>(1, msId()), * vb_p);
				}
			}
		}
	  }
	  /////////////////////////
		// Now use the model data generator to fill in the model data.
		// Temporarily make the VisBuffer writable, if it wasn't already.

		Bool wasWritable = vb_p->setWritability(true);
		modelDataGenerator_p->getModelVis(const_cast <VisBuffer2 &>(* vb_p));
		vb_p->setWritability(wasWritable);

		// Put the model cube into the provided container.  If that turns out to
		// be the model component of the VIIs VB2 then this will be a no-op.

		value = vb_p->visCubeModel();
		return true; // filled it
	}

	return false; // Did not fill
}

//**********************************************************************
// Methods to access the subtables.
//**********************************************************************

// Access to antenna subtable
const casacore::MSAntennaColumns& VisibilityIteratorImpl2::antennaSubtablecols() const
{
    return msIter_p->msColumns().antenna();
}

// Access to dataDescription subtable
const casacore::MSDataDescColumns& VisibilityIteratorImpl2::dataDescriptionSubtablecols() const
{
    return msIter_p->msColumns().dataDescription();
}

// Access to feed subtable
const casacore::MSFeedColumns& VisibilityIteratorImpl2::feedSubtablecols() const
{
    return msIter_p->msColumns().feed();
}

// Access to field subtable
const casacore::MSFieldColumns& VisibilityIteratorImpl2::fieldSubtablecols() const
{
    return msIter_p->msColumns().field();
}

// Access to flagCmd subtable
const casacore::MSFlagCmdColumns& VisibilityIteratorImpl2::flagCmdSubtablecols() const
{
    return msIter_p->msColumns().flagCmd();
}

// Access to history subtable
const casacore::MSHistoryColumns& VisibilityIteratorImpl2::historySubtablecols() const
{
    return msIter_p->msColumns().history();
}

// Access to observation subtable
const casacore::MSObservationColumns& VisibilityIteratorImpl2::observationSubtablecols() const
{
    return msIter_p->msColumns().observation();
}

// Access to pointing subtable
const casacore::MSPointingColumns& VisibilityIteratorImpl2::pointingSubtablecols() const
{
    return msIter_p->msColumns().pointing();
}

// Access to polarization subtable
const casacore::MSPolarizationColumns& VisibilityIteratorImpl2::polarizationSubtablecols() const
{
    return msIter_p->msColumns().polarization();
}

// Access to processor subtable
const casacore::MSProcessorColumns& VisibilityIteratorImpl2::processorSubtablecols() const
{
    return msIter_p->msColumns().processor();
}

// Access to spectralWindow subtable
const casacore::MSSpWindowColumns& VisibilityIteratorImpl2::spectralWindowSubtablecols() const
{
    return msIter_p->msColumns().spectralWindow();
}

// Access to state subtable
const casacore::MSStateColumns& VisibilityIteratorImpl2::stateSubtablecols() const
{
    return msIter_p->msColumns().state();
}

// Access to doppler subtable
const casacore::MSDopplerColumns& VisibilityIteratorImpl2::dopplerSubtablecols() const
{
    return msIter_p->msColumns().doppler();
}

// Access to freqOffset subtable
const casacore::MSFreqOffsetColumns& VisibilityIteratorImpl2::freqOffsetSubtablecols() const
{
    return msIter_p->msColumns().freqOffset();
}

// Access to source subtable
const casacore::MSSourceColumns& VisibilityIteratorImpl2::sourceSubtablecols() const
{
    return msIter_p->msColumns().source();
}

// Access to sysCal subtable
const casacore::MSSysCalColumns& VisibilityIteratorImpl2::sysCalSubtablecols() const
{
    return msIter_p->msColumns().sysCal();
}

// Access to weather subtable
const casacore::MSWeatherColumns& VisibilityIteratorImpl2::weatherSubtablecols() const
{
    return msIter_p->msColumns().weather();
}

} // end namespace vi

} //# NAMESPACE CASA - END
