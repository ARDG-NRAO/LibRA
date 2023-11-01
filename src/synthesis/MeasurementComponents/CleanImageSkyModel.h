//# CleanImageSkyModel.h: Definition for CleanImageSkyModel
//# Copyright (C) 1996,1997,1998,2000
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

#ifndef SYNTHESIS_CLEANIMAGESKYMODEL_H
#define SYNTHESIS_CLEANIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/ImageSkyModel.h>

namespace casacore{

template <class T> class PagedImage;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// Image Sky Model: Image-based Model for the Sky Brightness
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=SkyModel>SkyModel</linkto> class
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> class
//   <li> <linkto class=casacore::ImageInterface>ImageInterface</linkto> class
//   <li> <linkto class=casacore::PagedImage>PagedImage</linkto> class
//   <li> <linkto module=MeasurementComponents>MeasurementComponents</linkto> module
//   <li> <linkto class=VisSet>VisSet</linkto> class
// </prerequisite>
//
// <etymology>
// CleanImageSkyModel describes an interface for Models to be used in
// the SkyEquation. It is derived from <linkto class=SkyModel>SkyModel</linkto>.
// </etymology>
//
// <synopsis> 
// A CleanImageSkyModel contains a number of separate models. The interface to
// SkyEquation is via an image per model. <linkto class=SkyEquation>SkyEquation</linkto> uses this image to
// calculate Fourier transforms, etc. Some (most) SkyModels are
// solvable: the SkyEquation can be used by the SkyModel to return
// gradients with respect to itself (via the image interface). Thus
// for a SkyModel to solve for itself, it calls the SkyEquation
// methods to get gradients of chi-squared with respect to the
// image pixel values (thus returning an image: basically a residual
// image). The SkyModel then uses these gradients as appropriate to
// update itself.
// </synopsis> 
//
// <example>
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// The properties of a model of the sky must be described
// for the <linkto class=SkyEquation>SkyEquation</linkto>.
// </motivation>
//
// <todo asof="97/10/01">
// <li> Multiple images in SkyModel
// <li> ComponentModel
// </todo>


class CleanImageSkyModel : public ImageSkyModel {
public:

  // Empty constructor
  CleanImageSkyModel();

  // Copy constructor
  CleanImageSkyModel(const CleanImageSkyModel& sm);

  // Has a mask for model?
  casacore::Bool hasMask(casacore::Int model=0);

  // Add a componentlist
  virtual casacore::Bool add(ComponentList& compList);

  // Add an image. maxNumXfr is the maximum Number of transfer functions
  // that we might want to associate with this image.
  virtual casacore::Int add(casacore::ImageInterface<casacore::Float>& image, const casacore::Int maxNumXfr=100);
  
  // Add an mask  
  virtual casacore::Bool addMask(casacore::Int image, casacore::ImageInterface<casacore::Float>& mask);

  // Has a flux mask?
  casacore::Bool hasFluxMask(casacore::Int model=0);

  virtual casacore::Bool addFluxMask(casacore::Int image, casacore::ImageInterface<casacore::Float>& fluxMask);
  
  casacore::ImageInterface<casacore::Float>& fluxMask(casacore::Int model=0);

  // Destructor
  virtual ~CleanImageSkyModel();

  // Assignment operator
  CleanImageSkyModel& operator=(const CleanImageSkyModel& other);

  // Return masks
  // <group>
  casacore::ImageInterface<casacore::Float>& mask(casacore::Int model=0);
  // </group>
  //set to search for peak in I^2+Q^2+U^2+V^2 domain or each stokes plane seperately
  //Ignored for hogbom and msclean for now
  virtual void setJointStokesClean(casacore::Bool joint=true);


   // Return the maximum absolute value from residual images per field
  virtual casacore::Float maxField(casacore::Vector<casacore::Float>& imagemax, casacore::Vector<casacore::Float>& imagemin);


protected:

  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > mask_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > fluxmask_p;
  casacore::Bool doPolJoint_p;

};



} //# NAMESPACE CASA - END

#endif


