//# FlagAgentTimeFreqCrop.cc: This file contains the implementation of the FlagAgentTimeFreqCrop class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#include <flagging/Flagging/FlagAgentTimeFreqCrop.h>

// Polynomial-fitting classes
#include <scimath/Functionals/Polynomial.h>
#include <scimath/Fitting.h>
#include <scimath/Fitting/LinearFit.h>
#include <scimath/Fitting/GenericL2Fit.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// TODO : Get rid of this macro ?
#define MIN(a,b) ((a)<=(b) ? (a) : (b))

FlagAgentTimeFreqCrop::FlagAgentTimeFreqCrop(FlagDataHandler *dh, Record config, Bool writePrivateFlagCube, Bool flag):
		FlagAgentBase(dh,config,ANTENNA_PAIRS,writePrivateFlagCube,flag)
{
	setAgentParameters(config);

	// Request loading polarization map to FlagDataHandler
	flagDataHandler_p->setMapPolarizations(true);
}

FlagAgentTimeFreqCrop::~FlagAgentTimeFreqCrop()
{
	// Compiler automagically calls FlagAgentBase::~FlagAgentBase()
}

void FlagAgentTimeFreqCrop::setAgentParameters(Record config)
{
        logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
	int exists;

	exists = config.fieldNumber ("timecutoff");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpDouble && config.type(exists) != TpFloat  && config.type(exists) != TpInt )
	        {
			 throw( AipsError ( "Parameter 'timecutoff' must be of type 'double'" ) );
	        }
		
		time_cutoff_p = config.asDouble("timecutoff");
	}
	else
	{
		time_cutoff_p = 4.0;
	}

	*logger_p << logLevel_p << " timecutoff is " << time_cutoff_p << LogIO::POST;

	exists = config.fieldNumber ("freqcutoff");
	if (exists >= 0)
	{
	        if( config.type(exists) !=  TpDouble && config.type(exists) != TpFloat   && config.type(exists) != TpInt )
	        {
			 throw( AipsError ( "Parameter 'freqcutoff' must be of type 'double'" ) );
	        }
		
		freq_cutoff_p = config.asDouble("freqcutoff");
	}
	else
	{
		freq_cutoff_p = 3.0;
	}

	*logger_p << logLevel_p << " freqcutoff is " << freq_cutoff_p << LogIO::POST;

	exists = config.fieldNumber ("maxnpieces");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpInt )
	        {
			 throw( AipsError ( "Parameter 'maxnpieces' must be of type 'int'" ) );
	        }
		
		maxNPieces_p = config.asInt("maxnpieces");

		if ((maxNPieces_p<1) or (maxNPieces_p>9))
		{
		  throw ( AipsError (" Unsupported maxnpieces : " + String::toString(maxNPieces_p) + ". Supported values: 1-9") );
		}
	}
	else
	{
		maxNPieces_p = 7;
	}

	*logger_p << logLevel_p << " maxnpieces is " << maxNPieces_p << LogIO::POST;

	exists = config.fieldNumber ("timefit");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpString )
	        {
			 throw( AipsError ( "Parameter 'timefit' must be of type 'string'" ) );
	        }
		
		timeFitType_p = config.asString("timefit");
		if ((timeFitType_p.compare("line") != 0) and (timeFitType_p.compare("poly") != 0))
		{
		         throw ( AipsError ("Unsupported timefit: " + timeFitType_p +". Supported modes: line,poly" ) );
		}

	}
	else
	{
		timeFitType_p = "line";
	}

	*logger_p << logLevel_p << " timefit is " << timeFitType_p << LogIO::POST;

	exists = config.fieldNumber ("freqfit");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpString )
	        {
			 throw( AipsError ( "Parameter 'timefit' must be of type 'string'" ) );
	        }
		
		freqFitType_p = config.asString("freqfit");

		if ((freqFitType_p.compare("line") != 0) and (freqFitType_p.compare("poly") != 0))
		{
		         throw ( AipsError ("Unsupported freqfit: " + freqFitType_p +". Supported modes: line,poly" ) );
		}
	}
	else
	{
		freqFitType_p = "poly";
	}

	*logger_p << logLevel_p << " freqfit is " << freqFitType_p << LogIO::POST;

	exists = config.fieldNumber ("flagdimension");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpString )
	        {
			 throw( AipsError ( "Parameter 'flagdimension' must be of type 'string'" ) );
	        }
		
		flagDimension_p = config.asString("flagdimension");

		if ((flagDimension_p.compare("time") != 0) and (flagDimension_p.compare("freq") != 0)
				and (flagDimension_p.compare("timefreq") != 0) and (flagDimension_p.compare("freqtime") != 0))
		{
		         throw ( AipsError ("Unsupported flagdimension: " + flagDimension_p +". Supported modes: time,freq,timefreq,freqtime" ) );
		}
	}
	else
	{
		flagDimension_p = "freqtime";
	}

	*logger_p << logLevel_p << " flagdimension is " << flagDimension_p << LogIO::POST;

	exists = config.fieldNumber ("halfwin");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpInt )
	        {
			 throw( AipsError ( "Parameter 'halfwin' must be of type 'int'" ) );
	        }
		
		halfWin_p = config.asInt("halfwin");

		if ((halfWin_p < 1) or (halfWin_p > 3))
		{
		         throw ( AipsError ("Unsupported halfwin: " + String::toString(halfWin_p) +". Supported values : 1,2,3" ) );
		}
	}
	else
	{
		halfWin_p = 1;
	}

	*logger_p << logLevel_p << " halfwin is " << halfWin_p << LogIO::POST;

	exists = config.fieldNumber ("usewindowstats");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpString )
	        {
			 throw( AipsError ( "Parameter 'usewindowstats' must be of type 'string'" ) );
	        }
		
		winStats_p = config.asString("usewindowstats");

		if ((winStats_p.compare("none") != 0) and (winStats_p.compare("sum") != 0)
				and (winStats_p.compare("std") != 0) and (winStats_p.compare("both") != 0))
		{
		         throw ( AipsError ("Unsupported usewindowstats: " + winStats_p +". Supported modes : none,sum,std,both") );
		}
	}
	else
	{
		winStats_p = "none";
	}

	*logger_p << logLevel_p << " usewindowstats is " << winStats_p << LogIO::POST;


	return;
}

