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
#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/AWProjectWBFTNew.h>
//#include <synthesis/TransformMachines2/AWConvFuncEPJones.h>
#include <synthesis/TransformMachines2/EVLAAperture.h>
#include <synthesis/TransformMachines2/AWVisResampler.h>
#include <synthesis/TransformMachines2/AWVisResamplerHPG.h>
#include <synthesis/TransformMachines2/PointingOffsets.h>
#include <synthesis/TransformMachines2/VBStore.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/images/Images/ImageOpener.h>
#include <casacore/images/Images/ImageSummary.h>
#include <casacore/casa/System/ProgressMeter.h>
#include <casacore/casa/OS/Timer.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <cstdlib>
//#include <synthesis/TransformMachines2/HPGModelImage.h>

using namespace casa;
using namespace casacore;
using namespace casa::refim;
using namespace casacore;
//using namespace casa::test;

void createAWPFTMachine(const String ftmName,
			const String modelImageName,
			CountedPtr<refim::FTMachine>& theFT, CountedPtr<refim::FTMachine>& theIFT, 
			const String& telescopeName,
			MPosition& observatoryLocation,
			const String cfCache= "testCF.cf",
			const Int cfBufferSize=1024,
			const Int cfOversampling=20,
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
			const Float computePAStep=360.0,
			const Float rotatePAStep=360.0,
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
  else
    os << LogIO::NORMAL << "Performing WBAW-Projection" << LogIO::POST; // Loglevel PROGRESS
  // if((wprojPlane>1)&&(wprojPlane<64)) 
  //   {
  //     os << LogIO::WARN
  // 	 << "No. of w-planes set too low for W projection - recommend at least 128"
  // 	 << LogIO::POST;
  //   }
  
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
  if (ftmName == "awphpg") visResampler = new refim::AWVisResamplerHPG();
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
  //  Float pbLimit_l=1e-3;
  //  vector<float> posigdev = {300.0,300.0};
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

//

bool mdFromString(casacore::MDirection &theDir, const casacore::String &in)
{
   bool rstat(false);
   String tmpA, tmpB, tmpC;
   std::istringstream iss(in);
   iss >> tmpA >> tmpB >> tmpC;
   casacore::Quantity tmpQA;
   casacore::Quantity tmpQB;
   casacore::Quantity::read(tmpQA, tmpA);
   casacore::Quantity::read(tmpQB, tmpB);
   if(tmpC.length() > 0){
      MDirection::Types theRF;
      MDirection::getType(theRF, tmpC);
      theDir = MDirection (tmpQA, tmpQB, theRF);
      rstat = true;
   } else {
      theDir = MDirection (tmpQA, tmpQB);
      rstat = true;
   }
   return rstat;
}

std::map<casacore::Int, std::map<casacore::Int, casacore::Vector<casacore::Int> > > makeTheChanSelMap(MSSelection& mss)
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
      imageParams.refFreq=Quantity(ff/1e9,"GHz");
    }
  
  casacore::Block<const casacore::MeasurementSet *> msList(1); msList[0]=&selectedMS;
  CoordinateSystem csys = imageParams.buildCoordinateSystem(vi2,makeTheChanSelMap(msSelection),msList);
  IPosition imshape(4,imSize(0),imSize(1),imStokes,imNChan);
  return PagedImage<Complex>(imshape, csys, imageParams.imageName);
}
//
//-------------------------------------------------------------------------
//
std::tuple<Vector<Int>, Vector<Int> > loadMS(const String& msname,
					     const String& spwStr,
					     const String& fieldStr,
					     const String& uvDistStr,
					     MeasurementSet& thems,
					     MeasurementSet& selectedMS,
					     MSSelection& msSelection)
{
  //MeasurementSet thems;
  if (Table::isReadable(msname))
    thems=MeasurementSet(msname, Table::Update);
  else
    throw(AipsError(msname+" does exist or not readable"));
  //
  //-------------------------------------------------------------------
  //
  msSelection.setSpwExpr(spwStr);
  //msSelection.setAntennaExpr(antStr);
  msSelection.setFieldExpr(fieldStr);
  msSelection.setUvDistExpr(uvDistStr);
  selectedMS = MeasurementSet(thems);
  Vector<int> spwid, fieldid;
  TableExprNode exprNode=msSelection.toTableExprNode(&thems);
  if (!exprNode.isNull())
    {
      selectedMS = MS(thems(exprNode));
      spwid=msSelection.getSpwList();
      fieldid=msSelection.getFieldList();
    }
  return std::tuple<Vector<Int>, Vector<Int> >{spwid, fieldid};
}
//
//-------------------------------------------------------------------------
//

