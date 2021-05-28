//# VisBuffer.cc: buffer for iterating through MS in large blocks
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
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
//# $Id$

#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBufferAsyncWrapper.h>
#include <msvis/MSVis/UtilJ.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/MaskArrMath.h>
#include <casa/Arrays/ArrayUtil.h>
#include <casa/OS/Path.h>
#include <components/ComponentModels/ComponentList.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/GenSort.h>
#include <casa/OS/Timer.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MeasurementSets/MSIter.h>

#define CheckVisIter() checkVisIter (__func__, __FILE__, __LINE__)
#define CheckVisIter1(s) checkVisIter (__func__, __FILE__, __LINE__,s)
#define CheckVisIterBase() checkVisIterBase (__func__, __FILE__, __LINE__)


// For debugging; remove/comment-out when working
//#include "VLAT.h"
//#define Log(level, ...) \
//    {if (VlaData::loggingInitialized_p && level <= VlaData::logLevel_p) \
//         Logger::log (__VA_ARGS__);};

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

VisBuffer::VisBuffer()
    : corrSorted_p(false),
      lastPointTableRow_p(0),
      This(this),
      twoWayConnection_p(false),
      visIter_p(static_cast<ROVisibilityIterator *>(0))
{
    validate();
    oldMSId_p = -1;

    std::unique_ptr<casa::VisModelDataI> check(VisModelDataI::create());
    if (check)
        visModelData_p = check.release();
}

VisBuffer::VisBuffer(ROVisibilityIterator & iter)
    : This(this),
      visIter_p(&iter)
{
    iter.attachVisBuffer(*this);
    twoWayConnection_p = true;
    oldMSId_p = -1;
    corrSorted_p = false;

    std::unique_ptr<casa::VisModelDataI> check(VisModelDataI::create());
    if (check)
        visModelData_p = check.release();
}

VisBuffer::VisBuffer(const VisBuffer & vb)
    : This(this),
      visIter_p(static_cast<ROVisibilityIterator *>(0))
{
    corrSorted_p = false;
    operator=(vb);
}

VisBuffer & VisBuffer::operator=(const VisBuffer & other)
{
    if (this != &other) {
        assign(other);
        oldMSId_p = -1;
    }
    return *this;
}

VisBuffer &
VisBuffer::assign(const VisBuffer & other, Bool copy)
{
    if (not other.visModelData_p.null()) visModelData_p = other.visModelData_p->clone();

    if (other.corrSorted_p) {
        throw(AipsError("Cannot assign a VisBuffer that has had correlations sorted!"));
    }

    if (this != &other) {
        if (visIter_p != static_cast<ROVisibilityIterator *>(0) &&
            twoWayConnection_p &&
            visIter_p != other.getVisibilityIterator()) {

            // If this VB is attached to it's visibility iterator and the
            // assignment will result in association with a different VI then
            // detach it.

            visIter_p->detachVisBuffer(*this);
        }

        visIter_p = other.getVisibilityIterator ();
        other.copyMsInfo(oldMSId_p, msOK_p, newMS_p);

        twoWayConnection_p = false;

        if (visIter_p == static_cast<ROVisibilityIterator *>(0)) {
            validate();
            copyCache (other, true);  // force copying
        } else if (copy) {
            copyCache (other, false); // copy only if there's something there
        } else {
            invalidate();
        }

    }
    return *this;
}


void VisBuffer::copyCoordInfo(const VisBuffer& other, Bool force)
{
  // Just do the nominally non-row-dep values
  cacheCopyNormal(arrayIdOK_p, other.arrayIdOK(), arrayId_p, other, &VisBuffer::arrayId, force);
  cacheCopyNormal(dataDescriptionIdOK_p, other.dataDescriptionIdOK(), dataDescriptionId_p, other, &VisBuffer::dataDescriptionId, force);
  cacheCopyNormal(fieldIdOK_p, other.fieldIdOK(), fieldId_p, other, &VisBuffer::fieldId, force);
  cacheCopyNormal(spectralWindowOK_p, other.spectralWindowOK(), spectralWindow_p, other,
                  &VisBuffer::spectralWindow, force);
  cacheCopyNormal(nCorrOK_p, other.nCorrOK(), nCorr_p, other, &VisBuffer::nCorr, force);
  cacheCopyNormal(nChannelOK_p, other.nChannelOK(), nChannel_p, other, &VisBuffer::nChannel, force);
  cacheCopyArray(frequencyOK_p, other.frequencyOK(), frequency_p, other, &VisBuffer::frequency, force);
}

void
VisBuffer::copyCache (const VisBuffer & other, Bool force)
{
    // Copies cache status from the other VisBuffer and if the status is true
    // then the cached values are copied over from the other VisBuffer as well.

    // Keep in order so that finding omitted ones will be easier
    // in the future

    cacheCopyArray  (antenna1OK_p, other.antenna1OK (), antenna1_p, other, & VisBuffer::antenna1, force);
    cacheCopyArray  (antenna2OK_p, other.antenna2OK (), antenna2_p, other, & VisBuffer::antenna2, force);
    cacheCopyNormal (arrayIdOK_p, other.arrayIdOK (), arrayId_p, other, & VisBuffer::arrayId, force);
    ////cacheCopyArray  (chanAveBoundsOK_p, other.chanAveBoundsOK (), chanAveBounds_p, other, & VisBuffer::chanAveBounds, force);
    cacheCopyArray  (channelOK_p, other.channelOK (), channel_p, other, & VisBuffer::channel, force);
    cacheCopyArray  (cjonesOK_p, other.cjonesOK (), cjones_p, other, & VisBuffer::CJones, force);
    cacheCopyArray  (correctedVisCubeOK_p, other.correctedVisCubeOK (),
                     correctedVisCube_p, other, & VisBuffer::correctedVisCube, force);
    cacheCopyArray  (correctedVisibilityOK_p, other.correctedVisibilityOK (),
                     correctedVisibility_p, other, & VisBuffer::correctedVisibility, force);
    cacheCopyArray  (corrTypeOK_p, other.corrTypeOK (), corrType_p, other, & VisBuffer::corrType, force);
    cacheCopyNormal (dataDescriptionIdOK_p, other.dataDescriptionIdOK(), dataDescriptionId_p, other, & VisBuffer::dataDescriptionId, force);
    cacheCopyArray  (direction1OK_p, other.direction1OK (), direction1_p, other, & VisBuffer::direction1, force);
    cacheCopyArray  (direction2OK_p, other.direction2OK (), direction2_p, other, & VisBuffer::direction2, force);
    cacheCopyArray  (exposureOK_p, other.exposureOK (), exposure_p, other, & VisBuffer::exposure, force);
    cacheCopyArray  (feed1OK_p, other.feed1OK (), feed1_p, other, & VisBuffer::feed1, force);
    cacheCopyArray  (feed1_paOK_p, other.feed1_paOK (), feed1_pa_p, other, & VisBuffer::feed1_pa, force);
    cacheCopyArray  (feed2OK_p, other.feed2OK (), feed2_p, other, & VisBuffer::feed2, force);
    cacheCopyArray  (feed2_paOK_p, other.feed2_paOK (), feed2_pa_p, other, & VisBuffer::feed2_pa, force);
    cacheCopyNormal (fieldIdOK_p, other.fieldIdOK (), fieldId_p, other, & VisBuffer::fieldId, force);
    cacheCopyArray  (flagOK_p, other.flagOK (), flag_p, other, & VisBuffer::flag, force);
    cacheCopyArray  (flagCategoryOK_p, other.flagCategoryOK (), flagCategory_p, other, & VisBuffer::flagCategory, force);
    cacheCopyArray  (flagCubeOK_p, other.flagCubeOK (), flagCube_p, other, & VisBuffer::flagCube, force);
    cacheCopyArray  (flagRowOK_p, other.flagRowOK (), flagRow_p, other, & VisBuffer::flagRow, force);
    cacheCopyArray  (floatDataCubeOK_p, other.floatDataCubeOK (), floatDataCube_p, other, & VisBuffer::floatDataCube, force);
    cacheCopyArray  (frequencyOK_p, other.frequencyOK (), frequency_p, other, & VisBuffer::frequency, force);
    cacheCopyArray  (imagingWeightOK_p, other.imagingWeightOK (), imagingWeight_p, other, & VisBuffer::imagingWeight, force);
    //cacheCopyArray  (lsrFrequencyOK_p, other.lsrFrequencyOK (), lsrFrequency_p, other, & VisBuffer::lsrFrequency, force);
    cacheCopyArray  (modelVisCubeOK_p, other.modelVisCubeOK (), modelVisCube_p, other, & VisBuffer::modelVisCube, force);
    cacheCopyArray  (modelVisibilityOK_p, other.modelVisibilityOK (),
                     modelVisibility_p, other, & VisBuffer::modelVisibility, force);
    cacheCopyNormal (nChannelOK_p, other.nChannelOK (), nChannel_p, other, & VisBuffer::nChannel, force);
    cacheCopyNormal (nCorrOK_p, other.nCorrOK (), nCorr_p, other, & VisBuffer::nCorr, force);
    //cacheCopyNormal (nCatOK_p, other.nCatOK (), nCat_p, other, & VisBuffer::nCat, force);
    cacheCopyNormal (nRowOK_p, other.nRowOK (), nRow_p, other, & VisBuffer::nRow, force);
    cacheCopyArray  (observationIdOK_p, other.observationIdOK (), observationId_p, other, & VisBuffer::observationId, force);
    cacheCopyNormal (phaseCenterOK_p, other.phaseCenterOK (), phaseCenter_p, other, & VisBuffer::phaseCenter, force);
    cacheCopyNormal (polFrameOK_p, other.polFrameOK (), polFrame_p, other, & VisBuffer::polFrame, force);
    cacheCopyArray  (processorIdOK_p, other.processorIdOK (), processorId_p, other, & VisBuffer::processorId, force);
    cacheCopyArray  (rowIdsOK_p, other.rowIdsOK (), rowIds_p, other, & VisBuffer::rowIds, force);
    cacheCopyArray  (scanOK_p, other.scanOK (), scan_p, other, & VisBuffer::scan, force);
    cacheCopyArray  (sigmaOK_p, other.sigmaOK (), sigma_p, other, & VisBuffer::sigma, force);
    cacheCopyArray  (sigmaMatOK_p, other.sigmaMatOK (), sigmaMat_p, other, & VisBuffer::sigmaMat, force);
    cacheCopyNormal (spectralWindowOK_p, other.spectralWindowOK (), spectralWindow_p, other, & VisBuffer::spectralWindow, force);
    cacheCopyArray  (stateIdOK_p, other.stateIdOK (), stateId_p, other, & VisBuffer::stateId, force);
    cacheCopyArray  (timeOK_p, other.timeOK (), time_p, other, & VisBuffer::time, force);
    cacheCopyArray  (timeCentroidOK_p, other.timeCentroidOK (), timeCentroid_p, other, & VisBuffer::timeCentroid, force);
    cacheCopyArray  (timeIntervalOK_p, other.timeIntervalOK (), timeInterval_p, other, & VisBuffer::timeInterval, force);
    cacheCopyArray  (uvwOK_p, other.uvwOK (), uvw_p, other, & VisBuffer::uvw, force);
    cacheCopyArray  (uvwMatOK_p, other.uvwMatOK (), uvwMat_p, other, & VisBuffer::uvwMat, force);
    cacheCopyArray  (visCubeOK_p, other.visCubeOK (), visCube_p, other, & VisBuffer::visCube, force);
    cacheCopyArray  (visibilityOK_p, other.visibilityOK (), visibility_p, other, & VisBuffer::visibility, force);
    cacheCopyArray  (weightOK_p, other.weightOK (), weight_p, other, & VisBuffer::weight, force);
    ////cacheCopyArray  (weightCubeOK_p, other.weightCubeOK (), weightCube_p, other, & VisBuffer::weightCube, force);
    cacheCopyArray  (weightMatOK_p, other.weightMatOK (), weightMat_p, other, & VisBuffer::weightMat, force);
    cacheCopyArray  (weightSpectrumOK_p, other.weightSpectrumOK (),
                     weightSpectrum_p, other, & VisBuffer::weightSpectrum, force);

}

VisBuffer::~VisBuffer()
{
    if (visIter_p != static_cast<ROVisibilityIterator *>(0) && twoWayConnection_p) {
        visIter_p->detachVisBuffer(*this);
    }
}

