//# hummbee.cc: deconvolver functionality sits here; 
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

#include <hummbee.h>


/*bool synthesisimager::setupdeconvolution(const casac::record& decpars)
{
  Bool rstat(false);

  try 
    {
      casacore::Record rec = *toRecord( decpars );
      itsDeconvolver->setupDeconvolution( rec );
    } 
  catch  (AipsError x) 
    {
      RETHROW(x);
    }
  
  return rstat;
}

bool synthesisimager::runminorcycle()
{
  Bool rstat(false);
  
  try 
    {
      // This is a convenience function for tool-level usage, for the non-parallel case.
      // Duplicates the code from getsubiterbot(), executeminorcycle(), endminorcycle().
      casacore::Record iterbotrec = itsIterBot->getSubIterBot();

      # Get iteration control parameters
      #  iterbotrec = self.IBtool.getminorcyclecontrols()
      #  self.IBtool.resetminorcycleinfo()


      iterbotrec = itsDeconvolver->executeMinorCycle(iterbotrec);
      itsImager->endMinorCycle(iterbotrec);

      #exrec = self.SDtools[immod].executeminorcycle( iterbotrecord = iterbotrec )
      #print('.... iterdone for ', immod, ' : ' , exrec['iterdone'])
      #self.IBtool.mergeexecrecord( exrec ) # this is the same as endMinorCycle
      #
     } 
  catch  (AipsError x) 
    {
      RETHROW(x);
    }

  return rstat;
}

casac::record* synthesisimager::getsubiterbot()
{
  casac::record* rstat(0);
  try {
    rstat=fromRecord(itsImager->getSubIterBot());
  } catch  (AipsError x) 
    {
      RETHROW(x);
    }

  return rstat;
}  

casac::record* synthesisimager::setupiteration(const casac::record& iterpars)
{

  try 
    {
      casacore::Record recpars = *toRecord( iterpars );
      itsIterBot->setupIteration( recpars );
    } 
  catch  (AipsError x) 
    {
      RETHROW(x);
    }
  
  return getiterationdetails();
}

bool synthesisiterbot::resetminorcycleinfo()
{
  Bool rstat(false);

  try
    {
      itsIterBot->resetMinorCycleInfo();
     }
  catch  (AipsError x)
    {
      RETHROW(x);
    }

  return rstat;
}



  casac::record* synthesisiterbot::getminorcyclecontrols()
{
  casac::record* rstat(0);
  try {
    rstat=fromRecord(itsIterBot->getSubIterBot());
  } catch  (AipsError x)
    {
      RETHROW(x);
    }

  return rstat;
}


 def initializeIterationControl(self):
        # note that in CASA5 this is casac.synthesisiterbot
        self.IBtool = iterbotsink()
        itbot = self.IBtool.setupiteration(iterpars=self.iterpars)

#I don't need this
def _scatter(self, imtype, partname):
        imgname = self.allimpars['0']['imagename'];
        partlist = self._mkImagePartList(imgname, imtype, partname);

        for immod in range(self.NF):
            normpars = self.allnormpars[str(immod)];
            if len(partlist) > 1:
                normpars['partimagenames'] = partlist;
            print('gather_normpars: {}'.format(normpars))

        self.PStools.append(synthesisnormalizer())

        for immod in range(self.NF):
            self.PStools[immod].setupnormalizer(normpars=normpars)
            self.PStools[immod].dividemodelbyweight()
            self.PStools[immod].scattermodel()


 def hasConverged(self):
         # Merge peak-res info from all fields to decide iteration parameters
         time0=time.time()
         self.IBtool.resetminorcycleinfo()
         self.initrecs = []
         for immod in range(0,self.NF):
              initrec =  self.SDtools[immod].initminorcycle()
              self.IBtool.mergeinitrecord( initrec );
              self.initrecs.append(initrec)

         # Check with the iteration controller about convergence.
         #print("check convergence")
         stopflag = self.IBtool.cleanComplete()
         #print('Converged : ', stopflag)
         if( stopflag>0 ):
             stopreasons = ['iteration limit', 'threshold', 'force stop','no change in peak residual across two major cycles', 'peak residual increased by more than 3 times from the previous major cycle','peak residual increased by more than 3 times from the minimum reached','zero mask', 'any combination of n-sigma and other valid exit criterion']
             casalog.post("Reached global stopping criterion : " + stopreasons[stopflag-1], "INFO")

             # revert the current automask to the previous one 
             #if self.iterpars['interactive']:
             for immod in range(0,self.NF):
                     if self.alldecpars[str(immod)]['usemask'].count('auto')>0:
                        prevmask = self.allimpars[str(immod)]['imagename']+'.prev.mask'
                        if os.path.isdir(prevmask):
                          # Try to force rmtree even with an error as an nfs mounted disk gives an error 
                          #shutil.rmtree(self.allimpars[str(immod)]['imagename']+'.mask')
                          shutil.rmtree(self.allimpars[str(immod)]['imagename']+'.mask', ignore_errors=True)
                          # For NFS mounted disk it still leave .nfs* file(s) 
                          if os.path.isdir(self.allimpars[str(immod)]['imagename']+'.mask'):
                            import glob
                              if glob.glob(self.allimpars[str(immod)]['imagename']+'.mask/.nfs*'):
                                  for item in os.listdir(prevmask):
                                      src = os.path.join(prevmask,item)
                                      dst = os.path.join(self.allimpars[str(immod)]['imagename']+'.mask',item)
                                      if os.path.isdir(src):
                                          shutil.move(src, dst)
                                      else:
                                          shutil.copy2(src,dst)
                              shutil.rmtree(prevmask)
                          else:
                              shutil.move(prevmask,self.allimpars[str(immod)]['imagename']+'.mask')
                          casalog.post("[" + str(self.allimpars[str(immod)]['imagename']) + "] : Reverting output mask to one that was last used ", "INFO")

         casalog.post("***Time taken in checking hasConverged "+str(time.time()-time0), "INFO3")
         return (stopflag>0)



// prob don't need this since takeOneStep called in deconvolve() handles it already
int cleanComplete()
{  

}

//at cycle 0, cyclethreshold = threshold

 # Assign cyclethreshold explicitly to threshold
        threshold = threshold if (type(threshold) == str) else (str(threshold*1000)+'mJy')
        paramList.setIterPars({'cyclethreshold': threshold, 'cyclethresholdismutable': False})
*/




