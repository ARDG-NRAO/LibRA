/*
 * VisibilityBufferAsync.h
 *
 *  Created on: Nov 3, 2010
 *      Author: jjacobs
 */

#ifndef VISIBILITYBUFFERASYNC_H_
#define VISIBILITYBUFFERASYNC_H_

#include <msvis/MSVis/VisBuffer.h>

namespace casa {

class ROVisibilityIteratorAsync;

namespace asyncio {
    class VlaDatum;
    class VLAT;
}

class VisBufferAsync : public VisBuffer {

    friend class Rovia_Test;
    friend class ViReadImplAsync;
    friend class VisBufferAsyncWrapper;
    friend class VisBufferAutoPtr;
    friend class asyncio::VlaDatum;
    friend class asyncio::VLAT;

public:

    //VisBufferAsync (const VisBuffer& vb);

    ~VisBufferAsync ();

    VisBufferAsync & operator= (const VisBufferAsync & other);

    virtual void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& spws, casacore::Vector<casacore::Int>& nvischan);
    virtual VisBufferAsync & assign (const VisBuffer & vb, casacore::Bool copy);
    virtual casacore::Vector<casacore::MDirection> azel(casacore::Double time) const;
    virtual casacore::MDirection azel0(casacore::Double time) const;
    virtual VisBuffer * clone () const;
    //virtual casacore::Int dataDescriptionId() const;
    virtual void detachFromVisIter ();
    virtual casacore::Vector<casacore::Float> feed_pa(casacore::Double time) const;
    virtual casacore::Double hourang(casacore::Double time) const;
    virtual void invalidate (); // This one is booby-trapped right now
    virtual void invalidateAsync (); // Use this one in async code
//    virtual casacore::Vector<casacore::Double> & lsrFrequency ();
//    virtual const casacore::Vector<casacore::Double> & lsrFrequency () const;
    virtual void lsrFrequency(const casacore::Int& spw, casacore::Vector<casacore::Double>& freq, casacore::Bool& convert) const;
    virtual const casacore::MSColumns& msColumns() const;
    casacore::Int msId () const;
    virtual casacore::Bool newArrayId () const;
    virtual casacore::Bool newFieldId () const;
    casacore::Bool newMS() const;
    virtual casacore::Bool newSpectralWindow () const;
    casacore::Int nRowChunk() const{
      return nRowChunk_p;
    }

    casacore::Int numberAnt () const;
    casacore::Int numberCoh () const;
    virtual casacore::Vector<casacore::Float> parang(casacore::Double time) const;
    virtual casacore::Float parang0(casacore::Double time) const;
    virtual casacore::Int polarizationId() const;
    virtual casacore::Vector<casacore::rownr_t>& rowIds(){throw(casacore::AipsError("rowIds() not implemented for VBA."));}
    virtual const casacore::Vector<casacore::rownr_t>& rowIds() const {throw(casacore::AipsError("rowIds() const not implemented for VBA."));}
    virtual void setCorrectedVisCube(casacore::Complex c);
    virtual void setCorrectedVisCube (const casacore::Cube<casacore::Complex> & vis);
    virtual void setModelVisCube(casacore::Complex c);
    virtual void setModelVisCube (const casacore::Cube<casacore::Complex> & vis);
    virtual void setModelVisCube (const casacore::Vector<casacore::Float> & stokes);
    virtual void setVisCube(casacore::Complex c);
    virtual void setVisCube (const casacore::Cube<casacore::Complex>& vis);

protected:

    // The constructors are not public because creation should be performed
    // by a factory object (e.g., VisBufferAutoPtr).  The use of a factory
    // makes it possible to fine tune at run time whether a VisBuffer or a
    // VisBufferAsync is created.

    VisBufferAsync ();
    VisBufferAsync (const VisBufferAsync & other);
    VisBufferAsync (ROVisibilityIterator & iter);

