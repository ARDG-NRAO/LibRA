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

CountedPtr<refim::PolOuterProduct> setPOP(vi::VisBuffer2 &vb2, std::string &stokes, std::string &mType)
{
  CountedPtr<refim::PolOuterProduct> pop_l = new PolOuterProduct;
  
  return pop_l;
}

void Coyote(bool &restartUI, int &argc, char **argv,
	    string &MSNBuf, string &imageName,
	    string &telescopeName,
	    int &NX, float &cellSize,
	    string &stokes, string &refFreqStr, int &nW,
	    string &cfCache,
	    bool &WBAwp, bool &psTerm, bool aTerm, string &mType,
	    string &fieldStr, string &spwStr, string &phaseCenter,
	    bool &conjBeams,  
	    float &pbLimit,
	    int &cfBufferSize, int &cfOversampling)
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
  if (aTerm == 0) ATerm_l->setOpCode(CFTerms::NOOP);
  if (psTerm == 0) PSTerm_l->setOpCode(CFTerms::NOOP);
  CountedPtr<refim::ConvolutionFunction> awcf_l = new AWConvFunc(ATerm_l, PSTerm_l, WTerm_l, WBAwp, conjBeams);						
  awcf_l = AWProjectFT::makeCFObject(telescopeName, ATerm_l, PSTerm_l, WTerm_l, true, WBAwp, conjBeams);						
  CountedPtr<casa::refim::CFCache> cfc_l;
  CountedPtr<casa::refim::CFStore2> cfs2_l, cfswt2_l;
  
  
  Vector<int> spwidList, fieldidList;
  Vector<double> spwRefFreqList;
  string uvDistStr;
  bool doSPWDataIter = false;
  Vector<Int> imSize(2,NX);
  Matrix<Double> vbFreqSelection ;
  
  DataBase db(MSNBuf, fieldStr, spwStr, uvDistStr, WBAwp, nW,
	      doSPWDataIter);
  
  PagedImage<Complex> cgrid = makeEmptySkyImage4CF(*(db.vi2_l), db.selectedMS, db.msSelection, 
						   imageName, imSize, cellSize, phaseCenter, 
						   stokes, refFreqStr);
  // Creating a complex image from the user inputed image.
  // To be turned into the most basic parameters needed to make CF
  // To read an image on disk you need PagedImage. 
  // This is inherited from ImageInterface so should be able to pass
  // an image interface object from it.
  
  
  // cgrid.table().markForDelete();
  
  Int wConvSize = 0;
  Float pa=0.0, dpa=0.0;
  const Vector<Double> uvScale(3,0);
  Bool fillCF = false;
  
  Vector<Double> uvOffset;
  uvOffset.resize(3);
  uvOffset(0)=imSize[0];
  uvOffset(1)=imSize[1];
  uvOffset(2)=0;
  // Replace this with the actual polarization parser.
  Vector<int> polMap{4,0,1,2,3};
  
  // Initialize pop to have the right values
  
  CountedPtr<refim::PolOuterProduct> pop_l = setPOP(*(db.vb_l), stokes, mType);
  awcf_l->setPolMap(polMap);
  
  cerr << "CF Oversampling inside AWCF is : " << awcf_l->getOversampling() <<endl;
  
  awcf_l->makeConvFunction(cgrid , *(db.vb_l), wConvSize,
			   pop_l, pa, dpa, uvScale, uvOffset,
			   vbFreqSelection, *cfs2_l, *cfswt2_l, fillCF);
  
}

#endif
