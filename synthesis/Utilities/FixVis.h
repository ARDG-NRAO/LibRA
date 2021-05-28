//# FixVis.h: Does for MSes various fixes which do not involve calibrating.
//# Copyright (C) 2008
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
//#
#ifndef SYN_FIXVIS_H
#define SYN_FIXVIS_H

#include <casa/aips.h>
#include <casa/BasicSL/String.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <msvis/MSVis/MSUVWGenerator.h>

// casacore::UVWMachine Does rotation, including B1950 <-> J2000, refocusing, and maybe
// even SIN <-> (-)NCP reprojection of existing UVWs, but it does not generate
// UVWs from an antenna table.
// FTMachine::rotateUVW(casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Double>& dphase,
//                      const VisBuffer& vb)
#include <synthesis/TransformMachines/FTMachine.h>

namespace casacore{

class MeasurementSet;
class LogIO;
}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>Performs for MeasurementSets various fixes which do not involve calibrating.
// This includes (in order of implementation):
//   1. Generating and inserting (u, v, w)s into a casacore::MS that may or may not
//      already have them.  Includes antenna offsets when known.
//   2. Correcting for differential aberration.
//   3. Changing the phase tracking center.
//   4. Changing the equinox (B1950 -> J2000).
//   5. (maybe never) Changing the projection, i.e. SIN <-> (-)NCP.
// </summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class=casacore::MeasurementSet>MeasurementSet</linkto>
// </prerequisite>
//
// <etymology>
// It is a variation on the UVFIX task of AIPS.
// </etymology>
//
// <synopsis>
// (u, v, w)s are needed for imaging, but some data sets may not come with them
// included.
//
// FixVis can also be used to shift the tangent point, and correct for
// differential aberration.
// </synopsis>
//
// <example>
// <srcBlock>
//     casacore::MS inMS(fileName);
//     FixVis uvwfixer(inMS);
//     uvwfixer.setDataDescriptionIds(ddIds);
//     uvwfixer.setFields(fieldIds);
//     uvwfixer.fixuvw();
// </srcBlock>
// A <src>FixVis</src> object is constructed 
// and the baselines are calculated and stored in inMS.
// </example>
//
// <motivation>
// Currently (10/30/2008), ASDMs from either the ALMA Test Facility or the
// EVLA do not come with (u, v, w)s, and need to be processed with the UVFIX
// task in AIPS.  It would be preferable to process them entirely inside CASA.
// </motivation>
//
// <todo asof="">
// </todo>

// class FixVis;

class FixVis : public FTMachine
{
public:
// Constructor
  FixVis (casacore::MeasurementSet& ms, const casacore::String& dataColName=casacore::String("all"));

// Assignment (only copies reference to casacore::MS, need to reset selection etc)
  FixVis& operator=(FixVis& other);

// Destructor
  ~FixVis();

// Set the required field Ids and return the # of selected fields.
  casacore::uInt setFields(const casacore::Vector<casacore::Int>& fieldIds);

  //// Select by observationIDs (problematic at best)
  //void setObsIDs(const casacore::String& obs) {obsString_p = obs;}

  // Specifies new phase tracking centers for the selected fields
  void setPhaseDirs(const casacore::Vector<casacore::MDirection>& phaseDirs);

  // Specifies distances for each selected field according to distances, which
  // must be in m and the same order as the casacore::Vector given to setFields.  Throws
  // an exception if distances.nelements() != nsel_p.
  // Because of the way refocus() works, zeroes are ignored (no refocusing
  // done), but negative distances are accepted!
  void setDistances(const casacore::Vector<casacore::Double>& distances);

  // Calculate the (u, v, w)s and store them in ms_p.
  casacore::Bool calc_uvw(const casacore::String& refcode, const casacore::Bool reuse=true);

  // Convert the UVW column to a new reference frame by rotating the old
  // baselines instead of calculating fresh ones.
  void rotateUVW(const casacore::MDirection &indir, const casacore::MDirection::Ref& newref);

  // For things like rotation, differential aberration correction, etc., when
  // there already is a UVW column, using FTMachine.  Returns true if _any_
  // fields are modified.
  casacore::Bool fixvis(const casacore::String& refcode);
  virtual void setMiscInfo(const casacore::Int qualifier){(void)qualifier;};
  virtual void ComputeResiduals(VisBuffer&, casacore::Bool //useCorrected
                                ) {}
  virtual casacore::String name() const { return "FixVis";};

private:
  // Interpret field indices (casacore::MSSelection)
  casacore::Vector<casacore::Int> getFieldIdx(const casacore::String& fields);
  
  // Makes sure msc_p is ready, and returns false if it fails.
  casacore::Bool ready_msc_p();
  
