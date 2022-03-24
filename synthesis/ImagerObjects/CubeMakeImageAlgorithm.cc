//# CubeMajorCycleAlgorithm.cc: implementation of class to grid cube in parallel/serial 
//# Copyright (C) 2019
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//# License for more details.
//#
//# https://www.gnu.org/licenses/
//#
//# Queries concerning CASA should be submitted at
//#        https://help.nrao.edu
//#
//#        Postal address: CASA Project Manager 
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#include <casacore/lattices/Lattices/LatticeLocker.h>
#include <synthesis/ImagerObjects/CubeMakeImageAlgorithm.h>
#include <synthesis/ImagerObjects/SynthesisImagerVi2.h>
#include <casacore/casa/Containers/Record.h>
#include <synthesis/ImagerObjects/SimpleSIImageStore.h>
#include <imageanalysis/Utilities/SpectralImageUtil.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
extern Applicator applicator;

CubeMakeImageAlgorithm::CubeMakeImageAlgorithm() : myName_p("CubeMakeImageAlgorithm"), polRep_p(0), status_p(False){
	
}
CubeMakeImageAlgorithm::~CubeMakeImageAlgorithm() {
	
}
	
void CubeMakeImageAlgorithm::get() {

	//cerr << "in get for child process " << applicator.isWorker() << endl;
	Record imParsRec;
	Record vecSelParsRec;
	Record gridParsRec;
	// get data sel params #1
	applicator.get(vecSelParsRec);
	// get image sel params #2
	applicator.get(imParsRec);
	// get gridders params #3
	applicator.get(gridParsRec);
	// get which channel to process #4
	applicator.get(chanRange_p);
	cerr <<"GET chanRange " << chanRange_p << endl;
	// which type of image #5
	applicator.get(imageType_p);
	// weight params #6
	applicator.get(weightParams_p);
	// output complex_image name #7
	applicator.get(cimageName_p);
	//Somewhere before this we have to make sure that vecSelPars has more than 0 fields)
	dataSel_p.resize(vecSelParsRec.nfields());
	/// Fill the private variables
	for (Int k=0; k < Int(dataSel_p.nelements()); ++k){
		(dataSel_p[k]).fromRecord(vecSelParsRec.asRecord(String::toString(k)));
		//cerr << k << "datasel " << vecSelParsRec.asRecord(String::toString(k)) << endl;
	}
	//cerr << "GET record " << imParsRec << endl;
	imSel_p.fromRecord(imParsRec);
	gridSel_p.fromRecord(gridParsRec);
	if(gridParsRec.isDefined("ftmachine")){
		ftmRec_p=gridParsRec.asRecord("ftmachine");
	}
			
	
	
	
}
void CubeMakeImageAlgorithm::put() {
	
	
	//cerr << "in put " << status_p << endl;
	applicator.put(status_p);	
	
}
	
void CubeMakeImageAlgorithm::task(){
	status_p = False;
	//SynthesisImagerVi2 imgr;
	//imgr.selectData(dataSel_p);
	// We do not use chanchunking in this model
	gridSel_p.chanchunks = 1;

	//imgr.defineImage(imSel_p,gridSel_p);
	// need to find how many subfields/outliers have been set
	//CountedPtr<SIImageStore> imstor =imgr.imageStore(0);
	//CountedPtr<ImageInterface<Float> > resid=imstor->residual();
	//Int nchan = resid->shape()(3);
	//std::shared_ptr<SIImageStore> subImStor=imstor->getSubImageStore(0, 1, chanId_p, nchan, 0,1);
	
	SynthesisImagerVi2 subImgr;
	for (Int k=0; k < Int(dataSel_p.nelements()); ++k){
		//model is already cleared in original SythesisImagerVi2
		dataSel_p[k].incrmodel=True;
		subImgr.selectData(dataSel_p[k]);
	}
	subImgr.defineImage(imSel_p, gridSel_p);
	// TO DO get weight param and set weight
	if(!weightParams_p.isDefined("type") || weightParams_p.asString("type")=="natural")
		subImgr.weight("natural");
	else
		subImgr.weight(weightParams_p);
	
	//get the vi and ft machine
	CountedPtr<vi::VisibilityIterator2> vi=subImgr.getVi();
	CountedPtr<refim::FTMachine> ift=subImgr.getFTM(0);
	refim::FTMachine::Type seType=(refim::FTMachine::Type)imageType_p;
	
	if(seType >= refim::FTMachine::N_types || seType < refim::FTMachine::OBSERVED)
		throw(AipsError("CubeMakeImage error: Donot know how to make type of image requested"));
	PagedImage<Complex> origCImage(cimageName_p, TableLock::UserNoReadLocking);
	shared_ptr<ImageInterface<Complex> > subCimage;
	subCimage.reset(SpectralImageUtil::getChannel(origCImage, chanRange_p[0], chanRange_p[1], true));
	Matrix<Float> weight;
	ift->makeImage(seType, *vi, *subCimage, weight);
	status_p = True;
}
String&	CubeMakeImageAlgorithm::name(){
	return myName_p;
}


	
	
	
} //# NAMESPACE CASA - END
