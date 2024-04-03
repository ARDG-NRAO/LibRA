//# VisModelData.h: Calculate Model Visibilities for a buffer from model images / complist
//# Copyright (C) 2011
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#ifndef VISMODELDATA_H
#define VISMODELDATA_H
#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Arrays/Cube.h>
#include <synthesis/TransformMachines/ComponentFTMachine.h>
#include <msvis/MSVis/VisModelDataI.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

  class MeasurementSet;
  template <class T> class CountedPtr;
}

namespace casa { //# NAMESPACE CASA - BEGIN
//#forward
  class VisBuffer;
  namespace vi{ class VisBuffer2;}
  class ComponentList;  
  class FTMachine;
// <summary>
// Object to provide MODEL_DATA visibilities on demand
// </summary>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//  FTMachine and ComponentFTMachine
//
// </prerequisite>
//
// <etymology>
// Vis for Visibility
// ModelData is self explanatory
// </etymology>
//
// <synopsis>
// This Class also offer a lot of helper functions to query add and delete/modify models
// To be noted the models that are supported are FTMachines and Componentlist
// A given FIELD_ID  can have multiple models added to the same record 
// (i.e for e.g multiple spw models or multiple outlier images  or in the 
// case of componentlist multiple component list models. 
// A model may be valid for multiple fields e.g a mosaic
// To save storage in such cases multiple field id will point to the same model
// Look for the keyword "definedmodel_field_x" in the Source table keywords (or the main 
// table if source is not present).
// This will point to another key which is going to point to the model.
// Thus multiple "definedmodel_field_x"  may point to the same key.
// If there is no valid SOURCE table the key will hold a casacore::Record of the model to use 
// in the main table 
// Otherwise in the SOURCE table the key will hold the row of the SOURCE table in whose 
// SOURCE_MODEL cell the casacore::Record of the model to use will be resident.
// Now a given model casacore::Record in a SOURCE_MODEL cell  
// can hold multiple FTMachine's or Compnentlist (e.g multiple direction images or 
// spw images  associated with a given field)  
// and they are all cumulative (while respecting spw selection conditions) 
// when a request is made for the model visibility
// </synopsis>
//
// <example>
// <srcblock>
//  casacore::MeasurementSet theMS(.....)

//   VisibilityIterator vi(theMS,sort);
//   VisBuffer vb(vi);
//    casacore::MDirection myDir=vi.msColumns().field().phaseDirMeas(0);
//    ComponentList cl;
//    SkyComponent otherPoint(ComponentType::POINT);
//    otherPoint.flux() = Flux<casacore::Double>(0.00001, 0.0, 0.0, 0.00000);
//    otherPoint.shape().setRefDirection(myDir);
//    cl.add(otherPoint);
//    casacore::Record clrec;
//     clrec.define("type", "componentlist");
//   ///Define the fields ans spw for which this model is valid
//    clrec.define("fields", field);
//   clrec.define("spws", casacore::Vector<casacore::Int>(1, 0));
//    clrec.defineRecord("container", container);
//    casacore::Record outRec;
//    outRec.define("numcl", 1);
//    outRec.defineRecord("cl_0", clrec);

//     casacore::Vector<casacore::Int>spws(1,0);
//     Save model to the MS
//      VisModelData.putModel(theMS, container, field, spws, casacore::Vector<casacore::Int>(1,0), casacore::Vector<casacore::Int>(1,63), casacore::Vector<casacore::Int>(1,1), true, false);
//
//////now example to serve model visibility
// vi.origin();
///////////////
//VisModelData vm;
////           casacore::String modelkey; 
//            casacore::Int snum;
//   casacore::Bool hasmodkey=VisModelData::isModelDefined(vb.fieldId(), vi.ms(), modelkey, snum);
// Setup vm to serve the model for the fieldid()
//   if( hasmodkey){
//                      casacore::TableRecord modrec;
//                       VisModelData::getModelRecord(modelkey, modrec, visIter_p->ms())
//                       vm.addModel(modrec, casacore::Vector<casacore::Int>(1, msId()), vb);
//                      }
//    for (vi.originChunks();vi.moreChunks(); vi.nextChunk()){
//      for (vi.origin(); vi.more(); vi++){
//       This fills the vb.modelVisCube with the appropriate model visibility
//	  vm.getModelVis(vb);
//	cerr << "field " << vb.fieldId() << "  spw " << vb.spectralWindow() <<" max " <<  max(amplitude(vb.modelVisCube())) << endl;
 
//      }
//    }
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="2013/05/24">
// Allow validity of models for a given section of time only
//
// </todo>

