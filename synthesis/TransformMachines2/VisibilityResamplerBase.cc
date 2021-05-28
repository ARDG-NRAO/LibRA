// -*- C++ -*-
//# VisibilityResamplerBase.cc: Implementation of the VisibilityResamplerBase class
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

#include <synthesis/TransformMachines2/VisibilityResamplerBase.h>
#include <synthesis/TransformMachines2/PolOuterProduct.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <msvis/MSVis/AsynchronousTools.h>
#include <casa/Quanta/MVTime.h>
#include <fstream>

using namespace casacore;
namespace casa{
  using namespace refim;
  //
  //-----------------------------------------------------------------------------------
  //
  void VisibilityResamplerBase::copy(const VisibilityResamplerBase& other)
  {
    SynthesisUtils::SETVEC(uvwScale_p, other.uvwScale_p);
    SynthesisUtils::SETVEC(offset_p, other.offset_p);
    SynthesisUtils::SETVEC(dphase_p, other.dphase_p);
    SynthesisUtils::SETVEC(chanMap_p, other.chanMap_p);
    SynthesisUtils::SETVEC(polMap_p, other.polMap_p);
    SynthesisUtils::SETVEC(spwChanFreq_p, other.spwChanFreq_p);
    SynthesisUtils::SETVEC(spwChanConjFreq_p, other.spwChanConjFreq_p);
    SynthesisUtils::SETVEC(cfMap_p, other.cfMap_p);
    SynthesisUtils::SETVEC(conjCFMap_p, other.conjCFMap_p);
    //    vbRow2CFMap_p.assign(other.vbRow2CFMap_p);
    convFuncStore_p = other.convFuncStore_p;
    paTolerance_p = other.paTolerance_p;
    vb2CFBMap_p = other.vb2CFBMap_p;

  }
  
  void VisibilityResamplerBase::setVB2CFMap(const casacore::CountedPtr<refim::VB2CFBMap>& thisMap) 
  {
    vb2CFBMap_p = thisMap;
    // uInt n=vb2CFBMap_p->vbRow2BLMap_p.size();
    // for(uInt i=0;i<n;i++)
    //   cerr << i << " " << vb2CFBMap_p->vbRow2BLMap_p[i]<< " " << thisMap->vbRow2BLMap_p[i] << endl;
    // return;
  }
  //
  //-----------------------------------------------------------------------------------
  //
  VisibilityResamplerBase& VisibilityResamplerBase::operator=(const VisibilityResamplerBase& other)
  {
    copy(other);
    return *this;
  }
  //
  //-----------------------------------------------------------------------------------
  //
  // template void VisibilityResamplerBase::addTo4DArray(DComplex* store,const Int* iPos, Complex& val, Double& wt) __restrict__;
  // template void VisibilityResamplerBase::addTo4DArray(Complex* store,const Int* iPos, Complex& val, Double& wt) __restrict__;
  //
  //-----------------------------------------------------------------------------------
  //
  void VisibilityResamplerBase::sgrid(Int& uvwDim,
				      Double* __restrict__ pos, 
				      Int* __restrict__ loc, 
				      Int* __restrict__ off, 
				      Complex& phasor, const Int& irow,
				      // const Matrix<Double>& __restrict__ uvw, 
				      const Double* __restrict__ uvw, 
				      const Double&  dphase, 
				      const Double&  freq, 
				      const Double* __restrict__ scale, 
				      const Double* __restrict__ offset,
				      const Float* __restrict__ sampling) // __restrict__ 
				  // const Vector<Double>& __restrict__ scale, 
				  // const Vector<Double>& __restrict__ offset,
				  // const Vector<Float>& __restrict__ sampling) __restrict__ 
  {
    Double phase;
    Int ndim=2;

    for(Int idim=0;idim<ndim;idim++)
      {
	pos[idim]=scale[idim]*uvw[idim+irow*uvwDim]*freq/C::c+offset[idim];
	loc[idim]=(Int)std::floor(pos[idim]+0.5);
	off[idim]=(Int)std::floor(((loc[idim]-pos[idim])*sampling[idim])+0.5);
      }

    if (dphase != 0.0)
      {
	phase=-2.0*C::pi*dphase*freq/C::c;
	phasor=Complex(cos(phase), sin(phase));
      }
    else
      phasor=Complex(1.0);
    //cerr << "### " << pos[0] << " " << offset[0] << " " << off[0] << endl;
  }

