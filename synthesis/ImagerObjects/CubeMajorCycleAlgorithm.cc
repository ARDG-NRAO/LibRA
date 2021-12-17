//# CubeMajorCycleAlgorithm.cc: implementation of class to grid and degrid (and write model vis when necessary) in parallel/serial 
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
#include <synthesis/ImagerObjects/CubeMajorCycleAlgorithm.h>
#include <synthesis/ImagerObjects/SynthesisImagerVi2.h>
#include <synthesis/ImagerObjects/SynthesisNormalizer.h>
#include <casa/Containers/Record.h>
#include <synthesis/ImagerObjects/SimpleSIImageStore.h>
#include <imageanalysis/Utilities/SpectralImageUtil.h>
#include <casa/OS/Timer.h>
#include <chrono>
#include <thread>
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
extern Applicator applicator;

  CubeMajorCycleAlgorithm::CubeMajorCycleAlgorithm() : myName_p("CubeMajorCycleAlgorithm"),  ftmRec_p(0), iftmRec_p(0), polRep_p(0),startmodel_p(0), residualNames_p(0), psfNames_p(0), sumwtNames_p(0), weightNames_p(0), pbNames_p(0), movingSource_p(""),status_p(False), retuning_p(True), doPB_p(False), nterms_p(0){
	
}
CubeMajorCycleAlgorithm::~CubeMajorCycleAlgorithm() {
	
}
	
void CubeMajorCycleAlgorithm::get() {
	reset();
	//cerr << "in get for child process " << applicator.isWorker() << endl;
	Record vecImParsRec;
	Record vecSelParsRec;
	Record vecGridParsRec;
	// get data sel params #1
	applicator.get(vecSelParsRec);
	// get image sel params #2
	applicator.get(vecImParsRec);
	// get gridders params #3
	applicator.get(vecGridParsRec);
	// get which channel to process #4
	applicator.get(chanRange_p);
	//cerr <<"GET chanRange " << chanRange_p << endl;
	// psf or residual CubeMajorCycleAlgorithm #5
	applicator.get(dopsf_p);
	// store modelvis and other controls #6
	applicator.get(controlRecord_p);
	// weight params
	applicator.get(weightParams_p);
	//Somewhere before this we have to make sure that vecSelPars has more than 0 fields)
	dataSel_p.resize(vecSelParsRec.nfields());
	/// Fill the private variables
	for (Int k=0; k < Int(dataSel_p.nelements()); ++k){
		(dataSel_p[k]).fromRecord(vecSelParsRec.asRecord(String::toString(k)));
		//cerr << k << "datasel " << vecSelParsRec.asRecord(String::toString(k)) << endl;
	}
	//imsel and gridsel should be the same numbers (number of image fields)
	Int nmajorcycles=0;
	if(controlRecord_p.isDefined("nmajorcycles"))
		controlRecord_p.get("nmajorcycles",nmajorcycles);
	imSel_p.resize(vecImParsRec.nfields());
	gridSel_p.resize(vecImParsRec.nfields());
	ftmRec_p.resize(vecImParsRec.nfields());
	iftmRec_p.resize(vecImParsRec.nfields());
	polRep_p.resize(vecImParsRec.nfields());
	polRep_p.set(-1);
	startmodel_p.resize(vecImParsRec.nfields());
	startmodel_p.set(Vector<String>(0));
	//TESTOO 
	//Int CPUID;
	//MPI_Comm_rank(MPI_COMM_WORLD, &CPUID);
	//////////////////
	for (uInt k=0; k < imSel_p.nelements(); ++k){
		Record imSelRec=vecImParsRec.asRecord(String::toString(k));
		//cerr << k << " imsel " << imSelRec << endl;
		if(imSelRec.isDefined("polrep"))
			imSelRec.get("polrep", polRep_p[k]);
                ///Get moving source name
                if(imSelRec.isDefined("movingsource") && imSelRec.asString("movingsource") != ""){
                  imSelRec.get("movingsource", movingSource_p);
                }
		//cerr << CPUID << " POLREP " << polRep_p << endl;
		//Only first major cycle we need to reset model
		if(nmajorcycles==1)
			imSelRec.get("startmodel", startmodel_p[k]);
		//have to do that as fromRecord check does not like to have both model and startmodel on disk !
		imSelRec.define("startmodel", Vector<String>(0));
		(imSel_p[k]).fromRecord(imSelRec);
		Record recGridSel=vecGridParsRec.asRecord(String::toString(k));
		(gridSel_p[k]).fromRecord(recGridSel);
		if(!recGridSel.isDefined("ftmachine")){
			ftmRec_p.resize();
			iftmRec_p.resize();
		}
		if(ftmRec_p.nelements() >0){
			ftmRec_p[k]=recGridSel.asRecord("ftmachine");
			iftmRec_p[k]=recGridSel.asRecord("iftmachine");
		}
			
	}
	
	
}
void CubeMajorCycleAlgorithm::put() {
	
  //if(applicator.isSerial()){
  //		serialBug_p=Applicator::DONE;
		//applicator.put(serialBug_p);
		
  //	}
	//cerr << "in put " << status_p << endl;
  applicator.put(returnRec_p);
  applicator.put(status_p);	
	
}
	