bool
FlagAgentTimeFreqCrop::computeAntennaPairFlags(const vi::VisBuffer2 & /*visBuffer*/,
                                               VisMapper &visibilities,
                                               FlagMapper &flags,
                                               Int /*antenna1*/,
                                               Int /*antenna2*/,
                                               vector<uInt> & /*rows*/)
{
	// Call 'fltBaseAndFlag' as specified by the user.
	if(flagDimension_p == String("time"))
	  {
	    fitBaseAndFlag(timeFitType_p,String("time"),visibilities,flags);
	  }
	else if( flagDimension_p == String("freq") )
	  {
	    fitBaseAndFlag(freqFitType_p,String("freq"),visibilities,flags);
	  }
	else if( flagDimension_p == String("timefreq") )
	  {
	    fitBaseAndFlag(timeFitType_p,String("time"),visibilities,flags);
	    fitBaseAndFlag(freqFitType_p,String("freq"),visibilities,flags);
	  }
	else // freqtime (default)
	  {
	    fitBaseAndFlag(freqFitType_p,String("freq"),visibilities,flags);
	    fitBaseAndFlag(timeFitType_p,String("time"),visibilities,flags);
	  }

	return false;
}

//----------------------------------------------------------------------------------------------------------

// fitBaseAndFlag
// Average the data along the axis not-specified by 'direction' to generate a vector along 'direction'.
// Fit a piece-wise polynomial of type 'fittype' along the axis specified by 'direction'
// Divide the unaveraged data by this fit, and iteratively flag outliers - use flags.applyFlag()
void FlagAgentTimeFreqCrop :: fitBaseAndFlag(String fittype, String direction, VisMapper &visibilities,FlagMapper &flags)
{    
  // Get shapes
  IPosition flagCubeShape = visibilities.shape();
  uInt nChannels = flagCubeShape(0);
  uInt nTimes = flagCubeShape(1);

  // Work variables
  Vector<Float> avgDat,avgFit;
  Vector<Bool> avgFlag;
  Vector<Int> mind(2);
  Float tol=4.0,mn=1.0,sd=0,tpsd=0.0,tpsum=0.0, mval=0.0;
  Int mcnt=0;
  
  // Decide which direction to fit the base polynomial
  if(direction==String("freq"))
    { 
      mind[0]=nChannels;  mind[1]=nTimes;
      tol=freq_cutoff_p;
    }
  else if(direction==String("time") )
    { 
      mind[0]=nTimes;  mind[1]=nChannels;
      tol=time_cutoff_p;
    }
  else
    {
      throw AipsError("Internal Error. Unrecognized axis direction for tfcrop : " + direction);
    }
  
  // ALLOC : Resize temp arrays to either nChannel or nTime
  avgDat.resize(mind[0]);   avgDat=0.0;   
  avgFit.resize(mind[0]);    avgFit=0.0;                
  avgFlag.resize(mind[0]);  avgFlag=false;

  // A way to tell if anything is non-zero in this piece.
  Bool allzeros=true; 
  
  // STEP 1 : 
  // For each element in the dominant direction (axis0)
  // calculate the mean across the other direction (axis1).
  // The output average data and flags will later be sent for poly fitting...
  for(int i0=0;i0<mind[0];i0++)	
    {
      // Calc the mean across axes1 (with flags)
      mval=0.0;mcnt=0;
      for(uInt i1=0;i1<(uInt) mind[1];i1++)
	{
	  if(mind[0]==(Int) nChannels)// if i0 is channel, and i1 is time
	    {
	      if( ! ( flags.getModifiedFlags(i0,i1) ) ) //C// && usePreFlags_p ) )
		{
		  mval += visibilities(i0,i1);
		  mcnt++;
		}
	    }
	  else // if i1 is channel, and i0 is time
	    {
	      if( ! ( flags.getModifiedFlags(i1,i0) ) ) //C// && usePreFlags_p ) )
		{
		  mval += visibilities(i1,i0);
		  mcnt++;
		}
	    }
	}//for i1

      // Fill in the mean value across axis1 for i0
      avgDat[i0] = mcnt ? mval/mcnt : 0.0;
      // Fill in whether all visibilities are zero or not
      avgFlag[i0] = mcnt ? false : true;
      if(! avgFlag[i0] ) allzeros=false;

    }// for i0


  // STEP 2 : 
  // If there are any non-zero unflagged values in the average, 
  //     fit polynomials and flag. Otherwise, return.  
  if(allzeros==false)
    {
      // Fit a piece-wise polynomial across axis0 (or do a line)
      // This fit is done to the average computed in the previous step.
      if(fittype == String("poly"))	  // Piecewise Poly Fit
	{
	  fitPiecewisePoly(avgDat,avgFlag,avgFit,maxNPieces_p,4);	
	}
      else  // Line Fit 
	{
	  fitPiecewisePoly(avgDat,avgFlag,avgFit,1,1);	
	}
      
      // STEP 3 : 
      // Now, iterate through the data again and flag.
      // This time, iterate in reverse order : for each i1, get all i0
      // This is because 'avgFit' is of size mind[0], 
      //     and window stats are to be computed along i0
      for(uInt i1=0;i1<(uInt) mind[1];i1++)
	{
	  // STEP 3A : Divide out the clean bandpass 
	  avgDat=0,avgFlag=false; 
	  for(int i0=0;i0<mind[0];i0++)
	    {
	      if(mind[0]==(Int) nChannels)// if i0 is channel, and i1 is time
		{
		  avgFlag[i0] = flags.getModifiedFlags(i0,i1); //C// && usePreFlags_p;
		  if(avgFlag[i0]==false) avgDat[i0] = visibilities(i0,i1)/avgFit(i0);
		}
	      else // if i1 is channel, i0 is time
		{
		  avgFlag[i0] = flags.getModifiedFlags(i1,i0); //C// && usePreFlags_p;
		  if(avgFlag[i0]==false) avgDat[i0] = visibilities(i1,i0)/avgFit(i0);
		}
	    }//for i0

	  // STEP 3B
	  // Flag outliers based on absolute deviation from the model
	  // Do this as a robust fit
	  Float temp=0;
	  for(Int loop=0;loop<5;loop++)
	    {
	      // Calculate the standard-deviation of the normalized data w.r.to the mean
	      sd = calcStd(avgDat,avgFlag,mn);
	      
	      // Flag if the data differs from mn=1 by N sd
	      for(Int i0=0;i0<mind[0];i0++)
		{
		  if(avgFlag[i0]==false && fabs(avgDat[i0]-mn) > tol*sd) avgFlag[i0]=true ;
		}
	      
	      // Stop iterating if the deviation of the normalized data from the mean is less than 10%
	      if(fabs(temp-sd) < (Double)0.1)break;
	      // else go on for 5 iterations
	      temp=sd;
	    }//for loop
	  
	  // STEP 3C :
	  // Additional flagging based on sliding-window statistics
	  // Note : this step looks only at vis values, not existing flags.
	  if(halfWin_p>0 && (winStats_p != "none") )
	    {
	      // Start and end the sliding windows halfWin from the ends.
	      for(Int i0=halfWin_p;i0<mind[0]-halfWin_p;i0++)
		{
		  tpsum=0.0;tpsd=0.0;
		  
		  // Flag point i0  if average of N points around i0 crosses N sd
		  if(winStats_p=="sum" || winStats_p=="both")
		    {
		      for(Int i=i0-halfWin_p; i<i0+halfWin_p+1; i++) tpsum += fabs(avgDat[i]-mn);
		      if(tpsum/(2*halfWin_p+1.0) > tol*sd ) avgFlag[i0]=true;
		    }
		  
		  // Flag point i0 if the N point std around i0 is larger then N sd
		  if(winStats_p=="std" || winStats_p=="both")
		    {
		      for(Int i=i0-halfWin_p; i<i0+halfWin_p+1; i++) tpsd += (avgDat[i]-mn) * (avgDat[i]-mn) ;
		      if(sqrt( tpsd / (2*halfWin_p+1.0) ) > tol*sd)  avgFlag[i0]=true ;
		    }
		  
		}//for i0
	    }// if winStats != none


	  // STEP 3D :
	  // Fill the flags into the FlagMapper 
	  // Note : To minimize copies, we can call flags.applyFlag() directly from STEPS 3B and 3C,
	  //           in addition to filling in avgFlag for STEP 3B.  
	  //       However, the following ensures minimal calls to flags.applyFlag().
	  for(Int i0=0;i0<mind[0];i0++)
	    {
	      if(mind[0]==(Int) nChannels) // if i0 is channel, and i1 is time
		{
		  if(avgFlag[i0])
		  {
			  flags.applyFlag(i0,i1);
			  visBufferFlags_p += 1;
		  }
		}
	      else //if i1 is channel, and i0 is time
		{
		  if(avgFlag[i0])
		  {
			  flags.applyFlag(i1,i0);
			  visBufferFlags_p += 1;
		  }
		}
	    }// for i0
	  
	}//for i1
      
    }// if allzeros==false


  return;
  
}// end fitBaseAndFlag



