#include "VisibilityIterator.h"
#include "VisibilityIteratorImpl.h"
#include "VisibilityIteratorImplAsync.h"
#include <stdcasa/UtilJ.h>
#include <casacore/casa/System/AipsrcValue.h>

//#include <measures/Measures/MDoppler.h>
//#include <measures/Measures/MRadialVelocity.h>


#define CheckImplementationPointerR() Assert (readImpl_p != NULL);
#define CheckImplementationPointerW() Assert (writeImpl_p != NULL);

using namespace casacore;
namespace casa {

namespace asyncio {

PrefetchColumns
PrefetchColumns::prefetchColumnsAll ()
{
    // Create the set of all columns

    PrefetchColumns pc;

    for (int i = 0; i < VisBufferComponents::N_VisBufferComponents; ++ i){
        pc.insert ((VisBufferComponents::EnumType) i);
    }

    return pc;
}

PrefetchColumns
PrefetchColumns::prefetchAllColumnsExcept (Int firstColumn, ...)
{
    // Build a set of Prefetch columns named except.
    // The last arg must be either negative or
    // greater than or equal to N_PrefetchColumnIds

    va_list vaList;

    va_start (vaList, firstColumn);

    int id = firstColumn;
    PrefetchColumns except;

    while (id >= 0 && id < VisBufferComponents::N_VisBufferComponents){
        except.insert ((VisBufferComponents::EnumType)id);
        id = va_arg (vaList, int);
    }

    va_end (vaList);

    // Get the set of all columns and then subtract off the
    // caller specified columns.  Return the result

    PrefetchColumns allColumns = prefetchColumnsAll();
    PrefetchColumns result;

    set_difference (allColumns.begin(), allColumns.end(),
                    except.begin(), except.end(),
                    inserter (result, result.begin()));

    return result;

}

String
PrefetchColumns::columnName (Int id)
{
    assert (id >= 0 && id < VisBufferComponents::N_VisBufferComponents);

    // This method is called only occasionally for debuggin so at this time a
    // brute-force implmentation is acceptable.

    map<Int,String> names;

    names [VisBufferComponents::Ant1] = "Ant1";
    names [VisBufferComponents::Ant2] = "Ant2";
    names [VisBufferComponents::ArrayId] = "ArrayId";
    names [VisBufferComponents::Channel] = "Channel";
    names [VisBufferComponents::Cjones] = "Cjones";
    names [VisBufferComponents::CorrType] = "CorrType";
    names [VisBufferComponents::Corrected] = "Corrected";
    names [VisBufferComponents::CorrectedCube] = "CorrectedCube";
    names [VisBufferComponents::Direction1] = "Direction1";
    names [VisBufferComponents::Direction2] = "Direction2";
    names [VisBufferComponents::Exposure] = "Exposure";
    names [VisBufferComponents::Feed1] = "Feed1";
    names [VisBufferComponents::Feed1_pa] = "Feed1_pa";
    names [VisBufferComponents::Feed2] = "Feed2";
    names [VisBufferComponents::Feed2_pa] = "Feed2_pa";
    names [VisBufferComponents::FieldId] = "FieldId";
    names [VisBufferComponents::Flag] = "Flag";
    names [VisBufferComponents::FlagCategory] = "FlagCategory";
    names [VisBufferComponents::FlagCube] = "FlagCube";
    names [VisBufferComponents::FlagRow] = "FlagRow";
    names [VisBufferComponents::Freq] = "Freq";
    names [VisBufferComponents::ImagingWeight] = "ImagingWeight";
    names [VisBufferComponents::Model] = "Model";
    names [VisBufferComponents::ModelCube] = "ModelCube";
    names [VisBufferComponents::NChannel] = "NChannel";
    names [VisBufferComponents::NCorr] = "NCorr";
    names [VisBufferComponents::NRow] = "NRow";
    names [VisBufferComponents::ObservationId] = "ObservationId";
    names [VisBufferComponents::Observed] = "Observed";
    names [VisBufferComponents::ObservedCube] = "ObservedCube";
    names [VisBufferComponents::PhaseCenter] = "PhaseCenter";
    names [VisBufferComponents::PolFrame] = "PolFrame";
    names [VisBufferComponents::ProcessorId] = "ProcessorId";
    names [VisBufferComponents::Scan] = "Scan";
    names [VisBufferComponents::Sigma] = "Sigma";
    names [VisBufferComponents::SigmaMat] = "SigmaMat";
    names [VisBufferComponents::SpW] = "SpW";
    names [VisBufferComponents::StateId] = "StateId";
    names [VisBufferComponents::Time] = "Time";
    names [VisBufferComponents::TimeCentroid] = "TimeCentroid";
    names [VisBufferComponents::TimeInterval] = "TimeInterval";
    names [VisBufferComponents::Weight] = "Weight";
    names [VisBufferComponents::WeightMat] = "WeightMat";
    names [VisBufferComponents::WeightSpectrum] = "WeightSpectrum";
    names [VisBufferComponents::Uvw] = "Uvw";
    names [VisBufferComponents::UvwMat] = "UvwMat";

    return names [id];
}


PrefetchColumns
PrefetchColumns::prefetchColumns (Int firstColumn, ...)
{
    // Returns a set of Prefetch columns.  The last arg must be either negative or
    // greater than or equal to N_PrefetchColumnIds

    va_list vaList;

    va_start (vaList, firstColumn);

    Int id = firstColumn;
    PrefetchColumns pc;

    while (id >= 0 && id < VisBufferComponents::N_VisBufferComponents){
        pc.insert ((VisBufferComponents::EnumType) id);
        id = va_arg (vaList, Int);
    }

    va_end (vaList);

    return pc;
}
};


ROVisibilityIterator::ROVisibilityIterator ()
: readImpl_p (NULL)
{}

ROVisibilityIterator::ROVisibilityIterator (const MeasurementSet& ms,
                                            const Block<Int>& sortColumns,
                                            Double timeInterval,
                                            const Factory & factory)
{
    construct (NULL, Block<MeasurementSet> (1, ms), sortColumns, true,
               timeInterval, false, factory);
}

ROVisibilityIterator::ROVisibilityIterator (const MeasurementSet& ms,
                                            const Block<Int>& sortColumns,
                                            const Bool addDefaultSortCols,
                                            Double timeInterval)
{
    construct (NULL, Block<MeasurementSet> (1, ms), sortColumns, addDefaultSortCols,
               timeInterval, false, Factory ());
}


ROVisibilityIterator::ROVisibilityIterator (const Block<MeasurementSet>& mss,
                                            const Block<Int>& sortColumns,
                                            Double timeInterval)
{
    construct (NULL, mss, sortColumns, true, timeInterval, false, Factory ());
}

ROVisibilityIterator::ROVisibilityIterator (const Block<MeasurementSet>& mss,
                                            const Block<Int>& sortColumns,
                                            const Bool addDefaultSortCols,
                                            Double timeInterval)
{
    construct (NULL, mss, sortColumns, addDefaultSortCols, timeInterval, false, Factory());
}

ROVisibilityIterator::ROVisibilityIterator(const asyncio::PrefetchColumns * prefetchColumns,
                                           const MeasurementSet& ms,
                                           const Block<Int>& sortColumns,
                                           const Bool addDefaultSortCols,
                                           Double timeInterval)
{
    construct (prefetchColumns, Block<MeasurementSet> (1, ms), sortColumns, addDefaultSortCols,
               timeInterval, false, Factory ());
}

ROVisibilityIterator::ROVisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                                            const Block<MeasurementSet>& mss,
                                            const Block<Int>& sortColumns,
                                            const Bool addDefaultSortCols,
                                            Double timeInterval)
{
    construct (prefetchColumns, mss, sortColumns, addDefaultSortCols, timeInterval, false, Factory());
}




