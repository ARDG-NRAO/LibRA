// -*- C++ -*-
//# AWVisResampler.cc: Implementation of the AWVisResampler class
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

#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines2/AWVisResampler.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines/SynthesisMath.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/casa/OS/Timer.h>
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <iomanip>
#include <cfenv>
<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
#include <synthesis/TransformMachines2/FortranizedLoops.h>
=======
//#include <synthesis/TransformMachines2/FortranizedLoops.h>
>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
//#include <synthesis/TransformMachines2/hpg.hpp>

#ifdef _OPENMP
#include <omp.h>
#endif
extern "C" {
  void clLoopsToGrid();
};
//#include <casa/BasicMath/Functors.h>
using namespace casacore;
//using namespace hpg;
namespace casa{
  using namespace refim;
  //
  //-----------------------------------------------------------------------------------
  // Re-sample the griddedData on the VisBuffer (a.k.a gridding)
  //
  // Template instantiations for re-sampling onto a double precision
  // or single precision grid.
  //
  template 
  void AWVisResampler::addTo4DArray(DComplex* __restrict__ & store,
				    const Int* __restrict__ & iPos, 
				    const Vector<Int>& inc, 
				    Complex& nvalue, Complex& wt) __restrict__ ;
  template 
  void AWVisResampler::addTo4DArray(Complex* __restrict__ & store,
				    const Int* __restrict__ & iPos, 
				    const Vector<Int>& inc, 
				    Complex& nvalue, Complex& wt) __restrict__;

  template 
  void AWVisResampler::addTo4DArray_ptr(DComplex* __restrict__ & store,
				    const Int* __restrict__ & iPos, 
				    const Int* __restrict__ & inc, 
				    Complex& nvalue, Complex& wt) __restrict__ ;
  template 
  void AWVisResampler::addTo4DArray_ptr(Complex* __restrict__ & store,
				    const Int* __restrict__ & iPos, 
				    const Int* __restrict__ & inc, 
				    Complex& nvalue, Complex& wt) __restrict__ ;

  template
  void AWVisResampler::DataToGridImpl_p(Array<DComplex>& grid, VBStore& vbs, 
  					Matrix<Double>& sumwt,const Bool& dopsf,
  					Bool useConjFreqCF); // __restrict__;
  template
  void AWVisResampler::DataToGridImpl_p(Array<Complex>& grid, VBStore& vbs, 
					Matrix<Double>& sumwt,const Bool& dopsf,
					Bool useConjFreqCF); // __restrict__;

