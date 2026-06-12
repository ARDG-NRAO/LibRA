// -*- C++ -*-
//# DeprecatedCode.cc: Parking for code not used, but may be useful at some point
//                     This file is not compiled.
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
  
  // // This methods is depcricated and comes from AWConvFunc.cc.
  // // Keeping it here since it *might* have use sometime later and
  // // therefore want to push it on to SVN before deleting it form the
  // // active version of this file.
  // Matrix<Double> SynthesisUtils::getFreqRangePerSpw(const VisBuffer2& vb)
  // {
  //   //
  //   // Find the total effective bandwidth
  //   //
  //   Cube<Double> fminmax;
  //   Double fMax=0, fMin=MAX_FREQ;
  //   ArrayColumn<Double> spwCol=vb.subtableColumns().spectralWindow().chanFreq();
  //   fminmax.resize(spwChanSelFlag_p.shape()(0),spwChanSelFlag_p.shape()(1),2);
  //   fminmax=0;
  //   for (uInt ims=0; ims<spwChanSelFlag_p.shape()(0); ims++)
  //     for(uInt ispw=0; ispw<spwChanSelFlag_p.shape()(1); ispw++)
  // 	{
  // 	  fMax=0, fMin=MAX_FREQ;
  // 	  for(uInt ichan=0; ichan<spwChanSelFlag_p.shape()(2); ichan++)
  // 	    {
  // 	      if (spwChanSelFlag_p(ims,ispw,ichan)==1)
  // 		{
  // 		  Slicer slicer(IPosition(1,ichan), IPosition(1,1));
  // 		  Vector<Double> freq = spwCol(ispw)(slicer);
  // 		  if (freq(0) < fMin) fMin = freq(0);
  // 		  if (freq(0) > fMax) fMax = freq(0);
  // 		}
  // 	    }
  // 	  fminmax(ims,ispw,0)=fMin;
  // 	  fminmax(ims,ispw,1)=fMax;
  // 	}

  //   Matrix<Double> freqRangePerSpw(fminmax.shape()(1),2);
  //   for (uInt j=0;j<fminmax.shape()(1);j++) // SPW
  //     {
  // 	freqRangePerSpw(j,0)=0;
  // 	freqRangePerSpw(j,1)=MAX_FREQ;
  // 	for (uInt i=0;i<fminmax.shape()(0);i++) //MSes
  // 	  {
  // 	    if (freqRangePerSpw(j,0) < fminmax(i,j,0)) freqRangePerSpw(j,0)=fminmax(i,j,0);
  // 	    if (freqRangePerSpw(j,1) > fminmax(i,j,1)) freqRangePerSpw(j,1)=fminmax(i,j,1);
  // 	  }
  //     }
  //   for(uInt i=0;i<freqRangePerSpw.shape()(0);i++)
  //     {
  // 	if (freqRangePerSpw(i,0) == MAX_FREQ) freqRangePerSpw(i,0)=-1;
  // 	if (freqRangePerSpw(i,1) == 0) freqRangePerSpw(i,1)=-1;
  //     }

  //   return freqRangePerSpw;
  // }
