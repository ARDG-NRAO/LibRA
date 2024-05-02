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

#ifndef ASPMDSPAN_FUNC_H
#define ASPMDSPAN_FUNC_H


#include <synthesis/ImagerObjects/SDAlgorithmBase.h>
#include <synthesis/MeasurementEquations/AspMatrixCleaner.h>

#include <libracore/imageInterface.h>

using namespace casa;
using namespace casacore;
using namespace libracore;


template <typename T>
void Asp_mdspan(col_major_mdspan<T> model, 
  col_major_mdspan<T> psf, 
  col_major_mdspan<T> residual,
  col_major_mdspan<T> mask,
  size_t size_x, size_t size_y, 
  float& psfwidth,
  float& largestscale, float& fusedthreshold,
  int& nterms,
  float& gain, 
  float& threshold, float& nsigmathreshold,
  float& nsigma,
  int& cycleniter, float& cyclefactor,
  std::string& specmode,
  int nSubChans = 1, int chanid = 0)
{
  LogIO os( LogOrigin("Asp_mdspan","Asp_mdspan", WHERE) );
  AspMatrixCleaner itsCleaner;

  
  Matrix<T> psfMat(size_x, size_y, 0);
  /*for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      psfMat(i,j) = psf[i][j];
    }
  }*/
  mdspan2casamatrix<T>(psf, psfMat);
  itsCleaner.setPsf(psfMat);
  

  itsCleaner.setPsfWidth(psfwidth);

  //if user does not provide the largest scale, we calculate it internally.
  itsCleaner.setUserLargestScale(largestscale);

  itsCleaner.setInitScaleXfrs(psfwidth);

  itsCleaner.stopPointMode(-1);
  itsCleaner.ignoreCenterBox(true); // Clean full image
  

  if (fusedthreshold < 0)
  {
    os << LogIO::WARN << "Acceptable fusedthreshld values are >= 0. Changing fusedthreshold from " << fusedthreshold << " to 0." << LogIO::POST;
    fusedthreshold = 0.0;
  }

  itsCleaner.setFusedThreshold(fusedthreshold);
  
   
  Matrix<T> maskMat(size_x, size_y, 0);
  /*for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      maskMat(i,j) = mask[i][j];
    }
  }*/
  mdspan2casamatrix<T>(mask, maskMat);

  itsCleaner.setInitScaleMasks(maskMat);  //Array<Float> itsMatMask; 
  itsCleaner.setaspcontrol(0, 0, 0, Quantity(0.0, "%"));// Needs to come before the rest


  Matrix<T> dirtyMat(size_x, size_y, 0);
  /*for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      dirtyMat(i,j) = residual[i][j];
    }
  }*/
  mdspan2casamatrix<T>(residual, dirtyMat);
  itsCleaner.setDirty(dirtyMat);

  // InitScaleXfrs and InitScaleMasks should already be set
  vector<Float> itsScaleSizes;
  itsScaleSizes.clear();
  itsScaleSizes = itsCleaner.getActiveSetAspen();
  itsScaleSizes.push_back(0.0); // put 0 scale
  itsCleaner.defineAspScales(itsScaleSizes);


  // takeOneStep
  Quantity thresh(threshold, "Jy");
  itsCleaner.setaspcontrol(cycleniter, gain, thresh, Quantity(0.0, "%"));
   
  
  Matrix<T> modelMat(size_x, size_y, 0);
  /*for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      modelMat(i,j) = model[i][j];
    }
  }*/
  mdspan2casamatrix<T>(model, modelMat);
  

  // get initial peak residual
  T masksum = sum(maskMat);
  bool validMask = ( masksum > 0 );

  T startpeakresidual;
  Matrix<T> maskedDirty(size_x, size_y, 0);

  if (validMask)
  {
    for (int j = 0; j < size_y; j++)
    {
      for (int i = 0; i < size_x; i++)
      {
        if(maskMat(i,j) > 0.99)
          maskedDirty(i,j) = abs(dirtyMat(i,j));
        else
          maskedDirty(i,j) = 0;
      }
    }
    startpeakresidual = max(maskedDirty); 
  }
  else
  {
    startpeakresidual = max(abs(dirtyMat));
  }

  //cout << "startpeakresidual " << startpeakresidual << ", masksum " << masksum << endl;


  // retval
  //  1 = converged
  //  0 = not converged but behaving normally
  // -1 = not converged and stopped on cleaning consecutive smallest scale
  // -2 = not converged and either large scale hit negative or diverging
  // -3 = clean is diverging rather than converging
  itsCleaner.startingIteration( 0 );
  int  retval = itsCleaner.aspclean( modelMat );
  int iterdone = itsCleaner.numberIterations();

  if( retval==-1 ) {os << LogIO::WARN << "AspClean minor cycle stopped on cleaning consecutive smallest scale" << LogIO::POST; }
  if( retval==-2 ) {os << LogIO::WARN << "AspClean minor cycle stopped at large scale negative or diverging" << LogIO::POST;}
  if( retval==-3 ) {os << LogIO::WARN << "AspClean minor cycle stopped because it is diverging" << LogIO::POST; }

  // update residual - this is critical
  float peakresidual = itsCleaner.getterPeakResidual();
  float modelflux = sum( modelMat );

  // send back mdspan residual
  dirtyMat = itsCleaner.getterResidual();
  /*for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      residual[i][j] = dirtyMat(i,j);
    }
  }*/
  casamatrix2mdspan<T>(dirtyMat, residual);

  // send back STL model
  casamatrix2mdspan<T>(modelMat, model);
  /*for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      model[i][j] = modelMat(i,j);
    }
  }*/

  os << LogIO::NORMAL1  << "Asp: After one step, residual=" << peakresidual << " model=" << modelflux << " iters=" << iterdone << LogIO::POST;

  int stopCode = 0;

  // Reached cycleniter
  if (iterdone >= cycleniter ) 
    stopCode=1;
  
  // Reached cyclethreshold
  if (threshold >= nsigmathreshold) 
  {
    if(fabs(peakresidual) <= threshold) 
      stopCode=2;
  }
  // Reached n-sigma threshold
  else 
  {
    if(fabs(peakresidual) <= nsigmathreshold && !(iterdone <= 0)) 
    { 
      if (nsigma!=0.0) 
        stopCode=6; 
    }
  }

  // Zero iterations done
  if(iterdone == 0) 
    stopCode=3;

  // Diverged
  if(iterdone > 0 &&
      (fabs(startpeakresidual) > 0.0 ) &&
      (fabs(peakresidual) - fabs(startpeakresidual) )/ fabs(startpeakresidual) > 0.1 )
    {stopCode=4;}


  if(stopCode==0 && iterdone != cycleniter)
  {
    os << LogIO::NORMAL1 << "Exited minor cycle without satisfying stopping criteria " << LogIO::POST;
    stopCode=5;
  }

  os << "[";// << imagestore->getName();
  if(nSubChans>1) os << "C" << chanid ;
  //if(nSubPols>1) os << ":P" << polid ;
  //Int iterend = loopcontrols.getIterDone();
  os << "]"
     <<" iters=0->" << iterdone << " [" << iterdone << "]"
     << ", model=0->" << modelflux
     << ", peakres=" << startpeakresidual << "->" << peakresidual ;

  switch (stopCode)
    {
    case 0:
      os << ", Skipped this plane. Zero mask.";
      break;
    case 1:
      os << ", Reached cycleniter.";
      break;
    case 2:
      os << ", Reached cyclethreshold.";
      break;
    case 3:
      os << ", Zero iterations performed.";
      break;
    case 4:
      os << ", Possible divergence. Peak residual increased by 10% from minimum.";
      break;
    case 5:
      os << ", Exited Asp minor cycle without reaching any stopping criterion.";
      break;
    case 6:
      os << ", Reached n-sigma threshold.";
    default:
      break;
    }

     os << LogIO::POST;
}


#endif