  class VisModelData : public VisModelDataI {
 public:
  //empty constructor
  VisModelData();
  //From a FTMachine Record
  //VisModelData(const casacore::Record& ftmachinerec, const casacore::Vector<casacore::Int>& validfieldids, const casacore::Vector<casacore::Int>& msIds);
  virtual ~VisModelData();
  //Add Image/FTMachine to generate visibilities for
  //void addFTMachine(const casacore::Record& recordFTMachine, const casacore::Vector<casacore::Int>& validfieldids, const casacore::Vector<casacore::Int>& msIds);
  //Add componentlist to generate visibilities for
  //void addCompFTMachine(const ComponentList& cl, const casacore::Vector<casacore::Int>& validfieldids, 
  //			const casacore::Vector<casacore::Int>& msIds);
  //For simple model a special case for speed 
  
  void addFlatModel(const casacore::Vector<casacore::Double>& value, const casacore::Vector<casacore::Int>& validfieldids, 
		    const casacore::Vector<casacore::Int>& msIds);

  //add componentlists or ftmachines 
  void addModel(const casacore::RecordInterface& rec,  const casacore::Vector<casacore::Int>& msids, const VisBuffer& vb);
  void addModel(const casacore::RecordInterface& rec,  const casacore::Vector<casacore::Int>& msids, const vi::VisBuffer2& vb);
  VisModelDataI * clone ();

  //put the model data for this VisBuffer in the modelVisCube
  casacore::Bool getModelVis(VisBuffer& vb);
  casacore::Bool getModelVis(vi::VisBuffer2& vb);

  //this is a helper function that writes the model record to the ms 
  void putModelI(const casacore::MeasurementSet& thems, const casacore::RecordInterface& rec,
		 const casacore::Vector<casacore::Int>& validfields, const casacore::Vector<casacore::Int>& spws,
		 const casacore::Vector<casacore::Int>& starts, const casacore::Vector<casacore::Int>& nchan,
		 const casacore::Vector<casacore::Int>& incr, casacore::Bool iscomponentlist=true, casacore::Bool incremental=false)
  {
    putModel (thems, rec, validfields, spws, starts, nchan, incr, iscomponentlist, incremental);
  }
  static void putModel(const casacore::MeasurementSet& thems, const casacore::RecordInterface& rec, const casacore::Vector<casacore::Int>& validfields, const casacore::Vector<casacore::Int>& spws, const casacore::Vector<casacore::Int>& starts, const casacore::Vector<casacore::Int>& nchan,  const casacore::Vector<casacore::Int>& incr, casacore::Bool iscomponentlist=true, casacore::Bool incremental=false);
  //Verion 2.0 of putModel to support Intent
  // index Comb is a Matrix of n rows 4 columns 
  //Each row is a unique combination of field_id, spw_id, scan_number, state_id for which this
  //record is valid
  //chansel is Matrix of nspw rows and 4 columns (spw_id, start, nchan, incr)
  static void putModel(const casacore::MeasurementSet& thems,const casacore::RecordInterface& rec, const casacore::Matrix<casacore::Int>& indexComb, const casacore::Matrix<casacore::Int>& chansel,
					  casacore::Bool iscomponentlist=true, casacore::Bool incremental=false);
  void putModelI(const casacore::MeasurementSet& thems,const casacore::RecordInterface& rec, const casacore::Matrix<casacore::Int>& indexComb, const casacore::Matrix<casacore::Int>& chansel,
					  casacore::Bool iscomponentlist=true, casacore::Bool incremental=false){
 	putModel(thems, rec, indexComb, chansel, iscomponentlist, incremental);  
  };


  //helper function to clear the keywordSet of the ms of the model  for the fields 
  //in that ms
  void clearModelI(const casacore::MeasurementSet& thems) { clearModel (thems); }
  static void clearModel(const casacore::MeasurementSet& thems);
  // ...with field selection and optionally spw
  static void clearModel(const casacore::MeasurementSet& thems, const casacore::String field, const casacore::String spws=casacore::String(""));

  //Functions to see if model is defined in the casacore::MS either in the SOURCE table or else in the MAIN
  casacore::Bool isModelDefinedI(const casacore::Int fieldId, const casacore::MeasurementSet& thems, casacore::String& key, casacore::Int& sourceRow)
  {
    return isModelDefined (fieldId, thems, key, sourceRow);
  }
  static casacore::Bool isModelDefined(const casacore::Int fieldId, const casacore::MeasurementSet& thems, casacore::String& key, casacore::Int& sourceRow);
  static casacore::Bool isModelDefined(const casacore::String& elkey, const casacore::MeasurementSet& thems);

  //Get a given model that is defined by key
  //Forcing user to use a casacore::TableRecord rather than Generic casacore::RecordInterface ...just so as to avoid a copy.
  casacore::Bool getModelRecordI(const casacore::String& theKey, casacore::TableRecord& theRec, const casacore::MeasurementSet& theMs)
  {
    return getModelRecord (theKey, theRec, theMs);
  }
  static casacore::Bool getModelRecord(const casacore::String& theKey, casacore::TableRecord& theRec, const casacore::MeasurementSet& theMs);

