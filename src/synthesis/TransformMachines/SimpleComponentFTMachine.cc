//# SimpleComponentFTMachine.cc: Implementation of SimpleComponentFTMachine class
//# Copyright (C) 1997,1998,1999,2000,2001
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

#include <synthesis/TransformMachines/SimpleComponentFTMachine.h>
#include <casacore/scimath/Mathematics/RigidVector.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/SpectralModel.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/measures/Measures/UVWMachine.h>
#ifdef _OPENMP
#include <omp.h>
#endif


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

void SimpleComponentFTMachine::get(VisBuffer& vb, SkyComponent& component,
				   Int row)
{
  // If row is -1 then we pass through all rows
  uInt startRow, endRow;
  if (row < 0) {
    startRow = 0;
    endRow = vb.nRow() - 1;
  } else {
    startRow = endRow = row;
  }
  const uInt nRow = endRow - startRow + 1;
   
  component.flux().convertUnit(Unit("Jy"));
   // Rotate the uvw
  Matrix<Double> uvw(3, nRow);
  Vector<Double> dphase(nRow);
  {
    const Vector<RigidVector<Double,3> >& uvwBuff = vb.uvw();
    for (uInt i = startRow, n = 0; i <= endRow; i++, n++) {
      const RigidVector<Double,3>& uvwValue = uvwBuff(i);
      //NEGATING THE UV to be consitent with the GridFT::get
      for (Int idim = 0; idim < 2; idim++) {
	uvw(idim, n) = -uvwValue(idim);
      }
      uvw(2, n) = uvwValue(2);
    }
    rotateUVW(uvw, dphase, vb, component.shape().refDirection());
    dphase *= -C::_2pi;
  }
  uInt npol=vb.modelVisCube().shape()(0);
  uInt nChan=vb.modelVisCube().shape()(1);
  Cube<Complex> modelData;
  modelData.reference(vb.modelVisCube());
  Vector<Complex> visibility(4);
  //UNUSED: Double phase;
  //UNUSED: Double phaseMult;
  Vector<Double>& frequency = vb.frequency();
  Vector<Double> invLambda = frequency/C::c;
   
  // Find the offsets in polarization. 
  Vector<Int> corrType = vb.corrType().copy();
  {
    Int startPol = corrType(0);
    if((startPol > 4) && (startPol < 9)) {
      component.flux().convertPol(ComponentType::CIRCULAR);
      corrType -= 5;
    }
    else if((startPol > 8) && (startPol < 13)) {
      component.flux().convertPol(ComponentType::LINEAR);
      corrType -= 9;
    }
    else {
      component.flux().convertPol(ComponentType::STOKES);
      corrType -= startPol;
    }
  }


  uInt npart=1;
#ifdef _OPENMP
  npart= numthreads_p <0 ? omp_get_max_threads() : min(numthreads_p, omp_get_max_threads());
#endif
  if((nRow/npart)==0) npart=1;
  Block<Cube<DComplex> > dVisp(npart);
  Vector<uInt> nRowp(npart);
  Block<Matrix<Double> > uvwp(npart);
  Block<SkyComponent> compp(npart);
  Block<Int> startrow(npart);
  startrow[0]=0;

  nRowp.set(nRow/npart);
  nRowp(npart-1) += nRow%npart;
  Int sumrow=0;
  Record lala;
  String err;
  component.toRecord(err, lala);
  for (uInt k=0; k < npart; ++k){
    dVisp[k].resize(4, nChan, nRowp(k));
    uvwp[k].resize(3, nRowp(k));
    uvwp[k]=uvw(IPosition(2,0, sumrow  ), IPosition(2, 2, sumrow+nRowp(k)-1));
    compp[k].fromRecord(err, lala);
    sumrow+=nRowp(k);
    startrow[k]=0;
    for (uInt j=0; j < k; ++j) startrow[k]+=nRowp(j);
  }

  
//#pragma omp parallel default(none)  firstprivate(npart) shared(frequency,dVisp, uvwp, compp) num_threads(npart)
#pragma omp parallel firstprivate(npart) shared(dVisp, uvwp, compp) num_threads(npart)
{

#pragma omp for
  for (Int k=0; k < Int(npart); ++k){
    //Cube<DComplex> dVis(4, nChan, nRow);
    compp[k].visibility(dVisp[k], uvwp[k], frequency);
  }

}
  
  // Loop over all rows
  sumrow=0;
  
  Bool isCopy;  
  Complex *modData=modelData.getStorage(isCopy);
 
#pragma omp parallel default(none)  firstprivate(npart, npol, nChan, modData, corrType, nRowp, dphase, invLambda) shared(startrow,  dVisp) num_threads(npart)
  {
#pragma omp for
    for (Int k = 0; k < Int(npart); ++k) {
    

    
      applyPhasor(k, startrow, nRowp, dphase, invLambda, npol, nChan, corrType, dVisp[k], modData);

    }
    
  }
  
  modelData.putStorage(modData, isCopy);
  
}

