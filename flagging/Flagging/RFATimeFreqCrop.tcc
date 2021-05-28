//# RFATimeFreqCrop.cc: this defines RFATimeFreqCrop
//# Copyright (C) 2000,2001,2002
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
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <flagging/Flagging/RFATimeFreqCrop.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  //#define baselinecnt ( (NumAnt)*((NumAnt)+1)/2 - ((NumAnt)-ant1[bs])*((NumAnt)-ant1[bs]+1)/2 + (ant2[bs] - ant1[bs]) )
#define SELF(ant) ( (NumAnt)*((NumAnt)+1)/2 - ((NumAnt)-ant)*((NumAnt)-ant+1)/2 )
#define MIN(a,b) ((a)<=(b) ? (a) : (b))

//#define PLOT  // to activate the mean and clean bandpass plots
//#define UPLOT // to activate bandpass plots of each fit iteration

//#define DOPLOT  // to activate ds9 and bandpass-fit plots

/* Constructor for 'RFATimeFreqCrop' */
RFATimeFreqCrop :: RFATimeFreqCrop( RFChunkStats &ch,const casacore::RecordInterface &parm ):
	                    RFAFlagCubeBase(ch,parm) ,
	                    RFDataMapper(parm.asArrayString(RF_EXPR),parm.asString(RF_COLUMN)),
	                    vi(ch.visIter()),
	                    vb(ch.visBuf())
{
	ANT_TOL = parm.asDouble("ant_cutoff");
	BASELN_TOL = parm.asDouble("baseline_cutoff");
	T_TOL = parm.asDouble("time_amp_cutoff");
	F_TOL = parm.asDouble("freq_amp_cutoff");
	FlagLevel = parm.asInt("flag_level");
	CorrChoice = parm.asInt("auto_cross");
	NumTime = parm.asInt("num_time");
	ShowPlots = parm.asBool("showplots");
	FreqLineFit = parm.asBool("freqlinefit");
	MaxNPieces = parm.asInt("maxnpieces");
        DryRun = parm.asBool("dryrun");
        Expr = parm.asArrayString(RF_EXPR);
        Column = parm.asString(RF_COLUMN);
        IgnorePreflags = parm.asBool(RF_FIGNORE);
	//	cout << "Flagging on " << parm.asArrayString(RF_EXPR) << " for column : " << parm.asString(RF_COLUMN) << endl;
        StopAndExit=false;
	/*
        if(ShowPlots)
        {
           cmd = "xpaset -p ds9 plot new name flagger \n";
           system(cmd.data());
        }
	*/

}


/* Sets default values to parameters */
const casacore::RecordInterface & RFATimeFreqCrop::getDefaults ()
{
 static casacore::Record rec;
if( !rec.nfields() )
       {
	rec = RFAFlagCubeBase::getDefaults();
	rec.define(RF_NAME,"RFATimeFreqCrop");
	rec.define(RF_EXPR,"ABS I");
	rec.define(RF_COLUMN,"DATA");
	rec.define("ant_cutoff",0);
	rec.define("baseline_cutoff",0);
	rec.define("time_amp_cutoff",3);
	rec.define("freq_amp_cutoff",3);
	rec.define("flag_level",1);
	rec.define("auto_cross",1);
	rec.define("num_time",50);
	rec.define("column","DATA");
	rec.define("expr","ABS I");
	rec.define("fignore",false);
	rec.define("showplots",false);
	rec.define("freqlinefit",false);
	rec.define("maxnpieces",6);
        rec.define("dryrun",false);
//	rec.setcomment("ant_cutoff","Total autocorrelation amplitude threshold for a functional antenna");
//	rec.setcomment("time_amp_cutoff","Multiple/fraction of standard deviation, to set the threshold, while flagging across time");
//	rec.setcomment("freq_amp_cutoff","Multiple/fraction of standard deviation, to set the threshold, while flagging across frequency");
//	rec.setcomment("flag_level","Levels of Flagging");
       }
     return rec;
}



/* Called at the beginning of each chunk of data */
casacore::Bool RFATimeFreqCrop :: newChunk (casacore::Int &i)
{
  casacore::LogIO os(casacore::LogOrigin("tfcrop","newChunk","WHERE"));

  if(StopAndExit) 
    {
      //      cout << "newChunk :: NOT Working with data chunk : " << chunk.msName() << endl;
      return false;
    }

 
 //	cout << "newChunk :: Working with data chunk : " << chunk.msName() << endl;
 //	cout << "TimeSteps = " << num(TIME) << ", Baselines = " << num(IFR) << ", Chans = " << num(CHAN) << ", Polns = " << num(POLZN) << ", Ants = " << num(ANT) << endl;
 //	cout << "Parameters : " << " Antenna_tol=" << ANT_TOL << ", Baseline_tol=" << BASELN_TOL << ", Time_tol=" << T_TOL << "sigma, Freq_tol=" << F_TOL << "sigma, FlagLevel=" << FlagLevel << ", Flag_corr=" << casacore::String(CorrChoice?"Cross":"Auto") << endl;

	/* Initialize NumT - number of timestamps to work with in one go */
	if(NumTime==0) NumTime=50;
	if(num(TIME) >= NumTime) NumT = NumTime;
	else NumT = num(TIME);
	
	/* Assume that all baselines are present in this chunk */
	// TODO : check this.
       NumAnt = num(ANT);
       NumB = ((NumAnt)*((NumAnt)-1)/2 + (NumAnt));

       /* Number of polarizations */
       NumP = num(POLZN);
       //UUU : FORCE it to be 1 for now.....
       NumP=1;

       /* Number of channels */
        NumC = num(CHAN);

        /* Polarizations/correlations */

        corrmask = RFDataMapper::corrMask(chunk.visIter());
        casacore::Vector<casacore::Int> corrlist(num(POLZN));
        for(casacore::uInt corr=0; corr<num(POLZN); corr++)
	  corrlist[corr] = (casacore::Int) ( (corrmask >> corr) & 1 );
	
        /* Check that the above makes sense */
        if(NumC<1 || NumP<1 || NumB <1 || NumAnt<1 || NumT<1)
	  {
	    cout << "Invalid chunk shapes" << endl;
            return false;
	  }

	//	os << "Chunk=" << chunk.nchunk() << ", Field=" << chunk.visIter().fieldName() << ", FieldID=" << chunk.visIter().fieldId() << ", Spw=" << chunk.visIter().spectralWindow() << ", nTime=" << num(TIME) << " (" << NumT << " at a time), nBaseline=" << num(IFR) << ", nChan=" << num(CHAN) << ", nCorrs=" << num(POLZN) << " [" << chunk.getCorrString() << "]" <<  casacore::LogIO::POST;
	//cout << "Chunk=" << chunk.nchunk() << ", Field=" << chunk.visIter().fieldName() << ", FieldID=" << chunk.visIter().fieldId() << ", Spw=" << chunk.visIter().spectralWindow() << ", nTime=" << num(TIME) << " (" << NumT << " at a time), nBaseline=" << num(IFR) << ", nChan=" << num(CHAN) << ", nCorrs=" << num(POLZN) << " [" << chunk.getCorrString() << "]" <<  endl; // ". Flagging on " << Expr << endl; //" ->  correlations : " << corrlist << endl;
	//        cout << "Working with " << NumC << " x " << NumT << " subsets (nchan x ntime),  "<< Expr << " on the " << Column << " column, and applying flags to correlations : " << corrlist << endl;


	/* UUU : What is this ? */
	//       RFAFlagCubeBase::newChunk(i-=1);
       
       /* Allocate memory for one set of NumT timesteps. */
        AllocateMemory();

	return RFAFlagCubeBase::newChunk(i);
}


