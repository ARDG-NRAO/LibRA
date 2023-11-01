//# SIMapperCollection.cc: Implementation of Imager.h
//# Copyright (C) 1997-2008
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

#include <casacore/casa/Exceptions/Error.h>
#include <iostream>
#include <sstream>

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>

#include <casacore/casa/Logging.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>

#include <casacore/casa/OS/DirectoryIterator.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>

#include <casacore/casa/OS/HostInfo.h>

#include <casacore/ms/MeasurementSets/MSHistoryHandler.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>

#include <synthesis/ImagerObjects/SIMapperCollection.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>

#include <synthesis/TransformMachines/VisModelData.h>
#include <casacore/images/Regions/WCBox.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>

#include <sys/types.h>
#include <unistd.h>
using namespace std;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  
  SIMapperCollection::SIMapperCollection() 
  {
    LogIO os( LogOrigin("SIMapperCollection","Construct a mapperCollection",WHERE) );
    
    itsMappers.resize(0);
    oldMsId_p=-1;
    itsIsNonZeroModel=false;

  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  SIMapperCollection::~SIMapperCollection() 
  {
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  Bool SIMapperCollection::releaseImageLocks() 
  {
    Bool validflag=true;
    for(Int mapperid=0;mapperid<nMappers();mapperid++ )
      {
	validflag &= itsMappers[mapperid]->releaseImageLocks();
      }
    return validflag;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<String> SIMapperCollection::cleanupTempFiles(const String& mess) 
  {
    std::vector<String> outstr;
    auto appvectors = [](std::vector<String>&  a, const std::vector<String> b) {  a.insert(std::end(a), std::begin(b), std::end(b));};
    for(Int mapperid=0;mapperid<nMappers();mapperid++ )
      {
        if((itsMappers[mapperid]->getFTM2())){
          appvectors( outstr, (itsMappers[mapperid]->getFTM2(True)->cleanupTempFiles(mess)).tovector());
          appvectors(outstr, (itsMappers[mapperid]->getFTM2(False)->cleanupTempFiles(mess)).tovector());
        }
      }
    return outstr;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  
  /*
  // Allocate Memory and open images.
  void SIMapperCollection::addMapper( String mappertype,  
				      CountedPtr<SIImageStore> imagestore,
				      CountedPtr<FTMachine> ftmachine,
				      CountedPtr<FTMachine> iftmachine)
  {

    LogIO os( LogOrigin("SIMapperCollection","addMapper",WHERE) );

    CountedPtr<SIMapper> localMapper=NULL;
    Int nMappers = itsMappers.nelements();
    // Check 'mappertype' for valid types....
    if( mappertype == "default" )
      {
	localMapper = new SIMapperSingle( imagestore, ftmachine, iftmachine, nMappers );
      }
    
    else if( mappertype == "multiterm" )
      {
	localMapper = new SIMapperMultiTerm( imagestore, ftmachine, iftmachine,nMappers, ntaylorterms );
      }
    
    else
      {
	throw ( AipsError("Internal Error : Unrecognized Mapper Type in MapperCollection.addMapper") );
      }

    // If all is well, add to the list.
    itsMappers.resize(nMappers+1, true);
    itsMappers[nMappers] = localMapper;

  }
  */

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  void SIMapperCollection::addMapper( CountedPtr<SIMapper> map){
    Int nMappers = itsMappers.nelements();
    itsMappers.resize(nMappers+1, true);
    itsMappers[nMappers]=map;
  } 

  Int SIMapperCollection::nMappers()
  {
    return itsMappers.nelements();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  Vector<String> SIMapperCollection::getImageNames()
  {
    Vector<String> names( nMappers() );

    for(Int mapperid=0;mapperid<nMappers();mapperid++ )
      {
	names[mapperid] = itsMappers[mapperid]->getImageName();
      }

    return names;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////  Start VB dependent code /////////////////////////////////////////////
  void SIMapperCollection::initializeGrid(vi::VisBuffer2& vb, Bool dopsf, const Int mapperid)
  {
    if(mapperid<0)
      {
	for (uInt k=0; k < itsMappers.nelements(); ++k)
	  {
	    (itsMappers[k])->initializeGrid(vb,dopsf,true);
  	  }
      }
    else 
      {
	if (mapperid > (Int)itsMappers.nelements())
	  throw ( AipsError("Internal Error : SIMapperCollection::initializeGrid(): mapperid out of range") );
	else itsMappers[mapperid]->initializeGrid(vb, dopsf, true);
      }
  }

void SIMapperCollection::initializeGrid(vi::VisibilityIterator2& vi, Bool dopsf, const Int mapperid)
  {

    vi::VisBuffer2 *vb=vi.getVisBuffer();
    initializeGrid(*vb, dopsf, mapperid);
    if(mapperid<0)
      {
	for (uInt k=0; k < itsMappers.nelements(); ++k)
	  {
	    ((itsMappers[k])->getFTM2())->initBriggsWeightor(vi);
  	  }
      }
    else 
      {
	if (mapperid > (Int)itsMappers.nelements())
	  throw ( AipsError("Internal Error : SIMapperCollection::initializeGrid(): mapperid out of range") );
	else (itsMappers[mapperid]->getFTM2())->initBriggsWeightor(vi);
      }
  }

  ////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////OLD vi/vb //////////////////////////////////////////////
  void SIMapperCollection::initializeGrid(VisBuffer& vb, Bool dopsf, const Int mapperid)
  {
    if(mapperid<0)
      {
	for (uInt k=0; k < itsMappers.nelements(); ++k)
	  {
	    (itsMappers[k])->initializeGrid(vb,dopsf,true);
  	  }
      }
    else 
      {
	if (mapperid > (Int)itsMappers.nelements())
	  throw ( AipsError("Internal Error : SIMapperCollection::initializeGrid(): mapperid out of range") );
	else itsMappers[mapperid]->initializeGrid(vb, dopsf, true);
      }
  }

  //////////////////////////////////////////////////////////////////////////////

  void SIMapperCollection::grid(vi::VisBuffer2& vb, Bool dopsf, refim::FTMachine::Type col,
				Int mapperid)
  {
    if( itsIsNonZeroModel == true ) // Try to subtract model visibilities only if a model exists.
	{
	  if(col==refim::FTMachine::CORRECTED){
	//Dang i thought the new vb will return Data or FloatData if correctedData was
	//not there
	    if(MSMainColumns(vb.ms()).correctedData().isNull()){
	      col=refim::FTMachine::OBSERVED;
	      //			  cerr << "Max of visCube" << max(vb.visCube()) << " model " << max(vb.modelVisCube())<< endl;
	      vb.setVisCube(vb.visCube()-vb.visCubeModel());
	    }
	    else{
	      vb.setVisCubeCorrected(vb.visCubeCorrected()-vb.visCubeModel());
	    }
	  } 
	  else if (col==refim::FTMachine::OBSERVED) {
	    vb.setVisCube(vb.visCube()-vb.visCubeModel());
	  }
	}// if non zero model

    if(col==refim::FTMachine::CORRECTED &&
       MSMainColumns(vb.ms()).correctedData().isNull()){
      //cout << "Corrected column isn't there, using data instead" << endl;
      col=refim::FTMachine::OBSERVED;
    }
    
    if (mapperid < 0)
      {
	//cout << "Using column : " << col << endl;
	for (uInt k=0; k < itsMappers.nelements(); ++k)
	  {
	    (itsMappers[k])->grid(vb, dopsf, col);
	  }
      }
    else 
      {
	if (mapperid > (Int)itsMappers.nelements())
	  throw ( AipsError("Internal Error : SIMapperCollection::grid(): mapperid out of range") );
	else itsMappers[mapperid]->grid(vb, dopsf, col);
      }
  }

///////////////////
  ///////////////////////////////////////OLD VI/VB ///////////////////////////////////
  void SIMapperCollection::grid(VisBuffer& vb, Bool dopsf, FTMachine::Type col,
				Int mapperid)
  {
    if( itsIsNonZeroModel == true ) // Try to subtract model visibilities only if a model exists.
	{
	  if(col==FTMachine::CORRECTED){
		  if(vb.msColumns().correctedData().isNull()){
			  col=FTMachine::OBSERVED;
			  //			  cerr << "Max of visCube" << max(vb.visCube()) << " model " << max(vb.modelVisCube())<< endl;
			  vb.visCube()-=vb.modelVisCube();
		  }
		  else{
			  vb.correctedVisCube()-=vb.modelVisCube();
		  }
	  } 
	  else if (col==FTMachine::OBSERVED) {
			  vb.visCube()-=vb.modelVisCube();
	    }
	}// if non zero model

    if(col==FTMachine::CORRECTED && vb.msColumns().correctedData().isNull())
      {	    col=FTMachine::OBSERVED;}

    if (mapperid < 0)
      {
	for (uInt k=0; k < itsMappers.nelements(); ++k)
	  {
	    (itsMappers[k])->grid(vb, dopsf, col);
	    
	  }
      }
    else 
      {
	if (mapperid > (Int)itsMappers.nelements())
	  throw ( AipsError("Internal Error : SIMapperCollection::grid(): mapperid out of range") );
	else itsMappers[mapperid]->grid(vb, dopsf, col);
      }
  }
  ///////////////////////////////
  ////////////////////////////////
  void SIMapperCollection::finalizeGrid(vi::VisBuffer2& vb, Bool dopsf,const Int mapperid)
  {
    if(mapperid<0)
      {
	for (uInt k=0; k < itsMappers.nelements(); ++k)
      	  {
	    (itsMappers[k])->finalizeGrid(vb, dopsf);
      	  }
      }
    else 
      {
	if (mapperid > (Int)itsMappers.nelements())
	  throw ( AipsError("Internal Error : SIMapperCollection::finalizeGrid(): mapperid out of range") );
	else itsMappers[mapperid]->finalizeGrid(vb, dopsf);
      }
  }
  

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////OLD VI/VB////////////////////////////////////////////////
  void SIMapperCollection::finalizeGrid(VisBuffer& vb, Bool dopsf,const Int mapperid)
  {
    if(mapperid<0)
      {
	for (uInt k=0; k < itsMappers.nelements(); ++k)
      	  {
	    (itsMappers[k])->finalizeGrid(vb, dopsf);
      	  }
      }
    else 
      {
	if (mapperid > (Int)itsMappers.nelements())
	  throw ( AipsError("Internal Error : SIMapperCollection::finalizeGrid(): mapperid out of range") );
	else itsMappers[mapperid]->finalizeGrid(vb, dopsf);
      }
  }
  

//////////////////////////////////////////////////////////////////////////////////////////////////////
  void SIMapperCollection::initializeDegrid(vi::VisBuffer2& vb, const Int mapperid)
    {

      itsIsNonZeroModel = anyNonZeroModels();

      if( itsIsNonZeroModel == true )
	{
	  //	  vb.setModelVisCube( Complex(0.0,0.0) );
	  
	  if(mapperid<0)
	    {
	      for (uInt k=0; k < itsMappers.nelements(); ++k)
		{
    		  (itsMappers[k])->initializeDegrid(vb);
		}
	    }
	  else 
	    {
	      if (mapperid > (Int)itsMappers.nelements())
		throw ( AipsError("Internal Error : SIMapperCollection::initializeDegrid(): mapperid out of range") );
	      else itsMappers[mapperid]->initializeDegrid(vb);
	    }
	  
	}// if non zero model 
    }



///////////////////////////////////////OLD VI/VB ///////////////////////////////////////////////////
  void SIMapperCollection::initializeDegrid(VisBuffer& vb, const Int mapperid)
    {

      itsIsNonZeroModel = anyNonZeroModels();

      if( itsIsNonZeroModel == true )
	{
	  //	  vb.setModelVisCube( Complex(0.0,0.0) );
	  
	  if(mapperid<0)
	    {
	      for (uInt k=0; k < itsMappers.nelements(); ++k)
		{
    		  (itsMappers[k])->initializeDegrid(vb);
		}
	    }
	  else 
	    {
	      if (mapperid > (Int)itsMappers.nelements())
		throw ( AipsError("Internal Error : SIMapperCollection::initializeDegrid(): mapperid out of range") );
	      else itsMappers[mapperid]->initializeDegrid(vb);
	    }
	  
	}// if non zero model 
    }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  void SIMapperCollection::degrid(vi::VisBuffer2& vb, Bool saveVirtualMod, const Int mapperid)
    {
      if( itsIsNonZeroModel == true )
	{
	  if(mapperid<0)
	    {
	      for (uInt k=0; k < itsMappers.nelements(); ++k)
		{
		  (itsMappers[k])->degrid(vb);
		}
	    }
	  else 
	    {
	      if (mapperid > (Int)itsMappers.nelements())
		throw ( AipsError("Internal Error : SIMapperCollection::degrid(): mapperid out of range") );
	      else itsMappers[mapperid]->degrid(vb);
	    }
	  
	  if(saveVirtualMod){
	    saveVirtualModel(vb);
	  }
	}// if non zero model
    }
  
  void SIMapperCollection::addPB(vi::VisBuffer2& vb, PBMath& pbMath, const MDirection& altDir, const Bool useAltDir)
  {
    for (uInt k=0; k < itsMappers.nelements(); ++k)
      {
	(itsMappers[k])->addPB(vb,pbMath, altDir, useAltDir);
	
      }
  }


  /////////////////////////////////////OLD VI/VB ////////////////////////////////////////////////////
  void SIMapperCollection::degrid(VisBuffer& vb, Bool saveVirtualMod, const Int mapperid)
    {
      if( itsIsNonZeroModel == true )
	{
	  if(mapperid<0)
	    {
	      for (uInt k=0; k < itsMappers.nelements(); ++k)
		{
		  (itsMappers[k])->degrid(vb);
		}
	    }
	  else 
	    {
	      if (mapperid > (Int)itsMappers.nelements())
		throw ( AipsError("Internal Error : SIMapperCollection::degrid(): mapperid out of range") );
	      else itsMappers[mapperid]->degrid(vb);
	    }
	  
	  if(saveVirtualMod){
	    saveVirtualModel(vb);
	  }
	}// if non zero model
    }
  

  /////
  /////////////
  void SIMapperCollection::saveVirtualModel(VisBuffer& vb){



	  if(vb.msId() != oldMsId_p){
		  oldMsId_p=vb.msId();
		  /*Block< Vector<Int> > blockNGroup;
    				Block< Vector<Int> > blockStart;
    				Block< Vector<Int> > blockWidth;
    				Block< Vector<Int> > blockIncr;
    				Block< Vector<Int> > blockSpw;
    				vb.getChannelSelection(blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);
    				Vector<Int> fields = vb.msColumns().fieldId().getColumn();
    				const Int option = Sort::HeapSort | Sort::NoDuplicates;
    				const Sort::Order order = Sort::Ascending;
    				Int nfields = GenSort<Int>::sort (fields, order, option);

    				// Make sure  we have the right size

    				fields.resize(nfields, true);
		   */
		  //Int msid = vb.msId();
		  ROVisibilityIterator *viloc=vb.getVisibilityIterator();
		  for (uInt k=0; k < itsMappers.nelements(); ++k){
			  Record rec;
			  String modImage=viloc->ms().getPartNames()[0];
			  if(!((viloc->ms()).source().isNull()))
			    modImage=(viloc->ms()).source().tableName();
			  modImage=File::newUniqueName(modImage, "FT_MODEL").absoluteName();
			  Bool iscomp=itsMappers[k]->getCLRecord(rec);
			  if(iscomp || itsMappers[k]->getFTMRecord(rec, modImage)){

				  //VisModelData::putModel(vb.getVisibilityIterator()->ms(), rec, fields, blockSpw[msid], blockStart[msid],
				  //		blockWidth[msid], blockIncr[msid],
				  //                         iscomp, true);
				  VisibilityIterator * elvi=(dynamic_cast<VisibilityIterator* >(vb.getVisibilityIterator()));
				  if(elvi)
					  elvi->putModel(rec, iscomp, true);
				  //				  VisModelData::listModel(vb.getVisibilityIterator()->ms());
			  }

		  }



	  }



  }

  void SIMapperCollection::saveVirtualModel(vi::VisBuffer2& vb){



	  if(vb.msId() != oldMsId_p){
	    oldMsId_p=vb.msId();
	    
	    for (uInt k=0; k < itsMappers.nelements(); ++k){
	      Record rec;
	      String modImage=vb.ms().getPartNames()[0];
	      if(!((vb.ms()).source().isNull()))
		modImage=(vb.ms()).source().tableName();
	      modImage=File::newUniqueName(modImage, "FT_MODEL").absoluteName();
	      Bool iscomp=itsMappers[k]->getCLRecord(rec);
	      if(iscomp || itsMappers[k]->getFTMRecord(rec, modImage)){

		////Darn not implemented  
		//static_cast<VisibilityIteratorImpl2 *>(viloc->getImpl())->writeModel(rec, //iscomp, true);

                if(!iscomp && Table::isReadable(modImage)){
                  //make sure complex image is of compliant size/shape
                  (itsMappers[k]->imageStore())->intersectComplexImage(modImage);

                }
                VisibilityIterator2* vi=const_cast<VisibilityIterator2*>(vb.getVi());
                const_cast<MeasurementSet& >(vi->ms()).lock();
                /////TESTOO
                //Int CPUID;
                //MPI_Comm_rank(MPI_COMM_WORLD, &CPUID);
                //cerr << CPUID << " writing " << modImage << endl;
                /////////////////
                vi->writeModel(rec, iscomp, true);
                const_cast<MeasurementSet& >(vi->ms()).unlock();
				  //				  VisModelData::listModel(vb.getVisibilityIterator()->ms());
			  }

		  }



	  }



  }


  /////////////////////////////////////////////////////////
  void SIMapperCollection::finalizeDegrid(vi::VisBuffer2& /*vb*/, const Int mapperid)
    {
      if( itsIsNonZeroModel == true )
	{
	  if(mapperid<0)
	    {
	      for (uInt k=0; k < itsMappers.nelements(); ++k)
		{
  		  (itsMappers[k])->finalizeDegrid();
		  
		}
	    }
	  else 
	    {
	      if (mapperid > (Int)itsMappers.nelements())
		throw ( AipsError("Internal Error : SIMapperCollection::finalizeDegrid(): mapperid out of range") );
	      else itsMappers[mapperid]->finalizeDegrid();
	    }
	}// if non zero model
    }
  
 ////////////////////////////////////////////////////////
  void SIMapperCollection::finalizeDegrid(VisBuffer& /*vb*/, const Int mapperid)
    {
      if( itsIsNonZeroModel == true )
	{
	  if(mapperid<0)
	    {
	      for (uInt k=0; k < itsMappers.nelements(); ++k)
		{
  		  (itsMappers[k])->finalizeDegrid();
		  
		}
	    }
	  else 
	    {
	      if (mapperid > (Int)itsMappers.nelements())
		throw ( AipsError("Internal Error : SIMapperCollection::finalizeDegrid(): mapperid out of range") );
	      else itsMappers[mapperid]->finalizeDegrid();
	    }
	}// if non zero model
    }
  
  
  void SIMapperCollection::initPB()
  {
    for (uInt k=0; k < itsMappers.nelements(); ++k)
      {
	(itsMappers[k])->initPB();
      }
  }
  
  void SIMapperCollection::addPB(VisBuffer& vb, PBMath& pbMath)
  {
    for (uInt k=0; k < itsMappers.nelements(); ++k)
      {
	(itsMappers[k])->addPB(vb,pbMath);
	
      }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////// End of VB dependent code.
  ///////////////////////////////////////////////////////////////////////////////////////////////////////



  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////
  CountedPtr<SIImageStore> SIMapperCollection::imageStore(Int id)
  {
	if(uInt(id) < itsMappers.nelements())
	{
		return (itsMappers[id])->imageStore();
	}
	return make_shared<SIImageStore>( );
  }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
  Record SIMapperCollection::getFTMRecord(Int mapperid)
  {
    AlwaysAssert( mapperid >=0 && mapperid < nMappers() , AipsError );
    //return itsMappers[mapperid]->getFTMRecord();
    Record rec;
    return rec;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  void SIMapperCollection::checkOverlappingModels(String action)
  {
    // If nothing that overlaps, don't check.
    if(nMappers()==1) return;

    Int nmodels = nMappers();

    // If there is no model image (i.e. first major cycle with no starting model), don't check.
    Bool hasmodel=true;
    for (Int model=0;model<(nmodels-1); ++model) 
      { hasmodel = hasmodel && ((itsMappers[model])->imageStore())->hasModel();  }
    if( hasmodel==false ) { 
      //cout << "No model images to check overlap for." << endl; 
      return; 
    }

    // Internal check
    AlwaysAssert( action=="blank" || action=="restore" , AipsError );

    for (Int model=0;model<(nmodels-1); ++model) 
      {
	// Connect to one image for aux info.
        LatticeLocker lockmodel (*(((itsMappers[model])->imageStore())->model()), FileLocker::Write);
	SubImage<Float> modelimage( *(((itsMappers[model])->imageStore())->model()), true );

	uInt nTaylor0 = ((itsMappers[model])->imageStore())->getNTaylorTerms();

	CoordinateSystem cs0=modelimage.coordinates();
	IPosition iblc0(modelimage.shape().nelements(),0);
	IPosition itrc0(modelimage.shape());
	itrc0=itrc0-Int(1);
	LCBox lbox0(iblc0, itrc0, modelimage.shape());
	ImageRegion imagreg0(WCBox(lbox0, cs0));

	for (Int nextmodel=model+1; nextmodel < nmodels; ++nextmodel)
	  {
            LatticeLocker nextlockmodel (*(((itsMappers[nextmodel])->imageStore())->model()), FileLocker::Write);
	    SubImage<Float> nextmodelimage( *(((itsMappers[nextmodel])->imageStore())->model()) , true);

	    uInt nTaylor1 = ((itsMappers[nextmodel])->imageStore())->getNTaylorTerms();
	    
	    CoordinateSystem cs=nextmodelimage.coordinates();
	    IPosition iblc(nextmodelimage.shape().nelements(),0);
	    IPosition itrc(nextmodelimage.shape());
	    itrc=itrc-Int(1);
	    LCBox lbox(iblc, itrc, nextmodelimage.shape());
	    ImageRegion imagreg(WCBox(lbox, cs));
	    
	    try{

	      if( action.matches("blank") )
		{

                  //cerr << "blank MODEL image shape " << modelimage.shape() << "  " << nextmodelimage.shape() << endl;
                  
		  LatticeRegion latReg=imagreg.toLatticeRegion(modelimage.coordinates(), modelimage.shape());

		  for(uInt taylor=0;taylor<min(nTaylor0,nTaylor1);taylor++)
		    { // loop for taylor term
		      SubImage<Float> modelim( *(((itsMappers[model])->imageStore())->model(taylor)), true );
		      SubImage<Float> partToMask(modelim, imagreg, true);
                      LatticeLocker lock1 (partToMask, FileLocker::Write);
		      ArrayLattice<Bool> pixmask(latReg.get());
		      LatticeExpr<Float> myexpr(iif(pixmask, 0.0, partToMask) );
		      partToMask.copyData(myexpr);
		    }


		}
	      else // "restore"
		{
                  //cerr << "rsetore MODEL image shape " << modelimage.shape() << "  " << nextmodelimage.shape() << endl;
		LatticeRegion latReg0=imagreg0.toLatticeRegion(nextmodelimage.coordinates(), nextmodelimage.shape());
		LatticeRegion latReg=imagreg.toLatticeRegion(modelimage.coordinates(), modelimage.shape());
		ArrayLattice<Bool> pixmask(latReg.get());

		
		for(uInt taylor=0;taylor<min(nTaylor0,nTaylor1);taylor++)
		  {// loop for taylor term
		    SubImage<Float> modelim( *(((itsMappers[model])->imageStore())->model(taylor)), true );
		    SubImage<Float> nextmodelim( *(((itsMappers[nextmodel])->imageStore())->model(taylor)), true );

		    SubImage<Float> partToMerge(nextmodelim, imagreg0, true);
		    SubImage<Float> partToUnmask(modelim, imagreg, true);
                    LatticeLocker lock1 (partToUnmask, FileLocker::Write);
                    LatticeLocker lock2 (partToMerge, FileLocker::Write);
		    LatticeExpr<Float> myexpr0(iif(pixmask,partToMerge,partToUnmask));
		    partToUnmask.copyData(myexpr0);
		  }
		
		}
	    }
	    catch(AipsError &x){
	      //	      cout << "Hmm.... in here : "<< x.getMesg() << endl;
	      //no overlap you think ?
	      /*
		os << LogIO::WARN
		<< "no overlap or failure of copying the clean components"
		<< x.getMesg()
		<< LogIO::POST;
	*/
	      continue;
	    }
	  }
      }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  Long SIMapperCollection::estimateRAM(){
    Long mem=0;
    	for (uInt k=0; k < itsMappers.nelements(); ++k)
	  {
            if(! ((itsMappers[k])->getFTM2()))
              throw(AipsError("No VI/VB2 FTMachine set"));
            ///IFT
            if(((itsMappers[k])->getFTM2())->estimateRAM(((itsMappers[k])->imageStore()))> 0){
              mem+=((itsMappers[k])->getFTM2())->estimateRAM(((itsMappers[k])->imageStore()));
            //FT
              mem+=((itsMappers[k])->getFTM2(False))->estimateRAM(((itsMappers[k])->imageStore()));
            }
            else{
              //Assuming double precision...ignoring padding etc.
              mem+=6*sizeof(float)*(((itsMappers[k])->imageStore())->getShape().product());
            }
            //Imagestorages
            mem+=((itsMappers[k])->imageStore())->estimateRAM();
  	  }
        return mem;
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  Bool SIMapperCollection::anyNonZeroModels()
  {
    Bool validmodel=false;
    // If any one Mapper has a valid and nonzero model, return true.
    for (Int model=0;model<nMappers(); ++model) 
      { 
	validmodel |= (! ( ((itsMappers[model])->imageStore())->isModelEmpty() ));
      }
    return validmodel;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////

} //# NAMESPACE CASA - END