VisBuffer &
VisBuffer::operator-=(const VisBuffer & vb)
{
    // check the shapes
    AlwaysAssert(nRow_p == vb.nRow(), AipsError);
    AlwaysAssert(nChannel_p == vb.nChannel(), AipsError);
    AlwaysAssert(nCorr_p == vb.nCorr(), AipsError);
    // make sure flag and flagRow are current
    flag();
    flagRow();
    // flagCategory?

    // do the subtraction, or'ing the flags
    Int nRows = nRow ();
    Int nChannels = nChannel ();
    for (Int row = 0; row < nRows; row++) {
        if (vb.flagRow()(row)) {
            flagRow_p(row) = true;
        }
        if (!flagRow_p(row)) {
            for (Int chn = 0; chn < nChannels; chn++) {
                if (vb.flag()(chn, row)) {
                    flag_p(chn, row) = true;
                }
                if (!flag_p(chn, row)) {
                    visibility_p(chn, row) -= vb.visibility()(chn, row);
                }
            }
        }
    }
    return *this;
}

void
VisBuffer::attachToVisIter(ROVisibilityIterator & iter)
{
    if (visIter_p != static_cast<ROVisibilityIterator *>(0) && twoWayConnection_p) {
        visIter_p->detachVisBuffer(*this);
    }
    visIter_p = &iter;
    iter.attachVisBuffer(*this);
    twoWayConnection_p = true;
}

void
VisBuffer::detachFromVisIter ()
{
    if (visIter_p != NULL) {
        visIter_p->detachVisBuffer(* this);

        visIter_p = NULL;
    }
}

void VisBuffer::invalidate()
{

    setAllCacheStatuses (false);
    lastPointTableRow_p = 0;
}

void VisBuffer::validate()
{
    setAllCacheStatuses (true);
}

Int
VisBuffer::getOldMsId () const
{
    return oldMSId_p;
}

String VisBuffer::msName(Bool stripPath) const{
  String name="";
  if(visIter_p != NULL){
    name=visIter_p->ms().antenna().tableName();
    name.erase(name.length()-8);
    if(stripPath){
      Path path(name);
      return path.baseName();
    }
    
  }
  
  return name;
}

ROVisibilityIterator *
VisBuffer::getVisibilityIterator () const
{
    return visIter_p;
}

Matrix<Float> &
VisBuffer::imagingWeight ()
{
    static_cast<const VisBuffer *> (this) -> imagingWeight ();

    return imagingWeight_p;
}

const Matrix<Float> &
VisBuffer::imagingWeight () const
{
    const VisImagingWeight & weightGenerator = getVisibilityIterator()->getImagingWeightGenerator ();

    return imagingWeight (weightGenerator);
}

const Matrix<Float> &
VisBuffer::imagingWeight (const VisImagingWeight & weightGenerator) const
{
    if (imagingWeightOK_p){
        return imagingWeight_p;
    }

    if (weightGenerator.getType () == "none") {
        throw (AipsError ("Programmer Error... imaging weights not set"));
    }

    // Extract data weights correctly [nchan,nrow]
    //   NB:  VB::weight() yields corr-,chan-indep row weights
    Matrix<Float> wtm;
    if (weightSpectrum().nelements()==0) 
      wtm.reference(weight().reform(IPosition(2,1,nRow()))); // add degenerate chan axis
    else 
      weightGenerator.unPolChanWeight(wtm,weightSpectrum());  

    Matrix<Bool> flagmat = flag ();
    imagingWeight_p.resize (flagmat.shape ());

    Vector<Double> fvec;
    Matrix<Double> uvwmat;

    String type = weightGenerator.getType();
    if (weightGenerator.doFilter() || type == "uniform" || type == "radial"){
        fvec = frequency ();
        uvwmat = uvwMat ();
    }

    if (weightGenerator.getType () == "uniform") {

        weightGenerator.weightUniform (imagingWeight_p, flagmat, uvwmat, fvec, wtm, msId (), fieldId ());

    } else if (weightGenerator.getType () == "radial") {

        weightGenerator.weightRadial (imagingWeight_p, flagmat, uvwmat, fvec, wtm);

    } else {

        weightGenerator.weightNatural (imagingWeight_p, flagmat, wtm);
    }

    if (weightGenerator.doFilter ()) {

        weightGenerator.filter (imagingWeight_p, flagmat, uvwmat, fvec, wtm);
    }

    This->imagingWeightOK_p = true;

    return imagingWeight_p;
}


//const Matrix<Float> &
//VisBuffer::imagingWeight () const
//{
//    return imagingWeight ();
//}

Bool
VisBuffer::newArrayId () const
{
    CheckVisIter ();
    return visIter_p->newArrayId ();
}

Bool
VisBuffer::newFieldId () const
{
    CheckVisIter ();
    return visIter_p->newFieldId ();
}

Bool
VisBuffer::newSpectralWindow () const
{
    CheckVisIter ();
    return visIter_p->newSpectralWindow ();
}




void
VisBuffer::setAllCacheStatuses (bool status)
{
    antenna1OK_p = status;
    antenna2OK_p = status;
    arrayIdOK_p = status;
    channelOK_p = status;
    cjonesOK_p = status;
    correctedVisCubeOK_p = status;
    correctedVisibilityOK_p = status;
    corrTypeOK_p = status;
    dataDescriptionIdOK_p = status;
    direction1OK_p = status;
    firstDirection1OK_p=status;
    direction2OK_p = status;
    exposureOK_p  = status;
    feed1_paOK_p = status;
    feed1OK_p = status;
    feed2_paOK_p = status;
    feed2OK_p = status;
    fieldIdOK_p = status;
    flagCubeOK_p = status;
    flagOK_p = status;
    flagRowOK_p = status;
    flagCategoryOK_p = status;
    floatDataCubeOK_p  = status;
    frequencyOK_p = status;
    imagingWeightOK_p = status;
    ///////////lsrFrequencyOK_p = status;
    modelVisCubeOK_p = status;
    modelVisibilityOK_p = status;
    msOK_p = status;
    nChannelOK_p = status;
    nCorrOK_p = status;
    //    nCatOK_p = status;
    nRowOK_p = status;
    observationIdOK_p  = status;
    phaseCenterOK_p = status;
    polFrameOK_p = status;
    processorIdOK_p  = status;
    rowIdsOK_p = status;
    scanOK_p = status;
    sigmaMatOK_p = status;
    sigmaOK_p = status;
    spectralWindowOK_p = status;
    stateIdOK_p  = status;
    timeCentroidOK_p  = status;
    timeIntervalOK_p = status;
    timeOK_p = status;
    uvwMatOK_p = status;
    uvwOK_p = status;
    visCubeOK_p = status;
    visibilityOK_p = status;
    weightMatOK_p = status;
    weightOK_p = status;
    weightSpectrumOK_p = status;
    
}

Cube<Complex>& VisBuffer::dataCube(const MS::PredefinedColumns whichcol)
{
  switch(whichcol){
  case MS::DATA:
    return This->visCube();
  case MS::MODEL_DATA:
    return This->modelVisCube();
  case MS::CORRECTED_DATA:
    return This->correctedVisCube();
  default:
    throw(AipsError(MS::columnName(whichcol) + " is not supported as a data Cube."));
  }
}

const Cube<Complex>& VisBuffer::dataCube(const MS::PredefinedColumns whichcol) const
{
  switch(whichcol){
  case MS::DATA:
    return This->visCube();
  case MS::MODEL_DATA:
    return This->modelVisCube();
  case MS::CORRECTED_DATA:
    return This->correctedVisCube();
  default:
    throw(AipsError(MS::columnName(whichcol) + " is not supported as a data Cube."));
  }
}

void VisBuffer::freqAverage()
{
    Matrix<CStokesVector> newVisibility(1, nRow());
    Matrix<Bool> newFlag(1, nRow());
    newFlag = true;
    Double newFrequency;
    newFrequency = 0;
    Int nfreq = 0;
    Int nChan = nChannel();
    for (Int row = 0; row < nRow(); row++) {
        if (!flagRow()(row)) {
            Int n = 0;
            for (Int chn = 0; chn < nChan; chn++) {
                if (!flag()(chn, row)) {
                    newVisibility(0, row) += visibility()(chn, row);
                    newFlag(0, row) = false;
                    newFrequency += frequency()(chn);
                    n++;
                    nfreq++;
                }
            }
            if (n == 0) {
                flagRow()(row) = true;
            }
            if (n > 0) {
                newVisibility(0, row) *= 1.0f / n;
            }
        }
    }
    // Average frequency for this buffer (should really be row based)
    if (nfreq > 0) {
        newFrequency /= Double(nfreq);
    }
    nChannel_p = 1;
    flag_p.reference(newFlag);
    visibility_p.reference(newVisibility);
    frequency_p.resize(1);
    frequency_p(0) = newFrequency;
}

void VisBuffer::freqAveCubes()
{
    // TBD: Use weightSpec, if present, and update weightMat accordingly
    // TBD: Provide partial decimation option

    // Ensure visCube filled, at least
    visCube();

    // Freq-averaged shape
    IPosition csh = visCube().shape();
    csh(1) = 1; // One channel in output

    Cube<Complex> newVisCube(csh);
    newVisCube = Complex(0.0);
    Matrix<Bool> newFlag(1, nRow());
    newFlag = true;
    Double newFrequency;
    newFrequency = 0;
    Int nfreq = 0;
    Int nChan = nChannel();
    Int nCor = nCorr();
    for (Int row = 0; row < nRow(); row++) {
        if (!flagRow()(row)) {
            Int n = 0;
            for (Int chn = 0; chn < nChan; chn++) {
                if (!flag()(chn, row)) {
                    newFlag(0, row) = false;
                    newFrequency += frequency()(chn);
                    for (Int cor = 0; cor < nCor; cor++) {
                        newVisCube(cor, 0, row) += visCube()(cor, chn, row);
                    }
                    n++;
                    nfreq++;

                    if (row == -1)
                        cout << "V: "
                             << chn << " " << n << " "
                             << visCube()(0, chn, row) << " "
                             << newVisCube(0, 0, row) << " "
                             << endl;


                }
            }
            if (n == 0) {
                flagRow()(row) = true;
            }
            if (n > 0) {
                Matrix<Complex> nVC;
                nVC.reference(newVisCube.xyPlane(row));
                nVC *= Complex(1.0f / n);

                if (row == -1) {
                    cout << "V:-----> " << n << " " << newVisCube(0, 0, row) << endl;
                }


            }
        }
    }
    visCube_p.reference(newVisCube);

    // Now do model, if present
    if (modelVisCubeOK_p) {

        Cube<Complex> newModelVisCube(csh);
        newModelVisCube = Complex(0.0);
        for (Int row = 0; row < nRow(); row++) {
            if (!flagRow()(row)) {
                Int n = 0;
                for (Int chn = 0; chn < nChan; chn++) {
                    if (!flag()(chn, row)) {

                        n++;
                        for (Int cor = 0; cor < nCor; cor++) {
                            newModelVisCube(cor, 0, row) += modelVisCube()(cor, chn, row);
                        }

                        if (row == -1)
                            cout << "M: "
                                 << chn << " " << n << " "
                                 << modelVisCube()(0, chn, row) << " "
                                 << newModelVisCube(0, 0, row) << " "
                                 << endl;

                    }
                }
                if (n == 0) {
                    flagRow()(row) = true;
                }
                if (n > 0) {
                    Matrix<Complex> nMVC;
                    nMVC.reference(newModelVisCube.xyPlane(row));
                    nMVC *= Complex(1.0f / n);

                    if (row == -1) {
                        cout << "M:-----> " << n << " " << newModelVisCube(0, 0, row) << endl;
                    }
                }
            }
        }
        modelVisCube_p.reference(newModelVisCube);
    }

    // Use averaged flags
    flag_p.reference(newFlag);

    // Average frequency for this buffer
    //  (Strictly, this should really be row based, but doing this
    //   average here suggests frequency precision isn't so important)
    if (nfreq > 0) {
        newFrequency /= Double(nfreq);
    }
    nChannel_p = 1;
    frequency_p.resize(1);
    frequency_p(0) = newFrequency;

}

void VisBuffer::formStokes()
{

    // We must form the weights and flags correctly
    formStokesWeightandFlag();

    // Now do whatever data is present
    if (visCubeOK_p) {
        formStokes(visCube_p);
    }

    if (modelVisCubeOK_p) {
        formStokes(modelVisCube_p);
    }

    if (correctedVisCubeOK_p) {
        formStokes(correctedVisCube_p);
    }

    if (floatDataCubeOK_p) {
        formStokes(floatDataCube_p);
    }
}

void
VisBuffer::lsrFrequency(const Int & spw, Vector<Double>& freq, Bool & convert,
			const Bool ignoreconv) const
{
    CheckVisIter ();
    visIter_p->lsrFrequency(spw, freq, convert, ignoreconv);
}