ROVisibilityIterator::ROVisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                                            const Block<MeasurementSet>& mss,
                                            const Block<Int>& sortColumns,
                                            const Bool addDefaultSortCols,
                                            Double timeInterval,
                                            Bool writable)
{
    construct (prefetchColumns, mss, sortColumns, addDefaultSortCols, timeInterval, writable, Factory());
}


ROVisibilityIterator::ROVisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                                            const ROVisibilityIterator & other)
{
    // For potentially creating an asynchronous ROVI from a synchronous one

    Bool createAsAsynchronous = prefetchColumns != NULL && isAsynchronousIoEnabled ();

    if (createAsAsynchronous){

        Bool writable = dynamic_cast <const VisibilityIterator *> (& other) != NULL;

        readImpl_p = new ViReadImplAsync (* prefetchColumns, * other.readImpl_p, writable);
    }
    else{
        readImpl_p = new VisibilityIteratorReadImpl (* other.readImpl_p, this);
    }
}

ROVisibilityIterator::ROVisibilityIterator (const ROVisibilityIterator & other)
{
    if (other.readImpl_p == NULL){
        readImpl_p = NULL;
    }
    else {
        readImpl_p = other.readImpl_p -> clone(this);
    }
}


ROVisibilityIterator::~ROVisibilityIterator ()
{
    delete readImpl_p;
}

void
ROVisibilityIterator::construct (const asyncio::PrefetchColumns * prefetchColumns,
                                 const Block<MeasurementSet>& mss,
                                 const Block<Int>& sortColumns,
                                 const Bool addDefaultSortCols,
                                 Double timeInterval,
                                 Bool writable,
                                 const Factory & factory)
{
    readImpl_p = factory (prefetchColumns, mss, sortColumns, addDefaultSortCols, timeInterval);

    if (readImpl_p == NULL) {

        // Factory didn't create the read implementation so decide whether to create a
        // synchronous or asynchronous read implementation.

        Bool createAsAsynchronous = prefetchColumns != NULL && isAsynchronousIoEnabled ();

        if (createAsAsynchronous){
            readImpl_p = new ViReadImplAsync (mss, * prefetchColumns, sortColumns,
                                              addDefaultSortCols, timeInterval, writable);
        }
        else{
            readImpl_p = new VisibilityIteratorReadImpl (this, mss, sortColumns,
                                                         addDefaultSortCols, timeInterval);
        }
    }

    if (! writable){
        readImpl_p->originChunks();
    }
}

ROVisibilityIterator &
ROVisibilityIterator::operator= (const ROVisibilityIterator &other)
{
    // Kill off the current read implementation

    if (readImpl_p != NULL){
        delete readImpl_p;
        readImpl_p = NULL;
    }

    // If the other's read implementation is not null then
    // put a clone of it into this object's read implementation.

    if (other.readImpl_p != NULL){

        readImpl_p = other.readImpl_p->clone (this);
    }
    return * this;
}

ROVisibilityIterator &
ROVisibilityIterator::operator++ (int)
{
    advance ();

    return * this;
}

ROVisibilityIterator &
ROVisibilityIterator::operator++ ()
{
    advance ();

    return * this;
}

void
ROVisibilityIterator::advance ()
{
    CheckImplementationPointerR ();
    readImpl_p->advance ();
}

Bool
ROVisibilityIterator::allBeamOffsetsZero () const
{
    CheckImplementationPointerR ();
    return readImpl_p->allBeamOffsetsZero ();
}

void
ROVisibilityIterator::allSelectedSpectralWindows (Vector<Int>& spws, Vector<Int>& nvischan)
{
    CheckImplementationPointerR ();
    readImpl_p->allSelectedSpectralWindows (spws, nvischan);
}

Vector<Int>&
ROVisibilityIterator::antenna1 (Vector<Int>& ant1) const
{
    CheckImplementationPointerR ();
    return readImpl_p->antenna1 (ant1);
}

Vector<Int>&
ROVisibilityIterator::antenna2 (Vector<Int>& ant2) const
{
    CheckImplementationPointerR ();
    return readImpl_p->antenna2 (ant2);
}

const Vector<String>&
ROVisibilityIterator::antennaMounts () const
{
    CheckImplementationPointerR ();
    return readImpl_p->antennaMounts ();
}

