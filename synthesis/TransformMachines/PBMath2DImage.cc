//# PBMath2DImage.cc: Implementation for PBMath2DImage
//# Copyright (C) 1996,1997,1998,1999,2003
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
 
#include <casa/aips.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>
#include <scimath/Mathematics/FFTServer.h>
#include <synthesis/TransformMachines/PBMath2DImage.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <images/Images/ImageRegrid.h>
#include <images/Images/ImageSummary.h>
#include <msvis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <casa/Utilities/Assert.h>
#include <components/ComponentModels/ComponentType.h>
#include <casa/Quanta.h>
#include <measures/Measures.h>
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;
typedef unsigned long long ooLong; 
namespace casa {

PBMath2DImage::PBMath2DImage(ImageInterface<Float>& reJonesImage):
  PBMath2D(), reJonesImage_p(0), reRegridJonesImage_p(0),
  imJonesImage_p(0), imRegridJonesImage_p(0),
  incrementsReJones_p(0), incrementsImJones_p(0),
  referencePixelReJones_p(0), referencePixelImJones_p(0),
  pa_p(0.0)
{
  LogIO os(LogOrigin("PBMath2DImage", "PBMath2DImage"));

  os << "Using two-dimensional purely real image model for antenna voltage pattern" << LogIO::POST;

  reJonesImage_p = new TempImage<Float>(reJonesImage.shape(), reJonesImage.coordinates());
  reJonesImage_p->copyData(reJonesImage);
  incrementsReJones_p=new Vector<Double>(reJonesImage_p->coordinates().directionCoordinate(0).increment());
  referencePixelReJones_p=new Vector<Double>(reJonesImage_p->coordinates().directionCoordinate(0).referencePixel());

};

PBMath2DImage::PBMath2DImage(ImageInterface<Float>& reJonesImage,
			     ImageInterface<Float>& imJonesImage) :
  PBMath2D(), reJonesImage_p(0), reRegridJonesImage_p(0),
  imJonesImage_p(0), imRegridJonesImage_p(0), 
  incrementsReJones_p(0), incrementsImJones_p(0),
  referencePixelReJones_p(0), referencePixelImJones_p(0),
  pa_p(0.0)
{

  LogIO os(LogOrigin("PBMath2DImage", "PBMath2DImage"));

  os << "Using two-dimensional real and imaginary image models for antenna voltage pattern" << LogIO::POST;

  checkJonesCongruent(reJonesImage, imJonesImage);

  // Save images and useful information 
  reJonesImage_p = new TempImage<Float>(reJonesImage.shape(), reJonesImage.coordinates());
  reJonesImage_p->copyData(reJonesImage);
  incrementsReJones_p=new Vector<Double>(reJonesImage_p->coordinates().directionCoordinate(0).increment());
  referencePixelReJones_p=new Vector<Double>(reJonesImage_p->coordinates().directionCoordinate(0).referencePixel());

  imJonesImage_p = new TempImage<Float>(imJonesImage.shape(), imJonesImage.coordinates());
  imJonesImage_p->copyData(imJonesImage);
  incrementsImJones_p=new Vector<Double>(imJonesImage_p->coordinates().directionCoordinate(0).increment());
  referencePixelImJones_p=new Vector<Double>(imJonesImage_p->coordinates().directionCoordinate(0).referencePixel());
};
PBMath2DImage::PBMath2DImage(const ImageInterface<Complex>& jonesImage) :
  PBMath2D(), reJonesImage_p(0), reRegridJonesImage_p(0),
  imJonesImage_p(0), imRegridJonesImage_p(0), 
  incrementsReJones_p(0), incrementsImJones_p(0),
  referencePixelReJones_p(0), referencePixelImJones_p(0),
  pa_p(0.0){
  reJonesImage_p = new TempImage<Float>(jonesImage.shape(), jonesImage.coordinates());
  reJonesImage_p->copyData(LatticeExpr<float> (real(jonesImage)));
  imJonesImage_p = new TempImage<Float>(jonesImage.shape(), jonesImage.coordinates());
  imJonesImage_p->copyData(LatticeExpr<float> (imag(jonesImage)));
  incrementsReJones_p=new Vector<Double>(reJonesImage_p->coordinates().directionCoordinate(0).increment());
  referencePixelReJones_p=new Vector<Double>(reJonesImage_p->coordinates().directionCoordinate(0).referencePixel());
  incrementsImJones_p=new Vector<Double>(imJonesImage_p->coordinates().directionCoordinate(0).increment());
  referencePixelImJones_p=new Vector<Double>(imJonesImage_p->coordinates().directionCoordinate(0).referencePixel());
}



PBMath2DImage::~PBMath2DImage()
{
  if(reJonesImage_p) delete reJonesImage_p; reJonesImage_p=0;
  if(imJonesImage_p) delete imJonesImage_p; imJonesImage_p=0;
  if(incrementsReJones_p) delete incrementsReJones_p; incrementsReJones_p=0;
  if(incrementsImJones_p) delete incrementsImJones_p; incrementsImJones_p=0;
  if(referencePixelReJones_p) delete referencePixelReJones_p; referencePixelReJones_p=0;
  if(referencePixelImJones_p) delete referencePixelImJones_p; referencePixelImJones_p=0;
};

PBMath2DImage& PBMath2DImage::operator=(const PBMath2DImage& other)
{
  if (this == &other)
    return *this;

  PBMath2D::operator=(other);
  reJonesImage_p = other.reJonesImage_p;
  imJonesImage_p = other.imJonesImage_p;
  incrementsReJones_p = other.incrementsReJones_p;
  incrementsImJones_p = other.incrementsImJones_p;
  referencePixelReJones_p = other.referencePixelReJones_p;
  referencePixelImJones_p = other.referencePixelImJones_p;

  return *this;
};



void
PBMath2DImage::summary(Int nValues)
{
  PBMath2D::summary(nValues);
  LogIO os(LogOrigin("PBMath2DImage", "summary"));

  {
    os << "Original image of the real part of Jones matrix:" << LogIO::POST;
    ImageSummary<Float> is(*reJonesImage_p);
    is.list(os);
  }

  if(imJonesImage_p) {
    os << "Original image of the imaginary part of Jones matrix:"
       << LogIO::POST;
    ImageSummary<Float> is(*imJonesImage_p);
    is.list(os);
  }

};

// Apply the Jones matrices to the input cube (x,y,pol)
// giving an output cube (x,y,pol). These two methods 
// should be moved to Fortran for optimization

// Complex to Complex
void 
PBMath2DImage::applyJones(const Array<Float>* reJones,
			  const Array<Float>* imJones,
			  const Array<Complex>& in,
			  Array<Complex>& out,
			  Vector<Int>& polmap,
			  Bool /*inverse*/,
			  Bool /*conjugate*/,
			  Int ipower,  // ie, 1=VP, 2=PB
			  Float /*cutoff*/,
			  Bool circular,
			  Bool forward)
{

  LogIO os(LogOrigin("PBMath2DImage", "applyJones"));
  // This should never be called
  if((ipower!=2)&&(ipower!=1)) {
    os << "Logic error - trying to apply illegal power of PB"
       << LogIO::EXCEPTION;
  }

  Int nx=in.shape()(0);
  Int ny=in.shape()(1);
  Int npol=in.shape()(2);

  // Loop through x, y coordinates of this cube
  Matrix<Complex> cmat(2,2);
  IPosition sp0(4, 0, 0, polmap(3), 0);
  IPosition sp1(4, 0, 0, polmap(2), 0);
  IPosition sp2(4, 0, 0, polmap(1), 0);
  IPosition sp3(4, 0, 0, polmap(0), 0);
  for (Int ix=0; ix<nx; ix++) {
    sp0(0)=ix;
    sp1(0)=ix;
    sp2(0)=ix;
    sp3(0)=ix;

    for (Int iy=0; iy<ny; iy++) {

      sp0(1)=iy;
      sp1(1)=iy;
      sp2(1)=iy;
      sp3(1)=iy;

      // E Jones for this pixel
      mjJones4 j4;
      if(imJones) {
	cmat(0,0)=Complex((*reJones)(sp0), (*imJones)(sp0));
	cmat(1,0)=Complex((*reJones)(sp1), (*imJones)(sp1));
	cmat(0,1)=Complex((*reJones)(sp2), (*imJones)(sp2));
	cmat(1,1)=Complex((*reJones)(sp3), (*imJones)(sp3));
      }
      else {
	cmat(0,0)=Complex((*reJones)(sp0), 0.0);
	cmat(1,0)=Complex((*reJones)(sp1), 0.0);
	cmat(0,1)=Complex((*reJones)(sp2), 0.0);
	cmat(1,1)=Complex((*reJones)(sp3), 0.0);
      }
      mjJones2 j2(cmat);

      // Make the relevant Jones matrix
      // E
      if(ipower==1) { // VP
	mjJones2 j2unit(Complex(1.0, 0.0));
	directProduct(j4, j2, j2unit);
      }
      // Primary beam = E . conj(E)
      else if(ipower==2) { // PB
	// Make the conjugate before constructing the
	// mjJones since otherwise reference semantics
	// get us
	mjJones2 j2conj(conj(cmat));
	directProduct(j4, j2, j2conj);
      }

      // Subtlety - we have to distinguish between applying
      // the PB and applying the adjoint. The former is needed
      // for predictions (sky->UV) and the latter is needed
      // for inversion (UV->sky). For circular polarization
      // this affects only the cross hand terms
      if(!forward) {
	j4=mjJones4(adjoint(j4.matrix()));
      }

      // Now apply the Jones matrix
      if(npol==1) {
	IPosition ip0(4, ix, iy, 0, 0);
	CStokesVector outCS(in(ip0), 0.0, 0.0, 0.0);
	outCS*=j4;
	out(ip0)=outCS(0);
      }
      else if(npol==2) {
	IPosition ip0(4, ix, iy, 0, 0);
	IPosition ip1(4, ix, iy, 1, 0);
	if(circular) {
	  CStokesVector outCS(in(ip0), 0.0, 0.0, in(ip1));
	  outCS*=j4;
	  out(ip0)=outCS(0);
	  out(ip1)=outCS(3);
	}
	else {
	  CStokesVector outCS(in(ip0), in(ip1), 0.0, 0.0);
	  outCS*=j4;
	  out(ip0)=outCS(0);
	  out(ip1)=outCS(1);
	}
      }
      else if(npol==4) {
	IPosition ip0(4, ix, iy, 0, 0);
	IPosition ip1(4, ix, iy, 1, 0);
	IPosition ip2(4, ix, iy, 2, 0);
	IPosition ip3(4, ix, iy, 3, 0);
	CStokesVector outCS(in(ip0), in(ip1), in(ip2), in(ip3));
	outCS*=j4;
	out(ip0)=outCS(0);
	out(ip1)=outCS(1);
	out(ip2)=outCS(2);
	out(ip3)=outCS(3);
      }
    }
  }
}


// Complex to Complex
void 
PBMath2DImage::applyJonesFast(const Float*& reJones,
			  const Float*& imJones,
			  const Array<Complex>& in,
			  Array<Complex>& out,
			  Vector<Int>& polmap,
			  Bool /*inverse*/,
			  Bool /*conjugate*/,
			  Int ipower,  // ie, 1=VP, 2=PB
			  Float /*cutoff*/,
			  Bool circular,
			  Bool forward)
{

  LogIO os(LogOrigin("PBMath2DImage", "applyJones"));
  // This should never be called
  if((ipower!=2)&&(ipower!=1)) {
    os << "Logic error - trying to apply illegal power of PB"
       << LogIO::EXCEPTION;
  }

  Bool delpolmap;
  const Int  *polmap1=polmap.getStorage(delpolmap);
  const Float* reJones1=reJones;
  const Float* imJones1=imJones;

  Int nx=in.shape()(0);
  Int ny=in.shape()(1);
  Int npol=in.shape()(2);
  Bool lala=false;
  Float laloo=0.0;
  
  Bool delin, delout;
  Complex *outstor=out.getStorage(delout);
  const Complex *instor=in.getStorage(delin);
  //Int ind0, ind1, ind2, ind3; 
  // Loop through x, y coordinates of this cube
  Matrix<Complex> cmat(2,2);
  IPosition sp0(4, 0, 0, polmap(3), 0);
  IPosition sp1(4, 0, 0, polmap(2), 0);
  IPosition sp2(4, 0, 0, polmap(1), 0);
  IPosition sp3(4, 0, 0, polmap(0), 0);


   #pragma omp parallel default(none)  shared(outstor) firstprivate(reJones1, imJones1 , instor, polmap1, lala, laloo, ipower, circular, forward, nx,ny, npol)

  {
    #pragma omp for
  for (Int ix=0; ix<nx; ix++) {
    //sp0(0)=ix;
    //sp1(0)=ix;
    //sp2(0)=ix;
    //sp3(0)=ix;
    
       applyJonesFastX(reJones1, imJones1, instor, outstor, polmap1,
			  lala,	  lala,ipower,  // ie, 1=VP, 2=PB
		    laloo,circular,forward,ix, nx, ny, npol);
       /* for (Int iy=0; iy<ny; iy++) {

      //sp0(1)=iy;
      //sp1(1)=iy;
      //sp2(1)=iy;
      //sp3(1)=iy;

      ind0=ix+nx*iy +(nx*ny)*polmap(3);
      ind1=ix+nx*iy +(nx*ny)*polmap(2);
      ind2=ix+nx*iy +(nx*ny)*polmap(1);
      ind3=ix+nx*iy +(nx*ny)*polmap(0);
      // E Jones for this pixel
      mjJones4 j4;
      if(imJones) {
	cmat(0,0)=Complex(reJones[ind0], imJones[ind0]);
	cmat(1,0)=Complex(reJones[ind1], imJones[ind1]);
	cmat(0,1)=Complex(reJones[ind2], imJones[ind2]);
	cmat(1,1)=Complex(reJones[ind3], imJones[ind3]);
      }
      else {
	cmat(0,0)=Complex(reJones[ind0], 0.0);
	cmat(1,0)=Complex(reJones[ind1], 0.0);
	cmat(0,1)=Complex(reJones[ind2], 0.0);
	cmat(1,1)=Complex(reJones[ind3], 0.0);
      }
      mjJones2 j2(cmat);

      // Make the relevant Jones matrix
      // E
      if(ipower==1) { // VP
	mjJones2 j2unit(Complex(1.0, 0.0));
	directProduct(j4, j2, j2unit);
      }
      // Primary beam = E . conj(E)
      else if(ipower==2) { // PB
	// Make the conjugate before constructing the
	// mjJones since otherwise reference semantics
	// get us
	mjJones2 j2conj(conj(cmat));
	directProduct(j4, j2, j2conj);
      }

      // Subtlety - we have to distinguish between applying
      // the PB and applying the adjoint. The former is needed
      // for predictions (sky->UV) and the latter is needed
      // for inversion (UV->sky). For circular polarization
      // this affects only the cross hand terms
      if(!forward) {
	j4=mjJones4(adjoint(j4.matrix()));
      }

      // Now apply the Jones matrix
      if(npol==1) {
	//IPosition ip0(4, ix, iy, 0, 0);
	ind0=ix+iy*nx;
	CStokesVector outCS(instor[ind0], 0.0, 0.0, 0.0);
	outCS*=j4;
	outstor[ind0]=outCS(0);
      }
      else if(npol==2) {
	//IPosition ip0(4, ix, iy, 0, 0);
	//IPosition ip1(4, ix, iy, 1, 0);
	ind0=ix+iy*nx;
	ind1=ix+iy*nx+nx*ny;
	if(circular) {
	  CStokesVector outCS(instor[ind0], 0.0, 0.0, instor[ind1]);
	  outCS*=j4;
	  outstor[ind0]=outCS(0);
	  outstor[ind1]=outCS(3);
	}
	else {
	  CStokesVector outCS(instor[ind0], instor[ind1], 0.0, 0.0);
	  outCS*=j4;
	  outstor[ind0]=outCS(0);
	  outstor[ind1]=outCS(1);
	}
      }
      else if(npol==4) {
	//IPosition ip0(4, ix, iy, 0, 0);
	//IPosition ip1(4, ix, iy, 1, 0);
	//IPosition ip2(4, ix, iy, 2, 0);
	//IPosition ip3(4, ix, iy, 3, 0);
	ind0=ix+iy*nx;
	ind1=ix+iy*nx+nx*ny;
	ind2=ix+iy*nx+2*nx*ny;
	ind3=ix+iy*nx+3*nx*ny;
	CStokesVector outCS(instor[ind0], instor[ind1], instor[ind2], instor[ind3]);
	outCS*=j4;
	outstor[ind0]=outCS(0);
	outstor[ind1]=outCS(1);
	outstor[ind2]=outCS(2);
	outstor[ind3]=outCS(3);
      }
    }
       */
    
  }
  } //OMP
  out.putStorage(outstor, delout);
  in.freeStorage(instor, delin);
  polmap.freeStorage(polmap1, delpolmap);
}




void 
PBMath2DImage::applyJonesFastX(const Float*& reJones,
			  const Float*& imJones,
			  const Complex*& instor,
			  Complex*& outstor,
			  const Int*& polmap,
			  Bool /*inverse*/,
			  Bool /*conjugate*/,
			  Int ipower,  // ie, 1=VP, 2=PB
			  Float /*cutoff*/,
			  Bool circular,
			       Bool forward,
			       const Int ix, const Int nx, const Int ny, const Int npol){

  ooLong ind0, ind1, ind2, ind3; 
  // Loop through x, y coordinates of this cube
  Matrix<Complex> cmat(2,2);

  for (ooLong iy=0; iy< ooLong(ny) ; iy++) {
    
    //sp0(1)=iy;
    //sp1(1)=iy;
    //sp2(1)=iy;
    //sp3(1)=iy;
    
    ind0=ooLong(ix)+ooLong(nx)*iy +(ooLong(nx)*ooLong(ny))*ooLong(polmap[3]);
    ind1=ooLong(ix)+ooLong(nx)*iy +(ooLong(nx)*ooLong(ny))*ooLong(polmap[2]);
    ind2=ooLong(ix)+ooLong(nx)*iy +(ooLong(nx)*ooLong(ny))*ooLong(polmap[1]);
    ind3=ooLong(ix)+ooLong(nx)*iy +(ooLong(nx)*ooLong(ny))*ooLong(polmap[0]);
    // E Jones for this pixel
    mjJones4 j4;
    if(imJones) {
      cmat(0,0)=Complex(reJones[ind0], imJones[ind0]);
      cmat(1,0)=Complex(reJones[ind1], imJones[ind1]);
      cmat(0,1)=Complex(reJones[ind2], imJones[ind2]);
      cmat(1,1)=Complex(reJones[ind3], imJones[ind3]);
    }
    else {
      cmat(0,0)=Complex(reJones[ind0], 0.0);
      cmat(1,0)=Complex(reJones[ind1], 0.0);
      cmat(0,1)=Complex(reJones[ind2], 0.0);
      cmat(1,1)=Complex(reJones[ind3], 0.0);
    }
    mjJones2 j2(cmat);
    
    // Make the relevant Jones matrix
    // E
    if(ipower==1) { // VP
      mjJones2 j2unit(Complex(1.0, 0.0));
      directProduct(j4, j2, j2unit);
    }
    // Primary beam = E . conj(E)
    else if(ipower==2) { // PB
      // Make the conjugate before constructing the
      // mjJones since otherwise reference semantics
      // get us
      mjJones2 j2conj(conj(cmat));
      directProduct(j4, j2, j2conj);
    }
    
    // Subtlety - we have to distinguish between applying
    // the PB and applying the adjoint. The former is needed
    // for predictions (sky->UV) and the latter is needed
    // for inversion (UV->sky). For circular polarization
    // this affects only the cross hand terms
    if(!forward) {
      j4=mjJones4(adjoint(j4.matrix()));
    }
    
    // Now apply the Jones matrix
      if(npol==1) {
	//IPosition ip0(4, ix, iy, 0, 0);
	ind0=ix+iy*nx;
	CStokesVector outCS(instor[ind0], 0.0, 0.0, 0.0);
	outCS*=j4;
	outstor[ind0]=outCS(0);
      }
      else if(npol==2) {
	//IPosition ip0(4, ix, iy, 0, 0);
	//IPosition ip1(4, ix, iy, 1, 0);
	ind0=ix+iy*nx;
	ind1=ix+iy*nx+nx*ny;
	if(circular) {
	  CStokesVector outCS(instor[ind0], 0.0, 0.0, instor[ind1]);
	  outCS*=j4;
	  outstor[ind0]=outCS(0);
	  outstor[ind1]=outCS(3);
	}
	else {
	  CStokesVector outCS(instor[ind0], instor[ind1], 0.0, 0.0);
	  outCS*=j4;
	  outstor[ind0]=outCS(0);
	  outstor[ind1]=outCS(1);
	}
      }
      else if(npol==4) {
	//IPosition ip0(4, ix, iy, 0, 0);
	//IPosition ip1(4, ix, iy, 1, 0);
	//IPosition ip2(4, ix, iy, 2, 0);
	//IPosition ip3(4, ix, iy, 3, 0);
	ind0=ix+iy*nx;
	ind1=ix+iy*nx+nx*ny;
	ind2=ix+iy*nx+2*nx*ny;
	ind3=ix+iy*nx+3*nx*ny;
	CStokesVector outCS(instor[ind0], instor[ind1], instor[ind2], instor[ind3]);
	outCS*=j4;
	outstor[ind0]=outCS(0);
	outstor[ind1]=outCS(1);
	outstor[ind2]=outCS(2);
	outstor[ind3]=outCS(3);
      }
  }





}


// Float to Float on real part of complex stokes
// This is only really useful for the weights image
// from SkyEquation - perhaps make SkyEquation a
// friend?
void 
PBMath2DImage::applyJones(const Array<Float>* reJones,
			  const Array<Float>* imJones,
			  const Array<Float>& in,
			  Array<Float>& out,
			  Vector<Int>& polmap,
			  Float /*cutoff*/,
			  Bool circular)
{
  LogIO os(LogOrigin("PBMath2DImage", "applyJones"));


  Matrix<Complex> s(4,4), sAdjoint(4,4);
  Matrix<Complex> Lambda(4,4);
  s.set(Complex(0.0,0.0));
  sAdjoint.set(Complex(0.0));
  Lambda.set(Complex(0.0,0.0));
  s=0;
  if(!circular) {
    s(0,0)=Complex(0.5);
    s(0,1)=Complex(0.5);
    s(1,2)=Complex(0.5);
    s(1,3)=Complex(0.0,0.5);
    s(2,2)=Complex(0.5);
    s(2,3)=Complex(0.0,-0.5);
    s(3,0)=Complex(0.5);
    s(3,1)=Complex(-0.5);
  }
  else {
    s(0,0)=Complex(0.5);
    s(0,3)=Complex(0.5);
    s(1,1)=Complex(0.5);
    s(1,2)=Complex(0.0,0.5);
    s(2,1)=Complex(0.5);
    s(2,2)=Complex(0.0,-0.5);
    s(3,0)=Complex(0.5);
    s(3,3)=Complex(-0.5);
  }
  sAdjoint=adjoint(s);

  Int nx=in.shape()(0);
  Int ny=in.shape()(1);
  Int npol=in.shape()(2);

  Matrix<Complex> cmat(2,2);
  Matrix<Complex> prod(4,4);
  prod.set(Complex(0.0));
  IPosition sp0(4, 0, 0, polmap(3), 0);
  IPosition sp1(4, 0, 0, polmap(2), 0);
  IPosition sp2(4, 0, 0, polmap(1), 0);
  IPosition sp3(4, 0, 0, polmap(0), 0);
  
  IPosition ip0(4, 0, 0, 0, 0);
  IPosition ip1(4, 0, 0, 1, 0);
  IPosition ip2(4, 0, 0, 2, 0);
  IPosition ip3(4, 0, 0, 3, 0);
  for (Int ix=0; ix<nx; ++ix) {
    sp0(0)=ix;
    sp1(0)=ix;
    sp2(0)=ix;
    sp3(0)=ix;
    ip0(0)=ix;
    ip1(0)=ix;
    ip2(0)=ix;
    ip3(0)=ix;
    for (Int iy=0; iy<ny; ++iy) {

      sp0(1)=iy;
      sp1(1)=iy;
      sp2(1)=iy;
      sp3(1)=iy;

      ip0(1)=iy;
      ip1(1)=iy;
      ip2(1)=iy;
      ip3(1)=iy;

      // Lambda is the covariance matrix of the noise in the
      // image plane for each complex stokes e.g. RR, LR, etc.
      Lambda=0.0;
      if(npol==1) {
	Lambda(0,0)=in(ip0);
	Lambda(3,3)=in(ip0);
      }
      else if(npol==2) {
	Lambda(0,0)=in(ip0);
	Lambda(3,3)=in(ip1);
      }
      else if(npol==4) {
	Lambda(0,0)=in(ip0);
	Lambda(1,1)=in(ip1);
	Lambda(2,2)=in(ip2);
	Lambda(3,3)=in(ip3);
      }

      // E Jones for this pixel
      mjJones4 j4(Matrix<Complex>(4, 4, Complex(0.0, 0.0)));
      if(imJones) {
	cmat(0,0)=Complex((*reJones)(sp0), (*imJones)(sp0));
	cmat(1,0)=Complex((*reJones)(sp1), (*imJones)(sp1));
	cmat(0,1)=Complex((*reJones)(sp2), (*imJones)(sp2));
	cmat(1,1)=Complex((*reJones)(sp3), (*imJones)(sp3));
      }
      else {
	cmat(0,0)=Complex((*reJones)(sp0), 0.0);
	cmat(1,0)=Complex((*reJones)(sp1), 0.0);
	cmat(0,1)=Complex((*reJones)(sp2), 0.0);
	cmat(1,1)=Complex((*reJones)(sp3), 0.0);
      }
      mjJones2 j2(cmat);

      // Direct product of E with conj(E) ~ PB
      mjJones2 j2conj(conj(cmat));
      directProduct(j4, j2, j2conj);
      // We need the element by element multiplication
      Matrix<Complex> matj4(j4.matrix());
      Matrix<Complex> amatj4(adjoint(matj4));
      // Noise covar in complex stokes = Adjoint(PB) . Lambda . PB
      Lambda=product(amatj4, product(Lambda, matj4));
      prod=product(sAdjoint,product(Lambda,s));
      // Noise covar in real stokes
      if(npol==1) {
	out(ip0)=real(prod(0,0))+real(prod(3,3));
      }
      else if (npol==2) {
	out(ip0)=2.0*real(prod(0,0));
	out(ip1)=2.0*real(prod(3,3));
      }
      else if (npol==4) {
	out(ip0)=2.0*real(prod(0,0));
	out(ip1)=2.0*real(prod(1,1));
	out(ip2)=2.0*real(prod(2,2));
	out(ip3)=2.0*real(prod(3,3));
      }
    }
  }

}

ImageInterface<Complex>& PBMath2DImage::apply(const ImageInterface<Complex>& in,
					      ImageInterface<Complex>& out,
					      const MDirection& sp,
					      const Quantity parAngle,	      
					      const BeamSquint::SquintType /*doSquint*/,
					      Bool inverse,
					      Bool conjugate,
					      Int ipower,  // ie, 1=VP, 2=PB
					      Float cutoff,
					      Bool forward)
{
  LogIO os(LogOrigin("PBMath2DImage", "apply"));

  if(!in.shape().isEqual(out.shape(), true)) {
    os << "Input and output images have different shapes" 
       << LogIO::EXCEPTION;
    
  }

//   {
//     PagedImage<Float> inImage(in.shape(), in.coordinates(), "RealBeforeApplyJones");
//     LatticeExpr<Float> le(real(in));
//     inImage.copyData(le);
//   }
//   {
//     PagedImage<Float> inImage(in.shape(), in.coordinates(), "ImagBeforeApplyJones");
//     LatticeExpr<Float> le(imag(in));
//     inImage.copyData(le);
//   }


  const IPosition oshape(out.shape());
  updateJones(out.coordinates(), oshape, sp, parAngle);
  
  // Read through the images in pol, x, y, and frequency
  IPosition iShape(in.shape());
  IPosition oShape(out.shape());
  IPosition jShape(reRegridJonesImage_p->shape());
  Int nx=iShape(0);
  Int ny=iShape(1);
  Int npol=iShape(2);

  // Find out if these are circular images
  Vector<Int> polmap(4);
  StokesImageUtil::PolRep polframe;
  Int insm=StokesImageUtil::CStokesPolMap(polmap, polframe, in.coordinates());
  Int outsm=StokesImageUtil::CStokesPolMap(polmap, polframe, out.coordinates());
  if(insm!=outsm) {
    os << "Input and Output images have different polarization frames"
       << LogIO::EXCEPTION;
  }
  Bool circular=(polframe==StokesImageUtil::CIRCULAR);
	      
  // Now get the polarization remapping for the Jones image
  //Int jsm=    // CStokesPolMap sets polmap and polframe.
  StokesImageUtil::CStokesPolMap(polmap, polframe,
                                 reJonesImage_p->coordinates());

  // For the input and output images, get all polarizations for x, y plane
  IPosition inPlane(4, nx, ny, npol, 1);
  LatticeStepper inls(iShape, inPlane, IPosition(4, 0, 1, 2, 3));
  RO_LatticeIterator<Complex> inli(in, inls);

  IPosition outPlane(4, nx, ny, npol, 1);
  LatticeStepper outls(oShape, outPlane, IPosition(4, 0, 1, 2, 3));
  LatticeIterator<Complex> outli(out, outls);

  // For the Jones image, get all 4 polarizations for one x, y plane
  IPosition jPlane(4, nx, ny, 4, 1);
  LatticeStepper jls(jShape, jPlane, IPosition(4, 0, 1, 2, 3));
  RO_LatticeIterator<Float> reJonesli(*reRegridJonesImage_p, jls);
  reJonesli.reset();

  Bool delreal, delimag;
  if(imRegridJonesImage_p) {
    RO_LatticeIterator<Float> imJonesli(*imRegridJonesImage_p, jls);
    
    imJonesli.reset();
    for (inli.reset(), outli.reset(); !inli.atEnd(); inli++, outli++) {
      const Float *restor=reJonesli.cursor().getStorage(delreal);
      const Float *imstor=imJonesli.cursor().getStorage(delimag);
      applyJonesFast(restor, imstor,
		 inli.cursor(), outli.rwCursor(), polmap,
		 inverse, conjugate,
		 ipower, cutoff, circular, forward);
      reJonesli.cursor().freeStorage(restor, delreal);
      imJonesli.cursor().freeStorage(imstor, delimag);
    }
  }
  else {
    for (inli.reset(), outli.reset(); !inli.atEnd(); inli++, outli++) {
      const Float *restor=reJonesli.cursor().getStorage(delreal);
      const Float *imstor=NULL;
      applyJonesFast(restor,
		 imstor,
		 inli.cursor(),
		 outli.rwCursor(), polmap, inverse, conjugate,
		 ipower, cutoff, circular, forward);
      reJonesli.cursor().freeStorage(restor, delreal);
    }
  }

//   {
//     PagedImage<Float> outImage(out.shape(), out.coordinates(), "RealAfterApplyJones");
//     LatticeExpr<Float> le(real(out));
//     outImage.copyData(le);
//   }
//   {
//     PagedImage<Float> outImage(out.shape(), out.coordinates(), "ImagAfterApplyJones");
//     LatticeExpr<Float> le(imag(out));
//     outImage.copyData(le);
//   }
  return out;
}

ImageInterface<Float>& PBMath2DImage::apply(const ImageInterface<Float>& in,
					    ImageInterface<Float>& out,
					    const MDirection& sp,
					    const Quantity parAngle,	      
					    const BeamSquint::SquintType /*doSquint*/,
					    Float cutoff,
					    Int /*ipower*/)
{
  LogIO os(LogOrigin("PBMath2DImage", "apply"));

  const IPosition oshape(out.shape());
  if(!in.shape().isEqual(out.shape(), true)) {
    os << "Input and output images have different shapes" 
       << LogIO::EXCEPTION;
    
  }

  updateJones(out.coordinates(), oshape, sp, parAngle);
  
  // Read through the images in pol, x, y, and frequency
  IPosition iShape(in.shape());
  IPosition oShape(out.shape());
  IPosition jShape(reRegridJonesImage_p->shape());
  Int nx=iShape(0);
  Int ny=iShape(1);
  Int npol=iShape(2);

  // Find out if these are circular images
  Vector<Int> polmap(4);
  StokesImageUtil::PolRep polframe;
  Int insm=StokesImageUtil::CStokesPolMap(polmap, polframe, in.coordinates());
  Int outsm=StokesImageUtil::CStokesPolMap(polmap, polframe, out.coordinates());
  if(insm!=outsm) {
    os << "Input and Output images have different polarization frames"
       << LogIO::EXCEPTION;
  }
  Bool circular=(insm<1);

  // Now get the polarization remapping for the Jones image
  //Int jsm=  
  StokesImageUtil::CStokesPolMap(polmap, polframe,
  					 reJonesImage_p->coordinates());
  // For the input and output images, get all polarizations for x, y plane
  IPosition inPlane(4, nx, ny, npol, 1);
  LatticeStepper inls(iShape, inPlane, IPosition(4, 0, 1, 2, 3));
  RO_LatticeIterator<Float> inli(in, inls);

  IPosition outPlane(4, nx, ny, npol, 1);
  LatticeStepper outls(oShape, outPlane, IPosition(4, 0, 1, 2, 3));
  LatticeIterator<Float> outli(out, outls);

  // For the Jones image, get all 4 polarizations for one x, y plane
  IPosition jPlane(4, nx, ny, 4, 1);
  LatticeStepper jls(jShape, jPlane, IPosition(4, 0, 1, 2, 3));
  RO_LatticeIterator<Float> reJonesli(*reRegridJonesImage_p, jls);
  reJonesli.reset();

  if(imRegridJonesImage_p) {
    RO_LatticeIterator<Float> imJonesli(*imRegridJonesImage_p, jls);
    
    imJonesli.reset();
    for (inli.reset(), outli.reset(); !inli.atEnd(); inli++, outli++) {
      applyJones(&reJonesli.cursor(), &imJonesli.cursor(),
		 inli.cursor(), outli.rwCursor(), polmap, cutoff, circular);
    }
  }
  else {
    for (inli.reset(), outli.reset(); !inli.atEnd(); inli++, outli++) {
      applyJones(&(reJonesli.cursor()),
		 (const Array<Float>*)0,
		 inli.cursor(),
		 outli.rwCursor(), polmap, cutoff, circular);
    }
  }

  return out;
}


SkyComponent& PBMath2DImage::apply(SkyComponent& in,
				   SkyComponent& out,
				   const MDirection& sp,
				   const Quantity frequency,	      
				   const Quantity parAngle,	      
				   const BeamSquint::SquintType /*doSquint*/,
				   Bool /*inverse*/,
				   Bool /*conjugate*/,
				   Int ipower,  // ie, 1=VP, 2=PB, 4=PB^2
				   Float /*cutoff*/,
				   Bool forward)
{
  LogIO os(LogOrigin("PBMath2DImage", "apply"));

  // Now get the polarization remapping for the Jones image
  Vector<Int> polmap(4);
  StokesImageUtil::PolRep polframe;

  // jsm (= circular or linear) is not used, but CStokesPolMap also sets polframe.
  //Int jsm=
  StokesImageUtil::CStokesPolMap(polmap, polframe, reJonesImage_p->coordinates());

  // First get the frequency of the output image
  Double desiredFrequency=frequency.getValue("Hz");
  
  CoordinateSystem reCoords(reJonesImage_p->coordinates());
  Int spectralIndex=reCoords.findCoordinate(Coordinate::SPECTRAL);
  AlwaysAssert(spectralIndex>=0, AipsError);
  SpectralCoordinate
    imageSpectralCoord=reCoords.spectralCoordinate(spectralIndex);
  Double reFrequency=imageSpectralCoord.referenceValue()(0);
  Double freqScale = desiredFrequency/reFrequency;

  // Find the coordinates of the Sky Component in the
  // real and imag Jones images, remembering about the
  // pointing position sp. We do the opposite from the
  // image case - we convert the component position
  // back to an offset from the pointing position
  // and then calculate the location in Az,El
  MDirection compLoc=in.shape().refDirection();
  MVDirection spmvd(sp.getAngle());
  MVDirection compmvd(compLoc.getAngle());
  Vector<Double> world(2);
  // Scale for the observing frequency: when the desired frequency
  // is higher than the frequency for the Jones image, the separation
  // should be LARGER.
  Double sep=spmvd.separation(compmvd,"rad").getValue();
  sep*=freqScale;

  Double pa=+spmvd.positionAngle(compmvd,"rad").getValue();
  pa-=parAngle.getValue("rad");
  world(0)=-sep*sin(pa);
  world(1)=sep*cos(pa);

  Vector<Double> rePix(2), imPix(2);
  {
    Int directionIndex=reCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate
      reDirectionCoord=reCoords.directionCoordinate(directionIndex);
    reDirectionCoord.toPixel(rePix, world);
  }
  if(imJonesImage_p) {
    CoordinateSystem imCoords(imJonesImage_p->coordinates());
    Int directionIndex=imCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate
      imDirectionCoord=imCoords.directionCoordinate(directionIndex);
    imDirectionCoord.toPixel(imPix, world);
  }

  // E Jones for this pixel
  mjJones4 j4;

  Bool offImage=false;
  Matrix<Complex> cmat(2,2);
  Array<Float> reJones;
  Int nx=reJonesImage_p->shape()(0);
  Int ny=reJonesImage_p->shape()(1);
  Int npol=reJonesImage_p->shape()(2);

  Int px=Int(rePix(0)+0.5);
  Int py=Int(rePix(1)+0.5);
  if(imJonesImage_p) {
    Array<Float> imJones;
    if((px>-1)&&(px<nx)&&(py>-1)&&(py<ny)) {
      IPosition blc(4, px, py, 0, 0);
      IPosition len(4, 1, 1, npol, 1);
      IPosition ip0(4, 0, 0, polmap(3), 0);
      IPosition ip1(4, 0, 0, polmap(2), 0);
      IPosition ip2(4, 0, 0, polmap(1), 0);
      IPosition ip3(4, 0, 0, polmap(0), 0);
      reJonesImage_p->getSlice(reJones, blc, len);
      imJonesImage_p->getSlice(imJones, blc, len);
      cmat(0,0)=Complex(reJones(ip0), imJones(ip0));
      cmat(1,0)=Complex(reJones(ip1), imJones(ip1));
      cmat(0,1)=Complex(reJones(ip2), imJones(ip2));
      cmat(1,1)=Complex(reJones(ip3), imJones(ip3));
    }
    else {
      //      cerr << freqScale << " " << px << " " << py << " " << world << " " << sep << " " << pa << endl;
      offImage = true;
    }
  }
  else {
    if((px>-1)&&(px<nx)&&(py>-1)&&(py<ny)) {
      IPosition blc(4, px, py, 0, 0);
      IPosition len(4, 1, 1, npol, 1);
      IPosition ip0(4, 0, 0, polmap(3), 0);
      IPosition ip1(4, 0, 0, polmap(2), 0);
      IPosition ip2(4, 0, 0, polmap(1), 0);
      IPosition ip3(4, 0, 0, polmap(0), 0);
      reJonesImage_p->getSlice(reJones, blc, len);
      cmat(0,0)=Complex(reJones(ip0), 0.0);
      cmat(1,0)=Complex(reJones(ip1), 0.0);
      cmat(0,1)=Complex(reJones(ip2), 0.0);
      cmat(1,1)=Complex(reJones(ip3), 0.0);
    }
    else {
      offImage = true;
    }
      
  }

  // Check for the component being off source
  if(offImage) {
    Vector<Double> zero(4);
    zero=0.0;
    out=in.copy();
    out.flux().setValue(zero);
  }
  else {
    mjJones2 j2(cmat);
    
    // Make the relevant Jones matrix
    // E
    if(ipower==1) { // VP
      mjJones2 j2unit(Complex(1.0, 0.0));
      directProduct(j4, j2, j2unit);
    }
    // Primary beam = E . conj(E)
    else if(ipower==2) { // PB
      // Make the conjugate before constructing the
      // mjJones since otherwise reference semantics
      // get us
      mjJones2 j2conj(conj(cmat));
      directProduct(j4, j2, j2conj);
    }
    
    // Subtlety - we have to distinguish between applying
    // the PB and applying the adjoint. The former is needed
    // for predictions (sky->UV) and the latter is needed
    // for inversion (UV->sky)
    if(!forward) {
      j4=mjJones4(adjoint(j4.matrix()));
    }
    
    // Now apply the Jones matrix
    Vector<Complex> compFluxIn(4);
    if(polframe==StokesImageUtil::CIRCULAR) {
      in.flux().convertPol(ComponentType::CIRCULAR);
    }
    else {
      in.flux().convertPol(ComponentType::LINEAR);
    }
    // Reverse index convention
    for (Int pol=0;pol<4;pol++) {
      compFluxIn(3-pol)=in.flux().value()(pol);
    }
    CStokesVector outCS;
    outCS=compFluxIn;
    outCS*=j4;
    Vector<DComplex> compFlux(4);
    // Reverse index convention
    for (Int pol=0;pol<4;pol++) {
      compFlux(3-pol)=outCS(pol);
    }
    out=in.copy();
    out.flux().setValue(compFlux);
  }
  return out;
}

void PBMath2DImage::updateJones(const CoordinateSystem& coords,
				const IPosition& shape,
				const MDirection& pc,
				const Quantity& paAngle)
{

  LogIO os(LogOrigin("PBMath2DImage", "updateJones"));

  if (!StokesImageUtil::standardImageCoordinates(coords)) {
    os << "Image to be PB corrected does not have standard axes order"
       << LogIO::EXCEPTION;
    
  }
  // First get the frequency of the output image
  Int spectralIndex=coords.findCoordinate(Coordinate::SPECTRAL);
  AlwaysAssert(spectralIndex>=0, AipsError);
  SpectralCoordinate
    imageSpectralCoord=coords.spectralCoordinate(spectralIndex);
  Double desiredFrequency=imageSpectralCoord.referenceValue()(0);

  // Next get the frequency of the Jones image
  spectralIndex=reJonesImage_p->coordinates().findCoordinate(Coordinate::SPECTRAL);
  AlwaysAssert(spectralIndex>=0, AipsError);
  imageSpectralCoord=reJonesImage_p->coordinates().spectralCoordinate(spectralIndex);
  Double reFrequency=imageSpectralCoord.referenceValue()(0);

  Double reFreqScale = reFrequency/desiredFrequency;
  // os << "Desired frequency       = " << desiredFrequency << "Hz" << LogIO::POST;  
  //  os << "Real PB model frequency = " << reFrequency << "Hz" << LogIO::POST;  
  //  os << "Scaling real Jones image cell size by " << reFreqScale << LogIO::POST;

  Double imFreqScale = 1.0;
  if(imJonesImage_p) {
    spectralIndex=imJonesImage_p->coordinates().findCoordinate(Coordinate::SPECTRAL);
    AlwaysAssert(spectralIndex>=0, AipsError);
    imageSpectralCoord=imJonesImage_p->coordinates().spectralCoordinate(spectralIndex);
    Double imFrequency=imageSpectralCoord.referenceValue()(0);
    imFreqScale = imFrequency/desiredFrequency;
    //    os << "Imag PB model frequency = " << imFrequency << "Hz" << LogIO::POST;  
    //    os << "Scaling imag Jones image cell size by " << imFreqScale << LogIO::POST;
  }

  Float pa=-paAngle.getValue("rad");
  Matrix<Double> xform(2,2);
  xform(0,0) = cos(pa);
  xform(1,1) = cos(pa);
  xform(0,1) = -sin(pa);
  xform(1,0) = +sin(pa);

  //  cout << "updateJones: position angle = " << 180.0*pa/C::pi << endl;

  IPosition desiredShape(shape);
  desiredShape(2)=reJonesImage_p->shape()(2);
  desiredShape(3)=reJonesImage_p->shape()(3);
  Int jonesdirectionindex=reJonesImage_p->coordinates().findCoordinate(Coordinate::DIRECTION);
  Int imdirectionindex=coords.findCoordinate(Coordinate::DIRECTION);
  DirectionCoordinate
    imageDirectionCoord=coords.directionCoordinate(imdirectionindex);
  imageDirectionCoord.setWorldAxisUnits(reJonesImage_p->coordinates().directionCoordinate(jonesdirectionindex).worldAxisUnits());
  // Now set the desired coordinates for the regridded Jones images
  // The desired coordinates should have the same direction axis
  // as the image to input image and the same stokes and frequency
  // axes as the input Jones images.
  {
   

    AlwaysAssert(imdirectionindex>=0, AipsError);
    Int spectralIndex=coords.findCoordinate(Coordinate::SPECTRAL);
    AlwaysAssert(spectralIndex>=0, AipsError);
    SpectralCoordinate
      imageSpectralCoord=coords.spectralCoordinate(spectralIndex);
    Vector<Double> freq(1);
    freq(0)=desiredFrequency;
    imageSpectralCoord.setReferenceValue(freq);

    CoordinateSystem desiredCoords(reJonesImage_p->coordinates());
    desiredCoords.replaceCoordinate(imageDirectionCoord, jonesdirectionindex);
    desiredCoords.replaceCoordinate(imageSpectralCoord, spectralIndex);
 
    if(reRegridJonesImage_p && desiredCoords.near(reRegridJonesImage_p->coordinates(), 1.0e-4 ) && (desiredShape == (reRegridJonesImage_p->shape()))) {
	return;
      }

     // Delete any old images
    if(reRegridJonesImage_p) delete reRegridJonesImage_p; reRegridJonesImage_p=0;
    if(imRegridJonesImage_p) delete imRegridJonesImage_p; imRegridJonesImage_p=0;


   reRegridJonesImage_p = new TempImage<Float>(desiredShape,
						desiredCoords);
    reRegridJonesImage_p->set(0.0);
    if(imJonesImage_p) {
      imRegridJonesImage_p = new TempImage<Float>(desiredShape,
						  desiredCoords);
      imRegridJonesImage_p->set(0.0);
    }
    if(0) {
      os << "Regridded image of the real part of Jones matrix:" << LogIO::POST;
      ImageSummary<Float> is(*reRegridJonesImage_p);
      is.list(os);
    }

  }
    
  // Now fake the direction coordinates of the input Jones images to have
  // the same RA, DEC, etc as the pointing center and the xform matrix
  // needed to rotate +pa in angle.
  CoordinateSystem originalReJonesCoords(reJonesImage_p->coordinates());
  {
    AlwaysAssert(imdirectionindex>=0, AipsError);
    DirectionCoordinate
      reJonesDirectionCoord=coords.directionCoordinate(imdirectionindex);
    //////////////set the right units
    reJonesDirectionCoord.setWorldAxisUnits(reJonesImage_p->coordinates().directionCoordinate(jonesdirectionindex).worldAxisUnits());
    // Set the reference value to the pointing center
    reJonesDirectionCoord.setReferenceValue(pc.getAngle().getValue(reJonesDirectionCoord.worldAxisUnits()(0)));
    // Set the xform for a rotation
    reJonesDirectionCoord.setLinearTransform(xform);
    // Set the increments correctly to the original values, scaled 
    // as needed
    Vector<Double> increments(2);
    increments=reFreqScale*(*incrementsReJones_p);
    increments(0)=-increments(0);
    reJonesDirectionCoord.setIncrement(increments);
    reJonesDirectionCoord.setReferencePixel(*referencePixelReJones_p);

    // Now reset the coordinates of the Real Jones image
    CoordinateSystem reJonesCoords(reJonesImage_p->coordinates());
    reJonesCoords.replaceCoordinate(reJonesDirectionCoord, jonesdirectionindex);
    reJonesImage_p->setCoordinateInfo(reJonesCoords);

    if(0) {
      os << "Munged image of the real part of Jones matrix:" << LogIO::POST;
      ImageSummary<Float> is(*reJonesImage_p);
      is.list(os);
    }

  }

  if(imJonesImage_p) {
    AlwaysAssert(imdirectionindex>=0, AipsError);
    DirectionCoordinate
      imJonesDirectionCoord=coords.directionCoordinate(imdirectionindex);
    // Set the reference value to the pointing center
    imJonesDirectionCoord.setWorldAxisUnits(imJonesImage_p->coordinates().directionCoordinate(jonesdirectionindex).worldAxisUnits());
    imJonesDirectionCoord.setReferenceValue(pc.getAngle().getValue(imJonesDirectionCoord.worldAxisUnits()(0)));
    // Set the xform for a rotation
    imJonesDirectionCoord.setLinearTransform(xform);
    // Set the increments correctly to the original values
    Vector<Double> increments(2);
    increments=imFreqScale*(*incrementsImJones_p);
    increments(0)=-increments(0);
    imJonesDirectionCoord.setIncrement(increments);
    imJonesDirectionCoord.setReferencePixel(*referencePixelImJones_p);
    // Now reset the coordinates of the Imag Jones image
    CoordinateSystem imJonesCoords(imJonesImage_p->coordinates());
    imJonesCoords.replaceCoordinate(imJonesDirectionCoord, jonesdirectionindex);
    imJonesImage_p->setCoordinateInfo(imJonesCoords);
  }

  // Now do the regridding
  IPosition whichAxes(2, 0, 1);
  ImageRegrid<Float> ir;
  ir.regrid(*reRegridJonesImage_p, Interpolate2D::LINEAR, whichAxes,
	    *reJonesImage_p);
  if(imJonesImage_p) {
    ir.regrid(*imRegridJonesImage_p, Interpolate2D::LINEAR, whichAxes,
	      *imJonesImage_p);
  }

  // Check for empty PB
  LatticeExprNode maxRePB=max(*reRegridJonesImage_p);  
  LatticeExprNode maxImPB=max(*reRegridJonesImage_p);  
  if(maxRePB.getFloat()==0.0) {
    throw(AipsError("PBMath2DImage: regridded real Jones image is empty"));
  }
  if(maxImPB.getFloat()==0.0) {
    throw(AipsError("PBMath2DImage: regridded imag Jones image is empty"));
  }

  // For debugging purposes
//   if(0) {
//     PagedImage<Float> reRegridJones(reRegridJonesImage_p->shape(),
// 				    reRegridJonesImage_p->coordinates(),
// 				    "RealRegridJones");
//     reRegridJones.copyData(*reRegridJonesImage_p);

//     if(imJonesImage_p) {
//       PagedImage<Float> imRegridJones(imRegridJonesImage_p->shape(),
// 				      imRegridJonesImage_p->coordinates(),
// 				      "ImagRegridJones");
//       imRegridJones.copyData(*imRegridJonesImage_p);
//     }
//   }
  // Reset the coordinates
  reJonesImage_p->setCoordinateInfo(originalReJonesCoords);
  if(imJonesImage_p) {
    imJonesImage_p->setCoordinateInfo(originalReJonesCoords);
  }
  // Reset the coordinates
  reJonesImage_p->setCoordinateInfo(originalReJonesCoords);
  if(imJonesImage_p) {
    imJonesImage_p->setCoordinateInfo(originalReJonesCoords);
  }
}

void PBMath2DImage::checkJonesCongruent(ImageInterface<Float>& reJones,
					ImageInterface<Float>& imJones) 
{
  LogIO os(LogOrigin("PBMath2DImage", "checkJonesCongruent"));
  if(!reJones.shape().isEqual(imJones.shape(), true)) {
    os << "Real and imaginary primary beam images have different shapes" 
       <<  reJones.shape().asVector()
       <<  imJones.shape().asVector()
       << LogIO::EXCEPTION;
    
  }
  if(reJones.shape().asVector()(2)!=4) {
    os << "Primary beam images must have 4 polarization values: number is " 
       <<  reJones.shape().asVector()(2)
       << LogIO::EXCEPTION;
    
  }
  if (!StokesImageUtil::standardImageCoordinates(reJones)) {
    os << "Primary beam image (real part) does not have standard axes order"
       << LogIO::EXCEPTION;
    
  }
  if (!StokesImageUtil::standardImageCoordinates(imJones)) {
    os << "Primary beam image (imaginary part) does not have standard axes order"
       << LogIO::EXCEPTION;
    
  }
}

void PBMath2DImage::checkImageCongruent(ImageInterface<Float>& image)
{
  LogIO os(LogOrigin("PBMath2DImage", "checkImageCongruent"));
  if (!StokesImageUtil::standardImageCoordinates(image)) {
    os << "Image does not have standard axes order"
       << LogIO::EXCEPTION;
    
  }
}

Int PBMath2DImage::support(const CoordinateSystem& cs){
  
  Int spectralIndex=cs.findCoordinate(Coordinate::SPECTRAL);
  AlwaysAssert(spectralIndex>=0, AipsError);
  SpectralCoordinate
    imageSpectralCoord=cs.spectralCoordinate(spectralIndex);
  Double desiredFrequency=imageSpectralCoord.referenceValue()(0);

  // Next get the frequency of the Jones image
  spectralIndex=reJonesImage_p->coordinates().findCoordinate(Coordinate::SPECTRAL);
  AlwaysAssert(spectralIndex>=0, AipsError);
  imageSpectralCoord=reJonesImage_p->coordinates().spectralCoordinate(spectralIndex);
  Double reFrequency=imageSpectralCoord.referenceValue()(0);

  Double reFreqScale = reFrequency/desiredFrequency;

  Double npixels=reJonesImage_p->shape()(0)*reFreqScale;
  
  Int dirIndex=cs.findCoordinate(Coordinate::DIRECTION);
  DirectionCoordinate
   directionCoord=cs.directionCoordinate(dirIndex);
 
 Vector<String> dirunit=directionCoord.worldAxisUnits();

 npixels=npixels/fabs(directionCoord.increment()(0));
 dirIndex=reJonesImage_p->coordinates().findCoordinate(Coordinate::DIRECTION);

 directionCoord=(reJonesImage_p->coordinates()).directionCoordinate(dirIndex);
 directionCoord.setWorldAxisUnits(dirunit);
 npixels=npixels*fabs(directionCoord.increment()(0));

 return Int(floor(npixels));

}

} //#End casa namespace