Int main(int argc, char **argv)
{
  //
  // -------------------------------------- Just the UI -------------------------------------------------------------------
  //
  system("rm -rf ./refim_mawproject.ms");
  system("cp -r /home/casa/data/casatestdata/measurementset/evla/refim_mawproject.ms .");
  string MSNBuf="./refim_mawproject.ms";
  string ftmName="awphpg";
  string cfCache="test.cf";
  string fieldStr="";
  string spwStr="*";
  string uvDistStr="";
  string imageName="gooloo";
  string modelImageName="";
  string cmplxGridName="gooloo.cmplx";
  string phaseCenter="19h59m28.5 40d44m01.5 J2000";
  string stokes="I";
  string refFreqStr="3.0e9";
  string weighting="natural";

  float refFreq=3e09, freqBW=3e9;
  float cellSize=4.84813681109536e-05;
  int NX=512;
  Int NY=512;
  Int cfBufferSize=512;
  Int cfOversampling=20;
  Int nW=1;
  int nInt=200;
  int nchan=1;
  Bool WBAwp=true;
  Bool restartUI=false;
  Bool doPointing=false;
  Bool normalize=false;
  Bool doPBCorr= false;
  Bool conjBeams= false;
  Float pbLimit=1e-3;
  vector<float> posigdev = {300.0,300.0};

  //
  // -------------------------------------- End of UI -------------------------------------------------------------------
  //
  
      cerr << "###Info: Pointing sigma dev = " << posigdev[0] << "," << posigdev[1] << endl;
  //  restartUI=false;
  NY=NX;
  
  std::stringstream strNChan,strNInt;
  
  strNChan << nchan;
  strNInt << nInt;
  Timer timer;
  double griddingTime=0;
  try
    {
      if (phaseCenter=="") throw(AipsError("The phasecenter parameter needs to be set"));
      if (refFreqStr=="")  throw(AipsError("The reffreq parameter needs to be set"));
      if (NX<=0)           throw(AipsError("The imsize parameter needs to be set to a positive finite value"));
      if (cellSize<=0)     throw(AipsError("The cellsize parameter needs to be set to a positive finite value"));
      
      {
	Directory dirObj(modelImageName);
	if ((modelImageName!="") && ((!dirObj.exists()) || (!dirObj.isReadable())))
	  {
	    std::string msg;
	    msg="Model image \""+modelImageName + "\" not found.";
	    throw(AipsError(msg));
	  }
      }

//-------------------------------------------------------------------
      // Load the selected MS.  The original ms (thems), the selected
      // MS and the MSSelection objects are modified.  The selected
      // list of SPW and FIELD IDs are returned as a std::tuple.
      //
      MSSelection msSelection;
      MS theMS, selectedMS;
      {
	//	String msname=MSNBuf;
	Vector<int> spwid, fieldid;
	LogIO log_l(LogOrigin("tAWPHPG","main"));

	log_l << "Opening the MS (\"" << MSNBuf << "\"), applying data selection, " 
	      << "setting up data iterators...all that boring stuff."
	      << LogIO::POST;
	auto lists = loadMS(MSNBuf,
			    spwStr,
			    fieldStr,
			    uvDistStr,
			    theMS,
			    selectedMS,
			    msSelection);
	spwid=std::get<0>(lists);
	fieldid=std::get<1>(lists);
      }
      //
      //-------------------------------------------------------------------
      //

      //-------------------------------------------------------------------
      // Set up the data iterator
      //
      vi::VisibilityIterator2 vi2(selectedMS,vi::SortColumns(),true,0,10.0);
      {
	Matrix<Double> freqSelection= msSelection.getChanFreqList(NULL,true);
	//      vi2.setInterval(50.0);
	VisImagingWeight viw(weighting);
	vi2.useImagingWeight(viw);
	FrequencySelectionUsingFrame fsel(MFrequency::Types::LSRK);
	for(unsigned i=0;i<freqSelection.shape()(0); i++)
	  fsel.add(freqSelection(i,0), freqSelection(i,1), freqSelection(i,2));
	vi2.setFrequencySelection (fsel);
      }
      vi::VisBuffer2 *vb=vi2.getVisBuffer();

      //
      //-------------------------------------------------------------------
      // Make the empty grid with the sky image coordinates
      //
      Vector<Int> imSize(2,NX);
      String mode="mfs", startModelImageName="";
      
      // String gridName="cgrid.tmp";
      // if (cmplxGridName != "") gridName=cmplxGridName;

      casacore::MDirection pc;
      Int pcField = casa::refim::getPhaseCenter(selectedMS,pc);
      std::ostringstream oss;
      pc.print(oss);
      //      cerr << "PC = " << oss << endl;

      PagedImage<Complex> cgrid=makeEmptySkyImage(vi2, selectedMS, msSelection,
					       cmplxGridName, startModelImageName,
					       imSize, cellSize, phaseCenter,
					       stokes, refFreqStr, mode);
      PagedImage<Float> skyImage(cgrid.shape(),cgrid.coordinates(), imageName);
      if (cmplxGridName=="")
	cgrid.table().markForDelete();

      StokesImageUtil::From(cgrid, skyImage);
      if(vb->polarizationFrame()==MSIter::Linear) StokesImageUtil::changeCStokesRep(cgrid,StokesImageUtil::LINEAR);
      else StokesImageUtil::changeCStokesRep(cgrid, StokesImageUtil::CIRCULAR);

      //
      //-------------------------------------------------------------------
      //

      //-------------------------------------------------------------------
      // Create the AWP FTMachine.  The AWProjectionFT is construed
      // with the re-sampler depending on the ftmName (AWVisResampler
      // for ftmName='awproject' or AWVisResamplerHPG for
      // ftmName='awphpg')
      //
      CountedPtr<refim::FTMachine> ftm,iftm;
      //FTMachine * ftm=nullptr;
      MPosition loc;
      MeasTable::Observatory(loc, MSColumns(selectedMS).observation().telescopeName()(0));
      Bool useDoublePrec=true, aTermOn=true, psTermOn=false, mTermOn=false;

      createAWPFTMachine(ftmName, modelImageName, ftm, iftm,
			 String("EVLA"),
			 loc, cfCache, cfBufferSize, cfOversampling,WBAwp,nW,
			 useDoublePrec,
			 aTermOn,
			 psTermOn,
			 mTermOn,
			 doPointing,
			 doPBCorr,
			 conjBeams,
			 pbLimit,
			 posigdev
			 );
      {
	Matrix<Double> mssFreqSel;
	mssFreqSel  = msSelection.getChanFreqList(NULL,true);
	// Send in Freq info.
	//	cerr << "Sending frequency selection information " <<  mssFreqSel  <<  " to AWP FTM." << endl;
	ftm->setSpwFreqSelection( mssFreqSel );
	iftm->setSpwFreqSelection( mssFreqSel );
      }
      //
      //-------------------------------------------------------------------
      //

      //-------------------------------------------------------------------
      // Iterative over the MS, trigger the gridding.
      //
      {
	cgrid.set(Complex(0.0));
	Matrix<Float> weight;
	vi2.originChunks();
	vi2.origin();
	ftm->initializeToSky(cgrid, weight, *vb);
	//	cerr << "image.shape: " << cgrid.shape() << endl;
	refim::SimpleComponentFTMachine cft;
	
	timer.mark();
	int n=0;
	//Timer tvi;
	// Vector<refim::VBStore> vbsList;
	// vbsList.resize(10);
	unsigned long vol=0;
	ProgressMeter pm(1.0, selectedMS.nrow(),
			 "Gridding", "","","",true);

	casa::refim::FTMachine::Type dataCol=casa::refim::FTMachine::CORRECTED;
	if(selectedMS.tableDesc().isColumn("CORRECTED_DATA") ) dataCol = casa::refim::FTMachine::CORRECTED;
	else
	  {
	    LogIO log_l(LogOrigin("tAWPHPG","main"));
	    log_l << "CORRECTED_DATA column not found.  Using the DATA column instead." << LogIO::WARN << LogIO::POST;
	    dataCol = casa::refim::FTMachine::OBSERVED;
	  }

	for (vi2.originChunks();vi2.moreChunks(); vi2.nextChunk())
	  {
	    for (vi2.origin(); vi2.more(); vi2.next())
	      {
		//tvi.mark();

		// casacore::Cube<casacore::Complex> data(vb->visCube());
		// vol+=data.shape().product()*sizeof(casacore::Complex);

		//cft.get(*vb, cl);
		//vb->setVisCube(vb->visCubeModel());

		// Check if CORRECTED_DATA column exits.  Hope is that
		// the VisBuffer does reasonable caching (i.e., the
		// hope is that the code below does not read the data
		// twice).
		if (dataCol==casa::refim::FTMachine::CORRECTED)
		  vb->setVisCube(vb->visCubeCorrected());
		else
		  vb->setVisCube(vb->visCube());

		ftm->put(*vb);	

		vol+=vb->nRows();
		pm.update(Double(vol));
		n++;
		// cerr << "Iter " << n << " " << tvi.real() << endl;
		//		if (n > 0) break;
	      }
	    //	    if (n > 0) break;
	  }
	griddingTime += timer.real();
	cerr << "Gridding time: " << griddingTime << ". No. of rows processed: " << vol << ".  Data rate: " << vol/griddingTime << " rows/s" << endl;
	ftm->finalizeToSky();
	
	// Get the weights.  Do nothing with them for now.
	//Bool normalize=false;
	ftm->getImage(weight, normalize);
	// Convert the skyImage (retrived in ftm->finalizeToSky()) and
	// convert it from Feed basis to Stokes basis.
	StokesImageUtil::To(skyImage, cgrid);
	
	//	cerr << "val at center " << cgrid.getAt(IPosition(4, NX/2, NY/2, 0, 0)) << endl;
      }
      //detach the ms for cleaning up
      selectedMS=MeasurementSet();
    } 
  catch (AipsError& x) 
    {
      cerr << "AipsError exception: " << x.getMesg() << endl;
      //      RestartUI(RENTER);
      return(1);
    }
  catch(...)
    {
      throw(AipsError("Some kind of Exception caught"));
      //x << "###AipsError: " << "\t\"" <<  x.what() << "\"" << endl;
      //      RestartUI(RENTER);
      return(1);
    }
  cerr <<"OK" << endl;
  exit(0);
}

