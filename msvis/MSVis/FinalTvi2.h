//# VisibilityIterator.h: Step through the MeasurementEquation by visibility
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the Implied warranty of MERCHANTABILITY or
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
//# $Id: VisibilityIterator2.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#if ! defined (MSVIS_WritingVi2_H_121115_0950)
#define MSVIS_WritingVi2_H_121115_0950

#include <casa/aips.h>
#include <ms/MeasurementSets.h>
#include <msvis/MSVis/TransformingVi2.h>
#include <msvis/MSVis/ViColumns2.h>

#include <map>
#include <vector>

namespace casa { //# NAMESPACE CASA - BEGIN


namespace vi {

//# forward decl

class VisBuffer2;

class ChannelSelector;
class ChannelSelectorCache;
typedef casacore::Vector<casacore::Vector <casacore::Slice> > ChannelSlicer;
class SpectralWindowChannelsCache;
class SpectralWindowChannels;
class SubtableColumns;


// <summary>
// FinalTvi2 - Class that serves as the final Tvi2 in a pipeline; it adds simple write functionality that
// allows writing back the data contained in the associated VB2.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
// </prerequisite>
//
// <etymology>
// The FinalTvi2 in a transforming VI2 pipeline.
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// <code>
// //
// </code>
// </example>
//
// <motivation>
// At the end of a Transforming VI pipeline it can be handy to write out the result.  Putting a FinalTvi2
// as the last transforming VI allows the output to disk of the transformed data into a brand new MS.
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="2014/08/21">
// </todo>

class FinalTvi2 : public TransformingVi2 {

public:

    FinalTvi2 (ViImplementation2 * inputVi,
               casacore::MeasurementSet & finalMs,
               casacore::Bool isWritable);

    // Destructor

    virtual ~FinalTvi2 ();

    //   +==================================+
    //   |                                  |
    //   | Iteration Control and Monitoring |
    //   |                                  |
    //   +==================================+


    virtual void origin ();
    virtual void next ();

    //   +=========================+
    //   |                         |
    //   | Subchunk casacore::Data Accessors |
    //   |                         |
    //   +=========================+


    //   +------------------------+
    //   |                        |
    //   | Angular casacore::Data Providers |
    //   |                        |
    //   +------------------------+


    //   +=========================+
    //   |                         |
    //   | Chunk and casacore::MS Level casacore::Data |
    //   |                         |
    //   +=========================+



    //   +-------------------+
    //   |                   |
    //   | Writeback Methods |
    //   |                   |
    //   +-------------------+

    // This method writes back any changed (dirty) components of the provided
    // VisBuffer and is the preferred method for writing data out.

    virtual void writeBackChanges (VisBuffer2 * vb);

    // These methods will throw a not-implemented exception if called.  They are here
    // because the interface requires them but the intended use of FinalTvi2 expects
    // that the data will be output using only the writeBackChanges method.

    virtual void writeFlag (const casacore::Matrix<casacore::Bool> & flag);
    virtual void writeFlag (const casacore::Cube<casacore::Bool> & flag);
    virtual void writeFlagRow (const casacore::Vector<casacore::Bool> & rowflags);
    virtual void writeFlagCategory(const casacore::Array<casacore::Bool>& fc);
    virtual void writeVisCorrected (const casacore::Cube<casacore::Complex> & vis);
    virtual void writeVisModel (const casacore::Cube<casacore::Complex> & vis);
    virtual void writeVisObserved (const casacore::Cube<casacore::Complex> & vis);
    virtual void writeWeight (const casacore::Matrix<casacore::Float> & wt);
    virtual void writeWeightSpectrum (const casacore::Cube<casacore::Float> & wtsp);
    virtual void writeSigma (const casacore::Matrix <casacore::Float> & sig);
    virtual void writeModel(const casacore::RecordInterface& rec, casacore::Bool iscomponentlist=true,
                            casacore::Bool incremental=false);

protected:

    //void configureNewSubchunk ();

    // These are the methods that actually write the data in the VB2 out to the new
    // casacore::MS when called by writeBackChanges.  The VB's data are handled as four categories:
    //    data - the various visibility data and the associated weight, weight spectrum, sigma, etc.
    //    keys - the values of the columns that ID the row (e.g., antennas, DdId, etc.)
    //    misc - the column values that don't fall in the data or key categories.

    void writeDataValues (casacore::MeasurementSet & ms, const casacore::RefRows & rows);
    void writeKeyValues (casacore::MeasurementSet & ms, const casacore::RefRows & rows);
    void writeMiscellaneousValues (casacore::MeasurementSet & ms, const casacore::RefRows & rows);

private:

    ViColumns2     columns_p;
    casacore::Bool           columnsAttached_p;
    casacore::MeasurementSet ms_p;

};

} // end namespace vi

} //# NAMESPACE CASA - END

#endif // ! defined (MSVIS_WritingVi2_H_121115_0950)


