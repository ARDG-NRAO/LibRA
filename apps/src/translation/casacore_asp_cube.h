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

#ifndef CASACORE_ASP_CUBE_FUNC_H
#define CASACORE_ASP_CUBE_FUNC_H

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
#include <imageanalysis/Utilities/SpectralImageUtil.h>

#include <Asp/asp.h>


using namespace casa;
using namespace casacore;

void makeTempImage(std::shared_ptr<ImageInterface<Float> >& outptr,  const String& imagename, const Int chanBeg, const Int chanEnd, const Bool writelock=false){
    //For testing that locks are placed in the right places use userlocking
    PagedImage<Float> im(imagename, writelock ? TableLock::UserLocking : TableLock::UserNoReadLocking);
    
    SubImage<Float> *tmpptr=nullptr;
    

    ///END of TESTOO
    if(writelock){
      im.lock(FileLocker::Write, 1000);
      tmpptr=SpectralImageUtil::getChannel(im, chanBeg, chanEnd, false);
    }
    else
      tmpptr=SpectralImageUtil::getChannel(im, chanBeg, chanEnd, false);
    if(tmpptr){
      IPosition tileshape=tmpptr->shape();
      tileshape[2]=1; tileshape[3]=1;
      TiledShape tshape(tmpptr->shape(),tileshape);
   
      outptr.reset(new TempImage<Float>(tshape, tmpptr->coordinates()));
        
      
      if(writelock){
        LatticeLocker lock1 (*(tmpptr), FileLocker::Write);
        outptr->copyData(*tmpptr);
        //cerr << "IMAGENAME " << imagename << " masked " << im.isMasked() << " tmptr  " << tmpptr->isMasked() << endl;
        if(tmpptr->isMasked()){
          outptr->makeMask ("mask0", true, true, false, true);
          outptr->pixelMask().put(tmpptr->getMask());
          //      cerr << "tempimage SUM of bit mask" << ntrue(tmpptr->pixelMask().get()) << " out   " << ntrue(outptr->pixelMask().get()) << endl;
        }
      }
      else{
        LatticeLocker lock1 (*(tmpptr), FileLocker::Read);
        outptr->copyData(*tmpptr);
        //cerr << "false IMAGENAME " << imagename << " masked " << im.isMasked() << " tmptr  " << tmpptr->isMasked() << endl;
        
        if(tmpptr->isMasked()){
          outptr->makeMask ("mask0", true, true, false, true);
          outptr->pixelMask().put(tmpptr->getMask());
          //cerr << "tempimage SUM of bit mask" << ntrue(tmpptr->pixelMask().get()) << " out   " << ntrue(outptr->pixelMask().get()) << endl;
        }
      }
      ImageInfo iinfo=tmpptr->imageInfo();
      outptr->setImageInfo(iinfo);
      delete tmpptr;
    }
    
    im.unlock();
  }

 void writeBackToFullImage(const String imagename, const Int chanBeg, const Int chanEnd, std::shared_ptr<ImageInterface<Float> > subimptr){
   PagedImage<Float> im(imagename, TableLock::UserLocking);
   //PagedImage<Float> im(imagename, TableLock::AutoLocking);
    SubImage<Float> *tmpptr=nullptr; 
    tmpptr=SpectralImageUtil::getChannel(im, chanBeg, chanEnd, true);
    {
    
      LatticeLocker lock1 (*(tmpptr), FileLocker::Write);
      tmpptr->copyData(*(subimptr));
    }        
    im.unlock();
    delete tmpptr;
                 
  }



void casacore_asp_cube(std::string& imageName, std::string& modelImageName,
        float& largestscale, float& fusedthreshold,
        int& nterms,
        float& gain, float& threshold,
        float& nsigma,
        int& cycleniter, float& cyclefactor,
        std::string& specmode)
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
      //std::cout << "residual shape " << itsImages->residual()->shape() << std::endl; // 512,512,1,5

    int numchan = itsImages->residual()->shape()[3];

    // converting Matrix to STL 
    casacore::Array<casacore::Float> itsMatPsf, itsMatResidual, itsMatModel;
    casacore::Array<casacore::Float> itsMatMask;
    itsImages->residual()->get( itsMatResidual, false );
    itsImages->model()->get( itsMatModel, false );
    itsImages->psf()->get( itsMatPsf, false );
    itsImages->mask()->get( itsMatMask, false );
    //std::cout << "its psf shape " << itsMatPsf.shape() << endl; //[512, 512, 1, 5]
    //std::cout << "its mask shape " << itsMatMask.shape() << " max " << max(itsMatMask) << endl;
    
    const int nx = itsMatPsf.shape()(0);
    const int ny = itsMatPsf.shape()(1);
    
    
    AspMatrixCleaner itsCleaner;

    for (int chanid = 0; chanid < numchan; chanid++)
    {
        vector<vector<float>> model(nx, vector<float> (ny, 0));
        vector<vector<float>> psf(nx, vector<float> (ny, 0));
        vector<vector<float>> residual(nx, vector<float> (ny, 0));
        vector<vector<float>> mask(nx, vector<float> (ny, 1));
        float maxPsf = 0;

        for (int j = 0; j < ny; j++)
        {
          for (int i = 0; i < nx; i++)
          {
            model[i][j] = itsMatModel(IPosition(4, i, j, 0, chanid));
            mask[i][j] = itsMatMask(IPosition(4, i, j, 0, chanid));
            psf[i][j] = itsMatPsf(IPosition(4, i, j, 0, chanid));
            residual[i][j] = itsMatResidual(IPosition(4, i, j, 0, chanid));
            
            if (psf[i][j] > maxPsf) // for verifying psf later
              maxPsf = psf[i][j];
          }
        }
        
        int chanBeg = chanid;
        int chanEnd = chanid;
        std::shared_ptr<ImageInterface<Float> >subpsf=nullptr;
        makeTempImage(subpsf, imageName+".psf", chanBeg, chanEnd);
        //std::cout << "subpsf shape " << subpsf->shape() << " maxPsf " << maxPsf << endl;
        float psfwidth = 0;
        if (maxPsf == 1) //valid psf
            psfwidth = itsCleaner.getPsfGaussianWidth(*subpsf);

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
                   specmode,
                   numchan, chanid
                   );

        //////////Write back to files ////////////

        for (int j = 0; j < ny; j++)
        {
          for (int i = 0; i < nx; i++)
          {
            itsMatModel(IPosition(4, i, j, 0, chanid)) = model[i][j];
            itsMatResidual(IPosition(4, i, j, 0, chanid)) = residual[i][j];     
          }
        }
        
     } // end for all channels

     (itsImages->residual())->put( itsMatResidual );
     (itsImages->model())->put( itsMatModel );

      itsImages->releaseLocks();


}

#endif