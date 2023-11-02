/*
 * VisBuffer2Adapter.h
 *
 *  Created on: Oct 9, 2012
 *      Author: jjacobs
 */

#ifndef VISBUFFER2ADAPTER_H_
#define VISBUFFER2ADAPTER_H_

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisBufferImpl2.h>
#include <casacore/casa/BasicSL/String.h>
#include <stdcasa/UtilJ.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>

using casa::utilj::toStdError;

#define IllegalOperation() \
    Throw (casacore::String::format ("Illegal operation: %s", __func__));

#define CheckWritability() \
    ThrowIf (vb2Rw_p == 0, casacore::String::format ("VB is not writable in %s", __func__));

namespace casa {

namespace vi {

class VisBuffer2Adapter : public VisBuffer {

public:

    VisBuffer2Adapter (VisBuffer2 * vb) : vb2_p (vb) , vb2Rw_p (vb)
    {
        construct();
    }
    VisBuffer2Adapter (const VisBuffer2 * vb) : msColumns_p (0), vb2_p (vb), vb2Rw_p (0)
    {
        construct();
    }

    void
    construct ()
    {
        const VisibilityIteratorImpl2 * vi =
	        dynamic_cast<const VisibilityIteratorImpl2 *>(
		        dynamic_cast<const VisBufferImpl2 *>(vb2_p)->getViiP());

        msColumns_p = vi->msColumnsKluge();

        vi->allSpectralWindowsSelected (spectralWindows_p, nChannels_p);

    }

    ~VisBuffer2Adapter () {}

    virtual VisBuffer & assign(const VisBuffer &, casacore::Bool = true) {IllegalOperation();}

    virtual VisBuffer * clone () const {IllegalOperation();}

    virtual void invalidate(){IllegalOperation();}

//#if 0

    // <group>
    // Access functions
    //
    virtual casacore::Int & nCorr() { nCorr_p = vb2_p->nCorrelations(); return nCorr_p;}
    virtual casacore::Int nCorr() const { return vb2_p-> nCorrelations(); }

    virtual casacore::Int & nChannel() { nChannelsScalar_p = vb2_p-> nChannels(); return nChannelsScalar_p;}
    virtual casacore::Int nChannel() const { return vb2_p-> nChannels(); }

    virtual casacore::Vector<casacore::Int>& channel() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::Int>& channel() const { IllegalOperation(); }

    virtual casacore::Int & nRow() { nRows_p = vb2_p->nRows (); return nRows_p;}

    virtual casacore::Vector<casacore::Int>& antenna1() { return const_cast<casacore::Vector<casacore::Int>&> (vb2_p-> antenna1());}
    virtual const casacore::Vector<casacore::Int>& antenna1() const { return vb2_p-> antenna1(); }

    virtual casacore::Vector<casacore::Int>& antenna2() { return const_cast<casacore::Vector<casacore::Int>&> (vb2_p-> antenna2());}
    virtual const casacore::Vector<casacore::Int>& antenna2() const { return vb2_p-> antenna2(); }

    virtual casacore::Vector<casacore::Int>& feed1() { return const_cast<casacore::Vector<casacore::Int>&> (vb2_p-> feed1());}
    virtual const casacore::Vector<casacore::Int>& feed1() const { return vb2_p-> feed1(); }

    virtual casacore::Vector<casacore::Int>& feed2() { return const_cast<casacore::Vector<casacore::Int>&> (vb2_p-> feed2());}
    virtual const casacore::Vector<casacore::Int>& feed2() const { return vb2_p-> feed2(); }

    // feed1_pa() and feed2_pa() return an array of parallactic angles
    // (each corresponds to the first receptor of the feed) one for each
    // row in the current buffer. In contrast, feed_pa() calculates
    // the angles for each antenna. These methods are implemented for
    // VisBuffer only to benefit from caching of the feed and antenna IDs.

    virtual casacore::Vector<casacore::Float>& feed1_pa() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::Float>& feed1_pa() const { return vb2_p-> feedPa1(); }

    virtual casacore::Vector<casacore::Float>& feed2_pa() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::Float>& feed2_pa() const { return vb2_p-> feedPa2(); }

    virtual casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& CJones() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& CJones() const { return vb2_p-> cjones(); }

