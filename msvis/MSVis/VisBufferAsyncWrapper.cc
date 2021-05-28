#include <msvis/MSVis/VisBufferAsyncWrapper.h>

#include "AsynchronousTools.h"
using namespace casacore;
using namespace casa::async;

#include "UtilJ.h"
using namespace casacore;
using namespace casa::utilj;

#include <msvis/MSVis/AsynchronousInterface.h>
#include <msvis/MSVis/VisibilityIteratorImplAsync.h>
#include <msvis/MSVis/VLAT.h>

#define CheckWrap() \
        if (wrappedVba_p == NULL){\
            Throw ("VisBufferAsyncWrapper: No attached VBA");\
        }

#define Log(level, ...) \
    {if (casa::asyncio::AsynchronousInterface::logThis (level)) \
         Logger::get()->log (__VA_ARGS__);};

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

VisBufferAsyncWrapper::VisBufferAsyncWrapper ()
 : wrappedVba_p (NULL),
   wrappedVisIterAsync_p (NULL)
{}

VisBufferAsyncWrapper::VisBufferAsyncWrapper (ROVisibilityIterator & iter)
 : wrappedVba_p (NULL)
{
    Assert (iter.isAsynchronous ());

    attachToVisIterAsync (iter);
}

VisBufferAsyncWrapper::VisBufferAsyncWrapper (const VisBufferAsync & other)
 : wrappedVba_p (NULL),
   wrappedVisIterAsync_p (NULL)
{
    wrappedVba_p = new VisBufferAsync (other);
}

VisBufferAsyncWrapper::~VisBufferAsyncWrapper ()
{
    if (wrappedVisIterAsync_p != NULL){
        detachFromVisIter();
    }

    delete wrappedVba_p;
}

VisBufferAsyncWrapper &
VisBufferAsyncWrapper::operator= (const VisBufferAsyncWrapper & other)
{
    if (this != & other){

        assign (other, true);
    }

    return * this;
}

VisBufferAsyncWrapper &
VisBufferAsyncWrapper::assign (const VisBuffer & vb, Bool copy)
{
    // existing?

    if (wrappedVisIterAsync_p != NULL){
        detachFromVisIter ();
    }

    if (wrappedVba_p == NULL){
        wrappedVba_p = new VisBufferAsync ();
    }

    wrappedVba_p->assign (vb, copy);

    return * this;
}

VisBufferAsyncWrapper &
VisBufferAsyncWrapper::operator-= (const VisBuffer & vb)
{
    CheckWrap ();
    wrappedVba_p->operator-= (vb);

    return * this;
}

void
VisBufferAsyncWrapper::allSelectedSpectralWindows (Vector<Int>& spws, Vector<Int>& nvischan)
{
  CheckWrap ();
  wrappedVba_p->allSelectedSpectralWindows (spws, nvischan);
}

void
VisBufferAsyncWrapper::allSelectedSpectralWindows (Vector<Int>& spws, Vector<Int>& nvischan) const
{
  CheckWrap ();
  wrappedVba_p->allSelectedSpectralWindows (spws, nvischan);
}

Vector<Int>
VisBufferAsyncWrapper::antIdRange () const
{
  CheckWrap ();
  return wrappedVba_p->antIdRange ();
}

Vector<Int>&
VisBufferAsyncWrapper::antenna1 ()
{
  CheckWrap ();
  return wrappedVba_p->antenna1 ();
}

const Vector<Int>&
VisBufferAsyncWrapper::antenna1 () const
{
  CheckWrap ();
  return wrappedVba_p->antenna1 ();
}

Vector<Int>&
VisBufferAsyncWrapper::antenna2 ()
{
  CheckWrap ();
  return wrappedVba_p->antenna2 ();
}

const Vector<Int>&
VisBufferAsyncWrapper::antenna2 () const
{
  CheckWrap ();
  return wrappedVba_p->antenna2 ();
}

Int
VisBufferAsyncWrapper::arrayId () const
{
  CheckWrap ();
  return wrappedVba_p->arrayId ();
}

void
VisBufferAsyncWrapper::attachToVisIter (ROVisibilityIterator & iter)
{
    ROVisibilityIterator * rovia = dynamic_cast<ROVisibilityIterator *> (& iter);

    ThrowIf (rovia == NULL, "Attempt to attach VisBufferAsyncWraper to synchronous ROVisibilityIterator");

    attachToVisIterAsync (* rovia);
}

void
VisBufferAsyncWrapper::attachToVisIterAsync (ROVisibilityIterator & iter)
{
    Log (2, "VBAW::attachToVisIterAsync this=%08x, iter=%08x\n", this, & iter);

    iter.attachVisBuffer (*this);
    wrappedVisIterAsync_p = & iter;

    attachWrappedVb (& iter);
}

void
VisBufferAsyncWrapper::attachWrappedVb (ROVisibilityIterator * iter)
{
    // Attach the wrapped VBA so that it can get needed information from
    // the VisibilityIterator.  Make connetion 1-way so that the wrapped
    // VB does not try to detach when destroyed.

    if (wrappedVba_p != NULL){
        wrappedVba_p->visIter_p = iter;
        wrappedVba_p->twoWayConnection_p = false;
    }
}



