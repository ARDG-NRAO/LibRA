// -*- C++ -*-
//# PolOuterProduct.cc: Implementation of the PolOuterProduct class
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
#include <synthesis/TransformMachines/PolOuterProduct.h>

using namespace casacore;
namespace casa{
  //---------------------------------------------------------------------
  //
  // Map indices to the vis. pols. used.  This remains fixed for a
  // given imaging run.
  //
  // cfIndices_p are filled by this method and is determined by the
  // Stokes-I setting for a given imaging run.  Since this also
  // depends on the Pol. mapping in the MS, this mapping is determined
  // once (but has to be determined programmatically for each MS and
  // can't be a hard-coded convention here).
  //
  void PolOuterProduct::initCFMaps(const Vector<Int>& vbPol, const Vector<Int>& visPol2ImMap)
  {
    Int np=0;
    //muellerType_p=HYBRID;
    for (uInt i=0;i<visPol2ImMap.nelements(); i++) 
      {
	Int n;
	if (visPol2ImMap(i) >= 0) 
	  {
	    n=translateStokesToGeneric(vbPol(i));
	    cfIndices_p.resize(n+1,true);
	    if (muellerType_p == DIAGONAL)	  cfIndices_p(n).resize(1);
	    else if (muellerType_p == FULL)	  cfIndices_p(n).resize(4);
	
	    for(uInt j=0;j<cfIndices_p(n).nelements();j++) cfIndices_p(n)(j)=np++;
		
	  }
      }	  
    //	if (visPol2ImMap(i) >= 0)
	{
		if(muellerType_p == HYBRID)
		{
		  //Int n;
		  //n=translateStokesToGeneric(vbPol(i));
		  //cerr<<"N = :"<<n<<endl;
		  cfIndices_p.resize(4);
		  cfIndices_p(0).resize(3);
		  cfIndices_p(1).resize(3);
		  cfIndices_p(2).resize(3);
		  cfIndices_p(3).resize(3);
		  //		  cfIndices_p(0)(0) = 0;
		  //		  cfIndices_p(0)(1) = 1;
		  //		  cfIndices_p(0)(2) = 2;
		  //		  cfIndices_p(1)(0) = 4;
		  //		  cfIndices_p(1)(1) = 5;
		  //		  cfIndices_p(1)(2) = 7;
		  //		  cfIndices_p(2)(0) = 8;
		  //		  cfIndices_p(2)(1) = 10;
		  //		  cfIndices_p(2)(2) = 11;
		  //		  cfIndices_p(3)(0) = 13;
		  //		  cfIndices_p(3)(1) = 14;
		  //		  cfIndices_p(3)(2) = 15;

		  cfIndices_p(0)(0) = 0;
		  cfIndices_p(0)(1) = 1;
		  cfIndices_p(0)(2) = 2;
		  cfIndices_p(1)(0) = 3;
		  cfIndices_p(1)(1) = 4;
		  cfIndices_p(1)(2) = 5;
		  cfIndices_p(2)(0) = 6;
		  cfIndices_p(2)(1) = 7;
		  cfIndices_p(2)(2) = 8;
		  cfIndices_p(3)(0) = 9;
		  cfIndices_p(3)(1) = 10;
		  cfIndices_p(3)(2) = 11;
		}

	}
	//cout<<"cfIndices_p in PolOuterProduct::initCFMaps"<<cfIndices_p;

  }
  //---------------------------------------------------------------------
  //
  // Make a map between the vbPol indices and indices to get the CFs
  // to be used.  This can be dynamic, depending on the meaning of the
  // pol. axis of the visCube in the VB (which can change per VB).
  //
  PolMapType& PolOuterProduct::makePol2CFMat_p(const Vector<Int>& vbPol, 
					       const Vector<Int>& vbPol2ImMap,
					       PolMapType& outerProdNdx2VBPolMap)
  {
    Int nVBPol=vbPol.nelements();
    outerProdNdx2VBPolMap.resize(nVBPol);
    for (Int i=0;i<nVBPol; i++)
      if (vbPol2ImMap(i) >= 0)
	{
	  Int n = translateStokesToGeneric(vbPol(i));
	  outerProdNdx2VBPolMap(i).assign(cfIndices_p(n));
	  //cout<<"cfIndices_p"<<cfIndices_p<<endl;
	  if (muellerType_p == HYBRID)
            {
                outerProdNdx2VBPolMap(i).resize(3);
                Int n=translateStokesToGeneric(vbPol(i));
                if (n==GPP)
                {
                        outerProdNdx2VBPolMap(0)(0)=0; //cfIndices_p(0)(0)=0;
                        outerProdNdx2VBPolMap(0)(1)=1; //cfIndices_p(0)(1)=1;
                        outerProdNdx2VBPolMap(0)(2)=2; //cfIndices_p(0)(2)=2;
                        //cout<<"\n outerProdNdx2VBPolMap: "<< outerProdNdx2VBPolMap(0);
                        //cout<<"\n cfIndices_p: "<< cfIndices_p(0)<< "\n";
                }
                else if (n==GPQ)
                {
                        outerProdNdx2VBPolMap(1)(0)=4; //cfIndices_p(1)(0)=4; 
                        outerProdNdx2VBPolMap(1)(1)=5; //cfIndices_p(1)(1)=5; 
                        outerProdNdx2VBPolMap(1)(2)=7; //cfIndices_p(1)(2)=7;

                        //cout<<"\n outerProdNdx2VBPolMap: "<< outerProdNdx2VBPolMap(1);
                        //cout<<"\n cfIndices_p: "<< cfIndices_p<< "\n";
                }
                else if (n==GQP)
                {
                        outerProdNdx2VBPolMap(2)(0)=8; //cfIndices_p(2)(0)=8;
                        outerProdNdx2VBPolMap(2)(1)=10; //cfIndices_p(2)(1)=10;
                        outerProdNdx2VBPolMap(2)(2)=11; //cfIndices_p(2)(2)=11;
                        //cout<<"\n outerProdNdx2VBPolMap: "<< outerProdNdx2VBPolMap(2);
                        //cout<<"\n cfIndices_p: "<< cfIndices_p(2)<< "\n";
                }
                else if (n==GQQ)
                {
                        outerProdNdx2VBPolMap(3)(0)=13; //cfIndices_p(3)(0)=13; 
                        outerProdNdx2VBPolMap(3)(1)=14; //cfIndices_p(3)(1)=14;
                        outerProdNdx2VBPolMap(3)(2)=15; //cfIndices_p(3)(2)=15;
                        //cout<<"\n outerProdNdx2VBPolMap: "<< outerProdNdx2VBPolMap(3);
                        //cout<<"\n cfIndices_p: "<< cfIndices_p(3)<< "\n";
                }
            }
	}
    return outerProdNdx2VBPolMap;
  }

