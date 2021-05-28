/*
 * MsRows.h
 *
 *  Created on: Feb 20, 2013
 *      Author: jjacobs
 */

#if ! defined (MsVis_MsRows_H)
#define MsVis_MsRows_H

#include <msvis/MSVis/VisBuffer2.h>
#include <vector>

namespace casa {

namespace vi{

    class VisBuffer2;
}

namespace ms {

//class MsRowAttributes {
//
//public:
//
//    MsRowAttributes (casacore::Int row, VisBuffer2 * vb)
//    : row_p (row), vb_p (vb)
//    {}
//
//    casacore::Int antenna1 () const;
//    casacore::Int antenna2 () const;
//    casacore::Int array () const;
//    casacore::Int dataDescriptionId () const;
//    casacore::Int feed1 () const;
//    casacore::Int feed2 () const;
//    casacore::Int field () const;
//    casacore::Int observation () const;
//    casacore::Int processorId () const;
//    casacore::Int scan () const;
//    casacore::Int state () const;
//
//private:
//};
//
//class MsRowCoordinates {
//
//public:
//
//    MsRowCoordinates (casacore::Int row, VisBuffer2 * vb)
//    : row_p (row), vb_p (vb)
//    {}
//
//    casacore::Double exposure () const;
//    casacore::Int fieldId () const;
//    casacore::Double interval () const;
//    casacore::Double time () const {
//        return vb_p->time() (row_p);
//    }
//    casacore::Double timeCentroid () const;
//    const casacore::Vector<casacore::Double> & uvw () const;
//
//private:
//
//    casacore::Int row_p;
//    vi::VisBuffer2 * vb_p;
//
//};
//
//class MsRowData {
//
//public:
//
//    MsRowData (casacore::Int row, VisBuffer2 * vb)
//    : row_p (row), vb_p (vb)
//    {}
//
//    const casacore::Complex & corrected (casacore::Int correlation, casacore::Int channel) const;
//    const casacore::Complex & model (casacore::Int correlation, casacore::Int channel) const;
//    const casacore::Complex & observed (casacore::Int correlation, casacore::Int channel) const
//    {
//       return vb_p->visCube () (correlation, channel, row_p);
//    }
//    const casacore::Float sigma () const;
//    const casacore::Float weight () const;
//    const casacore::Float weightSpectrum (casacore::Int correlation, casacore::Int channel) const;
//
//private:
//
//    casacore::Int row_p;
//    const vi::VisBuffer2 * vb_p;
//
//};
//
//class MsRowFlagging {
//
//public:
//
//    MsRowFlagging (casacore::Int row, VisBuffer2 * vb)
//    : row_p (row), vb_p (vb)
//    {}
//
//    casacore::Bool isFlagged () const;
//    casacore::Bool isFlagged (casacore::Int correlation, casacore::Int channel) const;
//
//    void setFlag (casacore::Bool isFlagged);
//    void setFlag (casacore::Bool isFlagged, casacore::Int correlation, casacore::Int channel);
//
//private:
//
//    casacore::Int row_p;
//    vi::VisBuffer2 * vb_p;
//};
//
//class MsRow {
//
//public:
//
//    const MsRowAttributes & attributes () const;
//    const MsRowCoordinates & coordinates () const;
//    const MsRowData & data () const;
//    const MsRowFlagging & flagging () const;
//
//protected:
//
//private:
//
//    int row_p;
//};

class MsRow {

public:

    // Constructors

    // Constructor for read-only access.
    // Attempt to write will throw exception.

    MsRow (casacore::rownr_t row, const vi::VisBuffer2 * vb);

    // Constructor for read/write access

    MsRow (casacore::rownr_t row, vi::VisBuffer2 * vb);

    virtual ~MsRow () {}

    virtual void changeRow (casacore::rownr_t row) { row_p = row;}

    virtual casacore::Int antenna1 () const = 0;
    virtual casacore::Int antenna2 () const = 0;
    virtual casacore::Int arrayId () const = 0;
    virtual casacore::Int correlationType () const = 0;
    virtual casacore::Int dataDescriptionId () const = 0;
    virtual casacore::Int feed1 () const = 0;
    virtual casacore::Int feed2 () const = 0;
    virtual casacore::Int fieldId () const = 0;
    virtual casacore::Int observationId () const = 0;
    virtual casacore::Int processorId () const = 0;
    virtual casacore::Int scanNumber () const = 0;
    virtual casacore::Int stateId () const = 0;
    virtual casacore::Double exposure () const = 0;
    virtual casacore::Double interval () const = 0;
    virtual casacore::Int rowId () const = 0;
    virtual casacore::Int spectralWindow () const = 0;
    virtual casacore::Double time () const = 0;
    virtual casacore::Double timeCentroid () const = 0;