/* Called at the beginning of each PASS */
void RFATimeFreqCrop :: startData (bool verbose) 
{
  //  cout  << "StartData - reset time-counter" << endl;

	RFAFlagCubeBase::startData(verbose);

	iterTimecnt=0; // running count of visbuffers gone by.
	timecnt=0;
  
	///	(chunk.visIter()).setRowBlocking(0); 

}

void RFATimeFreqCrop :: AllocateMemory()
{
	
	/* casacore::Cube to hold visibility amplitudes : POLZN x CHAN x (IFR*TIME) */
        visc.resize(NumP,NumC,NumB*NumT);
	visc=0;
	
	/* casacore::Cube to hold visibility flags : POLZN x CHAN x (IFR*TIME) */
	flagc.resize(NumP,NumC,NumB*NumT);
	flagc=true;

	/* casacore::Vector to hold Row Flags : (IFR*TIME) */
	rowflags.resize(NumB*NumT);
	rowflags=true;

        /* casacore::Vector to hold baseline flags - to prevent unnecessary computation */
        baselineflags.resize(NumB);
        baselineflags=false;
	
	/* casacore::Cube to hold MEAN bandpasses : POLZN x IFR x CHAN */
	/* casacore::Cube to hold CLEAN bandpasses : POLZN x IFR x CHAN */
	if(CorrChoice == 0)
	  {
		meanBP.resize(NumP,NumAnt,NumC);
		cleanBP.resize(NumP,NumAnt,NumC);
	  }
	else
	  {
		meanBP.resize(NumP,NumB,NumC);
		cleanBP.resize(NumP,NumB,NumC);
	  }

	matpos = meanBP.shape();
	meanBP=0.0;
	cleanBP=0.0;

	//	cout << " BP Shape = " << matpos << endl;

	/* casacore::Cube to hold flags for the entire Chunk (channel subset, but all times) */
	chunkflags.resize(NumP,NumC,NumB*num(TIME));
        chunkflags=true;

	
	/* Temporary workspace vectors */	
	tempBP.resize(NumC);tempTS.resize(NumT);
	flagBP.resize(NumC);flagTS.resize(NumT);
	fitBP.resize(NumC);fitTS.resize(NumT);

	tempBP=0;tempTS=0;flagBP=false;flagTS=false;fitBP=0;fitTS=0;


}



/* Called once for every TIMESTAMP - for each VisBuf */
RFA::IterMode RFATimeFreqCrop :: iterTime (casacore::uInt itime) 
{
  
  //    cout << "iterTime :: " << itime << endl;
  //    RFAFlagCubeBase::iterTime(itime);
    RFDataMapper::setVisBuffer(vb);
   flag.advance(itime,true);
    
    //    vv = &vb.visCube(); // extract a viscube - one timestamp - one VisBuf
    //    vi.flag(ff); // extract the corresponding flags
    
    if(ant1.shape() != (vb.antenna1()).shape())
	ant1.resize((vb.antenna1()).shape());
    ant1 = vb.antenna1();
    
    if(ant2.shape() != (vb.antenna2()).shape())
	ant2.resize((vb.antenna2()).shape());
    ant2 = vb.antenna2();
    //const casacore::Vector<casacore::Int> &ifrs( chunk.ifrNums() );
    
    casacore::uInt nBaselinesInData = ant1.nelements();
    //    cout << "ant1 nelements  : " << nBaselinesInData << " timecnt : " << timecnt << " itertimecnt : " << iterTimecnt << endl;

      /* Polarizations/correlations */
      corrmask = RFDataMapper::corrMask(chunk.visIter());
      casacore::Vector<casacore::Int> corrlist(num(POLZN));
      for(casacore::uInt corr=0; corr<num(POLZN); corr++)
	corrlist[corr] = (casacore::Int) ( (corrmask >> corr) & 1 );

      //      if(nBaselinesInData != num(IFR)) cout << "nbaselines is not consistent !" << endl; 
        
    // Read in the data AND any existing flags into the flagCube - accumulate 
    //   timecnt : casacore::Time counter for each NumT subset
    //   itime : The row counter for each chunk
    casacore::Bool tfl;
    for(casacore::uInt pl=0;pl<NumP;pl++)
      {
    	for(casacore::uInt bs=0;bs<nBaselinesInData;bs++)
	  {
            casacore::Int countflags=0, countpnts=0;
            casacore::uInt baselinecnt = BaselineIndex(bs,ant1[bs],ant2[bs]);
            AlwaysAssert( baselinecnt<NumB, casacore::AipsError );
	    // Read in rowflag
            rowflags( (timecnt*NumB)+baselinecnt ) = flag.getRowFlag( chunk.ifrNum(bs), itime);
	    for(casacore::uInt ch=0;ch<NumC;ch++)
	      {
                // read the data. mapvalue evaluates 'expr'.
		visc(pl,ch,(timecnt*NumB)+baselinecnt) = mapValue(ch,bs);
                // read existing flags
		tfl = chunk.npass() ? flag.anyFlagged(ch,chunk.ifrNum(bs)) : flag.preFlagged(ch,chunk.ifrNum(bs));
                // sync with rowflag
                if( rowflags( (timecnt*NumB)+baselinecnt ) ) tfl=true;
                // ignore previous flags....
                if(IgnorePreflags) tfl=false;
 
                // Fill in the NumT sized flag array
		flagc(pl,ch,(timecnt*NumB)+baselinecnt) = tfl; //flag.anyFlagged(ch,ifrs(bs));

                // Fill in the chunk sized flag array
		chunkflags(pl,ch,(itime*NumB)+baselinecnt) = tfl; //flag.anyFlagged(ch,ifrs(bs));
                
                // Counters
		countpnts++;
		if(tfl) countflags ++;
	      }
	    //	    if(countflags>0) cout << "Time : " << itime << " Preflags for baseline : " << bs << " (" << ant1(bs) << "," << ant2(bs) << ") : " << countflags << " out of " << countpnts << " " << corrlist << endl;
	  }
      }
    
    timecnt++;
    iterTimecnt++; // running count of visbuffers going by.
    
    
    /* BEGIN TIME-FLAGGING ALGORITHM HERE */
    
    /* After accumulating NumT timestamps, start processing this block */
    /////if(iterTimecnt > 0 && (timecnt==NumT || iterTimecnt == (vi.nRowChunk()/NumB)))
    if(iterTimecnt > 0 && (timecnt==NumT || itime==(num(TIME)-1) ))
      {
        //ut << " timecnt : " << timecnt << "   itime : " << itime << "  iterTimecnt : " << iterTimecnt << "   NumT : " << NumT << endl;
	//        casacore::Int ctimes = timecnt;
        casacore::Int ctimes = NumT; // User-specified time-interval
        NumT = timecnt; // Available time-interval. Usually same as NumT - but could be less.
        //ut << " NumT going into all functions : " << NumT << endl;

	FlagZeros();		

	RunTFCrop();
	
	if(ShowFlagPlots() == RFA::STOP)
	  return RFA::STOP;
	
	ExtendFlags();
	
	FillChunkFlags();    

        // reset NumT to the user-specified time-interval
        NumT = ctimes;	

	// reset the NumT time counter !!!
	timecnt=0;
      }
    //    flag.advance(itime,true);
    return RFA::CONT; 
    ////RFAFlagCubeBase::iterTime(itime);
}


/* Called for each ROW - each baseline in a VisBuf */
// Fill in the visibilities and flags here.
// flag.advance() has to get called in iterTime, for iterRow to see the correct values.
RFA::IterMode RFATimeFreqCrop :: iterRow  (casacore::uInt irow ) 
{
  AlwaysAssert( irow <= NumT*NumB , casacore::AipsError);
  /* DUMMY CALL */
  return RFAFlagCubeBase::iterRow(irow);
}



