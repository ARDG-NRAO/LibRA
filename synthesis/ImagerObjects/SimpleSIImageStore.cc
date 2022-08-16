/*
 * SimpleSIImageStore is an implementation without much checking the caller must make sure pointers exist before using them
 * Copyright (C) 2019  Associated Universities, Inc. Washington DC, USA.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 * Queries concerning CASA should be submitted at
 *        https://help.nrao.edu
 *
 *         Postal address: CASA Project Manager 
 *         National Radio Astronomy Observatory
 *         520 Edgemont Road
 *         Charlottesville, VA 22903-2475 USA
 */
#include <casacore/casa/Exceptions/Error.h>
#include <iostream>
#include <synthesis/ImagerObjects/SimpleSIImageStore.h>

using namespace std;

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN
	
	SimpleSIImageStore::SimpleSIImageStore  (casacore::String &imageName, const shared_ptr<ImageInterface<Float> > &modelim,
	       const shared_ptr<ImageInterface<Float> > &residim, const shared_ptr<ImageInterface<Float> > &psfim,
	       const shared_ptr<ImageInterface<Float> > &weightim,const shared_ptr<ImageInterface<Float> > &restoredim,
	       const shared_ptr<ImageInterface<Float> > &maskim,const shared_ptr<ImageInterface<Float> > &sumwtim,
	       const shared_ptr<ImageInterface<Float> > &gridwtim, const shared_ptr<ImageInterface<Float> > &pbim,
               const shared_ptr<ImageInterface<Float> > &restoredpbcorim,
               const  shared_ptr<ImageInterface<Float> > & tempworkimage, const Bool useweightimage) : SIImageStore() {
			if(!psfim && !residim && !modelim)  {
				throw(AipsError("SimpleSIImagestore has to have a valid residual or psf image"));
			}
			else{
                          shared_ptr<ImageInterface<Float> >theim=psfim ? psfim : residim;
                          ///this is constructed for divide/mult modelbyweight
                          if(!theim)
                            theim=modelim;
                          itsCoordSys=theim->coordinates();
                          itsImageShape=theim->shape();
                          itsParentImageShape=itsImageShape; //validate looks for that
			}
			if(useweightimage && !weightim)
				throw(AipsError("SimpleSIImagestore has to have a valid weightimage for this kind of weighting scheme"));
			itsImageName = casacore::String(imageName);
			itsPsf = psfim;
			itsModel=modelim;
			itsResidual=residim;
			itsWeight=weightim;
			itsImage=restoredim;
			itsSumWt=sumwtim;
			itsMask=maskim;
			itsImagePBcor=restoredpbcorim;
                        itsTempWorkIm=tempworkimage;
			itsPB=pbim;
                        itsGridWt=gridwtim;
			itsUseWeight=useweightimage;
                        
		
		
		
	}
	shared_ptr<ImageInterface<Float> > SimpleSIImageStore::psf(uInt){
		if(!itsPsf)
			throw(AipsError("Programmer's error: calling for psf without setting it"));
		return itsPsf;
		
	}
	shared_ptr<ImageInterface<Float> > SimpleSIImageStore::residual(uInt){
		if(!itsResidual)
			throw(AipsError("Programmer's error: calling for residual without setting it"));
		return itsResidual;
		
	}
	shared_ptr<ImageInterface<Float> > SimpleSIImageStore::weight(uInt){
		if(!itsWeight)
			throw(AipsError("Programmer's error: calling for weight without setting it"));
		return itsWeight;
		
	}
	shared_ptr<ImageInterface<Float> > SimpleSIImageStore::model(uInt){
		if(!itsModel)
			throw(AipsError("Programmer's error: calling for model without setting it"));
		return itsModel;
		
	}
       shared_ptr<ImageInterface<Float> > SimpleSIImageStore::mask(uInt){
		if(!itsMask)
			throw(AipsError("Programmer's error: calling for model without setting it"));
		return itsMask;
		
	}
        shared_ptr<ImageInterface<Float> > SimpleSIImageStore::pb(uInt){
		if(!itsPB)
			throw(AipsError("Programmer's error: calling for pb without setting it"));
		return itsPB;
		
	}
  shared_ptr<ImageInterface<Float> > SimpleSIImageStore::tempworkimage(uInt){
		if(!itsTempWorkIm)
			throw(AipsError("Programmer's error: calling for temporary image without setting it"));
		return itsTempWorkIm;
		
	}
	shared_ptr<ImageInterface<Float> > SimpleSIImageStore::sumwt(uInt){
		if(!itsSumWt)
			throw(AipsError("Programmer's error: calling for sumweight without setting it"));
		return itsSumWt;
		
	}
	
	shared_ptr<ImageInterface<Complex> > SimpleSIImageStore::forwardGrid(uInt){
		if ( !itsForwardGrid ) {
			Vector<Int> whichStokes ( 0 );
			IPosition cimageShape;
                        //			cimageShape=itsImageShape;
                        if(!itsModel)
                          throw(AipsError("have to have model to predict"));
                        cimageShape=itsModel->shape();
                        CoordinateSystem cs=itsModel->coordinates();
			MFrequency::Types freqframe = cs.spectralCoordinate ( cs.findCoordinate ( Coordinate::SPECTRAL ) ).frequencySystem ( True );
			// No need to set a conversion layer if image is in LSRK already or it is 'Undefined'
			if ( freqframe != MFrequency::LSRK && freqframe!=MFrequency::Undefined && freqframe!=MFrequency::REST ) {
				itsCoordSys.setSpectralConversion ( "LSRK" );
                                cs.setSpectralConversion("LSRK");
			}
			CoordinateSystem cimageCoord = StokesImageUtil::CStokesCoord ( cs,
                                       whichStokes, itsDataPolRep );
			cimageShape ( 2 ) =whichStokes.nelements();
			cimageCoord.setObsInfo(itsCoordSys.obsInfo());
			//cout << "Making forward grid of shape : " << cimageShape << " for imshape : " << itsImageShape << endl;
			itsForwardGrid.reset ( new TempImage<Complex> ( TiledShape ( cimageShape, tileShape() ), cimageCoord, memoryBeforeLattice() ) );

		}
		return itsForwardGrid;
		
	}
	shared_ptr<ImageInterface<Complex> > SimpleSIImageStore::backwardGrid(uInt){
		if(!itsBackwardGrid){
			Vector<Int> whichStokes(0);
			IPosition cimageShape;
			cimageShape=itsImageShape;
			MFrequency::Types freqframe = itsCoordSys.spectralCoordinate(itsCoordSys.findCoordinate(Coordinate::SPECTRAL)).frequencySystem(True);
			// No need to set a conversion layer if image is in LSRK already or it is 'Undefined'
			if(freqframe != MFrequency::LSRK && freqframe!=MFrequency::Undefined && freqframe!=MFrequency::REST ) 
			{ itsCoordSys.setSpectralConversion("LSRK"); }
			CoordinateSystem cimageCoord = StokesImageUtil::CStokesCoord( itsCoordSys,
								  whichStokes, itsDataPolRep);
			cimageShape(2)=whichStokes.nelements();
			cimageCoord.setObsInfo(itsCoordSys.obsInfo());
			//cout << "Making backward grid of shape : " << cimageShape << " for imshape : " << itsImageShape << endl;
			itsBackwardGrid.reset( new TempImage<Complex>(TiledShape(cimageShape, tileShape()), cimageCoord, memoryBeforeLattice()) );
			
		}
		return itsBackwardGrid;
		
	}
	shared_ptr<SIImageStore> SimpleSIImageStore::getSubImageStore(const Int facet, const Int nfacets, const Int chan, const Int nchanchunks, const Int pol, const Int npolchunks){
          vector<shared_ptr<ImageInterface<Float> > >myImages={itsModel, itsResidual, itsPsf, itsWeight, itsImage, itsMask, itsSumWt, itsGridWt, itsPB, itsImagePBcor, itsTempWorkIm};
		if(itsSumWt){
			setUseWeightImage(*itsSumWt, itsUseWeight);
		}
		vector<shared_ptr<ImageInterface<Float> >  > subimPtrs(myImages.size(), nullptr);
		for (auto it=myImages.begin(); it != myImages.end() ; ++it){
			if((*it))
				subimPtrs[it-myImages.begin()]=makeSubImage(facet, nfacets, chan, nchanchunks, pol, npolchunks, *(*it));
		}
		shared_ptr<SIImageStore> retval(new SimpleSIImageStore(itsImageName, subimPtrs[0], subimPtrs[1], subimPtrs[2], subimPtrs[3], subimPtrs[4], subimPtrs[5], subimPtrs[6], subimPtrs[7], subimPtrs[8], subimPtrs[9], subimPtrs[10], itsUseWeight));
		
		
		return retval;
		/*
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &weightim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &restoredim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &maskim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &sumwtim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &gridwtim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &pbim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &restoredpbcorim,
		*/
	}
	Bool SimpleSIImageStore::releaseLocks(){
		if( itsPsf ) itsPsf->unlock();
		if( itsModel ) itsModel->unlock(); 
		if( itsResidual ) itsResidual->unlock() ;
		if( itsImage ) itsImage->unlock();
		if( itsWeight ) itsWeight->unlock();
		if( itsMask ) itsMask->unlock();
		if( itsSumWt ) itsSumWt->unlock() ;
		if( itsGridWt ) itsGridWt->unlock();
		if( itsPB ) itsPB->unlock() ;
		if( itsImagePBcor ) itsImagePBcor->unlock();
		
		return True;
	}
	
	
	
	
} //# NAMESPACE CASA - END
