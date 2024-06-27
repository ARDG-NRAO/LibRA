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

#ifndef RESTORE_FUNC_H
#define RESTORE_FUNC_H


#include <synthesis/ImagerObjects/SDAlgorithmBase.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
//#include <casacore/casa/Quanta/Quantum.h>
//#include <casacore/casa/Quanta/QuantumHolder.h>

//#include <synthesis/MeasurementEquations/AspMatrixCleaner.h>

#include <libracore/imageInterface.h>

using namespace casa;
using namespace casacore;
using namespace libracore;


// Convert String to Quantity
std::string stringToQuantity(String instr, Quantity& qa)
{
  
  if ( Quantity::read( qa, instr ) ) { return std::string(""); }
  else  { return std::string("Error in converting " + instr + " to a Quantity \n"); }
}

/*

restoringbeam ({string, stringVec}='') - Restoring beam shape/size to use.
- restoringbeam=’’ or [‘’]
A Gaussian fitted to the PSF main lobe (separately per image plane).
- restoringbeam=’10.0arcsec’
Use a circular Gaussian of this width for all planes
- restoringbeam=[‘8.0arcsec’,’10.0arcsec’]
or 
- restoringbeam=[‘8.0arcsec’,’10.0arcsec’,’45deg’]
Use this elliptical Gaussian for all planes
- restoringbeam=’common’ --> Not implementing here b/c this is a low-level API
Automatically estimate a common beam shape/size appropriate for
all planes.
Note : For any restoring beam different from the native resolution
the model image is convolved with the beam and added to
residuals that have been convolved to the same target resolution.

<imagename>.image: the residual + the model convolved with the clean beam; 
this is the final image (unit: Jy/beam, where beam refers to the clean beam)
*/

