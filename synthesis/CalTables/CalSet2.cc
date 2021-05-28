//# CalSet.cc: Implementation of Calibration parameter cache
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <synthesis/CalTables/CalSet.h>

#include <casa/Arrays.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/GenSort.h>
#include <casa/Exceptions/Error.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


void smooth(CalSet<Complex>& cs,
	    const String& smtype,
	    const Double& smtime,
	    Vector<Int> selfields) {

  // half-width
  Double thw(smtime/2.0);

  // Workspace
  Vector<Double> times;
  Vector<Int> fields;
  Vector<Int> slotidx;
  Vector<Complex> p;
  Vector<Bool> pOK, newpOK;
  Vector<Float> amp;
  Vector<Float> pha;
  Vector<Bool> mask;
  Float newamp(0.0), newpha(0.0);

  IPosition blc(4,0,0,0,0);
  IPosition trc(4,0,0,0,0);
  IPosition vec(1,0);
  
  // For each spw
  for (Int ispw=0;ispw<cs.nSpw();++ispw) {
    
    Int nSlot=cs.nTime(ispw);
    
    // Only if more than one slot in this spw
    if (nSlot>1) {
      
      vec(0)=nSlot;
      trc(3)=nSlot-1;

      times.reference(cs.time(ispw));
      fields.reference(cs.fieldId(ispw));
      slotidx.resize(nSlot);
      indgen(slotidx);
      newpOK.resize(nSlot);

      // Discern how many fields we must do
      Int nFld=selfields.nelements();
      // Do all fields present, if none explicitly specificed
      if (nFld==0) {
	selfields=fields;
	nFld=genSort(selfields,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
	selfields.resize(nFld,true);
      }

      // Arrange to mask/index each field
      PtrBlock<Vector<Bool>*> fldmask(nSlot,NULL);
      PtrBlock<Vector<Int>*> fldidx(nSlot,NULL);
      for (Int ifld=0;ifld<nFld;++ifld) {
	fldmask[ifld] = new Vector<Bool>;
	(*fldmask[ifld]) = (fields==selfields(ifld));
	fldidx[ifld] = new Vector<Int>;
	(*fldidx[ifld]) = slotidx((*fldmask[ifld])).getCompressedArray();
      }	

      // For each elem (ant or baseline)
      for (Int ielem=0;ielem<cs.nElem();++ielem) {
	blc(2)=trc(2)=ielem;
	// For each channel
	for (int ichan=0;ichan<cs.nChan(ispw);++ichan) {
	  blc(1)=trc(1)=ichan;
	  // For each param (pol)
	  for (Int ipar=0;ipar<cs.nPar();++ipar) {
	    blc(0)=trc(0)=ipar;
	    
	    // Reference slices of par/parOK
	    p.reference(cs.par(ispw)(blc,trc).reform(vec));
	    pOK.reference(cs.parOK(ispw)(blc,trc).reform(vec));
	    newpOK=pOK;

	    IPosition psh(p.shape());
	    amp.resize(psh);
	    pha.resize(psh);

       /*
	    cout << ispw << " "
		 << ielem << " "
		 << ichan << " "
		 << ipar << " "
		 << "p.shape() = " << p.shape() << " "
		 << "pOK.shape() = " << pOK.shape() << " "
		 << "amp.shape() = " << amp.shape() << " "
		 << "pha.shape() = " << pha.shape() << " "
		 << endl;
       */
	    // Copy out amp and phase for processing
	    amplitude(amp,p);
	    phase(pha,p);

	    // Filter each field separately
	    for (Int ifld=0;ifld<nFld;++ifld) {
	    
	      Vector<Int> idx;
	      idx.reference(*fldidx[ifld]);

	      // If more than one slot for this field
	      Int nidx=idx.nelements();
	      if (nidx>1) {
	    
		// Remove any phase cycles
		// (TBD: improve this algorithm?)
		Float phdif(0.0);
		for (Int i=1;i<nidx;++i) {
		  phdif=pha(idx(i))-pha(idx(i-1));
		  if (phdif > C::pi) {
		    pha(idx(i)) -= (2*C::pi);
		    //		    cout << " **************cycle+++++++++++" << endl;
		  }
		  else if (phdif < -C::pi) {
		    pha(idx(i)) += (2*C::pi);
		    //		    cout << " **************cycle-----------" << endl;
		  }
		}

		Vector<Bool> mask;
		for (Int i=0;i<nidx;++i) {
		  // Make mask
		  mask = (*fldmask[ifld]);
		  mask = (mask && pOK);
		  mask = (mask && ( (times >  (times(idx(i))-thw)) && 
				    (times <= (times(idx(i))+thw)) ) );

		  // Avoid explicit zeros, for now
		  mask = (mask && amp>=FLT_MIN);


		  //cout << "    " << ifld << " " << i << " " << idx(i) << " ";
		  //for (Int j=0;j<mask.nelements();++j)
		  //  cout << mask(j);
		  //cout << endl;

		  if (ntrue(mask)>0) {
		    if (smtype=="mean") {
		      newamp = mean(amp(mask));
		      newpha = mean(pha(mask));
		    }
		    else if (smtype=="median") {
		      newamp = median(amp(mask),false);
		      newpha = median(pha(mask),false);
		    }
		    p(idx(i)) = Complex(cos(newpha),sin(newpha))*newamp;
		    newpOK(idx(i))=true;
		  }
		  else 
		    newpOK(idx(i))=false;
		
		} // i
	      } // nidx>1
	    } // ifld
	    // keep new ok info
	    pOK=newpOK;
	  } // ipar
	} // ichan
      } // ielem

      // Delete the PtrBlocks
      for (Int ifld=0;ifld<nFld;++ifld) {
	delete fldmask[ifld];
	delete fldidx[ifld];
      }
      fldmask=NULL;
      fldidx=NULL;
    } // nSlot>1
  } // ispw
	
}

} //# NAMESPACE CASA - END