void SimpleComponentFTMachine::applyPhasor(Int part, const Block<Int>& startrow, const Vector<uInt>& nRowp,  const Vector<Double>& dphase, const Vector<Double>& invLambda, const Int npol, const Int nchan, const Vector<Int>& corrType, const Cube<DComplex>& dVis, Complex*& modData){

  Int r;
  Int rowoff;
  Double phaseMult;
  Double phase;
  for (uInt j=0; j< nRowp[part]; ++j){  
    r=startrow[part]+j;
    rowoff=r*nchan*npol; 
    phaseMult = dphase(r);
    for (Int chn = 0; chn < nchan; chn++) {
      phase = phaseMult * invLambda(chn);
      Complex phasor(cos(phase), sin(phase));
      for (Int pol=0; pol < npol; pol++) {
	const DComplex& val = dVis(corrType(pol), chn, j);
	modData[rowoff+chn*npol+pol] = Complex(val.real(), val.imag()) * conj(phasor);
      }
    }
  }



}


void SimpleComponentFTMachine::get(VisBuffer& vb, const ComponentList& compList,
				   Int row)
{
  // If row is -1 then we pass through all rows
  uInt startRow, endRow;
  if (row < 0) {
    startRow = 0;
    endRow = vb.nRow() - 1;
  } else {
    startRow = endRow = row;
  }
  const uInt nRow = endRow - startRow + 1;
   
   // Rotate the uvw
  Matrix<Double> uvw0(3, nRow);
  Matrix<Double> uvw(3, nRow); 
  Vector<Double> dphase(nRow);

  const Vector<RigidVector<Double,3> >& uvwBuff = vb.uvw();
  for (uInt i = startRow, n = 0; i <= endRow; i++, n++) {
    const RigidVector<Double,3>& uvwValue = uvwBuff(i);
    //NEGATING the UV to be consitent with GridFT::get
    for (Int idim = 0; idim < 2; idim++) {
      uvw(idim, n) = -uvwValue(idim);
    }
    uvw(2, n) = uvwValue(2);
  }
  uvw0=uvw.copy();
  
  uInt ncomponents=compList.nelements();
  uInt npol=vb.modelVisCube().shape()(0);
  uInt nChan=vb.modelVisCube().shape()(1);
  Cube<Complex> modelData;
  modelData.reference(vb.modelVisCube());
  modelData=0.0;

  Vector<Complex> visibility(4);
  //UNUSED: Double phase;
  //UNUSED: Double phaseMult;
  Vector<Double> frequency;
  frequency= vb.frequency();
  Vector<Double> invLambda = frequency/C::c;
  // Find the offsets in polarization. 
  Vector<Int> corrTypeL = vb.corrType().copy();
  Vector<Int> corrTypeC = vb.corrType().copy();
  Vector<Int> corrType = vb.corrType().copy();
  corrTypeL -= 9;
  corrTypeC -= 5;
  Cube<DComplex> dVis(4, nChan, nRow);
  ComponentType::Polarisation poltype=ComponentType::CIRCULAR;
  if(anyGT(Int(Stokes::RR), vb.corrType())){
    poltype=ComponentType::STOKES;
    corrType = vb.corrType()-1;
  }
  else if(vb.polFrame()==MSIter::Linear) {
    poltype=ComponentType::LINEAR;
    corrType = corrTypeL;
    } else {
    poltype=ComponentType::CIRCULAR;
    corrType = corrTypeC;
  }

 uInt npart=1;
#ifdef _OPENMP
 npart= numthreads_p <0 ? omp_get_max_threads() : min(numthreads_p, omp_get_max_threads());
#endif



  if((nRow/npart)==0) npart=1;
  Block<Cube<DComplex> > dVisp(npart);
  Vector<uInt> nRowp(npart);
  Block<Matrix<Double> > uvwp(npart);
  Block<ComponentList> compp(npart);
  Block<MeasFrame> mFramep(npart);
  Block<MDirection> mDir(npart);
  Block<uInt> startrow(npart);

  nRowp.set(nRow/npart);
  nRowp(npart-1) += nRow%npart;

  
  Block<Vector<Double> > dphasecomp(ncomponents);
  Block<Matrix<Double> > uvwcomp(ncomponents);
  Block<Block<Vector<Double> > > dphasecomps(npart);
  Block<Block<Matrix<Double> > > uvwcomps(npart);
  for (uInt jj=0; jj < npart; ++jj){
    dphasecomps[jj].resize(ncomponents);
    uvwcomps[jj].resize(ncomponents);
  }
  ///Have to do this in one thread as MeasFrame and thus uvwmachine is not thread safe
  for (uInt k=0; k < ncomponents; ++k){
    uvwcomp[k]=uvw;
    dphasecomp[k].resize(nRow);
    rotateUVW(uvwcomp[k], dphasecomp[k],  vb, compList.component(k).shape().refDirection());
    dphasecomp[k] *= -C::_2pi;
    Int sumrow=0;
    for (uInt jj=0; jj < npart; ++jj){
      dphasecomps[jj][k]=dphasecomp[k](IPosition(1,sumrow), IPosition(1, sumrow+nRowp(jj)-1));
      uvwcomps[jj][k]=uvwcomp[k](IPosition(2,0, sumrow  ), IPosition(2, 2, sumrow+nRowp(jj)-1));
      sumrow+=nRowp(jj);
    }
   
  }



  //UNUSED: Int sumrow=0;
  Record lala;
  String err;
  compList.toRecord(err, lala);
  for (uInt k=0; k < npart; ++k){
    compp[k].fromRecord(err, lala);
    startrow[k]=0;
    for (uInt j=0; j < k; ++j) startrow[k]+=nRowp(j);
  }
  Bool isCopy;
  Complex *modData=modelData.getStorage(isCopy);
  MDirection dataMDir=vb.phaseCenter(phaseCenterTime_p);

#pragma omp parallel default(none)  firstprivate(npart, npol, nChan, modData, corrType, nRowp,  invLambda, frequency, poltype) shared(startrow, compp, uvwcomps, dphasecomps) num_threads(npart)
  {
#pragma omp for

    for (Int k=0; k < Int(npart); ++k){

    predictVis(modData, invLambda, frequency, compp[k],  poltype, corrType, 
	       startrow[k], nRowp[k], nChan, npol, uvwcomps[k], dphasecomps[k]);

  }

  
  }
  modelData.putStorage(modData, isCopy);
 
}


  void SimpleComponentFTMachine::predictVis(Complex*& modData, const Vector<Double>& invLambda, 
					    const Vector<Double>& frequency, const ComponentList& compList,   
					    ComponentType::Polarisation poltype, const Vector<Int>& corrType, 
					    const uInt startrow, const uInt nrows, const uInt nchan, const uInt npol, const Block<Matrix<Double> > & uvwcomp, const Block<Vector<Double> > & dphasecomp){
    Cube<DComplex> dVis(4, nchan, nrows);
    uInt ncomponents=compList.nelements();
    Vector<Double> dphase(nrows);
    
    for (uInt icomp=0;icomp<ncomponents; ++icomp) {
      SkyComponent component=compList.component(icomp);
      component.flux().convertUnit(Unit("Jy"));
      component.flux().convertPol(poltype);
      MDirection compdir=component.shape().refDirection();
      Vector<Double> thisRow(3);
      thisRow=0.0;
      //UNUSED: uInt i;
      Matrix<Double> uvw;
      uvw=uvwcomp[icomp];
      Vector<Double> dphase;
      dphase=dphasecomp[icomp];
      component.visibility(dVis, uvw, frequency);
      Double phaseMult;
      Double phase;
      uInt realrow;
      //// Loop over all rows
      for (uInt r = 0; r < nrows ; r++) {
	phaseMult = dphase(r);
	realrow=r+startrow;
	for (uInt chn = 0; chn < nchan; chn++) {
	  phase = phaseMult * invLambda(chn);	
	  Complex phasor(cos(phase), sin(phase));
	  for (uInt pol=0; pol < npol; pol++) {
	    const DComplex& val = dVis(corrType(pol), chn, r);
	    modData[realrow*npol*nchan+chn*npol + pol] += Complex(val.real(), val.imag()) * conj(phasor);
	  }
	}
      }


    }
    
  }  



} //# NAMESPACE CASA - END