    void attachToVisIter(ROVisibilityIterator & iter);
    void checkVisIter (const char * func, const char * file, int line, const char * extra = "") const;
    void clear ();
    void construct ();
    virtual void copyAsyncValues (const VisBufferAsync & other);
    virtual void copyCache (const VisBuffer & other, casacore::Bool force);
    template<typename T> void copyVector (const casacore::Vector<T> & from, casacore::Vector<T> & to);
    casacore::Vector<casacore::MDirection>& fillDirection1();
    casacore::Vector<casacore::MDirection>& fillDirection2();
    void fillFrom (const VisBufferAsync & other);
    casacore::MDirection & fillPhaseCenter();
    casacore::Bool getAllBeamOffsetsZero () const;
    const casacore::Vector <casacore::String> & getAntennaMounts () const;
    const casacore::Cube <casacore::RigidVector <casacore::Double, 2> > & getBeamOffsets () const;
    const casacore::MeasurementSet & getMs () const;
    casacore::Int getNSpw () const;
    casacore::MDirection getPhaseCenter () const;
    const casacore::Cube <casacore::Double> & getReceptorAngles () const;
    void setAngleInfo (casacore::Bool allBeamOffsetsZero,
                       const casacore::Vector<casacore::String> antennaMounts,
                       casacore::Cube<casacore::RigidVector<casacore::Double, 2> > beamOffsets,
                       const casacore::Cube<casacore::Double> & receptorAngles);
    void initializeScalars ();
    /////void setDataDescriptionId (casacore::Int id);
    void setFilling (casacore::Bool isFilling);
    void setLsrInfo (const casacore::Block <casacore::Int> & channelGroupNumber,
                     const casacore::Block <casacore::Int> & channelIncrement,
                     const casacore::Block <casacore::Int> & channelStart,
                     const casacore::Block <casacore::Int> & channelWidth,
                     const casacore::MPosition & observatoryPosition,
                     const casacore::MDirection & phaseCenter,
                     casacore::Bool velocitySelection);
    void setMeasurementSet (const casacore::MeasurementSet & ms);
    void setMeasurementSetId (casacore::Int id, bool isNew);
    void setMEpoch (const casacore::MEpoch & mEpoch);
    void setMSD (const casacore::MSDerivedValues & msd);
    void setNAntennas (casacore::Int);
    void setNCoh (casacore::Int);
    void setNSpw (casacore::Int);
    void setNewEntityFlags (bool newArrayId, bool newFieldId, bool newSpectralWindow);
    void setPolarizationId (casacore::Int);
    void setNRowChunk (casacore::Int);
    void setReceptor0Angle (const casacore::Vector<casacore::Float> & receptor0Angle);
    void setRowIds (const casacore::Vector<casacore::rownr_t> & rowIds);
    void setSelectedNVisibilityChannels (const casacore::Vector<casacore::Int> & nVisibilityChannels);
    void setSelectedSpectralWindows (const casacore::Vector<casacore::Int> & spectralWindows);
    void setTopoFreqs (const casacore::Vector<casacore::Double> & lsrFreq, const casacore::Vector<casacore::Double> & selFreq_p);
    void setVisibilityShape (const casacore::IPosition & pvisibilityShape);
    void updateCoordInfo (const VisBuffer *, const casacore::Bool dirDependent=true);

    static casacore::MDirection unsharedCopyDirection (const casacore::MDirection & direction);
    static void unsharedCopyDirectionVector (casacore::Vector<casacore::MDirection> & direction);
    static casacore::MEpoch unsharedCopyEpoch (const casacore::MEpoch & mEpoch);
    static casacore::MPosition unsharedCopyPosition (const casacore::MPosition & position);

private:

    casacore::Bool fillAllBeamOffsetsZero ();
    casacore::Vector <casacore::String> fillAntennaMounts ();
    casacore::Cube <casacore::RigidVector <casacore::Double, 2> > fillBeamOffsets ();
    casacore::Cube <casacore::Double> fillReceptorAngles ();

    casacore::Bool                           allBeamOffsetsZero_p;
    casacore::Vector<casacore::String>                 antennaMounts_p;
    mutable casacore::Vector<casacore::MDirection>     azelCached_p;      // mutable because it is a cached value
    mutable casacore::Double                 azelCachedTime_p;  // mutable because it is a cached value
    casacore::Cube<casacore::RigidVector<casacore::Double, 2> >  beamOffsets_p;
    casacore::Block<casacore::Int>                     channelGroupNumber_p;
    casacore::Block<casacore::Int>                     channelIncrement_p;
    casacore::Block<casacore::Int>                     channelStart_p;
    casacore::Block<casacore::Int>                     channelWidth_p;
    //casacore::Int                            dataDescriptionId_p;
    mutable casacore::Vector<casacore::Float>          feedpaCached_p;      // mutable because it is a cached value
    mutable casacore::Double                 feedpaCachedTime_p;  // mutable because it is a cached value
    casacore::Bool                           isFilling_p;
    casacore::Vector<casacore::Double>                 lsrFrequency_p; // calculated by getTopoFreqs if velSelection_p
    casacore::MEpoch                         mEpoch_p;
    const casacore::MeasurementSet *         measurementSet_p;  // [use]
    mutable casacore::MSColumns *          msColumns_p; // [own]
    casacore::MSDerivedValues *              msd_p; // [own]
    casacore::Int                            nAntennas_p;
    casacore::Int                            nCoh_p;
    casacore::Bool                           newArrayId_p;
    casacore::Bool                           newFieldId_p;
    casacore::Bool                           newSpectralWindow_p;
    casacore::Int                            nRowChunk_p;
    casacore::Int                            nSpw_p;
    //const casacore::ScalarColumn<casacore::Int> *    obsMFreqTypes_p; // [use]
    casacore::MPosition                      observatoryPosition_p;
    mutable casacore::Vector<casacore::Float>          parangCached_p;      // mutable because it is a cached value
    mutable casacore::Double                 parangCachedTime_p;  // mutable because it is a cached value
    casacore::Int                            polarizationId_p;
    casacore::Vector<casacore::Float>                  receptor0Angle_p;
    casacore::Cube<casacore::Double>                   receptorAngles_p;
    casacore::Vector<casacore::Double>                 selFreq_p;
    casacore::Vector<casacore::Int>                    selectedNVisibilityChannels_p;
    casacore::Vector<casacore::Int>                    selectedSpectralWindows_p;
    casacore::Bool                           velSelection_p;
    casacore::IPosition                      visibilityShape_p;
};


template<typename T>
void VisBufferAsync::copyVector (const casacore::Vector<T> & from, casacore::Vector<T> & to)
{
    // Make an independent copy of the vector.
    // N.B.: the independence is only at the top casacore::Vector level
    //       so any deep dependence is not undone

    casacore::Vector<T> tmp = from;
    to = tmp.copy();
}


} // end namespace casa


#endif /* VISIBILITYBUFFERASYNC_H_ */