/* If any data points are exactly zero, make sure corresponding flags
   are set. For the baseline mapper - this is an indicator of which baselines
   are missing (RowFlags */
void RFATimeFreqCrop :: FlagZeros()
{
  casacore::Float temp=0;
  //casacore::Bool flg=false;
  baselineflags=false;
  
  /* Check if the data in all channels are filled with zeros.
     If so, set the flags to zero  */    
  /* Also, if rowflags are set, set flags to zero. */
  /* Also, if all chans and times are flagged for a baseline, set the baselineflag
     baselineflag is used internally to skip unnecessary baselines */
  
  for(casacore::uInt pl=0;pl<NumP;pl++)
    {
      for(casacore::uInt bs=0;bs<NumB;bs++)
	{
          casacore::Bool bflag=true; // default is flagged. If anything is unflagged, this will change to false
	  for(casacore::uInt tm=0;tm<NumT;tm++)
	    {
              // If rowflag is set, flag all chans in it
              if(rowflags(tm*NumB+bs))
		{
                  for(casacore::uInt ch=0;ch<NumC;ch++)
		    flagc(pl,ch,tm*NumB+bs) = true;
		}
	      
              // Count flags across channels, and also count the data.
	      temp=0;
	      //flg=true;
	      for(casacore::uInt ch=0;ch<NumC;ch++)
		{
		  temp += visc(pl,ch,tm*NumB+bs);
		  //flg &= flagc(pl,ch,tm*NumB+bs);
		}
	      
              // If data is zero for all channels (not read in), set flags to zero.
	      if(temp==0) 
		{
		  rowflags(tm*NumB+bs)=true;
		  for(casacore::uInt ch=0;ch<NumC;ch++)
		    flagc(pl,ch,tm*NumB+bs)=true;
		}
	      
	      // Count flags across channels and time,,,,,
              // If any flag is false, bflag will become false
              for(casacore::uInt ch=0; ch<NumC; ch++)
                bflag &= flagc(pl,ch,tm*NumB+bs);
	      
	    }// for tm
          // If all times/chans are flagged for this baseline, set the baselineflag.
	  if(bflag) baselineflags(bs)=true;
	  else baselineflags(bs)=false;
	}// for bs
      casacore::Int ubs=0;
      for(casacore::uInt bs=0;bs<NumB;bs++)
           if(!baselineflags(bs)) ubs++;
      if(ShowPlots) cout << "Working with " << ubs << " unflagged baseline(s). " << endl;
    }// for pl
}// end of FlagZeros()




void RFATimeFreqCrop :: RunTFCrop()
{
  casacore::uInt a1,a2;
  
  meanBP = 0;
  cleanBP = 0;
  
  for(casacore::uInt pl=0;pl<NumP;pl++)
    {
      for(casacore::uInt bs=0;bs<NumB;bs++)
	{
	  if( !baselineflags(bs) )
	    {
	      Ants(bs,&a1,&a2);
	      if((CorrChoice==0 && a1 == a2)||(CorrChoice!=0 && a1 != a2)) 
		{
		  
		  FlagTimeSeries(pl,bs);    
		  
		  FitCleanBandPass(pl,bs);
		  
		  FlagBandPass(pl,bs);
		  
		  GrowFlags(pl,bs);
		}// if corrchoice
	    }// if baseline is not flagged
	}// end for bs
    }// end for pl  
  
}// end runTFCrop




/* Flag in time, and build the average bandpass */
/* Grow flags by one timestep, check for complete flagged baselines. */
void RFATimeFreqCrop :: FlagTimeSeries(casacore::uInt pl, casacore::uInt bs)
{
  //casacore::Float mn=0;
  casacore::Float sd=0,temp=0,flagcnt=0,tol=0;
  casacore::uInt a1,a2;
  //casacore::Bool flg=false;
  /* For each Channel - fit lines to 1-D data in time - flag according 
   * to them and build up the mean bandpass */
  
  casacore::Float rmean=0;
  Ants(bs,&a1,&a2);
	  //			cout << " Antennas : " << a1 << " & " << a2 << endl;
	  for(casacore::uInt ch=0;ch<NumC;ch++)
	    {
	      tempTS=0;flagTS=false;
	      for(casacore::uInt tm=0;tm<NumT;tm++)
		{
		  tempTS[tm] = visc(pl,ch,((tm*NumB)+bs));
		  flagTS[tm] = flagc(pl,ch,((tm*NumB)+bs));
		}//for tm
	      
	      
	      rmean += UMean(tempTS,flagTS);
	      
	      temp=0;
	      for(int loop=0;loop<5;loop++)
		{
		  // UUU : HERE - give choice of PolyFit in time...
		  LineFit(tempTS,flagTS,fitTS,0,tempTS.nelements()-1);	
		  sd = UStd(tempTS,flagTS,fitTS);
		  
		  for(casacore::uInt i=0;i<NumT;i++)
		    if(flagTS[i]==false && fabs(tempTS[i]-fitTS[i]) > T_TOL*sd)
		      {
			flagTS[i]=true ;flagcnt++;
		      }
		  if(fabs(temp-sd) < 0.1)break;
		  temp=sd;
		}
	      
	      // If sum of 2 adjacent flags also crosses threshold, flag 
	      /*
	      for(casacore::uInt i=1;i<NumT-1;i++)
		{
		  if(flagTS[i])
		    {
		      if( ( fabs(tempTS[i-1]-fitTS[i-1]) + fabs(tempTS[i+1]-fitTS[i+1]) ) > T_TOL*sd )
			{flagTS[i-1]=true; flagTS[i+1]=true;}
		    }
		}
	      */
	      
	      meanBP(pl,bs,ch) = UMean(tempTS,flagTS) ;
	      
	      /* write flags to local flag cube */
	      for(casacore::uInt tm=0;tm<NumT;tm++)
		flagc(pl,ch,((tm*NumB)+bs))=flagTS[tm];
	      
	    }//for ch
	  
	  
	  /* Check for completely flagged ants/bs */
	  if(1)
	    {
	      if((CorrChoice==0 && a1 == a2)||(CorrChoice!=0 && a1 != a2)) 
		{
		  if(CorrChoice==0)tol=ANT_TOL;
		  else tol=BASELN_TOL;
		  if(fabs(rmean/float(NumC)) < tol)
		    {
		      for(casacore::uInt ch=0;ch<NumC;ch++)
			for(casacore::uInt tm=0;tm<NumT;tm++)
			  flagc(pl,ch,((tm*NumB)+bs))=true;
		      if(CorrChoice==0) 
			cout << "Antenna Flagged : " << a1 << endl;
		      else
			cout << "Mean : " << rmean/NumC << " : Baseline Flagged : " << a1 << ":" << a2 << endl;
		    }
		  
		}
	    }///if(0);		
}// end of FlagTimeSeries    

/* Fit a smooth bandpass to the mean bandpass and store it 
 *  one for each baseline */

// matpos  :  NumP. NumB. NumC

