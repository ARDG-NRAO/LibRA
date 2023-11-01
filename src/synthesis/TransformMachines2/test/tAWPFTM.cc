//# tAWPFTM.cc: Tests the AWProject class of FTMachines
//# Copyright (C) 2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/Flux.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <synthesis/TransformMachines2/SimpleComponentFTMachine.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/casa/namespace.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/Utilities/Regex.h>
#include "MakeMS.h"
#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/AWProjectWBFTNew.h>
//#include <synthesis/TransformMachines2/AWConvFuncEPJones.h>
#include <synthesis/TransformMachines2/EVLAAperture.h>
#include <synthesis/TransformMachines2/AWVisResampler.h>
#include <synthesis/TransformMachines2/PointingOffsets.h>

using namespace casa;
using namespace casacore;
using namespace casa::refim;
using namespace casacore;
using namespace casa::test;

void createAWPFTMachine(CountedPtr<refim::FTMachine>& theFT, CountedPtr<refim::FTMachine>& theIFT, 
			const String& telescopeName,
			MPosition& observatoryLocation,
			const Int cfBufferSize=1024,
			const Int cfOversampling=20,
			//------------------------------
			const Int wprojPlane=1,
			//const Float=1.0,
			const Bool useDoublePrec=true,
			//------------------------------
			const Bool aTermOn= true,
			const Bool psTermOn= true,
			const Bool mTermOn= false,
			const Bool wbAWP= true,
			const String cfCache= "testCF.cf",
			const Bool doPointing= false,
			const Bool doPBCorr= true,
			const Bool conjBeams= true,
			const Float computePAStep=360.0,
			const Float rotatePAStep=5.0,
			const Int cache=1000000000,
			const Int tile=16,
			const String imageNamePrefix=String(""))
  
{
  LogIO os( LogOrigin("tAWPFTM","createAWPFTMachine",WHERE));
  
  if (wprojPlane<=1)
    {
      os << LogIO::NORMAL
	 << "You are using wprojplanes=1. Doing co-planar imaging (no w-projection needed)" 
	 << LogIO::POST;
      os << LogIO::NORMAL << "Performing WBA-Projection" << LogIO::POST; // Loglevel PROGRESS
    }
  if((wprojPlane>1)&&(wprojPlane<64)) 
    {
      os << LogIO::WARN
	 << "No. of w-planes set too low for W projection - recommend at least 128"
	 << LogIO::POST;
      os << LogIO::NORMAL << "Performing WBAW-Projection" << LogIO::POST; // Loglevel PROGRESS
    }
  
  // MSObservationColumns msoc(mss4vi_p[0].observation());
  // String telescopeName=msoc.telescopeName()(0);
  CountedPtr<refim::ConvolutionFunction> awConvFunc = AWProjectFT::makeCFObject(telescopeName, 
									 aTermOn,
									 psTermOn, true, mTermOn, wbAWP,
									 false);
   CountedPtr<refim::PointingOffsets> po = new refim::PointingOffsets();
   po->setOverSampling(awConvFunc->getOversampling());
   po->setDoPointing(doPointing);
   // awConvFunc->setPointingOffsets(po);
  //
  // Construct the appropriate re-sampler.
  //
  CountedPtr<refim::VisibilityResamplerBase> visResampler;
  visResampler = new refim::AWVisResampler();
  //
  // Construct and initialize the CF cache object.
  //
  CountedPtr<refim::CFCache> cfCacheObj;
  
  //
  // Finally construct the FTMachine with the CFCache, ConvFunc and
  // Re-sampler objects.  
  //
  Float pbLimit_l=1e-3;
  vector<float> posigdev = {10.0,10.0};
  theFT = new refim::AWProjectWBFTNew(wprojPlane, cache/2, 
				      cfCacheObj, awConvFunc, 
				      visResampler,
				      /*true */doPointing, posigdev, doPBCorr, 
				      tile, computePAStep, pbLimit_l, true,conjBeams,
				      useDoublePrec);
  
  cfCacheObj = new refim::CFCache();
  cfCacheObj->setCacheDir(cfCache.data());
  cfCacheObj->setWtImagePrefix(imageNamePrefix.c_str());
  cfCacheObj->initCache2();
  
  theFT->setCFCache(cfCacheObj);
  
  
  Quantity rotateOTF(rotatePAStep,"deg");
  static_cast<refim::AWProjectWBFTNew &>(*theFT).setObservatoryLocation(observatoryLocation);
  static_cast<refim::AWProjectWBFTNew &>(*theFT).setPAIncrement(Quantity(computePAStep,"deg"),rotateOTF);
  
  theIFT = new refim::AWProjectWBFTNew(static_cast<refim::AWProjectWBFTNew &>(*theFT));
  
  //// Send in Freq info.
  // os << "Sending frequency selection information " <<  mssFreqSel_p  <<  " to AWP FTM." << LogIO::POST;
  // theFT->setSpwFreqSelection( mssFreqSel_p );
  // theIFT->setSpwFreqSelection( mssFreqSel_p );
  
}

