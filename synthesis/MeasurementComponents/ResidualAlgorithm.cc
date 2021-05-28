//# ResidualAlgorithm.cc:class providing for parallelized residualimagemaking
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002
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
//#  $Id$

#include <casa/Utilities/Assert.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <synthesis/MeasurementComponents/ResidualAlgorithm.h>
#include <casa/OS/Time.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/Lattices/SubLattice.h>   
#include <lattices/Lattices/TiledShape.h>  
#include <casa/System/ProgressMeter.h>
#include <synthesis/Parallel/Applicator.h>
#include <unistd.h>
#include <casa/Containers/Record.h>   

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

extern Applicator applicator;


ResidualAlgorithm::ResidualAlgorithm() : myName_p("ResidualAlgorithm")
{

};

ResidualAlgorithm::~ResidualAlgorithm()
{

};

void ResidualAlgorithm::get(){

  // Receive data from the controller process
  applicator.get(model_p);
  applicator.get(msName_p);
  applicator.get(incremental_p); 

  // Receiving image parameters
  Record container_image;
  String errorString;
  applicator.get(container_image);
  cImage_p= new TempImage<Complex>();
  cImage_p->fromRecord(errorString, container_image);

  // Receiving and making FT machine 
  Record container_ft;
  applicator.get(container_ft);
  ft_p=new GridFT(container_ft);
  Matrix<Float> weight_temp;
  applicator.get(weight_temp);

  if (weight_p) delete weight_p;
  weight_p = new Matrix<Float> (weight_temp);

};

void ResidualAlgorithm::put(){

  // Send data back to the controller process
  applicator.put(model_p);

  // Sending image parameters
  Record container_image;
  String errorString;
  cImage_p->toRecord(errorString, container_image);
  applicator.put(container_image);
  Bool withImage=false;

  // Sending FT machine parameters
  Record container_ft;
  ft_p->toRecord(errorString, container_ft, withImage);
  applicator.put(container_ft);
  applicator.put(*weight_p);

  delete ft_p;
  delete cImage_p;
  weight_p->resize();

};

String& ResidualAlgorithm::name(){

  // Return the algorithm name
  return myName_p;

};

void ResidualAlgorithm::task(){

  // Form the parallel residual image

  ms_p= new MeasurementSet(msName_p, TableLock(TableLock::UserNoReadLocking)
			   , Table::Update);
        
  Block<Int> sort(0);
  Matrix<Int> noselection;
  noselection=MeasurementSet::ANTENNA1;
  Double interval=0; 

  // Construct the VisibilityIterator
  VisSet vs(*ms_p, sort, noselection, interval);
  ROVisIter& vi(vs.iter());



  // Row blocking defines the memory used
  vi.setRowBlocking(100000);
  VisBuffer vb(vi);

  // Change the model polarization frame
  if(vb.polFrame()==MSIter::Linear) {
    StokesImageUtil::changeCStokesRep(*cImage_p,
				      StokesImageUtil::LINEAR);
  }
  else {
    StokesImageUtil::changeCStokesRep(*cImage_p,
				      StokesImageUtil::CIRCULAR);
  }

  vi.originChunks();
  vi.origin();
  initializePut(vb, model_p);
 
  // Iterate through the visibility data
  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    for (vi.origin(); vi.more(); vi++) {
	  
      vb.modelVisCube()-=vb.correctedVisCube();
      vb.setVisCube(vb.modelVisCube());
      putResidualVis(vb, model_p);
	
    }
  }

  ft_p->finalizeToSky();
      
  Matrix<Float> delta;
  cImage_p->copyData(ft_p->getImage(delta, false));
  (*weight_p)+=delta;

  ms_p->unlock();
  delete ms_p;
};

void ResidualAlgorithm::initializePut(const VisBuffer& vb, Int /*model*/) {
  ft_p->initializeToSky(*cImage_p, *weight_p,vb);
  // Need to add support for varying SkyJones
}


void ResidualAlgorithm::putResidualVis(VisBuffer& vb, Int /*model*/) {
    Bool dopsf=false;
    // Need to add support for varying SkyJones
    ft_p->put(vb, -1, dopsf);
}





} //# NAMESPACE CASA - END

