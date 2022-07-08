// -*- C++ -*-
//# BaselineType.cc: Implementation of the PhaseGrad class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
//


/* 

The intent of BaselineType Object is two fold

1. Based on the type of the telescope and antenna define an enumerated baseline Type
2. This is also the location where PhaseGrad for the Pointing Table for a given BlType
calculated.

The first point is still an action time to finish. This will allow for CF production
for a truly heterogenous array, such as ALMA.
The second however is what the rest of the code here is accomplishing. We compute
the number of antenna groups that have a clustered pointing value based on nsigma
binning criterion from the user. We then parse the pointing Table and determine the
variance of the antennnas after subtracting the mean pointing direction. We then bin
the antennas based on their deviation from the mean into one of the bins. This defines
our antenna groups. We will then utilize nG + nG C 2 number of phase gradients during
imaging with doPointing=True


 */

#include <synthesis/TransformMachines2/BaselineType.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/scimath/Mathematics/Combinatorics.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/BasicSL/Constants.h>
using namespace casacore;
namespace casa{
  using namespace refim;
  BaselineType::~BaselineType() 
  {
    vectorPhaseGradCalculator_p.resize(0);
  };
  
  // -----------------------------------------------------------------

  BaselineType::BaselineType(): vbRows_p(), doPointing_p(true), cachedGroups_p(false), vectorPhaseGradCalculator_p(), totalGroups_p(0), antennaGroups_p(), cachedAntennaGroups_p() , vbRow2BLMap_p(), cachedPointingOffsets_p(), cachedAntPointingOffsets_p(), antPair_p(), binsx_p(), binsy_p()
  {
    newPhaseGradComputed_p = false;
    vectorPhaseGradCalculator_p.resize(0);
    mapBLGroup_p.resize(0,0);
    binsx_p = 0;
    binsy_p = 0;
  };

  BaselineType& BaselineType::operator=(const BaselineType& other)
  {
    if(this!=&other)
      {
	vectorPhaseGradCalculator_p = other.vectorPhaseGradCalculator_p;

      }
    return *this;
  
  };

  vector< vector<double> > BaselineType::pairSort(const vector<int>& antArray, const vector<vector<double> >& poArray) 
  { 
    int n = antArray.size();
    pair<int, vector<double> > pairt[antArray.size()]; 
  
    // Storing the respective array 
    // elements in pairs. 
    for (int i = 0; i < n; i++)  
      { 
        pairt[i].first = antArray[i]; 
        pairt[i].second = {poArray[0][i],poArray[1][i]}; 
      } 
  
    // Sorting the pair array. 
    sort(pairt, pairt + n); 

    vector <vector<double> > tempPOArray;
    tempPOArray.resize(poArray.size());
    tempPOArray[0].resize(poArray[0].size());
    tempPOArray[1].resize(poArray[1].size());
    vector<double> tempvec;
    for (int i = 0; i < n; i++)  
      {
	tempvec = pairt[i].second;
	tempPOArray[0][i] = tempvec[0];
	tempPOArray[1][i] = tempvec[1];	
      }

    return tempPOArray;
    
  } 

  int BaselineType::returnIdx(const vector<int>& inpArray, const int& searchVal)
  {

    // cerr << "============================================" << endl;
    // for(unsigned int idx=0;idx < inpArray.size(); idx++)
    // 	cerr << "inpArray[idx] " << inpArray[idx] << "   " << searchVal << endl;
    // cerr << "============================================" << endl;
    
    auto itrBLMap_p = find(inpArray.begin(),inpArray.end(), searchVal);
    int idx = distance(inpArray.begin(), itrBLMap_p);
    return idx;
	  

    // for(unsigned int idx=0;idx < inpArray.size(); idx++)
    //   {
    // 	if(inpArray[idx] == searchVal)
    // 	  {
    // 	    return idx;
    // 	  }
    // 	else
    // 	  {
    // 	    return -1;
    // 	  }
    //   }
  }

