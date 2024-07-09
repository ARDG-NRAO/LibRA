// Copyright 2021 Associated Universities, Inc. Washington DC, USA.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IMAGEINTERFACE_FUNC_H
#define IMAGEINTERFACE_FUNC_H

#include <iostream>

#include <mdspan.hpp>
#include <casacore/casa/namespace.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Arrays/Matrix.h>

#include <casacore/scimath/Fitting/NonLinearFitLM.h>
#include <casacore/scimath/Functionals/Gaussian2D.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>

namespace libracore {

using extents_type = Kokkos::dextents<size_t, 2>;

template <typename T>
using row_major_mdspan = Kokkos::mdspan<T, extents_type, Kokkos::layout_right>;
// p.s. casa matrix is col major
template <typename T>
using col_major_mdspan = Kokkos::mdspan<T, extents_type, Kokkos::layout_left>;


template <typename T>
int mdspan2casamatrix(
  libracore::col_major_mdspan<T> md,
  casacore::Matrix<T>& matrix)
{
    size_t nx = md.extent(0);
    size_t ny = md.extent(1);

    if (size_t(matrix.shape()(0)) != nx || size_t(matrix.shape()(1)) != ny)
    {
        std::cerr << "Sizes of mdspan and casa matrix do not match.\n";
        return EXIT_FAILURE;
    }

    
    for (std::size_t j = 0; j < ny; ++j)
        for (std::size_t i = 0; i < nx; ++i)
        #if MDSPAN_USE_BRACKET_OPERATOR
            matrix(i,j) = md[i, j];
        #else
            matrix(i,j) = md(i, j);
        #endif

    return(0);
}

template <typename T>
int casamatrix2mdspan(casacore::Matrix<T>& matrix,
    libracore::col_major_mdspan<T> md)
{
    const size_t nx = size_t(matrix.shape()(0));
    const size_t ny = size_t(matrix.shape()(1));

    if (md.extent(0) != nx || md.extent(1) != ny)
    {
        std::cerr << "Sizes of mdspan and casa matrix do not match.\n";
        return EXIT_FAILURE;
    }

    for (std::size_t j = 0; j < ny; ++j)
        for (std::size_t i = 0; i < nx; ++i)
        #if MDSPAN_USE_BRACKET_OPERATOR
            md[i, j] = matrix(i, j);
        #else
            md(i, j) = matrix(i, j);
        #endif

    return(0);

}


inline void locatePeakPSFMatrix(Matrix<float>& lpsf, int& xpos, int& ypos, float& amp){
    amp=0; 
    xpos=0;
    ypos=0;
    float psfmax;
    float psfmin;
    IPosition psfposmax(lpsf.ndim());
    IPosition psfposmin(lpsf.ndim());   

    minMax(psfmin, psfmax, psfposmin, psfposmax, lpsf); 
    xpos=psfposmax(0);
    ypos=psfposmax(1);

    amp=lpsf(xpos,ypos);

}


inline bool FitGaussianPSFMatrix(Matrix<float>& psf, vector<double>& deltas_v, vector<float>& beam, float psfcutoff=0.35) 
{  
  LogIO os(LogOrigin("libracore", "FitGaussianPSFMatrix()",WHERE));
  os << LogIO::DEBUG1 << "Psfcutoff is  " << psfcutoff << LogIO::POST;
    
  Float npix = 20;
  Int expand_pixel = 5;
  Int target_npoints = 3001;
  String InterpMethod = "CUBIC";

  //##########################################################
  
  Vector<Double> deltas(2, 0.0);
  deltas(0)= deltas_v[0];
  deltas(1)= deltas_v[1];

  
  int nx = psf.shape()(0);
  int ny = psf.shape()(1);
  
  int px=0;
  int py=0;
  float bamp=0;
  
  locatePeakPSFMatrix(psf, px, py, bamp);

  //check if peak is ZERO
  if( bamp < 1e-07 ) {
    throw(AipsError("Psf peak is zero"));
  }

  //check if peak is outside inner quarter
  if(px < nx/4.0 || px > 3.0*nx/4.0 || py < ny/4.0 || py > 3.0*ny/4.0) {    
    throw(AipsError("Peak of psf is outside the inner quarter of defined image"));
  }

 
  if((bamp > 1.1) || (bamp < 0.9)) // No warning if 10% error in PSF peak
    os << LogIO::WARN << "Peak of PSF is " << bamp << LogIO::POST;

  static const Float fdiam = 2.5*abs(deltas(0))/C::arcsec;

    try{  

      if(bamp == 0.0) {
          beam[0] = fdiam;
          beam[1] = fdiam;
          beam[2] = 0.0;
        os << LogIO::WARN << "Could not find peak " << LogIO::POST;
        return false;
      }
        
      psf/=bamp; //Normalize

      // The selection code attempts to avoid including any sidelobes, even
      // if they exceed the threshold, by starting from the center column and
      // working out, exiting the loop when it crosses the threshold.  It
      // assumes that the first time it finds a "good" point starting from
      // the center and working out that it's in the main lobe.  Narrow,
      // sharply ringing beams inclined at 45 degrees will confuse it, but 
      // that's even more pathological than most VLBI beams.
      
      float amin = psfcutoff;
      int nrow = npix;
      
      //we sample the central part of a, 2*nrow+1 by 2*nrow+1
      Bool converg = false;
      Vector<Double> solution;
      Int kounter = 0;
        
     while(amin > 0.009 && !converg && (kounter < 50))
     {
          kounter+=1;
          Int npoints=0;
          Vector<Double> y, sigma;
          Matrix<Double> x;
          IPosition blc(2), trc(2);
             
          casa::StokesImageUtil::FindNpoints(npoints, blc, trc, nrow, amin, px, py, deltas, x , y, sigma, psf); 
          os << LogIO::DEBUG1 << "First FindNpoints is " << npoints << LogIO::POST;
             
          blc = blc - expand_pixel;
          trc = trc + expand_pixel;
             
          if(blc(0) < 0) blc(0)=0;
          if(blc(1) < 0) blc(1)=0;
          if(trc(0) >= psf.shape()(0)) trc(0) = psf.shape()(0)-1;
          if(trc(1) >= psf.shape()(1)) trc(1) = psf.shape()(1)-1;
             
          Matrix<Float> lpsfWindowed = psf(blc,trc);
          os << LogIO::DEBUG1 << "The windowed Psf shape is " << lpsfWindowed.shape() << LogIO::POST;
          Matrix<Float> resampledPsf;
             
          Int oversampling = (Int) sqrt(target_npoints/(lpsfWindowed.shape()(0) * lpsfWindowed.shape()(1)));
           if(oversampling == 0)
                 oversampling = 1;
           
           os << LogIO::DEBUG1 << "The oversampling is " << oversampling << LogIO::POST;
             
          casa::StokesImageUtil::ResamplePSF(lpsfWindowed, oversampling, resampledPsf,InterpMethod); 
          os << LogIO::DEBUG1 << "The resampled windowed Psf shape is " << lpsfWindowed.shape() << LogIO::POST;
             
          Float minVal, maxVal;
          IPosition minPos(2);
          IPosition maxPos(2);
          minMax(minVal, maxVal, minPos, maxPos, resampledPsf);
          resampledPsf = resampledPsf/maxVal;
            
          Vector<Double> resampledDeltas = deltas/ (Double) oversampling;
             
          Int minLen;
          if((trc(0) - blc(0)) > (trc(1) - blc(1)))
            minLen = trc(1) - blc(1) + 1;
          else
            minLen = trc(0) - blc(0) + 1;
          
             
          Int nrowRe = (Int) (oversampling*minLen - 1)/2;
          casa::StokesImageUtil::FindNpoints(npoints, blc, trc, nrowRe, amin,  maxPos(0), maxPos(1), resampledDeltas, x , y, sigma, resampledPsf);
          os << LogIO::DEBUG1 << "Second FindNpoints is " << npoints << LogIO::POST;

          Gaussian2D<AutoDiff<Double> > gauss2d;
          gauss2d[0] = 1.0; //Height of Gaussian
          gauss2d[1] = 0.0; //The center of the Gaussian in the x direction
          gauss2d[2] = 0.0; //The center of the Gaussian in the y direction.
          //gauss2d[3] = 2.5*abs(resampledDeltas(0)); //The width (FWHM) of the Gaussian on one axis.
          gauss2d[3] = 2.5*abs(deltas(0));
          gauss2d[4] = 0.5; //A modified axial ratio.
          
          // Fix height and center
          gauss2d.mask(0) = false;
          gauss2d.mask(1) = false;
          gauss2d.mask(2) = false;
          
          // CAS-13515: Fitting sometimes fails with "NonLinearFitLM: error in loop solution". This occurs in casacore/scimath/Fitting/NonLinearFitLM.tcc LSQFit::invertRect() due to a matrix that can not be inverted.
          // This problem is solved by retrying fit with a different position angle.
          Bool loopSolutionFound=false;
          Int retryCounter = 0;
          Double posAng = 1.0; //Initial position angle.
          Int nRetries = 10;
          while(!loopSolutionFound && retryCounter < nRetries)
          {
              //Create casacore fitter and fit Gaussian to subset of PSF interpolated values.
              gauss2d[5] = posAng; //The position angle.
              NonLinearFitLM<Double> fitter;
              // Set maximum number of iterations to 1000
              fitter.setMaxIter(1000);
              
              // Set converge criteria.  Default is 0.001
              fitter.setCriteria(0.0001);
              
              // Set the function and initial values
              fitter.setFunction(gauss2d);
              
              try{
                  // The current parameter values are used as the initial guess.
                  solution = fitter.fit(x, y, sigma);
                  loopSolutionFound = true;
              }catch(AipsError &x_error){
                  loopSolutionFound = false;
                  retryCounter++;
                  posAng = 1.0 + retryCounter*M_PI/nRetries;
                  os << LogIO::NORMAL3 << "Fit failed, another atempt will be made with position angle  " << posAng  << " rad." << LogIO::POST;
              }
              converg=fitter.converged();
          }
             
          if(!loopSolutionFound)
          {
            throw(AipsError(String("Error in libracore::FitGaussianPSFMatrix: error in loop solution.")));
          }

          if (!converg) 
          {
            beam[0]=2.5*abs(deltas(0))/C::arcsec;
            beam[1]=2.5*abs(deltas(0))/C::arcsec;
            beam[2]=0.0;

            //fit did not coverge...reduce the minimum i.e expand the field a bit
            amin/=1.5;
            
            os << LogIO::WARN << "The fit did not coverge; another atempt will be made with psfcutoff " << amin << LogIO::POST;
          }
      
     }
     if (converg) 
     {
       if (abs(solution(4))>1.0) 
       {
         beam[0]=abs(solution(3)*solution(4))/C::arcsec;
         beam[1]=abs(solution(3))/C::arcsec;
         beam[2]=solution(5)/C::degree-90.0;
       } 
       else 
       {
         beam[0]=abs(solution(3))/C::arcsec;
         beam[1]=abs(solution(3)*solution(4))/C::arcsec;
         beam[2]=solution(5)/C::degree;
       }
       
       beam[2]=fmod(beam[2], Float(360.0));
       
       while (abs(beam[2]/90.0)> 1) 
       {
         if (beam[2] > 270.0) beam[2]-=360.0;
         else if (beam[2] > 90.0) beam[2] -=180.0;
         else if (beam[2] < -270.0) beam[2] +=360.0;
         else beam[2]+=180.0;
       }
         
       return true;
     }
     else os << LogIO::WARN << "The fit did not coverge; check your PSF" <<
        LogIO::POST; 
     return false;

    } catch (AipsError &x_error) {
         beam[0] = fdiam;
         beam[1] = fdiam;
         beam[2] = 0.0;
         os << LogIO::SEVERE << "Caught Exception: "<< x_error.getMesg() <<
          LogIO::POST;
        return false;
   } 

}


void testing();

} // end namespace 

#endif