/* Calculate the MEAN of 'vect' ignoring values flagged in 'flag' */
Float FlagAgentTimeFreqCrop :: calcMean(Vector<Float> &vect, Vector<Bool> &flag)
{
  Float mean=0;
  Int cnt=0;
  for(uInt i=0;i<vect.nelements();i++)
    if(flag[i]==false)
      {
	mean += vect[i];
	cnt++;
      }
  if(cnt==0) cnt=1;
  return mean/cnt;
}


/* Calculate the variance of 'vect' w.r.to a given 'fit' 
 * ignoring values flagged in 'flag' 
 * Use  median ( data - median(data) )   as the estimator of variance
 */
Float FlagAgentTimeFreqCrop :: calcVar(Vector<Float> &vect, Vector<Bool> &flag, Vector<Float> &fit)
{
  //// Float var=0;
  uInt n=0,cnt=0;
  n = vect.nelements() < fit.nelements() ? vect.nelements() : fit.nelements();
  for(uInt i=0;i<n;i++)
    {
      if(flag[i]==false)cnt++;
    }
  
  Vector<Float> validvals(cnt);
  cnt=0;
  for(uInt i=0;i<n;i++)
    {
      if(flag[i]==false)
	{
	  validvals[cnt] = fit[i] < (Double)1e-6 ? (Double)0.0 : fabs( (vect[i] - fit[i])/fit[i] );
	  cnt++;
	}
    }
  
  Float med=0.0;
  
  if(validvals.nelements())
    {
      med = median(validvals,false);
      
      //cout << "validvals : " << validvals << endl;
      //cout << "median : " << med << endl;
      
      for(uInt i=0;i<validvals.nelements();i++)
	validvals[i] = fabs( validvals[i]-med );
      
      med = median(validvals,false);
      //cout << "median(data-median(data)) : " << med << endl;
    }
  
  return med;
}