  Vector<Vector<double> > BaselineType::stl2DVecToCasa2DVec (const vector<vector<double> >& stlVec)
  {
    int xrange = stlVec.size();
    int yrange = stlVec[0].size();
    Vector < Vector<double> > casVec;
    casVec.resize(xrange,0);
    for (int ii = 0; ii<xrange; ii++)
      {
	casVec(ii).resize(yrange,0);
	for (int jj=0; jj<yrange; jj++)
	  casVec(ii)(jj) = stlVec[ii][jj];
      }

    return casVec;
  }


  Matrix<vector<int> > BaselineType::findAntennaGroups(const vi::VisBuffer2& vb, 
						      const CountedPtr<PointingOffsets>& pointingOffsets_p, 
						      const double& sigmaDev)
    {
      if (doPointing_p==false) return antennaGroups_p;
      else if(!cachedGroups_p)
	{
	  totalGroups_p = 0;
	  // const Int nRows = vb.nRows();
	  cachedPointingOffsets_p.assign(pointingOffsets_p->pullPointingOffsets());
	  vector< vector <double> > antDirPix_l = pointingOffsets_p->fetchAntOffsetToPix(vb, doPointing_p);
	  cachedAntPointingOffsets_p.reference(stl2DVecToCasa2DVec(antDirPix_l));
	  MVDirection vbdir=vb.direction1()(0).getValue();	
	  vector<double> phaseDirPix_l = (pointingOffsets_p->toPix(vb,vbdir,vbdir)).tovector();

	  // Step 1 the problem scale is smaller than what we want to solve.
	  // so we find the unique antennas per VB for the join of antenna1 and antenna2.
	  
	  vector<int> uniqueAnt1, uniqueAnt2;
	  vector<int>::iterator ant1Itr, ant2Itr;
	  uniqueAnt1 = (vb.antenna1()).tovector();
	  uniqueAnt2 = (vb.antenna2()).tovector();

	  uniqueAnt1.insert(uniqueAnt1.begin(),uniqueAnt2.begin(),uniqueAnt2.end());
	  sort(uniqueAnt1.begin(),uniqueAnt1.end());
	  ant1Itr = unique(uniqueAnt1.begin(),uniqueAnt1.end());
	  uniqueAnt1.resize(distance(uniqueAnt1.begin(),ant1Itr));
	  
	  

	  vector< vector <double> > poAnt1(2), poAnt2(2);
	  for(unsigned int ii=0; ii<poAnt1.size();ii++)
	    {
	      poAnt1[ii].resize(poAnt1[ii].size(),0);
	      poAnt2[ii].resize(poAnt2[ii].size(),0);
	    }

	  poAnt1[0] = antDirPix_l[0];
	  poAnt1[1] = antDirPix_l[1];
	  // poAnt2[0] = antDirPix_l[2];
	  // poAnt2[1] = antDirPix_l[3];



	  vector<double> pixSum(2,0), pixMean(2,0), pixVar(2,0), pixSigma(2,0), minPO(2,0), maxPO(2,0), minAntPO(2,0);
	  vector<vector<double> > diffAntPointing_l(2), antGrdPointing_l(2), minDiffAntPointing_l(2);
	  Vector<Vector<double> > casa_antGrdPointing_l(2);

	  diffAntPointing_l[0].resize(poAnt1[0].size());
	  diffAntPointing_l[1].resize(poAnt1[1].size());
	  antGrdPointing_l[0].resize(poAnt1[0].size());
	  antGrdPointing_l[1].resize(poAnt1[1].size());
	  casa_antGrdPointing_l[0].resize(poAnt1[0].size());
	  casa_antGrdPointing_l[1].resize(poAnt1[1].size());

	  minAntPO[0] = *min_element(poAnt1[0].begin(), poAnt1[0].end());
	  minAntPO[1] = *min_element(poAnt1[1].begin(), poAnt1[1].end());

	  
	  // cerr << "#################################"<<endl;
	  for (unsigned int i=0; i<uniqueAnt1.size();i++)
	    {
	  //     // diffAntPointing_l[0][i] = poAnt1[0][i] - phaseDirPix_l[0];
	  //     // diffAntPointing_l[1][i] = poAnt1[1][i] - phaseDirPix_l[1];
	      diffAntPointing_l[0][i] = poAnt1[0][i] - minAntPO[0];
	      diffAntPointing_l[1][i] = poAnt1[1][i] - minAntPO[1];

	      // cerr << "Ant "<< uniqueAnt1[i] << " PO: "<< poAnt1[0][i] - phaseDirPix_l[0]  << " " << poAnt1[1][i] - phaseDirPix_l[1] <<endl;
	    }
	  // cerr << "#################################"<<endl;
	  

	  for(unsigned int ii=0; ii < pixSum.size();ii++)
	    {

	      pixSum[ii]  = accumulate(poAnt1[ii].begin(),poAnt1[ii].end(),0.0);
	      pixMean[ii] = pixSum[ii]/poAnt1[ii].size();

	      for(unsigned int jj=0; jj < poAnt1[ii].size(); jj++)
		{
		  pixVar[ii] += (poAnt1[ii][jj] - pixMean[ii])*(poAnt1[ii][jj] - pixMean[ii]);
		}
	      // for_each (poAnt1[ii].begin(), poAnt1[ii].end(), [&](const double d) {
	      // 	  pixVar[ii] += (d - phaseDirPix_l[0]) * (d - phaseDirPix_l[0]);
	      // 	});
	      //pixMean[ii] = (pixSum[ii] - poAnt1[ii].size()*phaseDirPix_l[ii])/poAnt1[ii].size();
	      pixSigma[ii] = sqrt(pixVar[ii])/poAnt1[ii].size();

	      // maxPO[ii] = *max_element(poAnt1[ii].begin(), poAnt1[ii].end()); 
	      // minPO[ii] = *min_element(poAnt1[ii].begin(), poAnt1[ii].end());
	      maxPO[ii] = *max_element(diffAntPointing_l[ii].begin(), diffAntPointing_l[ii].end());
	      minPO[ii] = *min_element(diffAntPointing_l[ii].begin(), diffAntPointing_l[ii].end());

	    }
	  // cerr << "Maximum Pointing offset was : "<< maxPO[0] << " "<< maxPO[1]<< " " <<ceil ((maxPO[0] - minPO[0])/sigmaDev) << endl;
	  // cerr << "Minimum Pointing offset was : "<< minPO[0] << " "<< minPO[1]<< " " <<ceil ((maxPO[1] - minPO[1])/sigmaDev)<< endl;
	  // The single loop above computes all the statistics we need.

	  // pixSum[1] = accumulate(poAnt1[1].begin(),poAnt1[1].end(),0.0);
	  // pixMean[0] = pixSum[0]/poAnt1[0].size();
	  // pixMean[1] = pixSum[1]/poAnt1[1].size();




	  // double combinedSigma = sqrt(pixSigma[0]*pixSigma[0] + pixSigma[1]*pixSigma[1]);
	  // // if(combinedSigma > sigmaDev)
	    
	  // cerr << "Bin Size in pixels : "<< sigmaDev << endl;

	  binsx_p = ceil((fabs(maxPO[0] - minPO[0])/sigmaDev)+1); 
	  binsy_p = ceil((fabs(maxPO[1] - minPO[1])/sigmaDev)+1);
	  
	  deltaBinsX_p = sigmaDev;
	  deltaBinsX_p = sigmaDev;

	  if (binsx_p%2==0) binsx_p++;
	  if (binsy_p%2==0) binsy_p++;

	  // cerr <<"Binsx : "<<binsx_p <<" Binsy : "<<binsy_p<<endl;
	  antennaGroups_p.resize(binsx_p,binsy_p);
	  Matrix<vector<float> > meanAntGrdPO_l(binsx_p,binsy_p);
	  meanAntGrdPO_l.resize(binsx_p,binsy_p);
	  // cerr << "AntennaGroups_p shape : "<<antennaGroups_p.shape()<<endl;
	  for(int ii=0; ii < binsx_p; ii++)
	    for(int jj=0; jj < binsy_p; jj++)
	      {
		antennaGroups_p(ii,jj).resize(0);
		meanAntGrdPO_l(ii,jj).resize(2,0);
	      }

	  int ii,jj; 
	  // int xOrigin=floor(binsx_p/2.0), yOrigin=floor(binsy_p/2.0);
	  // // cerr <<"xOrigin : "<<xOrigin <<" yOrigin : "<<yOrigin<<endl;
	  // int poGridOriginX = floor (maxPO[0] - minPO[0])/2.0 , poGridOriginY =  floor (maxPO[1] - minPO[1])/2.0 ;

	  // cerr <<"poGridOriginX : "<<poGridOriginX <<" poGridOriginY : "<<poGridOriginY<<endl;
	  for(unsigned int kk=0; kk < poAnt1[0].size(); kk++)
	    {
	      	  // ii = max(0,min((int)(diffAntPointing_l[0][kk]/sigmaDev + xOrigin), binsx_p-1));
		  // jj = max(0,min((int)(diffAntPointing_l[1][kk]/sigmaDev + yOrigin), binsy_p-1));
		  // ii = (int)(diffAntPointing_l[0][kk]/sigmaDev + xOrigin);
		  // jj = (int)(diffAntPointing_l[1][kk]/sigmaDev + yOrigin);
	      
		  ii = floor(diffAntPointing_l[0][kk]/sigmaDev);
		  jj = floor(diffAntPointing_l[1][kk]/sigmaDev);
		  
		  // cerr << ii << " " << jj << " " << diffAntPointing_l[0][kk]/sigmaDev + minAntPO[0]<< " " <<diffAntPointing_l[1][kk]/sigmaDev + minAntPO[1]<< " " << antennaGroups_p(ii,jj).size() << " " <<endl;

		  // cerr << ii << " " << jj << " " << poAnt1[0][kk]/pixSigma[0] << " " <<poAnt1[1][kk]/pixSigma[1] << " " << antennaGroups_p(ii,jj).size() << endl;
		  // cerr << ii << " " << jj<< " " << poAnt1[0][kk] << " "<< poAnt1[0][kk] << endl;
		  antennaGroups_p(ii,jj).push_back(uniqueAnt1[kk]);
		  antGrdPointing_l[0][kk] = minAntPO[0] + (ii-1)*sigmaDev;
		  antGrdPointing_l[1][kk] = minAntPO[1] + (jj-1)*sigmaDev;
		  
		  meanAntGrdPO_l(ii,jj)[0] += poAnt1[0][kk];
		  meanAntGrdPO_l(ii,jj)[1] += poAnt1[1][kk];
		  // casa_antGrdPointing_l(0)(kk) = minAntPO[0] + (ii-1)*sigmaDev;
		  // casa_antGrdPointing_l(1)(kk) = minAntPO[1] + (jj-1)*sigmaDev;
		  
		  // antGrdPointing_l[0][kk] = poGridOriginX + (ii-1)*sigmaDev;
		  // antGrdPointing_l[1][kk] = poGridOriginY + (jj-1)*sigmaDev;

	    }

	  for(int ii=0; ii<binsx_p; ii++)
	    for(int jj=0; jj<binsy_p; jj++)
	      if (antennaGroups_p(ii,jj).size() > 0)
		{
		  meanAntGrdPO_l(ii,jj)[0] = meanAntGrdPO_l(ii,jj)[0]/antennaGroups_p(ii,jj).size();
		  meanAntGrdPO_l(ii,jj)[1] = meanAntGrdPO_l(ii,jj)[1]/antennaGroups_p(ii,jj).size();
		}
	  for(unsigned int kk=0; kk < uniqueAnt1.size(); kk++)
	    for(int ii=0; ii<binsx_p; ii++)
	      for(int jj=0; jj<binsy_p; jj++)
		if(!antennaGroups_p(ii,jj).empty())
		  if(find(antennaGroups_p(ii,jj).begin(),antennaGroups_p(ii,jj).end(),uniqueAnt1[kk]) != antennaGroups_p(ii,jj).end())
		    {
		      casa_antGrdPointing_l(0)(kk) = meanAntGrdPO_l(ii,jj)[0];
		      casa_antGrdPointing_l(1)(kk) = meanAntGrdPO_l(ii,jj)[1];
		      // cerr << "AG: " << ii << " " << jj << " " << antennaGroups_p(ii,jj).size() << " with PO " << meanAntGrdPO_l(ii,jj)[0] << " "<< meanAntGrdPO_l(ii,jj)[1] << endl;
		    }
	  
	  // 	cerr << "AG: " << ii << " " << jj << " " << antennaGroups_p(ii,jj).size() << " with PO " << poGridOriginX + (ii-1)*sigmaDev << " " << poGridOriginY + (jj-1)*sigmaDev << endl;
	  pointingOffsets_p->setAntGridPointingOffsets(casa_antGrdPointing_l);
	  {
	    LogIO log_l(LogOrigin("BaselineType", "findAntennaGroups"));
	    ostringstream ostr;
		    
	    for(int ii=0; ii<binsx_p; ii++)
	      for(int jj=0; jj<binsy_p; jj++)
		if(antennaGroups_p(ii,jj).size() > 0)
		  {
		    totalGroups_p += 1;
		    ostr <<"Antenna in bin " << totalGroups_p << " is : ";
		    for (unsigned int kk=0;kk<antennaGroups_p(ii,jj).size();kk++)
		      ostr <<antennaGroups_p(ii,jj).at(kk)<<" ";
		    ostr << endl;
		  }
	    log_l << ostr.str() << LogIO::POST;
	  }
	  //	  cerr << "total groups " <<totalGroups_p<<endl;
	  cacheAntGroups(antennaGroups_p);
	  return antennaGroups_p;
	}
      else
	{ 
	  // cerr<<"Utilizing cachedAntennaGroups_p"<<endl;
	  return cachedAntennaGroups_p;

	}

    }


