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

#include <Restore/restore.h>


using namespace casa;
using namespace casacore;
using namespace libracore;

// it also assumes the input model and residual images are of the same name,
void casacore_restore(std::string& imageName, bool& doPBCorr)
{
    

      std::shared_ptr<SIImageStore> itsImages;
      itsImages.reset( new SIImageStore( imageName, true, true) ); 
      
      // this does fitting gaussian to psf and set up the correct restoring beam 
      //float MaxPsfSidelobe =  itsImages->getPSFSidelobeLevel(); 

    

      // converting Matrix to STL
      casacore::Array<casacore::Float> itsMatPsf, itsMatResidual, itsMatModel;
    itsImages->residual()->get( itsMatResidual, true );
    itsImages->model()->get( itsMatModel, true );
    itsImages->psf()->get( itsMatPsf, true );
    
    Matrix<Float> matPsf(itsMatPsf);
    Matrix<Float> matModel(itsMatModel);
    Matrix<Float> matResidual(itsMatResidual);

    AlwaysAssert(matPsf.shape()==matModel.shape(), AipsError);
    AlwaysAssert(matPsf.shape()==matResidual.shape(), AipsError);

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
    auto emptydata5 = std::make_unique<float[]>(nx * ny);
    auto image = col_major_mdspan<float>(emptydata5.get(), nx, ny);
    
    casamatrix2mdspan<float>(matModel, model);
    casamatrix2mdspan<float>(matPsf, psf);
    casamatrix2mdspan<float>(matResidual, residual);

    bool pbcor = doPBCorr;
    auto emptydata6 = std::make_unique<float[]>(nx * ny);
    auto pb = col_major_mdspan<float>(emptydata6.get(), nx, ny);
    auto emptydata7 = std::make_unique<float[]>(nx * ny);
    auto image_pbcor = col_major_mdspan<float>(emptydata7.get(), nx, ny);

    if (doPBCorr)
    {
      casacore::Array<casacore::Float> itsMatPB;
      itsImages->pb()->get( itsMatPB, true );
      Matrix<Float> matPB(itsMatPB);
      casamatrix2mdspan<float>(matPB, pb);
    }

    //std::cout << "matPsf(2000,2000) " << matPsf(2000,2000) << std::endl;
    //std::cout << "psf(2000,2000) " << psf(2000,2000) << std::endl;

    // getting information of coordinate system and the fitted gaussian beam
    Vector<double> rp = itsImages->psf()->coordinates().referencePixel();
    Vector<double> d = itsImages->psf()->coordinates().increment();
    double refi = rp(0);
    double refj = rp(1);
    Vector<double> fd(fabs(d));
    double inci = fd(0);
    double incj = fd(1);

    // fitting gaussian to psf here and define the three gaussian parameters. 
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
    // another way to get PSF beam
    // Get PSF Beams....
    /*ImageInfo ii = itsImages->psf()->imageInfo();
    ImageBeamSet itsPSFBeams = ii.getBeamSet();
    GaussianBeam beam = itsPSFBeams.getBeam();*/
 
    // the following conversion is critical
    double majaxis = beam.getMajor().get("arcsec").getValue() * C::arcsec;
    double minaxis = beam.getMinor().get("arcsec").getValue() * C::arcsec;
    double pa = (beam.getPA().get("deg").getValue() + 90.0)* C::degree;
    /*cout << "majaxis " << majaxis << endl;
    cout << "arcsec " << C::arcsec << endl;
    cout << "degree " << C::degree << endl;*/

    /* this is wrong
    double majaxis = beam.getMajor("arcsec");
    double minaxis = beam.getMinor("arcsec");
    double pa = beam.getPA("deg", True);*/
    
    /* correct fitting values for the unit test
    majaxis = 4.90554e-06;
    minaxis = 4.67228e-06;
    pa = 2.76764;*/

    //////////interface to the raw restore function////////
   
    Restore<float>(model, residual,
      image,
      nx, ny, 
      refi, refj, inci, incj,
      majaxis, minaxis, pa,
      pbcor, pb, image_pbcor);


  //////////Write back to files ////////////
    // convert mdspan back to casa matrix
    Matrix<Float> matImage(nx, ny, 0);
    mdspan2casamatrix<float>(image, matImage);   
    (itsImages->image())->put(matImage);

    if (doPBCorr)
    {
      Matrix<Float> matImagePBCor(nx, ny, 0);
      mdspan2casamatrix<float>(image_pbcor, matImagePBCor);   
      (itsImages->imagepbcor())->put(matImagePBCor);
    }

    itsImages->releaseLocks();

}


