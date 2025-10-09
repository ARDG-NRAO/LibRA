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

#ifndef CASACORE_ASP_FUNC_H
#define CASACORE_ASP_FUNC_H

#include <casacore/casa/namespace.h>
#include <casacore/casa/Logging/LogIO.h>

#include <casacore/casa/aips.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/measures/Measures/MDirection.h>

#include <synthesis/ImagerObjects/SDAlgorithmBase.h>
#include <synthesis/ImagerObjects/SDAlgorithmHogbomClean.h>
#include <synthesis/ImagerObjects/SDAlgorithmClarkClean.h>
#include <synthesis/ImagerObjects/SDAlgorithmClarkClean2.h>
#include <synthesis/ImagerObjects/SDAlgorithmMSMFS.h>
#include <synthesis/ImagerObjects/SDAlgorithmMSClean.h>
#include <synthesis/ImagerObjects/SDAlgorithmMEM.h>
#include <synthesis/ImagerObjects/SDAlgorithmAAspClean.h>

#include <synthesis/ImagerObjects/SDMaskHandler.h>
#include <synthesis/ImagerObjects/SIMinorCycleController.h>

#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>

#include <synthesis/ImagerObjects/SynthesisDeconvolver.h>
#include <synthesis/ImagerObjects/grpcInteractiveClean.h>

#include <Asp/asp.h>


using namespace casa;
using namespace casacore;