void VisBuffer::formStokesWeightandFlag()
{

    // Ensure corrType, weightMat and flagCube are filled
    corrType();
    weightMat();
    flagCube();

    switch (nCorr()) {
    case 4: {

        Slice all = Slice();
        Slice pp(0, 1, 1), pq(1, 1, 1), qp(2, 1, 1), qq(3, 1, 1);
        Slice a(0, 1, 1), b(1, 1, 1), c(2, 1, 1), d(3, 1, 1);

        // Sort for linears
        if (polFrame() == MSIter::Linear) {
            d = Slice(1, 1, 1); // Q
            b = Slice(2, 1, 1); // U
            c = Slice(3, 1, 1); // V
        }

        Matrix<Float> newWtMat(weightMat_p.shape());
        newWtMat(a, all) = newWtMat(d, all) = (weightMat_p(pp, all) + weightMat_p(qq, all));
        newWtMat(b, all) = newWtMat(c, all) = (weightMat_p(pq, all) + weightMat_p(qp, all));
        weightMat_p.reference(newWtMat);

        Cube<Bool> newFlagCube(flagCube_p.shape());
        newFlagCube(a, all, all) = newFlagCube(d, all, all) = (flagCube_p(pp, all, all) | flagCube_p(qq, all, all));
        newFlagCube(b, all, all) = newFlagCube(c, all, all) = (flagCube_p(pq, all, all) | flagCube_p(qp, all, all));
        flagCube_p.reference(newFlagCube);

        corrType_p(0) = Stokes::I;
        corrType_p(1) = Stokes::Q;
        corrType_p(2) = Stokes::U;
        corrType_p(3) = Stokes::V;

        break;
    }
    case 2: {
        // parallel hands only
        Slice all = Slice();
        Slice pp(0, 1, 1), qq(1, 1, 1);
        Slice a(0, 1, 1), d(1, 1, 1);

        Matrix<Float> newWtMat(weightMat_p.shape());
        newWtMat(a, all) = newWtMat(d, all) = weightMat_p(pp, all) + weightMat_p(qq, all);
        weightMat_p.reference(newWtMat);

        Cube<Bool> newFlagCube(flagCube_p.shape());
        newFlagCube(a, all, all) = newFlagCube(d, all, all) = flagCube_p(pp, all, all) | flagCube_p(qq, all, all);
        flagCube_p.reference(newFlagCube);

        corrType_p(0) = Stokes::I;
        corrType_p(1) = ((polFrame() == MSIter::Circular) ? Stokes::V : Stokes::Q);

        break;
    }
    case 1: {

        // Just need to re-label as I
        corrType_p(0) = Stokes::I;

    }
    default: {
        cout << "Insufficient correlations to form Stokes" << endl;
        break;
    }
    }

}



void VisBuffer::formStokes(Cube<Complex>& vis)
{

    Cube<Complex> newvis(vis.shape());
    newvis.set(0.0);
    Slice all = Slice();

    switch (nCorr()) {
    case 4: {

        Slice pp(0, 1, 1), pq(1, 1, 1), qp(2, 1, 1), qq(3, 1, 1);
        Slice a(0, 1, 1), b(1, 1, 1), c(2, 1, 1), d(3, 1, 1);

        if (polFrame() == MSIter::Linear) {
            d = Slice(1, 1, 1); // Q
            b = Slice(2, 1, 1); // U
            c = Slice(3, 1, 1); // V
        }

        newvis(a, all, all) = (vis(pp, all, all) + vis(qq, all, all)); //  I / I
        newvis(d, all, all) = (vis(pp, all, all) - vis(qq, all, all)); //  V / Q

        newvis(b, all, all) = (vis(pq, all, all) + vis(qp, all, all)); //  Q / U
        newvis(c, all, all) = (vis(pq, all, all) - vis(qp, all, all)) / Complex(0.0, 1.0); //  U / V
        newvis /= Complex(2.0);

        vis.reference(newvis);

        break;
    }
    case 2: {
        // parallel hands only
        Slice pp(0, 1, 1), qq(1, 1, 1);
        Slice a(0, 1, 1), d(1, 1, 1);

        newvis(a, all, all) = (vis(pp, all, all) + vis(qq, all, all)); //  I / I
        newvis(d, all, all) = (vis(pp, all, all) - vis(qq, all, all)); //  V / Q
        newvis /= Complex(2.0);

        vis.reference(newvis);

        break;
    }
    case 1: {
        // need do nothing for single correlation case
        break;
    }
    default: {
        cout << "Insufficient correlations to form Stokes" << endl;
        break;
    }
    }
}

void VisBuffer::formStokes(Cube<Float>& fcube)
{
    Cube<Float> newfcube(fcube.shape());
    newfcube.set(0.0);
    Slice all = Slice();

    switch (nCorr()) {
    case 4: {
        throw(AipsError(
                  "Forming all 4 Stokes parameters out of FLOAT_DATA is not supported."));

        Slice pp(0, 1, 1), pq(1, 1, 1), qp(2, 1, 1), qq(3, 1, 1);
        Slice a(0, 1, 1), b(1, 1, 1), c(2, 1, 1), d(3, 1, 1);

        if (polFrame() == MSIter::Linear) {
            d = Slice(1, 1, 1); // Q
            b = Slice(2, 1, 1); // U
            c = Slice(3, 1, 1); // V
        }

        newfcube(a, all, all) = (fcube(pp, all, all) + fcube(qq, all, all)); //  I / I
        newfcube(d, all, all) = (fcube(pp, all, all) - fcube(qq, all, all)); //  V / Q

        newfcube(b, all, all) = (fcube(pq, all, all) + fcube(qp, all, all)); //  Q / U

        ////  U / V
        // This clearly isn't going to work.  AFAICT it is impossible to
        // simultaneously measure all 4 polarizations with the same feed, so
        // FLOAT_DATA shouldn't come with 4 polarizations.
        //newfcube(c,all,all)=(fcube(pq,all,all)-fcube(qp,all,all))/Complex(0.0,1.0);
        newfcube(c, all, all) = (fcube(pq, all, all) - fcube(qp, all, all));

        // The cast is necessary to stop the compiler from promoting it to a Complex.
        newfcube *= static_cast<Float>(0.5);

        fcube.reference(newfcube);

        break;
    }
    case 2: {
        // parallel hands only
        Slice pp(0, 1, 1), qq(1, 1, 1);
        Slice a(0, 1, 1), d(1, 1, 1);

        newfcube(a, all, all) = (fcube(pp, all, all) + fcube(qq, all, all)); //  I / I
        newfcube(d, all, all) = (fcube(pp, all, all) - fcube(qq, all, all)); //  V / Q

        // The cast is necessary to stop the compiler from promoting it to a Complex.
        newfcube *= static_cast<Float>(0.5);

        fcube.reference(newfcube);

        break;
    }
    case 1: {
        // need do nothing for single correlation case
        break;
    }
    default: {
        cout << "Insufficient correlations to form Stokes" << endl;
        break;
    }
    }
}

void VisBuffer::channelAve(const Matrix<Int>& chanavebounds,Bool calmode)
{

    // TBD:
    //  a. nChanAve examination
    //  b. calmode=T  DONE
    //  c. doWtSp clauses and rowWtFac (not needed?)
    //  d. divide-by-zero safety  DONE
    //  e. no-averaging case   sigma<->weight


    //  Only do something if there is something to do
    if ( chanavebounds.nelements()>0 ) {

        // refer to the supplied chanavebounds
        chanAveBounds_p.reference(chanavebounds);

        // Examine chanAveBounds_p for consistency, detect nChanAve
        //   TBD: improve actual examination...
        Int nChanAve = abs(chanAveBounds_p(0,1)-chanAveBounds_p(0,0))+1;
        nChanAve = max(1,nChanAve);  // ensure >0

        Int nChanOut(chanAveBounds_p.nrow());
        Vector<Int> chans(channel()); // Ensure channels pre-filled

        // Row weight and sigma currently refer to the number of channels in the
        // MS, regardless of any selection.
        //Int nAllChan0 = visIter_p->msColumns().spectralWindow().numChan()(spectralWindow());

        const Bool doWtSp(visIter_p->existsWeightSpectrum());

        // Apply averaging to whatever data is present
        if (visCubeOK_p)          chanAveVisCube(visCube(),nChanOut);
        if (modelVisCubeOK_p)     chanAveVisCube(modelVisCube(),nChanOut);
        if (correctedVisCubeOK_p) chanAveVisCube(correctedVisCube(),nChanOut);
        if (floatDataCubeOK_p)    chanAveVisCube(floatDataCube(), nChanOut);

        uInt nCor = nCorr();
        uInt nrows = nRow();
        Matrix<Float> rowWtFac(nCor, nrows);

        uInt nch = flagCube().shape()(1);   // # of selected channels
        //Double selChanFac;

        if(doWtSp){                                    // Get the total weight spectrum
            const Cube<Float>& wtsp(weightSpectrum());   // while it is unaveraged.

            for(uInt row = 0; row < nrows; ++row){
                for(uInt icor = 0; icor < nCor; ++icor){
                    rowWtFac(icor, row) = 0.0;
                    for(uInt ichan = 0; ichan < nch; ++ichan)
                        // Presumably the input row weight was set without taking flagging
                        // into account.
                        rowWtFac(icor, row) += wtsp(icor, ichan, row);
                }
            }
        }
        else
            rowWtFac = 1.0;

        // The false makes it leave weightSpectrum() averaged if it is present.
        if(flagCubeOK_p)          chanAveFlagCube(flagCube(), nChanOut, false);

        if(flagCategoryOK_p)
            chanAveFlagCategory(flagCategory(), nChanOut);

        // Collapse the frequency values themselves, and count the number of
        // selected channels.
        // TBD: move this up to bounds calculation loop?
        Vector<Double> newFreq(nChanOut,0.0);
        Vector<Int> newChan(nChanOut,0);
        frequency(); // Ensure frequencies pre-filled
        Int nChan0(chans.nelements());
        Int ichan=0;
        Int totn = 0;
        for(Int ochan = 0; ochan < nChanOut; ++ochan){
            Int n = 0;

            while(chans[ichan] >= chanAveBounds_p(ochan, 0) &&
                    chans[ichan] <= chanAveBounds_p(ochan, 1) &&
                    ichan < nChan0){
                ++n;
                newFreq[ochan] += (frequency()[ichan] - newFreq[ochan]) / n;
                newChan[ochan] += chans[ichan];
                ichan++;
            }
            if (n>0) {
                newChan[ochan] /= n;
                totn += n;
            }
        }

        // Install the new values
        frequency().reference(newFreq);
        channel().reference(newChan);
        nChannel()=nChanOut;

        if(doWtSp){
            // chanAccCube(weightSpectrum(), nChanOut); already done.
            const Cube<Float>& wtsp(weightSpectrum());

            for(uInt row = 0; row < nrows; ++row){
                for(uInt icor = 0; icor < nCor; ++icor){
                    Float totwtsp = rowWtFac(icor, row);

                    rowWtFac(icor, row) = 0.0;
                    for(Int ochan = 0; ochan < nChanOut; ++ochan){
                        Float oswt = wtsp(icor, ochan, row);       // output spectral
                        // weight
                        if(oswt > 0.0)
                            rowWtFac(icor, row) += oswt;
                        else
                            flagCube()(icor, ochan, row) = true;
                    }
                    if(totwtsp > 0.0)
                        rowWtFac(icor, row) /= totwtsp;
                }
            }
        }
        // This is slightly fudgy because it ignores the unselected part of
        // weightSpectrum.  Unfortunately now that selection is applied to
        // weightSpectrum, we can't get at the unselected channel weights.
        //Float selChanFac = static_cast<Float>(totn) / nAllChan0;

        for(uInt row = 0; row < nrows; ++row){
            for(uInt icor = 0; icor < nCor; ++icor){
                // Float rwfcr = rowWtFac(icor, row);

                if (calmode) {

                    // Downstream processing in calibration will use weightMat only

                    // Just magnify by channal averaging factor
                    weightMat()(icor, row) *= nChanAve;

                    /*
	  if(totn < nAllChan0)
	    weightMat()(icor, row) *= selChanFac * rwfcr;
	  if(rwfcr > 0.0)          // Unlike WEIGHT, SIGMA is for a single chan.
	    sigmaMat()(icor, row) /= sqrt(nch * rwfcr / nChanOut);
                     */
                }
                else {


                    // selectively update sigma and weight info according to which
                    //   columns were processed

                    if (visCubeOK_p || floatDataCubeOK_p) {
                        // update sigmaMat by inverse sqrt of number of channels averaged
                        sigmaMat()(icor,row) /= sqrt(Double(nChanAve));   // nChanAve>0 already ensured

                        if (!correctedVisCubeOK_p) {
                            // calc weightMat from sigmaMat
                            Float &s= sigmaMat()(icor,row);
                            weightMat()(icor,row)= (s>0.0 ? 1./square(s) : FLT_EPSILON);
                        }
                    }

                    if (correctedVisCubeOK_p || modelVisCubeOK_p) {
                        // update weightMat
                        weightMat()(icor,row)*=Double(nChanAve);
                        if (!visCubeOK_p) {
                            // calc sigmaMat from weightMat
                            Float &w = weightMat()(icor,row);
                            sigmaMat()(icor,row)= ( w>0.0 ? 1./sqrt(w) : FLT_MAX );
                        }
                    }

                } // calmode

            }
        }
    } // chanavebounds

    // do we need to do something here that is datacolumn-specific to get sigma/weight reconciled?

}

