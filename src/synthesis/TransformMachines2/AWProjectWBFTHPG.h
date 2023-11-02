//# AWProjectWBFTHPG.h: Definition for AWProjectWBFTHPG
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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

#ifndef SYNTHESIS_TRANSFORM2_AWPROJECTWBFTHPG_H
#define SYNTHESIS_TRANSFORM2_AWPROJECTWBFTHPG_H

#include <synthesis/TransformMachines2/AWProjectWBFT.h>

namespace casa
{ //# NAMESPACE CASA - BEGIN
  namespace refim
  {
    class AWProjectWBFTHPG : public AWProjectWBFT
    {
    public:
      AWProjectWBFTHPG(casacore::Int nFacets, casacore::Long cachesize,
			  casacore::CountedPtr<CFCache>& cfcache,
			  casacore::CountedPtr<ConvolutionFunction>& cf,
			  casacore::CountedPtr<VisibilityResamplerBase>& visResampler,
			  casacore::Bool applyPointingOffset=true,
			  vector<float> pointingOffsetSigDev = {10,10},
			  casacore::Bool doPBCorr=true,
			  casacore::Int tilesize=16, 
			  casacore::Float paSteps=5.0, 
			  casacore::Float pbLimit=5e-4,
			  casacore::Bool usezero=false,
			  casacore::Bool conjBeams_p=true,
			  casacore::Bool doublePrecGrid=false):
	AWProjectWBFT(nFacets, cachesize, cfcache, cf, visResampler,
			 applyPointingOffset, pointingOffsetSigDev,
			 doPBCorr, tilesize, paSteps, pbLimit, usezero,
			 conjBeams_p, doublePrecGrid),applyFFT_p(False)
      {};
      
      ~AWProjectWBFTHPG(){};
      
      virtual casacore::String name() const {return "AWProjectWBFTHPG";};

      // Assignment operator
      AWProjectWBFTHPG &operator=(const AWProjectWBFTHPG &other)
      {
	if(this!=&other) 
	  {
	    //Do the base parameters
	    //	      AWProjectWBFT::operator=(other);
	    
	    applyFFT_p=other.applyFFT_p;
	  }
	return *this;
      };
      
