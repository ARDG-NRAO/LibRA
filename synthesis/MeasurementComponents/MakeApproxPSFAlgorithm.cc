//# MakeAppproxPSFAlgorithm.cc: implementation of MakeApproxPSFAlgorithm.h
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
//# $Id$

#include <casa/BasicSL/String.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Array.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/Lattices/PagedArray.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <casa/OS/File.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/sstream.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <synthesis/MeasurementComponents/MakeApproxPSFAlgorithm.h>
#include <casa/OS/Time.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <unistd.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

extern Applicator applicator; 

MakeApproxPSFAlgorithm::MakeApproxPSFAlgorithm() : 
  myName_p("MakeApproxPSFAlgorithm"), 
  msFileName_p(""), beam_p(0), beamArray_p(0)
{
};

MakeApproxPSFAlgorithm::~MakeApproxPSFAlgorithm(){
};

void MakeApproxPSFAlgorithm::get(){

  Record container_ft;
  applicator.get(container_ft);

  // Construct the FTMachine
  ft_p = new GridFT(container_ft);
  applicator.get(msFileName_p);
  applicator.get(weight_p);
  
  Record image_container;
  applicator.get(image_container);
  cImage_p = new TempImage<Complex> ();
  String error;
  cImage_p->fromRecord(error, image_container);
  psf_p = new TempImage<Float>(cImage_p->shape(), cImage_p->coordinates());
};

void MakeApproxPSFAlgorithm::put(){

  applicator.put(beam_p);
  applicator.put(*beamArray_p);
  delete ft_p;
  delete cImage_p;
  delete psf_p;
  beamArray_p->resize();
  weight_p.resize();
};

String& MakeApproxPSFAlgorithm::name(){
  return myName_p;
};
 
void MakeApproxPSFAlgorithm::task(){

  try{
    MeasurementSet ms(msFileName_p, TableLock(TableLock::UserNoReadLocking),
		      Table::Update);
    // Deal with subtle sorted_table locking issues
    ms.unlock(); 
    Block<Int> sort(0);
    Matrix<Int> noselection;
    Double interval=10.0;
    VisSet vs(ms, sort, noselection, interval);

    ft_p->makeImage(FTMachine::PSF, vs, *cImage_p,
		    weight_p);
    StokesImageUtil::To(*psf_p, *cImage_p);

    beam_p= *(new Vector<Float>(3));
    StokesImageUtil::FitGaussianPSF(*psf_p, beam_p);

    Int nx=psf_p->shape()(0);
    Int ny=psf_p->shape()(1);
    LCBox psfbox(IPosition(4, 0, 0, 0, 0), 
		 IPosition(4, nx-1, ny-1, 0, 0),
		 psf_p->shape());
  
    SubLattice<Float>  psf_sl (*psf_p, psfbox, true);
    Array<Float> tempbeamArray;
    //Bool rStat = ((Lattice<Float> &)psf_sl).get(tempbeamArray);
    //UNUSED: Bool rStat = psf_sl.get(tempbeamArray);

    if (beamArray_p) delete beamArray_p;
    beamArray_p= new Array<Float>(tempbeamArray);

    ms.unlock();

  } catch (AipsError x) {
    cout <<  "Exceptionally yours: " << x.getMesg() << endl;
  };
};








} //# NAMESPACE CASA - END