void RFATimeFreqCrop :: FitCleanBandPass(casacore::uInt pl, casacore::uInt bs)
{    
  //casacore::Float mn=0,sd=0,temp=0,flagcnt=0,tol=0;
  casacore::uInt a1,a2;
  casacore::Bool flg=false;
  
	  Ants(bs,&a1,&a2);
	  /* Fit a smooth bandpass */
	  flg=true;
	  for(casacore::uInt ch=0;ch<NumC;ch++)	
	    {
	      tempBP[ch] = meanBP(pl,bs,ch);
	      if(tempBP[ch] != 0) flg=false;
	    }
	  
	  if(flg==false)
	    {
	      /* Piecewise Poly Fit to the meanBP */
	      if(!FreqLineFit)
		{
		  CleanBand(tempBP,fitBP);	
		}
	      else
		{
		  /* LineFit to flag off a line fit in frequency */
		  flagBP=false;
		  for(casacore::uInt ch=0;ch<tempBP.nelements();ch++)
		    if(tempBP[ch]==0) flagBP[ch]=true;
		  LineFit(tempBP,flagBP,fitBP,0,tempBP.nelements()-1);	
		}
	      
#ifdef PLOT 
	      if(CorrChoice==0)
		cout<<" Antenna : "<<bs<<" Polzn : "<<pl<<endl;
	      else
		{
		  Ants(bs,&a1,&a2);
		  cout << " Baseline : " << a1 << ":" << a2 << " Polzn : " << pl << endl;
		}
	      Plot_ds9(tempBP.nelements(), tempBP,fitBP);  // Plot the band
#endif
	    }
	  /*	  else
		  {
		  Ants(bs,&a1,&a2);
                  emptylist += casacore::String::toString(a1)+"-"+casacore::String::toString(a2)+" ";
		  // 	         cout << "meanBP is filled with zeros : baseline : " << a1 << "-" << a2 << endl;
		  }
	  */
	  for(casacore::uInt ch=0;ch<NumC;ch++)
	    {
	      if(flg==false) cleanBP(pl,bs,ch)= fitBP[ch];
	      else cleanBP(pl,bs,ch)=0;
	    }
	  
}// end FitCleanBandPass    

/* FLAGGING IN FREQUENCY */
void RFATimeFreqCrop :: FlagBandPass(casacore::uInt pl, casacore::uInt bs)
{
  casacore::Float mn=0,sd=0,temp=0,flagcnt=0;
  casacore::uInt a1,a2;
  //casacore::Bool flg=false;
  

	  Ants(bs,&a1,&a2);
	  
	  for(casacore::uInt tm=0;tm<NumT;tm++)
	    {
	      /* Divide (or subtract) out the clean bandpass */
	      tempBP=0,flagBP=0;
	      for(casacore::uInt ch=0;ch<NumC;ch++)
		{
		  flagBP[ch] = flagc(pl,ch,((tm*NumB)+bs));
		  if(flagBP[ch]==false)
		    tempBP[ch] = visc(pl,ch,((tm*NumB)+bs))/cleanBP(pl,bs,ch);
		}//for ch
	      
	      /* Flag outliers */
	      temp=0;
	      for(casacore::Int loop=0;loop<5;loop++)
		{
		  mn=1;
		  sd = UStd(tempBP,flagBP,mn);
		  
		  for(casacore::uInt ch=0;ch<NumC;ch++)
		    {
		      if(flagBP[ch]==false && fabs(tempBP[ch]-mn) > F_TOL*sd)
			{
			  flagBP[ch]=true ;flagcnt++;
			}
		    }
		  if(fabs(temp-sd) < 0.1)break;
		  temp=sd;
		}
	      
	      /* If sum of power in two adjacent channels is more than thresh, flag both side chans */
	      if(FlagLevel>0)
		{
	      for(casacore::uInt ch=1;ch<NumC-1;ch++)
		{
		  if(flagBP[ch])
		    {
		      if( ( fabs(tempBP[ch-1]-mn) + fabs(tempBP[ch+1]-mn) ) > F_TOL*sd )
			{flagBP[ch-1]=true; flagBP[ch+1]=true;}
		    }
		}
		}

	      /* Fill the flags into the visbuffer array */
	      for(casacore::uInt ch=0;ch<NumC;ch++)
		flagc(pl,ch,((tm*NumB)+bs))=flagBP[ch];
	      
	      
	    }//for tm
	  
}// end FlagBandPass    

/* APPLY FLAG HEURISTICS ON THE FLAGS FOR ALL AUTOCORRELATIONS */
void RFATimeFreqCrop :: GrowFlags(casacore::uInt pl, casacore::uInt bs)
{
  casacore::uInt a1,a2;
  //casacore::Bool flg=false;
  
  if(FlagLevel > 0)
    {
	      Ants(bs,&a1,&a2);
	      
	      for(casacore::uInt ch=0;ch<NumC;ch++)
		{ 
		  // casacore::uInt fsum=0;
		  for(casacore::uInt tm=0;tm<NumT;tm++)
		    {       
		      if(FlagLevel>1) // flag level 2 and above...
			{
			  // flagging one timestamp before and after
			  if(tm>0)
			    if(flagc(pl,ch,((tm*NumB+bs)))==true)
			      flagc(pl,ch,(((tm-1)*NumB+bs)))=true;
			  
			  if((NumT-tm)<NumT-1)
			    if(flagc(pl,ch,(((NumT-tm)*NumB+bs)))==true)
			      flagc(pl,ch,(((NumT-tm+1)*NumB+bs)))=true;
			}
		      
		      if(FlagLevel>1) // flag level 2 and above
			{
			  // flagging one channel before and after
			  if(ch>0)
			    if(flagc(pl,ch,((tm*NumB+bs)))==true)
			      flagc(pl,ch-1,((tm*NumB+bs)))=true;
			  
			  if((NumC-ch)<NumC-1)
			    if(flagc(pl,(NumC-ch),(tm*NumB+bs))==true)
			      flagc(pl,(NumC-ch+1),(tm*NumB+bs))=true;
			}
		      
		      if(FlagLevel>0) // flag level 1 and above
			{
			  /* If previous and next channel are flagged, flag it */
			  if(ch>0 && ch < NumC-1)
			    {
			      if( flagc(pl,ch-1,(tm*NumB+bs) ) == true 
				  && flagc(pl,ch+1,(tm*NumB+bs) ) == true  )
				flagc(pl,ch,(tm*NumB+bs) ) = true;
			    }
			  /* If previous and next timestamp are flagged, flag it */
			  if(tm>0 && tm < NumT-1)
			    {
			      if( flagc(pl,ch,((tm-1)*NumB+bs) ) == true 
				  && flagc(pl,ch,((tm+1)*NumB+bs) ) == true  )
				flagc(pl,ch,(tm*NumB+bs) ) = true;
			    }
			}
		      
		      if(FlagLevel>1) // flag level 2 and above
			{
			  /* If next two channels are flagged, flag it */
			  if(ch < NumC-2)
			    if( flagc(pl,ch+1,(tm*NumB+bs)) == true 
				&& flagc(pl,ch+2,(tm*NumB+bs) ) == true  )
			      flagc(pl,ch,(tm*NumB+bs) ) = true;
			}
		      
		    }//for tm
		  
		  // if more than 60% of the timetange flagged - flag whole timerange for that channel
		  //if(fsum < 0.4*NumT && FlagLevel > 1) // flag level 2 
		  //	for(casacore::uInt tm=0;tm<NumT;tm++)
		  //		flagc(pl,ch,((tm*NumB+bs)))=true;
		}//for ch
	      
	      if(FlagLevel>0) // flag level 1 and above
		{
		  /* If more than 4 surrounding points are flagged, flag it */
		  casacore::uInt fsum2=0;
		  for(casacore::uInt ch=1;ch<NumC-1;ch++)
		    {
		      for(casacore::uInt tm=1;tm<NumT-1;tm++)
			{
			  fsum2 = (casacore::uInt)(flagc(pl,ch-1,(((tm-1)*NumB+bs)))) + (casacore::uInt)(flagc(pl,ch-1,((tm*NumB+bs)))) + 
			    (casacore::uInt)(flagc(pl,ch-1,(((tm+1)*NumB+bs)))) + (casacore::uInt)(flagc(pl,ch,(((tm-1)*NumB+bs)))) + 
			    (casacore::uInt)(flagc(pl,ch,(((tm+1)*NumB+bs)))) + (casacore::uInt)(flagc(pl,ch+1,(((tm-1)*NumB+bs)))) + 
			    (casacore::uInt)(flagc(pl,ch+1,((tm*NumB+bs)))) + (casacore::uInt)(flagc(pl,ch+1,(((tm+1)*NumB+bs))));
			  if(fsum2 > 4) flagc(pl,ch,((tm*NumB+bs))) = true;
			} // for tm
		    }// for ch
		}// if FlagLevel>0
	      
	      if(FlagLevel>0) // flaglevel = 1 and above
		{
		  casacore::uInt fsum2=0;
		  /* Grow flags in time */
		  for(casacore::uInt ch=0;ch<NumC;ch++)
		    { 
		      fsum2=0;
		      /* count unflagged points for this channel (all times) */
		      for(casacore::uInt tm=0;tm<NumT;tm++)
			fsum2 += (flagc(pl,ch,((tm*NumB+bs)))==true)?0:1 ; 
		      /*if more than 50% of the timetange flagged - flag whole timerange for that channel */
		      if(fsum2 < 0.5*NumT)
			for(casacore::uInt tm=0;tm<NumT;tm++)
			  flagc(pl,ch,((tm*NumB+bs)))=true;
		    }// for ch
		}// if flaglevel>0
	      
    }//if flag_level
  
  // Count flags
  /*
    casacore::Float runningcount=0, runningflag=0;
    runningcount=0;
    runningflag=0;
    for(int pl=0;pl<NumP;pl++)
    {
    for(casacore::uInt bs=0;bs<NumB;bs++)
    {
    Ants(bs,&a1,&a2);
    if(CorrChoice==0)
    {if(a1 != a2) continue;} // choose autocorrelations
    else
    {if(a1==a2) continue;} // choose cross correlations
    
    for(int ch=0;ch<NumC;ch++)
    {
    for(casacore::uInt tm=0;tm<NumT;tm++)
    {
    runningflag += casacore::Float(flagc(pl,ch,(tm*NumB)+bs));
    runningcount++ ;
    }// for tm
    }//for ch
    }// for bs
    }// for pl
    
    
    cout << " Flagged : " << 100 * runningflag/runningcount << " % for timesteps " << iterTimecnt-NumT << " - " << iterTimecnt << endl;
  */
}// end of GrowFlags