void CubeMajorCycleAlgorithm::task(){
	LogIO logger(LogOrigin("CubeMajorCycleAlgorithm", "task", WHERE));
	status_p = True;
        try{
          //Timer tim;
          //tim.mark();
          //SynthesisImagerVi2 imgr;
          //imgr.selectData(dataSel_p);
          // We do not use chanchunking in this model
          for (uInt k=0; k < gridSel_p.nelements(); ++k)
            gridSel_p[k].chanchunks = 1;
          
          //imgr.defineImage(imSel_p,gridSel_p);
          // need to find how many subfields/outliers have been set
          //CountedPtr<SIImageStore> imstor =imgr.imageStore(0);
          //CountedPtr<ImageInterface<Float> > resid=imstor->residual();
          //Int nchan = resid->shape()(3);
          //std::shared_ptr<SIImageStore> subImStor=imstor->getSubImageStore(0, 1, chanId_p, nchan, 0,1);
          
          SynthesisImagerVi2 subImgr;
	  subImgr.setCubeGridding(False);
          for (Int k=0; k < Int(dataSel_p.nelements()); ++k){
            //The original SynthesisImager would have cleared the model if it was requested
            dataSel_p[k].incrmodel=True;
            dataSel_p[k].freqbeg="";
            subImgr.selectData(dataSel_p[k]);
          }
          //cerr <<"***Time for select data " << tim.real() << endl;
          //tim.mark();
          subImgr.setMovingSource(movingSource_p);
          Vector<CountedPtr<SIImageStore> > subImStor(imSel_p.nelements());
          //copy as shared_ptr as we mix the usage of countedptr and sharedptr
          Vector<std::shared_ptr<SIImageStore> > subImStorShared(imSel_p.nelements());
          Vector<SIImageStore *> p(imSel_p.nelements());
          //Do multifield in one process only for now 
          //TODO if all fields have same nchan then partition all on all subcalls
          if(chanRange_p[0]==0){
            for (uInt k=0; k < imSel_p.nelements(); ++k){
              subImStorShared[k]=subImageStore(k);
              subImStor[k]=CountedPtr<SIImageStore>(subImStorShared[k]);
	     
              if(ftmRec_p.nelements()>0){
                subImgr.defineImage(subImStor[k], ftmRec_p[k], iftmRec_p[k]);	
              }else{
                subImgr.defineImage(subImStor[k],  imSel_p[k], gridSel_p[k]);
              }
            }
          }else{
            subImStor.resize(1);
            subImStorShared[0]=subImageStore(0);
            subImStor[0]=CountedPtr<SIImageStore>(subImStorShared[0]);

            if(ftmRec_p.nelements()>0){
              subImgr.defineImage(subImStor[0], ftmRec_p[0], iftmRec_p[0]);
            }else{
              subImgr.defineImage(subImStor[0],  imSel_p[0], gridSel_p[0]);
            }
          }
          subImgr.setCubeGridding(False);
          // TO DO get weight param and set weight
          if(!weightParams_p.isDefined("type") || weightParams_p.asString("type")=="natural"){
            subImgr.weight("natural");
          }
          else{
            if(controlRecord_p.isDefined("weightdensity")){
              String densName=controlRecord_p.asString("weightdensity");
              //cerr << "Loading weightdensity " << densName << endl;
              if(Table::isReadable(densName))
                subImgr.setWeightDensity(densName);
            }
            else{
              subImgr.weight(weightParams_p);
            }
          }
          ///Now do the selection tuning if needed
          if(imSel_p[0].mode !="cubedata"){
            //cerr << "IN RETUNING " << endl;
            if(retuning_p)
              subImgr.tuneSelectData();
          }
          
          //cerr << "***Time for all other setting " << tim.real() << endl;
          //tim.mark();
          if (!dopsf_p){
            ///In case weightimages for mosaicft is done load it...we can get rid of this if we are using fromrecord ftm
            ///doing it for non-psf only ...psf divides it by sumwt for some reason in
            ///SIImageStore ..so restart with psf creates havoc
          subImgr.loadMosaicSensitivity();
            Record outrec=subImgr.executeMajorCycle(controlRecord_p);
            if(outrec.isDefined("tempfilenames")){
	      returnRec_p.define("tempfilenames", outrec.asArrayString("tempfilenames"));
	    }
            for(uInt k=0; k < subImStor.nelements(); ++k){
              if(controlRecord_p.isDefined("dividebyweight") && controlRecord_p.asBool("dividebyweight")){
                {
                  if(controlRecord_p.isDefined("normpars")){
                    Record normpars=controlRecord_p.asRecord("normpars");
                    SynthesisNormalizer norm;
                    if(nterms_p[k] > 0){
                      if(!normpars.isDefined("nterms"))
                        normpars.define("nterms", uInt(nterms_p[k]));
                      normpars.define("deconvolver", "mtmfs");
                    }
                    
                    norm.setupNormalizer(normpars);
                    norm.setImageStore(subImStorShared[k]);
                    norm.divideResidualByWeight();
                  }
                  else{
                    LatticeLocker lock1 (*(subImStor[k]->residual()), FileLocker::Write);
                    LatticeLocker lock2 (*(subImStor[k]->sumwt()), FileLocker::Read);
                    subImStor[k]->divideResidualByWeight();
                    //subImStor[k]->residual()->flush();
                  }
                }
                subImStor[k]->residual()->unlock();
                if(subImStor[k]->hasModel())
                  subImStor[k]->model()->unlock();
              }
              Int chanBeg=0; Int chanEnd=0;
              if(k==0){
                chanBeg=chanRange_p[0];
                chanEnd=chanRange_p[1];
              }
              else{
                chanBeg=0;
                chanEnd=subImStor[k]->sumwt()->shape()[3]-1;
              }
              if(subImStor[k]->getType() != "multiterm"){
                writeBackToFullImage(residualNames_p[k], chanBeg, chanEnd, (subImStor[k]->residual()));
                writeBackToFullImage(sumwtNames_p[k], chanBeg, chanEnd, (subImStor[k]->sumwt()));
              }
              
            }
            
            
          }
          else{
            Record&& outrec=subImgr.makePSF();
	    if(outrec.isDefined("tempfilenames")){
	      returnRec_p.define("tempfilenames", outrec.asArrayString("tempfilenames"));
	    }
	    ////tclean expects a PB to be always there...
	    //so for standard make it
	    subImgr.makePB();
            for(uInt k=0; k < subImStor.nelements(); ++k){
              if(controlRecord_p.isDefined("dividebyweight") && controlRecord_p.asBool("dividebyweight"))
		{
                  if(controlRecord_p.isDefined("normpars")){
                    Record normpars=controlRecord_p.asRecord("normpars");
                    if(nterms_p[k] > 0){
                      if(!normpars.isDefined("nterms"))
                        normpars.define("nterms", uInt(nterms_p[k]));
                      normpars.define("deconvolver", "mtmfs");
                    }
                    //                        cerr  << k << " " << nterms_p[k] <<" NORMPARS " << normpars << endl;
                    SynthesisNormalizer norm;
                    norm.setupNormalizer(normpars);
                    norm.setImageStore(subImStorShared[k]);
                    norm.dividePSFByWeight();
		    copyBeamSet(*(subImStorShared[k]->psf()), k);
                  }
                  else{
                    LatticeLocker lock1 (*(subImStor[k]->psf()), FileLocker::Write);
                    LatticeLocker lock2 (*(subImStor[k]->sumwt()), FileLocker::Read);
                    subImStor[k]->dividePSFByWeight();
		    copyBeamSet(*(subImStor[k]->psf()), k);
                    //subImStor[k]->psf()->flush();
                  }
		}
              Int chanBeg=0; Int chanEnd=0;
              if(k==0){
                chanBeg=chanRange_p[0];
                chanEnd=chanRange_p[1];
              }
              else{
                chanBeg=0;
                chanEnd=subImStor[k]->sumwt()->shape()[3]-1;
              }
              if(subImStor[k]->getType() != "multiterm"){
                writeBackToFullImage(psfNames_p[k], chanBeg, chanEnd, (subImStor[k]->psf()));
                writeBackToFullImage(sumwtNames_p[k], chanBeg, chanEnd, (subImStor[k]->sumwt()));
		if((subImStor[k]->hasSensitivity()) && Table::isWritable(weightNames_p[k])){
		  writeBackToFullImage(weightNames_p[k], chanBeg, chanEnd, (subImStor[k]->weight()));
		  
		}
		if( doPB_p && Table::isWritable(pbNames_p[k])){
		  writeBackToFullImage(pbNames_p[k], chanBeg, chanEnd, (subImStor[k]->pb()));
		  
		}
		
              }
              subImStor[k]->psf()->unlock();
              
            }
          }
	//cerr << "***Time gridding/ffting " << tim.real() << endl;
          status_p = True;
        }
        catch (AipsError x) {

           LogIO os( LogOrigin("SynthesisImagerVi2","CubeMajorCycle",WHERE) );
           os << LogIO::WARN << "Exception for chan range "  << chanRange_p  << " ---   "<< x.getMesg()   << LogIO::POST;
          cerr << "##################################\n#############################\nException: " << x.getMesg() << endl;

          status_p=false;
        }
        catch(std::exception& exc) {
          logger << "Exception (std): " << exc.what() << LogIO::SEVERE;
          returnRec_p=Record();
        }
        catch(...){
          cerr << "###################################\n#######################3##\nUnknown exception "  << endl;
	  std::exception_ptr eptr=std::current_exception();
	  try{
	    if(eptr)
	      std::rethrow_exception(eptr);
	  }
	  catch(const std::exception& a){
	    LogIO os( LogOrigin("SynthesisImagerVi2","CubeMajorCycle",WHERE) );
	    os  << LogIO::WARN << "Unknown Exception for chan range "  << chanRange_p  << " ---   "<<  a.what()   << LogIO::POST;
	  }

          logger << "Unknown exception "  << LogIO::SEVERE;
          status_p=False;
        }
}
String&	CubeMajorCycleAlgorithm::name(){
	return myName_p;
}

  shared_ptr<SIImageStore> CubeMajorCycleAlgorithm::subImageStore(const int imId){
	//For some reason multiterm deconvolver is allowed with cubes !
	String isMTdeconv="";
        nterms_p.resize(imId+1, True);
        nterms_p[imId]=-1;
	if(imId==0 && imSel_p[imId].deconvolver=="mtmfs"){
          isMTdeconv=".tt0";
          nterms_p[0]=1;
        }
        
	String residname=imSel_p[imId].imageName+".residual"+isMTdeconv;
	String psfname=imSel_p[imId].imageName+".psf"+isMTdeconv;
	String sumwgtname=imSel_p[imId].imageName+".sumwt"+isMTdeconv;
	if(imId > 0 && !Table::isReadable(sumwgtname)){
		return multiTermImageStore(imId);
	}
	shared_ptr<ImageInterface<Float> >subpsf=nullptr;
	shared_ptr<ImageInterface<Float> >subresid=nullptr;
	shared_ptr<ImageInterface<Float> >submodel=nullptr;
	shared_ptr<ImageInterface<Float> > subweight=nullptr;
        shared_ptr<ImageInterface<Float> > subpb=nullptr;
	//cerr << imId << " sumwt name " << sumwgtname << endl;
	String workingdir="";
	if(controlRecord_p.isDefined("workingdirectory"))
		controlRecord_p.get("workingdirectory", workingdir);
	//cerr <<"WORKINGDIR " << workingdir << endl;
	if(Table::isReadable(workingdir+"/"+sumwgtname)){
		workingdir=workingdir+"/";
		residname=workingdir+residname;
		psfname=workingdir+psfname;
		sumwgtname=workingdir+sumwgtname;
		
	}
	else if(!Table::isReadable(sumwgtname) )
		throw(AipsError("Programmer error: sumwt disk image is non existant")); 
	else
		workingdir="";
	///Use user locking to make sure locks are compliant
	PagedImage<Float> sumwt(sumwgtname, TableLock::UserLocking);
        sumwt.lock(FileLocker::Write, 200);
	//PagedImage<Float> sumwt(sumwgtname, TableLock::AutoNoReadLocking);
        if(sumwtNames_p.nelements() <= uInt(imId)){
          sumwtNames_p.resize(imId+1, True);
          psfNames_p.resize(imId+1, True);
          residualNames_p.resize(imId+1, True);
          sumwtNames_p[imId]=sumwgtname;
          residualNames_p[imId]=residname;
          psfNames_p[imId]=psfname;
        }
        Int nchannels=sumwt.shape()[3];

       
       
	//Should be partitioning for main image only
	//chanRange
	Int chanBeg=0;
	Int chanEnd=0;
	if(imId==0){
		chanBeg=chanRange_p[0];
		chanEnd=chanRange_p[1];
	}
	else{
		chanBeg=0;
		chanEnd=sumwt.shape()[3]-1;
	}
        sumwt.tempClose();
	////For some small channel ms's retuning trigger a vi2/vb2 bug in nChannels
	///avoid retuning for small images
	////Skipping this here..
	//overloaded SynthesisImagerVi2::retune to do this check
	//if(nchannels < 30 && imId==0 && ((chanEnd-chanBeg) < 10)){
        //  retuning_p=False;
        //}
	//cerr << "chanBeg " << chanBeg << " chanEnd " << chanEnd << " imId " << imId << endl;
        Vector<String> weightnams(controlRecord_p.asArrayString("weightnames"));
        Vector<String> pbnams(controlRecord_p.asArrayString("pbnames"));
        pbNames_p.resize();
        pbNames_p=pbnams;
	weightNames_p.resize();
	weightNames_p=weightnams;
        
        if(imId >= int(weightNames_p.nelements()))
          throw(AipsError("Number of weight images does not match number of image fields defined"));
        if(Table::isReadable(workingdir+weightNames_p[imId])){
		weightNames_p[imId]=workingdir+weightNames_p[imId];
	}
         if(Table::isReadable(workingdir+pbNames_p[imId])){
		pbNames_p[imId]=workingdir+pbNames_p[imId];
	}
       
	if(dopsf_p){
          //PagedImage<Float> psf(psfname, TableLock::UserNoReadLocking);
          //subpsf.reset(SpectralImageUtil::getChannel(psf, chanBeg, chanEnd, true));
          getSubImage(subpsf, chanBeg, chanEnd, psfname, False);
	  //cerr << "PBNAMES " << pbNames_p << endl;
	  if(Table::isReadable(pbNames_p[imId])){

	    doPB_p=True;
	    getSubImage(subpb, chanBeg, chanEnd, pbNames_p[imId], False);
	  }
	}
	else{
          //need to loop over all fields somewhere
          //PagedImage<Float> resid(residname, TableLock::UserNoReadLocking);
          //subresid.reset(SpectralImageUtil::getChannel(resid, chanBeg, chanEnd, true));
          getSubImage(subresid, chanBeg, chanEnd, residname, False);
 
          if(controlRecord_p.isDefined("modelnames")){
			Vector<String> modelnames(controlRecord_p.asArrayString("modelnames"));
			if(imId >= int(modelnames.nelements()))
				throw(AipsError("Number of model images does not match number of image fields defined"));
			if(Table::isReadable(workingdir+modelnames[imId])){
				modelnames[imId]=workingdir+modelnames[imId];
			}
			if(Table::isReadable(modelnames[imId])){
                          	
				
				///Pass some extra channels for interpolation while degridding..should match or be less than in SynthesisImager::tuneSelect
				Int startmodchan=(chanBeg-2) >0 ? chanBeg-2 : 0;
				Int endmodchan=(chanEnd+2) < nchannels ? chanEnd+2 : nchannels-1 ;
				//cerr << "START END mod " << startmodchan << "  " << endmodchan << endl;
				//Darn has to lock it as writable because overlap in SIMapperCollection code 
				//wants that...though we are not really modifying it here
				//Bool writeisneeded=(imSel_p.nelements()!=1 || startmodel_p[imId].nelements() >0);
                                //PagedImage<Float> model(modelnames[imId], TableLock::UserNoReadLocking);
				//submodel.reset(SpectralImageUtil::getChannel(model, startmodchan, endmodchan, writeisneeded));
				getSubImage(submodel, startmodchan, endmodchan, modelnames[imId], True);
                                //
                                if(Table::isReadable(weightNames_p[imId])){
                                  divideModelByWeight(imSel_p[imId].imageName, submodel, startmodchan, endmodchan, weightNames_p[imId]);
                                }
				//ImageInterface<Float>* modim=new PagedImage<Float>(modelnames[imId], TableLock::UserNoReadLocking);
				//submodel.reset(modim);
				
			}
			
		}
		
	}
	
	if(Table::isReadable(weightNames_p[imId])){
	  //PagedImage<Float> weight(weightnames[imId], TableLock::UserNoReadLocking);
	  //subweight.reset(SpectralImageUtil::getChannel(weight, chanBeg, chanEnd, true));
	  getSubImage(subweight, chanBeg, chanEnd, weightNames_p[imId], True); 
	}
	shared_ptr<ImageInterface<Float> >subsumwt=nullptr;
        //	subsumwt.reset(SpectralImageUtil::getChannel(sumwt, chanBeg, chanEnd, true));
        getSubImage(subsumwt, chanBeg, chanEnd, sumwgtname, True);
	bool useweightimage=(subweight) ? true : false;
        shared_ptr<SIImageStore> subimstor(new SimpleSIImageStore(imSel_p[imId].imageName, submodel, subresid, subpsf, subweight, nullptr, nullptr, subsumwt, nullptr, subpb, nullptr, nullptr, useweightimage));
	if(polRep_p[imId]< 0)
		throw(AipsError("data polarization type is not defined"));
	StokesImageUtil::PolRep polrep=(StokesImageUtil::PolRep)polRep_p[imId];
	subimstor->setDataPolFrame(polrep);
	if(startmodel_p[imId].nelements() >0){
	  LatticeLocker lock1 (*(subimstor->model()), FileLocker::Write);
		subimstor->setModelImage(startmodel_p[imId]);	
	}
	//cerr << "subimagestor TYPE" << subimstor->getType() << endl;
	return subimstor;
}

  
  std::shared_ptr<SIImageStore> CubeMajorCycleAlgorithm::multiTermImageStore(const Int imId){
	uInt nterms=0;
	String sumwgtname=imSel_p[imId].imageName+".sumwt.tt"+String::toString(nterms);
	while (Table::isReadable(sumwgtname)){
		++nterms;
		sumwgtname=imSel_p[imId].imageName+".sumwt.tt"+String::toString(nterms);
	}
	if(nterms==0){
		throw(AipsError("outlier "+String::toString(imId)+" field weight image is not defined"));
	}
	nterms=(nterms+1)/2;
        nterms_p[imId]=nterms;
        shared_ptr<SIImageStore> subimstor(new SIImageStoreMultiTerm(imSel_p[imId].imageName, nterms, True));
	if(polRep_p[imId]< 0)
		throw(AipsError("data polarization type is not defined"));
	StokesImageUtil::PolRep polrep=(StokesImageUtil::PolRep)polRep_p[imId];
	subimstor->setDataPolFrame(polrep);
	return subimstor;
}
  void CubeMajorCycleAlgorithm::divideModelByWeight(casacore::String imageName, shared_ptr<ImageInterface<Float> >&submodel, const Int startchan, const Int endchan, const String weightname){
    if(controlRecord_p.isDefined("dividebyweight") && controlRecord_p.asBool("dividebyweight")){
      if(controlRecord_p.isDefined("normpars")){
                        Record normpars=controlRecord_p.asRecord("normpars");
                        SynthesisNormalizer norm;
                        norm.setupNormalizer(normpars);
                        shared_ptr<ImageInterface<Float> > subweight=nullptr;
                        getSubImage(subweight, startchan, endchan, weightname, True);
			LatticeLocker lock1(*(subweight), FileLocker::Read);
			LatticeLocker lock2(*(submodel), FileLocker::Read);
                        std::shared_ptr<SIImageStore> subimstor(new SimpleSIImageStore(imageName, submodel, nullptr, nullptr, subweight, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, True));
                        norm.setImageStore(subimstor);
                        norm.divideModelByWeight();
                        
      }
    }
  }



                                                                                                                                                     
