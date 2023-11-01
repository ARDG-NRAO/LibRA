/*
 * Vbi2MsRow.cc
 *
 *  Created on: Aug 22, 2013
 *      Author: jjacobs
 */

#include <casacore/casa/Exceptions.h>
#include <casacore/measures/Measures/MDirection.h>
#include <msvis/MSVis/Vbi2MsRow.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisModelDataI.h>
#include <msvis/MSVis/VisBufferImpl2.h>
#include <msvis/MSVis/VisBufferImpl2Internal.h>

#define AssertWritable() ThrowIf (! isWritable (), "Cannot write to this MsRow object")

using casa::vi::VisBufferImpl2;

using namespace casacore;
namespace casa {

namespace ms {


Vbi2MsRow::Vbi2MsRow (rownr_t row, const VisBufferImpl2 * vb)
: MsRow (row, vb),
  correctedCache_p (& VisBufferImpl2::visCubeCorrected),
  flagCache_p (& VisBufferImpl2::flagCube),
  modelCache_p(& VisBufferImpl2::visCubeModel),
  observedCache_p(&VisBufferImpl2::visCube),
  sigmaCache_p (&VisBufferImpl2::sigma),
  sigmaSpectrumCache_p(& VisBufferImpl2::sigmaSpectrum),
  weightCache_p (&VisBufferImpl2::weight),
  weightSpectrumCache_p(& VisBufferImpl2::weightSpectrum),
  vbi2_p (const_cast<VisBufferImpl2 *> (vb))
{
    configureArrayCaches ();
}


Vbi2MsRow::Vbi2MsRow (rownr_t row, VisBufferImpl2 * vb)
: MsRow (row, vb),
  correctedCache_p (& VisBufferImpl2::visCubeCorrected),
  flagCache_p (& VisBufferImpl2::flagCube),
  modelCache_p(& VisBufferImpl2::visCubeModel),
  observedCache_p(&VisBufferImpl2::visCube),
  sigmaCache_p (&VisBufferImpl2::sigma),
  sigmaSpectrumCache_p(& VisBufferImpl2::sigmaSpectrum),
  weightCache_p (&VisBufferImpl2::weight),
  weightSpectrumCache_p(& VisBufferImpl2::weightSpectrum),
  vbi2_p (vb)
{
    configureArrayCaches ();
}

void
Vbi2MsRow::configureArrayCaches ()
{
    arrayCaches_p.push_back (& correctedCache_p);
    arrayCaches_p.push_back (& flagCache_p);
    arrayCaches_p.push_back (& modelCache_p);
    arrayCaches_p.push_back (& observedCache_p);
    arrayCaches_p.push_back (& sigmaCache_p);
    arrayCaches_p.push_back (& sigmaSpectrumCache_p);
    arrayCaches_p.push_back (& weightCache_p);
    arrayCaches_p.push_back (& weightSpectrumCache_p);
}

void
Vbi2MsRow::clearArrayCaches ()
{
    for (std::vector<CachedArrayBase *>::iterator i = arrayCaches_p.begin();
         i != arrayCaches_p.end();
         i ++){
        (* i)->clearCache();
    }
}

void
Vbi2MsRow::changeRow (rownr_t row)
{
    MsRow::changeRow (row);
    clearArrayCaches ();
}

////////////////////////////////////////////////////////////
//
// Scalar getters
//

Int
Vbi2MsRow::antenna1 () const
{
    return vbi2_p->antenna1 () (row ());
}

Int
Vbi2MsRow::antenna2 () const
{
    return vbi2_p->antenna2 () (row ());
}

Int
Vbi2MsRow::arrayId () const
{
    return vbi2_p->arrayId () (row ());
}

Int
Vbi2MsRow::correlationType () const
{
    return vbi2_p->correlationTypes () (row ());
}


template <typename T, typename U>
void
Vbi2MsRow::copyIf (Bool copyThis, Vbi2MsRow * other,
                   void (Vbi2MsRow::* setter) (T),
                   U (Vbi2MsRow::* getter) () const)
{
    if (copyThis){
        (this ->* setter) ((other ->* getter) ());
    }
}

void
Vbi2MsRow::copy (Vbi2MsRow * other,
                 const VisBufferComponents2 & componentsToCopy)
{
    // Copy the data contents of the other row into this one

    setAntenna1 (other->antenna1 ());
    setAntenna2 (other->antenna2 ());
    setArrayId (other->arrayId ());
    setDataDescriptionId (other->dataDescriptionId ());
    setExposure (other->exposure ());
    setFeed1 (other->feed1 ());
    setFeed2 (other->feed2 ());
    setFieldId (other->fieldId ());
    setFlags (other->flags ());
    setInterval (other->interval ());
    setObservationId (other->observationId ());
    setProcessorId (other->processorId ());
    setRowFlag (other->isRowFlagged());
    setRowId (other->rowId());
    setScanNumber (other->scanNumber ());
    setSigma (other->sigma ());
    setSpectralWindow (other->spectralWindow());
    setStateId (other->stateId ());
    setTime (other->time ());
    setTimeCentroid (other->timeCentroid ());
    setUvw (other->uvw ());
    setWeight (other->weight ());

    // Optionally copied fields

    copyIf (componentsToCopy.contains (VisBufferComponent2::VisibilityCubeCorrected), other,
            & Vbi2MsRow::setCorrected, & Vbi2MsRow::corrected );
    copyIf (componentsToCopy.contains (VisBufferComponent2::VisibilityCubeModel), other,
            & Vbi2MsRow::setModel, & Vbi2MsRow::model );
    copyIf (componentsToCopy.contains (VisBufferComponent2::VisibilityCubeObserved), other,
            & Vbi2MsRow::setObserved, & Vbi2MsRow::observed );
    copyIf (componentsToCopy.contains (VisBufferComponent2::VisibilityCubeFloat), other,
            & Vbi2MsRow::setSingleDishData, & Vbi2MsRow::singleDishData );
    copyIf (componentsToCopy.contains (VisBufferComponent2::WeightSpectrum), other,
            & Vbi2MsRow::setWeightSpectrum, & Vbi2MsRow::weightSpectrum );
    copyIf (componentsToCopy.contains (VisBufferComponent2::SigmaSpectrum), other,
            & Vbi2MsRow::setSigmaSpectrum, & Vbi2MsRow::sigmaSpectrum );
}


Int
Vbi2MsRow::dataDescriptionId () const
{
    return vbi2_p->dataDescriptionIds () (row ());
}

Int
Vbi2MsRow::feed1 () const
{
    return vbi2_p->feed1 () (row ());
}

Int
Vbi2MsRow::feed2 () const
{
    return vbi2_p->feed2 () (row ());
}

Int
Vbi2MsRow::fieldId () const
{
    return vbi2_p->fieldId () (row ());
}

VisBufferImpl2 *
Vbi2MsRow::getVbi () const
{
    return vbi2_p;
}


Bool
Vbi2MsRow::isRowFlagged () const
{
    return vbi2_p->flagRow () (row ());
}

const Matrix<Bool> &
Vbi2MsRow::flags () const
{
    return flagCache_p.getCachedPlane (vbi2_p, row());
}

Matrix<Bool> &
Vbi2MsRow::flagsMutable ()
{
    return flagCache_p.getCachedPlane (vbi2_p, row());
}

Bool
Vbi2MsRow::isFlagged (Int correlation, Int channel) const
{
    return vbi2_p->flagCube () (correlation,channel, row ());
}


Int
Vbi2MsRow::observationId () const
{
    return vbi2_p->observationId () (row ());
}

Int
Vbi2MsRow::processorId () const
{
    return vbi2_p->processorId () (row ());
}

Int
Vbi2MsRow::scanNumber () const
{
    return vbi2_p->scan () (row ());
}

Int
Vbi2MsRow::stateId () const
{
    return vbi2_p->stateId () (row ());
}

Double
Vbi2MsRow::exposure () const
{
    return vbi2_p->exposure () (row ());
}

Double
Vbi2MsRow::interval () const
{
    return vbi2_p->timeInterval () (row ());
}

Int
Vbi2MsRow::rowId () const
{
    return vbi2_p->rowIds() (row());
}

Float
Vbi2MsRow::sigma (Int correlation) const
{
    return vbi2_p->sigma () (correlation, row ());
}

Int
Vbi2MsRow::spectralWindow () const
{
    return vbi2_p->spectralWindows() (row());
}


Double
Vbi2MsRow::time () const
{
    return vbi2_p->time () (row ());
}
Double
Vbi2MsRow::timeCentroid () const
{
    return vbi2_p->timeCentroid () (row ());
}

Float
Vbi2MsRow::weight (Int correlation) const
{
    return vbi2_p->weight () (correlation, row ());
}


////////////////////////////////////////////////////////////
//
// Scalar setters
//

void
Vbi2MsRow::setAntenna1 (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->antenna1_p.getRef (false) (row ()) = value;
}

void
Vbi2MsRow::setAntenna2 (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->antenna2_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setArrayId (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->arrayId_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setCorrelationType (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->corrType_p.getRef (false)(row ()) = value;
}



void
Vbi2MsRow::setDataDescriptionId (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->dataDescriptionIds_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setFeed1 (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->feed1_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setFeed2 (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->feed2_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setFieldId (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->fieldId_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setRowId (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->rowIds_p.getRef (false) (row ()) = value;
}

void
Vbi2MsRow::setRowFlag (Bool isFlagged)
{
    AssertWritable ();

    vbi2_p->cache_p->flagRow_p.getRef (false)(row ()) = isFlagged;
}

void
Vbi2MsRow::setFlags (Bool isFlagged, Int correlation, Int channel)
{
    AssertWritable ();

    vbi2_p->cache_p->flagCube_p.getRef (false)(correlation,channel, row ()) = isFlagged;
}

void
Vbi2MsRow::setFlags (const Matrix<Bool> & value)
{
    AssertWritable ();

    Matrix<Bool> & flags = flagCache_p.getCachedPlane (vbi2_p, row());
    flags = value;
}

void
Vbi2MsRow::setObservationId (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->observationId_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setProcessorId (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->processorId_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setScanNumber (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->scan_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setSpectralWindow (Int value)
{
    vbi2_p->cache_p->spectralWindows_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setStateId (Int value)
{
    AssertWritable();

    vbi2_p->cache_p->stateId_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setExposure (Double value)
{
    AssertWritable();

    vbi2_p->cache_p->exposure_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setInterval (Double value)
{
    AssertWritable();

    vbi2_p->cache_p->timeInterval_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setSigma (Int correlation, Float value)
{
    AssertWritable();

    vbi2_p->cache_p->sigma_p.getRef (false)(correlation, row ()) = value;
}

void
Vbi2MsRow::setTime (Double value)
{
    AssertWritable();

    vbi2_p->cache_p->time_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setTimeCentroid (Double value)
{
    AssertWritable();

    vbi2_p->cache_p->timeCentroid_p.getRef (false)(row ()) = value;
}

void
Vbi2MsRow::setWeight (Int correlation, Float value)
{
    AssertWritable();

    vbi2_p->cache_p->weight_p.getRef (false)(correlation, row ()) = value;
}

////////////////////////////////////////////////////////////
//
// Nonscalar getters
//

const Complex &
Vbi2MsRow::corrected (Int correlation, Int channel) const
{
    return vbi2_p->visCubeCorrected () (correlation, channel, row ());
}


Float
Vbi2MsRow::weightSpectrum (Int correlation, Int channel) const
{
    return vbi2_p->weightSpectrum () (correlation, channel, row ());
}


Float
Vbi2MsRow::sigmaSpectrum (Int correlation, Int channel) const
{
    return vbi2_p->sigmaSpectrum () (correlation, channel, row ());
}


const Complex &
Vbi2MsRow::model (Int correlation, Int channel) const
{
    return vbi2_p->visCubeModel () (correlation, channel, row ());
}


const Complex &
Vbi2MsRow::observed (Int correlation, Int channel) const
{
    return vbi2_p->visCube () (correlation, channel, row ());
}

const Float & Vbi2MsRow::singleDishData (Int correlation, Int channel) const
{
    return vbi2_p->visCubeFloat () (correlation, channel, row ());
}

const Vector<Double>
Vbi2MsRow::uvw () const
{
    return vbi2_p->uvw ().column (row());
}

void
Vbi2MsRow::setUvw (const Vector<Double> & value)
{
    vbi2_p->cache_p->uvw_p.getRef(false).column (row()) = value;
}

const Matrix<Complex> &
Vbi2MsRow::corrected () const
{
    return correctedCache_p.getCachedPlane (vbi2_p, row());
}

Matrix<Complex> &
Vbi2MsRow::correctedMutable ()
{
    return correctedCache_p.getCachedPlane (vbi2_p, row());
}

void
Vbi2MsRow::setCorrected (const Matrix<Complex> & value)
{
    AssertWritable();

    Matrix<Complex> & corrected = correctedCache_p.getCachedPlane (vbi2_p, row());
    corrected = value;
}

const Matrix<Complex> &
Vbi2MsRow::model () const
{
    return modelCache_p.getCachedPlane (vbi2_p, row());
}

Matrix<Complex> &
Vbi2MsRow::modelMutable ()
{
    return modelCache_p.getCachedPlane (vbi2_p, row());
}

void
Vbi2MsRow::setModel (const Matrix<Complex> & value)
{
    AssertWritable();

    Matrix<Complex> & model = modelCache_p.getCachedPlane (vbi2_p, row());
    model = value;
}

const Matrix<Complex> &
Vbi2MsRow::observed () const
{
    return observedCache_p.getCachedPlane (vbi2_p, row());
}

Matrix<Complex> &
Vbi2MsRow::observedMutable ()
{
    return observedCache_p.getCachedPlane (vbi2_p, row());
}

void
Vbi2MsRow::setObserved (const Matrix<Complex> & value)
{
    AssertWritable();

    Matrix<Complex> & observed = observedCache_p.getCachedPlane (vbi2_p, row());
    observed = value;
}

const Vector<Float> &
Vbi2MsRow::sigma () const
{
    return sigmaCache_p.getCachedColumn (vbi2_p, row());
}

Vector<Float> &
Vbi2MsRow::sigmaMutable () const
{
    return sigmaCache_p.getCachedColumn (vbi2_p, row());
}

void
Vbi2MsRow::setSigma (const Vector<Float> & value)
{
    AssertWritable();

    Vector<Float> & sigma = sigmaCache_p.getCachedColumn (vbi2_p, row());
    sigma = value;
}

const Matrix<Float> Vbi2MsRow::singleDishData () const
{
    return vbi2_p->visCubeFloat ().xyPlane (row());
}

Matrix<Float>
Vbi2MsRow::singleDishDataMutable ()
{
    return vbi2_p->visCubeFloat ().xyPlane (row());
}

void
Vbi2MsRow::setSingleDishData (const Matrix<Float> & value)
{
    vbi2_p->cache_p->floatDataCube_p.getRef(false).xyPlane (row()) = value;
}



const Vector<Float> &
Vbi2MsRow::weight () const
{
    return weightCache_p.getCachedColumn (vbi2_p, row());
}

Vector<Float> &
Vbi2MsRow::weightMutable () const
{
    return weightCache_p.getCachedColumn (vbi2_p, row());
}

void
Vbi2MsRow::setWeight (const Vector<Float> & value)
{
    AssertWritable();

    Vector<Float> & weight = weightCache_p.getCachedColumn (vbi2_p, row());
    weight = value;
}

void
Vbi2MsRow::setWeightSpectrum (const Matrix<Float> & value)
{
    AssertWritable();

    Matrix<Float> & weightSpectrum = weightSpectrumCache_p.getCachedPlane (vbi2_p, row());
    weightSpectrum = value;
}

const Matrix<Float> &
Vbi2MsRow::weightSpectrum () const
{
    return weightSpectrumCache_p.getCachedPlane (vbi2_p, row());
}

Matrix<Float> &
Vbi2MsRow::weightSpectrumMutable () const
{
    return weightSpectrumCache_p.getCachedPlane (vbi2_p, row());
}

const Matrix<Float> &
Vbi2MsRow::sigmaSpectrum () const
{
    return sigmaSpectrumCache_p.getCachedPlane (vbi2_p, row());
}

Matrix<Float> &
Vbi2MsRow::sigmaSpectrumMutable () const
{
    return sigmaSpectrumCache_p.getCachedPlane (vbi2_p, row());
}

void
Vbi2MsRow::setSigmaSpectrum (const Matrix<Float> & value)
{
    AssertWritable();

    Matrix<Float> & sigmaSpectrum = sigmaSpectrumCache_p.getCachedPlane (vbi2_p, row());
    sigmaSpectrum = value;
}

////////////////////////////////////////////////////////////
//
// Nonscalar setters
//

void
Vbi2MsRow::setCorrected (Int correlation, Int channel, const Complex & value)
{
    AssertWritable();

    vbi2_p->cache_p->correctedVisCube_p.getRef (false)(correlation, channel, row ()) = value;
}

void
Vbi2MsRow::setWeightSpectrum (Int correlation, Int channel, Float value)
{
    AssertWritable();

    vbi2_p->cache_p->weightSpectrum_p.getRef (false)(correlation, channel, row ()) = value;
}


void
Vbi2MsRow::setSigmaSpectrum (Int correlation, Int channel, Float value)
{
    AssertWritable();

    vbi2_p->cache_p->sigmaSpectrum_p.getRef (false)(correlation, channel, row ()) = value;
}


void
Vbi2MsRow::setModel (Int correlation, Int channel, const Complex & value)
{
    AssertWritable();

    vbi2_p->cache_p->modelVisCube_p.getRef (false)(correlation, channel, row ()) = value;
}

void
Vbi2MsRow::setObserved (Int correlation, Int channel, const Complex & value)
{
    AssertWritable();

    vbi2_p->cache_p->visCube_p.getRef (false)(correlation, channel, row ()) = value;
}

void
Vbi2MsRow::setSingleDishData (Int correlation, Int channel, const Float & value)
{
    AssertWritable();

    vbi2_p->cache_p->floatDataCube_p.getRef (false)(correlation, channel, row ()) = value;
}



}
}