const Block<Int>& ROVisibilityIterator::getSortColumns() const
{
  CheckImplementationPointerR();
  return readImpl_p->getSortColumns();
}

const MeasurementSet &
ROVisibilityIterator::getMeasurementSet () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getMeasurementSet ();
}


const MeasurementSet&
ROVisibilityIterator::ms () const
{
    CheckImplementationPointerR ();
    return readImpl_p->ms ();
}

Int
ROVisibilityIterator::arrayId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->arrayId ();
}


void
ROVisibilityIterator::attachColumns (const Table &t)
{
    CheckImplementationPointerR ();
    readImpl_p->attachColumns (t);
}

const Table
ROVisibilityIterator::attachTable () const
{
    CheckImplementationPointerR ();
    return readImpl_p->attachTable ();
}

void
ROVisibilityIterator::attachVisBuffer (VisBuffer& vb)
{
    CheckImplementationPointerR ();
    readImpl_p->attachVisBuffer (vb);
}

Vector<MDirection>
ROVisibilityIterator::azel (Double time) const
{
    CheckImplementationPointerR ();
    return readImpl_p->azel (time);
}

MDirection
ROVisibilityIterator::azel0 (Double time) const
{
    CheckImplementationPointerR ();
    return readImpl_p->azel0 (time);
}

void
ROVisibilityIterator::azelCalculate (Double time, MSDerivedValues & msd, Vector<MDirection> & azel,
                                     Int nAnt, const MEpoch & mEpoch0)
{
    VisibilityIteratorReadImpl::azelCalculate (time, msd, azel, nAnt, mEpoch0);
}

void
ROVisibilityIterator::azel0Calculate (Double time, MSDerivedValues & msd,
		                      MDirection & azel0, const MEpoch & mEpoch0)
{
    VisibilityIteratorReadImpl::azel0Calculate (time, msd, azel0, mEpoch0);
}

Vector<Int>&
ROVisibilityIterator::channel (Vector<Int>& chan) const
{
    CheckImplementationPointerR ();
    return readImpl_p->channel (chan);
}

Int
ROVisibilityIterator::channelGroupSize () const
{
    CheckImplementationPointerR ();
    return readImpl_p->channelGroupSize ();
}

Int
ROVisibilityIterator::channelIndex () const
{
    CheckImplementationPointerR ();
    return readImpl_p->channelIndex ();
}

Vector<SquareMatrix<Complex,2> >&
ROVisibilityIterator::CJones (Vector<SquareMatrix<Complex,2> >& cjones) const
{
    CheckImplementationPointerR ();
    return readImpl_p->CJones (cjones);
}


Vector<Int>&
ROVisibilityIterator::corrType (Vector<Int>& corrTypes) const
{
    CheckImplementationPointerR ();
    return readImpl_p->corrType (corrTypes);
}

Int
ROVisibilityIterator::dataDescriptionId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->dataDescriptionId ();
}

void
ROVisibilityIterator::detachVisBuffer (VisBuffer& vb)
{
    CheckImplementationPointerR ();
    readImpl_p->detachVisBuffer (vb);
}

void
ROVisibilityIterator::doChannelSelection ()
{
    CheckImplementationPointerR ();
    readImpl_p->doChannelSelection ();
}

Bool
ROVisibilityIterator::existsColumn (VisBufferComponents::EnumType id) const
{
    CheckImplementationPointerR ();

    return readImpl_p->existsColumn (id);
}

Bool ROVisibilityIterator::existsFlagCategory() const
{
  CheckImplementationPointerR ();
  return readImpl_p->existsFlagCategory();
}

Bool
ROVisibilityIterator::existsWeightSpectrum () const
{
    CheckImplementationPointerR ();
    return readImpl_p->existsWeightSpectrum ();
}

Vector<Double>&
ROVisibilityIterator::exposure (Vector<Double>& expo) const
{
    CheckImplementationPointerR ();
    return readImpl_p->exposure (expo);
}

Vector<Float>
ROVisibilityIterator::feed_paCalculate(Double time, MSDerivedValues & msd,
  									    Int nAntennas, const MEpoch & mEpoch0,
									    const Vector<Float> & receptor0Angle)
{
    return VisibilityIteratorReadImpl::feed_paCalculate (time, msd, nAntennas, mEpoch0, receptor0Angle);
}



Vector<Int>&
ROVisibilityIterator::feed1 (Vector<Int>& fd1) const
{
    CheckImplementationPointerR ();
    return readImpl_p->feed1 (fd1);
}

Vector<Int>&
ROVisibilityIterator::feed2 (Vector<Int>& fd2) const
{
    CheckImplementationPointerR ();
    return readImpl_p->feed2 (fd2);
}

Vector<Float>
ROVisibilityIterator::feed_pa (Double time) const
{
    CheckImplementationPointerR ();
    return readImpl_p->feed_pa (time);
}

Int
ROVisibilityIterator::fieldId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->fieldId ();
}

String
ROVisibilityIterator::fieldName () const
{
    CheckImplementationPointerR ();
    return readImpl_p->fieldName ();
}

Cube<Bool>&
ROVisibilityIterator::flag (Cube<Bool>& flags) const
{
    CheckImplementationPointerR ();
    return readImpl_p->flag (flags);
}

Matrix<Bool>&
ROVisibilityIterator::flag (Matrix<Bool>& flags) const
{
    CheckImplementationPointerR ();
    return readImpl_p->flag (flags);
}

Array<Bool>&
ROVisibilityIterator::flagCategory (Array<Bool>& flagCategories) const
{
    CheckImplementationPointerR ();
    return readImpl_p->flagCategory (flagCategories);
}

Vector<Bool>&
ROVisibilityIterator::flagRow (Vector<Bool>& rowflags) const
{
    CheckImplementationPointerR ();
    return readImpl_p->flagRow (rowflags);
}

Cube<Float>&
ROVisibilityIterator::floatData (Cube<Float>& fcube) const
{
    CheckImplementationPointerR ();
    return readImpl_p->floatData (fcube);
}