Vector<MDirection>
VisBufferAsyncWrapper::azel (Double time) const
{
  CheckWrap ();
  return wrappedVba_p->azel (time);
}

MDirection
VisBufferAsyncWrapper::azel0 (Double time) const
{
  CheckWrap ();
  return wrappedVba_p->azel0 (time);
}

Vector<Double>&
VisBufferAsyncWrapper::azel0Vec (Double time, Vector<Double>& azelVec) const
{
  CheckWrap ();
  return wrappedVba_p->azel0Vec (time, azelVec);
}

Matrix<Double>&
VisBufferAsyncWrapper::azelMat (Double time, Matrix<Double>& azelMat) const
{
  CheckWrap ();
  return wrappedVba_p->azelMat (time, azelMat);
}

template<class T> void
VisBufferAsyncWrapper::chanAccCube (Cube<T>& data, Int nChanOut)
{
  CheckWrap ();
  return wrappedVba_p->chanAccCube (data, nChanOut);
}

void
VisBufferAsyncWrapper::chanAveFlagCube (Cube<Bool>& flagcube, const Int nChanOut,
                                        const Bool restoreWeightSpectrum)
{
  CheckWrap ();
  wrappedVba_p->chanAveFlagCube (flagcube, nChanOut, restoreWeightSpectrum);
}

//template<class T>
//void chanAveVisCube (Cube<T>& data, Int nChanOut)
//{
//  CheckWrap ();
//  wrappedVba_p->chanAveVisCube (Cube<T>& data, Int nChanOut);
//}

Vector<Int>&
VisBufferAsyncWrapper::channel ()
{
  CheckWrap ();
  return wrappedVba_p->channel ();
}

const Vector<Int>&
VisBufferAsyncWrapper::channel () const
{
  CheckWrap ();
  return wrappedVba_p->channel ();
}

void
VisBufferAsyncWrapper::channelAve (const Matrix<Int>& chanavebounds)
{
  CheckWrap ();
  wrappedVba_p->channelAve (chanavebounds);
}

Bool
VisBufferAsyncWrapper::checkMSId ()
{
  CheckWrap ();
  return wrappedVba_p->checkMSId ();
}

void
VisBufferAsyncWrapper::checkVisIter (const char * func, const char * file, int line) const
{
  CheckWrap ();
  wrappedVba_p->checkVisIter (func, file, line);
}

VisBuffer *
VisBufferAsyncWrapper::clone ()
{
    return wrappedVba_p->clone();
}

Vector<SquareMatrix<Complex, 2> >&
VisBufferAsyncWrapper::CJones ()
{
  CheckWrap ();
  return wrappedVba_p->CJones ();
}

const Vector<SquareMatrix<Complex, 2> >&
VisBufferAsyncWrapper::CJones () const
{
  CheckWrap ();
  return wrappedVba_p->CJones ();
}

void
VisBufferAsyncWrapper::copyCache (const VisBuffer & other, Bool force)
{
  assert (! force);

  CheckWrap ();
  wrappedVba_p->copyCache (other, force);
}

Vector<Int>&
VisBufferAsyncWrapper::corrType ()
{
  CheckWrap ();
  return wrappedVba_p->corrType ();
}

const Vector<Int>&
VisBufferAsyncWrapper::corrType () const
{
  CheckWrap ();
  return wrappedVba_p->corrType ();
}

Cube<Complex>&
VisBufferAsyncWrapper::correctedVisCube ()
{
  CheckWrap ();
  return wrappedVba_p->correctedVisCube ();
}

const Cube<Complex>&
VisBufferAsyncWrapper::correctedVisCube () const
{
  CheckWrap ();
  return wrappedVba_p->correctedVisCube ();
}

Matrix<CStokesVector>&
VisBufferAsyncWrapper::correctedVisibility ()
{
  CheckWrap ();
  return wrappedVba_p->correctedVisibility ();
}

const Matrix<CStokesVector>&
VisBufferAsyncWrapper::correctedVisibility () const
{
  CheckWrap ();
  return wrappedVba_p->correctedVisibility ();
}

Cube<Complex>&
VisBufferAsyncWrapper::dataCube (const MS::PredefinedColumns whichcol)
{
  CheckWrap ();
  return wrappedVba_p->dataCube (whichcol);
}

const Cube<Complex>&
VisBufferAsyncWrapper::dataCube (const MS::PredefinedColumns whichcol) const
{
    CheckWrap ();
    return wrappedVba_p->dataCube (whichcol);
}

Int
VisBufferAsyncWrapper::dataDescriptionId () const
{
  CheckWrap ();
  return wrappedVba_p->dataDescriptionId ();
}

