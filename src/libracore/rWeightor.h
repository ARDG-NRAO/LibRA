// -*- C++ -*-
//# rWeightor.h: Definition of the global rWeightor function
//#     that implements various imaging weighting schemes
//# Copyright (C) 2021
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
//# Correspondence concerning this should be addressed as follows:
//#        Postal address: National Radio Astronomy Observatory
//#                        1003 Lopezville Road,
//#                        Socorro, NM - 87801, USA
//#
//# $Id$

#ifndef LIBRACORE_RWEIGHTOR_H
#define LIBRACORE_RWEIGHTOR_H

//
// Embedded code in roadrunner.cc put in this file to minimize visual
// overload by code that should not require attention (or even deeper
// understanding). This is code extract from CASA systhesis module
// into a stand-alone function to reduce the dependency graph.
//
#include <casacore/casa/namespace.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <synthesis/TransformMachines2/FTMachine.h> //casa::refim namespace

//class casa::VisImagingWeight;
//casa::VisImagingWeight
using namespace casa;
using namespace casa::refim;
using namespace casacore;
/**
 * @fn void Weightor(casacore::MeasurementSet& ms, const std::string& dataColumnName, const std::string& weighting, const std::string& rmode, const float& robust, const std::string& uvDistStr)
 * @brief Applies weighting to the data in a MeasurementSet.
 * @param ms A reference to the MeasurementSet.
 * @param dataColumnName The name of the data column.
 * @param weighting The weighting scheme.
 * @param rmode The robustness mode.
 * @param robust The robustness parameter.
 * @param uvDistStr The UV distance string.
 */
