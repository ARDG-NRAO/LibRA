//# PredictAlgorithm.cc:  class providing for parallelized predict methods
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
//# $Id$

#include <casacore/casa/Utilities/Assert.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MatrixMath.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <synthesis/MeasurementComponents/PredictAlgorithm.h>
#include <casacore/casa/OS/Time.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/casa/System/ProgressMeter.h>
#include <synthesis/Parallel/Applicator.h>
#include <unistd.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/lattices/Lattices/SubLattice.h>   
#include <casacore/lattices/Lattices/TiledShape.h>  

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

extern Applicator applicator;

PredictAlgorithm::PredictAlgorithm() : myName_p("PredictAlgorithm")
{
};

PredictAlgorithm::~PredictAlgorithm()
{
};

void PredictAlgorithm::get(){

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
};

void PredictAlgorithm::put(){

  // Send data back to the controller process
  applicator.put(model_p);

  // Sending FT machine parameters
  String errorString;
  Record container_ft;
  ft_p->toRecord(errorString, container_ft);
  applicator.put(container_ft);
  delete ft_p;
  delete cImage_p;

};

String& PredictAlgorithm::name(){

  // Return the algorithm name
  return myName_p;

};

void PredictAlgorithm::task(){
  
  // Predict the model visibilities in parallel
  ms_p= new MeasurementSet(msName_p, TableLock(TableLock::UserNoReadLocking),
			   Table::Update);
        
  Block<Int> sort(0);
  Matrix<Int> noselection;
  noselection=MeasurementSet::ANTENNA1;
  Double interval=0; 
  ms_p->unlock(); 

  // Construct the VisibilityIterator
  VisSet vs(*ms_p, sort, noselection, interval);
  VisIter& vi(vs.iter());
  // Row blocking defines the memory used
  vi.setRowBlocking(200000); 
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
      

  // Initialize get (i.e. Transform from Sky)
  vi.originChunks();
  vi.origin();      
  initializeGet(vi, 0, model_p, incremental_p);


  Int chunkNo=0;
  Int intraChunk=0;
  // Iterate through the visibility data

  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    chunkNo++;
    for (vi.origin(); vi.more(); vi++) {  
      intraChunk++;     
      if(!incremental_p && model_p==0){ 
	vb.setModelVisCube(Complex(0.0,0.0));
	ms_p->lock();
	vi.setVis(vb.modelVisCube(),VisibilityIterator::Model);
	ms_p->unlock();
      }
      getModelVis(vb,model_p,incremental_p);
	
      vi.setVis(vb.modelVisCube(),VisibilityIterator::Model);
      ms_p->flush(true);
      ms_p->unlock(); // this is the unlock for the lock in getModelVis

    }
        
  }
      
  ms_p->unlock();
  delete ms_p; 

};

void PredictAlgorithm::initializeGet(const VisBuffer& vb, Int /*row*/, Int /*model*/,
				     Bool /*incremental*/) {

  // Need to add support for varying SkyJones
  ft_p->initializeToVis(*cImage_p, vb);
}

VisBuffer& PredictAlgorithm::getModelVis(VisBuffer& result, Int /*model*/,
                                         Bool /*incremental*/) {
  
  // Need to add support for varying SkyJones

  //UNUSED: Int nRow=result.nRow();
  result.modelVisCube(); // get the visibility so vb will have it
  VisBuffer vb(result);

  ft_p->get(vb);
  ms_p->lock(FileLocker::Write);
  result.invalidate();// reget it in case it has changed in the mean time
  // important as other processors might have changed the values
  result.modelVisCube();
  result.modelVisCube()+=vb.modelVisCube();   
  return result;
}









} //# NAMESPACE CASA - END

