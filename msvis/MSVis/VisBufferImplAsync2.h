/*
 * VisibilityBufferAsync.h
 *
 *  Created on: Nov 3, 2010
 *      Author: jjacobs
 */

#ifndef VISIBILITYBUFFERASYNC2_H_
#define VISIBILITYBUFFERASYNC2_H_

#include <msvis/MSVis/VisBufferImpl2.h>

namespace casa {
namespace vi {

class VisibilityIteratorAsync2;
class VlaDatum;
class VLAT;

class MsIterInfo {

public:

    void fill (const casacore::MSIter & msIter);

    casacore::Bool allBeamOffsetsZero () const;
    casacore::Vector<casacore::String>  & antennaMounts () const;
    casacore::Int arrayId () const;
    casacore::Int dataDescriptionId () const;
    casacore::Int fieldId () const;
    casacore::String fieldName () const;
    const casacore::MFrequency & frequency0 () const;
    const casacore::Vector<casacore::Double> & getBeamOffsets () const;
    casacore::Bool more () const;
    casacore::Int msId () const;
    casacore::Bool newArray () const;
    casacore::Int newDataDescriptionId () const;
    casacore::Bool newField () const;
    casacore::Bool newMS () const;
    casacore::Bool newSpectralWindow () const;
    casacore::Int numMS () const;
    const casacore::MDirection & phaseCenter () const;
    casacore::Int polFrame () const;
    casacore::Int polarizationId () const;
    const casacore::Matrix<casacore::Double> & receptorAngle () const;
    const casacore::String & sourceName () const;
    casacore::Int spectralWindowId () const;
    const casacore::MPosition & telescopePosition () const;


private:

    casacore::Bool allBeamOffsetsZero_p;
    casacore::Vector<casacore::String>  antennaMounts_p;
    casacore::Int arrayId_p;
    casacore::Int dataDescriptionId_p;
    casacore::Int fieldId_p;
    casacore::String fieldName_p;
    casacore::MFrequency frequency0_p;
    casacore::Vector<casacore::Double> getBeamOffsets_p;
    casacore::Bool more_p;
    casacore::Int msId_p;
    casacore::Bool newArray_p;
    casacore::Int newDataDescriptionId_p;
    casacore::Bool newField_p;
    casacore::Bool newMS_p;
    casacore::Bool newSpectralWindow_p;
    casacore::Int numMS_p;
    casacore::MDirection phaseCenter_p;
    casacore::Int polFrame_p;
    casacore::Int polarizationId_p;
    casacore::Matrix<casacore::Double> receptorAngle_p;
    casacore::String sourceName_p;
    casacore::Int spectralWindowId_p;
    casacore::MPosition telescopePosition_p;

};

class VisBufferImplAsync2 : public VisBufferImpl2 {

    friend class Rovia_Test;
    friend class ViImplAsync2;
    friend class VisBufferAsync2Wrapper2;
    friend class VlaDatum;
    friend class VLAT;

public:

    //VisBufferAsync2 (const VisBuffer& vb);

    ~VisBufferImplAsync2 ();

    const MsIterInfo & msIterInfo () const;

protected:

    // The constructors are not public because creation should be performed
    // by a factory object (e.g., VisBufferAutoPtr).  The use of a factory
    // makes it possible to fine tune at run time whether a VisBuffer or a
    // VisBufferAsync2 is created.

