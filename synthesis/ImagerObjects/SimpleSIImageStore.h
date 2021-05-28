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

#ifndef SYNTHESIS_SIMPLESIIMAGESTORE_H
#define SYNTHESIS_SIMPLESIIMAGESTORE_H

#include <synthesis/ImagerObjects/SIImageStore.h>
/**
 * 
 */
namespace casa{//# NAMESPACE CASA - BEGIN

class SimpleSIImageStore : public SIImageStore
{
public:
	
	SimpleSIImageStore(const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &modelim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &residim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &psfim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &weightim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &restoredim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &maskim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &sumwtim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &gridwtim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &pbim,
	       const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &restoredpbcorim,
                    const std::shared_ptr<casacore::ImageInterface<casacore::Float> > &tempworkimage=nullptr,       
		   const casacore::Bool useweightimage=false);
	
	virtual casacore::String getType(){return "SimpleSIImageStore";};
	
	virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > psf(casacore::uInt term=0);
	virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > residual(casacore::uInt term=0);
    virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > weight(casacore::uInt term=0);
    virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > model(casacore::uInt term=0);
//    virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > image(casacore::uInt term=0);
    virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > mask(casacore::uInt term=0);
    virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > pb(casacore::uInt term=0);
  virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > tempworkimage(casacore::uInt term=0);
    virtual std::shared_ptr<casacore::ImageInterface<casacore::Complex> > forwardGrid(casacore::uInt term=0);
    virtual std::shared_ptr<casacore::ImageInterface<casacore::Complex> > backwardGrid(casacore::uInt term=0);
    
    virtual std::shared_ptr<casacore::ImageInterface<casacore::Float> > sumwt(casacore::uInt term=0);
	
	

  virtual casacore::Bool hasSensitivity(){return (bool) itsWeight;}
  virtual casacore::Bool hasPB(){return (bool) itsPB;}

  virtual casacore::Bool hasMask(){return (bool) itsMask; }
  virtual casacore::Bool hasModel() {return (bool) itsModel;}
  virtual casacore::Bool hasPsf() {return (bool) itsPsf;}
  virtual casacore::Bool hasResidual() {return (bool) itsResidual;}
  //hasResidualImage is not overloaded 
  virtual casacore::Bool hasResidualImage() {return false;}
  virtual casacore::Bool hasSumWt() {return (bool) itsSumWt;}
  ///So far no need for overloading this
  //virtual casacore::Bool hasRestored() {return doesImageExist(itsImageName+imageExts(IMAGE));}
  virtual std::shared_ptr<SIImageStore> getSubImageStore(const casacore::Int facet=0, const casacore::Int nfacets=1, 
						    const casacore::Int chan=0, const casacore::Int nchanchunks=1, 
						    const casacore::Int pol=0, const casacore::Int npolchunks=1);
  virtual casacore::Bool releaseLocks();

private:
  std::shared_ptr<casacore::ImageInterface<casacore::Float> > itsPsf, itsModel, itsResidual, itsWeight, itsImage, itsSumWt, itsImagePBcor, itsPB, itsTempWorkIm;
  	std::shared_ptr<casacore::ImageInterface<casacore::Complex> > itsForwardGrid, itsBackwardGrid;


	
};
}//# NAMESPACE CASA - END
#endif // SIMPLESIIMAGESTORE_H
