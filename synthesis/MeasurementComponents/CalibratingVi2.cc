//# CalibratingVi2.cc: Implementation of the CalibratingVi2 class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#include <synthesis/MeasurementComponents/CalibratingVi2.h>
#include <synthesis/MeasurementComponents/Calibrater.h>
#include <synthesis/CalLibrary/CalLibraryTools.h>
#include <casa/Arrays/ArrayPartMath.h>
#include <casa/Arrays/MaskArrMath.h>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN




// -----------------------------------------------------------------------
//

CalibratingParameters::CalibratingParameters() :
  byCalLib_p(false),
  calLibRecord_p(Record()),
  corrFactor_p(1.0)  // temporary, for initial testing  (default is a non-trivial factor)
{}

CalibratingParameters::CalibratingParameters(const Record& calLibRecord) :
  byCalLib_p(false),
  calLibRecord_p(calLibRecord),
  corrFactor_p(1.0)  
{

  if (calLibRecord_p.isDefined("calfactor")) {
    //cout << "CalibratingParameters::ctor:  found calfactor." << endl;
    // Detect calfactor in the specified Record
    corrFactor_p=calLibRecord_p.asFloat("calfactor");
    byCalLib_p = false;  // signal not using a real callib
  }
  else if (calLibRecord_p.nfields()>0) {
    //cout << "CalibratingParameters::ctor:  found non-trivial callib." << endl;
    // Apparently this will be a real callib
    byCalLib_p = true;   // signal using a real callib
  }
  else
    throw(AipsError("Invalid use of callib Record"));
  validate();
}

// Construct using callib parser
CalibratingParameters::CalibratingParameters(const String& callib) :
  byCalLib_p(true),
  calLibRecord_p(callibSetParams(callib)),
  corrFactor_p(1.0)  
{
  validate();
}

CalibratingParameters::CalibratingParameters(Float corrFactor) :
  byCalLib_p(false),
  calLibRecord_p(Record()),
  corrFactor_p(corrFactor)  // temporary, for initial testing
{
  validate();
}

CalibratingParameters::CalibratingParameters(const CalibratingParameters& other)
{
  *this = other;
}

CalibratingParameters& CalibratingParameters::operator=(const CalibratingParameters& other)
{
  if (this != &other) {
    byCalLib_p = other.byCalLib_p;
    calLibRecord_p = other.calLibRecord_p;
    corrFactor_p = other.corrFactor_p;
    validate();
  }
  return *this;
}

Bool CalibratingParameters::byCalLib() const
{
  return byCalLib_p;
}

const Record& CalibratingParameters::getCalLibRecord() const
{
  return calLibRecord_p;
}

// temporary, for initial testing
Float CalibratingParameters::getCorrFactor() const
{
  return corrFactor_p;
}


void CalibratingParameters::setCalLibRecord(const Record& calLibRecord)
{
  calLibRecord_p = calLibRecord;
}

// temporary, for initial testing
void CalibratingParameters::setCorrFactor(Float corrFactor)
{
  corrFactor_p = corrFactor;
}

void CalibratingParameters::validate() const
{
  // nothing meaningful to do yet
}



// -----------------------------------------------------------------------
CalibratingVi2::CalibratingVi2(	vi::ViImplementation2 * inputVii,
				const CalibratingParameters& calpar) :
  TransformingVi2 (inputVii),
  cb_p(0),
  ve_p(0),
  corrFactor_p(calpar.getCorrFactor()), // temporary
  visCalibrationOK_p(False)
{

  // Initialize underlying ViImpl2
  getVii()->originChunks();
  getVii()->origin();
 
  // Make the internal VisBuffer2 for CalibratingVi2 clients
  setVisBuffer(createAttachedVisBuffer (VbRekeyable));

}