  //
  //-----------------------------------------------------------------------------------
  //
  // Int VisibilityResamplerBase::makeVBRow2CFBMap(CFStore2& cfs,
  // 					       ConvolutionFunction& cf,
  // 					       const VisBuffer2& vbs, 
  // 					       const Quantity& dPA,
  // 					       const Vector<Int>& /*dataChan2ImChanMap*/,
  // 					       const Vector<Int>& /*dataPol2ImPolMap*/,
  // 					       const Vector<Double>& pointingOffset)
  // {
  //   //    VBRow2CFMapType& vbRow2CFMap_p,
  //   const Int nRow=vbs.nRows(); 
  //   //UNUSED: nChan=dataChan2ImChanMap.nelements(), 
  //   //UNUSED: nPol=dataPol2ImPolMap.nelements();
  //   //    vbRow2CFMap_p.resize(nPol, nChan, nRow);
  //   vbRow2CFBMap_p.resize(nRow);
  //   Quantity pa(getPA(vbs),"rad");
  //   PolOuterProduct outerProduct;
  //   Int statusCode=CFDefs::MEMCACHE;
  //   for (Int irow=0;irow<nRow;irow++)
  //     {
  // 	//
  // 	// Translate antenna ID to antenna type
  // 	//
  // 	Int ant1Type = cf.mapAntIDToAntType(vbs.antenna1()(irow)),
  // 	  ant2Type = cf.mapAntIDToAntType(vbs.antenna2()(irow));
  // 	//
  // 	// Get the CFBuffer for the given PA and baseline catagorized
  // 	// by the two antenna types.  For homgeneous arrays, all
  // 	// baselines will map to a single antenna-type pair.
  // 	//

  // 	CountedPtr<CFBuffer> cfb_l;
  // 	try
  // 	  {
  // 	   cfb_l = cfs.getCFBuffer(pa, dPA, ant1Type, ant2Type);
  // 	   //cfb_l->show("From VRB: ");
  // 	  }
  // 	catch (CFNotCached& x)
  // 	  {
  // 	    LogIO log_l(LogOrigin("VisibilityResamplerBase2", "makeVBRow2CFMap"));
  // 	    log_l << "CFs not cached for " << pa.getValue("deg") 
  // 		  << " deg, dPA = " << dPA.getValue("deg") 
  // 		  << " Field ID = " << vbs.fieldId()(0);
  // 	      //		  << " TimeStamps(0-10) = " << vbs.feedPa1(getCurrentTimeStamp(vbs)).nelements() << " ";
  // 	      //		  << " TimeStamps(0-10) = " << vbs.feedPa1().nelements() << " ";
  // 	    // for (Int i=0;i<10;i++) 
  // 	    //   {
  // 	    // 	//		log_l << MVTime(vbs.time()(i)).string(MVTime::TIME) << " ";
  // 	    // 	log_l << vbs.time()(i)/1.0e8 << " ";
  // 	    // 	log_l << "(" << (vbs.feed_pa(getCurrentTimeStamp(vbs))(i))*57.2956 << ") ";
  // 	    //   }
  // 	    log_l << " Ant1Type, Ant2Type = " << ant1Type << "," << ant2Type << LogIO::POST;
  // 	    statusCode=CFDefs::NOTCACHED;
  // 	  }

  // 	if (statusCode==CFDefs::NOTCACHED)
  // 	  {
  // 	    break;
  // 	  }
  // 	else
  // 	  {
  // 	    cfb_l->setPointingOffset(pointingOffset);
  // 	    vbRow2CFBMap_p(irow) = cfb_l;
  // 	  }

  // 	/*
  // 	//
  // 	// Now do the in-row mappings.
  // 	// 
  // 	// Convert the VB polarizations to MuelllerElements.  
  // 	for (Int ichan=0;ichan<nChan;ichan++)
  // 	  {
  // 	    //	    Double freq = vb.freq_p(ichan), wVal=vbs.uvw_p(irow,2);
  // 	    Double freq = vbs.frequency()(ichan), wVal=abs(vbs.uvw()(irow)(2));
  // 	    wVal *= freq/C::c;
  // 	    for (Int ipol=0;ipol<nPol;ipol++)
  // 	      {
  // 		Vector<Int> vbPol = vbs.corrType();
  // 		if (dataPol2ImPolMap(ipol) >= 0)
  // 		  {
  // 		    // The translate global functions comes from
  // 		    // PolOuterProduct.{cc,h}.
  // 		    //
  // 		    // The code below translates, e.g.,
  // 		    // Stokes::RR-->PolCrossProduct::RR-->MuellerElement.
  // 		    MuellerElementType muellerElement;// = outerProduct.getMuellerElement(translateStokesToCrossPol(vbPol(ipol)));
  // 		    Bool found=false;
  // 		    Double f,w;
  // 		    f=cfb_l->nearestFreq(found,freq);
  // 		    w=cfb_l->nearestWVal(found,wVal);
  // 		    if (!found) log_l << "Nearest freq. or w value not found " 
  // 				      << freq << " " << wVal << LogIO::EXCEPTION;

  // 		    vbRow2CFMap_p(ipol,ichan,irow) = cfb_l->getCFCellPtr(f, w, muellerElement);

  // 		    // Bool Dummy;
  // 		    // if (irow == 1)
  // 		    //   {
  // 		    // 	cerr << "#### " << ipol << ", " << ichan << ", " << irow << " " 
  // 		    // 	     << vbRow2CFMap_p(ipol,ichan,irow)->getStorage()->getStorage(Dummy) << endl;
  // 		    //   }
  // 		  }
  // 	      }
  // 	  }
  // 	*/
  //     }
  //   return statusCode;
  // }

using namespace casacore;
};// end namespace casa