    // Note that feed_pa is a function instead of a cached value
    virtual casacore::Vector<casacore::Float> feed_pa(casacore::Double time) const{ return vb2_p-> feedPa (time); }

    // direction1() and direction2() return arrays of directions where
    // the first and the second antenna/feed are pointed to. One value for
    // each row in the current buffer.
    virtual casacore::Vector<casacore::MDirection>& direction1() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::MDirection>& direction1()  const { return vb2_p-> direction1(); }

    virtual casacore::Vector<casacore::MDirection>& direction2() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::MDirection>& direction2()  const { return vb2_p-> direction2(); }


    // NOMINAL parallactic angle (feed p.a. offset NOT included)
    virtual casacore::Float parang0(casacore::Double time) const{ return vb2_p-> parang0(time); }
    virtual casacore::Vector<casacore::Float> parang(casacore::Double time) const{ return vb2_p-> parang(time); }

    // Note that azel is a function instead of a cached value
    virtual casacore::MDirection azel0(casacore::Double time) const{ return vb2_p-> azel0(time); }
    virtual casacore::Vector<casacore::Double>& azel0Vec(casacore::Double , casacore::Vector<casacore::Double>& ) const{ IllegalOperation(); }
    virtual casacore::Vector<casacore::MDirection> azel(casacore::Double time) const{ return vb2_p-> azel(time); }
    virtual casacore::Matrix<casacore::Double>& azelMat(casacore::Double , casacore::Matrix<casacore::Double>& ) const{ IllegalOperation(); }

    // Hour angle for specified time
    virtual casacore::Double hourang(casacore::Double time) const{ return vb2_p-> hourang(time); }

    virtual casacore::Int fieldId() const { return vb2_p-> fieldId()(0); }

    virtual casacore::Int& fieldIdRef() { IllegalOperation(); }

    virtual casacore::Int& arrayIdRef() { IllegalOperation(); }

    virtual casacore::Int arrayId() const { return vb2_p-> arrayId()(0); }

    // Return flag for each channel & row
    virtual casacore::Matrix<casacore::Bool>& flag() { IllegalOperation(); }
    virtual const casacore::Matrix<casacore::Bool>& flag() const { IllegalOperation(); }

    // Return flag for each polarization, channel and row
    virtual casacore::Cube<casacore::Bool>& flagCube() { return const_cast<casacore::Cube<casacore::Bool>&> (vb2_p-> flagCube());}
    virtual const casacore::Cube<casacore::Bool>& flagCube() const { return vb2_p-> flagCube(); }

    virtual casacore::Vector<casacore::Bool>& flagRow() { return const_cast<casacore::Vector<casacore::Bool>&> (vb2_p-> flagRow());}
    virtual const casacore::Vector<casacore::Bool>& flagRow() const { return vb2_p-> flagRow(); }

    // Return flags for each polarization, channel, category, and row.
    virtual casacore::Array<casacore::Bool>& flagCategory() { IllegalOperation (); }
    virtual const casacore::Array<casacore::Bool>& flagCategory() const { return vb2_p-> flagCategory(); }

    virtual casacore::Vector<casacore::Int>& scan() { return const_cast<casacore::Vector<casacore::Int>&> (vb2_p-> scan());}
    virtual const casacore::Vector<casacore::Int>& scan() const { return vb2_p-> scan(); }

    // scalar version for convenience, when scan known constant for
    // entire iteration/buffer.
    virtual casacore::Int scan0() { IllegalOperation(); }

    virtual casacore::Vector<casacore::Int>& processorId() { return const_cast<casacore::Vector<casacore::Int>&> (vb2_p-> processorId());}
    virtual const casacore::Vector<casacore::Int>& processorId() const { return vb2_p-> processorId(); }

    virtual casacore::Vector<casacore::Int>& observationId() { return const_cast<casacore::Vector<casacore::Int>&> (vb2_p-> observationId());}
    virtual const casacore::Vector<casacore::Int>& observationId() const { return vb2_p-> observationId(); }

    virtual casacore::Vector<casacore::Int>& stateId() { return const_cast<casacore::Vector<casacore::Int>&> (vb2_p-> stateId());}
    virtual const casacore::Vector<casacore::Int>& stateId() const { return vb2_p-> stateId(); }