// -----------------------------------------------------------------------
CalibratingVi2::CalibratingVi2( vi::ViImplementation2 * inputVii,
                                const CalibratingParameters& calpar,
                                String msname) :
  TransformingVi2 (inputVii),
  //cb_p(new OldCalibrater(msname)),
  cb_p(Calibrater::factory(msname)),
  ve_p(0),
  corrFactor_p(1.0),
  visCalibrationOK_p(False)
{

  if (calpar.byCalLib()) {
    // Arrange calibration
    cb_p->validatecallib(calpar.getCalLibRecord());
    cb_p->setcallib2(calpar.getCalLibRecord(),&(inputVii->ms())); // Use underlying MS!
    cb_p->applystate();
    // Point to VisEquation
    ve_p = cb_p->ve();
  }
  else {
    // Simple mode using only the calfactor (good for tests)
    corrFactor_p=calpar.getCorrFactor();
  }

  // Initialize underlying ViImpl2
  getVii()->originChunks();
  getVii()->origin();
 
  // Make a VisBuffer for CalibratingVi2 clients 
  setVisBuffer(createAttachedVisBuffer (VbRekeyable));

}
// -----------------------------------------------------------------------
CalibratingVi2::CalibratingVi2( vi::ViImplementation2 * inputVii,
                                VisEquation *ve) :
  TransformingVi2 (inputVii),
  cb_p(0),
  ve_p(ve),
  corrFactor_p(1.0),
  visCalibrationOK_p(False)
{

  // Initialize underlying ViImpl2
  getVii()->originChunks();
  getVii()->origin();
 
  // Make a VisBuffer for CalibratingVi2 clients
  setVisBuffer(createAttachedVisBuffer (VbRekeyable));

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
CalibratingVi2::~CalibratingVi2()
{
  //  cout << "  ~CalVi2:      " << this << endl;
  // ve_p is a borrowed pointer, so need not delete here
  ve_p=0;

  // Delete Calibrater object if present
  if (cb_p) delete cb_p;
  cb_p=0;

}



// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void
CalibratingVi2::origin() 
{

  // Drive underlying VII2
  getVii()->origin();

  // Keep my VB2 happily synchronized
  //  (this comes from TransformingVi2)
  configureNewSubchunk();

  // Data/wts not yet corrected
  visCalibrationOK_p=False;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void CalibratingVi2::next() 
{

  // Drive underlying VII2
  getVii()->next();

  // Keep my VB2 happily synchronized
  //  (this comes from TransformingVi2)
  configureNewSubchunk();

  // Data/wts not yet corrected
  visCalibrationOK_p=False;

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void CalibratingVi2::flag(Cube<Bool>& flagC) const
{
  //  cout << "CVI2::flag(Cube)...";

  // Call for correction, which might set some flags
  calibrateCurrentVB();

  // copy result to caller's Cube<Bool>
  flagC.assign(getVii()->getVisBuffer()->flagCube());

}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void CalibratingVi2::weight(Matrix<Float>& wt) const
{
 
  //  cout << "CVI2::weight...";

  // Call for correction
  //   TBD: optimize w.r.t. calibrating only the weights?
  calibrateCurrentVB();

  // copy result to caller's Matrix<Float>
  wt.assign(getVii()->getVisBuffer()->weight());

}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void CalibratingVi2::weightSpectrum(Cube<Float>& wtsp) const
{

  //  cout << "CVI2::weightSpectrum...";

  if (this->weightSpectrumExists()) {

    // Call for correction
    //   TBD: optimize w.r.t. calibrating only the weights?
    calibrateCurrentVB();
    
    // copy result to caller's Matrix<Float>
    wtsp.assign(getVii()->getVisBuffer()->weightSpectrum());

  }
  else {
    // same as ordinary VisibilityIteratorImpl2
    wtsp.resize(0,0,0);
  }
}




// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void CalibratingVi2::visibilityCorrected(Cube<Complex>& vis) const
{

  //  cout << "CVI2::visibilityCorrected...";

  // TBD:
  //  o consider if underlying VisBuffer should be maintained const? 
  //       (and this layer's VB set and adjusted)
  //       (will this break on-demand VB stuff?)
  //  o make corresponding visibilityModel method...  (solve context)

  // Call the actual correction method
  //  (only does the actual work, if needed)
  calibrateCurrentVB();

  // copy result to caller's Cube<Complex>
  vis.assign(getVii()->getVisBuffer()->visCubeCorrected());

}



// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool CalibratingVi2::existsColumn(VisBufferComponent2 id) const
{
    Bool result;
    switch (id){

    case VisBufferComponent2::VisibilityCorrected:
    case VisBufferComponent2::VisibilityCubeCorrected:
    case VisBufferComponent2::VisibilityCubeModel:

        result = true;
        break;

    default:
        result = getVii()->existsColumn(id);
        break;
    }

    return result;

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void CalibratingVi2::calibrateCurrentVB() const
{
  // This method must call NO ordinary VB2 accessors that require ViImpl 
  //   methods that are defined in this class, _even_implicitly_, because 
  //   the VB2 uses the VI2 that has its ViImpl overridden by these local 
  //   methods.  This causes infinite loops!!!
  // One way to avoid this is to ensure that every method in this class 
  //   initializes the VB2 fields via getVii() methods.....

  //  cout << " calibrateCurrentVB(): " << boolalpha << visCalibrationOK_p;

  // Do the correction, if not done yet
  if (!visCalibrationOK_p) {

    // We will operate on the underlying VB
    VisBuffer2 *vb = getVii()->getVisBuffer();

    // sense if WEIGHT_SPECTRUM exists
    Bool doWtSp = getVii()->weightSpectrumExists();

    // Fill current flags, raw weights, and raw vis
    vb->flagCube();
    vb->resetWeightsUsingSigma();  //  this is smart re spec weights or not

    // Initialize "corrected" data from "raw" data
    initCorrected(vb);

    // If the VisEquation is set, use it, otherwise use the corrFactor_p
    if (ve_p) {
      // Apply calibration via the VisEquation
      ve_p->correct2(*vb,False,doWtSp);

      // Set unchan'd weights, in case they are requested
      if (doWtSp)
	vb->setWeight(partialMedians(vb->weightSpectrum(),IPosition(1,1)));
	
    }
    else {
    
      // Use supplied corrFactor_p to make this corrected data look changed
      Cube<Complex> vCC(vb->visCubeCorrected());
      vCC*=corrFactor_p;
      vb->setVisCubeCorrected(vCC);
      
      if (doWtSp) {
	// Calibrate the WS
	Cube<Float> wS(vb->weightSpectrum());   // Was set above
	wS/=(corrFactor_p*corrFactor_p);
	vb->setWeightSpectrum(wS);
	// Set W via median on chan axis
	vb->setWeight(partialMedians(wS,IPosition(1,1)));
      }
      else {
	// Just calibrate the W
	Matrix<Float> w(vb->weight());          // Was set above
	w/=(corrFactor_p*corrFactor_p);
	vb->setWeight(w);
      }
    }

    // Signal that we have applied the correction, to avoid unnecessary redundancy
    visCalibrationOK_p=True;

    //    cout << "-->" << visCalibrationOK_p;

  }    
  //  cout << endl;
}

void CalibratingVi2::initCorrected(casa::vi::VisBuffer2* vb) const {

  if (getVii()->existsColumn(VisBufferComponent2::VisibilityCubeFloat)) {
    // Convert FLOAT_DATA to Complex, and assign
    Cube<Float> f(vb->visCubeFloat());
    Cube<Complex> c;
    c.resize(f.shape());
    convertArray(c,f);
    vb->setVisCubeCorrected(c);
  }
  else
    // Just copy the (already-Complex DATA column)
    vb->setVisCubeCorrected(vb->visCube());

  //  cout << "CalVi2::setCorrected()" << endl;
}

CalVi2LayerFactory::CalVi2LayerFactory(const CalibratingParameters& pars)
  : ViiLayerFactory(),
    calpars_(pars)
{}


// CalVi2-specific layer-creater
ViImplementation2 * CalVi2LayerFactory::createInstance (ViImplementation2* vii0) const {
  // Make the CalibratingVi2, using supplied ViImplementation2, and return it
  ViImplementation2 *vii = new CalibratingVi2(vii0,calpars_);
  return vii;
}


 /********************************/


// -----------------------------------------------------------------------
CalSolvingVi2::CalSolvingVi2(	vi::ViImplementation2 * inputVii,
				const CalibratingParameters& calpar) :
  CalibratingVi2 (inputVii,calpar),
  corrDepFlags_(false)
{
  corrFactor_p=sqrt(corrFactor_p);
}

// -----------------------------------------------------------------------
CalSolvingVi2::CalSolvingVi2( vi::ViImplementation2 * inputVii,
                                const CalibratingParameters& calpar,
                                String msname) :
  CalibratingVi2 (inputVii,calpar,msname),
  corrDepFlags_(false)
{
  // Nothing specialized to do here (except ctor parent, above)
}

// -----------------------------------------------------------------------
CalSolvingVi2::CalSolvingVi2( vi::ViImplementation2 * inputVii,
			      VisEquation *ve,
			      const Bool& corrDepFlags) :
  CalibratingVi2 (inputVii,ve),
  corrDepFlags_(corrDepFlags)
{
  // Nothing specialized to do here (except ctor parent, above)
}

//#define REPORTTIMING

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
CalSolvingVi2::~CalSolvingVi2()
{
  //  cout << "   ~CalSolVi2:  " << this << endl;
  //  cout << "CSVi2::calibrateCurrentVB: Is WtSpec automatic?" << endl;
  // Nothing specialized to do here (except ctor parent, above)

#ifdef REPORTTIMEING
  cout << "CalSolvingVi2 stats: " << endl;
  cout << " nVB_=" << nVB_
       << " nVB0_=" << nVB0_ << endl;

  cout << " Tcal_=" 
       << Tcalws_ 
       << "+" << Tcalfl_ 
       << "+" << Tcal2_ 
       << "=" << Tcalws_+Tcalfl_+Tcal2_
       << endl;

  cout << " Tio_=" << Tio_ 
       << endl;
  cout << "  Samples=" 
       << " total=" << ntotal_ 
       << " flagged=" << nflagged_ << " (" << Double(nflagged_)/Double(ntotal_) << " of total)"
       << " skippable=" << nskipped_ << " (" << Double(nskipped_)/Double(nflagged_) << " of flagged)"
       << endl;
  cout << " spurious fraction=" << Double(nflagged_-nskipped_)/Double(ntotal_-nskipped_) << endl;
  cout << " good     fraction=" << Double(ntotal_-nflagged_)/Double(ntotal_-nskipped_) << endl;
#endif


}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void
CalSolvingVi2::originChunks(casacore::Bool forceRewind) 
{

  // Zero counters/timers
  ntotal_=nflagged_=nskipped_=nVB_=nVB0_=0;
  Tio_=Tcalws_=Tcalfl_=Tcal2_=0.0;

  // Call next layer
  getVii()->originChunks(forceRewind);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void
CalSolvingVi2::origin() 
{

  // Drive underlying VII2
  getVii()->origin();

  //ntotal_+=getVii()->getVisBuffer()->flagCube().nelements();
  //nflagged_+=ntrue(getVii()->getVisBuffer()->flagCube());

  //  if (nfalse(getVii()->getVisBuffer()->flagCube())==0) {
  //    cout << "Using (origin)..." << endl;
  //  }

  /*

  // Step over completely flagged VB2s in the current chunk
  // NB: last one will be used in any case
  if (getVii()->more()) {  // not already the last VB2 in this chunk
    while (getVii()->more()) { 
      if (nfalse(getVii()->getVisBuffer()->flagCube())==0) {
	// This VB2 is entirely flagged, step to next one
	getVii()->next();
	cout << "skipping (origin)..." << endl;
      }
      else
	// This VB2 has some unflagged data, so use it
	break;
    }
  }
  */
  /*
  cout << "Samples (origin) =" 
       << " total=" << ntotal_ 
       << " flagged=" << nflagged_ 
       << " skipped=" << nskipped_ 
       << endl;
  */
  // Keep my VB2 happily synchronized
  //  (this comes from TransformingVi2)
  configureNewSubchunk();

  // Data/wts not yet corrected
  visCalibrationOK_p=False;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void CalSolvingVi2::next() 
{

  // Drive underlying VII2
  getVii()->next();

  /*

  // Step over completely flagged VB2s in the current chunk
  // NB: last one will be used in any case
  if (getVii()->more()) {  // not already the last VB2 in this chunk
    while (getVii()->more()) { 
      if (nfalse(getVii()->getVisBuffer()->flagCube())==0) {
	// This VB2 is entirely flagged, step to next one
	ntotal_+=getVii()->getVisBuffer()->flagCube().nelements();
	nflagged_+=ntrue(getVii()->getVisBuffer()->flagCube());
	nskipped_+=ntrue(getVii()->getVisBuffer()->flagCube());
	getVii()->next();
      }
      else
	// This VB2 has some unflagged data, so use it
	break;
    }
  }
  */

  // Keep my VB2 happily synchronized
  //  (this comes from TransformingVi2)
  configureNewSubchunk();

  // Data/wts not yet corrected
  visCalibrationOK_p=False;

  /*
  cout << "Samples (next)   =" 
       << " total=" << ntotal_ 
       << " flagged=" << nflagged_ 
       << " skipped=" << nskipped_ 
       << endl;
  */

}



// -----------------------------------------------------------------------
void CalSolvingVi2::visibilityModel(Cube<Complex>& mod) const
{

  //  cout << "CSVI2::visibilityModel...";

  // Call the actual correction method
  //  (only does the actual work, if needed)
  calibrateCurrentVB();

  // copy result to caller's Cube<Complex>
  //  (this is no-op when mod has same data address as visCubeModel())
  mod.assign(getVii()->getVisBuffer()->visCubeModel());

}

// -----------------------------------------------------------------------
void CalSolvingVi2::calibrateCurrentVB() const
{

  //cout << " CalSolvingVi2::calibrateCurrentVB(): " << boolalpha << visCalibrationOK_p;


  // Do the correction, if not done yet
  if (!visCalibrationOK_p) {

#ifdef _OPENMP
    Double time0=omp_get_wtime();
#endif

    // Count the VBs that we are processing
    ++nVB_;

    // Get the underlying ViImpl2's VisBuffer, to munge it
    VisBuffer2 *vb = getVii()->getVisBuffer();

    // sense if WEIGHT_SPECTRUM exists
    //  (should be true in CalSolvingVi2!)
    Bool doWtSp = this->weightSpectrumExists();

    // Fill flags
    Cube<Bool> fl(vb->flagCube());

    // Fill model
    // NB:  model-filling handled below, in ve_p (or on-demand)
    vb->visCubeModel();

    // Initialize the to-be-calibrated weights 
    //   (this fills WS, if available from below)
    //  TBD: better semantics: vb->setCorrDataWtSpec(vb->dataWtSpec()); 
    vb->resetWeightsUsingSigma();

    // Initialize corrected data w/ data
    initCorrected(vb);

    // Counting
    Int64 nsamp=fl.nelements();
    if (nfalse(fl)==0) {
      nskipped_+=nsamp;
      ++nVB0_;
    }
    ntotal_+=nsamp;
    nflagged_+=ntrue(fl);

#ifdef _OPENMP
    Double time1a=omp_get_wtime();
#endif

    // Ensure we got weightSpectrum (fill it, if not)
    verifyWeightSpectrum(vb);

#ifdef _OPENMP
    Double time1b=omp_get_wtime();
#endif

    // Set old-style flags, FOR NOW
    if (!corrDepFlags_)
      corrIndepFlags(vb);
    
#ifdef _OPENMP
    Double time2=omp_get_wtime();
#endif

    // If the VisEquation is set, use it, otherwise use the corrFactor_p
    if (ve_p) {
      
      // Apply calibration via the VisEquation
      ve_p->collapse2(*vb);   // ,False,doWtSp);

      // Set unchan'd weights, in case they are requested
      //if (doWtSp)
      //  vb->setWeight(partialMedians(vb->weightSpectrum(),IPosition(1,1)));
	
    }
    else {
    
      // Use supplied corrFactor_p to make this corrected data look changed
      //  In leiu of working VisEquation that is TBD

      // Correct data
      Cube<Complex> vCC(vb->visCubeCorrected());
      vCC*=corrFactor_p;
      vb->setVisCubeCorrected(vCC);

      // Corrupt the model
      Cube<Complex> vCM(vb->visCubeModel());
      vCM/=corrFactor_p;
      vb->setVisCubeModel(vCM);

      Cube<Float> modA2(square(amplitude(vCM)));
      Cube<Bool> modmask(modA2>0.0f);

      MaskedArray<Complex> vCCm=vCC(modmask);
      MaskedArray<Complex> vCMm=vCM(modmask);
      MaskedArray<Float> modA2m(modA2(modmask));

      // Divide corr data by mod data (where mod non-zero)
      vCCm=vCCm/vCMm;
      vCMm=Complex(1.0);  // model now unity
      //vCMm=vCMm/vCMm;        // less efficient?
     
      if (doWtSp) {
	// Calibrate the WS
	Cube<Float> wS(vb->weightSpectrum());   // Was set above
	wS/=(corrFactor_p*corrFactor_p);

	// adjust for model division
	MaskedArray<Float> wSm(wS(modmask));
	wSm=wSm*modA2m;

	vb->setWeightSpectrum(wS);
	// Set W via median on chan axis
	vb->setWeight(partialMedians(wS,IPosition(1,1)));
      }
      else {
	// Just calibrate the W
	Matrix<Float> w(vb->weight());          // Was set above
	w/=(corrFactor_p*corrFactor_p);
	vb->setWeight(w);
      }
    }

    // Signal that we have applied the correction, to avoid unnecessary redundancy
    visCalibrationOK_p=True;
    
#ifdef _OPENMP
    Double time3=omp_get_wtime();

    Double dTio=time1a-time0;
    Double dTcalws=time1b-time1a;
    Double dTcalfl=time2-time1b;
    Double dTcal2=time3-time2;
    Tio_+=dTio;
    Tcalws_+=dTcalws;
    Tcalfl_+=dTcalfl;
    Tcal2_+=dTcal2;
#endif
    
    /*
    cout << "nVB_=" << nVB_
	 << " dTio = " << dTio
	 << " dTcal1 = " << dTcal1
	 << " dTcal2 = " << dTcal2
	 << " Tio_ = " << Tio_ 
	 << " Tcal = " << Tcal1 
	 << "+" << Tcal2_ 
	 << "=" << Tcal1_+Tcal2_
	 << endl;
    */

  }
  
  //  cout << endl;
}

void CalSolvingVi2::corrIndepFlags(casa::vi::VisBuffer2* vb) const {

  if (vb->nCorrelations()==1)
    // Nothing to do if only one correlation
    return;

  // If more than one correlation, if any is flagged, all are (per row, chan)
  Cube<Bool> flc(vb->flagCube());

  VectorIterator<Bool> fvi(flc,0);
  Vector<Bool>& fviv(fvi.vector());  // stays sync'd!
  Int nCor=fviv.nelements();
  while (!fvi.pastEnd()) {
    for (Int icor=0;icor<nCor;++icor) {
      if (fviv(icor)) {
	fviv.set(true);
	continue;  // jump out of icor loop if flag=T found
      }
    }
    fvi.next();
  }

  return;
}

void CalSolvingVi2::verifyWeightSpectrum(casa::vi::VisBuffer2* vb) const {

  // If we didn't get WS from below, populate it in the specified vb
  if (!getVii()->weightSpectrumExists()) {
    IPosition sh=vb->getShape();
    Cube<Float> wtsp(sh,0.0f);

    // Unchan'd weight as Cube w/ degenerate chan axis
    Cube<Float> wtsp0(vb->weight().reform(IPosition(3,sh(0),1,sh(2))));
    
    VectorIterator<Float> ivi(wtsp0,1);
    Vector<Float>& iviv(ivi.vector());   // stays sync'd!
    VectorIterator<Float> ovi(wtsp,1);
    Vector<Float>& oviv(ovi.vector());   // stays sync'd!
    
    while (!ivi.pastEnd()) {
      oviv.set(iviv(0));
      ivi.next();
      ovi.next();
    }
    
    // Set it in the vb
    vb->setWeightSpectrum(wtsp);
  }
  
  // In all cases set flagged cells' weights to zero
  //  (This is ok in solving context.)
  Cube<Float> wtsp(vb->weightSpectrum());
  wtsp(vb->flagCube())=0.0;

}

CalSolvingVi2LayerFactory::CalSolvingVi2LayerFactory(const CalibratingParameters& pars)
  : CalVi2LayerFactory(pars)
{}


// CalSolvingVi2-specific layer-creater
ViImplementation2 * CalSolvingVi2LayerFactory::createInstance (ViImplementation2* vii0) const {
  // Make the CalibratingVi2, using supplied ViImplementation2, and return it
  ViImplementation2 *vii = new CalSolvingVi2(vii0,calpars_);
  return vii;
}



CalSolvingVi2LayerFactoryByVE::CalSolvingVi2LayerFactoryByVE(VisEquation *ve,const casacore::Bool& corrDepFlags)
  : ViiLayerFactory(),
    ve_p(ve),
    corrDepFlags_(corrDepFlags)
{
  //  ve_p->state();

}

// CalSolvingVi2-specific layer-creater
ViImplementation2 * CalSolvingVi2LayerFactoryByVE::createInstance (ViImplementation2* vii0) const {
  // Make the CalibratingVi2, using supplied ViImplementation2, and return it
  ViImplementation2 *vii = new CalSolvingVi2(vii0,ve_p,corrDepFlags_);
  return vii;
}








} //# NAMESPACE VI - END
} //# NAMESPACE CASA - END


