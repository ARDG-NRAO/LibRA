//
//-----------------------------------------------------------------------------------
// Page-in the required CFs and load them on the device.
//
std::tuple<hpg::opt_t<hpg::Error>, hpg::CFSimpleIndexer> 
AWVisResamplerHPG::loadCF(VBStore& vbs, Int& nGridPol, Int& nDataPol,
			  Vector<Int>& wNdxList,
			  Vector<Int>& spwNdxList,
			  bool send_to_device)
  {
    LogIO log_l(LogOrigin("HPGLoadCF","loadCF"));


    // Int vbSpw = (vbs.vb_p)->spectralWindows()(0);
    // if (cachedVBSpw_p != vbSpw)
    //   cachedVBSpw_p=vbSpw;

    Int irow=0, nWCF=1, nFreqCF, nPolCF;
    Vector<float> sampling(2);
    Vector<Int> support(2), cfShape;
    Vector<Double> wVals, fVals; PolMapType mVals, mNdx, conjMVals, conjMNdx;

    Bool Dummy;
    Double vbPA  = vbs.paQuant_p.getValue("rad");
    //Double *freq = vbs.freq_p.getStorage(Dummy);
    Double cfRefFreq,dataWVal=0,fIncr, wIncr;

    CountedPtr<CFBuffer> cfb = (*vb2CFBMap_p)[irow];
    cfb->getCoordList(fVals,wVals,mNdx, mVals, conjMNdx, conjMVals, fIncr, wIncr);

    nWCF = wNdxList.nelements();
    nFreqCF = spwNdxList.nelements();

    // Get the oversampling parameter.
    {
      Float s;
      int dummyWNdx=0;

      int refSpw;
      //refSpw = getNearestSpw(fVals,vbs.imRefFreq_p);
      //Double ref =
      SynthesisUtils::stdNearestValue(fVals.tovector(), vbs.imRefFreq_p, refSpw);
      // cerr << "RefSpw, W, F, RefFreq: "
      // 	   << refSpw << " " << wVals.nelements() << " " << fVals.nelements() << " " << vbs.imRefFreq_p
      // 	   << endl;
      
      cfb->getParams(cfRefFreq, s, support(0), support(1),refSpw,dummyWNdx,0);
      sampling(0) = sampling(1) = SynthesisUtils::nint(s);
    }
    
    nPolCF=0;
    std::map<int,int> p2m; // MP: map of mNdx[][] value to mueller index for CFArray
    if (cfArray.oversampling()==0)
      {
	//unsigned nc=mNdx[0].shape()(0);
	unsigned nr=mNdx.shape()(0);
	for (int r=0;r<nr;r++)
	  {
            // MP: the use of "p2m" fixes the issue mentioned below
            for (int c=0;c<mNdx[r].shape()[0];c++) {
              auto p = mNdx[r][c];
              if (p >= 0 && p2m.count(p) == 0) {
                auto m = p2m.size();
                p2m[p] = m;
              }
            }
	    // This assumes that all entries are unique elements of
	    // the Mueller matrix.  In effect, this ignores symmetries
	    // in the Mueller matrix elements, which can (should?) be
	    // exploited to reduce memory footprint.
	    // SB: nPolCF += conjMNdx[r].shape()(0); 
	    //cerr  << mNdx[r] << endl;
	  }
        nPolCF = p2m.size();
	log_l << "CFBuffer shape (nW x nF x nPol): " << nWCF << " x " << nFreqCF << " x " << nPolCF << LogIO::POST;

	unsigned nCF; 
        nCF = nWCF * nFreqCF;
	log_l << "Setting cfArray size: " << "nCF: " << nCF << " x " << nPolCF << " sampling: " << sampling(0) << LogIO::POST;

	cfArray.setSize((unsigned)(nCF),(unsigned)sampling(0));
      }
		      
    // CFSimpleIndexer(
    //   const axis_desc_t& baseline_class,
    //   const axis_desc_t& time,
    //   const axis_desc_t& w_plane,
    //   const axis_desc_t& frequency,
    //   unsigned mueller)

    //                               nBLType, nTime/PA, nW, nFreq, nPol
    hpg::CFSimpleIndexer cfsi({1,false},{1,false},{nWCF,true},{nFreqCF,true}, nPolCF);

    unsigned iGrp=0; // HPG Group index
    for(int iFreq=0; iFreq < nFreqCF; iFreq++) // CASA CF Freq-index
      {
	for(int iW=0; iW < nWCF; iW++)   // CASA CF W-index
	  {
	    for(int ipol=0; ipol < nDataPol; ipol++)  // CASA CF Pol-index
	      {  
		Int targetIMPol=polMap_p(ipol);
		if ((targetIMPol>=0) && (targetIMPol<nGridPol)) 
		  {
		    Vector<int> mRow = mNdx[ipol];
		    // ipol determines the targetIMPol.  Each targetIMPol gets a row of CFs (mRow).
		    // visVecElements is gridded using the convFuncV and added to the target grid.
				  
		    for (uInt mCols=0;mCols<mRow.nelements(); mCols++) 
		      {
			//int visVecElement=mCols;
			int muellerElement;
			Complex* convFuncV=NULL;
			//if (hpgCFInit==0)
			{
			  int fNdx=spwNdxList[iFreq];
			  int wNdx=wNdxList[iW];
			  //cerr << "F:" << fNdx << " W:" << wNdx << " m:" << mCols << " ip:" << ipol << " Fr:" << iFreq << endl;
			  convFuncV=getConvFunc_p(vbPA,cfShape, support,muellerElement,
						  cfb, dataWVal,
						  fNdx, wNdx,
						  mNdx, conjMNdx, ipol,  mCols);
			
			  //                                 (BL, PA, W, Freq, Pol)
			  hpg::CFCellIndex cfCellNdx(0,0,wNdx,fNdx,(mNdx[ipol][mCols])); // MP: last arg changed from mNdx[ipol][mCols]
			  //			hpg::CFCellIndex cfCellNdx(0,0,dummyNdx,iCF,(mNdx[ipol][mCols])); // MP: last arg changed from mNdx[ipol][mCols]
			  // SB: Check if p2m.at(mNdx[ipol][mCols] should go to mNdx[ipol][mCols]
			  // tt[0] ==> pol
			  // tt[1] ==> cube : should always be 0
			  // tt[2] ==> group : linearized index from (BLType, PA, W, SPW)
			  std::array<unsigned, 3> tt=cfsi.cf_index(cfCellNdx);
			  cfArray.resize(iGrp,cfShape(0), cfShape(1));//This allocates internal buffers in CFArray.
			  // SB: cfArray.setValues(convFuncV, iCF, cfShape(0), cfShape(1), mNdx[ipol][mCols], 0); 
			
			  // cfArray.setValues(convFuncV, tt[2], cfShape(0), cfShape(1), tt[0], tt[2]);

			  // Load pixel array only if the cfArray is
			  // to be sent to the device.  This function
			  // can then be re-used to determine the
			  // largest CFArrayShape.
			  if (send_to_device)
			    cfArray.setValues(convFuncV, iGrp, cfShape(0), cfShape(1), tt[0], tt[1]);
			  //if (ipol==0)
			  //cerr << "Supp@for ipol " << ipol << " " << support[0] << endl;
			}
		      }
		  }
	      } // Pol loop
	    //
	    // In libhpg, each CASA CF (with all the required
	    // polarization planes) is a single Group in CFArray.  So
	    // inrement the group index after loading each CASA CF
	    // (including all the required polarization planes).  This
	    // liearizes the BLType, PA/Time, W and SPW indexes (BLType
	    // and PA indexes when those loops are added above).
	    //
	    iGrp++;
	  } // W loop
	if (nWCF > 1) iGrp=0;
      } // Freq loop
    //    cerr << endl;
    hpg::opt_t<hpg::Error> err;
    if (send_to_device)
      {
	// allocate_convolution_function_region() should be called
	// *once* with the CFArray with the *largest* memory footprint
	// for the problem.  Currently, the largetst CFArray will be
	// for SPW=0.  This allocation will therefore happen in the
	// call to set_convolution_function() internally and will not
	// happen again (since all other CFArrays will be smaller).
	//
	// However in general, the largets CFArray for the problem
	// should be determined and allocation made once.
	//err= hpgGridder_p->allocate_convolution_function_region(&cfArray);

	// log_l << "Sending CFs..." << LogIO::POST;
	//if (!err)
	err=hpgGridder_p->set_convolution_function(hpg::Device::OpenMP, std::move(cfArray));
      }

    std::tuple<hpg::opt_t<hpg::Error>, hpg::CFSimpleIndexer> retup = std::make_tuple(err, cfsi);
    //    return cfsi;
    return retup;
  }
