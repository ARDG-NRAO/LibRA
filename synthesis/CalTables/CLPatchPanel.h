//# CLPatchPanel.h: Definition for Calibration patch panel
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#ifndef CALTABLES_CLPATCHPANEL_H
#define CALTABLES_CLPATCHPANEL_H

#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTColumns.h>
#include <synthesis/CalTables/CTTimeInterp1.h>
#include <synthesis/CalTables/RIorAParray.h>
#include <synthesis/CalTables/VisCalEnum.h>
#include <scimath/Mathematics/InterpolateArray1D.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MeasurementSets/MSField.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/aips.h>

//#include <casa/BasicSL/Constants.h>
//#include <casa/OS/File.h>
//#include <casa/Logging/LogMessage.h>
//#include <casa/Logging/LogSink.h>

namespace casacore{

class MSSelectableTable;
}

namespace casa { //# NAMESPACE CASA - BEGIN


// A specialized key class for Calibration patch panel elements
class CalPatchKey
{
public:
  CalPatchKey(casacore::IPosition keyids);
  virtual ~CalPatchKey() {};
  virtual casacore::Bool operator<(const CalPatchKey& other) const;
  virtual casacore::String print() const =0;
private:
  casacore::Vector<casacore::Int> cpk_;
};

class MSCalPatchKey : virtual public CalPatchKey
{
public:
  MSCalPatchKey(casacore::Int obs,casacore::Int fld,casacore::Int ent,casacore::Int spw,casacore::Int ant=-1);
  virtual ~MSCalPatchKey(){};
  virtual casacore::String print() const;
private:
  casacore::Int obs_,fld_,ent_,spw_,ant_;
};


class CTCalPatchKey : virtual public CalPatchKey
{
public:
  CTCalPatchKey(casacore::Int clsl,casacore::Int obs,casacore::Int fld,casacore::Int spw,casacore::Int ant=-1);
  virtual ~CTCalPatchKey(){};
  virtual casacore::String print() const;
private:
  casacore::Int clsl_,obs_,fld_,spw_,ant_;
};

// A base class for calmaps
class CalMap
{
public:

  // Null ctor (defaults everything)
  CalMap();

  // Construct from a Vector
  CalMap(const casacore::Vector<casacore::Int>& calmap);  

  // Construct from a casacore::Record (int:int; TBD: need an casacore::MS to parse strings)
  //  CalMap(const casacore::Record calmap):

  // Construct from a casacore::String (an algorithm or selection)
  //CalMap(casacore::String calmaptype,const NewCalTable& ct, const casacore::MeasurementSet ms,
  //	 casacore::Vector<casacore::Int> availids=casacore::Vector<casacore::Int>());
  // Construct from casacore::String and 2 Vectors (default, and explict to/from override)
  // TBD

  // Index method returns the ct id mapped to the specified msid
  casacore::Int operator()(casacore::Int msid) const; // ,const casacore::Vector<casacore::Int> availids=casacore::Vector<casacore::Int>()) const;

  // Return the ctids required for a set a msids (from among those available)
  //  TBD: what to do if nominally required ctid is UNavailable (return only the avail ones?)
  casacore::Vector<casacore::Int> ctids(const casacore::Vector<casacore::Int>& msids) const;//   const casacore::Vector<casacore::Int> availctids=casacore::Vector<casacore::Int>(0)) const;

  // Return the msids that will be satisfied by a specific ctid
  casacore::Vector<casacore::Int> msids(casacore::Int ctid,const casacore::Vector<casacore::Int>& superset=casacore::Vector<casacore::Int>()) const;

  // Return the verbatim (casacore::Vector) map
  casacore::Vector<casacore::Int> vmap() const { return vcalmap_; };

protected:
  casacore::Vector<casacore::Int> vcalmap_;  // simple
  //casacore::Matrix<casacore::Int> mcalmap_;  // per-id prioritized, e.g., 'nearest'
};

class FieldCalMap : public CalMap
{
public:

  // Null ctor
  FieldCalMap();

  // Construct from casacore::Vector<casacore::Int> (override CalMap)
  FieldCalMap(const casacore::Vector<casacore::Int>& calmap);

