//# tGridFT.cc: Tests the Synthesis model data serving
//# Copyright (C) 2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/Flux.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/GridFT.h>
#include <synthesis/TransformMachines2/SetJyGridFT.h>
#include <synthesis/TransformMachines2/WProjectFT.h>
#include <synthesis/TransformMachines2/SimpleComponentFTMachine.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/casa/namespace.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/Utilities/Regex.h>
#include "MakeMS.h"

using namespace std;
using namespace casa;
using namespace casacore;
using namespace casa::refim;
using namespace casa::test;

refim::FTMachine* makeFTM(String ftmachine, MPosition loc, MDirection dir){
  if(ftmachine=="GridFT")
    return new refim::GridFT(1000000, 16, "SF", loc, 1.0, false, false);
  if(ftmachine=="SetJyGridFT")
    return new refim::SetJyGridFT(1000000, 16, "SF", loc, dir, 1.0, false, false);
  if(ftmachine=="WProjectFT")
    return new refim::WProjectFT(50, loc, 1000000, 16);
  return nullptr;

}

Int main(/*int argc, char **argv*/){
  try{
    MDirection thedir(Quantity(20.0, "deg"), Quantity(20.0, "deg"));
    String msname("Test.ms");
    MakeMS::makems(msname, thedir);
    MeasurementSet thems(msname, Table::Update);
    thems.markForDelete();
    vi::VisibilityIterator2 vi2(thems,vi::SortColumns(),true);
    vi::VisBuffer2 *vb=vi2.getVisBuffer();
    VisImagingWeight viw("natural");
    vi2.useImagingWeight(viw);
    ComponentList cl;
    SkyComponent otherPoint(ComponentType::POINT);
    otherPoint.flux() = Flux<Double>(6.66e-2, 0.0, 0.0, 0.00000);
    otherPoint.shape().setRefDirection(thedir);
    cl.add(otherPoint);
    Matrix<Double> xform(2,2);
    xform = 0.0;
    xform.diagonal() = 1.0;
    DirectionCoordinate dc(MDirection::J2000, Projection::SIN, Quantity(20.0,"deg"), Quantity(20.0, "deg"),
			   Quantity(0.5, "arcsec"), Quantity(0.5,"arcsec"),
			   xform, 50.0, 50.0, 999.0, 
			   999.0);
    Vector<Int> whichStokes(1, Stokes::I);
    StokesCoordinate stc(whichStokes);
    SpectralCoordinate spc(MFrequency::LSRK, 1.5e9, 1e6, 0.0 , 1.420405752E9);
    CoordinateSystem cs;
    cs.addCoordinate(dc); cs.addCoordinate(stc); cs.addCoordinate(spc);
    TempImage<Complex> im(IPosition(4,100,100,1,1), cs);//, "gulu.image");
    std::vector<String> ftmachines={"GridFT", "WProjectFT", "SetJyGridFT"};
    refim::FTMachine * ftm=nullptr;
    for (uInt k=0; k < ftmachines.size(); ++k){
     cerr << "########### Test " << ftmachines[k] << endl; 
      im.set(Complex(0.0));
      MPosition loc;
      MeasTable::Observatory(loc, MSColumns(thems).observation().telescopeName()(0));
      ftm=makeFTM(ftmachines[k], loc, thedir);
      Matrix<Float> weight;
      vi2.originChunks();
      vi2.origin();
      ftm->initializeToSky(im, weight, *vb);
      refim::SimpleComponentFTMachine cft;
    
      for (vi2.originChunks();vi2.moreChunks(); vi2.nextChunk()){
	for (vi2.origin(); vi2.more(); vi2.next()){
	  cft.get(*vb, cl);
	  vb->setVisCube(vb->visCubeModel());
	  ftm->put(*vb);	
	}
      }
      ftm->finalizeToSky();
      
      ftm->getImage(weight, true);
    
    //  cerr << "val at center " << im.getAt(IPosition(4, 50, 50, 0, 0)) << endl;
      AlwaysAssertExit(near(6.66e-2, real( im.getAt(IPosition(4, 50, 50, 0, 0))), 1.0e-5));
      ///Let us degrid now
      im.set(Complex(0.0));
      im.putAt(Complex(999.0, 0.0),IPosition(4, 50, 50, 0, 0) );
      vi2.originChunks();
      vi2.origin();
      ftm->initializeToVis(im, *vb);
      for (vi2.originChunks();vi2.moreChunks(); vi2.nextChunk()){
	for (vi2.origin(); vi2.more(); vi2.next()){
	  ftm->get(*vb);
	  //cerr << "mod " << (vb->visCubeModel())(0,0,0) << endl;
	  AlwaysAssertExit(near(999.0, abs((vb->visCubeModel())(0,0,0)), ftmachines[k]=="WProjectFT" ? 1e-3 : 1.0e-5));
	}
      }
    }
    //detach the ms for cleaning up
    thems=MeasurementSet();
     } catch (AipsError x) {
    cout << "Caught exception " << endl;
    cout << x.getMesg() << endl;
    return(1);
  }
  cerr <<"OK" << endl;
  exit(0);
}