    // Gets SPECTRAL_WINDOW/CHAN_FREQ (in Hz, acc. to the casacore::MS def'n v.2).
    virtual casacore::Vector<casacore::Double>& frequency() {
        return const_cast<casacore::Vector<casacore::Double>&> (vb2_p->getFrequencies(0));
    }
    virtual const casacore::Vector<casacore::Double>& frequency() const
    {
        return vb2_p->getFrequencies(0);
    }

    //if ignoreconv=true..frequency is served as is in the data frame
    virtual void lsrFrequency(const casacore::Int & spw, casacore::Vector<casacore::Double>& freq, casacore::Bool & convert,
                              const casacore::Bool ignoreConv = false) const
    {
        const VisibilityIteratorImpl2 * vi =
	        dynamic_cast<const VisibilityIteratorImpl2 *>(
		        dynamic_cast<const VisBufferImpl2 *>(vb2_p)->getViiP());

        casacore::Int frame = -1;
        if (ignoreConv){
            frame = vi->getObservatoryFrequencyType ();
        }
        else{
            frame = casacore::MFrequency::LSRK;
        }

        convert = frame != casacore::MFrequency::LSRK; // make this parameter write-only

        freq = vi->getFrequencies (this->time()(0), frame, spw, vi->msId());
    }

    virtual casacore::Int numberCoh () const { IllegalOperation(); }

    virtual casacore::MDirection & phaseCenter() { IllegalOperation (); }
    virtual casacore::MDirection phaseCenter() const { return vb2_p-> phaseCenter(); }

    virtual casacore::Int polFrame() const { return vb2_p-> polarizationFrame(); }

    virtual casacore::Vector<casacore::Int>& corrType() { return const_cast<casacore::Vector<casacore::Int> &> (vb2_p-> correlationTypes()); }
    virtual const casacore::Vector<casacore::Int>& corrType() const { return vb2_p-> correlationTypes(); }

    virtual casacore::Vector<casacore::Float>& sigma() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::Float>& sigma() const { IllegalOperation(); }

    virtual casacore::Matrix<casacore::Float>& sigmaMat() { IllegalOperation (); }
    virtual const casacore::Matrix<casacore::Float>& sigmaMat() const { return vb2_p-> sigma(); }

    virtual casacore::Int & spectralWindow() { spectralWindow_p = vb2_p->spectralWindows()(0); return spectralWindow_p;}
    virtual casacore::Int spectralWindow() const { return vb2_p-> spectralWindows()(0); }
    virtual casacore::Int polarizationId() const { return vb2_p-> polarizationId(); }
    virtual casacore::Int& dataDescriptionIdRef() { IllegalOperation(); }
    virtual casacore::Int dataDescriptionId() const { return vb2_p-> dataDescriptionIds()(0); }
    virtual casacore::Vector<casacore::Double>& time() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::Double>& time() const { return vb2_p-> time(); }

    virtual casacore::Vector<casacore::Double>& timeCentroid() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::Double>& timeCentroid() const { return vb2_p-> timeCentroid(); }

    virtual casacore::Vector<casacore::Double>& timeInterval() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::Double>& timeInterval() const { return vb2_p-> timeInterval(); }

    virtual casacore::Vector<casacore::Double>& exposure() { IllegalOperation (); }
    virtual const casacore::Vector<casacore::Double>& exposure() const { return vb2_p-> exposure(); }

    virtual casacore::Vector<casacore::RigidVector<casacore::Double, 3> >& uvw()
    {
        if (uvw_p.empty()){

            fillUvw ();

        }

        return uvw_p;
    }

    virtual const casacore::Vector<casacore::RigidVector<casacore::Double, 3> >& uvw() const
    {
        if (uvw_p.empty()){

            fillUvw ();

        }

        return uvw_p;
    }

    void
    fillUvw() const {

        const casacore::Matrix<casacore::Double> & u = vb2_p->uvw();

        casacore::Int nRows = u.shape()(1);
        uvw_p.resize (nRows);

        for (casacore::Int i = 0; i < nRows; i++){
            casacore::RigidVector<casacore::Double,3> t;
            for (casacore::Int j = 0; j < 3; j++){
                t (j) = u (j, i);
            }
            uvw_p (i) = t;
        }
    }