Vector<Double>&
ROVisibilityIterator::frequency (Vector<Double>& freq) const
{
    CheckImplementationPointerR ();
    return readImpl_p->frequency (freq);
}

const Cube<RigidVector<Double, 2> >&
ROVisibilityIterator::getBeamOffsets () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getBeamOffsets ();
}

//ArrayColumn
//<Double> &
//ROVisibilityIterator::getChannelFrequency () const
//{
//    CheckImplementationPointerR ();
//    return readImpl_p->getChannelFrequency ();
//}
//
//Block<Int>
//ROVisibilityIterator::getChannelGroupNumber () const
//{
//    CheckImplementationPointerR ();
//    return readImpl_p->getChannelGroupNumber ();
//}
//
//Block<Int>
//ROVisibilityIterator::getChannelIncrement () const
//{
//    CheckImplementationPointerR ();
//    return readImpl_p->getChannelIncrement ();
//}

void
ROVisibilityIterator::getChannelSelection (Block< Vector<Int> >& blockNGroup,
                                           Block< Vector<Int> >& blockStart,
                                           Block< Vector<Int> >& blockWidth,
                                           Block< Vector<Int> >& blockIncr,
                                           Block< Vector<Int> >& blockSpw)
{
    CheckImplementationPointerR ();
    readImpl_p->getChannelSelection (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);
}

//Block<Int>
//ROVisibilityIterator::getChannelStart () const
//{
//    CheckImplementationPointerR ();
//    return readImpl_p->getChannelStart ();
//}
//
//Block<Int>
//ROVisibilityIterator::getChannelWidth () const
//{
//    CheckImplementationPointerR ();
//    return readImpl_p->getChannelWidth ();
//}

void
ROVisibilityIterator::getCol (const ScalarColumn<Bool> &column, Vector<Bool> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, array, resize);
}

void
ROVisibilityIterator::getCol (const ScalarColumn<Int> &column, Vector<Int> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, array, resize);
}

void
ROVisibilityIterator::getCol (const ScalarColumn<Double> &column, Vector<Double> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, array, resize);
}

void
ROVisibilityIterator::getCol (const ArrayColumn<Bool> &column, Array<Bool> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, array, resize);
}

void
ROVisibilityIterator::getCol (const ArrayColumn<Float> &column, Array<Float> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, array, resize);
}

void
ROVisibilityIterator::getCol (const ArrayColumn<Double> &column, Array<Double> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, array, resize);
}

void
ROVisibilityIterator::getCol (const ArrayColumn<Complex> &column, Array<Complex> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, array, resize);
}

void
ROVisibilityIterator::getCol (const ArrayColumn<Bool> &column, const Slicer &slicer, Array<Bool> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, slicer, array, resize);
}

void
ROVisibilityIterator::getCol (const ArrayColumn<Float> &column, const Slicer &slicer, Array<Float> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, slicer, array, resize);
}

void
ROVisibilityIterator::getCol (const ArrayColumn<Complex> &column, const Slicer &slicer, Array<Complex> &array, Bool resize ) const
{
    CheckImplementationPointerR ();
    readImpl_p->getCol (column, slicer, array, resize);
}

void
ROVisibilityIterator::getDataColumn (DataColumn whichOne, const Slicer& slicer,
                                     Cube<Complex>& data) const
{
    CheckImplementationPointerR ();
    readImpl_p->getDataColumn (whichOne, slicer, data);
}

void
ROVisibilityIterator::getDataColumn (DataColumn whichOne, Cube<Complex>& data) const
{
    CheckImplementationPointerR ();
    readImpl_p->getDataColumn (whichOne, data);
}

Int
ROVisibilityIterator::getDataDescriptionId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getDataDescriptionId ();
}

void
ROVisibilityIterator::getFloatDataColumn (const Slicer& slicer, Cube<Float>& data) const
{
    CheckImplementationPointerR ();
    readImpl_p->getFloatDataColumn (slicer, data);
}

void
ROVisibilityIterator::getFloatDataColumn (Cube<Float>& data) const
{
    CheckImplementationPointerR ();
    readImpl_p->getFloatDataColumn (data);
}

//void
//ROVisibilityIterator::getInterpolatedFloatDataFlagWeight () const
//{
//    CheckImplementationPointerR ();
//    readImpl_p->getInterpolatedFloatDataFlagWeight ();
//}

//void
//ROVisibilityIterator::getInterpolatedVisFlagWeight (DataColumn whichOne) const
//{
//    CheckImplementationPointerR ();
//    readImpl_p->getInterpolatedVisFlagWeight (whichOne);
//}

void
ROVisibilityIterator::getLsrInfo (Block<Int> & channelGroupNumber,
                                  Block<Int> & channelIncrement,
                                  Block<Int> & channelStart,
                                  Block<Int> & channelWidth,
                                  MPosition & observatoryPositon,
                                  MDirection & phaseCenter,
                                  Bool & velocitySelection) const
{
    CheckImplementationPointerR ();
    readImpl_p->getLsrInfo (channelGroupNumber, channelIncrement, channelStart, channelWidth, observatoryPositon, phaseCenter, velocitySelection);
}

MEpoch
ROVisibilityIterator::getEpoch () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getEpoch ();
}

const MSDerivedValues &
ROVisibilityIterator::getMSD () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getMSD ();
}

Int
ROVisibilityIterator::getMeasurementSetId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getMeasurementSetId ();
}

vector<MeasurementSet>
ROVisibilityIterator::getMeasurementSets () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getMeasurementSets ();
}

Int
ROVisibilityIterator::getNAntennas () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getNAntennas ();
}

//MFrequency::Types
//ROVisibilityIterator::getObservatoryFrequencyType () const
//{
//    CheckImplementationPointerR ();
//    return readImpl_p->getObservatoryFrequencyType ();
//}

//MPosition
//ROVisibilityIterator::getObservatoryPosition () const
//{
//    CheckImplementationPointerR ();
//
//    return readImpl_p->getObservatoryPosition ();
//}

//MDirection
//ROVisibilityIterator::getPhaseCenter () const
//{
//    CheckImplementationPointerR ();
//
//    return readImpl_p->getPhaseCenter ();
//}