  Complex* AWVisResampler::getConvFunc_p(const double& vbPA, Vector<Int>& cfShape,
					 Vector<int>& support,
					 int& muellerElement,
					 CountedPtr<CFBuffer>& cfb,
					 Double& wVal, Int& fndx, Int& wndx,
					 PolMapType& mNdx, PolMapType& conjMNdx,
					 Int& ipol, uInt& mRow)
  {
    Bool Dummy;
    Array<Complex> *convFuncV;
    CFCell *cfcell;
    //
    // Since we conjugate the CF depending on the sign of the w-value,
    // pick the appropriate element of the Mueller Matrix (see note on
    // this for details). Without reading the note/understanding,
    // fiddle with this logic at your own risk (can easily lead to a
    // lot of grief. --Sanjay).
    //
    //timer_p.mark();

    int pndx;
    if (wVal > 0.0) 
      {
	pndx=mNdx[ipol][mRow];
	// cfcell=&(*(cfb->getCFCellPtr(fndx,wndx,mNdx[ipol][mRow])));
        // CFCell& cfO=cfb(fndx,wndx,mNdx[ipol][mRow]);
	// convFuncV = &(*cfO.getStorage());
	// support(0)=support(1)=cfO.xSupport_p;
      }
    else
      {
	pndx=conjMNdx[ipol][mRow];
	// cfcell=&(*(cfb->getCFCellPtr(fndx,wndx,conjMNdx[ipol][mRow])));
	// CFCell& cfO=cfb(fndx,wndx,conjMNdx[ipol][mRow]);
	// convFuncV = &(*cfO.getStorage());
	// support(0)=support(1)=cfO.xSupport_p;
      }
    //pndx=1;
    //    cerr << "CFC indexes(w,f,p),ipol: " << wndx << " " << fndx << " " << pndx << " " << ipol << endl;
    cfcell=&(*(cfb->getCFCellPtr(fndx,wndx,pndx)));

<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
=======
    //cerr << "CF Name: " << cfcell->fileName_p << endl;

>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
    convFuncV = &(*cfcell->getStorage());
    support(0)=support(1)=cfcell->xSupport_p;

    // Get the pointer to the CFCell storage (a single CF)
    //    if ((convFuncV = &(*cfcell->getStorage())) == NULL)
    if (convFuncV == NULL)
      throw(SynthesisFTMachineError("cfcell->getStorage() == null"));

    // Load the CF if it not already loaded.  If a new CF is loaded,
    // check if it needs to be rotated.
    if (convFuncV->shape().product() == 0)
      {
	Array<Complex>  tt=SynthesisUtils::getCFPixels(cfb->getCFCacheDir(), cfcell->fileName_p);
	cfcell->setStorage(tt);

	//cerr << (cfcell->isRotationallySymmetric_p?"o":"+");

	// No rotation necessary if the CF is rotationally symmetric
	if (!(cfcell->isRotationallySymmetric_p))
	  {
	    CFCell *baseCFC=NULL;
	    // Rotate only if the difference between CF PA and VB PA
	    // is greater than paTolerance.
	    SynthesisUtils::rotate2(vbPA, *baseCFC, *cfcell, paTolerance_p);
	  }
	convFuncV = &(*cfcell->getStorage());
      }

    //cfShape.reference(cfcell->cfShape_p);
     cfShape.assign(convFuncV->shape().asVector());

     // Always extract the Mueller element value from mNdx.  mNdx
     // carries the direct mapping between Mueller Matrix and
     // Visibility vector.
     //     muellerElement=cfb->getCFCellPtr(fndx,wndx,mNdx[ipol][mRow])->muellerElement_p;
     muellerElement=cfcell->muellerElement_p;
    
    //    cfShape.assign(cfcell->cfShape_p);
     //runTimeG1_p += timer_p.real();


    return convFuncV->getStorage(Dummy);
  };

  //
  //-----------------------------------------------------------------------------------
  //
  void AWVisResampler::cachePhaseGrad_p(const Vector<Double>& pointingOffset,
					const Vector<Int>&cfShape,
					const Vector<Int>& convOrigin,
					const Double& /*cfRefFreq*/,
                                        const Double& /*imRefFreq*/,
					const Int& spwID, const Int& fieldId)
  {
    if (
    	((fabs(pointingOffset[0]-cached_PointingOffset_p[0])) > 1e-6) ||
    	((fabs(pointingOffset[1]-cached_PointingOffset_p[1])) > 1e-6) ||
    	(cached_phaseGrad_p.shape()[0] < cfShape[0])              ||
    	(cached_phaseGrad_p.shape()[1] < cfShape[1])
    	)
      {
	LogIO log_l(LogOrigin("AWVisResampler","cachePhaseGrad[R&D]"));
	log_l << "Computing phase gradiant for pointing offset " 
	      << pointingOffset << cfShape << " " << cached_phaseGrad_p.shape() 
	      << "(SPW: " << spwID << " Field: " << fieldId << ")"
	      << LogIO::DEBUGGING
	      << LogIO::POST;
	Int nx=cfShape(0), ny=cfShape(1);
	Double grad;
	Complex phx,phy;

	cached_phaseGrad_p.resize(nx,ny);
	cached_PointingOffset_p = pointingOffset;
	
	for(Int ix=0;ix<nx;ix++)
	  {
	    grad = (ix-convOrigin[0])*pointingOffset[0];
	    Double sx,cx;
	    SINCOS(grad,sx,cx);

	    phx = Complex(cx,sx);
	    for(Int iy=0;iy<ny;iy++)
	      {
		grad = (iy-convOrigin[1])*pointingOffset[1];
		Double sy,cy;
		SINCOS(grad,sy,cy);
		phy = Complex(cy,sy);
		cached_phaseGrad_p(ix,iy)=phx*phy;
	      }
	  }
      }
  }

