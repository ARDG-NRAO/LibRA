// -*- C++ -*-
//# MakeComponents.h: Definition of the MakeComponents class
//# Copyright (C) 2021
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
//# Correspondence concerning this should be addressed as follows:
//#        Postal address: National Radio Astronomy Observatory
//#                        1003 Lopezville Road,
//#                        Socorro, NM - 87801, USA
//#
//# $Id$

#ifndef ROADRUNNER_MAKECOMPONENTS_H
#define ROADRUNNER_MAKECOMPONENTS_H
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/tables/TaQL/ExprNode.h>

#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/AWVisResampler.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/AWVisResamplerHPG.h>
#include <synthesis/TransformMachines2/AWProjectWBFTHPG.h>
#include <synthesis/TransformMachines2/MakeCFArray.h>
#include <synthesis/TransformMachines2/ThreadCoordinator.h>
#include <hpg/hpg.hpp>

std::tuple<CountedPtr<refim::CFCache>,CountedPtr<refim::VisibilityResamplerBase>>
createAWPFTMachine(const String ftmName,
		   const String modelImageName,
		   CountedPtr<refim::FTMachine>& theFT, 
		   const String& telescopeName,
		   MPosition& observatoryLocation,
		   const String cfCache= "testCF.cf",
		   const Bool wbAWP= true,
		   //------------------------------
		   const Int wprojPlane=1,
		   //const Float=1.0,
		   const Bool useDoublePrec=true,
		   //------------------------------
		   const Bool aTermOn= true,
		   const Bool psTermOn= false,
		   const Bool mTermOn= false,
		   const Bool doPointing= true,
		   const Bool doPBCorr= true,
		   const Bool conjBeams= true,
		   Float pbLimit_l=1e-3,
		   vector<float> posigdev = {300.0,300.0},
		   const String imageNamePrefix=String(""),
		   const String imagingMode="residual",
		   const Float computePAStep=360.0,
		   const Float rotatePAStep=360.0,
		   const Int cache=1000000000,
		   const Int tile=16)
  