  // Algorithmic ctor that uses casacore::MS and CT meta info
  FieldCalMap(const casacore::String fieldcalmap, const casacore::MeasurementSet& ms, const NewCalTable& ct);
  FieldCalMap(const casacore::String fieldcalmap, 
	      const casacore::MeasurementSet& ms, const NewCalTable& ct,
	      casacore::String& extfldsel);

private:

  // Calculate the simple nearest field map
  void setNearestFieldMap(const casacore::MeasurementSet& ms, const NewCalTable& ct);
  void setNearestFieldMap(const NewCalTable& ctasms, const NewCalTable& ct);
  void setNearestFieldMap(const casacore::MSFieldColumns& msfc, const ROCTColumns& ctc);

  // Parse field selection map
  void setSelectedFieldMap(const casacore::String& fieldsel,
			   const casacore::MeasurementSet& ms,const NewCalTable& ct);
  void setSelectedFieldMap(const casacore::String& fieldsel,
			   const casacore::MeasurementSet& ms,const NewCalTable& ct,
			   casacore::String& extfldsel);

  // User's specification
  casacore::String fieldcalmap_;


};




// A class to parse the contents of a single CalLib slice 
//  i.e., for one caltable, one setup
class CalLibSlice
{
public:
  CalLibSlice(casacore::String obs,casacore::String fld, casacore::String ent, casacore::String spw,
	      casacore::String tinterp,casacore::String finterp,
	      casacore::Vector<casacore::Int> obsmap=casacore::Vector<casacore::Int>(1,-1), 
	      casacore::Vector<casacore::Int> fldmap=casacore::Vector<casacore::Int>(1,-1), 
	      casacore::Vector<casacore::Int> spwmap=casacore::Vector<casacore::Int>(1,-1), 
	      casacore::Vector<casacore::Int> antmap=casacore::Vector<casacore::Int>(1,-1));
  CalLibSlice(const casacore::Record& clslice,
	      const casacore::MeasurementSet& ms=casacore::MeasurementSet(),
	      const NewCalTable& ct=NewCalTable());


  casacore::String obs,fld,ent,spw;
  casacore::String tinterp,finterp;
  CalMap obsmap, fldmap, spwmap, antmap;

  // validation
  static casacore::Bool validateCLS(const casacore::Record& clslice);

  // Extract as a record
  casacore::Record asRecord();

  // Report maps
  casacore::String state();

  // Support external field selection (for trimming in-focus caltable)
  casacore::String extfldsel;

};


class CLPPResult
{
public:

  CLPPResult();
  CLPPResult(const casacore::IPosition& shape);
  CLPPResult(casacore::uInt nPar,casacore::uInt nFPar,casacore::uInt nChan,casacore::uInt nelem);
  CLPPResult& operator=(const CLPPResult& other); // avoids deep casacore::Array copies

  void resize(casacore::uInt nPar,casacore::uInt nFPar,casacore::uInt nChan,casacore::uInt nelem);

  casacore::Matrix<casacore::Float> operator()(casacore::Int ielem) { return result_.xyPlane(ielem); };
  casacore::Matrix<casacore::Float> result(casacore::Int ielem) { return result_.xyPlane(ielem); };
  casacore::Matrix<casacore::Bool> resultFlag(casacore::Int ielem) { return resultFlag_.xyPlane(ielem); };

  casacore::Cube<casacore::Float> result_;
  casacore::Cube<casacore::Bool> resultFlag_;

  //private:

};






class CLPatchPanel
{
public:

  // From caltable name and casacore::MS shapes
  CLPatchPanel(const casacore::String& ctname,
	       const casacore::Record& callib,
	       VisCalEnum::MatrixType mtype,
	       casacore::Int nPar,
	       const CTTIFactoryPtr cttifactoryptr=&CTTimeInterp1::factory);

   // From caltable name and (selected) casacore::MS 
  CLPatchPanel(const casacore::String& ctname,
	       const casacore::MeasurementSet& ms,
	       const casacore::Record& callib,
	       VisCalEnum::MatrixType mtype,
	       casacore::Int nPar,
	       const CTTIFactoryPtr cttifactoryptr=&CTTimeInterp1::factory);
  // Destructor
  virtual ~CLPatchPanel();