void
VisBufferAsyncWrapper::detachFromVisIter ()
{
    Log (2, "VBAW::detachFromVisIterAsync this=%08x, iter=%08x\n", this, wrappedVisIterAsync_p);

    if (wrappedVisIterAsync_p != NULL){

        Log (2, "VisBufferAsyncWrapper::detachFromVisIter this=%08x\n", this);

        wrappedVisIterAsync_p->detachVisBuffer(* this);

        wrappedVisIterAsync_p = NULL;
    }
}

Vector<MDirection>&
VisBufferAsyncWrapper::direction1 ()
{
  CheckWrap ();
  return wrappedVba_p->direction1 ();
}

const Vector<MDirection>&
VisBufferAsyncWrapper::direction1 ()  const
{
  CheckWrap ();
  return wrappedVba_p->direction1 ();
}

Vector<MDirection>&
VisBufferAsyncWrapper::direction2 ()
{
  CheckWrap ();
  return wrappedVba_p->direction2 ();
}

const Vector<MDirection>&
VisBufferAsyncWrapper::direction2 ()  const
{
  CheckWrap ();
  return wrappedVba_p->direction2 ();
}

Bool
VisBufferAsyncWrapper::existsWeightSpectrum () const
{
  CheckWrap ();
  return wrappedVba_p->existsWeightSpectrum ();
}

Vector<Double>&
VisBufferAsyncWrapper::exposure ()
{
  CheckWrap ();
  return wrappedVba_p->exposure ();
}

const Vector<Double>&
VisBufferAsyncWrapper::exposure () const
{
  CheckWrap ();
  return wrappedVba_p->exposure ();
}

Vector<Int>&
VisBufferAsyncWrapper::feed1 ()
{
  CheckWrap ();
  return wrappedVba_p->feed1 ();
}

const Vector<Int>&
VisBufferAsyncWrapper::feed1 () const
{
  CheckWrap ();
  return wrappedVba_p->feed1 ();
}

Vector<Float>&
VisBufferAsyncWrapper::feed1_pa ()
{
  CheckWrap ();
  return wrappedVba_p->feed1_pa ();
}

const Vector<Float>&
VisBufferAsyncWrapper::feed1_pa () const
{
  CheckWrap ();
  return wrappedVba_p->feed1_pa ();
}

Vector<Int>&
VisBufferAsyncWrapper::feed2 ()
{
  CheckWrap ();
  return wrappedVba_p->feed2 ();
}

const Vector<Int>&
VisBufferAsyncWrapper::feed2 () const
{
  CheckWrap ();
  return wrappedVba_p->feed2 ();
}

Vector<Float>&
VisBufferAsyncWrapper::feed2_pa ()
{
  CheckWrap ();
  return wrappedVba_p->feed2_pa ();
}

const Vector<Float>&
VisBufferAsyncWrapper::feed2_pa () const
{
  CheckWrap ();
  return wrappedVba_p->feed2_pa ();
}

Vector<Float>
VisBufferAsyncWrapper::feed_pa (Double time) const
{
  CheckWrap ();
  return wrappedVba_p->feed_pa (time);
}

Int
VisBufferAsyncWrapper::fieldId () const
{
  CheckWrap ();
  return wrappedVba_p->fieldId ();
}

Int
VisBufferAsyncWrapper::polarizationId() const
{
  CheckWrap ();
  return wrappedVba_p->polarizationId ();
}



// Fill Routines

Vector<Int>&
VisBufferAsyncWrapper::fillAnt1 ()
{
  CheckWrap ();
  return wrappedVba_p->fillAnt1 ();
}

Vector<Int>&
VisBufferAsyncWrapper::fillAnt2 ()
{
  CheckWrap ();
  return wrappedVba_p->fillAnt2 ();
}

Int &
VisBufferAsyncWrapper::fillArrayId ()
{
  CheckWrap ();
  return wrappedVba_p->fillArrayId ();
}

//Matrix<Int>&
//VisBufferAsyncWrapper::fillChanAveBounds ()
//{
//  CheckWrap ();
//  return wrappedVba_p->fillChanAveBounds ();
//}

Vector<Int>&
VisBufferAsyncWrapper::fillChannel ()
{
  CheckWrap ();
  return wrappedVba_p->fillChannel ();
}

Vector<SquareMatrix<Complex, 2> >&
VisBufferAsyncWrapper::fillCjones ()
{
  CheckWrap ();
  return wrappedVba_p->fillCjones ();
}

Vector<Int>&
VisBufferAsyncWrapper::fillCorrType ()
{
  CheckWrap ();
  return wrappedVba_p->fillCorrType ();
}

Vector<MDirection>&
VisBufferAsyncWrapper::fillDirection1 ()
{
  CheckWrap ();
  return wrappedVba_p->fillDirection1 ();
}

Vector<MDirection>&
VisBufferAsyncWrapper::fillDirection2 ()
{
  CheckWrap ();
  return wrappedVba_p->fillDirection2 ();
}

Vector<Double>&
VisBufferAsyncWrapper::fillExposure ()
{
  CheckWrap ();
  return wrappedVba_p->fillExposure ();
}

