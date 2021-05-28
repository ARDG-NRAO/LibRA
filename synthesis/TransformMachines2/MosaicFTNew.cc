//# Mosaicft.cc: Implementation of MosaicFTNew class
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

#include <synthesis/TransformMachines2/MosaicFTNew.h>


#include <msvis/MSVis/VisibilityIterator.h>
#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/UnitVal.h>
#include <measures/Measures/Stokes.h>
#include <measures/Measures/UVWMachine.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/BasicSL/Constants.h>
#include <scimath/Mathematics/FFTServer.h>

#include <synthesis/TransformMachines/StokesImageUtil.h>

#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <images/Regions/WCBox.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Slice.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MatrixIter.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/LatticeCache.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <casa/Utilities/CompositeNumber.h>
#include <casa/OS/Timer.h>
#include <casa/OS/HostInfo.h>
#include <casa/sstream.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
namespace refim {//# namespace for imaging refactor
using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::refim;

  refim::FTMachine* MosaicFTNew::cloneFTM(){
    return new MosaicFTNew(*this);
  }

  MosaicFTNew::MosaicFTNew(const RecordInterface& stateRec)
  : MosaicFT(stateRec)
{
  
}
// Finalize the FFT to the Sky. Here we actually do the FFT and
// return the resulting image
ImageInterface<Complex>& MosaicFTNew::getImage(Matrix<Float>& weights,
					    Bool normalize) 
{
  //AlwaysAssert(lattice, AipsError);
  AlwaysAssert(image, AipsError);

   if((griddedData.nelements() ==0) && (griddedData2.nelements()==0)){
    throw(AipsError("Programmer error ...request for image without right order of calls"));
  }

  logIO() << LogOrigin("MosaicFTNew", "getImage") << LogIO::NORMAL;
  
  weights.resize(sumWeight.shape());
  
  convertArray(weights, sumWeight);
  
  // If the weights are all zero then we cannot normalize
  // otherwise we don't care.
  if(max(weights)==0.0) {
    if(normalize) {
      logIO() << LogIO::SEVERE << "No useful data in MosaicFTNew: weights all zero"
	      << LogIO::POST;
    }
    else {
      logIO() << LogIO::WARN << "No useful data in MosaicFTNew: weights all zero"
	      << LogIO::POST;
      image->set(0.0);
    }
  }
  else {
    
    //const IPosition latticeShape = lattice->shape();
    
    logIO() << LogIO::DEBUGGING
	    << "Starting FFT and scaling of image" << LogIO::POST;
    if(useDoubleGrid_p){
      ArrayLattice<DComplex> darrayLattice(griddedData2);
      ft_p.c2cFFT(darrayLattice,false);
      griddedData.resize(griddedData2.shape());
      convertArray(griddedData, griddedData2);
      
      //Don't need the double-prec grid anymore...
      griddedData2.resize();
      arrayLattice = new ArrayLattice<Complex>(griddedData);
      lattice=arrayLattice;

    }
    else{
      arrayLattice = new ArrayLattice<Complex>(griddedData);
      lattice=arrayLattice;
      ft_p.c2cFFT(*lattice,false);
    }
    
    {
      Int inx = lattice->shape()(0);
      Int iny = lattice->shape()(1);
      Vector<Complex> correction(inx);
      correction=Complex(1.0, 0.0);
       Vector<Float> sincConvX(nx);
       for (Int ix=0;ix<nx;ix++) {
	 Float x=C::pi*Float(ix-nx/2)/(Float(nx)*Float(convSampling));
	 if(ix==nx/2) {
	   sincConvX(ix)=1.0;
	 }
	 else {
	   sincConvX(ix)=sin(x)/x;
	 }
       }
       Vector<Float> sincConvY(ny);
      for (Int ix=0;ix<ny;ix++) {
	Float x=C::pi*Float(ix-ny/2)/(Float(ny)*Float(convSampling));
	if(ix==ny/2) {
	  sincConvY(ix)=1.0;
	}
	else {
	  sincConvY(ix)=sin(x)/x;
	}
      }


      IPosition cursorShape(4, inx, 1, 1, 1);
      IPosition axisPath(4, 0, 1, 2, 3);
      LatticeStepper lsx(lattice->shape(), cursorShape, axisPath);
      LatticeIterator<Complex> lix(*lattice, lsx);
      for(lix.reset();!lix.atEnd();lix++) 
	{
	Int pol=lix.position()(2);
	Int chan=lix.position()(3);
	Int iy=lix.position()(1);
	for (Int ix=0;ix<nx;ix++) {
	    correction(ix)=1.0/(sincConvX(ix)*sincConvY(iy));
	}
	if(normalize) 
	  {
	    if(weights(pol, chan)!=0.0) 
	      {
		Complex rnorm(Float(inx)*Float(iny)/weights(pol,chan));
		lix.rwVectorCursor()*=rnorm*correction;
	      }
	    else {
	      lix.woCursor()=0.0;
	    }
	}	  else {
	  Complex rnorm(Float(inx)*Float(iny));
	  lix.rwVectorCursor()*=rnorm*correction;
	}
      }
    }

    //if(!isTiled) 
    {
      LatticeLocker lock1 (*(image), FileLocker::Write);
      // Check the section from the image BEFORE converting to a lattice 
      IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		    (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
      IPosition stride(4, 1);
      IPosition trc(blc+image->shape()-stride);
      
      // Do the copy
      IPosition start(4, 0);
      image->put(griddedData(blc, trc));
    }
  }
  if(!arrayLattice.null()) arrayLattice=0;
  if(!lattice.null()) lattice=0;
  griddedData.resize();
  image->clearCache();
  return *image;
}

// Get weight image
void MosaicFTNew::getWeightImage(ImageInterface<Float>& weightImage,
			      Matrix<Float>& weights) 
{
  
  logIO() << LogOrigin("MosaicFTNew", "getWeightImage") << LogIO::NORMAL;
  
  weights.resize(sumWeight.shape());
  convertArray(weights,sumWeight);
  Record rec=skyCoverage_p->miscInfo();
  Float inx=1;
  Float iny=1;
  Bool isscaled=rec.isDefined("isscaled") && rec.asBool("isscaled");
  //cerr << "isscaled " << isscaled << " max " << max(skyCoverage_p->get()) << endl;
  if( !isscaled){
    inx = skyCoverage_p->shape()(0);
    iny = skyCoverage_p->shape()(1);
  }
  //cerr << "inx, iny " << inx << "   " << iny << endl;
  weightImage.copyData( (LatticeExpr<Float>) ( (*skyCoverage_p)*inx*iny ) );
 
   skyCoverage_p->tempClose();

}


} // REFIM ends
} //# NAMESPACE CASA - END
