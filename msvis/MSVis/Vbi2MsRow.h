/*
 * Vbi2MsRow.h
 *
 *  Created on: Aug 22, 2013
 *      Author: jjacobs
 */

#ifndef VBI2MSROW_H_
#define VBI2MSROW_H_

#include <casacore/casa/Arrays/Array.h>
#include <msvis/MSVis/MsRows.h>

// Forward Decls

namespace casa {

namespace vi {

class VisBufferImpl2;

}

}

namespace casa {

namespace ms {

class CachedArrayBase {

public:

    CachedArrayBase () : cached_p (false) {}
    virtual ~CachedArrayBase () {}

    void clearCache () { cached_p = false;}

protected:

    casacore::Bool isCached () const { return cached_p;}
    void setCached () { cached_p = true;}

private:

    casacore::Bool cached_p;
};

template <typename T>
class CachedPlane : public CachedArrayBase {

public:

typedef const casacore::Cube<T> & (casa::vi::VisBufferImpl2::* Accessor) () const;

CachedPlane (Accessor accessor) : accessor_p (accessor) {}

casacore::Matrix<T> &
getCachedPlane (casa::vi::VisBufferImpl2 * vb, casacore::Int row)
{
    if (! isCached()){

        //cache_p.reference ((vb ->* accessor_p)().xyPlane (row)); // replace with something more efficient
        referenceMatrix (cache_p, (vb ->* accessor_p)(), row);
        setCached ();
    }

    return cache_p;
}

private:

    static void
    referenceMatrix (casacore::Matrix<T> & cache, const casacore::Cube<T> & src, casacore::Int row)
    {
        casacore::IPosition shape = src.shape ();
        shape.resize (2);

        // This is a bit sleazy but it seems to be helpful to performance.
        // Assumes contiguously stored cube.

        T * storage = const_cast <T *> (& src (casacore::IPosition (3, 0, 0, row)));

        cache.takeStorage (shape, storage, casacore::SHARE);
    }

    Accessor accessor_p;
    casacore::Matrix<T> cache_p;
};

template <typename T>
class CachedColumn : public CachedArrayBase {

public:

typedef const casacore::Matrix<T> & (casa::vi::VisBufferImpl2::* Accessor) () const;

CachedColumn (Accessor accessor) : accessor_p (accessor) {}

casacore::Vector<T> &
getCachedColumn (casa::vi::VisBufferImpl2 * vb, casacore::Int row)
{
    if (! isCached()){

        referenceVector (cache_p, (vb ->* accessor_p)(), row);
        setCached ();
    }

    return cache_p;
}

private:

    static void
    referenceVector (casacore::Vector<T> & cache, const casacore::Matrix<T> & src, casacore::Int row)
    {
        casacore::IPosition shape = src.shape ();
        shape.resize (1);

        // This is a bit sleazy but it seems to be helpful to performance.
        // Assumes contiguously stored cube.

        T * storage = const_cast <T *> (& src (casacore::IPosition (2, 0, row)));

        cache.takeStorage (shape, storage, casacore::SHARE);
    }

    Accessor accessor_p;
    casacore::Vector<T> cache_p;
};


class Vbi2MsRow : public MsRow {

public:

    // Constructors

    // Constructor for read-only access.
    // Attempt to write will throw exception.

    Vbi2MsRow (casacore::rownr_t row, const vi::VisBufferImpl2 * vb);

    // Constructor for read/write access

    Vbi2MsRow (casacore::rownr_t row, vi::VisBufferImpl2 * vb);

    virtual ~Vbi2MsRow () {}

    void changeRow (casacore::rownr_t row);
    void copy (Vbi2MsRow * other,
               const VisBufferComponents2 & componentsToCopy);

    casacore::Int antenna1 () const;
    casacore::Int antenna2 () const;
    casacore::Int arrayId () const;
    casacore::Int correlationType () const;
    casacore::Int dataDescriptionId () const;
    casacore::Int feed1 () const;
    casacore::Int feed2 () const;
    casacore::Int fieldId () const;
    casacore::Int observationId () const;
    casacore::Int rowId () const;
    casacore::Int processorId () const;
    casacore::Int scanNumber () const;
    casacore::Int stateId () const;
    casacore::Double exposure () const;
    casacore::Double interval () const;
    casacore::Int spectralWindow () const;
    casacore::Double time () const;
    casacore::Double timeCentroid () const;

    void setAntenna1 (casacore::Int);
    void setAntenna2 (casacore::Int);
    void setArrayId (casacore::Int);
    void setCorrelationType (casacore::Int);
    void setDataDescriptionId (casacore::Int);
    void setFeed1 (casacore::Int);
    void setFeed2 (casacore::Int);
    void setFieldId (casacore::Int);
    void setObservationId (casacore::Int);
    void setProcessorId (casacore::Int);
    void setRowId (casacore::Int);
    void setScanNumber (casacore::Int);
    void setStateId (casacore::Int);
    void setExposure (casacore::Double);
    void setInterval (casacore::Double);
    void setSpectralWindow (casacore::Int);
    void setTime (casacore::Double);
    void setTimeCentroid (casacore::Double);