VisibilityIteratorReadImpl *
ROVisibilityIterator::getReadImpl () const
{
    return readImpl_p;
}

void
ROVisibilityIterator::getSpwInFreqRange (Block<Vector<Int> >& spw,
                                         Block<Vector<Int> >& start,
                                         Block<Vector<Int> >& nchan,
                                         Double freqStart,
                                         Double freqEnd,
                                         Double freqStep,
                                         MFrequency::Types freqFrame)
{
    CheckImplementationPointerR ();
    readImpl_p->getSpwInFreqRange (spw, start, nchan, freqStart, freqEnd, freqStep, freqFrame);
}

void ROVisibilityIterator::getFreqInSpwRange(Double& freqStart, Double& freqEnd, MFrequency::Types freqframe){
  CheckImplementationPointerR ();
  readImpl_p->getFreqInSpwRange(freqStart, freqEnd, freqframe);
}


SubChunkPair
ROVisibilityIterator::getSubchunkId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getSubchunkId ();
}

void
ROVisibilityIterator::getTopoFreqs ()
{
    CheckImplementationPointerR ();
    readImpl_p->getTopoFreqs ();
}

void
ROVisibilityIterator::getTopoFreqs (Vector<Double> & lsrFreq, Vector<Double> & selFreq)
{
    CheckImplementationPointerR ();
    readImpl_p->getTopoFreqs (lsrFreq, selFreq);
}

VisBuffer *
ROVisibilityIterator::getVisBuffer ()
{
    CheckImplementationPointerR ();
    return readImpl_p->getVisBuffer ();
}

Double
ROVisibilityIterator::hourang (Double time) const
{
    CheckImplementationPointerR ();
    return readImpl_p->hourang (time);
}

Double
ROVisibilityIterator::hourangCalculate (Double time, MSDerivedValues & msd, const MEpoch & mEpoch0)
{
    return VisibilityIteratorReadImpl::hourangCalculate (time, msd, mEpoch0);
}

//Matrix<Float>&
//ROVisibilityIterator::imagingWeight (Matrix<Float>& wt) const
//{
//    CheckImplementationPointerR ();
//    return readImpl_p->imagingWeight (wt);
//}

const VisImagingWeight &
ROVisibilityIterator::getImagingWeightGenerator () const
{
    CheckImplementationPointerR ();
    return readImpl_p->getImagingWeightGenerator ();
}


//Bool
//ROVisibilityIterator::isAsyncEnabled () const
//{
//    CheckImplementationPointerR ();
//
//    return readImpl_p->isAsyncEnabled ();
//}


Bool
ROVisibilityIterator::isAsynchronous () const
{
    Bool isAsync = readImpl_p != NULL && dynamic_cast<const ViReadImplAsync *> (readImpl_p) != NULL;

    return isAsync;
}


Bool
ROVisibilityIterator::isAsynchronousIoEnabled()
{
    // Determines whether asynchronous I/O is enabled by looking for the
    // expected AipsRc value.  If not found then async i/o is disabled.

    Bool isEnabled;
    AipsrcValue<Bool>::find (isEnabled, getAsyncRcBase () + ".enabled", false);

    return isEnabled;
}

Bool
ROVisibilityIterator::isInSelectedSPW (const Int& spw)
{
    CheckImplementationPointerR ();
    return readImpl_p->isInSelectedSPW (spw);
}

Bool
ROVisibilityIterator::isWritable () const
{
    CheckImplementationPointerR ();
    return readImpl_p->isWritable ();
}

Vector<Double>&
ROVisibilityIterator::lsrFrequency (Vector<Double>& freq) const
{
    CheckImplementationPointerR ();
    return readImpl_p->lsrFrequency (freq);
}

void
ROVisibilityIterator::lsrFrequency (const Int& spw, Vector<Double>& freq, Bool& convert, const Bool ignoreconv)
{
    CheckImplementationPointerR ();
    readImpl_p->lsrFrequency (spw, freq, convert, ignoreconv);
}

void
ROVisibilityIterator::lsrFrequency (const Int& spw,
                            Vector<Double>& freq,
                            Bool & convert,
                            const Block<Int> & chanStart,
                            const Block<Int> & chanWidth,
                            const Block<Int> & chanInc,
                            const Block<Int> & numChanGroup,
                            const ArrayColumn <Double> & chanFreqs,
                            const ScalarColumn<Int> & obsMFreqTypes,
                            const MEpoch & ep,
                            const MPosition & obsPos,
                            const MDirection & dir)
{
    VisibilityIteratorReadImpl::lsrFrequency (spw, freq, convert, chanStart, chanWidth, chanInc,
                                              numChanGroup, chanFreqs, obsMFreqTypes, ep, obsPos, dir);
}


Bool
ROVisibilityIterator::more () const
{
    CheckImplementationPointerR ();
    return readImpl_p->more ();
}

Bool
ROVisibilityIterator::moreChunks () const
{
    CheckImplementationPointerR ();
    return readImpl_p->moreChunks ();
}

const MSColumns &
ROVisibilityIterator::msColumns () const
{
    CheckImplementationPointerR ();
    return readImpl_p->msColumns ();
}

Int
ROVisibilityIterator::msId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->msId ();
}

Int
ROVisibilityIterator::nCorr () const
{
    CheckImplementationPointerR ();
    return readImpl_p->nCorr ();
}

Int
ROVisibilityIterator::nRow () const
{
    CheckImplementationPointerR ();
    return readImpl_p->nRow ();
}

Int
ROVisibilityIterator::nRowChunk () const
{
    CheckImplementationPointerR ();
    return readImpl_p->nRowChunk ();
}

Int
ROVisibilityIterator::nSubInterval () const
{
    CheckImplementationPointerR ();
    return readImpl_p->nSubInterval ();
}

Bool
ROVisibilityIterator::newArrayId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->newArrayId ();
}

Bool
ROVisibilityIterator::newFieldId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->newFieldId ();
}

