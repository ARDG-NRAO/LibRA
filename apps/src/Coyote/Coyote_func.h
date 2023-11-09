// -*- C++ -*-
// # Coyote_func.h: Definition of the Coyote function
// # Copyright (C) 2021
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$

#ifndef COYOTE_H
#define COYOTE_H

#include <casacore/casa/namespace.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/Logging/LogFilter.h>
#include <casacore/casa/System/ProgressMeter.h>

#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/tables/TaQL/ExprNode.h>

#include <casacore/ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>

#include <synthesis/TransformMachines2/MakeCFArray.h>
#include <synthesis/TransformMachines2/ThreadCoordinator.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/AWConvFunc.h>
#include <synthesis/TransformMachines2/CFCache.h>
#include <synthesis/TransformMachines2/PolOuterProduct.h>

#include <RoadRunner/rWeightor.h>
#include <RoadRunner/DataIterations.h>
#include <RoadRunner/DataBase.h>
#include <RoadRunner/MakeComponents.h>
#include <RoadRunner/Roadrunner_func.h>


#include <hpg/hpg.hpp>
using namespace casa;
using namespace casa::refim;
using namespace casacore;
using namespace std;

PagedImage<Complex> makeEmptySkyImage4CF(VisibilityIterator2& vi2,
					 const MeasurementSet& selectedMS,
					 MSSelection& msSelection,
					 const String& imageName,
					 const Vector<Int>& imSize, const float& cellSize,
					 const String& phaseCenter, const String& stokes,
					 const String& refFreq)
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
  
  MDirection mphaseCenter;
  mdFromString(mphaseCenter, phaseCenter);
  
  SynthesisParamsGrid gridParams;
  SynthesisParamsImage imageParams;
  imageParams.imageName=imageName;
  imageParams.imsize=imSize;
  imageParams.cellsize=qCellSize;
  imageParams.phaseCenter = mphaseCenter;
  imageParams.stokes=stokes;
  imageParams.mode=String("mfs");
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

CountedPtr<refim::PolOuterProduct> setPOP(vi::VisBuffer2 &vb2,
					  Vector<casacore::Stokes::StokesTypes> visPolMap,
					  Vector<int> polMap,
					  std::string &stokes, std::string &mType)
{
  CountedPtr<refim::PolOuterProduct> pop_l = new PolOuterProduct;
  
  //------------------------a mess----------------------------------------------------
  Vector<Int> intpolmap(visPolMap.nelements());
  for (uInt kk=0; kk < intpolmap.nelements(); ++kk){
    intpolmap[kk]=Int(visPolMap[kk]);
  }
  pop_l->initCFMaps(intpolmap, polMap);

  PolMapType polMat, polIndexMat, conjPolMat, conjPolIndexMat;
  Vector<Int> visPol(vb2.correlationTypes());
  polMat = pop_l->makePolMat(visPol,polMap);
  //cerr << visPol << " " << polMap << endl;
  polIndexMat = pop_l->makePol2CFMat(visPol,polMap);

  conjPolMat = pop_l->makeConjPolMat(visPol,polMap);
  conjPolIndexMat = pop_l->makeConjPol2CFMat(visPol,polMap);

  // cerr << "polMap: "; cerr << polMap << endl;
  // cerr << "visPolMap: "; cerr << visPolMap << endl;
  //------------------------a mess----------------------------------------------------

  return pop_l;
}