      //---------------------------------------------------------------------------------------
      // Overloading getImage() to not do FFT here.  The appropriate
      // FFT is applied on the GPU.  Here, only conversion and copy
      // from DP to SP image is done.
      //
      virtual casacore::ImageInterface<casacore::Complex>&
      getImage(casacore::Matrix<casacore::Float>& weights,
	       casacore::Bool normalize=false)
      {
	LogIO log_l(LogOrigin("AWProjectWBFTHPG", "getImage[R&D]"));
	//
	AlwaysAssert(image, AipsError);
	
	weights.resize(sumWeight.shape());
	convertArray(weights, sumWeight);
	//  
	// If the weights are all zero then we cannot normalize otherwise
	// we don't care.
	//
	if(max(weights)==0.0) 
	  log_l 
	    << "No useful data in " << name() << ".  Weights all zero"
	    << LogIO::POST;
	else
	  {
	    log_l << "Sum of weights: " << weights << " "
		  << max(griddedData2) << " " << min(griddedData2)
		  << LogIO::POST;
	    cerr << "Sum of weights: " << setprecision(20) << weights
		 << endl;
	  }
	
	//    
	// x and y transforms (lattice has the gridded vis.  Make the
	// dirty images)
	//
	if (useDoubleGrid_p)
	  {
	    //ArrayLattice<DComplex> darrayLattice(griddedData2);
	    // {
	    //   griddedData.resize(griddedData2.shape());
	    //   convertArray(griddedData, griddedData2);
	    //   storeArrayAsImage(String("cgrid_"+visResampler_p->name()+".im"), image->coordinates(), griddedData);
	    // }
	    //LatticeFFT::cfft2d(darrayLattice,false);
	    
	    griddedData.resize(griddedData2.shape());
	    convertArray(griddedData, griddedData2);
	    SynthesisUtilMethods::getResource("mem peak in getImage");
	    
	    //Don't need the double-prec grid anymore...
	    griddedData2.resize();
	    arrayLattice = new ArrayLattice<Complex>(griddedData);
	    lattice=arrayLattice;
	  }
	else
	  {
	    lattice = new ArrayLattice<Complex>(griddedData);
	  }
	
	//
	// Now normalize the dirty image.
	//
	// Since *lattice is not copied to *image till the end of this
	// method, normalizeImage also needs to work with Lattices
	// (rather than ImageInterface).
	//
	// //normalizeImage(*lattice,sumWeight,*avgPB_p,fftNormalization);
	//	normalizeImage(*lattice,sumWeight,*avgPB_p, *avgPBSq_p, fftNormalization);
	
	//nx ny normalization from GridFT...
	{
	  Int inx = lattice->shape()(0);
	  Int iny = lattice->shape()(1);

	  Int npixCorr= max(inx,iny);
	  Vector<Float> sincConv(npixCorr);
	  Vector<Complex> correction(npixCorr);
	  for (Int ix=0;ix<npixCorr;ix++)
	    {
	      Float x=C::pi*Float(ix-npixCorr/2)/(Float(npixCorr)*Float(convSampling));
	      if(ix==npixCorr/2)  sincConv(ix)=1.0;
	      else                sincConv(ix)=sin(x)/x;
	    }


	  // Do the Grid-correction
	  IPosition cursorShape(4, inx, 1, 1, 1);
	  IPosition axisPath(4, 0, 1, 2, 3);
	  LatticeStepper lsx(lattice->shape(), cursorShape, axisPath);
	  LatticeIterator<Complex> lix(*lattice, lsx);

	  for(lix.reset();!lix.atEnd();lix++)
	    {
	      Int pol=lix.position()(2);
	      Int chan=lix.position()(3);
	      if(weights(pol, chan)!=0.0)
		{
		  Int iy=lix.position()(1);
		  for (Int ix=0;ix<nx;ix++) correction(ix)=sincConv(ix)*sincConv(iy);
		  lix.rwVectorCursor()/=correction;
		  if(normalize)
		    {
		      Complex rnorm(Float(inx)*Float(iny)/weights(pol,chan));
		      lix.rwCursor()*=rnorm;
		    }
		}
	      else
		{
		  lix.woCursor()=0.0;
		}
	    }




	  // for(lix.reset();!lix.atEnd();lix++) 
	  //   {
	  //     Int pol=lix.position()(2);
	  //     Int chan=lix.position()(3);
	  //     if (normalize)
	  // 	{
	  // 	  if(weights(pol,chan)!=0.0)
	  // 	    {
	  // 	      Complex rnorm(Float(inx)*Float(iny)/(sincConv(inx)*sincConv(iny)* weights(pol,chan) ));
	  // 	      lix.rwCursor()*=rnorm;
	  // 	    }
	  // 	  else
	  // 	    lix.woCursor()=0.0;
	  // 	}
	  //     else
	  // 	lix.rwCursor() /= sincConv(inx)*sincConv(iny);
	  //   }
	}
	if(!isTiled) 
	  {
	    //
	    // Check the section from the image BEFORE converting to a lattice 
	    //
	    IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
			  (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
	    IPosition stride(4, 1);
	    IPosition trc(blc+image->shape()-stride);
	    //
	    // Do the copy
	    //
	    image->put(griddedData(blc, trc));
	    
	    
	    if(!arrayLattice.null()) arrayLattice=0;
	    if(!lattice.null()) lattice=0;
	    griddedData.resize(IPosition(1,0));
	  }
	// {
	//   TempImage<Complex> tt(lattice->shape(), image->coordinates());
	//   tt.put(lattice->get());
	//   storeImg(String("uvgrid"+visResampler_p->name()+".im"), *image,true);
	// }
	
	return *image;
      }
      //
      //------------------------------------------------------------
      // version for use with HPG
      //
      virtual void initializeToVis(casacore::ImageInterface<casacore::Complex>& iimage,
				   const vi::VisBuffer2& vb)
      {
	//    initializeToVisDP(iimage, vb);
	LogIO log_l(LogOrigin("AWProjectWBFTHPG", "initializeToVis[R&D]"));
	image=&iimage;
    
	ok();
    
	init();
	makingPSF = false;
	initMaps(vb);
	//visResampler_p->setMaps(chanMap, polMap);
    
	findConvFunction(*image, vb);
	if (isDryRun) return;
	//  
	// Initialize the maps for polarization and channel. These maps
	// translate visibility indices into image indices
	//

	nx    = image->shape()(0);
	ny    = image->shape()(1);
	npol  = image->shape()(2);
	nchan = image->shape()(3);
    
	//
	// If we are memory-based then read the image in and create an
	// ArrayLattice otherwise just use the PagedImage
	//

	isTiled=false;

	{
	  IPosition gridShape(4, nx, ny, npol, nchan);
	  griddedData.resize(gridShape);
	  griddedData=Complex(0.0);
      
	  IPosition stride(4, 1);
	  IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
			(ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
	  IPosition trc(blc+image->shape()-stride);
	
	  IPosition start(4, 0);
	  griddedData(blc, trc) = image->getSlice(start, image->shape());
	
	  arrayLattice = new ArrayLattice<Complex>(griddedData);
	  lattice=arrayLattice;
	}
      }
      
    private:
      
      Bool applyFFT_p;
    };
  } //# NAMESPACE CASA - END
};
#endif