    virtual casacore::Matrix<casacore::Double>& uvwMat() { IllegalOperation (); }
    virtual const casacore::Matrix<casacore::Double>& uvwMat() const { return vb2_p-> uvw(); }

    virtual casacore::Matrix<CStokesVector>& visibility() { IllegalOperation (); }
    virtual const casacore::Matrix<CStokesVector>& visibility() const { IllegalOperation (); }

    virtual casacore::Matrix<CStokesVector>& modelVisibility() { IllegalOperation (); }
    virtual const casacore::Matrix<CStokesVector>& modelVisibility() const { IllegalOperation (); }

    virtual casacore::Matrix<CStokesVector>& correctedVisibility() { IllegalOperation (); }
    virtual const casacore::Matrix<CStokesVector>& correctedVisibility() const { IllegalOperation (); }

    virtual casacore::Cube<casacore::Complex>& visCube() { IllegalOperation (); }
    virtual const casacore::Cube<casacore::Complex>& visCube() const { return vb2_p-> visCube(); }

    virtual casacore::Cube<casacore::Complex>& modelVisCube() { return const_cast<casacore::Cube<casacore::Complex> &> (vb2_p->visCubeModel()); }

    virtual casacore::Cube<casacore::Complex>& modelVisCube(const casacore::Bool & ) { IllegalOperation (); }

    virtual const casacore::Cube<casacore::Complex>& modelVisCube() const { return vb2_p->visCubeModel(); }

    virtual casacore::Cube<casacore::Complex>& correctedVisCube() { IllegalOperation(); }
    virtual const casacore::Cube<casacore::Complex>& correctedVisCube() const { return vb2_p-> visCubeCorrected(); }

    // Return visCube(), modelVisCube(), or correctedVisCube(),
    // according to whichcol.  They throw an exception if whichcol is
    // unsupported, but note the encouraged default.
    // TODO: Optionally return DATA if whichcol is unavailable.
    casacore::Cube<casacore::Complex>& dataCube(const casacore::MS::PredefinedColumns whichcol=casacore::MS::DATA);
    const casacore::Cube<casacore::Complex>& dataCube(const casacore::MS::PredefinedColumns
                                  whichcol=casacore::MS::DATA) const;

    virtual casacore::Cube<casacore::Float>& floatDataCube() { IllegalOperation(); }
    virtual const casacore::Cube<casacore::Float>& floatDataCube() const { return vb2_p-> visCubeFloat (); }

    // Returns the weights for each row averaged over the parallel hand correlations.
    virtual casacore::Vector<casacore::Float>& weight() { IllegalOperation(); }
    virtual const casacore::Vector<casacore::Float>& weight() const { IllegalOperation(); }

    // Returns the nPol_p x curNumRow_p weight matrix.
    virtual casacore::Matrix<casacore::Float>& weightMat() { IllegalOperation(); }
    virtual const casacore::Matrix<casacore::Float>& weightMat() const { return vb2_p-> weight(); }

    // Is a valid WEIGHT_SPECTRUM available?
    virtual casacore::Bool existsWeightSpectrum() const { IllegalOperation(); }

    virtual casacore::Cube<casacore::Float>& weightSpectrum() { IllegalOperation();; }
    virtual const casacore::Cube<casacore::Float>& weightSpectrum() const { return vb2_p-> weightSpectrum(); }

    virtual casacore::Matrix<casacore::Float>& imagingWeight(){ IllegalOperation(); }
    virtual const casacore::Matrix<casacore::Float>& imagingWeight() const{ return vb2_p-> imagingWeight(); }

    virtual casacore::Cube<casacore::Float>& weightCube() { IllegalOperation(); }
    //</group>

    //<group>
    // Utility functions to provide coordinate or column ranges of the
    // data in the VisBuffer. Flagging is applied before computing the ranges.
    //
    // Generic accessor to column ranges of integer type, as specified by
    // enumerations defined in class MSCalEnums. Throws an exception
    // if the enum is not for a recognized integer column.
    virtual casacore::Vector<casacore::Int> vecIntRange(const MSCalEnums::colDef & ) const{ IllegalOperation(); }

