/*
 * LinearMosaic.cc
 //# Copyright (C) 2015
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
 *  Created on: Feb 24, 2015
 *      Author: kgolap
 */

#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/ImageRegrid.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/WCBox.h>
#include <synthesis/MeasurementEquations/Imager.h>
#include <synthesis/MeasurementEquations/LinearMosaic.h>
using namespace std;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


	LinearMosaic::LinearMosaic (): outImage_p(NULL), outWgt_p(NULL), outImName_p(""), outWgtName_p(""), weightType_p(1), linmosType_p(2){


	}
	LinearMosaic::LinearMosaic(const String outim, const String outwgt, const MDirection& imcen, const Int nx,
			const Int ny, const Quantity cellx, const Quantity celly, const Int linmostype) :outImage_p(NULL), outWgt_p(NULL), linmosType_p(linmostype) {
		outImName_p=outim;
		///Null image so assigning same weight type as requested
		imageWeightType_p=linmostype;
		weightType_p=linmostype;

		outWgtName_p= outwgt=="" ? outim+String(".weight") : outwgt;
		nx_p=nx;
		ny_p=ny;
		imcen_p=imcen;
		cellx_p=cellx;
		celly_p=celly;

	 }
	LinearMosaic::LinearMosaic(const String outim, const String outwgt, const MDirection& imcen, const Int nx, const Int ny,
				   Vector<CountedPtr<ImageInterface<Float> > >& ims,
				   Vector<CountedPtr<ImageInterface<Float> > >& wgtims, const Int linmostype) : linmosType_p(linmostype){
		Int nchan=ims[0]->shape()[3];
		Int npol=ims[0]->shape()[2];
		for (uInt k=0; k< ims.nelements(); ++k){
			if(nchan != ims[k]->shape()[3] || nchan != wgtims[k]->shape()[3])
				ThrowCc("images should have the same number of channels");
			if(npol != ims[k]->shape()[2] || npol != (wgtims[k])->shape()[2])
				ThrowCc("images should have the same number of polarization planes");
		}
		CoordinateSystem cs=ims[0]->coordinates();
		makeEmptyImage(outim, cs, imcen, nx, ny,npol, nchan);
		makeEmptyImage(outwgt, cs, imcen, nx, ny,npol, nchan);
		PagedImage<Float> outdiskim(outim);
		PagedImage<Float> outdiskwgt(outwgt);
		setOutImages(outdiskim, outdiskwgt, 2, 2);
		makeMosaic(ims, wgtims);

	}
	void LinearMosaic::setlinmostype(const Int linmostype){
		linmosType_p=linmostype;
	}
	Bool LinearMosaic::makeMosaic(ImageInterface<Float>& outim, ImageInterface<Float>& outwgt,
				      Vector<CountedPtr<ImageInterface<Float> > >& ims,
				      Vector<CountedPtr<ImageInterface<Float> > >& wgtims){

		Bool retval=true;
		if(ims.nelements() != wgtims.nelements())
			ThrowCc("Unequal number of images and weight images ");
		for (uInt k=0; k < ims.nelements(); ++k){
			retval=retval && addOnToImage(outim, outwgt, *(ims[k]), *(wgtims[k]), (k==ims.nelements()-1));
		}
		return retval;

	}
	Bool LinearMosaic::makeMosaic(
				  	  Vector<CountedPtr<ImageInterface<Float> > >& ims,
				  	  Vector<CountedPtr<ImageInterface<Float> > >& wgtims){

			Bool retval=true;
			if(outImage_p.null() && outImName_p=="")
				ThrowCc("No output image or weight image defined");
			if(outImage_p.null())
				createOutImages(ims[0]->coordinates(), ims[0]->shape()[2], ims[0]->shape()[3] );
			if(ims.nelements() != wgtims.nelements())
				ThrowCc("Unequal number of images and weight images ");
			for (uInt k=0; k < ims.nelements(); ++k){
				retval=retval && addOnToImage(*outImage_p, *outWgt_p, *(ims[k]), *(wgtims[k]), (k==ims.nelements()-1));
				ims[k]=0;
				wgtims[k]=0;
			}
			return retval;

		}
	void LinearMosaic::setOutImages(ImageInterface<Float>& outim, ImageInterface<Float>& outwgt, const Int imageWeightType, const Int weightType){
		outImage_p=CountedPtr<ImageInterface<Float> >(&outim, false);
		outWgt_p = CountedPtr<ImageInterface<Float> >(&outwgt, false);
		imageWeightType_p=imageWeightType;
		weightType_p=weightType;
	}
	void LinearMosaic::setOutImages(const String& outim, const String& outwgt, const Int imageWeightType, const Int weightType){
			outImage_p=new PagedImage<Float>(outim);
			outWgt_p = new PagedImage<Float>(outwgt);
			imageWeightType_p=imageWeightType;
			weightType_p=weightType;
	}
	Bool LinearMosaic::addOnToImage(ImageInterface<Float>& outim, ImageInterface<Float>& outwgt, const ImageInterface<Float>& inIm,
			  const ImageInterface<Float>& inWgt, Bool unWeightOutIm){

		if(inIm.shape() != inWgt.shape())
		  ThrowCc("Image "+ inIm.name() + " and Weight image "+inWgt.name() +". have to be similar...please regrid appropriately");
		Double meminMB=Double(HostInfo::memoryTotal(true))/1024.0;

		//if(!outImIsWeighted){
		{
		  // cerr << "limosType " << linmosType_p << " imageWeightType " << imageWeightType_p << " weightType " << weightType_p << endl;
			if(linmosType_p==2){
			  if(weightType_p==1)
			    outwgt.copyData((LatticeExpr<Float>)(outwgt*outwgt));
			  weightType_p=2;
			  if(imageWeightType_p==1)
			    outim.copyData((LatticeExpr<Float>)(outim*sqrt(outwgt)));
			  else if( imageWeightType_p==0)
			    outim.copyData((LatticeExpr<Float>)(outim*outwgt));
			  imageWeightType_p=2;
			    
			}
			//cerr << " imageWeightType " << imageWeightType_p << " weightType " << weightType_p << endl;
			if(linmosType_p==1){
			  	if(weightType_p==2)
					outwgt.copyData((LatticeExpr<Float>)(sqrt(abs(outwgt))));
				weightType_p=1;
				if(imageWeightType_p==0)
					outim.copyData((LatticeExpr<Float>)(outim*outwgt));
				else if( imageWeightType_p==2)
					outim.copyData((LatticeExpr<Float>)(iif(outwgt > (0.0),
						       (outim/outwgt), 0)));
				imageWeightType_p=1;
			       
			}
		}

		CoordinateSystem incs=inIm.coordinates();



		IPosition iblc(inIm.shape().nelements(),0);
		IPosition itrc=inIm.shape();
		//		cerr << "itrc " << itrc << endl;
		itrc -=1;



		//cerr << "blc " << iblc << " trc " << itrc << endl;
		LCBox lbox(iblc, itrc, inIm.shape());
		WCBox wbox(lbox, incs);
		//cerr << "wbox " << wbox.toRecord("") << endl;
		ImageRegion imagreg(wbox );
		SubImage<Float> subOutIm;
		SubImage<Float> subOutWgt;
		try{
	       	
		  subOutIm=SubImage<Float>(outim, imagreg, true);
		  subOutWgt=SubImage<Float>(outwgt, imagreg, true);
		}
		catch(...){
			//Failed to make a subimage let us use the full image
		  //cerr << "Failed to make subImage " << x.what()<< endl;
			subOutIm=SubImage<Float>(outim, true);
			subOutWgt=SubImage<Float>(outwgt, true);

		}
		TempImage<Float> fullImage(subOutWgt.shape(), subOutIm.coordinates(), meminMB/8.0);
		TempImage<Float> fullWeight(subOutWgt.shape(), subOutIm.coordinates(), meminMB/8.0);
		fullImage.set(0.0);
		fullWeight.set(0.0);
		ImageRegrid<Float> regridder;
		{

			if(linmosType_p==2){
				TempImage<Float> trueWeightIm(inWgt.shape(), inWgt.coordinates(), meminMB/8.0);
				if(inWgt.getDefaultMask() != ""){
				  Imager::copyMask(trueWeightIm, inWgt,  inWgt.getDefaultMask());
				  fullWeight.makeMask(inWgt.getDefaultMask(), true, true, true, true);
				}
				trueWeightIm.copyData((LatticeExpr<Float>)(inWgt*inWgt));
				
				regridder.regrid( fullWeight, Interpolate2D::LINEAR,
						IPosition(2,0,1), trueWeightIm);
				TempImage<Float> inWeightedIm(inIm.shape(), inIm.coordinates(), meminMB/8.0);	
				if(inIm.getDefaultMask() != ""){
				  Imager::copyMask(inWeightedIm, inIm,  inIm.getDefaultMask());
				  fullImage.makeMask(inIm.getDefaultMask(), true, true, true, true);
				}
				
				inWeightedIm.copyData((LatticeExpr<Float>)(inIm*inWgt));
				ImageRegrid<Float> regridder;
				regridder.regrid( fullImage, Interpolate2D::LINEAR,
						IPosition(2,0,1), inWeightedIm);

			}
			else if (linmosType_p==1){
			  if(inIm.getDefaultMask() != ""){
				  fullImage.makeMask(inIm.getDefaultMask(), true, true, true, true);
			  }
			  if(inWgt.getDefaultMask() != ""){
			    fullWeight.makeMask(inWgt.getDefaultMask(), true, true, true, true);
			  }
				regridder.regrid( fullWeight, Interpolate2D::LINEAR,
										IPosition(2,0,1), inWgt);
				regridder.regrid( fullImage, Interpolate2D::LINEAR,
										IPosition(2,0,1), inIm);
			}
		}

		subOutWgt.copyData((LatticeExpr<Float>)(subOutWgt+fullWeight));
		
		subOutIm.copyData((LatticeExpr<Float>)(subOutIm+fullImage));
		//if(wMax > 0.0){
		//	outim.copyData((LatticeExpr<Float>)(outim/wMax));
		//	outwgt.copyData((LatticeExpr<Float>)(outwgt/wMax));
		//}
		if(unWeightOutIm){
		  //LatticeExprNode outmax0 = max( outim );
		  
		  //cerr << "Bef max of out " << outmax0.getFloat() << endl;
		  
		  LatticeExprNode elmax = max( outwgt );
		  
		  //Cannot trust anything below 1e-6
		  Float wMinLimit =  1e-6* elmax.getFloat();
		  outim.copyData((LatticeExpr<Float>)(iif(outwgt > (wMinLimit),
						       (outim/outwgt), 0)));
		  //LatticeExprNode outmax = max( outim );
		  
		  //cerr << "max of out " << outmax.getFloat() << endl;
			imageWeightType_p=0;
		}

		return true;
	}


	void LinearMosaic::saultWeightImage(const String& outimname, const Float& fracPeakWgt){
		if(outImage_p.null())
			ThrowCc("Mosaic image and weight must be set");
		PagedImage<Float> outdiskim(outImage_p->shape(), outImage_p->coordinates(), outimname);
		LatticeExprNode elmax = max( *outWgt_p );
		Float wMax =  elmax.getFloat();
		wMax *=fracPeakWgt;
		LatticeExpr<Float> weightMath;
		if(imageWeightType_p==0){
			if(weightType_p==2)
				weightMath=(LatticeExpr<Float>)(iif((*outWgt_p) > (wMax),
									       (*outImage_p), (*outImage_p)*(*outWgt_p)));
			else if(weightType_p==1)
				weightMath=(LatticeExpr<Float>)(iif((*outWgt_p) > (wMax),
													       (*outImage_p), (*outImage_p)*(*outWgt_p)*(*outWgt_p)));
		}
		if(imageWeightType_p==1){
			if( weightType_p==2)
				weightMath=(LatticeExpr<Float>)(iif((*outWgt_p) > (wMax),
													       (*outImage_p)/sqrt(*outWgt_p), (*outImage_p)*sqrt(*outWgt_p)));
			else if(weightType_p==1)
				weightMath=(LatticeExpr<Float>)(iif((*outWgt_p) > (wMax),
																	       (*outImage_p)/(*outWgt_p), (*outImage_p)*(*outWgt_p)));
		}
		if(imageWeightType_p==2){
					if( weightType_p==2)
						weightMath=(LatticeExpr<Float>)(iif((*outWgt_p) > (wMax),
															       (*outImage_p)/(*outWgt_p), (*outImage_p)));
					else if(weightType_p==1)
						weightMath=(LatticeExpr<Float>)(iif((*outWgt_p) > (wMax),
																			       (*outImage_p)/((*outWgt_p)*(*outWgt_p)), (*outImage_p)));
		}

		outdiskim.copyData(weightMath);


	}
	void LinearMosaic::createOutImages(const CoordinateSystem& cs, const Int npol, const Int nchan ){
		makeEmptyImage(outImName_p, cs, imcen_p, nx_p, ny_p, npol, nchan );
		makeEmptyImage(outWgtName_p, cs, imcen_p, nx_p, ny_p, npol, nchan );
		outImage_p=new PagedImage<Float>(outImName_p);
		outWgt_p= new PagedImage<Float>(outWgtName_p);
	}
	void LinearMosaic::makeEmptyImage(const String imagename, const CoordinateSystem& cs, const MDirection& imcen, const Int nx, const Int ny,
				const Int npol, const Int nchan){
		CoordinateSystem outcs=cs;
		Int dirAx=outcs.findCoordinate(Coordinate::DIRECTION);
		DirectionCoordinate dc=outcs.directionCoordinate(dirAx);
		Vector<Double> incr=dc.increment();
		String elunit=dc.worldAxisUnits()[0];
		if(cellx_p.getValue() !=  0.0)
			incr[0]=-cellx_p.getValue(elunit);
		if(celly_p.getValue() != 0.0)
			incr[1]=celly_p.getValue(elunit);
		dc.setIncrement(incr);
		Vector<Double> cenVec(2);
		cenVec[0]=Double(nx)/2.0; cenVec[1]=Double(ny)/2.0;
		dc.setReferencePixel(cenVec);
		MDirection::Types eltype;
		MDirection::getType(eltype, imcen.getRefString());
		dc.setReferenceValue(imcen.getAngle().getValue(elunit));
		dc.setReferenceFrame(eltype);
		outcs.replaceCoordinate(dc, dirAx);
		PagedImage<Float> outdiskim(IPosition(4, nx, ny, npol, nchan), outcs, imagename);
		outdiskim.set(0.0);
	}

using namespace casacore;
} //end namespace casa
