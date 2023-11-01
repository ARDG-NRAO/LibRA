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
#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines/GridFT.h>
#include <synthesis/TransformMachines/SimpleComponentFTMachine.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/namespace.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/Utilities/Regex.h>
#include "MakeMS.h"

using namespace std;
using namespace casa;
using namespace casacore;
using namespace casa::test;

Int main(/*int argc, char **argv*/){

  MDirection thedir(Quantity(20.0, "deg"), Quantity(20.0, "deg"));
  String msname("Test.ms");
  MakeMS::makems(msname, thedir);
  MeasurementSet thems(msname, Table::Update);
  thems.markForDelete();
  Block<Int> sort;
  VisibilityIterator vi(thems,sort,0.0);
  VisBuffer vb(vi);
  VisImagingWeight viw("natural");
  vi.useImagingWeight(viw);
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
   TempImage<Complex> im(IPosition(4,100,100,1,1), cs); //, "gulu0.image");
   im.set(Complex(0.0));
   MPosition loc;
   MeasTable::Observatory(loc, MSColumns(thems).observation().telescopeName()(0));
   GridFT ftm(1000000, 16, "SF", loc, 1.0, false, false);
   Matrix<Float> weight;
   vi.originChunks();
   vi.origin();
   ftm.initializeToSky(im, weight, vb);
   SimpleComponentFTMachine cft;
  
   for (vi.originChunks();vi.moreChunks(); vi.nextChunk()){
     for (vi.origin(); vi.more(); vi++){
       cft.get(vb, cl);
       vb.setVisCube(vb.modelVisCube());
       ftm.put(vb);
     }
   }

   ftm.getImage(weight, true);
   ftm.finalizeToSky();
   //cerr << "val at center " << real(im.getAt(IPosition(4, 50, 50, 0, 0))) - 6.66e-2 << endl;
   AlwaysAssertExit(near(6.66e-2, real( im.getAt(IPosition(4, 50, 50, 0, 0))), 1.0e-5));

   exit(0);
}
