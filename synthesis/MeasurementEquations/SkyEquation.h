//# SkyEquation.h: SkyEquation definition
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_SKYEQUATION_H
#define SYNTHESIS_SKYEQUATION_H

#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <synthesis/TransformMachines/FTMachine.h>
#include <msvis/MSVis/VisBuffer.h>
#include <ms/MeasurementSets/MSMainEnums.h>

namespace casacore{

class UVWMachine;
template <class T> class ImageInterface;
template <class T> class TempImage;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> Relate Sky brightness to the visibility </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto module="MeasurementComponents">MeasurementComponents</linkto> module
// </prerequisite>
//
// <etymology>
// Sky Equation encapsulates the equation between the sky brightness
// and the visibility (or coherence) measured by a generic instrument
// </etymology>
//
// <synopsis> 
// This is responsible for the Sky-based part of Measurement Equation of the Generic
// Interferometer due to Hamaker, Bregman and Sault and later extended
// by Noordam, and Cornwell.
//
// See <linkto module="MeasurementEquations">MeasurementEquations</linkto>
// for more details of the form of the SkyEquation.
//
// The principal use of SkyEquation is that, as described in 
// <linkto module="MeasurementEquations">MeasurementEquations</linkto>,
// the gradients of chi-squared may be calculated and returned
// as an image.
//
// The following components can be plugged into SkyEquation
// <ul>
// <li> Antenna-based direction-dependent terms: <linkto class="SkyJones">SkyJones</linkto>
// <li> Sky brightness model: <linkto class="SkyModel">SkyModel</linkto>
// <li> Fourier transform machine: <linkto class="FTMachine">FTMachine</linkto>
// </ul>
// </synopsis> 
//
// <example>
// <srcblock>
//
//      // Read the VisSet from disk
//      VisSet vs("3c84.MS");
//
//      casacore::PagedImage<casacore::Float> im("3c84.modelImage");
//
//      // Create an ImageSkyModel from an image on disk
//      ImageSkyModel ism(im);
//
//      // FTMachine
//      GridFT ft;
//
//      SkyEquation se(ism, vs, ft);
//
//      // Make a Clean Image and write it out
//      HogbomCleanImageSkyModel csm(ism);
//      if (csm.solveSkyModel()) {
//        casacore::PagedImage<casacore::Float> cleanImage=csm.getImage();
//        cleanImage.setName("3c84.cleanImage");
//      }
//
// </srcblock>
// </example>
//
// <motivation>
// SkyEquation is needed to encapsulate part of the measurement equation for 
// both single dish and synthesis observations. The idea is that the structure of many
// imaging algorithms is much the same for many different types of telescope.
// SkyEquation is part of a framework of classes that are
// designed for synthesis and single dish imaging. The others are the 
// <linkto module=MeasurementComponents>MeasurementComponents</linkto>.
//
// </motivation>
//
// <todo asof="98/2/17">
// <li> Implement SkyJones
// </todo>

// Forward declarations
class SkyJones;
class SkyModel;
class VisSet;
class FTMachine;
class SkyComponent;
class ComponentList;
class ComponentFTMachine;
class ROVisibilityIterator;
class VisibilityIterator;


class SkyEquation {
public:

  // Define a SkyEquation linking a VisSet vs with a SkyModel sm
  // using an FTMachine ft for transforms in both directions
  SkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft);
  