  // casacore::List the fields
  static void listModel(const casacore::MeasurementSet& thems);

  static FTMachine* NEW_FT(const casacore::Record& ftrec);
  //check if an addFT or addCompFT is necessary
  //casacore::Bool hasFT(casacore::Int msid, casacore::Int fieldid);
  //casacore::Bool hasCL(casacore::Int msid, casacore::Int fieldid);
  //returns a -1 if there is no model for this combination of ms,field,spw...but has not been  // checked yet if there is one
  //returns a -2 if it has been tested before but does have it.
  //returns a 1 if it has a model stored 
  casacore::Int hasModel(casacore::Int msid, casacore::Int field, casacore::Int spw); 
  casacore::Bool isVersion2() {return (version_p==2);};
  ///init has to be called once at least to make sure isVersion2 to be correct
  void init(const VisBuffer& vb);
  void init(const vi::VisBuffer2& ){throw(casacore::AipsError("Called the wrong version of VisModelData"));};
  private:
  void initializeToVis();
  casacore::Vector<casacore::CountedPtr<ComponentList> >getCL(const casacore::Int msId, const casacore::Int fieldId, casacore::Int spw);
  casacore::Vector<casacore::CountedPtr<FTMachine> >getFT(const casacore::Int msId, const casacore::Int fieldId, casacore::Int spw);
  static casacore::Bool addToRec(casacore::TableRecord& therec, const casacore::Vector<casacore::Int>& spws);
  static casacore::Bool addToRec(casacore::TableRecord& therec, const casacore::Matrix<casacore::Int>& combindex);
  static casacore::Bool removeSpwFromMachineRec(casacore::RecordInterface& ftclrec, const casacore::Vector<casacore::Int>& spws);
  static casacore::Bool removeFTFromRec(casacore::TableRecord& therec, const casacore::String& keyval, const casacore::Bool relabel=true);
  static casacore::Bool removeSpw(casacore::TableRecord& therec, const casacore::Vector<casacore::Int>& spws, const casacore::Vector<casacore::Int>& fields=casacore::Vector<casacore::Int>(0));
  static casacore::Bool putModelRecord(const casacore::Vector<casacore::Int>& fieldIds, casacore::TableRecord& theRec, casacore::MeasurementSet& theMS);
  //Remove the casacore::Record which has the given key...will exterminate it from both the Source table or Main table
  static void removeRecordByKey(casacore::MeasurementSet& theMS, const casacore::String& theKey);
  //put the casacore::Record by key if sourcerownum=-1 then it is saved in the main table
  //this default should only be used  if the optional SOURCE table in non-existant
  static void putRecordByKey(casacore::MeasurementSet& theMS, const casacore::String& theKey, const casacore::TableRecord& theRec, const casacore::Int sourceRowNum=-1);
  static void deleteDiskImage(casacore::MeasurementSet& theMS, const casacore::String& theKey);
  static casacore::Int  firstSourceRowRecord(const casacore::Int field, const casacore::MeasurementSet& theMS, 
				   casacore::TableRecord& rec);
  static void modifyDiskImagePath(casacore::Record& rec, const VisBuffer& vb);
  void getMatchingMachines(casacore::Vector<casacore::CountedPtr<FTMachine> >& ft, casacore::Vector<casacore::CountedPtr<ComponentList> >& cl, const VisBuffer& vb);
  void updateHolders(const VisBuffer& vb, const std::vector<casacore::Int>& index);
  //get all the combinations of field, spw, scan, state in this visbuffer
  void getUniqueIndicesComb(const VisBuffer& vb, casacore::Vector< casacore::Vector<casacore::Int> >& retval);
  casacore::Block<casacore::Vector<casacore::CountedPtr<ComponentList> > > clholder_p;
  casacore::Block<casacore::Vector<casacore::CountedPtr<FTMachine> > > ftholder_p;
  casacore::Block<casacore::Vector<casacore::Double> > flatholder_p;
  casacore::CountedPtr<ComponentFTMachine> cft_p;
  casacore::Cube<casacore::Int> ftindex_p;
  casacore::Cube<casacore::Int> clindex_p;
  //These 2 maps will hold the index of ftholder_p or clholder_p from key (field_id, spw_id, scan, state, msid)
  std::map<std::vector<casacore::Int>, casacore::Int > ftindex2_p;
  std::map<std::vector<casacore::Int>, casacore::Int > clindex2_p;
  casacore::Int version_p;
  static casacore::Bool initialize;
};

}//end namespace
#endif // VISMODELDATA_H