  // Convert the directions (phase tracking centers, + DELAY_DIR and
  // REFERENCE_DIR if they start in the same frame) in the FIELD table to
  // newFrame.  Note that each direction column in the table only allows one
  // reference frame for the entire column, so all fields must share the same
  // frame.  Calls ready_msc_p() as a side effect.
  void convertFieldDirs(const casacore::MDirection::Types outType);

  // Private worker function for convertFieldDirs().
  void convertFieldCols(casacore::MSFieldColumns& msfcs,
                        const casacore::MDirection::Ref& newFrame,
                        const casacore::Bool doAll3);

  // Calls ready_msc_p() as a side effect.
  casacore::Bool makeSelection(const casacore::Int selectedField);
  
  // Does phase tracking center and distance adjustment for mssel_p.
  void processSelected(casacore::uInt numInSel);

  // FTMachine declares a LOT of pure virtual functions which FixVis does not
  // need.  They are declared as no-ops here for now.
  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<float>&, casacore::Bool) {return *image;}
  virtual void normalizeImage(casacore::Lattice<casacore::Complex>&,// skyImage,
			      const casacore::Matrix<casacore::Double>&,// sumOfWts,
			      casacore::Lattice<casacore::Float>&,// sensitivityImage,
			      casacore::Bool //fftNorm
                              )
  {throw(casacore::AipsError("normalizeImage::normalizeImage() called"));}

  void getWeightImage(casacore::ImageInterface<float>&, casacore::Matrix<float>&) {}
  void get(VisBuffer&, casacore::Int) {}
  void put(const VisBuffer& , casacore::Int , casacore::Bool , FTMachine::Type) {};

  // casacore::Bool getRestFreq(casacore::Vector<casacore::Double>& restFreq, const casacore::Int spw, const casacore::Int fldID);
  //  void setObsInfo(casacore::ObsInfo& obsinfo);

  void ok();
  void init();

  // Initialize transform to Visibility plane using the image
  // as a template.
  void initializeToVis(casacore::ImageInterface<casacore::Complex>& image,
		       const VisBuffer& vb);
  
  // Finalize transform to Visibility plane.
  void finalizeToVis();

  // Initialize transform to Sky plane: initializes the image
  void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,  casacore::Matrix<casacore::Float>& weight,
		       const VisBuffer& vb);
  
  // Defined here only because FTMachine declares it purely virtual.
  void finalizeToSky() {}

  // TODO?: trackDir.
  casacore::Bool setImageField(const casacore::Int fieldid,
                     const casacore::Bool dotrackDir=false //, const casacore::MDirection& trackDir
                     );

  casacore::Bool lock();
  void unlock();

  // Log functions and variables
  casacore::LogIO sink_p;
  casacore::LogIO& logSink() {return sink_p;}

  // Initialized in ctor.  Make sure the order there matches with the order here.
  casacore::MeasurementSet ms_p;			// casacore::Input/Output MS
  casacore::MSColumns      *msc_p;		// Ptr. to columns of mssel_p.
  casacore::uInt           nsel_p;		// Number of selected fields.
  casacore::uInt           nAllFields_p;	        // The total # of fields in the MS.
  const casacore::uInt       npix_p;              // Not that there are any real pixels.
  const casacore::IPosition  cimageShape_p;       // No image will be made, but
  const casacore::IPosition  tileShape_p;         // the coords are stored in a fake one.
  const casacore::TiledShape tiledShape_p;
  casacore::Bool             antennaSel_p;        // Is selection being done by antenna?
  casacore::Bool             freqFrameValid_p;    // Freq frame is good and valid
                                        // conversions can be done (or not)
  //casacore::String           obsString_p;       // obsID selection
  casacore::Vector<casacore::Int>      antennaId_p;         // casacore::MSSelection::indexExprStr() doesn't
                                        // work with casacore::Vector<casacore::uInt>.
  casacore::Vector<casacore::String>   antennaSelStr_p;
  casacore::Vector<casacore::Double>   distances_p;	        // new distances (m) for each selected
                                        // field
  casacore::Vector<casacore::MS::PredefinedColumns> dataCols_p;
  casacore::uInt             nDataCols_p;
  casacore::uInt             nchan_p;
  casacore::Vector<casacore::Int>      spectralwindowids_p;
  casacore::uInt             lockCounter_p;
  
  // Not initialized in ctor.
  casacore::MeasurementSet     mssel_p;           // The selected part of ms_p.
  casacore::ObsInfo            latestObsInfo_p;  
  casacore::Vector<casacore::Int>        DDIds_p;	        // DataDescription Ids to process
  casacore::Vector<casacore::Int>        FieldIds_p;        // Field Ids to process
  casacore::Vector<casacore::MDirection> phaseDirs_p;       // new phase centers for each selected
                                        // field
  casacore::Int                nSpw_p;	        // Number of spws
  casacore::MPosition          mLocation_p;
  casacore::Bool               doTrackSource_p;
  casacore::Int                fieldid_p;
};
  
} //# NAMESPACE CASA - END

#endif