Bool
ROVisibilityIterator::newMS () const
{
    CheckImplementationPointerR ();
    return readImpl_p->newMS ();
}

Bool
ROVisibilityIterator::newSpectralWindow () const
{
    CheckImplementationPointerR ();
    return readImpl_p->newSpectralWindow ();
}

ROVisibilityIterator&
ROVisibilityIterator::nextChunk ()
{
    CheckImplementationPointerR ();
    readImpl_p->nextChunk ();

    return * this;
}

Int
ROVisibilityIterator::numberAnt ()
{
    CheckImplementationPointerR ();
    return readImpl_p->numberAnt ();
}

Int
ROVisibilityIterator::numberCoh ()
{
    CheckImplementationPointerR ();
    return readImpl_p->numberCoh ();
}

Int
ROVisibilityIterator::numberDDId ()
{
    CheckImplementationPointerR ();
    return readImpl_p->numberDDId ();
}

Int
ROVisibilityIterator::numberPol ()
{
    CheckImplementationPointerR ();
    return readImpl_p->numberPol ();
}

Int
ROVisibilityIterator::numberSpw ()
{
    CheckImplementationPointerR ();
    return readImpl_p->numberSpw ();
}

Vector<Int>&
ROVisibilityIterator::observationId (Vector<Int>& obsids) const
{
    CheckImplementationPointerR ();
    return readImpl_p->observationId (obsids);
}

void
ROVisibilityIterator::origin ()
{
    CheckImplementationPointerR ();
    readImpl_p->origin ();
}

void
ROVisibilityIterator::originChunks ()
{
    CheckImplementationPointerR ();
    readImpl_p->originChunks ();
}

void
ROVisibilityIterator::originChunks (Bool forceRewind)
{
    CheckImplementationPointerR ();
    readImpl_p->originChunks (forceRewind);
}

Vector<Float>
ROVisibilityIterator::parang (Double time) const
{
    CheckImplementationPointerR ();
    return readImpl_p->parang (time);
}

const Float&
ROVisibilityIterator::parang0 (Double time) const
{
    CheckImplementationPointerR ();
    return readImpl_p->parang0 (time);
}


Float
ROVisibilityIterator::parang0Calculate (Double time, MSDerivedValues & msd, const MEpoch & epoch0)
{
    return VisibilityIteratorReadImpl::parang0Calculate (time, msd, epoch0);
}

Vector<Float>
ROVisibilityIterator::parangCalculate (Double time, MSDerivedValues & msd,
                                       int nAntennas, const MEpoch mEpoch0)
{
    return VisibilityIteratorReadImpl::parangCalculate (time, msd, nAntennas, mEpoch0);
}

const MDirection&
ROVisibilityIterator::phaseCenter () const
{
    CheckImplementationPointerR ();
    return readImpl_p->phaseCenter ();
}
const MDirection
ROVisibilityIterator::phaseCenter (const Int fieldid, const Double time) const
{
    CheckImplementationPointerR ();
    return readImpl_p->phaseCenter (fieldid, time);
}
Int
ROVisibilityIterator::polFrame () const
{
    CheckImplementationPointerR ();
    return readImpl_p->polFrame ();
}

Int
ROVisibilityIterator::polarizationId () const
{
    CheckImplementationPointerR ();
    return readImpl_p->polarizationId ();
}

Vector<Int>&
ROVisibilityIterator::processorId (Vector<Int>& procids) const
{
    CheckImplementationPointerR ();
    return readImpl_p->processorId (procids);
}

const Cube<Double>&
ROVisibilityIterator::receptorAngles () const
{
    CheckImplementationPointerR ();
    return readImpl_p->receptorAngles ();
}

Vector<Float>
ROVisibilityIterator::getReceptor0Angle ()
{
    CheckImplementationPointerR ();
    return readImpl_p->getReceptor0Angle();
}

//Vector<uInt> getRowIds ();

Vector<rownr_t>&
ROVisibilityIterator::rowIds (Vector<rownr_t>& rowids) const
{
    CheckImplementationPointerR ();
    return readImpl_p->rowIds (rowids);
}

Vector<rownr_t>
ROVisibilityIterator::getRowIds() const
{
    CheckImplementationPointerR();
    return readImpl_p->getRowIds();
}

Vector<Int>&
ROVisibilityIterator::scan (Vector<Int>& scans) const
{
    CheckImplementationPointerR ();
    return readImpl_p->scan (scans);
}

ROVisibilityIterator&
ROVisibilityIterator::selectChannel (Int nGroup, 
                                     Int start,
                                     Int width,
                                     Int increment,
                                     Int spectralWindow)
{
    CheckImplementationPointerR ();
    readImpl_p->selectChannel (nGroup, start, width, increment, spectralWindow);
    readImpl_p->originChunks ();

    return * this;
}

ROVisibilityIterator&
ROVisibilityIterator::selectChannel (Block< Vector<Int> >& blockNGroup,
                                     Block< Vector<Int> >& blockStart,
                                     Block< Vector<Int> >& blockWidth,
                                     Block< Vector<Int> >& blockIncr,
                                     Block< Vector<Int> >& blockSpw)
{
    CheckImplementationPointerR ();
    readImpl_p->selectChannel (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);

    return * this;
}

ROVisibilityIterator&
ROVisibilityIterator::selectVelocity (Int nChan,
                                      const MVRadialVelocity& vStart,
                                      const MVRadialVelocity& vInc,
                                      MRadialVelocity::Types rvType,
                                      MDoppler::Types dType,
                                      Bool precise)
{
    CheckImplementationPointerR ();

    readImpl_p->selectVelocity (nChan, vStart, vInc, rvType, dType, precise);

    return * this;
}

void
ROVisibilityIterator::setAsyncEnabled (Bool enable)
{
    CheckImplementationPointerR ();
    readImpl_p->setAsyncEnabled (enable);
}

Double ROVisibilityIterator::getInterval() const
{
    CheckImplementationPointerR ();
    return readImpl_p->getInterval();
}

void
ROVisibilityIterator::setInterval (Double timeInterval)
{
    CheckImplementationPointerR ();
    readImpl_p->setInterval (timeInterval);
}