Int main(int argc, char **argv)
{
  Int NX=256, NY=256, cfBufferSize=512, cfOversampling=20;
  if (argc > 1) {sscanf(argv[1],"%d",&NX); NY=NX;}
  if (argc > 2) {sscanf(argv[2],"%d",&cfBufferSize);};
  if (argc > 3) {sscanf(argv[3],"%d",&cfOversampling);};
  
  try
    {
      MDirection thedir(Quantity(20.0, "deg"), Quantity(20.0, "deg"));
      String msname("Test.ms");
      MakeMS::makems(msname, thedir);
      MeasurementSet thems(msname, Table::Update);
      thems.markForDelete();
      vi::VisibilityIterator2 vi2(thems,vi::SortColumns(),true);
      vi::VisBuffer2 *vb=vi2.getVisBuffer();
      VisImagingWeight viw("natural");
      vi2.useImagingWeight(viw);
      ComponentList cl;
      SkyComponent otherPoint(ComponentType::POINT);
      otherPoint.flux() = Flux<Double>(6.66e-2, 0.0, 0.0, 0.00000);
      otherPoint.shape().setRefDirection(thedir);
      cl.add(otherPoint);
      Matrix<Double> xform(2,2);
      xform = 0.0;
      xform.diagonal() = 1.0;
      DirectionCoordinate dc(MDirection::J2000, Projection::SIN, Quantity(20.0,"deg"), Quantity(20.0, "deg"),
			     Quantity(10.5, "arcsec"), Quantity(10.5,"arcsec"),
			     xform, (NX/2.0), (NY/2.0), 999.0, 
			     999.0);
      Vector<Int> whichStokes(1, Stokes::I);
      StokesCoordinate stc(whichStokes);
      SpectralCoordinate spc(MFrequency::LSRK, 1.5e9, 1e6, 0.0 , 1.420405752E9);
      CoordinateSystem cs;
      cs.addCoordinate(dc); cs.addCoordinate(stc); cs.addCoordinate(spc);
      TempImage<Complex> im(IPosition(4,NX,NY,1,1), cs);//, "gulu.image");

      CountedPtr<refim::FTMachine> ftm,iftm;
      //FTMachine * ftm=nullptr;
      MPosition loc;
      MeasTable::Observatory(loc, MSColumns(thems).observation().telescopeName()(0));
      createAWPFTMachine(ftm, iftm,
			 String("EVLA"),
			 loc, cfBufferSize, cfOversampling);
      {
	MSSelection thisSelection;
	thisSelection.setSpwExpr(String("*"));
	TableExprNode exprNode=thisSelection.toTableExprNode(&thems);
	
	Matrix<Double> mssFreqSel;
	mssFreqSel  = thisSelection.getChanFreqList(NULL,true);
	// Send in Freq info.
	cerr << "Sending frequency selection information " <<  mssFreqSel  <<  " to AWP FTM." << endl;
	ftm->setSpwFreqSelection( mssFreqSel );
	iftm->setSpwFreqSelection( mssFreqSel );
      }

      {
	im.set(Complex(0.0));
	Matrix<Float> weight;
	vi2.originChunks();
	vi2.origin();
	ftm->initializeToSky(im, weight, *vb);
	refim::SimpleComponentFTMachine cft;
	
	for (vi2.originChunks();vi2.moreChunks(); vi2.nextChunk())
	  {
	    for (vi2.origin(); vi2.more(); vi2.next())
	      {
		cft.get(*vb, cl);
		vb->setVisCube(vb->visCubeModel());
		ftm->put(*vb);	
	      }
	  }
	ftm->finalizeToSky();
	
	ftm->getImage(weight, true);
	
	//  cerr << "val at center " << im.getAt(IPosition(4, 50, 50, 0, 0)) << endl;
	AlwaysAssertExit(near(6.66e-2, real( im.getAt(IPosition(4, (NX/2), (NY/2), 0, 0))), 1.0e-5));
	///Let us degrid now
	im.set(Complex(0.0));
	im.putAt(Complex(999.0, 0.0),IPosition(4, (NX/2), (NY/2), 0, 0) );
	vi2.originChunks();
	vi2.origin();
	ftm->initializeToVis(im, *vb);
	for (vi2.originChunks();vi2.moreChunks(); vi2.nextChunk())
	  {
	    for (vi2.origin(); vi2.more(); vi2.next())
	      {
		// Initializing the model cube.  It needs to be touched before it can be set.
		vb->visCubeModel();// Touch the cube.  This will "load" it and expand the buffer to the required shape.
		vb->setVisCubeModel(Complex(0.0,0.0)); // Set the internal buffer.
		ftm->get(*vb);
		//		cerr << "mod " << vb->visCubeModel().shape() << " " << (vb->visCubeModel())(0,0,0) << endl;
		AlwaysAssertExit(near(999.0, abs((vb->visCubeModel())(0,0,0)), 1e-5 ));
	      }
	  }
      }
      //detach the ms for cleaning up
      thems=MeasurementSet();
    } 
  catch (AipsError x) 
    {
      cerr << "Caught exception " << endl;
      cerr << x.getMesg() << endl;
      return(1);
    }
  cerr <<"OK" << endl;
  exit(0);
}