  // Is specific calibration explicitly available for a obs,fld,intent,spw,ant combination?
  casacore::Bool calAvailable(casacore::Int obs, casacore::Int fld, casacore::Int ent, 
			      casacore::Int spw, casacore::Int ant=-1);
  // Are specified indices OK for this CL (i.e., not explicitly marked as expected but absent)
  //  Will be calibrated if calAvailable=true, will be ignored if calAvailable()=false
  casacore::Bool MSIndicesOK(casacore::Int obs, casacore::Int fld, casacore::Int ent, 
			     casacore::Int spw, casacore::Int ant=-1);

  // Interpolate, given input obs, field, intent, spw, timestamp, & (optionally) freq 
  //    returns T if new result (anywhere,anyhow)
  //  For casacore::Complex params (calls casacore::Float version)
  casacore::Bool interpolate(casacore::Cube<casacore::Complex>& resultC, casacore::Cube<casacore::Bool>& resFlag,
		   casacore::Int obs, casacore::Int fld, casacore::Int ent, casacore::Int spw, 
		   casacore::Double time, casacore::Double freq=-1.0);
  //  For casacore::Float params
  casacore::Bool interpolate(casacore::Cube<casacore::Float>& resultR, casacore::Cube<casacore::Bool>& resFlag,
		   casacore::Int obs, casacore::Int fld, casacore::Int ent, casacore::Int spw, 
		   casacore::Double time, casacore::Double freq=-1.0);

  // Interpolate, given input obs, field, intent, spw, timestamp, & freq list
  //   This is for freqdep interpolation context
  //    returns T if new result (anywhere,anyhow)
  //  For casacore::Complex params (calls casacore::Float version)
  casacore::Bool interpolate(casacore::Cube<casacore::Complex>& resultC, casacore::Cube<casacore::Bool>& resFlag,
		   casacore::Int obs, casacore::Int fld, casacore::Int ent, casacore::Int spw, 
		   casacore::Double time, const casacore::Vector<casacore::Double>& freq);
  //  For casacore::Float params
  casacore::Bool interpolate(casacore::Cube<casacore::Float>& resultR, casacore::Cube<casacore::Bool>& resFlag,
		   casacore::Int obs, casacore::Int fld, casacore::Int ent, casacore::Int spw, 
		   casacore::Double time, const casacore::Vector<casacore::Double>& freq);

  // Const access to various state
  // TBD

  casacore::Bool getTresult(casacore::Cube<casacore::Float>& resultR, casacore::Cube<casacore::Bool>& resFlag,
		  casacore::Int obs, casacore::Int fld, casacore::Int ent, casacore::Int spw);


  // Access to CalTable's freq info
  const casacore::Vector<casacore::Double>& freqIn(casacore::Int spw)  {return freqIn_[spw]; }; // per spw
  const casacore::Vector<casacore::Double>& refFreqIn() { return refFreqIn_; };  // indexed by spw

  // Report state
  void listmappings();
  void state();

  // CASA version of the internal caltable
  casacore::String CTCASAvers() { return ct_.CASAvers(); };

private:

  // Null ctor does nothing
  CLPatchPanel() :mtype_(VisCalEnum::GLOBAL) {};



  // Methods to support layered selection
  void selectOnCTorMS(casacore::Table& ctout,casacore::MSSelectableTable& msst,
		      const casacore::String& obs, const casacore::String& fld, 
		      const casacore::String& ent,
		      const casacore::String& spw, const casacore::String& ant,
		      const casacore::String& taql);
  void selectOnMS(casacore::MeasurementSet& msout,const casacore::MeasurementSet& msin,
		  const casacore::String& obs, const casacore::String& fld, 
		  const casacore::String& ent,
		  const casacore::String& spw, const casacore::String& ant);
  void selectOnCT(NewCalTable& ctout,const NewCalTable& ctin,
		  const casacore::String& obs, const casacore::String& fld, 
		  const casacore::String& spw, const casacore::String& ant1);

  // Extract unique indices from caltables
  casacore::Vector<casacore::Int> getCLuniqueObsIds(NewCalTable& ct) {return getCLuniqueIds(ct,"obs"); };
  casacore::Vector<casacore::Int> getCLuniqueFldIds(NewCalTable& ct) {return getCLuniqueIds(ct,"fld"); };
  casacore::Vector<casacore::Int> getCLuniqueSpwIds(NewCalTable& ct) {return getCLuniqueIds(ct,"spw"); };
  casacore::Vector<casacore::Int> getCLuniqueIds(NewCalTable& ct, casacore::String vcol);