void Coyote(bool &restartUI, int &argc, char **argv,
	    string &MSNBuf, string &imageName,
	    string &telescopeName,
	    int &NX, float &cellSize,
	    string &stokes, string &refFreqStr, int &nW,
	    string &cfCache,
	    string& imageNamePrefix,
	    bool &WBAwp, bool &psTerm, bool aTerm, string &mType,
	    float& pa, float& dpa,
	    string &fieldStr, string &spwStr, string &phaseCenter,
	    bool &conjBeams,  
	    float &pbLimit,
	    int &cfBufferSize, int &cfOversampling,
	    bool& dryRun)
{
  LogFilter filter(LogMessage::NORMAL);
  LogSink::globalSink().filter(filter);
  LogIO log_l(LogOrigin("roadrunner", "Roadrunner_func"));
  
  bool wTerm = false;
  if (nW > 1) wTerm=true;
  // Instantiate AWCF object so we can use it to make cf.
  // The AWCF::makeConvFunc requires the following objects
  // CountedPtr<refim::ConvolutionFunction> awcf_l = AWProjectFT::makeCFObject(telescopeName, 
  //  aTerm,
  //  psTerm, wTerm, true, WBAwp,
  //  conjBeams);
  CountedPtr<refim::PSTerm> PSTerm_l = new PSTerm();
  CountedPtr<refim::ATerm> ATerm_l = AWProjectFT::createTelescopeATerm(telescopeName, aTerm);
  CountedPtr<refim::WTerm> WTerm_l = new WTerm();

  if (nW==1) WTerm_l->setOpCode(CFTerms::NOOP);
  if (aTerm == false) ATerm_l->setOpCode(CFTerms::NOOP);
  if (psTerm == false) PSTerm_l->setOpCode(CFTerms::NOOP);

  CountedPtr<refim::ConvolutionFunction> awcf_l = new AWConvFunc(ATerm_l, PSTerm_l, WTerm_l, WBAwp, conjBeams);						
  awcf_l = AWProjectFT::makeCFObject(telescopeName, ATerm_l, PSTerm_l, WTerm_l, true, WBAwp, conjBeams);						

  //  const String imageNamePrefix=imageNamePrefix;
  CountedPtr<refim::CFCache> cfCacheObj_l = new refim::CFCache();
  cfCacheObj_l->setCacheDir(cfCache.data());
  cfCacheObj_l->setLazyFill(refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1);
  cfCacheObj_l->setWtImagePrefix(imageNamePrefix.c_str());
  try
    {
      cfCacheObj_l->initCache2(false, dpa, -1.0,
			       casacore::String(imageNamePrefix)+casacore::String("CFS*")); // This would load CFs based on imageNamePrefix
    }
  catch (AipsError &e)
    {
      // Ignore the "CFS is empty" exception.
      //cerr << e.what() << endl;
    }
  CountedPtr<casa::refim::CFStore2> cfs2_l, cfswt2_l;

  if (!cfCacheObj_l.null())
    {
      cfs2_l = CountedPtr<CFStore2>(&(cfCacheObj_l->memCache2_p)[0],false);//new CFStore2;
      cfswt2_l =  CountedPtr<CFStore2>(&cfCacheObj_l->memCacheWt2_p[0],false);//new CFStore2;
    }

  Vector<int> spwidList, fieldidList;
  Vector<double> spwRefFreqList;
  string uvDistStr;
  bool doSPWDataIter = false;
  Vector<Int> imSize(2,NX);
  Matrix<Double> vbFreqSelection ;
  
  DataBase db(MSNBuf, fieldStr, spwStr, uvDistStr, WBAwp, nW,
	      doSPWDataIter);
  
  // Creating a complex image from the user inputed image.
  // To be turned into the most basic parameters needed to make CF
  // To read an image on disk you need PagedImage. 
  // This is inherited from ImageInterface so should be able to pass
  // an image interface object from it.
  PagedImage<Complex> cgrid = makeEmptySkyImage4CF(*(db.vi2_l), db.selectedMS, db.msSelection, 
						   imageName, imSize, cellSize, phaseCenter, 
						   stokes, refFreqStr);
  cgrid.table().markForDelete();
  
  Int wConvSize = nW;
  const Vector<Double> uvScale(3,0);
  Bool fillCF = !dryRun;
  
  Vector<Double> uvOffset;
  uvOffset.resize(3);
  uvOffset(0)=imSize[0];
  uvOffset(1)=imSize[1];
  uvOffset(2)=0;
  // Replace this with the actual polarization parser.
  Vector<int> polMap;
  Vector<casacore::Stokes::StokesTypes> visPolMap;//{0,1,2,3};

  refim::SynthesisUtils::matchPol(*(db.vb_l),cgrid.coordinates(),polMap,visPolMap);
  // Initialize pop to have the right values
  
  CountedPtr<refim::PolOuterProduct> pop_p = setPOP(*(db.vb_l), visPolMap, polMap, stokes, mType);


  // spwidList      = db.spwidList;
  // fieldidList    = db.fieldidList;
  // spwRefFreqList = db.spwRefFreqList;
  Matrix<Double> mssFreqSel;
  mssFreqSel  = db.msSelection.getChanFreqList(NULL,true);
  awcf_l->setPolMap(polMap);
  //  awcf_l->setSpwSelection(spwChanSelFlag_p);
  awcf_l->setSpwFreqSelection(mssFreqSel);
  
  // cerr << "CF Oversampling inside AWCF is : " << awcf_l->getOversampling() <<endl;
  
  try
    {
      double D2R=C::pi/180.0;
      if (abs(pa) > 180.0) pa=getPA(*(db.vb_l));
      pa *= D2R; dpa *= D2R;

      // This currently makes both CF and WTCF. 
      awcf_l->makeConvFunction(cgrid , *(db.vb_l), wConvSize,
			       pop_p, pa, dpa, uvScale, uvOffset,
			       vbFreqSelection,
			       *cfs2_l, *cfswt2_l,
			       fillCF);

      cerr << "CFS shapes: " << cfs2_l->getStorage()[0,0].shape() << " " << cfswt2_l->getStorage()[0,0].shape() << endl;
      //      cfs2_l->show("CFStore",cerr,true);
      cfs2_l->makePersistent(cfCacheObj_l->getCacheDir().c_str(),"","", Quantity(pa,"rad"),Quantity(dpa,"rad"),0,0);
      cfswt2_l->makePersistent(cfCacheObj_l->getCacheDir().c_str(),"","WT",Quantity(pa,"rad"),Quantity(dpa,"rad"),0,0);

      Double memUsed=cfs2_l->memUsage();
      String unit(" KB");
      memUsed = (Int)(memUsed/1024.0+0.5);
      if (memUsed > 1024) {memUsed /=1024; unit=" MB";}

      LogIO log_l(LogOrigin("AWProjectFT2", "findConvFunction[R&D]"));
      log_l << "Convolution function memory footprint:" 
	    << (Int)(memUsed) << unit << " out of a maximum of "
	    << HostInfo::memoryTotal(true)/1024 << " MB" << LogIO::POST;
    }
  catch(AipsError &e)
    {
      cerr << e.what() << endl;
    }
  
}

#endif