/* GRAY SCALE DISPLAYS ON ds9 */
RFA::IterMode RFATimeFreqCrop :: ShowFlagPlots()
{    
  casacore::uInt a1,a2;
  if(ShowPlots)
    {
      
      //	cout << "About to display : allocating cubes" << endl;	
      /*      
	      casacore::Float **dispdat=NULL,**flagdat=NULL;
	      dispdat = (casacore::Float **) malloc(sizeof(casacore::Float *) * NumT + sizeof(casacore::Float)*NumC*NumT);
	      for(casacore::uInt i=0;i<NumT;i++)
	      dispdat[i] = (casacore::Float *) (dispdat + NumT) + i * NumC;
	      
	      flagdat = (casacore::Float **) malloc(sizeof(casacore::Float *) * NumT + sizeof(casacore::Float)*NumC*NumT);
	      for(casacore::uInt i=0;i<NumT;i++)
	      flagdat[i] = (casacore::Float *) (flagdat + NumT) + i * NumC;
      */
      
      casacore::IPosition shp(2),tshp(2); shp(0)=NumC; shp(1)=NumT;
      casacore::Matrix<casacore::Float> dispdat(shp), flagdat(shp);
      
      //	cout << "About to display : allocated. "  << endl;
      
      char choice = 'a';
      
      casacore::Float runningsum=0, runningflag=0, oldrunningflag=0;
      for(casacore::uInt pl=0;pl<NumP;pl++)
	{
	  if(choice == 's') continue;
	  for(casacore::uInt bs=0;bs<NumB;bs++)
	    {
	      if(choice == 's') continue;
	      if(baselineflags(bs)) continue;
	      
	      runningsum=0;
	      runningflag=0;
              oldrunningflag=0;
	      Ants(bs,&a1,&a2);
	      if(CorrChoice==0)
		{if(a1 != a2) continue;} // choose autocorrelations
	      else
		{if(a1==a2) continue;} // choose cross correlations
	      
	      for(casacore::uInt ch=0;ch<NumC;ch++)	
		{
		  tempBP[ch] = meanBP(pl,bs,ch);
		  fitBP[ch] = cleanBP(pl,bs,ch);
		}
	      
	      for(casacore::uInt ch=0;ch<NumC;ch++)
		{ 
		  for(casacore::uInt tm=0;tm<NumT;tm++)
		    {       
                      // casacore::Data with pre-flags
		      dispdat(ch,tm) = visc(pl,ch,(((tm*NumB)+bs))) * (!chunkflags(pl,ch,((tm+iterTimecnt-NumT)*NumB)+bs));
                      // casacore::Data with all flags (pre and new)
		      flagdat(ch,tm) = dispdat(ch,tm)*(!flagc(pl,ch,(tm*NumB)+bs));
                      // Sum of the visibilities (all of them, flagged and unflagged)
		      runningsum += visc(pl,ch,(((tm*NumB)+bs)));
		      /*
                      // Count of all flags
		      runningflag += (casacore::Float)(flagc(pl,ch,(tm*NumB)+bs));
                      // Count of only pre-flags
                      oldrunningflag += (casacore::Float)(chunkflags(pl,ch,((tm+iterTimecnt-NumT)*NumB)+bs));
		      */  ////// CHECK that iterTimecnt is correct, and a valid part of chunkflags is being read !!!!!!!!!
                      // Count of all flags
		      if( (flagc(pl,ch,(tm*NumB)+bs)) ) runningflag++;
                      // Count of only pre-flags
                      if(chunkflags(pl,ch,((tm+iterTimecnt-NumT)*NumB)+bs)) oldrunningflag++;
		    }//for tm
		}//for ch
	      
	      //Plot on ds9 !!
	      
	      //		cout << "Antenna1 : " << a1 << "  Antenna2 : " << a2 << "  Polarization : " << pl << endl;
	      //		cout << "Vis sum : " << runningsum << " Flag % : " << 100 * runningflag/(NumC*NumT) << endl;
	      //	      cout << " Flagged : " << 100 * runningflag/(NumC*NumT) << " %" << endl;
	      cout << " Flagged : " << 100 * runningflag/(NumC*NumT) << " %  (Pre-Flag : " << 100 * oldrunningflag/(NumC*NumT) << " %) on " << Expr << " for timesteps " << iterTimecnt-NumT << " - " << iterTimecnt << " on baseline " << a1 << "-" << a2;
	      
	      if(!runningsum)
		{
		  cout << " : No non-zero data !" << endl;
		}
	      else
		{
                  cout << endl;
		  
		  Display_ds9(NumC,NumT,dispdat,1);
		  Display_ds9(NumC,NumT,flagdat,2);
		  Plot_ds9(tempBP.nelements(), tempBP, fitBP);
		  //UPlot(tempBP,fitBP,0,tempBP.nelements()-1);  // Plot the band
		  
		  cout << "Press <c> to continue display, <s> to stop display but continue flagging, <q> to quit." << endl;
		  //getchar();
		  cin >> choice;
		  //		    cout << " Choice : " << choice << endl;
		  switch(choice)
		    {
		    case 'q': 
		      ShowPlots = false; 
		      StopAndExit = true;
		      cout << "Exiting flagger" << endl;
		      return RFA::STOP;
		    case 's': 
		      ShowPlots = false;
		      cout << "Stopping display. Continuing flagging." << endl;
		      break;
		    default:
		      break;
		    }
		}
	    }//for bs
	}//for pl
      
    }// end of if ShowPlots
    return RFA::CONT;
}// end ShowFlagPlots