    virtual void setAntenna1 (casacore::Int) = 0;
    virtual void setAntenna2 (casacore::Int) = 0;
    virtual void setArrayId (casacore::Int) = 0;
    virtual void setCorrelationType (casacore::Int) = 0;
    virtual void setDataDescriptionId (casacore::Int) = 0;
    virtual void setFeed1 (casacore::Int) = 0;
    virtual void setFeed2 (casacore::Int) = 0;
    virtual void setFieldId (casacore::Int) = 0;
    virtual void setObservationId (casacore::Int) = 0;
    virtual void setProcessorId (casacore::Int) = 0;
    virtual void setScanNumber (casacore::Int) = 0;
    virtual void setStateId (casacore::Int) = 0;
    virtual void setExposure (casacore::Double) = 0;
    virtual void setInterval (casacore::Double) = 0;
    virtual void setRowId (casacore::Int) = 0;
    virtual void setSigma (casacore::Int correlation, casacore::Float value) = 0;
    virtual void setSigma (const casacore::Vector<casacore::Float> &  value) = 0;
    virtual void setTime (casacore::Double) = 0;
    virtual void setTimeCentroid (casacore::Double) = 0;
    virtual void setWeight (casacore::Int correlation, casacore::Float value) = 0;
    virtual void setWeight (const casacore::Vector<casacore::Float> & value) = 0;

    virtual const casacore::Vector<casacore::Double> uvw () const = 0;
    virtual void setUvw (const casacore::Vector<casacore::Double> &) = 0;

    virtual const casacore::Complex & corrected (casacore::Int correlation, casacore::Int channel) const = 0;
    virtual const casacore::Matrix<casacore::Complex> & corrected () const = 0;
    virtual void setCorrected (casacore::Int correlation, casacore::Int channel, const casacore::Complex & value) = 0;
    virtual void setCorrected (const casacore::Matrix<casacore::Complex> & value) = 0;

    virtual const casacore::Complex & model (casacore::Int correlation, casacore::Int channel) const = 0;
    virtual const casacore::Matrix<casacore::Complex> & model () const = 0;
    virtual void setModel(casacore::Int correlation, casacore::Int channel, const casacore::Complex & value) = 0;
    virtual void setModel (const casacore::Matrix<casacore::Complex> & value) = 0;

    virtual const casacore::Complex & observed (casacore::Int correlation, casacore::Int channel) const = 0;
    virtual const casacore::Matrix<casacore::Complex> & observed () const = 0;
    virtual void setObserved (casacore::Int correlation, casacore::Int channel, const casacore::Complex & value) = 0;
    virtual void setObserved (const casacore::Matrix<casacore::Complex> & value) = 0;

    virtual const casacore::Float & singleDishData (casacore::Int correlation, casacore::Int channel) const = 0;
    virtual const casacore::Matrix<casacore::Float> singleDishData () const = 0;
    virtual void setSingleDishData (casacore::Int correlation, casacore::Int channel, const casacore::Float & value) = 0;
    virtual void setSingleDishData (const casacore::Matrix<casacore::Float> & value) = 0;

    virtual casacore::Float sigma (casacore::Int correlation) const = 0;
    virtual const casacore::Vector<casacore::Float> & sigma () const = 0;
    virtual casacore::Float weight (casacore::Int correlation) const = 0;
    virtual const casacore::Vector<casacore::Float> & weight () const = 0;
    virtual casacore::Float weightSpectrum (casacore::Int correlation, casacore::Int channel) const = 0;
    virtual const casacore::Matrix<casacore::Float> & weightSpectrum () const = 0;
    virtual casacore::Float sigmaSpectrum (casacore::Int correlation, casacore::Int channel) const = 0;
    virtual const casacore::Matrix<casacore::Float> & sigmaSpectrum () const = 0;

    virtual casacore::Bool isRowFlagged () const = 0;
    virtual const casacore::Matrix<casacore::Bool> & flags () const = 0;
    virtual void setFlags (const casacore::Matrix<casacore::Bool> &) = 0;
    virtual casacore::Bool isFlagged (casacore::Int correlation, casacore::Int channel) const = 0;

    virtual void setRowFlag (casacore::Bool isFlagged) = 0;
    virtual void setFlags (casacore::Bool isFlagged, casacore::Int correlation, casacore::Int channel) = 0;

protected:

    casacore::Bool isWritable () const;
    casacore::uInt row () const;
    vi::VisBuffer2 * vb () const;

private:

    MsRow (const MsRow & other); // no copying

    const casacore::Bool isWritable_p;
    casacore::Int row_p;
    vi::VisBuffer2 * vb_p; // [use]
};

class MsRows {

public:

    typedef std::vector<MsRow *> Rows;
    typedef Rows::const_iterator const_iterator;
    typedef Rows::iterator iterator;

    MsRows (vi::VisBuffer2 * vb);

    MsRow & operator() (casacore::Int i);
    const MsRow & operator() (casacore::Int i) const;

    const_iterator begin () const;
    iterator begin ();
    const_iterator end () const;
    iterator end ();
    MsRow & row (casacore::Int row);
    const MsRow & row (casacore::Int row) const;
    size_t size () const;

protected:

private:

    Rows rows_p;
    vi::VisBuffer2 * visBuffer_p; // [use]
};

// MsRows rows;
// casacore::Double t = rows(i).time();
// casacore::Double t = vb.time()(i)

} // end namespace ms

} // end namespace casa

#endif // ! defined (MsVis_MsRows_H)