Vector<Int>&
VisBufferAsyncWrapper::fillFeed1 ()
{
  CheckWrap ();
  return wrappedVba_p->fillFeed1 ();
}

Vector<Float>&
VisBufferAsyncWrapper::fillFeed1_pa ()
{
  CheckWrap ();
  return wrappedVba_p->fillFeed1_pa ();
}

Vector<Int>&
VisBufferAsyncWrapper::fillFeed2 ()
{
  CheckWrap ();
  return wrappedVba_p->fillFeed2 ();
}

Vector<Float>&
VisBufferAsyncWrapper::fillFeed2_pa ()
{
  CheckWrap ();
  return wrappedVba_p->fillFeed2_pa ();
}

Int &
VisBufferAsyncWrapper::fillFieldId ()
{
  CheckWrap ();
  return wrappedVba_p->fillFieldId ();
}

Matrix<Bool>&
VisBufferAsyncWrapper::fillFlag ()
{
  CheckWrap ();
  return wrappedVba_p->fillFlag ();
}

Array<Bool>&
VisBufferAsyncWrapper::fillFlagCategory ()
{
  CheckWrap ();
  return wrappedVba_p->fillFlagCategory ();
}

Cube<Bool>&
VisBufferAsyncWrapper::fillFlagCube ()
{
  CheckWrap ();
  return wrappedVba_p->fillFlagCube ();
}

Vector<Bool> &
VisBufferAsyncWrapper::fillFlagRow ()
{
  CheckWrap ();
  return wrappedVba_p->fillFlagRow ();
}

Cube<Float>&
VisBufferAsyncWrapper::fillFloatDataCube ()
{
  CheckWrap ();
  return wrappedVba_p->fillFloatDataCube ();
}

Vector<Double>&
VisBufferAsyncWrapper::fillFreq ()
{
  CheckWrap ();
  return wrappedVba_p->fillFreq ();
}

//Matrix<Float>&
//VisBufferAsyncWrapper::fillImagingWeight ()
//{
//  CheckWrap ();
//  return wrappedVba_p->fillImagingWeight ();
//}

//Vector<Double>&
//VisBufferAsyncWrapper::fillLSRFreq ()
//{
//  CheckWrap ();
//  return wrappedVba_p->fillLSRFreq ();
//}

Int &
VisBufferAsyncWrapper::fillnChannel ()
{
  CheckWrap ();
  return wrappedVba_p->fillnChannel ();
}

Int &
VisBufferAsyncWrapper::fillnCorr ()
{
  CheckWrap ();
  return wrappedVba_p->fillnCorr ();
}

Int &
VisBufferAsyncWrapper::fillnRow ()
{
  CheckWrap ();
  return wrappedVba_p->fillnRow ();
}

Vector<Int> &
VisBufferAsyncWrapper::fillObservationId ()
{
  CheckWrap ();
  return wrappedVba_p->fillObservationId ();
}

MDirection &
VisBufferAsyncWrapper::fillPhaseCenter ()
{
  CheckWrap ();
  return wrappedVba_p->fillPhaseCenter ();
}

Int &
VisBufferAsyncWrapper::fillPolFrame ()
{
  CheckWrap ();
  return wrappedVba_p->fillPolFrame ();
}

Vector<Int> &
VisBufferAsyncWrapper::fillProcessorId ()
{
  CheckWrap ();
  return wrappedVba_p->fillProcessorId ();
}

Vector<Int> &
VisBufferAsyncWrapper::fillScan ()
{
  CheckWrap ();
  return wrappedVba_p->fillScan ();
}

Vector<Float>&
VisBufferAsyncWrapper::fillSigma ()
{
  CheckWrap ();
  return wrappedVba_p->fillSigma ();
}

Matrix<Float>&
VisBufferAsyncWrapper::fillSigmaMat ()
{
  CheckWrap ();
  return wrappedVba_p->fillSigmaMat ();
}

Int &
VisBufferAsyncWrapper::fillSpW ()
{
  CheckWrap ();
  return wrappedVba_p->fillSpW ();
}

Vector<Int> &
VisBufferAsyncWrapper::fillStateId ()
{
  CheckWrap ();
  return wrappedVba_p->fillStateId ();
}

Vector<Double>&
VisBufferAsyncWrapper::fillTime ()
{
  CheckWrap ();
  return wrappedVba_p->fillTime ();
}

Vector<Double>&
VisBufferAsyncWrapper::fillTimeCentroid ()
{
  CheckWrap ();
  return wrappedVba_p->fillTimeCentroid ();
}

Vector<Double>&
VisBufferAsyncWrapper::fillTimeInterval ()
{
  CheckWrap ();
  return wrappedVba_p->fillTimeInterval ();
}

Vector<RigidVector<Double, 3> >&
VisBufferAsyncWrapper::filluvw ()
{
  CheckWrap ();
  return wrappedVba_p->filluvw ();
}

Matrix<Double>&
VisBufferAsyncWrapper::filluvwMat ()
{
  CheckWrap ();
  return wrappedVba_p->filluvwMat ();
}