  //
  //-----------------------------------------------------------------------------------
  // Template implementation for DataToGrid
  //
  template <class T>
  void AWVisResampler::DataToGridImpl_p(Array<T>& grid,  VBStore& vbs, 
					Matrix<Double>& sumwt,const Bool& dopsf,
					Bool /*useConjFreqCF*/)
  {
    Int nDataChan, nDataPol, nGridPol, nGridChan, nx, ny, nw;//, nCFFreq;
    Int targetIMChan, targetIMPol, rbeg, rend;//, PolnPlane, ConjPlane;
    Int startChan, endChan;
    

    Vector<Float> sampling(2),scaledSampling(2);
    Vector<Int> support(2),loc(3), iloc(4),scaledSupport(2);
    Vector<Double> pos(3), off(3);
    Vector<Int> igrdpos(4);

    Complex phasor, nvalue /*, wt */;
    DComplex norm;
    Vector<Int> cfShape;
    // cfShape=(*vb2CFBMap_p)[0]->getStorage()(0,0,0)->getStorage()->shape().asVector();

    Vector<Int> convOrigin;// = (cfShape)/2;
    Double cfRefFreq;
    const Matrix<Double> UVW=vbs.uvw_p;

    //    Double cfScale=1.0;

    //timer_p.mark();
    rbeg = 0;       rend = vbs.nRow_p;
    rbeg = vbs.beginRow_p;
    rend = vbs.endRow_p;
    
    nx = grid.shape()[0]; ny = grid.shape()[1]; 
    nGridPol = grid.shape()[2]; nGridChan = grid.shape()[3];

    // const std::array<unsigned, 4> grid_size{(uInt)nx, (uInt)ny, (uInt)nGridPol, (uInt)nGridChan};
    // const std::array<unsigned, 4> cf_size{31, 21, 2, 3};
    // const std::array<float, 2> grid_scale{0.1, -0.1};
    // auto st0 = hpg::Gridder(hpg::Device::Cuda, 2, grid_size, grid_scale);


    nDataPol  = vbs.flagCube_p.shape()[0];
    nDataChan = vbs.flagCube_p.shape()[1];

    Bool Dummy, gDummy, 
      accumCFs=((UVW.nelements() == 0) && dopsf);

    T* __restrict__ gridStore = grid.getStorage(gDummy);
      
    Double *freq=vbs.freq_p.getStorage(Dummy);

    cacheAxisIncrements(grid.shape().asVector(), gridInc_p);

    Bool * __restrict__ flagCube_ptr=vbs.flagCube_p.getStorage(Dummy);
    Bool * __restrict__ rowFlag_ptr = vbs.rowFlag_p.getStorage(Dummy);;
    Float * __restrict__ imgWts_ptr = vbs.imagingWeight_p.getStorage(Dummy);
    Complex * __restrict__ visCube_ptr = vbs.visCube_p.getStorage(Dummy);

    Vector<Double> wVals, fVals; PolMapType mVals, mNdx, conjMVals, conjMNdx;
    Double fIncr, wIncr;
    CountedPtr<CFBuffer> cfb = (*vb2CFBMap_p)[0];
    bool finitePointingOffsets=cfb->finitePointingOffsets();

    cfb->getCoordList(fVals,wVals,mNdx, mVals, conjMNdx, conjMVals, fIncr, wIncr);

    nw = wVals.nelements();
    iloc = 0;

    IPosition shp=vbs.flagCube_p.shape();
   if (accumCFs)
     {
	startChan = vbs.startChan_p;
	endChan = vbs.endChan_p;
     }
    else 
      {
	startChan = 0;
	endChan = nDataChan;
      }

   Int vbSpw = (vbs.vb_p)->spectralWindows()(0);
   Double vbPA = vbs.paQuant_p.getValue("rad");
<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
   //Int vbFieldID = -1;//((const Int)((vbs.vb_p)->fieldId()(0)));
   
   // TEMP. DELETE THIS
   // rbeg=9;
   // rend=10; // For a single-vis test
   //startChan=32; endChan=33;
   //   cerr << "SPW, ChanMap: " << vbSpw << " " << chanMap_p << " " << rbeg << " " << rend << " " << startChan << " " << endChan << endl;

   // cerr << "polMap: " << polMap_p << endl;
   // cerr << "mNdx: " << endl;
   // for (int kk=0;kk<mNdx.size();kk++)
   //   {
   //     for(int ll=0;ll<mNdx[kk].size();ll++)
   // 	 cerr << mNdx[kk][ll] << " ";
   //     cerr << endl;
   //   }
   // cerr << "conjMNdx: " << endl;
   // for (int kk=0;kk<conjMNdx.size();kk++)
   //   {
   //     for(int ll=0;ll<conjMNdx[kk].size();ll++)
   // 	 cerr << conjMNdx[kk][ll] << " ";
   //     cerr << endl;
   //   }
   //   cerr << "---------------------------------------------------------------" << endl;
   for(Int irow=rbeg; irow< rend; irow++){   
      //      if ((UVW.nelements() == 0)) 
=======

   for(Int irow=rbeg; irow< rend; irow++){   
>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
      
      if(!(*(rowFlag_ptr+irow)))
	{   
	  setFieldPhaseGrad((vb2CFBMap_p->vectorPhaseGradCalculator_p[vb2CFBMap_p->vbRow2BLMap_p[irow]])->field_phaseGrad_p);
	  cfb = (*vb2CFBMap_p)[irow];

	  for(Int ichan=startChan; ichan< endChan; ichan++)
	    {
	      if (*(imgWts_ptr + ichan+irow*nDataChan)!=0.0) 
		{  
		  targetIMChan=chanMap_p[ichan];
		  
		  if((targetIMChan>=0) && (targetIMChan<nGridChan)) 
		    {
<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
		      //timer_p.mark();
		      //Double dataWVal = vbs.vb_p->uvw()(2,irow);
		      Double dataWVal = (UVW.nelements() > 0) ? UVW(2,irow) : 0.0;
		      Int wndx = cfb->nearestWNdx(abs(dataWVal)*freq[ichan]/C::c);

		      // Double conjFreq=sqrt(2*conjRefFreq*conjRefFreq - freq[ichan]*freq[ichan]);
		      // Int fndx = cfb.nearestFreqNdx(freq[ichan]),
		      // 	conjFNdx =cfb.nearestFreqNdx(conjFreq);

		      // Int fndx=cfb.nearestFreqNdx(vbSpw,ichan), conjFNdx=cfb.nearestFreqNdx(vbSpw,ichan,true);
		      // Int cfFreqNdx;
		      // if (vbs.conjBeams_p) cfFreqNdx = cfb.nearestFreqNdx(vbSpw,ichan,true);// Get the conj. freq. index
		      // else  cfFreqNdx = cfb.nearestFreqNdx(vbSpw,ichan);
		      Int cfFreqNdx = cfb->nearestFreqNdx(vbSpw,ichan,vbs.conjBeams_p);
=======
>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc

		      Double dataWVal = (UVW.nelements() > 0) ? dataWVal = UVW(2,irow) : 0.0;
		      Int wndx = cfb->nearestWNdx(dataWVal*freq[ichan]/C::c);
		      Int cfFreqNdx = cfb->nearestFreqNdx(vbSpw,ichan,vbs.conjBeams_p);
		      Float s;
		      //
		      //------------------------------------------------------------------------------
		      //
		      // Using the int-index version for Freq, W and Muellerelements
		      //
		      //------------------------------------------------------------------------------
		      //

<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
		      //timer_p.mark();
=======
>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
		      cfb->getParams(cfRefFreq, s, support(0), support(1),cfFreqNdx,wndx,0);

		      sampling(0) = sampling(1) = SynthesisUtils::nint(s);
<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
		      
		      //timer_p.mark();
		      sgrid(pos,loc,off, phasor, irow, UVW, dphase_p[irow], freq[ichan], 
			    uvwScale_p, offset_p, sampling);
		      //runTimeG5_p += timer_p.real();
=======
>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
		      
		      sgrid(pos,loc,off, phasor, irow, UVW, dphase_p[irow], freq[ichan], 
			    uvwScale_p, offset_p, sampling);
			{
			  // Loop over all image-plane polarization planes.

			  for(Int ipol=0; ipol< nDataPol; ipol++) 
			    { 
			      if((!(*(flagCube_ptr + ipol + ichan*nDataPol + irow*nDataPol*nDataChan))))
				{  
				  targetIMPol=polMap_p(ipol);
				  if ((targetIMPol>=0) && (targetIMPol<nGridPol)) 
				    {
				      igrdpos[2]=targetIMPol; igrdpos[3]=targetIMChan;
				      
				      norm = 0.0;
				      // Loop over all relevant elements of the Mueller matrix for the polarization
				      // ipol.
				      Vector<int> conjMRow = conjMNdx[ipol];
<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
				      //cerr << "conjMRow = " << conjMRow << endl;
				      // ipol determines the targetIMPol.  Each targetIMPol gets a row of CFs (mRow).
				      // visVecElements is gridded using the convFuncV and added to the target grid.
=======
>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc

//cfFreqNdx=0;
				      for (uInt mCols=0;mCols<conjMRow.nelements(); mCols++) 
					{
					  int visVecElement=mCols, muellerElement;

					  Complex* convFuncV=NULL;
					  //timer_p.mark();
					  try
					    {
					      convFuncV=getConvFunc_p(vbPA,
								      cfShape, support,muellerElement,
								      cfb, dataWVal, cfFreqNdx,
								      wndx, mNdx, conjMNdx, ipol,  mCols);
					    }
					  catch (SynthesisFTMachineError& x)
					    {
					      LogIO log_l(LogOrigin("AWVisResampler[R&D]","DataToGridImpl_p"));
					      log_l << x.getMesg() << LogIO::EXCEPTION;
					    }
					  // Extract the vis. vector element corresponding to the mCols column of the conjMRow row of the Mueller matrix.

					  visVecElement=(int)(muellerElement%nDataPol);
					  // If the vis. vector element is flagged, don't grid it.
					  if(((*(flagCube_ptr + visVecElement + ichan*nDataPol + irow*nDataPol*nDataChan)))) break;

					  if(dopsf) nvalue=Complex(*(imgWts_ptr + ichan + irow*nDataChan));
					  else      nvalue=Complex(*(imgWts_ptr+ichan+irow*nDataChan))*
					   	      (*(visCube_ptr+visVecElement+ichan*nDataPol+irow*nDataChan*nDataPol)*phasor);

<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
					  // if (ipol==1)
					  // if (UVW.nelements() > 0)
					  //   cerr << "Vis: " << irow << " " << ipol << " "
					  // 	 <<  *(visCube_ptr+visVecElement+ichan*nDataPol+irow*nDataChan*nDataPol) << " "
					  // 	 << *(imgWts_ptr + ichan + irow*nDataChan) <<  " " 
					  // 	 << UVW(0,irow) << " " << UVW(1,irow) << " " << UVW(2,irow) << " "
					  // 	 << wndx << " " << cfFreqNdx << " " << vbSpw << endl;


					  if (!onGrid(nx, ny, nw, loc, support)) break;
					  //runTimeG6_p += timer_p.real();
=======
					  if (!onGrid(nx, ny, nw, loc, support)) break;

>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
					  convOrigin=cfShape/2;
					  
					  cacheAxisIncrements(cfShape, cfInc_p);
					  nVisGridded_p++;
#include <synthesis/TransformMachines2/accumulateToGrid.inc>
					}
				      sumwt(targetIMPol,targetIMChan) += vbs.imagingWeight_p(ichan, irow)*fabs(norm);
				      //		      *(sumWt_ptr+apol+achan*nGridChan)+= *(imgWts_ptr+ichan+irow*nDataChan);
				    }
				}
			      // else
			      // 	{
			      // 	  cerr << "Vis0: " << irow << " " << ipol << " " << UVW(0,irow) << " " << UVW(1,irow) << " " << UVW(2,irow) << endl;
			      // 	}
			    } // End poln-loop
			}
		    }
		}
	    } // End chan-loop
	}
    } // End row-loop
<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
   //   cerr <<  std::setprecision(20) << "WTS: " << sumwt(0,0) << endl;
   // cerr << "sumwt: " << dopsf << " " << sumwt << endl;
    // exit(0);
   //runTimeG_p = timer_p.real() + runTimeG1_p + runTimeG2_p + runTimeG3_p + runTimeG4_p + runTimeG5_p + runTimeG6_p + runTimeG7_p;
=======

>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
    T *tt=(T *)gridStore;
    grid.putStorage(tt,gDummy);
  }
  //
  //-----------------------------------------------------------------------------------
  // Re-sample VisBuffer to a regular grid (griddedData) (a.k.a. de-gridding)
  //
  void AWVisResampler::GridToData(VBStore& vbs, const Array<Complex>& grid)
  {
    Int nDataChan, nDataPol, nGridPol, nGridChan, nx, ny,nw;//, nCFFreq;
    Int achan, apol, rbeg, rend;//, PolnPlane, ConjPlane;
    Vector<Float> sampling(2);//scaledSampling(2);
    Vector<Int> support(2),loc(3), iloc(4);
    Vector<Double> pos(3), off(3);
    
    IPosition grdpos(4);
    
    Vector<Complex> norm(4,0.0);
    Complex phasor, nvalue;
    CountedPtr<CFBuffer> cfb=(*vb2CFBMap_p)[0];
    Vector<Int> cfShape=cfb->getStorage()(0,0,0)->getStorage()->shape().asVector();
    Bool finitePointingOffset=cfb->finitePointingOffsets();

    Vector<Int> convOrigin = (cfShape)/2;
    Double cfRefFreq;//cfScale=1.0
    
    rbeg=0;
    rend=vbs.nRow_p;
    rbeg = vbs.beginRow_p;
    rend = vbs.endRow_p;
    nx       = grid.shape()[0]; ny        = grid.shape()[1];
     nGridPol = grid.shape()[2]; nGridChan = grid.shape()[3];
    
    nDataPol  = vbs.flagCube_p.shape()[0];
    nDataChan = vbs.flagCube_p.shape()[1];
    
    //
    // The following code reduces most array accesses to the simplest
    // possible to improve performance.  However this made no
    // difference in the run-time performance compared to Vector,
    // Matrix and Cube indexing.
    //
    Bool Dummy;
    const Complex* __restrict__ gridStore = grid.getStorage(Dummy);
    (void)gridStore;
    Vector<Int> igrdpos(4);
    Double *freq=vbs.freq_p.getStorage(Dummy);
    Bool *rowFlag=vbs.rowFlag_p.getStorage(Dummy);
    
    Matrix<Double>& uvw=vbs.uvw_p;
    Cube<Complex>&  visCube=vbs.visCube_p;
    Cube<Bool>&     flagCube=vbs.flagCube_p;
    
    Vector<Int> gridInc, cfInc;
    
    cacheAxisIncrements(grid.shape().asVector(), gridInc_p);
    //cacheAxisIncrements(cfShape, cfInc_p);
    // Initialize the co-ordinates used for reading the CF values The
    // CFs are 4D arrays, with the last two axis degenerate (of length
    // 1).  The last two axis were formerly the W-, and
    // Polarization-axis.
    iloc = 0;
    Int vbSpw = (vbs.vb_p)->spectralWindows()(0);
    Double vbPA = vbs.paQuant_p.getValue("rad");

    Vector<Double> wVals, fVals; PolMapType mVals, mNdx, conjMVals, conjMNdx;
    Double fIncr, wIncr;
    cfb->getCoordList(fVals,wVals,mNdx, mVals, conjMNdx, conjMVals, fIncr, wIncr);
    nw = wVals.nelements();

    for(Int irow=rbeg; irow<rend; irow++) {
      if(!rowFlag[irow]) {

	setFieldPhaseGrad((vb2CFBMap_p->vectorPhaseGradCalculator_p[vb2CFBMap_p->vbRow2BLMap_p[irow]])->field_phaseGrad_p);
	cfb = (*vb2CFBMap_p)[irow];
	
	for (Int ichan=0; ichan < nDataChan; ichan++) {
	  achan=chanMap_p[ichan];
	  
	  if((achan>=0) && (achan<nGridChan)) {
	    Double dataWVal = (vbs.vb_p->uvw()(2,irow));
	    Int wndx = cfb->nearestWNdx(abs(dataWVal)*freq[ichan]/C::c);
	    Int fndx = cfb->nearestFreqNdx(vbSpw,ichan);
	    Float s;

	    cfb->getParams(cfRefFreq,s,support(0),support(1),fndx,wndx,0);
	    sampling(0) = sampling(1) = SynthesisUtils::nint(s);
	    
	    sgrid(pos,loc,off,phasor,irow,uvw,dphase_p[irow],freq[ichan],
		  uvwScale_p,offset_p,sampling);
	    
	    Bool isOnGrid;

	    {
	      for(Int ipol=0; ipol < nDataPol; ipol++)
		{
		  if(!flagCube(ipol,ichan,irow))
		    { 
		      apol=polMap_p[ipol];
		  
		      if((apol>=0) && (apol<nGridPol))
			{
			  igrdpos[2]=apol; igrdpos[3]=achan;
			  nvalue=0.0;      norm(ipol)=0.0;
		    
			  // With VBRow2CFMap in use, CF for each pol. plane is a separate 2D Array.  
			  for (uInt mCol=0; mCol<conjMNdx[ipol].nelements(); mCol++)
			    {
			      //
			      int visGridElement, muellerElement;
			      // Get the pointer to the storage for the CF
			      // indexed by the Freq, W-term and Mueller
			      // Element.
			      //
			      Complex*  convFuncV=NULL;
			      try
				{
				  convFuncV = getConvFunc_p(vbPA,cfShape, support, muellerElement,
							    cfb, dataWVal, fndx, wndx, conjMNdx,mNdx,
							    ipol, mCol);
				}
			      catch (SynthesisFTMachineError& x)
				{
				  LogIO log_l(LogOrigin("AWVisResampler[R&D]","GridToData"));
				  log_l << x.getMesg() << LogIO::EXCEPTION;
				}
			      // Set the polarization plane of the gridded data to use for predicting with the CF from mCols column
			      visGridElement=(int)(muellerElement%nDataPol);
			      igrdpos[2]=polMap_p[visGridElement];
			      //
			      // Compute the incrmenets and center pixel for the current CF
			      //
			      if ((isOnGrid=onGrid(nx, ny, nw, loc, support))==false) break;
			      cacheAxisIncrements(cfShape, cfInc_p);
			      convOrigin = (cfShape)/2;

#include <synthesis/TransformMachines2/accumulateFromGrid.inc>
			    }
			  if (norm[ipol] != Complex(0.0)) visCube(ipol,ichan,irow)=nvalue/norm[ipol]; // Goes with FortranizedLoopsFromGrid.cc
			}
		    }
		}
	    }
	  }
	}
      }
    } // End row-loop
  }
  //
  //-----------------------------------------------------------------------------------
  //
void AWVisResampler::sgrid(Vector<Double>& pos, Vector<Int>& loc, 
			   Vector<Double>& off, Complex& phasor, 
			   const Int& irow, const Matrix<Double>& uvw, 
			   const Double& dphase, const Double& freq, 
			   const Vector<Double>& scale, //float in HPG
			   const Vector<Double>& offset,
			   const Vector<Float>& sampling)
{
  // inv_lambda float in HPG Double here
  Double phase;
  Vector<Double> uvw_l(3,0); // This allows gridding of weights
  // centered on the uv-origin
  if (uvw.nelements() > 0) for(Int i=0;i<3;i++) uvw_l[i]=uvw(i,irow);
  
  pos(2)=sqrt(abs(scale[2]*uvw_l(2)*freq/C::c))+offset[2];
  //loc(2)=SynthesisUtils::nint(pos[2]);
  loc(2)=std::lrint(pos[2]);
  off(2)=0;
  
  for(Int idim=0;idim<2;idim++)
    {
      pos[idim]=scale[idim]*uvw_l(idim)*freq/C::c+(offset[idim]);
<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
      //loc[idim]=SynthesisUtils::nint(pos[idim]);
      loc[idim]=std::lrint(pos[idim]);
      //off[idim]=SynthesisUtils::nint((loc[idim]-pos[idim])*sampling[idim]);
=======

      loc[idim]=std::lrint(pos[idim]);

>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
      off[idim]=std::lrint((loc[idim]-pos[idim])*sampling[idim]);
    }
  
  if (dphase != 0.0)
    {
      phase=-2.0*C::pi*dphase*freq/C::c;
      Double sp,cp;
      SINCOS(phase,sp,cp);
      phasor=Complex(cp,sp);
    }
  else
    phasor=Complex(1.0);
<<<<<<< HEAD:casa5/code/synthesis/TransformMachines2/AWVisResampler.cc
  // //  if ((loc(0)==845) && (loc(1)==356))
  //   cerr << "AWP coord: " << " " 
  // 	 << pos[0] << " " << pos[1] << " "
  // 	 << loc(0) << " " << loc(1) << " " 
  // 	 << offset << " " 
  // 	 << uvw_l(0) << " " << uvw_l(1) << " " 
  // 	 << freq/C::c << scale[0] << " " 
  // 	 << scale[1] << " " 
  // 	 << off[0] << " " << off[1] << " "
  // 	 << std::lrint((loc[0]-pos[0])*sampling[0]) << " "
  // 	 << std::lrint((loc[1]-pos[1])*sampling[1]) << " "
  // 	 << std::fegetround()
  // 	 << endl;
  // exit(0);
}
//
//-----------------------------------------------------------------------------------
//
Bool AWVisResampler::reindex(const Vector<Int>& in, Vector<Int>& out,
			     const Double& sinDPA, const Double& cosDPA,
			     const Vector<Int>& Origin, const Vector<Int>& size)
{
  
  Bool onGrid=false;
  Int ix=in[0], iy=in[1];
  if (sinDPA != 0.0)
    {
      ix = SynthesisUtils::nint(cosDPA*in[0] + sinDPA*in[1]);
      iy = SynthesisUtils::nint(-sinDPA*in[0] + cosDPA*in[1]);
    }
  out[0]=ix+Origin[0];
  out[1]=iy+Origin[1];
  
  onGrid = ((out[0] >= 0) && (out[0] < size[0]) &&
	    (out[1] >= 0) && (out[1] < size[1]));
  if (!onGrid)
    cerr << "CF index out of range: " << out << " " << size << endl;
  return onGrid;
}


// void lineCFArea(const Int& th,
// 		  const Double& sinDPA,
// 		  const Double& cosDPA,
// 		  const Complex*__restrict__& convFuncV,
// 		  const Vector<Int>& cfShape,
// 		  const Vector<Int>& convOrigin,
// 		  const Int& cfInc,
// 		  Vector<Int>& iloc,
// 		  Vector<Int>& tiloc,
// 		  const Int* supportPtr,
// 		  const Float* samplingPtr,
// 		  const Double* offPtr,
// 		  Complex *cfAreaArrPtr)
// {
//   cfAreaArrPtr[th]=0.0;
//   for(Int ix=-supportPtr[0]; ix <= supportPtr[0]; ix++) 
//     {
// 	iloc[0]=(Int)((samplingPtr[0]*ix+offPtr[0])-1);//+convOrigin[0];
// 	tiloc=iloc;
// 	if (reindex(iloc,tiloc,sinDPA, cosDPA, 
// 		    convOrigin, cfShape))
// 	  {
// 	    wt = getFrom4DArray((const Complex * __restrict__ &)convFuncV, 
// 				tiloc,cfInc);
// 	    if (dataWVal > 0.0) wt = conj(wt);
// 	    cfAreaArrPtr[th] += wt;
// 	  }
//     }
// }

Complex AWVisResampler::getCFArea(Complex* __restrict__& convFuncV, 
				  Double& wVal, 
				  Vector<Int>& scaledSupport, 
				  Vector<Float>& scaledSampling,
				  Vector<Double>& off,
				  Vector<Int>& convOrigin, 
				  Vector<Int>& cfShape,
				  Double& sinDPA, 
				  Double& cosDPA)
{
  Vector<Int> iloc(4,0),tiloc(4);
  Complex cfArea=0, wt;
  Bool dummy;
  Int *supportPtr=scaledSupport.getStorage(dummy);
  Double *offPtr=off.getStorage(dummy);
  Float *samplingPtr=scaledSampling.getStorage(dummy);
  Int Nth=1;
  Vector<Complex> cfAreaArr(Nth);
  Complex *cfAreaArrPtr=cfAreaArr.getStorage(dummy);
  
  for(Int iy=-supportPtr[1]; iy <= supportPtr[1]; iy++) 
    {
      iloc(1)=(Int)((samplingPtr[1]*iy+offPtr[1])-1);//+convOrigin[1];
      for (Int th=0;th<Nth;th++)
	{
	  cfAreaArr[th]=0.0;
	  for(Int ix=-supportPtr[0]; ix <= supportPtr[0]; ix++) 
	    {
	      iloc[0]=(Int)((samplingPtr[0]*ix+offPtr[0])-1);//+convOrigin[0];
	      tiloc=iloc;
	      if (reindex(iloc,tiloc,sinDPA, cosDPA, 
			  convOrigin, cfShape))
		{
		  wt = getFrom4DArray((const Complex * __restrict__ &)convFuncV, 
				      tiloc,cfInc_p);
		  if (wVal > 0.0) wt = conj(wt);
		  cfAreaArrPtr[th] += wt;
		}
	    }
	}
      cfArea += sum(cfAreaArr);
    }
  //    cerr << "cfArea: " << scaledSupport << " " << scaledSampling << " " << cfShape << " " << convOrigin << " " << cfArea << endl;
  return cfArea;
=======
>>>>>>> CAS-13857-WITH-HPG:casatools/src/code/synthesis/TransformMachines2/AWVisResampler.cc
}
using namespace casacore;
};// end namespace casa