/* Calculate the STANDARD DEVN. of 'vect' w.r.to a given 'fit' 
 * ignoring values flagged in 'flag' */
Float FlagAgentTimeFreqCrop :: calcStd(Vector<Float> &vect, Vector<Bool> &flag, Vector<Float> &fit)
{
  Float std=0;
  uInt n=0,cnt=0;
  n = vect.nelements() < fit.nelements() ? vect.nelements() : fit.nelements();
  for(uInt i=0;i<n;i++)
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
Float FlagAgentTimeFreqCrop :: calcStd(Vector<Float> &vect, Vector<Bool> &flag, Float mean)
{
  Float std=0;
  uInt cnt=0;
  for(uInt i=0;i<vect.nelements();i++)
    if(flag[i]==false)
      {
	cnt++;
	std += (vect[i]-mean)*(vect[i]-mean);
      }
  return sqrt(std/cnt);
}

/* Fit Piecewise polynomials to 'data' and get the 'fit' */
void FlagAgentTimeFreqCrop :: fitPiecewisePoly(Vector<Float> &data, Vector<Bool> &flag, Vector<Float> &fit, uInt maxnpieces, uInt maxdeg)
{
  Int deg=0;//,start=0;
  Int left=0,right=0;
  Float sd,TOL=3;
  Vector<Float> tdata;
  
  // ALLOC : Another temp array to hold modified data
  tdata.resize(data.nelements());
  tdata = data;
  
  AlwaysAssert(data.nelements()==flag.nelements(), AipsError);    
  
  /* replace empty data values by adjacent values */
  for(uInt i=0;i<tdata.nelements();i++)
    {
      if(tdata[i]==0)
	{
	  if(i==0)// find first non-zero value and set to that.
	    {
	      Int ind=0;
	      for(uInt j=1;j<tdata.nelements();j++)
		if(tdata[j]!=0){ind=j;break;}
	      if(ind==0) tdata[i]=0;
	      else tdata[i]=tdata[ind];
	    }
	  else// find next non-zero value and interpolate.
	    {
	      Int indr=0;
	      for(uInt j=i+1;j<tdata.nelements();j++)
		if(tdata[j]!=0){indr=j;break;}
	      Int indl=-1;
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
  for(uInt i=0;i<tdata.nelements();i++)
    if(tdata[i]==0) 
      {
	flag[i]=true;
      }
  
  fit = tdata;
  
  Int psize=1;
  Int leftover=1,leftover_front=0,npieces=1;
  
  deg=1;
  npieces=1;
  
  for(uInt j=0;j<5;j++)
    {
      npieces = MIN(2*j+1, maxnpieces);
      if(j>1) {deg=2;}
      if(j>2) {deg=3;}
      deg = MIN(deg,(Int) maxdeg);
      
      psize = (int)(tdata.nelements()/npieces);
      //     cout << "Iter : " << j << " with Deg : " << deg << " and Piece-size : " << psize << endl;
      
      leftover = (int)(tdata.nelements() % npieces);
      
      leftover_front = (int)(leftover/2.0);
      
      left=0; right=tdata.nelements()-1;
      for(Int p=0;p<npieces;p++)
	{
	  if(npieces>1)
	    {
	      left = leftover_front + p*psize;
	      right = left + psize; 
	      
	      if(p==0) {left = 0; right = leftover_front + psize;}
	      if(p==npieces-1) {right = tdata.nelements()-1;} 
	    }
	  if(deg==1) 
	    lineFit(tdata,flag,fit,left,right);
	  else 
	    //lineFit(tdata,flag,fit,left,right);
	    polyFit(tdata,flag,fit,left,right,deg);
	}
      
      /* Now, smooth the fit - make this nicer later */
      
      int winstart=0,winend=0;
      float winsum=0.0;
      int offset=2;
      for(uInt i=offset;i<tdata.nelements()-offset;i++)
	{
	  winstart = i-offset;
	  winend = i+offset;
	  if(winstart<0)winstart=0;
	  if(winend>=(int) tdata.nelements())winend=tdata.nelements()-1;
	  if(winend <= winstart) break;
	  winsum=0.0;
	  for(uInt wi=winstart;wi<=(uInt) winend;++wi)
	    winsum += fit[wi];
	  fit[i] = winsum/(winend-winstart+1);
	}
      
      
      /* Calculate the STD of the fit */
      sd = calcStd(tdata,flag,fit);
      if(j>=2)  TOL=2;
      else TOL=3;
      
      
      /* Detect outliers */
      for(uInt i=0;i<tdata.nelements();i++)
	{
	  if(tdata[i]-fit[i] > TOL*sd) 
	    flag[i]=true;
	}
      
    } // for j
  
} // end of fitPiecewisePoly



  /* Fit a polynomial to 'data' from lim1 to lim2, of given degree 'deg', 
   * taking care of flags in 'flag', and returning the fitted values in 'fit' */
void FlagAgentTimeFreqCrop :: polyFit(Vector<Float> &data,Vector<Bool> &flag, Vector<Float> &fit, uInt lim1, uInt lim2,uInt deg)
{
  Vector<Double> x;
  Vector<Double> y;
  Vector<Double> sig;
  Vector<Double> solution;
  
  uInt cnt=0;
  for(uInt i=lim1;i<=lim2;i++)
    if(flag[i]==false) cnt++;
  
  if(cnt <= deg)
    {
      lineFit(data,flag,fit,lim1,lim2);
      return;
    }
  
  
  LinearFit<Double> fitter;
  Polynomial<Double> combination(deg);
  
  combination.setCoefficient(0,0.0);
  if (deg >= 1) combination.setCoefficient(1, 0.0);
  if (deg >= 2) combination.setCoefficient(2, 0.0);
  if (deg >= 3) combination.setCoefficient(3, 0.0);
  if (deg >= 4) combination.setCoefficient(4, 0.0);
  
  // ALLOC : Resize to the length of each 'piece' in the piecewise fit.
  x.resize(lim2-lim1+1);
  y.resize(lim2-lim1+1);
  sig.resize(lim2-lim1+1);
  solution.resize(deg+1);
  
  for(uInt i=lim1;i<=lim2;i++)
    {
      x[i-lim1] = i+1;
      y[i-lim1] = data[i];
      sig[i-lim1] = (flag[i]==true)?0:1;
    }
  
  fitter.asWeight(true);
  fitter.setFunction(combination);
  solution = fitter.fit(x,y,sig);
  
  for(uInt i=lim1;i<=lim2;i++)
    {
      fit[i]=0;
      for(uInt j=0;j<deg+1;j++)
	fit[i] += solution[j]*pow((double)(x[i-lim1]),(double)j);
    }
  
}



/* Fit a LINE to 'data' from lim1 to lim2, taking care of flags in 
 * 'flag', and returning the fitted values in 'fit' */
void FlagAgentTimeFreqCrop :: lineFit(Vector<Float> &data, Vector<Bool> &flag, Vector<Float> &fit, uInt lim1, uInt lim2)
{
  float Sx = 0, Sy = 0, Sxx = 0, Sxy = 0, S = 0, a, b, sd, mn;
  
  mn = calcMean(data, flag);
  sd = calcStd (data, flag, mn);
  
  for (uInt i = lim1; i <= lim2; i++)
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
  
  for (uInt i = lim1; i <= lim2; i++)
    fit[i] = a + b * i;
  
}



} //# NAMESPACE CASA - END