// same as above but calling restore_psf (i.e. provide the psf, instead of 
// the fitted gaussian parameters directly)
void casacore_restore_psf(std::string& imageName, bool& doPBCorr)
{
      std::shared_ptr<SIImageStore> itsImages;
      itsImages.reset( new SIImageStore( imageName, true, true) ); 
      

      // converting Matrix to STL
      casacore::Array<casacore::Float> itsMatPsf, itsMatResidual, itsMatModel;
    itsImages->residual()->get( itsMatResidual, true );
    itsImages->model()->get( itsMatModel, true );
    itsImages->psf()->get( itsMatPsf, true );
    
    Matrix<Float> matPsf(itsMatPsf);
    Matrix<Float> matModel(itsMatModel);
    Matrix<Float> matResidual(itsMatResidual);

    AlwaysAssert(matPsf.shape()==matModel.shape(), AipsError);
    AlwaysAssert(matPsf.shape()==matResidual.shape(), AipsError);

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
    auto emptydata5 = std::make_unique<float[]>(nx * ny);
    auto image = col_major_mdspan<float>(emptydata5.get(), nx, ny);
    
    casamatrix2mdspan<float>(matModel, model);
    casamatrix2mdspan<float>(matPsf, psf);
    casamatrix2mdspan<float>(matResidual, residual);

    bool pbcor = doPBCorr;
    auto emptydata6 = std::make_unique<float[]>(nx * ny);
    auto pb = col_major_mdspan<float>(emptydata6.get(), nx, ny);
    auto emptydata7 = std::make_unique<float[]>(nx * ny);
    auto image_pbcor = col_major_mdspan<float>(emptydata7.get(), nx, ny);

    if (doPBCorr)
    {
      casacore::Array<casacore::Float> itsMatPB;
      itsImages->pb()->get( itsMatPB, true );
      Matrix<Float> matPB(itsMatPB);
      casamatrix2mdspan<float>(matPB, pb);
    }

    // getting information of coordinate system and the fitted gaussian beam
    Vector<double> rp = itsImages->psf()->coordinates().referencePixel();
    Vector<double> d = itsImages->psf()->coordinates().increment();
    double refi = rp(0);
    double refj = rp(1);
    Vector<double> fd(fabs(d));
    double inci = fd(0);
    double incj = fd(1);

    
    /* correct fitting values for the unit test
    majaxis = 4.90554e-06;
    minaxis = 4.67228e-06;
    pa = 2.76764;*/

    //////////interface to the raw restore_psf function////////
    Restore_psf<float>(model, psf, residual,
      image,
      nx, ny, 
      refi, refj, inci, incj,
      pbcor, pb, image_pbcor);


  //////////Write back to files ////////////
    // convert mdspan back to casa matrix
    Matrix<Float> matImage(nx, ny, 0);
    mdspan2casamatrix<float>(image, matImage);   
    (itsImages->image())->put(matImage);

    if (doPBCorr)
    {
      Matrix<Float> matImagePBCor(nx, ny, 0);
      mdspan2casamatrix<float>(image_pbcor, matImagePBCor);   
      (itsImages->imagepbcor())->put(matImagePBCor);
    }

    itsImages->releaseLocks();

}

#endif