{
  LogIO os( LogOrigin("roadrunner","createAWPFTMachine",WHERE));
  
  if (wprojPlane<=1)
    {
      os << LogIO::NORMAL
	 << "You are using wprojplanes=1. Doing co-planar imaging (no w-projection needed)" 
	 << LogIO::POST;
      os << LogIO::NORMAL << "Performing WBA-Projection" << LogIO::POST; // Loglevel PROGRESS
    }
  else
    os << LogIO::NORMAL << "Performing WBAW-Projection" << LogIO::POST; // Loglevel PROGRESS
  
  // MSObservationColumns msoc(mss4vi_p[0].observation());
  // String telescopeName=msoc.telescopeName()(0);
  CountedPtr<refim::ConvolutionFunction> awConvFunc = AWProjectFT::makeCFObject(telescopeName, 
									 aTermOn,
									 psTermOn, true, mTermOn, wbAWP,
									 false);
  //
  // Construct the appropriate re-sampler.
  //
  CountedPtr<refim::VisibilityResamplerBase> visResampler;
  if (ftmName == "awphpg") visResampler = new refim::AWVisResamplerHPG(false);
  else visResampler = new refim::AWVisResampler();
  visResampler->setModelImage(modelImageName);
  //
  // Construct and initialize the CF cache object.
  //
  CountedPtr<refim::CFCache> cfCacheObj;
  
  //
  // Finally construct the FTMachine with the CFCache, ConvFunc and
  // Re-sampler objects.  
  //
  if (ftmName == "awphpg")
    theFT = new refim::AWProjectWBFTHPG(wprojPlane, cache/2,
					   cfCacheObj, awConvFunc,
					   visResampler,
					   /*true */doPointing, posigdev, doPBCorr,
					   tile, computePAStep, pbLimit_l, true,conjBeams,
					   useDoublePrec);
  else
    theFT = new refim::AWProjectWBFT(wprojPlane, cache/2,
					   cfCacheObj, awConvFunc,
					   visResampler,
					   /*true */doPointing, posigdev, doPBCorr,
					   tile, computePAStep, pbLimit_l, true,conjBeams,
					   useDoublePrec);
  
  if (imagingMode=="weight")    {theFT->setFTMType(casa::refim::FTMachine::WEIGHT);}
  else if (imagingMode=="psf")  {theFT->setFTMType(casa::refim::FTMachine::PSF);}
  else if (imagingMode=="snrpsf")  {theFT->setFTMType(casa::refim::FTMachine::SNRPSF);}

  cfCacheObj = new refim::CFCache();
  cfCacheObj->setCacheDir(cfCache.data());
  cfCacheObj->setLazyFill(refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1);
  cfCacheObj->setWtImagePrefix(imageNamePrefix.c_str());
  //cfCacheObj->initCache2(); // This loads, both CFS* and WTCFS* CFs.  In roadrunner, only one of them required (depending on the "imagingmode" parameter)
  if (theFT->ftmType()   == casa::refim::FTMachine::PSF 
      || theFT->ftmType()== casa::refim::FTMachine::WEIGHT)
    cfCacheObj->initCache2(false, 400.0, -1.0,casacore::String("WTCFS*")); // This would load only WTCFS* CFs
  else
    cfCacheObj->initCache2(false, 400.0, -1.0,casacore::String("CFS*")); // This would load only CFS* CFs
  // {
  //   CountedPtr<casa::refim::CFBuffer> cfb_l = cfCacheObj->memCache2_p[0].getCFBuffer(0,0);
  //   cfb_l->show("cfb: ");
  // }
  
  theFT->setCFCache(cfCacheObj);
  
  
  Quantity rotateOTF(rotatePAStep,"deg");
  static_cast<refim::AWProjectFT &>(*theFT).setObservatoryLocation(observatoryLocation);
  static_cast<refim::AWProjectFT &>(*theFT).setPAIncrement(Quantity(computePAStep,"deg"),rotateOTF);

  // Send in Freq info. Reference code for enabling arbitrary
  // freq. resolution for wbawp imaging (currently it is set to SPW
  // resolution).
  //
  // os << "Sending frequency selection information " <<  mssFreqSel_p  <<  " to AWP FTM." << LogIO::POST;
  // theFT->setSpwFreqSelection( mssFreqSel_p );
  // theIFT->setSpwFreqSelection( mssFreqSel_p );

  std::tuple<CountedPtr<refim::CFCache>,CountedPtr<refim::VisibilityResamplerBase>> retup = std::make_tuple(cfCacheObj,visResampler); 

  return retup;
}
//
//-------------------------------------------------------------------------
//
bool mdFromString(casacore::MDirection &theDir, const casacore::String &in)
{
   bool rstat(false);
   String tmpA, tmpB, tmpC;
   std::istringstream iss(in);
   iss >> tmpA >> tmpB >> tmpC;
   ushort ntrials=0;

   while((rstat==false) && (ntrials < 2))
     {
       try
	 {
	   casacore::Quantity tmpQA;
	   casacore::Quantity tmpQB;
	   casacore::Quantity::read(tmpQA, tmpA);
	   casacore::Quantity::read(tmpQB, tmpB);
	   if(tmpC.length() > 0)
	     {
	       MDirection::Types theRF;
	       MDirection::getType(theRF, tmpC);
	       theDir = MDirection (tmpQA, tmpQB, theRF);
	       rstat = true;
	     }
	   else
	     {
	       theDir = MDirection (tmpQA, tmpQB);
	       rstat = true;
	     }
	 }
       catch (AipsError& e)
	 {
	   cerr << e.what() << endl;
	   if (ntrials > 1) throw(e);
	   // Assume that the string is in the EPOCH RA DEC format
	   // (and therefore failed).  So try RA DEC EPOCH format.
	   String tt=tmpA;
	   tmpA=tmpB;
	   tmpB=tmpC;
	   tmpC=tt;
	   cerr << "###Informational: Re-trying phasecenter specification as " << tmpA << " " << tmpB << " " << tmpC << "...";
	   ntrials++;
	 }
     }
   if (ntrials > 0) {rstat ? cerr << "success!" << endl : cerr << "failed." << endl;}

   return rstat;
}
//
//-------------------------------------------------------------------------
//
std::map<casacore::Int, std::map<casacore::Int, casacore::Vector<casacore::Int> > >
makeTheChanSelMap(MSSelection& mss)
{
  Matrix<Int> mssChanSel = mss.getChanList();
  IPosition mssChanSelShape=mssChanSel.shape();

  std::map<casacore::Int, std::map<casacore::Int, casacore::Vector<casacore::Int> > > chanselKurfuffel;
  std::map<casacore::Int, casacore::Vector<casacore::Int> > channelsPerSpw;
  for(int i=0;i<mssChanSelShape[0];i++)
    {
      Vector<Int> channels(2,-1);
      int spw=mssChanSel(i,0);
      channels[0]=mssChanSel(i,2)-mssChanSel(i,1)+1;//mssChanSel(i,1);
      channels[1]=mssChanSel(i,1);//mssChanSel(i,2);
      channelsPerSpw[spw]=channels;
      //      cerr << "SPW:CHAN " << spw << " " << channels << endl;
    }
  chanselKurfuffel[0]=channelsPerSpw;
  return chanselKurfuffel;
}
//
//-------------------------------------------------------------------------
//
PagedImage<Complex> makeEmptySkyImage(VisibilityIterator2& vi2,
				      const MeasurementSet& selectedMS,
				      MSSelection& msSelection,
				      const String& imageName, const String& startModelImageName,
				      const Vector<Int>& imSize, const float& cellSize,
				      const String& phaseCenter, const String& stokes,
				      const String& refFreq,
				      const String& mode)
{
  Vector<Quantity> qCellSize(2);
  for (int i=0;i<2;i++)
    {
      String qUnit("arcsec");
      qCellSize[i]=Quantity(cellSize, qUnit);
    }

  int imStokes=1;
  if (stokes=="I") imStokes=1;
  else if (stokes=="IV") imStokes=2;
  else if (stokes=="IQUV") imStokes=4;

  int imNChan=1;
  if (mode=="mfs") imNChan=1;
  // else if (mode=="pseudo") {}
  // else if (mode=="spectral") {imnchan=datanchan[0];imstart=datastart[0];imstep=datastep[0];}
  
  MDirection mphaseCenter;
  mdFromString(mphaseCenter, phaseCenter);

  SynthesisParamsGrid gridParams;
  SynthesisParamsImage imageParams;
  imageParams.imageName=imageName;
  imageParams.startModel=startModelImageName;
  imageParams.imsize=imSize;
  imageParams.cellsize=qCellSize;
  imageParams.phaseCenter = mphaseCenter;
  imageParams.stokes=stokes;
  imageParams.mode=mode;
  imageParams.frame=String("LSRK");
  imageParams.veltype=String("radio");

  //
  // There are two items related to ref. freq. "reffreq" (a String) and "refFreq" (a Quantity).
  // Set them both.
  //
  if (refFreq != "")
    {
      imageParams.reffreq=refFreq;
     
      std::istringstream iss(refFreq);
      Double ff;
      iss >> ff;
      //      cerr << "Ref. freq. = " << ff << endl;
      imageParams.refFreq=Quantity(ff,"GHz");
    }
  
  casacore::Block<const casacore::MeasurementSet *> msList(1); msList[0]=&selectedMS;
  CoordinateSystem csys = imageParams.buildCoordinateSystem(vi2,makeTheChanSelMap(msSelection),msList);
  IPosition imshape(4,imSize(0),imSize(1),imStokes,imNChan);
  return PagedImage<Complex>(imshape, csys, imageParams.imageName+".tmp");
}