template <typename T>
void Restore(col_major_mdspan<T> model, 
  /*col_major_mdspan<T> psf,*/ 
  col_major_mdspan<T> residual,
  /*col_major_mdspan<T> mask,*/
  col_major_mdspan<T> image,
  size_t size_x, size_t size_y, 
  double refi, double refj, double inci, double incj,
  double majaxis, double minaxis, double pa,
  /*int nSubChans = 1, int chanid = 0,*/
  /*std::string majaxis="", std::string minaxis="", std::string pa="",*/
  bool pbcor = false, col_major_mdspan<T> pb = NULL, col_major_mdspan<T> image_pbcor=NULL)
{
  LogIO os( LogOrigin("Restore","Restore", WHERE) );
  
  Matrix<T> dirtyMat(size_x, size_y, 0);
  mdspan2casamatrix<T>(residual, dirtyMat);
  
  
  Matrix<T> modelMat(size_x, size_y, 0);  
  mdspan2casamatrix<T>(model, modelMat);
  

  // perform "restore"
  

  // do below if inputs are string
  /*if (majaxis=="" && minaxis=="" && pa=="")
  {
    // genie work on this in version 2.
    // need to write own FitGaussianPSF that takes Matrix and coordinates.increment (i.e. delta)
    SubImage<Float> subPsf( *psf() , psfslice, True );
    StokesImageUtil::FitGaussianPSF( subPsf, restoringbeam, psfcutoff ); // use STL fitting?
  }*/
  // do not need the below in V1 since the function is mostly used when the 
  // gaussian we are using to restore is the same as the one fitted to the PSF mainline.
  /*else if (majaxis!="" && minaxis=="" && pa=="")
  {
    stringToQuantity(majaxis, majaxis_q);
    restoringbeam.setMajorMinor(majaxis_q, majaxis_q);
  }
  else if (majaxis!="" && minaxis!="" && pa=="")
  {
    stringToQuantity(majaxis, majaxis_q);
    stringToQuantity(minaxis, minaxis_q);
    restoringbeam.setMajorMinor(majaxis_q, minaxis_q);
  }
  else if (majaxis!="" && minaxis!="" && pa!="")
  {
    stringToQuantity(majaxis, majaxis_q);
    stringToQuantity(minaxis, minaxis_q);
    stringToQuantity(pa, pa_q);
    restoringbeam.setMajorMinor(majaxis_q, minaxis_q);
    restoringbeam.setPA(pa_q);
  }*/


  // Before restoring, check for an empty model image and don't convolve (but still smooth residuals)
  bool emptyModel = false;
  
  if(fabs(sum(modelMat)) < 1e-08)
  { 
    os << LogIO::WARN << "Restoring with an empty model image. Only residuals will be processed to form the output restored image." << LogIO::POST;
    emptyModel = true;
  }

  if(max(dirtyMat) == 0.0)  
  {
    os << LogIO::WARN << "Cannot restore without a residual image" << LogIO::POST;
    return;
  }

  // Set restoring beam options
  //imagestore->restore( itsRestoringBeam /* from casa tclean inputs*/), itsUseBeam );
  //restore(GaussianBeam& rbeam /*(i.e. itsRestoringBeam)*/, String& usebeam, uInt term=0, Float psfcutoff=0.35)
  /*GaussianBeam beam;
  SubImage<Float> subPsf( *psf() , psfslice, True );
  StokesImageUtil::FitGaussianPSF( subPsf, beam,psfcutoff );*/

  // no need since we don't have to keep a set.
  //itsRestoredBeams=ImageBeamSet(rbeam);
  //GaussianBeam beam = itsRestoredBeams.getBeam();


  // Make an gaussian PSF matrix
  // no need to normalize
  /*std::vector<float> beam(3);
  beam[0] = majaxis_q.get("arcsec").getValue();
  beam[1] = minaxis_q.get("arcsec").getValue();
  beam[2] = pa_q.get("deg").getValue();*/
  std::vector<double> beam(3);
  beam[0] = majaxis;
  beam[1] = minaxis;
  beam[2] = pa;

  double cospa = cos(beam[2]);
  double sinpa = sin(beam[2]);
  /*std::vector<double> rp = psf.coordinates().referencePixel();
  std::vector<double> d = psf.coordinates().increment();
  double refi = rp(0);
  double refj = rp(1);*/
  AlwaysAssert(beam[0]>0.0,AipsError);
  AlwaysAssert(beam[1]>0.0,AipsError);
  double sbmaj, sbmin;
  sbmaj = 4.0*log(2.0)*square(1.0/beam[0]);
  sbmin = 4.0*log(2.0)*square(1.0/beam[1]);
  //std::vector<double>fd(fabs(d));
  Matrix<T> beamMat(size_x, size_y, 0);

  for (int j = 0; j < size_y; j++) {
    for (int i = 0; i < size_x; i++) {
      double x =   cospa * (double(i)-refi)*inci + sinpa * (double(j)-refj)*incj;
      double y = - sinpa * (double(i)-refi)*inci + cospa * (double(j)-refj)*incj;
      double radius = sbmaj*square(x) + sbmin*square(y);
      if (radius < 20.0) 
        beamMat(i,j) = exp(-radius);
      else 
        beamMat(i,j)=0.;
    }
  }

  // Initialize restored image
  Matrix<T> imageMat(size_x, size_y, 0);
  if( !emptyModel ) { 
     // Copy model into it
     imageMat = modelMat;
     // Smooth model by beam
     //StokesImageUtil::Convolve(imageMat, restoringbeam); // replaced by below
    
    // Make FFT machine
    FFTServer<Float,Complex> fft(IPosition(2, size_x, size_y));
    
    // smooth the model by the beam
    Matrix<Complex> xfr;
    Matrix<Complex> cft;

    fft.fft0(xfr, beamMat);
    
    fft.fft0(cft, imageMat);
    cft *= xfr;
    
    fft.fft0(imageMat, cft, false);
    fft.flip(imageMat, false, false);

  }
  

   // Add residual image Genie need to do in version 2
   // If need to rescale residuals, make a copy and do it. 
    /*if( !rbeam.isNull() || usebeam == "common") 
    {
    //    rescaleResolution(chanid, subResidual, beam, itsPSFBeams.getBeam(chanid, polid));
    TempImage<Float> tmpSubResidualCopy( IPosition(4,nx,ny,1,1), subResidual.coordinates());
    tmpSubResidualCopy.copyData( subResidual );
    
    rescaleResolution(chanid, tmpSubResidualCopy, beam, itsPSFBeams.getBeam(chanid, polid));
      //subRestored.copyData( LatticeExpr<Float>( subRestored + tmpSubResidualCopy  ) );
      imageMat += dirtyMat;
    }
    else// if no need to rescale residuals, just add the residuals.
    {*/
      //subRestored.copyData( LatticeExpr<Float>( subRestored + subResidual  ) );
      imageMat += dirtyMat; 
    //}

    if (pbcor)
    {
      // don't need
      //if( (image(term)->getDefaultMask()).matches("mask0") ) removeMask( image(term) );
      //copyMask(residual(term),image(term));

      if(pb.extent(0) == 0 && pb.extent(1) == 0)
      {
        // Cannot pbcor without pb
        os << LogIO::WARN << "Cannot pbcor without pb" << LogIO::POST;
        return;
      }

      Matrix<T> pbMat(size_x, size_y, 0); 
      mdspan2casamatrix<T>(pb, pbMat);
      
      Matrix<T> image_pbcorMat(size_x, size_y, 0);

      float pbmaxval = max(pbMat);

      if( pbmaxval<=0.0 )
      {
          os << LogIO::WARN << "Skipping PBCOR because pb max is zero " << LogIO::POST;
      }
      else
      {
          //LatticeExpr<Float> thepbcor( iif( *(pbsubim) > 0.0 , (*(restoredsubim))/(*(pbsubim)) , 0.0 ) );
          //pbcorsubim->copyData( thepbcor );

          for (std::size_t j = 0; j < size_y; ++j)
          {  
            for (std::size_t i = 0; i < size_x; ++i)
            {
              if (pbMat(i,j) > 0.0)
                image_pbcorMat(i,j) = imageMat(i,j) / pbMat(i,j);
            }
          }
      }

      // send back STL image_pbcor
      casamatrix2mdspan<T>(image_pbcorMat, image_pbcor);
    }

  // send back STL retored image
  casamatrix2mdspan<T>(imageMat, image);
 
}




