//# SIMapper.cc: Implementation of SIMapper.h
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

#include <casa/Exceptions/Error.h>
#include <casa/iostream.h>
#include <casa/sstream.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>

#include <casa/Logging.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <casa/OS/DirectoryIterator.h>
#include <casa/OS/File.h>
#include <casa/OS/Path.h>

#include <casa/OS/HostInfo.h>
#include <lattices/Lattices/LatticeLocker.h>
#include <ms/MeasurementSets/MSHistoryHandler.h>
#include <ms/MeasurementSets/MeasurementSet.h>

#include <synthesis/TransformMachines/BeamSkyJones.h>
#include <synthesis/TransformMachines/SkyJones.h>
#include <synthesis/TransformMachines/SimpleComponentFTMachine.h>
#include <synthesis/TransformMachines2/SimpleComponentFTMachine.h>
#include <synthesis/TransformMachines/SimpCompGridMachine.h>
//#include <synthesis/ImagerObjects/SIMapperBase.h>
#include <synthesis/ImagerObjects/SIMapper.h>


#include <sys/types.h>
#include <unistd.h>
using namespace std;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  
  SIMapper::SIMapper( CountedPtr<SIImageStore>& imagestore, 
		      CountedPtr<FTMachine>& ftm, 
		      CountedPtr<FTMachine>& iftm) : useViVb2_p(false)
  {
    //    LogIO os( LogOrigin("SIMapper","Construct a mapper",WHERE) );
    ft_p=ftm;
    ift_p=iftm;
    ft2_p=NULL;
    ift2_p=NULL;
    cft_p=NULL;
    itsImages=imagestore;
  }
  SIMapper::SIMapper( CountedPtr<SIImageStore>& imagestore, 
		      CountedPtr<refim::FTMachine>& ftm, 
		      CountedPtr<refim::FTMachine>& iftm) : useViVb2_p(true)
  {
    //LogIO os( LogOrigin("SIMapper","Construct a mapper",WHERE) );
    ft2_p=ftm;
    ift2_p=iftm;
    cft_p=NULL;
    itsImages=imagestore;
  }

  SIMapper::SIMapper(const ComponentList& cl, 
		     String& whichMachine)
  {
    ft_p=NULL;
    ift_p=NULL;
    ft2_p=NULL;
    ift2_p=NULL;
    
    itsImages=NULL;

    if(whichMachine=="SimpleComponentFTMachine"){
      cft_p=new SimpleComponentFTMachine();
      cft2_p=new refim::SimpleComponentFTMachine();
    }
    else{
      //SD style component gridding
      cft_p=new SimpleComponentGridMachine();
      /////NEED to be done here
      //cft2_p=
    }
    cl_p=cl;
      
  }
  
  SIMapper::~SIMapper() 
  {
  }
  
  // #############################################
  // #############################################
  // #######  Gridding / De-gridding functions ###########
  // #############################################
  // #############################################
  void SIMapper::initializeGrid(vi::VisBuffer2& vb, Bool dopsf, Bool /*firstaccess*/)
  {
    
    //LogIO os( LogOrigin("SIMapper","initializeGrid",WHERE) );
    if(!useViVb2_p)
      throw(AipsError("Programmer Error: using vi2 mode with vi1 constructor"));
       //Componentlist FTM has nothing to do

       if(ift2_p.null())
       	return;

       ift2_p->initializeToSkyNew( dopsf, vb, itsImages);

       /////////////DEBUG
       //       CoordinateSystem csys = itsImages->getCSys();
       //       cout << "SIMapper : im spectral axis : " <<  csys.spectralCoordinate().referenceValue() << " at " << csys.spectralCoordinate().referencePixel() << " with increment " << csys.spectralCoordinate().increment() << endl;

     }

  void SIMapper::initializeGrid(VisBuffer& vb, Bool dopsf, Bool /*firstaccess*/)
     {

       //LogIO os( LogOrigin("SIMapper","initializeGrid",WHERE) );
       if(useViVb2_p)
	 throw(AipsError("Programmer Error: using vi1 mode with vi2 constructor"));
       //Componentlist FTM has nothing to do
       if(ift_p.null())
       	return;

       ift_p->initializeToSkyNew( dopsf, vb, itsImages);

       /////////////DEBUG
       //       CoordinateSystem csys = itsImages->getCSys();
       //       cout << "SIMapper : im spectral axis : " <<  csys.spectralCoordinate().referenceValue() << " at " << csys.spectralCoordinate().referencePixel() << " with increment " << csys.spectralCoordinate().increment() << endl;

     }


  void SIMapper::grid(vi::VisBuffer2& vb, Bool dopsf, refim::FTMachine::Type col,
		      const Int whichFTM)
   {
     //LogIO os( LogOrigin("SIMapper","grid",WHERE) );
     if(!useViVb2_p)
       throw(AipsError("Programmer Error: using vi2 mode with vi1 constructor"));
     //Componentlist FTM has no gridding to do
     (void)whichFTM;

     if(ift2_p.null())
       return;
     
     ift2_p->put(vb, -1, dopsf, col);
     
   }

  /////////////////OLD vi/vb version
  void SIMapper::grid(VisBuffer& vb, Bool dopsf, FTMachine::Type col,
		      const Int whichFTM)
   {
     //LogIO os( LogOrigin("SIMapper","grid",WHERE) );
      if(useViVb2_p)
	 throw(AipsError("Programmer Error: using vi1 mode with vi2 constructor"));
     //Componentlist FTM has no gridding to do
     (void)whichFTM;

     if(ift_p.null())
       return;
     
     ift_p->put(vb, -1, dopsf, col);
     
   }

  void SIMapper::finalizeGrid(vi::VisBuffer2& vb, Bool dopsf)
    {
      //LogIO os( LogOrigin("SIMapper","finalizeGrid",WHERE) );
      if(!useViVb2_p)
	 throw(AipsError("Programmer Error: using vi2 mode with vi1 constructor"));
      if(ift2_p.null())
      	return;

      ift2_p->finalizeToSkyNew( dopsf, vb, itsImages );

    }
  //////////////OLD VI/VB version
  void SIMapper::finalizeGrid(VisBuffer& vb, Bool dopsf)
    {
      //LogIO os( LogOrigin("SIMapper","finalizeGrid",WHERE) );
       if(useViVb2_p)
	 throw(AipsError("Programmer Error: using vi1 mode with vi2 constructor"));
      if(ift_p.null())
      	return;

      ift_p->finalizeToSkyNew( dopsf, vb, itsImages );

    }

  void SIMapper::initializeDegrid(vi::VisBuffer2& vb, const Int /*row*/)
  {
    //LogIO os( LogOrigin("SIMapper", "initializeDegrid",WHERE) );
     if(!useViVb2_p)
       throw(AipsError("Programmer Error: using vi2 mode with vii constructor")); 
    if(ft2_p.null() && cft2_p.null())
      return;
    
    if(ft2_p)
      ft2_p->initializeToVisNew(vb, itsImages);

  }
  //////////////////OLD vi/vb version
  void SIMapper::initializeDegrid(VisBuffer& vb, const Int /*row*/)
  {
    //LogIO os( LogOrigin("SIMapper", "initializeDegrid",WHERE) );
     if(useViVb2_p)
       throw(AipsError("Programmer Error: using vi1 mode with vi2 constructor"));

    if(ft_p.null() && cft_p.null())
      return;
    if(ft_p)
      ft_p->initializeToVisNew(vb, itsImages);

  }

  void SIMapper::degrid(vi::VisBuffer2& vb)
    {
      //LogIO os( LogOrigin("SIMapper","degrid",WHERE) );
      if(!useViVb2_p)
       throw(AipsError("Programmer Error: using vi2 mode with vi1 constructor"));

    
      ///This should not be called even but heck let's ignore
      if(ft2_p.null() and cft2_p.null())
      	return;

      Cube<Complex> origCube;
      origCube.assign(vb.visCubeModel()); 

      
      
      Cube<Complex> mod(vb.nCorrelations(), vb.nChannels(), vb.nRows(), Complex(0.0));
      vb.setVisCubeModel( mod);
      if( ! ft2_p.null() ) { ft2_p->get(vb); }
      if( ! cft2_p.null() ) { cft2_p->get(vb, cl_p); }
      
      origCube+=vb.visCubeModel();
      vb.setVisCubeModel(origCube);

    }

  void SIMapper::addPB(vi::VisBuffer2& vb, PBMath& pbMath, const MDirection& altDir, const Bool useAltDir)
  {
    CoordinateSystem imageCoord=itsImages->pb()->coordinates();
     
    IPosition imShape=itsImages->pb()->shape();

    

    MDirection wcenter;
    if(useAltDir)
      wcenter=altDir;
    else{
      MSColumns mscol(vb.ms());
      wcenter=mscol.field().phaseDirMeas(vb.fieldId()(0), vb.time()(0));
    }
    TempImage<Float> pbTemp(imShape, imageCoord);
    TempImage<Complex> ctemp(imShape, imageCoord);
    ctemp.set(1.0);
    pbMath.applyPB(ctemp, ctemp, wcenter, Quantity(0.0, "deg"), BeamSquint::NONE);
    StokesImageUtil::To(pbTemp, ctemp);
    LatticeLocker lock1(*(itsImages->pb()), FileLocker::Write);
    itsImages->pb()->copyData(  (LatticeExpr<Float>)((*(itsImages->pb()))+pbTemp) );

  }//addPB
  


  ////////////////////Old vi/Vb version

  void SIMapper::degrid(VisBuffer& vb)
    {
      //LogIO os( LogOrigin("SIMapper","degrid",WHERE) );
    
     if(useViVb2_p)
       throw(AipsError("Programmer Error: using vi1 mode with vi2 constructor"));
      ///This should not be called even but heck let's ignore
      if(ft_p.null() and cft_p.null())
      	return;

      Cube<Complex> origCube;
      origCube.assign(vb.modelVisCube()); 

      vb.setModelVisCube( Complex(0.0,0.0) );
      
      if( ! ft_p.null() ) { ft_p->get(vb); }
      if( ! cft_p.null() ) { cft_p->get(vb, cl_p); }
      
      vb.modelVisCube()+=origCube;

    }


  void SIMapper::finalizeDegrid()
  {
    //LogIO os( LogOrigin("SIMapper","finalizeDegrid",WHERE) );
    if(!useViVb2_p)
      ft_p->finalizeToVis();
    else
      ft2_p->finalizeToVis();
  }


  Bool SIMapper::getFTMRecord(Record& rec, const String diskimage)
  {
    //LogIO os( LogOrigin("SIMapper","getFTMRecord",WHERE) );
    if(ft_p.null() && !useViVb2_p)
    	return false;
     if(ft2_p.null() && useViVb2_p)
    	return false;
    String err;
    if(!useViVb2_p)
      return ft_p->toRecord(err, rec, true, diskimage);
    return ft2_p->toRecord(err, rec, true, diskimage);
    // rec = itsFTM->toRecord();

  }
  Bool SIMapper::getCLRecord(Record& rec)
  {
    if(cft_p.null() && cft2_p.null())
      return false;
    String err;
    return cl_p.toRecord(err, rec);
  }

  
  void SIMapper::initPB()
  {
    LatticeLocker lock1(*(itsImages->pb()), FileLocker::Write);
    itsImages->pb()->set(0.0);
  }

  void SIMapper::addPB(VisBuffer& vb, PBMath& pbMath)
  {
    CoordinateSystem imageCoord=itsImages->pb()->coordinates();
     
    IPosition imShape=itsImages->pb()->shape();
    
    MDirection wcenter=vb.msColumns().field().phaseDirMeas(vb.fieldId());
    TempImage<Float> pbTemp(imShape, imageCoord);
    TempImage<Complex> ctemp(imShape, imageCoord);
    ctemp.set(1.0);
    pbMath.applyPB(ctemp, ctemp, wcenter, Quantity(0.0, "deg"), BeamSquint::NONE);
    StokesImageUtil::To(pbTemp, ctemp);
    LatticeLocker lock1(*(itsImages->pb()), FileLocker::Write);
    itsImages->pb()->copyData(  (LatticeExpr<Float>)((*(itsImages->pb()))+pbTemp) );

  }//addPB
  

} //# NAMESPACE CASA - END