void CubeMajorCycleAlgorithm::reset(){
		
		dataSel_p.resize();
		imSel_p.resize();
		gridSel_p.resize();
		ftmRec_p.resize();
		iftmRec_p.resize();
		polRep_p.resize();
		chanRange_p.resize();
		dopsf_p=False;
		controlRecord_p=Record();
		weightParams_p=Record();
		returnRec_p=Record();
		startmodel_p.resize();
		status_p=False;
                residualNames_p.resize();
                psfNames_p.resize();
                sumwtNames_p.resize();
		weightNames_p.resize();
                pbNames_p.resize();
                movingSource_p="";
                retuning_p=True;
                doPB_p=False;
                nterms_p.resize();
                
	
	
}
	
  void CubeMajorCycleAlgorithm::getSubImage(std::shared_ptr<ImageInterface<Float> >& subimptr, const Int chanBeg, const Int chanEnd, const String imagename, const Bool copy){
    LogIO os( LogOrigin("CubeMajorCycle","getSubImage",WHERE) );
    CountedPtr<PagedImage<Float> > im=nullptr;
    {///work around
      uInt exceptCounter=0;
      while(true){
	try{
	  im = new PagedImage<Float> (imagename, TableLock::UserLocking);
	  //PagedImage<Float> im(imagename, TableLock::AutoNoReadLocking);
          im->lock(FileLocker::Read, 1000);
          SubImage<Float> *tmpptr=nullptr;
          tmpptr=SpectralImageUtil::getChannel(*im, chanBeg, chanEnd, false);
          subimptr.reset(new TempImage<Float>(TiledShape(tmpptr->shape(), tmpptr->niceCursorShape()), tmpptr->coordinates()));
          
          if(copy)
            subimptr->copyData(*tmpptr);
          //subimptr->flush();
          im->unlock();
          delete tmpptr;
          break; ///get out of while loop
	}
	catch(AipsError &x){
          im=nullptr;
	  String mes=x.getMesg();
	  if(mes.contains("FilebufIO::readBlock") ){
	    std::this_thread::sleep_for(std::chrono::milliseconds(50));
	    os << LogIO::WARN << "#####CATCHING a sleep because "<< mes<< LogIO::POST;
	  }
	  else
	    throw(AipsError("Error in readimage "+imagename+" : "+mes));
	  
	  if(exceptCounter > 100){
	    throw(AipsError("Error in readimage got 100 of this exeception: "+mes));
	    
	  }
	  
	}
	++exceptCounter;
      }
    }	  

  }

  void CubeMajorCycleAlgorithm::writeBackToFullImage(const String imagename, const Int chanBeg, const Int chanEnd, std::shared_ptr<ImageInterface<Float> > subimptr){
    LogIO os( LogOrigin("CubeMajorCycle","writebacktofullimage",WHERE) );
    CountedPtr<PagedImage<Float> > im=nullptr;
    {///work around
      uInt exceptCounter=0;
      while(true){
	try{
	  im=new PagedImage<Float> (imagename, TableLock::UserLocking);
	  
          
          //PagedImage<Float> im(imagename, TableLock::AutoLocking);
          SubImage<Float> *tmpptr=nullptr;
          {
            tmpptr=SpectralImageUtil::getChannel(*im, chanBeg, chanEnd, true);
            LatticeLocker lock1 (*(tmpptr), FileLocker::Write);
            tmpptr->set(0.0);
            tmpptr->copyData(*(subimptr));
          }
          im->flush();
          im->unlock();
          delete tmpptr;
          break; //get out of while loop
	}
	catch(AipsError &x){
          
	  String mes=x.getMesg();
	  if(mes.contains("FilebufIO::readBlock") ){
	    std::this_thread::sleep_for(std::chrono::milliseconds(50));
	    os << LogIO::WARN << "#####CATCHING a sleep because "<< mes<< LogIO::POST;
	  }
	  else
	    throw(AipsError("Error in writing back image "+imagename+" : "+mes));
          
	  if(exceptCounter > 100){
	    throw(AipsError("Error in writeimage got 100 of this exeception: "+mes));
	    
	  }
          
        }
	++exceptCounter;
      }
    }//End of work around for table disappearing bug

    
  }
  void CubeMajorCycleAlgorithm::copyBeamSet(ImageInterface<Float>& subpsf, const Int imageid){
    //For now lets do for the main image only
    if(imageid != 0)
      return;
    ImageBeamSet iibeamset=subpsf.imageInfo().getBeamSet();
    uInt nchan=chanRange_p[1]-chanRange_p[0]+1;
    if(nchan ==iibeamset.nchan()){
      Matrix<GaussianBeam> gbs=iibeamset.getBeams();
      Cube<Float> beams(nchan, iibeamset.nstokes(),3);
      
      for (uInt k=0; k < nchan; ++k){
	for (uInt j=0; j < iibeamset.nstokes(); ++j){
	  beams(k,j, 0)=gbs(k, 0).getMajor("arcsec");
	  beams(k,j, 1)=gbs(k, 0).getMinor("arcsec");
	  beams(k,j, 2)=gbs(k, 0).getPA("deg", True);
	}
      }
      returnRec_p.define("imageid", imageid);
      returnRec_p.define("chanrange", chanRange_p);
      returnRec_p.define("beams", beams);


    }



  }
	
	
	
} //# NAMESPACE CASA - END