  PolMapType& PolOuterProduct::makePol2CFMat(const Vector<Int>& vbPol, 
					     const Vector<Int>& vbPol2ImMap)
  {return makePol2CFMat_p(vbPol,vbPol2ImMap, outerProductIndex2VBPolMap_p);}

  PolMapType& PolOuterProduct::makeConjPol2CFMat(const Vector<Int>& vbPol, 
						 const Vector<Int>& /*vbPol2ImMap*/)
  {
    Int nVBPol=vbPol.nelements();
    // Resize the conj. index map.
    conjOuterProductIndex2VBPolMap_p.resize(nVBPol);
    for (Int i=0;i<nVBPol;i++)
      conjOuterProductIndex2VBPolMap_p(i).assign(outerProductIndex2VBPolMap_p(i));
    //
    // For each conjOuterProduct entry, find the equivalent entry in outerProductMap.
    // Copy of the associated index from outerProductIndex map to conjOuterProductMap
    //
    for (uInt i=0;i<conjOuterProduct2VBPolMap_p.nelements();i++)
      for (uInt j=0;j<conjOuterProduct2VBPolMap_p(i).nelements();j++)
	{
	  Int el=conjOuterProduct2VBPolMap_p(i)(j);

	  for (uInt ii=0;ii<outerProduct2VBPolMap_p.nelements();ii++)
	    for (uInt jj=0;jj<outerProduct2VBPolMap_p(ii).nelements();jj++)
	      if (outerProduct2VBPolMap_p(ii)(jj) == el)
		{
		  conjOuterProductIndex2VBPolMap_p(i)(j) = outerProductIndex2VBPolMap_p(ii)(jj); 
		  break;
		}
	}
    return conjOuterProductIndex2VBPolMap_p;
    //    return makePol2CFMat_p(vbPol,vbPol2ImMap, conjOuterProductIndex2VBPolMap_p);
  }

