//# SIImageStore.cc: Implementation of Imager.h
//# Copyright (C) 1997-2008
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

#include <casa/Exceptions/Error.h>
#include <casa/iostream.h>
#include <casa/sstream.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>

#include <casa/Logging.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>
#include <casa/OS/DirectoryIterator.h>
#include <casa/OS/File.h>
#include <casa/OS/Path.h>
#include <lattices/Lattices/LatticeLocker.h>
#include <casa/OS/HostInfo.h>
#include <components/ComponentModels/GaussianDeconvolver.h>
#include <images/Images/TempImage.h>
#include <images/Images/PagedImage.h>
#include <imageanalysis/ImageAnalysis/CasaImageBeamSet.h>
#include <lattices/LatticeMath/LatticeMathUtil.h>
#include <ms/MeasurementSets/MSHistoryHandler.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <tables/Tables/TableUtil.h>

#include <synthesis/ImagerObjects/SIImageStore.h>
#include <synthesis/ImagerObjects/SDMaskHandler.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <images/Images/ImageRegrid.h>
#include <imageanalysis/ImageAnalysis/ImageStatsCalculator.h>

//#include <imageanalysis/ImageAnalysis/ImageMaskedPixelReplacer.h>

#include <sys/types.h>
#include <unistd.h>
using namespace std;

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

  //
  //===========================================================================
  // Global method that I (SB) could make work in SynthesisUtilsMethods.
  //
  template <class T>
  void openImage(const String& imagenamefull,std::shared_ptr<ImageInterface<T> >& imPtr )
  {
    LogIO logIO ( LogOrigin("SynthesisImager","openImage(name)") );
    try
      {
	if (Table::isReadable(imagenamefull))
	  imPtr.reset( new PagedImage<T>( imagenamefull ) );
      }
    catch (AipsError &x)
      {
	logIO << "Error in reading image \"" << imagenamefull << "\"" << LogIO::EXCEPTION;
      }
  }
  //
  //--------------------------------------------------------------
  //
  template 
  void openImage(const String& imagenamefull, std::shared_ptr<ImageInterface<Float> >& img );
  template 
  void openImage(const String& imagenamefull, std::shared_ptr<ImageInterface<Complex> >& img );
  //
  //===========================================================================


  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  /////       START of SIIMAGESTORE implementation
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  
  SIImageStore::SIImageStore()
  {
      
    itsPsf.reset( );
    itsModel.reset( );
    itsResidual.reset( );
    itsWeight.reset( );
    itsImage.reset( );
    itsMask.reset( );
    itsGridWt.reset( );
    itsPB.reset( );
    itsImagePBcor.reset();
    itsTempWorkIm.reset();

    itsSumWt.reset( );
    itsOverWrite=False;
    itsUseWeight=False;
    itsPBScaleFactor=1.0;

    itsNFacets=1;
    itsFacetId=0;
    itsNChanChunks = 1;
    itsChanId = 0;
    itsNPolChunks = 1;
    itsPolId = 0;

    itsImageShape=IPosition(4,0,0,0,0);
    itsImageName=String("");
    itsCoordSys=CoordinateSystem();
    itsMiscInfo=Record();
    init();
    
    
    //    validate();

  }

  // Used from SynthesisNormalizer::makeImageStore()
  SIImageStore::SIImageStore(const String &imagename,
			     const CoordinateSystem &imcoordsys,
			     const IPosition &imshape,
                             const String &objectname,
                             const Record &miscinfo,
			     //	const Int nfacets,
			     const Bool /*overwrite*/,
			     const Bool useweightimage)
  // TODO : Add parameter to indicate weight image shape. 
  {
    LogIO os( LogOrigin("SIImageStore","Open new Images",WHERE) );
      

    itsPsf.reset( );
    itsModel.reset( );
    itsResidual.reset( );
    itsWeight.reset( );
    itsImage.reset( );
    itsMask.reset( );
    itsGridWt.reset( );
    itsPB.reset( );
    itsImagePBcor.reset( );
    itsTempWorkIm.reset();

    itsSumWt.reset( );
    itsOverWrite=False; // Hard Coding this. See CAS-6937. overwrite;
    itsUseWeight=useweightimage;
    itsPBScaleFactor=1.0;

    itsNFacets=1;
    itsFacetId=0;
    itsNChanChunks = 1;
    itsChanId = 0;
    itsNPolChunks = 1;
    itsPolId = 0;

    itsImageName = imagename;
    itsCoordSys = imcoordsys;
    itsImageShape = imshape;
    itsObjectName = objectname;
    itsMiscInfo = miscinfo;

    init();

    validate();
  }

  // Used from SynthesisNormalizer::makeImageStore()
  SIImageStore::SIImageStore(const String &imagename, const Bool ignorefacets, const Bool ignoresumwt)
  {
    LogIO os( LogOrigin("SIImageStore","Open existing Images",WHERE) );
      

    itsPsf.reset( );
    itsModel.reset( );
    itsResidual.reset( );
    itsWeight.reset( );   
    itsImage.reset( );
    itsMask.reset( );
    itsGridWt.reset( );
    itsPB.reset( );
    itsImagePBcor.reset( );
    itsTempWorkIm.reset();
    itsMiscInfo=Record();

    itsSumWt.reset( );
    itsNFacets=1;
    itsFacetId=0;
    itsNChanChunks = 1;
    itsChanId = 0;
    itsNPolChunks = 1;
    itsPolId = 0;
    
    itsOverWrite=False;
    //need to to this init now so that imageExts is initialized
    init();

    itsImageName = imagename;

    // Since this constructor creates an ImStore from images on disk, it needs at least one of the
    // images to actually be present on disk, from which it can retrieve shape and coordsys information.

    if( doesImageExist(itsImageName+String(".residual")) || 
	doesImageExist(itsImageName+String(".psf")) ||
	doesImageExist(itsImageName+String(".model")) ||
	doesImageExist(itsImageName+String(".gridwt"))  )
      {
	std::shared_ptr<ImageInterface<Float> > imptr;
	if( doesImageExist(itsImageName+String(".psf")) )
	  {
	    buildImage( imptr, (itsImageName+String(".psf")) );
	    //            itsObjectName=imptr->imageInfo().objectName();
	    //	    itsMiscInfo=imptr->miscInfo();
	  }
	else if ( doesImageExist(itsImageName+String(".residual")) ){
	  buildImage( imptr, (itsImageName+String(".residual")) );
	  //          itsObjectName=imptr->imageInfo().objectName();
	  //	  itsMiscInfo=imptr->miscInfo();
	}
	else if ( doesImageExist(itsImageName+String(".model")) ){
	  buildImage( imptr, (itsImageName+String(".model")) );
	  //          itsObjectName=imptr->imageInfo().objectName();
	  //	  itsMiscInfo=imptr->miscInfo();
	}
	else
	  {
	    // How can this be right ? 
	    buildImage( imptr, (itsImageName+String(".gridwt")) );
	  }

	itsObjectName=imptr->imageInfo().objectName();
	itsImageShape=imptr->shape();
	itsCoordSys = imptr->coordinates();
	itsMiscInfo=imptr->miscInfo();
	
      }
    else
      {
	throw( AipsError( "PSF, Residual, Model Image (or sumwt) do not exist. Please create one of them." ) );
      }
    
    if( doesImageExist(itsImageName+String(".residual")) || 
	doesImageExist(itsImageName+String(".psf")) )
      {
	if( doesImageExist(itsImageName+String(".sumwt")) )
	  {
	    std::shared_ptr<ImageInterface<Float> > imptr;
	    //imptr.reset( new PagedImage<Float> (itsImageName+String(".sumwt")) );
	    buildImage( imptr, (itsImageName+String(".sumwt")) );
	    itsNFacets = imptr->shape()[0];
	    itsFacetId = 0;
	    itsUseWeight = getUseWeightImage( *imptr );
	    itsPBScaleFactor=1.0; ///// No need to set properly here as it will be calc'd in ()
	    /////redo this here as psf may have different coordinates
	    itsCoordSys = imptr->coordinates();
	    itsMiscInfo=imptr->miscInfo();
	    if( itsUseWeight && ! doesImageExist(itsImageName+String(".weight")) )
	      {
		throw(AipsError("Internal error : Sumwt has a useweightimage=True but the weight image does not exist."));
	      }
	  }
	else
	  {
	    if(!ignoresumwt)
	      {throw( AipsError( "SumWt information does not exist. Please create either a PSF or Residual" ) );}
	    else
	      {
		os << "SumWt does not exist. Proceeding only with PSF" << LogIO::POST;
		std::shared_ptr<ImageInterface<Float> > imptr;
		//imptr.reset( new PagedImage<Float> (itsImageName+String(".sumwt")) );
		if( doesImageExist(itsImageName+String(".residual") ) )
		  { buildImage( imptr, (itsImageName+String(".residual")) ); }
		else
		  { buildImage( imptr, (itsImageName+String(".psf")) ); }
		
		itsNFacets=1;
		itsFacetId=0;
		itsUseWeight=False;
		itsPBScaleFactor=1.0;
		itsCoordSys = imptr->coordinates();
		itsMiscInfo=imptr->miscInfo();
	      }
	  }
      }// if psf or residual exist...

    if( ignorefacets==True ) itsNFacets= 1;

    init();
    
    validate();
  }

  // used from getSubImageStore(), for example when creating the facets list
  // this would be safer if it was refactored as a copy constructor of the generic stuff +
  // initialization of the facet related parameters
  SIImageStore::SIImageStore(const std::shared_ptr<ImageInterface<Float> > &modelim,
			     const std::shared_ptr<ImageInterface<Float> > &residim,
			     const std::shared_ptr<ImageInterface<Float> > &psfim,
			     const std::shared_ptr<ImageInterface<Float> > &weightim,
			     const std::shared_ptr<ImageInterface<Float> > &restoredim,
			     const std::shared_ptr<ImageInterface<Float> > &maskim,
			     const std::shared_ptr<ImageInterface<Float> > &sumwtim,
			     const std::shared_ptr<ImageInterface<Float> > &gridwtim,
			     const std::shared_ptr<ImageInterface<Float> > &pbim,
			     const std::shared_ptr<ImageInterface<Float> > &restoredpbcorim,
                             const std::shared_ptr<ImageInterface<Float> > & tempworkimage,
			     const CoordinateSystem &csys,
			     const IPosition &imshape,
			     const String &imagename,
			     const String &objectname,
			     const Record &miscinfo,
			     const Int facet, const Int nfacets,
			     const Int chan, const Int nchanchunks,
			     const Int pol, const Int npolchunks,
			     const Bool useweightimage)
  {
      

    itsPsf=psfim;
    itsModel=modelim;
    itsResidual=residim;
    /* if(residim){
     LatticeLocker lock1(*itsResidual, FileLocker::Read);
     cerr << "RESIDMAX " << max(itsResidual->get()) <<  "   " << max(residim->get()) << endl;
     }*/
    itsWeight=weightim;
    itsImage=restoredim;
    itsMask=maskim;

    itsSumWt=sumwtim;

    itsGridWt=gridwtim;
    itsPB=pbim;
    itsImagePBcor=restoredpbcorim;
    itsTempWorkIm=tempworkimage;

    itsPBScaleFactor=1.0;// No need to set properly here as it will be computed in makePSF.

    itsObjectName = objectname;
    itsMiscInfo = miscinfo;

    itsNFacets = nfacets;
    itsFacetId = facet;
    itsNChanChunks = nchanchunks;
    itsChanId = chan;
    itsNPolChunks = npolchunks;
    itsPolId = pol;

    itsOverWrite=False;
    itsUseWeight=useweightimage;

    itsParentImageShape = imshape; 
    itsImageShape = imshape;

    itsParentCoordSys = csys;
    itsCoordSys = csys; // Hopefully this doesn't change for a reference image
    itsImageName = imagename;

    //-----------------------
    init(); // Connect parent pointers to the images.
    //-----------------------

    // Set these to null, to be set later upon first access.
    itsPsf.reset( );
    itsModel.reset( );
    itsResidual.reset( );
    itsWeight.reset( );
    itsImage.reset( );
    itsMask.reset( );
    itsSumWt.reset( );
    itsPB.reset( );

    validate();
  }
  
   void SIImageStore::validate()
  {
    /// There are two valid states. Check for at least one of them. 
    Bool state = False;
    
    stringstream oss;
    oss << "shape:" << itsImageShape << " parentimageshape:" << itsParentImageShape 
	<< " nfacets:" << itsNFacets << "x" << itsNFacets << " facetid:" << itsFacetId 
	<< " nchanchunks:" << itsNChanChunks << " chanid:" << itsChanId 
	<< " npolchunks:" << itsNPolChunks << " polid:" << itsPolId 
	<< " coord-dim:" << itsCoordSys.nPixelAxes() 
	<< " psf/res:" << (hasPsf() || hasResidual()) ;
    if( hasSumWt() ) oss << " sumwtshape : " << sumwt()->shape() ; 
	oss << endl;


    try {

    if( itsCoordSys.nPixelAxes() != 4 ) state=False;
    
    /// (1) Regular imagestore 
    if( itsNFacets==1 && itsFacetId==0 
	&& itsNChanChunks==1 && itsChanId==0
	&& itsNPolChunks==1 && itsPolId==0 )  {
      Bool check1 = hasSumWt() && sumwt()->shape()[0]==1;
      if(  (itsImageShape.isEqual(itsParentImageShape) ) && ( check1 || !hasSumWt() )
	   && itsParentImageShape.product() > 0 ) state=True;
      }
    /// (2) Reference Sub Imagestore 
    else if ( ( itsNFacets>1 && itsFacetId >=0 )
	      || ( itsNChanChunks>1 && itsChanId >=0 ) 
	      || ( itsNPolChunks>1 && itsPolId >=0 )   ) {
      // If shape is still unset, even when the first image has been made....
      Bool check1 = ( itsImageShape.product() > 0 && ( hasPsf() || hasResidual() ) );
      Bool check2 = ( itsImageShape.isEqual(IPosition(4,0,0,0,0)) && ( !hasPsf() && !hasResidual() ) );
      Bool check3 = hasSumWt() && sumwt()->shape()[0]==1; // One facet only.

      if( ( check1 || check2 ) && ( itsParentImageShape.product()>0 ) 
	  && ( itsFacetId < itsNFacets*itsNFacets ) 
	  //	  && ( itsChanId <= itsNChanChunks )   // chanchunks can be larger...
	  && ( itsPolId < itsNPolChunks ) 
	  && ( check3 || !hasSumWt() ) )  state = True;
    }

    } catch( AipsError &x )  {
      state = False;
      oss << "  |||||  " << x.getMesg() << endl;
    }

    //      cout << " SIIM:validate : " << oss.str() << endl;

    if( state == False )  throw(AipsError("Internal Error : Invalid ImageStore state : "+ oss.str()) );
    
    return;
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  std::shared_ptr<SIImageStore> SIImageStore::getSubImageStore(const Int facet, const Int nfacets, 
							  const Int chan, const Int nchanchunks, 
							  const Int pol, const Int npolchunks)
  {
   
    return std::make_shared<SIImageStore>(itsModel, itsResidual, itsPsf, itsWeight, itsImage, itsMask, itsSumWt, itsGridWt, itsPB, itsImagePBcor, itsTempWorkIm, itsCoordSys, itsImageShape, itsImageName, itsObjectName, itsMiscInfo, facet, nfacets, chan, nchanchunks, pol, npolchunks, itsUseWeight);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //// Either read an image from disk, or construct one. 

  std::shared_ptr<ImageInterface<Float> > SIImageStore::openImage(const String imagenamefull, 
							     const Bool overwrite, 
							     const Bool dosumwt, const Int nfacetsperside, const Bool checkCoordSys)
  {


    std::shared_ptr<ImageInterface<Float> > imPtr;
    IPosition useShape( itsParentImageShape );

    if( dosumwt ) // change shape to sumwt image shape.
      {
	useShape[0] = nfacetsperside;
	useShape[1] = nfacetsperside;
	//	cout << "openImage : Making sumwt grid : using shape : " << useShape << endl;
      }

    //    overwrite=False; /// HARD CODING THIS. See CAS-6937.

    //    cout << "Open image : " << imagenamefull << "    useShape : " << useShape << endl;

    // if image exists
    //      if overwrite=T
    //           try to make new image
    //           if not, try to open existing image
    //                     if cannot, complain.
    //       if overwrite=F
    //           try to open existing image
    //           if cannot, complain
    // if image does not exist
    //       try to make new image
    //       if cannot, complain
    Bool dbg=False;
    if( doesImageExist( imagenamefull ) )
      {
	///// not used since itsOverWrite is hardcoded to FALSE (CAS-6937)
	if (overwrite) //overwrite and make new image
	  {
	    if(dbg) cout << "Trying to overwrite and open new image named : " << imagenamefull << " ow:"<< overwrite << endl;
	    try{
	      buildImage(imPtr, useShape, itsParentCoordSys, imagenamefull) ;
              LatticeLocker lock1(*imPtr, FileLocker::Write);
	      // initialize to zeros...
	      imPtr->set(0.0);
	    }
	    catch (AipsError &x){
	      if(dbg)cout << "Cannot overwrite : " << x.getMesg() << endl;
	      if(dbg)cout << "Open already ? : " << Table::isOpened( imagenamefull ) << "  Writable ? : " << Table::isWritable( imagenamefull ) << endl;
	    if(Table::isWritable( imagenamefull ))
	      {
		if(dbg) cout << "--- Trying to open existing image : "<< imagenamefull << endl;
		try{
		  buildImage( imPtr, imagenamefull );
		}
		catch (AipsError &x){
		  throw( AipsError("Writable table exists, but cannot open : " + x.getMesg() ) );
		}
	      }// is table writable
	    else
	      {
		throw( AipsError("Cannot overwrite existing image. : " + x.getMesg() ) );
	      }
	    }
	  }// overwrite existing image
	else // open existing image ( Always tries this )
	  {
	    if(1) //Table::isWritable( imagenamefull ))
	      {
		if(dbg) cout << "Trying to open existing image : "<< imagenamefull << endl;
		try{
		  buildImage( imPtr, imagenamefull ) ;

		  if( !dosumwt)
		    {
		      //cout << useShape << "  and " << imPtr->shape() << " ---- " << useShape.product() << " : " << itsCoordSys.nCoordinates() << endl;
		      // Check if coordsys and shape of this image are consistent with current ones (if filled)

		      if( useShape.product()>0 &&  ! useShape.isEqual( imPtr->shape() ) )
			{
			  ostringstream oo1,oo2;
			  oo1 << useShape; oo2 << imPtr->shape();
			  throw( AipsError( "There is a shape mismatch between existing images ("+oo2.str()+") and current parameters ("+oo1.str()+"). If you are attempting to restart a run with a new image shape, please change imagename and supply the old model or mask as inputs (via the startmodel or mask parameters) so that they can be regridded to the new shape before continuing." ) );
			}

		      
		     
		      if( itsParentCoordSys.nCoordinates()>0 &&  checkCoordSys && ! itsParentCoordSys.near( imPtr->coordinates()) )
			{
			  /// Implement an exception to get CAS-9977 to work. Modify the current coordsys to match the parent.
			  /// "The DirectionCoordinates have differing latpoles"
			  if( itsParentCoordSys.errorMessage().contains("differing latpoles") )
			    {
			      DirectionCoordinate dcord = itsParentCoordSys.directionCoordinate();
			      //cout << "Latpole from parent : " << dcord.longLatPoles() << endl;
			      
			      DirectionCoordinate dcord2 = imPtr->coordinates().directionCoordinate();
			      //cout << "Latpole from imPtr : " << dcord2.longLatPoles() << endl;
		      
			    LogIO os( LogOrigin("SIImageStore","Open existing Images",WHERE) );
			    os  << " Mismatch in Csys latpoles between existing image on disk (" << dcord2.longLatPoles() <<  ") and current imaging run (" << dcord.longLatPoles() << ") : " << itsParentCoordSys.errorMessage() << " -- Resetting to match image on disk" << LogIO::WARN << LogIO::POST;
			    //			     cout << "Set the coordinate from the Parent value" << endl;
			    //imPtr->setCoordinateInfo( itsParentCoordSys );
			    itsParentCoordSys = imPtr->coordinates();
			    }
			  
			  // Check again. 
			  if ( ! itsParentCoordSys.near( imPtr->coordinates() ) )
			    {
			      //cout << " Still different..." << endl;
			      throw( AipsError( "There is a coordinate system mismatch between existing images on disk and current parameters ("+itsParentCoordSys.errorMessage()+"). If you are attempting to restart a run, please change imagename and supply the old model or mask as inputs (via the startmodel or mask parameters) so that they can be regridded to the new coordinate system before continuing. " ) );
			    }
			      //}
			}
		    }// not dosumwt
		}
		catch (AipsError &x){
		  throw( AipsError("Cannot open existing image : "+imagenamefull+" : " + x.getMesg() ) );
		}
	      }// is table writable
	    else // table exists but not writeable
	      {
		if(dbg)cout << "Table exists but not writeable : " << imagenamefull << "  --- Open : " << Table::isOpened( imagenamefull ) << endl;
		throw( AipsError("Image exists but not able to open for writes :"+imagenamefull+ ". Opened elsewhere : " + String::toString(Table::isOpened(imagenamefull))) );
	      }
	  }// open existing image
      }// if image exists
      else // image doesn't exist. make new one
	{
	  if(dbg) cout << "Trying to open new image named : " << imagenamefull <<  endl;
	  try{
	    buildImage(imPtr, useShape, itsParentCoordSys, imagenamefull) ;
	    // initialize to zeros...
            {
            LatticeLocker lock1(*imPtr, FileLocker::Write);
	    imPtr->set(0.0);
	    imPtr->flush();
            imPtr->unlock();
            }
	  }
	  catch (AipsError &x){
	    throw( AipsError("Cannot make new image. : " + x.getMesg() ) );
	  }
	}


      //////////////////////////////////////
    /*      
    if( overwrite || !Table::isWritable( imagenamefull ) )
      {
	cout << "Trying to open new image named : " << imagenamefull << " ow:"<< overwrite << endl;
	imPtr.reset( new PagedImage<Float> (useShape, itsCoordSys, imagenamefull) );
	// initialize to zeros...
	imPtr->set(0.0);
      }
    else
      {
	if(Table::isWritable( imagenamefull ))
	  {
	    cout << "Trying to open existing image : "<< imagenamefull << endl;
	    try{
	      imPtr.reset( new PagedImage<Float>( imagenamefull ) );
	    }
	    catch (AipsError &x){
	      cerr << "Writable table exists, but cannot open. Creating temp image. : " << x.getMesg() << endl;
	      imPtr.reset( new TempImage<Float> (useShape, itsCoordSys) );
	      //  imPtr=new PagedImage<Float> (useShape, itsCoordSys, imagenamefull);
	      imPtr->set(0.0);
	    }
	  }
	else
	  {
	    cerr << "Table " << imagenamefull << " is not writeable. Creating temp image." << endl;
	    imPtr.reset( new TempImage<Float> (useShape, itsCoordSys) );
	    imPtr->set(0.0);
	  }
      }
    */
    return imPtr;
  }

  void SIImageStore::buildImage(std::shared_ptr<ImageInterface<Float> > &imptr, IPosition shape,
                                CoordinateSystem csys, const String name)
  {
    LogIO os( LogOrigin("SIImageStore", "Open non-existing image", WHERE) );
    os  <<"Opening image, name: " << name << LogIO::DEBUG1;

    itsOpened++;
    //For some reason cannot open a new paged image with UserNoread directly
    {
      PagedImage<Float> godot(shape, csys, name);
      godot.unlock();
    }
    TableLock::LockOption locktype=TableLock::AutoNoReadLocking;
    /*if((name.contains(imageExts(PSF)) && !name.contains(imageExts(PSF)+".tt"))|| (name.contains(imageExts(RESIDUAL))&& !name.contains(imageExts(RESIDUAL)+".tt")) || (name.contains(imageExts(SUMWT)) && !name.contains(imageExts(SUMWT)+".tt"))){
      locktype=TableLock::UserNoReadLocking;
      }*/
    imptr.reset( new PagedImage<Float> (name, locktype) );
    {
      LatticeLocker lock1(*imptr, FileLocker::Write);
      initMetaInfo(imptr, name);
      imptr->unlock();
    }
    /*
    Int MEMFACTOR = 18;
    Double memoryMB=HostInfo::memoryTotal(True)/1024/(MEMFACTOR*itsOpened);


    TempImage<Float> *tptr = new TempImage( TiledShape(shape, tileShape()), csys, memoryBeforeLattice() ) ;

    tptr->setMaximumCacheSize(shape.product());
    tptr->cleanCache();

    imptr.reset( tptr );
    
    */
  }

  void SIImageStore::buildImage(std::shared_ptr<ImageInterface<Float> > &imptr, const String name)
  {
    LogIO os(LogOrigin("SIImageStore", "Open existing Images", WHERE));
    os  <<"Opening image, name: " << name << LogIO::DEBUG1;

    itsOpened++;
    if(Table::isReadable(name)){
      TableLock::LockOption locktype=TableLock::AutoNoReadLocking;
      /*if((name.contains(imageExts(PSF)) && !name.contains(imageExts(PSF)+".tt"))|| (name.contains(imageExts(RESIDUAL))&& !name.contains(imageExts(RESIDUAL)+".tt")) || (name.contains(imageExts(SUMWT)) && !name.contains(imageExts(SUMWT)+".tt"))){
        locktype=TableLock::UserNoReadLocking;
        }*/
      Table table(name, locktype);
      String type = table.tableInfo().type();
      if (type != TableInfo::type(TableInfo::PAGEDIMAGE)) {

        imptr.reset( new PagedImage<Float>( table ) );
        imptr->unlock();
        return;
      }
    }
        LatticeBase* latt =ImageOpener::openImage(name);
    if(!latt)
      {
	throw(AipsError("Error in opening Image : "+name));
      }
    DataType dtype=latt->dataType();
    if(dtype==TpFloat)
      {
	imptr.reset(dynamic_cast<ImageInterface<Float>* >(latt));
      }
    else
      {
	throw AipsError( "Need image to have float values :  "+name);
      }

    /*    
    std::shared_ptr<casacore::ImageInterface<Float> > fim;
    std::shared_ptr<casacore::ImageInterface<Complex> > cim;

    std::tie(fim , cim)=ImageFactory::fromFile(name);
    if(fim)
      {
	imptr.reset( dynamic_cast<std::shared_ptr<casacore::ImageInterface<Float> > >(*fim) );
      }
    else
      {
	throw( AipsError("Cannot open with ImageFactory : "+name));
      }
    */




    /*
    IPosition cimageShape;
    CoordinateSystem cimageCoord = StokesImageUtil::CStokesCoord( itsCoordSys,
								  whichStokes, itsDataPolRep);
    */

  }

  /**
   * Sets ImageInfo and MiscInfo on an image
   *
   * @param imptr image to initialize
   */
  void SIImageStore::initMetaInfo(std::shared_ptr<ImageInterface<Float> > &imptr,
                                  const String name)
  {
      // Check objectname, as one of the mandatory fields. What this is meant to check is -
      // has the metainfo been initialized? If not, grab info from associated PSF
    LatticeLocker lock1(*imptr, FileLocker::Write);
      if (not itsObjectName.empty()) {
          ImageInfo info = imptr->imageInfo();
          info.setObjectName(itsObjectName);
          imptr->setImageInfo(info);
          imptr->setMiscInfo(itsMiscInfo);
      } else if (std::string::npos == name.find(imageExts(PSF))) {
          auto srcImg = psf(0);
          if (nullptr != srcImg) {
              imptr->setImageInfo(srcImg->imageInfo());
              imptr->setMiscInfo(srcImg->miscInfo());
          }
      }
      imptr->unlock();
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SIImageStore::setMiscInfo(const Record miscinfo)
  {
    itsMiscInfo = miscinfo;
  }

  void SIImageStore::setObjectName(const String name)
  {
    itsObjectName = name;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  std::shared_ptr<ImageInterface<Float> > SIImageStore::makeSubImage(const Int facet, const Int nfacets,
								const Int chan, const Int nchanchunks,
								const Int pol, const Int npolchunks,
								ImageInterface<Float>& image)
  {
    //assuming n x n facets
    Int nx_facets=Int(sqrt(Double(nfacets)));
    LogIO os( LogOrigin("SynthesisImager","makeFacet") );
     // Make the output image
    Slicer imageSlicer;

    // Add checks for all dimensions..
    if((facet>(nfacets-1))||(facet<0)) {
      os << LogIO::SEVERE << "Illegal facet " << facet << LogIO::POST;
      return std::shared_ptr<ImageInterface<Float> >();
    }
    IPosition imshp=image.shape();
    IPosition blc(imshp.nelements(), 0);
    IPosition trc=imshp-1;
    IPosition inc(imshp.nelements(), 1);

    /// Facets
    Int facetx = facet % nx_facets; 
    Int facety = (facet - facetx) / nx_facets;
    Int sizex = imshp(0) / nx_facets;
    Int sizey = imshp(1) / nx_facets;
    blc(1) = facety * sizey; 
    trc(1) = blc(1) + sizey - 1;
    blc(0) = facetx * sizex; 
    trc(0) = blc(0) + sizex - 1;

    /// Pol Chunks
    Int sizepol = imshp(2) / npolchunks;
    blc(2) = pol * sizepol;
    trc(2) = blc(2) + sizepol - 1;

    /// Chan Chunks
    Int sizechan = imshp(3) / nchanchunks;
    blc(3) = chan * sizechan;
    trc(3) = blc(3) + sizechan - 1;

    LCBox::verify(blc, trc, inc, imshp);
    Slicer imslice(blc, trc, inc, Slicer::endIsLast);

    // Now create the sub image
    std::shared_ptr<ImageInterface<Float> >  referenceImage( new SubImage<Float>(image, imslice, True) );
    {
      LatticeLocker lock1(*referenceImage, FileLocker::Write);
      referenceImage->setMiscInfo(image.miscInfo());
      referenceImage->setUnits(image.units());

    }

    // cout << "Made Ref subimage of shape : " << referenceImage->shape() << endl;

    return referenceImage;
    
  }



  //////////////////////////////////////////////////////////////////////////////////////////////////////

  SIImageStore::~SIImageStore() 
  {
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  Bool SIImageStore::releaseLocks() 
  {
    //LogIO os( LogOrigin("SIImageStore","releaseLocks",WHERE) );

    //    String fname( itsImageName+String(".info") );
    //    makePersistent( fname );

    if( itsPsf ) releaseImage( itsPsf );
    if( itsModel ) { releaseImage( itsModel ); }
    if( itsResidual ) releaseImage( itsResidual );
    if( itsImage ) releaseImage( itsImage );
    if( itsWeight ) releaseImage( itsWeight );
    if( itsMask ) releaseImage( itsMask );
    if( itsSumWt ) releaseImage( itsSumWt );
    if( itsGridWt ) releaseImage( itsGridWt );
    if( itsPB ) releaseImage( itsPB );
    if( itsImagePBcor ) releaseImage( itsImagePBcor );

    return True; // do something more intelligent here.
  }

  Bool SIImageStore::releaseComplexGrids() 
  {
    //LogIO os( LogOrigin("SIImageStore","releaseComplexGrids",WHERE) );

    if( itsForwardGrid ) releaseImage( itsForwardGrid );
    if( itsBackwardGrid ) releaseImage( itsBackwardGrid );

    return True; // do something more intelligent here.
  }

  void SIImageStore::releaseImage( std::shared_ptr<ImageInterface<Float> > &im )
  {
    //cerr << this << " releaseimage " << im.get() << endl;
    //LogIO os( LogOrigin("SIImageStore","releaseLocks",WHERE) );
    im->flush();
    //os << LogIO::WARN << "clear cache" << LogIO::POST;
    im->clearCache();
    //os << LogIO::WARN << "unlock" << LogIO::POST;
    im->unlock();
    //os << LogIO::WARN << "tempClose" << LogIO::POST;
    //im->tempClose();
    //os << LogIO::WARN << "NULL" << LogIO::POST;
    im.reset();  // This was added to allow modification by modules independently
  }
  
  void SIImageStore::releaseImage( std::shared_ptr<ImageInterface<Complex> > &im )
  {
    im->tempClose();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  Vector<String> SIImageStore::getModelImageName()
  {
    Vector<String> mods(1);
    mods[0]=itsImageName + imageExts(MODEL);
    return mods;
  }

  Long SIImageStore::estimateRAM(){
    //right now this is estimated at 2MB for the 2 complex lattices;
    return Long(2000);
  }
  void SIImageStore::setModelImage( const Vector<String> &modelnames)
  {
    LogIO os( LogOrigin("SIImageStore","setModelImage",WHERE) );

    if( modelnames.nelements() > 1 )
      {throw( AipsError("Multiple starting model images are currently not supported. Please merge them before supplying as input to startmodel"));
	/// If needed, THIS is the place to add code to support lists of model images... perhaps regrid separately and add them up or some such thing.
      }

    if( modelnames.nelements()==1 ) { setModelImageOne( modelnames[0] ); }
  }



  void SIImageStore::setModelImageOne( const String &modelname , Int nterm)
  {
    LogIO os( LogOrigin("SIImageStore","setModelImageOne",WHERE) );

    if(modelname==String("")) return;

    Bool multiterm=False;
    if(nterm>-1)multiterm=True;
    if(nterm==-1)nterm=0;

    Directory immodel( modelname ); //  +String(".model") );
    if( !immodel.exists() ) 
      {
	os << "Starting model image " << modelname <<  " does not exist. No initial prediction will be done" << ((multiterm)?String(" for term")+String::toString(nterm) :"") << LogIO::POST;
	return;
      }

    // master merge 2019.01.08 - leaving in the commnets for now but clean up after it is verified 
    //SHARED_PTR<PagedImage<Float> > newmodel( new PagedImage<Float>( modelname ) ); //+String(".model") ) );
    //SHARED_PTR<ImageInterface<Float> > newmodel;
    std::shared_ptr<ImageInterface<Float> > newmodel;
    buildImage(newmodel, modelname);
    // in master
    //std::shared_ptr<PagedImage<Float> > newmodel( new PagedImage<Float>( modelname ) ); //+String(".model") ) );

    Bool hasMask = newmodel->isMasked(); /// || newmodel->hasPixelMask() ;
    
    if( hasMask )
      {
	
	os << "Input startmodel has an internal mask. Setting masked pixels to zero" << LogIO::POST;
	
	try {

	  ////// Neat function to replace masked pixels, but it will do it in-place.
          ////// We need to not modify the input model on disk, but apply the mask only OTF before
          //////  regridding to the target image,.
	  //	  ImageMaskedPixelReplacer<Float> impr( newmodel, 0, "" );
	  //	  impr.replace("0", False, False );

	  
	  TempImage<Float> maskmodel( newmodel->shape(), newmodel->coordinates() );
	  IPosition inshape = newmodel->shape();
	  for(Int pol=0; pol<inshape[2]; pol++)
	    {
	      for(Int chan=0; chan<inshape[3]; chan++)
		{
		  IPosition pos(4,0,0,pol,chan);
		  std::shared_ptr<ImageInterface<Float> > subim=makeSubImage(0,1, 
									chan, inshape[3],
									pol, inshape[2], 
									(*newmodel) );
		  
		  std::shared_ptr<ImageInterface<Float> > submaskmodel=makeSubImage(0,1, 
									       chan, inshape[3],
									       pol, inshape[2], 
									       maskmodel );
		  
		  ArrayLattice<Bool> pixmask( subim->getMask() );
		  LatticeExpr<Float> masked( (*subim) * iif(pixmask,1.0,0.0) );
		  submaskmodel->copyData( masked );
		}
	    }
	  
	  

	  // Check shapes, coordsys with those of other images.  If different, try to re-grid here.
	  if( (newmodel->shape() != model(nterm)->shape()) ||  (! itsCoordSys.near(newmodel->coordinates() )) )
	    {
	      os << "Regridding input model " << modelname << " to target coordinate system for " << itsImageName << ".model" << ((multiterm)?".tt"+String::toString(nterm) :"") << LogIO::POST;
	      regridToModelImage( maskmodel, nterm );
	    }
	  else
	    {
	      os << "Copying input model " << modelname << " to " << itsImageName << ".model" << ((multiterm)?".tt"+String::toString(nterm) :"")  << LogIO::POST;
	      model(nterm)->copyData( LatticeExpr<Float> (maskmodel) );
	    }
	  
	  
	} catch (AipsError &x) {
	  throw(AipsError("Setting masked pixels to zero for input startmodel : "+ x.getMesg()));
	}
	
      }// hasMask
    else // nomask
      {
	
	// Check shapes, coordsys with those of other images.  If different, try to re-grid here.
	if( (newmodel->shape() != model(nterm)->shape()) ||  (! itsCoordSys.near(newmodel->coordinates() )) )
	  {
	    os << "Regridding input model " << modelname << " to target coordinate system for " << itsImageName << ".model" << ((multiterm)?".tt"+String::toString(nterm) :"") << LogIO::POST;
	    regridToModelImage( *newmodel, nterm );
	  }
	else
	  {
	    os << "Copying input model " << modelname << " to " << itsImageName << ".model" << ((multiterm)?".tt"+String::toString(nterm) :"")  << LogIO::POST;
	    model(nterm)->copyData( LatticeExpr<Float> (*newmodel) );
	  }
      }//nomask
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  IPosition SIImageStore::getShape()
  {
    return itsImageShape;
  }

  String SIImageStore::getName()
  {
    return itsImageName;
  }

  uInt SIImageStore::getNTaylorTerms(Bool /*dopsf*/)
  {
    return 1;
  }

  /*
  void SIImageStore::checkRef( std::shared_ptr<ImageInterface<Float> > ptr, const String label )
  {
    if( ! ptr && itsImageName==String("reference") ) 
      {throw(AipsError("Internal Error : Attempt to access null subImageStore "+label + " by reference."));}
  }
  */

  void SIImageStore::accessImage( std::shared_ptr<ImageInterface<Float> > &ptr, 
		    std::shared_ptr<ImageInterface<Float> > &parentptr, 
		    const String label )
  {
    // if ptr is not null, assume it's OK. Perhaps add more checks.

    //cerr << "ACCIM itsptr" << ptr.get() << " parent " << parentptr.get() << " label " << label << endl;
    
    Bool sw=False;
    if( label.contains(imageExts(SUMWT)) ) sw=True;
    
    if( !ptr )
      {
	//cout << itsNFacets << " " << itsNChanChunks << " " << itsNPolChunks << endl;
	if( itsNFacets>1 || itsNChanChunks>1 || itsNPolChunks>1 )
	  {
	    if( ! parentptr ) 
	      {
		//cout << "Making parent : " << itsImageName+label << "    sw : " << sw << endl; 
		parentptr = openImage(itsImageName+label , itsOverWrite, sw, itsNFacets );  
		if( sw) {setUseWeightImage( *parentptr, itsUseWeight ); }
	      }
	    //	    cout << "Making facet " << itsFacetId << " out of " << itsNFacets << endl;
	    //cout << "Making chunk " << itsChanId << " out of " << itsNChanChunks << endl;
	    //ptr = makeFacet( itsFacetId, itsNFacets*itsNFacets, *parentptr );
	    ptr = makeSubImage( itsFacetId, itsNFacets*itsNFacets, 
				itsChanId, itsNChanChunks, 
				itsPolId, itsNPolChunks, 
				*parentptr );
	    if( ! sw )
	      {
		itsImageShape = ptr->shape(); // over and over again.... FIX.
		itsCoordSys = ptr->coordinates();
		itsMiscInfo = ptr->miscInfo();
	      }

	    //cout << "accessImage : " << label << " : sumwt : " << sw << " : shape : " << itsImageShape << endl;
    
	  }
	else
	  { //no facets of chanchunks
	    if(!parentptr){
	    ///coordsys for psf can be different ...shape should be the same.
	      ptr = openImage(itsImageName+label , itsOverWrite, sw, 1, !(label.contains(imageExts(PSF)))); }
	    else{
	      ptr=parentptr;
	    }
	    //cout << "Opening image : " << itsImageName+label << " of shape " << ptr->shape() << endl;
	  }
      }

    //cerr << "ACCIM2 ptr " << ptr.get() << " lock " << itsReadLock.get() << endl;
    //itsReadLock.reset(new LatticeLocker(*ptr, FileLocker::Read));
    
    //    DirectionCoordinate dcord = ptr->coordinates().directionCoordinate();
    //    cout << "Latpole from " << label << " : " << dcord.longLatPoles() << endl;

  }


  std::shared_ptr<ImageInterface<Float> > SIImageStore::psf(uInt /*nterm*/)
  {
    accessImage( itsPsf, itsParentPsf, imageExts(PSF) );
    
    return itsPsf;
  }
  std::shared_ptr<ImageInterface<Float> > SIImageStore::residual(uInt /*nterm*/)
  {
    accessImage( itsResidual, itsParentResidual, imageExts(RESIDUAL) );
    //    Record mi = itsResidual->miscInfo(); ostringstream oss;mi.print(oss);cout<<"MiscInfo(res) : " << oss.str() << endl;
    return itsResidual;
  }
  std::shared_ptr<ImageInterface<Float> > SIImageStore::weight(uInt /*nterm*/)
  {
    accessImage( itsWeight, itsParentWeight, imageExts(WEIGHT) );
    return itsWeight;
  }

  std::shared_ptr<ImageInterface<Float> > SIImageStore::sumwt(uInt /*nterm*/)
  {

    accessImage( itsSumWt, itsParentSumWt, imageExts(SUMWT) );
    
    if( itsNFacets>1 || itsNChanChunks>1 || itsNPolChunks>1 ) 
      { itsUseWeight = getUseWeightImage( *itsParentSumWt );}
    setUseWeightImage( *itsSumWt , itsUseWeight); // Sets a flag in the SumWt image. 
    
    return itsSumWt;
  }

  std::shared_ptr<ImageInterface<Float> > SIImageStore::model(uInt /*nterm*/)
  {
    accessImage( itsModel, itsParentModel, imageExts(MODEL) );
    LatticeLocker lock1(*itsModel, FileLocker::Write);
    // Set up header info the first time. 
    itsModel->setUnits("Jy/pixel");

    return itsModel;
  }
  std::shared_ptr<ImageInterface<Float> > SIImageStore::image(uInt /*nterm*/)
  {
    accessImage( itsImage, itsParentImage, imageExts(IMAGE) );
    LatticeLocker lock1(*itsImage, FileLocker::Write);
    itsImage->setUnits(Unit("Jy/beam"));
    return itsImage;
  }

  std::shared_ptr<ImageInterface<Float> > SIImageStore::mask(uInt /*nterm*/)
  {
    accessImage( itsMask, itsParentMask, imageExts(MASK) );
    return itsMask;
  }
  std::shared_ptr<ImageInterface<Float> > SIImageStore::gridwt(IPosition newshape)

  {
    if(newshape.empty()){
      accessImage( itsGridWt, itsParentGridWt, imageExts(GRIDWT) );
    }
    else{
      if(!itsGridWt  || (itsGridWt && (itsGridWt->shape() != newshape))){
	itsGridWt.reset();  // deassign previous one hopefully it'll close it
	CoordinateSystem newcoordsys=itsCoordSys;
	if(newshape.nelements() > 4){
	  Matrix<Double> pc(1,1);      pc = 1.0;
	  LinearCoordinate zc(Vector<String>(1, "FIELD_ORDER"), Vector<String>(1, ""), Vector<Double>(1, 0.0), Vector<Double>(1,1.0), pc, Vector<Double>(1,0.0));
	  newcoordsys.addCoordinate(zc);
        }
	  itsGridWt.reset(new PagedImage<Float>(newshape, newcoordsys, itsImageName+ imageExts(GRIDWT)));
	  initMetaInfo(itsGridWt, itsImageName+ imageExts(GRIDWT));

      }
    }
    /// change the coordinate system here, to uv.
    return itsGridWt;
  }

  std::shared_ptr<ImageInterface<Float> > SIImageStore::tempworkimage(uInt /*term*/){
    if(itsTempWorkIm) return itsTempWorkIm;
    itsTempWorkIm.reset(new PagedImage<Float>(itsImageShape, itsCoordSys, itsImageName+ ".work.temp"));
    static_cast<PagedImage<Float>* > (itsTempWorkIm.get())->set(0.0);
    itsTempWorkIm->flush();
    static_cast<PagedImage<Float>* > (itsTempWorkIm.get())->table().markForDelete();
    return itsTempWorkIm;
  }
  
  std::shared_ptr<ImageInterface<Float> > SIImageStore::pb(uInt /*nterm*/)
  {
    accessImage( itsPB, itsParentPB, imageExts(PB) );
    return itsPB;
  }
  std::shared_ptr<ImageInterface<Float> > SIImageStore::imagepbcor(uInt /*nterm*/)
  {
    accessImage( itsImagePBcor, itsParentImagePBcor, imageExts(IMAGEPBCOR) );
    LatticeLocker lock1(*itsImagePBcor, FileLocker::Write);
    itsImagePBcor->setUnits(Unit("Jy/beam"));
    return itsImagePBcor;
  }

  std::shared_ptr<ImageInterface<Complex> > SIImageStore::forwardGrid(uInt /*nterm*/){
    if( itsForwardGrid ) // && (itsForwardGrid->shape() == itsImageShape))
      {
	//	cout << "Forward grid has shape : " << itsForwardGrid->shape() << endl;
	return itsForwardGrid;
      }
    Vector<Int> whichStokes(0);
    IPosition cimageShape;
    cimageShape=itsImageShape;
    MFrequency::Types freqframe = itsCoordSys.spectralCoordinate(itsCoordSys.findCoordinate(Coordinate::SPECTRAL)).frequencySystem(True);
    // No need to set a conversion layer if image is in LSRK already or it is 'Undefined'
    if(freqframe != MFrequency::LSRK && freqframe!=MFrequency::Undefined && freqframe!=MFrequency::REST) 
      { itsCoordSys.setSpectralConversion("LSRK"); }
    CoordinateSystem cimageCoord = StokesImageUtil::CStokesCoord( itsCoordSys,
								  whichStokes, itsDataPolRep);
    cimageCoord.setObsInfo(itsCoordSys.obsInfo());
    cimageShape(2)=whichStokes.nelements();      
    //cout << "Making forward grid of shape : " << cimageShape << " for imshape : " << itsImageShape << endl;
    itsForwardGrid.reset( new TempImage<Complex>(TiledShape(cimageShape, tileShape()), cimageCoord, memoryBeforeLattice()) );
    //if(image())
    if(hasRestored()){
      LatticeLocker lock1(*itsForwardGrid, FileLocker::Write);
      itsForwardGrid->setImageInfo((image())->imageInfo());

    }
    return itsForwardGrid;
  }

  std::shared_ptr<ImageInterface<Complex> > SIImageStore::backwardGrid(uInt /*nterm*/){
    if( itsBackwardGrid ) //&& (itsBackwardGrid->shape() == itsImageShape))
      {
	//	cout << "Backward grid has shape : " << itsBackwardGrid->shape() << endl;
	return itsBackwardGrid;
      }
    Vector<Int> whichStokes(0);
    IPosition cimageShape;
    cimageShape=itsImageShape;
    MFrequency::Types freqframe = itsCoordSys.spectralCoordinate(itsCoordSys.findCoordinate(Coordinate::SPECTRAL)).frequencySystem(True);
    // No need to set a conversion layer if image is in LSRK already or it is 'Undefined'
    if(freqframe != MFrequency::LSRK && freqframe!=MFrequency::Undefined && freqframe!=MFrequency::REST ) 
      { itsCoordSys.setSpectralConversion("LSRK"); }
    CoordinateSystem cimageCoord = StokesImageUtil::CStokesCoord( itsCoordSys,
								  whichStokes, itsDataPolRep);
    cimageCoord.setObsInfo(itsCoordSys.obsInfo());
    cimageShape(2)=whichStokes.nelements();
    //cout << "Making backward grid of shape : " << cimageShape << " for imshape : " << itsImageShape << endl;
    itsBackwardGrid.reset( new TempImage<Complex>(TiledShape(cimageShape, tileShape()), cimageCoord, memoryBeforeLattice()) );
    //if(image())
    if(hasRestored()){
      LatticeLocker lock1(*itsBackwardGrid, FileLocker::Write);
      itsBackwardGrid->setImageInfo((image())->imageInfo());
    }
    return itsBackwardGrid;
    }
  Double SIImageStore::memoryBeforeLattice(){
	  //Calculate how much memory to use per temporary images before disking
    return 1.0;  /// in MB
  }
  IPosition SIImageStore::tileShape(){
	  //Need to have settable stuff here or algorith to determine this
	  return IPosition(4, min(itsImageShape[0],1000), min(itsImageShape[1],1000), 1, 1);
  }

  // TODO : Move to an image-wrapper class ? Same function exists in SynthesisDeconvolver.
  Bool SIImageStore::doesImageExist(String imagename)
  {
    LogIO os( LogOrigin("SIImageStore","doesImageExist",WHERE) );
    Directory image( imagename );
    return image.exists();
  }


  void SIImageStore::resetImages( Bool resetpsf, Bool resetresidual, Bool resetweight )
  {
    if( resetpsf ){
      LatticeLocker lock1(*(psf()), FileLocker::Write);
      psf()->set(0.0);

    }
    if( resetresidual ) {
      //      removeMask( residual() );
      LatticeLocker lock1(*(residual()), FileLocker::Write);
      residual()->set(0.0);
    }
    if( resetweight && itsWeight ){
      LatticeLocker lock1(*(weight()), FileLocker::Write);
      weight()->set(0.0);
    }
    if( resetweight ){
      LatticeLocker lock1(*(sumwt()), FileLocker::Write);
      sumwt()->set(0.0);
    }
  }

  void SIImageStore::addImages( std::shared_ptr<SIImageStore> imagestoadd,
				Bool addpsf, Bool addresidual, Bool addweight, Bool adddensity)
  {

    if(addpsf)
      {
	LatticeExpr<Float> adderPsf( *(psf()) + *(imagestoadd->psf()) ); 
	psf()->copyData(adderPsf);
      }
    if(addresidual)
      {
	LatticeExpr<Float> adderRes( *(residual()) + *(imagestoadd->residual()) ); 
	residual()->copyData(adderRes);
      }
    if(addweight)
      {
	if( getUseWeightImage( *(imagestoadd->sumwt()) ) ) // Access and add weight only if it is needed.
	  {
	    LatticeExpr<Float> adderWeight( *(weight()) + *(imagestoadd->weight()) ); 
	    weight()->copyData(adderWeight);
	  }

	/*
	Array<Float> qqq, www;
	imagestoadd->sumwt()->get(qqq,True);
	sumwt()->get(www,True);
	cout << "SUMWT : Adding : " << qqq << " to " << www << endl;
	*/

	LatticeExpr<Float> adderSumWt( *(sumwt()) + *(imagestoadd->sumwt()) ); 
	sumwt()->copyData(adderSumWt);
	setUseWeightImage( *sumwt(),  getUseWeightImage(*(imagestoadd->sumwt()) ) );

      }
    if(adddensity)
      {
	LatticeExpr<Float> adderDensity( *(gridwt()) + *(imagestoadd->gridwt()) ); 
	gridwt()->copyData(adderDensity);
      }

  }

void SIImageStore::setWeightDensity( std::shared_ptr<SIImageStore> imagetoset )
  {
    LogIO os( LogOrigin("SIImageStore","setWeightDensity",WHERE) );
    //gridwt()->copyData( LatticeExpr<Float> ( *(imagetoset->gridwt()) ) );
    //looks like you have to call them before hand or it crashes in parallel sometimes
    IPosition shape=(imagetoset->gridwt())->shape();
    os << LogIO::DEBUG2 << "SHAPES " << imagetoset->gridwt()->shape() << "   " << gridwt()->shape() << endl;
    (gridwt(shape))->copyData( LatticeExpr<Float> ( *(imagetoset->gridwt()) ) );

  }

  // TODO
  //    cout << "WARN :   get getPbMax right for cube.... weight is indexed on chan and pol." << endl;
  Double SIImageStore::getPbMax()
  {

    //// Don't do any extra norm. Minor cycle will operate with native PB.
    //return 1.0;

    //// Normalize PB to 1 at the center of the image OF CHANNEL ZERO
    
    //        IPosition shp = weight(0)->shape();
    //     IPosition center(4, shp[0]/2, shp[1]/2,0,0);
    //    return sqrt(   weight(0)->getAt( center )   );
    

    //// Normalize PB to 1 at the location of the maximum (across all chans..)
    
    LatticeExprNode le( sqrt(max( *(weight(0)) )) );
    return le.getFloat();
    
  }


  Double SIImageStore::getPbMax(Int pol,Int chan)
  {

    //// Normalize PB to 1 at the location of the maximum (per pol,chan)
    
    CountedPtr<ImageInterface<Float> > subim=makeSubImage(0,1, 
							  chan, itsImageShape[3],
							  pol, itsImageShape[2], 
							  *weight(0) );

    LatticeExprNode le( sqrt(max( *subim )) );
    return le.getFloat();
  }

  void  SIImageStore::makePBFromWeight(const Float pblimit)
  {
   LogIO os( LogOrigin("SIImageStore","makePBFromWeight",WHERE) );

    	for(Int pol=0; pol<itsImageShape[2]; pol++)
	  {
	       for(Int chan=0; chan<itsImageShape[3]; chan++)
	      {

		itsPBScaleFactor = getPbMax(pol,chan);
		
		if(itsPBScaleFactor<=0){os << LogIO::NORMAL1 << "Skipping normalization for C:" << chan << " P:" << pol << " because pb max is zero " << LogIO::POST;}
		else {

		  CountedPtr<ImageInterface<Float> > wtsubim=makeSubImage(0,1, 
									  chan, itsImageShape[3],
									  pol, itsImageShape[2], 
									  *weight() );
		  CountedPtr<ImageInterface<Float> > pbsubim=makeSubImage(0,1, 
									  chan, itsImageShape[3],
									  pol, itsImageShape[2], 
									  *pb() );
		  
		  
		  LatticeExpr<Float> normed( sqrt(abs(*wtsubim)) / itsPBScaleFactor  );
		  LatticeExpr<Float> limited( iif( normed > fabs(pblimit) , normed, 0.0 ) );
		  pbsubim->copyData( limited );
		}// if not zero
	      }
	  }

        if((pb()->getDefaultMask()==""))
	  {
	    //Remove the old mask as it is no longer valid
	    //removeMask( pb() );

	    //	    if( pblimit >= 0.0 )
	      {
		//MSK//	
		LatticeExpr<Bool> pbmask( iif( *pb() > fabs(pblimit) , True , False ) );
		//MSK// 
		createMask( pbmask, pb() );
	      }

	  }
	weight()->unlock();
	pb()->unlock();
  }

  void  SIImageStore::makePBImage(const Float pblimit)
  {
   LogIO os( LogOrigin("SIImageStore","makePBImage",WHERE) );

   if( hasPB() )
     {

    	for(Int pol=0; pol<itsImageShape[2]; pol++)
	  {
	       for(Int chan=0; chan<itsImageShape[3]; chan++)
	      {

		  CountedPtr<ImageInterface<Float> > pbsubim=makeSubImage(0,1, 
									  chan, itsImageShape[3],
									  pol, itsImageShape[2], 
									  *pb() );

		  // Norm by the max
		  LatticeExprNode elmax= max( *pbsubim );
		  Float fmax = abs(elmax.getFloat());
		  //If there are multiple overlap of beam such that the peak is larger than 1 then normalize
		  //otherwise leave as is
		  if(fmax>1.0)
		    {
		      LatticeExpr<Float> normed( (*pbsubim) / fmax  );
		      LatticeExpr<Float> limited( iif( normed > fabs(pblimit) , normed, 0.0 ) );
		      pbsubim->copyData( limited );
		    }
		  else
		    {
		      LatticeExpr<Float> limited( iif((*pbsubim) > fabs(pblimit) , (*pbsubim), 0.0 ) );
		      pbsubim->copyData( limited );
		    }
	      }
	  }

	if((pb()->getDefaultMask()==""))// && pblimit >= 0.0)
	  {
	    //	    removeMask( pb() );
	    //MSK//		
	    LatticeExpr<Bool> pbmask( iif( *pb() > fabs(pblimit) , True , False ) );
	    //MSK// 
	    createMask( pbmask, pb() );
	  }

     }// if hasPB

  }

  Bool SIImageStore::createMask(LatticeExpr<Bool> &lemask, 
				CountedPtr<ImageInterface<Float> > outimage)
  {
    //cout << "Calling makeMask for mask0 for " << outimage->name() << endl;
    try
      {
        LatticeLocker lock1(*outimage, FileLocker::Write);
	ImageRegion outreg = outimage->makeMask("mask0",False,True);
	LCRegion& outmask=outreg.asMask();
	outmask.copyData(lemask);
	outimage->defineRegion("mask0",outreg, RegionHandler::Masks, True);
        
	outimage->setDefaultMask("mask0");
	
	outimage->unlock();
	outimage->tempClose();
	
	//    outimage->table().unmarkForDelete();      
      }
    catch (const AipsError& x) {
      throw(AipsError("Error in creating internal T/F mask : " + x.getMesg() ));
    }

    return True;
  }

  Bool SIImageStore::copyMask(CountedPtr<ImageInterface<Float> > inimage,
				CountedPtr<ImageInterface<Float> > outimage)
  {
    //    cout << "In copyMask for " << outimage->name() << endl;

    try
      {
	if( (inimage->getDefaultMask()).matches("mask0") ) // input mask exists.
	  {LatticeLocker lock1(*outimage, FileLocker::Write);
	    removeMask(outimage);
	    
	    // // clear output image mask
	    // if( (outimage->getDefaultMask()).matches("mask0") ) 
	    //   {outimage->setDefaultMask(""); 
	    // 	outimage->removeRegion("mask0");}
	    // get mask from input image
	    
	    ImageRegion outreg=outimage->makeMask("mask0", False, True);
	    LCRegion& outmask=outreg.asMask();
	    outmask.copyData(inimage->getRegion("mask0").asLCRegion());
	    outimage->defineRegion("mask0",outreg, RegionHandler::Masks,True);
	    outimage->setDefaultMask("mask0");
	  }
      }
    catch (const AipsError& x) {
      throw(AipsError("Error in copying internal T/F mask : " + x.getMesg() ));
    }

    return True;
  }
  
  void SIImageStore::removeMask(CountedPtr<ImageInterface<Float> > im)
  {
    try
      {
	// // clear output image mask
	// if( (im->getDefaultMask()).matches("mask0") ) 
	//   {im->setDefaultMask(""); 
	// 	im->removeRegion("mask0");}
	///Remove the old mask as it is no longer valid
        LatticeLocker lock1(*im, FileLocker::Write);
	if (im-> getDefaultMask() != String(""))
	  {
	    String strung=im->getDefaultMask();
	    im->setDefaultMask("");
	    im->removeRegion(strung);
	  } 
	if( im->hasRegion("mask0") )
	  {
	    im->removeRegion("mask0");
	  }
      }
    catch (const AipsError& x)
      {
	throw(AipsError("Error in deleting internal T/F mask : " + x.getMesg() ));
      }
  } 
  void SIImageStore:: rescaleResolution(Int chan, 
					ImageInterface<Float>& image, 
					const GaussianBeam& newbeam, 
					const GaussianBeam& oldbeam){

    LogIO os( LogOrigin("SIImageStore","rescaleResolution",WHERE) );
    GaussianBeam toBeUsed(Quantity(0.0, "arcsec"),Quantity(0.0, "arcsec"), 
			  Quantity(0.0, "deg")) ;
    try {
      Bool samesize = GaussianDeconvolver::deconvolve(toBeUsed, newbeam, oldbeam);

      /*
      os << LogIO::NORMAL2 << "Chan : " << chan << " : Input beam : : " << oldbeam.getMajor(Unit("arcsec")) << " arcsec, " << oldbeam.getMinor(Unit("arcsec"))<< " arcsec, " << oldbeam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
      os << LogIO::NORMAL2 << "Target beam : " << newbeam.getMajor(Unit("arcsec")) << " arcsec, " << newbeam.getMinor(Unit("arcsec"))<< " arcsec, " << newbeam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
      os << LogIO::NORMAL2 << "Beam to be used : " << toBeUsed.getMajor(Unit("arcsec")) << " arcsec, " << toBeUsed.getMinor(Unit("arcsec"))<< " arcsec, " << toBeUsed.getPA(Unit("deg")) << " deg" << LogIO::POST; 
      os << LogIO::NORMAL2 << "SameSize ? " << samesize << endl;
      */
      
      if( samesize )
	{
	  os << LogIO::NORMAL2 << "Input and output beam sizes are the same for Channel : " << chan << ". Not convolving residuals." << LogIO::POST;
	}
	else 
	{
	  Double pixwidth=sqrt(image.coordinates().increment()(0)*image.coordinates().increment()(0)+image.coordinates().increment()(1)*image.coordinates().increment()(1));
	  
	  if(toBeUsed.getMinor(image.coordinates().worldAxisUnits()[0]) > pixwidth)
	    {
	      //cerr << "old beam area " << oldbeam.getArea("rad2") << " new beam " << newbeam.getArea("rad2") << endl;
	      StokesImageUtil::Convolve(image, toBeUsed, True);
	      image.copyData(LatticeExpr<Float>(image*newbeam.getArea("rad2")/ oldbeam.getArea("rad2")));
	    }
	}
    }
    catch (const AipsError& x) {
      //throw(AipsError("Cannot convolve to new beam: may be smaller than old beam : " + x.getMesg() ));
      os << LogIO::WARN << "Cannot convolve to new beam for Channel : " << chan <<  " : " << x.getMesg() << LogIO::POST;
    }
    

  }


  

  void SIImageStore::dividePSFByWeight(const Float /* pblimit*/)
  {
    LogIO os( LogOrigin("SIImageStore","dividePSFByWeight",WHERE) );

    LatticeLocker lock1 (*(psf()), FileLocker::Write);
    normPSF();

    if( itsUseWeight )
    { 
	
	divideImageByWeightVal( *weight() ); 
    }
    (psf())->unlock();
    
  }

  void SIImageStore::normalizePrimaryBeam(const Float pblimit)
  {
    LogIO os( LogOrigin("SIImageStore","normalizePrimaryBeam",WHERE) );

    //    cout << "In dividePSFByWeight : itsUseWeight : " << itsUseWeight << endl;
    if( itsUseWeight )
    { 
	
	for(Int pol=0; pol<itsImageShape[2]; pol++)
	  {
	    for(Int chan=0; chan<itsImageShape[3]; chan++)
	      {
		os << LogIO::NORMAL1 << "Scale factor for [C" +String::toString(chan) + ":P" + String::toString(pol) + "] to keep the model image w.r.to a PB of max=1 is " << getPbMax(pol,chan) << LogIO::POST;
	      }//chan
	  }//pol

	makePBFromWeight(pblimit);
	
    }//if itsUseWeight
    else { makePBImage(pblimit); } // OR... just check that it exists already.
    
   }

  // Make another for the PSF too.
  void SIImageStore::divideResidualByWeight(Float pblimit,String normtype)
  {
    LogIO os( LogOrigin("SIImageStore","divideResidualByWeight",WHERE) );
    LatticeLocker lock1 (*(residual()), FileLocker::Write);

    // Normalize by the sumwt, per plane. 
    Bool didNorm = divideImageByWeightVal( *residual() );
    if( itsUseWeight )
      {
	
	for(Int pol=0; pol<itsImageShape[2]; pol++)
	  {
	    for(Int chan=0; chan<itsImageShape[3]; chan++)
	      {
		
		itsPBScaleFactor = getPbMax(pol,chan);
		//	cout << " pbscale : " << itsPBScaleFactor << endl;
		if(itsPBScaleFactor<=0){os << LogIO::NORMAL1 << "Skipping normalization for C:" << chan << " P:" << pol << " because pb max is zero " << LogIO::POST;}
		else {

		CountedPtr<ImageInterface<Float> > wtsubim=makeSubImage(0,1, 
								      chan, itsImageShape[3],
								      pol, itsImageShape[2], 
								      *weight() );
		CountedPtr<ImageInterface<Float> > ressubim=makeSubImage(0,1, 
								      chan, itsImageShape[3],
								      pol, itsImageShape[2], 
								      *residual() );

		
		LatticeExpr<Float> ratio;
		Float scalepb=1.0;
		if( normtype=="flatnoise"){
		  LatticeExpr<Float>deno = LatticeExpr<Float> ( sqrt( abs(*(wtsubim)) ) * itsPBScaleFactor );
		  os << LogIO::NORMAL1 ;
		  os <<  "[C" +String::toString(chan) + ":P" + String::toString(pol) + "] ";
		  os << "Dividing " << itsImageName+String(".residual") ;
		  os << " by [ sqrt(weightimage) * " << itsPBScaleFactor ;
		  os << " ] to get flat noise with unit pb peak."<< LogIO::POST;

           
          scalepb=fabs(pblimit)*itsPBScaleFactor*itsPBScaleFactor;
		  LatticeExpr<Float> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
		  LatticeExpr<Float> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
		  ratio=( ( (*(ressubim)) * mask ) / ( deno + maskinv ) );
		}
		else if(normtype=="pbsquare"){
		  Float deno =  itsPBScaleFactor*itsPBScaleFactor ;
		  os << LogIO::NORMAL1 ;
		  os <<  "[C" +String::toString(chan) + ":P" + String::toString(pol) + "] ";
		  os << "Dividing " << itsImageName+String(".residual") ;
		  os << itsPBScaleFactor ;
		  os << " ] to get optimal noise with unit pb peak."<< LogIO::POST;
		  scalepb=fabs(pblimit)*itsPBScaleFactor*itsPBScaleFactor;
		  //LatticeExpr<Float> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
		  //LatticeExpr<Float> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
		  ratio=(  (*(ressubim)) / ( deno ) );
		}
		else if( normtype=="flatsky") {

		  LatticeExpr<Float> deno = LatticeExpr<Float> ( *(wtsubim) );
		  os << LogIO::NORMAL1 ;
		  os <<  "[C" +String::toString(chan) + ":P" + String::toString(pol) + "] ";
		  os << "Dividing " << itsImageName+String(".residual") ;
		  os << " by [ weight ] to get flat sky"<< LogIO::POST;
		  scalepb=fabs(pblimit*pblimit)*itsPBScaleFactor*itsPBScaleFactor;
		  LatticeExpr<Float> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
		  LatticeExpr<Float> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
		  ratio=( ( (*(ressubim)) * mask ) / ( deno + maskinv ) );
		}

		//		IPosition ip(4,itsImageShape[0]/2,itsImageShape[1]/2,0,0);
		//Float resval = ressubim->getAt(ip);

		//LatticeExpr<Float> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
		//LatticeExpr<Float> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
		//LatticeExpr<Float> ratio( ( (*(ressubim)) * mask ) / ( deno + maskinv ) );
		
		//above blocks all sources outside minpb but visible with weight coverage
		//which could be cleaned out...one could use below for that
		//LatticeExpr<Float> ratio(iif( deno > scalepb, (*(ressubim))/ deno, *ressubim ) );

		ressubim->copyData(ratio);

		//cout << "Val of residual before|after normalizing at center for pol " << pol << " chan " << chan << " : " << resval << "|" << ressubim->getAt(ip) << " weight : " << wtsubim->getAt(ip) << endl;
		}// if not zero
	      }//chan
	  }//pol
	
      }
    
    // If no normalization happened, print a warning. The user must check if it's right or not.
    // Or... later if we get a gridder that does pre-norms, this warning can go. 
    if( (didNorm | itsUseWeight) != True ) 
      os << LogIO::WARN << "No normalization done to residual" << LogIO::POST;
    
    ///// A T/F mask in the residual will confuse users looking at the interactive clean
    ///// window
        if((residual()->getDefaultMask()=="") && hasPB()  &&  pblimit >=0.0 )
       {copyMask(pb(),residual());}

	if( pblimit <0.0 && (residual()->getDefaultMask()).matches("mask0") ) removeMask( residual() );

        residual()->unlock();


  }
  

  void SIImageStore::divideModelByWeight(Float pblimit, const String normtype)
  {
    LogIO os( LogOrigin("SIImageStore","divideModelByWeight",WHERE) );

    
        if(itsUseWeight // only when needed
	   && weight() )// i.e. only when possible. For an initial starting model, don't need wt anyway.
      {

	if( normtype=="flatsky") {
	 
	  LatticeExprNode LEN = max( *model());
	  os << LogIO::NORMAL1 << "Model is already flat sky with peak flux : " << LEN.getFloat();
	  os << ". No need to divide before prediction" << LogIO::POST;
	  
	  return;
	  }
	else if( normtype=="flatnoise"){

	  for(Int pol=0; pol<itsImageShape[2]; pol++)
	    {
	      for(Int chan=0; chan<itsImageShape[3]; chan++)
		{
		  
		  itsPBScaleFactor = getPbMax(pol,chan);
		  //	cout << " pbscale : " << itsPBScaleFactor << endl;
		if(itsPBScaleFactor<=0){os << LogIO::NORMAL1 << "Skipping normalization for C:" << chan << " P:" << pol << " because pb max is zero " << LogIO::POST;}
		else {
		  
		  CountedPtr<ImageInterface<Float> > wtsubim=makeSubImage(0,1, 
									  chan, itsImageShape[3],
									  pol, itsImageShape[2], 
									  *weight() );
		  CountedPtr<ImageInterface<Float> > modsubim=makeSubImage(0,1, 
									   chan, itsImageShape[3],
									   pol, itsImageShape[2], 
									   *model() );
		  os << LogIO::NORMAL1 ;
		  os <<  "[C" +String::toString(chan) + ":P" + String::toString(pol) + "] ";
		  os << "Dividing " << itsImageName+String(".model") ;
		  os << " by [ sqrt(weight) / " << itsPBScaleFactor ;
		  os <<" ] to get to flat sky model before prediction" << LogIO::POST;
		  
		 
		  LatticeExpr<Float> deno( sqrt( abs(*(wtsubim)) ) / itsPBScaleFactor );
		  
		  LatticeExpr<Float> mask( iif( (deno) > fabs(pblimit) , 1.0, 0.0 ) );
		  LatticeExpr<Float> maskinv( iif( (deno) > fabs(pblimit) , 0.0, 1.0 ) );
		  LatticeExpr<Float> ratio( ( (*(modsubim)) * mask ) / ( deno + maskinv ) );
		  
		  // 
		  //The above has a problem...mask is cutting out clean components found 
		  // outside pblimit ...use below if this is what is wanted
        //LatticeExpr<Float> ratio(iif(sqrt(abs(*(wtsubim))) < (fabs(pblimit)*itsPBScaleFactor), 0.0,  (*(modsubim))/(sqrt( abs(*(wtsubim)))  / itsPBScaleFactor))); 
		  // LatticeExpr<Float> ratio(iif((sqrt(abs(*(wtsubim))) ==0.0), 0.0,  ((*(modsubim))*itsPBScaleFactor)/sqrt( abs(*(wtsubim))) ));
		  
		  IPosition ip(4,itsImageShape[0]/2,itsImageShape[1]/2,0,0);
		  ///		  Float modval = modsubim->getAt(ip);
		  //LatticeExprNode aminval( min(*modsubim) );
		  //LatticeExprNode amaxval( max(*modsubim) );
		  //cout << "Before ---- min : " << aminval.getFloat() << " max : " << amaxval.getFloat() << endl;

		  modsubim->copyData(ratio);
		  //		  cout << "Val of model before|after flattening at center for pol " << pol << " chan " << chan << " : " << modval << "|" << modsubim->getAt(ip) << " weight : " << wtsubim->getAt(ip) << endl;
		  //LatticeExprNode minval( min(*modsubim) );
		  //LatticeExprNode maxval( max(*modsubim) );
		  //cout << "After ---- min : " << minval.getFloat() << " max : " << maxval.getFloat() << endl;
		}// if not zero
		}//chan
	    }//pol

	}
	else if( normtype=="pbsquare"){

	  for(Int pol=0; pol<itsImageShape[2]; pol++)
	    {
	      for(Int chan=0; chan<itsImageShape[3]; chan++)
		{
		  
		  itsPBScaleFactor = getPbMax(pol,chan);
		  //	cout << " pbscale : " << itsPBScaleFactor << endl;
		if(itsPBScaleFactor<=0){os << LogIO::NORMAL1 << "Skipping normalization for C:" << chan << " P:" << pol << " because pb max is zero " << LogIO::POST;}
		else {
		  
		  CountedPtr<ImageInterface<Float> > wtsubim=makeSubImage(0,1, 
									  chan, itsImageShape[3],
									  pol, itsImageShape[2], 
									  *weight() );
		  CountedPtr<ImageInterface<Float> > modsubim=makeSubImage(0,1, 
									   chan, itsImageShape[3],
									   pol, itsImageShape[2], 
									   *model() );
		  os << LogIO::NORMAL1 ;
		  os <<  "[C" +String::toString(chan) + ":P" + String::toString(pol) + "] ";
		  os << "Dividing " << itsImageName+String(".model") ;
		  os << " by [ (weight) / " << itsPBScaleFactor ;
		  os <<" ] to restore to optimal sky model before prediction" << LogIO::POST;
		  
		   
		  LatticeExpr<Float> deno(  abs(*(wtsubim))  / (itsPBScaleFactor*itsPBScaleFactor) );
		  
		  LatticeExpr<Float> mask( iif( (deno) > fabs(pblimit) , 1.0, 0.0 ) );
		  LatticeExpr<Float> maskinv( iif( (deno) > fabs(pblimit) , 0.0, 1.0 ) );
		  LatticeExpr<Float> ratio( ( (*(modsubim)) * mask ) / ( deno + maskinv ) );
		  
		  // 
		  //The above has a problem...mask is cutting out clean components found 
		  // outside pblimit ...use below if this is what is wanted
		  // LatticeExpr<Float> ratio(iif(abs(*(wtsubim)) == 0.0, *modsubim,  (*(modsubim))/( abs(*(wtsubim))  / (itsPBScaleFactor*itsPBScaleFactor)))); 
		  
		  IPosition ip(4,itsImageShape[0]/2,itsImageShape[1]/2,0,0);
		  ///		  Float modval = modsubim->getAt(ip);
		  //LatticeExprNode aminval( min(*modsubim) );
		  //LatticeExprNode amaxval( max(*modsubim) );
		  //cout << "Before ---- min : " << aminval.getFloat() << " max : " << amaxval.getFloat() << endl;

		  modsubim->copyData(ratio);
		  
		  //		  cout << "Val of model before|after flattening at center for pol " << pol << " chan " << chan << " : " << modval << "|" << modsubim->getAt(ip) << " weight : " << wtsubim->getAt(ip) << endl;
		  //LatticeExprNode minval( min(*modsubim) );
		  //LatticeExprNode maxval( max(*modsubim) );
		  //cout << "After ---- min : " << minval.getFloat() << " max : " << maxval.getFloat() << endl;
		}// if not zero
		}//chan
	    }//pol

	}

	//	storeImg(String("flatmodel.im"), *model());
	
      }
    }
  
  void SIImageStore::multiplyModelByWeight(Float pblimit, const String normtype)
  {
   LogIO os( LogOrigin("SIImageStore","multiplyModelByWeight",WHERE) );

        if(itsUseWeight // only when needed
	   && weight() )// i.e. only when possible. For an initial starting model, don't need wt anyway.
      {
	if( normtype=="flatsky") {
	  os << "Model is already flat sky. No need to multiply back after prediction" << LogIO::POST;
	  return;
	  }
	else if( normtype=="flatnoise"){

	  for(Int pol=0; pol<itsImageShape[2]; pol++)
	    {
	      for(Int chan=0; chan<itsImageShape[3]; chan++)
		{
		  
		  itsPBScaleFactor = getPbMax(pol,chan);
		  //	cout << " pbscale : " << itsPBScaleFactor << endl;
		if(itsPBScaleFactor<=0){os << LogIO::NORMAL1 << "Skipping normalization for C:" << chan << " P:" << pol << " because pb max is zero " << LogIO::POST;}
		else {
		  
		  CountedPtr<ImageInterface<Float> > wtsubim=makeSubImage(0,1, 
									  chan, itsImageShape[3],
									  pol, itsImageShape[2], 
									  *weight() );
		  CountedPtr<ImageInterface<Float> > modsubim=makeSubImage(0,1, 
									   chan, itsImageShape[3],
									   pol, itsImageShape[2], 
									   *model() );

		 

		  os << LogIO::NORMAL1 ;
		  os <<  "[C" +String::toString(chan) + ":P" + String::toString(pol) + "] ";
		  os << "Multiplying " << itsImageName+String(".model") ;
		  os << " by [ sqrt(weight) / " << itsPBScaleFactor;
		  os <<  " ] to take model back to flat noise with unit pb peak." << LogIO::POST;

		  LatticeExpr<Float> deno( sqrt( abs(*(wtsubim)) ) / itsPBScaleFactor );
		  
		  LatticeExpr<Float> mask( iif( (deno) > fabs(pblimit) , 1.0, 0.0 ) );
		  LatticeExpr<Float> maskinv( iif( (deno) > fabs(pblimit) , 0.0, 1.0 ) );
		  LatticeExpr<Float> ratio( ( (*(modsubim)) * mask ) * ( deno + maskinv ) );
		 
		  /////See comment in divmodel and divresidual for below usage 
		  //LatticeExpr<Float> ratio ( (*(modsubim)) * sqrt( abs(*(wtsubim)))  / itsPBScaleFactor );
		  modsubim->copyData(ratio);
      
		}// if not zero
		}//chan
	    }//pol
	}
	else if( normtype=="pbsquare"){

	  for(Int pol=0; pol<itsImageShape[2]; pol++)
	    {
	      for(Int chan=0; chan<itsImageShape[3]; chan++)
		{
		  
		  itsPBScaleFactor = getPbMax(pol,chan);
		  //	cout << " pbscale : " << itsPBScaleFactor << endl;
		if(itsPBScaleFactor<=0){os << LogIO::NORMAL1 << "Skipping normalization for C:" << chan << " P:" << pol << " because pb max is zero " << LogIO::POST;}
		else {
		  
		  CountedPtr<ImageInterface<Float> > wtsubim=makeSubImage(0,1, 
									  chan, itsImageShape[3],
									  pol, itsImageShape[2], 
									  *weight() );
		  CountedPtr<ImageInterface<Float> > modsubim=makeSubImage(0,1, 
									   chan, itsImageShape[3],
									   pol, itsImageShape[2], 
									   *model() );

		 

		  os << LogIO::NORMAL1 ;
		  os <<  "[C" +String::toString(chan) + ":P" + String::toString(pol) + "] ";
		  os << "Multiplying " << itsImageName+String(".model") ;
		  os << " by [ weight / " << itsPBScaleFactor*itsPBScaleFactor;
		  os <<  " ] to take model back to flat noise with unit pb peak." << LogIO::POST;
		  	  
		  LatticeExpr<Float> deno(  abs(*(wtsubim))  / (itsPBScaleFactor*itsPBScaleFactor) );
		  
		  LatticeExpr<Float> mask( iif( (deno) > fabs(pblimit) , 1.0, 0.0 ) );
		  LatticeExpr<Float> maskinv( iif( (deno) > fabs(pblimit) , 0.0, 1.0 ) );
		  LatticeExpr<Float> ratio( ( (*(modsubim)) * mask ) * ( deno + maskinv ) );
		 
		  /////See comment in divmodel and divresidual for below usage 
		  //LatticeExpr<Float> ratio ( (*(modsubim)) * sqrt( abs(*(wtsubim))  / itsPBScaleFactor) );
		  modsubim->copyData(ratio);
		}// if not zero
		}//chan
	    }//pol
	}	
      }
  }
  
  GaussianBeam SIImageStore::getPSFGaussian(Float psfcutoff)
  {
    GaussianBeam beam;
    try
      {
	if( psf()->ndim() > 0 )
	  {
          LatticeLocker lock2 (*(psf()), FileLocker::Read);
          StokesImageUtil::FitGaussianPSF( *(psf()), beam, psfcutoff);
	  }
      }
    catch(AipsError &x)
      {
	//	LogIO os( LogOrigin("SIImageStore","getPSFGaussian",WHERE) );
	//	os << "Error in fitting a Gaussian to the PSF : " << x.getMesg() << LogIO::POST;
	throw( AipsError("Error in fitting a Gaussian to the PSF : " + x.getMesg()) );
      }

    return beam;
  }

  void SIImageStore::makeImageBeamSet(Float psfcutoff, const Bool forcefit)
  {
    clock_t begin = clock();
      
    LogIO os( LogOrigin("SIImageStore","getPSFGaussian",WHERE) );
    // For all chans/pols, call getPSFGaussian() and put it into ImageBeamSet(chan,pol).
    AlwaysAssert( itsImageShape.nelements() == 4, AipsError );
    uInt nx = itsImageShape[0];
    uInt ny = itsImageShape[1];
    uInt npol = itsImageShape[2];
    uInt nchan = itsImageShape[3];
    ImageInfo ii = psf()->imageInfo();
    ImageBeamSet iibeamset=ii.getBeamSet();
    if(iibeamset.nchan()==nchan && iibeamset.nstokes()==npol && forcefit==False){
      itsPSFBeams=iibeamset;
      itsRestoredBeams=iibeamset;
      return;
    }
    itsPSFBeams.resize( nchan, npol );
    itsRestoredBeams.resize(nchan, npol);
    //    cout << "makeImBeamSet : imshape : " << itsImageShape << endl;

    String blankpsfs="";
    for( uInt chanid=0; chanid<nchan;chanid++) {
      for( uInt polid=0; polid<npol; polid++ ) {
    LatticeLocker lock2 (*(psf()), FileLocker::Read);

	IPosition substart(4,0,0,polid,chanid);
	IPosition substop(4,nx-1,ny-1,polid,chanid);
	Slicer psfslice(substart, substop,Slicer::endIsLast);
	SubImage<Float> subPsf( *psf() , psfslice, True );
	GaussianBeam beam;

	Bool tryfit=True;
        
        LatticeExprNode le( max(subPsf) );
        tryfit = le.getFloat()>0.0;
	if(tryfit)
	  {
	    try
	      {
		StokesImageUtil::FitGaussianPSF( subPsf, beam,psfcutoff );
		itsPSFBeams.setBeam( chanid, polid, beam );
		itsRestoredBeams.setBeam(chanid, polid, beam);
	      }
	    catch(AipsError &x)
	      {
	    	os << LogIO::WARN << "[Chan" << chanid << ":Pol" << polid << "] Error Gaussian fit to PSF : " << x.getMesg() ;
		//		os << LogIO::POST;
		os << " :  Setting restoring beam to largest valid beam." << LogIO::POST;
	      }
	  }
	else
	  {
	    //	    os << LogIO::WARN << "[Chan" << chanid << ":Pol" << polid << "] PSF is blank. Setting null restoring beam." << LogIO::POST ;
	    blankpsfs += "[C" +String::toString(chanid) + ":P" + String::toString(polid) + "] ";
	  }

      }// end of pol loop
    }// end of chan loop

    if( blankpsfs.length() >0 )
      os << LogIO::WARN << "PSF is blank for" << blankpsfs << LogIO::POST;

    //// Replace null (and bad) beams with the good one. 
    ////GaussianBeam maxbeam = findGoodBeam(True);

    //// Replace null beams by a tiny tiny beam, just to get past the ImageInfo restriction that
    //// all planes must have non-null beams.
    
    GaussianBeam defaultbeam=itsPSFBeams.getMaxAreaBeam();
    ///many of the unittests in tsdimaging seem to depend on having 0 area beams
    ///which throws and exception when it is stored in the image
    ///(so setting them to some small number)!
    if(defaultbeam.getArea("rad2")==0.0){
      Quantity majax(1e-06,"arcsec"),minax(1e-06,"arcsec"),pa(0.0,"deg");
      defaultbeam.setMajorMinor(majax,minax);
      defaultbeam.setPA(pa);
    }
    for( uInt chanid=0; chanid<nchan;chanid++) {
      for( uInt polid=0; polid<npol; polid++ ) {
	if( (itsPSFBeams.getBeam(chanid, polid)).isNull() ) 
	  { itsPSFBeams.setBeam( chanid, polid, defaultbeam );
	    itsRestoredBeams.setBeam( chanid, polid, defaultbeam );
	  }
      }// end of pol loop
    }// end of chan loop
    
    /*        
    //// Fill in gaps if there are any --- with the MAX Area beam. 
    /////    GaussianBeam maxbeam = itsPSFBeams.getMaxAreaBeam();
    if( maxbeam.isNull() ) {
	os << LogIO::WARN << "No planes have non-zero restoring beams. Forcing artificial 1.0arcsec beam." << LogIO::POST;
	Quantity majax(1.0,"arcsec"),minax(1.0,"arcsec"),pa(0.0,"deg");
	maxbeam.setMajorMinor(majax,minax);
	maxbeam.setPA(pa);
      }
    else  {
	for( Int chanid=0; chanid<nchan;chanid++) {
	  for( Int polid=0; polid<npol; polid++ ) {
	    if( (itsPSFBeams.getBeam(chanid, polid)).isNull() ) 
	      { itsPSFBeams.setBeam( chanid, polid, maxbeam ); }
	  }// end of pol loop
	}// end of chan loop
      }
    */


    /// For lack of a better place, store this inside the PSF image. To be read later and used to restore
   
    ii.setBeams( itsPSFBeams );
    {
      LatticeLocker lock1(*(psf()), FileLocker::Write);
      psf()->setImageInfo(ii);
      psf()->unlock();
    }
    clock_t end = clock();
    os << LogIO::NORMAL << "Time to fit Gaussian to PSF " << double(end - begin) / CLOCKS_PER_SEC << LogIO::POST;
  }// end of make beam set


  ImageBeamSet SIImageStore::getChannelBeamSet(const Int channel){

    return getChannelSliceBeamSet(channel, channel);
  }


  ImageBeamSet SIImageStore::getChannelSliceBeamSet(const Int begChan, const Int endChan){
     ImageBeamSet bs=getBeamSet();
    if(bs.shape()[0]==1)
      return bs;
    if(begChan > endChan || begChan <0)
      throw(AipsError("Inconsistent slice of beam in channel requested"));
    if(bs.shape()[0] < (endChan-1))
      throw(AipsError("beam of channel "+String::toString(endChan)+" does not exist"));
    IPosition blc(2, begChan, 0);
    IPosition trc(2, endChan, bs.shape()[1]-1);
    Matrix<GaussianBeam> sliceBeam=bs.getBeams()(blc, trc);
    ImageBeamSet subBeamSet(sliceBeam);
    return subBeamSet;

  }
  void SIImageStore::setBeamSet(const ImageBeamSet& bs){

    itsPSFBeams=bs;
  }
  
  ImageBeamSet SIImageStore::getBeamSet(Float psfcutoff)
  {
    IPosition beamshp = itsPSFBeams.shape();
    AlwaysAssert( beamshp.nelements()==2 , AipsError );
    if( beamshp[0]==0 || beamshp[1]==0 ) {makeImageBeamSet(psfcutoff);}
    return itsPSFBeams; 
  }

  void SIImageStore::printBeamSet(Bool verbose)
  {
    LogIO os( LogOrigin("SIImageStore","printBeamSet",WHERE) );
    AlwaysAssert( itsImageShape.nelements() == 4, AipsError );
    if( itsImageShape[3] == 1 && itsImageShape[2]==1 )
      {
	GaussianBeam beam = itsPSFBeams.getBeam();
	os << "Beam : " << beam.getMajor(Unit("arcsec")) << " arcsec, " << beam.getMinor(Unit("arcsec"))<< " arcsec, " << beam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
 }
    else
      {
        // per CAS-11415, verbose=True when restoringbeam='common'
        if( verbose ) 
          {
            ostringstream oss;
            oss<<"Beam";
	    Int nchan = itsImageShape[3];
	    for( Int chanid=0; chanid<nchan;chanid++) {
	      Int polid=0;
	      //	  for( Int polid=0; polid<npol; polid++ ) {
	      GaussianBeam beam = itsPSFBeams.getBeam( chanid, polid );
	      oss << " [C" << chanid << "]: " << beam.getMajor(Unit("arcsec")) << " arcsec, " << beam.getMinor(Unit("arcsec"))<< " arcsec, " << beam.getPA(Unit("deg")) << " deg";
	    }//for chanid
            os << oss.str() << LogIO::POST;
          }
        else 
          { 
	// TODO : Enable this, when this function doesn't complain about 0 rest freq.
	//                                 or when rest freq is never zero !
	try{
		itsPSFBeams.summarize( os, False, itsCoordSys );
                // per CAS-11415 request, not turn on this one (it prints out per-channel beam in each line in the logger)
		//itsPSFBeams.summarize( os, verbose, itsCoordSys );
	}
	catch(AipsError &x)
	  {
	    os << LogIO::WARN << "Error while printing (compact) restoring beam summary : " <<  x.getMesg() << LogIO::POST;
	    os << "Printing long summary" << LogIO::POST;
	    
	    AlwaysAssert( itsImageShape.nelements() == 4, AipsError );
	    //Int npol = itsImageShape[2];
	    Int nchan = itsImageShape[3];
	    for( Int chanid=0; chanid<nchan;chanid++) {
	      Int polid=0;
	      //	  for( Int polid=0; polid<npol; polid++ ) {
	      GaussianBeam beam = itsPSFBeams.getBeam( chanid, polid );
	      os << "Beam [C" << chanid << "]: " << beam.getMajor(Unit("arcsec")) << " arcsec, " << beam.getMinor(Unit("arcsec"))<< " arcsec, " << beam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
	      //}//for polid
	    }//for chanid
	  }// catch
        }
      }
  }
  
  /////////////////////////////// Restore all planes.

  void SIImageStore::restore(GaussianBeam& rbeam, String& usebeam, uInt term, Float psfcutoff)
  {
    LogIO os( LogOrigin("SIImageStore","restore",WHERE) );
    //     << ". Optionally, PB-correct too." << LogIO::POST;

    AlwaysAssert( itsImageShape.nelements() == 4, AipsError );
    Int nx = itsImageShape[0];
    Int ny = itsImageShape[1];
    Int npol = itsImageShape[2];
    Int nchan = itsImageShape[3];

    /*    if( !hasResidualImage() )
      {
	// Cannot restore without residual/dirty image.
	os << "Cannot restore without residual image" << LogIO::POST;
	return;
      }
    */

    //// Get/fill the beamset
    IPosition beamset = itsPSFBeams.shape();
    AlwaysAssert( beamset.nelements()==2 , AipsError );
    if( beamset[0] != nchan || beamset[1] != npol )
      {
	
	// Get PSF Beams....
	ImageInfo ii = psf()->imageInfo();
	itsPSFBeams = ii.getBeamSet();
	itsRestoredBeams=itsPSFBeams;
	IPosition beamset2 = itsPSFBeams.shape();

	AlwaysAssert( beamset2.nelements()==2 , AipsError );
	if( beamset2[0] != nchan || beamset2[1] != npol )
	  {
	    // Make new beams.
	    os << LogIO::WARN << "Couldn't find pre-computed restoring beams. Re-fitting." << LogIO::POST;
	    makeImageBeamSet(psfcutoff);
	  }
      }

    // toggle for printing common beam to the log 
    Bool printcommonbeam(False);
    //// Modify the beamset if needed
    //// if ( rbeam is Null and usebeam=="" ) Don't do anything.
    //// If rbeam is Null but usebeam=='common', calculate a common beam and set 'rbeam'
    //// If rbeam is given (or exists due to 'common'), just use it.
    if( rbeam.isNull() && usebeam=="common") {
      os << "Getting common beam" << LogIO::POST;
      rbeam = CasaImageBeamSet(itsPSFBeams).getCommonBeam();
      os << "Common Beam : " << rbeam.getMajor(Unit("arcsec")) << " arcsec, " << rbeam.getMinor(Unit("arcsec"))<< " arcsec, " << rbeam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
      printcommonbeam=True;
    }
    if( !rbeam.isNull() ) {
      /*for( Int chanid=0; chanid<nchan;chanid++) {
	for( Int polid=0; polid<npol; polid++ ) {
	  itsPSFBeams.setBeam( chanid, polid, rbeam );
	  /// Still need to add the 'common beam' option.
	}//for chanid
      }//for polid
      */
      itsRestoredBeams=ImageBeamSet(rbeam);
      GaussianBeam beam = itsRestoredBeams.getBeam();
     
      //if commonbeam has not printed in the log
      if (!printcommonbeam) {
        os << "Common Beam : " << beam.getMajor(Unit("arcsec")) << " arcsec, " << beam.getMinor(Unit("arcsec"))<< " arcsec, " << beam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
      printcommonbeam=True; // to avoid duplicate the info is printed to th elog
      }
    }// if rbeam not NULL
    //// Done modifying beamset if needed


    /// Before restoring, check for an empty model image and don't convolve (but still smooth residuals)
    /// (for CAS-8271 : make output restored image even if .model is zero... )
    Bool emptymodel = isModelEmpty();
    if(emptymodel) os << LogIO::WARN << "Restoring with an empty model image. Only residuals will be processed to form the output restored image." << LogIO::POST;


    LatticeLocker lock1(*(image(term)), FileLocker::Write);
    LatticeLocker lock2(*(residual(term)), FileLocker::Write);
    LatticeLocker lock3(*(model(term)), FileLocker::Read);
    //// Use beamset to restore
    for( Int chanid=0; chanid<nchan;chanid++) {
      for( Int polid=0; polid<npol; polid++ ) {
	
	IPosition substart(4,0,0,polid,chanid);
	IPosition substop(4,nx-1,ny-1,polid,chanid);
	Slicer imslice(substart, substop,Slicer::endIsLast);             
	SubImage<Float> subRestored( *image(term) , imslice, True );
	SubImage<Float> subModel( *model(term) , imslice, False );
	SubImage<Float> subResidual( *residual(term) , imslice, True );
	
	GaussianBeam beam = itsRestoredBeams.getBeam( chanid, polid );;
	//os << "Common Beam for chan : " << chanid << " : " << beam.getMajor(Unit("arcsec")) << " arcsec, " << beam.getMinor(Unit("arcsec"))<< " arcsec, " << beam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
        // only print per-chan beam if the common beam is not used for restoring beam
        if(!printcommonbeam) { 
	  os << "Beam for chan : " << chanid << " : " << beam.getMajor(Unit("arcsec")) << " arcsec, " << beam.getMinor(Unit("arcsec"))<< " arcsec, " << beam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
        }

	try
	  {
	    // Initialize restored image
	    subRestored.set(0.0);
	     if( !emptymodel ) { 
	       // Copy model into it
	       subRestored.copyData( LatticeExpr<Float>( subModel )  );
	       // Smooth model by beam
	       StokesImageUtil::Convolve( subRestored, beam);
	     }
	    // Add residual image
	    if( !rbeam.isNull() || usebeam == "common") // If need to rescale residuals, make a copy and do it.
	      {
		//		rescaleResolution(chanid, subResidual, beam, itsPSFBeams.getBeam(chanid, polid));
		TempImage<Float> tmpSubResidualCopy( IPosition(4,nx,ny,1,1), subResidual.coordinates());
		tmpSubResidualCopy.copyData( subResidual );
		
		rescaleResolution(chanid, tmpSubResidualCopy, beam, itsPSFBeams.getBeam(chanid, polid));
		subRestored.copyData( LatticeExpr<Float>( subRestored + tmpSubResidualCopy  ) );
	      }
	    else// if no need to rescale residuals, just add the residuals.
	      {
		
		
		subRestored.copyData( LatticeExpr<Float>( subRestored + subResidual  ) );
		
	      }
	    
	  }
	catch(AipsError &x)
	  {
	    throw( AipsError("Restoration Error in chan" + String::toString(chanid) + ":pol" + String::toString(polid) + " : " + x.getMesg() ) );
	  }
	
      }// end of pol loop
    }// end of chan loop
    
    try
      {
	//MSK//	
	if( hasPB() )
	  {
	    if( (image(term)->getDefaultMask()).matches("mask0") ) removeMask( image(term) );
	    copyMask(residual(term),image(term));
	  }

	//	if(hasPB()){copyMask(residual(term),image(term));}
	ImageInfo iminf = image(term)->imageInfo();
        //iminf.setBeams( itsRestoredBeams);

	os << "Beam Set : Single beam : " << itsRestoredBeams.hasSingleBeam() << "  Multi-beam : " << itsRestoredBeams.hasMultiBeam() << LogIO::DEBUG2;

	iminf.removeRestoringBeam();

	if( itsRestoredBeams.hasSingleBeam() )
	  { iminf.setRestoringBeam( itsRestoredBeams.getBeam() );}
	else
	  {iminf.setBeams( itsRestoredBeams);}

	image(term)->setImageInfo(iminf);
 
      }
    catch(AipsError &x)
      {
	throw( AipsError("Restoration Error  : "  + x.getMesg() ) );
      }
	
  }// end of restore()

  GaussianBeam SIImageStore::findGoodBeam()
  {
    LogIO os( LogOrigin("SIImageStore","findGoodBeam",WHERE) );
    IPosition beamshp = itsPSFBeams.shape();
    AlwaysAssert( beamshp.nelements()==2 , AipsError );

    /*
    if( beamshp[0] != nchan || beamshp[1] != npol )
      {
	// Make new beams.
	os << LogIO::WARN << "Couldn't find pre-computed restoring beams. Re-fitting." << LogIO::POST;
	makeImageBeamSet();
      }
    */

    Vector<Float> areas(beamshp[0]*beamshp[1]);
    Vector<Float> axrat(beamshp[0]*beamshp[1]);
    areas=0.0; axrat=1.0;
    Vector<Bool> flags( areas.nelements() );
    flags=False;
    
    Int cnt=0;
    for( Int chanid=0; chanid<beamshp[0];chanid++) {
      for( Int polid=0; polid<beamshp[1]; polid++ ) {
	GaussianBeam beam = itsPSFBeams(chanid, polid);
	if( !beam.isNull() && beam.getMajor(Unit("arcsec"))>1.1e-06  )  // larger than default filler beam.
	  {
	    areas[cnt] = beam.getArea( Unit("arcsec2") );
	    axrat[cnt] = beam.getMajor( Unit("arcsec") ) / beam.getMinor( Unit("arcsec") );
	  }
	else {
	  flags[cnt] = True;
	}
	cnt++;
      }//for chanid
    }//for polid
    
    Vector<Float> fit( areas.nelements() );
    Vector<Float> fitaxr( areas.nelements() );
    for (Int loop=0;loop<5;loop++)  {
      /// Filter on outliers in PSF beam area
      lineFit( areas, flags, fit, 0, areas.nelements()-1 );
      Float sd = calcStd( areas , flags, fit );
      for (uInt  i=0;i<areas.nelements();i++) {
	if( fabs( areas[i] - fit[i] ) > 3*sd ) flags[i]=True;
      }
      /// Filter on outliers in PSF axial ratio
      lineFit( axrat, flags, fitaxr, 0, areas.nelements()-1 );
      Float sdaxr = calcStd( axrat , flags, fitaxr );
      for (uInt  i=0;i<areas.nelements();i++) {
	if( fabs( axrat[i] - fitaxr[i] ) > 3*sdaxr ) flags[i]=True;
      }
    }
    //    cout << "Original areas : " << areas << endl;
    //    cout << "Original axrats : " << axrat << endl;
    //    cout << "Flags : " << flags << endl;

    // Find max area good beam.
    GaussianBeam goodbeam;
    Int cid=0,pid=0;
    Float maxval=0.0;
    cnt=0;
    for( Int chanid=0; chanid<beamshp[0];chanid++) {
      for( Int polid=0; polid<beamshp[1]; polid++ ) {
	if( flags[cnt] == False ){ 
	  if( areas[cnt] > maxval ) {maxval = areas[cnt]; goodbeam = itsPSFBeams.getBeam(chanid,polid);cid=chanid;pid=polid;}
	}
	cnt++;
      }//polid
    }//chanid

    os << "Picking common beam from C"<<cid<<":P"<<pid<<" : " << goodbeam.getMajor(Unit("arcsec")) << " arcsec, " << goodbeam.getMinor(Unit("arcsec"))<< " arcsec, " << goodbeam.getPA(Unit("deg")) << " deg" << LogIO::POST; 

    Bool badbeam=False;
    for(uInt i=0;i<flags.nelements();i++){if(flags[i]==True) badbeam=True;}

    if( badbeam == True ) 
      { 
	os << "(Ignored beams from :";
	cnt=0;
	for( Int chanid=0; chanid<beamshp[0];chanid++) {
	  for( Int polid=0; polid<beamshp[1]; polid++ ) {
	    if( flags[cnt] == True ){ 
	      os << " C"<<chanid<<":P"<<polid;
	    }
	    cnt++;
	  }//polid
	}//chanid
	os << " as outliers either by area or by axial ratio)" << LogIO::POST;
      } 


    /*
    // Replace 'bad' psfs with the chosen one.
    if( goodbeam.isNull() ) {
      os << LogIO::WARN << "No planes have non-zero restoring beams. Forcing artificial 1.0arcsec beam." << LogIO::POST;
      Quantity majax(1.0,"arcsec"),minax(1.0,"arcsec"),pa(0.0,"deg");
      goodbeam.setMajorMinor(majax,minax);
      goodbeam.setPA(pa);
    }
    else  {
      cnt=0;
      for( Int chanid=0; chanid<nchan;chanid++) {
	for( Int polid=0; polid<npol; polid++ ) {
	  if( flags[cnt]==True ) 
	    { itsPSFBeams.setBeam( chanid, polid, goodbeam ); }
	  cnt++;
	}// end of pol loop
      }// end of chan loop
    }
    */

    return goodbeam;
  }// end of findGoodBeam

  ///////////////////////// Funcs to calculate robust mean and fit, taking into account 'flagged' points.
void SIImageStore :: lineFit(Vector<Float> &data, Vector<Bool> &flag, Vector<Float> &fit, uInt lim1, uInt lim2)
{
  float Sx = 0, Sy = 0, Sxx = 0, Sxy = 0, S = 0, a, b, sd, mn;
  
  mn = calcMean(data, flag);
  sd = calcStd (data, flag, mn);
  
  for (uInt i = lim1; i <= lim2; i++)
    {
      if (flag[i] == False) // if unflagged
	{
	  S += 1 / (sd * sd);
	  Sx += i / (sd * sd);
	  Sy += data[i] / (sd * sd);
	  Sxx += (i * i) / (sd * sd);
	  Sxy += (i * data[i]) / (sd * sd);
	}
    }
  a = (Sxx * Sy - Sx * Sxy) / (S * Sxx - Sx * Sx);
  b = (S * Sxy - Sx * Sy) / (S * Sxx - Sx * Sx);
  
  for (uInt i = lim1; i <= lim2; i++)
    fit[i] = a + b * i;
  
}
/* Calculate the MEAN of 'vect' ignoring values flagged in 'flag' */
Float SIImageStore :: calcMean(Vector<Float> &vect, Vector<Bool> &flag)
{
  Float mean=0;
  Int cnt=0;
  for(uInt i=0;i<vect.nelements();i++)
    if(flag[i]==False)
      {
	mean += vect[i];
	cnt++;
      }
  if(cnt==0) cnt=1;
  return mean/cnt;
}
Float SIImageStore :: calcStd(Vector<Float> &vect, Vector<Bool> &flag, Vector<Float> &fit)
{
  Float std=0;
  uInt n=0,cnt=0;
  n = vect.nelements() < fit.nelements() ? vect.nelements() : fit.nelements();
  for(uInt i=0;i<n;i++)
    if(flag[i]==False)
      {
	cnt++;
	std += (vect[i]-fit[i])*(vect[i]-fit[i]);
      }
  if(cnt==0) cnt=1;
  return sqrt(std/cnt);

}
Float SIImageStore :: calcStd(Vector<Float> &vect, Vector<Bool> &flag, Float mean)
{
  Float std=0;
  uInt cnt=0;
  for(uInt i=0;i<vect.nelements();i++)
    if(flag[i]==False)
      {
	cnt++;
	std += (vect[i]-mean)*(vect[i]-mean);
      }
  return sqrt(std/cnt);
}

  ///////////////////////// End of Funcs to calculate robust mean and fit.



/*
  GaussianBeam SIImageStore::restorePlane()
  {

    LogIO os( LogOrigin("SIImageStore","restorePlane",WHERE) );
    //     << ". Optionally, PB-correct too." << LogIO::POST;

    Bool validbeam=False;
    GaussianBeam beam;
    try
      {
	// Fit a Gaussian to the PSF.
	beam = getPSFGaussian();
	validbeam = True;
      }
    catch(AipsError &x)
      {
	os << LogIO::WARN << "Beam fit error : " + x.getMesg() << LogIO::POST;
      }
    
    try
      {
	if( validbeam==True )
	  {
	    //os << "[" << itsImageName << "] " ;  // Add when parent image name is available.
	    //os << "Restore with beam : " << beam.getMajor(Unit("arcmin")) << " arcmin, " << beam.getMinor(Unit("arcmin"))<< " arcmin, " << beam.getPA(Unit("deg")) << " deg" << LogIO::POST; 
	    
	    // Initialize restored image
	    image()->set(0.0);
	    // Copy model into it
	    image()->copyData( LatticeExpr<Float>( *(model()) )  );
	    // Smooth model by beam
	    StokesImageUtil::Convolve( *(image()), beam);
	    // Add residual image
	    image()->copyData( LatticeExpr<Float>( *(image()) + *(residual())  ) );
	    
	    // Set restoring beam into the image
	    ImageInfo ii = image()->imageInfo();
	    //ii.setRestoringBeam(beam);
	    ii.setBeams(beam);
	    image()->setImageInfo(ii);
	  }
      }
    catch(AipsError &x)
      {
	throw( AipsError("Restoration Error : " + x.getMesg() ) );
      }
	
    return beam;

  }
*/

  void SIImageStore::pbcor(uInt term)
  {

    LogIO os( LogOrigin("SIImageStore","pbcor",WHERE) );

    if( !hasRestored() || !hasPB() )
      {
	// Cannot pbcor without restored image and pb
	os << LogIO::WARN << "Cannot pbcor without restored image and pb" << LogIO::POST;
	return;
      }
    LatticeLocker lock1(*(image(term)), FileLocker::Write);
        LatticeLocker lock2(*(pb(term)), FileLocker::Write);
        LatticeLocker lock3(*(imagepbcor(term)), FileLocker::Write);

	for(Int pol=0; pol<itsImageShape[2]; pol++)
	  {
	    for(Int chan=0; chan<itsImageShape[3]; chan++)
	      {
		
		CountedPtr<ImageInterface<Float> > restoredsubim=makeSubImage(0,1, 
								      chan, itsImageShape[3],
								      pol, itsImageShape[2], 
								      *image(term) );
		CountedPtr<ImageInterface<Float> > pbsubim=makeSubImage(0,1, 
								      chan, itsImageShape[3],
								      pol, itsImageShape[2], 
								      *pb() );

		CountedPtr<ImageInterface<Float> > pbcorsubim=makeSubImage(0,1, 
								      chan, itsImageShape[3],
								      pol, itsImageShape[2], 
								      *imagepbcor(term) );


		LatticeExprNode pbmax( max( *pbsubim ) );
		Float pbmaxval = pbmax.getFloat();

		if( pbmaxval<=0.0 )
		  {
		    os << LogIO::WARN << "Skipping PBCOR for C:" << chan << " P:" << pol << " because pb max is zero " << LogIO::POST;
		    pbcorsubim->set(0.0);
		  }
		else
		  {

		    LatticeExpr<Float> thepbcor( iif( *(pbsubim) > 0.0 , (*(restoredsubim))/(*(pbsubim)) , 0.0 ) );
		    pbcorsubim->copyData( thepbcor );

		}// if not zero
	      }//chan
	  }//pol

	// Copy over the PB mask.
        if((imagepbcor(term)->getDefaultMask()=="") && hasPB())
	  {copyMask(pb(),imagepbcor(term));}

	// Set restoring beam info
	ImageInfo iminf = image(term)->imageInfo();
        //iminf.setBeams( itsRestoredBeams );
	imagepbcor(term)->setImageInfo(iminf);

  }// end pbcor

  Matrix<Float> SIImageStore::getSumWt(ImageInterface<Float>& target)
  {
    Record miscinfo = target.miscInfo();
    
    Matrix<Float> sumwt;
    sumwt.resize();
    if( miscinfo.isDefined("sumwt") 
	&& (miscinfo.dataType("sumwt")==TpArrayFloat || miscinfo.dataType("sumwt")==TpArrayDouble  )  ) 
      { miscinfo.get( "sumwt" , sumwt ); } 
    else   { sumwt.resize( IPosition(2, target.shape()[2], target.shape()[3] ) ); sumwt = 1.0;  }
    
    return sumwt;
  }
  
  void SIImageStore::setSumWt(ImageInterface<Float>& target, Matrix<Float>& sumwt)
  {
    Record miscinfo = target.miscInfo();
    miscinfo.define("sumwt", sumwt);
    LatticeLocker lock1(target, FileLocker::Write);
    target.setMiscInfo( miscinfo );
  }
  

  Bool SIImageStore::getUseWeightImage(ImageInterface<Float>& target)
  {
    Record miscinfo = target.miscInfo();
    Bool useweightimage;
    if( miscinfo.isDefined("useweightimage") && miscinfo.dataType("useweightimage")==TpBool )
      { miscinfo.get( "useweightimage", useweightimage );  }
    else { useweightimage = False; }

    return useweightimage;
  }
  /*
  Bool SIImageStore::getUseWeightImage()
  {
    if( ! itsParentSumWt )
      return False;
    else 
     return  getUseWeightImage( *itsParentSumWt );
  }
  */
  void SIImageStore::setUseWeightImage(ImageInterface<Float>& target, Bool useweightimage)
  {
    Record miscinfo = target.miscInfo();
    miscinfo.define("useweightimage", useweightimage);
    LatticeLocker lock1(target, FileLocker::Write);
    target.setMiscInfo( miscinfo );
  }
  


  Bool SIImageStore::divideImageByWeightVal( ImageInterface<Float>& target )
  {

    Array<Float> lsumwt;
    sumwt()->get( lsumwt , False ); // For MT, this will always pick the zeroth sumwt, which it should.
    LatticeLocker lock2(target, FileLocker::Write);
    
    IPosition imshape = target.shape();

    //cerr << " SumWt  : " << lsumwt << " sumwtshape : " << lsumwt.shape() << " image shape : " << imshape << endl;

    AlwaysAssert( lsumwt.shape()[2] == imshape[2] , AipsError ); // polplanes
    AlwaysAssert( lsumwt.shape()[3] == imshape[3] , AipsError ); // chanplanes

    Bool div=False; // flag to signal if division actually happened, or weights are all 1.0

    for(Int pol=0; pol<lsumwt.shape()[2]; pol++)
      {
	for(Int chan=0; chan<lsumwt.shape()[3]; chan++)
	  {
	    IPosition pos(4,0,0,pol,chan);
	    if( lsumwt(pos) != 1.0 )
	      { 
		//		SubImage<Float>* subim=makePlane(  chan, True ,pol, True, target );
		std::shared_ptr<ImageInterface<Float> > subim=makeSubImage(0,1, 
								      chan, lsumwt.shape()[3],
								      pol, lsumwt.shape()[2], 
								      target );
		if ( lsumwt(pos) > 1e-07 ) {
		    LatticeExpr<Float> le( (*subim)/lsumwt(pos) );
		    subim->copyData( le );
		  }
		else  {
		    subim->set(0.0);
		  }
		div=True;
	      }
	  }
      }

    //    if( div==True ) cout << "Div image by sumwt : " << lsumwt << endl;
    //    else cout << "Already normalized" << endl;

    //    lsumwt = 1.0; setSumWt( target , lsumwt );

    return div;
  }

  void  SIImageStore::normPSF(Int term)
  {

    for(Int pol=0; pol<itsImageShape[2]; pol++)
      {
	for(Int chan=0; chan<itsImageShape[3]; chan++)
	  {
	    ///	    IPosition center(4,itsImageShape[0]/2,itsImageShape[1]/2,pol,chan);
	    
	    std::shared_ptr<ImageInterface<Float> > subim=makeSubImage(0,1, 
								  chan, itsImageShape[3],
								  pol, itsImageShape[2], 
								  (*psf(term)) );

	    std::shared_ptr<ImageInterface<Float> > subim0=makeSubImage(0,1, 
								  chan, itsImageShape[3],
								  pol, itsImageShape[2], 
								  (*psf(0)) );

            LatticeLocker lock1(*(subim), FileLocker::Write);

	    LatticeExprNode themax( max(*(subim0)) );
	    Float maxim = themax.getFloat();
	    
	    if ( maxim > 1e-07 )
	      {
		LatticeExpr<Float> normed( (*(subim)) / maxim );
		subim->copyData( normed );
	      }
	    else
	      {
		subim->set(0.0);
	      }
	  }//chan
      }//pol

  }

  void SIImageStore::calcSensitivity()
  {
    LogIO os( LogOrigin("SIImageStore","calcSensitivity",WHERE) );

    Array<Float> lsumwt;
    sumwt()->get( lsumwt , False ); // For MT, this will always pick the zeroth sumwt, which it should.

    IPosition shp( lsumwt.shape() );
    //cout << "Sumwt shape : " << shp << " : " << lsumwt << endl;
    //AlwaysAssert( shp.nelements()==4 , AipsError );
    
    os << "[" << itsImageName << "] Theoretical sensitivity (Jy/bm):" ;
    
    IPosition it(4,0,0,0,0);
    for ( it[0]=0; it[0]<shp[0]; it[0]++)
      for ( it[1]=0; it[1]<shp[1]; it[1]++)
	for ( it[2]=0; it[2]<shp[2]; it[2]++)
	  for ( it[3]=0; it[3]<shp[3]; it[3]++)
	    {
	      if( shp[0]>1 ){os << "f"<< it[0]+(it[1]*shp[0]) << ":" ;}
	      if( shp[3]>1 ) { os << "c"<< it[3] << ":"; }
	      if( shp[2]>1 ) { os << "p"<< it[2]<< ":" ; }
	      if( lsumwt( it )  > 1e-07 ) 
		{ 
		  os << 1.0/(sqrt(lsumwt(it))) << " " ;
		}
	      else
		{
		  os << "none" << " ";
		}
	    }
    
    os << LogIO::POST;

    //    Array<Float> sens = 1.0/sqrtsumwt;


  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////   Utility Functions to gather statistics on the imagestore.

Float SIImageStore::getPeakResidual()
{
    LogIO os( LogOrigin("SIImageStore","getPeakResidual",WHERE) );
    LatticeLocker lock2 (*(residual()), FileLocker::Read);
    ArrayLattice<Bool> pixelmask(residual()->getMask());
    LatticeExpr<Float> resd(iif(pixelmask,abs(*residual()),0));
    //LatticeExprNode pres( max(abs( *residual() ) ));
    LatticeExprNode pres( max(resd) );
    Float maxresidual = pres.getFloat();
    
    return maxresidual;
  }
  
Float SIImageStore::getPeakResidualWithinMask()
  {
    LogIO os( LogOrigin("SIImageStore","getPeakResidualWithinMask",WHERE) );
    //Float minresmask, maxresmask, minres, maxres;
    //findMinMax( residual()->get(), mask()->get(), minres, maxres, minresmask, maxresmask );

    ArrayLattice<Bool> pixelmask(residual()->getMask());
    //findMinMaxLattice(*residual(), *mask() , pixelmask, maxres,maxresmask, minres, minresmask);
    LatticeExpr<Float> resd(iif(((*mask()) > 0) && pixelmask ,abs((*residual())),0));
    LatticeExprNode pres( max(resd) );
    Float maxresidual = pres.getFloat();
    //Float maxresidual=max( abs(maxresmask), abs(minresmask) );
    return maxresidual;
  }

  // Calculate the total model flux
Float SIImageStore::getModelFlux(uInt term)
  {
    //    LogIO os( LogOrigin("SIImageStore","getModelFlux",WHERE) );
    LatticeLocker lock2 (*(model(term)), FileLocker::Read);
    LatticeExprNode mflux( sum( *model(term) ) );
    Float modelflux = mflux.getFloat();
    //    Float modelflux = sum( model(term)->get() );

    return modelflux;
  }

  // Check for non-zero model (this is different from getting model flux, for derived SIIMMT)
Bool SIImageStore::isModelEmpty()
  {
    if( !itsModel && (! hasModel()) ) return True;
    else return  ( fabs( getModelFlux(0) ) < 1e-08 );
  }


void SIImageStore::setPSFSidelobeLevel(const Float level){

  itsPSFSideLobeLevel=level;
}
  // Calculate the PSF sidelobe level...
  Float SIImageStore::getPSFSidelobeLevel()
  {
    LogIO os( LogOrigin("SIImageStore","getPSFSidelobeLevel",WHERE) );
    //cerr << "*****PSF sidelobe "<< itsPSFSideLobeLevel << endl;
    /// Calculate only once, store and return for all subsequent calls.
    if( itsPSFSideLobeLevel == 0.0 )
      {

	ImageBeamSet thebeams = getBeamSet();
        LatticeLocker lock2 (*(psf()), FileLocker::Read);
        
	//------------------------------------------------------------
	IPosition oneplaneshape( itsImageShape );
	AlwaysAssert( oneplaneshape.nelements()==4, AipsError );
	oneplaneshape[2]=1; oneplaneshape[3]=1;
	TempImage<Float> psfbeam( oneplaneshape, itsCoordSys );
	
	// In a loop through channels, subtract out or mask out the main lobe
	Float allmin=0.0, allmax=0.0;
	for(Int pol=0; pol<itsImageShape[2]; pol++)
	  {
	    for(Int chan=0; chan<itsImageShape[3]; chan++)
	      {
		std::shared_ptr<ImageInterface<Float> > onepsf=makeSubImage(0,1, 
								       chan, itsImageShape[3],
								       pol, itsImageShape[2], 
								       (*psf()) );
		
		
		GaussianBeam beam = thebeams.getBeam( chan, pol );
		Vector<Float> abeam(3); // Holds bmaj, bmin, pa  in asec, asec, deg 
		abeam[0] = beam.getMajor().get("arcsec").getValue() * C::arcsec;
		abeam[1] = beam.getMinor().get("arcsec").getValue() * C::arcsec;
		abeam[2] = (beam.getPA().get("deg").getValue() + 90.0)* C::degree;

		//cout << "Beam : " << abeam << endl;

		StokesImageUtil::MakeGaussianPSF( psfbeam,  abeam, False);

		//		storeImg( String("psfbeam.im"), psfbeam );
	
		//Subtract from PSF plane
		LatticeExpr<Float> delobed(  (*onepsf) - psfbeam  );
		
		// For debugging
		//onepsf->copyData( delobed );
		
		//Calc max and min and accumulate across channels. 
		
		LatticeExprNode minval_le( min( *onepsf ) );
		LatticeExprNode maxval_le( max( delobed ) );

		Float minval = minval_le.getFloat();
		Float maxval = maxval_le.getFloat();

		if( minval < allmin ) allmin = minval;
		if( maxval > allmax ) allmax = maxval;

		//cout << "Chan : " << chan << "   minval : " << minval << "  maxval : " << maxval << endl;
		
	      }//chan
	  }//pol
	
	//------------------------------------------------------------

	itsPSFSideLobeLevel = max( fabs(allmin), fabs(allmax) );

	//os << "PSF min : " << allmin << " max : " << allmax << " psfsidelobelevel : " << itsPSFSideLobeLevel << LogIO::POST;

      }// if changed.
    
    //    LatticeExprNode psfside( min( *psf() ) );
    //    itsPSFSideLobeLevel = fabs( psfside.getFloat() );

    //cout << "PSF sidelobe level : " << itsPSFSideLobeLevel << endl;
    return itsPSFSideLobeLevel;
  }

  void SIImageStore::findMinMax(const Array<Float>& lattice,
					const Array<Float>& mask,
					Float& minVal, Float& maxVal,
					Float& minValMask, Float& maxValMask)
  {
    IPosition posmin(lattice.shape().nelements(), 0);
    IPosition posmax(lattice.shape().nelements(), 0);

    if( sum(mask) <1e-06 ) {minValMask=0.0; maxValMask=0.0;}
    else { minMaxMasked(minValMask, maxValMask, posmin, posmax, lattice,mask); }

    minMax( minVal, maxVal, posmin, posmax, lattice );
  }

Array<Double> SIImageStore::calcRobustRMS(Array<Double>& mdns, const Float pbmasklevel, const Bool fastcalc)
{    
  LogIO os( LogOrigin("SIImageStore","calcRobustRMS",WHERE) );
  Record*  regionPtr=0;
  String LELmask("");
  LatticeLocker lockres (*(residual()), FileLocker::Read);
  ArrayLattice<Bool> pbmasklat(residual()->shape());
  pbmasklat.set(False);
  LatticeExpr<Bool> pbmask(pbmasklat);
  if (hasPB()) {
    // set bool mask: False = masked
    pbmask = LatticeExpr<Bool> (iif(*pb() > pbmasklevel, True, False));
    os << LogIO::DEBUG1 << "pbmask to be attached===> nfalse(pbmask.getMask())="<<nfalse(pbmask.getMask())<<endl; 
  }
  
   
  Record thestats;
  if (fastcalc) { // older calculation 
    // need to apply pbmask if present to be consistent between fastcalc = true and false.
    //TempImage<Float>* tempRes = new TempImage<Float>(residual()->shape(), residual()->coordinates(), SDMaskHandler::memoryToUse());
    auto tempRes = std::shared_ptr<TempImage<Float>>(new TempImage<Float>(residual()->shape(), residual()->coordinates(), SDMaskHandler::memoryToUse()));
    tempRes->copyData(*residual());
    tempRes->attachMask(pbmask);
    //thestats = SDMaskHandler::calcImageStatistics(*residual(), LELmask, regionPtr, True);
    thestats = SDMaskHandler::calcImageStatistics(*tempRes, LELmask, regionPtr, True);
  }
  else { // older way to calculate 
    // use the new statistic calculation algorithm
    Vector<Bool> dummyvec;
    // TT: 2018.08.01 using revised version (the older version of this is renameed to calcRobustImageStatisticsOld)
    thestats = SDMaskHandler::calcRobustImageStatistics(*residual(), *mask(), pbmask,  LELmask, regionPtr, True, dummyvec);
  }
    

  /***
  ImageStatsCalculator imcalc( residual(), regionPtr, LELmask, False); 

  Vector<Int> axes(2);
  axes[0] = 0;
  axes[1] = 1;
  imcalc.setAxes(axes);
  imcalc.setRobust(True);
  Record thestats = imcalc.statistics();
  //cout<<"thestats="<<thestats<<endl;
  ***/

  //Array<Double>rmss, mads, mdns;
  Array<Double>rmss, mads;
  //thestats.get(RecordFieldId("max"), maxs);
  thestats.get(RecordFieldId("rms"), rmss);
  thestats.get(RecordFieldId("medabsdevmed"), mads);
  thestats.get(RecordFieldId("median"), mdns);
  
  //os << LogIO::DEBUG1 << "Max : " << maxs << LogIO::POST;
  os << LogIO::DEBUG1 << "RMS : " << rmss << LogIO::POST;
  os << LogIO::DEBUG1 << "MAD : " << mads << LogIO::POST;
  
  // this for the new noise calc
  //return mdns+mads*1.4826;
  // this is the old noise calc
  return mads*1.4826;
}

  void SIImageStore::printImageStats()
  {
    LogIO os( LogOrigin("SIImageStore","printImageStats",WHERE) );
    Float minresmask=0, maxresmask=0, minres=0, maxres=0;
    //    findMinMax( residual()->get(), mask()->get(), minres, maxres, minresmask, maxresmask );
    LatticeLocker lock1 (*(residual()), FileLocker::Read);
    ArrayLattice<Bool> pixelmask(residual()->getMask());
    if(hasMask())
      {
        LatticeLocker lock2 (*(mask()), FileLocker::Read);
	findMinMaxLattice(*residual(), *mask() , pixelmask, maxres,maxresmask, minres, minresmask);
      }
    else
      {
	//LatticeExprNode pres( max( *residual() ) );
	LatticeExprNode pres( max( iif(pixelmask,*residual(),0) ) );
	maxres = pres.getFloat();
	//LatticeExprNode pres2( min( *residual() ) );
	LatticeExprNode pres2( min( iif(pixelmask,*residual(),0) ) );
	minres = pres2.getFloat();
      }

    os << "[" << itsImageName << "]" ;
    os << " Peak residual (max,min) " ;
    if( minresmask!=0.0 || maxresmask!=0.0 )
      { os << "within mask : (" << maxresmask << "," << minresmask << ") "; }
    os << "over full image : (" << maxres << "," << minres << ")" << LogIO::POST;

    os << "[" << itsImageName << "] Total Model Flux : " << getModelFlux() << LogIO::POST; 

    
    Record*  regionPtr=0;
    String LELmask("");
    Record thestats = SDMaskHandler::calcImageStatistics(*residual(), LELmask, regionPtr, True);
    Array<Double> maxs, mins;
    thestats.get(RecordFieldId("max"), maxs);
    thestats.get(RecordFieldId("min"), mins);
    //os << LogIO::DEBUG1 << "Max : " << maxs << LogIO::POST;
    //os << LogIO::DEBUG1 << "Min : " << mins << LogIO::POST;
    
  }

  // Calculate the total model flux
  Float SIImageStore::getMaskSum()
  {
    LogIO os( LogOrigin("SIImageStore","getMaskSum",WHERE) );
    LatticeLocker lock2 (*(mask()), FileLocker::Read);
    LatticeExprNode msum( sum( *mask() ) );
    Float masksum = msum.getFloat();

    //    Float masksum = sum( mask()->get() );

    return masksum;
  }

Bool SIImageStore::findMinMaxLattice(const Lattice<Float>& lattice, 
				     const Lattice<Float>& mask,
                                     const Lattice<Bool>& pixelmask,
				     Float& maxAbs, Float& maxAbsMask, 
				     Float& minAbs, Float& minAbsMask )
{

  //FOR DEGUG
  //LogIO os( LogOrigin("SIImageStore","findMinMaxLattice",WHERE) );

  maxAbs=0.0;maxAbsMask=0.0;
  minAbs=1e+10;minAbsMask=1e+10;
  LatticeLocker lock1 (const_cast<Lattice<Float>& > (lattice), FileLocker::Read);
  LatticeLocker lock2 (const_cast<Lattice<Float>& >(mask), FileLocker::Read);
  LatticeLocker lock3 (const_cast<Lattice<Bool>& >(pixelmask), FileLocker::Read);
  const IPosition tileShape = lattice.niceCursorShape();
  TiledLineStepper ls(lattice.shape(), tileShape, 0);
  {
    RO_LatticeIterator<Float> li(lattice, ls);
    RO_LatticeIterator<Float> mi(mask, ls);
    RO_LatticeIterator<Bool> pmi(pixelmask, ls);
    for(li.reset(),mi.reset(),pmi.reset();!li.atEnd();li++, mi++, pmi++) {
      IPosition posMax=li.position();
      IPosition posMin=li.position();
      IPosition posMaxMask=li.position();
      IPosition posMinMask=li.position();
      Float maxVal=0.0;
      Float minVal=0.0;
      Float maxValMask=0.0;
      Float minValMask=0.0;


      // skip if lattice chunk is masked entirely.
      if(ntrue(pmi.cursor()) > 0 ) {
        const MaskedArray<Float> marr(li.cursor(), pmi.cursor());
        const MaskedArray<Float> marrinmask(li.cursor() * mi.cursor(), pmi.cursor());
      //minMax( minVal, maxVal, posMin, posMax, li.cursor() );
      minMax( minVal, maxVal, posMin, posMax, marr );
      //minMaxMasked(minValMask, maxValMask, posMin, posMax, li.cursor(), mi.cursor());
      minMax(minValMask, maxValMask, posMin, posMax, marrinmask);
      
      //os<<"DONE minMax"<<LogIO::POST; 
      if( (maxVal) > (maxAbs) ) maxAbs = maxVal;
      if( (maxValMask) > (maxAbsMask) ) maxAbsMask = maxValMask;

      if( (minVal) < (minAbs) ) minAbs = minVal;
      if( (minValMask) < (minAbsMask) ) minAbsMask = minValMask;
      }

    }
  }

  return True;


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //
  //-------------------------------------------------------------
  // Initialize the internals of the object.  Perhaps other such
  // initializations in the constructors can be moved here too.
  //
  void SIImageStore::init()
  {
    imageExts.resize(MAX_IMAGE_IDS);
    
    imageExts(MASK)=".mask";
    imageExts(PSF)=".psf";
    imageExts(MODEL)=".model";
    imageExts(RESIDUAL)=".residual";
    imageExts(WEIGHT)=".weight";
    imageExts(IMAGE)=".image";
    imageExts(SUMWT)=".sumwt";
    imageExts(GRIDWT)=".gridwt";
    imageExts(PB)=".pb";
    imageExts(FORWARDGRID)=".forward";
    imageExts(BACKWARDGRID)=".backward";
    imageExts(IMAGEPBCOR)=".image.pbcor";

    itsParentPsf = itsPsf;
    itsParentModel=itsModel;
    itsParentResidual=itsResidual;
    itsParentWeight=itsWeight;
    itsParentImage=itsImage;
    itsParentSumWt=itsSumWt;
    itsParentMask=itsMask;
    itsParentImagePBcor=itsImagePBcor;

    //    cout << "parent shape : " << itsParentImageShape << "   shape : " << itsImageShape << endl;
    itsParentImageShape = itsImageShape;
    itsParentCoordSys = itsCoordSys;

    if( itsNFacets>1 || itsNChanChunks>1 || itsNPolChunks>1 ) { itsImageShape=IPosition(4,0,0,0,0); }

    itsOpened=0;

    itsPSFSideLobeLevel=0.0;
    itsReadLock=nullptr;
    itsDataPolRep=StokesImageUtil::UNKNOWN; //Should throw an exception if it is not initialized properly
  }


void SIImageStore::regridToModelImage( ImageInterface<Float> &inputimage, Int term )
  {
    try 
      {

    //output coords
	IPosition outshape = itsImageShape;
	CoordinateSystem outcsys = itsCoordSys;
	DirectionCoordinate outDirCsys = outcsys.directionCoordinate();
	SpectralCoordinate outSpecCsys = outcsys.spectralCoordinate();
     
	// do regrid   
	IPosition axes(3,0, 1, 2);
	IPosition inshape = inputimage.shape();
	CoordinateSystem incsys = inputimage.coordinates(); 
	DirectionCoordinate inDirCsys = incsys.directionCoordinate();
	SpectralCoordinate inSpecCsys = incsys.spectralCoordinate();

	Vector<Int> dirAxes = CoordinateUtil::findDirectionAxes(incsys);
	axes(0) = dirAxes(0); 
	axes(1) = dirAxes(1);
	axes(2) = CoordinateUtil::findSpectralAxis(incsys);
	
	try {
	  ImageRegrid<Float> imregrid;
	  imregrid.regrid( *(model(term)), Interpolate2D::LINEAR, axes, inputimage ); 
	} catch (AipsError &x) {
	  throw(AipsError("ImageRegrid error : "+ x.getMesg()));
	}
	
      }catch(AipsError &x)
      {
	throw(AipsError("Error in regridding input model image to target coordsys : " + x.getMesg()));
      }
  }

  //
  //---------------------------------------------------------------
  //
  void SIImageStore::makePersistent(String& fileName)
  {
    LogIO logIO(LogOrigin("SIImageStore", "makePersistent"));
    ofstream outFile; outFile.open(fileName.c_str(),std::ofstream::out);
    if (!outFile) logIO << "Failed to open filed \"" << fileName << "\"" << LogIO::EXCEPTION;
    //  String itsImageName;
    outFile << "itsImageNameBase: " << itsImageName << endl;

    //IPosition itsImageShape;
    outFile << "itsImageShape: " << itsImageShape.nelements() << " ";
    for (uInt i=0;i<itsImageShape.nelements(); i++) outFile << itsImageShape(i) << " "; outFile << endl;

    // Don't know what to do with this.  Looks like this gets
    // filled-in from one of the images.  So load this from one of the
    // images if they exist?
    //CoordinateSystem itsCoordSys; 

    // Int itsNFacets;
    outFile << "itsNFacets: " << itsNFacets << endl;
    outFile << "itsUseWeight: " << itsUseWeight << endl;
    

    // Misc Information to go into the header. 
    //    Record itsMiscInfo; 
    itsMiscInfo.print(outFile);
    
    // std::shared_ptr<ImageInterface<Float> > itsMask, itsPsf, itsModel, itsResidual, itsWeight, itsImage, itsSumWt;
    // std::shared_ptr<ImageInterface<Complex> > itsForwardGrid, itsBackwardGrid;

    Vector<Bool> ImageExists(MAX_IMAGE_IDS);
    if ( ! itsMask )     ImageExists(MASK)=False;
    if ( ! itsPsf )      ImageExists(PSF)=False;
    if ( ! itsModel )    ImageExists(MODEL)=False;
    if ( ! itsResidual ) ImageExists(RESIDUAL)=False;
    if ( ! itsWeight )   ImageExists(WEIGHT)=False;
    if ( ! itsImage )    ImageExists(IMAGE)=False;
    if ( ! itsSumWt )    ImageExists(SUMWT)=False;
    if ( ! itsGridWt )   ImageExists(GRIDWT)=False;
    if ( ! itsPB )       ImageExists(PB)=False;

    if ( ! itsForwardGrid )    ImageExists(FORWARDGRID)=False;
    if ( ! itsBackwardGrid )   ImageExists(BACKWARDGRID)=False;
    
    outFile << "ImagesExist: " << ImageExists << endl;
  }
  //
  //---------------------------------------------------------------
  //
  void SIImageStore::recreate(String& fileName)
  {
    LogIO logIO(LogOrigin("SIImageStore", "recreate"));
    ifstream inFile; inFile.open(fileName.c_str(),std::ofstream::out);
    if (!inFile) logIO << "Failed to open filed \"" << fileName << "\"" << LogIO::EXCEPTION;

    String token;
    inFile >> token; if (token == "itsImageNameBase:") inFile >> itsImageName;

    inFile >> token; 
    if (token=="itsImageShape:")
      {
	Int n;
	inFile >> n;
	itsImageShape.resize(n);
	for (Int i=0;i<n; i++) inFile >> itsImageShape(i);
      }

    // Int itsNFacets;
    inFile >> token; if (token=="itsNFacets:") inFile >> itsNFacets;
    inFile >> token; if (token=="itsUseWeight:") inFile >> itsUseWeight;

    Bool coordSysLoaded=False;
    String itsName;      
    try 
      {
	itsName=itsImageName+imageExts(MASK);casa::openImage(itsName,     itsMask);
	if (coordSysLoaded==False) {itsCoordSys=itsMask->coordinates(); itsMiscInfo=itsImage->miscInfo();coordSysLoaded=True;}
      } catch (AipsIO& x) {logIO << "\"" << itsName << "\" not found." << LogIO::WARN;};
    try 
      {
	itsName=itsImageName+imageExts(MODEL);casa::openImage(itsName,    itsModel);
	if (coordSysLoaded==False) {itsCoordSys=itsModel->coordinates(); itsMiscInfo=itsModel->miscInfo();coordSysLoaded=True;}
      } catch (AipsIO& x) {logIO << "\"" << itsName << "\" not found." << LogIO::WARN;};
    try 
      {
	itsName=itsImageName+imageExts(RESIDUAL);casa::openImage(itsName, itsResidual);
	if (coordSysLoaded==False) {itsCoordSys=itsResidual->coordinates(); itsMiscInfo=itsResidual->miscInfo();coordSysLoaded=True;}
      } catch (AipsIO& x) {logIO << "\"" << itsName << "\" not found." << LogIO::WARN;};
    try 
      {
	itsName=itsImageName+imageExts(WEIGHT);casa::openImage(itsName,   itsWeight);
	if (coordSysLoaded==False) {itsCoordSys=itsWeight->coordinates(); itsMiscInfo=itsWeight->miscInfo();coordSysLoaded=True;}
      } catch (AipsIO& x) {logIO << "\"" << itsName << "\" not found." << LogIO::WARN;};
    try 
      {
	itsName=itsImageName+imageExts(IMAGE);casa::openImage(itsName,    itsImage);
	if (coordSysLoaded==False) {itsCoordSys=itsImage->coordinates(); itsMiscInfo=itsImage->miscInfo();coordSysLoaded=True;}
      } catch (AipsIO& x) {logIO << "\"" << itsName << "\" not found." << LogIO::WARN;};
    try 
      {
	itsName=itsImageName+imageExts(SUMWT);casa::openImage(itsName,    itsSumWt);
	if (coordSysLoaded==False) {itsCoordSys=itsSumWt->coordinates(); itsMiscInfo=itsSumWt->miscInfo();coordSysLoaded=True;}
      } catch (AipsIO& x) {logIO << "\"" << itsName << "\" not found." << LogIO::WARN;};
    try
      {
	itsName=itsImageName+imageExts(PSF);casa::openImage(itsName,      itsPsf);
	if (coordSysLoaded==False) {itsCoordSys=itsPsf->coordinates(); itsMiscInfo=itsPsf->miscInfo();coordSysLoaded=True;}
      } catch (AipsIO& x) {logIO << "\"" << itsName << "\" not found." << LogIO::WARN;};
    try
      {
	casa::openImage(itsImageName+imageExts(FORWARDGRID),  itsForwardGrid);
	casa::openImage(itsImageName+imageExts(BACKWARDGRID), itsBackwardGrid);
      }
    catch (AipsError& x)
      {
	logIO << "Did not find forward and/or backward grid.  Just say'n..." << LogIO::POST;
      }

  }
//////////////
  Bool SIImageStore::intersectComplexImage(const String& ComplexImageName){
        Vector<Int> whichStokes(0);
	CoordinateSystem cimageCoord = StokesImageUtil::CStokesCoord( itsCoordSys,
								  whichStokes, itsDataPolRep);


        //cerr <<"itsDataPolRep " << itsDataPolRep << endl;
        
        CountedPtr<PagedImage<Complex> > compliantImage =nullptr;
        {
          PagedImage<Complex>inputImage(ComplexImageName);
          IPosition theShape=itsImageShape;
          theShape(0)=inputImage.shape()(0);
          theShape(1)=inputImage.shape()(1);
          CoordinateSystem inpcsys=inputImage.coordinates();
          Vector<Double> refpix=cimageCoord.referencePixel();
          refpix(0)+=(theShape(0)-itsImageShape(0))/2.0;
          refpix(1)+=(theShape(1)-itsImageShape(1))/2.0;
          cimageCoord.setReferencePixel(refpix);
          String tmpImage=File::newUniqueName(".", "TempImage").absoluteName();
          compliantImage=new PagedImage<Complex>(theShape, cimageCoord, tmpImage);
          compliantImage->set(0.0);
          IPosition iblc(theShape.nelements(),0);
          IPosition itrc=theShape-1;
          //cerr << "blc "  << iblc << " trc " << itrc  << " shape " << theShape << endl;
          LCBox lbox(iblc, itrc, theShape);
          ImageRegion imagreg(WCBox(lbox, cimageCoord));
		
          
          SubImage<Complex> subim(inputImage, imagreg, false);
          //cerr << "shapes " << inputImage.shape() << "  sub " << subim.shape() << " compl " << compliantImage->shape() << endl;
          compliantImage->copyData(subim);
			
        }
        TableUtil::deleteTable(ComplexImageName);
        compliantImage->rename(ComplexImageName);
        return True;
		
  }
	

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

} //# NAMESPACE CASA - END