void
ROVisibilityIterator::setRowBlocking (Int nRows) // for use by Async I/O *ONLY
{
    CheckImplementationPointerR ();
    readImpl_p->setRowBlocking (nRows);
}

void
ROVisibilityIterator::setSelTable ()
{
    CheckImplementationPointerR ();
    readImpl_p->setSelTable ();
}

void
ROVisibilityIterator::setState ()
{
    CheckImplementationPointerR ();
    readImpl_p->setState ();
}

void
ROVisibilityIterator::setTileCache ()
{
    CheckImplementationPointerR ();
    readImpl_p->setTileCache ();
}

Vector<Float>&
ROVisibilityIterator::sigma (Vector<Float>& sig) const
{
    CheckImplementationPointerR ();
    return readImpl_p->sigma (sig);
}

Matrix<Float>&
ROVisibilityIterator::sigmaMat (Matrix<Float>& sigmat) const
{
    CheckImplementationPointerR ();
    return readImpl_p->sigmaMat (sigmat);
}

void
ROVisibilityIterator::slicesToMatrices (Vector<Matrix<Int> >& matv,
                                        const Vector<Vector<Slice> >& slicesv,
                                        const Vector<Int>& widthsv) const
{
    CheckImplementationPointerR ();
    readImpl_p->slicesToMatrices (matv, slicesv, widthsv);
}

void
ROVisibilityIterator::slurp () const
{
    CheckImplementationPointerR ();
    readImpl_p->slurp ();
}

String
ROVisibilityIterator::sourceName () const
{
    CheckImplementationPointerR ();
    return readImpl_p->sourceName ();
}

Int
ROVisibilityIterator::spectralWindow () const
{
    CheckImplementationPointerR ();
    return readImpl_p->spectralWindow ();
}

Vector<Int>&
ROVisibilityIterator::stateId (Vector<Int>& stateids) const
{
    CheckImplementationPointerR ();
    return readImpl_p->stateId (stateids);
}

Vector<Double>&
ROVisibilityIterator::time (Vector<Double>& t) const
{
    CheckImplementationPointerR ();
    return readImpl_p->time (t);
}

Vector<Double>&
ROVisibilityIterator::timeCentroid (Vector<Double>& t) const
{
    CheckImplementationPointerR ();
    return readImpl_p->timeCentroid (t);
}

Vector<Double>&
ROVisibilityIterator::timeInterval (Vector<Double>& ti) const
{
    CheckImplementationPointerR ();
    return readImpl_p->timeInterval (ti);
}

void
ROVisibilityIterator::updateSlicer ()
{
    CheckImplementationPointerR ();
    readImpl_p->updateSlicer ();
}

void
ROVisibilityIterator::update_rowIds () const
{
    CheckImplementationPointerR ();
    readImpl_p->update_rowIds ();
}

void
ROVisibilityIterator::useImagingWeight (const VisImagingWeight& imWgt)
{
    CheckImplementationPointerR ();
    readImpl_p->useImagingWeight (imWgt);
}

Vector<RigidVector<Double, 3> >&
ROVisibilityIterator::uvw (Vector<RigidVector<Double, 3> >& uvwvec) const
{
    CheckImplementationPointerR ();
    return readImpl_p->uvw (uvwvec);
}

Matrix<Double>&
ROVisibilityIterator::uvwMat (Matrix<Double>& uvwmat) const
{
    CheckImplementationPointerR ();
    return readImpl_p->uvwMat (uvwmat);
}

ROVisibilityIterator&
ROVisibilityIterator::velInterpolation (const String& type)
{
    CheckImplementationPointerR ();

    readImpl_p->velInterpolation (type);

    return * this;
}

Cube<Complex>&
ROVisibilityIterator::visibility (Cube<Complex>& vis,
                                  DataColumn whichOne) const
{
    CheckImplementationPointerR ();
    return readImpl_p->visibility (vis, whichOne);
}

Matrix<CStokesVector>&
ROVisibilityIterator::visibility (Matrix<CStokesVector>& vis,
                                  DataColumn whichOne) const
{
    CheckImplementationPointerR ();
    return readImpl_p->visibility (vis, whichOne);
}

IPosition
ROVisibilityIterator::visibilityShape () const
{
    CheckImplementationPointerR ();
    return readImpl_p->visibilityShape ();
}

Vector<Float>&
ROVisibilityIterator::weight (Vector<Float>& wt) const
{
    CheckImplementationPointerR ();
    return readImpl_p->weight (wt);
}

Matrix<Float>&
ROVisibilityIterator::weightMat (Matrix<Float>& wtmat) const
{
    CheckImplementationPointerR ();
    return readImpl_p->weightMat (wtmat);
}

Cube<Float>&
ROVisibilityIterator::weightSpectrum (Cube<Float>& wtsp) const
{
    CheckImplementationPointerR ();
    return readImpl_p->weightSpectrum (wtsp);
}



VisibilityIterator::VisibilityIterator ()
: ROVisibilityIterator (),
  writeImpl_p (NULL)
{}

VisibilityIterator::VisibilityIterator (MeasurementSet & ms,
                                        const Block<Int>& sortColumns,
                                        Double timeInterval)
: ROVisibilityIterator (NULL, Block<MeasurementSet> (1, ms), sortColumns,
                        true, timeInterval, true)

{
    construct ();
}

VisibilityIterator::VisibilityIterator (MeasurementSet & ms,
                                        const Block<Int>& sortColumns,
                                        const Bool addDefaultSortCols,
                                        Double timeInterval)
: ROVisibilityIterator (NULL, Block<MeasurementSet> (1, ms), sortColumns,
                        addDefaultSortCols, timeInterval, true)
{
    construct ();
}

VisibilityIterator::VisibilityIterator (const Block<MeasurementSet>& mss,
                                        const Block<Int>& sortColumns,
                                        Double timeInterval)
: ROVisibilityIterator (NULL, mss, sortColumns, true, timeInterval, true)
{
    construct ();
}

