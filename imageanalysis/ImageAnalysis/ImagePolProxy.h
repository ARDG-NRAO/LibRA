//# ImagePolProxy.h: a casa namespace class for imagepol tool
//# Copyright (C) 2007
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
//# $Id: ImagePolProxy.h 20299 2008-04-03 05:56:44Z gervandiepen $

#ifndef IMAGES_IMAGEPOL_H
#define IMAGES_IMAGEPOL_H

#include <imageanalysis/ImageAnalysis/ImagePolarimetry.h>

namespace casacore{

class LogIO;
class String;
class Record;
class Normal;
template<class T> class ImageInterface;
}

namespace casa {

//# Forward declarations

// Implementation of the image polarimetry functionality
// available from casapy.

class ImagePol
{

 public:
  // "imagepol" constructors 
  ImagePol();
  ImagePol(casacore::ImageInterface<casacore::Float>& im);
  virtual ~ImagePol();

  // Make test image
  casacore::Bool imagepoltestimage(const casacore::String& outFile = "imagepol.iquv",
			 const casacore::Vector<casacore::Double>& rm = casacore::Vector<casacore::Double>(1, 0.0),
			 casacore::Bool rmDefault = true,
			 casacore::Double pa0 = 0.0, casacore::Double sigma = 0.01,
			 casacore::Int nx = 32, casacore::Int ny = 32, casacore::Int nf = 32,
			 casacore::Double f0 = 1.4e9, casacore::Double df = 128.0e6);

  casacore::Bool open(casacore::ImageInterface<casacore::Float>& im);
  casacore::Bool open(const casacore::String& infile);

  // Depolarization ratio
  // The image containing the delpolratio is in the returnrec 
  // Can be recovered using casacore::ImageInterface::fromRecord
  casacore::Bool depolratio(casacore::ImageInterface<casacore::Float>*& rtnim, const casacore::String& infile, 
		  casacore::Bool debias = false,
		  casacore::Double clip = 10.0, casacore::Double sigma = -1, 
		  const casacore::String& oufile="");  

  //casacore::Complex linear polarization image is stored in outfile
  casacore::Bool complexlinpol(const casacore::String& outfile);


  // Summary
  void summary() const;

  // sigma
  casacore::Float sigma(casacore::Float clip = 10.0) const;

  // casacore::Stokes I
  casacore::Bool stokesI(casacore::ImageInterface<casacore::Float>*& rtnim, const casacore::String& outfile="");
  casacore::Float sigmaStokesI(casacore::Float clip = 10.0) const;

  // casacore::Stokes Q
  casacore::Bool stokesQ(casacore::ImageInterface<casacore::Float>*& rtnim, const casacore::String& outfile="");
  casacore::Float sigmaStokesQ(casacore::Float clip = 10.0) const;

  // casacore::Stokes U
  casacore::Bool stokesU(casacore::ImageInterface<casacore::Float>*& rtnim, const casacore::String& outfile="");
  casacore::Float sigmaStokesU(casacore::Float clip = 10.0) const;

  // casacore::Stokes V
  casacore::Bool stokesV(casacore::ImageInterface<casacore::Float>*& rtnim, const casacore::String& outfile="");
  casacore::Float sigmaStokesV(casacore::Float clip = 10.0) const;
  casacore::Float sigmaLinPolInt(casacore::Float clip = 10.0, casacore::Float sigma = -1) const;

  // Total polarized intensity.
  casacore::Float sigmaTotPolInt(casacore::Float clip = 10.0, casacore::Float sigma = -1) const;

  // casacore::Complex linear polarization
  void complexLinearPolarization (const casacore::String& outfile);

  // casacore::Complex linear polarization
  void complexFractionalLinearPolarization (const casacore::String& outfile);

  // Linearly polarized position angle
  casacore::Bool sigmaLinPolPosAng(casacore::ImageInterface<casacore::Float>*& rtnim, casacore::Float clip = 10.0,
			 casacore::Float sigma = -1, const casacore::String& outfile = "");

  // Fractional linearly polarized intensity
  casacore::Bool fracLinPol(casacore::ImageInterface<casacore::Float>*& rtnim, casacore::Bool debias = false,
		  casacore::Float clip = 10.0,
		  casacore::Float sigma = -1, const casacore::String& outfile = "");
  casacore::Bool sigmaFracLinPol(casacore::ImageInterface<casacore::Float>*& rtnim, casacore::Float clip = 10.0,
		       casacore::Float sigma = -1, const casacore::String& outfile = "");

  // Fractional total polarized intensity
  casacore::Bool fracTotPol(casacore::ImageInterface<casacore::Float>*& rtnim, casacore::Bool debias = false,
		  casacore::Float clip = 10.0,
		  casacore::Float sigma = -1, const casacore::String& outfile = "");
  casacore::Bool sigmaFracTotPol(casacore::ImageInterface<casacore::Float>*& rtnim, casacore::Float clip = 10.0,
		       casacore::Float sigma = -1, const casacore::String& outfile = "");

