// -*- C++ -*-
//
// File to include to get the inner gridding loops without a function call
//
//--------------------------------------------------------------------------------
  {
    Int iLoc[2];
    Bool Dummy;
    Complex wt;

    Vector<Int> iCFPos(4,0);
    const Int * __restrict__ gridInc_p_ptr= gridInc_p.getStorage(Dummy);
    const Int * __restrict__ iGrdPosPtr = igrdpos.getStorage(Dummy);
    T* __restrict__ gridStore = grid.getStorage(Dummy);

    const Int* scaledSupport_ptr=support.getStorage(Dummy);
    const Float *scaledSampling_ptr=sampling.getStorage(Dummy);
    const Double *off_ptr=off.getStorage(Dummy);
    const Int *loc_ptr = loc.getStorage(Dummy);
    Int *iCFPos_ptr = iCFPos.getStorage(Dummy);
    const Int* convOrigin_ptr=convOrigin.getStorage(Dummy);
    Int *igrdpos_ptr=igrdpos.getStorage(Dummy);
    Int *cfInc_p_ptr = cfInc_p.getStorage(Dummy);
    
    Int phaseGradOrigin_l[2]; 
    phaseGradOrigin_l[0] = cached_phaseGrad_p.shape()(0)/2;
    phaseGradOrigin_l[1] = cached_phaseGrad_p.shape()(1)/2;

    for(Int iy=-scaledSupport_ptr[1]; iy <= scaledSupport_ptr[1]; iy++) 
      {
	//	iloc_ptr[1]=SynthesisUtils::nint((scaledSampling_ptr[1]*iy+off_ptr[1])-1);
	iLoc[1]=(Int)((scaledSampling_ptr[1]*iy+off_ptr[1])-1);
	igrdpos_ptr[1]=loc_ptr[1]+iy;
        iCFPos_ptr[1] =iLoc[1]+convOrigin_ptr[1];

	for(Int ix=-scaledSupport_ptr[0]; ix <= scaledSupport_ptr[0]; ix++) 
	  {
	    //	    iloc_ptr[0]=SynthesisUtils::nint((scaledSampling_ptr[0]*ix+off_ptr[0])-1);
	    iLoc[0]=(Int)((scaledSampling_ptr[0]*ix+off_ptr[0])-1);
	    igrdpos_ptr[0]=loc_ptr[0]+ix;
	    iCFPos_ptr[0] =iLoc[0]+convOrigin_ptr[0];
	    wt = getFrom4DArray((const Complex * __restrict__ &)convFuncV, 
				iCFPos_ptr,cfInc_p_ptr);//cfArea;
	    if (dataWVal > 0.0) {wt = conj(wt);}
	    norm += (wt);
	    if (finitePointingOffsets)
	      {
		wt *= cached_phaseGrad_p(iLoc[0]+phaseGradOrigin_l[0],
					 iLoc[1]+phaseGradOrigin_l[1]);
	      }
	    addTo4DArray_ptr(gridStore,iGrdPosPtr,gridInc_p_ptr, nvalue,wt);
	  }
      }
  }
