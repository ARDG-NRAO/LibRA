// -*- C++ -*-
//# AWProjectWBFT.cc: Implementation of AWProjectWBFT class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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

#include <synthesis/TransformMachines2/AWProjectWBFTNew.h>

#include <synthesis/TransformMachines2/AWVisResampler.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <casacore/scimath/Mathematics/Convolver.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/OS/HostInfo.h>
#include <sstream>
#include <casacore/casa/Utilities/CompositeNumber.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{
  //---------------------------------------------------------------
  // 

//-----------------------------------------------------------------------
  FTMachine* AWProjectWBFTNew::cloneFTM(){
    return new AWProjectWBFTNew(*this);
  }


  void AWProjectWBFTNew::ftWeightImage(Lattice<Complex>& wtImage, 
				    const Matrix<Float>& sumWt,
				    const Bool& doFFTNorm)
  {
    LogIO log_l(LogOrigin("AWProjectWBFTNew2", "ftWeightImage[R&D]"));
    if (wtImageFTDone_p) return;

    // Bool doSumWtNorm=true;
    // if (sumWt.shape().nelements()==0) doSumWtNorm=false;

    if ((sumWt.shape().nelements() < 2) || 
	(sumWt.shape()(0) != wtImage.shape()(2)) || 
	(sumWt.shape()(1) != wtImage.shape()(3)))
      log_l << "Sum of weights per poln and chan is required" << LogIO::EXCEPTION;
    //
    // Use only the amplitude of the gridded weights.  LatticeExpr
    // classes while useful, appear to be too strict about types (the
    // code below will not compile if the abs(wtImage) is not
    // converted back to a complex type).

    // LatticeExpr<Complex> le(abs(wtImage)*Complex(1,0));
    // wtImage.copyData(le);

    // {
    //   String name("wtimg.im");
    //   storeArrayAsImage(name,image->coordinates(),wtImage.get());
    // }
    LatticeFFT::cfft2d(wtImage,false);
    // {
    //   String name("ftwtimg.im");
    //   storeArrayAsImage(name,image->coordinates(),wtImage.get());
    // }
    wtImageFTDone_p=true;

    Int sizeX=wtImage.shape()(0), sizeY=wtImage.shape()(1);

    Array<Complex> wtBuf; wtImage.get(wtBuf,false);
    ArrayLattice<Complex> wtLat(wtBuf,true);
    //
    // Copy one 2D plane at a time, normalizing by the sum of weights
    // and possibly 2D FFT.
    //
    // Set up Lattice iterators on wtImage and sensitivityImage
    //
    IPosition axisPath(4, 0, 1, 2, 3);
    IPosition cursorShape(4, sizeX, sizeY, 1, 1);

    LatticeStepper wtImStepper(wtImage.shape(), cursorShape, axisPath);
    LatticeIterator<Complex> wtImIter(wtImage, wtImStepper);
    //
    // Iterate over channel and polarization axis
    //
    if (!doFFTNorm) sizeX=sizeY=1;
    //
    // Normalize each frequency and polarization plane of the complex
    // sensitivity pattern
    //
    // getSumOfCFWeights() returns the sum-of-weight appropriate for
    // computing sensitivity patterns (which could be different from
    // the data-sum-of-weights.
    //

    // USEFUL DEBUG MESSAGE
    //cerr << "SumCFWt: " << getSumOfCFWeights() << " " << max(wtBuf) << " " << sensitivityPatternQualifier_p << endl;
    for(wtImIter.reset(); !wtImIter.atEnd(); wtImIter++)
      {
	// Int pol_l=wtImIter.position()(2), chan_l=wtImIter.position()(3);
	// Lets write some mildly obfuscated code ~[8-)
	//if ((sensitivityPatternQualifier_p == -1) && (doSumWtNorm))
	//  sumwt_l = ((sumwt_l = getSumOfCFWeights()(pol_l,chan_l))==0)?1.0:sumwt_l;

	//sumwt_l = getSumOfCFWeights()(pol_l,chan_l);

	wtImIter.rwCursor() = (wtImIter.rwCursor()
			       *Float(sizeX)*Float(sizeY)
			       //U			       /sumwt_l
			       );

	////////////////////	wtImIter.rwCursor() = sqrt( fabs(wtImIter.rwCursor()) );

	//Double maxval = fabs( max( wtImIter.rwCursor() ) );
	//cout << "sumwt from WBAWPNew::ftWeightImage : " << sumwt_l << "  max val in wtimg : " << maxval << endl;



	//	sumwt_l = getSumOfCFWeights()(pol_l,chan_l);
	//	weightRatio_p = maxval * Float(sizeX)*Float(sizeY) / sumwt_l;
      }

  }

 
} //# NAMESPACE CASA - END
}