Matrix<CStokesVector>&
VisBufferAsyncWrapper::fillVis (VisibilityIterator::DataColumn whichOne)
{
  CheckWrap ();
  return wrappedVba_p->fillVis (whichOne);
}

Cube<Complex>&
VisBufferAsyncWrapper::fillVisCube (VisibilityIterator::DataColumn whichOne)
{
  CheckWrap ();
  return wrappedVba_p->fillVisCube (whichOne);
}

Vector<Float>&
VisBufferAsyncWrapper::fillWeight ()
{
  CheckWrap ();
  return wrappedVba_p->fillWeight ();
}

Matrix<Float>&
VisBufferAsyncWrapper::fillWeightMat ()
{
  CheckWrap ();
  return wrappedVba_p->fillWeightMat ();
}

Cube<Float>&
VisBufferAsyncWrapper::fillWeightSpectrum ()
{
  CheckWrap ();
  return wrappedVba_p->fillWeightSpectrum ();
}


Matrix<Bool>&
VisBufferAsyncWrapper::flag ()
{
  CheckWrap ();
  return wrappedVba_p->flag ();
}

const Matrix<Bool>&
VisBufferAsyncWrapper::flag () const
{
  CheckWrap ();
  return wrappedVba_p->flag ();
}

Array<Bool>&
VisBufferAsyncWrapper::flagCategory ()
{
  CheckWrap ();
  return wrappedVba_p->flagCategory ();
}

const Array<Bool>&
VisBufferAsyncWrapper::flagCategory () const
{
  CheckWrap ();
  return wrappedVba_p->flagCategory ();
}

Cube<Bool>&
VisBufferAsyncWrapper::flagCube ()
{
  CheckWrap ();
  return wrappedVba_p->flagCube ();
}

const Cube<Bool>&
VisBufferAsyncWrapper::flagCube () const
{
  CheckWrap ();
  return wrappedVba_p->flagCube ();
}

Vector<Bool>&
VisBufferAsyncWrapper::flagRow ()
{
  CheckWrap ();
  return wrappedVba_p->flagRow ();
}

const Vector<Bool>&
VisBufferAsyncWrapper::flagRow () const
{
  CheckWrap ();
  return wrappedVba_p->flagRow ();
}

Cube<Float>&
VisBufferAsyncWrapper::floatDataCube ()
{
  CheckWrap ();
  return wrappedVba_p->floatDataCube ();
}

const Cube<Float>&
VisBufferAsyncWrapper::floatDataCube () const
{
  CheckWrap ();
  return wrappedVba_p->floatDataCube ();
}

void
VisBufferAsyncWrapper::formStokes ()
{
  CheckWrap ();
  wrappedVba_p->formStokes ();
}

void
VisBufferAsyncWrapper::formStokes (Cube<Complex>& vis)
{
  CheckWrap ();
  wrappedVba_p->formStokes (vis);
}

void
VisBufferAsyncWrapper::formStokes (Cube<Float>& fcube)
{
  CheckWrap ();
  wrappedVba_p->formStokes (fcube);
}

void
VisBufferAsyncWrapper::formStokesWeightandFlag ()
{
  CheckWrap ();
  wrappedVba_p->formStokesWeightandFlag ();
}

void
VisBufferAsyncWrapper::freqAveCubes ()
{
  CheckWrap ();
  wrappedVba_p->freqAveCubes ();
}

void
VisBufferAsyncWrapper::freqAverage ()
{
  CheckWrap ();
  wrappedVba_p->freqAverage ();
}

Vector<Double>&
VisBufferAsyncWrapper::frequency ()
{
  CheckWrap ();
  return wrappedVba_p->frequency ();
}

const Vector<Double>&
VisBufferAsyncWrapper::frequency () const
{
  CheckWrap ();
  return wrappedVba_p->frequency ();
}

const VisImagingWeight &
VisBufferAsyncWrapper::getImagingWeightGenerator () const
{
    return wrappedVisIterAsync_p->getImagingWeightGenerator();
}

Int
VisBufferAsyncWrapper::getOldMsId () const
{
    return wrappedVba_p->oldMSId_p;
}


ROVisibilityIterator *
VisBufferAsyncWrapper::getVisibilityIterator () const
{
    return wrappedVisIterAsync_p;
}


Double
VisBufferAsyncWrapper::hourang (Double time) const
{
  CheckWrap ();
  return wrappedVba_p->hourang (time);
}

Matrix<Float>&
VisBufferAsyncWrapper::imagingWeight ()
{
  CheckWrap ();
  static_cast <const VisBufferAsyncWrapper *> (this)->imagingWeight ();

  return wrappedVba_p->imagingWeight_p;
}

const Matrix<Float>&
VisBufferAsyncWrapper::imagingWeight () const
{
  CheckWrap ();
  Assert (wrappedVisIterAsync_p != NULL);

  const VisImagingWeight & weightGenerator (wrappedVisIterAsync_p->getImagingWeightGenerator());

  return wrappedVba_p->imagingWeight (weightGenerator);
}