// // Get from UI
// //-------------------------------
// string cmplxGridName;
// string phaseCenter; 
// string stokes;
// string refFreqStr;
// string mode;
// string weighting;
// string cfCache;
// string imageNamePrefix;
// string imagingMode;

// int imSize; 
// int NX, nW;

// float cellSize;
// float robust;
// float pbLimit;
// vector<float> posigdev;

// bool WBAwp;
// bool doPointing;
// bool normalize;
// bool doPBCorr;
// bool conjBeams;
// bool doSPWDataIter;
// bool aTermOn;
// bool psTermOn;
// bool mTermOn;
// //-------------------------------



// PagedImage<Complex> cgrid;
// PagedImage<Float> skyImage;

// void run(const DataBase& db, const UI& ui)
// {

//       //
//       //-------------------------------------------------------------------
//       // Make the empty grid with the sky image coordinates
//       //
//       Vector<Int> imSize(2,NX);
//       String mode="mfs", startModelImageName="";

//       cgrid=makeEmptySkyImage(*(db.vi2_l), db.selectedMS, db.msSelection,
// 			      cmplxGridName, startModelImageName,
// 			      imSize, cellSize, phaseCenter,
// 			      stokes, refFreqStr, mode);
//       skyImage = PagedImage<Float>(cgrid.shape(),cgrid.coordinates(), imageName);

//       cgrid.table().markForDelete();

//       // Setup the weighting scheme in the supplied VI2
//       weightor(*(db.vi2_l),
// 	       cgrid.coordinates(), // CSys of the sky image
// 	       cgrid.shape(),       // X-Y shape of the sky image
// 	       weighting,
// 	       rmode,
// 	       casacore::Quantity(0.0, "Jy"),  // noise
// 	       robust);

//       // remove all images except the reduced one on the root rank
//       if (!isRoot)
// 	skyImage.table().markForDelete();

//       StokesImageUtil::From(cgrid, skyImage);
//       if(db.vb_l->polarizationFrame()==MSIter::Linear) StokesImageUtil::changeCStokesRep(cgrid,StokesImageUtil::LINEAR);
//       else StokesImageUtil::changeCStokesRep(cgrid, StokesImageUtil::CIRCULAR);

