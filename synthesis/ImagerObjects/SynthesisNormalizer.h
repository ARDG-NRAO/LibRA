//# SynthesisNormalizer.h: Gather and Scatter operations for parallel major cycles.
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#
//# $Id$

#ifndef SYNTHESIS_SYNTHESISPARSYNC_H
#define SYNTHESIS_SYNTHESISPARSYNC_H

#include <casa/aips.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>

#include<synthesis/ImagerObjects/SDAlgorithmBase.h>
#include<synthesis/ImagerObjects/SDAlgorithmHogbomClean.h>
#include<synthesis/ImagerObjects/SDMaskHandler.h>
#include <synthesis/ImagerObjects/SIMinorCycleController.h>

#include <synthesis/ImagerObjects/SIImageStore.h>
#include <synthesis/ImagerObjects/SIImageStoreMultiTerm.h>

namespace casacore{

class MeasurementSet;
template<class T> class ImageInterface;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations

// <summary> Class that contains functions needed for imager </summary>

class SynthesisNormalizer 
{
 public:
  // Default constructor

  SynthesisNormalizer();
  ~SynthesisNormalizer();

  // Copy constructor and assignment operator

  // make all pure-inputs const
  void setupNormalizer(casacore::Record normpars);

  // Gather all part images to the 'full' one
  void gatherImages(casacore::Bool dopsf, casacore::Bool doresidual, casacore::Bool dodensity);

  // 'Gather' the pb ( just one node makes it.. )
  void gatherPB();

  // Copy out model to all pieces. Currently a No-Op.
  void scatterModel();

  // Gather all part gridded weights and add them up.
  void gatherWeightDensity();
  // Scatter summed gridded weights to all parts
  void scatterWeightDensity();

  std::shared_ptr<SIImageStore> getImageStore();
  void setImageStore( SIImageStore* imstore );
  void setImageStore( std::shared_ptr<SIImageStore>& imstore );

  void divideResidualByWeight();
  void dividePSFByWeight();
  void divideModelByWeight();
  void multiplyModelByWeight();

  void normalizePrimaryBeam();

protected:

 // Normalize. This can later change to be more general, i.e. used for PB-correction too...
  // Check if images exist on disk and are all the same shape
  casacore::Bool setupImagesOnDisk();
  casacore::Bool doImagesExist( casacore::String imagename );

  std::shared_ptr<SIImageStore> makeImageStore( const casacore::String &imagename );
  std::shared_ptr<SIImageStore> makeImageStore( const casacore::String &imagename,
                                           const casacore::PagedImage<casacore::Float> &part,
                                           casacore::Bool useweightimage );

  void setPsfFromOneFacet();

  /////////////// Member Objects

  std::shared_ptr<SIImageStore> itsImages;
  casacore::Vector<std::shared_ptr<SIImageStore> > itsPartImages;
  casacore::Block<std::shared_ptr<SIImageStore> > itsFacetImageStores;

  casacore::IPosition itsImageShape;
  
  casacore::String itsImageName;
  casacore::Vector<casacore::String> itsPartImageNames;
  casacore::String itsStartingModelName;
  casacore::Float itsPBLimit;

  casacore::String itsMapperType;
  casacore::uInt itsNTaylorTerms, itsNFacets;

  casacore::String itsNormType;

  casacore::String itsUseBeam;
    
  casacore::Float itsPsfcutoff;

};


} //# NAMESPACE CASA - END

#endif