void
VisBufferAsyncWrapper::invalidate ()
{
  CheckWrap ();
  wrappedVba_p->invalidate ();
}

//Vector<Double>&
//VisBufferAsyncWrapper::lsrFrequency ()
//{
//  CheckWrap ();
//  return wrappedVba_p->lsrFrequency ();
//}
//
//const Vector<Double>&
//VisBufferAsyncWrapper::lsrFrequency () const
//{
//  CheckWrap ();
//  return wrappedVba_p->lsrFrequency ();
//}

void
VisBufferAsyncWrapper::lsrFrequency (const Int & spw, Vector<Double>& freq, Bool & convert) const
{
  CheckWrap ();
  wrappedVba_p->lsrFrequency (spw, freq, convert);
}

Cube<Complex>&
VisBufferAsyncWrapper::modelVisCube ()
{
  CheckWrap ();
  return wrappedVba_p->modelVisCube ();
}

Cube<Complex>&
VisBufferAsyncWrapper::modelVisCube (const Bool & matchVisCubeShape)
{
  CheckWrap ();
  return wrappedVba_p->modelVisCube (matchVisCubeShape);
}

const Cube<Complex>&
VisBufferAsyncWrapper::modelVisCube () const
{
  CheckWrap ();
  return wrappedVba_p->modelVisCube ();
}

Matrix<CStokesVector>&
VisBufferAsyncWrapper::modelVisibility ()
{
  CheckWrap ();
  return wrappedVba_p->modelVisibility ();
}

const Matrix<CStokesVector>&
VisBufferAsyncWrapper::modelVisibility () const
{
  CheckWrap ();
  return wrappedVba_p->modelVisibility ();
}

const MSColumns &
VisBufferAsyncWrapper::msColumns () const
{
  CheckWrap ();
  return wrappedVba_p->msColumns ();
}

Int
VisBufferAsyncWrapper::msId () const
{
  CheckWrap ();
  return wrappedVba_p->msId ();
}

Int &
VisBufferAsyncWrapper::nChannel ()
{
  CheckWrap ();
  return wrappedVba_p->nChannel ();
}

Int
VisBufferAsyncWrapper::nChannel () const
{
  CheckWrap ();
  return wrappedVba_p->nChannel ();
}

Int &
VisBufferAsyncWrapper::nCorr ()
{
  CheckWrap ();
  return wrappedVba_p->nCorr ();
}

Int
VisBufferAsyncWrapper::nCorr () const
{
  CheckWrap ();
  return wrappedVba_p->nCorr ();
}

Bool
VisBufferAsyncWrapper::newArrayId () const
{
    CheckWrap ();
    return wrappedVba_p->newArrayId ();
}

Bool
VisBufferAsyncWrapper::newFieldId () const
{
    CheckWrap ();
    return wrappedVba_p->newArrayId ();
}

Bool
VisBufferAsyncWrapper::newSpectralWindow () const
{
    CheckWrap ();
    return wrappedVba_p->newArrayId ();
}

Int &
VisBufferAsyncWrapper::nRow ()
{
  CheckWrap ();
  return wrappedVba_p->nRow ();
}

Int
VisBufferAsyncWrapper::nRow () const
{
  CheckWrap ();
  return wrappedVba_p->nRow ();
}

Bool
VisBufferAsyncWrapper::newMS () const
{
  CheckWrap ();
  return wrappedVba_p->newMS ();
}

Bool
VisBufferAsyncWrapper::nonCanonCorr ()
{
  CheckWrap ();
  return wrappedVba_p->nonCanonCorr ();
}

void
VisBufferAsyncWrapper::normalize (const Bool & phaseOnly)
{
  CheckWrap ();
  wrappedVba_p->normalize (phaseOnly);
}

Int
VisBufferAsyncWrapper::numberAnt () const
{
  CheckWrap ();
  return wrappedVba_p->numberAnt ();
}

Int
VisBufferAsyncWrapper::numberCoh () const
{
  CheckWrap ();
  return wrappedVba_p->numberCoh ();
}

Vector<Int>&
VisBufferAsyncWrapper::observationId ()
{
  CheckWrap ();
  return wrappedVba_p->observationId ();
}

const Vector<Int>&
VisBufferAsyncWrapper::observationId () const
{
  CheckWrap ();
  return wrappedVba_p->observationId ();
}

Vector<Float>
VisBufferAsyncWrapper::parang (Double time) const
{
  CheckWrap ();
  return wrappedVba_p->parang (time);
}

Float
VisBufferAsyncWrapper::parang0 (Double time) const
{
  CheckWrap ();
  return wrappedVba_p->parang0 (time);
}

MDirection &
VisBufferAsyncWrapper::phaseCenter ()
{
  CheckWrap ();
  return wrappedVba_p->phaseCenter ();
}

MDirection
VisBufferAsyncWrapper::phaseCenter () const
{
  CheckWrap ();
  return wrappedVba_p->phaseCenter ();
}

