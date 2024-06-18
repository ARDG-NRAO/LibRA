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

#ifndef CASACORE_ASP_MDSPAN_FUNC_H
#define CASACORE_RESTORE_FUNC_H

#include <casacore/casa/namespace.h>
#include <casacore/casa/Logging/LogIO.h>

#include <casacore/casa/aips.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/measures/Measures/MDirection.h>

/*#include <synthesis/ImagerObjects/SDAlgorithmBase.h>
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
#include <synthesis/ImagerObjects/grpcInteractiveClean.h>*/

#include <Restore/restore.h>


using namespace casa;
using namespace casacore;
using namespace libracore;

// the API assumes mask is provided as a disk image
// it also assumes the input model and residual images are of the same name,
void casacore_restore(std::string& imageName, bool& doPBCorr)
{

      /*SynthesisParamsDeconv decPars_p;
      decPars_p.setDefaults();

      //deconvolution params
      decPars_p.imageName=imageName;
      decPars_p.algorithm="asp";
      decPars_p.startModel=modelImageName;
      decPars_p.deconvolverId=0;
      decPars_p.scales = Vector<Float>(0);
      decPars_p.maskType="user"; //genie
      
      

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
      decPars_p.nsigma = nsigma;*/ // default is 0 which indicates no PB is required

    

      std::shared_ptr<SIImageStore> itsImages;
      itsImages.reset( new SIImageStore( imageName, true, true) ); 

      float MaxPsfSidelobe =  itsImages->getPSFSidelobeLevel();

    

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
   
    
    // convert casa matrix to mdspan
    //float emptydata[nx * ny]; // this causes stack overflow 
    // need to initialize array like the following
    auto emptydata1 = std::make_unique<float[]>(nx * ny);
    auto psf = col_major_mdspan<float>(emptydata1.get(), nx, ny);
    auto emptydata2 = std::make_unique<float[]>(nx * ny);
    auto model = col_major_mdspan<float>(emptydata2.get(), nx, ny);
    auto emptydata3 = std::make_unique<float[]>(nx * ny);
    auto residual = col_major_mdspan<float>(emptydata3.get(), nx, ny);
    auto emptydata4 = std::make_unique<float[]>(nx * ny);
    auto mask = col_major_mdspan<float>(emptydata4.get(), nx, ny);
    auto emptydata5 = std::make_unique<float[]>(nx * ny);
    auto image = col_major_mdspan<float>(emptydata5.get(), nx, ny);
    
    casamatrix2mdspan<float>(matModel, model);
    casamatrix2mdspan<float>(matMask, mask);
    casamatrix2mdspan<float>(matPsf, psf);
    casamatrix2mdspan<float>(matResidual, residual);

    //std::cout << "matPsf(2000,2000) " << matPsf(2000,2000) << std::endl;
    //std::cout << "psf(2000,2000) " << psf(2000,2000) << std::endl;

    // getting information of coordinate system and the fitted gaussian beam
    Vector<double> rp = itsImages->psf()->coordinates().referencePixel();
    Vector<double> d = itsImages->psf()->coordinates().increment();
    double refi = rp(0);
    double refj = rp(1);
    Vector<double>fd(fabs(d));
    double inci = fd(0);
    double incj = fd(1);

    // genie to do: add code for fitting gaussian to psf here and define the three gaussian parameters. 
    GaussianBeam beam;
    try
    {
        StokesImageUtil::FitGaussianPSF(*(itsImages->psf()), beam);
    }
    catch(AipsError &x)
    {
      //os << "Error in fitting a Gaussian to the PSF : " << x.getMesg() << LogIO::POST;
      throw( AipsError("Error in fitting a Gaussian to the PSF" + x.getMesg()) );
    }
    double majaxis = beam.getMajor("arcsec");
    double minaxis = beam.getMinor("arcsec");
    double pa = beam.getPA("deg", True);

    //////////interface to the raw restore function////////
    int nSubChans = 1; 
    int chanid = 0;
    bool pbcor = false;
    auto emptydata6 = std::make_unique<float[]>(nx * ny);
    auto pb = col_major_mdspan<float>(emptydata6.get(), nx, ny);
    auto emptydata7 = std::make_unique<float[]>(nx * ny);
    auto image_pbcor = col_major_mdspan<float>(emptydata7.get(), nx, ny);
    
    Restore<float>(model, psf, residual, mask,
      image,
      nx, ny, 
      refi, refj, inci, incj,
      majaxis, minaxis, pa,
      nSubChans, chanid,
      pbcor, pb, image_pbcor);


  //////////Write back to files ////////////
    // convert mdspan back to casa matrix
    Matrix<Float> matImage(nx, ny, 0);
    mdspan2casamatrix<float>(image, matImage);
   
    (itsImages->image())->put(matImage);

    itsImages->releaseLocks();

}

#endif