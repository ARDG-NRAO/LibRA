//# CubeMinorCycleAlgorithm.cc: implementation of class to deconvolve cube in parallel/serial 
//# Copyright (C) 2019
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//# License for more details.
//#
//# https://www.gnu.org/licenses/
//#
//# Queries concerning CASA should be submitted at
//#        https://help.nrao.edu
//#
//#        Postal address: CASA Project Manager 
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#include <casacore/lattices/Lattices/LatticeLocker.h>
#include <synthesis/ImagerObjects/CubeMinorCycleAlgorithm.h>
#include <synthesis/ImagerObjects/SynthesisDeconvolver.h>
#include <casacore/casa/Containers/Record.h>
#include <synthesis/ImagerObjects/SimpleSIImageStore.h>
#include <imageanalysis/Utilities/SpectralImageUtil.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
extern Applicator applicator;

  CubeMinorCycleAlgorithm::CubeMinorCycleAlgorithm() : myName_p("CubeMinorCycleAlgorithm"), autoMaskOn_p(False),chanFlag_p(0), status_p(False){
	
}
CubeMinorCycleAlgorithm::~CubeMinorCycleAlgorithm() {
	
}
	
void CubeMinorCycleAlgorithm::get() {
  ///New instructions reset previous state
  reset();
	//cerr << "in get for child process " << applicator.isWorker() << endl;
	Record decParsRec;
     
	// get deconv params record #1
	applicator.get(decParsRec);
	// get iter control rec #2
	applicator.get(iterBotRec_p);
	// channel range to deconvolve #3
	applicator.get(chanRange_p);
	//get psf image name #4
	applicator.get(psfName_p);
	//get residual name #5
	applicator.get(residualName_p);
	//get model name #6
	applicator.get(modelName_p);
	//get mask name #7
	applicator.get(maskName_p);
        // get pb name #8
        applicator.get(pbName_p);
        //get beamsetrec #9
        //applicator.get(beamsetRec_p);
        //get psfsidelobelev #9
        applicator.get(psfSidelobeLevel_p);
        //get chanflag #10
        Record chanflagRec;
        applicator.get(chanflagRec);
        chanFlag_p.resize();
        chanflagRec.get("chanflag", chanFlag_p);
        statsRec_p=chanflagRec.asRecord("statsrec");
	//cerr <<"GET statsRec " << statsRec_p << endl;
	decPars_p.fromRecord(decParsRec);
	

	
}
void CubeMinorCycleAlgorithm::put() {
	
  ///# 1  chanrange processed 
  applicator.put(chanRange_p);
	//cerr << "in put " << status_p << endl;
  //#2 chanflag
  chanFlagRec_p.define("chanflag", chanFlag_p);
  //cerr << "PUT statsRec "<< statsRec_p << endl; 
  chanFlagRec_p.defineRecord("statsrec", statsRec_p);
  applicator.put(chanFlagRec_p);
  ///#3 return record of deconvolver
  // cerr << "nfield " << returnRec_p.nfields() << endl;
  applicator.put(returnRec_p);	
	
}
	
