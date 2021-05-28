//# CalVisBuffer.cc: Extends VisBuffer for calibration purposes
//# Copyright (C) 2008
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

#include <msvis/MSVis/CalVisBuffer.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

CalVisBuffer::CalVisBuffer() : 
  VisBuffer(),
  focusChan_p(-1),
  //  infocusFlagCube_p(),
  infocusFlag_p(),
  infocusVisCube_p(),
  infocusModelVisCube_p(),
  residuals_p(),
  residFlag_p(),
  diffResiduals_p()
{}

CalVisBuffer::CalVisBuffer(ROVisibilityIterator& iter) :
  VisBuffer(iter),
  focusChan_p(-1),
  //  infocusFlagCube_p(),
  infocusFlag_p(),
  infocusVisCube_p(),
  infocusModelVisCube_p(),
  residuals_p(),
  residFlag_p(),
  diffResiduals_p()

{}

CalVisBuffer::CalVisBuffer(const CalVisBuffer& vb) :
  VisBuffer(vb),
  focusChan_p(-1),
  //  infocusFlagCube_p(),
  infocusFlag_p(),
  infocusVisCube_p(),
  infocusModelVisCube_p(),
  residuals_p(),
  residFlag_p(),
  diffResiduals_p()
{}

CalVisBuffer::~CalVisBuffer()
{}

CalVisBuffer& CalVisBuffer::operator=(const VisBuffer& other)
{

  if (this!=&other) {
    // delete workspaces
    this->cleanUp();
    // call parent
    VisBuffer::operator=(other);
  }

  return *this;
}

CalVisBuffer& CalVisBuffer::assign(const VisBuffer& other, Bool copy)
{
  // delete workspaces 
  this->cleanUp();

  // Call parent:
  VisBuffer::assign(other,copy);

  return *this;

}

void CalVisBuffer::updateCoordInfo(const VisBuffer * /*vb=NULL*/, const Bool /*dirDependent=true*/)
{
  // Just do the nominally non-row-dep values
  arrayId();
  fieldId();
  spectralWindow();
  nCorr();
  nChannel();
  frequency();

}    
  
void CalVisBuffer::enforceAPonData(const String& apmode)
{

  // ONLY if something to do
  if (apmode=="A" || apmode=="P") {
    Int nCor(nCorr());
    Float amp(1.0);
    Complex cor(1.0);
    Bool *flR=flagRow().data();
    Bool *fl =flag().data();
    Vector<Float> ampCorr(nCor);
    Vector<Int> n(nCor,0);
    for (Int irow=0;irow<nRow();++irow,++flR) {
      if (!flagRow()(irow)) {
	ampCorr=0.0f;
	n=0;
	for (Int ich=0;ich<nChannel();++ich,++fl) {
	  if (!flag()(ich,irow)) {
	    for (Int icorr=0;icorr<nCor;icorr++) {
	      
	      amp=abs(visCube()(icorr,ich,irow));
	      if (amp>0.0f) {
		
		if (apmode=="P") {
		  // we will scale by amp to make data phase-only
		  cor=Complex(amp,0.0);
		  // keep track for weight adjustment
		  ampCorr(icorr)+=abs(cor);
		  n(icorr)++;
		}
		else if (apmode=="A")
		  // we will scale by "phase" to make data amp-only
		  cor=visCube()(icorr,ich,irow)/amp;
		
		// Apply the complex scaling
		visCube()(icorr,ich,irow)/=cor;
	      }
	    } // icorr
	  } // !*fl
	} // ich
	// Make appropriate weight adjustment
	//  (only required for phase-only, since only it rescales data)
	if (apmode=="P") {
	  for (Int icorr=0;icorr<nCor;icorr++)
	    if (n(icorr)>0)
	      // weights adjusted by square of the mean(amp)
	      weightMat()(icorr,irow)*=square(ampCorr(icorr)/Float(n(icorr)));
	    else
	      // weights now zero
	      weightMat()(icorr,irow)=0.0f;
	}
      } // !*flR
    } // irow

  } // phase- or amp-only

  //  cout << "amp(visCube())=" << amplitude(visCube().reform(IPosition(1,visCube().nelements()))) << endl;

}



void CalVisBuffer::setFocusChan(const Int focusChan) 
{
  // Nominally focus on the whole data array
  IPosition focusblc(3,0,0,0);
  IPosition focustrc(visCube().shape());
  focustrc-=1;
  
  // if focusChan non-negative, select the single channel
  if (focusChan>-1) 
    focusblc(1)=focustrc(1)=focusChan;
  
  //  infocusFlagCube_p.reference(flagCube()(focusblc,focustrc));
  infocusFlag_p.reference(flag()(focusblc.getLast(2),focustrc.getLast(2)));
  infocusVisCube_p.reference(visCube()(focusblc,focustrc));
  infocusModelVisCube_p.reference(modelVisCube()(focusblc,focustrc));

  // Remember current in-focus channel
  focusChan_p=focusChan;

}

void CalVisBuffer::sizeResiduals(const Int& nPar,
				 const Int& nDiff)
{

  IPosition ip1(visCube().shape());
  if (focusChan_p>-1)
    ip1(1)=1;
  residuals_p.resize(ip1);
  residuals_p.set(0.0);

  if (nPar>0 && nDiff>0) {
    IPosition ip2(5,ip1(0),nPar,ip1(1),ip1(2),nDiff);
    diffResiduals_p.resize(ip2);
    diffResiduals_p.set(0.0);
  }

}

void CalVisBuffer::initResidWithModel() 
{
  // Copy (literally) the in-focus model to the residual workspace
  // TBD:  obey flags here!?
  // TBD:  weights and flagCube...
  residuals_p = infocusModelVisCube_p;

  // TBD: should probably move this to setFocusChan, so that
  //  we can optimize handling of flags better (e.g., prior to repeated
  //  calls to SVC.differentiate, etc.)  (initResidWithModel should
  //  be viewed as just _refreshing_ the residuals with the model
  //  data for a new trial corrupt)
  if (focusChan_p>-1) {
    // copy flags so they are contiguous
    residFlag_p.resize(infocusFlag_p.shape());
    residFlag_p=infocusFlag_p;
  }
  else
    // just reference the whole (contiguous) infocusFlag array
    residFlag_p.reference(infocusFlag_p);
    
  // Ensure contiguity, because CalSolver will depend on this
  AlwaysAssert(residFlag_p.contiguousStorage(),AipsError);
  AlwaysAssert(residuals_p.contiguousStorage(),AipsError);

}

void CalVisBuffer::finalizeResiduals() 
{
  // Subtract in-focus obs data from residuals workspace
  residuals_p -= infocusVisCube_p;

  // TBD: zero flagged samples here?
    
}

void CalVisBuffer::cleanUp() 
{

  // Zero-size all workspaces
  //  infocusFlagCube_p.resize();
  infocusFlag_p.resize();
  infocusVisCube_p.resize();
  infocusModelVisCube_p.resize();

  residuals_p.resize();
  residFlag_p.resize();
  diffResiduals_p.resize();

}



} //# NAMESPACE CASA - END