template <typename T>
void Restore_psf(col_major_mdspan<T> model, 
  col_major_mdspan<T> psf, 
  col_major_mdspan<T> residual,
  col_major_mdspan<T> image,
  size_t size_x, size_t size_y, 
  double refi, double refj, double inci, double incj,
  /*int nSubChans = 1, int chanid = 0,*/
  bool pbcor = false, col_major_mdspan<T> pb = NULL, col_major_mdspan<T> image_pbcor=NULL,
  double psfcutoff = 0.35)
{
  LogIO os( LogOrigin("Restore","Restore", WHERE) );
  
  Matrix<T> psfMat(size_x, size_y, 0);  
  mdspan2casamatrix<T>(psf, psfMat);

  Matrix<T> dirtyMat(size_x, size_y, 0);
  mdspan2casamatrix<T>(residual, dirtyMat);
    
  Matrix<T> modelMat(size_x, size_y, 0);  
  mdspan2casamatrix<T>(model, modelMat);
  


  // Before restoring, check for an empty model image and don't convolve (but still smooth residuals)
  bool emptyModel = false;
  
  if(fabs(sum(modelMat)) < 1e-08)
  { 
    os << LogIO::WARN << "Restoring with an empty model image. Only residuals will be processed to form the output restored image." << LogIO::POST;
    emptyModel = true;
  }

  if(max(dirtyMat) == 0.0)  
  {
    os << LogIO::WARN << "Cannot restore without a residual image" << LogIO::POST;
    return;
  }

  // Set restoring beam options
  //imagestore->restore( itsRestoringBeam /* from casa tclean inputs*/), itsUseBeam );
  //restore(GaussianBeam& rbeam /*(i.e. itsRestoringBeam)*/, String& usebeam, uInt term=0, Float psfcutoff=0.35)
  /*GaussianBeam beam;
  SubImage<Float> subPsf( *psf() , psfslice, True );
  StokesImageUtil::FitGaussianPSF( subPsf, beam,psfcutoff );*/

  // no need since we don't have to keep a set.
  //itsRestoredBeams=ImageBeamSet(rbeam);
  //GaussianBeam beam = itsRestoredBeams.getBeam();

  vector<double> deltas(2);
  deltas[0] = inci;
  deltas[1] = incj;

  vector<float> beam(3);
  FitGaussianPSFMatrix(psfMat, deltas, beam, psfcutoff); 


  // the following conversion is critical
  double majaxis = (double)beam[0] * C::arcsec;
  double minaxis = (double)beam[1] * C::arcsec;
  double pa = ((double)beam[2] + 90.0) * C::degree;
  /*cout << "majaxis " << majaxis << endl;
  cout << "arcsec " << C::arcsec << endl;
  cout << "degree " << C::degree << endl;*/


  Restore(model, residual, image,
  size_x, size_y, 
  refi, refj, inci, incj,
  majaxis, minaxis, pa,
  pbcor, pb, image_pbcor);

}


#endif