void
VisBufferAsyncWrapper::phaseCenterShift (Double dx, Double dy)
{
  CheckWrap ();
  wrappedVba_p->phaseCenterShift (dx, dy);
}

Int
VisBufferAsyncWrapper::polFrame () const
{
  CheckWrap ();
  return wrappedVba_p->polFrame ();
}

Vector<Int>&
VisBufferAsyncWrapper::processorId ()
{
  CheckWrap ();
  return wrappedVba_p->processorId ();
}

const Vector<Int>&
VisBufferAsyncWrapper::processorId () const
{
  CheckWrap ();
  return wrappedVba_p->processorId ();
}

void
VisBufferAsyncWrapper::refModelVis (const Matrix<CStokesVector>& mvis)
{
  CheckWrap ();
  wrappedVba_p->refModelVis (mvis);
}

VisBufferAsync *
VisBufferAsyncWrapper::releaseVba ()
{
    VisBufferAsync * vba = wrappedVba_p;
    wrappedVba_p = NULL;
    return vba;
}


void
VisBufferAsyncWrapper::removeScratchCols ()
{
  CheckWrap ();
  wrappedVba_p->removeScratchCols ();
}

void
VisBufferAsyncWrapper::resetWeightMat ()
{
  CheckWrap ();
  wrappedVba_p->resetWeightMat ();
}

Vector<rownr_t>&
VisBufferAsyncWrapper::rowIds ()
{
  CheckWrap ();
  return wrappedVba_p->rowIds ();
}

const Vector<rownr_t>&
VisBufferAsyncWrapper::rowIds () const
{
  CheckWrap ();
  return wrappedVba_p->rowIds ();
}

Vector<Int>&
VisBufferAsyncWrapper::scan ()
{
  CheckWrap ();
  return wrappedVba_p->scan ();
}

const Vector<Int>&
VisBufferAsyncWrapper::scan () const
{
  CheckWrap ();
  return wrappedVba_p->scan ();
}

Int
VisBufferAsyncWrapper::scan0 ()
{
  CheckWrap ();
  return wrappedVba_p->scan0 ();
}

void
VisBufferAsyncWrapper::setAllCacheStatuses (bool status)
{
  CheckWrap ();
  wrappedVba_p->setAllCacheStatuses (status);
}

void
VisBufferAsyncWrapper::setCorrectedVisCube (Complex c)
{
  CheckWrap ();
  wrappedVba_p->setCorrectedVisCube (c);
}

void
VisBufferAsyncWrapper::setCorrectedVisCube (const Cube<Complex> & vis)
{
  CheckWrap ();
  wrappedVba_p->setCorrectedVisCube (vis);
}

void
VisBufferAsyncWrapper::setFloatDataCube (const Cube<Float> & fcube)
{
  CheckWrap ();
  wrappedVba_p->setFloatDataCube (fcube);
}

void
VisBufferAsyncWrapper::setModelVisCube (Complex c)
{
  CheckWrap ();
  wrappedVba_p->setModelVisCube (c);
}

void
VisBufferAsyncWrapper::setModelVisCube (const Cube<Complex> & vis)
{
  CheckWrap ();
  wrappedVba_p->setModelVisCube (vis);
}

void
VisBufferAsyncWrapper::setModelVisCube (const Vector<Float> & stokes)
{
  CheckWrap ();
  wrappedVba_p->setModelVisCube (stokes);
}

void
VisBufferAsyncWrapper::setVisCube (Complex c)
{
  CheckWrap ();
  wrappedVba_p->setVisCube (c);
}

void
VisBufferAsyncWrapper::setVisCube (const Cube<Complex>& vis)
{
  CheckWrap ();
  wrappedVba_p->setVisCube (vis);
}

Vector<Float>&
VisBufferAsyncWrapper::sigma ()
{
  CheckWrap ();
  return wrappedVba_p->sigma ();
}

const Vector<Float>&
VisBufferAsyncWrapper::sigma () const
{
  CheckWrap ();
  return wrappedVba_p->sigma ();
}

Matrix<Float>&
VisBufferAsyncWrapper::sigmaMat ()
{
  CheckWrap ();
  return wrappedVba_p->sigmaMat ();
}

const Matrix<Float>&
VisBufferAsyncWrapper::sigmaMat () const
{
  CheckWrap ();
  return wrappedVba_p->sigmaMat ();
}

void
VisBufferAsyncWrapper::sortCorr ()
{
  CheckWrap ();
  wrappedVba_p->sortCorr ();
}

Int &
VisBufferAsyncWrapper::spectralWindow ()
{
  CheckWrap ();
  return wrappedVba_p->spectralWindow ();
}

Int
VisBufferAsyncWrapper::spectralWindow () const
{
  CheckWrap ();
  return wrappedVba_p->spectralWindow ();
}

Vector<Int>&
VisBufferAsyncWrapper::stateId ()
{
  CheckWrap ();
  return wrappedVba_p->stateId ();
}

