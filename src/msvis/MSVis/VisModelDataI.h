
#include <casacore/casa/Arrays/ArrayFwd.h>
#include <casacore/casa/aips.h>
#include <vector>
#if ! defined (MSVIS_VisModelDataI_H)
#define MSVIS_VisModelDataI_H

namespace casacore{

  class MeasurementSet;
  class RecordInterface;
  class String;
  class TableRecord;

}

namespace casa {

  class VisBuffer;
  namespace vi{class VisBuffer2;}

  
  class VisModelDataI;



class VisModelDataI {

 public:

  typedef VisModelDataI * (* Factory) ();

  static bool setFactory (Factory, casacore::Int whichone=0);

  //empty constructor
  VisModelDataI() {}

  //From a FTMachine Record
  //VisModelDataI(const casacore::Record& ftmachinerec, const casacore::Vector<casacore::Int>& validfieldids, const casacore::Vector<casacore::Int>& msIds);
  virtual ~VisModelDataI() {}

  //Add Image/FTMachine to generate visibilities for
  //void addFTMachine(const casacore::Record& recordFTMachine, const casacore::Vector<casacore::Int>& validfieldids, const casacore::Vector<casacore::Int>& msIds);
  //Add componentlist to generate visibilities for
  //void addCompFTMachine(const ComponentList& cl, const casacore::Vector<casacore::Int>& validfieldids, 
  //			const casacore::Vector<casacore::Int>& msIds);
  //For simple model a special case for speed 
  
  // void addFlatModel(const casacore::Vector<casacore::Double>& value, const casacore::Vector<casacore::Int>& validfieldids, 
  // 		    const casacore::Vector<casacore::Int>& msIds);

  // //add componentlists or ftmachines 
  virtual void addModel(const casacore::RecordInterface& rec,  const casacore::Vector<casacore::Int>& msids, const VisBuffer& vb) = 0;
  virtual void addModel(const casacore::RecordInterface& rec,  const casacore::Vector<casacore::Int>& msids, const vi::VisBuffer2& vb) = 0;
  virtual VisModelDataI * clone () = 0;

  static VisModelDataI * create ();
  static VisModelDataI * create2 ();
  // //put the model data for this VisBuffer in the modelVisCube
  virtual casacore::Bool getModelVis(VisBuffer& vb) = 0;
  virtual casacore::Bool getModelVis(vi::VisBuffer2& vb) = 0;
  // //this is a helper function that writes the model record to the ms 

  virtual void putModelI (const casacore::MeasurementSet& thems, const casacore::RecordInterface& rec,
		  	  const casacore::Vector<casacore::Int>& validfields, const casacore::Vector<casacore::Int>& spws,
			  const casacore::Vector<casacore::Int>& starts, const casacore::Vector<casacore::Int>& nchan,
			  const casacore::Vector<casacore::Int>& incr, casacore::Bool iscomponentlist=true, casacore::Bool incremental=false) = 0;
			  //Version 2 record to keep track of state and scan number
  virtual void putModelI(const casacore::MeasurementSet& thems,const casacore::RecordInterface& rec, const casacore::Matrix<casacore::Int>& indexComb, const casacore::Matrix<casacore::Int>& chansel,
					  casacore::Bool iscomponentlist=true, casacore::Bool incremental=false)=0;

  // //helper function to clear the keywordSet of the ms of the model  for the fields 
  // //in that ms
  virtual void clearModelI(const casacore::MeasurementSet& thems) = 0;
  // // ...with field selection and optionally spw
  // static void clearModel(const casacore::MeasurementSet& thems, const casacore::String field, const casacore::String spws=casacore::String(""));

  // //Functions to see if model is defined in the casacore::MS either in the SOURCE table or else in the MAIN
  virtual casacore::Bool isModelDefinedI(const casacore::Int fieldId, const casacore::MeasurementSet& thems, casacore::String& key, casacore::Int& sourceRow) = 0;
  // static casacore::Bool isModelDefined(const casacore::String& elkey, const casacore::MeasurementSet& thems);

  // //Get a given model that is defined by key
  // //Forcing user to use a casacore::TableRecord rather than Generic casacore::RecordInterface ...just so as to avoid a copy.
  virtual casacore::Bool getModelRecordI(const casacore::String& theKey, casacore::TableRecord& theRec, const casacore::MeasurementSet& theMs) = 0;

  // // casacore::List the fields
  // static void listModel(const casacore::MeasurementSet& thems);

  // static FTMachine* NEW_FT(const casacore::Record& ftrec);
  // //check if an addFT or addCompFT is necessary
  // //casacore::Bool hasFT(casacore::Int msid, casacore::Int fieldid);
  // //casacore::Bool hasCL(casacore::Int msid, casacore::Int fieldid);
  // //returns a -1 if there is no model for this combination of ms,field,spw...but has not been  // checked yet if there is one
  // //returns a -2 if it has been tested before but does have it.
  // //returns a 1 if it has a model stored 
  virtual casacore::Int hasModel(casacore::Int msid, casacore::Int field, casacore::Int spw) = 0; 
  
  virtual casacore::Bool isVersion2() = 0;
  ///Make sure that VisModel has been called once loaded checks and
  ///detection of version 1 or 2 is valid
  virtual void init(const VisBuffer& vb) = 0;
  virtual void init(const vi::VisBuffer2& vb) = 0;

 private:

  static Factory factory_p;
  static Factory factory2_p;

};



}

#endif // ! defined (MSVIS_VisModelDataI_H)