void VisBuffer::chanAveFlagCube(Cube<Bool>& flagcube, Int nChanOut,
                                const Bool restoreWeightSpectrum)
{
  IPosition csh(flagcube.shape());
  Int nChan0 = csh(1);
  csh(1) = nChanOut;

  if(nChan0 < nChanOut)
    // It's possible that data has already been averaged.  I could try
    // refilling data if I knew which column to use, but I don't.
    // Chuck it to the caller.
    throw(AipsError("Can't average " + String(nChan0) + " channels to " +
                    String(nChanOut) + " channels!"));

  Vector<Int>& chans(channel());
  if(nChan0 == nChanOut && chans.nelements() > 0 && chans[0] == 0)
    return;    // No-op.

  Cube<Bool> newFlag(csh);
  newFlag = true;

  const Bool doWtSp(visIter_p->existsWeightSpectrum());

  // weightSpectrum could be either averaged or unaveraged.  Choose averaged.
  if(doWtSp && weightSpectrum().shape()[1] > nChanOut)
    chanAccCube(weightSpectrum(), nChanOut);

  Int nCor = nCorr();
  Int ichan;
  for (Int row=0; row<nRow(); row++) {
    if (!flagRow()(row)) {
      ichan = 0;
      for (Int ochan = 0; ochan < nChanOut; ++ochan) {
	while (chans[ichan] >= chanAveBounds_p(ochan, 0) &&
	       chans[ichan] <= chanAveBounds_p(ochan, 1) &&
	       ichan < nChan0) {
	  for(Int icor = 0; icor < nCor; ++icor){
	    if(!flagcube(icor, ichan, row)) 
	      newFlag(icor, ochan, row) = false;
          }
	  ++ichan;
	}
      }
    }
  }
  // Use averaged flags
  flagcube.reference(newFlag);

  if(doWtSp && restoreWeightSpectrum)
    fillWeightSpectrum();
}

void VisBuffer::chanAveFlagCategory(Array<Bool>& flagcat, const Int nChanOut)
{
  if(flagcat.nelements() == 0)      // Averaging a degenerate Array is fast,
    return;                         // and avoids the conformance check below.

  IPosition csh(flagcat.shape());
  Int nChan0 = csh(1);
  csh(1) = nChanOut;

  if(nChan0 < nChanOut)
    // It's possible that data has already been averaged.  I could try
    // refilling data if I knew which column to use, but I don't.
    // Chuck it to the caller.
    throw(AipsError("Can't average " + String(nChan0) + " channels to " +
                    String(nChanOut) + " channels!"));

  Vector<Int>& chans(channel());
  if(nChan0 == nChanOut && chans.nelements() > 0 && chans[0] == 0)
    return;    // No-op.

  Array<Bool> newFC(csh);
  newFC = true;

  Int nCor = nCorr();
  Int nCat = csh(2);
  Int ichan;
  for(Int row = 0; row < nRow(); ++row){
    ichan = 0;
    for(Int ochan = 0; ochan < nChanOut; ++ochan){
      while(chans[ichan] >= chanAveBounds_p(ochan, 0) &&
            chans[ichan] <= chanAveBounds_p(ochan, 1) &&
            ichan < nChan0) {
        for(Int icor = 0; icor < nCor; ++icor){
          for(Int icat = 0; icat < nCat; ++icat){
            if(!flagcat(IPosition(4, icor, ichan, icat, row))) 
              newFC(IPosition(4, icor, ochan, icat, row)) = false;
          }
        }
        ++ichan;
      }
    }
  }
  // Use averaged flag Categories
  flagcat.reference(newFC);
}