const Vector<Int>&
VisBufferAsyncWrapper::stateId () const
{
  CheckWrap ();
  return wrappedVba_p->stateId ();
}

Vector<Double>&
VisBufferAsyncWrapper::time ()
{
  CheckWrap ();
  return wrappedVba_p->time ();
}

const Vector<Double>&
VisBufferAsyncWrapper::time () const
{
  CheckWrap ();
  return wrappedVba_p->time ();
}

Vector<Double>&
VisBufferAsyncWrapper::timeCentroid ()
{
  CheckWrap ();
  return wrappedVba_p->timeCentroid ();
}

const Vector<Double>&
VisBufferAsyncWrapper::timeCentroid () const
{
  CheckWrap ();
  return wrappedVba_p->timeCentroid ();
}

Vector<Double>&
VisBufferAsyncWrapper::timeInterval ()
{
  CheckWrap ();
  return wrappedVba_p->timeInterval ();
}

const Vector<Double>&
VisBufferAsyncWrapper::timeInterval () const
{
  CheckWrap ();
  return wrappedVba_p->timeInterval ();
}

Bool
VisBufferAsyncWrapper::timeRange (MEpoch & rTime, MVEpoch & rTimeEP, MVEpoch & rInterval) const
{
  CheckWrap ();
  return wrappedVba_p->timeRange (rTime, rTimeEP, rInterval);
}

void
VisBufferAsyncWrapper::unSortCorr ()
{
  CheckWrap ();
  wrappedVba_p->unSortCorr ();
}

Vector<Int>
VisBufferAsyncWrapper::unique (const Vector<Int>& indices) const
{
  CheckWrap ();
  return wrappedVba_p->unique (indices);
}

void
VisBufferAsyncWrapper::updateCoordInfo (const VisBuffer * vb)
{
  CheckWrap ();
  wrappedVba_p->updateCoordInfo (vb);
}

Vector<RigidVector<Double, 3> >&
VisBufferAsyncWrapper::uvw ()
{
  CheckWrap ();
  return wrappedVba_p->uvw ();
}

const Vector<RigidVector<Double, 3> >&
VisBufferAsyncWrapper::uvw () const
{
  CheckWrap ();
  return wrappedVba_p->uvw ();
}

Matrix<Double>&
VisBufferAsyncWrapper::uvwMat ()
{
  CheckWrap ();
  return wrappedVba_p->uvwMat ();
}

const Matrix<Double>&
VisBufferAsyncWrapper::uvwMat () const
{
  CheckWrap ();
  return wrappedVba_p->uvwMat ();
}

void
VisBufferAsyncWrapper::validate ()
{
  CheckWrap ();
  wrappedVba_p->validate ();
}

Vector<Int>
VisBufferAsyncWrapper::vecIntRange (const MSCalEnums::colDef & calEnum) const
{
  CheckWrap ();
  return wrappedVba_p->vecIntRange (calEnum);
}

Cube<Complex>&
VisBufferAsyncWrapper::visCube ()
{
  CheckWrap ();
  return wrappedVba_p->visCube ();
}

const Cube<Complex>&
VisBufferAsyncWrapper::visCube () const
{
  CheckWrap ();
  return wrappedVba_p->visCube ();
}

Matrix<CStokesVector>&
VisBufferAsyncWrapper::visibility ()
{
  CheckWrap ();
  return wrappedVba_p->visibility ();
}

const Matrix<CStokesVector>&
VisBufferAsyncWrapper::visibility () const
{
  CheckWrap ();
  return wrappedVba_p->visibility ();
}

Vector<Float>&
VisBufferAsyncWrapper::weight ()
{
  CheckWrap ();
  return wrappedVba_p->weight ();
}

const Vector<Float>&
VisBufferAsyncWrapper::weight () const
{
  CheckWrap ();
  return wrappedVba_p->weight ();
}

Cube<Float>&
VisBufferAsyncWrapper::weightCube ()
{
  CheckWrap ();
  return wrappedVba_p->weightCube ();
}

Matrix<Float>&
VisBufferAsyncWrapper::weightMat ()
{
  CheckWrap ();
  return wrappedVba_p->weightMat ();
}

const Matrix<Float>&
VisBufferAsyncWrapper::weightMat () const
{
  CheckWrap ();
  return wrappedVba_p->weightMat ();
}

Cube<Float>&
VisBufferAsyncWrapper::weightSpectrum ()
{
  CheckWrap ();
  return wrappedVba_p->weightSpectrum ();
}

const Cube<Float>&
VisBufferAsyncWrapper::weightSpectrum () const
{
  CheckWrap ();
  return wrappedVba_p->weightSpectrum ();
}

void
VisBufferAsyncWrapper::wrap (VisBufferAsync * vba)
{
    assert (vba != wrappedVba_p);

    if (wrappedVba_p != NULL){
        delete wrappedVba_p;
        wrappedVba_p = NULL;
    }

    wrappedVba_p = vba;
    attachWrappedVb (wrappedVisIterAsync_p);
}

using namespace casacore;
} // end namespace casa

