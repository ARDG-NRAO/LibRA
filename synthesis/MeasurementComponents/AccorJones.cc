//# AccorJones.cc: Implementation of AccorJones
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003,2011,2017
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

#include <synthesis/MeasurementComponents/AccorJones.h>

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/Aipsrc.h>

#include <casa/sstream.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// **********************************************************
//  AccorJones Implementations
//

AccorJones::AccorJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisJones(vs)    // immediate parent
{
  if (prtlev()>2) cout << "Accor::Accor(vs)" << endl;
}

AccorJones::AccorJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisJones(msname,MSnAnt,MSnSpw)    // immediate parent
{
  if (prtlev()>2) cout << "Accor::Accor(msname,MSnAnt,MSnSpw)" << endl;
}

AccorJones::AccorJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisJones(msmc)    // immediate parent
{
  if (prtlev()>2) cout << "Accor::Accor(msmc)" << endl;
}

AccorJones::AccorJones(const Int& nAnt) :
  VisCal(nAnt),
  VisMueller(nAnt),
  SolvableVisJones(nAnt)
{
  if (prtlev()>2) cout << "Accor::Accor(nAnt)" << endl;
}

AccorJones::~AccorJones() {
  if (prtlev()>2) cout << "Accor::~Accor()" << endl;
}

void AccorJones::selfSolveOne(VisBuffGroupAcc&) {

  // NB: Use "AccorJones::selfSolveOne(SDBList& sdbs)" instead!
  throw(AipsError("VisBuffGroupAcc is invalid for AccorJones"));
}

void AccorJones::selfSolveOne(SDBList& sdbs) {

  // Use just the first SDB in the SDBList, for now
  SolveDataBuffer& sdb(sdbs(0));

  // Make a guess at antenna-based G
  //  Correlation membership-dependencexm
  //  assumes corrs in canonical order
  //  nCorr = 1: use icorr=0
  //  nCorr = 2: use icorr=[0,1]
  //  nCorr = 4: use icorr=[0,3]

  Int nCorr(2);
  Int nDataCorr(sdb.nCorrelations());
  Vector<Int> corridx(nCorr,0);
  if (nDataCorr==2) {
    corridx(0)=0;
    corridx(1)=1;
  }
  else if (nDataCorr==4) {
    corridx(0)=0;
    corridx(1)=3;
  }

  Int guesschan(sdb.nChannels()-1);

  //  cout << "guesschan = " << guesschan << endl;
  //  cout << "nCorr = " << nCorr << endl;
  //  cout << "corridx = " << corridx << endl;

  // Find out which ants are available
  Int nRow=sdb.nRows();
  Vector<Bool> rowok(nRow,False);
  for (Int irow=0;irow<nRow;++irow) {

    const Int& a1(sdb.antenna1()(irow));
    const Int& a2(sdb.antenna2()(irow));

    // Is this row ok?
    rowok(irow)= (!sdb.flagRow()(irow) &&
		  a1==a2);
  }

  const Cube<Complex>& V(sdb.visCubeCorrected());
  Float amp(0.0);
  solveCPar()=Complex(1.0);
  solveParOK()=False;
  for (Int irow=0;irow<nRow;++irow) {

    if (rowok(irow)) {
      const Int& a1=sdb.antenna1()(irow);
      const Int& a2=sdb.antenna2()(irow);

      if (a1 == a2) {

        for (Int icorr=0;icorr<nCorr;icorr++) {
          const Complex& Vi(V(corridx(icorr),guesschan,irow));
          amp=abs(Vi);
          if (amp>0.0f) {
	    solveCPar()(icorr,0,a1) = sqrt(amp);
	    solveParOK()(icorr,0,a1) = True;
	  }
	}
      }
    } // rowok
  } // irow

  // For scalar data, set "other" pol soln to zero
  if (nDataCorr == 1)
    solveCPar()(IPosition(3,1,0,0),IPosition(3,1,0,nAnt()-1))=Complex(0.0);
}

// Fill the trivial DJ matrix elements
void AccorJones::initTrivDJ() {

  if (prtlev()>4) cout << "   Accor::initTrivDJ" << endl;

  // Must be trivial
  AlwaysAssert((trivialDJ()),AipsError);

  //  1 0     0 0
  //  0 0  &  0 1
  //
  if (diffJElem().nelements()==0) {
    diffJElem().resize(IPosition(4,2,2,1,1));
    diffJElem()=0.0;
    diffJElem()(IPosition(4,0,0,0,0))=Complex(1.0);
    diffJElem()(IPosition(4,1,1,0,0))=Complex(1.0);
  }

}

void AccorJones::keepNCT() {
  for (Int elem=0;elem<nElem();++elem) {
    ct_->addRow(1);

    CTMainColumns ncmc(*ct_);

    // We are adding to the most-recently added row
    Int row=ct_->nrow()-1;

    // Meta-info
    ncmc.time().put(row,refTime());
    ncmc.fieldId().put(row,currField());
    ncmc.spwId().put(row,currSpw());
    ncmc.scanNo().put(row,currScan());
    ncmc.obsId().put(row,currObs());
    ncmc.interval().put(row,0.0);
    ncmc.antenna1().put(row,elem);
    ncmc.antenna2().put(row,-1);

    // Params
    ncmc.cparam().put(row,solveAllCPar().xyPlane(elem));

    // Stats
    ncmc.paramerr().put(row,solveAllParErr().xyPlane(elem));
    ncmc.snr().put(row,solveAllParErr().xyPlane(elem));
    ncmc.flag().put(row,!solveAllParOK().xyPlane(elem));
  }

  // This spw now has some solutions in it
  spwOK_(currSpw())=True;
}

} //# NAMESPACE CASA - END