void CubeMinorCycleAlgorithm::task(){
	LogIO logger(LogOrigin("CubeMinorCycleAlgorithm", "task", WHERE));

	status_p = False;
	try{
	 
          SynthesisDeconvolver subDeconv;
	  Bool writeBackAutomask=True;
          subDeconv.setupDeconvolution(decPars_p);
          std::shared_ptr<SIImageStore> subimstor=subImageStore();
          //ImageBeamSet bs=ImageBeamSet::fromRecord(beamsetRec_p);
          ImageBeamSet bs=(subimstor->psf()->imageInfo()).getBeamSet();
          subimstor->setBeamSet(bs);
          subimstor->setPSFSidelobeLevel(psfSidelobeLevel_p);
          LatticeLocker lock1 (*(subimstor->model()), FileLocker::Write, 30);
          Record prevresrec=subDeconv.initMinorCycle(subimstor);

          // unused
	  // Float prevPeakRes=prevresrec.asFloat("peakresidual");
	  Bool doDeconv=True;
          if(autoMaskOn_p){
	    subDeconv.setChanFlag(chanFlag_p);
	    subDeconv.setRobustStats(statsRec_p);
	    Int automaskflag=iterBotRec_p.asInt("onlyautomask");
	    LogIO os( LogOrigin("SynthesisDeconvolver",automaskflag==1 ? "excuteAutomask" : "executeMinorCycle",WHERE) );
	    os << "Processing channels in range " << chanRange_p << LogIO::POST;
	    if(automaskflag==1){
	      doDeconv=False;
	      if(iterBotRec_p.isDefined("cycleniter"))
		 subDeconv.setMinorCycleControl(iterBotRec_p);
	    }
	    //cerr << "ITERDONE " << iterBotRec_p.asInt("iterdone")<< " itermask flag " << automaskflag << endl;
	    subDeconv.setIterDone(iterBotRec_p.asInt("iterdone"));
	    if(automaskflag !=0){
	      //this is already sent in as part of subimstor
	      //subDeconv.setPosMask(subimstor->tempworkimage());
	     
	      subDeconv.setAutoMask();
	      /*
	      Record resRec=subDeconv.initMinorCycle(subimstor);
	      //cerr << "POST resrec " << resRec << endl;
	      //cerr << "peakResidual " << resRec.asFloat("peakresidual") << " cyclethreshold " << iterBotRec_p.asFloat("cyclethreshold") << " as double " << iterBotRec_p.asDouble("cyclethreshold") << endl;
	      if(resRec.isDefined("peakresidual") && iterBotRec_p.isDefined("cyclethreshold")){
		
		Float peakresidual=resRec.asFloat("peakresidual");
		Float peakresidualnomask=resRec.asFloat("peakresidualnomask");
		Float cyclethreshold=iterBotRec_p.asFloat("cyclethreshold");
		if(iterBotRec_p.isDefined("psffraction")){
		  cyclethreshold=iterBotRec_p.asFloat("psffraction")*peakresidual;	
		  cyclethreshold= max(cyclethreshold, iterBotRec_p.asFloat("threshold"));
		  if(automaskflag==-1){

		    //		    cerr << "old cyclethreshold " <<iterBotRec_p.asFloat("cyclethreshold") << " new " << cyclethreshold << endl;
		    iterBotRec_p.removeField("cyclethreshold");
		    iterBotRec_p.define("cyclethreshold", cyclethreshold);
		  }
		}
		
		
		  //if(peakresidual < iterBotRec_p.asFloat("cyclethreshold"))
		if(peakresidual < cyclethreshold)
		  writeBackAutomask=False;
		
		writeBackAutomask=False;
		//Its better to always write the automask 
		//cerr << "chanRange " << chanRange_p << endl;
		//cerr << "WRITEBACK " << writeBackAutomask<< " peakres " <<  peakresidual << " prev " <<  prevPeakRes << " nomask " << peakresidualnomask << endl;
		
	      }
	      */

	      writeBackAutomask=True;
	      //Its better to always write the automask 
	     
	    }
	    else{
	      writeBackAutomask=False;
	    }
	    
          } else {
            LogIO os( LogOrigin("CubeMinorCycleAlgorithm","task",WHERE) );
            os << "Processing channels in range " << chanRange_p << LogIO::POST;
          } // end if(autoMaskOn_p)
          //subDeconv.setupMask();
	  if(doDeconv)
	    returnRec_p=subDeconv.executeCoreMinorCycle(iterBotRec_p);
	  else
	    returnRec_p.define("doneautomask", True);
          chanFlag_p.resize();
          chanFlag_p=subDeconv.getChanFlag();
	  statsRec_p=Record();
	  statsRec_p=subDeconv.getRobustStats();
	  if(doDeconv){
	    writeBackToFullImage(modelName_p, chanRange_p[0], chanRange_p[1], (subimstor->model()));

	  }
          if(autoMaskOn_p && writeBackAutomask){
            writeBackToFullImage(posMaskName_p, chanRange_p[0], chanRange_p[1], (subimstor->tempworkimage()));
            writeBackToFullImage(maskName_p, chanRange_p[0], chanRange_p[1], (subimstor->mask()));
          }
        }
        catch (AipsError x) {
          logger << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
          returnRec_p=Record();
        }
        catch(std::exception& exc) {
          logger << LogIO::SEVERE << "Exception (std): " << exc.what() << LogIO::POST;
          returnRec_p=Record();
        }
        catch(...){
          logger << LogIO::SEVERE << "Unknown exception" << LogIO::POST;
          returnRec_p=Record();
        }
        
       	status_p = True;
}
String&	CubeMinorCycleAlgorithm::name(){
	return myName_p;
}