/* Extend Flags
   (1) If flagging on self-correlations, extend to cross-corrs.
   (2) If requested, extend across polarization (fiddle with corrmask)
   (3) If requested, extend across baseline/antenna
*/
void RFATimeFreqCrop :: ExtendFlags()
{    
  casacore::uInt a1,a2;
  
  /* FLAG BASELINES FROM THE SELF FLAGS */
  if(CorrChoice ==0)
    {	
      cout << " Flagging Cross correlations from self correlation flags " << endl;
      for(casacore::uInt pl=0;pl<NumP;pl++)
	{
	  for(casacore::uInt bs=0;bs<NumB;bs++)
	    {
	      if(baselineflags(bs)) continue;
	      Ants(bs,&a1,&a2);
	      if(a1 == a2) continue; // choose cross correlations
	      for(casacore::uInt ch=0;ch<NumC;ch++)
		{
		  for(casacore::uInt tm=0;tm<NumT;tm++)
		    {
		      flagc(pl,ch,((tm*NumB+bs))) = flagc(pl,ch,((tm*NumB)+SELF(a1))) | flagc(pl,ch,((tm*NumB)+SELF(a1))); 
		    }//for tm
		}//for ch
	    }//for bs
	}//for pl
    }
  
}// end Extend Flags    

void RFATimeFreqCrop :: FillChunkFlags()
{
  //cout << " Diagnostics on flag cube " << endl;
  
  for(casacore::uInt pl=0;pl<NumP;pl++)
    {
      for(casacore::uInt bs=0;bs<NumB;bs++)
	{
	  //if(RowFlags(pl,(tm*NumB)+bs)==true)
	  //	continue;
	  for(casacore::uInt ch=0;ch<NumC;ch++)
	    {
	      for(casacore::uInt tm=0;tm<NumT;tm++)
		{
		  if(flagc(pl,ch,((tm*NumB)+bs))==true )  
		    {
		      chunkflags(pl,ch,((tm+iterTimecnt-NumT)*NumB)+bs) = true;
		    }
		}// for tm
	    }//for ch
	}// for bs
    }// for pl
  
  // what is this ??  
  timecnt=0;
  
}// end of FillChunkFlags



/*RedFlagger::run - ends loop for all agents with endData 
 * Calls endData once at the end of each PASS */
RFA::IterMode RFATimeFreqCrop :: endData  () 
{
  //  cout << " In End Data. Ending timecnt :  "  << timecnt << endl;
  
  RFAFlagCubeBase::endData();
  return RFA::STOP;
}


/* Called at the beginning of each PASS */
void RFATimeFreqCrop :: startFlag (bool verbose) 
{
  //  corrmask = RFDataMapper::corrMask(chunk.visIter());
  //  cout  << "StartFlag : corrmask : " << corrmask << endl;
  
  RFAFlagCubeBase::startFlag(verbose);
  
}


/* Write Flags to casacore::MS */
void RFATimeFreqCrop :: iterFlag(casacore::uInt itime)
{
  //  cout << "iterFlag :: Set flags for time : " << itime << endl;
  
  // FLAG DATA
  
  if(!DryRun)
    {
      flag.advance(itime);
      corrmask = RFDataMapper::corrMask(chunk.visIter());
      
      const casacore::Vector<casacore::Int> &ifrs( chunk.ifrNums() );
      if(ant1.shape() != (vb.antenna1()).shape())
	ant1.resize((vb.antenna1()).shape());
      ant1 = vb.antenna1();
      
      if(ant2.shape() != (vb.antenna2()).shape())
	ant2.resize((vb.antenna2()).shape());
      ant2 = vb.antenna2();
      
      casacore::uInt nbs = ant1.nelements();
      for(casacore::uInt pl=0;pl<NumP;pl++)
	{
	  for(casacore::uInt bs=0;bs<nbs;bs++)
	    {
              casacore::Bool bflag=true;
	      casacore::uInt baselinecnt = BaselineIndex(bs,ant1[bs],ant2[bs]);
	      for(casacore::uInt ch=0;ch<NumC;ch++)
		{
		  if(chunkflags(pl,ch,(itime*NumB)+baselinecnt)==(casacore::Bool)true)
		    flag.setFlag(ch,ifrs(bs));
                  bflag &= chunkflags(pl,ch,(itime*NumB)+baselinecnt);
		}
              if(bflag) flag.setRowFlag(ifrs(bs),itime);
	    }
	}
      
      flag.setMSFlags(itime);
      
    }// if not dry-run
  else
    {
      RFAFlagCubeBase::iterFlag(itime);
    }
  
  /// FLAG ROWS  
  /*
    ant1 = vb.antenna1();
    ant2 = vb.antenna2();
    casacore::uInt npols = (chunkflags.shape())[0];
    casacore::uInt nbs = ant1.nelements();
    
    vi.flag(ff);
    vi.flagRow(fr);
    
    for(casacore::uInt bs=0;bs<nbs;bs++)
    {
    casacore::Bool rowflag=true;
    for(casacore::uInt pl=0;pl<npols;pl++)
    {
    for(casacore::uInt ch=StartChan;ch<=EndChan;ch++)
    {
    ff(pl,ch,bs) = chunkflags(pl,ch-StartChan,(itime*NumB)+baselinecnt);
    rowflag &= ff(pl,ch,bs);
    }
    }
    if(rowflag) fr[bs]=true;
    }
    
    //vi.setFlag(ff);
    //vi.setFlagRow(fr); 
    
    chunk.visIter().setFlag(ff);
    chunk.visIter().setFlagRow(fr);
    
  */
}

/*RedFlagger::run - calls 'endChunk()' on all agents. */

void RFATimeFreqCrop :: endChunk () 
{
  casacore::LogIO os(casacore::LogOrigin("tfcrop","endChunk","WHERE"));
  //     cout << "endChunk : counting flags" << endl;
  // Count flags
  if(!StopAndExit)
    {
      casacore::Float runningcount=0, runningflag=0;
      
      //const casacore::Vector<casacore::Int> &ifrs( chunk.ifrNums() );
      if(ant1.shape() != (vb.antenna1()).shape())
	ant1.resize((vb.antenna1()).shape());
      ant1 = vb.antenna1();
      
      if(ant2.shape() != (vb.antenna2()).shape())
	ant2.resize((vb.antenna2()).shape());
      ant2 = vb.antenna2();
      
      casacore::uInt nbs = ant1.nelements();
      for(casacore::uInt pl=0;pl<NumP;pl++)
	{
	  for(casacore::uInt bs=0;bs<nbs;bs++)
	    {
	      casacore::uInt baselinecnt = BaselineIndex(bs,ant1[bs],ant2[bs]);
	      for(casacore::uInt ch=0;ch<NumC;ch++)
		{
		  for(casacore::uInt tm=0;tm<num(TIME);tm++)
		    {
		      if (chunkflags(pl,ch,((tm)*NumB)+baselinecnt)) runningflag++;
		      runningcount++;
		    }
		}
	    }
	}
      
      /* Polarizations/correlations */
      corrmask = RFDataMapper::corrMask(chunk.visIter());
      casacore::Vector<casacore::Int> corrlist(num(POLZN));
      for(casacore::uInt corr=0; corr<num(POLZN); corr++)
	corrlist[corr] = (casacore::Int) ( (corrmask >> corr) & 1 );
      
      //      cout << "--> Flagged " << 100 * runningflag/runningcount << " % on " << Expr << ". Applying to correlations : " << corrlist << endl;
      os << "TFCROP : Flagged " << 100 * runningflag/runningcount << " % on " << Expr << ". Applying to corrs : " << corrlist;
      if(DryRun) os << " (Not writing flags to MS)" << casacore::LogIO::POST;
      else os << " (Writing flags to MS)" << casacore::LogIO::POST;
    }
  RFAFlagCubeBase::endChunk();
  ///  (chunk.visIter()).setRowBlocking(0); //reset to default
  //      	cout << " End of endChunk !!" << endl;
}