    // Antenna id. range (includes both ANTENNA1 and ANTENNA2 columns)
    virtual casacore::Vector<casacore::Int> antIdRange() const{ IllegalOperation(); }

    // casacore::Time range
    virtual casacore::Bool timeRange(casacore::MEpoch & , casacore::MVEpoch & , casacore::MVEpoch & ) const
    { IllegalOperation(); }

    // Return the row Ids from the original ms. If the ms used is a subset of
    // another ms then rowIds() return the row ids of the original ms.
    virtual casacore::Vector<casacore::rownr_t>& rowIds(){ IllegalOperation(); }

    virtual const casacore::Vector<casacore::rownr_t>& rowIds() const { return vb2_p-> rowIds(); };

    //</group>

    // Frequency average the buffer (visibility() column only)
    virtual void freqAverage(){ IllegalOperation(); }

    // Frequency average the buffer (visCube and [if present] modelVisCube)
    void freqAveCubes(){ IllegalOperation(); }

    // Average channel axis according to chanavebounds, for whichever of DATA,
    // MODEL_DATA, CORRECTED_DATA, FLOAT_DATA, FLAG, and WEIGHT_SPECTRUM are
    // present.  It will only treat the first 5 as present if they have already
    // been loaded into the buffer!
    void channelAve(const casacore::Matrix<casacore::Int>& ){ IllegalOperation(); }

    // Average channel axis by factor.
    template<class T> void chanAveVisCube(casacore::Cube<T>& , casacore::Int ){ IllegalOperation(); }

    // Accumulate channel axis by factor, without applying WEIGHT_SPECTRUM even
    // if it is present.
    // It is primarily intended for averaging WEIGHT_SPECTRUM itself.
    template<class T> void chanAccCube(casacore::Cube<T>& , casacore::Int ){ IllegalOperation(); }

    // This defaults to no conceptual side effects, but usually it is more
    // efficient to let it leave weightSpectrum() in a channel averaged state.
    // restoreWeightSpectrum has no effect if !existsWeightSpectrum().
    virtual void chanAveFlagCube(casacore::Cube<casacore::Bool>& , const casacore::Int ,
                         const casacore::Bool  = true){ IllegalOperation(); }

    // Doesn't do anything if flagcat is degenerate.
    void chanAveFlagCategory(casacore::Array<casacore::Bool>& flagcat, const casacore::Int nChanOut);

    // Form casacore::Stokes parameters from correlations
    //  (these are preliminary versions)
    virtual void formStokes(){ IllegalOperation(); }
    virtual void formStokesWeightandFlag(){ IllegalOperation(); }
    virtual void formStokes(casacore::Cube<casacore::Complex>& ){ IllegalOperation(); }
    virtual void formStokes(casacore::Cube<casacore::Float>& ){ IllegalOperation(); }    // Will throw up if asked to do all 4.

    // casacore::Sort/unsort the correlations, if necessary
    //  (Rudimentary handling of non-canonically sorted correlations--use with care!)
    virtual void sortCorr(){ IllegalOperation(); }
    virtual void unSortCorr(){ IllegalOperation(); }

    // Normalize the visCube by the modelVisCube
    //   (and optionally also divide visCube_p by its normalized amp)
    virtual void normalize(const casacore::Bool &  = false){ CheckWritability(); vb2Rw_p-> normalize(); }

    // Fill weightMat according to sigma column
    virtual void resetWeightMat(){ IllegalOperation(); }

    // Rotate visibility phase for given vector (dim = nrow of vb) of phases (metres)
    virtual void phaseCenterShift(const casacore::Vector<casacore::Double>& ){ IllegalOperation(); }
    // Rotate visibility phase for phase center offsets (arcsecs)
    virtual void phaseCenterShift(casacore::Double , casacore::Double ){ IllegalOperation(); }

    // Update coordinate info - useful for copied VisBuffers that need
    // to retain some state for later reference.
    // Presently this fills antenna, array, field and spectralWindow ids, time,
    // frequency and number of rows.
    // if dirDependent is set to false the expensive direction dependent calculation of parallactic or direction of
    // antenna from pointing table is avoided
    //Add more as needed.
    virtual void updateCoordInfo(const VisBuffer *  = NULL, const casacore::Bool =true){ IllegalOperation(); }