  // Depolarization ratio
  casacore::Bool depolarizationRatio (casacore::ImageInterface<casacore::Float>*& rtnim, 
			    const casacore::String& infile,
			    casacore::Bool debias = false, casacore::Float clip = 10.0,
			    casacore::Float sigma = -1, const casacore::String& outfile = "");
  casacore::Bool sigmaDepolarizationRatio (casacore::ImageInterface<casacore::Float>*& rtnim,
				 const casacore::String& infile,
				 casacore::Bool debias = false, casacore::Float clip = 10.0,
				 casacore::Float sigma = -1, const casacore::String& outfile = "");

  // Find Rotation casacore::Measure from Fourier method
  void fourierRotationMeasure(const casacore::String& outfile = "",
			      const casacore::String& outfileAmp = "",
			      const casacore::String& outfilePA = "",
			      const casacore::String& outfileReal = "",
			      const casacore::String& outfileImag = "",
			      casacore::Bool zeroZeroLag = false);

  // Find Rotation casacore::Measure from traditional method
  void rotationMeasure(const casacore::String& outRM = "", const casacore::String& outRMErr = "",
		       const casacore::String& outPA0 = "", const casacore::String& outPA0Err = "",
		       const casacore::String& outNTurns = "",
		       const casacore::String& outChiSq = "",
		       casacore::Int axis = -1, casacore::Float varQU = -1, casacore::Float rmFg = 0.0,
		       casacore::Float rmMax = 0.0, casacore::Float maxPaErr = 1e30/*,
		       const casacore::String& plotter = "",
		       casacore::Int nx = 5, casacore::Int ny = 5*/);

  // Make a complex image
  void makeComplex (const casacore::String& complex, const casacore::String& real = "",
		    const casacore::String& imag = "", const casacore::String& amp = "",
		    const casacore::String& phase = "");

  SPCIIF getImage() const { return itsImPol->imageInterface(); }

 private:
  casacore::LogIO *itsLog;
  ImagePolarimetry *itsImPol;
  
  casacore::Bool copyImage(casacore::ImageInterface<casacore::Float>*& out, const casacore::ImageInterface<casacore::Float>&in, 
	    const casacore::String& outfile="", casacore::Bool overwrite=true);
  // Copy miscellaneous (MiscInfo, casacore::ImageInfo, history, units)
  void copyMiscellaneous (casacore::ImageInterface<casacore::Complex>& out,
			  const casacore::ImageInterface<casacore::Float>& in);
  void copyMiscellaneous (casacore::ImageInterface<casacore::Float>& out,
			 const casacore::ImageInterface<casacore::Float>& in);
  void fiddleStokesCoordinate(casacore::ImageInterface<casacore::Float>& ie,
			      casacore::Stokes::StokesTypes type);
  void fiddleStokesCoordinate(casacore::ImageInterface<casacore::Complex>& ie,
			      casacore::Stokes::StokesTypes type);
  // Make a casacore::PagedImage or casacore::TempImage output
bool _makeImage (
    casacore::ImageInterface<casacore::Complex>*& out, 
    const casacore::String& outfile, const casacore::CoordinateSystem& cSys,
    const casacore::IPosition& shape, bool isMasked=false,
    bool tempAllowed=true
);

std::shared_ptr<casacore::ImageInterface<casacore::Complex>> _makeImage ( 
    const casacore::String& outfile, const casacore::CoordinateSystem& cSys,
    const casacore::IPosition& shape, bool isMasked=false,
    bool tempAllowed=true
);

  casacore::Bool makeImage (casacore::ImageInterface<casacore::Float>*& out, 
		  const casacore::String& outfile, const casacore::CoordinateSystem& cSys,
		  const casacore::IPosition& shape, casacore::Bool isMasked=false,
		  casacore::Bool tempAllowed=true);
  // Make an IQUV image with some dummy RM data
  casacore::Bool  makeIQUVImage (casacore::ImageInterface<casacore::Float>*& pImOut, const casacore::String& outfile, 
		       casacore::Double sigma, 
                       casacore::Double pa0, const casacore::Vector<casacore::Float>& rm, 
		       const casacore::IPosition& shape,
                       casacore::Double f0, casacore::Double dF);
  // Fill IQUV image with casacore::Stokes values from RM data
  casacore::Bool fillIQUV (casacore::ImageInterface<casacore::Float>& im, casacore::uInt stokesAxis,
		 casacore::uInt spectralAxis, const casacore::Vector<casacore::Float>& rm, 
		 casacore::Float pa0);
  // Add noise to Array
  void addNoise (casacore::Array<casacore::Float>& slice, casacore::Normal& noiseGen);
  // Centre reference pixelin image
  void centreRefPix (casacore::CoordinateSystem& cSys, const casacore::IPosition& shape);
  // Make and define a mask
  casacore::Bool makeMask(casacore::ImageInterface<casacore::Float>& out, casacore::Bool init=false);
  casacore::Bool makeMask(casacore::ImageInterface<casacore::Complex>& out, casacore::Bool init=false);
  // What casacore::Stokes type?  Exception if more than one.
  casacore::Stokes::StokesTypes stokesType(const casacore::CoordinateSystem& cSys);


};

} // casa namespace

#endif