std::shared_ptr<SIImageStore> CubeMinorCycleAlgorithm::subImageStore(){
  std::shared_ptr<ImageInterface<Float> >subpsf=nullptr;
  std::shared_ptr<ImageInterface<Float> >subresid=nullptr;
  std::shared_ptr<ImageInterface<Float> >submodel=nullptr;
  std::shared_ptr<ImageInterface<Float> > submask=nullptr;
  std::shared_ptr<ImageInterface<Float> > subpb=nullptr;
  std::shared_ptr<ImageInterface<Float> > subposmask=nullptr;
	Int chanBeg=0;
	Int chanEnd=0;
	chanBeg=chanRange_p[0];
	chanEnd=chanRange_p[1];
	casacore::String imageName = decPars_p.imageName;
	//cerr << "chanBeg " << chanBeg << " chanEnd " << chanEnd << " imId " << imId << endl;
        
	
        makeTempImage(subpsf, psfName_p, chanBeg, chanEnd);
        makeTempImage(subresid, residualName_p, chanBeg, chanEnd);
        makeTempImage(submodel, modelName_p, chanBeg, chanEnd, True);
        makeTempImage(submask, maskName_p, chanBeg, chanEnd, True);
        //       PagedImage<Float> model(modelName_p, TableLock::UserLocking);
        //   submodel.reset(SpectralImageUtil::getChannel(model, chanBeg, chanEnd, true));
        if(!pbName_p.empty()){
           makeTempImage(subpb, pbName_p, chanBeg, chanEnd);
        }
        if(iterBotRec_p.isDefined("posmaskname") ){
            iterBotRec_p.get("posmaskname", posMaskName_p);
            if(Table::isReadable(posMaskName_p)){
                makeTempImage(subposmask, posMaskName_p, chanBeg, chanEnd, True);
                if(subposmask)
                  autoMaskOn_p=True;
              }
          }

            std::shared_ptr<SIImageStore> subimstor(new SimpleSIImageStore(imageName, submodel, subresid, subpsf, nullptr, nullptr, submask, nullptr, nullptr, subpb, nullptr, subposmask));
        
	//cerr << "subimagestor TYPE" << subimstor->getType() << endl;
	return subimstor;
}
  void CubeMinorCycleAlgorithm::makeTempImage(std::shared_ptr<ImageInterface<Float> >& outptr,  const String& imagename, const Int chanBeg, const Int chanEnd, const Bool writelock){
    //For testing that locks are placed in the right places use userlocking
    PagedImage<Float> im(imagename, writelock ? TableLock::UserLocking : TableLock::UserNoReadLocking);
    //PagedImage<Float> im(imagename, writelock ? TableLock::AutoLocking : TableLock::AutoNoReadLocking);
    
    SubImage<Float> *tmpptr=nullptr;
    //LatticeLocker lockread (im, FileLocker::Read);
    //if(writelock)
    //  im.lock(FileLocker::Write, 1000);
    ////TESTOO
    //outptr.reset(SpectralImageUtil::getChannel(im, chanBeg, chanEnd, writelock));
    

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
      ///TESTOO
      /*if(imagename.contains(".residual")){
        String newresid=File::newUniqueName("./", "newResid").absoluteName();
        outptr.reset(new PagedImage<Float>(tshape, tmpptr->coordinates(), newresid));
      	}
      ////TESTOO
      else
      */
      outptr.reset(new TempImage<Float>(tshape, tmpptr->coordinates()));
	
      
      if(writelock){
	LatticeLocker lock1 (*(tmpptr), FileLocker::Write);
	outptr->copyData(*tmpptr);
	//cerr << "IMAGENAME " << imagename << " masked " << im.isMasked() << " tmptr  " << tmpptr->isMasked() << endl;
	if(tmpptr->isMasked()){
	  outptr->makeMask ("mask0", true, true, false, true);
	  outptr->pixelMask().put(tmpptr->getMask());
	  //	  cerr << "tempimage SUM of bit mask" << ntrue(tmpptr->pixelMask().get()) << " out   " << ntrue(outptr->pixelMask().get()) << endl;
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
 void CubeMinorCycleAlgorithm::writeBackToFullImage(const String imagename, const Int chanBeg, const Int chanEnd, std::shared_ptr<ImageInterface<Float> > subimptr){
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
void CubeMinorCycleAlgorithm::reset(){
		
  iterBotRec_p=Record();
  modelName_p="";
  residualName_p="";
  psfName_p="";
  maskName_p="";
  pbName_p="";
  posMaskName_p="";
  chanRange_p.resize();
  returnRec_p=Record();
  beamsetRec_p=Record();
  //psfSidelobeLevel_p;
  autoMaskOn_p=False;
  chanFlag_p.resize();
  statsRec_p=Record();
  chanFlagRec_p=Record();
  status_p=False;
                
	
	
}	
	
	
} //# NAMESPACE CASA - END