/* Destructor for RFATimeFreqCrop */


RFATimeFreqCrop :: ~RFATimeFreqCrop () 
{
  //cout << "destructor for RFATimeFreqCrop" << endl;
}




/* Calculate the MEAN of 'vect' ignoring values flagged in 'flag' */
casacore::Float RFATimeFreqCrop :: UMean(casacore::Vector<casacore::Float> vect, casacore::Vector<casacore::Bool> flag)
{
  casacore::Float mean=0;
  int cnt=0;
  for(int i=0;i<(int)vect.nelements();i++)
    if(flag[i]==false)
      {
	mean += vect[i];
	cnt++;
      }
  if(cnt==0) cnt=1;
  return mean/cnt;
}


/* Calculate the STANDARD DEVN. of 'vect' w.r.to a given 'fit' 
 * ignoring values flagged in 'flag' */
casacore::Float RFATimeFreqCrop :: UStd(casacore::Vector<casacore::Float> vect, casacore::Vector<casacore::Bool> flag, casacore::Vector<casacore::Float> fit)
{
  casacore::Float std=0;
  int n=0,cnt=0;
  n = vect.nelements() < fit.nelements() ? vect.nelements() : fit.nelements();
  for(int i=0;i<n;i++)
    if(flag[i]==false)
      {
	cnt++;
	std += (vect[i]-fit[i])*(vect[i]-fit[i]);
      }
  if(cnt==0) cnt=1;
  return sqrt(std/cnt);
}


/* Calculate the STANDARD DEVN. of 'vect' w.r.to a given mean 
 * ignoring values flagged in 'flag' */
casacore::Float RFATimeFreqCrop :: UStd(casacore::Vector<casacore::Float> vect, casacore::Vector<casacore::Bool> flag, casacore::Float mean)
{
  casacore::Float std=0;
  int cnt=0;
  for(int i=0;i<(int)vect.nelements();i++)
    if(flag[i]==false)
      {
	cnt++;
	std += (vect[i]-mean)*(vect[i]-mean);
      }
  return sqrt(std/cnt);
}

/* Fit Piecewise polynomials to 'data' and get the 'fit' */
void RFATimeFreqCrop :: CleanBand(casacore::Vector<casacore::Float> data,casacore::Vector<casacore::Float> fit)
{
  //    casacore::Int step=0,ind=0;
  casacore::Int deg=0; //start=0;
  casacore::Int left=0,right=0;
  //  casacore::Int le=0,ri=0;
  casacore::Float sd,TOL=3;
  casacore::Vector<casacore::Float> tdata;
  casacore::Vector<casacore::Bool> tfband;
  
  tfband.resize(data.nelements());
  tdata.resize(data.nelements());
  
  tfband = false;
  tdata = data;
  
  /* replace empty data values by adjacent values */
  for(casacore::uInt i=0;i<tdata.nelements();i++)
    {
      if(tdata[i]==0)
	{
	  if(i==0)// find first non-zero value and set to that.
	    {
	      casacore::Int ind=0;
	      for(casacore::uInt j=1;j<tdata.nelements();j++)
		if(tdata[j]!=0){ind=j;break;}
	      if(ind==0) tdata[i]=0;
	      else tdata[i]=tdata[ind];
	    }
	  else// find next non-zero value and interpolate.
	    {
	      casacore::Int indr=0;
	      for(casacore::uInt j=i+1;j<tdata.nelements();j++)
		if(tdata[j]!=0){indr=j;break;}
	      casacore::Int indl=-1;
	      for(int j = i ; j >= 0 ; j--)
		if(tdata[j]!=0){indl=j;break;}
	      
	      if(indl==-1 && indr==0) tdata[i]=0;
	      if(indl>-1 && indr==0) tdata[i]=tdata[indl];
	      if(indl==-1 && indr>0) tdata[i]=tdata[indr];
	      if(indl>-1 && indr>0) tdata[i]=(tdata[indl]+tdata[indr])/2.0;
	    }
	}
    }
  
  
  /* If there still are empty points (entire spectrum is flagged) flag it. */
  for(casacore::uInt i=0;i<tdata.nelements();i++)
    if(tdata[i]==0) 
      {
	//cout << "chan " << i << " is blank" << endl;
	tfband[i]=true;
      }
  
  fit = tdata;
  
  casacore::Int psize=1;
  casacore::Int leftover=1,leftover_front=0,npieces=1;
  
  deg=1;
  npieces=1;
  
  for(casacore::uInt j=0;j<=4;j++)
    {
      //     if(j==0) {deg = 1;npieces=1;}
      //     if(j==1) {deg = 1;npieces=5;}
      //     if(j==2) {deg = 2;npieces=6;}
      //     if(j==3) {deg = 3;npieces=7;}
      //     if(j==4) {deg = 3;npieces=8;}
      
      npieces = MIN(2*j+1, MaxNPieces);
      if(j>1) {deg=2;}
      if(j>2) {deg=3;}
      
      psize = (int)(tdata.nelements()/npieces);
      //     cout << "Iter : " << j << " with Deg : " << deg << " and Piece-size : " << psize << endl;
      
      leftover = (int)(tdata.nelements() % npieces);
      
      leftover_front = (int)(leftover/2.0);
      
      left=0; right=tdata.nelements()-1;
      for(casacore::Int p=0;p<npieces;p++)
	{
	  if(npieces>1)
	    {
	      left = leftover_front + p*psize;
	      right = left + psize; 
	      
	      if(p==0) {left = 0; right = leftover_front + psize;}
	      if(p==npieces-1) {right = tdata.nelements()-1;} 
	    }
	  if(deg==1) 
	    LineFit(tdata,tfband,fit,left,right);
	  else 
	    PolyFit(tdata,tfband,fit,left,right,deg);
	}
      
      /* Now, smooth the fit - make this nicer later */
      
      int winstart=0, winend=0;
      float winsum=0.0;
      int offset=2;
      for(casacore::uInt i=offset;i<tdata.nelements()-offset;i++)
	{
	  winstart = i-offset;
	  winend = i+offset;
	  if(winstart<0)winstart=0;
	  if(static_cast<casacore::uInt>(winend)>=tdata.nelements()) winend=tdata.nelements()-1;
	  if(winend <= winstart) break;
	  winsum=0.0;
	  for(casacore::uInt wi=winstart;wi<=static_cast<casacore::uInt>(winend);++wi)
	    winsum += fit[wi];
	  fit[i] = winsum/(winend-winstart+1);
	}
      
      
      /* Calculate the STD of the fit */
      sd = UStd(tdata,tfband,fit);
      if(j>=2)  TOL=2;
      else TOL=3;
      
      /* Display the Fit and the data */
#ifdef UPLOT 
      Plot_ds9(data.nelements(),data,fit1);	 
#endif
      
      /* Detect outliers */
      for(casacore::uInt i=0;i<tdata.nelements();i++)
	{
	  if(tdata[i]-fit[i] > TOL*sd) 
	    tfband[i]=true;
	}
      
    } // for j
  
} // end of CleanBand



  /* Fit a polynomial to 'data' from lim1 to lim2, of given degree 'deg', 
   * taking care of flags in 'flag', and returning the fitted values in 'fit' */