void casacore_asp(std::string& imageName, std::string& modelImageName,
        float& largestscale, float& fusedthreshold,
        int& nterms,
        float& gain, float& threshold,
        float& nsigma,
        int& cycleniter, float& cyclefactor,
        /*vector<string>& mask,*/ std::string& specmode)
{

      SynthesisParamsDeconv decPars_p;
      decPars_p.setDefaults();

      //deconvolution params
      decPars_p.imageName=imageName;
      decPars_p.algorithm="asp";
      decPars_p.startModel=modelImageName;
      decPars_p.deconvolverId=0;
      decPars_p.scales = Vector<Float>(0);
      decPars_p.maskType="user"; //genie
      
      
      //if (mask.size() == 0)
      //{
        decPars_p.maskString = "";
        decPars_p.maskList.resize(1);
        decPars_p.maskList[0] = "";
      //}
      /*else if(mask.size() == 1)
      {
        decPars_p.maskString = mask[0];
        decPars_p.maskList.resize(1);
        decPars_p.maskList[0] = mask[0];
      }
      else
      {
        decPars_p.maskString = mask[0];
        decPars_p.maskList = Vector<String>(mask);
      } */       

      decPars_p.pbMask=0.0; //genie
      decPars_p.autoMaskAlgorithm="thresh";
      decPars_p.maskThreshold=""; //genie
      decPars_p.maskResolution=""; //genie
      decPars_p.fracOfPeak=0.0; //genie
      decPars_p.nMask=0; //genie
      decPars_p.interactive=false;
      decPars_p.autoAdjust=False; //genie
      decPars_p.fusedThreshold = fusedthreshold;
      decPars_p.specmode=specmode; //deconvolve task does not have this
      decPars_p.largestscale = largestscale;
      decPars_p.scalebias = 0.0;
      decPars_p.nTaylorTerms = nterms;
      decPars_p.nsigma = nsigma; // default is 0 which indicates no PB is required

      SynthesisDeconvolver itsDeconvolver;
      itsDeconvolver.setupDeconvolution(decPars_p);
      

      std::shared_ptr<SIImageStore> itsImages;
      itsImages.reset( new SIImageStore( imageName, true, true) ); 

      float MaxPsfSidelobe =  itsImages->getPSFSidelobeLevel();

      //itsDeconvolver.initMinorCycle(); 
      //itsDeconvolver.setupMask();

      Float masksum;
      if( ! itsImages->hasMask() ) // i.e. if there is no existing mask to re-use...
      { masksum = -1.0; }
      else
      { 
        masksum = itsImages->getMaskSum();
        itsImages->mask()->unlock();
      }
      Bool validMask = ( masksum > 0 );
      Float PeakResidual= validMask ? itsImages->getPeakResidualWithinMask() : itsImages->getPeakResidual();

      //std::cout << "masksum " << masksum << std::endl; 

      float minpsffraction = 0.05;
      float maxpsffraction = 0.8;
      float CycleFactor = 1.0;
      
      Float psffraction = MaxPsfSidelobe * CycleFactor;
      psffraction = std::max(psffraction, minpsffraction);
      psffraction = std::min(psffraction, maxpsffraction);
      Float cyclethreshold = PeakResidual * psffraction;
      threshold = std::max(threshold, cyclethreshold);

      //std::cout << "MaxPsfSidelobe " << MaxPsfSidelobe << " threshold " << threshold << endl;

      AspMatrixCleaner itsCleaner;
      float psfwidth = itsCleaner.getPsfGaussianWidth(*(itsImages->psf()));
      //std::cout << "psfwidth " << psfwidth << std::endl;

      float nsigmathreshold = 0;
      if (nsigma > 0)
      {
        bool fastnoise = true;
        Array<Double> medians, robustrms;
        robustrms = itsImages->calcRobustRMS(medians, 0.0, fastnoise);
        Double minval, maxval;
        IPosition minpos, maxpos;
        minMax(minval, maxval, minpos, maxpos, robustrms);

        bool useautomask = false;
      
        if (useautomask) {
          nsigmathreshold = (float)(medians(maxpos)) + nsigma * (float)maxval;
        }
        else {
          nsigmathreshold = nsigma * (float)maxval;
        }
      }
      //std::cout << "nsigmathreshold " << nsigmathreshold << std::endl;

    if(itsImages->residual()->shape()[3]> 1)
    {
    }

      // converting Matrix to STL 
      casacore::Array<casacore::Float> itsMatPsf, itsMatResidual, itsMatModel;
      casacore::Array<casacore::Float> itsMatMask;
    itsImages->residual()->get( itsMatResidual, true );
    itsImages->model()->get( itsMatModel, true );
    itsImages->psf()->get( itsMatPsf, true );
    itsImages->mask()->get( itsMatMask, true );
    //std::cout << "its shape " << itsMatPsf.shape() << endl;
    
    Matrix<Float> matPsf(itsMatPsf);
    Matrix<Float> matModel(itsMatModel);
    Matrix<Float> matResidual(itsMatResidual);
    Matrix<Float> matMask(itsMatMask);

    AlwaysAssert(matPsf.shape()==matModel.shape(), AipsError);
    AlwaysAssert(matPsf.shape()==matResidual.shape(), AipsError);
    AlwaysAssert(matPsf.shape()==matMask.shape(), AipsError);

    const size_t nx = (size_t)matPsf.shape()(0);
    const size_t ny = (size_t)matPsf.shape()(1);
    //std::cout << "nx = " << nx << " ny " << ny << std::endl;
    
    vector<vector<float>> model(nx, vector<float> (ny, 0));
    vector<vector<float>> psf(nx, vector<float> (ny, 0));
    vector<vector<float>> residual(nx, vector<float> (ny, 0));
    vector<vector<float>> mask(nx, vector<float> (ny, 1));
    
    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        model[i][j] = matModel(i,j);
        mask[i][j] = matMask(i,j);
        psf[i][j] = matPsf(i,j);
        residual[i][j] = matResidual(i,j);
        
      }
    }
    

//////////interface to the Raw Asp layer////////

    Asp<float>(model, psf, residual,
               mask,
               nx, ny,
               psfwidth,
               largestscale, fusedthreshold,
               nterms,
               gain, 
               threshold, nsigmathreshold,
               nsigma,
               cycleniter, cyclefactor,
               specmode
               );

  //////////Write back to files ////////////

    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        matModel(i,j) = model[i][j];
        matMask(i,j) = mask[i][j];
        matPsf(i,j) = psf[i][j];
        matResidual(i,j) = residual[i][j];     
      }
    }
    
    (itsImages->residual())->put( matResidual );
    (itsImages->model())->put( matModel );

    itsImages->releaseLocks();


}

#endif