    const casacore::Vector<casacore::Double> uvw () const;
    const casacore::Double & uvw (casacore::Int i) const;
    void setUvw (const casacore::Vector<casacore::Double> &);
    void setUvw (casacore::Int i, const casacore::Vector<casacore::Double> &);

    const casacore::Complex & corrected (casacore::Int correlation, casacore::Int channel) const;
    const casacore::Matrix<casacore::Complex> & corrected () const;
    casacore::Matrix<casacore::Complex> & correctedMutable ();
    void setCorrected (casacore::Int correlation, casacore::Int channel, const casacore::Complex & value);
    void setCorrected (const casacore::Matrix<casacore::Complex> & value);

    const casacore::Complex & model (casacore::Int correlation, casacore::Int channel) const;
    const casacore::Matrix<casacore::Complex> & model () const;
    casacore::Matrix<casacore::Complex> & modelMutable ();
    void setModel(casacore::Int correlation, casacore::Int channel, const casacore::Complex & value);
    void setModel (const casacore::Matrix<casacore::Complex> & value);

    const casacore::Complex & observed (casacore::Int correlation, casacore::Int channel) const;
    const casacore::Matrix<casacore::Complex> & observed () const;
    casacore::Matrix<casacore::Complex> & observedMutable ();
    void setObserved (casacore::Int correlation, casacore::Int channel, const casacore::Complex & value);
    void setObserved (const casacore::Matrix<casacore::Complex> & value);

    const casacore::Float & singleDishData (casacore::Int correlation, casacore::Int channel) const;
    const casacore::Matrix<casacore::Float> singleDishData () const;
    casacore::Matrix<casacore::Float> singleDishDataMutable ();
    void setSingleDishData (casacore::Int correlation, casacore::Int channel, const casacore::Float & value);
    void setSingleDishData (const casacore::Matrix<casacore::Float> & value);

    casacore::Float sigma (casacore::Int correlation) const;
    const casacore::Vector<casacore::Float> & sigma () const;
    casacore::Vector<casacore::Float> & sigmaMutable () const;
    void setSigma (casacore::Int correlation, casacore::Float value);
    void setSigma (const casacore::Vector<casacore::Float> & value);
    casacore::Float weight (casacore::Int correlation) const;
    const casacore::Vector<casacore::Float> & weight () const;
    casacore::Vector<casacore::Float> & weightMutable () const;
    void setWeight (casacore::Int correlation, casacore::Float value);
    void setWeight (const casacore::Vector<casacore::Float> & value);
    casacore::Float weightSpectrum (casacore::Int correlation, casacore::Int channel) const;
    void setWeightSpectrum (casacore::Int correlation, casacore::Int channel, casacore::Float value);
    void setWeightSpectrum (const casacore::Matrix<casacore::Float> & value);
    const casacore::Matrix<casacore::Float> & weightSpectrum () const;
    casacore::Matrix<casacore::Float> & weightSpectrumMutable () const;
    casacore::Float sigmaSpectrum (casacore::Int correlation, casacore::Int channel) const;
    const casacore::Matrix<casacore::Float> & sigmaSpectrum () const;
    casacore::Matrix<casacore::Float> & sigmaSpectrumMutable () const;
    void setSigmaSpectrum (casacore::Int correlation, casacore::Int channel, casacore::Float value);
    void setSigmaSpectrum (const casacore::Matrix<casacore::Float> & value);

    casacore::Bool isRowFlagged () const;
    const casacore::Matrix<casacore::Bool> & flags () const;
    void setFlags (const casacore::Matrix<casacore::Bool> & flags);
    casacore::Bool isFlagged (casacore::Int correlation, casacore::Int channel) const;

    void setRowFlag (casacore::Bool isFlagged);
    void setFlags (casacore::Bool isFlagged, casacore::Int correlation, casacore::Int channel);

protected:

    template <typename T>
    void addToCachedArrays (T & cachedArray)
    {
        arrayCaches_p.push_back (& cachedArray);
    }

    void clearArrayCaches();
    casacore::Matrix<casacore::Bool> & flagsMutable ();
    vi::VisBufferImpl2 * getVbi () const;

private:

    void configureArrayCaches(); // called in ctor so do not override

    mutable CachedPlane<casacore::Complex> correctedCache_p;
    mutable CachedPlane<casacore::Bool> flagCache_p;
    mutable CachedPlane<casacore::Complex> modelCache_p;
    mutable CachedPlane<casacore::Complex> observedCache_p;
    mutable CachedColumn<casacore::Float> sigmaCache_p;
    mutable CachedPlane<casacore::Float> sigmaSpectrumCache_p;
    mutable CachedColumn<casacore::Float> weightCache_p;
    mutable CachedPlane<casacore::Float> weightSpectrumCache_p;

    std::vector<CachedArrayBase *> arrayCaches_p;

template <typename T, typename U>
void
copyIf (casacore::Bool copyThis, Vbi2MsRow * other,
                   void (Vbi2MsRow::* setter) (T),
                   U (Vbi2MsRow::* getter) () const);

    vi::VisBufferImpl2 * vbi2_p;

};

}

} // end namespace casa


#endif /* VBI2MSROW_H_ */