void RFATimeFreqCrop :: PolyFit(casacore::Vector<casacore::Float> data,casacore::Vector<casacore::Bool> flag, casacore::Vector<casacore::Float> fit, casacore::uInt lim1, casacore::uInt lim2,casacore::uInt deg)
{
  static casacore::Vector<casacore::Double> x;
  static casacore::Vector<casacore::Double> y;
  static casacore::Vector<casacore::Double> sig;
  static casacore::Vector<casacore::Double> solution;
  
  casacore::uInt cnt=0;
  for(casacore::uInt i=lim1;i<=lim2;i++)
    if(flag[i]==false) cnt++;
  
  if(cnt <= deg)
    {
      LineFit(data,flag,fit,lim1,lim2);
      return;
    }
  
  
  casacore::LinearFit<casacore::Double> fitter;
  casacore::Polynomial<casacore::AutoDiff<casacore::Double> > combination(deg);
  
  
  combination.setCoefficient(0,0.0);
  if (deg >= 1) combination.setCoefficient(1, 0.0);
  if (deg >= 2) combination.setCoefficient(2, 0.0);
  if (deg >= 3) combination.setCoefficient(3, 0.0);
  if (deg >= 4) combination.setCoefficient(4, 0.0);
  
  x.resize(lim2-lim1+1);
  y.resize(lim2-lim1+1);
  sig.resize(lim2-lim1+1);
  solution.resize(deg+1);
  
  for(casacore::uInt i=lim1;i<=lim2;i++)
    {
      x[i-lim1] = i+1;
      y[i-lim1] = data[i];
      sig[i-lim1] = (flag[i]==true)?0:1;
    }
  
  fitter.asWeight(true);
  
  fitter.setFunction(combination);
  solution = fitter.fit(x,y,sig);
  
  for(casacore::uInt i=lim1;i<=lim2;i++)
    {
      fit[i]=0;
      for(casacore::uInt j=0;j<deg+1;j++)
	fit[i] += solution[j]*pow((double)(x[i-lim1]),(double)j);
    }
  
}



/* Fit a LINE to 'data' from lim1 to lim2, taking care of flags in 
 * 'flag', and returning the fitted values in 'fit' */
void RFATimeFreqCrop :: LineFit(casacore::Vector<casacore::Float> data, casacore::Vector<casacore::Bool> flag, casacore::Vector<casacore::Float> fit, casacore::uInt lim1, casacore::uInt lim2)
{
  float Sx = 0, Sy = 0, Sxx = 0, Sxy = 0, S = 0, a, b, sd, mn;
  
  mn = UMean(data, flag);
  sd = UStd (data, flag, mn);
  
  for (casacore::uInt i = lim1; i <= lim2; i++)
    {
      if (flag[i] == false) // if unflagged
	{
	  S += 1 / (sd * sd);
	  Sx += i / (sd * sd);
	  Sy += data[i] / (sd * sd);
	  Sxx += (i * i) / (sd * sd);
	  Sxy += (i * data[i]) / (sd * sd);
	}
    }
  a = (Sxx * Sy - Sx * Sxy) / (S * Sxx - Sx * Sx);
  b = (S * Sxy - Sx * Sy) / (S * Sxx - Sx * Sx);
  
  for (casacore::uInt i = lim1; i <= lim2; i++)
    fit[i] = a + b * i;
  
}

/* Return antenna numbers from baseline number - upper triangle storage */
void RFATimeFreqCrop :: Ants(casacore::uInt bs, casacore::uInt *a1, casacore::uInt *a2)
{
  casacore::uInt sum=0,cnt=0;
  for(casacore::uInt i=(NumAnt);i>1;i--)
    {
      sum += i;
      if(sum<=bs) cnt++;
      else break;
    }
  *a1 = cnt;
  
  sum = (NumAnt)*((NumAnt)+1)/2 - ((NumAnt)-(*a1))*((NumAnt)-(*a1)+1)/2; 
  
  *a2 = bs - sum + (*a1);
}

/* Return baseline index from a pair of antenna numbers - upper triangle storage */
casacore::uInt RFATimeFreqCrop :: BaselineIndex(casacore::uInt /*row*/, casacore::uInt a1, casacore::uInt a2)
{
  return ( (NumAnt)*((NumAnt)+1)/2 - ((NumAnt)-a1)*((NumAnt)-a1+1)/2 + (a2 - a1) );
}



/* Display a 2D data set on DS9 in gray scale */
void RFATimeFreqCrop :: Display_ds9(casacore::Int xdim, casacore::Int ydim, casacore::Matrix<casacore::Float> &data, casacore::Int frame)
{
  
  FILE *SAOout = NULL;
  char tmp[100];
  char server[100] = "ds9";	
  int bitpix = -32;
  char xpa[100] = "";
  
  strcpy (xpa, "xpaset");
  
  sprintf (tmp, "%s %s \"array [xdim=%d,ydim=%d,bitpix=%d]\"\n",
	   xpa, server, xdim, ydim, bitpix);
  SAOout = (FILE *) popen (tmp, "w");
  
  if (frame > 0)
    {
      sprintf (tmp, "echo \"frame %d \" | %s %s \n", frame, xpa, server);
      system (tmp);
    }
  
  if (SAOout != NULL)
    {
      //      for (i = 0; i < ydim; i++)
      //	fwrite (data[i], sizeof (float) * xdim, 1, SAOout);
      casacore::Bool deleteit=false;
      casacore::Float *dataptr = data.getStorage(deleteit);
      fwrite(dataptr, sizeof(casacore::Float)*xdim*ydim, 1, SAOout);
      pclose (SAOout);
      //if(deleteit) data.freeStorage(dataptr,true);
    }
  else
    {
      perror ("Error in opening SAO - ds9 \n");
    }
  
  if (frame > 0)
    {
      system ("xpaset -p ds9 zoom to fit");
    }
  
  //cout << " Press enter to continue... " << endl;
  //getchar();
  
  
}

/* Display a line plot in DS9 !!! */
void RFATimeFreqCrop :: Plot_ds9(casacore::Int dim, casacore::Vector<casacore::Float> data1, casacore::Vector<casacore::Float> data2)
{
  
  //  FILE *SAOout = NULL;
  //char tmp[100];
  //char server[100] = "ds9";	
  //int bitpix = -32;
  int i;
  char xpa[100] = "";
  
  //static casacore::Bool firstentry=true;
  strcpy (xpa, "xpaset");
  
  casacore::String cmd("");
  
  {
    cmd = "xpaset -p ds9 plot flagger clear \n";
    system(cmd.data());
    //        SAOout = (FILE *) popen (tmp, "w");
  }
  
  
  cmd = "echo '";
  for(i=0;i<dim;i++)
    {
      cmd +=  casacore::String::toString(i) + " " + casacore::String::toString(data2[i]) + " " ;
    }
  cmd += "\n' | xpaset ds9 plot flagger data xy\n";
  cmd += "xpaset -p ds9 plot flagger color linear blue\n";
  cmd += "xpaset -p ds9 plot flagger line linear width 2.0\n";
  system(cmd.data());
  
  cmd = "echo '";
  for(i=0;i<dim;i++)
    {
      cmd +=  casacore::String::toString(i) + " " + casacore::String::toString(data1[i]) + " " ;
    }
  cmd += "\n' | xpaset ds9 plot flagger data xy\n";
  cmd += "xpaset -p ds9 plot flagger color linear red\n";
  cmd += "xpaset -p ds9 plot flagger line linear width 2.0\n";
  system(cmd.data());
  
}// end of plot_ds9


} //# NAMESPACE CASA - END