  // Extract unique indices (net selected) from MS
  casacore::Vector<casacore::Int> getMSuniqueIds(casacore::MeasurementSet& ms, casacore::String vcol);
  
  // Set generic antenna/baseline map
  //  void setElemMap();

  // Resample in frequency
  void resampleInFreq(casacore::Matrix<casacore::Float>& fres,casacore::Matrix<casacore::Bool>& fflg,const casacore::Vector<casacore::Double>& fout,
		      casacore::Matrix<casacore::Float>& tres,casacore::Matrix<casacore::Bool>& tflg,const casacore::Vector<casacore::Double>& fin,
		      casacore::String finterp);
  void resampleFlagsInFreq(casacore::Vector<casacore::Bool>& flgout,const casacore::Vector<casacore::Double>& fout,
                           casacore::Vector<casacore::Bool>& flgin,const casacore::Vector<casacore::Double>& fin,
			   casacore::String finterp);

  // Baseline index from antenna indices: (assumes a1<=a2 !!)
  //  inline casacore::Int blnidx(const casacore::Int& a1, const casacore::Int& a2, const casacore::Int& nAnt) { return  a1*nAnt-a1*(a1+1)/2+a2; };

  // Translate freq axis interpolation string
  casacore::InterpolateArray1D<casacore::Double,casacore::Float>::InterpolationMethod ftype(casacore::String& strtype);


  void recordBadMSIndices(const casacore::Vector<casacore::Int>& obs, const casacore::Vector<casacore::Int>& fld,
			  const casacore::Vector<casacore::Int>& ent, const casacore::Vector<casacore::Int>& spw);



  // PRIVATE DATA:
  
  // The Caltable
  NewCalTable ct_, ctasms_;

  // Associated (selected) MS
  casacore::MeasurementSet ms_;

  // casacore::Matrix type
  VisCalEnum::MatrixType mtype_;

  // Are parameters fundamentally complex?
  casacore::Bool isCmplx_;

  // The number of (casacore::Float) parameters (per-chan, per-element)
  casacore::Int nPar_, nFPar_;

  //  casacore::InterpolateArray1D<casacore::Double,casacore::Float>::InterpolationMethod ia1dmethod_;

  // CalTable freq axis info
  casacore::Vector<casacore::Int> nChanIn_;
  casacore::Vector<casacore::Vector<casacore::Double> > freqIn_;
  casacore::Vector<casacore::Double> refFreqIn_;

  // Obs, Field, Spw, Ant _output_ (casacore::MS) sizes 
  //   calibration required for up to this many
  casacore::Int nMSObs_, nMSFld_, nMSSpw_, nMSAnt_, nMSElem_;

  // Obs, Field, Spw, Ant _input_ (CalTable) sizes
  //  patch panels should not violate these (point to larger indices)
  casacore::Int nCTObs_, nCTFld_, nCTSpw_, nCTAnt_, nCTElem_;

  // Maps
  std::map<CTCalPatchKey,CLPPResult> clTres_;
  std::map<CTCalPatchKey,casacore::String> ciname_;
  std::map<CTCalPatchKey,CTTimeInterp1*> ci_;
  

  std::map<MSCalPatchKey,CLPPResult> msTres_,msFres_;
  std::map<MSCalPatchKey,casacore::String> msciname_;
  std::map<MSCalPatchKey,casacore::String> badmsciname_;
  std::map<MSCalPatchKey,casacore::Int> ctspw_;
  std::map<MSCalPatchKey,casacore::String> finterp_;
  std::map<MSCalPatchKey,CTTimeInterp1*> msci_;

  // Keep track of last cal result address (per spw)
  casacore::Vector<casacore::Float*> lastresadd_;  // [nMSspw_]

  // Control conjugation of baseline-based solutions when mapping requires
  //  casacore::Vector<casacore::Bool> conjTab_;


  casacore::Cube<casacore::Float> result_;
  casacore::Cube<casacore::Bool> resFlag_;

  casacore::LogIO logsink_;

  // The factory to use to create CTTimeInterp1s
  CTTIFactoryPtr cttifactoryptr_;

};


} //# NAMESPACE CASA - END

#endif