float Hummbee(
        string& imageName, string& modelImageName,
        string& deconvolver,
        vector<float>& scales,
        float& largestscale, float& fusedthreshold,
        int& nterms,
        float& gain, float& threshold,
        float& nsigma,
        int& cycleniter, float& cyclefactor,
        vector<string>& mask, string& specmode,
        bool& doPBCorr,
        string& imagingMode)
{
  LogIO os(LogOrigin("Hummbee","hummbee_func"));

  Float PeakResidual = 0.0;

  try
    { 
      SynthesisParamsDeconv decPars_p;
      decPars_p.setDefaults();

      //deconvolution params
      decPars_p.imageName=imageName;
      decPars_p.algorithm=deconvolver;
      decPars_p.startModel=modelImageName;
      decPars_p.deconvolverId=0;
      decPars_p.nTaylorTerms=1; 
      decPars_p.scales = Vector<Float>(scales);
      decPars_p.maskType="user"; //genie
      //decPars_p.maskString=mask[0]; 
      //decPars_p.maskList.resize(1); decPars_p.maskList[0]=mask;
      
      if (mask.size() == 0)
      {
        decPars_p.maskString = "";
        decPars_p.maskList.resize(1);
        decPars_p.maskList[0] = "";
      }
      else if(mask.size() == 1)
      {
        decPars_p.maskString = mask[0];
        decPars_p.maskList.resize(1);
        decPars_p.maskList[0] = mask[0];
      }
      else
      {
        decPars_p.maskString = mask[0];
        decPars_p.maskList = Vector<String>(mask);
      }
       

      decPars_p.pbMask=0.0; //genie
      decPars_p.autoMaskAlgorithm="thresh";
      decPars_p.maskThreshold=""; //genie
      decPars_p.maskResolution=""; //genie
      decPars_p.fracOfPeak=0.0; //genie
      decPars_p.nMask=0; //genie
      decPars_p.fastnoise = true; //genie
      decPars_p.interactive=false;
      decPars_p.autoAdjust=False; //genie
      decPars_p.fusedThreshold = fusedthreshold;
      decPars_p.specmode=specmode; //deconvolve task does not have this
      decPars_p.largestscale = largestscale;
      decPars_p.scalebias = 0.0;
      decPars_p.nTaylorTerms = nterms;
      decPars_p.nsigma = nsigma; // default is 0 which indicates no PB is required

      SynthesisDeconvolver itsDeconvolver;
      itsDeconvolver.setupDeconvolution(decPars_p);
      

      std::shared_ptr<SIImageStore> itsImages;
      if( deconvolver == "mtmfs" )
        {  itsImages.reset( new SIImageStoreMultiTerm( imageName, nterms, true, true ) ); }
      else
        {  itsImages.reset( new SIImageStore( imageName, true, true) ); }

      float MaxPsfSidelobe =  itsImages->getPSFSidelobeLevel();

      Float masksum;
      if( ! itsImages->hasMask() ) // i.e. if there is no existing mask to re-use...
      { masksum = -1.0; }
      else
      { 
        masksum = itsImages->getMaskSum();
        itsImages->mask()->unlock();
      }
      Bool validMask = ( masksum > 0 );
      PeakResidual= validMask ? itsImages->getPeakResidualWithinMask() : itsImages->getPeakResidual();

      std::cout << "masksum " << masksum << std::endl;
      
      float minpsffraction = 0.05;
      float maxpsffraction = 0.8;
      float CycleFactor = 1.0;
      
      Float psffraction = MaxPsfSidelobe * CycleFactor;
      psffraction = casacore::max(psffraction, minpsffraction);
      psffraction = casacore::min(psffraction, maxpsffraction);
      Float cyclethreshold = PeakResidual * psffraction;
      threshold = casacore::max(threshold, cyclethreshold);   

      // iteration parameters
      Record iterBotRec_p;

      iterBotRec_p.define("cycleniter", Int(cycleniter));
      iterBotRec_p.define("loopgain", Float(gain));
      iterBotRec_p.define("cyclethreshold", Float(threshold)); //at cycle 0, cyclethreshold = threshold 
      iterBotRec_p.define("nsigma", Float(nsigma));
      iterBotRec_p.define("thresholdreached", false);
      /*iterPars.cyclefactor=cyclefactor;*/

   
      //SynthesisDeconvolver itsDeconvolver;
      //Record iterBotRec;
      //itsDeconvolver.setupDeconvolution(decPars_p);
      itsDeconvolver.initMinorCycle(); // makeImageStore and StartModel, originally part of hasConverged
      //itsDeconvolver.setIterDone(); // don't need this. Controlled by cycleniter
      itsDeconvolver.setMinorCycleControl(iterBotRec_p);

      //int stopflag = cleanComplete(); //prob don't need this

      itsDeconvolver.setupMask(); // don't need to updateMask because not interactive

      // this is equivelant to runminorcycle()
      // don't need state sharing for interactive GUI and science code
  
      if (imagingMode == "deconvolve")
      {
        //itsIterBot.resetMinorCycleInfo(); // prob don't need this - reset interactive state (peak res etc) to 0 
        iterBotRec_p = itsDeconvolver.executeMinorCycle(iterBotRec_p);
        //itsIterBot.endMinorCycle(iterbotrec); // prob don't need this - mergeCycleExecutionRecord (peakres, etc)
                                                  // & getDetailsRecord from "state"
      }
      else if (imagingMode == "restore")
      {
         itsDeconvolver.restore();
         if (doPBCorr)
           itsDeconvolver.pbcor(); 
      }

      // return the peak residual for unit test
      PeakResidual= validMask ? itsImages->getPeakResidualWithinMask() : itsImages->getPeakResidual();

      //don't need this. Handled by SynthesisDeconvolver already.
      /*stopflag = cleanComplete();
      if(stopflag > 0)
      {
         stopreasons = ['iteration limit', 'threshold', 'force stop','no change in peak residual across two major cycles', 'peak residual increased by more than 3 times from the previous major cycle','peak residual increased by more than 3 times from the minimum reached','zero mask', 'any combination of n-sigma and other valid exit criterion']
         casalog.post("Reached global stopping criterion : " + stopreasons[stopflag-1], "INFO")
      }*/



      /**** Felipe's htclean runModelCycle
      def runModelCycle(self, imtype = 'residual', partname = 'SPW'):
        self.initializeDeconvolvers()
        self.initializeIterationControl() -- see above def. This calls iterbotsink().setupiteration

        stop = self.hasConverged()

        self.updateMask()

        print('modelCycle:parameters: {}'.format(self.allimpars['0']))

        self.runMinorCycle()
        self._scatter(imtype, partname) # don't need this 

        if self.hasConverged():
            os.system('touch stopIMCycles')
      
      // end of def
      */


       /* ## Get summary from iterbot -- don't need this
        if type(interactive) != bool and niter>0:
            retrec=decon.getSummary();

        #################################################
        #### Teardown -- don't need this for mode=deconvovle. 
        #### In makeFinalImage
        #################################################
        def makeFinalImages(self, imtype = 'residual', partname = 'SPW', gather = True, scatter = False):
          self.initializeDeconvolvers()
          self.initializeIterationControl()

          if gather:
              self._gather(imtype, partname, scatter = False)

          self.restoreImages()
          self.pbcorImages()


        ## Get records from iterbot, to be used in the next call to deconvolve
        iterrec = decon.getIterRecords()

        ## Restore images.
        if restoration==True:
            t0=time.time();
            decon.restoreImages()
            t1=time.time();
            casalog.post("***Time for restoring images: "+"%.2f"%(t1-t0)+" sec", "INFO3", "task_deconvolve");

        ##close tools
        decon.deleteTools()

      */

    }
  catch(AipsError& er)
    {
      os << er.what() << LogIO::SEVERE;
    }

    return (float(PeakResidual));

}
