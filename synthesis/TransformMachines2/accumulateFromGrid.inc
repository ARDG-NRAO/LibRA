// -*- C++ -*-
//
// File to include to have the inner loops for de-gridding
// (prediction) without a function call.
//
//--------------------------------------------------------------------------------
{
  Vector<Int> iCFPos(4,0);
  Int iLoc[2];
  Bool Dummy;
  Complex wt;

  const Int * __restrict__ gridInc_p_ptr = gridInc_p.getStorage(Dummy);
  const Complex * __restrict__ gridStore = grid.getStorage(Dummy);
  const Int* support_ptr    = support.getStorage(Dummy);
  const Float *sampling_ptr = sampling.getStorage(Dummy);
  const Double *off_ptr           = off.getStorage(Dummy);
  const Int* convOrigin_ptr       = convOrigin.getStorage(Dummy);
  const Int *loc_ptr = loc.getStorage(Dummy);

  Int * __restrict__       igrdpos_ptr   = igrdpos.getStorage(Dummy);
  Int *iCFPos_ptr                 = iCFPos.getStorage(Dummy);
  Int *cfInc_p_ptr                = cfInc_p.getStorage(Dummy);
  Int phaseGradOrigin_l[2]; 
  
  phaseGradOrigin_l[0] = cached_phaseGrad_p.shape()(0)/2;
  phaseGradOrigin_l[1] = cached_phaseGrad_p.shape()(1)/2;

  for(Int iy=-support_ptr[1]; iy <= support_ptr[1]; iy++) 
    {
      iLoc[1]    = SynthesisUtils::nint(sampling_ptr[1]*iy+off_ptr[1]-1);
      igrdpos_ptr[1] = loc_ptr[1]+iy;
      iCFPos_ptr[1]  = iLoc[1] + convOrigin_ptr[1];

      for(Int ix=-support_ptr[0]; ix <= support_ptr[0]; ix++) 
	{
	  iLoc[0]    = SynthesisUtils::nint(sampling_ptr[0]*ix+off_ptr[0]-1);
	  igrdpos_ptr[0] = loc_ptr[0]+ix;
	  iCFPos_ptr[0]  = iLoc[0] + convOrigin_ptr[0];
	  {
	    wt=getFrom4DArray((const Complex * __restrict__ &)convFuncV,
			      //igrdpos,cfInc_p);
			      iCFPos_ptr,cfInc_p_ptr);
	    if (dataWVal <= 0.0) wt = conj(wt);
	    norm(ipol)+=(wt);
	    if (finitePointingOffset) 
	      {
		wt *= conj(cached_phaseGrad_p(iLoc[0]+phaseGradOrigin_l[0],
					      iLoc[1]+phaseGradOrigin_l[1]));
	      }
	    // nvalue+=wt*grid(grdpos);
	    // The following uses raw index on the 4D grid
	    // nvalue+=wt*getFrom4DArray(gridStore,iPosPtr,gridInc);
	    nvalue+=wt*getFrom4DArray(gridStore,igrdpos_ptr,gridInc_p_ptr);
	    //nvalue+=wt*getFrom4DArray(gridStore,igrdpos,gridInc_p);
	  }
	}
    }
  nvalue = nvalue *conj(phasor);
}
//--------------------------------------------------------------------------------