//       //-------------------------------------------------------------------
//       // Create the AWP FTMachine.  The AWProjectionFT is construed
//       // with the re-sampler depending on the ftmName (AWVisResampler
//       // for ftmName='awproject' or AWVisResamplerHPG for
//       // ftmName='awphpg')
//       //
//       MPosition loc;
//       MeasTable::Observatory(loc, MSColumns(db.selectedMS).observation().telescopeName()(0));
//       Bool useDoublePrec=true, aTermOn=true, psTermOn=false, mTermOn=false, doPSF=false;

//       auto ret = 
// 	createAWPFTMachine(ftmName, modelImageName, ftm_g,
// 			   String("EVLA"),
// 			   loc, cfCache,
// 			   //cfBufferSize, cfOversampling,
// 			   WBAwp,nW,
// 			   useDoublePrec,
// 			   aTermOn,
// 			   psTermOn,
// 			   mTermOn,
// 			   doPointing,
// 			   doPBCorr,
// 			   conjBeams,
// 			   pbLimit,
// 			   posigdev,
// 			   imageNamePrefix,
// 			   imagingMode
// 			   );
//       CountedPtr<refim::CFCache>  cfc = get<0>(ret);
//       CountedPtr<refim::VisibilityResamplerBase> visResampler = get<1>(ret);
//       {
// 	Matrix<Double> mssFreqSel;
// 	mssFreqSel  = db.msSelection.getChanFreqList(NULL,true);
// 	// Send in Freq info.
// 	ftm_g->setSpwFreqSelection( mssFreqSel );

// 	doPSF=(ftm_g->ftmType()==casa::refim::FTMachine::PSF);
//       }

//       //-------------------------------------------------------------------
//       // Iterate over the MS, trigger the gridding.
//       //
//       cgrid.set(Complex(0.0));
//       Matrix<Float> weight;
//       db.vi2_l->originChunks();
//       db.vi2_l->origin();

//       // If no normalization is requested, AWProjectWBFT should not grid
//       // for .weight image.
//       //
//       // This de-couples calculations for .weight, .psf, .residual, which
//       // can now be done independently and in any order.
//       //
//       // The run for .residual image also save the sumwt image, since that
//       // matches what production CASA does.  Which SoW out of .weight,
//       // .psf, or .residual runs is the appropriate one is unresolved.
//       // Probably all should be used, but this needs to be carefully
//       // worked out.
//       if (!normalize) ftm_g->setPBReady(true);

//       if (imagingMode=="predict")
// 	ftm_g->initializeToVis(cgrid,*(db.vb_l));
//       else
// 	ftm_g->initializeToSky(cgrid, weight, *(db.vb_l));

//       // barrier needed for accurate elapsed time measurement
//       // mpi_barrier(MPI_COMM_WORLD);
//       timer.mark();

//       CountedPtr<casa::refim::CFStore2> cfs2_l;
//       if (!cfc.null())
// 	{
// 	  if (doPSF || (imagingMode=="weight"))
// 	    cfs2_l =  CountedPtr<casa::refim::CFStore2>(&cfc->memCacheWt2_p[0],false);//new CFStore2;
// 	  else
// 	    cfs2_l = CountedPtr<casa::refim::CFStore2>(&(cfc->memCache2_p)[0],false);//new CFStore2;
// 	}

//       Vector<int> chanMap, polMap;
//       visResampler->getMaps(chanMap, polMap);
//       int nGridPlanes = skyImage.shape()[2]; 
//       PolMapType mndx, conj_mndx;
//       {
// 	auto ret = makeMNdx(std::string("stokesI.mndx"), polMap, nGridPlanes);
// 	mndx      = std::get<0>(ret);
// 	conj_mndx = std::get<1>(ret);

// 	auto prt = [](std::string tag, PolMapType& p) -> void
// 		   {
// 		     cerr << tag << endl;
// 		     for( auto row : p)
// 		       {
// 			 for (auto col : row) cerr << col << " ";
// 			 cerr << endl;
// 		       }
// 		   };
// 	prt(std::string("mndx:"),mndx);
// 	prt(std::string("cmndx:"),conj_mndx);
//       }

//       int nDataPol;
//       // Extract SPW ref. frequency and the number of polarizations in the
//       // data (nDataPol).
//       {
// 	db.vi2_l->originChunks();
// 	db.vi2_l->origin();
// 	spwRefFreqList.assign(db.vb_l->subtableColumns().spectralWindow().refFrequency().getColumn());
// 	nDataPol  = db.vb_l->flagCube().shape()[0];
//       }

// }
#endif