  //---------------------------------------------------------------------
  //
  // Map the CF indices to physical outer product enums.
  //
  // The muellerRows_p mapping is an internal convention and is hard coded in protected method makeMap().
  //
  PolMapType& PolOuterProduct::makePolMat_p(const Vector<Int>& vbPol, 
					    const Vector<Int>& vbPol2ImMap,
					    PolMapType& outerProd2VBPolMap,
					    RigidVector<RigidVector<Int, 4>, 4>& mRows)
  {
    Int nVBPol=vbPol.nelements();
    
    outerProd2VBPolMap.resize(nVBPol);
    for (Int i=0; i<nVBPol; i++)
      {
	if (vbPol2ImMap(i) >= 0)
	  {
	    if (muellerType_p == DIAGONAL)
	      {
		outerProd2VBPolMap(i).resize(1);
		Int n=translateStokesToGeneric(vbPol(i));
		if (n==GPP) outerProd2VBPolMap(i)(0)=mRows(n)(0);
		if (n==GPQ) outerProd2VBPolMap(i)(0)=mRows(n)(1);
		if (n==GQP) outerProd2VBPolMap(i)(0)=mRows(n)(2);
		if (n==GQQ) outerProd2VBPolMap(i)(0)=mRows(n)(3);
	      }
	    else if (muellerType_p == FULL)
	      {
		outerProd2VBPolMap(i).resize(4);
		Int n=translateStokesToGeneric(vbPol(i));
		outerProd2VBPolMap(i).assign(mRows(n).vector());
	//	cout<<"\n outerProd2VBPolMap: "<< outerProd2VBPolMap(i);
	//	cout<<"\n mRows: "<< mRows(n)<< "\n";
	      }
// The Hybrid option ensures that we are omitting the anti-diagonal that is posing some normalization issue
// We do this by ensuring that the outerProd2VbPolMap is resized to 3 and set with the appropriate index.
	    else if (muellerType_p == HYBRID)
	    {
	    	outerProd2VBPolMap(i).resize(3);
                Int n=translateStokesToGeneric(vbPol(i));
		if (n==GPP)
		{
			outerProd2VBPolMap(0)(0)=mRows(0)(0);
			outerProd2VBPolMap(0)(1)=mRows(0)(1);
			outerProd2VBPolMap(0)(2)=mRows(0)(2);
			//cout<<"\n outerProd2VBPolMap: "<< outerProd2VBPolMap(0);
			//cout<<"\n mRows: "<< mRows(0)<< "\n";
		}
		else if (n==GPQ)
		{
			outerProd2VBPolMap(1)(0)=mRows(1)(0);
                        outerProd2VBPolMap(1)(1)=mRows(1)(1);
                        outerProd2VBPolMap(1)(2)=mRows(1)(3);
			//cout<<"\n outerProd2VBPolMap: "<< outerProd2VBPolMap(1);
			//cout<<"\n mRows: "<< mRows(1)<< "\n";
		}
		else if (n==GQP)
		{
			outerProd2VBPolMap(2)(0)=mRows(2)(0);
                        outerProd2VBPolMap(2)(1)=mRows(2)(2);
                        outerProd2VBPolMap(2)(2)=mRows(2)(3);
			//cout<<"\n outerProd2VBPolMap: "<< outerProd2VBPolMap(2);
			//cout<<"\n mRows: "<< mRows(2)<< "\n";
		}
		else if (n==GQQ)
		{
			outerProd2VBPolMap(3)(0)=mRows(3)(1);
                        outerProd2VBPolMap(3)(1)=mRows(3)(2);
                        outerProd2VBPolMap(3)(2)=mRows(3)(3);
			//cout<<"\n outerProd2VBPolMap: "<< outerProd2VBPolMap(3);
			//cout<<"\n mRows: "<< mRows(3)<< "\n";
		}
	    }
	
	  }
      }
    return outerProd2VBPolMap;
  }
  PolMapType& PolOuterProduct::makePolMat(const Vector<Int>& vbPol, 
					  const Vector<Int>& vbPol2ImMap)
  {return makePolMat_p(vbPol,vbPol2ImMap, outerProduct2VBPolMap_p, muellerRows_p);}
  PolMapType& PolOuterProduct::makeConjPolMat(const Vector<Int>& vbPol, 
					  const Vector<Int>& vbPol2ImMap)
  {return makePolMat_p(vbPol,vbPol2ImMap, conjOuterProduct2VBPolMap_p, conjMuellerRows_p);}
  //
  //---------------------------------------------------------------------
  //
  void PolOuterProduct::makePolMap(const Vector<CrossPolCircular>& pols)
  {
    for (uInt i=0;i<pols.nelements(); i++)
      setElement(pols(i),i);
  }
  //---------------------------------------------------------------------
  //
  // ToDo:Make hardcoded maps for this
  PolOuterProduct::GenericVBPol translateStokesToGeneric(const Int& vbPol)
  {
    if ((vbPol==Stokes::RR) || (vbPol==Stokes::XX)) return PolOuterProduct::GPP;
    if ((vbPol==Stokes::RL) || (vbPol==Stokes::XY)) return PolOuterProduct::GPQ;
    if ((vbPol==Stokes::LR) || (vbPol==Stokes::YX)) return PolOuterProduct::GQP;
    if ((vbPol==Stokes::LL) || (vbPol==Stokes::YY)) return PolOuterProduct::GQQ;
    return PolOuterProduct::JUSTWRONGVBPOL;
  }
  //
  //---------------------------------------------------------------------
  // ToDo: Make hardcoded maps for this
  Int translateGenericToStokes(const PolOuterProduct::GenericVBPol& gPol,
			       const MSIter::PolFrame& polFrame)
  {
    if (polFrame==MSIter::Circular)
      {
	if (gPol==PolOuterProduct::GPP) return Stokes::RR;
	if (gPol==PolOuterProduct::GPQ) return Stokes::RL;
	if (gPol==PolOuterProduct::GQP) return Stokes::LR;
	if (gPol==PolOuterProduct::GQQ) return Stokes::LL;
      }
    else if (polFrame==MSIter::Linear)
      {
	if (gPol==PolOuterProduct::GPP) return Stokes::XX;
	if (gPol==PolOuterProduct::GPQ) return Stokes::XY;
	if (gPol==PolOuterProduct::GQP) return Stokes::YX;
	if (gPol==PolOuterProduct::GQQ) return Stokes::YY;
      }
    return PolOuterProduct::JUSTWRONGVBPOL;
  }
  
};