  void BaselineType::cacheAntGroups(const Matrix<vector<int> > antennaGroups_p)
  {
    
    const int nx = antennaGroups_p.nrow(), ny = antennaGroups_p.ncolumn();
    cachedAntennaGroups_p.resize(nx,ny);
    cachedAntennaPO_p.resize(nx,ny);
    for (int ii=0; ii < nx; ii++)
      for (int jj=0; jj < ny; jj++)      
	{
	  int vecLen = antennaGroups_p(ii,jj).size();
	  cachedAntennaGroups_p(ii,jj).resize(vecLen);

	  for (int kk=0; kk<vecLen; kk++)
	    {
	      cachedAntennaGroups_p(ii,jj)[kk] = antennaGroups_p(ii,jj)[kk];
	    }
	}
    cachedGroups_p=true;

  }


  void BaselineType::makeVBRow2BLGMap(const vi::VisBuffer2& vb) 
  // const Matrix<vector<int> >& antennaGroups_p)
  {

    /* The goal here is take the antenna groups given a vb and return a baseline group
       map for every VB row. This will then allow for the computation of the phase grad
       once per phase grad per vb. */

    const Int nRows=vb.nRows(); 
    const Int nx = cachedAntennaGroups_p.nrow(), ny = cachedAntennaGroups_p.ncolumn();
    Int numAntGroups=0;
    vector<int> mapAntType1, mapAntType2;
    const Vector<Int> antenna1 = vb.antenna1(), antenna2 = vb.antenna2();

    mapAntGrp_p.resize(nx,ny);
    mapAntType1.resize(nRows);
    mapAntType2.resize(nRows);
    vbRow2BLMap_p.resize(nRows);

    antPair_p.resize(nRows,make_pair(0,0));
    // for (int kk=0; kk<nRows;kk++)
    //   {
    // 	antPair_p[kk].first = mapAntType1[kk];
    // 	antPair_p[kk].second = mapAntType2[kk];
    //   }

    blNeedsNewPOPG_p.resize(nRows);
    vector<int> uniqueBLType_p(nRows,0);
    for(int ii=0; ii<nRows;ii++)
      blNeedsNewPOPG_p[ii]=false;

    for (int ii=0; ii < nx; ii++)
      for (int jj=0; jj < ny; jj++)      
	{
	  mapAntGrp_p(ii,jj)=0;
	  if(cachedAntennaGroups_p(ii,jj).size() > 0)
	    {
	      numAntGroups++;
	      mapAntGrp_p(ii,jj) = numAntGroups;
	    }
	}

    // cout<<"mapAntGrp "<<mapAntGrp_p<<endl;
    if(numAntGroups > 0) // If any cell is populated...
      {
	mapBLGroup_p.resize(numAntGroups,numAntGroups);
	for (int ii=0; ii < numAntGroups; ii++)
	  for (int jj=0; jj < numAntGroups; jj++)
	    mapBLGroup_p(ii,jj) = ii*numAntGroups+jj+1;
      }
    else 
      {
	mapBLGroup_p.resize(1,1);
	mapBLGroup_p(0,0) = 0;
      }

    // cerr<<"mapBLGrp "<<mapBLGroup_p<< endl  <<" numAntGroups " << numAntGroups << " nx,ny " << nx <<" " << ny <<endl;

      
    // LogIO log_l(LogOrigin("VB2CFBMap", "makeVBRow2BLGMap[R&D]"));
    // log_l << "Number of Baseline Groups found " << (numAntGroups + (numAntGroups*(numAntGroups-1))/2) << LogIO::POST;

    for (int kk=0; kk<nRows;kk++)
      {
	for (int ii=0; ii < nx; ii++)
	  for (int jj=0; jj < ny; jj++)      
	    {
	      int n=cachedAntennaGroups_p(ii,jj).size();
	      if( n> 0)
		{
		for(int tt=0; tt < n; tt++)
		  {
		    if(antenna1[kk] == (cachedAntennaGroups_p(ii,jj))[tt])
		      mapAntType1[kk] = mapAntGrp_p(ii,jj);
		    // else
		    // 	mapAntType1[kk] = -1;
			
		    if(antenna2[kk] == (cachedAntennaGroups_p(ii,jj))[tt])
		      mapAntType2[kk] = mapAntGrp_p(ii,jj);
		    // else
		    // 	mapAntType2[kk] = -1;
		  }
		}
	    }   

      }

    // cerr << "Baseline Groups for vb Row are as follows : ";
    // for(int ii = 0; ii < mapAntType1.size(); ii++)
    // 	cerr<<mapAntType1[ii]<<"  "<<mapAntType2[ii]<<" " <<ii <<endl;
    // cerr<<endl<<"#############################################"<<endl;


      
    for (int kk=0; kk<nRows;kk++)
      {
	int ix,iy;
	ix=mapAntType1[kk]-1;
	iy=mapAntType2[kk]-1;

	if ((ix>=0) && (iy>=0)) 
	  {
	    vbRow2BLMap_p[kk] = mapBLGroup_p(ix,iy);
	    antPair_p[kk].first = ix;
	    antPair_p[kk].second = iy;
	  }
	else 
	  vbRow2BLMap_p[kk] = -1;
	// cerr << "kk : " << kk << " " << ix << " " << iy << " " << vbRow2BLMap_p[kk] << " " << mapBLGroup_p(ix,iy) << endl;
      }
      

  }
  


  int BaselineType::numPhaseGrads(const int& nG)
  {
    if(nG<2)
      return nG;
    else
      return nG + Combinatorics::choose(nG,2);


  }

};