VisibilityIterator::VisibilityIterator (const Block<MeasurementSet>& mss,
                                        const Block<Int>& sortColumns,
                                        const Bool addDefaultSortCols,
                                        Double timeInterval)
: ROVisibilityIterator (NULL, mss, sortColumns, addDefaultSortCols, timeInterval, true)
{
    construct ();
}

VisibilityIterator::VisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                                        MeasurementSet & ms,
                                        const Block<Int>& sortColumns,
                                        const Bool addDefaultSortCols,
                                        Double timeInterval)
: ROVisibilityIterator (prefetchColumns, Block<MeasurementSet> (1, ms), sortColumns,
                        addDefaultSortCols, timeInterval, true)
{
    construct ();
}


VisibilityIterator::VisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                                        const Block<MeasurementSet>& mss,
                                        const Block<Int>& sortColumns,
                                        const Bool addDefaultSortCols,
                                        Double timeInterval)
: ROVisibilityIterator (prefetchColumns, mss, sortColumns,
                        addDefaultSortCols, timeInterval, true)
{
    construct ();
}


VisibilityIterator::VisibilityIterator (const VisibilityIterator & other)
: ROVisibilityIterator (other)
{
    if (other.writeImpl_p == NULL){
        writeImpl_p = NULL;
    }
    else{
        writeImpl_p = other.writeImpl_p->clone(this);
    }
}

VisibilityIterator::VisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                                        const VisibilityIterator & other)
: ROVisibilityIterator (prefetchColumns, other)
{
    Bool createAsAsynchronous = prefetchColumns != NULL && isAsynchronousIoEnabled ();

    if (createAsAsynchronous){
        writeImpl_p = new ViWriteImplAsync (* prefetchColumns, * other.writeImpl_p, this);
    }
    else{
        writeImpl_p = other.writeImpl_p -> clone(this);
    }
}


VisibilityIterator::~VisibilityIterator ()
{
    delete writeImpl_p;
}

VisibilityIterator &
VisibilityIterator::operator= (const VisibilityIterator & other)
{
    // Let the superclass handle its part of the assignment

    ROVisibilityIterator::operator= (other);

    // Kill off the current write implementation

    if (writeImpl_p != NULL){
        delete writeImpl_p;
        writeImpl_p = NULL;
    }

    // If the other's read implementation is not null then
    // put a clone of it into this object's read implementation.

    if (other.writeImpl_p != NULL){

        writeImpl_p = other.writeImpl_p->clone (this);
    }
    return * this;
}

VisibilityIterator &
VisibilityIterator::operator++ (int)
{
    advance ();

    return * this;
}

VisibilityIterator &
VisibilityIterator::operator++ ()
{
    advance();

    return * this;
}

void
VisibilityIterator::attachColumns (const Table &t)
{
    CheckImplementationPointerW ();

    writeImpl_p->attachColumns (t);
}

void
VisibilityIterator::construct ()
{
    if (isAsynchronous ()){
        writeImpl_p = new ViWriteImplAsync (this);
    }
    else{
        writeImpl_p = new VisibilityIteratorWriteImpl (this);
    }

    /////////////////  readImpl_p->originChunks();
}

VisibilityIteratorWriteImpl *
VisibilityIterator::getWriteImpl () const
{
    return writeImpl_p;
}

Bool
VisibilityIterator::isWritable () const
{
    return true;
}

void
VisibilityIterator::setFlag (const Matrix<Bool>& flag)
{
    CheckImplementationPointerW ();
    writeImpl_p->setFlag (flag);
}

void
VisibilityIterator::setFlag (const Cube<Bool>& flag)
{
    CheckImplementationPointerW ();
    writeImpl_p->setFlag (flag);
}
void
VisibilityIterator::setFlagCategory(const Array<Bool>& flagCategory)
{
    CheckImplementationPointerW ();
    writeImpl_p->setFlagCategory (flagCategory);
}

void
VisibilityIterator::setFlagRow (const Vector<Bool>& rowflags)
{
    CheckImplementationPointerW ();
    writeImpl_p->setFlagRow (rowflags);
}

void
VisibilityIterator::setSigma (const Vector<Float>& sig)
{
    CheckImplementationPointerW ();
    writeImpl_p->setSigma (sig);
}

void
VisibilityIterator::setSigmaMat (const Matrix<Float>& sigmat)
{
    CheckImplementationPointerW ();
    writeImpl_p->setSigmaMat (sigmat);
}

void
VisibilityIterator::setVis (const Matrix<CStokesVector>& vis, DataColumn whichOne)
{
    CheckImplementationPointerW ();
    writeImpl_p->setVis (vis, whichOne);
}

void
VisibilityIterator::setVis (const Cube<Complex>& vis, DataColumn whichOne)
{
    CheckImplementationPointerW ();
    writeImpl_p->setVis (vis, whichOne);
}

void
VisibilityIterator::setVisAndFlag (const Cube<Complex>& vis, const Cube<Bool>& flag,
                                   DataColumn whichOne)
{
    CheckImplementationPointerW ();
    writeImpl_p->setVisAndFlag (vis, flag, whichOne);
}

void
VisibilityIterator::setWeight (const Vector<Float>& wt)
{
    CheckImplementationPointerW ();
    writeImpl_p->setWeight (wt);
}

void
VisibilityIterator::setWeightMat (const Matrix<Float>& wtmat)
{
    CheckImplementationPointerW ();
    writeImpl_p->setWeightMat (wtmat);
}

void
VisibilityIterator::setWeightSpectrum (const Cube<Float>& wtsp)
{
    CheckImplementationPointerW ();
    writeImpl_p->setWeightSpectrum (wtsp);
}

void VisibilityIterator::putModel(const RecordInterface& rec, Bool iscomponentlist, Bool incremental){
  CheckImplementationPointerW ();
  writeImpl_p->putModel(rec, iscomponentlist, incremental);
  
}



void VisibilityIterator::writeBack (VisBuffer * vb)
{
    CheckImplementationPointerW ();
    writeImpl_p->writeBack (vb);
}

using namespace casacore;
} // end namespace casa