    // Set the visibility to a constant, note that this only changes the buffer,
    // no values are written back to tables from here.
    virtual void setVisCube(casacore::Complex c){ CheckWritability(); vb2Rw_p-> setVisCube(c); }
    virtual void setModelVisCube(casacore::Complex c){ CheckWritability(); vb2Rw_p-> setVisCubeModel(c); }
    virtual void setCorrectedVisCube(casacore::Complex ){ IllegalOperation(); }

    // Set the visibility, note that this only changes the buffer,
    // no values are written back to tables from here.
    virtual void setVisCube(const casacore::Cube<casacore::Complex>& vis){ CheckWritability(); vb2Rw_p-> setVisCube(vis); }
    virtual void setModelVisCube(const casacore::Cube<casacore::Complex>& vis){ CheckWritability(); vb2Rw_p-> setVisCubeModel(vis); }
    virtual void setCorrectedVisCube(const casacore::Cube<casacore::Complex>& vis){ CheckWritability(); vb2Rw_p-> setVisCubeCorrected(vis); }

    // Like the above, but for FLOAT_DATA, keeping it as real floats.
    virtual void setFloatDataCube(const casacore::Cube<casacore::Float>& fcube){ CheckWritability(); vb2Rw_p-> setVisCubeFloat(fcube); }

    // Set model according to a casacore::Stokes vector
    virtual void setModelVisCube(const casacore::Vector<casacore::Float>& /*stokes*/){ IllegalOperation (); }

    // Reference external model visibilities
    virtual void refModelVis(const casacore::Matrix<CStokesVector>& ){ IllegalOperation(); }

    // Remove scratch cols data from vb
    virtual void removeScratchCols(){ IllegalOperation(); }

    // Access the current casacore::MSColumns object via VisIter
    virtual const casacore::MSColumns & msColumns() const
    {
        Assert (msColumns_p != 0);
        return * msColumns_p;
    }

    virtual  casacore::Int numberAnt () const{ return vb2_p-> nAntennas (); }

    // Get all selected spectral windows not just the one in the actual buffer
    virtual void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& spectralWindows, casacore::Vector<casacore::Int>& nChannels)
    {
        spectralWindows.assign (spectralWindows_p);
        nChannels.assign (nChannels_p);
    }

    virtual void getChannelSelection(casacore::Block< casacore::Vector<casacore::Int> >& ,
				   casacore::Block< casacore::Vector<casacore::Int> >& ,
				   casacore::Block< casacore::Vector<casacore::Int> >& ,
				   casacore::Block< casacore::Vector<casacore::Int> >& ,
				   casacore::Block< casacore::Vector<casacore::Int> >& ) const { IllegalOperation(); }
    void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& spws, casacore::Vector<casacore::Int>& nvischan) const ;

    // Return the actual msid, useful if using multiple ms to monitor which
    // ms in the  list is being dealt with
    virtual casacore::Int msId() const { return vb2_p-> msId(); }

    //checked if the ms has changed since the last chunk processed
    virtual casacore::Bool newMS() const { return vb2_p-> isNewMs(); }
    //get the name of the ms the buffer is at empty string if no visiter is attached
    virtual casacore::String msName(casacore::Bool =false) const{ return vb2_p-> msName(); }

    virtual casacore::Bool newArrayId () const{ return vb2_p-> isNewArrayId (); }
    virtual casacore::Bool newFieldId () const{ return vb2_p-> isNewFieldId (); }
    virtual casacore::Bool newSpectralWindow () const{ return vb2_p-> isNewSpectralWindow (); }

    //


private:


    const casacore::MSColumns * msColumns_p; // [use]
    casacore::Vector<casacore::Int> nChannels_p;
    casacore::Int nChannelsScalar_p;
    casacore::Int nCorr_p;
    casacore::Int nRows_p;
    casacore::Int spectralWindow_p;
    casacore::Vector<casacore::Int> spectralWindows_p;
    mutable casacore::Vector<casacore::RigidVector<casacore::Double, 3> > uvw_p;
    const vi::VisBuffer2 * vb2_p; // [use]
    vi::VisBuffer2 * vb2Rw_p; // [use]

};

} // end namespace vi

} // end namespace casa


#endif /* VISBUFFER2ADAPTER_H_ */