// Sort correlations: (PP,QQ,PQ,QP) -> (PP,PQ,QP,QQ)
void VisBuffer::sortCorr()
{

  // This method is for temporarily sorting the correlations
  //  into canonical order if the MS data is out-of-order
  // NB: Always sorts the weightMat()
  // NB: Only works on the visCube-style data
  // NB: It only sorts the data columns which are already present
  //     (so make sure the ones you need are already read!)
  // NB: It is the user's responsibility to run unSortCorr
  //     after using the sorted data to put it back in order
  // NB: corrType_p is NOT changed to match the sorted
  //     correlations (it is expected that this sort is
  //     temporary, and that we will run unSortCorr
  // NB: This method does nothing if no sort required

  // If nominal order is non-canonical (only for nCorr=4)
  //   and data not yet sorted
  if (nonCanonCorr() && !corrSorted_p) {


    // First, do weightMat
    {
      weightMat();    // (ensures it is filled)

      Vector<Float> wtmp(nRow());
      Vector<Float> w1, w2, w3;
      IPosition wblc(2, 0, 0), wtrc(2, 0, nRow() - 1), vec(1, nRow());

      wblc(0) = wtrc(0) = 1;
      w1.reference(weightMat_p(wblc, wtrc).reform(vec));
      wblc(0) = wtrc(0) = 2;
      w2.reference(weightMat_p(wblc, wtrc).reform(vec));
      wblc(0) = wtrc(0) = 3;
      w3.reference(weightMat_p(wblc, wtrc).reform(vec));
      wtmp = w1;
      w1 = w2;
      w2 = w3;
      w3 = wtmp;
    }

    // Now do data:

    // Work space, references, coords
    Matrix<Complex> tmp(nChannel(), nRow());
    Matrix<Complex> p1, p2, p3;
    IPosition blc(3, 0, 0, 0), trc(3, 0, nChannel() - 1, nRow() - 1), mat(2, nChannel(), nRow());

    // Do visCube if present
    if (visCubeOK_p && visCube_p.nelements() > 0) {
      blc(0) = trc(0) = 1;
      p1.reference(visCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 2;
      p2.reference(visCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 3;
      p3.reference(visCube_p(blc, trc).reform(mat));
      tmp = p1;
      p1 = p2;
      p2 = p3;
      p3 = tmp;
    }
    // Do modelVisCube if present
    if (modelVisCubeOK_p && modelVisCube_p.nelements() > 0) {
      blc(0) = trc(0) = 1;
      p1.reference(modelVisCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 2;
      p2.reference(modelVisCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 3;
      p3.reference(modelVisCube_p(blc, trc).reform(mat));
      tmp = p1;
      p1 = p2;
      p2 = p3;
      p3 = tmp;
    }
    // Do correctedVisCube if present
    if (correctedVisCubeOK_p && correctedVisCube_p.nelements() > 0) {
      blc(0) = trc(0) = 1;
      p1.reference(correctedVisCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 2;
      p2.reference(correctedVisCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 3;
      p3.reference(correctedVisCube_p(blc, trc).reform(mat));
      tmp = p1;
      p1 = p2;
      p2 = p3;
      p3 = tmp;
    }
    // Do floatDataCube if present
    if (floatDataCubeOK_p && floatDataCube_p.nelements() > 0) {
      Matrix<Float> tmp(nChannel(), nRow());
      Matrix<Float> p1, p2, p3;

      blc(0) = trc(0) = 1;
      p1.reference(floatDataCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 2;
      p2.reference(floatDataCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 3;
      p3.reference(floatDataCube_p(blc, trc).reform(mat));
      tmp = p1;
      p1 = p2;
      p2 = p3;
      p3 = tmp;
    }

    // Data is now sorted into canonical order
    corrSorted_p = true;
  }

}

// Unsort correlations: (PP,PQ,QP,QQ) -> (PP,QQ,PQ,QP)
void VisBuffer::unSortCorr()
{
  // This method is for restoring the non-canonical correlation
  //  sort order so that the data matches the order indicated
  //  by corrType()
  // NB: Always unsorts the weightMat()
  // NB: Only works on the visCube-style data
  // NB: It only unsorts the data columns which are already present
  //     (so make sure sortCorr sorted the ones you needed!)
  // NB: This method is a no-op if no sort required, or if
  //     sortCorr hadn't been run since the last unSortCorr

  // If nominal order is non-canonical (only for nCorr=4)
  //   and if data has been sorted
  if (nonCanonCorr() && corrSorted_p) {

    // First, do weights
    {
      Vector<Float> wtmp(nRow());
      Vector<Float> w1, w2, w3;
      IPosition wblc(1, 0, 0), wtrc(3, 0, nRow() - 1), vec(1, nRow());

      wblc(0) = wtrc(0) = 1;
      w1.reference(weightMat_p(wblc, wtrc).reform(vec));
      wblc(0) = wtrc(0) = 2;
      w2.reference(weightMat_p(wblc, wtrc).reform(vec));
      wblc(0) = wtrc(0) = 3;
      w3.reference(weightMat_p(wblc, wtrc).reform(vec));
      wtmp = w3;
      w3 = w2;
      w2 = w1;
      w1 = wtmp;
    }
    // Now do data:

    // Work space, references, coords
    Matrix<Complex> tmp(nChannel(), nRow());
    Matrix<Complex> p1, p2, p3;
    IPosition blc(3, 0, 0, 0), trc(3, 0, nChannel() - 1, nRow() - 1), mat(2, nChannel(), nRow());

    // Do visCube if present
    if (visCubeOK_p && visCube_p.nelements() > 0) {
      blc(0) = trc(0) = 1;
      p1.reference(visCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 2;
      p2.reference(visCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 3;
      p3.reference(visCube_p(blc, trc).reform(mat));
      tmp = p3;
      p3 = p2;
      p2 = p1;
      p1 = tmp;
    }
    // Do modelVisCube if present
    if (modelVisCubeOK_p && modelVisCube_p.nelements() > 0) {
      blc(0) = trc(0) = 1;
      p1.reference(modelVisCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 2;
      p2.reference(modelVisCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 3;
      p3.reference(modelVisCube_p(blc, trc).reform(mat));
      tmp = p3;
      p3 = p2;
      p2 = p1;
      p1 = tmp;
    }
    // Do correctedVisCube if present
    if (correctedVisCubeOK_p && correctedVisCube_p.nelements() > 0) {
      blc(0) = trc(0) = 1;
      p1.reference(correctedVisCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 2;
      p2.reference(correctedVisCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 3;
      p3.reference(correctedVisCube_p(blc, trc).reform(mat));
      tmp = p3;
      p3 = p2;
      p2 = p1;
      p1 = tmp;
    }
    // Do floatDataCube if present
    if (floatDataCubeOK_p && floatDataCube_p.nelements() > 0) {
      Matrix<Float> tmp(nChannel(), nRow());
      Matrix<Float> p1, p2, p3;

      blc(0) = trc(0) = 1;
      p1.reference(floatDataCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 2;
      p2.reference(floatDataCube_p(blc, trc).reform(mat));
      blc(0) = trc(0) = 3;
      p3.reference(floatDataCube_p(blc, trc).reform(mat));
      tmp = p3;
      p3 = p2;
      p2 = p1;
      p1 = tmp;
    }

    // Data is now back to corrType order
    corrSorted_p = false;
  }

}

Bool VisBuffer::nonCanonCorr()
{
  Vector<Int>& corrs(corrType());
  // Only a meaningful question is all 4 corrs present
  if (corrs.nelements() == 4)
    // (assumes corrs(0) is RR or XX)
    {
      return (corrs(1) == Stokes::LL || corrs(1) == Stokes::YY);
    } else
    // Assumed OK (fewer than 4 elements, or in canonical order already)
    {
      return false;
    }
}

// Fill weight matrix from sigma matrix
void VisBuffer::resetWeightMat()
{

  // fill sigmaMat_p, size weightMat_p storage
  sigmaMat();
  IPosition ip(sigmaMat_p.shape());
  weightMat_p.resize(ip);

  Int nPol(ip(0));
  Int nRow(ip(1));

  // Weight is inverse square of sigma (or zero[?])
  Float * w = weightMat_p.data();
  Float * s = sigmaMat_p.data();
  for (Int irow = 0; irow < nRow; ++irow)
    for (Int ipol = 0; ipol < nPol; ++ipol, ++w, ++s)
      if (*s > 0.0f) {
        *w = 1.0f / square(*s);
      } else {
        *w = 0.0f;
      }

  // As of 2014, we define wt = 1/sigma**2 (indep of nchan)
  // Scale by (unselected!) # of channels
  //  (to stay aligned with original nominal weights)
  //  Int nchan = msColumns().spectralWindow().numChan()(spectralWindow());
  //  weightMat_p *= Float(nchan);

  // weightMat_p now OK
  weightMatOK_p = true;

}


// Rotate visibility phase for phase center offsets
void VisBuffer::phaseCenterShift(const Vector<Double>& phase)
{

  AlwaysAssert(static_cast<Int>(phase.nelements()) == nRow(), AipsError);

  // phase is in metres 
  // phase*(-2*pi*f/c) gives phase for the channel of the given baseline in radian
  //   sign convention will _correct_ data

  Vector<Double> freq(frequency());
  Double ph, udx;
  Complex cph;

  for (Int irow = 0; irow < nRow(); ++irow){

    udx = phase(irow) * -2.0 * C::pi/C::c; // in radian/Hz

    for (Int ichn = 0; ichn < nChannel(); ++ichn) {
      // Calculate the Complex factor for this row and channel
      ph = udx * freq(ichn);

      if(ph!=0.){
	cph = Complex(cos(ph), sin(ph));
	// Shift each correlation:
	for (Int icor = 0; icor < nCorr(); ++icor) {
	  if (visCubeOK_p) {
	    visCube_p(icor, ichn, irow) *= cph;
	  }
	  if (modelVisCubeOK_p) {
	    modelVisCube_p(icor, ichn, irow) *= cph;
	  }
	  if (correctedVisCubeOK_p) {
	    correctedVisCube_p(icor, ichn, irow) *= cph;
	  }
	  // Of course floatDataCube does not have a phase to rotate.
	}
      }

    }
  }

}

// Rotate visibility phase for phase center offsets
void VisBuffer::phaseCenterShift(Double dx, Double dy)
{

  // no-op if no net shift
  if (!(abs(dx) > 0 || abs(dy) > 0)) {
    return;
  }

  // Offsets in radians (input is arcsec)
  dx *= (C::pi / 180.0 / 3600.0);
  dy *= (C::pi / 180.0 / 3600.0);

  // Extra path as fraction of U and V
  Vector<Double> udx;
  udx = uvwMat().row(0);
  Vector<Double> vdy;
  vdy = uvwMat().row(1);
  udx *= dx;  // in m
  vdy *= dy;

  // Combine axes
  udx += vdy;

  phaseCenterShift(udx);

}



// Divide visCube by modelVisCube
void VisBuffer::normalize(const Bool & /* phaseOnly */)
{

  // NB: phase-only now handled by SolvableVisCal
  //   (we will remove phaseOnly parameter later)


  // NB: Handles pol-dep weights in chan-indep way
  // TBD: optimizations?
  // TBD: Handle channel-dep weights?

  // Only if all relevant columns are present
  if (visCubeOK_p && modelVisCubeOK_p && weightMatOK_p) {

    //    cout << "Normalizing!----------------------------" << endl;

    Int nCor = nCorr();

    // Amplitude data
    Float amp(1.0);
    Vector<Float> ampCorr(nCor);
    Vector<Int> n(nCor);

    Bool * flR = flagRow().data();
    Bool * fl = flag().data();

    for (Int irow = 0; irow < nRow(); irow++, flR++) {
      if (!*flR) {
        ampCorr = 0.0f;
        n = 0;
        for (Int ich = 0; ich < nChannel(); ich++, fl++) {
          if (!*fl) {
            for (Int icorr = 0; icorr < nCor; icorr++) {
              amp = abs(modelVisCube_p(icorr, ich, irow));
              if (amp > 0.0f) {
                visCube_p(icorr, ich, irow) = Complex( DComplex(visCube_p(icorr, ich, irow)) /
                                                       DComplex(modelVisCube_p(icorr, ich, irow)) );

                modelVisCube_p(icorr, ich, irow) = Complex(1.0);
                ampCorr(icorr) += amp;
                n(icorr)++;
              } else
                // zero data if model is zero
                {
                  visCube_p(icorr, ich, irow) = 0.0;
                }
            }
          }
        }

        for (Int icorr = 0; icorr < nCor; icorr++) {
          if (n(icorr) > 0) {
            weightMat_p(icorr, irow) *= square(ampCorr(icorr) / Float(n(icorr)));
          } else {
            weightMat_p(icorr, irow) = 0.0f;
          }
        }

      } else {
        // Zero weight on this flagged row
        weightMat_p.column(irow) = 0.0f;

        // Advance fl over this row
        fl += nChannel();
      }
    }
  } else {
    throw(AipsError("Failed to normalize data by model!"));
  }
}

Vector<Int> VisBuffer::vecIntRange(const MSCalEnums::colDef & calEnum) const
{
  // Return a column range for a generic integer column as
  // identified by the enum specification in class MSCalEnums

  // Prepare the flag column masking
  LogicalArray mask(!flagRow());
  MaskedArray<Int>* maskArray;

  // A dummy vector for columns not yet supported (returns a value of [-1]);
  Vector<Int> nullIndex(antenna1().shape(), -1);

  switch (calEnum) {
    // ANTENNA1
  case MSC::ANTENNA1: {
    maskArray = new MaskedArray<Int>(antenna1(), mask);
    break;
  };
    // ANTENNA2
  case MSC::ANTENNA2: {
    maskArray = new MaskedArray<Int>(antenna2(), mask);
    break;
  };
    // FEED1
  case MSC::FEED1: {
    maskArray = new MaskedArray<Int>(feed1(), mask);
    break;
  };
    // FIELD_ID
  case MSC::FIELD_ID: {
    Vector<Int> fieldIdVec(antenna1().shape(), fieldId());
    maskArray = new MaskedArray<Int>(fieldIdVec, mask);
    break;
  };
    // ARRAY_ID
  case MSC::ARRAY_ID: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // OBSERVATION_ID
  case MSC::OBSERVATION_ID: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // SCAN_NUMBER
  case MSC::SCAN_NUMBER: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // PROCESSOR_ID
  case MSC::PROCESSOR_ID: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // PHASE_ID
  case MSC::PHASE_ID: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // STATE_ID
  case MSC::STATE_ID: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // PULSAR_BIN
  case MSC::PULSAR_BIN: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // PULSAR_GATE_ID
  case MSC::PULSAR_GATE_ID: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // FREQ_GROUP
  case MSC::FREQ_GROUP: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };
    // CALIBRATION_GROUP
  case MSC::CALIBRATION_GROUP: {
    maskArray = new MaskedArray<Int>(nullIndex, mask);
    break;
  };

  default: {
    throw(AipsError("Request for non-existent uv-data column"));
  };
  };

  // Return only unique indices
  Vector<Int> retval = unique(maskArray->getCompressedArray());
  if (maskArray) {
    delete(maskArray);
  }

  return retval;
};

Vector<Int> VisBuffer::antIdRange() const
{
  // Return a column range for ANTENNA_ID, including the
  // union of the ANTENNA1 and ANTENNA2 columns indices

  Vector<Int> ant1 = vecIntRange(MSC::ANTENNA1);
  Vector<Int> ant2 = vecIntRange(MSC::ANTENNA2);
  Vector<Int> ant12 = concatenateArray(ant1, ant2);

  // Return only unique index values
  return unique(ant12);
};

Bool VisBuffer::timeRange(MEpoch & rTime, MVEpoch & rTimeEP,
                          MVEpoch & rInterval) const
{
  // Return the time range of data in the vis buffer
  // (makes simplistic assumptions in the absence of
  // interval information for now)

  // Initialization
  Bool retval = false;

  if (nRow() > 0) {
    retval = true;
    LogicalArray mask(!flagRow());
    MaskedArray<Double> maskTime(time(), mask);
    Double minTime = min(maskTime);
    Double maxTime = max(maskTime);
    // Mean time
    rTime = MEpoch(Quantity((minTime + maxTime) / 2, "s"));
    // Extra precision time is always null for now
    rTimeEP = MVEpoch(Quantity(0, "s"));
    // Interval
    rInterval = MVEpoch(Quantity(maxTime - minTime, "s"));
  };
  return retval;
};


Vector<casacore::rownr_t>& VisBuffer::rowIds()
{
  if (!rowIdsOK_p) {

    rowIdsOK_p = true;
    visIter_p->rowIds(rowIds_p);
  }
  return rowIds_p;
}


void
VisBuffer::updateCoordInfo(const VisBuffer * vb, const  Bool dirDependent )
{
    updateCoord (vb, vb->antenna1OK (), & VisBuffer::antenna1, antenna1_p, antenna1OK_p);
    updateCoord (vb, vb->antenna2OK (), & VisBuffer::antenna2, antenna2_p, antenna2OK_p);
    updateCoordS (vb, vb->arrayIdOK (), & VisBuffer::arrayId, arrayId_p, arrayIdOK_p);
    updateCoordS (vb, vb->dataDescriptionIdOK(), & VisBuffer::dataDescriptionId, dataDescriptionId_p, dataDescriptionIdOK_p);
    updateCoordS (vb, vb->fieldIdOK (), & VisBuffer::fieldId, fieldId_p, fieldIdOK_p);
    updateCoordS (vb, vb->spectralWindowOK (), & VisBuffer::spectralWindow, spectralWindow_p, spectralWindowOK_p);
    updateCoord (vb, vb->timeOK (), & VisBuffer::time, time_p, timeOK_p);
    updateCoord (vb, vb->frequencyOK (), & VisBuffer::frequency, frequency_p, frequencyOK_p);
    updateCoordS (vb, vb->nRowOK (), & VisBuffer::nRow, nRow_p, nRowOK_p);

    vb->copyMsInfo (oldMSId_p, msOK_p, newMS_p);

    updateCoord (vb, vb->feed1OK (), & VisBuffer::feed1, feed1_p, feed1OK_p);
    updateCoord (vb, vb->feed2OK (), & VisBuffer::feed2, feed2_p, feed2OK_p);

    if(dirDependent){
        updateCoord (vb, vb->feed1_paOK (), & VisBuffer::feed1_pa, feed1_pa_p, feed1_paOK_p);
        updateCoord (vb, vb->feed2_paOK (), & VisBuffer::feed2_pa, feed2_pa_p, feed2_paOK_p);
        updateCoord (vb, vb->direction1OK (), & VisBuffer::direction1, direction1_p, direction1OK_p);
        updateCoord (vb, vb->direction2OK (), & VisBuffer::direction2, direction2_p, direction2OK_p);
    }

}

void VisBuffer::setVisCube(Complex c)
{
  visCube_p.resize(visIter_p->visibilityShape());
  visCube_p.set(c);
  visCubeOK_p = true;
}
void VisBuffer::setModelVisCube(Complex c)
{
  modelVisCube_p.resize(visIter_p->visibilityShape());
  modelVisCube_p.set(c);
  modelVisCubeOK_p = true;
}
void VisBuffer::setCorrectedVisCube(Complex c)
{
  correctedVisCube_p.resize(visIter_p->visibilityShape());
  correctedVisCube_p.set(c);
  correctedVisCubeOK_p = true;
}
void VisBuffer::setVisCube(const Cube<Complex>& vis)
{
  visCube_p.resize(vis.shape());
  visCube_p = vis;
  visCubeOK_p = true;
}
void VisBuffer::setModelVisCube(const Cube<Complex>& vis)
{
  modelVisCube_p.resize(vis.shape());
  modelVisCube_p = vis;
  modelVisCubeOK_p = true;
}

void VisBuffer::setModelVisCube(const Vector<Float>& stokes)
{

  /*
    cout << "Specified Stokes Parameters: " << stokes << endl;

    cout << "polFrame() = " << polFrame()
    << " " << MSIter::Circular
    << " " << MSIter::Linear
    << endl;
  */

  // Stokes parameters, nominally unpolarized, unit I
  Float I(1.0), Q(0.0), U(0.0), V(0.0);

  // Only fill as many as are specified, up to 4 (unspecified will be assumed zero)
  for (uInt i = 0; i < stokes.nelements(); ++i)
    switch (i) {
    case 0: {
      I = stokes(i);
      break;
    }
    case 1: {
      Q = stokes(i);
      break;
    }
    case 2: {
      U = stokes(i);
      break;
    }
    case 3: {
      V = stokes(i);
      break;
    }
    default: {
      break;
    }
    }

  // Convert to correlations, according to basis
  Vector<Complex> stkvis(4, Complex(0.0)); // initially all zero
  switch (polFrame()) {
  case MSIter::Circular: {
    stkvis(0) = Complex(I + V);
    stkvis(1) = Complex(Q, U);
    stkvis(2) = Complex(Q, -U);
    stkvis(3) = Complex(I - V);
    break;
  }
  case MSIter::Linear: {
    stkvis(0) = Complex(I + Q);
    stkvis(1) = Complex(U, V);
    stkvis(2) = Complex(U, -V);
    stkvis(3) = Complex(I - Q);
    break;
  }
  default:
    throw(AipsError("Model-setting only works for CIRCULAR and LINEAR bases, for now."));
    break;
  }

  // A map onto the actual correlations in the VisBuffer
  Vector<Int> corrmap;
  corrmap = corrType();
  corrmap -= corrmap(0);
  // This MUST yield indices < 4
  if (max(corrmap) > 3) {
    throw(AipsError("HELP! The correlations in the data are not normal!"));
  }


  // Set the modelVisCube accordingly
  modelVisCube_p.resize(visIter_p->visibilityShape());
  modelVisCube_p.set(0.0);
  for (Int icorr = 0; icorr < nCorr(); ++icorr)
    if (abs(stkvis(corrmap(icorr))) > 0.0) {
      modelVisCube_p(Slice(icorr, 1, 1), Slice(), Slice()).set(stkvis(corrmap(icorr)));
    }
  modelVisCubeOK_p = true;

  // Lookup flux density calibrator scaling, and apply it per channel...
  //  TBD


}

Int
VisBuffer::nRowChunk() const
{
    CheckVisIter ();
    return visIter_p->nRowChunk ();
}


Int VisBuffer::numberCoh () const
{
  CheckVisIter ();

  return visIter_p -> numberCoh ();
}


void
VisBuffer::checkVisIter (const char * func, const char * file, int line, const char * extra) const
{
  checkVisIterBase (func, file, line, extra);
}

void
VisBuffer::checkVisIterBase (const char * func, const char * file, int line, const char * extra) const
{
  if (visIter_p == NULL) {
    throw AipsError (String ("No VisibilityIterator is available to fill this field in (") +
                     func + extra + ")", file, line);
  }
}


void VisBuffer::setCorrectedVisCube(const Cube<Complex>& vis)
{
  correctedVisCube_p.resize(vis.shape());
  correctedVisCube_p = vis;
  correctedVisCubeOK_p = true;
}

void VisBuffer::setFloatDataCube(const Cube<Float>& fcube)
{
  floatDataCube_p.resize(fcube.shape());
  floatDataCube_p = fcube;
  floatDataCubeOK_p = true;
}

void VisBuffer::refModelVis(const Matrix<CStokesVector>& mvis)
{
  modelVisibility_p.resize();
  modelVisibility_p.reference(mvis);
  modelVisibilityOK_p = true;
}

void VisBuffer::removeScratchCols()
{
  // removes scratch data from the vb
  modelVisibility_p.resize();
  modelVisibilityOK_p = false;
  correctedVisibility_p.resize();
  correctedVisibilityOK_p = false;
}

Int& VisBuffer::fillnCorr()
{
  CheckVisIter ();
  nCorrOK_p = true;
  nCorr_p = corrType().nelements();
  return nCorr_p;
}

Vector<Int>& VisBuffer::fillObservationId()
{
  CheckVisIter();
  observationIdOK_p = true;
  return visIter_p->observationId(observationId_p);
}

Vector<Int>& VisBuffer::fillProcessorId()
{
  CheckVisIter();
  processorIdOK_p = true;
  return visIter_p->processorId(processorId_p);
}

Vector<Int>& VisBuffer::fillStateId()
{
  CheckVisIter();
  stateIdOK_p = true;
  return visIter_p->stateId(stateId_p);
}

Array<Bool>& VisBuffer::fillFlagCategory()
{
  CheckVisIter();
  flagCategoryOK_p = true;
  return visIter_p->flagCategory(flagCategory_p);
}

Int& VisBuffer::fillnChannel()
{
  CheckVisIter ();
  nChannelOK_p = true;
  //  nChannel_p=visIter_p->channelGroupSize();
  nChannel_p = channel().nelements();
  return nChannel_p;
}

Vector<Int>& VisBuffer::fillChannel()
{
  CheckVisIter ();
  channelOK_p = true;
  return visIter_p->channel(channel_p);
}

Int& VisBuffer::fillnRow()
{
  CheckVisIter ();
  nRowOK_p = true;
  nRow_p = visIter_p->nRow();
  return nRow_p;
}

Vector<Int>& VisBuffer::fillAnt1()
{
  CheckVisIter ();
  antenna1OK_p = true;
  return visIter_p->antenna1(antenna1_p);
}

Vector<Int>& VisBuffer::fillAnt2()
{
  CheckVisIter ();
  antenna2OK_p = true;
  return visIter_p->antenna2(antenna2_p);
}

Vector<Int>& VisBuffer::fillFeed1()
{
  CheckVisIter ();
  feed1OK_p = true;
  return visIter_p->feed1(feed1_p);
}

Vector<Int>& VisBuffer::fillFeed2()
{
  CheckVisIter ();
  feed2OK_p = true;
  return visIter_p->feed2(feed2_p);
}

Vector<SquareMatrix<Complex, 2> >& VisBuffer::fillCjones()
{
  CheckVisIter ();
  cjonesOK_p = true;
  return visIter_p->CJones(cjones_p);
}

Vector<Int>& VisBuffer::fillCorrType()
{
  CheckVisIter ();
  corrTypeOK_p = true;
  return visIter_p->corrType(corrType_p);
}

// calling fillFeed1_pa or fillFeed2_pa will fill antenna, feed
// and time caches automatically
Vector<Float>& VisBuffer::fillFeed1_pa()
{
  CheckVisIterBase ();

  // fill feed, antenna and time caches, if not filled before
  feed1();
  antenna1();
  time();
  feed1_paOK_p = true;
  feed1_pa_p.resize(antenna1_p.nelements()); // could also use nRow()

  // now actual calculations
  for (uInt row = 0; row < feed1_pa_p.nelements(); ++row) {
    const Vector<Float>& ant_pa = feed_pa(time_p(row)); // caching inside
    // ROVisibilityIterator, if the time doesn't change. Otherwise
    // we should probably fill both buffers for feed1 and feed2
    // simultaneously to speed up things.

    DebugAssert((uInt(antenna1_p(row)) < ant_pa.nelements()), AipsError);
    DebugAssert(antenna1_p(row) >= 0, AipsError);
    feed1_pa_p(row) = ant_pa(antenna1_p(row));
    // currently feed_pa returns only the first feed position angle
    // we need to add an offset if this row corresponds to a
    // different feed
    if (feed1_p(row))  // an if-statement to avoid unnecessary operations
      // in the single feed case, everything would
      // work without it.
      feed1_pa_p(row) += visIter_p->receptorAngles()(0,
                                                     antenna1_p(row), feed1_p(row)) -
        visIter_p->receptorAngles()(0, antenna1_p(row), 0);
  }
  return feed1_pa_p;
}

Vector<Float>& VisBuffer::fillFeed2_pa()
{
  CheckVisIterBase ();

  // fill feed, antenna and time caches, if not filled before
  feed2();
  antenna2();
  time();
  feed2_paOK_p = true;
  feed2_pa_p.resize(antenna2_p.nelements()); // could also use nRow()

  // now actual calculations
  for (uInt row = 0; row < feed2_pa_p.nelements(); ++row) {
    const Vector<Float>& ant_pa = feed_pa(time_p(row)); // caching inside
    // ROVisibilityIterator, if the time doesn't change. Otherwise
    // we should probably fill both buffers for feed1 and feed2
    // simultaneously to speed up things.
    DebugAssert((uInt(antenna2_p(row)) < ant_pa.nelements()), AipsError);
    DebugAssert(antenna2_p(row) >= 0, AipsError);
    feed2_pa_p(row) = ant_pa(antenna2_p(row));
    // currently feed_pa returns only the first feed position angle
    // we need to add an offset if this row correspods to a
    // different feed
    if (feed2_p(row))  // an if-statement to avoid unnecessary operations
      // in the single feed case, everything would
      // work without it.
      feed2_pa_p(row) += visIter_p->receptorAngles()(0,
                                                     antenna2_p(row), feed2_p(row)) -
        visIter_p->receptorAngles()(0, antenna2_p(row), 0);
  }
  return feed2_pa_p;
}

Vector<MDirection>& VisBuffer::fillDirection1()
{
  //Timer tim;
  //tim.mark();
  CheckVisIterBase ();
  // fill feed1_pa cache, antenna, feed and time will be filled automatically
  //feed1_pa();
  // commented above as it calculates the Par-angle for all antennas when
  //it may not be used at all...a 300% speedup on some large pointing table
  antenna1();
  feed1();
  time();
  direction1OK_p = true;
  firstDirection1OK_p=true;
  direction1_p.resize(antenna1_p.nelements()); // could also use nRow()
  const MSPointingColumns & mspc = msColumns().pointing();
  lastPointTableRow_p = mspc.pointingIndex(antenna1()(0),
                                           time()(0), lastPointTableRow_p);
  if (visIter_p->allBeamOffsetsZero() && lastPointTableRow_p < 0) {
    // if no true pointing information is found
    // just return the phase center from the field table
    direction1_p.set(phaseCenter());
    lastPointTableRow_p = 0;
    return direction1_p;
  }
  for (uInt row = 0; row < antenna1_p.nelements(); ++row) {
    DebugAssert(antenna1_p(row) >= 0, AipsError);
    DebugAssert(feed1_p(row) >= 0, AipsError);
    Int pointIndex1 = mspc.pointingIndex(antenna1()(row), time()(row), lastPointTableRow_p);

    //cout << "POINTINDEX " << pointIndex1 << endl;
    // if no true pointing information is found
    // use the phase center from the field table
    if (pointIndex1 >= 0) {
      lastPointTableRow_p = pointIndex1;
      direction1_p(row) = mspc.directionMeas(pointIndex1, timeInterval()(row));
    } else {
      direction1_p(row) = phaseCenter();
    }
    if (!visIter_p->allBeamOffsetsZero()) {
      //Now we can calculate the Par-angles
      feed1_pa();
      RigidVector<Double, 2> beamOffset =
        visIter_p->getBeamOffsets()(0, antenna1_p(row), feed1_p(row));
      if (visIter_p->antennaMounts()(antenna1_p(row)) == "ALT-AZ" ||
          visIter_p->antennaMounts()(antenna1_p(row)) == "alt-az") {
        SquareMatrix<Double, 2> xform(SquareMatrix<Double, 2>::General);
        // SquareMatrix' default constructor is a bit strange.
        // We will probably need to change it in the future
        Double cpa = cos(feed1_pa_p(row));
        Double spa = sin(feed1_pa_p(row));
        xform(0, 0) = cpa;
        xform(1, 1) = cpa;
        xform(0, 1) = -spa;
        xform(1, 0) = spa;
        beamOffset *= xform; // parallactic angle rotation
      }
      // x direction is flipped to convert az-el type frame to ra-dec
      direction1_p(row).shift(-beamOffset(0), beamOffset(1), true);
    }
  }
  //tim.show("fill dir1");
  //cerr << "allbeamOff " << visIter_p->allBeamOffsetsZero() << endl;
  firstDirection1_p=direction1_p[0];
  return direction1_p;
}

MDirection& VisBuffer::fillFirstDirection1()
{
  //Timer tim;
  //tim.mark();
  CheckVisIterBase ();
  // fill feed1_pa cache, antenna, feed and time will be filled automatically
  feed1();
  time();
  antenna1();
  
  //feed1_pa();
  firstDirection1OK_p=true;
  const MSPointingColumns & mspc = msColumns().pointing();
  lastPointTableRow_p = mspc.pointingIndex(antenna1()(0),
                                           time()(0), lastPointTableRow_p);
  if (visIter_p->allBeamOffsetsZero() && lastPointTableRow_p < 0) {
    // if no true pointing information is found
    // just return the phase center from the field table
    firstDirection1_p=phaseCenter();
    lastPointTableRow_p = 0;
    return firstDirection1_p;
  }

  Int pointIndex1=lastPointTableRow_p;

    //cout << "POINTINDEX " << pointIndex1 << endl;
    // if no true pointing information is found
    // use the phase center from the field table
  if (pointIndex1 >= 0) {
    
    firstDirection1_p = mspc.directionMeas(pointIndex1, timeInterval()(0));
  } else {
    firstDirection1_p = phaseCenter();
  }
  if (!visIter_p->allBeamOffsetsZero()) {
    feed1_pa();
    RigidVector<Double, 2> beamOffset =
      visIter_p->getBeamOffsets()(0, antenna1_p(0), feed1_p(0));
    if (visIter_p->antennaMounts()(antenna1_p(0)) == "ALT-AZ" ||
	visIter_p->antennaMounts()(antenna1_p(0)) == "alt-az") {
      SquareMatrix<Double, 2> xform(SquareMatrix<Double, 2>::General);
      // SquareMatrix' default constructor is a bit strange.
      // We will probably need to change it in the future
      Double cpa = cos(feed1_pa_p(0));
      Double spa = sin(feed1_pa_p(0));
      xform(0, 0) = cpa;
      xform(1, 1) = cpa;
      xform(0, 1) = -spa;
      xform(1, 0) = spa;
      beamOffset *= xform; // parallactic angle rotation
    }
    // x direction is flipped to convert az-el type frame to ra-dec
    firstDirection1_p.shift(-beamOffset(0), beamOffset(1), true);
    }
    
  return firstDirection1_p;
}
Vector<MDirection>& VisBuffer::fillDirection2()
{
  CheckVisIterBase ();
  // fill feed2_pa cache, antenna, feed and time will be filled automatically
  feed2_pa();
  direction2OK_p = true;
  direction2_p.resize(antenna2_p.nelements()); // could also use nRow()
  const MSPointingColumns & mspc = msColumns().pointing();
  lastPointTableRow_p = mspc.pointingIndex(antenna2()(0), time()(0), lastPointTableRow_p);
  if (visIter_p->allBeamOffsetsZero() && lastPointTableRow_p < 0) {
    // if no true pointing information is found
    // just return the phase center from the field table
    direction2_p.set(phaseCenter());
    lastPointTableRow_p = 0;
    return direction2_p;
  }
  for (uInt row = 0; row < antenna2_p.nelements(); ++row) {
    DebugAssert(antenna2_p(row) >= 0, AipsError);
    DebugAssert(feed2_p(row) >= 0, AipsError);
    Int pointIndex2 = mspc.pointingIndex(antenna2()(row), time()(row), lastPointTableRow_p);
    // if no true pointing information is found
    // use the phase center from the field table
    if (pointIndex2 >= 0) {
      lastPointTableRow_p = pointIndex2;
      direction2_p(row) = mspc.directionMeas(pointIndex2, timeInterval()(row));
    } else {
      direction2_p(row) = phaseCenter();
    }
    if (!visIter_p->allBeamOffsetsZero()) {
      RigidVector<Double, 2> beamOffset =
        visIter_p->getBeamOffsets()(0, antenna2_p(row), feed2_p(row));
      if (visIter_p->antennaMounts()(antenna2_p(row)) == "ALT-AZ" ||
          visIter_p->antennaMounts()(antenna2_p(row)) == "alt-az") {
        SquareMatrix<Double, 2> xform(SquareMatrix<Double, 2>::General);
        // SquareMatrix' default constructor is a bit strange.
        // We will probably need to change it in the future
        Double cpa = cos(feed2_pa_p(row));
        Double spa = sin(feed2_pa_p(row));
        xform(0, 0) = cpa;
        xform(1, 1) = cpa;
        xform(0, 1) = -spa;
        xform(1, 0) = spa;
        beamOffset *= xform; // parallactic angle rotation
      }
      // x direction is flipped to convert az-el type frame to ra-dec
      direction2_p(row).shift(-beamOffset(0), beamOffset(1), true);
    }
  }
  return direction2_p;
}

Int& VisBuffer::fillFieldId()
{
  CheckVisIter ();
  fieldIdOK_p = true;
  fieldId_p = visIter_p->fieldId();
  return fieldId_p;
}

Int& VisBuffer::fillArrayId()
{
  CheckVisIter ();
  arrayIdOK_p = true;
  arrayId_p = visIter_p->arrayId();
  return arrayId_p;
}

Int& VisBuffer::fillDataDescriptionId ()
{
  CheckVisIter ();
  dataDescriptionIdOK_p = true;
  dataDescriptionId_p = visIter_p->dataDescriptionId();
  return dataDescriptionId_p;
}

Matrix<Bool>& VisBuffer::fillFlag()
{
  CheckVisIter ();
  flagOK_p = true;
  return visIter_p->flag(flag_p);
}

Cube<Bool>& VisBuffer::fillFlagCube()
{
  CheckVisIter ();
  flagCubeOK_p = true;
  return visIter_p->flag(flagCube_p);
}

Vector<Bool>& VisBuffer::fillFlagRow()
{
  CheckVisIter ();
  flagRowOK_p = true;
  return visIter_p->flagRow(flagRow_p);
}

Vector<Int>& VisBuffer::fillScan()
{
  CheckVisIter ();
  scanOK_p = true;
  return visIter_p->scan(scan_p);
}

Vector<Double>& VisBuffer::fillFreq()
{
  CheckVisIter ();
  frequencyOK_p = true;
  return visIter_p->frequency(frequency_p);
}

//Vector<Double>& VisBuffer::fillLSRFreq()
//{
//  CheckVisIter ();
//  lsrFrequencyOK_p = true;
//  return visIter_p->lsrFrequency(lsrFrequency_p);
//}

MDirection& VisBuffer::fillPhaseCenter()
{
  CheckVisIter ();
  phaseCenterOK_p = true;
  return phaseCenter_p = visIter_p->phaseCenter();
}
const MDirection VisBuffer::phaseCenter(const Double time) const 
{
  CheckVisIter ();
  
  return This->phaseCenter(This->fieldId(), time);
}
const MDirection VisBuffer::phaseCenter(const Int field, const Double time) const 
{
  CheckVisIter ();
 
  return visIter_p->phaseCenter(field, time);
}

Int& VisBuffer::fillPolFrame()
{
  CheckVisIter ();
  polFrameOK_p = true;
  polFrame_p = visIter_p->polFrame();
  return polFrame_p;
}

Vector<Float>& VisBuffer::fillSigma()
{
  CheckVisIter ();
  sigmaOK_p = true;
  return visIter_p->sigma(sigma_p);
}

Matrix<Float>& VisBuffer::fillSigmaMat()
{
  CheckVisIter ();
  sigmaMatOK_p = true;
  return visIter_p->sigmaMat(sigmaMat_p);
}

Int& VisBuffer::fillSpW()
{
  CheckVisIter ();
  spectralWindowOK_p = true;
  spectralWindow_p = visIter_p->spectralWindow();
  return spectralWindow_p;
}

Vector<Double>& VisBuffer::fillTime()
{
  CheckVisIter ();
  timeOK_p = true;
  return visIter_p->time(time_p);
}

Vector<Double>& VisBuffer::fillTimeCentroid()
{
  CheckVisIter ();
  timeCentroidOK_p = true;
  return visIter_p->timeCentroid(timeCentroid_p);
}

Vector<Double>& VisBuffer::fillTimeInterval()
{
  CheckVisIter ();
  timeIntervalOK_p = true;
  return visIter_p->timeInterval(timeInterval_p);
}

Vector<Double>& VisBuffer::fillExposure()
{
  CheckVisIter ();
  exposureOK_p = true;
  return visIter_p->exposure(exposure_p);
}


Vector<RigidVector<Double, 3> >& VisBuffer::filluvw()
{
  CheckVisIter ();
  uvwOK_p = true;
  return visIter_p->uvw(uvw_p);
}

Matrix<Double>& VisBuffer::filluvwMat()
{
  CheckVisIter ();
  uvwMatOK_p = true;
  return visIter_p->uvwMat(uvwMat_p);
}

Matrix<CStokesVector>& VisBuffer::fillVis(VisibilityIterator::DataColumn whichOne)
{
  switch (whichOne) {
  case VisibilityIterator::Model:
    CheckVisIter1 (" (Model)");
    modelVisibilityOK_p = true;
    return visIter_p->visibility(modelVisibility_p, whichOne);
    break;
  case VisibilityIterator::Corrected:
    CheckVisIter1 (" (Corrected)");
    correctedVisibilityOK_p = true;
    return visIter_p->visibility(correctedVisibility_p, whichOne);
    break;
  case VisibilityIterator::Observed:
  default:
    CheckVisIter1 (" (Observed)");
    visibilityOK_p = true;
    return visIter_p->visibility(visibility_p, whichOne);
    break;
  }
}

Cube<Complex>& VisBuffer::fillVisCube(VisibilityIterator::DataColumn whichOne)
{
  switch (whichOne) {
  case VisibilityIterator::Model:
  {
	  CheckVisIter1 (" (Model)");
      modelVisCubeOK_p = true;
	  String modelkey; //=String("definedmodel_field_")+String::toString(fieldId());
	  Int snum;
	  Bool hasmodkey = False;
	  if (not visModelData_p.null()) hasmodkey = visModelData_p->isModelDefinedI(fieldId(), visIter_p->ms(), modelkey, snum);
	  if( hasmodkey || !(visIter_p->ms().tableDesc().isColumn("MODEL_DATA")))
	  {
	    
		  //cerr << "HASMOD " << visModelData_p.hasModel(msId(), fieldId(), spectralWindow()) << endl;
	    visModelData_p->init(*this);
	    ////This bit can be removed when we do not support old model vesions saved
	    if(!(visModelData_p->isVersion2())){
		  if(visModelData_p->hasModel(msId(), fieldId(), spectralWindow()) ==-1){
			  if(hasmodkey){
				  //String whichrec=visIter_p->ms().keywordSet().asString(modelkey);
				  TableRecord modrec;
				  if(visModelData_p->getModelRecordI(modelkey, modrec, visIter_p->ms())){
					  visModelData_p->addModel(modrec, Vector<Int>(1, msId()), *this);
				  }
			  }
		  }
	    }
	    ////////////////////////////
		  visModelData_p->getModelVis(*this);
	  }
	  else
	  {
		  visIter_p->visibility(modelVisCube_p, whichOne);
	  }
  }
    return modelVisCube_p;
    break;
  case VisibilityIterator::Corrected:
    CheckVisIter1 (" (Corrected)");
    correctedVisCubeOK_p = true;
    return visIter_p->visibility(correctedVisCube_p, whichOne);
    break;
  case VisibilityIterator::Observed:
  default:
    CheckVisIter1 (" (Observed)");
    visCubeOK_p = true;
    return visIter_p->visibility(visCube_p, whichOne);
    break;
  }
}

Cube<Float>& VisBuffer::fillFloatDataCube()
{
  CheckVisIter ();
  floatDataCubeOK_p = true;
  return visIter_p->floatData(floatDataCube_p);
}

Vector<Float>& VisBuffer::fillWeight()
{
  CheckVisIter ();
  weightOK_p = true;
  return visIter_p->weight(weight_p);
}

Matrix<Float>& VisBuffer::fillWeightMat()
{
  CheckVisIter ();
  weightMatOK_p = true;
  return visIter_p->weightMat(weightMat_p);
}

Cube<Float>& VisBuffer::fillWeightSpectrum()
{
  CheckVisIter ();
  weightSpectrumOK_p = true;
  return visIter_p->weightSpectrum(weightSpectrum_p);
}

//Matrix<Float>& VisBuffer::fillImagingWeight()
//{
//  CheckVisIter ();
//  imagingWeightOK_p = true;
//  return visIter_p->imagingWeight(imagingWeight_p);
//}

Vector<Float> VisBuffer::feed_pa(Double time) const
{
  return visIter_p->feed_pa(time);
}

Float VisBuffer::parang0(Double time) const
{
  return visIter_p->parang0(time);
}

Vector<Float> VisBuffer::parang(Double time) const
{
  return visIter_p->parang(time);
}

Int VisBuffer::numberAnt () const
{
  return msColumns().antenna().nrow(); /* for single (sub)array only.*/
}

MDirection VisBuffer::azel0(Double time) const
{
  return visIter_p->azel0(time);
}

Vector<Double>& VisBuffer::azel0Vec(Double time, Vector<Double>& azelVec) const
{
  MDirection azelMeas = This->azel0(time);
  azelVec.resize(2);
  azelVec = azelMeas.getAngle("deg").getValue();
  return azelVec;
}

Vector<MDirection> VisBuffer::azel(Double time) const
{
  return visIter_p->azel(time);
}

Matrix<Double>& VisBuffer::azelMat(Double time, Matrix<Double>& azelMat) const
{
  Vector<MDirection> azelMeas = This->azel(time);
  azelMat.resize(2, azelMeas.nelements());
  for (uInt iant = 0; iant < azelMeas.nelements(); ++iant) {
    azelMat.column(iant) = (azelMeas(iant).getAngle("deg").getValue());
  }
  return azelMat;

}

Double VisBuffer::hourang(Double time) const
{
  return visIter_p->hourang(time);
}

Vector<Int> VisBuffer::unique(const Vector<Int>& indices) const
{
  // Filter integer index arrays for unique values
  //
  uInt n = indices.nelements();
  Vector<Int> uniqIndices(n);
  if (n > 0) {
    // Sort temporary array in place
    Vector<Int> sortedIndices = indices.copy();
    GenSort<Int>::sort(sortedIndices);

    // Extract unique elements
    uniqIndices(0) = sortedIndices(0);
    uInt nUniq = 1;
    for (uInt i = 1; i < n; i++) {
      if (sortedIndices(i) != uniqIndices(nUniq - 1)) {
        uniqIndices(nUniq++) = sortedIndices(i);
      };
    };
    uniqIndices.resize(nUniq, true);
  };
  return uniqIndices;
}

void
VisBuffer::copyMsInfo (Int & msID, Bool & msOk, Bool & newMs) const
{
    msID = msId();
    msOk = msOK_p;
    newMs = newMS_p;
}


Bool VisBuffer::checkMSId()
{
  //if this is not a new iteration then don't even check;
  //Let the state be
  if (msOK_p) {
    return false;
  }

  if (visIter_p != static_cast<ROVisibilityIterator *> (0)) {

    if (oldMSId_p != visIter_p->msId()) {

      oldMSId_p = visIter_p->msId();
      newMS_p = true;
    } else {
      newMS_p = false;
    }

    msOK_p = true;
    return newMS_p;

  } else {
    return newMS_p;
  }

  return false;
}

VisBuffer *
VisBuffer::clone () const
{
    return new VisBuffer (* this);
}

void
VisBuffer::dirtyComponentsAdd (const VbDirtyComponents & dirtyComponents)
{
    dirtyComponents_p = dirtyComponents_p + dirtyComponents;
}

void
VisBuffer::dirtyComponentsAdd (VisBufferComponents::EnumType component)
{
    dirtyComponents_p = dirtyComponents_p + VbDirtyComponents::singleton (component);
}


void
VisBuffer::dirtyComponentsClear ()
{
    dirtyComponents_p = VbDirtyComponents::none();
}

VbDirtyComponents
VisBuffer::dirtyComponentsGet () const
{
    return dirtyComponents_p;
}


void
VisBuffer::dirtyComponentsSet (const VbDirtyComponents & dirtyComponents)
{
    dirtyComponents_p = dirtyComponents;
}

void
VisBuffer::dirtyComponentsSet (VisBufferComponents::EnumType component)
{
    dirtyComponents_p = VbDirtyComponents::singleton (component);
}

Bool VisBuffer::fetch(const asyncio::PrefetchColumns *pfc)
{
  Bool success = true;

  for(asyncio::PrefetchColumns::const_iterator c = pfc->begin();
      c != pfc->end(); ++c){
    switch(*c){
    case VisBufferComponents::Ant1:
      This->antenna1();
      break;
    case VisBufferComponents::Ant2:
      This->antenna2();
      break;
    case VisBufferComponents::ArrayId:
      This->arrayId();
      break;
    case VisBufferComponents::Channel:
      This->channel();
      break;
    case VisBufferComponents::CorrType:
      This->corrType();
      break;
    case VisBufferComponents::Corrected:
      This->correctedVisibility();
      break;
    case VisBufferComponents::CorrectedCube:
      This->correctedVisCube();
      break;
    case VisBufferComponents::DataDescriptionId:
      This->dataDescriptionId();
      break;
    case VisBufferComponents::Direction1:
      This->direction1();
      break;
    case VisBufferComponents::Direction2:
      This->direction2();
      break;
    case VisBufferComponents::Exposure:
      This->exposure();
      break;
    case VisBufferComponents::Feed1:
      This->feed1();
      break;
    case VisBufferComponents::Feed1_pa:
      This->feed1_pa();
      break;
    case VisBufferComponents::Feed2:
      This->feed2();
      break;
    case VisBufferComponents::Feed2_pa:
      This->feed2_pa();
      break;
    case VisBufferComponents::FieldId:
      This->fieldId();
      break;
    case VisBufferComponents::Flag:
      This->flag();
      break;
    case VisBufferComponents::FlagCube:
      This->flagCube();
      break;
    case VisBufferComponents::FlagCategory:
      This->flagCategory();
      break;
    case VisBufferComponents::FlagRow:
      This->flagRow();
      break;
    case VisBufferComponents::Freq:
      This->frequency();
      break;
    case VisBufferComponents::ImagingWeight:
      // This->imagingweight();                // do not fill this one
      break;
    case VisBufferComponents::Model:
      This->modelVisibility();
      break;
    case VisBufferComponents::ModelCube:
      This->modelVisCube();
      break;
    case VisBufferComponents::NChannel:
      This->nChannel();
      break;
    case VisBufferComponents::NCorr:
      This->nCorr();
      break;
    case VisBufferComponents::NRow:
      This->nRow();
      break;
    case VisBufferComponents::ObservationId:
      This->observationId();
      break;
    case VisBufferComponents::Observed:
      This->visibility();
      break;
    case VisBufferComponents::ObservedCube:
      This->visCube();
      break;
    case VisBufferComponents::PhaseCenter:
      This->phaseCenter();
      break;
    case VisBufferComponents::PolFrame:
      This->polFrame();
      break;
    case VisBufferComponents::ProcessorId:
      This->processorId();
      break;
    case VisBufferComponents::Scan:
      This->scan();
      break;
    case VisBufferComponents::Sigma:
      This->sigma();
      break;
    case VisBufferComponents::SigmaMat:
      This->sigmaMat();
      break;
    case VisBufferComponents::SpW:
      This->spectralWindow();
      break;
    case VisBufferComponents::StateId:
      This->stateId();
      break;
    case VisBufferComponents::Time:
      This->time();
      break;
    case VisBufferComponents::TimeCentroid:
      This->timeCentroid();
      break;
    case VisBufferComponents::TimeInterval:
      This->timeInterval();
      break;
    case VisBufferComponents::Uvw:
      This->uvw();
      break;
    case VisBufferComponents::UvwMat:
      This->uvwMat();
      break;
    case VisBufferComponents::Weight:
      This->weight();
      break;
    case VisBufferComponents::WeightMat:
      This->weightMat();
      break;
    case VisBufferComponents::WeightSpectrum:
      This->weightSpectrum();
      break;
    default:
      throw(AipsError("Unrecognized column type"));
    }
  }
  return success;
}

VisBufferAutoPtr::VisBufferAutoPtr ()
{
    visBuffer_p = NULL;
}

VisBufferAutoPtr::VisBufferAutoPtr (VisBufferAutoPtr & other)
{
    // Take ownership of the other's object

    visBuffer_p = other.visBuffer_p;
    other.visBuffer_p = NULL;
}

VisBufferAutoPtr::VisBufferAutoPtr (VisBuffer & vb)
{
    constructVb (& vb);
}

VisBufferAutoPtr::VisBufferAutoPtr (VisBuffer * vb)
{
    constructVb (vb);
}

VisBufferAutoPtr::VisBufferAutoPtr (ROVisibilityIterator & rovi)
{
    construct (& rovi, true);
}


VisBufferAutoPtr::VisBufferAutoPtr (ROVisibilityIterator * rovi)
{
    construct (rovi, true);
}

VisBufferAutoPtr::~VisBufferAutoPtr ()
{
    delete visBuffer_p;
}

VisBufferAutoPtr &
VisBufferAutoPtr::operator= (VisBufferAutoPtr & other)
{
    if (this != & other){

        delete visBuffer_p;  // release any currently referenced object

        // Take ownership of the other's object

        visBuffer_p = other.visBuffer_p;
        other.visBuffer_p = NULL;
    }

    return * this;
}

VisBuffer &
VisBufferAutoPtr::operator* () const
{
    assert (visBuffer_p != NULL);

    return * visBuffer_p;
}

VisBuffer *
VisBufferAutoPtr::operator-> () const
{
    assert (visBuffer_p != NULL);

    return visBuffer_p;
}

void
VisBufferAutoPtr::construct (ROVisibilityIterator * rovi, Bool attachVi)
{
    if (rovi->isAsynchronous ()){

        // Create an asynchronous VisBuffer

        VisBufferAsyncWrapper * vba;

        if (attachVi){
            vba = new VisBufferAsyncWrapper (* rovi);
        }
        else{
            vba = new VisBufferAsyncWrapper ();
        }

        visBuffer_p = vba;
    }
    else{

        // This is a synchronous VI so just create a synchronous VisBuffer.

        if (attachVi){
            visBuffer_p = new VisBuffer (* rovi);
        }
        else{
            visBuffer_p = new VisBuffer ();
        }
    }
}

void
VisBufferAutoPtr::constructVb (VisBuffer * vb)
{
    VisBufferAsync * vba = dynamic_cast<VisBufferAsync *> (vb);

    if (vba != NULL){

        // Create an asynchronous VisBuffer

        VisBufferAsyncWrapper * vbaNew = new VisBufferAsyncWrapper (* vba);

        visBuffer_p = vbaNew;
    }
    else{

        // This is a synchronous VI so just create a synchronous VisBuffer.

        visBuffer_p = new VisBuffer (* vb);
    }
}

VisBuffer *
VisBufferAutoPtr::get () const
{
    return visBuffer_p;
}




VisBuffer *
VisBufferAutoPtr::release ()
{
    VisBuffer * result = visBuffer_p;
    visBuffer_p = NULL;

    return result;
}


void
VisBufferAutoPtr::set (VisBuffer & vb)
{
    delete visBuffer_p;
    visBuffer_p = & vb;
}

void
VisBufferAutoPtr::set (VisBuffer * vb)
{
    delete visBuffer_p;
    visBuffer_p = vb;
}

void
VisBufferAutoPtr::set (ROVisibilityIterator * rovi, Bool attachIt)
{
    delete visBuffer_p;
    construct (rovi, attachIt);
}

void
VisBufferAutoPtr::set (ROVisibilityIterator & rovi, Bool attachIt)
{
    set (& rovi, attachIt);
}

const VbDirtyComponents VbDirtyComponents::all_p = initializeAll ();

VbDirtyComponents
VbDirtyComponents::operator+ (const VbDirtyComponents & other) const
{
    VbDirtyComponents result = * this;

    result.set_p.insert (other.begin(), other.end());

    return result;
}



VbDirtyComponents::const_iterator
VbDirtyComponents::begin () const
{
    return set_p.begin();
}

Bool
VbDirtyComponents::contains (VisBufferComponents::EnumType component) const
{
    return utilj::containsKey (component, set_p);
}

VbDirtyComponents::const_iterator
VbDirtyComponents::end () const
{
    return set_p.end();
}

VbDirtyComponents
VbDirtyComponents::all ()
{
    return all_p;
}

VbDirtyComponents
VbDirtyComponents::exceptThese (VisBufferComponents::EnumType component, ...)
{
    va_list vaList;

    va_start (vaList, component);

    VisBufferComponents::EnumType c = component;
    VbDirtyComponents dirtyComponents = all();

    while (c != VisBufferComponents::Unknown){

        ThrowIf (! all().contains (c), "Not a writable VB component: " + String::toString (c));

        dirtyComponents.set_p.erase (c);
        c = (VisBufferComponents::EnumType) va_arg (vaList, Int);
    }

    va_end (vaList);

    return dirtyComponents;

}

VbDirtyComponents
VbDirtyComponents::initializeAll ()
{

    VbDirtyComponents all;

    VisBufferComponents::EnumType
    writableComponents [] = {VisBufferComponents::Corrected,
                             VisBufferComponents::CorrectedCube,
                             VisBufferComponents::Flag,
                             VisBufferComponents::FlagCube,
                             VisBufferComponents::FlagRow,
                             VisBufferComponents::Model,
                             VisBufferComponents::ModelCube,
                             VisBufferComponents::Observed,
                             VisBufferComponents::ObservedCube,
                             VisBufferComponents::Sigma,
                             VisBufferComponents::SigmaMat,
                             VisBufferComponents::Weight,
                             VisBufferComponents::WeightMat,
                             VisBufferComponents::Unknown};

    for (Int i = 0; ; i++){

        if (writableComponents [i] == VisBufferComponents::Unknown){
            break;
        }

        all.set_p.insert (writableComponents [i]);
    }

    return all;
}

VbDirtyComponents
VbDirtyComponents::none ()
{
    return VbDirtyComponents ();
}

VbDirtyComponents
VbDirtyComponents::singleton (VisBufferComponents::EnumType component)
{
    ThrowIf (! all().contains (component), "Not a writable VB component.");
    VbDirtyComponents result;
    result.set_p.insert (component);

    return result;
}

VbDirtyComponents
VbDirtyComponents::these (VisBufferComponents::EnumType component, ...)
{
    va_list vaList;

    va_start (vaList, component);

    VisBufferComponents::EnumType c = component;
    VbDirtyComponents dirtyComponents;

    while (c != VisBufferComponents::Unknown){

        ThrowIf (! all().contains (c), "Not a writable VB component: " + String::toString (c));

        dirtyComponents.set_p.insert (c);
        c = (VisBufferComponents::EnumType ) va_arg (vaList, Int);
    }

    va_end (vaList);

    return dirtyComponents;
}




} //# NAMESPACE CASA - END