inline void
weightor(vi::VisibilityIterator2& vi_p,
	 const CoordinateSystem& itsMaxCoordSys, // CSys of the sky image
	 const IPosition& itsMaxShape,           // X-Y shape of the sky image
	 const casacore::String& type="natural", 
	 const casacore::String& rmode="norm",
	 const casacore::Quantity& noise=casacore::Quantity(0.0, "Jy"), 
	 const casacore::Double robust=0.0,
	 const casacore::Quantity& fieldofview=casacore::Quantity(0.0, "arcsec"),
	 const casacore::Int npixels=0, 
	 const casacore::Bool multiField=false,
	 //const casacore::Bool usecubebriggs=false,
	 const casacore::String& filtertype=casacore::String("Gaussian"),
	 const casacore::Quantity& filterbmaj=casacore::Quantity(0.0,"deg"),
	 const casacore::Quantity& filterbmin=casacore::Quantity(0.0,"deg"),
	 const casacore::Quantity& filterbpa=casacore::Quantity(0.0,"deg")  )
 // weightor(
 // 	 const String& type, const String& rmode,
 // 	 const Quantity& noise, const Double robust,
 // 	 const Quantity& fieldofview,
 // 	 const Int npixels, const Bool multiField, const Bool useCubeBriggs,
 // 	 const String& filtertype, const Quantity& filterbmaj,
 // 	 const Quantity& filterbmin, const Quantity& filterbpa   )
{
  LogIO os(LogOrigin("roadrunner", "weightor()", WHERE));
  casa::VisImagingWeight imwgt_p;
  try {
    //Int nx=itsMaxShape[0];
    //Int ny=itsMaxShape[1];
    
    //os << "GET IMAGE PARAMETERS" << LogIO::EXCEPTION;
    
    Quantity cellx=Quantity(itsMaxCoordSys.increment()[0], itsMaxCoordSys.worldAxisUnits()[0]);
    Quantity celly=Quantity(itsMaxCoordSys.increment()[1], itsMaxCoordSys.worldAxisUnits()[1]);
    os << LogIO::NORMAL // Loglevel INFO
       << "Set imaging weights : " ; //<< LogIO::POST;
    
    if (type=="natural") {
      os << LogIO::NORMAL // Loglevel INFO
	 << "Natural weighting" << LogIO::POST;
      imwgt_p=VisImagingWeight("natural");
    }
    else if (type=="radial") {
      os << "Radial weighting" << LogIO::POST;
      imwgt_p=VisImagingWeight("radial");
    }
    else{
      os << LogIO::NORMAL // Loglevel INFO
	 << type << " weighting" << LogIO::POST;
      vi_p.originChunks();
      vi_p.origin();
      
      //os << "GET IMAGE PARAMETERS" << LogIO::EXCEPTION;
      // if(!imageDefined_p)
      // 	  throw(AipsError("Need to define image"));
      Int nx=itsMaxShape[0];
      Int ny=itsMaxShape[1];
      Quantity cellx=Quantity(itsMaxCoordSys.increment()[0], itsMaxCoordSys.worldAxisUnits()[0]);
      Quantity celly=Quantity(itsMaxCoordSys.increment()[1], itsMaxCoordSys.worldAxisUnits()[1]);
      
      if(type=="superuniform"){
	//    		  if(!imageDefined_p) throw(AipsError("Please define image first"));
	Int actualNpix=npixels;
	if(actualNpix <=0)
	  actualNpix=3;
	os << LogIO::NORMAL // Loglevel INFO
	   << "SuperUniform weighting over a square cell spanning ["
	   << -actualNpix
	   << ", " << actualNpix << "] in the uv plane" << LogIO::POST;
	imwgt_p=VisImagingWeight(vi_p, rmode, noise, robust, nx,
				 ny, cellx, celly, actualNpix,
				 actualNpix, multiField);
      }
      else if ((type=="robust")||(type=="uniform")||(type=="briggs")) {
	//   		  if(!imageDefined_p) throw(AipsError("Please define image first"));
	Quantity actualFieldOfView_x(fieldofview), actualFieldOfView_y(fieldofview) ;
	Int actualNPixels_x(npixels),actualNPixels_y(npixels) ;
	String wtype;
	if(type=="briggs") {
	  wtype = "Briggs";
	}
	else {
	  wtype = "Uniform";
	}
	if(actualFieldOfView_x.get().getValue()==0.0&&actualNPixels_x==0) {
	  actualNPixels_x=nx;
	  actualFieldOfView_x=Quantity(actualNPixels_x*cellx.get("rad").getValue(),"rad");
	  actualNPixels_y=ny;
	  actualFieldOfView_y=Quantity(actualNPixels_y*celly.get("rad").getValue(),"rad");
	  os << LogIO::NORMAL // Loglevel INFO
	     << wtype
	     << " weighting: sidelobes will be suppressed over full image"
	     << LogIO::POST;
	}
	else if(actualFieldOfView_x.get().getValue()>0.0&&actualNPixels_x==0) {
	  actualNPixels_x=nx;
	  actualNPixels_y=ny;
	  os << LogIO::NORMAL // Loglevel INFO
	     << wtype
	     << " weighting: sidelobes will be suppressed over specified field of view: "
	     << actualFieldOfView_x.get("arcsec").getValue() << " arcsec by " 
	     << actualFieldOfView_y.get("arcsec").getValue()  << " arcsec" << LogIO::POST;
	}
	else if(actualFieldOfView_x.get().getValue()==0.0&&actualNPixels_x>0) {
	  actualFieldOfView_x=Quantity(actualNPixels_x*cellx.get("rad").getValue(),"rad");
	  actualFieldOfView_y=Quantity(actualNPixels_y*celly.get("rad").getValue(),"rad");
	  os << LogIO::NORMAL // Loglevel INFO
	     << wtype
	     << " weighting: sidelobes will be suppressed over full image field of view: "
	     << actualFieldOfView_x.get("arcsec").getValue() << " arcsec by " 
	     << actualFieldOfView_y.get("arcsec").getValue() << " arcsec" << LogIO::POST;
	}
	else {
	  os << LogIO::NORMAL // Loglevel INFO
	     << wtype
	     << " weighting: sidelobes will be suppressed over specified field of view: "
	     << actualFieldOfView_x.get("arcsec").getValue() << " arcsec by " 
	     << actualFieldOfView_y.get("arcsec").getValue() << " arcsec" << LogIO::POST;
	}
	os << LogIO::DEBUG1
	   << "Weighting used " << actualNPixels_x << " by " << actualNPixels_y << " uv pixels."
	   << LogIO::POST;
	Quantity actualCellSize_x(actualFieldOfView_x.get("rad").getValue()/actualNPixels_x, "rad");
	Quantity actualCellSize_y(actualFieldOfView_y.get("rad").getValue()/actualNPixels_y, "rad");
	
	
	//		  cerr << "rmode " << rmode << " noise " << noise << " robust " << robust << " npixels " << actualNPixels << " cellsize " << actualCellSize << " multifield " << multiField << endl;
	//		  Timer timer;
	//timer.mark();
	//Construct imwgt_p with old vi for now if old vi is in use as constructing with vi2 is slower
	//Determine if any image is cube
	
	// if(isSpectralCube() && useCubeBriggs){
	//   String outstr=String("Doing spectral cube Briggs weighting formula --  " + rmode + (rmode=="abs" ? " with estimated noise "+ String::toString(noise.getValue())+noise.getUnit()  : "")); 
	//   os << outstr << LogIO::POST;
	//   //VisImagingWeight nat("natural");
	//   //vi_p.useImagingWeight(nat);
	//   if(rmode=="abs" && robust==0.0 && noise.getValue()==0.0)
	//     throw(AipsError("Absolute Briggs formula does not allow for robust 0 and estimated noise per visibility 0"));
	//   CountedPtr<refim::BriggsCubeWeightor> bwgt=new refim::BriggsCubeWeightor(wtype=="Uniform" ? "none" : rmode, noise, robust, npixels, multiField);
	//   for (Int k=0; k < itsMappers.nMappers(); ++k){
	//     itsMappers.getFTM2(k)->setBriggsCubeWeight(bwgt);
	
	//   }
	// }
	// else
	{
	  imwgt_p=VisImagingWeight(vi_p, wtype=="Uniform" ? "none" : rmode, noise, robust,
				   actualNPixels_x, actualNPixels_y, actualCellSize_x,
				   actualCellSize_y, 0, 0, multiField);
	}
	
	/*
	  if(rvi_p !=NULL){
	  imwgt_p=VisImagingWeight(*rvi_p, rmode, noise, robust,
	  actualNPixels, actualNPixels, actualCellSize,
	  actualCellSize, 0, 0, multiField);
	  }
	  else{
	  ////This is slower by orders of magnitude as of 2014/06/25
	  imwgt_p=VisImagingWeight(*vi_p, rmode, noise, robust,
	  actualNPixels, actualNPixels, actualCellSize,
	  actualCellSize, 0, 0, multiField);
	  }
	*/
	//timer.show("After making visweight ");
	
      }
      else {
	//this->unlock();
	os << LogIO::SEVERE << "Unknown weighting " << type
	   << LogIO::EXCEPTION;
	//return false;
      }
    }
    
    //// UV-Tapering
    //cout << "Taper type : " << filtertype << " : " << (filtertype=="gaussian") <<  endl;
    if( filtertype == "gaussian" ) {
      //	   os << "Setting uv-taper" << LogIO::POST;
      imwgt_p.setFilter( filtertype,  filterbmaj, filterbmin, filterbpa );
    }
    vi_p.useImagingWeight(imwgt_p);
    ///////////////////////////////
    
    SynthesisUtilMethods::getResource("Set Weighting");
    
    ///	 return true;
    
  }
  catch(AipsError &x)
    {
      throw( AipsError("Error in Weighting : "+x.getMesg()) );
    }
  
  //  return imwgt_p;
}
#endif