  // Define a SkyEquation linking a VisSet vs with a SkyModel sm
  // using an FTMachine ft for transforms in both directions and
  // a ComponentFTMachine for the component lists
  SkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft,
	      ComponentFTMachine& cft,  casacore::Bool noModelcol=false);


  //SkyEquation with ROVisIter
  SkyEquation(SkyModel& sm, ROVisibilityIterator& vi, FTMachine& ft,
	      ComponentFTMachine& cft, casacore::Bool noModelCol);

  // Define a SkyEquation linking a VisSet vs with a SkyModel sm
  // using an FTMachine ft for Sky->Vis and ift for Vis->Sky
  SkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft, FTMachine& ift);

  // Define a SkyEquation linking a VisSet vs with a SkyModel sm
  // using an FTMachine ft for Sky->Vis and ift for Vis->Sky and
  // a ComponentFTMachine for the component lists
  // Default make use of MODEL_DATA Column of ms 
  SkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft, FTMachine& ift,
	      ComponentFTMachine& cft);


  // Return the SkyModel used by this SkyEquation
  SkyModel& skyModel() {return *sm_;};

  // Return the VisSet used by this SkyEquation
  VisSet& visSet() {return *vs_;};

  // Return the (Sky->Vis) FTMachine used by this SkyEquation
  FTMachine& fTMachine() {return *ft_;};
  
  // Return the (Vis->Sky) FTMachine used by this SkyEquation
  FTMachine& iFTMachine() {return *ift_;};
  
  // Return the (Sky->Vis) ComponentFTMachine used by this SkyEquation
  ComponentFTMachine& componentFTMachine() {return *cft_;};
  
  // Destructor
  virtual ~SkyEquation();

  // Assignment operator
  SkyEquation& operator=(const SkyEquation& other);

  // Copy constructor
  SkyEquation(const SkyEquation& other);

  // Add Jones matrices to be used for Gain calculations. Each SkyJones
  // knows what type it is and therefore where to slot in.
  void setSkyJones(SkyJones& j);

  // Make an approximate PSF for each model. The PSF is approximate
  // in the sense that it is a shift-invariant approximation
  virtual void makeApproxPSF(casacore::Int model, casacore::ImageInterface<casacore::Float>& PSF);

  // make all the approx psfs in one go
  virtual void makeApproxPSF(casacore::PtrBlock<casacore::ImageInterface<casacore::Float> *>& PSFs);

  // Make complex XFRs needed for incrementGradientChiSquared
  virtual void makeComplexXFRs();

  // Predict model coherence for the SkyModel. If this is
  // incremental then the model visibilities are not reset
  // but are simply added to
  //virtual void predict(casacore::Bool incremental=false);
  virtual void predict(casacore::Bool incremental=false, casacore::MS::PredefinedColumns Type=casacore::MS::MODEL_DATA);

  // Find sum of weights, Chi-squared, and the first and second derivatives
  // by transforming to the measurements. 
  // <group>
  virtual void gradientsChiSquared(const casacore::Matrix<casacore::Bool>& required, SkyJones& sj);
  virtual void gradientsChiSquared(casacore::Bool incremental, casacore::Bool commitToMS=false);
  // </group>

  // Solve for variables. Both the SkyModel and the SkyJones can in
  // principle be solved for.
  // <group>
  virtual casacore::Bool solveSkyModel();
  virtual casacore::Bool solveSkyJones(SkyJones& sj);
  // </group>

  // Set image plane weighting
  void setImagePlaneWeighting(const casacore::String& type, const casacore::Float minPB, 
			      const casacore::Float constPB)
    {scaleType_p = type; minPB_p = minPB; constPB_p = constPB;}

  // Lock and unlock the underlying MeasurementSet
  virtual void lock();
  virtual void unlock();
  
  // Return the name of the underlying MeasurementSet
  virtual casacore::String associatedMSName();


  //assign  the flux scale that the ftmachines have if they have
  virtual void getCoverageImage(casacore::Int model, casacore::ImageInterface<casacore::Float>& im);
  //Set this to true if the residual image for mosaic is to be in
  //pb^2 units (optimum mode for clean search for centimetric imaging)
  virtual void doFlatNoise(casacore::Bool doFlat=false){doflat_p=doFlat;};
  
  //get the weight image from the ftmachines
  virtual void getWeightImage(const casacore::Int, casacore::ImageInterface<casacore::Float>&){};

  virtual casacore::Bool isNewFTM(){return false;}

  virtual void setPhaseCenterTime(const casacore::Double time);
  virtual casacore::Double getPhaseCenterTime();
 protected:

  // Increment gradientsChiSquared. The image of SkyModel must contain
  // the increment to the image. For each model, a collection of
  // complex transfer functions are used to avoid gridding and
  // degridding all the visibilities.
  virtual void incrementGradientsChiSquared();

  // Do the full calculation - this must be called if the FTMachine 
  // cannot be represented by a Fourier transform

  virtual void fullGradientsChiSquared(casacore::Bool incremental=false);

  SkyEquation() {};
  
  // Check for validity
  casacore::Bool ok();
  
  // Apply Sky Jones to an image, also adjoint operation
  // <group>
  virtual void initializeGet(const VisBuffer& vb, casacore::Int row, casacore::Int model,
			     casacore::Bool incremental);
  

  virtual VisBuffer& get(VisBuffer& vb, casacore::Int model, casacore::Bool incremental, casacore::MS::PredefinedColumns Type=casacore::MS::MODEL_DATA);
  virtual void finalizeGet();
  virtual void initializePut(const VisBuffer &vb, casacore::Int model);
  
  virtual void put(const VisBuffer& vb, casacore::Int model, casacore::Bool dopsf=false, FTMachine::Type col=FTMachine::OBSERVED);
  
  virtual void finalizePut(const VisBuffer& vb, casacore::Int Model);
 
  // This encapsulates all of the change logic we should have to deal
  // with (short of returning a range of rows that has the same
  // SkyJones).  First we look to see if the first row of the VB
  // requires a new SkyJones; then we determine if there are
  // internal SkyJones changes in the VB which require going
  // row by row in the get/put formalism.
  virtual void changedSkyJonesLogic(const VisBuffer& vb, 
				    casacore::Bool& firstOneChanges,
				    casacore::Bool& internalChanges);

  // Have the SkyJones changed since their last application?
  virtual casacore::Bool changedSkyJones(const VisBuffer& vb, casacore::Int row);

  // Has the FTMachine changed since  last application?
  // <group>
  virtual casacore::Bool changedFTMachine(const VisBuffer& vb);
  virtual casacore::Bool changedIFTMachine(const VisBuffer& vb);
  // </group>

  // Do the Sky Jones change in this Visbuffer, starting from row1?
  // Returns row2, the last row with the same skyJones
  virtual casacore::Bool changedSkyJonesBuffer(const VisBuffer& vb, casacore::Int row1, casacore::Int& row2);

  virtual void resetSkyJones();
  virtual void assertSkyJones(const VisBuffer& vb, casacore::Int row);
  virtual casacore::ImageInterface<casacore::Complex>& applySkyJones(const VisBuffer& vb, casacore::Int row,
						 casacore::ImageInterface<casacore::Float>& in,
						 casacore::ImageInterface<casacore::Complex>& out);
  virtual void applySkyJonesInv(const VisBuffer& vb, casacore::Int row,
				casacore::ImageInterface<casacore::Complex>& in,
				casacore::ImageInterface<casacore::Float>& work,
				casacore::ImageInterface<casacore::Float>& out);
  virtual void applySkyJonesSquare(const VisBuffer& vb, casacore::Int row,
                                   casacore::Matrix<casacore::Float>& weights, 
                                   casacore::ImageInterface<casacore::Float>& work,
                                   casacore::ImageInterface<casacore::Float>& ggS);
  // </group>

  // Puts for calculating the complex XFRs
  // <group>
  virtual void initializePutXFR(const VisBuffer &vb, casacore::Int model, casacore::Int numXFR);
  virtual void putXFR(const VisBuffer& vb, casacore::Int model, casacore::Int& numXFR);
  virtual void finalizePutXFR(const VisBuffer& vb, casacore::Int Model, casacore::Int numXFR);
  // </group>

  // Puts for calculating the complex convolutions
  // <group>
  virtual void initializePutConvolve(const VisBuffer &vb, casacore::Int model,
				     casacore::Int numXFR);
  virtual void putConvolve(const VisBuffer& vb, casacore::Int model, casacore::Int& numXFR);
  virtual void finalizePutConvolve(const VisBuffer& vb, casacore::Int Model, casacore::Int numXFR);
  // </group>

  // Get, etc. for a SkyComponent is much simpler
  // <group>
  virtual VisBuffer& get(VisBuffer& vb, const SkyComponent& component);
  virtual VisBuffer& get(VisBuffer& vb, const ComponentList& components);
  // Do the sum of the gets for all the models for this visbuffer
  
  SkyComponent& applySkyJones(SkyComponent& corruptedComponent,
			      const VisBuffer& vb,
			      casacore::Int row);
  // </group>

  // Modify the ggS and Create the imageScale
  virtual void fixImageScale();

  // Deal with scaling or unscaling image or deltaImage
  // <group>
  virtual void scaleImage(casacore::Int model, casacore::Bool incremental);
  virtual void unScaleImage(casacore::Int model, casacore::Bool incremental);
  virtual void scaleImage(casacore::Int model=0);
  virtual void unScaleImage(casacore::Int model=0);
  virtual void scaleDeltaImage(casacore::Int model=0);
  virtual void unScaleDeltaImage(casacore::Int model=0);
  // </group>

  // Check the VisIter chunck size...force a more reasonable chunk 
  // if default is  too small 

  virtual void checkVisIterNumRows(ROVisibilityIterator& vi);

  //virtual void predictComponents(casacore::Bool& incremental, casacore::Bool& initialized);
  virtual void predictComponents(casacore::Bool& incremental, casacore::Bool& initialized,  casacore::MS::PredefinedColumns Type=casacore::MS::MODEL_DATA);

  
  // SkyModel
  SkyModel* sm_;

  // VisSet
  VisSet* vs_;
  //Visibilityiterators
  VisibilityIterator* wvi_p;
  ROVisibilityIterator* rvi_p;


  // FTMachine objects
  // <group>
  FTMachine* ft_;
  FTMachine* ift_;
  ComponentFTMachine* cft_;
  // </group>

  // casacore::List of terms in left to right order
  // <group>
  SkyJones* ej_;
  SkyJones* dj_;
  SkyJones* tj_;
  SkyJones* fj_;
  // </group>

  // Workspace
  // <group>
  casacore::Float chisq, sumwt;
  casacore::Float ggSMax_p;
  // </group>

  casacore::LogSink logSink_p;
  casacore::LogSink& logSink() {return logSink_p;};

  casacore::Int iDebug_p;
  
  // Previous VisBuffer, used to determine how to apply
  // SkyJones;  
  // Set in initializePut and initializePutXFR,
  // Used in finalizePut and finalizePutXFR

  mutable VisBufferAutoPtr vb_p;

  casacore::Float minPB_p;   // ignore model flux below this level in the generalized PB
  casacore::Float constPB_p; // make the fluxscale constant for PB above this level
  casacore::String scaleType_p;  // types:  NONE, or SAULT

  casacore::Bool isPSFWork_p; // working for PSF estimation

  casacore::Bool noModelCol_p;

  casacore::Vector<casacore::Bool> modelIsEmpty_p;

  //SkyJones::changed returns a true the first time its called
  //We have to ignore this at the very begining and first call to 'changed'
  //and not call finalizePut
  casacore::Bool isBeginingOfSkyJonesCache_p;
  casacore::Bool doflat_p;
};

} //# NAMESPACE CASA - END

#endif