    VisBufferImplAsync2 ();
    VisBufferImplAsync2 (VisibilityIterator2 & iter, casacore::Bool isWritable);

//    void attachToVisIter(VisibilityIterator2 & iter);
//    void checkVisIter (const char * func, const char * file, int line, const char * extra = "") const;
//    void clear ();
    void construct ();
//    virtual void copyAsyncValues (const VisBufferAsync2 & other);
//    virtual void copyCache (const VisBuffer2 & other, casacore::Bool force);
//    template<typename T> void copyVector (const casacore::Vector<T> & from, casacore::Vector<T> & to);
//    casacore::Vector<casacore::MDirection>& fillDirection1();
//    casacore::Vector<casacore::MDirection>& fillDirection2();
//    void fillFrom (const VisBufferAsync2 & other);
//    casacore::MDirection & fillPhaseCenter();
//    casacore::Bool getAllBeamOffsetsZero () const;
//    const casacore::Vector <casacore::String> & getAntennaMounts () const;
//    const casacore::Cube <casacore::RigidVector <casacore::Double, 2> > & getBeamOffsets () const;
//    const casacore::MeasurementSet & getMs () const;
//    casacore::Int getNSpw () const;
//    casacore::MDirection getPhaseCenter () const;
//    const casacore::Cube <casacore::Double> & getReceptorAngles () const;
//    void setAngleInfo (casacore::Bool allBeamOffsetsZero,
//                       const casacore::Vector<casacore::String> antennaMounts,
//                       casacore::Cube<casacore::RigidVector<casacore::Double, 2> > beamOffsets,
//                       const casacore::Cube<casacore::Double> & receptorAngles);
//    void initializeScalars ();
//    /////void setDataDescriptionId (casacore::Int id);
//    void setFilling (casacore::Bool isFilling);
//    void setLsrInfo (const casacore::Block <casacore::Int> & channelGroupNumber,
//                     const casacore::Block <casacore::Int> & channelIncrement,
//                     const casacore::Block <casacore::Int> & channelStart,
//                     const casacore::Block <casacore::Int> & channelWidth,
//                     const casacore::MPosition & observatoryPosition,
//                     const casacore::MDirection & phaseCenter,
//                     casacore::Bool velocitySelection);
//    void setMeasurementSet (const casacore::MeasurementSet & ms);
//    void setMeasurementSetId (casacore::Int id, bool isNew);
//    void setMEpoch (const casacore::MEpoch & mEpoch);
//    void setMSD (const casacore::MSDerivedValues & msd);
//    void setNAntennas (casacore::Int);
//    void setNCoh (casacore::Int);
//    void setNSpw (casacore::Int);
//    void setNewEntityFlags (bool newArrayId, bool newFieldId, bool newSpectralWindow);
//    void setPolarizationId (casacore::Int);
//    void setNRowChunk (casacore::Int);
//    void setReceptor0Angle (const casacore::Vector<casacore::Float> & receptor0Angle);
//    void setRowIds (const casacore::Vector<casacore::uInt> & rowIds);
//    void setSelectedNVisibilityChannels (const casacore::Vector<casacore::Int> & nVisibilityChannels);
//    void setSelectedSpectralWindows (const casacore::Vector<casacore::Int> & spectralWindows);
//    void setTopoFreqs (const casacore::Vector<casacore::Double> & lsrFreq, const casacore::Vector<casacore::Double> & selFreq_p);
//    void setVisibilityShape (const casacore::IPosition & pvisibilityShape);
//    void updateCoordInfo (const VisBuffer2 *, const casacore::Bool dirDependent=true);
//
//    static casacore::MDirection unsharedCopyDirection (const casacore::MDirection & direction);
//    static void unsharedCopyDirectionVector (casacore::Vector<casacore::MDirection> & direction);
//    static casacore::MEpoch unsharedCopyEpoch (const casacore::MEpoch & mEpoch);
//    static casacore::MPosition unsharedCopyPosition (const casacore::MPosition & position);

private:

//    casacore::Bool fillAllBeamOffsetsZero ();
//    casacore::Vector <casacore::String> fillAntennaMounts ();
//    casacore::Cube <casacore::RigidVector <casacore::Double, 2> > fillBeamOffsets ();
//    casacore::Cube <casacore::Double> fillReceptorAngles ();

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


//template<typename T>
//void VisBufferAsync2::copyVector (const casacore::Vector<T> & from, casacore::Vector<T> & to)
//{
//    // Make an independent copy of the vector.
//    // N.B.: the independence is only at the top casacore::Vector level
//    //       so any deep dependence is not undone
//
//    casacore::Vector<T> tmp = from;
//    to = tmp.copy();
//}


} // end namespace vi

} // end namespace casa


#endif /* VISIBILITYBUFFERASYNC_H_ */
